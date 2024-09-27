#include "inotify.h"

#include <climits>
#include <iostream>
#include <unistd.h>
#include <sys/inotify.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

Inotify::Inotify(const std::string& filename)
{
    fileDescr = inotify_init();
    if (fileDescr <= 0) {
        std::cerr << "File cannot be monitored: error in inotify_init." << std::endl;
        exit(1);
    }
    watchDecr = inotify_add_watch(fileDescr, filename.c_str(), IN_CLOSE_WRITE);
    if (watchDecr == -1) {
        std::cerr << "File cannot be monitored: error in inotify_add_watch." << std::endl;
        exit(1);
    }

    FD_ZERO(&rfds);
}

Inotify::~Inotify()
{
    if (inotify_rm_watch(fileDescr, watchDecr) < 0) {
        std::cerr << "File cannot be unmonitored: error in inotify_rm_watch." << std::endl;
    }
}

bool Inotify::hasFileBeenRewritten()
{
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    FD_SET(fileDescr, &rfds);
    int res = select(FD_SETSIZE, &rfds, nullptr, nullptr, &timeout);
    FD_ZERO(&rfds);

    if (res > 0) {
      char buf[BUF_LEN];
      read(fileDescr, buf, BUF_LEN);
    }
    return res > 0;
}
