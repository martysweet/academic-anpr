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
    std::vector<ROI> Regions = HoughHorizontalBandDetection(StartingRegion, 0.5, MinimumBandHeight);  // Find good horizontal band candidates

    // Debug message
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
        CreateRowProfile();    // Create y-axis edge profile for whole image

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
        }


    }else{
        // We haven't found numberplate

    }



    // Save the image for testing
    SaveImageToFile("output.bmp");

}

std::vector<ProjectionAnalysis> ANPRImage::RefineDetectedRegion(ROI & Region, int EdgeThreshold, int HeightThreshold){
    // Detect region with high peaks (hopefully the numberplate)
    std::vector<ProjectionAnalysis> PeakRegions = AnalyseAxisPeakProfile(RowProfile, Region, 'Y', 2, 0.55);

    // Drop regions below HeightThreshold
    for(int i = 0; i < PeakRegions.size(); i++){
        if(PeakRegions[i].Height() < HeightThreshold){
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

/*
    // Lets output to the user using SDL_Surfaces
    CreateWindowFlags("Hough Line Banding", Image->w, Image->h, 0);
    DisplaySurfaceUntilClose(Image);
    CloseWindow();
    */

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
            PlateRect.Height -= abs(Image->h - (PlateRect.y + PlateRect.Height)); // Take away the exact amount over it is
        }
        if(PlateRect.y < 0){
            PlateRect.y = PeakRegionsRefined[0].Lower.Coordinate;
        }

        // Lets see if this plate candidate is viable
      /*  RestoreToLoadedImage();
        PlateCandidate Plate;
        Plate.LoadBitmapImage(Image, PlateRect);
        Plate.AnalysePlate();*/

}

void ANPRImage::ExtractPotentialPlateRegion(ProjectionAnalysis PA){




}

