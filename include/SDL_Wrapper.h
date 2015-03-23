/*
 * SDL_Wrapper.h
 *
 * The functions prototyped in this file are wrappers around the SDL Library to handle
 * loading, manipulating, and saving images.
 *
 * Even though this file is written in C, you may use either C or C++ for your project.
 * Just change the file extensions of your files to the correct format e.g. .c or .cpp
 */

#ifndef SDL_WRAPPER_H
#define SDL_WRAPPER_H

#include "SDL.h"
#include "SDL_image.h"

#ifdef __cplusplus
extern "C" {
#endif

// Load Image
SDL_Surface* LoadImage(const char* ImageLocation);

// Create a New Image. Make sure to call SDL_FreeSurface once you have finished with it
SDL_Surface* CreateNewImage(int Width, int Height);

// Convert To Array
Uint8* GetPixelArray(SDL_Surface* LoadedImage, int *Width, int *Height);

// Get Pixel
Uint8* GetPixelByteAtPosition(SDL_Surface* LoadedImage, int X, int Y);

// Get RGB Values of a Pixel
void GetRGBValues(SDL_Surface* LoadedImage, int X, int Y, Uint8 *R, Uint8 *G, Uint8 *B);

// Get RGB Values of a Pixel
void GetRGBAValues(SDL_Surface* LoadedImage, int X, int Y, Uint8 *R, Uint8 *G, Uint8 *B, Uint8 *A);

// Set RGB Values of a Pixel
void SetRGBValues(SDL_Surface* LoadedImage, int X, int Y, Uint8 R, Uint8 G, Uint8 B);

void SetRGBAValues(SDL_Surface* LoadedImage, int X, int Y, Uint8 R, Uint8 G, Uint8 B, Uint8 A);

// Save Image as BMP
void SaveImageBMP(SDL_Surface* LoadedImage, const char * ImageLocation);

// Create Window
void CreateWindowFlags(const char * WindowName, int Width, int Height, Uint32 SDLFlags);
#define CreateWindow(WindowName, Width, Height) CreateWindowFlags(WindowName,Width,Height,0);

// Resize Window
void ResizeWindowFlags(int Width, int Height, Uint32 SDLFlags);
#define ResizeWindow(Width, Height) ResizeWindowFlags(Width,Height,0);

// Check Close Pressed
Uint8 CheckClosePressed(SDL_Event* Event);

// Close Window
void CloseWindow();

// Display Surface
void DisplaySurface(SDL_Surface * SurfaceToDisplay);

void DisplaySurfaceUntilClose(SDL_Surface* Surface);


/* Don't call the following functions unless you know what you are doing! */

// Get Window Surface
SDL_Surface* GetWindowSurface();

#ifdef __cplusplus
}
#endif

#endif
