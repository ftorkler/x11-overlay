#ifndef OVERLAY_ANSI_H
#define OVERLAY_ANSI_H

#include <string>
#include <vector>

#include "color.h"

class Ansi
{

public:

    static Color toColor(const std::string &ansi);
    
    static std::vector<std::string> split(const std::string text);

    // visible for testing
    static Color _to24bitColor(std::string code);
    // visible for testing
    static Color _to8bitColor(int code);

};

#endif // OVERLAY_ANSI_H