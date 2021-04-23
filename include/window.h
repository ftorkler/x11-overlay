#ifndef OVERLAY_WINDOW_H
#define OVERLAY_WINDOW_H

#include <string>
#include <X11/X.h>
#include <X11/Xlib.h>

class X11Window
{

public:

    X11Window(int x, int y, unsigned int width, unsigned int height);
    ~X11Window();

    void setActiveMonitor(int monitorIndex);
    void move(int x, int y);
    void resize(unsigned int width, unsigned int height);

    XColor createColor(unsigned short red, unsigned short green, unsigned short blue, unsigned short alpha);

    void clear() const;
    void flush() const;
    void drawRect(int x, int y, unsigned int w, unsigned int h, const XColor& color) const;
    void drawString(int x, int y, const std::string& text, const XColor& color) const;
    void setFont(const std::string& fontname);

    int getStringWidth(const std::string& text) const;
    int getStringHeight(const std::string& text) const;

private:

    void createWindowContext();

    int x, y;
    unsigned int width, height;
    int monitorOffsetX, monitorOffsetY;

    int screen;
    Display* display;
    Window rootWindow;
    Window window;
    Visual* visual;
    XSetWindowAttributes attributes;

    GC gc;
    XFontStruct* font;
    
};

#endif // OVERLAY_WINDOW_H