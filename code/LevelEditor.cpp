#pragma once

#include "Drawing.cpp"
#include "Input.cpp"
#include "Utility.cpp"

#define TILE_PX 8

#define TILEMAP_WIDTH  (FRAME_BUFFER_WIDTH - (5 * TILE_PX))
#define TILEMAP_HEIGHT (FRAME_BUFFER_HEIGHT)

#define TILEMAP_WIDTH_PX  (TILEMAP_WIDTH  / TILE_PX)
#define TILEMAP_HEIGHT_PX (TILEMAP_HEIGHT / TILE_PX)

enum SerializeMode
{
    SER_MODE_READ = 0,
    SER_MODE_WRITE = 1,
};

enum TileType
{
    TILE_EMPTY = 0,
    TILE_GRASS = 1,
    TILE_WATER = 2,
    TILE_COUNT
};

struct Serializer
{
    SerializeMode mode;
    uint8_t* buffer; // 1 byte non signés (8 bits) = char*
    int      bufferCapacity;
    int      bufferUsed; // "cursor of were we are reading / writing in buffer"
};

struct TileButton
{
    int xPos, yPos;
    uint32_t color;
    bool isSelected;
    TileType tileType;
};

struct Entity
{
    int xPos, yPos;
};

// Variables 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Editor variables
// ---------------------------------------------------------------

Serializer historySteps[256];
int historyStepCount = 0;

TileButton tileButtons[10];
int tileButtonCount = 0;
uint32_t tileButtonSprites[10]
{
    GREEN, RED
};

TileButton* selectedButton = nullptr;

// ---------------------------------------------------------------

// Level variables 
// ---------------------------------------------------------------

int tiles[TILEMAP_WIDTH_PX * TILEMAP_HEIGHT_PX];

bool tilesModified = false; // For Undo system.

Entity entities[100];
int entityCount = 0;

// ---------------------------------------------------------------

// Functions 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

void Serialize(Serializer* serializer, void* ptr, size_t size)
{
    if (serializer->mode == SER_MODE_READ)
    {
        size_t remainingBytes = serializer->bufferCapacity - serializer->bufferUsed;

        if(remainingBytes < size)
        {
            LOG_ERROR("Serializer tried to read past end of buffer");
            exit(-1);
        }

        memcpy(ptr, serializer->buffer + serializer->bufferUsed, size);
        serializer->bufferUsed += size;
    }
    else 
    {
        size_t remainingCapacity = serializer->bufferCapacity - serializer->bufferUsed;

        if(remainingCapacity < size)
        {
            LOG_ERROR("Serializer out of memory");
            exit(-1);
        }

        memcpy(serializer->buffer + serializer->bufferUsed, ptr, size);
        serializer->bufferUsed += size;
    }
}

void SerializeLevel(Serializer* serializer)
{
    // Tiles
    Serialize(serializer, tiles, sizeof(tiles));

    // Entities
    Serialize(serializer, &entityCount, sizeof(int));
    Serialize(serializer, entities, sizeof(Entity) * entityCount);
}

void SaveLevel()
{
    LOG("Saving Level...");

    Serializer serializer{};
    serializer.mode = SER_MODE_WRITE;
    serializer.bufferCapacity = 10000;
    serializer.buffer = (uint8_t*)malloc(serializer.bufferCapacity);

    SerializeLevel(&serializer);

    FILE* file = fopen("save.level", "wb");
    fwrite(serializer.buffer, serializer.bufferUsed, 1, file);

    fclose(file);
}

void LoadLevel()
{
    LOG("Loading Level...");

    Span fileData = loadEntireFile("save.level");

    Serializer serializer{};
    serializer.mode = SER_MODE_READ;
    serializer.buffer = fileData.ptr;
    serializer.bufferCapacity = fileData.size;
    serializer.bufferUsed = 0; // Just to be explicit.

    SerializeLevel(&serializer);
}

void HistoryCommit()
{
    LOG("COMMIT");

    Serializer serializer{};
    serializer.mode = SER_MODE_WRITE;
    serializer.bufferCapacity = 10000;
    serializer.buffer = (uint8_t*)malloc(serializer.bufferCapacity);

    SerializeLevel(&serializer);

    historySteps[historyStepCount++] = serializer;
}

