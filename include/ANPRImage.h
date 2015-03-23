#ifndef ANPR_H
#define ANPR_H

#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <numeric>

#include "DataStructures.h"
#include "PlateCandidate.h"

class ANPRImage: public BitmapImage  {
    public:
        ANPRImage(std::string FileLocation);
        bool ProcessGlobalImage();

    private:
        void MergePlateRegions(std::vector<ROI> Regions, ROI WorkingArea);
        std::string OutputPath;
};

#endif // ANPRIMAGE_H
