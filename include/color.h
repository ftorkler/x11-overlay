#ifndef OVERLAY_COLOR_H
#define OVERLAY_COLOR_H

#include <string>

class Color
{

public:

    unsigned char r, g, b, a;
    
    Color(unsigned char r = 255, 
          unsigned char g = 255, 
          unsigned char b = 255, 
          unsigned char a = 255): r(r), g(g), b(b), a(a) { };

    static Color fromAnsi(const std::string& ansi);

    bool operator == (Color other) 
    { 
        return r == other.r && 
               g == other.g && 
               b == other.b && 
               a == other.a; 
    }

    bool operator != (Color other) 
    { 
        return !(*this == other); 
    }

    static Color _fromAnsi24bit(std::string code);
    static Color _fromAnsi8bit(int code);

};

#endif // OVERLAY_COLOR_H
