#ifndef __SCIUTERR__SDL_HPP__
#define __SCIUTERR__SDL_HPP__

#include <string>
#include <SDL2/SDL.h>

SDL_Window* sdl_init(const int screen_width, const int screen_height);

void sdl_quit(SDL_Window* window);

SDL_Surface* optimize_surface(SDL_Surface* surface, const SDL_PixelFormat* format);

SDL_Surface* load_surface(const std::string path, const SDL_PixelFormat* format=nullptr);

SDL_Texture* load_texture( const std::string path, SDL_Renderer* renderer );

#endif
