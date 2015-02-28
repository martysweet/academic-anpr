#ifndef ANPRIMAGE_H
#define ANPRIMAGE_H

#include <vector>
#include <sstream>
#include "BitmapImage.h"

struct BandBoundary {
    int b0;
    int bm;
    int b1;
    int Area;
};

class ANPRImage : public BitmapImage
{
    public:
        void ProcessGlobalImage();


    protected:

        // Core Arithmetic Routines
        void VerticalBandDetection(int Iterations, float Constant=0.55);
        void DrawVerticalBandDetection();
        int GetMaxArrayValue(std::vector<int> intArray);
        void AnalyseBandBoundary();

    private:
        void CreateIntensityRowProfileY();
        void CreateRowProfileY();
        void Test();
        void CreateRowProfileX();
        std::vector<int> IntensityRowProfileY;
        std::vector<int> RowProfileX;
        std::vector<int> RowProfileY;
        std::vector<BandBoundary> BandBoundariesX;
        std::vector<BandBoundary> BandBoundariesY;
};

#endif // ANPRIMAGE_H
