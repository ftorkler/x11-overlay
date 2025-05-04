#ifndef OVERLAY_GUI_H
#define OVERLAY_GUI_H

#include <string>
#include <X11/Xlib.h>
#include <vector>
#include <map>

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
    void setFont(unsigned int fontIndex, const std::string& font);

    void flush();
    void clearMessages();
    void addMessage(const std::string& message);

    static std::string orientationToString(Orientation orientation);
    static Orientation orientationFromString(const std::string& input);

private:

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

    int x, y;
    unsigned int w, h;

    DrawCmd(const unsigned int& width = 0, const unsigned int& height = 0): x(0), y(0), w(width), h(height) { }
    virtual ~DrawCmd() { }
    virtual void draw(X11Canvas* canvas, float alpha) const = 0;

};

class DrawColorCmd : public DrawCmd
{

public:

    DrawColorCmd(const Color& color);

    virtual void draw(X11Canvas* canvas, float alpha) const;

private:

    Color color;

};

class DrawRectCmd : public DrawCmd
{

public:

    DrawRectCmd(unsigned int w, unsigned int h);

    virtual void draw(X11Canvas* canvas, float alpha) const;

};

class DrawTextCmd : public DrawCmd
{

public:

    DrawTextCmd(unsigned int w, unsigned int h, const std::string& text);

    virtual void draw(X11Canvas* canvas, float alpha) const;

private:

    std::string text;

};

class DrawSelectFontCmd : public DrawCmd
{

public:

    DrawSelectFontCmd(unsigned int fontIndex);

    virtual void draw(X11Canvas* canvas, float alpha) const;

private:

    unsigned int fontIndex;

};

class Line
{

public:

    int x, y;
    unsigned int w, h;
    unsigned int baseline;
    const std::vector<DrawCmd*> drawBgCommands;
    const std::vector<DrawCmd*> drawFgCommands;

    Line(
        unsigned int baseline,
        const std::vector<DrawCmd*>& drawBgCommands,
        const std::vector<DrawCmd*>& drawFgCommands);

    void drawBg(X11Canvas* canvas, float alpha) const;
    void drawFg(X11Canvas* canvas, float alpha) const;

};

#endif // OVERLAY_GUI_H
