#ifndef PLATECANDIDATE_H
#define PLATECANDIDATE_H

#include <vector>
#include <cmath>
#include <set>
#include <algorithm>

#include "BitmapImage.h"
#include "DataStructures.h"
#include "DisjointSet.h"

class PlateCandidate :  public BitmapImage {
    public:
        void AnalysePlate();
        ROI GetPlateRegion();
        std::vector<ROI> GetPlateCharacters(int MonochromeRegion = 15);
        std::vector<ROI> CCA();

    protected:

    private:
        void ROIFix(ROI &Region);


};

#endif // PLATECANDIDATE_H
