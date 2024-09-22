#ifndef OVERLAY_GUI_H
#define OVERLAY_GUI_H

#include <string>
#include <X11/Xlib.h>
#include <vector>

#include "ansi.h"
#include "color.h"

class X11Canvas;
class X11Window;
class DrawCmd;

class Gui
{

public:

    enum Orientation {
        N, NE, E, SE, S, SW, W, NW, CENTER, NONE
    } orientation;

    Gui();
    ~Gui();

    void setDefaultForgroundColor(const Color& color);
    void setDefaultBackgroundColor(const Color& color);
    void setColorProfile(Ansi::Profile profile);
    void setDimming(const float& dimming);
    void setMouseOverDimming(const float& dimming);
    void setMouseOverTolerance(unsigned int tolerance);
    void setOrientation(Orientation orientation);
    void setScreenEdgeSpacing(unsigned int spacing);
    void setLineSpacing(unsigned int spacing);
    void setMonitorIndex(unsigned int index);
    void setFont(const std::string& font);

    void flush();
    void clearMessages();
    void addMessage(const std::string& message);

    static std::string orientationToString(Orientation orientation);
    static Orientation orientationFromString(const std::string& input);

private:

    struct ClippingAABB {
        ClippingAABB(int x, int y, unsigned int w, unsigned int h): x(x), y(y), w(w), h(h) {}
        const int x, y;
        const unsigned int w, h;
    };

    std::string trimForOrientation(const std::string& text) const;
    bool isMouseOver() const;
    int calcXforOrientation(unsigned int innerWidth, unsigned int outerWidth, unsigned int spacing) const;
    int calcYforOrientation(unsigned int innerHeight, unsigned int outerHeight, unsigned int spacing) const;
    void updateWindowPosition() const;

    X11Window* window;
    X11Canvas* canvas;
    std::vector<DrawCmd*> drawBgCommands;
    std::vector<DrawCmd*> drawFgCommands;
    std::vector<ClippingAABB> clippingBoxes;
    int messageY, messageMaxWidth;
    bool mouseOver;
    bool redraw, recalc;
    bool increaseIntensity;
    std::string lastFgColor;
    Ansi::Profile colorProfile;

    unsigned int screenEdgeSpacing;
    unsigned int lineSpacing;
    unsigned int mouseOverTolerance;
    float alpha;
    float mouseOverAlpha;
    Color fgColor;
    Color bgColor;

};


class DrawCmd
{

public:

    virtual ~DrawCmd() {};
    virtual void draw(X11Canvas* canvas, int offsetX, float alpha) const = 0;

};

class DrawColorCmd : public DrawCmd
{

public:

    DrawColorCmd(const Color& color);

    virtual void draw(X11Canvas* canvas, int offsetX, float alpha) const;

private:

    Color color;

};

class DrawRectCmd : public DrawCmd
{

public:

    DrawRectCmd(int x, int y, unsigned int w, unsigned int h);

    virtual void draw(X11Canvas* canvas, int offsetX, float alpha) const;

private:

    int x, y;
    unsigned int w, h;

};

class DrawTextCmd : public DrawCmd
{

public:

    DrawTextCmd(int x, int y, const std::string& text);

    virtual void draw(X11Canvas* canvas, int offsetX, float alpha) const;

private:

    int x, y;
    std::string text;

};

#endif // OVERLAY_GUI_H
