#include "gui.h"

#include <algorithm>
#include <iostream>

#include "window.h"

Gui::Gui()
:
    orientation(NW),
    messageY(0), 
    messageMaxWidth(0),
    mouseOver(false),
    dirty(true),
    dirtyLines(true),
    screenEdgeSpacing(0),
    lineSpacing(0),
    mouseOverTolerance(0),
    dimming(0.75)
{
    window = new X11Window(0, 0, 480, 640);

    setDefaultBackgroundColor(0, 0, 0, 100);
    setDefaultForgroundColor(255, 0, 0, 200);
}

Gui::~Gui()
{
    delete window;
}

void Gui::setDefaultForgroundColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    dirty = true;
    fgColor.r = r;
    fgColor.g = g;
    fgColor.b = b;
    fgColor.a = a;
    fgColor.xcolor = window->createColor(r, g, b, a);
    setMouseOverDimming(dimming);
}

void Gui::setDefaultBackgroundColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    dirty = true;
    bgColor.r = r;
    bgColor.g = g;
    bgColor.b = b;
    bgColor.a = a;
    bgColor.xcolor = window->createColor(r, g, b, a);
    setMouseOverDimming(dimming);
}

void Gui::setMouseOverDimming(const float& dimming)
{
    dirty = true;
    this->dimming = dimming;
    fgColor.xcolorDim = window->createColor(fgColor.r, fgColor.g, fgColor.b, (unsigned char)(fgColor.a * (1.0 - dimming)));
    bgColor.xcolorDim = window->createColor(bgColor.r, bgColor.g, bgColor.b, (unsigned char)(bgColor.a * (1.0 - dimming)));
}

void Gui::setMouseOverTolerance(unsigned int tolerance)
{
    dirty = true;
    mouseOverTolerance = tolerance;
}

void Gui::setOrientation(Orientation orientation) 
{
    dirty = true;
    dirtyLines = true;
    this->orientation = orientation;
}

void Gui::setScreenEdgeSpacing(unsigned int spacing)
{
    dirty = true;
    screenEdgeSpacing = spacing;
}

void Gui::setLineSpacing(unsigned int spacing)
{
    dirty = true;
    lineSpacing = spacing;
}

void Gui::flush()
{
    int w = messageMaxWidth;
    int h = messageY;

    if (dirtyLines) {
        dirtyLines = false;

        for (auto& line : lines) {
            line.x = calcXforOrientation(line.w, messageMaxWidth, 0);
        }
    }

    bool currentMouseOver = isMouseOver();
    dirty |= mouseOver == currentMouseOver;
    mouseOver = currentMouseOver;

    if (!dirty) {
        return;
    }

    window->clear();

    if (w >0 && h > 0) {
        window->resize(w, h);   
        updateWindowPosition();

        for (auto line : lines) {
            if (line.message.size()) {
                window->drawRect(line.x, line.y, line.w, line.h, mouseOver ? bgColor.xcolorDim : bgColor.xcolor);
                window->drawString(line.x, line.y, line.message, mouseOver ? fgColor.xcolorDim : fgColor.xcolor);
            }
        }
    }

    window->flush();

    dirty = false;
}

void Gui::clearMessages() 
{
    dirty = true;
    dirtyLines = true;

    messageMaxWidth = 0;
    messageY = 0;

    lines.clear();
}


void Gui::addMessage(const std::string& message) 
{
    dirty = true;
    dirtyLines = true;

    std::string trimmedMessage = trimForOrientation(message);

    Dimesion dim = window->getStringDimension(trimmedMessage);

    lines.emplace_back(Line(0, messageY, dim.x, dim.y, trimmedMessage));

    if (dim.x > messageMaxWidth) {
        messageMaxWidth = dim.x;
    }
    messageY += dim.y + lineSpacing;
}

std::string Gui::trimForOrientation(const std::string& text) const
{
    if (orientation != NW && orientation != W && orientation != SW) {
        size_t start = text.find_first_not_of(" ");
        std::string mutableText = (start == std::string::npos) ? "" : text.substr(start);

        if (orientation == NE || orientation == E || orientation == SE) {
            for (int i=0; i<(int)start; ++i) {
                mutableText.append(" ");
            }
        }
        return mutableText;
    }
    return text;
}

bool Gui::isMouseOver() const
{
    int w = messageMaxWidth;
    int h = messageY;
    int t = mouseOverTolerance;

    Position pos = window->getMousePosition();
    bool isInFrame = 
        pos.x + t >= 0 && 
        pos.y + t >= 0 && 
        pos.x - t < w && 
        pos.y - t < h;
        
    if (isInFrame) {
        for (auto line : lines) {
            if (pos.x + t >= line.x && 
                pos.y + t >= line.y && 
                pos.x - t <= line.x + (int)line.w && 
                pos.y - t <= line.y + (int)line.h) 
            {
                return true;
            }
        }
    }
    return false;
}

int Gui::calcXforOrientation(unsigned int innerWidth, unsigned int outerWidth, unsigned int spacing) const
{
    switch (orientation) {
        case NW:
        case W:
        case SW:
        default:
            return spacing;
        case N:
        case CENTER:
        case S:
            return outerWidth / 2 - innerWidth / 2;
        case NE:
        case E:
        case SE:
            return outerWidth - innerWidth - spacing;
    }
}

int Gui::calcYforOrientation(unsigned int innerHeight, unsigned outerHeight, unsigned int spacing) const
{
    switch (orientation) {
        case NW:
        case N:
        case NE:
        default:
            return spacing;
        case W:
        case CENTER:
        case E:
            return outerHeight / 2 - innerHeight / 2;
        case SW:
        case S:
        case SE:
            return outerHeight - innerHeight - spacing;
    }
}

void Gui::updateWindowPosition() const
{
    window->move(
        calcXforOrientation(window->getWidth(), window->getMonitorWidth(), screenEdgeSpacing),
        calcYforOrientation(window->getHeight(), window->getMonitorHeight(), screenEdgeSpacing));
}
