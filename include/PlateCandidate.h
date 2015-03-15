#ifndef PLATECANDIDATE_H
#define PLATECANDIDATE_H

#include <vector>
#include <cmath>
#include "../src/DataStructures.cpp"
#include <DisjointSet.h>
#include <set>

#include <AnalyseImage.h>
class PlateCandidate :  public AnalyseImage {
    public:
        PlateCandidate();
        virtual ~PlateCandidate();
        void AnalysePlate();

    protected:


    private:


};

#endif // PLATECANDIDATE_H
