#ifndef BITMAPIMAGE_H
#define BITMAPIMAGE_H

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "../SDL_Wrapper.h"
#include "../src/DataStructures.cpp"



class BitmapImage
{
    public:
        BitmapImage(); // Constructor
        virtual ~BitmapImage(); // Destructor

        /* Image Loading/Saving Functions */
        void LoadBitmapImage(std::string Filename);
        void LoadBitmapImage(SDL_Surface* InputImage, Rectangle Rect);
        void SaveImageToFile(std::string Filename);
        void RestoreToLoadedImage();

    // Allow these variables to be accessed by inheritied classes
    protected:
        SDL_Surface* Image;

        /* Matrix Manipulation Functions */
        void  ImageConvolutionMatrixTransformation(std::vector<ConvolutionMatrix> Convolutions);
        ConvolutionMatrix CannyX, CannyY;
        ConvolutionMatrix HorizontalRank;

        /* Line Drawing Functions */
        void DrawLine(int x1, int y1, int x2, int y2, Uint8 R, Uint8 G, Uint8 B);
        void DrawRectangle(int x, int y, int w, int h,  Uint8 R, Uint8 G, Uint8 B);
        void DrawRectangle(Rectangle Rect, Uint8 R, Uint8 G, Uint8 B);

        /* Whole Image Editing Function */
        void ImageToBW(int Threshold = 200, bool Inverted = false);
        void CannyEdgeDetection();
        void ImageToGrayscale();
        void NormaliseImage();
        void HistogramEqualisation();
        void ImageToAdaptiveMonochrome(int Area=30, bool Inverted = false);

        /* Image Analysis Functions */
        void CreateGrayscaleMapArray();
        int  GetGrayscaleMapValue(int x, int y);
        std::vector<HoughPoint> LocateHoughPoints(float Threshold = 0.5);
        void CreateIntegralArray();
        int  GetIntegralValue(int x, int y);
        int  GetIntegralAreaAverage(int x, int y, int Area=12);




    // Don't allow these variables to be accessed by inheritied classes
    private:
        SDL_Surface* LoadedImage;

        /* Private Datastores */
        std::vector<int> GrayscaleArray;
        std::vector<int> IntegralArray;


        /* Matrix Manipulation Functions */
        float  ProcessKernelFilter(int x, int y, ConvolutionMatrix Kernel);

        /* Line Drawing Functions */
        void OrderCoordinates(int &x1, int &y1, int &x2, int &y2);

        /* Matrix Manipulation Functions */
        void  InitialiseConvolutionMatrix(ConvolutionMatrix &Struct, int Columns, int Rows, float Weight, std::initializer_list<float> List);
};

#endif // BITMAPIMAGE_H
