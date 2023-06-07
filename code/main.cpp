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

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Functions 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma region Player Functions

void Jump(Player& player)
{
    if (!player.isJumping)
    {
        player.isJumping = true;

        jumpSound = PlaySound(1.0f, cScale[10], 2.0 / 44100, -0.01, false);
    }
}

#pragma endregion Player Functions

void Restart()
{
    gameOver = false;

    gameSpeed = startGameSpeed;

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
    scoreSpeed = startScoreSpeed;
    frameCounter = 0;
    scoreTextPosX = 36;
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
        }
        else if (gameOver)
        {
            Restart();
        }
    }
    
    if (OnKeyPressed(KB_KEY_ESCAPE))
    {
        exit(1);
    }

    if (OnKeyPressed(KB_KEY_S))
    {
        SaveLevel();
    }

    if (OnKeyPressed(KB_KEY_L))
    {
        LoadLevel();
    }

    if (OnMousePressed(MOUSE_LEFT) && OnKeyBeingPressed(KB_KEY_LEFT_CONTROL))
    {
        SpawnEntity(mouseX, mouseY);
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

    mfb_get_monitor_scale(window, &monitorScaleX, &monitorScaleY);

    InitializeInputCallbacks(window);

    // Providing a seed value
	srand((unsigned int) time(NULL));

    windows_enable_colors_in_command_prompt();

    SetupSound();

    CreateBackground();

    rndFrame = GetRandomInt(50, 60);

    SoundClip clip = loadSoundClip("assets/RaceTheme.wav");
    PlaySoundClip(clip, 0.5f, 440, 0, 0, true);
}

#pragma region Update Functions

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
    DrawFullRect (((int)(FRAME_BUFFER_WIDTH / 2)), ((int)(FRAME_BUFFER_HEIGHT / 4)) + 4, 90, 28, BLACK);
    DrawEmptyRect(((int)(FRAME_BUFFER_WIDTH / 2)), ((int)(FRAME_BUFFER_HEIGHT / 4)) + 4, 90, 28, WHITE);
    DrawTextWithColor("press space", (int)(FRAME_BUFFER_WIDTH / 2), (int)(FRAME_BUFFER_HEIGHT / 4), WHITE);
    DrawTextWithColor("to start", ((int)(FRAME_BUFFER_WIDTH / 2)) + 1, ((int)(FRAME_BUFFER_HEIGHT / 4)) + 9, WHITE);
    
    DrawFullRect (((int)(FRAME_BUFFER_WIDTH / 2)), 3 * ((int)(FRAME_BUFFER_HEIGHT / 4)) + 4, 90, 28, BLACK);
    DrawEmptyRect(((int)(FRAME_BUFFER_WIDTH / 2)), 3 * ((int)(FRAME_BUFFER_HEIGHT / 4)) + 4, 90, 28, WHITE);
    DrawTextWithColor("press escape", (int)(FRAME_BUFFER_WIDTH / 2), 90, WHITE); 
    DrawTextWithColor("to exit", ((int)(FRAME_BUFFER_WIDTH / 2)) + 1, 99, WHITE); 
}

void HandleEnemySpawn()
{
    if (frameCounter % rndFrame == 0)
    {
        if (GetRandomInt(1, 5) == 5)
        {
            SpawnEnemy(birdSprite ,FRAME_BUFFER_WIDTH + GetRandomInt(20, 50), 70, player.score);
        }
        else 
        {
            SpawnEnemy(pigSprite ,FRAME_BUFFER_WIDTH + GetRandomInt(20, 50), 92, player.score);
        }

        rndFrame = GetRandomInt(50, 60);

        frameCounter = 0;
    }
}

