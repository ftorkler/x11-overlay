
#ifndef OVERLAY_CONFIG_H
#define OVERLAY_CONFIG_H

#include <string>
#include <ostream>

#include "ansi.h"
#include "gui.h"

struct option;

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
    std::string tempDefaultForegroundColor;
    std::string tempDefaultBackgroundColor;
    Color defaultForegroundColor;
    Color defaultBackgroundColor;
    // behavior
    int dimming;
    int mouseOverDimming;
    int mouseOverTolerance;

    static Config defaultConfig();
    static Config fromParameters(int argc, char** argv);
    static Config fromFile(const std::string& filename, bool suppressWarning = false);

    static std::string getDefaultConfigFilePath();
    static void exitWithVersionNumber();
    static void exitWithUsage(int exitCode);

private:

    Config();

    void finalizeColors();

    static Ansi::Profile assertProfileParameter(const std::string& param);
    static Gui::Orientation assertOrientationParameter(const std::string& param);
    static int assertIntParameter(const std::string& param, int min, int max);
    static Color assertAnsiColorParameter(const std::string& param, Ansi::Profile colorProfile);
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
    static void printSectionKeyValueAnsiColor(std::ostream& os, const std::string& key, Ansi::Sequence seq, const Color& color)
    {
        std::string prefix;
        switch (seq)
        {
        case Ansi::Sequence::FOREGROUND_COLOR:
            prefix = "[38;2;";
            break;
        case Ansi::Sequence::BACKGROUND_COLOR:
            prefix = "[48;2;";
            break;
        default:
            prefix = "";
        }
        os << key << "=" << prefix << (int)color.r << ";" << (int)color.g << ";" << (int)color.b << "m" << std::endl;
    }

    static std::string lookupLongOptionName(const struct option* longOptions, int shortOption);

};

#endif // OVERLAY_CONFIG_H