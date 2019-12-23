#include <stdio.h>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Surface* optimize_surface(SDL_Surface* surface, const SDL_PixelFormat* format);

SDL_Surface* load_surface(const std::string path, const SDL_PixelFormat* format=nullptr);

SDL_Texture* load_texture( const std::string path, SDL_Renderer* renderer );
