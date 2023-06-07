#include "AudioManager.cpp"
#include "Background.cpp"
#include "Drawing.cpp"
#include "Enemy.cpp"
#include "Input.cpp"
#include "LevelEditor.cpp"
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

bool gameStarted = false;
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