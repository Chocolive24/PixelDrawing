#include <stdio.h>
#include <iostream>
#include <string.h>
#include <malloc.h>

#include "AudioManager.cpp"
#include "GUI.cpp"
#include "Drawing.cpp"
#include "FrameAllocator.cpp"
#include "Input.cpp"
#include "GameLevels.cpp"
#include "LevelEditor.cpp"
#include "Player.cpp"
#include "Utility.cpp"
#include "Timer.cpp"

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Variables 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

struct mfb_window *window;

uint32_t fps = 60;

// Title screen variables 
// ---------------------------------------------------------------

Player player{ };

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Functions 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

void HandleInputs()
{
    // On Key Pressed Events.
    // ------------------------------------------------------

    // if (KeyWasPressed(KB_KEY_SPACE))
    // {
    //     editorOpen = true;
    // }
    
    if (KeyWasPressed(KB_KEY_ESCAPE))
    {
        if (gameStarted || levelSelectOpen || editorOpen)
        {
            gameStarted = false;
            levelSelectOpen = false;
            editorOpen = false;
            isPlayerEditing = false;
            player.started = false;
        }
        else 
        {
            exit(0);
        }
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

    FrameAllocatorInit(10000);

    CreateTitleScreen();

    if (!puzzleTheme.samples)
    {
        puzzleTheme = loadSoundClip("assets/puzzleTheme.wav");
        PlaySoundClip(puzzleTheme, 1.f, 440, 0, 0, true);
    }

    InitializeGUI(player);
}

#pragma region Update Functions


void UpdateGame()
{
    if (!player.started)
    {
        gameStarted = false;
        levelSelectOpen = false;
        editorOpen = false;
        isPlayerEditing = false;
        player.started = false;

        return;
    }

    if (player.isEditingLvl)
    {
        UpdateLevelEditor();
    }
    else 
    {
        DrawLevelTiles();
    }
    
    player.Update();
}

void Update()
{
    // int score = 100;
    // char* scoreText = (char*)FrameAlloc(sizeof(char) * 10);

    // int s2 = 200;
    // char* scoreText2 = (char*)FrameAlloc(sizeof(char) * 20);

    // int* intTest = (int*)FrameAlloc(sizeof(int));
    // *intTest = 10;
    do 
    {
        // #ifdef __EMSCRIPTEN__
        // nowTime = mfb_timer_now(timer);
        // deltaTime = nowTime - lastRenderedFrameTime;
        // targetDeltaTime = 1.f/60.f;
        // if (deltaTime < targetDeltaTime) {
        //     // si on a pas depassé notre target delta time, on rend la main au browser puis on revient en haut de la boucle
        //     mfb_wait_sync(window);
        //     continue;
        // }
        // lastRenderedFrameTime = nowTime;
        // #endif

        // snprintf(scoreText,  sizeof(char) * 10, "%i", score);
        // snprintf(scoreText2, sizeof(char) * 20, "%i", s2);
        // *intTest += 1;

        // printf("%c", frameAllocatorBuffer[0]);
        // printf("%c", frameAllocatorBuffer[1]);
        // printf("%c ", frameAllocatorBuffer[2]);

        // printf("%c", frameAllocatorBuffer[10]);
        // printf("%c", frameAllocatorBuffer[11]);
        // printf("%c\n", frameAllocatorBuffer[12]);

        // DrawMyText(scoreText, 10, 10);
        // DrawMyText(scoreText2, 10, 20);

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
        
        memset(windowBuffer, 0, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));
        memset(frameBuffer,  0, FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT * sizeof(uint32_t));

        if (gameStarted || editorOpen)
        {
            for (int y = 0; y < TILEMAP_HEIGHT; y++)
            {
                for (int x = 0; x < TILEMAP_WIDTH ; x++)
                {
                    DrawPixel(x, y, LIGHT_BLUE);
                }
            }
        }

        TimerTick();

        HandleInputs();

        UpdateGUI();

        if (gameStarted)
        {
            UpdateGame();
        }
        // else if (levelSelectOpen)
        // {
        //     UpdateLevelSelection();
        // }
        else if (editorOpen)
        {
            UpdateLevelEditor();
        }
        else 
        {
            UpdateTitleScreen();
        }

        FrameAllocatorTick();

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

    // free(window);
    // FreeDrawingMemory();
    // FreeSoundsMemory();
    // free(puzzleTheme.samples);

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------