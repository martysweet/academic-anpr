#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "SDL_Wrapper.h"
#include "include/BitmapImage.h"
#include "include/ANPRImage.h"
#include "include/PlateCandidate.h"
#include "include/OCRAnalysis.h"


int main(){



    ANPRImage **InputImage;
    InputImage = new ANPRImage *[10]; // Amount of images being worked on

    PlateCandidate **NumberPlate;
    NumberPlate = new PlateCandidate *[10]; // Amount of images being worked on


    // Load Image
    for(int i=7; i <= 9; i++){

        std::string Location = "00" + std::to_string(i) + ".jpg";
        std::cout << "Processing: " << Location << std::endl;
/*
        InputImage[i] = new ANPRImage;
        InputImage[i]->LoadBitmapImage(Location);
        InputImage[i]->ProcessGlobalImage(50);
        delete InputImage[i];
*/


        NumberPlate[i] = new PlateCandidate;
        NumberPlate[i]->LoadBitmapImage(Location);
        NumberPlate[i]->AnalysePlate();
        delete NumberPlate[i];

    }


    return 0;
}




