#include "config.h"

#include <bits/getopt_core.h>
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <fstream>
#include <limits.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <string.h>
#include <pwd.h>

#include "ansi.h"
#include "color.h"
#include "gui.h"

static constexpr const int SHORT_OPT_FG_COLOR = 1000;
static constexpr const int SHORT_OPT_FG_ALPHA = 1001;
static constexpr const int SHORT_OPT_BG_COLOR = 1002;
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
static constexpr const char* SECTION_BEHAVIOR = "Behavior";
static constexpr const char* SECTION_BEHAVIOR_TOLERANCE = "Tolerance";
static constexpr const char* SECTION_BEHAVIOR_DIMMING = "Dimming";
static constexpr const char* SECTION_BEHAVIOR_MOUSE_OVER_DIMMING = "MouseOverDimming";


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
    // colors
    colorProfile(Ansi::Profile::VGA),
    tempDefaultForegroundColor(""),
    tempDefaultBackgroundColor(""),
    defaultForegroundColor(Color(-1,-1,-1,-1)),
    defaultBackgroundColor(Color(-1,-1,-1,-1)),
    // behavior
    dimming(INT_MIN),
    mouseOverDimming(INT_MIN),
    mouseOverTolerance(INT_MIN)
{
    // font
    for(int i=0; i<10; i++) {
        fontName[i]="";
        fontSize[i]=0;
    }
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
    for(int i=0; i<10; i++) {
        if (other.fontName[i] != unsetConfig.fontName[i]) this->fontName[i] = other.fontName[i];
        if (other.fontSize[i] != unsetConfig.fontSize[i]) this->fontSize[i] = other.fontSize[i];
    }
    // colors
    if (other.colorProfile != unsetConfig.colorProfile) this->colorProfile = other.colorProfile;
    if (other.tempDefaultForegroundColor != unsetConfig.tempDefaultForegroundColor) this->tempDefaultForegroundColor = other.tempDefaultForegroundColor;
    if (other.tempDefaultBackgroundColor != unsetConfig.tempDefaultBackgroundColor) this->tempDefaultBackgroundColor = other.tempDefaultBackgroundColor;
    if (other.defaultForegroundColor != unsetConfig.defaultForegroundColor) this->defaultForegroundColor = other.defaultForegroundColor;
    if (other.defaultBackgroundColor != unsetConfig.defaultBackgroundColor) this->defaultBackgroundColor = other.defaultBackgroundColor;
    // behavior
    if (other.dimming != unsetConfig.dimming) this->dimming = other.dimming;
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

    printSection(os, SECTION_BEHAVIOR);
    printSectionKeyValue(os, SECTION_BEHAVIOR_DIMMING, dimming);
    printSectionKeyValue(os, SECTION_BEHAVIOR_MOUSE_OVER_DIMMING, mouseOverDimming);
    printSectionKeyValue(os, SECTION_BEHAVIOR_TOLERANCE, mouseOverTolerance);

    os << "--------------------------" << std::endl;
}

void Config::printSection(std::ostream& os, const std::string& section)
{
    os << "[" << section << "]" << std::endl;
}

void Config::exitWithVersionNumber()
{
    // print recent version parsed from CHANGELOG file at compile time
    std::cout << "overlay " << VERSION << std::endl;
    exit(0);
}

void Config::exitWithUsage(int exitCode)
{
    std::cout << ""
        "usage: overlay [OPTIONS] <INPUT_FILE>\n"
        "\n"
        "  -c, --config=FILE      file path to read configuration from\n"
        "  -h, --help             prints this help text\n"
        "  -v, --verbose          be verbose and print some debug output\n"
        "  -V, --version          print version number and quit\n"
        "\n"
        "Positioning:\n"
        "  -e PIXEL               screen edge spacing in pixels; defaults to '0'\n"
        "  -l PIXEL               line spacing in pixels; defaults to '0'\n"
        "  -m INDEX               monitor to use; defaults to '0'\n"
        "  -o ORIENTATION         orientation to align window and lines; defaults to 'NW'\n"
        "                         possible values are N, NE, E, SE, S, SW, W, NW and CENTER\n"
        "\n"
        "Font:\n"
        "  -f, --font-name=FONT   font name; defaults to 'NotoSansMono'\n"
        "  -s, --font-size=SIZE   font size; defaults to '12'\n"
        "\n"
        "Colors:\n"
        "  -p, --profile=PROFILE  profile for ansi colors; values are VGA or XP\n"
        "      --fg-color=COLOR   foreground color; defaults to '[97m' (equals '[38;2;255;255;255m')\n"
        "      --fg-alpha=ALPHA   foreground alpha; defaults to '200'\n"
        "      --bg-color=COLOR   background color; defaults to '[40m' (equals '[48;2;0;0;0')\n"
        "      --bg-alpha=ALPHA   background alpha; defaults to '100'\n"
        "\n"
        "Behavior:\n"
        "  -d, --dim=PERCENT      dim the text on mouse over; defaults to '75'%\n"
        "  -D PERCENT             dim the text in general; defaults to '0'%\n"
        "  -t PIXEL               pixel tolerance for mouse over dimming; defaults to '0'\n"
        "";
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
    // font, init only font #0
//    for(int i=0; i<10; i++) {
    config.fontName[0] = "NotoSansMono";
    config.fontSize[0] = 12;
//    }
    // colors
    config.colorProfile = Ansi::Profile::VGA;
    config.tempDefaultForegroundColor = "";
    config.tempDefaultBackgroundColor = "";
    config.defaultForegroundColor = Color(255, 255, 255, 255);
    config.defaultBackgroundColor = Color(0, 0, 0, 100);
    // behavior
    config.dimming = 0;
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
    const option* optIter = longOptions;
    while (optIter->val != 0 && optIter++) {
        if (optIter->val == shortOption) {
            return optIter->name;
        }
    }

    return "";
}

