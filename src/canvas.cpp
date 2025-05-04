#include "canvas.h"

#include <X11/X.h>
#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>
#include <cstdlib>
#include <fontconfig/fontconfig.h>
#include <iostream>
#include <string>

#include "color.h"
#include "pair.h"

X11Canvas::X11Canvas(Display* display, Window window, Visual* visual, int screen)
:
    display(display),
    visual(visual),
    screen(screen),
    fontIndex(0),
    xftFonts()
{
    colormap = DefaultColormap(display, screen);
    xftDraw = XftDrawCreate(display, window, visual, colormap);

    if (!XftColorAllocName(display, visual, colormap, "#000000", &xftColor)) {
        std::cout << "creating dummy color... FAILED" << std::endl;
    }
    setFont(0, "");
}

X11Canvas::~X11Canvas()
{
    XftColorFree(display, visual, colormap, &xftColor);
    for (auto xftFont : xftFonts) {
        if (xftFont) {
            XftFontClose(display, xftFont);
        }
    }
    XftDrawDestroy(xftDraw);
}

void X11Canvas::setFont(unsigned int index, const std::string& fontname)
{
    if (index < 0 || index > 9) {
        return;
    }
    XftFont* xftFont = XftFontOpenName(display, screen, fontname.c_str());
    if (!xftFont) {
        std::cout << "loading Xft font... FAILED" << std::endl;
        exit(1);
    }
    if (xftFonts[index]) {
        XftFontClose(display, xftFonts[index]);
    }
    xftFonts[index] = xftFont;
}

unsigned int X11Canvas::selectFont(unsigned int index) {
    if (index >= 0 && index <= 9 && xftFonts[index]) {
        fontIndex = index;
    }
    return fontIndex;
}

unsigned int X11Canvas::getSelectedFontAscent() const
{
    return xftFonts[fontIndex]->ascent;
}

void X11Canvas::setColor(const Color& color)
{
    XRenderColor oldColor = xftColor.color;
    XRenderColor newColor;
    newColor.red = (unsigned short)color.r * color.a;
    newColor.green = (unsigned short)color.g * color.a;
    newColor.blue = (unsigned short)color.b * color.a;
    newColor.alpha = (unsigned short)color.a * 255;

    if (newColor.red != oldColor.red ||
        newColor.green != oldColor.green ||
        newColor.blue != oldColor.blue ||
        newColor.alpha != oldColor.alpha)
    {
        XftColorFree(display, visual, colormap, &xftColor);
        if (!XftColorAllocValue(display, visual, colormap, &newColor, &xftColor)) {
            std::cout << "creating color rgba(" << (int)color.r << "," << (int)color.g << "," << (int)color.b << "," << (int)color.a << ")... FAILED" << std::endl;
            exit(1);
        }
    }
}

void X11Canvas::drawRect(int x, int y, unsigned int w, unsigned int h) const
{
    XftDrawRect(xftDraw, &xftColor, x, y, w, h);
}

void X11Canvas::drawString(int x, int y, const std::string& text) const
{
    XftDrawStringUtf8(xftDraw, &xftColor, xftFonts[fontIndex], x, y, (const FcChar8*)text.c_str(), text.size());
}

IntPair X11Canvas::getStringDimension(const std::string& text) const
{
    XGlyphInfo extents;
    XftTextExtentsUtf8(display, xftFonts[fontIndex], (const FcChar8*)text.c_str(), text.size(), &extents);

    IntPair p;
    p.w = extents.xOff;
    p.h = xftFonts[fontIndex]->ascent + xftFonts[fontIndex]->descent;
    return p;
}
