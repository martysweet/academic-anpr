#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "SDL_Wrapper.h"
#include "include/BitmapImage.h"
#include "include/NumberPlate.h"

int main(){

    // Load Image
    std::string Location = "007.jpg";


    NumberPlate PlateInstance;
    PlateInstance.LoadImageFromFile(Location);
    PlateInstance.ProcessGlobalImage();

    return 0;
}

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



