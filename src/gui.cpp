#include "gui.h"

#include <algorithm>
#include <iostream>

#include "ansi.h"
#include "canvas.h"
#include "window.h"

Gui::Gui()
:
    orientation(NW),
    messageY(0), 
    messageMaxWidth(0),
    mouseOver(false),
    redraw(true),
    recalc(true),
    increaseIntensity(false),
    colorProfile(Ansi::Profile::VGA),
    screenEdgeSpacing(0),
    lineSpacing(0),
    mouseOverTolerance(0),
    alpha(0.25)
{
    window = new X11Window(0, 0, 480, 640);
    canvas = window->createCanvas();

    setDefaultBackgroundColor(0, 0, 0, 100);
    setDefaultForgroundColor(255, 255, 255, 200);
    clearMessages();
}

Gui::~Gui()
{
    delete canvas;
    delete window;
}

void Gui::setDefaultForgroundColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    redraw = true;
    fgColor = Color(r, g, b, a);
}

void Gui::setDefaultBackgroundColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    redraw = true;
    bgColor = Color(r, g, b, a);
}

void Gui::setColorProfile(Ansi::Profile profile)
{
    colorProfile = profile;
}

void Gui::setMouseOverDimming(const float& dimming)
{
    redraw = true;
    this->alpha = 1.0f - dimming;
}

void Gui::setMouseOverTolerance(unsigned int tolerance)
{
    redraw = true;
    mouseOverTolerance = tolerance;
}

void Gui::setOrientation(Orientation orientation) 
{
    redraw = true;
    recalc = true;
    this->orientation = orientation;
}

void Gui::setScreenEdgeSpacing(unsigned int spacing)
{
    redraw = true;
    screenEdgeSpacing = spacing;
}

void Gui::setLineSpacing(unsigned int spacing)
{
    redraw = true;
    recalc = true;
    lineSpacing = spacing;
}

void Gui::setMonitorIndex(unsigned int index)
{
    redraw = true;
    recalc = true;
    window->setActiveMonitor(index);
}

void Gui::flush()
{
    int w = messageMaxWidth;
    int h = messageY;

    if (recalc) {
        // TODO recalculation of individual line positions (or simply reinsert all messages once again?)
        recalc = false;
    }

    bool currentMouseOver = isMouseOver();
    redraw |= mouseOver != currentMouseOver;
    mouseOver = currentMouseOver;

    if (!redraw) {
        return;
    }

    window->clear();

    if (w > 0 && h > 0) {
        window->resize(w, h);
        updateWindowPosition();

        int offsetX = calcXforOrientation(0, messageMaxWidth, 0);

        float a = mouseOver ? alpha : 1.0f;
        DrawColorCmd(bgColor).draw(canvas, offsetX, a);
        for (auto cmd : drawBgCommands) {
            cmd->draw(canvas, offsetX, a);
        }
        DrawColorCmd(fgColor).draw(canvas, offsetX, a);
        for (auto cmd : drawFgCommands) {
            cmd->draw(canvas, offsetX, a);
        }
    }

    window->flush();

    redraw = false;
}

void Gui::clearMessages() 
{
    redraw = true;
    increaseIntensity = false;
    lastFgColor = "\e[37m";

    messageMaxWidth = 0;
    messageY = 0;

    drawBgCommands.clear();
    drawFgCommands.clear();
    clippingBoxes.clear();
}

void Gui::addMessage(const std::string& message) 
{
    redraw = true;

    std::string trimmedMessage = trimForOrientation(message);

    Dimesion dim = canvas->getStringDimension(trimmedMessage);

    if (!trimmedMessage.empty()) {

        std::vector<std::string> chunks = Ansi::split(trimmedMessage);
        
        int w = 0;
        for (auto chunk : chunks) {
            w += Ansi::parseControlSequence(chunk) == Ansi::Sequence::NONE ? canvas->getStringDimension(chunk).w : 0;
        }

        int x = calcXforOrientation(w, 0, 0);
        clippingBoxes.emplace_back(ClippingAABB(x, messageY, w, dim.h));

        for (auto chunk : chunks) {
            Ansi::Sequence sequence = Ansi::parseControlSequence(chunk);
            switch (sequence)
            {
            case Ansi::RESET:
                increaseIntensity = false;
                drawFgCommands.emplace_back(new DrawColorCmd(fgColor));
                drawBgCommands.emplace_back(new DrawColorCmd(bgColor));
                break;
            case Ansi::RESET_FOREGROUND:
                drawFgCommands.emplace_back(new DrawColorCmd(fgColor));
                break;
            case Ansi::RESET_BACKGROUND:
                drawBgCommands.emplace_back(new DrawColorCmd(bgColor));
                break;
            case Ansi::FOREGROUND_COLOR:
                lastFgColor = chunk;
                drawFgCommands.emplace_back(new DrawColorCmd(Ansi::toColor(chunk, increaseIntensity, colorProfile)));
                break;
            case Ansi::BACKGROUND_COLOR:
                drawBgCommands.emplace_back(new DrawColorCmd(Ansi::toColor(chunk, false, colorProfile)));
                break;
            case Ansi::INCREASE_INTENSITY:
                increaseIntensity = true;
                drawFgCommands.emplace_back(new DrawColorCmd(Ansi::toColor(lastFgColor, increaseIntensity, colorProfile)));
                break;
            case Ansi::DECREASED_INTENSITY:
                increaseIntensity = false;
                drawFgCommands.emplace_back(new DrawColorCmd(Ansi::toColor(lastFgColor, increaseIntensity, colorProfile)));
                break;
            case Ansi::NORMAL_INTENSITY:
                increaseIntensity = false;
                drawFgCommands.emplace_back(new DrawColorCmd(Ansi::toColor(lastFgColor, increaseIntensity, colorProfile)));
                break;
            case Ansi::NONE:
            {
                int chunkW = canvas->getStringDimension(chunk).w;
                drawBgCommands.emplace_back(new DrawRectCmd(x, messageY, chunkW, dim.h));
                drawFgCommands.emplace_back(new DrawTextCmd(x, messageY, chunk));
                x += chunkW;
                break;
            }
            default:
                break;
            }
        }

        if (w > messageMaxWidth) {
            messageMaxWidth = w;
        }
    }

    messageY += dim.h + lineSpacing;
}

