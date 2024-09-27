#ifndef OVERLAY_INOTIFY_H
#define OVERLAY_INOTIFY_H

#include <string>

class Inotify
{

public:
    
    Inotify(const std::string& filename);
    ~Inotify();

    bool hasFileBeenRewritten();

private:

    int fileDescr;
    int watchDecr;
    fd_set rfds;
    struct timeval timeout;

};

#endif // OVERLAY_INOTIFY_H
