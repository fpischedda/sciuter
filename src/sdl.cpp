#include <string>
#include <sciuter/sdl.hpp>
#include <SDL2/SDL_image.h>

using namespace std;

SDL_Window* sdl_init(const int screen_width, const int screen_height)
{
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
	SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return nullptr;
    }

    if(! IMG_Init(IMG_INIT_PNG))
    {
	SDL_Log("SDL_Image could not initialize! SDL_Error: %s", SDL_GetError());
        return nullptr;
    }

    //Create window
    SDL_Window* window = SDL_CreateWindow(
            "SDL Tutorial",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_UNDEFINED,
            screen_width,
            screen_height,
            SDL_WINDOW_SHOWN);

    if( nullptr == window)
    {
	SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
        return nullptr;
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
        SDL_Log("Unable to optimize surface! SDL Error: %s", SDL_GetError());
    }

    return optimized;
}

SDL_Surface* load_surface(const std::string& path, const SDL_PixelFormat* format)
{
    //Load image at specified path
    SDL_Surface* surface = IMG_Load( path.c_str() );
    if( nullptr == surface)
    {
        SDL_Log("Unable to load image %s! SDL_image Error: %s",
		path.c_str(),
	        IMG_GetError());
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
        SDL_Log("Unable to load image %s! SDL_image Error: %s",
		path.c_str(),
		IMG_GetError());
    }
    else
    {
        //Create texture from surface pixels
        texture = SDL_CreateTextureFromSurface( renderer, surface );
        if( nullptr == texture )
        {
            SDL_Log("Unable to create texture from %s! SDL Error: %s",
		    path.c_str(),
		    SDL_GetError());
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( surface );
    }

    return texture;
}
