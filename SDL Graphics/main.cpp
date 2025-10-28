//
//  main.cpp
//  SDL Graphics
//
//  Created by Stachu on 26.10.2025.
//
//  TODO:
//  - average fps / frametime
//  - try microseconds instead of nanoseconds
//  - measure average runtime of:
//     - now().time_since_epoch()
//     - std::chrono::nanoseconds subtraction
//     - std::this_thread::sleep_for()
//
//  DONE:
//  - frame time and fps counter
//  - frame time limiter
//  - changed SDL_GetTicks() (in milliseconds) to std::chrono::high_resolution_clock().now().time_since_epoch() (in nanoseconds) instead of to clock() (in microseconds and didn't work properly)

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
static std::chrono::nanoseconds programStart_timestamp;
/* frame time / fps (in nanoseconds with std::chrono::high_resolution_clock().now().time_since_epoch()) */
static std::chrono::steady_clock perfClock;
static std::chrono::nanoseconds lastFrame_timestamp;
static std::chrono::nanoseconds frameTime;
/* fps limit */
static std::chrono::nanoseconds lastSwap_timestamp;
static std::chrono::nanoseconds frameRenderTime;

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
    perfClock = std::chrono::high_resolution_clock();
    programStart_timestamp = perfClock.now().time_since_epoch();
    lastSwap_timestamp = perfClock.now().time_since_epoch();  /* just in case */
    lastFrame_timestamp = perfClock.now().time_since_epoch();  /* just in case */
    
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
    const int now = floor(((perfClock.now().time_since_epoch() - programStart_timestamp).count()) / 1000000000);
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
    
    /* Limit fps to 60 (sleep til 16.666666ms (adjusted) before swapping) */
    frameRenderTime = perfClock.now().time_since_epoch() - lastSwap_timestamp;
    std::this_thread::sleep_for(std::chrono::nanoseconds(15666000) - frameRenderTime);
    
    /* Swap buffers (show the image to the screen) */
    lastSwap_timestamp = perfClock.now().time_since_epoch();  /* save swap timestamp for fps limit */
    SDL_RenderPresent(renderer);
    
    /* Count frame time (in nanoseconds) */
    frameTime = perfClock.now().time_since_epoch() - lastFrame_timestamp;
    lastFrame_timestamp = perfClock.now().time_since_epoch();
    /* Log frame time and fps */
    SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "frame time (milliseconds): %.3f", (double)std::chrono::nanoseconds(frameTime).count() / 1000000);
    SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "fps: %.3f", (1000000000 / (double)std::chrono::nanoseconds(frameTime).count()));
    
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
//    SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "%d", 1000);
}