std::string Gui::trimForOrientation(const std::string& text) const
{
    if (orientation != NW && orientation != W && orientation != SW) {
        size_t start = text.find_first_not_of(" ");
        std::string mutableText = (start == std::string::npos) ? "" : text.substr(start);

        if (orientation == NE || orientation == E || orientation == SE) {
            for (int i=0; i<(int)start; ++i) {
                mutableText.append(" ");
            }
        }
        return mutableText;
    }
    return text;
}

bool Gui::isMouseOver() const
{
    int w = messageMaxWidth;
    int h = messageY;
    int t = mouseOverTolerance;

    Position pos = window->getMousePosition();
    bool isInFrame = 
        pos.x + t >= 0 && 
        pos.y + t >= 0 && 
        pos.x - t < w && 
        pos.y - t < h;
        
    if (isInFrame) {
        int offsetX = calcXforOrientation(0, messageMaxWidth, 0);
        for (auto box : clippingBoxes) {
            if (pos.x + t >= box.x + offsetX && 
                pos.y + t >= box.y && 
                pos.x - t <= box.x + offsetX + (int)box.w && 
                pos.y - t <= box.y + (int)box.h)
            {
                return true;
            }
        }
    }
    return false;
}

int Gui::calcXforOrientation(unsigned int innerWidth, unsigned int outerWidth, unsigned int spacing) const
{
    switch (orientation) {
        case NW:
        case W:
        case SW:
        default:
            return spacing;
        case N:
        case CENTER:
        case S:
            return outerWidth / 2 - innerWidth / 2;
        case NE:
        case E:
        case SE:
            return outerWidth - innerWidth - spacing;
    }
}

int Gui::calcYforOrientation(unsigned int innerHeight, unsigned outerHeight, unsigned int spacing) const
{
    switch (orientation) {
        case NW:
        case N:
        case NE:
        default:
            return spacing;
        case W:
        case CENTER:
        case E:
            return outerHeight / 2 - innerHeight / 2;
        case SW:
        case S:
        case SE:
            return outerHeight - innerHeight - spacing;
    }
}

void Gui::updateWindowPosition() const
{
    window->move(
        calcXforOrientation(window->getWidth(), window->getMonitorWidth(), screenEdgeSpacing),
        calcYforOrientation(window->getHeight(), window->getMonitorHeight(), screenEdgeSpacing));
}

std::string Gui::orientationToString(Orientation orientation)
{
    switch (orientation)
    {
        case N: return "N";
        case NE: return "NE";
        case E: return "E";
        case SE: return "SE";
        case S: return "S";
        case SW: return "SW";
        case W: return "W";
        case NW: return "NW";
        case CENTER: return "CENTER";
        default: return "";
    }
}

Gui::Orientation Gui::orientationFromString(const std::string& input)
{
    for (int orientation=N; orientation<NONE; ++orientation) {
        if (input == orientationToString(static_cast<Orientation>(orientation))) {
            return static_cast<Orientation>(orientation);
        }
    }
    return Orientation::NONE;
}



DrawColorCmd::DrawColorCmd(const Color& color)
:
    color(color)
{
}

 void DrawColorCmd::draw(X11Canvas* canvas, int offsetX, float alpha) const
 {
     canvas->setColor(Color(color.r, color.g, color.b, color.a * alpha));
 }

DrawRectCmd::DrawRectCmd(int x, int y, unsigned int w, unsigned int h)
:
    DrawCmd(),
    x(x),
    y(y),
    w(w),
    h(h)
{
}

void DrawRectCmd::draw(X11Canvas* canvas, int offsetX, float alpha) const
{
    canvas->drawRect(x + offsetX, y, w, h);
}

DrawTextCmd::DrawTextCmd(int x, int y, const std::string& text)
:
    DrawCmd(),
    x(x),
    y(y),
    text(text)
{
}

void DrawTextCmd::draw(X11Canvas* canvas, int offsetX, float alpha) const
{
    canvas->drawString(x + offsetX, y, text);
}
