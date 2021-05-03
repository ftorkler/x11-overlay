
#ifndef OVERLAY_CONFIG_H
#define OVERLAY_CONFIG_H

#include <string>

#include "gui.h"

class Config 
{

public:

    ~Config();

    Config overrideWith(const Config& other);

    std::string inputFile;
    Gui::Orientation orientation;
    int dimming;
    int mouseOverTolerance;
    int screenEdgeSpacing;
    int lineSpacing;

    static Config defaultConfig();
    static Config fromParameters(int argc, char** argv);

private:

    Config();

    static Gui::Orientation assertOrientationParameter(char c, const std::string& param);
    static int assertIntParameter(char c, const std::string& param);
    static void exitWithUsage(int exitCode);

};

#endif // OVERLAY_CONFIG_H