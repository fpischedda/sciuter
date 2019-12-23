//Using SDL and standard IO
#include <stdio.h>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "sdl.hpp"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void main_loop(SDL_Window* window)
{
    bool quit = false;
    SDL_Event e;

    //Create renderer for window
    SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    if( renderer == NULL )
    {
        printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        return;
    }

    //Initialize renderer color
    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    SDL_Texture* background = load_texture("resources/images/background.png", renderer);

    while( !quit )
    {
        if( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            switch( e.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    quit = true;
                    break;
            }
        }

        //Clear screen
        SDL_RenderClear( renderer );

        //Render texture to screen
        SDL_RenderCopy( renderer, background, NULL, NULL );

        //Update screen
        SDL_RenderPresent( renderer );
    }
    SDL_DestroyTexture(background);
    SDL_DestroyRenderer( renderer );
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
    IMG_Quit();
    SDL_Quit();

    return 0;
}
