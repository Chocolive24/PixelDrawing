#include <stdio.h>
#include <iostream>
#include <string.h>
#include <malloc.h>
#include <MiniFB.h>

// #define se passe avant la compilation. Ca va remplacer tous les noms de variables par les valeurs qu'on leur donne.
// Window constants 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

#define WINDOW_FAC 4

#define FRAME_BUFFER_WIDTH 150
#define FRAME_BUFFER_HEIGHT 120

#define WINDOW_WIDTH (FRAME_BUFFER_WIDTH * WINDOW_FAC)
#define WINDOW_HEIGHT (FRAME_BUFFER_HEIGHT * WINDOW_FAC)

#define WINDOW_MEMORY (WINDOW_WIDTH * WINDOW_HEIGHT * 4)

#include "Drawing.cpp"

// Structs
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct Player
{
    bitmap_t sprite;
    int xStartPos, yStartPos;
    int xPos, yPos;
    bool isJumping;
    int jumpHeight;
    int score;

    int GetBottomPos()
    {
        return (int)(yPos + sprite.pixel_size_y / 2);
    }
}
Player;

typedef struct Enemy
{
    bitmap_t sprite;
    int xStartPos, yStartPos;
    int xPos, yPos;
}
Enemy;

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Variables 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

struct mfb_window *window;

bool keyStates[KB_KEY_LAST + 1];
bool previousKeyStates[KB_KEY_LAST + 1];

bool gameStarted;

bool mustApplyGravity = false;

Player player
{ 
    player.sprite = LoadImage("assets/Penguin.png"), 
    player.xStartPos = 50, player.yStartPos = 100 - (int)(player.sprite.pixel_size_y / 2),
    player.xPos = player.xStartPos, player.yPos = player.yStartPos,
    player.isJumping = false,
    player.jumpHeight = 20,
    player.score = 0
};

Enemy enemies[10];
int enemyCount;

int frameCounter;

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Functions 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma region Player Functions

void Jump(Player& player)
{
    if (!player.isJumping)
    {
        player.isJumping = true;
    }
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
    // On Key Pressed Events.
    // ------------------------------------------------------

    if (OnKeyPressed(KB_KEY_SPACE))
    {
        if (gameStarted)
        {
            Jump(player);
        }
        else 
        {
            gameStarted = true;
        }
    }
    if (OnKeyPressed(KB_KEY_ESCAPE))
    {
        exit(1);
    }

    // ------------------------------------------------------

    // On Key Released Events.
    // ------------------------------------------------------

    if (OnKeyReleased(KB_KEY_ENTER))
    {
        printf("Enter released \n");
    }

    // ------------------------------------------------------
}

#pragma endregion Input Functions


#pragma region Enemy Functions

void SpawnEnemy(int xStart, int yStart)
{
    if (enemyCount == 10)
    {
        return;
    }

    Enemy enemy 
    { 
        enemy.sprite = enemySprite, 
        enemy.xStartPos = xStart, enemy.yStartPos = yStart, 
        enemy.xPos = xStart, enemy.yPos = yStart
    };

    enemies[enemyCount] = enemy;
    enemyCount++;
}

#pragma endregion Enemy Functions

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
    windowBuffer = (uint32_t*) malloc(WINDOW_MEMORY);

    frameBuffer = (uint32_t*) malloc(FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT * 4);

    mfb_set_keyboard_callback(window, OnKeyboardEvent);

    LoadAllImages();
}

void HandleJump()
{
    if (player.yPos <= player.yStartPos - player.jumpHeight)
    {
        player.yPos = player.yStartPos - player.jumpHeight;
        mustApplyGravity = true;
    }

    if (player.isJumping && !mustApplyGravity)
    {
        player.yPos -= 1.2f;
    }

    if (player.GetBottomPos() >= 100 && player.isJumping)
    {
        player.yPos = 100 - (int)(player.sprite.pixel_size_y / 2);
        player.isJumping = false;
        mustApplyGravity = false;
    }

    if (mustApplyGravity)
    {
        player.yPos += 2.f;
    }
}

void Update()
{
    do 
    {
        memcpy(previousKeyStates, keyStates, sizeof(keyStates));

        resize_bitmap(windowBuffer, WINDOW_WIDTH, WINDOW_HEIGHT, frameBuffer, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

        int state;
        state = mfb_update_ex(window, windowBuffer, WINDOW_WIDTH, WINDOW_HEIGHT);

        if (state < 0) 
        {
            window = NULL;
            break;
        }

        memset(windowBuffer, 0, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));
        memset(frameBuffer, 0, FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT * sizeof(uint32_t));

        HandleInputs();

        if (!gameStarted)
        {
            DrawText("Main Menu", (int)(FRAME_BUFFER_WIDTH / 2), (int)(FRAME_BUFFER_HEIGHT / 2));
        }

        else
        {
            HandleJump();

            DrawHorizontalLine(0, FRAME_BUFFER_WIDTH - 1, 64, RED);
            DrawHorizontalLine(0, FRAME_BUFFER_WIDTH - 1, 80, 0xFFFFAA00);
            DrawHorizontalLine(0, FRAME_BUFFER_WIDTH - 1, 100, GREEN);

            if (frameCounter >= 60)
            {
                SpawnEnemy(FRAME_BUFFER_WIDTH + 20, 90);
                frameCounter = 0;
            }

            for(int i = 0; i < enemyCount; i++)
            {
                Enemy* enemy = &enemies[i];

                DrawBitmap((unsigned char*)enemy->sprite.pixels, enemy->xPos--, enemy->yPos, enemy->sprite.pixel_size_x, enemy->sprite.pixel_size_y);

                if (enemy->xPos + (int)enemy->sprite.pixel_size_x / 2 < 0)
                {
                    enemies[i] = enemies[enemyCount-1];
                    enemyCount--;
                    i--;
                }
            }

            DrawBitmap((unsigned char*)player.sprite.pixels, player.xPos, player.yPos, player.sprite.pixel_size_x, player.sprite.pixel_size_y);

            char scoreText[15];
            player.score += 10;
            snprintf(scoreText, 15, "score : %i", player.score);

            DrawText(scoreText, 10, 10); // \x03 pour hexa

            frameCounter++;
        }

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