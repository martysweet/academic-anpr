#include "PlateCandidate.h"

ROI PlateCandidate::GetPlateRegion(){

    // Preprocess the plate
    HistogramEqualisation();
    NormaliseImage();
    ImageToAdaptiveMonochrome(15, true);

    // Variables
    ROI Region;

    // Get possible charactor regions, ordered by x
    std::vector<ROI> PlateCandidates = CCA();

    // Get a rough region to work with
    if(PlateCandidates.size() > 0){
        Region.Rect.x      = PlateCandidates[0].Rect.x - Image->w*0.2;      // First char x + 0.3ImageW
        Region.Rect.y      = PlateCandidates[0].Rect.y - Image->h*0.1;      // First char y + 0.3ImageH
        Region.Rect.Height = PlateCandidates[0].Rect.Height + Image->h*0.2; // First char Height + 0.3ImageH
            // Distance between the first char and last char + width of the last char + 0.3ImageW
        Region.Rect.Width  = PlateCandidates[PlateCandidates.size()-1].Rect.x - PlateCandidates[0].Rect.x
                            + PlateCandidates[PlateCandidates.size()-1].Rect.Width + Image->w*0.6;

        // Verify and adjust so the region is within the bounds of the image
        ROIFix( Region ); // Passed via reference
    }

    // Return the ROI
    return Region;
}

std::vector<ROI> PlateCandidate::GetPlateCharacters(int MonochromeRegion){

    // Preprocess the plate
    NormaliseImage(); // 5 works for bright images, but for good images is way to much...
    HistogramEqualisation();
    ImageToAdaptiveMonochrome(MonochromeRegion, true);

    // Get the characters from the region defined
    return CCA();
}

std::vector<ROI> PlateCandidate::CCA(){
    // Use Blob Grouping and Connected Component Analysis
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

            // Skip pixel if its background
            if(PixelValue == 0){
                continue;
            }

            // Find Neighbours
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

    // Order by the BlobPixel Occurances - TODO: Can we just read in the map here? Hmmmm
    std::vector<std::pair<int,int>> LabelCount(BlobPixelCount.begin(), BlobPixelCount.end());
    std::sort(LabelCount.begin(), LabelCount.end(), [](const std::pair<int,int> &element1, const std::pair<int,int> &element2) {
                                                        return element1.second > element2.second;
                                                    });

    // Limit this to the top 60 labels/regions
    LabelCount.erase(LabelCount.begin() + 60, LabelCount.end());

    std::vector<ROI> BlobRegions;
    for(int i=0; i < LabelCount.size(); i++){

        //std::cout << "Processing Label: " << LabelCount[i].first << " Value: " << LabelCount[i].second << std::endl;

        // Set variables for this region
        int xMax = 0;
        int xMin = Image->w;
        int yMax = 0;
        int yMin = Image->h;
        int PixelCount = 0;

        // Find pixels with this label in PixelToLabelMap and set them to green (GENERAL REGION MATCH)
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

        // Now we have boundaries of each Region
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

        // Normal Char | 1 or L
        if( ((1.3 <= d) && (d <= 1.7)) || ((5.2 <= d) && (d <= 5.8))        // 2001
            || ((1.5 <= d) && (d <= 1.9)) || ((4.7 <= d) && (d <= 5.2)) ){  // Pre-2001
            // Good candidate
            BlobRegions[i].Score = 1000;
            DrawRectangle(BlobRegions[i].Rect, 255, 255, 0);
            CharacterCandidate.push_back(BlobRegions[i]);
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
                    if(Configuration::Debug){
                        std::cout << " > Union join of " << i << "," << j << std::endl;
                    }
                }
            }
    }

    // Now we should have sets of unions whose boxes are close to eachother
    // Find highest occurance of a set
    std::map<int,int> SetVoteCount;
    for(int i = 0; i < CharacterCandidate.size(); i++){
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
    }

    // If the candidate == BestGroup, lets make it a new colour
    std::vector<ROI> FinalCandidates;
    for(int i = 0; i < CharacterCandidate.size(); i++){
        if(CharGroup.Find(i) == BestGroup){
            DrawRectangle(CharacterCandidate[i].Rect, 255, 0, 102);
            FinalCandidates.push_back(CharacterCandidate[i]);
        }
    }

    // See how many FinalCandidates we have
    if(Configuration::Debug){
        if(FinalCandidates.size() >= 4){
            std::cout << " >> We have " << FinalCandidates.size() << " characters" << std::endl;
        }else{
            std::cout << " >> Ignoring plate" << std::endl;
        }
    }

    return FinalCandidates;
}

void PlateCandidate::ROIFix(ROI &Region){

    // Ensure the ROI cooordinates are within the image bounds, or adjust
    Rectangle & R = Region.Rect;

    // Validate the x coordinate
    if(R.x < 0){
        R.x = 0;
    }
    else if(R.x > Image->w){
        R.x = Image->w; // This should never happen, exception?
    }

    // Validate the width
    if(R.x + R.Width > Image->w){
        R.Width = Image->w - R.x;
    }

    // Validate the y coordinate
    if(R.y < 0){
        R.y = 0;
    }
    else if(R.y > Image->h){
        R.y = Image->h; // This should never happen, exception?
    }

    // Validate Height
    if(R.y + R.Height > Image->h){
        R.Height = Image->h - R.y;
    }


}
