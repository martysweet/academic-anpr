#include "OCRAnalysis.h"

OCRAnalysis::OCRAnalysis()
{
    //ctor
}

OCRAnalysis::~OCRAnalysis()
{
    //dtor
}

void OCRAnalysis::Process(int Position){



    // If the ratio of the image is around 5 then it's going to be 1
    int h = Image->h;
    int w = Image->w;
    float d = (float)h/(float)w; // Ratio

    if ((4.7 <= d) && (d <= 5.8)){
        std::cout << "Detected: 1" << std::endl;
    }

    // Scale the image to testing size

    // Make the image black & white
    ImageToBW(90, false);

    // Create voting matrix for the possible characters
    std::map<char,int> CharacterVoting;
    std::string PossibleValues = "0123456789abcdefghjklmnopqrstuvwxyz"; //Misses i and o (These should be handled later)
    for( char c : PossibleValues){
        CharacterVoting[c] = 0;
    }

    // We could do pixel comparison to get a % guess
        // Go through folder of testing images

            // Compare the two images, counting matching black pixels



    // Then edge detection








    CreateWindowFlags("Character", Image->w, Image->h, 0);
    DisplaySurfaceUntilClose(Image);
    CloseWindow();


}
