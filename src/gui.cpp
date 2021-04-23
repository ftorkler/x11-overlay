#include "gui.h"

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
    int w = window->getStringWidth(message);
    int h = window->getStringHeight(message);

    if (w > messageMaxWidth) {
        messageMaxWidth = w;
    }

    if (message.size()) {
        window->drawRect(0, messageY, w, h, bgColor);
    }
    window->drawString(0, messageY, message, redColor);

    messageY += h + 1;
}
