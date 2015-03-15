#include "BitmapImage.h"

BitmapImage::BitmapImage(){

    // Create needed Convolution Matricies
    // This is done here to allow difference sizes of matrices in one datatype
    InitialiseConvolutionMatrix(CannyX,      3, 3, 1,  {-1,0,1,-2,0,2,-1,0,1});
    InitialiseConvolutionMatrix(CannyY,      3, 3, 1,  {-1,-2,-1,0,0,0,1,2,1});
    InitialiseConvolutionMatrix(HorizontalRank,     21, 3, 1,  { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                                                0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,0.04,
                                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  });
    InitialiseConvolutionMatrix(Median,             3, 3, 8,  {1, 1, 1, 1, 0, 1, 1, 1, 1});
}

BitmapImage::~BitmapImage(){
    //dtor
    //IntegralArray.clear();

}


/* Image Loading/Saving Functions */
void BitmapImage::SaveImageToFile(std::string Filename){
    SaveImageBMP(Image, Filename.c_str());
}

void BitmapImage::LoadBitmapImage(std::string Filename){
    Image = LoadImage(Filename.c_str());
    // Check
    if(Image == NULL){
        std::cerr << "Cannot load " << Filename << std::endl;
        std::exit(1);
    }
    LoadedImage = Image; // Duplicate Loaded Image
}

void BitmapImage::LoadBitmapImage(SDL_Surface* InputImage, Rectangle Rect){
    Uint8 R = 0, G = 0, B = 0;
    Image = CreateNewImage(Rect.Width, Rect.Height);
    // Map pixels to new image
    for(int y = 0; y < Rect.Height; y++){
        for(int x = 0; x < Rect.Width; x++){
            GetRGBValues(InputImage, Rect.x+x, Rect.y+y, &R, &G, &B);
            SetRGBValues(Image, x, y, R, G, B);
        }
    }
    LoadedImage = Image; // Duplicate Loaded Image
}

void BitmapImage::RestoreToLoadedImage(){
    Image = LoadedImage;
}


/* Matrix Manipulation Functions */
void  BitmapImage::InitialiseConvolutionMatrix(ConvolutionMatrix &Struct, int Columns, int Rows, float Weight, std::initializer_list<float> List){
    Struct.Columns   = Columns;
    Struct.Rows      = Rows;
    Struct.Weight    = Weight;
    Struct.Matrix.insert( Struct.Matrix.end(), List ); // Requires C++11
}

float BitmapImage::ProcessKernelFilter(int x, int y, ConvolutionMatrix Kernel){

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

        return abs(Total/Kernel.Weight);    // Round to INT
}

