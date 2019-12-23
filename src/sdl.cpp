#include <stdio.h>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Surface* optimize_surface(SDL_Surface* surface, const SDL_PixelFormat* format)
{
    //Convert surface to screen format
    SDL_Surface* optimized = SDL_ConvertSurface( surface, format, 0 );
    if( nullptr == optimized)
    {
        printf( "Unable to optimize surface! SDL Error: %s\n", SDL_GetError() );
    }

    return optimized;
}

SDL_Surface* load_surface(const std::string path, const SDL_PixelFormat* format=nullptr)
{
    //Load image at specified path
    SDL_Surface* surface = IMG_Load( path.c_str() );
    if( nullptr == surface)
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
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
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        texture = SDL_CreateTextureFromSurface( renderer, surface );
        if( nullptr == texture )
        {
            printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( surface );
    }

    return texture;
}

