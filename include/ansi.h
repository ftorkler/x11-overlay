#ifndef OVERLAY_ANSI_H
#define OVERLAY_ANSI_H

#include <string>
#include <vector>

#include "color.h"

class Ansi
{

public:

    enum Sequence {
        NONE,
        FOREGROUND_COLOR,
        BACKGROUND_COLOR,
        RESET,
        RESET_FOREGROUND,
        RESET_BACKGROUND,
        UNKNOWN
    };

    static Color toColor(const std::string &ansi);
    // visible for testing
    static Color _to24bitColor(std::string code);
    // visible for testing
    static Color _to8bitColor(int code);

    static Sequence parseControlSequence(const std::string& text);

    static std::vector<std::string> split(const std::string text);
    // visible for testing
    static void subsplit(const std::string& text, std::vector<std::string>* result);

};

#endif // OVERLAY_ANSI_H