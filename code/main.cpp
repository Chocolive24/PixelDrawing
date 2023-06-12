#include <stdio.h>
#include <iostream>
#include <string.h>
#include <malloc.h>

#include "AudioManager.cpp"
#include "Button.cpp"
#include "Drawing.cpp"
#include "Input.cpp"
#include "LevelEditor.cpp"
#include "Player.cpp"
#include "Utility.cpp"
#include "Timer.cpp"

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Variables 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

struct mfb_window *window;

uint32_t fps = 60;

bool gameStarted, levelSelectOpen ,editorOpen;

// Title screen variables 
// ---------------------------------------------------------------

Button titleScreenButtons[3];
int  titleScrButtonCount = 0;

Player player{ };

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Functions 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

void HandleInputs()
{
    // On Key Pressed Events.
    // ------------------------------------------------------

    if (KeyWasPressed(KB_KEY_SPACE))
    {
        editorOpen = true;
    }
    
    if (KeyWasPressed(KB_KEY_ESCAPE))
    {
        if (gameStarted || levelSelectOpen || editorOpen)
        {
            gameStarted = false;
            levelSelectOpen = false;
            editorOpen = false;
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

void CreateTitleScreenButton(Rect rect, uint32_t color, uint32_t selectedColor, Text text, void (*activateEffect)())
{
    if (titleScrButtonCount >= 3)
    {
        return;
    }

    Rect frame {rect.xPos, rect.yPos, rect.width, rect.height, WHITE};

    Button button{rect, frame, color, selectedColor, text, false, activateEffect};

    // Rect rect{xPos - width / 2, yPos - height / 2, width, height, color};

    // rect.xPos -= rect.width / 2

    // Button button{rect, color, selectedColor, text, false, actionFunc};

    // Button button = CreateButton(rect, color, selectedColor, text, actionFunc);

    titleScreenButtons[titleScrButtonCount++] = button;
}

void RunGame()
{
    ClearTileButtons();

    CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tilePlayerWallSprite,   TILE_PLAYER_WALL, 10);
    CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tileSpringSprite,     TILE_SPRING, 10);

    InitializeLevelEditor(true);
    LoadLevel();
    
    player.SelfReset();
    player.isEditingLvl = true;

    gameStarted = true;

    // SoundClip clip = loadSoundClip("assets/mainTheme.wav");
    // PlaySoundClip(clip, 0.5f, 440, 0, 0, true);
}

void RunLevelSelection()
{
    levelSelectOpen = true;
    LOG("LEVEL SELECTION");
}

void RunLevelEditor()
{
    ClearTileButtons();

    CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tileWallSprite,   TILE_WALL, 10);
    CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tileFireSprite,    TILE_FIRE, 10);
    CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tileSpringSprite,   TILE_SPRING, 10);
    CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tileFruitSprite,   TILE_FRUIT, 10);

    InitializeLevelEditor(false);
    ResetTilemap();
    editorOpen = true;
}

void CreateTitleScreen()
{
    Rect gameRectB{ ((int)(FRAME_BUFFER_WIDTH / 2)), ((int)(FRAME_BUFFER_HEIGHT / 4)), 100, 20, BLUE };
    Text gameTextB{ "start game", ((int)(FRAME_BUFFER_WIDTH / 2)), ((int)(FRAME_BUFFER_HEIGHT / 4)), WHITE };

    CreateTitleScreenButton(gameRectB, BLUE, LIGHT_BLUE, gameTextB, &RunGame);

    Rect lvlSltRecB{ ((int)(FRAME_BUFFER_WIDTH / 2)), 2 * ((int)(FRAME_BUFFER_HEIGHT / 4)), 100, 20, BLUE };
    Text lvlSltTextB{ "select level", ((int)(FRAME_BUFFER_WIDTH / 2)), 2 * ((int)(FRAME_BUFFER_HEIGHT / 4)), WHITE };

    CreateTitleScreenButton(lvlSltRecB, BLUE, LIGHT_BLUE, lvlSltTextB, &RunLevelSelection);

    Rect editorRectB{ ((int)(FRAME_BUFFER_WIDTH / 2)), 3 * ((int)(FRAME_BUFFER_HEIGHT / 4)), 100, 20, BLUE };
    Text editorTextB{ "level editor", ((int)(FRAME_BUFFER_WIDTH / 2)), 3 * ((int)(FRAME_BUFFER_HEIGHT / 4)), WHITE };

    CreateTitleScreenButton(editorRectB, BLUE, LIGHT_BLUE, editorTextB, &RunLevelEditor);
}

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

    CreateTitleScreen();
}

#pragma region Update Functions


void UpdateTitleScreen()
{
    for (int i = 0; i < titleScrButtonCount; i++)
    {
        Button* b = &titleScreenButtons[i];

        b->Update();

        if (b->isSelected && MouseWasPressed(MOUSE_LEFT))
        {
            b->activateEffect();
        }
    }
}

void UpdateGame()
{
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
        
        memset(windowBuffer, 0, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));
        memset(frameBuffer,  0, FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT * sizeof(uint32_t));

        TimerTick();

        HandleInputs();

        if (gameStarted)
        {
            UpdateGame();
        }
        else if (levelSelectOpen)
        {
            //UpdateLevelSelection()
        }
        else if (editorOpen)
        {
            UpdateLevelEditor();
        }
        else 
        {
            UpdateTitleScreen();
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