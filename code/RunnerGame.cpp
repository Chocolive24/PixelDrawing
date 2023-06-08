#include "AudioManager.cpp"
#include "Background.cpp"
#include "Drawing.cpp"
#include "Enemy.cpp"
#include "Input.cpp"
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

bool gameOver = false;

bool mustApplyGravity = false;

int   startGameSpeed = 2;
float gameSpeed = startGameSpeed;
int   adjustedGameSpeed = gameSpeed;
float remainingPixelToParcour;

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

int startScoreSpeed = 10;
int scoreSpeed = startScoreSpeed;
int frameCounter, rndFrame, frameToUpdateScore;

mfb_timer* timer = mfb_timer_create();
double lastRenderedFrameTime = -1000;
double deltaTime, nowTime;
double targetDeltaTime =  1.f/60.f;

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

void InitializeRunnerGame()
{
    CreateBackground();

    rndFrame = GetRandomInt(50, 60);

    SoundClip clip = loadSoundClip("assets/RaceTheme.wav");
    PlaySoundClip(clip, 0.5f, 440, 0, 0, true);
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

void UpdateRunnerGame()
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

    frameCounter++;
    soundFrameCounter++;
}