//
//  main.cpp
//  SDL Graphics
//
//  Created by Stachu on 26.10.2025.
//
//  TODO:
//  - try changing SDL_GetTicks():Uint64 to clock():clock_t (requires <ctime>)
//
//  DONE:
//  - filling the screen with changing colors
//  - frame time and fps counter
//  - frame time limiter

#define SDL_MAIN_USE_CALLBACKS 1    /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
/* For sleeping */
#include <thread>
#include <chrono>
/* For time */
//#include <ctime>  /* not working properly (a sec is something between 1 and 2 sec) */

/* Prepare the pointers for window and renderer */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

/* Prepare variables for time features */
static Uint64 programStart_timestamp = 0;
/* frame time / fps */
static Uint64 lastFrame_timestamp = 0;
static Uint64 frameTime = 0;
/* fps limit */
static Uint64 lastSwap_timestamp = 0;
static Uint64 frameRenderTime = 0;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char* argv[])
{
    /* Set up app metadata */
    SDL_SetAppMetadata("SDL Graphics", "0.1.1", "name.stachu.SDL-Graphics");
    /* Check if SDL Video works */
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("SDL Graphics", 800, 600, SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
        SDL_Log("Couldn't create a window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    
    /* Set timestamps */
    programStart_timestamp = SDL_GetTicks();
    lastFrame_timestamp = SDL_GetTicks();  /* just in case */
    lastSwap_timestamp = SDL_GetTicks();  /* just in case */
    
    /* Clear the screen with black */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    /* Go on with the program */
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appevent, SDL_Event *event)
{
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    /*   Clear the screan with changing rgb every second */
    /* get time (in seconds, minus start time, floored) */
    const int now = (SDL_floor((SDL_GetTicks() - programStart_timestamp) / 1000));
    switch (now % 3) {
        case 0:
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            break;
        case 1:
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            break;
        case 2:
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            break;
        default:
            break;
    }
    SDL_RenderClear(renderer);
    
    /* Limit fps */
    frameRenderTime = SDL_GetTicks() - lastSwap_timestamp;
    std::this_thread::sleep_for(std::chrono::milliseconds(16 - frameRenderTime));
    
    /* Swap buffers (show the image to the screen) */
    lastSwap_timestamp = SDL_GetTicks();  /* save swap timestamp */
    SDL_RenderPresent(renderer);
    
    /* Count frame time */
    frameTime = SDL_GetTicks() - lastFrame_timestamp;
    lastFrame_timestamp = SDL_GetTicks();
    /* Log frame time and fps */
    SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "frame time: %d", (int)frameTime);
    SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "fps: %d", (int)(1000 / (int)frameTime));
    
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
//    SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "%d", 1000);
}
