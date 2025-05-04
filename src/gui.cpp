#include "gui.h"

#include <algorithm>
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
    clearMessages();
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

void Gui::setFont(unsigned int fontIndex, const std::string& font)
{
    redraw = true;
    canvas->setFont(fontIndex, font);
}

void Gui::flush()
{
    layoutLines();

    bool currentMouseOver = isMouseOver();
    redraw |= mouseOver != currentMouseOver;
    redraw |= window->isActiveMonitorChanged() && updateWindowPosition();
    mouseOver = currentMouseOver;

    if (!redraw) {
        return;
    }

    window->clear();

    if (!lines.empty()) {
        canvas->selectFont(0);

        float a = mouseOver ? mouseOverAlpha : alpha;
        DrawColorCmd(bgColor).draw(canvas, a);
        for (auto line : lines) {
            line->drawBg(canvas, a);
        }
        DrawColorCmd(fgColor).draw(canvas, a);
        for (auto line : lines) {
            line->drawFg(canvas, a);
        }
    }

    window->flush();

    redraw = false;
}

void Gui::clearMessages()
{
    redraw = true;
    recalc = true;
    increaseIntensity = false;
    lastFgColor = "\e[37m";

    for (auto line : lines) {
        delete line;
    }
    lines.clear();
}

void Gui::addMessage(const std::string& message)
{
    redraw = true;
    recalc = true;
    std::vector<DrawCmd*> drawBgCommands;
    std::vector<DrawCmd*> drawFgCommands;

    std::string trimmedMessage = trimForOrientation(orientation, trimLinefeedsAndApplyTabs(message));
    unsigned int maxFontAscent = 0;

    for (auto chunk : Ansi::split(trimmedMessage)) {
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
        case Ansi::Sequence::DEFAULT_FONT:
        case Ansi::Sequence::ALTERNATIVE_FONT:
            drawFgCommands.emplace_back(new DrawSelectFontCmd(canvas->selectFont(Ansi::toFontIndex(chunk))));
            break;
        case Ansi::Sequence::NONE:
        {
            maxFontAscent = std::max(maxFontAscent, canvas->getSelectedFontAscent());
            Dimesion chunkDim = canvas->getStringDimension(chunk);
            drawBgCommands.emplace_back(new DrawRectCmd(chunkDim.w, chunkDim.h));
            drawFgCommands.emplace_back(new DrawTextCmd(chunkDim.w, chunkDim.h, chunk));
            break;
        }
        default:
            break;
        }
    }

    lines.emplace_back(new Line(
        maxFontAscent,
        drawBgCommands,
        drawFgCommands
    ));
}

void Gui::layoutLines()
{
    if (!recalc) { return; }

    unsigned int maxW = 0;
    unsigned int maxH = 0;
    for (auto line : lines) {
        maxW = std::max(maxW, line->w);
    }
    int cmdX;
    for (auto line : lines) {
        line->x = calcXforOrientation(line->w, maxW, 0);
        line->y = maxH;

        cmdX = line->x;
        for (auto bgCmd : line->drawBgCommands) {
            bgCmd->x = cmdX; cmdX += bgCmd->w;
            bgCmd->y = line->y;
            bgCmd->h = line->h;
        }
        cmdX = line->x;
        for (auto fgCmd : line->drawFgCommands) {
            fgCmd->x = cmdX; cmdX += fgCmd->w;
            fgCmd->y = line->y + line->baseline;
        }

        maxH += line->h + lineSpacing;
    }

    window->resize(maxW, maxH);
    updateWindowPosition();

    recalc = false;
}

const std::string Gui::trimLinefeedsAndApplyTabs(const std::string& text)
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

const std::string Gui::trimForOrientation(const Orientation orientation, const std::string& text)
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
    int w = window->getWidth();
    int h = window->getHeight();
    int t = mouseOverTolerance;

    Position pos = window->getMousePosition();
    bool isInFrame =
        pos.x + t >= 0 &&
        pos.y + t >= 0 &&
        pos.x - t < w &&
        pos.y - t < h;

    if (isInFrame) {
        for (auto line : lines) {
            if (pos.x + t >= line->x &&
                pos.y + t >= line->y &&
                pos.x - t <= line->x + (int)line->w &&
                pos.y - t <= line->y + (int)line->h)
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

bool Gui::updateWindowPosition() const
{
    window->move(
        calcXforOrientation(window->getWidth(), window->getMonitorWidth(), screenEdgeSpacing),
        calcYforOrientation(window->getHeight(), window->getMonitorHeight(), screenEdgeSpacing));
    return true;
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
    DrawCmd(),
    color(color)
{
}

 void DrawColorCmd::draw(X11Canvas* canvas, float alpha) const
 {
     canvas->setColor(Color(color.r, color.g, color.b, color.a * alpha));
 }

DrawRectCmd::DrawRectCmd(unsigned int w, unsigned int h)
:
    DrawCmd(w, h)
{
}

void DrawRectCmd::draw(X11Canvas* canvas, float alpha) const
{
    canvas->drawRect(x, y, w, h);
}

DrawTextCmd::DrawTextCmd(unsigned int w, unsigned int h, const std::string& text)
:
    DrawCmd(w, h),
    text(text)
{
}

void DrawTextCmd::draw(X11Canvas* canvas, float alpha) const
{
    canvas->drawString(x, y, text);
}

DrawSelectFontCmd::DrawSelectFontCmd(unsigned int fontIndex)
:
    DrawCmd(),
    fontIndex(fontIndex)
{
}

void DrawSelectFontCmd::draw(X11Canvas* canvas, float alpha) const
{
    canvas->selectFont(fontIndex);
}

Line::Line(
    unsigned int baseline,
    const std::vector<DrawCmd*>& drawBgCommands,
    const std::vector<DrawCmd*>& drawFgCommands)
:
    x(0),
    y(0),
    w(0),
    h(0),
    baseline(baseline),
    drawBgCommands(drawBgCommands),
    drawFgCommands(drawFgCommands)
{
    for (auto drawCmd : drawFgCommands) {
        w += drawCmd->w;
        h = std::max(h, drawCmd->h);
    }
}

void Line::drawBg(X11Canvas* canvas, float alpha) const
{
    for (auto cmd : drawBgCommands) {
        cmd->draw(canvas, alpha);
    }
}

void Line::drawFg(X11Canvas* canvas, float alpha) const
{
    for (auto cmd : drawFgCommands) {
        cmd->draw(canvas, alpha);
    }
}
