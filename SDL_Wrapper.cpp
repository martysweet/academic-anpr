#include "SDL_Wrapper.h"
#include <stdio.h>
#include <string.h>

enum WRAP_SDL{
	WRAP_SDL_INITIALIZED,
	WRAP_SDL_NOT_INITIALIZED
};

SDL_Surface* Window = NULL;
Uint8 SDLInitialized = WRAP_SDL_NOT_INITIALIZED;

// Load Image
SDL_Surface* LoadImage(const char* ImageLocation)
{
	// Initialize Variables
	SDL_Surface *SDL_S_Return = NULL;

	// Load Image
	SDL_S_Return = IMG_Load(ImageLocation);

	// If it hasn't been loaded...
	if(SDL_S_Return == NULL)
	{
		// Send out an Error Message
		fprintf(stderr, "Error: %s\n", IMG_GetError());
	}

	// Return the Surface (Will be NULL if file isn't loaded)
	return SDL_S_Return;
}

// Create New Image
SDL_Surface* CreateNewImage(int Width, int Height)
{
	return SDL_CreateRGBSurface(0, Width, Height, 24, 0, 0, 0, 0);
}

// Convert To Array
Uint8* GetPixelArray(SDL_Surface* LoadedImage, int *Width, int *Height)
{
	// Initialize Variables
	Uint8* Pixl_Return = NULL;

	// Check if Image is actually loaded
	if(LoadedImage == NULL)
		return NULL;

	// Get Pixels
	Pixl_Return = (Uint8*) LoadedImage->pixels;

	// Set Width and Height
	*Width = LoadedImage->w;
	*Height = LoadedImage->h;

	// Return Array
	return Pixl_Return;
}

// Get Pixel
Uint8* GetPixelByteAtPosition(SDL_Surface* LoadedImage, int X, int Y)
{
	int bpp = LoadedImage->format->BytesPerPixel;

    return (Uint8*) LoadedImage->pixels + Y * LoadedImage->pitch + X * bpp;
}

// Get RGB Values of a Pixel
void GetRGBValues(SDL_Surface* LoadedImage, int X, int Y, Uint8 *R, Uint8 *G, Uint8 *B)
{
	// Initialize Variables
	Uint32 Pxl_Colour;
	Uint8* Pxl = GetPixelByteAtPosition(LoadedImage, X, Y);

	// Convert the Pixel into Correct Format
	switch(LoadedImage->format->BytesPerPixel)
	{
    case 1:
        Pxl_Colour = *Pxl;
        break;

    case 2:
        Pxl_Colour = *(Uint16 *) Pxl;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            Pxl_Colour = Pxl[0] << 16 | Pxl[1] << 8 | Pxl[2];
        else
            Pxl_Colour = Pxl[0] | Pxl[1] << 8 | Pxl[2] << 16;
        break;

    case 4:
        Pxl_Colour = *(Uint32 *) Pxl;
        break;

    default:
		Pxl_Colour = 0;
    }

	// Convert into RGB
	SDL_GetRGB(Pxl_Colour, LoadedImage->format, R, G, B);

}

// Get RGB Values of a Pixel
void GetRGBAValues(SDL_Surface* LoadedImage, int X, int Y, Uint8 *R, Uint8 *G, Uint8 *B, Uint8 *A)
{
	// Initialize Variables
	Uint32 Pxl_Colour;
	Uint8* Pxl = GetPixelByteAtPosition(LoadedImage, X, Y);

	// Convert the Pixel into Correct Format
	switch(LoadedImage->format->BytesPerPixel)
	{
    case 1:
        Pxl_Colour = *Pxl;
        break;

    case 2:
        Pxl_Colour = *(Uint16 *) Pxl;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            Pxl_Colour = Pxl[0] << 24 | Pxl[1] << 16 | Pxl[2] << 8 | Pxl[3];
        else
            Pxl_Colour = Pxl[0] | Pxl[1] << 8 | Pxl[2] << 16 | Pxl[3] << 24;
        break;

    case 4:
        Pxl_Colour = *(Uint32 *) Pxl;
        break;

    default:
		Pxl_Colour = 0;
    }

	// Convert into RGBA
	SDL_GetRGBA(Pxl_Colour, LoadedImage->format, R, G, B, A);
}

