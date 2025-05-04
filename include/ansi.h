#ifndef OVERLAY_ANSI_H
#define OVERLAY_ANSI_H

#include <string>
#include <vector>

#include "color.h"

#define ANSI_INIT "\e["
#define ANSI_START '\e'
#define ANSI_END 'm'
#define ANSI_DELIMITER ';'

class Ansi
{

public:

    enum class Sequence {
        NONE = 0,
        FOREGROUND_COLOR,
        BACKGROUND_COLOR,
        INCREASE_INTENSITY,
        DECREASED_INTENSITY,
        NORMAL_INTENSITY,
        RESET,
        RESET_FOREGROUND,
        RESET_BACKGROUND,
        DEFAULT_FONT,
        ALTERNATIVE_FONT,
        UNKNOWN
    };

    enum Profile {
        VGA = 0,
        XP,
        NONE
    };

    static std::string profileToString(Profile profile);
    static Profile profileFromString(const std::string& input);

    static Color toColor(const std::string& ansi, Color fallbackColor, bool increaseIntensity = false, Profile profile = Profile::XP);
    static unsigned int toFontIndex(const std::string& ansi);

    static Sequence parseControlSequence(const std::string& text);

    static std::vector<std::string> split(const std::string text);

 private:

    static Color _to24bitColor(std::string code, Color fallbackColor);
    static Color _to8bitColor(int code, Color fallbackColor, Profile profile = Profile::XP);

    static void subsplit(const std::string& text, std::vector<std::string>* result);

};

#endif // OVERLAY_ANSI_H
