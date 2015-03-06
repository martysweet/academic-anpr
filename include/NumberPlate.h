#ifndef NUMBERPLATE_H
#define NUMBERPLATE_H

#include "BitmapImage.h"
#include "ANPRImage.h"


class NumberPlate : public ANPRImage
{
    public:
        NumberPlate();
        virtual ~NumberPlate();
        void ProcessNumberPlate();
    protected:
    private:
        void SegmentCharactors();
        void RefinePlateY();
};

#endif // NUMBERPLATE_H
