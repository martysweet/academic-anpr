#include "ANPRImage.h"


ANPRImage::ANPRImage(){

}

void ANPRImage::ProcessGlobalImage(){
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
    std::vector<ROI> Regions = HoughHorizontalBandDetection(StartingRegion, 0.5, 150);  // Find good horizontal band candidates

    // Debug message
    std::cout << " >> Step 1: Found " << Regions.size() << " regions for analysis" << std::endl;
    for(int i = 0; i < Regions.size(); i++){
            std::cout << "  >> y0: " << Regions[i].Rect.y << " -> " << Regions[i].Rect.y+Regions[i].Rect.Height << std::endl;
           /* std::vector<ROI> Regions2 = HoughHorizontalBandDetection(Regions[i], 0.4, 70);
            std::cout << "   >> Recurse: Found " << Regions2.size() << " regions for analysis" << std::endl;
            DrawLine(10, Regions[i].Rect.y, 10, Regions[i].Rect.y+Regions[i].Rect.Height, 255, 0, 255);
            for(int k = 0; k < Regions2.size(); k++){
                std::cout << "    >> y0: " << Regions2[i].Rect.y << " -> " << Regions2[i].Rect.y+Regions2[i].Rect.Height << std::endl;
                DrawLine(20+k, Regions2[i].Rect.y, 20+k, Regions2[i].Rect.y+Regions2[i].Rect.Height, 255, 0, 0);
            }*/
    }


    // Compute a score for each ROI
    if(Regions.size() > 0){
        // Reset the image and preform a RowProfile
        RestoreToLoadedImage();
        ImageToGrayscale();
        std::vector< ConvolutionMatrix > MyFilters;
        MyFilters.push_back(HorizontalRank);
        ImageConvolutionMatrixTransformation(MyFilters);
        CreateRowProfile();    // Create y-axis edge profile for whole image
        for(int i = 0; i < Regions.size(); i++){
            ScoreDetectedRegion(Regions[i]);
        }
    }

    // Save the image for testing
    SaveImageToFile("output.bmp");

}

void ANPRImage::ScoreDetectedRegion(ROI & Region, int EdgeThreshold, int HeightThreshold){
    // Detect region with high peaks (hopefully the numberplate)
    std::vector<ProjectionAnalysis> PeakRegions = AnalyseAxisPeakProfile(RowProfile, Region, 'Y', 2, 0.55);

    for(int i = 0; i < PeakRegions.size(); i++){
        // Lets score
        ProjectionAnalysis &PA = PeakRegions[i]; // Temp Pointer
        PA.Score += 0.15 * abs(PA.Lower.Coordinate - PA.Upper.Coordinate);  // Smallest Height
        PA.Score += 0.25 * PA.Middle.Value;                                 // Peak Value
        PA.Score += 0.40 * PA.Area;                                         // Area bounded by Lower -> Upper
        std::cout << "   >> Candidate Score: " << PA.Middle.Coordinate << " " << PA.Area << " - Score: " << PA.Score << std::endl;
    }

    // Order by Score
    std::sort(PeakRegions.begin(), PeakRegions.end(), [](const ProjectionAnalysis &a, const ProjectionAnalysis &b){
                                                            return a.Score > b.Score; // Requires C++11
                                                        });                           // Makes high scores first


    for(int i = 0; i < 1; i++){
        ProjectionAnalysis &PA = PeakRegions[i]; // Temp Pointer

        ROI TempRegion;
        TempRegion.Rect.x = 0;
        TempRegion.Rect.y = PA.Lower.Coordinate;
        TempRegion.Rect.Width = Image->w;
        TempRegion.Rect.Height = PA.Upper.Coordinate - PA.Lower.Coordinate;

        // Refine estimates
        std::vector<ProjectionAnalysis> PeakRegionsRefined = AnalyseAxisPeakProfile(RowProfile, TempRegion, 'Y', 1, 0.6);

        // Define rough plate region
        Rectangle PlateRect;
        PlateRect.x      = 0;
        PlateRect.y      = PeakRegionsRefined[0].Lower.Coordinate-5;
        PlateRect.Width  = Image->w;
        PlateRect.Height = (PeakRegionsRefined[0].Upper.Coordinate - PeakRegionsRefined[0].Lower.Coordinate)+10;

        // Lets see if this plate candidate is viable
        RestoreToLoadedImage();
        PlateCandidate Plate;
        Plate.LoadBitmapImage(Image, PlateRect);
        Plate.AnalysePlate();



        std::cout << "   >> Candidate Score: " << PA.Middle.Coordinate << " " << PA.Area << " - Score: " << PA.Score << std::endl;
    }

}
