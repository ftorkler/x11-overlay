#ifndef OVERLAY_GUI_H
#define OVERLAY_GUI_H

#include <string>
#include <X11/Xlib.h>
#include <vector>

#include "ansi.h"
#include "color.h"

class X11Canvas;
class X11Window;
class Line;

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

    static const std::string trimLinefeedsAndApplyTabs(const std::string& text);
    static const std::string trimForOrientation(const Orientation orientation, const std::string& text);
    void layoutLines();
    bool isMouseOver() const;
    int calcXforOrientation(unsigned int innerWidth, unsigned int outerWidth, unsigned int spacing) const;
    int calcYforOrientation(unsigned int innerHeight, unsigned int outerHeight, unsigned int spacing) const;
    bool updateWindowPosition() const;

    X11Window* window;
    X11Canvas* canvas;
    std::vector<Line*> lines;
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

    DrawCmd(const unsigned int& width = 0, const unsigned int& height = 0): w(width), h(height) { }
    virtual ~DrawCmd() { }
    virtual void draw(X11Canvas* canvas, int offsetX, int offsetY, float alpha) const = 0;

    unsigned int getWidth() const { return w; }
    unsigned int getHeight() const { return h; }

private:

    unsigned int w;
    unsigned int h;

};

class DrawColorCmd : public DrawCmd
{

public:

    DrawColorCmd(const Color& color);

    virtual void draw(X11Canvas* canvas, int offsetX, int offsetY, float alpha) const;

private:

    Color color;

};

class DrawRectCmd : public DrawCmd
{

public:

    DrawRectCmd(unsigned int w, unsigned int h);

    virtual void draw(X11Canvas* canvas, int offsetX, int offsetY, float alpha) const;

};

class DrawTextCmd : public DrawCmd
{

public:

    DrawTextCmd(unsigned int w, unsigned int h, const std::string& text);

    virtual void draw(X11Canvas* canvas, int offsetX, int offsetY, float alpha) const;

private:

    std::string text;

};

class Line
{

public:

    int x, y, w, h;

    Line(
        const std::vector<DrawCmd*>& drawBgCommands,
        const std::vector<DrawCmd*>& drawFgCommands);

    void drawBg(X11Canvas* canvas, float alpha) const;
    void drawFg(X11Canvas* canvas, float alpha) const;

private:

    std::vector<DrawCmd*> drawBgCommands;
    std::vector<DrawCmd*> drawFgCommands;

};

#endif // OVERLAY_GUI_H
