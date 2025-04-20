#ifndef OVERLAY_CANVAS_H
#define OVERLAY_CANVAS_H

#include <string>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include "color.h"
#include "pair.h"

class X11Canvas
{

public:

    X11Canvas(Display* display, Window window, Visual* visual, int screen);
    ~X11Canvas();

    void setFont(int n, const std::string& fontname);
    void setColor(const Color& color);
    void drawRect(int x, int y, unsigned int w, unsigned int h) const;
    void drawString(int x, int y, int font_n, const std::string& text) const;
    IntPair getStringDimension(int font_n, const std::string& text) const;

private:

    Display* display;
    Visual* visual;
    Colormap colormap;
    int screen;

    XftFont* xftFonts[10];
    XftDraw* xftDraw;
    XftColor xftColor;

};

#endif // OVERLAY_CANVAS_H
