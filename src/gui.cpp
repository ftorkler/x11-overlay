#include "gui.h"

#include <algorithm>
#include <iostream>

#include "canvas.h"
#include "window.h"

Gui::Gui()
:
    orientation(NW),
    messageY(0), 
    messageMaxWidth(0),
    mouseOver(false),
    redraw(true),
    recalc(true),
    screenEdgeSpacing(0),
    lineSpacing(0),
    mouseOverTolerance(0),
    dimming(0.75)
{
    window = new X11Window(0, 0, 480, 640);
    canvas = window->createCanvas();

    setDefaultBackgroundColor(0, 0, 0, 100);
    setDefaultForgroundColor(255, 0, 0, 200);
}

Gui::~Gui()
{
    delete canvas;
    delete window;
}

void Gui::setDefaultForgroundColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    redraw = true;
    fgColor = Color(r, g, b, a);
    setMouseOverDimming(dimming);
}

void Gui::setDefaultBackgroundColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    redraw = true;
    bgColor = Color(r, g, b, a);
    setMouseOverDimming(dimming);
}

void Gui::setMouseOverDimming(const float& dimming)
{
    redraw = true;
    this->dimming = dimming;
    fgColorDim = Color(fgColor.r, fgColor.g, fgColor.b, (unsigned char)(fgColor.a * (1.0 - dimming)));
    bgColorDim = Color(bgColor.r, bgColor.g, bgColor.b, (unsigned char)(bgColor.a * (1.0 - dimming)));
}

void Gui::setMouseOverTolerance(unsigned int tolerance)
{
    redraw = true;
    mouseOverTolerance = tolerance;
}

void Gui::setOrientation(Orientation orientation) 
{
    redraw = true;
    recalc = true;
    this->orientation = orientation;
}

void Gui::setScreenEdgeSpacing(unsigned int spacing)
{
    redraw = true;
    screenEdgeSpacing = spacing;
}

void Gui::setLineSpacing(unsigned int spacing)
{
    redraw = true;
    recalc = true;
    lineSpacing = spacing;
}

void Gui::flush()
{
    int w = messageMaxWidth;
    int h = messageY;

    if (recalc) {
        // TODO recalculation of individual line positions (or simply reinsert all messages once again?)
    }

    bool currentMouseOver = isMouseOver();
    redraw |= mouseOver == currentMouseOver;
    mouseOver = currentMouseOver;

    if (!redraw) {
        return;
    }

    window->clear();

    if (w >0 && h > 0) {
        window->resize(w, h);   
        updateWindowPosition();

        int offsetX = calcXforOrientation(0, messageMaxWidth, 0);

        canvas->setColor(mouseOver ? bgColorDim : bgColor);
        for (auto cmd : drawBgCommands) {
            cmd->draw(canvas, offsetX, mouseOver);
        }
        canvas->setColor(mouseOver ? fgColorDim : fgColor);
        for (auto cmd : drawFgCommands) {
            cmd->draw(canvas, offsetX, mouseOver);
        }
    }

    window->flush();

    redraw = false;
}

void Gui::clearMessages() 
{
    redraw = true;

    messageMaxWidth = 0;
    messageY = 0;

    drawBgCommands.erase(drawBgCommands.begin(), drawBgCommands.end());
    drawFgCommands.erase(drawFgCommands.begin(), drawFgCommands.end());
    clippingBoxes.clear();
}


void Gui::addMessage(const std::string& message) 
{
    redraw = true;

    std::string trimmedMessage = trimForOrientation(message);

    Dimesion dim = canvas->getStringDimension(trimmedMessage);

    if (!trimmedMessage.empty()) {
        int x = calcXforOrientation(dim.w, 0, 0);
        clippingBoxes.emplace_back(ClippingAABB(x, messageY, dim.w, dim.h));
        drawBgCommands.emplace_back(new DrawRectCmd(x, messageY, dim.w, dim.h));

        drawFgCommands.emplace_back(new DrawTextCmd(x, messageY, trimmedMessage));
    }

    if (dim.x > messageMaxWidth) {
        messageMaxWidth = dim.w;
    }
    messageY += dim.h + lineSpacing;
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
        int offsetX = calcXforOrientation(0, messageMaxWidth, 0);
        for (auto box : clippingBoxes) {
            if (pos.x + t >= box.x + offsetX && 
                pos.y + t >= box.y && 
                pos.x - t <= box.x + offsetX + (int)box.w && 
                pos.y - t <= box.y + (int)box.h)
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

std::string Gui::orientationToString(Orientation orientation)
{
    switch (orientation)
    {
        case N: return "N";
        case NE: return "NE";
        case E: return "E";
        case SE: return "SE";
        case S: return "S";
        case SW: return "SW";
        case W: return "W";
        case NW: return "NW";
        case CENTER: return "CENTER";
        default: return "";
    }
}

Gui::Orientation Gui::orientationFromString(const std::string& input)
{
    for (int orientation=N; orientation<NONE; ++orientation) {
        if (input == orientationToString(static_cast<Orientation>(orientation))) {
            return static_cast<Orientation>(orientation);
        }
    }
    return Orientation::NONE;
}



DrawColorCmd::DrawColorCmd(const Color& color)
:
    color(color)
{
}

 void DrawColorCmd::draw(X11Canvas* canvas, int offsetX, bool isMouseOver) const
 {
     canvas->setColor(color);
 }

DrawRectCmd::DrawRectCmd(int x, int y, unsigned int w, unsigned int h)
:
    DrawCmd(),
    x(x),
    y(y),
    w(w),
    h(h)
{
}

void DrawRectCmd::draw(X11Canvas* canvas, int offsetX, bool isMouseOver) const
{
    canvas->drawRect(x + offsetX, y, w, h);
}

DrawTextCmd::DrawTextCmd(int x, int y, const std::string& text)
:
    DrawCmd(),
    x(x),
    y(y),
    text(text)
{
}

void DrawTextCmd::draw(X11Canvas* canvas, int offsetX, bool isMouseOver) const
{
    canvas->drawString(x + offsetX, y, text);
}
