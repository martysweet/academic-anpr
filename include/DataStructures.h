#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <vector>

struct Rectangle {
    int x;
    int y;
    int Width;
    int Height;
};

struct ROI {
    Rectangle Rect;
    int Score = 0;
    int Pixels = 0;
};

struct Point {
    int x;
    int y;
};

#endif // DATASTRUCT_H
