#include "gui.h"

#include <iostream>

#include "window.h"

Gui::Gui()
:
    messageY(0), 
    messageMaxWidth(0),
    mouseOver(false)
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
    mouseOver = pos.x >= 0 && pos.y >= 0 && pos.x < w && pos.y < h;

    window->resize(w, h);
    window->flush();
    window->clear();

    messageMaxWidth = 0;
    messageY = 0;
}

void Gui::drawMessage(const std::string& message) 
{
    Dimesion dim = window->getStringDimension(message);

    if (dim.x > messageMaxWidth) {
        messageMaxWidth = dim.x;
    }

    if (message.size()) {
        window->drawRect(0, messageY, dim.x, dim.y, mouseOver ? bgDimColor : bgColor);
    }
    window->drawString(0, messageY, message, mouseOver ? redDimColor : redColor);

    messageY += dim.y + 1;
}
