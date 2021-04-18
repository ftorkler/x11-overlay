#ifndef OVERLAY_WINDOW_H
#define OVERLAY_WINDOW_H

#include <string>
#include <X11/X.h>
#include <X11/Xlib.h>

class X11Window
{

public:

    X11Window(int x, int y, int width, int height);
    ~X11Window();

    void clear();
    void flush();

private:

    void createWindowContext();

    int x, y, width, height;

    int screen;
    Display* display;
    Window rootWindow;
    Window window;
    Visual* visual;
    XSetWindowAttributes attributes;
    
};

#endif // OVERLAY_WINDOW_H