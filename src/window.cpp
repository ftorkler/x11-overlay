#include "window.h"

#include <iostream>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

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
    setFont("9x15bold");
}

X11Window::~X11Window() 
{
    XFreeFont(display, font);
    XFreeGC(display, gc);
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
    visual = DefaultVisual(display, screen);

    XVisualInfo visualInfo;
    XMatchVisualInfo(display, screen, 32, TrueColor, &visualInfo);

    attributes.colormap = XCreateColormap(display, rootWindow, visualInfo.visual, AllocNone);
    // attributes.background_pixel = createColor(255,255,255,64).pixel;
    attributes.override_redirect = true;

    unsigned long attrMask = CWColormap|CWBorderPixel|CWBackPixel|CWEventMask|CWWinGravity|CWBitGravity|CWSaveUnder|CWDontPropagate|CWOverrideRedirect;
    window = XCreateWindow(display, rootWindow, x, y, width, height, 0, visualInfo.depth, InputOutput, visualInfo.visual, attrMask, &attributes);

    XserverRegion region = XFixesCreateRegion(display, NULL, 0);
    XFixesSetWindowShapeRegion(display, window, ShapeInput, 0, 0, region);
    XFixesDestroyRegion(display, region);

    XMapWindow(display, window);

    gc = XCreateGC(display, window, 0, 0);
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
    delete monitors;

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
    this->width = width;
    this->height = height;
    XResizeWindow(display, window, width, height);
}

void X11Window::setFont(const std::string& fontname)
{
    if (font) {
        XFreeFont(display, font);
    }

    font = XLoadQueryFont(display, fontname.c_str());
    if (!font) {
        std::cout << "loading X font... FAILED" << std::endl;
        exit(1);
    }
    XSetFont(display, gc, font->fid);
}

XColor X11Window::createColor(
    unsigned short red, 
    unsigned short green, 
    unsigned short blue, 
    unsigned short alpha) 
{
    XColor color;
    color.red = red * alpha;
    color.green = green * alpha;
    color.blue = blue * alpha;
    color.flags = DoRed | DoGreen | DoBlue;

    if (!XAllocColor(display, DefaultColormap(display, screen), &color)) {
        std::cout << "creating X color... FAILED" << std::endl;
        exit(1);
    }

    color.pixel |= alpha << 24;
    return color;
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

void X11Window::drawRect(int x, int y, unsigned int w, unsigned int h, const XColor& color) const
{
    XSetForeground(display, gc, color.pixel);
    XFillRectangle(display, window, gc, x, y, w, h);
}

void X11Window::drawString(int x, int y, const std::string& text, const XColor& color) const
{
    XSetForeground(display, gc, color.pixel);
    XDrawString(display, window, gc, x, y + font->ascent, text.c_str(), text.length());
}

Position X11Window::getStringDimension(const std::string& text) const
{
    Position pos;
    pos.x = XTextWidth(font, text.c_str(), text.size()) - 1;
    pos.y = font->ascent + font->descent;
    return pos;
}
