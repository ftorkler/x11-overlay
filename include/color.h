#ifndef OVERLAY_COLOR_H
#define OVERLAY_COLOR_H

struct Color
{

public:

    unsigned char r, g, b, a;
    
    Color(unsigned char r = 255, 
          unsigned char g = 255, 
          unsigned char b = 255, 
          unsigned char a = 255): r(r), g(g), b(b), a(a) { };

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
};

#endif // OVERLAY_COLOR_H
