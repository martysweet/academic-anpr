#include "BitmapImage.h"

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
    LoadedImage = SDL_ConvertSurface(Image, Image->format, Image->flags); // Duplicate Loaded Image incase of needed restore
}

void BitmapImage::LoadBitmapImage(SDL_Surface* InputImage){
    Rectangle R;
    R.x = 0;
    R.y = 0;
    R.Width = InputImage->w;
    R.Height = InputImage->h;
    LoadBitmapImage(InputImage, R);
}

void BitmapImage::CreateBitmapImage(int w, int h, bool White){
    Image = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0 ,0);
    if(White){
        for(int y = 0; y <= Image->h-1; y++){
            for(int x = 0; x <= Image->w-1; x++){
                SetRGBValues(Image, x, y, 255, 255, 255);
            }
        }
    }
    LoadedImage = SDL_ConvertSurface(Image, Image->format, Image->flags);
}

void BitmapImage::RestoreToLoadedImage(){
    SDL_FreeSurface(Image); // Remove the image data
    Image = SDL_ConvertSurface(LoadedImage, LoadedImage->format, LoadedImage->flags); // Load the saved data into Image
}

void BitmapImage::GetBitmapSurface(SDL_Surface* OutputSurface){
    OutputSurface = SDL_ConvertSurface(Image, Image->format, Image->flags); // Load the saved data into Image
}

void BitmapImage::DebugDisplayImageToWindow(std::string Title){
    CreateWindowFlags(Title.c_str(), Image->w, Image->h, 0);
    DisplaySurfaceUntilClose(Image);
    CloseWindow();
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

void BitmapImage::DrawLine(Point Point1, Point Point2, Uint8 R, Uint8 G, Uint8 B){
    DrawLine(Point1.x, Point1.y, Point2.x, Point2.y, R, G, B);
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

void BitmapImage::SetPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B){
    SetRGBValues(Image, x, y, R, G, B);
}

/* Whole Image Editing Function */
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

void BitmapImage::NormaliseImage(int nMax){
    CreateGrayscaleMapArray();
    int iMin = 255, iMax = 0, nMin = 0, Intensity, NewIntensity;
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
