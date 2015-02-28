#include "BitmapImage.h"

BitmapImage::BitmapImage(){

    // Create needed Convolution Matricies
    // This is done here to allow difference sizes of matrices in one datatype
    InitialiseConvolutionMatrix(SobelHorizontal,    3, 3, 1,  {-1,  -2,  -1,     0,  0,  0,      1,  2,  1});
    InitialiseConvolutionMatrix(SobelVertical,      3, 3, 1,  {-1,   0,  1,     -2,  0,  2,     -1,  0,  1});
    InitialiseConvolutionMatrix(GaussianBlur1,      3, 3, 1,  {0.045, 0.122, 0.045, 0.122, 0.332, 0.122, 0.045, 0.122, 0.045});
    InitialiseConvolutionMatrix(GaussianBlur2,      3, 3, 16, {1, 2, 1, 2, 4, 2, 1, 2, 1});
    InitialiseConvolutionMatrix(GaussianBlur3,      3, 3, 5,  {0, 1, 0, 1, 1, 1, 0, 1, 0});
    InitialiseConvolutionMatrix(HorizontalRank,     21, 3, 1,  { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                                                 0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,
                                                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  });
    InitialiseConvolutionMatrix(VerticalRank,       3, 25, 1,  {0,0.04,0,
                                                                  0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,0,0.04,0,


                                                                  });

    InitialiseConvolutionMatrix(Blur1,              5, 5, 66,  {4,4,0,4,4,4,4,0,4,4,0,0,2,0,0,4,4,0,4,4,4,4,0,4,4});
    InitialiseConvolutionMatrix(EdgeHorizontal,     5, 3, 1,  {-1,-1,-1,-1,-1,0,0,0,0,0,1,1,1,1,1});
    InitialiseConvolutionMatrix(EdgeVertical,       3, 3, 1,  {-1,0,1,-1,0,1,-1,0,1});
    InitialiseConvolutionMatrix(Median,             3, 3, 8,  {1, 1, 1, 1, 0, 1, 1, 1, 1});
    InitialiseConvolutionMatrix(EdgeDetect4,        3, 3, 1,  {-1, -1, -1, 0, 0, 0, 1, 1, 1});

}

BitmapImage::~BitmapImage(){
    //dtor
    IntegralArray.clear();
}

void BitmapImage::InitialiseConvolutionMatrix(ConvolutionMatrix &Struct, int Columns, int Rows, float Weight, std::initializer_list<float> List){
    Struct.Columns   = Columns;
    Struct.Rows      = Rows;
    Struct.Weight    = Weight;
    Struct.Matrix.insert( Struct.Matrix.end(), List ); // Requires C++11
}

/*
    BitmapImage::OrderCoordinates(): Ensures (x1,y1) is to the left of (x2,y2)
*/
void BitmapImage::OrderCoordinates(int &x1, int &y1, int &x2, int &y2){
    if(x2 < x1){
        int tx2 = x2;  // Temp
        int ty2 = y2;
        x2 = x1;
        y2 = y1;
        x1 = tx2;
        y1 = ty2;
    }
}

/*
    BitmapImage::DrawLine(): Draws a line on image from (x1,y1) to (x2,y2)
*/
void BitmapImage::DrawLine(int x1, int y1, int x2, int y2, Uint8 R, Uint8 G, Uint8 B){
    OrderCoordinates(x1, y1, x2, y2); // Make sure the largest is (x2,y2)
    int dx = x2-x1;
    int dy = y2-y1;
    if(dx != 0){
        for(int x = x1; x <= x2; x++){
            int y = y1 + dy * (x - x1) / dx;
            SetRGBValues(Image, x, y, R, G, B);
        }
    }else{
        for(int y=y1; y <= y2; y++){
            SetRGBValues(Image, x1, y, R, G, B);
        }
    }
}

/*
    BitmapImage::DrawRectangle(): Draws a rectangle on image from (x,y) given the dimensions w,h
*/
void BitmapImage::DrawRectangle(int x, int y, int w, int h,  Uint8 R, Uint8 G, Uint8 B){
    // Top Line _
    DrawLine(x,   y,   x+w, y,   R, G, B);
    // Bottom Line _
    DrawLine(x,   y+h, x+w, y+h, R, G, B);
    // Right Line |
    DrawLine(x+w, y,   x+w, y+h, R, G, B);
    // Left Line |
    DrawLine(x,   y,   x,   y+h, R, G, B);
}

