#include <iostream>
#include <string>
#include <sciuter/sdl.hpp>
#include <SDL2/SDL_image.h>

using namespace std;

SDL_Window* sdl_init(const int screen_width, const int screen_height)
{
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        cout << "SDL could not initialize! SDL_Error: "
	     << SDL_GetError()
	     << endl;
        return nullptr;
    }

    if(! IMG_Init(IMG_INIT_PNG))
    {
        cout << "SDL_Image could not initialize! SDL_Error: "
             << IMG_GetError()
	     << endl;
        return nullptr;
    }

    //Create window
    SDL_Window* window = SDL_CreateWindow(
            "SDL Tutorial",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            screen_width,
            screen_height,
            SDL_WINDOW_SHOWN);

    if( nullptr == window)
    {
        cout << "Window could not be created! SDL_Error: "
	     << SDL_GetError()
	     << endl;
    }

    return window;
}

void sdl_quit(SDL_Window* window)
{
    //Destroy window
    SDL_DestroyWindow( window );

    IMG_Quit();
    SDL_Quit();
}

SDL_Surface* optimize_surface(SDL_Surface* surface, const SDL_PixelFormat* format)
{
    //Convert surface to screen format
    SDL_Surface* optimized = SDL_ConvertSurface( surface, format, 0 );
    if( nullptr == optimized)
    {
        cout << "Unable to optimize surface! SDL Error: "
	     << SDL_GetError()
	     << endl;
    }

    return optimized;
}

SDL_Surface* load_surface(const std::string& path, const SDL_PixelFormat* format)
{
    //Load image at specified path
    SDL_Surface* surface = IMG_Load( path.c_str() );
    if( nullptr == surface)
    {
        cout << "Unable to load image " << path << "! SDL_image Error: "
	     << IMG_GetError()
	     << endl;
    }

    if( nullptr != format )
    {
        SDL_Surface* optimized = optimize_surface(surface, format);
        SDL_FreeSurface(surface);
        return optimized;
    }

    return surface;
}

SDL_Texture* load_texture( const std::string path, SDL_Renderer* renderer )
{
    //The final texture
    SDL_Texture* texture = nullptr;

    //Load image at specified path
    SDL_Surface* surface = IMG_Load( path.c_str() );
    if( nullptr == surface )
    {
        cout << "Unable to load image " << path << "! SDL_image Error: "
	     << IMG_GetError()
	     << endl;
    }
    else
    {
        //Create texture from surface pixels
        texture = SDL_CreateTextureFromSurface( renderer, surface );
        if( nullptr == texture )
        {
            cout << "Unable to create texture from " << path << "! SDL Error: "
						  << SDL_GetError()
						  << endl;
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( surface );
    }

    return texture;
}
