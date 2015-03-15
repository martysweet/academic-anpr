#include "BitmapImage.h"

BitmapImage::BitmapImage(){

    // Create needed Convolution Matricies
    // This is done here to allow difference sizes of matrices in one datatype
    InitialiseConvolutionMatrix(SobelHorizontal,    3, 3, 1,  {-1,  -2,  -1,     0,  0,  0,      1,  2,  1});
    InitialiseConvolutionMatrix(SobelVertical,      3, 3, 1,  {-1,   0,  1,     -2,  0,  2,     -1,  0,  1});

    InitialiseConvolutionMatrix(CannyX,      3, 3, 1,  {-1,0,1,-2,0,2,-1,0,1});
    InitialiseConvolutionMatrix(CannyY,      3, 3, 1,  {-1,-2,-1,0,0,0,1,2,1});

    InitialiseConvolutionMatrix(GaussianBlur1,      5, 5, 159,{2,4,5,4,2,4,9,12,9,4,5,12,15,12,5,4,9,12,9,4,2,4,5,4,2});
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

int  BitmapImage::GetIntegralValue(int x, int y){
    if(x < 0 || y < 0){
        return 0;       // The values are to the top/left of the image
    }else{
        return IntegralArray[(Image->w*y)+x];
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

int  BitmapImage::GetGrayscaleMapValue(int x, int y){
    if(x < 0 || y < 0 || x > Image->w-1 || y > Image->h-1){
        return 0;       // The values are to the top/left of the image
    }else{
        return GrayscaleArray[(Image->w*y)+x];
    }
}

float  BitmapImage::ProcessKernelFilter(int x, int y, ConvolutionMatrix Kernel){

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

void BitmapImage::LocateHoughLines(std::vector<HoughPoint> & HoughPoints, float LengthThreshold){
    CreateGrayscaleMapArray();
    int MaxCount = 0, r = 0;

    // Create voting matrix

    int MaxLineLength = sqrt(Image->h*Image->h + Image->w*Image->w)+5;
    std::vector<std::vector<int>> VotingMatrix;
    VotingMatrix.resize( 180  , std::vector<int>( MaxLineLength*2 , 0 ) );

   // auto *VotingMatrix = new int[180][750];

    // Go through image computing angles for each edge
    for(int y = 0; y < Image->h; y++){
        for(int x = 0; x < Image->w; x++){

            if(GetGrayscaleMapValue(x, y) > 250){
               for(int t = 0; t < 180; t++){
                // Add r to accumulator
                r = (x*cos(t*0.017432925) + y*sin(t*0.0174532925))+MaxLineLength; // So negative MaxLineLength == VM(t,0)
                VotingMatrix[t][r]++;
                if( VotingMatrix[t][r] > MaxCount )
                    MaxCount = VotingMatrix[t][r];

               }
            }

        }
    }

    // Cleanup Found Lines
    HoughPoint HPoint; // Temp Struct
    for(int t=0; t < 180; t++){
        for(int r=0; r < MaxLineLength*2; r++){
            if((VotingMatrix[t][r] > MaxCount*LengthThreshold)){
                HPoint.t = t;
                HPoint.r = r-MaxLineLength; // Allow negative rhos
                HoughPoints.push_back(HPoint);
            }
        }
    }
}


#include "ANPRImage.h"




void ANPRImage::ProcessGlobalImage(){
    // Convert the image to Grayscale
    ImageToGrayscale();
   // ImageGrayscaleToDifferentialMonochrome();
    //SobelEdgeDetection();
 //   EdgeDetection();

    //


    NormaliseImage();
    HistogramEqualisation();
    CannyEdgeDetection();


    std::vector<HoughPoint> HoughPoints;
    LocateHoughLines(HoughPoints, 0.55);

    // Horixontal Banding
    std::vector<int> HorizontalBands;
    HorizontalBands.resize((int)ceil(Image->h/5), 0); // Devide image into chunks of 5 pixels

for(int i=0; i<HoughPoints.size(); i++){


    float Px, Py, p1x, p2x, p1y, p2y, a, b;
    float x0, y0;


    float t = HoughPoints[i].t;
    float r = HoughPoints[i].r;

    if(t == 0){
        p1x = r;
        p2x = r;
        p1y = 0;
        p2y = Image->h;
           DrawLine((int)p1x, (int)p1y, (int)p2x, (int)p2y, 255, 0, 0);
    }else{

        a = cos(t*0.0174532925);
        b = sin(t*0.0174532925);

        x0 = a*r;
        y0 = b*r;

        p1x = x0 + 6000*(-b);
        p1y = y0 + 6000*(a);

        p2x = x0 - 6000*(-b);
        p2y = y0 - 6000*(a);

  //  std::cout << (int)p1x << "," << (int)p1y << " " << (int)p2x << "," << (int)p2y << std::endl;
    DrawLine((int)p1x, (int)p1y, (int)p2x, (int)p2y, 0, 255, 0);
    }

    if((t > 88) && (t < 92)){   // Consider this line to be horizontal
        float yAvg;
        // Calculate average y for horizontal banding
        yAvg = (p1y+p2y)/2;
        HorizontalBands[(int)yAvg/5]++; // Add a vote to line group

    }





}

    // Lets try do some magic
    std::vector<Rect> Regions;
    Rect HorizonalBand; // Temp variable
    int TopLine = -1, BottomLine = -1, Height; // Line position in pixels
    for(int i = 0; i < HorizontalBands.size(); i++){
        //std::cout << i << " " << HorizontalBands[i] << std::endl;
        if(TopLine < 0 && HorizontalBands[i] != 0){
            // We have a line which we shall consider to be our top horizontal line
            TopLine = i*5;
        }
        else if(BottomLine < 0 && HorizontalBands[i] != 0){
            BottomLine = i*5;
        }
        else if(BottomLine < 0 && i == HorizontalBands.size()-1){ // Last iteration we need to stop counting lines
            BottomLine = i*5;
        }

        if(TopLine >= 0 && BottomLine > 0){
            // We have a region to consider
            std::cout << TopLine << "->" << BottomLine << std::endl;
            DrawLine(10+i, TopLine, 10+i, BottomLine, 255, 0, 0);
            Height = BottomLine-TopLine;

            // We've only gone and done it
            if(Height > 10){
                HorizonalBand.x = 0;
                HorizonalBand.y = TopLine;
                HorizonalBand.Height = Height;
                HorizonalBand.Width = Image->h;
                Regions.push_back(HorizonalBand);
            }

            TopLine = BottomLine;
            BottomLine = -1;
        }





    }


SaveImageToFile("output.bmp");
    /*

    int maxLength = sqrt(Image->w*Image->w + Image->h*Image->h);


    for(int i=0; i<Lines.size(); i++){

    int Px, Py, p1x, p2x, p1y, p2y;
    std::cout << Lines[i].Distance << " " << Lines[i].Angle << std::endl;

    int th = Lines[i].Angle;
    int r  = Lines[i].Distance;

    if(Lines[i].Angle == 0){
        int p1x = Lines[i].Distance;
        int p2x = Lines[i].Distance;
        int p1y = 0;
        int p2y = Image->h;
    }else{
        int p1x = 0;
        int p2x = Lines[i].Distance;
        int p1y = (-cos(th)/sin(th)) + (r / sin(th));
        int p2y = (-cos(th)/sin(th))*Image->w + (r / sin(th));
    }

    int px  = Lines[i].Distance * cos(Lines[i].Angle);
    int py  = Lines[i].Distance * sin(Lines[i].Angle);


    int p1x = px + maxLength * cos(Lines[i].Angle);
    int p1y = py + maxLength * sin(Lines[i].Angle);


    std::cout << " = " << p1x << " " << p1y << std::endl;

    int p2x = px - maxLength * cos(Lines[i].Angle);
    int p2y = py - maxLength * sin(Lines[i].Angle);

    if(p1x > Image->w){
        p1x = Image->w;
    }
    if(p2x > Image->w){
        p2x = Image->w;
    }
    if(p1y > Image->h){
        p1y = Image->h;
    }
    if(p2y > Image->h){
        p2y = Image->h;
    }

    if(p1x < 0){
        p1x = 0;
    }
    if(p2x < 0){
        p2x = 0;
    }

    if(p1y < 0){
        p1y = 0;
    }
    if(p2y < 0){
        p2y = 0;
    }

    DrawLine(p1x, p1y, p2x, p2y, 255, 0, 0);
   // DrawLine(0, 500, 500, 500, 0, 255, 0);

   //SetRGBValues(Image, p1x, p1y, 255, 0, 0);

    }
*/


// Has m exceeded our edge threshold?
//if(PeakRegions[i].Middle.Value > EdgeThreshold){
    // Has the height exceeded our threshold?
    //if((PeakRegions[i].Upper.Coordinate - PeakRegions[i].Lower.Coordinate) > HeightThreshold){
        // Preform intensity analysis



    //}
//}

/*
    // Do two consectutive pixels have a high difference in grayscale?
    int lastPixel = 0;
    int pixel = 0;
    for(int y=0; y<Image->h; y++){
        GetRGBValues(Image, 0, y, &R, &G, &B);
        lastPixel = R; // Image is grayscale so only have to look at one value
        for(int x=1; x<Image->w; x++){
            GetRGBValues(Image, x, y, &R, &G, &B);
            pixel = R;

            // Is there a large difference
            if(abs(lastPixel - pixel) > 5){
                // Replace last pixel with black
                SetRGBValues(Image, x-1, y,0,0,0);
            }else{
                SetRGBValues(Image, x-1, y,255,255,255);
            }

            lastPixel = pixel;
        }
    }
*/


/*


    // Sobel Horizontal Mask
    int Gx[3][3] =  {   -1, -2, -1,
                         0,  0,  0,
                         1,  2,  1   };

    // Sobel Vertical Mask
    int Gy[3][3] =  {   -1,  0,  1,
                        -2,  0,  2,
                        -1,  0,  1   };

    // Scan x
     lastPixel = 0;
     pixel = 0;
    int* rowProfile2 = new int[Image->w];

    for(int i=0; i<Image->w; i++){
        rowProfile2[i] = 0;
    }

    for(int x=0; x<Image->w; x++){
        for(int y=0; y<Image->h; y++){
            GetRGBValues(Image, x, y, &R, &G, &B);
            if(R == 255 && G == 255 && B == 255){
                pixel = 255; // White
            }else{
                pixel = 0; //Black
            }
            if(pixel != lastPixel){
                rowProfile2[x]++; // We have detected an edge
            }
            lastPixel = pixel;
        }

        // Score on the image
        for(int y=0; y<rowProfile2[x]; y++){
            SetRGBValues(Image, x, y, 255, 255, 0);
        }
    }






    // Find the boundaries of the 3 highest value
    for(int i=0; i<10; i++){

        // Row is high
        int highRow = 0;
        int highValue = 0;
        for(int i=0; i<Image->h; i++){

            if(rowProfile[i] > highValue){
                highRow = i;
                highValue = rowProfile[i];
            }

        }

        // Find yb0
        int yb0 = 0; // Row ID
        for(int b0=0; b0<highRow; b0++){
            if(rowProfile[b0] <= 0.55*highValue){
                yb0 = b0;
            }
        }

        // Find yb1
        int yb1 = 0;
        int yb1v = getHighValue(rowProfile, Image->h);
        for(int b1=highRow; b1<Image->h; b1++){
            if(rowProfile[b1] <= 0.55*highValue){
                if(rowProfile[b1] < yb1v){
                    yb1 = b1;
                    yb1v = rowProfile[b1];
                }

            }
        }

        // Draw rectangle
        drawRectangle(Image, 0, yb0, Image->w-1, yb1-yb0, 0, 255, 0);

        // Zero values between yb0 and yb1
        for(int z=yb0; z<yb1; z++){
            rowProfile[z] = 0;

            //for(int x=200; x<Image->w; x++)
              //  SetRGBValues(Image, x, z, i*85, i*85, 128);
                // Create new image of each segment
                // Create new image from function
                // Save smaller image object to list
                // Do the same process with constant 0.42
                // Check if we have the properities of a number plate7
                    // Create preferenvce score

        }



    }



    //drawRectangle(Image, 200, 200, 600, 800, 255, 0, 255);
   // drawLine(Image, 600, 600, 900, 900, 255, 0 , 0);


    // Output the image
    SaveImageBMP(Image, "output.bmp");

*/





/*
    for(int i = 0; i<ImageSurface->w*ImageSurface->h; i++){
     cout << integralImage[i];
    }

    // Localised monochroming
    for(int y = (ImageSurface->h - 1)/2; y >= 0; y--)
    {
        for(int x = 0; x < ImageSurface->w; x++)
        {


            // Sum surrounding pixels
            // sum = D-C-B+A
            // int[width*height]
            // 11x11 around pixel
           // int a,b,c,d;




            //threshold  = sum/sampled; // Devide by pixels

            // RGB of this pixel
            GetRGBValues(ImageSurface, x, y, &R, &G, &B);
            if( (R+G+B)/3 > 128 ){
                SetRGBValues(ImageSurface,x,y,255,255,255);
            }else{
                SetRGBValues(ImageSurface,x,y,0,0,0);
            }

        }
    }

    for(int y=0; y<ImageSurface->h/2; y++){
        for(int x=0; x<ImageSurface->w; x++){
            SetRGBValues(ImageSurface,x,y,255,255,255);
        }
    }


    // Create Integral Image
    int* integralImage = new int[Image->w*Image->h];
    int element = 0;

    for(int y=0; y<Image->h; y++){
        for(int x=0; x<Image->w; x++){

            // Reset Values
            sum = 0;

            // Get values in x,y box from origin
            // S(x,y) = i(x,y) + s(x-1,y) + s(x,y-1) - s(x-1,y-1) (Remove the repeated segment)
            GetRGBValues(Image, x, y, &R, &G, &B);
            integralImage[element] = R+G+B+ii(integralImage, Image, x-1,y)+ii(integralImage, Image, x,y-1)-ii(integralImage, Image, x-1, y-1);

            // Increase pixel location
            element++;

        }
    }


    // Adaptive monochroming
    int a,b,c,d;
    for(int y=0; y<Image->h; y++){
        for(int x=0; x<Image->w; x++){

        // Some range around the pixel.
        a = s(Image, integralImage, x-7, y-7);
        b = s(Image, integralImage, x+6, y-7);
        c = s(Image, integralImage, x-7, y+6);
        d = s(Image, integralImage, x+6, y+6);

        GetRGBValues(Image, x, y, &R, &G, &B);
        sum = R+G+B;
        int val = (d+a-c-b)/(12*12);
        if( (R+G+B) > val ){
            SetRGBValues(Image,x,y,255,255,255);
        }else{
            SetRGBValues(Image,x,y,0,0,0);
        }

         // Output the image
    //   SaveImageBMP(Image, "output.bmp");


        }
    }

    // Force 4px border
    for(int y=0; y<5; y++){
         for(int x=0; x<Image->w; x++){
            SetRGBValues(Image, x, y, 0, 0, 0);
         }
    }



    // Scan for edges in y direction
    int lastPixel = 0;
    int pixel = 0;
    int* rowProfile = new int[Image->h];

    for(int i=0; i<Image->h; i++){
        rowProfile[i] = 0;
    }


    for(int y=0; y<Image->h; y++){
        for(int x=0; x<Image->w; x++){
            GetRGBValues(Image, x, y, &R, &G, &B);
            if(R == 255 && G == 255 && B == 255){
                pixel = 255; // White
            }else{
                pixel = 0; //Black
            }
            if(pixel != lastPixel){
                rowProfile[y]++; // We have detected an edge
            }
            lastPixel = pixel;
        }
    }

    int highRow = 0;
    int highValue = 0;
    for(int i=0; i<Image->h; i++){

        //cout << rowProfile[i];
        if(rowProfile[i] > highValue){
            highRow = i;
            highValue = rowProfile[i];
        }

    }

    // Draw on recommended guide
    for(int x=0; x<Image->w; x++){
        SetRGBValues(Image, x, highRow, 0, 255, 0);
    }


*/


    /*
    // Prepare the image
    std::vector< ConvolutionMatrix > MyFilters;
    MyFilters.push_back(GaussianBlur1);
    MyFilters.push_back(Median);

    ImageConvolutionMatrixTransformation(MyFilters);

   // ImageToAdaptiveMonochrome();
   //CreateIntensityRowProfileY();


  //  VerticalBandDetection(4);
   // DrawVerticalBandDetection();
    SaveImageToFile("output.bmp");

    MyFilters.clear();
    MyFilters.push_back(SobelHorizontal);
    MyFilters.push_back(SobelVertical);
    ImageConvolutionMatrixTransformation(MyFilters);

    ImageToBW(100);

 //   ImageConvolutionMatrixTransformation(MyFilters);

    SaveImageToFile("output2.bmp");


    MyFilters.clear();
    MyFilters.push_back(EdgeHorizontal);
    MyFilters.push_back(EdgeVertical);
    ImageConvolutionMatrixTransformation(MyFilters);
*/
    SaveImageToFile("output3.bmp");
/*
 MyFilters.clear();
    MyFilters.push_back(EdgeHorizontal);
    ImageConvolutionMatrixTransformation(MyFilters);

    SaveImageToFile("output4.bmp");

*/
    CreateIntensityRowProfileY();


  //  VerticalBandDetection(4);
   // DrawVerticalBandDetection();


    AnalyseBandBoundary();
    SaveImageToFile("output4.bmp");

/*
    CreateIntensityRowProfileY();
    CreateRowProfileY();

    VerticalBandDetection(4);

*/


}





int  ANPRImage::GetMaxArrayValue(std::vector<int> intArray){
    int m = 0;
    for(int i=0; i<intArray.size(); i++){
        if(intArray[i] > m)
            m = intArray[i];
    }
    return m;
}

void ANPRImage::CreateRowProfileY(bool GraphicalOutput){
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

        if(GraphicalOutput == true){
            // Score on the image
            for(int x=0; x < RowProfileY[y]; x++){
                SetRGBValues(Image, x, y, 255, 0, 0);
            }
        }
    }
}

void ANPRImage::VerticalBandDetection(std::vector<int> RowProfile, int Iterations, float Constant){
    // Initialise the Vector
    BandBoundariesY.resize(BandBoundariesY.size()+1);
    // Find the boundaries of the Iterations highest value
    for(int i = 1; i <= Iterations; i++){ // Start at 1 as struct vector does

        int ybm = 0;    // Middle Pointer
        int ybmv = 0;   // Middle Value

        // Find the highest row ybm
        for(int i = 0; i < RowProfile.size(); i++){
            if(RowProfile[i] > ybmv){
                ybm = i;
                ybmv = RowProfile[i];
            }
        }

        // Find yb0
        int yb0 = 0; // Row ID
        for(int b0 = 0; b0 < ybm; b0++){
            if(RowProfile[b0] <= Constant*ybmv){
                yb0 = b0;
            }
        }

        // Find yb1
        int yb1 = 0; // Row ID
        int yb1v = ybmv; // Maximum value in array
        for(int b1 = ybm; b1 < RowProfile.size(); b1++){
            if(RowProfile[b1] <= (Constant*ybmv)){
                if(RowProfile[b1] < yb1v){
                    yb1 = b1;
                    yb1v = RowProfile[b1];
                }
            }
        }

        // We have the lower, middle and upper value
        // Sum values and add to array of struct
        // Zero values between yb0 and yb1, add to Area
        int AreaSum = 0;
        if(yb1 == 0){
            yb1 = ybm;
        }
        for(int z=yb0; z<yb1; z++){
            AreaSum += RowProfile[z];
            RowProfile[z] = 0;
        }

        if(AreaSum != 0){
            BandBoundariesY.resize(RowProfile.size()+1);
            BandBoundariesY[i].b0 = yb0;
            BandBoundariesY[i].bm = ybm;
            BandBoundariesY[i].b1 = yb1;
            BandBoundariesY[i].Area = AreaSum;
        }
    }
}

void ANPRImage::DrawVerticalBandDetection(){
    for(int i = 0; i < BandBoundariesY.size(); i++){
       //DrawLine(20, BandBoundariesY[i].b0, Image->w-21, BandBoundariesY[i].b1, 255, 0, 255);
        DrawRectangle(20, BandBoundariesY[i].b0, Image->w-21, BandBoundariesY[i].b1-BandBoundariesY[i].b0, 0, 255, 255);
    }
    std::cout << BandBoundariesY.size()-1;
}

void ANPRImage::CreateIntensityRowProfileY(){
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

void ANPRImage::AnalyseBandBoundary(){
    Uint8 R = 0, G = 0, B = 0;
    // Assume Y for now
    for(int i = 1; i < BandBoundariesY.size(); i++){
        // We have a boundary to work with

        // Create new image
        SDL_Surface* Band = CreateNewImage(Image->w, BandBoundariesY[i].b1-BandBoundariesY[i].b0);

        std::cout << "Image b0: " << BandBoundariesY[i].b0 << std::endl;
        std::cout << "Image b1: " << BandBoundariesY[i].b1 << std::endl;

        // Copy area into image
        int SetY = 0;
        for(int y = BandBoundariesY[i].b0; y < BandBoundariesY[i].b1; y++){
            for(int x = 0; x < Image->w; x++){
                GetRGBValues(Image, x, y, &R, &G, &B);
                SetRGBValues(Band, x, SetY, R, G, B);
            }
            SetY++;
        }



     // Save the image
     std::stringstream SavePath;
     SavePath << "Boundary" << i << ".bmp";
     std::string myString = SavePath.str();
     SaveImageBMP(Band, myString.c_str());
     SDL_FreeSurface(Band);


    }

}

void ANPRImage::CreateRowProfileX(){
    Uint8 R = 0, G = 0, B = 0;
    int Pixel, LastPixel;
    RowProfileX.resize(Image->w);
    std::fill(RowProfileX.begin(), RowProfileX.end(), 0); // Set all values to zero

    for(int x=0; x < Image->w; x++){
        for(int y=0; y < Image->h; y++){
            GetRGBValues(Image, x, y, &R, &G, &B);
            if(R == 255 && G == 255 && B == 255){
                Pixel = 255; // White
            }else{
                Pixel = 0;   // Black
            }
            if(Pixel != LastPixel){
                RowProfileX[y]++; // We have detected an edge
            }
            LastPixel = Pixel;
        }

        // Score on the image
       for(int y=0; y < RowProfileX[x]; y++){
           SetRGBValues(Image, x, y, 255, 0, 0);
       }
    }
}

void ANPRImage::CreateIntensityRowProfileX(bool GraphicalOutput){
    Uint8 R = 0, G = 0, B = 0;
    IntensityRowProfileX.resize(Image->w);
    std::fill(IntensityRowProfileX.begin(), IntensityRowProfileX.end(), 0); // Set all values to zero

    for(int x=0; x < Image->w; x++){
        int Sum = 0;
        for(int y=0; y < Image->h; y++){
            GetRGBValues(Image, x, y, &R, &G, &B);
            Sum += R;
        }
        IntensityRowProfileX[x] = Sum;

        if(GraphicalOutput == true){
            // Score on the image
            for(int y=0; y < IntensityRowProfileX[x]/256; y++){
                SetRGBValues(Image, x, y, 0, 0, 255);
            }
        }
    }
}




#ifndef BITMAPIMAGE_H
#define BITMAPIMAGE_H

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "../SDL_Wrapper.h"
#include "DataStructures.cpp"



class BitmapImage
{
    public:
        BitmapImage(); // Constructor
        virtual ~BitmapImage(); // Destructor

        // Misc
        void LoadImageFromFile(std::string Filename);
        void SaveImageToFile(std::string Filename);

    /* Allow these variables to be accessed by inheritied classes */
    protected:
        SDL_Surface* Image;

        // Image editing functions
        void ImageToGrayscale();
        void SobelEdgeDetection();
        void EdgeDetection();
        void CannyEdgeDetection();
        void NormaliseImage();
        void HistogramEqualisation();
        void ImageToBW(int Threshold=200, bool Inverted=false);
        void ImageToAdaptiveMonochrome(int Area=30);
        void ImageToLocalisedMonochrome(int Threshold=80);
        void ImageGrayscaleToDifferentialMonochrome(int Difference=10);
        void DrawLine(int x1, int y1, int x2, int y2,   Uint8 R, Uint8 G, Uint8 B);
        void DrawRectangle(int x, int y, int w, int h,  Uint8 R, Uint8 G, Uint8 B);
        void ImageConvolutionMatrixTransformation(std::vector<ConvolutionMatrix> Convolutions);
        ConvolutionMatrix SobelVertical, SobelHorizontal, GaussianBlur1, GaussianBlur2, GaussianBlur3, CannyX, CannyY;
        ConvolutionMatrix EdgeHorizontal,Median, EdgeVertical, EdgeDetect3, EdgeDetect4, HorizontalRank, VerticalRank, Blur1;
        void CreateGrayscaleMapArray();
        void LocateHoughLines(std::vector<HoughPoint> & HoughPoints, float LengthThreshold=0.5);
        void ThresholdHoughPoints(std::vector<int> & HoughPoints, float Threshold, Rect Area);
    /* Don't allow these variables to be accessed by inheritied classes */
    private:
        // Core Arithmetic
        void CreateIntegralArray();
        int  GetIntegralValue(int x, int y);
        int  GetIntegralAreaAverage(int x, int y, int Area=12);
        float  ProcessKernelFilter(int x, int y, ConvolutionMatrix Kernel);
        int  GetGrayscaleMapValue(int x, int y);


        void OrderCoordinates(int &x1, int &y1, int &x2, int &y2);

        void InitialiseConvolutionMatrix(ConvolutionMatrix &Struct, int Columns, int Rows, float Weight, std::initializer_list<float> List);
        std::vector<int> IntegralArray;
        std::vector<int> GrayscaleArray;

};

#endif // BITMAPIMAGE_H

#ifndef ANPRIMAGE_H
#define ANPRIMAGE_H

#include <vector>
#include <sstream>
#include "BitmapImage.h"
#include "DataStructures.cpp"



class ANPRImage : public BitmapImage {
    public:
        void ProcessGlobalImage();


    protected:

        // Core Arithmetic Routines
        void VerticalBandDetection(std::vector<int> RowProfile, int Iterations, float Constant=0.55);
        void HorizontalBandDetection(std::vector<int> & HoughPoints);

        void DrawVerticalBandDetection();
        int  GetMaxArrayValue(std::vector<int> intArray);
        void AnalyseBandBoundary();
        void CreateIntensityRowProfileY();
        void CreateRowProfileY(bool GraphicalOutput=false);
        void CreateRowProfileX();
        void CreateIntensityRowProfileX(bool GraphicalOutput=false);




        std::vector<int> IntensityRowProfileX;
        std::vector<int> IntensityRowProfileY;
        std::vector<int> RowProfileX;
        std::vector<int> RowProfileY;
        std::vector<BandBoundary> BandBoundariesX;
        std::vector<BandBoundary> BandBoundariesY;


    private:

};

#endif // ANPRIMAGE_H