/*
    BitmapImage::ImageToLocalisedMonochrome(): Converts the image to localised monochrome
*/
void BitmapImage::ImageToLocalisedMonochrome(int Threshold){
    Uint8 R = 0, G = 0, B = 0;
    for(int y = 0; y < Image->h; y++){
		for(int x = 0; x < Image->w; x++){
            // RGB of this pixel
            GetRGBValues(Image, x, y, &R, &G, &B);
            // Threshold of 80 to get good black
            if( (R+G+B)/3 > Threshold ){
                SetRGBValues(Image,x,y,255,255,255);
            }else{
                SetRGBValues(Image,x,y,0,0,0);
            }
		}
	}
}

/*
    BitmapImage::ImageToGrayscale(): Converts the image to grayscale
*/
void BitmapImage::ImageToGrayscale(){
    Uint8 R = 0, G = 0, B = 0;
    int s = 0; // Sampled variable
    for(int y = 0; y < Image->h; y++){
        for(int x = 0; x < Image->w; x++){
            GetRGBValues(Image, x, y, &R, &G, &B);
            s = (0.299*R+0.587*G+0.114*B);
            SetRGBValues(Image, x, y, s, s, s);
        }
    }
}

/*
    BitmapImage::GetIntegralValue(): Return the integral value of position (x,y)
*/
inline int BitmapImage::GetIntegralValue(int x, int y){
    if(x < 0 || y < 0){
        return 0;       // The values are to the top/left of the image
    }else{
        return IntegralArray[(Image->w*y)+x];
    }
}

/*
    BitmapImage::CreateIntegralArray(): Creates an integral array of the image
*/
void BitmapImage::CreateIntegralArray(){
    Uint8 R = 0, G = 0, B = 0;
    int element = 0;
    IntegralArray.resize(Image->w*Image->h);
    for(int y=0; y<Image->h; y++){
        for(int x=0; x<Image->w; x++){
            // Get values in x,y box from origin
            // S(x,y) = i(x,y) + s(x-1,y) + s(x,y-1) - s(x-1,y-1) (To remove the repeated segment)
            GetRGBValues(Image, x, y, &R, &G, &B);
            IntegralArray[element] = R+G+B+GetIntegralValue(x-1,y)+GetIntegralValue(x,y-1)-GetIntegralValue(x-1, y-1);
            element++; // Increase pixel location
        }
    }
}

/*
    BitmapImage:GetIntegralAreaAverage(): Returns a value 0-255 of the areas average, forced within image boundaries
*/
inline int BitmapImage::GetIntegralAreaAverage(int x, int y, int area){
    int a, b, c, d, i, o; // i = inside sitting | o = outside sitting

    /*
     If any of the boundaries go out of range area*area reposition (x,y)
     to acheive the area*area average.
    */
    i = area/2; // Rounded down
    o = i+1;    // Offset by 1

    if(x+i > Image->w){
        x -= abs(Image->w-x-i); // Gone off the grid to the RIGHT
    }

    if(y+i > Image->h){
        y -= abs(Image->h-y-i); // Gone off the grid to the BOTTOM
    }

    if(x-o < 0){
        x += abs(x-o); // Gone off the grid to the LEFT
    }

    if(y-o < 0){
        y += abs(y-o); // Gone off the grid to the TOP
    }


    // Calculate the memory reads
    a = GetIntegralValue(x-o, y-o);
    c = GetIntegralValue(x-o, y+i);
    b = GetIntegralValue(x+i, y-o);
    d = GetIntegralValue(x+i, y+i);

    // Return the Average
    return (a+d-c-b)/((area*area)+area*20);
}

/*
    BitmapImage::ImageToAdaptiveMonochrome(): Uses intensity sampling to give better monochrome
*/
void BitmapImage::ImageToAdaptiveMonochrome(int SampleArea){
    Uint8 R = 0, G = 0, B = 0;
    int AverageIntensity, PixelIntensity;
    CreateIntegralArray(); // Create the Integral Array
    for(int y = 0; y < Image->h; y++){
        for(int x = 0; x < Image->w; x++){
        // Get average intensity around pixel
        AverageIntensity = GetIntegralAreaAverage(x, y, SampleArea);
        GetRGBValues(Image, x, y, &R, &G, &B);
        PixelIntensity = R+G+B;
        if( PixelIntensity > AverageIntensity ){
            SetRGBValues(Image,x,y,255,255,255); // White
        }else{
            SetRGBValues(Image,x,y,0,0,0);  // Black
        }
        }
    }

}

