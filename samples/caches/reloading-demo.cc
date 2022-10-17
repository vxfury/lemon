#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <vector>
#include <sstream>
#include <iostream>

#define TOSTRING(line) #line
#define LOCATION(file, line) \
    &file ":" TOSTRING(line)[(__builtin_strrchr(file, '/') ? (__builtin_strrchr(file, '/') - file + 1) : 0)]

#define CACHES_TRACE(fmt, ...)                                                                          \
    do {                                                                                                \
        char buff[32];                                                                                  \
        struct tm tm;                                                                                   \
        struct timeval tv;                                                                              \
        gettimeofday(&tv, NULL);                                                                        \
        localtime_r(&tv.tv_sec, &tm);                                                                   \
        size_t len = strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", &tm);                            \
        snprintf(&buff[len], sizeof(buff) - len, ".%03d", ((tv.tv_usec + 500) / 1000) % 1000);          \
        printf("\033[2;3m%s\033[0m <%s> " fmt "\n", buff, LOCATION(__FILE__, __LINE__), ##__VA_ARGS__); \
    } while (0);

#include "misc/ini.h"
#include "caches/reloading.h"

class TestReloading {
  public:
    static TestReloading &Instance()
    {
        static caches::Reloading<TestReloading> instance(1, "demo.ini");
        return instance.GetActivated();
    }

    int Reload()
    {
        struct stat newstat;
        int err = stat(file, &newstat);
        if (err == 0 && newstat.st_mtime > m_laststat.st_mtime) {
            m_laststat = newstat;

            ++called_times;
            parser.LoadFile(file);
            // usleep(rand() % (10 * 1000)); // for testing

            return 0;
        }
        return -1;
    }

    auto &GetParser()
    {
        return parser;
    }

  private:
    const char *file;
    int called_times;
    struct stat m_laststat;

    TestReloading(const char *file) : file(file), called_times(0)
    {
        parser.SetUnicode();
        parser.LoadFile(file);
        stat(file, &m_laststat);
    }

    ~TestReloading()
    {
        CACHES_TRACE("called times: %d", called_times);
    }
    parsers::INIParser parser;
    friend class caches::Reloading<TestReloading>;
};

template <typename T>
inline void DoNotOptimize(T const &value)
{
    asm volatile("" : : "r,m"(value) : "memory");
}

int main()
{
    std::vector<std::thread> workers;
    for (size_t i = 0; i < 2 * std::thread::hardware_concurrency(); i++) {
        workers.emplace_back([&]() {
            time_t st = time(nullptr);
            while (time(nullptr) < st + 10) {
                DoNotOptimize(TestReloading::Instance());
            }
        });
    }

    std::thread([]() {
        time_t st = time(nullptr);
        while (time(nullptr) < st + 8) {
            auto &parser = TestReloading::Instance().GetParser();
            parser.SetValue("section", "key", "value");
            parser.DumpFile("demo.ini");
            CACHES_TRACE("File saved");
            usleep(rand() % (5000 * 1000));
        };
    }).join();

    std::thread changer;
    for (auto &worker : workers) {
        worker.join();
    }

    std::cerr << "Enter any key to exit ..." << std::endl;
    getchar();

    return 0;
}
