#pragma once 

#include "Drawing.cpp"
#include "Utility.cpp"

typedef struct Tile
{
    bitmap_t sprite;
    int xPos, yPos;
}
Tile;

// Variables 
// -------------------------------------------------------------------------------------------------------------------------------

bitmap_t groundSprites[2]
{
    LoadImage("assets/tile1.png"),
    LoadImage("assets/tile2.png")
};

Tile groundTiles[20];
int groundTileCount;

bitmap_t mountainsSprite = LoadImage("assets/Mountains.png");

Tile mountainTiles[3];
int mountainTileCount;

// -------------------------------------------------------------------------------------------------------------------------------

// Functions 
// -------------------------------------------------------------------------------------------------------------------------------

void SpawnGroundTile(int xStart, int yStart)
{
    if (groundTileCount == 20)
    {
        return;
    }

    Tile tile
    {
        tile.sprite = groundSprites[GetRandomNbr(0, 3)],
        tile.xPos = xStart, tile.yPos = yStart
    };

    groundTiles[groundTileCount] = tile;
    groundTileCount++;
}

void SpawnMountainTile(int xStart, int yStart)
{
    if (mountainTileCount == 2)
    {
        return;
    }

    Tile tile {tile.sprite = mountainsSprite, tile.xPos = xStart, tile.yPos = yStart};

    mountainTiles[mountainTileCount] = tile;
    mountainTileCount++;
}

void CreateFirstGroundTiles()
{
    for (int i = 0; i < 20; i++)
    {
        SpawnGroundTile(i * 20, 110);
    }
}

void CreateFirstMountainTiles()
{
    for (int i = 0; i < 2; i++)
    {
        SpawnMountainTile((FRAME_BUFFER_WIDTH / 2) + (i * 200), FRAME_BUFFER_HEIGHT / 2 - 10);
    }
}

void CreateBackground()
{
    CreateFirstGroundTiles();
    CreateFirstMountainTiles();
}

void DrawBackground(int gameSpeed, int adjustedGameSpeed, bool gameOver, bool gameStarted)
{
    for (int i = 0; i < groundTileCount; i++)
    {
        Tile* groundTile = &groundTiles[i];

        groundTile->xPos -= gameOver || !gameStarted? 0 : adjustedGameSpeed;

        DrawBitmap(((unsigned char*)groundTile->sprite.pixels), groundTile->xPos, groundTile->yPos, 
                    groundTile->sprite.pixel_size_x, groundTile->sprite.pixel_size_y);

        if (groundTile->xPos + (int)groundTile->sprite.pixel_size_x / 2 < 0)
        {
            groundTiles[i] = groundTiles[groundTileCount-1];
            groundTileCount--;
            i--;
            SpawnGroundTile(groundTiles[groundTileCount].xPos + 20, 110);
        }
    }

    for (int i = 0; i < mountainTileCount; i++)
    {
        Tile* moutainTile = &mountainTiles[i];

        moutainTile->xPos -= gameOver || !gameStarted ? 0 : (int)(gameSpeed / 2);

        DrawBitmap(((unsigned char*)moutainTile->sprite.pixels), moutainTile->xPos, moutainTile->yPos, 
                    moutainTile->sprite.pixel_size_x, moutainTile->sprite.pixel_size_y);

        if (moutainTile->xPos + (int)moutainTile->sprite.pixel_size_x / 2 < 0)
        {
            mountainTiles[i] = mountainTiles[mountainTileCount-1];
            mountainTileCount--;
            i--;
            SpawnMountainTile(mountainTiles[mountainTileCount].xPos + 200, FRAME_BUFFER_HEIGHT / 2 - 10);
        }
    }
}

// -------------------------------------------------------------------------------------------------------------------------------