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
 //   std::vector< ConvolutionMatrix > MyFilters;
  //  MyFilters.push_back(WhiteFill);
   // ImageConvolutionMatrixTransformation(MyFilters);

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
    std::map<int,int> BlobPixelCount;
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
                UniqueLabels.insert(Label);             // Add to unique list

                // Create or increment the Label Count
                if(BlobPixelCount.find(Label) == BlobPixelCount.end()){
                    BlobPixelCount[Label] = 1;
                }else{
                    BlobPixelCount[Label]++;
                }


            }
        }
    }

    std::vector<std::pair<int,int>> LabelCount(BlobPixelCount.begin(), BlobPixelCount.end());

    // Order by the BlobPixel Occurances
    std::sort(LabelCount.begin(), LabelCount.end(), [](const std::pair<int,int> &element1, const std::pair<int,int> &element2) {
                                                        return element1.second > element2.second;
                                                    });

    // Limit this to the top 15 labels
    LabelCount.erase(LabelCount.begin() + 15, LabelCount.end());

    std::vector<ROI> BlobRegions;
    for(int i=0; i < LabelCount.size(); i++){

        if(LabelCount[i].second > 30){
         std::cout << "Found: Label " << LabelCount[i].first << " " << LabelCount[i].second << std::endl;
        }
        // Set variables for this region
        int xMax = 0;
        int xMin = Image->w;
        int yMax = 0;
        int yMin = Image->h;
        int PixelCount = 0;

        // Find pixels with this label in PixelToLabelMap and set them to green
        for(int j=0; j < PixelToLabelMap.size(); j++){



            if(PixelToLabelMap[j] == LabelCount[i].first){
                // Pixel is part of this label
                PixelCount++;
                int y = j/(Image->w);
                int x = j-(y*Image->w);
                // We can compute the region by magic
                if(x < xMin){
                    xMin = x;
                }
                if(x > xMax){
                    xMax = x;
                }
                if(y < yMin){
                    yMin = y;
                }
                if(y > yMax){
                    yMax = y;
                }
                SetRGBValues(Image, x, y, 255, 0, 0);
            }

        }

        // Now we have boundaries
        //   xmin   xmax
        //   ymin   ymax
        ROI BlobRegion;
        BlobRegion.Rect.x = xMin;
        BlobRegion.Rect.y = yMin;
        BlobRegion.Rect.Height = yMax-yMin; // Height
        BlobRegion.Rect.Width = xMax-xMin; // Width
        BlobRegion.Pixels = PixelCount;

        BlobRegions.push_back(BlobRegion);

        // Plot the region as a rect
        DrawRectangle(BlobRegion.Rect, 0, 255, 0);
    }



    // Do analysis on all found BlobRegions
    std::vector<ROI> CharacterCandidate;
    for(int i=0; i < BlobRegions.size(); i++){
        int h = BlobRegions[i].Rect.Height;
        int w = BlobRegions[i].Rect.Width;
        float d = (float)h/(float)w; // Ratio

        // Ensure height is bigger than width
        if(h < w){
            continue;
        }

        // 2001 Reg | Normal Char | 1 or L
        if( ((1.3 <= d) && (d <= 1.7)) || ((5.2 <= d) && (d <= 5.8)) ){
            // Good candidate
            BlobRegions[i].Score = 1001;
            DrawRectangle(BlobRegions[i].Rect, 255, 255, 0);
            CharacterCandidate.push_back(BlobRegions[i]);
            std::cout << "2001+" << std::endl;
        }


        // Pre 2001
        else if( ((1.5 <= d) && (d <= 1.9)) || ((4.7 <= d) && (d <= 5.1)) ){
            // Good candidate
            BlobRegions[i].Score = 1000;
            DrawRectangle(BlobRegions[i].Rect, 0, 255, 255);
            CharacterCandidate.push_back(BlobRegions[i]);
            std::cout << "Before 2001" << std::endl;
        }

    }
    // Clean up all CCA varables here
    BlobRegions.clear();
    PixelToLabelMap.clear();



    // Lets find a grouping of CharacterCandidates
    // Order the candidates by their x-axis coordinate
    std::sort( CharacterCandidate.begin( ), CharacterCandidate.end( ), [ ]( const ROI& Left, const ROI& Right ){
                                                                                return Left.Rect.x < Right.Rect.x;
                                                                         });

    // Create a set representation for each Charactor Candidate
    DisjointSet CharGroup;
    // More sets than I do in the gym
    // Create the needed number of sets
    for(int i = 0; i < CharacterCandidate.size(); i++){
        CharGroup.MakeSet();
    }

    // For each box, starting with smallest x
    for(int i = 0; i < CharacterCandidate.size(); i++){

        std::cout << CharacterCandidate[i].Rect.x << std::endl;
        // Compute search region
        Point Point1;
        Point Point2;
        Point Point3;
        Point1.x = CharacterCandidate[i].Rect.x + CharacterCandidate[i].Rect.Width + (CharacterCandidate[i].Rect.Height/2); // This gives roughly 20mm right when Height ~80
        Point1.y = CharacterCandidate[i].Rect.y;
        Point2.x = Point1.x;
        Point2.y = CharacterCandidate[i].Rect.y + (CharacterCandidate[i].Rect.Height/2); // Half way
        Point3.x = Point1.x;
        Point3.y = CharacterCandidate[i].Rect.y + CharacterCandidate[i].Rect.Height;
        DrawLine(Point1, Point3, 255, 153, 0);

        // Lets look through all boxes to the right of this one?
            for(int j=i; j < CharacterCandidate.size(); j++){
                Rectangle R = CharacterCandidate[j].Rect;
                // Do any coordiates of this box, fit in the region above? TODO: I want this to work
                if( (Point1.x >= R.x) && (Point1.x <= R.x + R.Width) && (Point1.y >= R.y) && (Point1.y <= R.y + R.Height)
                 || (Point2.x >= R.x) && (Point2.x <= R.x + R.Width) && (Point2.y >= R.y) && (Point2.y <= R.y + R.Height)
                 || (Point3.x >= R.x) && (Point3.x <= R.x + R.Width) && (Point3.y >= R.y) && (Point3.y <= R.y + R.Height) ){
                    // We have a match!
                    CharGroup.Union(i,j);
                    std::cout << "Match" << i << " " << j << std::endl;
                }
            }
    }

    // Now we should have sets of unions whose boxes are close to eachother
    // Find highest occurance of a set
    std::map<int,int> SetVoteCount;


    for(int i = 0; i < CharacterCandidate.size(); i++){
        std::cout << CharGroup.Find(i) << std::endl;

        if(SetVoteCount.find(CharGroup.Find(i)) == SetVoteCount.end()){
            SetVoteCount[CharGroup.Find(i)] = 1;
        }else{
            SetVoteCount[CharGroup.Find(i)]++;
        }

    }


    // Order SetVote keeping the key map
    std::vector<std::pair<int,int>> SetVote(SetVoteCount.begin(), SetVoteCount.end());
    std::sort(SetVote.begin(), SetVote.end(), [](const std::pair<int,int> &element1, const std::pair<int,int> &element2) {
                                                        return element1.second > element2.second;
                                                    });




    // Take the first SetVote Item as the best group
    int BestGroup = 0; // If this isn't set using the if statement, it won't matter anyway
    if(SetVote.size() > 0){
        BestGroup = SetVote[0].first;
        std::cout << "Best:" << BestGroup << std::endl;
    }

    // If the candidate == BestGroup, lets make it a new colour
    ROI FinalCandidates;
    for(int i = 0; i < CharacterCandidate.size(); i++){
        if(CharGroup.Find(i) == BestGroup){
            DrawRectangle(CharacterCandidate[i].Rect, 255, 0, 102);
            FinalCandidates.push_back(CharacterCandidate[i]);
        }
    }


