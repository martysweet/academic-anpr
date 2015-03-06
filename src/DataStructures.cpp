#include <vector>

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

struct Rect {
    int x;
    int y;
    int Width;
    int Height;
};

struct HoughPoint {
    int t;
    int r;
};

struct Point {
    int x;
    int y;
}
