#include "ANPRImage.h"


ANPRImage::ANPRImage(){

}


bool ANPRImage::ProcessGlobalImage(int MinimumBandHeight){

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
    PlateCandidate **NumberPlate;
    NumberPlate = new PlateCandidate *[10]; // Amount of images being worked on
    std::vector< std::vector<ROI> > CharacterExtractions;
    int p = 0;
    for(int i=100; i<=100; i += 100){
        // Resize
        //RegionalImage.ResizeWidth(100);
        // Get Characters
        NumberPlate[p] = new PlateCandidate;
        NumberPlate[p]->LoadBitmapImage(LoadedImage, Region.Rect);
        // Push the result of this into a vector for later processing
        CharacterExtractions.push_back(NumberPlate[p]->GetPlateCharacters());
        // Cleanup
        delete NumberPlate[p];
        // Increment class counter
        p++;
    }

    // Find the best result
    int ExtractionVote = 0;
    int ExtractionMaxSize = 0;

    // So we have a vector containing a vector of ROI
    for(int i = 0; i < CharacterExtractions.size(); i++){
        std::vector<ROI> Extraction = CharacterExtractions[i];
        if(Extraction.size() > ExtractionMaxSize){
            ExtractionMaxSize = Extraction.size();
            ExtractionVote = i;
        }
    }

    // Output the best result
    std::cout << "Best Extraction - Attempt: " << ExtractionVote;
    std::cout << " providing " << CharacterExtractions[ExtractionVote].size() << " characters" << std::endl;

    // Merge the characters and output to screen

        // We need to consider that the ROI which we have retrived will not be correct scale in relation to our input image

    return true;
}
