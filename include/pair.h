#ifndef OVERLAY_PAIR_H
#define OVERLAY_PAIR_H

struct IntPair
{
    union
    {
        int x, w;
    };
    union
    {
        int y, h;
    };
};

typedef IntPair Position;
typedef IntPair Dimesion;

#endif // OVERLAY_PAIR_H