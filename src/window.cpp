#include "window.h"

#include <iostream>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

#include "canvas.h"

X11Window::X11Window(int x, int y, unsigned int width, unsigned int height) 
: 
    x(x),
    y(y),
    width(width),
    height(height),
    monitor()
{
    createWindowContext();
    setActiveMonitor(0);
}

X11Window::~X11Window() 
{
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void X11Window::createWindowContext() 
{   
    display = XOpenDisplay(getenv("DISPLAY"));
    if (!display) {
        std::cout << "opening X display... FAILED" << std::endl;
        exit(1);
    }

    screen = DefaultScreen(display);
    rootWindow = DefaultRootWindow(display);

    XMatchVisualInfo(display, screen, 32, TrueColor, &visualInfo);

    XSetWindowAttributes attributes;
    attributes.colormap = XCreateColormap(display, rootWindow, visualInfo.visual, AllocNone);
    attributes.border_pixel = BlackPixel(display, screen);
    attributes.background_pixel = BlackPixel(display, screen);
    attributes.override_redirect = true;

    unsigned long attrMask = CWColormap | CWBorderPixel | CWBackPixel | CWOverrideRedirect;
    window = XCreateWindow(display, rootWindow, x, y, width, height, 0, visualInfo.depth, InputOutput, visualInfo.visual, attrMask, &attributes);

    XserverRegion region = XFixesCreateRegion(display, NULL, 0);
    XFixesSetWindowShapeRegion(display, window, ShapeInput, 0, 0, region);
    XFixesDestroyRegion(display, region);

    XMapWindow(display, window);
}

void X11Window::setActiveMonitor(int monitorIndex)
{
    int monitorCount;
    XRRMonitorInfo* monitors = XRRGetMonitors(display, rootWindow, true, &monitorCount);

    int index = std::min(monitorIndex, monitorCount - 1);
    monitor.x = monitors[index].x;
    monitor.y = monitors[index].y;
    monitor.width = monitors[index].width;
    monitor.height = monitors[index].height;

    move(x, y);
}

unsigned int X11Window::getWidth() const
{
    return width;
}

unsigned int X11Window::getHeight() const
{
    return height;
}

unsigned int X11Window::getMonitorWidth() const
{
    return monitor.width;
}

unsigned int X11Window::getMonitorHeight() const
{
    return monitor.height;
}

void X11Window::move(int x, int y)
{
    this->x = x;
    this->y = y;
    XMoveWindow(display, window, monitor.x + x, monitor.y + y);
}

void X11Window::resize(unsigned int width, unsigned int height)
{
    if (this->window != width || this->height != height) {
        this->width = width;
        this->height = height;
        XResizeWindow(display, window, width, height);
    }
}

X11Canvas* X11Window::createCanvas() const 
{
    return new X11Canvas(display, window, visualInfo.visual, screen);
}

bool X11Window::hasNextEvent()
{
    return XPending(display);
}

const XEvent& X11Window::nextEvent()
{
    XNextEvent(display, &event);
    return event;
}

Position X11Window::getMousePosition() const
{
    int rx,ry;
    unsigned int modifs;
    Window root, child;
    Position pos;

    if (!XQueryPointer(display, window, &root, &child, &rx, &ry, &pos.x, &pos.y, &modifs)) {
        std::cout << "querying pointer... FAILED" << std::endl;
        exit(1);
    }
    return pos;
}

void X11Window::clear() const
{
    XClearWindow(display, window);
}

void X11Window::flush() const
{
    XFlush(display);
}
