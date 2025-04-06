#include "ansi.h"

#include <cstddef>
#include <deque>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "color.h"

#define PREFIX_FG_COLOR "\e[38"
#define PREFIX_BG_COLOR "\e[48"
#define PREFIX_COLOR_LEN 4
#define INFIX_COLOR_8BIT ";5;"
#define PREFIX_COLOR_8BIT_LEN 7
#define INFIX_COLOR_24BIT ";2;"
#define PREFIX_COLOR_24BIT_LEN 7

// https://en.wikipedia.org/wiki/ANSI_escape_code
Color Ansi::toColor(const std::string& ansi, Color fallbackColor, bool increaseIntensity, Ansi::Profile profile)
{
    int pos;
    if (ansi.rfind(ANSI_INIT) != 0) {
        // fallback
        return fallbackColor;
    }

    if (ansi.find(PREFIX_FG_COLOR) == 0 || ansi.find(PREFIX_BG_COLOR) == 0) {
        // 24bit - ESC[38;2;[colorprofile];<r>;<g>;<b>m - foreground color
        // 24bit - ESC[48;2;[colorprofile];<r>;<g>;<b>m - background color
        if (ansi.find(INFIX_COLOR_24BIT) == PREFIX_COLOR_LEN && (pos = ansi.rfind(ANSI_END)) > PREFIX_COLOR_24BIT_LEN) {
            std::string colorCode = ansi.substr(PREFIX_COLOR_24BIT_LEN, pos - PREFIX_COLOR_24BIT_LEN);
            return _to24bitColor(colorCode, fallbackColor);
        }

        // 8bit - ESC[38;5;<code>m - foreground color
        // 8bit - ESC[48;5;<code>m - background color
        if (ansi.find(INFIX_COLOR_8BIT) == PREFIX_COLOR_LEN && (pos = ansi.rfind(ANSI_END)) > PREFIX_COLOR_8BIT_LEN) {
            std::string colorCode = ansi.substr(PREFIX_COLOR_8BIT_LEN, pos - PREFIX_COLOR_8BIT_LEN);
            return _to8bitColor(stoi(colorCode), fallbackColor, profile);
        }
    }

    //3-4bit
    if (ansi.find(ANSI_DELIMITER) == std::string::npos && (pos = ansi.rfind(ANSI_END)) >= 4) {
        int colorCode = stoi(ansi.substr(2, pos - 2));
        if (colorCode >= 30 && colorCode <= 37) {
            int intensitiyLift = increaseIntensity ? 8 : 0;
            return _to8bitColor(colorCode - 30 + intensitiyLift, fallbackColor, profile);
        }
        if (colorCode >= 40 && colorCode <= 47) {
            int intensitiyLift = increaseIntensity ? 8 : 0;
            return _to8bitColor(colorCode - 40 + intensitiyLift, fallbackColor, profile);
        }
        if (colorCode >= 90 && colorCode <= 97) {
            return _to8bitColor(colorCode - 90 + 8, fallbackColor, profile);
        }
        if (colorCode >= 100 && colorCode <= 107) {
            return _to8bitColor(colorCode - 100 + 8, fallbackColor, profile);
        }
    }

    return fallbackColor;
}

Color Ansi::_to24bitColor(std::string code, Color fallbackColor)
{
    std::stringstream ss(code);
    std::vector<int> tokens;
    std::string token;
    while (std::getline(ss, token, ANSI_DELIMITER)) {
        tokens.emplace_back(stoi(token));
    }

    int n = tokens.size();
    if (n >= 3) {
        int r = tokens[n-3];
        int g = tokens[n-2];
        int b = tokens[n-1];

        if (r >= 0 && r <= 255 &&
            g >= 0 && g <= 255 &&
            b >= 0 && b <= 255)
        {
            return Color(r, g, b);
        }
    }
    return fallbackColor;
}