void Undo()
{
    LOG("UNDO");

    printf("history %i\n", historyStepCount);

    if (historyStepCount <= 1)
    {
        LOG("Nothing to undo");
        return;
    }

    if (historyStepCount >= ARR_LEN(historySteps) - 1)
    {
        LOG("WARNING : undo stack is full !");
        return;
    }

    Serializer serializer = historySteps[historyStepCount - 2];
    free(historySteps[historyStepCount - 1].buffer);
    historyStepCount--;

    serializer.mode = SER_MODE_READ;
    serializer.bufferCapacity = serializer.bufferUsed;
    serializer.bufferUsed = 0;

    SerializeLevel(&serializer);
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

void CreateTileButton(int xPos, int yPos, uint32_t color, TileType tileType)
{
    if (tileButtonCount >= TILE_COUNT)
    {
        return;
    }

    TileButton t { xPos, yPos, color, false, tileType };
    tileButtons[tileButtonCount++] = t;
}

void SelectButton(TileButton* button)
{
    selectedButton = button;
    selectedButton->isSelected = true;
}

void InitializeLevelEditor()
{
    // Commit a blank state for the tile map to undo the first paint.
    HistoryCommit();

    int xPos = TILEMAP_WIDTH + (2 * TILE_PX);
    int yPos = -TILE_PX;

    CreateTileButton(xPos, yPos += (2 * TILE_PX), GREEN, TILE_GRASS);
    CreateTileButton(xPos, yPos += (2 * TILE_PX), RED,   TILE_WATER);

    SelectButton(&tileButtons[0]);
}

void DrawButtonTiles()
{
    for (int i = 0; i < TILE_COUNT - 1; i++)
    {
        TileButton* t = &tileButtons[i];

        DrawFullRect(t->xPos, t->yPos, TILE_PX, TILE_PX, t->color);

        bool isMouseOverButton = mouseX >= t->xPos && mouseX < t->xPos + TILE_PX &&
                                 mouseY >= t->yPos && mouseY < t->yPos + TILE_PX;

        if (isMouseOverButton || t->isSelected)
        {
            DrawEmptyRect(t->xPos, t->yPos, TILE_PX, TILE_PX, WHITE);

            if (MouseWasPressed(MOUSE_LEFT) && isMouseOverButton)
            {
                // If there is a selected button, unselected it.
                if (selectedButton)
                {
                    selectedButton->isSelected = false;
                }

                // Select the new button.
                SelectButton(t);
            }
        }
    }
}

void DrawLevelTiles()
{
    for (int y = 0; y < TILEMAP_HEIGHT_PX; y++)
    {
        for (int x = 0; x < TILEMAP_WIDTH_PX; x++)
        {
            int tile_type = tiles[y * TILEMAP_WIDTH_PX + x];

            if (tile_type == TILE_GRASS)
            {
                DrawFullRect(x * TILE_PX, y * TILE_PX, TILE_PX, TILE_PX, GREEN);
            }
            else if (tile_type == TILE_WATER)
            {
                DrawFullRect(x * TILE_PX, y * TILE_PX, TILE_PX, TILE_PX, RED);
            }
            else 
            {
                DrawFullRect(x * TILE_PX, y * TILE_PX, TILE_PX, TILE_PX, LIGHT_BLUE);
            }

            if (mouseX >= x * TILE_PX && mouseX < (x * TILE_PX) + TILE_PX && 
                mouseY >= y * TILE_PX && mouseY < (y * TILE_PX) + TILE_PX)
            {
                if (MouseBeingPressed(MOUSE_LEFT))
                {
                    if (!selectedButton)
                    {
                        return;
                    }

                    tiles[y * TILEMAP_WIDTH_PX + x] = selectedButton->tileType;
                    tilesModified = true;
                }
                if (MouseBeingPressed(MOUSE_RIGHT))
                {
                    int currentTileType = tiles[y * TILEMAP_WIDTH_PX + x];

                    if (currentTileType != TILE_EMPTY)
                    {
                        tiles[y * TILEMAP_WIDTH_PX + x] = TILE_EMPTY;
                        tilesModified = true;
                    }
                }

                if (MouseWasPressed(MOUSE_LEFT) && KeyBeingPressed(KB_KEY_LEFT_CONTROL))
                {
                    SpawnEntity(mouseX, mouseY);
                    tilesModified = true;
                }
            }
        }
    }
}

void ResetTilemap()
{
    for (int y = 0; y < TILEMAP_HEIGHT_PX; y++)
    {
        for (int x = 0; x < TILEMAP_WIDTH_PX; x++)
        {
            tiles[y * TILEMAP_WIDTH_PX + x] = TILE_EMPTY;
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
    if (KeyWasPressed(KB_KEY_S))
    {
        SaveLevel();
    }

    if (KeyWasPressed(KB_KEY_L))
    {
        LoadLevel();
    }

    if ((KeyBeingPressed(KB_KEY_LEFT_CONTROL) || KeyBeingPressed(KB_KEY_LEFT_SUPER)) && KeyWasPressed(KB_KEY_Y)) // Z
    {
        Undo();
    }

    if ((KeyBeingPressed(KB_KEY_LEFT_CONTROL) || KeyBeingPressed(KB_KEY_LEFT_SUPER)) && KeyWasPressed(KB_KEY_D)) // Z
    {
        ResetTilemap();
    }

    DrawButtonTiles();

    DrawLevelTiles();

    DrawEntities();

    if (mouseX >= 0 && mouseX < TILEMAP_WIDTH && 
        mouseY >= 0 && mouseY < TILEMAP_HEIGHT)
    {
        // Draw the cursor
        DrawEmptyRect(mouseX / TILE_PX * TILE_PX, mouseY / TILE_PX * TILE_PX, TILE_PX, TILE_PX, WHITE);
    }

    if (tilesModified && (MouseWasPressed(MOUSE_LEFT) || MouseWasPressed(MOUSE_RIGHT)))
    {
        HistoryCommit();
        tilesModified = false;
    }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------
