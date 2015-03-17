#ifndef OCRANALYSIS_H
#define OCRANALYSIS_H

#include <string>
#include <map>

#include "AnalyseImage.h"


class OCRAnalysis : public AnalyseImage
{
    public:
        /** Default constructor */
        OCRAnalysis();
        /** Default destructor */
        virtual ~OCRAnalysis();
        void Process(int Position);
    protected:
    private:
};

#endif // OCRANALYSIS_H
