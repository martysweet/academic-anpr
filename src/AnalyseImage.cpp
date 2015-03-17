#include "AnalyseImage.h"

AnalyseImage::AnalyseImage(){
    //ctor
}

AnalyseImage::~AnalyseImage(){
    //dtor
    HoughVotingMatrix.clear();
    HoughMaxLineLength = 0;
    HoughMaxLineLength = 0;
}

void AnalyseImage::ComputeHoughPointLine(HoughPoint &HoughPoint, Point &Point1, Point &Point2){
    if(HoughPoint.t == 0){
        // Compute vertical lines
        Point1.x = HoughPoint.r;
        Point1.y = 0;
        Point2.x = HoughPoint.r;
        Point2.y = Image->h;
    }else{
        // Compute other lines
        float a, b, x0, y0, m;
        a = cos(HoughPoint.t * DEGTORADIAN);
        b = sin(HoughPoint.t * DEGTORADIAN);
        x0 = a*HoughPoint.r;
        y0 = b*HoughPoint.r;
        m  = sqrt(Image->h*Image->h + Image->w*Image->w);
        // Define Points
        Point1.x = x0 + m*(-b);
        Point1.y = y0 + m*(a);
        Point2.x = x0 - m*(-b);
        Point2.y = y0 - m*(a);
    }
}

void AnalyseImage::CreateRowProfile(){
    // Initialise Variables
    int Pixel, LastPixel;
    RowProfile.resize(Image->h, 0);
    CreateGrayscaleMapArray();      // Ensure Grayscale map exists

    // Loop through the image
    for(int y = 0; y < Image->h; y++){
        for(int x = 0; x < Image->w; x++){
            Pixel = GetGrayscaleMapValue(x, y); // Get current pixel value
            if(Pixel != LastPixel){
                RowProfile[y]++; // We have detected an edge
            }
            LastPixel = Pixel;
        }


        // Score on the image
        for(int x=0; x < RowProfile[y]; x++){
            SetRGBValues(Image, x, y, 255, 0, 0);
        }

    }


}

