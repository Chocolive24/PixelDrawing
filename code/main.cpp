#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <random>


// #define se passe avant la compilation. Ca va remplacer tous les noms de variables par les valeurs qu'on leur donne.
// Window constants 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

#define WINDOW_FAC 4

#define FRAME_BUFFER_WIDTH 200
#define FRAME_BUFFER_HEIGHT 120

#define WINDOW_WIDTH (FRAME_BUFFER_WIDTH * WINDOW_FAC)
#define WINDOW_HEIGHT (FRAME_BUFFER_HEIGHT * WINDOW_FAC)

#define WINDOW_MEMORY (WINDOW_WIDTH * WINDOW_HEIGHT * 4)

#define COLLISION_OFFSET 2 // Amount to subtract from collision detection pixels

#include "Drawing.cpp"
#include "Ground.cpp"
#include "Enemy.cpp"
#include "Utility.cpp"



// Structs
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct Player
{
    bitmap_t sprite;
    int xStartPos, yStartPos;
    int xPos, yPos;
    int halfWidth, halfHeight;
    bool isJumping;
    int jumpHeight;
    int score;

    int GetBottomPos()
    {
        return (int)(yPos + sprite.pixel_size_y / 2);
    }
}
Player;



// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Variables 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

struct mfb_window *window;

bool keyStates[KB_KEY_LAST + 1];
bool previousKeyStates[KB_KEY_LAST + 1];

bool gameStarted = false;
bool gameOver = false;

bool mustApplyGravity = false;

char scoreText[15];
int scoreTextPosX = 36, scoreTextPosY = 0.1 * FRAME_BUFFER_HEIGHT;

Player player
{ 
    player.sprite = LoadImage("assets/Penguin.png"), 
    player.xStartPos = 25, player.yStartPos = 100 - (int)(player.sprite.pixel_size_y / 2),
    player.xPos = player.xStartPos, player.yPos = player.yStartPos,
    player.halfWidth = player.sprite.pixel_size_x / 2, player.halfHeight = player.sprite.pixel_size_y / 2,
    player.isJumping = false,
    player.jumpHeight = 50,
    player.score = 0
};



int scoreSpeed = 10;
int frameCounter;

bitmap_t penguinSprite;


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
        if (gameStarted && ! gameOver)
        {
            Jump(player);
        }
        else if (!gameStarted)
        {
            gameStarted = true;
            SpawnEnemy(FRAME_BUFFER_WIDTH + 50, 90, player.score);
        }
        else if (gameOver)
        {
            gameOver = false;

            for (int i = 0; i < enemyCount; i++)
            {
                enemies[i] = enemies[enemyCount-1];
                enemyCount--;
                i--;
            }

            player.score = 0;
            player.yPos = player.yStartPos;
            player.isJumping = false;

            mustApplyGravity = false;
            scoreSpeed = 4;
            frameCounter = 0;
            scoreTextPosX = 36;

            SpawnEnemy(FRAME_BUFFER_WIDTH + 50, 90, player.score);
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

    // Providing a seed value
	srand((unsigned int) time(NULL));

    CreateFirstGroundTiles();
}

void HandleJump()
{
    if (player.yPos <= player.yStartPos - player.jumpHeight) // || OnKeyReleased(KB_KEY_SPACE) && player.yPos < player.yStartPos
    {
        //player.yPos = player.yStartPos - player.jumpHeight;
        mustApplyGravity = true;
    }

    if (player.isJumping && !mustApplyGravity)
    {
        player.yPos -= 2.f;
    }

    if (player.GetBottomPos() >= 100 && player.isJumping)
    {
        player.yPos = 100 - (int)(player.sprite.pixel_size_y / 2);
        player.isJumping = false;
        mustApplyGravity = false;
    }

    if (mustApplyGravity)
    {
        player.yPos += 3.f;
    }
}

bool IsCollisionDetected(Player &player, Enemy &enemy)
{
    return player.xPos - player.halfWidth  + COLLISION_OFFSET < enemy.xPos + enemy.halfWidth  - COLLISION_OFFSET &&
           player.xPos + player.halfWidth  - COLLISION_OFFSET > enemy.xPos - enemy.halfWidth  + COLLISION_OFFSET &&
           player.yPos - player.halfHeight + COLLISION_OFFSET < enemy.yPos + enemy.halfHeight - COLLISION_OFFSET &&
           player.yPos + player.halfHeight - COLLISION_OFFSET > enemy.yPos - enemy.halfHeight + COLLISION_OFFSET;
}

