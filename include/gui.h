#ifndef OVERLAY_GUI_H
#define OVERLAY_GUI_H

#include <string>
#include <X11/Xlib.h>
#include <vector>

class X11Window;

class Gui
{

public:

    enum Orientation {
        N, NE, E, SE, S, SW, W, NW, CENTER
    } orientation;

    Gui();
    ~Gui();

    void setMouseOverTolerance(unsigned int tolerance);
    void setOrientation(Orientation orientation);
    void setScreenEdgeSpacing(unsigned int spacing);
    void setLineSpacing(unsigned int spacing);

    void flush();
    void clearMessages();
    void addMessage(const std::string& message);

private:

    struct Line {
        
        int x, y;
        const unsigned int w, h;
        const std::string message;

        Line(int x, int y, unsigned int w, unsigned  int h, const std::string& message) 
            : x(x), y(y), w(w), h(h), message(message) { };
    };

    std::string trimForOrientation(const std::string& text) const;
    bool isMouseOver() const;
    int calcXforOrientation(unsigned int innerWidth, unsigned int outerWidth, unsigned int spacing) const;
    int calcYforOrientation(unsigned int innerHeight, unsigned int outerHeight, unsigned int spacing) const;
    void updateWindowPosition() const;

    X11Window* window;
    std::vector<Line> lines;
    int messageY, messageMaxWidth;
    bool mouseOver;
    bool dirty, dirtyLines;

    unsigned int screenEdgeSpacing;
    unsigned int lineSpacing;
    unsigned int mouseOverTolerance;
    XColor bgColor;
    XColor bgDimColor;
    XColor redColor;
    XColor redDimColor;

};

#endif // OVERLAY_GUI_H