/*
    // Create average of pixels in the CharacterCandidates - CharacterCandidate[i].Pixels
    float AverageYValue = accumulate(CharacterCandidate.begin(), CharacterCandidate.end(), 0.0,
                                        [](int Sum, const ROI& Element) { return Sum + Element.Rect.y; }) / CharacterCandidate.size();


    // Calculate the STD DEV
    float AvgDiff = 0.0; // SUM:(Wi - Wavg)^2
    for(int i = 0; i < CharacterCandidate.size(); i++){
        AvgDiff += (CharacterCandidate[i].Rect.y - AverageYValue)*(CharacterCandidate[i].Rect.y - AverageYValue); // (Wi - Wavg)^2
    }

    float StdDev = ceil(sqrt(AvgDiff/CharacterCandidate.size()));

    std::cout << "STDev: " << StdDev << std::endl;

    // Lets go through the candidate region for further analysis
    for(int i=0; i < CharacterCandidate.size(); i++){
        int h = CharacterCandidate[i].Rect.Height;
        int w = CharacterCandidate[i].Rect.Width;

        // Discard if this Candidate is has less pixels than the average
        if((CharacterCandidate[i].Rect.y <= AverageYValue+StdDev) && (AverageYValue-StdDev <= CharacterCandidate[i].Rect.y)){
              DrawRectangle(CharacterCandidate[i].Rect, 255, 255, 0);
          //  CharacterCandidate.erase(CharacterCandidate.begin()+i);
          //  i--;
        }

    }*/



    CreateWindowFlags("Plate Candidate CCA", Image->w, Image->h, 0);
    DisplaySurfaceUntilClose(Image);
    CloseWindow();




/*
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
*/
}