void UpdateTitleScreen()
{
    DrawText("press space", (int)(FRAME_BUFFER_WIDTH / 2), (int)(FRAME_BUFFER_HEIGHT / 4));
    DrawText("to start", ((int)(FRAME_BUFFER_WIDTH / 2)) + 1, ((int)(FRAME_BUFFER_HEIGHT / 4)) + 9);
    DrawEmptyRect(55, ((int)(FRAME_BUFFER_HEIGHT / 4)) - 11, 90, 28, WHITE);

    DrawText("press escape", (int)(FRAME_BUFFER_WIDTH / 2), 90); 
    DrawText("to exit", ((int)(FRAME_BUFFER_WIDTH / 2)) + 1, 99); 
    DrawEmptyRect(55, 90 - 11, 90, 28, WHITE);
}

void UpdateGame()
{
    DrawGround(gameOver);

    for(int i = 0; i < enemyCount; i++)
    {
        Enemy* enemy = &enemies[i];

        if (IsCollisionDetected(player, *enemy))
        {
            gameOver = true;
        }

        
        enemy->xPos -= gameOver ? 0 : enemy->speed;
        
       
        DrawBitmap((unsigned char*)enemy->sprite.pixels, enemy->xPos, enemy->yPos, enemy->sprite.pixel_size_x, enemy->sprite.pixel_size_y);
        
        if (enemy->xPos + (int)enemy->sprite.pixel_size_x / 2 < 0)
        {
            enemies[i] = enemies[enemyCount-1];
            enemyCount--;
            i--;
        }
    }

    

    // for (int k = 0; k < FRAME_BUFFER_WIDTH; k += FRAME_BUFFER_WIDTH / 10)
    // {
    //     for (int y = FRAME_BUFFER_HEIGHT - 20; y < FRAME_BUFFER_HEIGHT; y ++)
    //     {
    //         for (int x = k; x < k + FRAME_BUFFER_WIDTH / 10; x++)
    //         {
    //             DrawPixel(x, y, GREEN);
    //         }
    //     }
    // }

    DrawBitmap((unsigned char*)player.sprite.pixels, player.xPos, player.yPos, player.sprite.pixel_size_x, player.sprite.pixel_size_y);

    DrawText(scoreText, scoreTextPosX, scoreTextPosY);

    if (!gameOver)
    {
        HandleJump();

        if (frameCounter >= 60)
        {
            if (GetRandomNbr(1, 3) == 3)
            {
                SpawnEnemy(FRAME_BUFFER_WIDTH + GetRandomNbr(0, 20), 70, player.score);
            }
            else 
            {
                SpawnEnemy(FRAME_BUFFER_WIDTH + GetRandomNbr(0, 20), 90, player.score);
            }

            frameCounter = 0;
        }

        int frameToUpdateScore = (int)(60 / scoreSpeed) > 0 ? (int)(60 / scoreSpeed) : 1;

        if (frameCounter % frameToUpdateScore == 0)
        {
            player.score ++;

            if (player.score % 100 == 0)
            {
                scoreSpeed++;
            }

            if (player.score == 10 || player.score == 100 ||player.score == 1000)
            {
                scoreTextPosX += 4;
            }
        }

        snprintf(scoreText, 15, "score : %i", player.score);

        frameCounter++;
    }
    else 
    {
        DrawText("Game Over", (int)(FRAME_BUFFER_WIDTH / 2), (int)(FRAME_BUFFER_HEIGHT / 3));
        DrawText("press space", (int)(FRAME_BUFFER_WIDTH / 2), ((int)(FRAME_BUFFER_HEIGHT / 3)) + 20);
        DrawText("to restart", (int)(FRAME_BUFFER_WIDTH / 2),  ((int)(FRAME_BUFFER_HEIGHT / 3)) + 29);
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
            UpdateTitleScreen();
        }
        else
        {
            UpdateGame();
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