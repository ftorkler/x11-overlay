#include "gui.h"

#include <iostream>

#include "window.h"

Gui::Gui()
:
    messageY(0), 
    messageMaxWidth(0)
{
    window = new X11Window(100, 100, 480, 640);

    bgColor = window->createColor(0, 0, 0, 100);
    redColor = window->createColor(255, 0, 0, 200);
}

Gui::~Gui()
{
    delete window;
}

void Gui::flush()
{
    window->resize(messageMaxWidth, messageY);
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
        window->drawRect(0, messageY, dim.x, dim.y, bgColor);
    }
    window->drawString(0, messageY, message, redColor);

    messageY += dim.y + 1;
}
