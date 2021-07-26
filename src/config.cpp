#include "config.h"

#include <iostream>
#include <limits.h>
#include <unistd.h>

Config::Config()
:
    inputFile(),
    orientation(Gui::Orientation::NONE),
    dimming(INT_MIN),
    mouseOverTolerance(INT_MIN),
    screenEdgeSpacing(INT_MIN),
    lineSpacing(INT_MIN),
    monitorIndex(INT_MIN)
{
}

Config::~Config()
{
}

Config Config::overrideWith(const Config& other)
{
    Config unsetConfig;
    if (other.inputFile != unsetConfig.inputFile) this->inputFile = other.inputFile;
    if (other.orientation != unsetConfig.orientation) this->orientation = other.orientation;
    if (other.dimming != unsetConfig.dimming) this->dimming = other.dimming;
    if (other.mouseOverTolerance != unsetConfig.mouseOverTolerance) this->mouseOverTolerance = other.mouseOverTolerance;
    if (other.screenEdgeSpacing != unsetConfig.screenEdgeSpacing) this->screenEdgeSpacing = other.screenEdgeSpacing;
    if (other.lineSpacing != unsetConfig.lineSpacing) this->lineSpacing = other.lineSpacing;
    if (other.monitorIndex != unsetConfig.monitorIndex) this->monitorIndex = other.monitorIndex;
    return *this;
}

void Config::exitWithUsage(int exitCode)
{
    std::cout << "usage: overlay [OPTIONS] <INPUT_FILE>" << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  -d <percent>        how much the window dims on mouse over; defaults to '75'%" << std::endl;
    std::cout << "  -e <pixel>          screen edge spacing in pixels; defaults to '0'" << std::endl;
    std::cout << "  -h                  prints this help text" << std::endl;
    std::cout << "  -l <pixel>          line spacing in pixels; defaults to '0'" << std::endl;
    std::cout << "  -m <index>          monitor to use; defaults to '0'" << std::endl;
    std::cout << "  -o <value>          orientation to align window and lines; defaults to 'NW'" << std::endl;
    std::cout << "                      possible values are N, NE, E, SE, S, SW, W, NW and CENTER" << std::endl;
    std::cout << "  -t <pixel>          tolerance in pixel for mouse over dimming; defaults to '0'" << std::endl;
    exit(exitCode);
}

Config Config::defaultConfig()
{
    Config config;
    config.inputFile = "",
    config.orientation = Gui::Orientation::NW;
    config.dimming = 75;
    config.mouseOverTolerance = 0;
    config.screenEdgeSpacing = 0;
    config.lineSpacing = 0;
    config.monitorIndex = 0;
    return config;
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
    while((opt = getopt(argc, argv, "-:d:e:h:l:m:o:t")) != -1)
    {
        switch(opt)
        {
            case 'd':
                config.dimming = assertIntParameter(opt, optarg);
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
            case 't':
                config.mouseOverTolerance = assertIntParameter(opt, optarg);
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

    if (config.inputFile.empty()) {
        std::cout << "ERROR: parameter 'INPUT_FILE' needs a value" << std::endl;
        std::cout << std::endl;
        exitWithUsage(1);
    }

    if (argHelp) {
        exitWithUsage(0);
    }

    return config;
}

