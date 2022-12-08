#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <string>
#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <atomic>
#include <condition_variable>

#if defined(BSD) && !defined(__GNU__)
#ifdef __APPLE__
#include <IOKit/serial/ioss.h>
#endif
#include <sys/ioctl.h>
#endif

#define TOSTRING(line) #line
#define LOCATION(file, line) \
    &file ":" TOSTRING(line)[(__builtin_strrchr(file, '/') ? (__builtin_strrchr(file, '/') - file + 1) : 0)]

#define SENSOR_TRACE(fmt, ...)                                                                          \
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

class ECSense {
  public:
    enum { ACTIVE, PASSTIVE };
    enum { SENSOR_HCHO = 0x17, SENSOR_TVOC = 0x18 };
    ECSense(const char *dev) : m_fd(-1), m_mode(PASSTIVE), m_dev(dev), m_shutdown(false)
    {
        if (Setup() == 0) {
            m_active_monitor = std::thread([this]() {
                while (true) {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_mode_cond.wait_for(lock, std::chrono::milliseconds(500), [this]() {
                        return m_mode == ACTIVE || m_shutdown;
                    });
                    if (m_shutdown) {
                        return;
                    }
                    if (m_mode != ACTIVE) {
                        continue;
                    }

                    unsigned char RESPONSE[9] = {0};
                    if (Recv(RESPONSE, sizeof(RESPONSE)) == 0 && Checksum(RESPONSE + 1, 7) == RESPONSE[8]) {
                        float temperature = 0.0f, humidity = 0.0f;
                        float gas_xg_m3 =
                            static_cast<float>(RESPONSE[2] << 8 | RESPONSE[3]) / pow(10.0, sensor_decimal_places);
                        if (m_callback) {
                            m_callback(SensorType(), SensorUnit(), gas_xg_m3);
                        } else {
                            SENSOR_TRACE("%s: %.1f %s", SensorType(), gas_xg_m3, SensorUnit());
                        }
                    }
                }
            });
        }
    }

    ~ECSense()
    {
        m_shutdown.store(true);
        m_mode_cond.notify_all();
        m_active_monitor.join();
        if (m_fd >= 0) {
            tcflush(m_fd, TCIOFLUSH);
            close(m_fd), m_fd = -1;
        }
    }

    const char *SensorType()
    {
        switch (sensor_type) {
            case SENSOR_HCHO:
                return "HCHO";
            case SENSOR_TVOC:
                return "TVOC";
        }
        return "<UNKOWN>";
    }

    const char *SensorUnit()
    {
        switch (sensor_unit) {
            case UNIT_PPB:
                return "μg/m³";
            case UNIT_PPM:
                return "mg/m³";
            case UNIT_PER:
                return "10g/m³";
        }
        return "<UNKOWN>";
    }

    int SetActiveMode(std::function<void(const char *, const char *, float)> callback)
    {
        int err;
        const unsigned char REQUEST[] = {0xFF, 0x01, 0x78, 0x40, 0x00, 0x00, 0x00, 0x00, 0x47};
        if ((err = Send(REQUEST, sizeof(REQUEST))) != 0) {
            SENSOR_TRACE("Change mode failed");
            return err;
        }
        m_mode = ACTIVE;
        m_mode_cond.notify_all();
        m_callback = callback;

        return 0;
    }

    int SetPasstiveMode()
    {
        const unsigned char REQUEST[] = {0xFF, 0x01, 0x78, 0x41, 0x00, 0x00, 0x00, 0x00, 0x46};
        if (int err = Send(REQUEST, sizeof(REQUEST)); err != 0) {
            return err;
        }
        m_mode = PASSTIVE;
        return 0;
    }

