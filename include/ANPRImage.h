#ifndef ANPR_H
#define ANPR_H

#include <vector>
#include <sstream>
#include <algorithm>

#include "../src/DataStructures.cpp"

#include "AnalyseImage.h"
#include "PlateCandidate.h"
//#include "OCRAnalysis.h"
class ANPRImage: public AnalyseImage  {
    public:
        ANPRImage();
        /* Callable Functions */
        void ProcessGlobalImage(int MinimumBandHeight);


    protected:

        /* Plate Detection Functions */
        std::vector<ProjectionAnalysis> RefineDetectedRegion(ROI &Region, int EdgeThreshold = 10, int HeightThreshold = 70);


    private:
        std::vector<ROI> ExtractPotentialPlateRegion(ROI Region);
        std::vector<ROI> HoughHorizontalBandDetection(ROI InputRegion, float HoughThreshold = 0.5,  int MinimumBandHeight = 10, int PixelChunking = 5);
        void RefinePotentialPlateBand(ProjectionAnalysis PA);

};

#endif // ANPRIMAGE_H
