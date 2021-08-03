
#ifndef OVERLAY_CONFIG_H
#define OVERLAY_CONFIG_H

#include <string>

#include "gui.h"

class Config 
{

public:

    ~Config();

    Config overrideWith(const Config& other);

    // main
    std::string inputFile;
    // positon
    int monitorIndex;
    Gui::Orientation orientation;
    int screenEdgeSpacing;
    int lineSpacing;
    // mouse over
    int mouseOverTolerance;
    int mouseOverDimming;

    static Config defaultConfig();
    static Config fromParameters(int argc, char** argv);
    static Config fromFile(const std::string& filename, bool suppressWarning = false);

    static std::string getDefaultConfigFilePath();
    static void exitWithUsage(int exitCode);

    // visible for testing
    static bool parseEmptyLine(std::string line);
    static bool parseCommentLine(std::string line);
    static bool parseSectionLine(std::string line, std::string& section);
    static bool parseKeyValueLine(std::string line, std::string section, Config& config);

private:

    Config();

    static Gui::Orientation assertOrientationParameter(char c, const std::string& param);
    static int assertIntParameter(char c, const std::string& param);
    static std::string& trim(std::string& str);

};

#endif // OVERLAY_CONFIG_H