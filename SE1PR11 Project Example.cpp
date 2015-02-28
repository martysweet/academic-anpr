#include <iostream>
#include <string>
#include <vector>

#include "SDL_Wrapper.h"
using namespace std;

void DisplaySurfaceUntilClose(SDL_Surface* Surface);

int a(int argc, char * argv[])
{
	string Location = "Lighthouse.jpg";

	// Load in the Image
	cout << "Loading Image : " << Location << endl;
	SDL_Surface* ImageSurface = LoadImage(Location.c_str());

	// Check if the image has been loaded correctly
	if (ImageSurface == NULL)
	{
		cout << "!!! Unable to Load Image. !!!" << endl;
		cout << "Press Enter to Exit the Program." << endl;
		getchar();
		return 1;
	}

	// Get The Array of the Pixels (Access to the Raw Data Pointer)
	//
	// This is a 1D array, however represents the entire image.
	int width = 0, height = 0;
	Uint8* ImageArray = GetPixelArray(ImageSurface, &width, &height);

	// Creates a window called 'Lighthouse' at the same width and height as the original image
	CreateWindow("Lighthouse", width, height);
	cout << "To continue: close the window, or press ESC in window." << endl;
	DisplaySurfaceUntilClose(ImageSurface); // Display the image in the window
	CloseWindow(); // Close the window (you don't have to do this, this is just an example of what it does).

	cout << endl;

	// Gets the RGB Value from a Pixel Location
	Uint8 R = 0, G = 0, B = 0;
	GetRGBValues(ImageSurface, 10, 10, &R, &G, &B);

	cout << "RGB Values at Position 10,10 are : " << (int) R << ", " << (int) G << ", " << (int) B << endl;

	// Sets the RGB Value at a Pixel Location
	for(int y = ImageSurface->h - 1; y >= 0; y--)
	{
		for(int x = 0; x < ImageSurface->w; x++)
		{
			SetRGBValues(ImageSurface,x,y,255,0,0);
		}
	}

	// Creates a window called 'Red Image' at the same width and height as the original lighthouse image
	CreateWindow("Red Image", width, height);
	cout << "To continue: close the window, or press ESC in window." << endl;
	DisplaySurfaceUntilClose(ImageSurface);
	CloseWindow();

	// Saves the Image
	SaveImageBMP(ImageSurface, "Red.bmp");

	// Creates a New Image
	SDL_Surface* newImage = CreateNewImage(200,200);

	// Sets Everything to Blue in the new image
	for(int y = newImage->h - 1; y >= 0; y--)
	{
		for(int x = 0; x < newImage->w; x++)
		{
			SetRGBValues(newImage,x,y,0,0,255);
		}
	}

	// Creates a window called 'Blue Image' at the same width and height as the original lighthouse image
	CreateWindow("Blue Image", width, height);
	cout << "To continue: close the window, or press ESC in window." << endl;
	DisplaySurfaceUntilClose(newImage); // Note: The DisplaySurface() function from the wrapper called within displays a warning because newImage is a different size to the window
	CloseWindow();

	// Saves the Image
	SaveImageBMP(newImage, "Blue.bmp");

	// IMPORTANT : Use SDL_FreeSurface once you have finished with any image (including the ones you have loaded up)
	SDL_FreeSurface(ImageSurface);
	SDL_FreeSurface(newImage);

	return 0;
}

void DisplaySurfaceUntilClose(SDL_Surface* Surface)
{
	int open = 1; // In C++, one would ordinarily use bool for this. In C, something like this has to be used.
	SDL_Event event;

	// Display the image in the window
	DisplaySurface(Surface);

	// While we aren't closing the window...
	while (open == 1)
	{
		if (SDL_PollEvent(&event))
		{
			// Close the window if it is closed
			if (CheckClosePressed(&event))
			{
				open = 0;
				continue;
			}

			/* This switch statement shows an example of how to check keys that the user has pressed. Most keys on a keyboard can be checked using the SDLK_<key> enum values,
			   for instance SDLK_q specifies the 'q' key, or SDLK_RETURN specifies the return/enter key (not the one on the number pad).

			   If you are interested, many other events can be checked; however you will have to research the SDL 1.2 documentation yourself! */
			switch (event.type)
			{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					open = 0;
					break;
				default:
					break;
				}

				break;
			default:
				break;
			}
		}
	}
}
