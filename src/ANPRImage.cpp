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