    int SetLowPowerMode(bool enabled = true)
    {
        int err;
        const unsigned char version[] = {0x19, 0x07, 0x06, 0x13, 0x47, 0x25};
        if (memcmp(m_version, version, sizeof(version)) >= 0) {
            if (!enabled) {
                unsigned char REQUEST[] = {0xA1, 0x53, 0x6C, 0x65, 0x65, 0x70, 0x32};
                if ((err = Send(REQUEST, sizeof(REQUEST))) != 0) {
                    SENSOR_TRACE("Send Request Failed");
                    return err;
                }
            } else {
                unsigned char REQUEST[] = {0xA2, 0x45, 0x78, 0x69, 0x74, 0x32};
                if ((err = Send(REQUEST, sizeof(REQUEST))) != 0) {
                    SENSOR_TRACE("Send Request Failed");
                    return err;
                }
            }
            unsigned char RESPONSE[9];
            if ((err = Recv(RESPONSE, sizeof(RESPONSE))) != 0) {
                SENSOR_TRACE("Recv Responce Failed");
                return err;
            }
        } else {
            if (!enabled) {
                unsigned char REQUEST[] = {0xAF, 0x53, 0x6C, 0x65, 0x65, 0x70};
                if ((err = Send(REQUEST, sizeof(REQUEST))) != 0) {
                    SENSOR_TRACE("Send Request Failed");
                    return err;
                }
            } else {
                unsigned char REQUEST[] = {0xAE, 0x45, 0x78, 0x69, 0x74};
                if ((err = Send(REQUEST, sizeof(REQUEST))) != 0) {
                    SENSOR_TRACE("Send Request Failed");
                    return err;
                }
            }
            unsigned char RESPONSE[2];
            if ((err = Recv(RESPONSE, sizeof(RESPONSE))) != 0) {
                SENSOR_TRACE("Recv Responce Failed");
                return err;
            }
        }

        return 0;
    }

    int SetRunningLight(bool enabled = true)
    {
        int err;
        if (!enabled) {
            unsigned char REQUEST[] = {0xFF, 0x01, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77};
            if ((err = Send(REQUEST, sizeof(REQUEST))) != 0) {
                SENSOR_TRACE("Send Request Failed");
                return err;
            }
        } else {
            unsigned char REQUEST[] = {0xFF, 0x01, 0x89, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76};
            if ((err = Send(REQUEST, sizeof(REQUEST))) != 0) {
                SENSOR_TRACE("Send Request Failed");
                return err;
            }
        }
        unsigned char RESPONSE[2];
        if ((err = Recv(RESPONSE, sizeof(RESPONSE))) != 0) {
            SENSOR_TRACE("Recv Responce Failed");
            return err;
        }

        return 0;
    }

    int GetRunningLight()
    {
        int err;
        unsigned char REQUEST[] = {0xFF, 0x01, 0x8A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75};
        if ((err = Send(REQUEST, sizeof(REQUEST))) != 0) {
            SENSOR_TRACE("Send Request Failed");
            return err;
        }
        unsigned char RESPONSE[9];
        if ((err = Recv(RESPONSE, sizeof(RESPONSE))) != 0) {
            SENSOR_TRACE("Recv Responce Failed");
            return err;
        }

        return RESPONSE[2] ? 1 : 0;
    }

