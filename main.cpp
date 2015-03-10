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
    std::string Location = "005.jpg";

    ANPRImage InputImage;
    InputImage.LoadBitmapImage(Location);
    InputImage.ProcessGlobalImage();

    return 0;
}




