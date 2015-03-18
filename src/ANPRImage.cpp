#include "ANPRImage.h"


ANPRImage::ANPRImage(){

}

/*
    This function takes the raw input image, and will initiate other processes before returning the number plate.
    1) Convert Image for detection
    2) Detect wide regions of the image
*/
void ANPRImage::ProcessGlobalImage(int MinimumBandHeight){
    // Convert the image to Grayscale
    ImageToGrayscale();

    // Preprocess the Image using BitmapImage class editing functions
    NormaliseImage();
    HistogramEqualisation();
    CannyEdgeDetection();

    // Preform Horizontal Band Detection using HoughPoints
    // Create ROI of whole image
    ROI StartingRegion;
    StartingRegion.Rect.x = 0;
    StartingRegion.Rect.y = 0;
    StartingRegion.Rect.Height = Image->h;
    StartingRegion.Rect.Width  = Image->w;
    std::vector<ROI> Regions = HoughHorizontalBandDetection(StartingRegion, 0.5, MinimumBandHeight, 20);  // Find good horizontal band candidates

  // std::vector<ROI> Regions;
   // Regions.push_back(StartingRegion);



    // Debug message
   //   DrawRectangle(Regions[0].Rect, 255,0,0);
    std::cout << " >> Global Image Banding: Found " << Regions.size() << " regions for analysis" << std::endl;

    // Compute a score for each ROI
    if(Regions.size() > 0){

        // Reset the image and preform a RowProfile
        RestoreToLoadedImage();
        ImageToGrayscale();

        // Horizontal rank to improve row profile
        std::vector< ConvolutionMatrix > MyFilters;
        MyFilters.push_back(HorizontalRank);
        ImageConvolutionMatrixTransformation(MyFilters);
        CreateRowProfile();    // Create vertical projection

        // For each large region, split into vertical projection snapshot regions
        for(int i = 0; i < Regions.size(); i++){
            RestoreToLoadedImage();
            DrawRectangle(Regions[i].Rect, 255, 255, 255);
            DebugDisplayImageToWindow("Horizontal Banded Region");

            std::cout << "  >> Detecting plates within:  y0: " << Regions[i].Rect.y << " -> " << Regions[i].Rect.y+Regions[i].Rect.Height << std::endl;
            // Get list of Peaks from row profile analysis
            std::vector<ProjectionAnalysis> PeakRegions = RefineDetectedRegion(Regions[i]);
            // Check for a plate within each region
            for(int i=0; i < PeakRegions.size(); i++){
                std::cout << "   >> Refined Band Candidate " << i << " Score: " << PeakRegions[i].Score << std::endl;
                RefinePotentialPlateBand(PeakRegions[i]);
            }





        }
    }

    // Save the image for testing
    SaveImageToFile("output.bmp");

}



std::vector<ROI> ANPRImage::HoughHorizontalBandDetection(ROI InputRegion, float HoughThreshold,  int MinimumBandHeight, int PixelChunking){

    // Preform initial thresholding
    std::vector<HoughPoint> HoughPoints = LocateHoughPoints(HoughThreshold); // Locate hough points

    // Horizontal Banding
    std::vector<int> HorizontalBandVoting;
    HorizontalBandVoting.resize((int)ceil(Image->h/PixelChunking)+1, 0); // Devide whole image into chunks of PixelChunking pixels for voting, regardless of ROI

    // Merge all similar lines (within PixelChunking y pixels) into Horizontal Bands in HorizontalBandVoting
    float yAverage;
    bool HorizontalBandPresent = false;
    for(int i = 0; i < HoughPoints.size(); i++){
        if((HoughPoints[i].t > 86) && (HoughPoints[i].t < 95)){         // Consider line to be horizontal

            // Calculate line equations as required
            Point Point1, Point2;
            ComputeHoughPointLine(HoughPoints[i], Point1, Point2);

            // Calculate average y for horizontal banding
            yAverage = (Point1.y + Point2.y)/2;
           // DrawLine(0, yAverage, Image->w, yAverage, 0, 255, 0);
            // Is the yAverage in the boundaries we desire?
            if((yAverage >= InputRegion.Rect.y) && (yAverage <= InputRegion.Rect.y+InputRegion.Rect.Height)){
                // Add vote
                HorizontalBandVoting[(int)yAverage/PixelChunking]++;        // Add a vote to line group
            }
        }
    }

    // Define Horizontal Bands for further processing
    std::vector<ROI> Regions;
    ROI  Region;
    int TopLine = -1, BottomLine = -1, Height; // Line position in pixels
    int MaxLine = (InputRegion.Rect.y+InputRegion.Rect.Height)/PixelChunking;
    int MaxVote = *std::max_element(HorizontalBandVoting.begin(), HorizontalBandVoting.end());
    for(int i = InputRegion.Rect.y/PixelChunking; i < MaxLine; i++){
        // If line is < 0.7 x MaxVote, let's skip it
       /* if(HorizontalBandVoting[i]  == 1){
            continue;
        }*/
      //  DrawLine(0, i*PixelChunking, HorizontalBandVoting[i], i*PixelChunking, 0, 0, 255);
        // Find a line which we shall consider to be our top horizontal line, otherwise, a bottom line
        if(TopLine < 0 && HorizontalBandVoting[i] != 0){
            TopLine = i * PixelChunking;
        }
        else if((BottomLine < 0 && HorizontalBandVoting[i] != 0) || (BottomLine < 0 && i == MaxLine-1)){
            BottomLine = (i * PixelChunking)+PixelChunking;   // We have found another line or reached the end, go to beginning of next boundary
        }

        // We have a region to consider
        if(TopLine >= 0 && BottomLine > 0){
            // If the band is big enough, consider it
            Height = BottomLine-TopLine;
            if((Region.Rect.y + Height) > Image->h){    // Make sure we don't go off the bottom of the image
                Height = Image->h-TopLine;
            }
            if(Height > MinimumBandHeight){
                Region.Rect.x = 0;
                Region.Rect.y = TopLine;
                Region.Rect.Height = Height;
                Region.Rect.Width = Image->w;
                Regions.push_back(Region);
            }
            TopLine = BottomLine;   // Define new TopLine
            BottomLine = -1;        // Undefine BottomLine
        }
    }

// Return the regions we have found
return Regions;

}