    // The format for actively reading the gas concentration value is as follows
    int GetGasConcentration(float &gas_xg_m3, float &gas_ppx)
    {
        int err;
        unsigned char REQUEST[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
        if ((err = Send(REQUEST, sizeof(REQUEST))) != 0) {
            SENSOR_TRACE("Send Request Failed");
            return err;
        }

        unsigned char RESPONSE[9];
        if ((err = Recv(RESPONSE, sizeof(RESPONSE))) != 0) {
            SENSOR_TRACE("Recv Responce Failed");
            return err;
        }
        if (RESPONSE[0] != 0xFF || RESPONSE[1] != REQUEST[1] || Checksum(RESPONSE + 1, 7) != RESPONSE[8]) {
            SENSOR_TRACE("Responce Checksum Failed");
            return -EBADMSG;
        }
        gas_xg_m3 = static_cast<float>(RESPONSE[2] << 8 | RESPONSE[3]) / pow(10.0, sensor_decimal_places);
        gas_ppx = static_cast<float>(RESPONSE[6] << 8 | RESPONSE[7]) / pow(10.0, sensor_decimal_places);

        return 0;
    }

    // Get the current temperature and humidity
    int GetTemperatureAndHumidity(float &temperature, float &humidity, bool withchecksum = false)
    {
        int err;
        unsigned char REQUEST[] = {0xD2};
        if (withchecksum) {
            REQUEST[0] = 0xD6;
        }
        if ((err = Send(REQUEST, sizeof(REQUEST))) != 0) {
            SENSOR_TRACE("Send Request Failed");
            return err;
        }

        unsigned char RESPONSE[5];
        if ((err = Recv(RESPONSE, sizeof(RESPONSE))) != 0) {
            SENSOR_TRACE("Recv Responce Failed");
            return err;
        }
        if (withchecksum) {
            if (Checksum(RESPONSE, 4) != RESPONSE[4]) {
                SENSOR_TRACE("Checksum failed, 0x%X != 0x%X", Checksum(RESPONSE, 4), RESPONSE[4]);
                return -EBADMSG;
            }
        }
        temperature = static_cast<float>((int)(RESPONSE[0] << 8 | RESPONSE[1])) / 100.0;       // Celsius
        humidity = static_cast<float>((unsigned int)(RESPONSE[2] << 8 | RESPONSE[3])) / 100.0; // rh%

        return 0;
    }

    // Combined reading command of gas concentration value and temperature and humidity
    int GetGasConcentrationAndTemperatureAndHumidity(float &gas_xg_m3, float &gas_ppx, float &temperature,
                                                     float &humidity)
    {
        int err;
        unsigned char REQUEST[9] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};
        if ((err = Send(REQUEST, sizeof(REQUEST))) != 0) {
            SENSOR_TRACE("Send Request Failed");
            return err;
        }

        unsigned char RESPONSE[13];
        if ((err = Recv(RESPONSE, sizeof(RESPONSE))) != 0) {
            SENSOR_TRACE("Recv Responce Failed");
            return err;
        }
        if (Checksum(RESPONSE + 1, 11) != RESPONSE[12]) {
            SENSOR_TRACE("Responce Checksum Failed");
            return -EBADMSG;
        }
        gas_xg_m3 = static_cast<float>(RESPONSE[2] << 8 | RESPONSE[3]) / pow(10.0, sensor_decimal_places);
        gas_ppx = static_cast<float>(RESPONSE[6] << 8 | RESPONSE[7]) / pow(10.0, sensor_decimal_places);
        temperature = static_cast<float>((int)(RESPONSE[8] << 8 | RESPONSE[9])) / 100.0;         // Celsius
        humidity = static_cast<float>((unsigned int)(RESPONSE[10] << 8 | RESPONSE[11])) / 100.0; // rh%

        return 0;
    }

  private:
    int m_fd;
    int m_mode;
    const char *m_dev;
    unsigned char m_version[6];
    std::atomic_bool m_shutdown;
    std::mutex m_mutex;
    std::condition_variable m_mode_cond;
    std::thread m_active_monitor;
    std::function<void(const char *, const char *, float)> m_callback;

    // sensor properties
    unsigned int sensor_type, sensor_unit, sensor_sign;
    unsigned int sensor_maxinum, sensor_decimal_places;

    int Setup()
    {
        m_fd = open(m_dev, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);
        if (m_fd < 0) {
            SENSOR_TRACE("Error(%d): %s, fd = %d", errno, strerror(errno), m_fd);
            return -errno;
        }
        struct termios attr;
        if (tcgetattr(m_fd, &attr) < 0) {
            SENSOR_TRACE("Error(%d): %s, fd = %d", errno, strerror(errno), m_fd);
            return -errno;
        }
        cfsetspeed(&attr, B9600);

        attr.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS | HUPCL);
        attr.c_cflag |= (CS8 | CLOCAL | CREAD);

        // Need to see which flags should be unset.
        // This is not the "correct" way to fix this, as it just turns all flags off
        // rather than specifying the specific flag setting.
        attr.c_iflag = 0;
        attr.c_lflag &= ~(ICANON | ISIG | ECHO | ECHOE | ECHOK | ECHONL | ECHOCTL | ECHOPRT | ECHOKE | IEXTEN);
        attr.c_lflag &= ~(INPCK | IXON | IXOFF | IXANY | ICRNL);
        attr.c_oflag &= ~(OPOST | ONLCR);

        for (int i = 0; i < sizeof(attr.c_cc); i++) {
            attr.c_cc[i] = _POSIX_VDISABLE;
        }
        attr.c_cc[VTIME] = 0;
        attr.c_cc[VMIN] = 1;

        // This will prevent additional opens except by root-owned processes.
        if (ioctl(m_fd, TIOCEXCL) < 0) {
            SENSOR_TRACE("Error(%d): %s, fd = %d", errno, strerror(errno), m_fd);
            return -errno;
        }

        if (tcsetattr(m_fd, TCSAFLUSH, &attr) < 0) { // apply the settings to the port
            SENSOR_TRACE("Error(%d): %s, fd = %d", errno, strerror(errno), m_fd);
            return -errno;
        }

