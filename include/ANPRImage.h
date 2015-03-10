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
        void ProcessGlobalImage();


    protected:

        /* Plate Detection Functions */
        void ScoreDetectedRegion(ROI &Region, int EdgeThreshold = 10, int HeightThreshold = 20);


    private:


};

#endif // ANPRIMAGE_H
