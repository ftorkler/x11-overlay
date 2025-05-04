#ifndef OVERLAY_CANVAS_H
#define OVERLAY_CANVAS_H

#include <string>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <vector>

#include "color.h"
#include "pair.h"

class X11Canvas
{

public:

    X11Canvas(Display* display, Window window, Visual* visual, int screen);
    ~X11Canvas();

    void setFont(unsigned int fontIndex, const std::string& fontname);
    unsigned int selectFont(unsigned int fontIndex);
    unsigned int getSelectedFontAscent() const;

    //void setFont(const std::string& fontname);
    void setColor(const Color& color);
    void drawRect(int x, int y, unsigned int w, unsigned int h) const;
    void drawString(int x, int y, const std::string& text) const;
    IntPair getStringDimension(const std::string& text) const;

private:

    Display* display;
    Visual* visual;
    Colormap colormap;
    int screen;

    unsigned int fontIndex;
    XftFont* xftFonts[10];
    XftDraw* xftDraw;
    XftColor xftColor;

};

#endif // OVERLAY_CANVAS_H