        int err;
        if ((err =
                 DetectDevice(sensor_type, sensor_maxinum, sensor_unit, sensor_decimal_places, sensor_sign, m_version))
            == 0) {
            SENSOR_TRACE("%s: maxinum = %u %s, decimal_places = %u, sign = %u, version = "
                         "0X%02X%02X%02X%02X%02X%02X",
                         SensorType(), sensor_maxinum, sensor_unit == UNIT_PPB ? "ppb" : "ppm", sensor_decimal_places,
                         sensor_sign, m_version[0], m_version[1], m_version[2], m_version[3], m_version[4],
                         m_version[5]);
        } else {
            return err;
        }

        return 0;
    }

    inline int Recv(unsigned char *data, size_t size)
    {
        if (m_fd < 0) {
            SENSOR_TRACE("tty not opened");
            return -errno;
        }
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(m_fd, &rfds);
        struct timeval timeout = {.tv_sec = 3, .tv_usec = 0};
        if (select(m_fd + 1, &rfds, NULL, NULL, &timeout) >= 1) {
            if (FD_ISSET(m_fd, &rfds)) {
                FD_CLR(m_fd, &rfds);
                if (read(m_fd, data, size) < 0) {
                    SENSOR_TRACE("Error(%d): %s, status = 0x%x", errno, strerror(errno), fcntl(m_fd, F_GETFL, 0));
                    return -errno;
                }
                return 0;
            }
        }
        // SENSOR_TRACE("Timeout to Recv");
        return -ETIMEDOUT; /* timeout */
    }

    inline int Send(const unsigned char *data, size_t size)
    {
        if (m_fd < 0) {
            SENSOR_TRACE("tty not opened");
            return -errno;
        }
        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(m_fd, &wfds);
        struct timeval timeout = {.tv_sec = 3, .tv_usec = 0};
        if (select(m_fd + 1, NULL, &wfds, NULL, &timeout) >= 1) {
            if (FD_ISSET(m_fd, &wfds)) {
                FD_CLR(m_fd, &wfds);
                if (write(m_fd, data, size) < 0) {
                    SENSOR_TRACE("Error(%d): %s, status = 0x%x", errno, strerror(errno), fcntl(m_fd, F_GETFL, 0));
                    return -errno;
                }
                return 0;
            }
        }
        // SENSOR_TRACE("Timeout to Send");
        return -ETIMEDOUT; /* timeout */
    }

    static inline unsigned char Checksum(const unsigned char *data, size_t size)
    {
        unsigned char chksum = 0;
        for (unsigned i = 0; i < size; i++) {
            chksum += data[i];
        }
        return ~chksum + 1;
    }

    // Get the sensor type, maximum range, unit, and decimal places
    enum {
        UNIT_PPM = 0x02, // mg/m³, ppm
        UNIT_PPB = 0x04, // μg/m³, ppb
        UNIT_PER = 0x08, // 10g/m³, %
    };
    int DetectDevice(unsigned int &sensor_type, unsigned int &maxinum, unsigned int &unit, unsigned int &decimal_places,
                     unsigned int &data_sign, unsigned char version[6])
    {
        int err = SetPasstiveMode(); // set to passtive mode before detecting

        // get the sensor type, maximum range, unit, and decimal places
        {
            // Two request format supported: 0xD1, 0xD7
            unsigned char REQUEST[] = {0xD7}, RESPONSE[9];
            if ((err = Send(REQUEST, sizeof(REQUEST))) != 0) {
                SENSOR_TRACE("Send Request Failed");
                return err;
            }
            if ((err = Recv(RESPONSE, sizeof(RESPONSE))) != 0) {
                SENSOR_TRACE("Recv Responce Failed");
                return err;
            }
            if (Checksum(RESPONSE + 1, 7) != RESPONSE[8]) {
                SENSOR_TRACE("Responce Checksum Failed");
                return -EBADMSG;
            }

            if (REQUEST[0] == 0xD1) {
                sensor_type = RESPONSE[0];
                maxinum = RESPONSE[1] << 8 | RESPONSE[2];
                unit = RESPONSE[3];
                decimal_places = RESPONSE[7] >> 4;
                data_sign = RESPONSE[7] & 0x07;
            } else if (REQUEST[0] == 0xD7) {
                if (RESPONSE[0] != 0xFF || RESPONSE[1] != 0xD7) {
                    SENSOR_TRACE("Bad repsonse");
                    return -EBADMSG;
                }
                sensor_type = RESPONSE[2];
                maxinum = RESPONSE[3] << 8 | RESPONSE[4];
                unit = RESPONSE[5];
                decimal_places = RESPONSE[6] >> 4;
                data_sign = RESPONSE[6] & 0x07;
            }

            return 0;
        }

        // get current version number
        {
            unsigned char REQUEST[] = {0xD3};
            if ((err = Send(REQUEST, sizeof(REQUEST))) != 0) {
                SENSOR_TRACE("Send Request Failed");
                return err;
            }
            bzero(version, sizeof(m_version));
            if ((err = Recv(version, sizeof(m_version))) != 0) {
                SENSOR_TRACE("Recv Responce Failed");
                return err;
            }
        }

        return 0;
    }
};

