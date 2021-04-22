#include "window.h"

#include <iostream>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xrandr.h>

X11Window::X11Window(int x, int y, int width, int height) 
: 
    x(x),
    y(y),
    width(width),
    height(height)
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
    XMoveWindow(display, window, monitors[index].x + x, monitors[index].y + y);

    delete monitors;
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

int X11Window::getStringWidth(const std::string& text) const
{
    return XTextWidth(font, text.c_str(), text.size()) - 1;
}

int X11Window::getStringHeight(const std::string& text) const
{
    return font->ascent + font->descent;
}