void ANPRImage::RefinePotentialPlateBand(ProjectionAnalysis PA){

        // Create a region for refinement
        ROI TempRegion;
        TempRegion.Rect.x = 0;
        TempRegion.Rect.y = PA.Lower.Coordinate;
        TempRegion.Rect.Width = Image->w;
        TempRegion.Rect.Height = PA.Upper.Coordinate - PA.Lower.Coordinate;

        // Refine estimates
        std::vector<ProjectionAnalysis> PeakRegionsRefined = AnalyseAxisPeakProfile(RowProfile, TempRegion, 'Y', 1, 0.6);

        // Define rough plate region (adding padding)
        Rectangle PlateRect;
        PlateRect.x      = 0;
        PlateRect.y      = PeakRegionsRefined[0].Lower.Coordinate-10;
        PlateRect.Width  = Image->w;
        PlateRect.Height = (PeakRegionsRefined[0].Upper.Coordinate - PeakRegionsRefined[0].Lower.Coordinate)+20;

        // Cleanup potential over/under flows
        if(PlateRect.y + PlateRect.Height > Image->h) {
            PlateRect.Height = Image->h - PlateRect.y; // Make Height the max possible
        }
        if(PlateRect.y < 0){
            PlateRect.y = PeakRegionsRefined[0].Lower.Coordinate;
        }

        // Lets see if this plate candidate is viable
        PlateCandidate Plate;
        Plate.LoadBitmapImage(LoadedImage, PlateRect); // Create plate instance of defined region
        Plate.AnalysePlate();

}

std::vector<ProjectionAnalysis> ANPRImage::RefineDetectedRegion(ROI & Region, int EdgeThreshold, int HeightThreshold){
    // Detect region with high peaks (hopefully the numberplate)
    std::vector<ProjectionAnalysis> PeakRegions = AnalyseAxisPeakProfile(RowProfile, Region, 'Y', 2, 0.55);

    // Drop regions below HeightThreshold
    for(int i = 0; i < PeakRegions.size(); i++){
        if(PeakRegions[i].Height() < HeightThreshold){
            std::cout << "    >> Droping refined region with height of :" << PeakRegions[i].Height() << std::endl;
            PeakRegions.erase(PeakRegions.begin()+i);
            i--;
        }
    }

    for(int i = 0; i < PeakRegions.size(); i++){
        // Lets score
        ProjectionAnalysis &PA = PeakRegions[i]; // Temp Pointer
        PA.Score += 0.15 * abs(PA.Lower.Coordinate - PA.Upper.Coordinate);  // Smallest Height
        PA.Score += 0.25 * PA.Middle.Value;                                 // Peak Value
        PA.Score += 0.40 * PA.Area;                                         // Area bounded by Lower -> Upper
    }

    // Order by Score
    std::sort(PeakRegions.begin(), PeakRegions.end(), [](const ProjectionAnalysis &a, const ProjectionAnalysis &b){
                                                            return a.Score > b.Score; // Requires C++11
                                                        });                           // Makes high scores first

    // Now we have an ordered list of vector<ProjectionAnalysis>
    return PeakRegions;
}



















