#ifndef OVERLAY_WINDOW_H
#define OVERLAY_WINDOW_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include "pair.h"

class X11Canvas;

class X11Window
{

public:

    X11Window(int x, int y, unsigned int width, unsigned int height);
    ~X11Window();

    void setActiveMonitor(int monitorIndex);
    unsigned int getMonitorWidth() const;
    unsigned int getMonitorHeight() const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;

    void move(int x, int y);
    void resize(unsigned int width, unsigned int height);

    X11Canvas* createCanvas() const;
    bool hasNextEvent();
    const XEvent& nextEvent();
    Position getMousePosition() const;

    void clear() const;
    void flush() const;

private:

    void createWindowContext();

    int x, y;
    unsigned int width, height;

    int screen;
    Display* display;
    Window rootWindow;
    Window window;
    XVisualInfo visualInfo;
    XRRMonitorInfo monitor;

    XEvent event;
};

#endif // OVERLAY_WINDOW_H
