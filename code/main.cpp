#include <stdio.h>
#include <iostream>
#include <string.h>
#include <malloc.h>
#include <MiniFB.h>

// #define se passe avant la compilation. Ca va remplacer tous les noms de variables par les valeurs qu'on leur donne.
// Window constants 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define WINDOW_MEMORY (WINDOW_WIDTH * WINDOW_HEIGHT * 4)

#include "Drawing.cpp"

// Structs
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct Player
{
    bitmap_t sprite;
    int xPos, yPos;
}
Player;
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Variables 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

struct mfb_window *window;

bool keyStates[KB_KEY_LAST + 1];
bool previousKeyStates[KB_KEY_LAST + 1];

Player player{ player.sprite = LoadImage("assets/Penguin.png"), player.xPos = 200, player.yPos = 300};
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Functions 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma region Player Functions

void Jump(Player& player)
{
    player.yPos -= 100;
}

#pragma endregion Player Functions

#pragma region Input Functions

void OnKeyboardEvent(struct mfb_window *window, mfb_key key, mfb_key_mod mod, bool isPressed)
{
    keyStates[key] = isPressed;
}

bool OnKeyBeingPressed(mfb_key key)
{
    return keyStates[key];
}

bool OnKeyPressed(mfb_key key)
{
    return !previousKeyStates[key] && keyStates[key];
}

bool OnKeyReleased(mfb_key key)
{
    return previousKeyStates[key] && !keyStates[key];
}

void HandleInputs()
{
    if (OnKeyPressed(KB_KEY_SPACE))
    {
        Jump(player);
    }
    if (OnKeyReleased(KB_KEY_ENTER))
    {
        printf("Enter released \n");
    }
}

#pragma endregion Input Functions

void Start()
{
    printf("Hello World \n");

    window = mfb_open_ex("Game", WINDOW_WIDTH, WINDOW_HEIGHT, WF_RESIZABLE);

    if (!window)
    {
        exit(1);
    }
        
    // malloc alloue de la mémoire (ici 800 * 600 * 4 (4 bytes pour un int32) de mémoire)
    // buffer est un terme pour designer un ensemble de données 
    // un frame buffer contient les data des pixels de la fenêtre de jeu.
    buffer = (uint32_t*) malloc(WINDOW_MEMORY);

    mfb_set_keyboard_callback(window, OnKeyboardEvent);
}

void Update()
{
    do 
    {
        memcpy(previousKeyStates, keyStates, sizeof(keyStates));

        int state;

        state = mfb_update_ex(window, buffer, WINDOW_WIDTH, WINDOW_HEIGHT);

        if (state < 0) 
        {
            window = NULL;
            break;
        }

        HandleInputs();

        memset(buffer, 0, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));

        DrawScaledBitmap((unsigned char*)player.sprite.pixels, player.xPos, player.yPos, player.sprite.pixel_size_x, player.sprite.pixel_size_y, 4.f);

    } while(mfb_wait_sync(window));
}

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