#include <stdio.h>
#include <iostream>
#include <string.h>
#include <malloc.h>

#include "AudioManager.cpp"
#include "Background.cpp"
#include "Drawing.cpp"
#include "Enemy.cpp"
#include "Input.cpp"
#include "LevelEditor.cpp"
#include "RunnerGame.cpp"
#include "Utility.cpp"


// Variables 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

struct mfb_window *window;

uint32_t fps = 60;

bool gameStarted = false;

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Functions 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

void HandleInputs()
{
    // On Key Pressed Events.
    // ------------------------------------------------------

    if (KeyWasPressed(KB_KEY_SPACE))
    {
        if (gameStarted && ! gameOver)
        {
            Jump(player);
        }
        else if (!gameStarted)
        {
            gameStarted = true;
        }
        else if (gameOver)
        {
            Restart();
        }
    }
    
    if (KeyWasPressed(KB_KEY_ESCAPE))
    {
        exit(0);
    }

    // ------------------------------------------------------

    // On Key Released Events.
    // ------------------------------------------------------

    if (KeyWasReleased(KB_KEY_ENTER))
    {
        printf("Enter released \n");
    }

    // ------------------------------------------------------
}

#pragma endregion Input Functions

void Start()
{
    window = mfb_open_ex("Game", WINDOW_WIDTH, WINDOW_HEIGHT, WF_RESIZABLE);

    if (!window)
    {
        exit(1);
    }
        
    // malloc alloue de la mémoire (ici 800 * 600 * 4 (4 bytes pour un int32) de mémoire)
    // buffer est un terme pour designer un ensemble de données 
    // un frame buffer contient les data des pixels de la fenêtre de jeu.
    windowBuffer = (uint32_t*) malloc(WINDOW_MEMORY);
    frameBuffer =  (uint32_t*) malloc(FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT * 4);

    mfb_get_monitor_scale(window, &monitorScaleX, &monitorScaleY);

    mfb_set_target_fps(fps);

    InitializeInputCallbacks(window);

    windows_enable_colors_in_command_prompt();

    SetupSound();

    InitializeLevelEditor();
    InitializeRunnerGame();
}

#pragma region Update Functions


void UpdateTitleScreen()
{
    DrawFullRect (((int)(FRAME_BUFFER_WIDTH / 2)), ((int)(FRAME_BUFFER_HEIGHT / 4)) + 4, 90, 28, BLACK);
    DrawEmptyRect(((int)(FRAME_BUFFER_WIDTH / 2)), ((int)(FRAME_BUFFER_HEIGHT / 4)) + 4, 90, 28, WHITE);
    DrawTextWithColor("press space", (int)(FRAME_BUFFER_WIDTH / 2), (int)(FRAME_BUFFER_HEIGHT / 4), WHITE);
    DrawTextWithColor("to start", ((int)(FRAME_BUFFER_WIDTH / 2)) + 1, ((int)(FRAME_BUFFER_HEIGHT / 4)) + 9, WHITE);
    
    DrawFullRect (((int)(FRAME_BUFFER_WIDTH / 2)), 3 * ((int)(FRAME_BUFFER_HEIGHT / 4)) + 4, 90, 28, BLACK);
    DrawEmptyRect(((int)(FRAME_BUFFER_WIDTH / 2)), 3 * ((int)(FRAME_BUFFER_HEIGHT / 4)) + 4, 90, 28, WHITE);
    DrawTextWithColor("press escape", (int)(FRAME_BUFFER_WIDTH / 2), 90, WHITE); 
    DrawTextWithColor("to exit", ((int)(FRAME_BUFFER_WIDTH / 2)) + 1, 99, WHITE); 
}

void Update()
{
    do 
    {
        #ifdef __EMSCRIPTEN__
        nowTime = mfb_timer_now(timer);
        deltaTime = nowTime - lastRenderedFrameTime;
        targetDeltaTime = 1.f/60.f;
        if (deltaTime < targetDeltaTime) {
            // si on a pas depassé notre target delta time, on rend la main au browser puis on revient en haut de la boucle
            mfb_wait_sync(window);
            continue;
        }
        lastRenderedFrameTime = nowTime;
        #endif

        memcpy(previousKeyStates, keyStates, sizeof(keyStates));
        memcpy(previousMouseButtonStates, mouseButtonStates, sizeof(mouseButtonStates));

        ResizeBuffer(windowBuffer, WINDOW_WIDTH, WINDOW_HEIGHT, frameBuffer, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

        int state;
        state = mfb_update_ex(window, windowBuffer, WINDOW_WIDTH, WINDOW_HEIGHT);

        if (state < 0) 
        {
            window = NULL;
            break;
        }

        HandleInputs();

        UpdateLevelEditor();

        memset(windowBuffer, 0, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));
        memset(frameBuffer,  0, FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT * sizeof(uint32_t));

        DrawBackground(gameSpeed, adjustedGameSpeed, gameOver, gameStarted);
        DrawBitmap((unsigned char*)player.sprite.pixels, player.xPos, player.yPos, player.sprite.pixel_size_x, player.sprite.pixel_size_y);

        if (!gameStarted)
        {
            UpdateTitleScreen();
        }
        else
        {
            UpdateRunnerGame();
        }

    } while(mfb_wait_sync(window));
}

#pragma endregion Update Functions

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Main
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

int main()
{
    Start();

    Update();

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------