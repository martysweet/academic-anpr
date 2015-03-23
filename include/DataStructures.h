#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <vector>

/* Rectangle */
struct Rectangle {
    int x;
    int y;
    int Width;
    int Height;
};

/* Region of Interest */
struct ROI {
    Rectangle Rect;
    int Score = 0;
    int Pixels = 0;
};

/* 2D Point */
struct Point {
    int x;
    int y;
};

/* Global Configuration */
namespace Configuration {
    extern bool Debug;
    extern bool SDLOutput;
    extern bool SaveOutput;
    extern int  Cases;
    extern int  Buffer;
    extern int  Spacing;
};


#endif // DATASTRUCT_H