Config Config::fromParameters(int argc, char** argv)
{
    Config config;

    const char* const short_opts = "-c:d:D:e:f:hl:m:o:p:s:t:vV";
    const option long_opts[] = {
            {"config",    required_argument, nullptr, 'c'},
            {"help",      no_argument,       nullptr, 'h'},
            {"verbose",   no_argument,       nullptr, 'v'},
            {"version",   no_argument,       nullptr, 'V'},
            {"font-name", required_argument, nullptr, 'f'},
            {"font-size", required_argument, nullptr, 's'},
            {"profile",   required_argument, nullptr, 'p'},
            {"fg-color",  required_argument, nullptr, SHORT_OPT_FG_COLOR},
            {"fg-alpha",  required_argument, nullptr, SHORT_OPT_FG_ALPHA},
            {"bg-color",  required_argument, nullptr, SHORT_OPT_BG_COLOR},
            {"bg-alpha",  required_argument, nullptr, SHORT_OPT_BG_ALPHA},
            {"dim",       required_argument, nullptr, 'd'},
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
                case 'h':
                    exitWithUsage(0);
                case 'v':
                    config.verbose = true;
                    break;
                case 'V':
                    exitWithVersionNumber();
                    break;

                // Positioning
                case 'e':
                    config.screenEdgeSpacing = assertIntParameter(optarg, -1, -1);
                    break;
                case 'l':
                    config.lineSpacing = assertIntParameter(optarg, 0, -1);
                    break;
                case 'm':
                    config.monitorIndex = assertIntParameter(optarg, 0, -1);
                    break;
                case 'o':
                    config.orientation = assertOrientationParameter(optarg);
                    break;

                // Font
                case 'f':
//if start with [0-9]: use as font index
                    if( isdigit(optarg[0]) && optarg[1]==':' ) {
                        optarg[1]=0;
                        int n = atoi(optarg);
                        config.fontName[n] = optarg+2;
                    } else {
                        config.fontName[0] = optarg;
                    }

                    break;
                case 's':
//if start with [0-9]: use as font index
                    if( isdigit(optarg[0]) && optarg[1]==':' ) {
                        optarg[1]=0;
                        int n = atoi(optarg);
                        config.fontSize[n] = assertIntParameter(optarg+2, 1, -1);
                    } else {
                        config.fontSize[0] = assertIntParameter(optarg, 1, -1);
                    }
                    break;

                // Colors
                case 'p':
                    config.colorProfile = assertProfileParameter(optarg);
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

                // Behavior
                case 'd':
                    config.mouseOverDimming = assertIntParameter(optarg, 0, 100);
                    break;
                case 'D':
                    config.dimming = assertIntParameter(optarg, 0, 100);
                    break;
                case 't':
                    config.mouseOverTolerance = assertIntParameter(optarg, 0, -1);
                    break;

                // other
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
        std::string longOption = lookupLongOptionName(long_opts, opt);
        std::string shortOption = opt < 127 ? std::string(1, opt) : "";
        std::string seperator = !longOption.empty()  && !shortOption.empty() ? ", " : "";
        std::cout << "ERROR: option '" << shortOption << seperator << longOption << "' " << e.what() << ", but was '" << optarg << "' " << std::endl;
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

            static int get_idx(const char*arg, const char*reference)
            {
                if(strlen(arg)!=(strlen(reference)+2)) return -1; // be sure that key has at least 2 extra chars
                if(strncmp(arg, reference, strlen(reference))!=0) return -1; //something totally unrelated
                if(arg[strlen(reference)] != '.') return -1; // index is like '.4'
                if(isdigit(arg[strlen(reference)+1])) return -1; // index is digit 0..9
                return atoi(arg+strlen(reference)+1); // use that that string ends with index digit
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
                config.fontName[0] = value;
                return true;
            }
            if (key == SECTION_FONT_SIZE) {
                config.fontSize[0] = assertIntParameter(value, 1, -1);
                return true;
            }

            
            int idx;

            idx=get_idx(key.c_str(), SECTION_FONT_NAME);
            if(0<=idx and idx<=9) {
                config.fontName[idx] = value;
                return true;
            }

            idx=get_idx(key.c_str(), SECTION_FONT_SIZE);
            if(0<=idx and idx<=9) {
                config.fontSize[idx] = assertIntParameter(value, 1, -1);
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
        if (section == SECTION_BEHAVIOR)
        {
            if (key == SECTION_BEHAVIOR_DIMMING) {
                config.dimming = assertIntParameter(value, 0, 100);
                return true;
            }
            if (key == SECTION_BEHAVIOR_MOUSE_OVER_DIMMING) {
                config.mouseOverDimming = assertIntParameter(value, 0, 100);
                return true;
            }
            if (key == SECTION_BEHAVIOR_TOLERANCE) {
                config.mouseOverTolerance = assertIntParameter(value, 0, -1);
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