void  BitmapImage::ImageConvolutionMatrixTransformation(std::vector<ConvolutionMatrix> Convolutions){
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


/* Line Drawing Functions */
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

void BitmapImage::DrawLine(int x1, int y1, int x2, int y2, Uint8 R, Uint8 G, Uint8 B){
    OrderCoordinates(x1, y1, x2, y2); // Make sure the largest is (x2,y2)
    int dx = x2-x1;
    int dy = y2-y1;
    if(dx != 0){
        for(int x = x1; x <= x2; x++){
            int y = y1 + dy * (x - x1) / dx;
            if( (y > 0 && y < Image->h) && (x > 0 && x < Image->w)){
                SetRGBValues(Image, x, y, R, G, B);
            }
        }
    }else{
        for(int y=y1; y <= y2; y++){
            if( (y > 0 && y < Image->h) && (x1 > 0 && x1 < Image->w)){
                SetRGBValues(Image, x1, y, R, G, B);
            }
        }
    }
}

void BitmapImage::DrawRectangle(Rectangle Rect, Uint8 R, Uint8 G, Uint8 B){
    // Overload to allow more readable code
    DrawRectangle(Rect.x, Rect.y, Rect.Width, Rect.Height, R, G, B);
}

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



/* Whole Image Editing Function */
void BitmapImage::ImageToBW(int Threshold, bool Inverted){
    Uint8 R = 0, G = 0, B = 0;
    for(int y = 0; y < Image->h; y++){
        for(int x = 0; x < Image->w; x++){
            GetRGBValues(Image, x, y, &R, &G, &B);
            if(R > Threshold){
                if(Inverted = true){
                    SetRGBValues(Image, x, y, 0, 0, 0);
                }else{
                    SetRGBValues(Image, x, y, 255, 255, 255);
                }
            }else{
                if(Inverted = true){
                    SetRGBValues(Image, x, y, 255, 255, 255);
                }else{
                    SetRGBValues(Image, x, y, 0, 0, 0);
                }
                SetRGBValues(Image, x, y, 255,255,255);
            }
        }
    }
}

void BitmapImage::CannyEdgeDetection(){
    // ALways Create a Grayscale Array for the current state
    CreateGrayscaleMapArray();

    // Create new Image
    SDL_Surface* DestinationImage = CreateNewImage(Image->w, Image->h);

    int LowerThreshold=20, UpperThreshold=LowerThreshold*3;

    int Pixel, Gx, Gy, I1, I2;
    float A, G;

    for(int y = 0; y < Image->h; y++){
        for(int x = 0; x < Image->w; x++){

        G = 0;
        Gx = ProcessKernelFilter(x, y, CannyX); // Apply Filters
        Gy = ProcessKernelFilter(x, y, CannyY);
        G  = sqrt((Gx*Gx) + (Gy*Gy));   // Calculate magnitude
        if(Gx == 0){
            A = 0;
        }else{
            A  = atan(Gy/Gx)*(180/M_PI);               // Angle
        }


        // Round Angles
        if(((0 < A) && (A <= 22.5)) || ((157.5 < A) && (A <= 180))){ // Right operator always </>= to catch all value
            A  = 0; // Horizontal
            I1 = GetGrayscaleMapValue(x-1, y);
            I2 = GetGrayscaleMapValue(x+1, y);
        }
        else if((22.5 < A) && (A <= 67.5)){
            A = 45; // +ve Diagonal
            I1 = GetGrayscaleMapValue(x+1, y+1);
            I2 = GetGrayscaleMapValue(x-1, y-1);
        }
        else if((67.5 < A) && (A <= 122.5)){
            A = 90; // Vertical
            I1 = GetGrayscaleMapValue(x, y-1);
            I2 = GetGrayscaleMapValue(x, y+1);
        }
        else if((112.5 < A) && (A <= 157.5)){
            A = 135; // -ve Diagonal
            I1 = GetGrayscaleMapValue(x-1, y-1);
            I2 = GetGrayscaleMapValue(x+1, y+1);
        }

        // Suppress if less than gradients, cleans the image
        if( G < I1 || G < I2){
            G = 0;
        }

        // HYSTERSIS
        if(G >= UpperThreshold){
            G = 255;
        }
        else if(G <= LowerThreshold){
            G = 0;
        }
        else if(LowerThreshold < G < UpperThreshold){
            // Accept if connected to pixel > UpperThreshold
            bool Accept = false;
            for(int m=-1; m < 2; m++){
                for(int n=-1; n < 2; n++){
                    if(GetGrayscaleMapValue(x+n,y+m) > UpperThreshold){
                        Accept = true;
                        break;
                    }
                if(Accept == true) break;
                }
            }
            // Reject the value
            if(Accept == false){
             G = 0;
            }else{
             G = 255;
            }
        }



        // Cap high values at 255
        if(G > 255){
            G = 255;
        }


        // Set in new image
        SetRGBValues(DestinationImage, x, y, G, G, G);


        }
    }

   Image = DestinationImage;

}

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

void BitmapImage::NormaliseImage(){
    CreateGrayscaleMapArray();
    int iMin = 255, iMax = 0, nMin = 0, nMax = 100, Intensity, NewIntensity;
    // Find max/min of image
    for(int y = 0; y < Image->h; y++){
        for(int x = 0; x < Image->w; x++){
            Intensity = GetGrayscaleMapValue(x, y);
            if(Intensity > iMax){
                iMax = Intensity;
            }
            if(Intensity < iMin){
                iMin = Intensity;
            }
        }
    }
    // Calculate normalised intensity for image
    for(int y = 0; y < Image->h; y++){
        for(int x = 0; x < Image->w; x++){
            Intensity = GetGrayscaleMapValue(x, y);
            NewIntensity = (Intensity-iMin)*(nMax-nMin)/(iMax-iMin)+nMin;
            SetRGBValues(Image, x, y, NewIntensity, NewIntensity, NewIntensity);
        }
    }


}

void BitmapImage::HistogramEqualisation(){
    CreateGrayscaleMapArray(); // Ensure we have latest grayscale map
    float Histogram[256][3] = {0}; // 0 = count, 1 = cdf, 2 = cdfscaled
    int Grayscale;

    // Create Histogram CDF
    for(int y = 0; y < Image->h; y++){
        for(int x = 0; x < Image->w; x++){
            Grayscale = GetGrayscaleMapValue(x, y);
            Histogram[Grayscale][0]++; // Add one to grayscale count
        }
    }

    // Compute CDF
    int CDFMin = 0, PixelCount = 0;
    for(int i = 1; i < 256; i++){

        Histogram[i][1] = Histogram[i][0] + Histogram[i-1][1]; // Compute CDF


        if(CDFMin == 0 && Histogram[i][1] > 0){
            CDFMin = Histogram[i][1]; // Lowest CDF Value has just been set
        }

        if(CDFMin != 0){    // Calculate the histogram normalisation
            Histogram[i][2] = (int)ceil(((Histogram[i][1]-CDFMin)/((Image->h*Image->w)-CDFMin))*255);
        }   // Otherwise we leave CDFScaled as 0 (from initialisation)

    }

    // Set the normalised Image Values
    for(int y = 0; y < Image->h; y++){
        for(int x = 0; x < Image->w; x++){
            Grayscale = GetGrayscaleMapValue(x, y);
            SetRGBValues(Image, x, y, Histogram[Grayscale][2], Histogram[Grayscale][2], Histogram[Grayscale][2]);
        }
    }
}

void BitmapImage::ImageToAdaptiveMonochrome(int SampleArea, bool Inverted){
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
            if(Inverted == false){
                SetRGBValues(Image,x,y,255,255,255); // White
            }else{
                SetRGBValues(Image,x,y,0,0,0);  // Black
            }
        }else{
            if(Inverted == false){
                SetRGBValues(Image,x,y,0,0,0);  // Black
            }else{
                SetRGBValues(Image,x,y,255,255,255); // White
            }
        }



        }
    }

}

/* Image Analysis Functions */
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

int  BitmapImage::GetGrayscaleMapValue(int x, int y){
    // Do we need to create a map of grayscale values?
    if(GrayscaleArray.size() != Image->w*Image->h){
        CreateGrayscaleMapArray();
    }
    if(x < 0 || y < 0 || x > Image->w-1 || y > Image->h-1){
        return 0;       // The values are to the top/left of the image
    }else{
        return GrayscaleArray[(Image->w*y)+x];
    }
}



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

int  BitmapImage::GetIntegralValue(int x, int y){
    if(x < 0 || y < 0){
        return 0;       // The values are to the top/left of the image
    }else{
        return IntegralArray[(Image->w*y)+x];
    }
}

int  BitmapImage::GetIntegralAreaAverage(int x, int y, int Area){
    int a, b, c, d, i, o; // i = inside sitting | o = outside sitting

    /*
     If any of the boundaries go out of range area*area reposition (x,y)
     to acheive the area*area average.
    */
    i = Area/2; // Rounded down
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
    return (a+d-c-b)/((Area*Area)+Area);
}

