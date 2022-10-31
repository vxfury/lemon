#pragma once

#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#define TOSTRING(line) #line
#define LOCATION(file, line) \
    &file ":" TOSTRING(line)[(__builtin_strrchr(file, '/') ? (__builtin_strrchr(file, '/') - file + 1) : 0)]

#define LEMON_TRACE(fmt, ...)                                                                           \
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

#undef LOCATION
#undef TOSTRING
