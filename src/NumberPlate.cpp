#include "NumberPlate.h"


void NumberPlate::ProcessGlobalImage(){
    // Convert the image to Grayscale
    ImageToGrayscale();
   // ImageGrayscaleToDifferentialMonochrome();
    //SobelEdgeDetection();
 //   EdgeDetection();


   // SobelEdgeDetection();
    std::vector< ConvolutionMatrix > MyFilters;

    MyFilters.push_back(SobelHorizontal);
    //MyFilters.push_back(GaussianBlur2);

   ImageConvolutionMatrixTransformation(MyFilters);

/*
    CreateIntensityRowProfileY();
    CreateRowProfileY();

    VerticalBandDetection(4);
    DrawVerticalBandDetection();
*/
    SaveImageToFile("output.bmp");
}


int NumberPlate::GetMaxArrayValue(std::vector<int> intArray){
    int m = 0;
    for(int i=0; i<intArray.size(); i++){
        if(intArray[i] > m)
            m = intArray[i];
    }
    return m;
}

void NumberPlate::CreateRowProfileY(){
    Uint8 R = 0, G = 0, B = 0;
    int Pixel, LastPixel;
    RowProfileY.resize(Image->h);
    std::fill(RowProfileY.begin(), RowProfileY.end(), 0); // Set all values to zero

    for(int y=0; y < Image->h; y++){
        for(int x=0; x < Image->w; x++){
            GetRGBValues(Image, x, y, &R, &G, &B);
            if(R == 255 && G == 255 && B == 255){
                Pixel = 255; // White
            }else{
                Pixel = 0;   // Black
            }
            if(Pixel != LastPixel){
                RowProfileY[y]++; // We have detected an edge
            }
            LastPixel = Pixel;
        }

        // Score on the image
        for(int x=0; x < RowProfileY[y]; x++){
            SetRGBValues(Image, x, y, 255, 0, 0);
        }
    }
}

void NumberPlate::VerticalBandDetection(int Iterations, float Constant){
    CreateRowProfileY();
    // Initialise the Vector
    BandBoundariesY.resize(BandBoundariesY.size()+1);
    // Find the boundaries of the Iterations highest value
    for(int i = 1; i <= Iterations; i++){ // Start at 1 as struct vector does

        int ybm = 0;    // Middle Pointer
        int ybmv = 0;   // Middle Value

        // Find the highest row ybm
        for(int i = 0; i < IntensityRowProfileY.size(); i++){
            if(IntensityRowProfileY[i] > ybmv){
                ybm = i;
                ybmv = IntensityRowProfileY[i];
            }
        }

        // Find yb0
        int yb0 = 0; // Row ID
        for(int b0 = 0; b0 < ybm; b0++){
            if(IntensityRowProfileY[b0] <= Constant*ybmv){
                yb0 = b0;
            }
        }

        // Find yb1
        int yb1 = 0; // Row ID
        int yb1v = ybmv; // Maximum value in array
        for(int b1 = ybm; b1 < IntensityRowProfileY.size(); b1++){
            if(IntensityRowProfileY[b1] <= Constant*ybmv){
                if(IntensityRowProfileY[b1] < yb1v){
                    yb1 = b1;
                    yb1v = IntensityRowProfileY[b1];
                }
            }
        }

        // We have the lower, middle and upper value
        //  Sum values and add to array of struct
        // Zero values between yb0 and yb1, add to Area
        int AreaSum = 0;
        if(yb1 == 0){
            yb1 = ybm;
        }
        for(int z=yb0; z<yb1; z++){
            AreaSum += IntensityRowProfileY[z];
            IntensityRowProfileY[z] = 0;
        }

        if(AreaSum != 0){
            BandBoundariesY.resize(BandBoundariesY.size()+1);
            BandBoundariesY[i].b0 = yb0;
            BandBoundariesY[i].bm = ybm;
            BandBoundariesY[i].b1 = yb1;
            BandBoundariesY[i].Area = AreaSum;
        }
    }
}

void NumberPlate::DrawVerticalBandDetection(){
    for(int i = 0; i < BandBoundariesY.size(); i++){
        DrawRectangle(20, BandBoundariesY[i].b0, Image->w-21, BandBoundariesY[i].b1-BandBoundariesY[i].b0, 0, 0, 255);
    }
    std::cout << BandBoundariesY.size()-1;
}

void NumberPlate::CreateIntensityRowProfileY(){
    Uint8 R = 0, G = 0, B = 0;
    IntensityRowProfileY.resize(Image->h);
    std::fill(IntensityRowProfileY.begin(), IntensityRowProfileY.end(), 0); // Set all values to zero

    for(int y=11; y < Image->h; y++){
        int Sum = 0;
        for(int x=11; x < Image->w; x++){
// TODO (marty#1#): For some reason RGB is not being picked up
            GetRGBValues(Image, x, y, &R, &G, &B);
             Sum += R;
        }
        IntensityRowProfileY[y] = Sum;

        // Score on the image
        for(int x=0; x < IntensityRowProfileY[y]/265; x++){
            SetRGBValues(Image, x, y, 0, 0, 255);
        }
    }
}
