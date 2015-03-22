#include "ANPRImage.h"

ANPRImage::ANPRImage(std::string FileLocation){
    // Attempt to load the input file
    LoadBitmapImage(FileLocation);
    // Create output file path, taking into account / \ paths and changing the file extension
    int SlashTest = FileLocation.find_last_of("/"); // Unix Paths
    int DotTest = FileLocation.find_last_of(".");
    if(SlashTest == std::string::npos){
        SlashTest = FileLocation.find_last_of("\\"); // Windows Paths
    }
    // Compute the output filename
    if(SlashTest == std::string::npos){
        OutputPath = "plate_" + FileLocation.substr(0, DotTest-SlashTest-1) + ".bmp"; // If path contains no slashes
    }else{
        OutputPath = "plate_" + FileLocation.substr(SlashTest+1, DotTest-SlashTest-1) + ".bmp";   // Path has slashes
    }
    // If debug, notify the user of output location
    if(Configuration::Debug && Configuration::SaveOutput){
        std::cout << "Output: " << OutputPath << std::endl;
    }
}


bool ANPRImage::ProcessGlobalImage(){

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
    if(Configuration::Debug){
        std::cout << " >>> Region found: " << Region.Rect.x << " " << Region.Rect.y << " " << Region.Rect.Height <<" " << Region.Rect.Width << std::endl;
        DebugDisplayImageToWindow("Located Plate Region");
    }

    // Scale the image according to user input, each time checking charactors and scoring
    std::vector< std::vector<ROI> > CharacterExtractions;
    int p = 0;
    // Load the numberplate
    PlateCandidate NumberPlate;
    NumberPlate.LoadBitmapImage(LoadedImage, Region.Rect);
    for(int i = 3; i <= (3 * Configuration::Cases); i += 3){
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
        // For each result map, sum the score of all the elements within for use in weighting
        std::vector<ROI> CE = CharacterExtractions[ExtractionMapMatrix[i]];
        int Score = std::accumulate(CE.begin(), CE.end(), 0.0, [](const int Sum, ROI const &Region) {
                                                                            return Sum + Region.Score;
                                                                        });
        ExtractionVotingMatrix[i] *= i + Score;
    }

    // Get highest count matrix value
    std::vector<int>::iterator ExtractionVoteIt;
    int ExtractionVotingMaxID;
    ExtractionVoteIt        = std::max_element(ExtractionVotingMatrix.begin(), ExtractionVotingMatrix.end());
    ExtractionVotingMaxID   = std::distance(ExtractionVotingMatrix.begin(), ExtractionVoteIt) ;

    int ExtractionTestCase;
    ExtractionTestCase  = ExtractionMapMatrix[ExtractionVotingMaxID];


    std::vector<ROI> BestResult = CharacterExtractions.at(ExtractionTestCase);
    std::cout << " >>>> Best Extraction Attempt: " << ExtractionTestCase;
    std::cout << " providing " << BestResult.size() << " characters" << std::endl;
    // Merge the characters and output to screen
    MergePlateRegions(BestResult, Region);


    // If in debug, ask if the user wants to continue to next image
    if(Configuration::Debug){
        std::cout << "Press any key to continue...";
        std::cin.ignore();
    }

    return true;
}

void ANPRImage::MergePlateRegions( std::vector<ROI> Regions, ROI WorkingArea){

    BitmapImage ResultImage; // Output Surface
    // Only merge characters if we have enough, else output the WorkingArea
    if(Regions.size() > 4){


        for(int i = 0; i < Regions.size(); i++){
            // Add buffering to each region if requried by the user
            Regions[i].Rect.x -= Configuration::Buffer;
            Regions[i].Rect.y -= Configuration::Buffer;
            Regions[i].Rect.Width += Configuration::Buffer*2;
            Regions[i].Rect.Height += Configuration::Buffer*2;
        }

        // Find required width
        int TotalWidth = std::accumulate(Regions.begin(), Regions.end(), Configuration::Spacing*Regions.size(), [](const int Sum, ROI const &Region) {
                                                                            return Sum + Region.Rect.Width;
                                                                        });

        // Find max height
        ROI THeightRegion = *std::max_element(Regions.begin(), Regions.end(), [] (const ROI &Element1, const ROI &Element2 ){
                                                                                return Element1.Rect.Height < Element2.Rect.Height;
                                                                            });
        int TotalHeight = THeightRegion.Rect.Height;

        // Remove small noise regions
        bool RemovedRegions = false;
        for(int i = 0; i < Regions.size(); i++){
            if( Regions[i].Rect.Height < TotalHeight/2){
                Regions.erase(Regions.begin()+i);
                RemovedRegions = true;
                i--;
            }
        }

        // If we have removed regions, recompute required width
        if(RemovedRegions){
            TotalWidth = std::accumulate(Regions.begin(), Regions.end(), Configuration::Spacing*Regions.size(), [](const int Sum, ROI const &Region) {
                                                                            return Sum + Region.Rect.Width;
                                                                        });
        }


        // Create the required surface
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
            StartX += Configuration::Spacing;
        }
    }else{
        // If we have less than 4 characters
        std::cout << " >>>>> Attempt <4 outputting whole plate region." << std::endl;
        WorkingArea.Rect.x += 1; // Add one to x,y to hide possible boundaries
        WorkingArea.Rect.y += 1;
        WorkingArea.Rect.Width -= 1;
        WorkingArea.Rect.Height -= 1;
        ResultImage.LoadBitmapImage(Image, WorkingArea.Rect);
    }

    // Display the result
    if(Configuration::SDLOutput){
        ResultImage.DebugDisplayImageToWindow("Result");
    }

    // Save to file
    if(Configuration::SaveOutput){
        ResultImage.SaveImageToFile(OutputPath);
    }
}
