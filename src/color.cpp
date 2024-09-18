#include "color.h"

Color::Color(
    unsigned char r, 
    unsigned char g, 
    unsigned char b, 
    unsigned char a)
: 
    r(r), 
    g(g), 
    b(b), 
    a(a) 
{
}

bool Color::operator==(Color other) const
{
    return r == other.r &&
           g == other.g &&
           b == other.b &&
           a == other.a;
}

bool Color::operator != (Color other) const
{ 
    return !(*this == other); 
}
