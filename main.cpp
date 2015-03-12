#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "SDL_Wrapper.h"
#include "include/BitmapImage.h"
#include "include/ANPRImage.h"
#include "include/PlateCandidate.h"


int main(){

    // Load Image
    for(int i=1; i < 6; i++){

        std::string Location = "00" + std::to_string(i) + ".jpg";
        std::cout << "Processing: " << Location << std::endl;
        ANPRImage InputImage;
        InputImage.LoadBitmapImage(Location);
        InputImage.ProcessGlobalImage(100);

    }


    return 0;
}




