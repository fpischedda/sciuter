/**
 * Testing EnTT ECS library using SDL2 as the media library
 * Implementing a shmup to have fun while I experiment
 */
#include <random>
#include <string>
#include <sciuter/sdl.hpp>
#include <sciuter/game.hpp>

//game dimension constants
const int AREA_WIDTH = 640;
const int AREA_HEIGHT = 480;

int main( int argc, char* args[] )
{
    SDL_Window* window = sdl_init(AREA_WIDTH, AREA_HEIGHT);

    if( NULL == window)
    {
	SDL_Log("window creation failed: %s", SDL_GetError());
	return 1;
    }

    SDL_DisplayMode dm;

    if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
    {
	SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
	return 1;
    }

    int scale = dm.w / AREA_WIDTH;

    // if upscaling I want the window to be a bit smaller than the screen
    // (in future enable this check only for windowed mode)
    if(scale > 1 && AREA_WIDTH * scale == dm.w) {
	scale = scale - 1;
    }

    SDL_SetWindowSize(window, AREA_WIDTH * scale, AREA_HEIGHT * scale);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, 10);

    main_loop(window, scale);

    //Quit SDL subsystems
    sdl_quit(window);

    return 0;
}