std::vector<ProjectionAnalysis> AnalyseImage::AnalyseAxisPeakProfile(std::vector<int> &InputProfile, ROI Region, char Axis, int Iterations, float Constant){
    // Generic Axis Profiling
    // Create a new AxisProfile in accordance with the defined region
    std::vector<int> AxisProfile;

    // Default vales for Axis Y
    int a = Region.Rect.y;
    int b = Region.Rect.Height;
    // Redefine for Axis X
    if(Axis == 'X'){
        a = Region.Rect.x;
        b = Region.Rect.Width;
    }
    AxisProfile.assign(InputProfile.begin()+a, InputProfile.begin()+a+b); // coordinate + (width or height) elements

    // Created required variables for profiling
    std::vector<ProjectionAnalysis> Results;
    int cbm, cbmv, cb0, cb0v, cb1, cb1v;
    for(int k = 0; k < Iterations; k++){

        // Find the highest profile value cbm
        cbm = 0;    // Middle Pointer
        cbmv = 0;   // Middle Value
        for(int i = 0; i < AxisProfile.size(); i++){
            if(AxisProfile[i] > cbmv){
                cbm  = i;
                cbmv = AxisProfile[i];
            }
        }

        // If 0, then we have run out of data to test on, break iterations
        if(cbmv == 0){
            break;
        }

        // Find cb0 (Lower Boundary)
        cb0 = 0; // Row ID
        for(int b0 = 0; b0 < cbm; b0++){
            if(AxisProfile[b0] <= Constant*cbmv){
                cb0 = b0;
            }
        }
        cb0v = AxisProfile[cb0]; // Allocate value for later use

        // Find cb1 (Upper Boundary)
        cb1  = cbm; // Row ID
        cb1v = cbmv; // Maximum value in array
        for(int b1 = cbm; b1 < AxisProfile.size(); b1++){
            if(AxisProfile[b1] <= (Constant*cbmv)){
                if(AxisProfile[b1] < cb1v){
                    cb1 = b1;
                    cb1v = AxisProfile[b1];
                }
            }
        }

        // If cb1 is still set to middle, make it the highest available value, otherwise leave
       /* if(cb1 == cbm){
            for(int b1 = AxisProfile.size(); b1 > cbm; b1--){
                if(AxisProfile[b1] != 0){
                    cb1  = b1;
                    cb1v = AxisProfile[b1];
                   // break;
                }
            }
        }*/

        // We have the lower, middle and upper value
        ProjectionAnalysis Result;
        Result.Lower  = {a+cb0, cb0v}; // Coordinate, Value
        Result.Middle = {a+cbm, cbmv};
        Result.Upper  = {a+cb1, cb1v};


        // Zero the range we have found ready for next iteration
        for(int z = cb0; z <= cb1; z++){
            Result.Area += AxisProfile[z];   // Compute area into result
            AxisProfile[z] = 0;
        }


        // Debug (Draw box of boundary)
        Rectangle LocatedArea;
        if(Axis == 'Y'){
            LocatedArea.x       = 1+k*3;
            LocatedArea.y       = Result.Lower.Coordinate;
            LocatedArea.Width   = Region.Rect.Width - (2+k*3);
            LocatedArea.Height  = Result.Upper.Coordinate-Result.Lower.Coordinate;
        }else{ // X-Axis
            LocatedArea.x       = Result.Lower.Coordinate;
            LocatedArea.y       = 1+k*3;
            LocatedArea.Width   = Result.Upper.Coordinate-Result.Lower.Coordinate;
            LocatedArea.Height  = Region.Rect.Height;
        }

        // Draw for debug
        DrawRectangle( LocatedArea, Constant*255, 255, Constant*255 );


        // Store Possible Candidate
        Results.push_back(Result);

    }

    return Results;

}

void AnalyseImage::CreateIntensityColumnProfile(){
    ColumnIntensityProfile.resize(Image->w, 0);
    CreateGrayscaleMapArray();
    for(int x = 0; x < Image->w; x++){
        for(int y = 0; y < Image->h; y++){
            // Count up the pixels which are 255
            if(GetGrayscaleMapValue(x, y) == 255){
                ColumnIntensityProfile[x]++;
            }
        }
        DrawLine(x, 0, x, ColumnIntensityProfile[x], 255, 0, 0);
    }
 }

