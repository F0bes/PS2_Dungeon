#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>
#include "./constants.h"
#define SDL_MAIN_HANDLED
///////////////////////////////////////////////////////////////////////////////
// Global variables
///////////////////////////////////////////////////////////////////////////////
int game_is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int last_frame_time = 0;

SDL_GameController* controller;
float delta_time;
///////////////////////////////////////////////////////////////////////////////

SDL_Surface* Splayer = NULL;
SDL_Texture* Tplayer = NULL;

SDL_Surface* Ssala1 = NULL;
SDL_Texture* Tsala1 = NULL;

///////////////////////////////////////////////////////////////////////////////
// Declare two game objects for the ball and the paddle
///////////////////////////////////////////////////////////////////////////////
struct game_object {
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} ball, paddle;

///////////////////////////////////////////////////////////////////////////////
// Function to initialize our SDL window
///////////////////////////////////////////////////////////////////////////////
int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return false;
    }
    controller = SDL_GameControllerOpen(0);
    if(controller == NULL){ 
        printf("Failed to open the game controller!\n");
        return false;
    }else 
        printf("Controller OK!\n");
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Function to poll SDL events and process keyboard input
///////////////////////////////////////////////////////////////////////////////
void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT:
            game_is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                game_is_running = false;
            if (event.key.keysym.sym == SDLK_LEFT)
                paddle.vel_x = -400;
            if (event.key.keysym.sym == SDLK_RIGHT)
                paddle.vel_x = +400;
            break;
        case SDL_KEYUP:
            if (event.key.keysym.sym == SDLK_LEFT)
                paddle.vel_x = 0;
            if (event.key.keysym.sym == SDLK_RIGHT)
                paddle.vel_x = 0;
            break;
    }

    const SDL_GameControllerButton UpState = SDL_GameControllerGetButton(controller,11);
    const SDL_GameControllerButton DownState = SDL_GameControllerGetButton(controller,12);
    const SDL_GameControllerButton LeftState = SDL_GameControllerGetButton(controller,13);
    const SDL_GameControllerButton RightState = SDL_GameControllerGetButton(controller,14); 

    if(RightState == SDL_PRESSED)
        paddle.x += 400 * delta_time;
    if(LeftState == SDL_PRESSED)
        paddle.x -= 400 * delta_time;
    if(UpState == SDL_PRESSED)
        paddle.y -= 400 * delta_time;
    if(DownState == SDL_PRESSED)
        paddle.y += 400 * delta_time;
}

///////////////////////////////////////////////////////////////////////////////
// Setup function that runs once at the beginning of our program
///////////////////////////////////////////////////////////////////////////////
void Nsetup(void) {
    // Initialize values for the the ball object
    ball.width = 15;
    ball.height = 15;
    ball.x = 20;
    ball.y = 20;
    ball.vel_x = 300;
    ball.vel_y = 300;

    // Initialize the values for the paddle object
    paddle.width = 100;
    paddle.height = 20;
    paddle.x = (WINDOW_WIDTH / 2) - (paddle.width / 2);
    paddle.y = WINDOW_HEIGHT - 40;

    Splayer = SDL_LoadBMP("Assets/player.bmp");
    Tplayer = SDL_CreateTextureFromSurface(renderer, Splayer);
    SDL_FreeSurface(Splayer);
    //SDL_SetTextureBlendMode(Tplayer, SDL_BLENDMODE_BLEND);
    //SDL_SetTextureAlphaMod(Tplayer,100);

    Ssala1 = SDL_LoadBMP("Assets/sala1.bmp");
    Tsala1 = SDL_CreateTextureFromSurface(renderer, Ssala1);
    SDL_FreeSurface(Ssala1);
}

///////////////////////////////////////////////////////////////////////////////
// Update function with a fixed time step
///////////////////////////////////////////////////////////////////////////////
void update(void) {
    // Calculate how much we have to wait until we reach the target frame time
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

    // Only delay if we are too fast too update this frame
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        SDL_Delay(time_to_wait);

    // Get a delta time factor converted to seconds to be used to update my objects
    delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;

    // Store the milliseconds of the current frame
    last_frame_time = SDL_GetTicks();

    // update ball and paddle position
    ball.x += ball.vel_x * delta_time;
    ball.y += ball.vel_y * delta_time;

    // Check for ball collision with the walls
    if (ball.x <= 0 || ball.x + ball.width >= WINDOW_WIDTH)
        ball.vel_x = -ball.vel_x;
    if (ball.y < 0)
        ball.vel_y = -ball.vel_y;

    // Check for ball collision with the paddle
    if (ball.y + ball.height >= paddle.y && ball.x + ball.width >= paddle.x && ball.x <= paddle.x + paddle.width)
        ball.vel_y = -ball.vel_y;

    // Prevent paddle from moving outside the boundaries of the window
    if (paddle.x <= 0)
        paddle.x = 0;
    if (paddle.x >= WINDOW_WIDTH - paddle.width)
        paddle.x = WINDOW_WIDTH - paddle.width;

    // Check for game over
    if (ball.y + ball.height > WINDOW_HEIGHT) {
        ball.x = WINDOW_WIDTH / 2;
        ball.y = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Render function to draw game objects in the SDL window
///////////////////////////////////////////////////////////////////////////////
void render(void) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect Rsala1 = {0,0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderCopy(renderer, Tsala1, NULL, &Rsala1);

    SDL_Rect Rplayer = {paddle.x,paddle.y, 64, 64};
    SDL_RenderCopy(renderer, Tplayer, NULL, &Rplayer);

    SDL_RenderPresent(renderer);
}

///////////////////////////////////////////////////////////////////////////////
// Function to destroy SDL window and renderer
///////////////////////////////////////////////////////////////////////////////
void destroy_window(void) {
    SDL_DestroyTexture(Tplayer);
    SDL_DestroyTexture(Tsala1);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

///////////////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* args[]) {
    game_is_running = initialize_window();

    Nsetup();

    while (game_is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}
