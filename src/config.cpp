#include "config.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>

Config::Config()
:
    configFile(""),
    verbose(false),
    inputFile(""),
    colorProfile(Ansi::Profile::VGA),
    monitorIndex(INT_MIN),
    orientation(Gui::Orientation::NONE),
    screenEdgeSpacing(INT_MIN),
    lineSpacing(INT_MIN),
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
    if (other.colorProfile != unsetConfig.colorProfile) this->colorProfile = other.colorProfile;
    if (other.orientation != unsetConfig.orientation) this->orientation = other.orientation;
    if (other.mouseOverDimming != unsetConfig.mouseOverDimming) this->mouseOverDimming = other.mouseOverDimming;
    if (other.mouseOverTolerance != unsetConfig.mouseOverTolerance) this->mouseOverTolerance = other.mouseOverTolerance;
    if (other.screenEdgeSpacing != unsetConfig.screenEdgeSpacing) this->screenEdgeSpacing = other.screenEdgeSpacing;
    if (other.lineSpacing != unsetConfig.lineSpacing) this->lineSpacing = other.lineSpacing;
    if (other.monitorIndex != unsetConfig.monitorIndex) this->monitorIndex = other.monitorIndex;
    return *this;
}

void Config::print(std::ostream& os) const
{
    os << "default config file path is '" << getDefaultConfigFilePath() << "'" << std::endl;
    os << "---- resulting config ----" << std::endl;
    os << "InputFile=" << inputFile << std::endl;    
    os << std::endl;
    os << "[Appearance]"<< std::endl;
    os << "ColorProfile=" << Ansi::profileToString(colorProfile) << std::endl;
    os << std::endl;
    os << "[Position]"<< std::endl;
    os << "MonitorIndex=" << monitorIndex << std::endl;
    os << "Orientation=" << Gui::orientationToString(orientation) << std::endl;
    os << "ScreenEdgeSpacing=" << screenEdgeSpacing << std::endl;
    os << "LineSpacing=" << lineSpacing << std::endl;
    os << std::endl;
    os << "[MouseOver]"<< std::endl;
    os << "Tolerance=" << mouseOverTolerance << std::endl;
    os << "Dimming=" << mouseOverDimming << std::endl;

    os << "--------------------------" << std::endl;
}

void Config::exitWithUsage(int exitCode)
{
    std::cout << "usage: overlay [OPTIONS] <INPUT_FILE>" << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  -c <file>           file path to read configuration from" << std::endl;
    std::cout << "  -d <percent>        how much the window dims on mouse over; defaults to '75'%" << std::endl;
    std::cout << "  -e <pixel>          screen edge spacing in pixels; defaults to '0'" << std::endl;
    std::cout << "  -h                  prints this help text" << std::endl;
    std::cout << "  -l <pixel>          line spacing in pixels; defaults to '0'" << std::endl;
    std::cout << "  -m <index>          monitor to use; defaults to '0'" << std::endl;
    std::cout << "  -o <value>          orientation to align window and lines; defaults to 'NW'" << std::endl;
    std::cout << "                      possible values are N, NE, E, SE, S, SW, W, NW and CENTER" << std::endl;
    std::cout << "  -p <value>          profile for ansi colors; values are VGA or XP" << std::endl;
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
    config.colorProfile = Ansi::Profile::VGA;
    config.orientation = Gui::Orientation::NW;
    config.mouseOverDimming = 75;
    config.mouseOverTolerance = 0;
    config.screenEdgeSpacing = 0;
    config.lineSpacing = 0;
    config.monitorIndex = 0;
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
    while((opt = getopt(argc, argv, "-c:d:e:hl:m:o:p:t:v")) != -1)
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
            if (section == "")
            {
                if (key == "InputFile") {
                    config.inputFile = value;
                    return true;
                }
            }
            else if (section == "Appearance") 
            {
                if (key == "ColorProfile") {
                    Ansi::Profile profile = Ansi::profileFromString(value);
                    if (profile == Ansi::Profile::NONE) {
                        throw std::runtime_error("invalid profile");
                    }
                    config.colorProfile = profile;
                    return true;
                }
            }
            else if (section == "Position") 
            {
                if (key == "MonitorIndex") {
                    config.monitorIndex = std::stoi(value);
                    return true;
                }
                if (key == "LineSpacing") {
                    config.lineSpacing = std::stoi(value);
                    return true;
                }
                if (key == "ScreenEdgeSpacing") {
                    config.screenEdgeSpacing = std::stoi(value);
                    return true;
                }
                if (key == "Orientation") {
                    Gui::Orientation orientation = Gui::orientationFromString(value);
                    if (orientation == Gui::Orientation::NONE) {
                        throw std::runtime_error("invalid orientation");
                    }
                    config.orientation = orientation;
                    return true;
                }
            }
            else if (section == "MouseOver") 
            {
                if (key == "Tolerance") {
                    config.mouseOverTolerance = std::stoi(value);
                    return true;
                }
                if (key == "Dimming") {
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