#include "config.h"
#include "ansi.h"

#include <getopt.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>

static constexpr const char* LONG_OPT_FG_COLOR = "fg-color";
static constexpr const int SHORT_OPT_FG_COLOR = 1000;
static constexpr const char* LONG_OPT_FG_ALPHA = "fg-alpha";
static constexpr const int SHORT_OPT_FG_ALPHA = 1001;
static constexpr const char* LONG_OPT_BG_COLOR = "bg-color";
static constexpr const int SHORT_OPT_BG_COLOR = 1002;
static constexpr const char* LONG_OPT_BG_ALPHA = "bg-alpha";
static constexpr const int SHORT_OPT_BG_ALPHA = 1003;

static constexpr const char* SECTION_NONE = "";
static constexpr const char* SECTION_NONE_INPUT_FILE = "InputFile";
static constexpr const char* SECTION_POSITIONING = "Positioning";
static constexpr const char* SECTION_POSITIONING_MONITOR_INDEX = "MonitorIndex";
static constexpr const char* SECTION_POSITIONING_ORIENTATION = "Orientation";
static constexpr const char* SECTION_POSITIONING_SCREEN_EDGE_SPACING = "ScreenEdgeSpacing";
static constexpr const char* SECTION_POSITIONING_LINE_SPACING = "LineSpacing";
static constexpr const char* SECTION_FONT = "Font";
static constexpr const char* SECTION_FONT_NAME = "Name";
static constexpr const char* SECTION_FONT_SIZE = "Size";
static constexpr const char* SECTION_COLORS = "Colors";
static constexpr const char* SECTION_COLORS_ANSI_PROFILE = "AnsiProfile";
static constexpr const char* SECTION_COLORS_FG_COLOR = "ForegroundColor";
static constexpr const char* SECTION_COLORS_FG_ALPHA = "ForegroundAlpha";
static constexpr const char* SECTION_COLORS_BG_COLOR = "BackgroundColor";
static constexpr const char* SECTION_COLORS_BG_ALPHA = "BackgroundAlpha";
static constexpr const char* SECTION_MOUSE_OVER = "MouseOver";
static constexpr const char* SECTION_MOUSE_OVER_TOLERANCE = "Tolerance";
static constexpr const char* SECTION_MOUSE_OVER_DIMMING = "Dimming";
static constexpr const char* SECTION_INTERVAL = "Intervals";


Config::Config()
:
    configFile(""),
    verbose(false),
    inputFile(""),
    // positioning
    monitorIndex(INT_MIN),
    orientation(Gui::Orientation::NONE),
    screenEdgeSpacing(INT_MIN),
    lineSpacing(INT_MIN),
    // font
    fontName(""),
    fontSize(0),
    // colors
    colorProfile(Ansi::Profile::VGA),
    tempDefaultForegroundColor(""),
    tempDefaultBackgroundColor(""),
    defaultForegroundColor(Color(-1,-1,-1,-1)),
    defaultBackgroundColor(Color(-1,-1,-1,-1)),
    // mouse over
    mouseOverTolerance(INT_MIN),
    mouseOverDimming(INT_MIN)
{
}

Config::~Config()
{
}

Config Config::overrideWith(const Config& other)
{
    Config unsetConfig;
    if (other.configFile != unsetConfig.configFile) this->configFile = other.configFile;
    if (other.verbose != unsetConfig.verbose) this->verbose = other.verbose;
    if (other.inputFile != unsetConfig.inputFile) this->inputFile = other.inputFile;
    // positioning
    if (other.monitorIndex != unsetConfig.monitorIndex) this->monitorIndex = other.monitorIndex;
    if (other.orientation != unsetConfig.orientation) this->orientation = other.orientation;
    if (other.screenEdgeSpacing != unsetConfig.screenEdgeSpacing) this->screenEdgeSpacing = other.screenEdgeSpacing;
    if (other.lineSpacing != unsetConfig.lineSpacing) this->lineSpacing = other.lineSpacing;
    // font
    if (other.fontName != unsetConfig.fontName) this->fontName = other.fontName;
    if (other.fontSize != unsetConfig.fontSize) this->fontSize = other.fontSize;
    // colors
    if (other.colorProfile != unsetConfig.colorProfile) this->colorProfile = other.colorProfile;
    if (other.tempDefaultForegroundColor != unsetConfig.tempDefaultForegroundColor) this->tempDefaultForegroundColor = other.tempDefaultForegroundColor;
    if (other.tempDefaultBackgroundColor != unsetConfig.tempDefaultBackgroundColor) this->tempDefaultBackgroundColor = other.tempDefaultBackgroundColor;
    if (other.defaultForegroundColor != unsetConfig.defaultForegroundColor) this->defaultForegroundColor = other.defaultForegroundColor;
    if (other.defaultBackgroundColor != unsetConfig.defaultBackgroundColor) this->defaultBackgroundColor = other.defaultBackgroundColor;
    // mouse over
    if (other.mouseOverDimming != unsetConfig.mouseOverDimming) this->mouseOverDimming = other.mouseOverDimming;
    if (other.mouseOverTolerance != unsetConfig.mouseOverTolerance) this->mouseOverTolerance = other.mouseOverTolerance;

    finalizeColors();

    return *this;
}