void HandleFootstepsSound()
{
    if (soundFrameCounter % 10 - (frameToUpdateScore / 2) == 0 && !player.isJumping)
    {
        stepSoundCount++;

        if (stepSoundCount % 2 == 0)
        {
            PlaySound(2.f, cScale[4] + GetRandomInt(10, 20), 0.001, -0.01, false);
        }
        else 
        {
            PlaySound(2.f, cScale[0] + GetRandomInt(10, 20),  0.001, 0.01, false);
        }
    }
}

void UpdateScore()
{
    frameToUpdateScore = (int)(60 / scoreSpeed) > 0 ? (int)(60 / scoreSpeed) : 1;

    if (frameCounter % frameToUpdateScore == 0)
    {
        player.score++;

        if (player.score % 50 == 0)
        {
            gameSpeed += 0.2f;
            adjustedGameSpeed = (int)gameSpeed;
        }

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
}

void DisplayGameOverBox()
{
    DrawFullRect((int)(FRAME_BUFFER_WIDTH / 2)  - 45, ((int)(FRAME_BUFFER_HEIGHT / 3)) + 15 - 25, 90, 50, BLACK);
    DrawEmptyRect((int)(FRAME_BUFFER_WIDTH / 2) - 45, ((int)(FRAME_BUFFER_HEIGHT / 3)) + 15 - 25, 90, 50, RED);
    DrawTextWithColor("Game Over", (int)(FRAME_BUFFER_WIDTH / 2), (int)(FRAME_BUFFER_HEIGHT / 3) + 5, RED);
    DrawTextWithColor("press space", (int)(FRAME_BUFFER_WIDTH / 2), ((int)(FRAME_BUFFER_HEIGHT / 3)) + 20, RED);
    DrawTextWithColor("to restart", (int)(FRAME_BUFFER_WIDTH / 2),  ((int)(FRAME_BUFFER_HEIGHT / 3)) + 29, RED);
}

void UpdateGame()
{
    adjustedGameSpeed = remainingPixelToParcour >= 1 ? (int)gameSpeed + remainingPixelToParcour-- : (int)gameSpeed;
    remainingPixelToParcour += gameSpeed - (int)gameSpeed;

    for(int i = 0; i < enemyCount; i++)
    {
        Enemy* enemy = &enemies[i];

        if (IsCollisionDetected(player, *enemy) && !gameOver)
        {
            gameOver = true;
            c = PlaySound(1.f, cScale[0], 0.00001, 0.01, false);
            d = PlaySound(1.f, cScale[4], 0.00001, 0.01, false);
            g = PlaySound(1.f, cScale[7], 0.00001, 0.01, false);
        }

        enemy->xPos -= gameOver ? 0 : adjustedGameSpeed;

        DrawBitmap((unsigned char*)enemy->sprite.pixels, enemy->xPos, enemy->yPos, enemy->sprite.pixel_size_x, enemy->sprite.pixel_size_y);
        
        if (enemy->xPos + (int)enemy->sprite.pixel_size_x / 2 < 0)
        {
            enemies[i] = enemies[enemyCount-1];
            enemyCount--;
            i--;
        }
    }

    DrawText(scoreText, scoreTextPosX, scoreTextPosY);

    if (!gameOver)
    {
        HandleJump();

        HandleEnemySpawn();

        UpdateScore();

        HandleFootstepsSound();
    }

    else 
    {
        DisplayGameOverBox();
    }
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
        #else
        mfb_set_target_fps(60);
        #endif

        HandleInputs();

        UpdateLevelEditor();

        memcpy(previousKeyStates, keyStates, sizeof(keyStates));
        memcpy(previousMouseButtonStates, mouseButtonStates, sizeof(mouseButtonStates));

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

        DrawBackground(gameSpeed, adjustedGameSpeed, gameOver, gameStarted);
        DrawBitmap((unsigned char*)player.sprite.pixels, player.xPos, player.yPos, player.sprite.pixel_size_x, player.sprite.pixel_size_y);

        if (!gameStarted)
        {
            UpdateTitleScreen();
        }
        else
        {
            UpdateGame();

            frameCounter++;
            soundFrameCounter++;
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