#ifndef PLATECANDIDATE_H
#define PLATECANDIDATE_H

#include <vector>
#include <cmath>
#include "DataStructures.h"
#include "DisjointSet.h"
#include <set>

#include "AnalyseImage.h"
#include "OCRAnalysis.h"
class PlateCandidate :  public AnalyseImage {
    public:
        PlateCandidate();
        virtual ~PlateCandidate();
        void AnalysePlate();
        ROI GetPlateRegion();
        std::vector<ROI> GetPlateCharacters();
        std::vector<ROI> CCA();

    protected:


    private:
        void ROIFix(ROI &Region);


};

#endif // PLATECANDIDATE_H
