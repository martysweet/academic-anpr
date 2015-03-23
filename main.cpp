#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "SDL_Wrapper.h"
#include "include/DataStructures.h"
#include "include/BitmapImage.h"
#include "include/ANPRImage.h"
#include "include/PlateCandidate.h"

namespace Configuration {
    bool Debug;
    bool SDLOutput;
    bool SaveOutput;
    int  Cases;
    int  Buffer;
    int  Spacing;
};

void OutputHelpMessage();

int main(int argc, char* argv[]){

    // If not arguments are specified, give help output
    if(argc <= 1){
        OutputHelpMessage();
        return 0;
    }

    // Define default parameters
    Configuration::Debug = false;
    Configuration::SDLOutput = false;
    Configuration::SaveOutput = true;
    Configuration::Cases = 12;
    Configuration::Buffer = 2;
    Configuration::Spacing = 5;
    std::vector<std::string> FileInputs;


    // Process each argument (skip first - filename)
    for(int i = 1; i < argc; i++){

        if(std::string(argv[i]) == "--debug"){
            Configuration::Debug = true;
        }
        else if(std::string(argv[i]) == "--cases"){     // Amount of AdaptiveMonochrome area attempts
            if(i+1 < argc){ // If there is a parameter after this one
                Configuration::Cases = atoi(argv[++i]);
            }else{
                std::cerr << "--cases must have a parameter" << std::endl;
                return false;
            }
        }
        else if(std::string(argv[i]) == "--buffer"){    // Charater region buffer
            if(i+1 < argc){
                Configuration::Buffer = atoi(argv[++i]);
            }else{
                std::cerr << "--buffer must have a parameter" << std::endl;
                return false;
            }
        }
        else if(std::string(argv[i]) == "--spacing"){   // Right spacing for each character
            if(i+1 < argc){
                Configuration::Spacing = atoi(argv[++i]);
            }else{
                std::cerr << "--spacing must have a parameter" << std::endl;
                return false;
            }
        }
        else if(std::string(argv[i]) == "--sdl"){       // Display result
            Configuration::SDLOutput = true;
        }
        else if(std::string(argv[i]) == "--nosave"){    // Save result to disk
            Configuration::SaveOutput = false;
        }
        else if(std::string(argv[i]) == "--help"){    // Help Message
            OutputHelpMessage();
            return 0;
        }
        else{
            // Assume the argument is a filename
            FileInputs.push_back(std::string(argv[i]));
        }
    }

    // Check we have some input files to work with
    if(FileInputs.size() == 0){
        std::cerr << "No input images specified!" << std::endl;
        return 1;
    }

    // Display the currently running parameters
    if(Configuration::Debug){
        std::cout << "Using parameters:" << std::endl;
        std::cout << "Debug: 1" << std::endl;
        std::cout << "SDLOutput: " << Configuration::SDLOutput << std::endl;
        std::cout << "SaveOutput: " << Configuration::SaveOutput << std::endl;
        std::cout << "Cases: " << Configuration::Cases << std::endl;
        std::cout << "Buffer: " << Configuration::Buffer << std::endl;
        std::cout << "Spacing: " << Configuration::Spacing << std::endl;
    }

    // Process the input images
    ANPRImage **InputImage;
    InputImage = new ANPRImage *[FileInputs.size()]; // Amount of images being worked on

    for(int i=0; i < FileInputs.size(); i++){
        std::cout << "Processing: " << FileInputs[i] << std::endl;
        try{
            InputImage[i] = new ANPRImage(FileInputs[i]);   // Create new ANPRImage instance
            InputImage[i]->ProcessGlobalImage();            // Process the image
            delete InputImage[i];
        }catch(...){
            std::cout << " > A fatal error occured processing " << FileInputs[i] << std::endl;
        }
    }


    return 0;
}

void OutputHelpMessage(){
        std::cout << "Command line execution: ./SimpleANPR image1.jpg [image2.jpg image2.jpg ...] [--debug] [--cases 12] [--buffer 2] [--spacing 5] [--sdl] [--nosave] [--help] \n";
        std::cout << "Images: Relative or absolute paths allowed, images must be of correct orientation\n";
        std::cout << "--debug: Displays more SDL steps and outputs debug information to the console\n";
        std::cout << "--cases: Amount of sample area iterations to try on each image\n";
        std::cout << "--buffer: Amount of pixels around the character to be extracted\n";
        std::cout << "--spacing: Amount of spacing to the right of each character in the output image\n";
        std::cout << "--sdl: Display the SDL window for the result\n";
        std::cout << "--nosave: Don't save the final result, otherwised saved as plate_image1.bmp to current working directory\n" ;
        std::cout << "--help: This information message\n" << std::endl;
}



