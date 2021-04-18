#include "window.h"

#include <iostream>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

X11Window::X11Window(int x, int y, int width, int height) 
: 
    x(x),
    y(y),
    width(width),
    height(height)
{
    createWindowContext();
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
    visual = DefaultVisual(display, screen);

    XVisualInfo visualInfo;
    XMatchVisualInfo(display, screen, 32, TrueColor, &visualInfo);

    attributes.colormap = XCreateColormap(display, rootWindow, visualInfo.visual, AllocNone);
    attributes.background_pixel = WhitePixel(display, screen);
    attributes.override_redirect = true;

    unsigned long attrMask = CWColormap|CWBorderPixel|CWBackPixel|CWEventMask|CWWinGravity|CWBitGravity|CWSaveUnder|CWDontPropagate|CWOverrideRedirect;
    window = XCreateWindow(display, rootWindow, x, y, width, height, 0, visualInfo.depth, InputOutput, visualInfo.visual, attrMask, &attributes);

    XserverRegion region = XFixesCreateRegion(display, NULL, 0);
    XFixesSetWindowShapeRegion(display, window, ShapeInput, 0, 0, region);
    XFixesDestroyRegion(display, region);

    XMapWindow(display, window);
}

void X11Window::clear() 
{
    XClearWindow(display, window);
}

void X11Window::flush() 
{
    XFlush(display);
}