// Set RGB Values of a Pixel
void SetRGBValues(SDL_Surface* LoadedImage, int X, int Y, Uint8 R, Uint8 G, Uint8 B)
{
	// Initialize Variables
	Uint32 Pxl_Colour;
    Uint8 *Pxl = GetPixelByteAtPosition(LoadedImage, X, Y);

	// Convert RGB into Correct Pixel Format
	Pxl_Colour = SDL_MapRGB(LoadedImage->format, R, G, B);

	// Set Pixel depending on Bits Per Pixel
    switch(LoadedImage->format->BytesPerPixel) {
    case 1:
        *Pxl = Pxl_Colour;
        break;

    case 2:
        *(Uint16*) Pxl = Pxl_Colour;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            Pxl[0] = (Pxl_Colour >> 16) & 0xff;
            Pxl[1] = (Pxl_Colour >> 8) & 0xff;
            Pxl[2] = Pxl_Colour & 0xff;
        } else {
            Pxl[0] = Pxl_Colour & 0xff;
            Pxl[1] = (Pxl_Colour >> 8) & 0xff;
            Pxl[2] = (Pxl_Colour >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32*) Pxl = Pxl_Colour;
        break;
    }
}

void SetRGBAValues(SDL_Surface* LoadedImage, int X, int Y, Uint8 R, Uint8 G, Uint8 B, Uint8 A)
{
	// Initialize Variables
	Uint32 Pxl_Colour;
    Uint8 *Pxl = GetPixelByteAtPosition(LoadedImage, X, Y);

	// Convert RGB into Correct Pixel Format
	Pxl_Colour = SDL_MapRGBA(LoadedImage->format, R, G, B, A);

	// Set Pixel depending on Bits Per Pixel
    switch(LoadedImage->format->BytesPerPixel) {
    case 1:
        *Pxl = Pxl_Colour;
        break;

    case 2:
        *(Uint16*) Pxl = Pxl_Colour;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            Pxl[0] = (Pxl_Colour >> 24) & 0xff;
            Pxl[1] = (Pxl_Colour >> 16) & 0xff;
            Pxl[2] = (Pxl_Colour >> 8) & 0xff;
			Pxl[3] = Pxl_Colour & 0xff;
        } else {
            Pxl[0] = Pxl_Colour & 0xff;
            Pxl[1] = (Pxl_Colour >> 8) & 0xff;
            Pxl[2] = (Pxl_Colour >> 16) & 0xff;
			Pxl[3] = (Pxl_Colour >> 24) & 0xff;
        }
        break;

    case 4:
        *(Uint32*) Pxl = Pxl_Colour;
        break;
    }
}

// Save Image as BMP
void SaveImageBMP(SDL_Surface* LoadedImage, const char * ImageLocation)
{
	SDL_SaveBMP(LoadedImage, ImageLocation);
}

// Create Window
void CreateWindowFlags(const char * WindowName, int Width, int Height, Uint32 SDLFlags)
{
	if (SDLInitialized == WRAP_SDL_NOT_INITIALIZED)
	{
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);
		SDLInitialized = WRAP_SDL_INITIALIZED;
	}

	SDL_WM_SetCaption(WindowName, WindowName);

	Window = SDL_SetVideoMode(Width, Height, 0, SDLFlags);
}

// Resize Window
void ResizeWindowFlags(int Width, int Height, Uint32 SDLFlags)
{
	if (SDLInitialized == WRAP_SDL_NOT_INITIALIZED)
	{
		printf("!!! Cannot resize a window that does not exist. Call CreateWindow first. !!!\n");
		printf("Press Enter to Close the Program\n");
		getchar();
		exit(1);
	}

	Window = SDL_SetVideoMode(Width, Height, 0, SDLFlags);
}

// Check Close Pressed
Uint8 CheckClosePressed(SDL_Event* Event)
{
	if (Event->type == SDL_QUIT)
	{
		return 1;
	}

	return 0;
}

// Close Window
void CloseWindow()
{
	if (SDLInitialized == WRAP_SDL_INITIALIZED)
	{
		SDL_Quit();
		Window = NULL;
		SDLInitialized = WRAP_SDL_NOT_INITIALIZED;
	}
}

// Display Surface
void DisplaySurface(SDL_Surface * SurfaceToDisplay)
{
	// Check if the window exists
	if (SDLInitialized == WRAP_SDL_NOT_INITIALIZED)
	{
		printf("!!! Error: Cannot display a surface before creating window. Call CreateWindow first. !!!\n");
		printf("Press Enter to Close the Program\n");
		getchar();
		exit(1);
	}

	int winWidth = Window->clip_rect.w;
	int winHeight = Window->clip_rect.h;

	int surWidth = SurfaceToDisplay->clip_rect.w;
	int surHeight = SurfaceToDisplay->clip_rect.h;

	// Display warning if the width/height of the surface and the window are different
	if ((winWidth != surWidth) || (winHeight != surHeight))
	{
		printf("!!! Warning: Surface has a different width/height than window. !!!\n");
	}

	// Blit the surface to the window's surface
	SDL_BlitSurface(SurfaceToDisplay, NULL, Window, NULL);

	// Tell the window to display the surface on the screen
	SDL_UpdateRect(Window, 0, 0, 0, 0);
}

// Get Window Surface
SDL_Surface* GetWindowSurface()
{
	return Window;
}
