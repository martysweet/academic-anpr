#include "PlateCandidate.h"

PlateCandidate::PlateCandidate(){
    //ctor
}

PlateCandidate::~PlateCandidate(){
    //dtor
}

void PlateCandidate::AnalysePlate(){

    // Lets output to the user using SDL_Surfaces
    CreateWindowFlags("Plate Candidate Original", Image->w, Image->h, 0);
    DisplaySurfaceUntilClose(Image);
    CloseWindow();

    NormaliseImage();
    HistogramEqualisation();


    std::vector< ConvolutionMatrix > MyFilters;
    MyFilters.push_back(Median);
    ImageConvolutionMatrixTransformation(MyFilters);

    CannyEdgeDetection();

    // This threshold might to totalty out, but we can increment 10 times if we don't get a good intensity profile
    // We should be doing adaptive thresholding here!
   // ImageToBW(40, true);
   // ImageToAdaptiveMonochrome(15, true);
   // NormaliseImage();
   // ImageToAdaptiveMonochrome(15, false);
   // HistogramEqualisation();


    CreateIntensityColumnProfile();

    ROI WholeImage;
    WholeImage.Rect.x = 0;
    WholeImage.Rect.y = 0;
    WholeImage.Rect.Height = Image->h;
    WholeImage.Rect.Width  = Image->w;
    std::vector<AverageGrouping> PossibleChars = AnalyseAxisAverageProfile(ColumnIntensityProfile, WholeImage, 'X', 9, 0.15);

    // Now we should have a list of possible chars (less than 9)
    // Lets calculate the average of these
    float AverageCharWidth = accumulate(PossibleChars.begin(), PossibleChars.end(), 0.0,
                                        [](int sum, const AverageGrouping& element) { return sum + element.Length; }) / PossibleChars.size();
    AverageCharWidth = ceil(AverageCharWidth); // Round up

    std::cout << "Average Width after limiting: " << AverageCharWidth << std::endl;

    // Calculate the STD DEV
    float AvgDiff = 0.0; // SUM:(Wi - Wavg)^2
    for(int i = 0; i < PossibleChars.size(); i++){
        AvgDiff += (PossibleChars[i].Length - AverageCharWidth)*(PossibleChars[i].Length - AverageCharWidth); // (Wi - Wavg)^2
    }

    float StdDev = ceil(sqrt(AvgDiff/PossibleChars.size()));

    std::cout << "Average STDDEV: " << StdDev << std::endl;
    if(StdDev > 10 || PossibleChars.size() < 4){
        std::cout << "!!! PLATE PROBABLY INVALID !!!" << std::endl;
    }else{
        std::cout << "*** PLATE LOOKS PROMISING ***" << std::endl;
    }

    // Draw a box
    for(int i = 0; i < PossibleChars.size(); i++){

    Rectangle MyBox;
    MyBox.y = 1;
    MyBox.x = PossibleChars[i].Start-StdDev;
    MyBox.Width = PossibleChars[i].Length+StdDev*2; // StdDev*2 applies both left and right margins
    MyBox.Height = Image->h - 2;

    DrawRectangle(MyBox, 0, 0, 255);

    }

     // Lets output to the user using SDL_Surfaces
    CreateWindowFlags("Plate Candidate", Image->w, Image->h, 0);
    DisplaySurfaceUntilClose(Image);
    CloseWindow();

   // ImageToAdaptiveMonochrome();
    SaveImageToFile("platecand.bmp");

}



