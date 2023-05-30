#pragma once

#include "Drawing.cpp"

typedef struct Enemy
{
    bitmap_t sprite;
    int xPos, yPos;
    int halfWidth, halfHeight;
    int speed;
}
Enemy;

// Variables 
// ------------------------------------------------------------------------------------------------------------------------------

bitmap_t pigSprite = LoadImage("assets/pig.png");
bitmap_t birdSprite = LoadImage("assets/bird.png");

Enemy enemies[10];
int enemyCount;
int spawnTimeMultiplicator = 3;

// ------------------------------------------------------------------------------------------------------------------------------

// Functions
// ------------------------------------------------------------------------------------------------------------------------------

void SpawnEnemy(bitmap_t sprite, int xStart, int yStart, int playerScore)
{
    if (enemyCount == 10)
    {
        return;
    }

    Enemy enemy 
    { 
        enemy.sprite = sprite, 
        enemy.xPos = xStart, enemy.yPos = yStart,
        enemy.halfWidth = enemy.sprite.pixel_size_x / 2, enemy.halfHeight = enemy.sprite.pixel_size_y / 2,
        enemy.speed = (int)(playerScore / 200) + 2
    };

    enemies[enemyCount] = enemy;
    enemyCount++;
}

// ------------------------------------------------------------------------------------------------------------------------------