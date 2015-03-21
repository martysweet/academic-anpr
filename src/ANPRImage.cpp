#include "ANPRImage.h"

bool ANPRImage::ProcessGlobalImage(int TestCases){

    // Make the image Grayscale for all further operations
    ImageToGrayscale();

    // Find the potential plate region
    PlateCandidate PlateRegion;
    PlateRegion.LoadBitmapImage(Image);         // Load current image into the Plate Region Search
    ROI Region = PlateRegion.GetPlateRegion();

    // See if we have a valid region (otherwise all will be 0)
    if(Region.Rect.Height == 0 || Region.Rect.Width == 0){
        return false;
    }

    // Debug found region
    DrawRectangle(Region.Rect, 255, 0, 0);
    std::cout << "Region found: " << Region.Rect.x << " " << Region.Rect.y << " " << Region.Rect.Height <<" " << Region.Rect.Width << std::endl;
    DebugDisplayImageToWindow("Yay");

    // Scale the image according to user input, each time checking charactors and scoring
    std::vector< std::vector<ROI> > CharacterExtractions;
    int p = 0;
    // Load the numberplate
    PlateCandidate NumberPlate;
    NumberPlate.LoadBitmapImage(LoadedImage, Region.Rect);
    for(int i = 3; i <= (3*TestCases); i += 3){
        // Push the result of this into a vector for later processing
        CharacterExtractions.push_back(NumberPlate.GetPlateCharacters(i));
        // Restore the number plates image for next test
        NumberPlate.RestoreToLoadedImage();
    }

    // Find the best result
    int ExtractionMaxSize = 0;

    // Find the most common Extraction Size
    // High Value
    for(int i = 0; i < CharacterExtractions.size(); i++){
        std::vector<ROI> Extraction = CharacterExtractions[i];
        if(Extraction.size() > ExtractionMaxSize){              // Find the largest character size
            ExtractionMaxSize = Extraction.size();            // Set how many characters have been found
        }
    }

    // Create a count matrix
    std::vector<int> ExtractionVotingMatrix;
    std::vector<int> ExtractionMapMatrix;
    int ExtractionVote = 0;
    // Resize voting/map
    ExtractionVotingMatrix.resize(ExtractionMaxSize + 1, 0);
    ExtractionMapMatrix.resize(ExtractionMaxSize + 1, 0);

    // Increment Count Vote
    for(int i = 0; i < CharacterExtractions.size(); i++){
        ExtractionVotingMatrix[CharacterExtractions[i].size()]++; // Increment Count Vote
        ExtractionMapMatrix[CharacterExtractions[i].size()] = i;  // Last found result with this size
    }

    // Weight the results
    for(int i = 0; i < ExtractionVotingMatrix.size(); i++){
        ExtractionVotingMatrix[i] *= i;
    }

    // Get highest count matrix value
    std::vector<int>::iterator ExtractionVoteIt;
    int ExtractionVotingMaxID;
    ExtractionVoteIt        = std::max_element(ExtractionVotingMatrix.begin(), ExtractionVotingMatrix.end());
    ExtractionVotingMaxID   = std::distance(ExtractionVotingMatrix.begin(), ExtractionVoteIt) ;

    int ExtractionTestCase;
    ExtractionTestCase  = ExtractionMapMatrix[ExtractionVotingMaxID];

    // Output the best result
    std::vector<ROI> BestResult = CharacterExtractions.at(ExtractionTestCase);
    std::cout << " >>>> Best Extraction Attempt: " << ExtractionVote;
    std::cout << " providing " << BestResult.size() << " characters" << std::endl;

    // Merge the characters and output to screen
    OutputCharactersToScreen(BestResult, Region);

    // If in debug, ask if the user wants to continue to next image
    std::cout << "Press any key to continue...";
    std::cin.ignore();

    return true;
}

void ANPRImage::OutputCharactersToScreen( std::vector<ROI> Regions, ROI WorkingArea, int Spacing, int Buffer ){
    // Add required buffering to each region if requried by the user
    for(int i = 0; i < Regions.size(); i++){
        Regions[i].Rect.x -= Buffer;
        Regions[i].Rect.y -= Buffer;
        Regions[i].Rect.Width += Buffer*2;
        Regions[i].Rect.Height += Buffer*2;
    }

    // Find required width
    int TotalWidth = std::accumulate(Regions.begin(), Regions.end(), Spacing*Regions.size(), [](const int Sum, ROI const &Region) {
                                                                        return Sum + Region.Rect.Width;
                                                                    });

    // Find max height
    ROI THeightRegion = *std::max_element(Regions.begin(), Regions.end(), [] (const ROI &Element1, const ROI &Element2 ){
                                                                            return Element1.Rect.Height < Element2.Rect.Height;
                                                                        });
    int TotalHeight = THeightRegion.Rect.Height;

    // Create the required surface
    BitmapImage ResultImage;
    ResultImage.CreateBitmapImage(TotalWidth, TotalHeight, true);

    // Draw each region into new Result Image
    int StartX = 0;
    int StartY = 0;
    int CurrentY = 0;
    Uint8 R = 0, G = 0, B = 0;
    for(int i = 0; i < Regions.size(); i++){
        Rectangle Rect = Regions[i].Rect;

        // Read each pixel
        for(int x = Rect.x; x < Rect.x + Rect.Width; x++){
            CurrentY = 0;
            for(int y = Rect.y; y < Rect.y + Rect.Height; y++){
                // Write pixel to new image
                GetRGBValues(Image, x+WorkingArea.Rect.x, y+WorkingArea.Rect.y, &R, &G, &B);
                ResultImage.SetPixel(StartX, CurrentY, R, G, B);
                CurrentY++;
            }
        StartX++;
        }
        StartX += Spacing;
    }

    // Display the result
    ResultImage.DebugDisplayImageToWindow("Result");
}
