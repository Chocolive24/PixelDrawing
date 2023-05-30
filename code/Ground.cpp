#pragma once 

#include "Drawing.cpp"
#include "Utility.cpp"

typedef struct GroundTile
{
    bitmap_t sprite;
    int xPos, yPos;
}
GroundTile;

bitmap_t groundSprites[2]
{
    LoadImage("assets/tile1.png"),
    LoadImage("assets/tile2.png")
};

GroundTile groundTiles[20];
int groundTileCount;

void SpawnGroundTile(int xStart, int yStart)
{
    if (groundTileCount == 20)
    {
        return;
    }

    GroundTile tile
    {
        tile.sprite = groundSprites[GetRandomNbr(0, 3)],
        tile.xPos = xStart, tile.yPos = yStart
    };

    groundTiles[groundTileCount] = tile;
    groundTileCount++;
}

void CreateFirstGroundTiles()
{
    for (int i = 0; i < 20; i++)
    {
        SpawnGroundTile(i * 20, 110);
    }
}

void DrawGround(bool gameOver)
{
    for (int i = 0; i < groundTileCount; i++)
    {
        GroundTile* groundTile = &groundTiles[i];

        groundTile->xPos -= gameOver ? 0 : 2;

        DrawBitmap(((unsigned char*)groundTile->sprite.pixels), groundTile->xPos, groundTile->yPos, 
                    groundTile->sprite.pixel_size_x, groundTile->sprite.pixel_size_y);

        if (groundTile->xPos + (int)groundTile->sprite.pixel_size_x / 2 < 0)
        {
            groundTiles[i] = groundTiles[groundTileCount-1];
            groundTileCount--;
            i--;
            SpawnGroundTile(380, 110);
        }
    }
}
