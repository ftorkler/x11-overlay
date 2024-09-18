#ifndef OVERLAY_COLOR_H
#define OVERLAY_COLOR_H

class Color
{

public:

    unsigned char r, g, b, a;
    
    Color(unsigned char r = 255, 
          unsigned char g = 255, 
          unsigned char b = 255, 
          unsigned char a = 255);

    bool operator == (Color other) const;
    bool operator != (Color other) const;

};

#endif // OVERLAY_COLOR_H