void Config::print(std::ostream& os) const
{
    os << "default config file path is '" << getDefaultConfigFilePath() << "'" << std::endl;
    os << "---- resulting config ----" << std::endl;
    printSectionKeyValue(os, SECTION_NONE_INPUT_FILE, inputFile);
    os << std::endl;

    printSection(os, SECTION_POSITIONING);
    printSectionKeyValue(os, SECTION_POSITIONING_MONITOR_INDEX, monitorIndex);
    printSectionKeyValue(os, SECTION_POSITIONING_ORIENTATION, Gui::orientationToString(orientation));
    printSectionKeyValue(os, SECTION_POSITIONING_SCREEN_EDGE_SPACING, screenEdgeSpacing);
    printSectionKeyValue(os, SECTION_POSITIONING_LINE_SPACING, lineSpacing);
    os << std::endl;

    printSection(os, SECTION_FONT);
    printSectionKeyValue(os, SECTION_FONT_NAME, fontName);
    printSectionKeyValue(os, SECTION_FONT_SIZE, fontSize);
    os << std::endl;

    printSection(os, SECTION_COLORS);
    printSectionKeyValue(os, SECTION_COLORS_ANSI_PROFILE, Ansi::profileToString(colorProfile));
    printSectionKeyValueAnsiColor(os, SECTION_COLORS_FG_COLOR, Ansi::Sequence::FOREGROUND_COLOR, defaultForegroundColor);
    printSectionKeyValue(os, SECTION_COLORS_FG_ALPHA, (int)defaultForegroundColor.a);
    printSectionKeyValueAnsiColor(os, SECTION_COLORS_BG_COLOR, Ansi::Sequence::BACKGROUND_COLOR, defaultBackgroundColor);
    printSectionKeyValue(os, SECTION_COLORS_BG_ALPHA, (int)defaultBackgroundColor.a);
    
    os << std::endl;

    printSection(os, SECTION_MOUSE_OVER);
    printSectionKeyValue(os, SECTION_MOUSE_OVER_TOLERANCE, mouseOverTolerance);
    printSectionKeyValue(os, SECTION_MOUSE_OVER_DIMMING, mouseOverDimming);

    os << "--------------------------" << std::endl;
}

void Config::printSection(std::ostream& os, const std::string& section)
{
    os << "[" << section << "]" << std::endl;
}

void Config::exitWithUsage(int exitCode)
{
    std::cout << "usage: overlay [OPTIONS] <INPUT_FILE>" << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  -c <file>           file path to read configuration from" << std::endl;
    std::cout << "  -d <percent>        how much the window dims on mouse over; defaults to '75'%" << std::endl;
    std::cout << "      --fg-color      default foreground color; defaults to '[97m' (same as '[38;2;255;255;255m')" << std::endl;
    std::cout << "      --fg-alpha      default foreground alpha; defaults to '200'" << std::endl;
    std::cout << "      --bg-color      default background color; defaults to '[40m' (same as '[48;2;0;0;0')" << std::endl;
    std::cout << "      --bg-alpha      default background alpha; defaults to '100'" << std::endl;
    std::cout << "  -e <pixel>          screen edge spacing in pixels; defaults to '0'" << std::endl;
    std::cout << "  -f <name>           font name; defaults to 'NotoSansMono'" << std::endl;
    std::cout << "  -h                  prints this help text" << std::endl;
    std::cout << "  -l <pixel>          line spacing in pixels; defaults to '0'" << std::endl;
    std::cout << "  -m <index>          monitor to use; defaults to '0'" << std::endl;
    std::cout << "  -o <value>          orientation to align window and lines; defaults to 'NW'" << std::endl;
    std::cout << "                      possible values are N, NE, E, SE, S, SW, W, NW and CENTER" << std::endl;
    std::cout << "  -p <value>          profile for ansi colors; values are VGA or XP" << std::endl;
    std::cout << "  -s <size>           font size; defaults to '12'" << std::endl;
    std::cout << "  -t <pixel>          tolerance in pixel for mouse over dimming; defaults to '0'" << std::endl;
    std::cout << "  -v                  be verbose and print some debug output" << std::endl;
    exit(exitCode);
}

