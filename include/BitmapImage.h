#ifndef BITMAPIMAGE_H
#define BITMAPIMAGE_H

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>

#include "SDL_Wrapper.h"
#include "DataStructures.h"


class BitmapImage
{
    public:
        /* Image Loading/Saving Functions */
        void LoadBitmapImage(std::string Filename);
        void LoadBitmapImage(SDL_Surface* InputImage);
        void LoadBitmapImage(SDL_Surface* InputImage, Rectangle Rect);
        void CreateBitmapImage(int w, int h, bool White = false);
        void SaveImageToFile(std::string Filename);
        void RestoreToLoadedImage();
        void GetBitmapSurface(SDL_Surface* OutputSurface);
        void SetPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);

        /* Image Editing */
        void ImageToAdaptiveMonochrome(int Area, bool Inverted = false);

        /* Debug Display */
        void DebugDisplayImageToWindow(std::string Title);

    protected:
        SDL_Surface* Image;
        SDL_Surface* LoadedImage;

        /* Line Drawing Functions */
        void DrawLine(int x1, int y1, int x2, int y2, Uint8 R, Uint8 G, Uint8 B);
        void DrawLine(Point Point1, Point Point2, Uint8 R, Uint8 G, Uint8 B);
        void DrawRectangle(int x, int y, int w, int h,  Uint8 R, Uint8 G, Uint8 B);
        void DrawRectangle(Rectangle Rect, Uint8 R, Uint8 G, Uint8 B);

        /* Whole Image Editing Function */
        void ImageToGrayscale();
        void NormaliseImage(int nMax = 100);
        void HistogramEqualisation();

        /* Image Analysis Functions */
        void CreateGrayscaleMapArray();
        int  GetGrayscaleMapValue(int x, int y);
        int  GetIntegralValue(int x, int y);


    private:
        /* Private Datastores */
        std::vector<int> GrayscaleArray;
        std::vector<int> IntegralArray;

        /* Line Drawing Functions */
        void OrderCoordinates(int &x1, int &y1, int &x2, int &y2);

        /* Image Analysis Functions */
        void CreateIntegralArray();
        int  GetIntegralAreaAverage(int x, int y, int Area);

};

#endif // BITMAPIMAGE_H
