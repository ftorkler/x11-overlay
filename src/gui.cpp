#include "gui.h"

#include <iostream>

#include "window.h"

Gui::Gui()
:
    messageY(0), 
    messageMaxWidth(0),
    mouseOver(false),
    dirty(true)
{
    window = new X11Window(100, 100, 480, 640);

    bgColor = window->createColor(0, 0, 0, 100);
    bgDimColor = window->createColor(0, 0, 0, 25);
    redColor = window->createColor(255, 0, 0, 200);
    redDimColor = window->createColor(255, 0, 0, 50);
}

Gui::~Gui()
{
    delete window;
}

void Gui::flush()
{
    int w = messageMaxWidth;
    int h = messageY;

    Position pos = window->getMousePosition();
    bool isMouseOver = pos.x >= 0 && pos.y >= 0 && pos.x < w && pos.y < h;
    dirty |= mouseOver == isMouseOver;
    mouseOver = isMouseOver;

    if (!dirty) {
        return;
    }

    if (w >0 && h > 0) {
        window->resize(w, h);   

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

void Gui::clearMessages() {
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
    messageY += dim.y + 1;
}
