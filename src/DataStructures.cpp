#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <vector>

#define DEGTORADIAN 0.0174532925

struct ConvolutionMatrix{
    std::vector<float> Matrix;
    int Rows;
    int Columns;
    float Weight;
};

struct BandBoundary {
    int b0;
    int bm;
    int b1;
    int Area;
};

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

struct HoughPoint {
    int t;
    int r;
};

struct Point {
    int x;
    int y;
};

struct ProjectionProfileResult {
    int Coordinate;
    int Value;
};

struct ProjectionAnalysis {
    ProjectionProfileResult Lower;
    ProjectionProfileResult Middle;
    ProjectionProfileResult Upper;
    int Area = 0;      // Would be good to replace these with functions
  //  int Height = 0;    // if possible.
    float Score = 0;
    int Height(){
        return Upper.Coordinate-Lower.Coordinate;
    }
};



struct AverageGrouping {
    int Start = -1;
    int End = -1;
    int Length = 0;
};


#endif // DATASTRUCT_H