Color Ansi::_to8bitColor(int code, Color fallbackColor, Ansi::Profile profile)
{
    Color colors[2][16] =
    {
        // VGA
        {
            // standard colors
            Color(0,0,0),       // 00: black
            Color(170,0,0),     // 01: red
            Color(0,170,0),     // 02: green
            Color(170,85,0),    // 03: yellow
            Color(0,0,170),     // 04: blue
            Color(170,0,170),   // 05: magenta
            Color(0,170,170),   // 06: cyan
            Color(170,170,170), // 07: white

            // high-intensity colors
            Color(85,85,85),    // 08: bright black
            Color(255,85,85),   // 09: bright red
            Color(85,255,85),   // 10: bright green
            Color(255,255,85),  // 11: bright yellow
            Color(85,85,255),   // 12: bright blue
            Color(255,85,255),  // 13: bright magenta
            Color(85,255,255),  // 14: bright cyan
            Color(255,255,255)  // 15: bright white
        },

        // windows XP
        {
            // standard colors
            Color(0,0,0),       // 00: black
            Color(128,0,0),     // 01: red
            Color(0,128,0),     // 02: green
            Color(128,128,0),   // 03: yellow
            Color(0,0,128),     // 04: blue
            Color(128,0,128),   // 05: magenta
            Color(0,128,128),   // 06: cyan
            Color(192,192,192), // 07: white

            // high-intensity colors
            Color(128,128,128), // 08: bright black
            Color(255,0,0),     // 09: bright red
            Color(0,255,0),     // 10: bright green
            Color(255,255,0),   // 11: bright yellow
            Color(0,0,255),     // 12: bright blue
            Color(255,0,255),   // 13: bright magenta
            Color(0,255,255),   // 14: bright cyan
            Color(255,255,255)  // 15: bright white
        }
    };

    if (code < 0 || code > 255)
    {
        return fallbackColor;
    }
    if (code < 16)
    {
        return colors[profile][code];
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

Ansi::Sequence Ansi::parseControlSequence(const std::string& text)
{
    if (text.length() < 4 || text.rfind(ANSI_INIT) != 0 || text.find(ANSI_END) != text.length()-1) {
        return Sequence::NONE;
    }

    std::string code;

    size_t pos = text.find_first_of(ANSI_DELIMITER);
    if (pos != std::string::npos) {
        code = text.substr(2, pos-2);
    } else {
        code = text.substr(2, text.size()-3);
    }

    if (code.size() > 3) {
        return Sequence::UNKNOWN;
    }

    switch (stoi(code))
    {
    case 0:
        return Sequence::RESET;
    case 1:
        return Sequence::INCREASE_INTENSITY;
    case 2:
        return Sequence::DECREASED_INTENSITY;
    case 22:
        return Sequence::NORMAL_INTENSITY;
    case 39:
        return Sequence::RESET_FOREGROUND;
    case 49:
        return Sequence::RESET_BACKGROUND;
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 90:
    case 91:
    case 92:
    case 93:
    case 94:
    case 95:
    case 96:
    case 97:
    case 98:
        return Sequence::FOREGROUND_COLOR;
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 100:
    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
    case 106:
    case 107:
    case 108:
        return Sequence::BACKGROUND_COLOR;
    default:
        return Sequence::UNKNOWN;
    }
}

std::vector<std::string> Ansi::split(const std::string text)
{
    std::string token[] = {
        std::string(ANSI_INIT),
        std::string(1, ANSI_END)
    };

    std::vector<std::string> result;
    size_t start = 0, end;
    int i = (text[0] == ANSI_INIT[0] && text[1] == ANSI_INIT[1]) ? 1 : 0;

    while ((end = text.find(token[i], start)) != std::string::npos) {
        end = end + i;
        subsplit(text.substr(start, end - start), &result);

        start = end;
        i = (i+1) % 2;
    }
    if (start != text.size()) {
        subsplit(text.substr(start, end), &result);
    }

    return result;
}

void Ansi::subsplit(const std::string& text, std::vector<std::string>* result) {
    if (text.size() <= 2 || text[0] != ANSI_START || text[text.size()-1] != ANSI_END) {
        result->emplace_back(text);
        return;
    }

    std::string token;
    std::istringstream iss(text.substr(2, text.size()-3));
    std::deque<int> codes;
    while (getline(iss, token, ANSI_DELIMITER)) {
        codes.push_back(stoi(token));
    }

    while (!codes.empty()) {
        int code = codes.front();
        codes.pop_front();

        if (code == 38 || code == 48 || code == 58) {
            std::stringstream sequence;
            sequence << ANSI_INIT << code;

            code = codes.front(); codes.pop_front();
            sequence << ANSI_DELIMITER << code;

            if (code == 2) {
                sequence << ANSI_DELIMITER << codes.front(); codes.pop_front();
                sequence << ANSI_DELIMITER << codes.front(); codes.pop_front();
                sequence << ANSI_DELIMITER << codes.front(); codes.pop_front();
            }
            else if (code == 5) {
                sequence << ANSI_DELIMITER << codes.front(); codes.pop_front();
            }
            else {
                std::cout << "parsing ansi control sequence 'ESC" << text.substr(1, text.size()-1) << "'... FAILED" << std::endl;
            }
            sequence << ANSI_END;
            result->emplace_back(sequence.str());
        }
        else {
            std::stringstream sequence;
            sequence << ANSI_INIT << code << ANSI_END;
            result->emplace_back(sequence.str());
        }
    }
}

std::string Ansi::profileToString(Profile profile)
{
    switch (profile)
    {
        case Profile::VGA: return "VGA";
        case Profile::XP: return "XP";
        default: return "";
    }
}

Ansi::Profile Ansi::profileFromString(const std::string& input)
{
    for (int profile=Profile::VGA; profile<Profile::NONE; ++profile) {
        if (input == profileToString(static_cast<Profile>(profile))) {
            return static_cast<Profile>(profile);
        }
    }
    return Profile::NONE;
}
