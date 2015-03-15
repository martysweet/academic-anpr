#include "PlateCandidate.h"

PlateCandidate::PlateCandidate(){
    //ctor
}

PlateCandidate::~PlateCandidate(){
    //dtor
}

void PlateCandidate::AnalysePlate(){

    // Lets output to the user using SDL_Surfaces
    CreateWindowFlags("Plate Candidate NEW LOAD", Image->w, Image->h, 0);
    DisplaySurfaceUntilClose(Image);
    CloseWindow();

    NormaliseImage();
    HistogramEqualisation();


    // This threshold might to totalty out, but we can increment 10 times if we don't get a good intensity profile
    // We should be doing adaptive thresholding here!
    ImageToAdaptiveMonochrome(15, true);


    // Use Blob Grouping and Connected Component Analysis

    // Lets fix the charactors if they are slightly over-exposed.
    // We will do this by using a convolution matrix, if the pixel is neighboured by 4? Pixels consider it White
    std::vector< ConvolutionMatrix > MyFilters;
    MyFilters.push_back(WhiteFill);
    ImageConvolutionMatrixTransformation(MyFilters);

    // We need to group objects together in the image
    // http://en.wikipedia.org/wiki/Connected-component_labeling
    // Initialise Variables
    std::vector<int> PixelToLabelMap;
    PixelToLabelMap.resize(Image->w*Image->h, -1); // Initialise the array to -1
    DisjointSet MappingSet; // Create new disjoint set structure
    int PixelValue = 0;
    int PixelIdent = 0;

    // Update the grayscale map array (which also has out of bounds error handling)
    CreateGrayscaleMapArray();
    // For each pixel
    for(int y = 0; y < Image->h; y++){
        for(int x = 0; x < Image->w; x++){
            // Pixel dependant variables
            PixelValue = GetGrayscaleMapValue(x, y);
            PixelIdent = (Image->w*y)+x; // Position of pixel in PixelToLabelMap

            if(PixelValue == 0){
                continue; // Skip pixel if its background
            }

            // Find Neighbours ( NEIGH THE HOUSE BELLOWED ACROSS THE FIELD)
            if(GetGrayscaleMapValue((x-1), y) == PixelValue){ // West Check
                // Assign the label from the pixel we checked
                 PixelToLabelMap[PixelIdent] = PixelToLabelMap[PixelIdent-1]; // West (To the Left)
            }
            if(GetGrayscaleMapValue(x, y-1) == PixelValue && GetGrayscaleMapValue((x-1), y) == PixelValue && PixelToLabelMap[PixelIdent-1] != PixelToLabelMap[PixelIdent-Image->w]){
                // North & West in same region, should be merged. Assign minimum of the labels and record equivilence
                int LabelN = PixelToLabelMap[PixelIdent-Image->w];  // North (Directly above current pixel)
                int LabelW = PixelToLabelMap[PixelIdent-1];         // West
                int Label  = MappingSet.Union(LabelN, LabelW);      // Merges sets and returns lowest
                PixelToLabelMap[PixelIdent] = Label;                // Assign new set to PixelToLabel Map
            }
            if(GetGrayscaleMapValue((x-1), y) != PixelValue && GetGrayscaleMapValue(x, y-1) == PixelValue){
                // Assign North label to Pixel
                PixelToLabelMap[PixelIdent] = PixelToLabelMap[PixelIdent-Image->w]; // North (Directly Above)

            }
            if(GetGrayscaleMapValue((x-1), y) != PixelValue && GetGrayscaleMapValue(x, y-1) != PixelValue){
                int Label = MappingSet.MakeSet();       // Create new set - Scope only in this IF statement
                PixelToLabelMap[PixelIdent] = Label;    // Assign new set to PixelToLabel Map
            }
        }
    }

    // Merge temporary assignments
    std::set<int> UniqueLabels; // Set used for unique assignment
    std::set< std::pair<int,int> > BlobPixelCount;
    for(int y = 0; y < Image->h; y++){
        for(int x = 0; x < Image->w; x++){
            // Pixel dependant variables
            PixelValue = GetGrayscaleMapValue(x, y);
            PixelIdent = (Image->w*y)+x; // Position of pixel in PixelToLabelMap

            if(PixelValue == 0){
                continue; // Skip pixel if its background
            }else{
                int Label = MappingSet.Find(PixelToLabelMap[PixelIdent]); // Find Parent Set
                PixelToLabelMap[PixelIdent] = Label;    // Set to parent Set
                std::pair<int,int> Pair;
                Pair.second = Label; // Unsorted label value
              //  if(UniqueLabels[])
                // TODO: Work out best storage method

                UniqueLabels.insert();             // Add to unique list

            }
        }
    }


    // Order by the BlobPixel Occurances
    std::sort(BlobPixelCount.begin(), BlobPixelCount.end());

    // Create a counting matrix for each set

    for(std::set<int>::iterator it=UniqueLabels.begin(); it != UniqueLabels.end(); it++){

        // Count the amount of occurances in PixelInLabelMap
        if(BlobPixelCount[*it] > 30){
            std::cout << BlobPixelCount[*it] << std::endl;
        }

    }





    CreateWindowFlags("Plate Candidate CCA", Image->w, Image->h, 0);
    DisplaySurfaceUntilClose(Image);
    CloseWindow();





    // Intensity Segmentation

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
    MyBox.Width = PossibleChars[i].Length+(StdDev*2); // StdDev*2 applies both left and right margins
    MyBox.Height = Image->h - 2;

    DrawRectangle(MyBox, 0, 0, 255);

    }

     // Lets output to the user using SDL_Surfaces
    CreateWindowFlags("Plate Candidate Intensity", Image->w, Image->h, 0);
    DisplaySurfaceUntilClose(Image);
    CloseWindow();

   // ImageToAdaptiveMonochrome();
    SaveImageToFile("platecand.bmp");

}



