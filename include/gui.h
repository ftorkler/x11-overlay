#ifndef OVERLAY_GUI_H
#define OVERLAY_GUI_H

#include <string>
#include <X11/Xlib.h>
#include <vector>

class X11Window;

class Gui
{

public:

    Gui();
    ~Gui();

    void setMouseOverTolerance(unsigned int tolerance);

    void flush();
    void clearMessages();
    void addMessage(const std::string& message);

private:

    struct Line {
        
        const int x, y;
        const unsigned int w, h;
        const std::string message;

        Line(int x, int y, unsigned int w, unsigned  int h, const std::string& message) 
            : x(x), y(y), w(w), h(h), message(message) { };
    };

    bool isMouseOver() const;

    X11Window* window;

    std::vector<Line> lines;
    int messageY, messageMaxWidth;
    unsigned int mouseOverTolerance;
    bool mouseOver;
    bool dirty;

    XColor bgColor;
    XColor bgDimColor;
    XColor redColor;
    XColor redDimColor;

};

#endif // OVERLAY_GUI_H
