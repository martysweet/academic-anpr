#ifndef BITMAPIMAGE_H
#define BITMAPIMAGE_H

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include "../SDL_Wrapper.h"

struct ConvolutionMatrix{
    std::vector<float> Matrix;
    int Rows;
    int Columns;
    float Weight;
};

class BitmapImage
{
    public:
        BitmapImage(); // Constructor
        virtual ~BitmapImage(); // Destructor

        // Misc
        void LoadImageFromFile(std::string Filename);
        void SaveImageToFile(std::string Filename);

    /* Allow these variables to be accessed by inheritied classes */
    protected:
        SDL_Surface* Image;

        // Image editing functions
        void ImageToGrayscale();
        void SobelEdgeDetection();
        void EdgeDetection();
        void ImageToBW(int Threshold=200);
        void ImageToAdaptiveMonochrome(int Area=30);
        void ImageToLocalisedMonochrome(int Threshold=80);
        void ImageGrayscaleToDifferentialMonochrome(int Difference=10);
        void DrawLine(int x1, int y1, int x2, int y2,   Uint8 R, Uint8 G, Uint8 B);
        void DrawRectangle(int x, int y, int w, int h,  Uint8 R, Uint8 G, Uint8 B);
        void ImageConvolutionMatrixTransformation(std::vector<ConvolutionMatrix> Convolutions);
        ConvolutionMatrix SobelVertical, SobelHorizontal, GaussianBlur1, GaussianBlur2, GaussianBlur3;
        ConvolutionMatrix EdgeHorizontal,Median, EdgeVertical, EdgeDetect3, EdgeDetect4, HorizontalRank, VerticalRank, Blur1;

    /* Don't allow these variables to be accessed by inheritied classes */
    private:
        // Core Arithmetic
        void CreateIntegralArray();
        int  GetIntegralValue(int x, int y);
        int  GetIntegralAreaAverage(int x, int y, int area=12);
        void CreateGrayscaleMapArray();
        int  GetGrayscaleMapValue(int x, int y);
        void OrderCoordinates(int &x1, int &y1, int &x2, int &y2);
        int  ProcessKernelFilter(int x, int y, ConvolutionMatrix Kernel);
        void InitialiseConvolutionMatrix(ConvolutionMatrix &Struct, int Columns, int Rows, float Weight, std::initializer_list<float> List);
        std::vector<int> IntegralArray;
        std::vector<int> GrayscaleArray;

};

#endif // BITMAPIMAGE_H