Config Config::defaultConfig()
{
    Config config;
    config.configFile = "",
    config.verbose = false,
    config.inputFile = "",
    // positioning
    config.monitorIndex = 0;
    config.orientation = Gui::Orientation::NW;
    config.screenEdgeSpacing = 0;
    config.lineSpacing = 0;
    // font
    config.fontName = "NotoSansMono";
    config.fontSize = 12;
    // colors
    config.colorProfile = Ansi::Profile::VGA;
    config.tempDefaultForegroundColor = "";
    config.tempDefaultBackgroundColor = "";
    config.defaultForegroundColor = Color(255, 255, 255, 255);
    config.defaultBackgroundColor = Color(0, 0, 0, 100);
    // mouse over behavior
    config.mouseOverDimming = 75;
    config.mouseOverTolerance = 0;
    return config;
}

Ansi::Profile Config::assertProfileParameter(const std::string& param)
{
    Ansi::Profile profile = Ansi::profileFromString(param);
    if (profile == Ansi::Profile::NONE) {
        throw std::runtime_error("must be [VGA,XP]");
    }
    return profile;
}

Gui::Orientation Config::assertOrientationParameter(const std::string& param)
{
    Gui::Orientation orientation = Gui::orientationFromString(param);
    if (orientation == Gui::Orientation::NONE) {
        throw std::runtime_error("must be [N,NE,E,SE,S,SW,W,NW,CENTER]");
    }
    return orientation;
}

int Config::assertIntParameter(const std::string& param, int min, int max)
{
    char* end;
    int value = strtol(param.c_str(), &end, 10);
    bool failedParsing = *end != '\0';
    if (failedParsing) {
        bool isNextParameter = !param.empty() && param[0] == '-';
        if (isNextParameter) {
            throw std::runtime_error("must have a value");
        }
        throw std::runtime_error("must be an integer value");
    }
    if ((min != -1 && value < min) || (max != -1 && value > max)) {
        std::string lower_bound = min != -1 ? std::to_string(min) : "";
        std::string upper_bound = max != -1 ? std::to_string(max) : "";
        std::stringstream errorMsg;
        errorMsg << "must be in the range [" << lower_bound << ".." << upper_bound << "]";
        throw std::runtime_error(errorMsg.str());
    }
    return value;
}

Color Config::assertAnsiColorParameter(const std::string& param, Ansi::Profile colorProfile) 
{
    std::string colorSequence;
    if (param.find(ANSI_START) != 0) {
        colorSequence = ANSI_START + param;
    } else {
        colorSequence = param;
    }

    Color fallbackColor = Color(0,0,0,0);
    Color color;
    Ansi::Sequence seq = Ansi::parseControlSequence(colorSequence);
    switch (seq)
    {
    case Ansi::Sequence::FOREGROUND_COLOR:
    case Ansi::Sequence::BACKGROUND_COLOR:
        color = Ansi::toColor(colorSequence, fallbackColor, false, colorProfile);
        if (color == fallbackColor) {
            throw std::runtime_error("is an invalid ansi color sequence");
        }
        return color;
    case Ansi::Sequence::NONE:
        throw std::runtime_error("is neither an ansi sequence for fore- nor background color");
    default:
        throw std::runtime_error("is an ansi sequence, but must be one for fore- or background color");
    }
}

std::string Config::lookupLongOptionName(const struct option* longOptions, int shortOption)
{
    if (shortOption > 128) {
        const option* opt = longOptions;
        while (opt->val != 0 && opt++) {
            if (opt->val == shortOption) {
                return opt->name;
            }
        }
    }

    return std::string(1, shortOption);
}