#include <vector>
#include <filesystem>
#include "cxxopt.h"

std::vector<std::string> ListSerialDevices()
{
    std::vector<std::string> devices;
    for (const auto &entry : std::__fs::filesystem::directory_iterator("/dev/")) {
        if (std::string(entry.path()).find("usb") == std::string::npos) {
            continue;
        }
        int fd = open(entry.path().c_str(), O_RDWR | O_NONBLOCK);
        if (fd < 0) {
            continue;
        }
        devices.push_back(entry.path());
        close(fd);
    }

    return devices;
}

int main(int argc, char **argv)
{
    struct level {
        float threshold;
        const char *level;
        const char *description;
    };
    std::string device = getarg("", "--device");

    if (device.empty()) {
        auto devices = ListSerialDevices();
        if (devices.size() > 0) {
            std::cout << "Availiable Devices:" << std::endl;
            for (size_t i = 0; auto dev : devices) {
                std::cout << "  " << (i++) << ". " << dev << std::endl;
            }

            const char *prompt = "Please Select Your Sensor: ";
        again:
            std::string line;
            fputs(prompt, stdout), fflush(stdout);
            std::getline(std::cin, line);
            int index = line.empty() ? -1 : std::stoi(line);
            if (index >= 0 and index < devices.size()) {
                device = devices.at(index);
            } else {
                prompt = "No such device, Please Select again: ";
                goto again;
            }
        }
    }
    if (device.empty()) {
        std::cerr << "ERROR: Device not specified" << std::endl;
        exit(1);
    }
    if (access(device.c_str(), F_OK) != 0) {
        std::cerr << "ERROR: Please Check device connection" << std::endl;
        exit(2);
    }

    ECSense sensor(device.c_str());
    bool monitor = getarg(false, "--monitor");
    sensor.SetRunningLight(getarg(false, "--light"));

    if (monitor) {
        sensor.SetActiveMode([](const char *type, const char *unit, float value) {
            SENSOR_TRACE("%s: %.1f %s", type, value, unit);
        });
        while (getchar() != 'q') {
        }
    } else {
        float gas_xg_m3 = 0.0f, gas_ppx = 0.0f, temperature = 0.0f, humidity = 0.0f;
        if (sensor.GetGasConcentrationAndTemperatureAndHumidity(gas_xg_m3, gas_ppx, temperature, humidity) == 0) {
            SENSOR_TRACE("%s: %.1f %s, Temperature: %.2f ℃, Humidity: %.2f %%", sensor.SensorType(), gas_xg_m3,
                         sensor.SensorUnit(), temperature, humidity);
        }
    }

    // 甲醛指标：https://zhuanlan.zhihu.com/p/342530551
    // 嗅阈值: 60 - 120μg/m³
    // 眼阈值：100 - 500μg/m³
    // 达到60 - 70μg/m³，儿童会发生轻微气喘
    // 达到100μg/m³时，有异味和不适感
    // 达到500μg/m³时，可刺激眼睛，引起流泪
    // 达到600μg/m³时，可引起咽喉不适或疼痛。浓度更高时，可引起恶心呕吐、咳嗽胸闷、气喘甚至肺水肿
    // 达到30mg/m³时，会立即致人死亡

    //? HCHO quality grade standard
    // Level 1: <60μg/m³, Very Low, Odour threshold
    // Level 2: <150μg/m³, Low, No irritation or impairment of well-being
    // Level 3: <1.24mg/m³, Accepted, Irritation or impairment of well-bing possiable in case of interaction with other
    //          exposure parameters
    // Level 4: <3.71mg/m³, Marginal, String in the nose, eyes, thriat
    // Level 4: <6.18mg/m³, High, Bearable for 30 minutes as max, increasing discomfort, breathlessness, lacrimation
    // Level 5: <12.35mg/m³, Very High, Bearable for less than 15 minutes, increasing discomfort, breathlessness,
    //          lacrimation
    // Level 6: <24.7mg/m³, Extremely High(Dangerous), Strong lacrimation already after a few minutes of exposure
    //          (lasting for uo to 1 hour after exposure), immediate breashlessness, coughing, severe burning in throat,
    //          nose and eyes
    // Level 7: >25mg/m³, Headaches, other neurotoxic effects apart from headaches possible
    // Level 8: >37.06mg/m³, Toxic pulmonary oedema, pneumonia, risk of death!
    struct level hcho_levels[] = {
        {0.06, "Very Low", "Odour threshold"},
        {0.15, "Low", "No irritation or impairment of well-being"},
        {1.24, "Accepted",
         "Irritation or impairment of well-bing possiable in case of interaction with other exposure parameters"},
        {3.71, "Marginal", "String in the nose, eyes, thriat"},
        {6.18, "High", "Bearable for 30 minutes as max, increasing discomfort, breathlessness, lacrimation"},
        {12.35, "Very High", "Bearable for less than 15 minutes, increasing discomfort, breathlessness, lacrimation"},
        {24.7, "Extremely High(Dangerous)",
         "Strong lacrimation already after a few minutes of exposure(lasting for uo to 1 hour after exposure), "
         "immediate breashlessness, coughing, severe burning in throat, nose and eyes"},
        {25, "Extremely High(Dangerous)", "Headaches, other neurotoxic effects apart from headaches possible"},
        {37.06, "Extremely High(Dangerous)", "Toxic pulmonary oedema, pneumonia, risk of death!"},
    };

    //? TVOC quality grade standard(https://easlab.com/iaqref.htm)
    // Level 1: <0.3mg/m³, Low, No irritation or discomfort is expected.
    // Level 2: <0.5mg/m³, Acceptable, Occasional irritation or discomfort may be possible with sensitive individuals.
    // Level 3: <1.0mg/m³, Marginal, Complaints about irritation and discomfort are possible in sensitive individuals
    // Level 4: <3.0mg/m³, High, Irritation and discomfort are very likely
    // Level 5: >3.0mg/m³, Very High, Irritation and discomfort are very possible.
    struct level tvoc_levels[] = {
        {0.3, "Low", "No irritation or discomfort is expected"},
        {0.5, "Acceptable", "Occasional irritation or discomfort may be possible with sensitive individuals"},
        {1.0, "Marginal", "Complaints about irritation and discomfort are possible in sensitive individuals"},
        {3.0, "High", "Irritation and discomfort are very likely"},
        {3.0, "Very High", "Irritation and discomfort are very possible."},
    };

    //? IAQ Air quality grade standard
    // Level 1: ≤0.3ppm, fresh air, open the window after the rain to ventilate, this level of fresh air can be obtained
    //          in the early morning park environment;
    // Level 2: ≤1.5ppm, trace pollution, indoor formaldehyde, TVOC and other harmful gases are near the critical point
    //          of the indoor air standard minimum limit;
    // Level 3: ≤3ppm, light pollution, people with sensitive constitutions have allergies or discomfort, and check if
    //          anyone smokes or drinks alcohol or disinfects with alcohol. It is recommended to open the window to
    //          ventilate or fresh air purification;
    // Level 4: ≤5ppm, moderate pollution, long-term exposure in this environment will affect health, people with
    //          complications or allergies should not stay for a long time, open windows as soon as possible or fresh
    //          air purification;
    // Level 5: ≤10ppm, severe pollution, indoor environment should be checked for hidden fire hazards, If the cooker
    //          hood is not turned on during the cooking process, gas leaks, smoking and drinking, use alcohol for
    //          disinfection or use toilet water perfume, etc., the windows should be opened as soon as possible for
    //          ventilation or fresh air purification;
    // Level 6: >10ppm, severe pollution, harmful substances exceeding the standard, excluding the effects of alcohol,
    //          toilet water or fragrance, etc., which may endanger personal safety. Open the window as soon as possible
    //          to vent or evacuate.

    return 0;
}
