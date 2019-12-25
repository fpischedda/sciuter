//Using SDL and standard IO
#include <stdio.h>
#include <string>
#include <sciuter/sdl.hpp>
#include <sciuter/animation.hpp>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void main_loop(SDL_Window* window)
{
    AnimationMap animations = TexturePackerAnimationLoader::load("resources/images/player.json");
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

int main( int argc, char* args[] )
{
    SDL_Window* window = sdl_init(SCREEN_WIDTH, SCREEN_HEIGHT);

    if( NULL != window)
    {
        main_loop(window);
    }

    //Quit SDL subsystems
    sdl_quit(window);

    return 0;
}
