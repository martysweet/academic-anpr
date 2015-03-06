#ifndef ANPRIMAGE_H
#define ANPRIMAGE_H

#include <vector>
#include <sstream>
#include "BitmapImage.h"
#include "DataStructures.cpp"



class ANPRImage : public BitmapImage {
    public:
        void ProcessGlobalImage();


    protected:

        // Core Arithmetic Routines
        void VerticalBandDetection(std::vector<int> RowProfile, int Iterations, float Constant=0.55);
        void HorizontalBandDetection(std::vector<int> & HoughPoints);

        void DrawVerticalBandDetection();
        int  GetMaxArrayValue(std::vector<int> intArray);
        void AnalyseBandBoundary();
        void CreateIntensityRowProfileY();
        void CreateRowProfileY(bool GraphicalOutput=false);
        void CreateRowProfileX();
        void CreateIntensityRowProfileX(bool GraphicalOutput=false);




        std::vector<int> IntensityRowProfileX;
        std::vector<int> IntensityRowProfileY;
        std::vector<int> RowProfileX;
        std::vector<int> RowProfileY;
        std::vector<BandBoundary> BandBoundariesX;
        std::vector<BandBoundary> BandBoundariesY;


    private:

};

#endif // ANPRIMAGE_H
