/* External Dependencies */
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <glm/glm.hpp>

/* Internal Dependencies */
#include <meta/ApplicationContext.h>
#include <scene/scene.h>
#include <scene/TestScreen.cpp>

#define DEBUG_MODE 1

/** Initialized on SDL_AppInit */
static std::shared_ptr<ApplicationContext> ctx;

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* comeOnGuysGenericsExist)
{
    ctx->onTick();
    
    ctx->onDrawCallRisingEdge();

    SDL_Renderer& renderer = ctx->frames().renderer();

    SDL_SetRenderDrawColor(&renderer, 255, 0, 0, 255);
    SDL_RenderClear(&renderer);

    ctx->onDraw();

    SDL_RenderPresent(&renderer);
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* comeOnGuysGenericsExist, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }

    // Force shutdown
    // Check if Ctrl + W is held down
    if (event->type == SDL_EVENT_KEY_DOWN) {
        SDL_Keymod modState = SDL_GetModState();
        if (modState & SDL_KMOD_CTRL) {
            // Check if the 'W' key was pressed
            if (event->key.key == SDLK_W) {
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Ctrl + W pressed. Exiting application.");
                return SDL_APP_SUCCESS;  // Results in exit code 0 (success)
            }
        }
    }

    ctx->input().onEvent(event);
    
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LogPriority::SDL_LOG_PRIORITY_TRACE);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Game starting up!");
    SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, "Initializing app metadata...");
    if(!SDL_SetAppMetadata("SDL Game", "0.1", "gbw.games.sdlgame")) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set app metadata: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, "Initializing Video subsystem...");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    
    SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, "Getting primary display...");
    SDL_DisplayID displayID = SDL_GetPrimaryDisplay();  
    SDL_Rect displayBounds;

    SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, "Loading display information...");
    if (!SDL_GetDisplayUsableBounds(displayID, &displayBounds)){
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't get display bounds: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_WindowFlags windowFlags = 
        // SDL_WINDOW_BORDERLESS | 
        SDL_WINDOW_FULLSCREEN | 
        SDL_WINDOW_OPENGL |
        SDL_WINDOW_HIGH_PIXEL_DENSITY |
        SDL_WINDOW_TRANSPARENT;

    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, "Initializing window and renderer...");
    if (!SDL_CreateWindowAndRenderer("SDL Game", 800, 600, windowFlags, &window, &renderer)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    glm::fvec2 displaySize = glm::fvec2(displayBounds.w, displayBounds.h);
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Display bounds: %.1f %.1f", displaySize.x, displaySize.y);

    ctx = ApplicationContext::create(
        &displaySize,windowFlags,window,renderer
    );    
    ctx->changeScene(new TestScreen(ctx));

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* comeOnGuysGenericsExist, SDL_AppResult result)
{
    std::cout << "Game over" << std::endl;
    /* SDL cleans up the window/renderer */
    /* ApplicationContext SHOULD loose last reference here and be collected... */
}
