//Using SDL and standard IO
#include <stdio.h>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Surface* load_surface(const std::string path)
{
    //Load image at specified path
    SDL_Surface* surface = IMG_Load( path.c_str() );
    if( nullptr == surface)
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    return surface;
}

SDL_Surface* optimize_surface(SDL_Surface* surface, SDL_PixelFormat* format)
{
    //Convert surface to screen format
    SDL_Surface* optimized = SDL_ConvertSurface( surface, format, 0 );
    if( nullptr == optimized)
    {
        printf( "Unable to optimize surface! SDL Error: %s\n", SDL_GetError() );
    }

    return optimized;
}

void main_loop(SDL_Window* window)
{
}

SDL_Window* init()
{
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf("SDL could not initialize! SDL_Error: %s\n",
                SDL_GetError());
        return nullptr;
    }

    if(! IMG_Init(IMG_INIT_PNG))
    {
        printf("SDL_Image could not initialize! SDL_Error: %s\n",
                IMG_GetError());
        return nullptr;
    }

    //Create window
    SDL_Window* window = SDL_CreateWindow(
            "SDL Tutorial",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN);

    if( nullptr == window)
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
    }

    return window;
}

int main( int argc, char* args[] )
{
    SDL_Window* window = init();

    if( NULL != window)
    {
        main_loop(window);

        //Destroy window
        SDL_DestroyWindow( window );
    }

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}
