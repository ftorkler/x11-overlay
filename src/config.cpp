#include "config.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>

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
    // mouse over
    if (other.mouseOverDimming != unsetConfig.mouseOverDimming) this->mouseOverDimming = other.mouseOverDimming;
    if (other.mouseOverTolerance != unsetConfig.mouseOverTolerance) this->mouseOverTolerance = other.mouseOverTolerance;
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
    config.fontName="NotoSansMono";
    config.fontSize=12;
    // colors
    config.colorProfile = Ansi::Profile::VGA;
    // mouse over
    config.mouseOverDimming = 75;
    config.mouseOverTolerance = 0;
    return config;
}

Ansi::Profile Config::assertProfileParameter(char c, const std::string& param)
{
    Ansi::Profile profile = Ansi::profileFromString(optarg);
    if (profile == Ansi::Profile::NONE) {
        std::cout << "ERROR: option '" << c << "' has invalid value: " << optarg << "; must be [VGA,XP]"<< std::endl;
        exit(1);
    }
    return profile;
}

Gui::Orientation Config::assertOrientationParameter(char c, const std::string& param)
{
    Gui::Orientation orientation = Gui::orientationFromString(optarg);
    if (orientation == Gui::Orientation::NONE) {
        std::cout << "ERROR: option '" << c << "' has invalid value: " << optarg << "; must be [N,NE,E,SE,S,SW,W,NW,CENTER]"<< std::endl;
        exit(1);
    }
    return orientation;
}

int Config::assertIntParameter(char c, const std::string& param)
{
    char* end;
    int value = strtol(param.c_str(), &end, 10);
    bool failedParsing = *end != '\0';
    if (failedParsing) {
        bool isNextParameter = !param.empty() && param[0] == '-';
        if (isNextParameter) {
            std::cout << "ERROR: option '" << c << "' needs a value" << std::endl;
            exit(1);
        }
        std::cout << "ERROR: option '" << c << "' is not an integer value: " << optarg << std::endl;
        exit(1);
    }
    return value;
}

Config Config::fromParameters(int argc, char** argv)
{
    bool argHelp = false;

    Config config;

    int opt;
    while((opt = getopt(argc, argv, "-c:d:e:f:hl:m:o:p:s:t:v")) != -1)
    {
        switch(opt)
        {
            case 'c':
                config.configFile = optarg;
                break;
            case 'd':
                config.mouseOverDimming = assertIntParameter(opt, optarg);
                break;
            case 'e':
                config.screenEdgeSpacing = assertIntParameter(opt, optarg);
                break;
            case 'f':
                config.fontName = optarg;
                break;
            case 'h':
                argHelp = true;
                break;
            case 'l':
                config.lineSpacing = assertIntParameter(opt, optarg);
                break;
            case 'm':
                config.monitorIndex = assertIntParameter(opt, optarg);
                break;
            case 'o':
                config.orientation = assertOrientationParameter(opt, optarg);
                break;
            case 'p':
                config.colorProfile = assertProfileParameter(opt, optarg);
                break;
            case 's':
                config.fontSize = assertIntParameter(opt, optarg);
                break;
            case 't':
                config.mouseOverTolerance = assertIntParameter(opt, optarg);
                break;
            case 'v':
                config.verbose = true;
                break;
            case 1:
                config.inputFile = optarg;
                break;
            case ':':
                std::cout << "ERROR: option '" << (char)optopt << "' needs a value" << std::endl;
                exit(1);
                break;
            case '?':
                std::cout << "ERROR: unknown option: " << (char)optopt << std::endl;
                std::cout << std::endl;
                exitWithUsage(1);
                break;
        }
    }

    if (argHelp) {
        exitWithUsage(0);
    }

    return config;
}

Config Config::fromFile(const std::string& filename, bool suppressWarning)
{
    Config config;

    std::ifstream filein = std::ifstream(filename);
    std::string section;
    int i = 0;
    for (std::string line; std::getline(filein, line); ++i) {
        if (!parseEmptyLine(line) &&
            !parseCommentLine(line) &&
            !parseSectionLine(line, section) &&
            !parseKeyValueLine(line, section, config))
        {
            std::cout << "WARN: config line:" << i << " is not a comment, section nor a key-value pair: '" << line << "'" << std::endl;
        }
    }
    filein.close();

    if (i == 0 && !suppressWarning) {
        std::cout << "WARN: no (valid) config file found at '" << filename << "'" << std::endl;
    }

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

        try {
            if (section == SECTION_NONE)
            {
                if (key == SECTION_NONE_INPUT_FILE) {
                    config.inputFile = value;
                    return true;
                }
            }
            else if (section == SECTION_POSITIONING) 
            {
                if (key == SECTION_POSITIONING_MONITOR_INDEX) {
                    config.monitorIndex = std::stoi(value);
                    return true;
                }
                if (key == SECTION_POSITIONING_ORIENTATION) {
                    Gui::Orientation orientation = Gui::orientationFromString(value);
                    if (orientation == Gui::Orientation::NONE) {
                        throw std::runtime_error("invalid orientation");
                    }
                    config.orientation = orientation;
                    return true;
                }
                if (key == SECTION_POSITIONING_SCREEN_EDGE_SPACING) {
                    config.screenEdgeSpacing = std::stoi(value);
                    return true;
                }
                if (key == SECTION_POSITIONING_LINE_SPACING) {
                    config.lineSpacing = std::stoi(value);
                    return true;
                }
            }
            else if (section == SECTION_FONT) 
            {
                if (key == SECTION_FONT_NAME) {
                    config.fontName = value;
                    return true;
                }
                if (key == SECTION_FONT_SIZE) {
                    config.fontSize = std::stoi(value);
                    return true;
                }
            }
            else if (section == SECTION_COLORS) 
            {
                if (key == SECTION_COLORS_ANSI_PROFILE) {
                    Ansi::Profile profile = Ansi::profileFromString(value);
                    if (profile == Ansi::Profile::NONE) {
                        throw std::runtime_error("invalid ansi profile");
                    }
                    config.colorProfile = profile;
                    return true;
                }
            }
            else if (section == SECTION_MOUSE_OVER) 
            {
                if (key == SECTION_MOUSE_OVER_TOLERANCE) {
                    config.mouseOverTolerance = std::stoi(value);
                    return true;
                }
                if (key == SECTION_MOUSE_OVER_DIMMING) {
                    config.mouseOverDimming = std::stoi(value);
                    return true;
                }
            }
        }
        catch (std::exception const&) {
            std::cout << "could not parse config file value '" << value << "' for key '" << key << "' (ignored)" << std::endl;
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