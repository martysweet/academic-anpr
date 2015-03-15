#ifndef ANALYSEIMAGE_H
#define ANALYSEIMAGE_H


#include <algorithm>
#include <vector>
#include <BitmapImage.h>
class AnalyseImage : public BitmapImage
{
    public:
        /** Default constructor */
        AnalyseImage();
        /** Default destructor */
        virtual ~AnalyseImage();

    protected:
        /* Plate Detection Functions */

        void ComputeHoughPointLine(HoughPoint &HoughPoint, Point &Point1, Point &Point2);
        std::vector<ProjectionAnalysis> AnalyseAxisPeakProfile(std::vector<int> &InputProfile, ROI Region, char Axis = 'Y', int Iterations = 2, float Constant = 0.55);
        std::vector<AverageGrouping> AnalyseAxisAverageProfile(std::vector<int> &InputProfile, ROI Region, char Axis = 'Y', int Iterations = 2, float Constant = 0.55);
        void CreateRowProfile();

        std::vector<int> RowProfile;
        /* Plate Detection Functions */
        std::vector<HoughPoint> LocateHoughPoints(float Threshold = 0.5);
        void CreateIntensityColumnProfile();
        std::vector<int> ColumnIntensityProfile;

    private:
        std::vector<std::vector<int>> HoughVotingMatrix;
        int HoughMaxLineLength;
        int HoughMaxVotingCount;

};

#endif // ANALYSEIMAGE_H
