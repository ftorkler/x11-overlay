#include "gui.h"

#include <iostream>

#include "window.h"

Gui::Gui()
:
    orientation(NW),
    messageY(0), 
    messageMaxWidth(0),
    screenEdgeSpacing(0),
    lineSpacing(0),
    mouseOverTolerance(0),
    mouseOver(false),
    dirty(true)
{
    window = new X11Window(0, 0, 480, 640);

    bgColor = window->createColor(0, 0, 0, 100);
    bgDimColor = window->createColor(0, 0, 0, 25);
    redColor = window->createColor(255, 0, 0, 200);
    redDimColor = window->createColor(255, 0, 0, 50);
}

Gui::~Gui()
{
    delete window;
}

void Gui::setMouseOverTolerance(unsigned int tolerance)
{
    dirty = true;
    mouseOverTolerance = tolerance;
}

void Gui::setOrientation(Orientation orientation) 
{
    dirty = true;
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

    bool currentMouseOver = isMouseOver();
    dirty |= mouseOver == currentMouseOver;
    mouseOver = currentMouseOver;

    if (!dirty) {
        return;
    }

    if (w >0 && h > 0) {
        window->resize(w, h);   
        updateWindowPosition();

        for (auto line : lines) {
            if (line.message.size()) {
                window->drawRect(line.x, line.y, line.w, line.h, mouseOver ? bgDimColor : bgColor);
                window->drawString(line.x, line.y, line.message, mouseOver ? redDimColor : redColor);        
            }
        }
    }

    window->flush();
    window->clear();

    dirty = false;
}

void Gui::clearMessages() 
{
    dirty = true;

    messageMaxWidth = 0;
    messageY = 0;

    lines.clear();
}

void Gui::addMessage(const std::string& message) 
{
    dirty = true;

    Dimesion dim = window->getStringDimension(message);

    lines.emplace_back(Line(0, messageY, dim.x, dim.y, message));

    if (dim.x > messageMaxWidth) {
        messageMaxWidth = dim.x;
    }
    messageY += dim.y + lineSpacing;
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

void Gui::updateWindowPosition() const
{
    int windowW = window->getWidth();
    int windowH = window->getHeight();
    int monitorW = window->getMonitorWidth();
    int monitorH = window->getMonitorHeight();

    int x, y;

    switch (orientation) {
        case NW:
        case W:
        case SW:
            x = 0 + screenEdgeSpacing;
            break;
        case N:
        case CENTER:
        case S:
            x = monitorW / 2 - windowW / 2;
            break;
        case NE:
        case E:
        case SE:
            x = monitorW - windowW - screenEdgeSpacing;
            break;
    }

    switch (orientation) {
        case NW:
        case N:
        case NE:
            y = 0 + screenEdgeSpacing;
            break;
        case W:
        case CENTER:
        case E:
            y = monitorH / 2 - windowH / 2;
            break;
        case SW:
        case S:
        case SE:
            y = monitorH - windowH - screenEdgeSpacing;
            break;
    }

    window->move(x, y);
}
