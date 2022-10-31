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

#define WIFI_TRACE(fmt, ...)                                                                            \
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

class ESP8266 {
  public:
  private:
    int m_fd;
    const char *m_dev;

    int Setup()
    {
        m_fd = open(m_dev, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);
        if (m_fd < 0) {
            WIFI_TRACE("Error(%d): %s, fd = %d", errno, strerror(errno), m_fd);
            return -errno;
        }
        struct termios attr;
        if (tcgetattr(m_fd, &attr) < 0) {
            WIFI_TRACE("Error(%d): %s, fd = %d", errno, strerror(errno), m_fd);
            return -errno;
        }
        cfsetspeed(&attr, B115200);

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
            WIFI_TRACE("Error(%d): %s, fd = %d", errno, strerror(errno), m_fd);
            return -errno;
        }

        if (tcsetattr(m_fd, TCSAFLUSH, &attr) < 0) { // apply the settings to the port
            WIFI_TRACE("Error(%d): %s, fd = %d", errno, strerror(errno), m_fd);
            return -errno;
        }

        return 0;
    }
};

/*
AT+CWMODE?\r\n
AT+CWMODE=3\r\n
AT+RST\r\n
AT+CWJAP="ChinaNet-F4KRCA","YRW6PKGF"\r\n
AT+CWSAP="Kiran-IoT","12345678",10,3\r\n
AT+CIPMUX=1\r\n
AT+CIPSERVER=1,8086\r\n
AT+CIPSTO=1200\r\n
AT+CIPSTART=0,"UDP","192.168.1.111",8086\r\n

AT+RESTORE\r\n
AT+CWMODE?\r\n
AT+CWMODE=1\r\n
AT+RST\r\n
AT+CWJAP="ChinaNet-F4KRCA","YRW6PKGF"\r\n
AT+CWSAP="Kiran-IoT","12345678",10,3\r\n

AT+CIUPDATE\r\n

*/

