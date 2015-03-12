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
        void RefinePotentialPlateBand(ProjectionAnalysis PA);

};

#endif // ANPRIMAGE_H
