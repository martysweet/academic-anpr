#ifndef ANPR_H
#define ANPR_H

#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "DataStructures.h"

#include "AnalyseImage.h"
#include "PlateCandidate.h"
//#include "OCRAnalysis.h"
class ANPRImage: public AnalyseImage  {
    public:
        ANPRImage();
        /* Callable Functions */
        bool ProcessGlobalImage(int TestCases=12);


    protected:

        /* Plate Detection Functions */
        std::vector<ProjectionAnalysis> RefineDetectedRegion(ROI &Region, int EdgeThreshold = 10, int HeightThreshold = 70);


    private:
        std::vector<ROI> ExtractPotentialPlateRegion(ROI Region);
        std::vector<ROI> HoughHorizontalBandDetection(ROI InputRegion, float HoughThreshold = 0.5,  int MinimumBandHeight = 10, int PixelChunking = 5);
        void RefinePotentialPlateBand(ProjectionAnalysis PA);
        void OutputCharactersToScreen(std::vector<ROI> Regions, ROI WorkingArea, int Spacing = 5, int Buffer = 2);
};

#endif // ANPRIMAGE_H