Config Config::fromParameters(int argc, char** argv)
{
    Config config;

    const char* const short_opts = "-c:d:e:f:hl:m:o:p:s:t:v";
    const option long_opts[] = {
            {LONG_OPT_FG_COLOR, required_argument, nullptr, SHORT_OPT_FG_COLOR},
            {LONG_OPT_FG_ALPHA, required_argument, nullptr, SHORT_OPT_FG_ALPHA},
            {LONG_OPT_BG_COLOR, required_argument, nullptr, SHORT_OPT_BG_COLOR},
            {LONG_OPT_BG_ALPHA, required_argument, nullptr, SHORT_OPT_BG_ALPHA},
            {nullptr, no_argument, nullptr, 0}
    };

    int opt;
    try 
    {
        while((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1)
        {
            switch(opt)
            {
                case 'c':
                    config.configFile = optarg;
                    break;
                case 'd':
                    config.mouseOverDimming = assertIntParameter(optarg, 0, 100);
                    break;
                case 'e':
                    config.screenEdgeSpacing = assertIntParameter(optarg, -1, -1);
                    break;
                case 'f':
                    config.fontName = optarg;
                    break;
                case 'h':
                    exitWithUsage(0);
                case 'l':
                    config.lineSpacing = assertIntParameter(optarg, 0, -1);
                    break;
                case 'm':
                    config.monitorIndex = assertIntParameter(optarg, 0, -1);
                    break;
                case 'o':
                    config.orientation = assertOrientationParameter(optarg);
                    break;
                case 'p':
                    config.colorProfile = assertProfileParameter(optarg);
                    break;
                case 's':
                    config.fontSize = assertIntParameter(optarg, 1, -1);
                    break;
                case 't':
                    config.mouseOverTolerance = assertIntParameter(optarg, 0, -1);
                    break;
                case SHORT_OPT_FG_COLOR:
                    assertAnsiColorParameter(optarg, config.colorProfile);
                    config.tempDefaultForegroundColor = optarg;
                    break;
                case SHORT_OPT_FG_ALPHA:
                    config.defaultForegroundColor.a = assertIntParameter(optarg, 0, 255);
                    break;
                case SHORT_OPT_BG_COLOR:
                    assertAnsiColorParameter(optarg, config.colorProfile);
                    config.tempDefaultBackgroundColor = optarg;
                    break;
                case SHORT_OPT_BG_ALPHA:
                    config.defaultBackgroundColor.a = assertIntParameter(optarg, 0, 255);
                    break;
                case 'v':
                    config.verbose = true;
                    break;
                case 1:
                    config.inputFile = optarg;
                    break;
                case ':':
                    throw std::runtime_error("needs a value");
                case '?':
                    throw std::runtime_error("is unknown");
            }
        }
    }
    catch(std::runtime_error const& e)
    {
        std::string option = lookupLongOptionName(long_opts, opt);
        std::cout << "ERROR: option '" << option << "' " << e.what() << ", but was '" << optarg << "' " << std::endl;
        exit(1);
    }

    config.finalizeColors();

    return config;
}

Config Config::fromFile(const std::string& filename, bool suppressWarning)
{
    Config config;

    std::ifstream filein = std::ifstream(filename);
    std::string section;
    int i = 0;
    for (std::string line; std::getline(filein, line); ++i) {
        try {
            if (!parseEmptyLine(line) &&
                !parseCommentLine(line) &&
                !parseSectionLine(line, section) &&
                !parseKeyValueLine(line, section, config))
            {
                throw std::runtime_error("not a comment, section nor a known key-value pair");
            }
        }
        catch (std::runtime_error const& e) {
            std::cout << "WARN: skipped line:" << i << "\t '" << line << "' - " << e.what() << "" << std::endl;
        }
    }
    filein.close();

    if (i == 0 && !suppressWarning) {
        std::cout << "WARN: no (valid) config file found at '" << filename << "'" << std::endl;
    }

    config.finalizeColors();

    return config;
}

bool Config::parseEmptyLine(std::string line)
{
    trim(line);
    return line.empty();
}

bool Config::parseCommentLine(std::string line)
{
    trim(line);
    return !line.empty() && (line[0] == ';' || line[0] == '#');
}

bool Config::parseSectionLine(std::string line, std::string& section)
{
    trim(line);
    if (line.size() <= 3 || line[0] != '[' || line[line.size()-1] != ']') {
        return false;
    }
    section.clear();
    section.append(line.substr(1, line.size()-2));
    trim(section);
    return true;
}

bool Config::parseKeyValueLine(std::string line, std::string section, Config& config)
{
    std::istringstream iss(line);
    std::string key;
    std::string value;
    if (std::getline(iss, key, '=') && std::getline(iss, value)) {
        trim(key);
        trim(value);

        if (key.empty() || value.empty()) {
            return false;
        }

        if (section == SECTION_NONE)
        {
            if (key == SECTION_NONE_INPUT_FILE) {
                config.inputFile = value;
                return true;
            }
        }
        if (section == SECTION_POSITIONING) 
        {
            if (key == SECTION_POSITIONING_MONITOR_INDEX) {
                config.monitorIndex = assertIntParameter(value, 0, -1);
                return true;
            }
            if (key == SECTION_POSITIONING_ORIENTATION) {
                config.orientation = assertOrientationParameter(value);
                return true;
            }
            if (key == SECTION_POSITIONING_SCREEN_EDGE_SPACING) {
                config.screenEdgeSpacing = assertIntParameter(value, -1, -1);
                return true;
            }
            if (key == SECTION_POSITIONING_LINE_SPACING) {
                config.lineSpacing = assertIntParameter(value, 0, -1);
                return true;
            }
        }
        if (section == SECTION_FONT) 
        {
            if (key == SECTION_FONT_NAME) {
                config.fontName = value;
                return true;
            }
            if (key == SECTION_FONT_SIZE) {
                config.fontSize = assertIntParameter(value, 1, -1);
                return true;
            }
        }
        if (section == SECTION_COLORS) 
        {
            if (key == SECTION_COLORS_ANSI_PROFILE) {
                config.colorProfile = assertProfileParameter(value);
                return true;
            }
            if (key == SECTION_COLORS_FG_COLOR) {
                assertAnsiColorParameter(value, config.colorProfile);
                config.tempDefaultForegroundColor = value;
                return true;
            }
            if (key == SECTION_COLORS_FG_ALPHA) {
                config.defaultForegroundColor.a = assertIntParameter(value, 0, 255);
                return true;
            }
            if (key == SECTION_COLORS_BG_COLOR) {
                assertAnsiColorParameter(value, config.colorProfile);
                config.tempDefaultBackgroundColor = value;
                return true;
            }
            if (key == SECTION_COLORS_BG_ALPHA) {
                config.defaultBackgroundColor.a = assertIntParameter(value, 0, 255);
                return true;
            }
        }
        if (section == SECTION_MOUSE_OVER) 
        {
            if (key == SECTION_MOUSE_OVER_TOLERANCE) {
                config.mouseOverTolerance = assertIntParameter(value, 0, -1);
                return true;
            }
            if (key == SECTION_MOUSE_OVER_DIMMING) {
                config.mouseOverDimming = assertIntParameter(value, 0, 100);
                return true;
            }
        }
    }

    return false;
}

std::string& Config::trim(std::string& str)
{
    const std::string CHARS_TO_TRIM = " \n\r\t";
    str.erase(0, str.find_first_not_of(CHARS_TO_TRIM));
    str.erase(str.find_last_not_of(CHARS_TO_TRIM) + 1);
    return str;
}

std::string Config::getDefaultConfigFilePath()
{
    const char* home;
    if ((home = getenv("HOME")) != nullptr ||
        (home = getpwuid(getuid())->pw_dir) != nullptr) 
    {
        return std::string(home) + "/.config/x11-overlayrc";
    }
    return "";
}

void Config::finalizeColors()
{
    if (!tempDefaultForegroundColor.empty()) {
        Color color = assertAnsiColorParameter(tempDefaultForegroundColor, colorProfile);
        defaultForegroundColor.r = color.r;
        defaultForegroundColor.g = color.g;
        defaultForegroundColor.b = color.b;
    }
    if (!tempDefaultBackgroundColor.empty()) {
        Color color = assertAnsiColorParameter(tempDefaultBackgroundColor, colorProfile);
        defaultBackgroundColor.r = color.r;
        defaultBackgroundColor.g = color.g;
        defaultBackgroundColor.b = color.b;
    }
}