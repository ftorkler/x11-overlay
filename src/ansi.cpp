#include "ansi.h"

#include <iostream>
#include <sstream>

#define ANSI_START "\e"
#define ANSI_END "m"

#define PREFIX_8BIT_FG "\e[38;5;"
#define PREFIX_8BIT_FG_LEN 7
#define PREFIX_24BIT_FG "\e[38;2;"
#define PREFIX_24BIT_FG_LEN 7
#define FALLBACK_COLOR Color(255, 255, 255)


// https://en.wikipedia.org/wiki/ANSI_escape_code
Color Ansi::toColor(const std::string& ansi)
{
    int pos;

    // 24bit - ESC[38;2;[colorprofile];<r>;<g>;<b>m - foreground color
    if (ansi.rfind(PREFIX_24BIT_FG) == 0 && (pos = ansi.rfind(ANSI_END)) > PREFIX_24BIT_FG_LEN) {
        std::string colorCode = ansi.substr(PREFIX_8BIT_FG_LEN, pos - PREFIX_8BIT_FG_LEN);
        return _to24bitColor(colorCode);
    }

    // 8bit - ESC[38;5;<code>m - foreground color
    if (ansi.rfind(PREFIX_8BIT_FG) == 0 && (pos = ansi.rfind(ANSI_END)) > PREFIX_8BIT_FG_LEN) {
        std::string colorCode = ansi.substr(PREFIX_8BIT_FG_LEN, pos - PREFIX_8BIT_FG_LEN);
        return _to8bitColor(stoi(colorCode));
    }

    // fallback
    return FALLBACK_COLOR;
}

Color Ansi::_to24bitColor(std::string code)
{
    std::stringstream ss(code);
    std::vector<int> tokens;        
    std::string token;
    while (std::getline(ss, token, ';')) {
        tokens.emplace_back(stoi(token));
    }

    int n = tokens.size();
    if (n >= 3) {
        int r = tokens[n-3];
        int g = tokens[n-2];
        int b = tokens[n-1];

        if (r >= 0 && r < 255 &&
            g >= 0 && g < 255 &&
            b >= 0 && b < 255) 
        {
            return Color(r, g, b);
        }
    }
    return FALLBACK_COLOR;
}

Color Ansi::_to8bitColor(int code)
{
    Color colors[] = {
        // standard colors
        Color(0,0,0),       // 00: #000000
        Color(128,0,0),     // 01: #800000
        Color(0,128,0),     // 02: #008000
        Color(128,128,0),   // 03: #808000
        Color(0,0,128),     // 04: #000080
        Color(128,0,128),   // 05: #800080
        Color(0,128,128),   // 06: #008080
        Color(192,192,192), // 07: #c0c0c0

        // high-intensity colors
        Color(128,128,128), // 08: #808080
        Color(255,0,0),     // 09: #ff0000
        Color(0,255,0),     // 10: #00ff00
        Color(255,255,0),   // 11: #ffff00
        Color(0,0,255),     // 12: #0000ff
        Color(255,0,255),   // 13: #ff00ff
        Color(0,255,255),   // 14: #00ffff
        Color(255,255,255)  // 15: #ffffff
    };

    if (code < 0 || code > 255)
    {
        return FALLBACK_COLOR;
    }
    if (code < 16)
    {
        return colors[code];
    }
    if (code > 231)
    {
        int s = (code - 232) * 10 + 8;
        return Color(s, s, s);
    }

    int n = code - 16;
    int b = n % 6;
    int g = ((n - b) / 6) % 6;
    int r = ((n - b - g * 6) / 36) % 6;

    return Color(
        r != 0 ? r * 40 + 55 : 0,
        g != 0 ? g * 40 + 55 : 0,
        b != 0 ? b * 40 + 55 : 0);
}

std::vector<std::string> Ansi::split(const std::string text)
{
    std::string token[] = {
        ANSI_START, 
        ANSI_END
    };

    std::vector<std::string> result;
    size_t start = 0, end;
    int i = 0;
    
    while ((end = text.find_first_of(token[i], start)) != std::string::npos) {
        end = end + i;
        result.emplace_back(text.substr(start, end - start));

        start = end;
        i = (i+1) % 2;
    }
    result.emplace_back(text.substr(start, end));

    return result;
}