std::vector<AverageGrouping> AnalyseImage::AnalyseAxisAverageProfile(std::vector<int> &InputProfile, ROI Region, char Axis, int Iterations, float Constant){
    // Generic Axis Profiling
    // Create a new AxisProfile in accordance with the defined region
    std::vector<int> AxisProfile;

    // Default vales for Axis Y
    int a = Region.Rect.y;
    int b = Region.Rect.Height;
    // Redefine for Axis X
    if(Axis == 'X'){
        a = Region.Rect.x;
        b = Region.Rect.Width;
    }
    AxisProfile.assign(InputProfile.begin()+a, InputProfile.begin()+a+b); // coordinate + (width or height) elements

    // Find Average of AxisProfile
    float AxisAverage = accumulate(AxisProfile.begin(), AxisProfile.end(), 0.0) / AxisProfile.size();

    // Nullify values below average
    for(int i = 0; i < AxisProfile.size(); i++){
        if(AxisProfile[i] < AxisAverage){
            // Maybe be more intelligent and check left and right of current values
            // to check if this is just a small blip down
            AxisProfile[i] = 0;
        }
    }

    // Break the dataset into groups of values
    // ex. 0,0,3,5,7,0,3,5,7,4,0,0,0,0,5,4,0,5,7,5
    // ex. {3,5,7}, {3,5,7,4}, {5,4}.. etc
    std::vector<AverageGrouping> AverageGroups;
    AverageGrouping CurrentGroup; // Start, End initialised to -1
    for(int i = 0; i < AxisProfile.size(); i++){

        if(AxisProfile[i] != 0 && CurrentGroup.Start == -1){
            CurrentGroup.Start = i; // We have a new start
        }else
        if(AxisProfile[i] != 0 && CurrentGroup.Start != -1){
            continue; // We are just another element in a lonely world
        }else
        if(AxisProfile[i] == 0 && CurrentGroup.Start != -1){
            CurrentGroup.End = i-1; // We have reached the end of the grouping
            // Add to storage
            CurrentGroup.Length = CurrentGroup.End-CurrentGroup.Start;
            AverageGroups.push_back(CurrentGroup);
            // Reset Variables
            CurrentGroup.Start = -1;
            CurrentGroup.End = -1;
        }

    }

    // Discard all groups with a length less than defined threshold
    for(int i = 0; i < AverageGroups.size(); i++){
        if(AverageGroups[i].Length < 10){
            AverageGroups.erase(AverageGroups.begin()+i);
            i--;
        }
    }

    // If we have more charactors than we should have, lets do the process again
    if(AverageGroups.size() > 9){
        // Find Average of AxisProfile (Keep in this scope though)
        float CurrentAvg = accumulate(AverageGroups.begin(), AverageGroups.end(), 0.0,
                                        [](int sum, const AverageGrouping& element) { return sum + element.Length; });
        CurrentAvg = CurrentAvg / AverageGroups.size();
        // Discard all groups below the calculated average
        for(int i = 0; i < AverageGroups.size(); i++){
            if(AverageGroups[i].Length < CurrentAvg){
                AverageGroups.erase(AverageGroups.begin()+i);
                i--;
            }
        }
    }


    // We now have a list of hopefully 9 groups with a defined length
    for(int i = 0; i < AverageGroups.size(); i++){
            std::cout << "Character Length: " << AverageGroups[i].Length << std::endl;
    }

    return AverageGroups;

}

std::vector<HoughPoint> AnalyseImage::LocateHoughPoints(float Threshold){

    // Variables for this function are declared in class definition.
    if(HoughVotingMatrix.size() == 0){
        CreateGrayscaleMapArray(); // Ensure we have a grayscale image
        int r = 0;

        // Create voting matrix
        HoughMaxLineLength = sqrt(Image->h*Image->h + Image->w*Image->w)+5;
        HoughVotingMatrix.resize( 180  , std::vector<int>( HoughMaxLineLength*2 , 0 ) );

        // Go through image computing angles for each edge
        for(int y = 0; y < Image->h; y++){
            for(int x = 0; x < Image->w; x++){
                if(GetGrayscaleMapValue(x, y) > 250){
                    for(int t = 0; t < 180; t++){
                        r = (x*cos(t * DEGTORADIAN) + y*sin(t * DEGTORADIAN))+HoughMaxLineLength; // So negative HoughMaxLineLength == VM(t,0)
                        HoughVotingMatrix[t][r]++; // Add to accumulator
                        if( HoughVotingMatrix[t][r] > HoughMaxVotingCount ){
                            HoughMaxVotingCount = HoughVotingMatrix[t][r];
                        }
                    }
                }
            }
        }
    }

    // Apply thresholding, the HoughVotingMatrix was created just now or in the last function call
    std::vector<HoughPoint> ThresholdPoints;
    HoughPoint HPoint; // Temp Struct
    for(int t=0; t < 180; t++){
        for(int r=0; r < HoughVotingMatrix[t].size(); r++){
            if((HoughVotingMatrix[t][r] > HoughMaxVotingCount*Threshold)){
                HPoint.t = t;
                HPoint.r = r-HoughMaxLineLength; // Allow negative rhos
                ThresholdPoints.push_back(HPoint);
            }
        }
    }

    // Return vector of Hough Points with applied threshold
    return ThresholdPoints;
}
