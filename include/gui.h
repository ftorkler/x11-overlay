#ifndef OVERLAY_GUI_H
#define OVERLAY_GUI_H

#include <string>
#include <X11/Xlib.h>

class X11Window;

class Gui
{

public:

    Gui();
    ~Gui();

    void flush();
    void drawMessage(const std::string& message);

private:

    X11Window* window;

    int messageY, messageMaxWidth;

    XColor bgColor;
    XColor redColor;

};

#endif // OVERLAY_GUI_H
