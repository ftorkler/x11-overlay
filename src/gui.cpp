#include "gui.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "ansi.h"
#include "canvas.h"
#include "color.h"
#include "pair.h"
#include "window.h"

Gui::Gui()
:
    orientation(Orientation::NW),
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

    setDefaultForgroundColor(Color(255, 255, 255, 200));
    setDefaultBackgroundColor(Color(0, 0, 0, 100));
    clearMessages();
}

Gui::~Gui()
{
    delete canvas;
    delete window;
}

void Gui::setDefaultForgroundColor(const Color& color)
{
    redraw = true;
    fgColor = color;
}

void Gui::setDefaultBackgroundColor(const Color& color)
{
    redraw = true;
    bgColor = color;
}

void Gui::setColorProfile(Ansi::Profile profile)
{
    redraw = true;
    colorProfile = profile;
}

void Gui::setDimming(const float& dimming)
{
    redraw = true;
    this->alpha = 1.0f - dimming;
}

void Gui::setMouseOverDimming(const float& dimming)
{
    redraw = true;
    this->mouseOverAlpha = 1.0f - dimming;
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

void Gui::setFont(const std::string& font)
{
    redraw = true;
    canvas->setFont(font);
}

void Gui::flush()
{
    int w = messageMaxWidth;
    int h = messageY;

    bool currentMouseOver = isMouseOver();
    redraw |= mouseOver != currentMouseOver;
    redraw |= window->updateActiveMonitor();
    mouseOver = currentMouseOver;

    if (!redraw && !recalc) {
        return;
    }
    recalc = false; // TODO recalculation of individual line positions (or simply reinsert all messages once again?)

    window->clear();

    if (w > 0 && h > 0) {
        window->resize(w, h);
        updateWindowPosition();

        int offsetX = calcXforOrientation(0, messageMaxWidth, 0);

        float a = mouseOver ? mouseOverAlpha : alpha;
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

    std::string trimmedMessage = trimForOrientation(trimLinefeedsAndApplyTabs(message));

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
            case Ansi::Sequence::RESET:
                increaseIntensity = false;
                drawFgCommands.emplace_back(new DrawColorCmd(fgColor));
                drawBgCommands.emplace_back(new DrawColorCmd(bgColor));
                break;
            case Ansi::Sequence::RESET_FOREGROUND:
                drawFgCommands.emplace_back(new DrawColorCmd(fgColor));
                break;
            case Ansi::Sequence::RESET_BACKGROUND:
                drawBgCommands.emplace_back(new DrawColorCmd(bgColor));
                break;
            case Ansi::Sequence::FOREGROUND_COLOR:
                lastFgColor = chunk;
                drawFgCommands.emplace_back(new DrawColorCmd(Ansi::toColor(chunk, increaseIntensity, colorProfile)));
                break;
            case Ansi::Sequence::BACKGROUND_COLOR:
                drawBgCommands.emplace_back(new DrawColorCmd(Ansi::toColor(chunk, false, colorProfile)));
                break;
            case Ansi::Sequence::INCREASE_INTENSITY:
                increaseIntensity = true;
                drawFgCommands.emplace_back(new DrawColorCmd(Ansi::toColor(lastFgColor, increaseIntensity, colorProfile)));
                break;
            case Ansi::Sequence::DECREASED_INTENSITY:
                increaseIntensity = false;
                drawFgCommands.emplace_back(new DrawColorCmd(Ansi::toColor(lastFgColor, increaseIntensity, colorProfile)));
                break;
            case Ansi::Sequence::NORMAL_INTENSITY:
                increaseIntensity = false;
                drawFgCommands.emplace_back(new DrawColorCmd(Ansi::toColor(lastFgColor, increaseIntensity, colorProfile)));
                break;
            case Ansi::Sequence::NONE:
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

const std::string Gui::trimLinefeedsAndApplyTabs(const std::string& text) const
{
    std::stringstream ss;
    int ss_size = 0;

    for (size_t i=0; i<text.length(); ++i) {
        const char c = text[i];
        switch (c) {
            case '\r':
            case '\n':
                break;
            case '\t':
                if (ss_size % 4 == 0) {
                    ss_size += 4;
                    ss << "    ";
                }
                while (ss_size % 4 != 0) {
                    ++ss_size;
                    ss << " ";
                }
                break;
            default:
                ss << c;
                ++ss_size;
        }
    }
    return ss.str();
}

const std::string Gui::trimForOrientation(const std::string& text) const
{
    // Original: "<L>TEXT<R>"  (<L/R> Indentation)
    // West:     "<L>TEXT<R>"
    // Center:   "TEXT"
    // East:     "<R>TEXT<L>"

    if (orientation == Orientation::NW ||
        orientation == Orientation::W ||
        orientation == Orientation::SW)
    {
        return text;
    }

    size_t start = text.find_first_not_of(" ");
    size_t end = text.find_last_not_of(" ");

    if (start == 0 && end == text.length()-1) {
        return text;
    }
    if (start == std::string::npos) {
        start = 0;
    }
    if (end == std::string::npos) {
        end = text.length()-1;
    }

    if (orientation == Orientation::N ||
        orientation == Orientation::CENTER ||
        orientation == Orientation::S)
    {
        return text.substr(start, end+1-start);
    }

    std::string left = text.substr(0, start);
    std::string middle = text.substr(start, end+1-start);
    std::string right = text.substr(end+1);
    return right + middle + left;
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
        case Orientation::NW:
        case Orientation::W:
        case Orientation::SW:
        default:
            return spacing;
        case Orientation::N:
        case Orientation::CENTER:
        case Orientation::S:
            return outerWidth / 2 - innerWidth / 2;
        case Orientation::NE:
        case Orientation::E:
        case Orientation::SE:
            return outerWidth - innerWidth - spacing;
    }
}

int Gui::calcYforOrientation(unsigned int innerHeight, unsigned outerHeight, unsigned int spacing) const
{
    switch (orientation) {
        case Orientation::NW:
        case Orientation::N:
        case Orientation::NE:
        default:
            return spacing;
        case Orientation::W:
        case Orientation::CENTER:
        case Orientation::E:
            return outerHeight / 2 - innerHeight / 2;
        case Orientation::SW:
        case Orientation::S:
        case Orientation::SE:
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
        case Gui::Orientation::N: return "N";
        case Gui::Orientation::NE: return "NE";
        case Gui::Orientation::E: return "E";
        case Gui::Orientation::SE: return "SE";
        case Gui::Orientation::S: return "S";
        case Gui::Orientation::SW: return "SW";
        case Gui::Orientation::W: return "W";
        case Gui::Orientation::NW: return "NW";
        case Gui::Orientation::CENTER: return "CENTER";
        default: return "";
    }
}

Gui::Orientation Gui::orientationFromString(const std::string& input)
{
    for (int orientation=Orientation::N; orientation<Orientation::NONE; ++orientation) {
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
