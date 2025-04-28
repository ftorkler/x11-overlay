#include "canvas.h"

#include <X11/X.h>
#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>
#include <cstdlib>
#include <fontconfig/fontconfig.h>
#include <iostream>
#include <string>
#include <cassert>

#include "color.h"
#include "pair.h"

X11Canvas::X11Canvas(Display* display, Window window, Visual* visual, int screen)
:
    display(display),
    visual(visual),
    screen(screen)
//    xftFont(nullptr)
{
    colormap = DefaultColormap(display, screen);
    xftDraw = XftDrawCreate(display, window, visual, colormap);

    if (!XftColorAllocName(display, visual, colormap, "#000000", &xftColor)) {
        std::cout << "creating dummy color... FAILED" << std::endl;
    }
    for(int i=0; i<10; i++) {
      xftFonts[i]=nullptr;
      setFont(i, "");
    }

}

X11Canvas::~X11Canvas()
{
    XftColorFree(display, visual, colormap, &xftColor);
    for(int i=0; i<10; i++)
      if (xftFonts[i])
        XftFontClose(display, xftFonts[i]);
    XftDrawDestroy(xftDraw);
}

void X11Canvas::setFont(int n, const std::string& fontname)
{
    if(n<0||n>9) return;
    if (xftFonts[n]) {
        XftFontClose(display, xftFonts[n]);
    }
    xftFonts[n] = XftFontOpenName(display, screen, fontname.c_str());
    if (!xftFonts[n]) {
        std::cout << "loading Xft font... FAILED" << std::endl;
        exit(1);
    }
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

void X11Canvas::drawString(int x, int y, int n, const std::string& text) const
{
    XftDrawStringUtf8(xftDraw, &xftColor, xftFonts[n], x, y + xftFonts[n]->ascent, (const FcChar8*)text.c_str(), text.size());
}

IntPair X11Canvas::getStringDimension(int n, const std::string& text) const
{
    assert(n>=0 && n<=9);

    XGlyphInfo extents;
    XftTextExtentsUtf8(display, xftFonts[n], (const FcChar8*)text.c_str(), text.size(), &extents);

    IntPair p;
    p.w = extents.xOff;
    p.h = xftFonts[n]->ascent + xftFonts[n]->descent;
    return p;
}
