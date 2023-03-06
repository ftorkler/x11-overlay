
#ifndef OVERLAY_CONFIG_H
#define OVERLAY_CONFIG_H

#include <string>
#include <ostream>

#include "ansi.h"
#include "gui.h"

class Config 
{

public:

    ~Config();

    Config overrideWith(const Config& other);
    void print(std::ostream& os) const;

    std::string configFile;
    bool verbose;

    // main
    std::string inputFile;
    // positon
    int monitorIndex;
    Gui::Orientation orientation;
    int screenEdgeSpacing;
    int lineSpacing;
    // font
    std::string fontName;
    int fontSize;
    // colors
    Ansi::Profile colorProfile;
    // mouse over
    int mouseOverTolerance;
    int mouseOverDimming;

    static Config defaultConfig();
    static Config fromParameters(int argc, char** argv);
    static Config fromFile(const std::string& filename, bool suppressWarning = false);

    static std::string getDefaultConfigFilePath();
    static void exitWithUsage(int exitCode);

private:

    Config();

    static Ansi::Profile assertProfileParameter(char c, const std::string& param);
    static Gui::Orientation assertOrientationParameter(char c, const std::string& param);
    static int assertIntParameter(char c, const std::string& param);
    static std::string& trim(std::string& str);

    static bool parseEmptyLine(std::string line);
    static bool parseCommentLine(std::string line);
    static bool parseSectionLine(std::string line, std::string& section);
    static bool parseKeyValueLine(std::string line, std::string section, Config& config);

    static void printSection(std::ostream& os, const std::string& section);
    template <typename T> 
    static void printSectionKeyValue(std::ostream& os, const std::string& key, const T& value)
    {
        os << key << "=" << value << std::endl;
    }

};

#endif // OVERLAY_CONFIG_H