std::vector<ROI> ANPRImage::ExtractPotentialPlateRegion(ROI Region){

    // Locate all horizontal lines in this region
    std::vector<HoughPoint> HoughPoints = LocateHoughPoints(0.7); // Locate hough points
    std::vector<int> HorizontalLines;
    std::vector<int> VerticalLines;

    Point Point1, Point2;
    int Average = 0;
    for(int i = 0; i < HoughPoints.size(); i++){
        // Get Horizontal & Vertical lines in defined region
        if((HoughPoints[i].t >= 89) && (HoughPoints[i].t <= 91)){         // Consider line to be horizontal
            // Calculate line equations as required
            ComputeHoughPointLine(HoughPoints[i], Point1, Point2);
            // Calculate average y-value
            Average = (Point1.y+Point2.y)/2;
            // Put into array, if in defined region
            if((Average >= Region.Rect.y) && (Average <= Region.Rect.y+Region.Rect.Height)){
                HorizontalLines.push_back(Average);
                DrawLine(Point1.x, Point1.y, Point2.x, Point2.y, 255, 0, 0);
            }

        }else if((HoughPoints[i].t >= 0) && (HoughPoints[i].t <= 2)){           // Consider line to be vertical
            // Calculate line equations as required
            ComputeHoughPointLine(HoughPoints[i], Point1, Point2);
            // Calculate average x-value
            Average = (Point1.x+Point2.x)/2;
            // Put into array, if in defined region
            if((Average >= Region.Rect.x) && (Average <= Region.Rect.x+Region.Rect.Width)){
                VerticalLines.push_back(Average);
              //  DrawLine(Point1.x, Point1.y, Point2.x, Point2.y, 0, 255, 0);
            }
        }
    }

    // Remove duplicate lines caused by averaging
    std::unique(HorizontalLines.begin(), HorizontalLines.end());
    std::unique(VerticalLines.begin(), VerticalLines.end());

    // Order the lines
    std::sort(HorizontalLines.begin(), HorizontalLines.end());
    std::sort(VerticalLines.begin(), VerticalLines.end());

    float XDiff, YDiff, Ratio;
    std::vector<ROI> PlateRegions;
 /*   ROI TempRegion;
    for(int Hs = 0; Hs < HorizontalLines.size()-1; Hs++){
        for(int He = Hs; He < HorizontalLines.size(); He++){
            for(int Vs = 0; Vs < VerticalLines.size()-1; Vs++){
                for(int Ve = Vs; Ve < VerticalLines.size(); Ve++){
                    // Compute ration
                    XDiff = abs(HorizontalLines[He]-HorizontalLines[Hs]);
                    YDiff = abs(VerticalLines[Ve]-VerticalLines[Vs]);
                    Ratio = abs(XDiff/YDiff);
                    // Valid Ratio?
                    if((Ratio >= 4.5) && (Ratio <= 5.5)){
                        // Create ROI
                        TempRegion.Rect.y = Hs;
                        TempRegion.Rect.Height = XDiff;
                        TempRegion.Rect.x = Vs;
                        TempRegion.Rect.Width = YDiff;
                        // Add to PlateRegions
                        PlateRegions.push_back(TempRegion);
                    }
                }
            }
        }
    }


*/
    return PlateRegions;


}

/*
    // Lets output to the user using SDL_Surfaces
    CreateWindowFlags("Hough Line Banding", Image->w, Image->h, 0);
    DisplaySurfaceUntilClose(Image);
    CloseWindow();
*/

/*
        // We need to give each of these internal regions a score
        for(int i = 0; i < Regions.size(); i++){
            std::cout << "  >> Detecting plates within:  y0: " << Regions[i].Rect.y << " -> " << Regions[i].Rect.y+Regions[i].Rect.Height << std::endl;
            // Get list of Peaks from row profile analysis
            std::vector<ProjectionAnalysis> PeakRegions = RefineDetectedRegion(Regions[i]);
            // Check for a plate within each region
            for(int i=0; i < PeakRegions.size(); i++){
                std::cout << "   >> Refined Band Candidate " << i << " Score: " << PeakRegions[i].Score << std::endl;
                RefinePotentialPlateBand(PeakRegions[i]);
            }
        } */

/*
           std::vector<ProjectionAnalysis> PeakRegions = AnalyseAxisPeakProfile(RowProfile, Regions[i], 'Y', 4, 0.55);
           for(int i = 0; i < PeakRegions.size(); i++){
                // Find rectangles in the region
                //std::vector<ROI> RectPlate = ExtractPotentialPlateRegion(Regions[i]);
                ROI SearchRegion;
                SearchRegion.Rect.x = 0;
                SearchRegion.Rect.Width = Image->w;
                SearchRegion.Rect.y = PeakRegions[i].Lower.Coordinate;
                SearchRegion.Rect.Height = PeakRegions[i].Upper.Coordinate - PeakRegions[i].Lower.Coordinate;

                std::vector<ProjectionAnalysis> PeakRegions2 = AnalyseAxisPeakProfile(RowProfile, SearchRegion, 'Y', 1, 0.55);
                CreateWindowFlags("Hough Line Banding", Image->w, Image->h, 0);
                DisplaySurfaceUntilClose(Image);
                CloseWindow();

            }*/
