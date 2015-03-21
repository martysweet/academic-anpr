#ifndef ANPR_H
#define ANPR_H

#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "DataStructures.h"
#include "PlateCandidate.h"

class ANPRImage: public BitmapImage  {
    public:
        bool ProcessGlobalImage(int TestCases=12);

    private:
        void OutputCharactersToScreen(std::vector<ROI> Regions, ROI WorkingArea, int Spacing = 5, int Buffer = 2);
};

#endif // ANPRIMAGE_H
