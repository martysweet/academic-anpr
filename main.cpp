#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "SDL_Wrapper.h"
#include "include/BitmapImage.h"
#include "include/ANPRImage.h"
#include "include/PlateCandidate.h"

int main(int argc, char* argv[]){

    if(argc <= 1){
        std::cout << "Command line execution: ./SimpleANPR image1.jpg [image2.jpg image2.jpg ...] [--debug] [--cases 12] [--buffer 10] [--spacing 10] [--sdl] [--nosave] \n";
        std::cout << "--debug: Displays more steps and outputs debug information to the console\n";
        std::cout << "--cases: Amount of sample area iterations to try each image\n";
        std::cout << "--spacing: Amount of spacing to the right of each character in the output image\n";
        std::cout << "--sdl: Display the SDL window for the result\n";
        std::cout << "--nosave: Don't save the final result, otherwised saved as plate_image1.jpg\n" << std::endl;
        return 0;
    }

    // Define default parameters
    bool Debug = false;
    bool SDLOutput = false;
    bool SaveOutput = true;
    int  Cases = 12;
    int  Buffer = 3;
    int  Spacing = 10;
    std::vector<std::string> FileInputs;

    // Process each argument (skip first - filename)
    for(int i = 1; i <= argc; i++){
        if(std::string(argv[i]) == "--debug"){
            Debug = true;
        }
        else if(std::string(argv[i]) == "--cases"){
            if(i+1 < argc){ // If there is a parameter after this one
                Cases = atoi(argv[i++]);
            }else{
                std::cerr << "--cases must have a parameter" << std::endl;
            }
        }
        else if(std::string(argv[i]) == "--buffer"){
            if(i+1 < argc){
                Buffer = atoi(argv[i++]);
            }else{
                std::cerr << "--buffer must have a parameter" << std::endl;
            }
        }
        else if(std::string(argv[i]) == "--spacing"){
            if(i+1 < argc){
                Spacing = atoi(argv[i++]);
            }else{
                std::cerr << "--spacing must have a parameter" << std::endl;
            }
        }
        else if(std::string(argv[i]) == "--sdl"){
            SDLOutput = true;
        }
        else if(std::string(argv[i]) == "--nosave"){
            SaveOutput = false;
        }
        else{
            // Assume the argument is a filename
            FileInputs.push_back(std::string(argv[i]));
        }
    }


    ANPRImage **InputImage;
    InputImage = new ANPRImage *[FileInputs.size()]; // Amount of images being worked on

    // Load Image
    for(int i=0; i <= FileInputs.size(); i++){

        std::cout << "Processing: " << FileInputs[i] << std::endl;

        InputImage[i] = new ANPRImage;
        InputImage[i]->LoadBitmapImage(FileInputs[i]);
        InputImage[i]->ProcessGlobalImage(12);
        delete InputImage[i];

    }


    return 0;
}




