#pragma once

#include "Drawing.cpp"

#define TILE_PX 8

enum TileType
{
    TILE_EMPTY = 0,
    TILE_GRASS = 1,
    TILE_WATER = 2,
    TILE_COUNT
};

struct Entity
{
    int xPos, yPos;
};

// Variables 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

int tiles[10 * 10];

Entity entities[100];
int entityCount = 0;

// Functions 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

void SaveLevel()
{
    LOG("Saving Level...");

    FILE* file = fopen("save.level", "wb");

    // Tiles
    fwrite(tiles, sizeof(tiles), 1, file);

    // Entites
    fwrite(&entityCount, sizeof(int), 1, file);
    fwrite(entities, sizeof(entities), 1, file);

    fclose(file);
}

void LoadLevel()
{
    LOG("Loading Level...");

    FILE* file = fopen("save.level", "rb");

    // Tiles
    fread(tiles, sizeof(tiles), 1, file);

    // Entities
    fread(&entityCount, sizeof(int), 1, file);
    fread(entities, sizeof(entities), 1, file);

    fclose(file);
}

void SpawnEntity(int x, int y)
{
    if (entityCount >= ARR_LEN(entities))
    {
        return;
    }

    Entity entity
    {
        entity.xPos = x, entity.yPos = y
    };

    entities[entityCount++] = entity;
}

void DrawTiles()
{
    for (int y = 0; y < 10; y++)
    {
        for (int x = 0; x < 10; x++)
        {
            int tile_type = tiles[y * 10 + x];

            if (tile_type == TILE_GRASS)
            {
                DrawFullRect(x * TILE_PX, y * TILE_PX, TILE_PX, TILE_PX, GREEN);
            }
            else if (tile_type == TILE_WATER)
            {
                DrawFullRect(x * TILE_PX, y * TILE_PX, TILE_PX, TILE_PX, BLUE);
            }

            if (mouseX >= x * TILE_PX && mouseX < (x * TILE_PX) + TILE_PX && 
                mouseY >= y * TILE_PX && mouseY < (y * TILE_PX) + TILE_PX)
            {
                if (OnMouseBeingPressed(MOUSE_LEFT))
                {
                    tiles[y * 10 + x] = TILE_GRASS;
                }
                if (OnMouseBeingPressed(MOUSE_RIGHT))
                {
                    tiles[y * 10 + x] = TILE_WATER;
                }
            }
        }
    }
}

void DrawEntities()
{
    for (int i = 0; i < entityCount; i++)
    {
        Entity* entity = &entities[i];

        DrawFullRect(entity->xPos, entity->yPos, 4, 4, RED);
    }
}

void UpdateLevelEditor()
{
    DrawTiles();

    DrawEntities();

    // Draw the cursor
    DrawEmptyRect(mouseX / TILE_PX * TILE_PX, mouseY / TILE_PX * TILE_PX, TILE_PX, TILE_PX, WHITE);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------
