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

    protected:


    private:


};

#endif // PLATECANDIDATE_H