void BitmapImage::ImageGrayscaleToDifferentialMonochrome(int Difference){
    Uint8 R = 0, G = 0, B = 0;
    // Do two consectutive pixels have a high difference in grayscale?
    int LastPixel, Pixel;
    for(int y = 0; y < Image->h; y++){
        GetRGBValues(Image, 0, y, &R, &G, &B); // First pixel in row
        LastPixel = R; // Image is grayscale so only have to look at one value
        for(int x = 1; x < Image->w; x++){
            GetRGBValues(Image, x, y, &R, &G, &B);
            Pixel = R;

            // Is there a large difference
            if(abs(LastPixel - Pixel) > Difference){
                // Replace last pixel with black
                SetRGBValues(Image, x-1, y, 0, 0, 0);
            }else{
                SetRGBValues(Image, x-1, y, 255, 255, 255);
            }
            LastPixel = Pixel;
        }
    }

}



/*
    BitmapImage::SaveImageToFile(): Save protected image to disk
*/
void BitmapImage::SaveImageToFile(std::string Filename){
    SaveImageBMP(Image, Filename.c_str());
}

void BitmapImage::LoadImageFromFile(std::string Filename){
    Image = LoadImage(Filename.c_str());
    // Check
    if(Image == NULL){
        std::cerr << "Cannot load " << Filename << std::endl;
        std::exit(1);
    }
}

/*

*/
void BitmapImage::ImageConvolutionMatrixTransformation(std::vector<ConvolutionMatrix> Convolutions){
    // ALways Create a Grayscale Array for the current state
    CreateGrayscaleMapArray();

    // Create new Image
    SDL_Surface* DestinationImage = CreateNewImage(Image->w, Image->h);

    int Pixel;

    for(int y = 0; y < Image->h; y++){
        for(int x = 0; x < Image->w; x++){

        Pixel = 0;
        for(int c = 0; c < Convolutions.size(); c++){
            Pixel  += ProcessKernelFilter(x, y, Convolutions[c]);   // Process the Convolution Matrix
        }


        // Cap high values at 255
        if(Pixel > 255){
            Pixel = 255;
        }


        // Set in new image
        SetRGBValues(DestinationImage, x, y, Pixel, Pixel, Pixel);


        }
    }

   Image = DestinationImage;

}

void BitmapImage::CreateGrayscaleMapArray(){
    Uint8 R = 0, G = 0, B = 0;
    GrayscaleArray.resize(Image->w*Image->h);
    int Element = 0;
    for(int y = 0; y < Image->h; y++){
        for(int x = 0; x < Image-> w; x++){
            GetRGBValues(Image, x, y, &R, &G, &B);
            GrayscaleArray[Element] = R;
            Element++;
        }
    }
}

int BitmapImage::GetGrayscaleMapValue(int x, int y){
    if(x < 0 || y < 0 || x > Image->w-1 || y > Image->h-1){
        return 0;       // The values are to the top/left of the image
    }else{
        return GrayscaleArray[(Image->w*y)+x];
    }
}

int BitmapImage::ProcessKernelFilter(int x, int y, ConvolutionMatrix Kernel){

        float Total = 0;

        // Integer Division
        int h = Kernel.Columns/2;
        int v = Kernel.Rows/2;

        // m = rows | n = columns
        for(int m=0; m < Kernel.Rows; m++){
            for(int n=0; n < Kernel.Columns; n++){

                //std::cout << GetGrayscaleMapValue(x-h+n, y-v+m) << std::endl;
                Total += GetGrayscaleMapValue(x-h+n, y-v+m) * Kernel.Matrix[ ((Kernel.Columns)*m) + n ];

            }
            //std::cout << m << ",";
        }

        return (int)abs(Total/Kernel.Weight);    // Round to INT
}


void BitmapImage::ImageToBW(int Threshold){
    Uint8 R = 0, G = 0, B = 0;
    for(int y = 0; y < Image->h; y++){
        for(int x = 0; x < Image->w; x++){
            GetRGBValues(Image, x, y, &R, &G, &B);
            if(R > Threshold){
                SetRGBValues(Image, x, y, 0, 0, 0);
            }else{
                SetRGBValues(Image, x, y, 255,255,255);
            }
        }
    }
}


