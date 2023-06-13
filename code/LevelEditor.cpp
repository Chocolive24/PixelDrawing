#pragma once

#include "GUI.cpp"
#include "Drawing.cpp"
#include "Input.cpp"
#include "Math.cpp"
#include "Utility.cpp"
#include "Timer.cpp"

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
    TILE_WALL,
    TILE_PLAYER_WALL,
    TILE_FIRE,
    TILE_SPRING,
    TILE_FRUIT,
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
    bitmap_t sprite;
    bool isSelected;
    TileType tileType;
    int tileNbr;
    Text tileNbrTxt;
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

bitmap_t tileWallSprite = LoadImage("assets/tileGreyWall.png");
bitmap_t tilePlayerWallSprite = LoadImage("assets/tileOrangeWall.png");
bitmap_t tileFireSprite = LoadImage("assets/tileFire.png");
bitmap_t tileSpringSprite = LoadImage("assets/tileSpring.png");
bitmap_t tileFruitSprite = LoadImage("assets/tileFruit.png");

TileButton* selectedButton = nullptr;

int buttonPosX, buttonPosY;
uint32_t cursorColor = WHITE;
bool isEmplacementValid;

bool isInGame, isPlayerEditing;

// ---------------------------------------------------------------

// Level variables 
// ---------------------------------------------------------------

int tiles[TILEMAP_WIDTH_PX * TILEMAP_HEIGHT_PX];
int levelTiles[TILEMAP_WIDTH_PX * TILEMAP_HEIGHT_PX];

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

void SaveLevel()
{
    LOG("Saving Level...");

    Serializer serializer{};
    serializer.mode = SER_MODE_WRITE;
    serializer.bufferCapacity = 10000;
    serializer.buffer = (uint8_t*)malloc(serializer.bufferCapacity);

    SerializeLevel(&serializer);

    FILE* file = fopen("assets//save.level", "wb");
    fwrite(serializer.buffer, serializer.bufferUsed, 1, file);

    fclose(file);
}

void LoadLevel()
{
    LOG("Loading Level...");

    Span fileData = loadEntireFile("assets//save.level");

    Serializer serializer{};
    serializer.mode = SER_MODE_READ;
    serializer.buffer = fileData.ptr;
    serializer.bufferCapacity = fileData.size;
    serializer.bufferUsed = 0; // Just to be explicit.

    SerializeLevel(&serializer);

    if (isInGame)
    {
        memcpy(levelTiles, tiles, sizeof(tiles));
    }

    //HistoryCommit();
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

void CreateTileButton(int xPos, int yPos, bitmap_t sprite, TileType tileType, int tileNbr)
{
    if (tileButtonCount >= 10)
    {
        return;
    }

    char literalString[4];
    Text text {literalString, xPos +  2 * TILE_PX, yPos + 2, WHITE };

    TileButton t { xPos, yPos, sprite, false, tileType, tileNbr, text };
    tileButtons[tileButtonCount++] = t;
}

void SelectButton(TileButton* button)
{
    selectedButton = button;
    selectedButton->isSelected = true;
}

void ClearTileButtons()
{
    buttonPosX = TILEMAP_WIDTH + (2 * TILE_PX);
    buttonPosY = -TILE_PX;

    //memset(tileButtons, 0, sizeof(tileButtons));

    for (int i = 0; i < tileButtonCount; i++)
    {
        tileButtons[i] = tileButtons[tileButtonCount--];
        i--;
    }
}

int GetTileAtPosition(int x, int y)
{
    return tiles[y * TILEMAP_WIDTH_PX + x];
}

Vector2Int GetPositionInTilemap(int x, int y)
{
    return Vector2Int{x / TILE_PX * TILE_PX, y / TILE_PX * TILE_PX };
}

void InitializeLevelEditor(bool isEditorInGame)
{
    isInGame = isEditorInGame;

    // Commit a blank state for the tile map to undo the first paint.
    HistoryCommit();

    for (int i = 0; i < tileButtonCount; i++)
    {
        TileButton* t = &tileButtons[i];

        t->isSelected = false;
    }

    SelectButton(&tileButtons[0]);
}

void DrawTileButtons()
{
    for (int i = 0; i < tileButtonCount; i++)
    {
        TileButton* t = &tileButtons[i];

        DrawBitmap((unsigned char*)t->sprite.pixels, t->xPos, t->yPos, t->sprite.pixel_size_x, t->sprite.pixel_size_y, false);

        //DrawFullRect(t->xPos, t->yPos, TILE_PX, TILE_PX, t->color, false);

        if (isInGame)
        {
            //snprintf(t->tileNbrTxt.literalString, 10, "%i", t->tileNbr);
            sprintf(t->tileNbrTxt.literalString, "%i", t->tileNbr);

            DrawMyText(t->tileNbrTxt.literalString, t->tileNbrTxt.xPos, t->tileNbrTxt.yPos);
        }

        bool isMouseOverButton = mouseX >= t->xPos && mouseX < t->xPos + TILE_PX &&
                                 mouseY >= t->yPos && mouseY < t->yPos + TILE_PX;

        if (isMouseOverButton || t->isSelected)
        {
            DrawEmptyRect(t->xPos - 1, t->yPos - 1, TILE_PX + 2, TILE_PX + 2, WHITE, false);

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

            if (tile_type == TILE_WALL)
            {
                DrawBitmap((unsigned char*)tileWallSprite.pixels, x * TILE_PX, y * TILE_PX, 
                            tileWallSprite.pixel_size_x, tileWallSprite.pixel_size_y, false);
            }
            else if (tile_type == TILE_PLAYER_WALL)
            {
                DrawBitmap((unsigned char*)tilePlayerWallSprite.pixels, x * TILE_PX, y * TILE_PX, 
                            tilePlayerWallSprite.pixel_size_x, tilePlayerWallSprite.pixel_size_y, false);
            }
            else if (tile_type == TILE_FIRE)
            {
                DrawBitmap((unsigned char*)tileFireSprite.pixels, x * TILE_PX, y * TILE_PX, 
                            tileFireSprite.pixel_size_x, tileFireSprite.pixel_size_y, false);
            }
            else if (tile_type == TILE_SPRING)
            {
                DrawBitmap((unsigned char*)tileSpringSprite.pixels, x * TILE_PX, y * TILE_PX, 
                            tileSpringSprite.pixel_size_x, tileSpringSprite.pixel_size_y, false);
            }
            else if (tile_type == TILE_FRUIT)
            {
                 DrawBitmap((unsigned char*)tileFruitSprite.pixels, x * TILE_PX, y * TILE_PX, 
                            tileFruitSprite.pixel_size_x, tileFruitSprite.pixel_size_y, false);
            }
            else 
            {
                DrawFullRect(x * TILE_PX, y * TILE_PX, TILE_PX, TILE_PX, LIGHT_BLUE, false);
            }

            if (mouseX >= x * TILE_PX && mouseX < (x * TILE_PX) + TILE_PX && 
                mouseY >= y * TILE_PX && mouseY < (y * TILE_PX) + TILE_PX)
            {
                if (!selectedButton)
                {
                    return;
                }

                if (isInGame && selectedButton->tileNbr <= 0)
                {
                    isEmplacementValid = false;
                }
                else if (isInGame && tiles[y * TILEMAP_WIDTH_PX + x] != TILE_EMPTY)
                {
                    isEmplacementValid = false;
                }
                else 
                {
                    isEmplacementValid = true;
                }

                if (selectedButton->tileType == TILE_SPRING)
                {
                    if (tiles[(y + 1) * TILEMAP_WIDTH_PX + x] != TILE_WALL && tiles[(y + 1) * TILEMAP_WIDTH_PX + x] != TILE_PLAYER_WALL)
                    {
                        isEmplacementValid = false;
                    }
                }
                
                if (isInGame && !isPlayerEditing)
                {
                    continue;
                }

                if (MouseBeingPressed(MOUSE_LEFT) && isEmplacementValid)
                {
                    tiles[y * TILEMAP_WIDTH_PX + x] = selectedButton->tileType;
                    tilesModified = true;

                    if (isInGame)
                    {
                        selectedButton->tileNbr--;
                    }
                }

                if (MouseBeingPressed(MOUSE_RIGHT))
                {
                    int currentTileType = tiles[y * TILEMAP_WIDTH_PX + x];

                    if (currentTileType != TILE_EMPTY)
                    {
                        if (isInGame)
                        {
                            if (levelTiles[y * TILEMAP_WIDTH_PX + x] ==  currentTileType)
                            {
                                continue;
                            }

                            for (int i = 0; i < tileButtonCount; i++)
                            {
                                if (tileButtons[i].tileType == tiles[y * TILEMAP_WIDTH_PX + x] )
                                {
                                    tileButtons[i].tileNbr++;
                                }
                            }
                        }

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

        DrawFullRect(entity->xPos, entity->yPos, 4, 4, RED, false);
    }
}

void UpdateLevelEditor()
{
    if (!isInGame)
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

        if ((KeyBeingPressed(KB_KEY_LEFT_CONTROL) || KeyBeingPressed(KB_KEY_LEFT_SUPER)) && KeyWasPressed(KB_KEY_D))
        {
            ResetTilemap();
        }
    }   

    DrawTileButtons();
    
    DrawLevelTiles();

    DrawEntities();

    if (mouseX >= 0 && mouseX < TILEMAP_WIDTH && 
        mouseY >= 0 && mouseY < TILEMAP_HEIGHT)
    {
        // Draw the cursor
        Vector2Int mPosInTilemap = GetPositionInTilemap(mouseX, mouseY);

        cursorColor = isEmplacementValid ? WHITE : RED;

        DrawEmptyRect( mPosInTilemap.x, mPosInTilemap.y, TILE_PX, TILE_PX, cursorColor, false);
    }

    if (tilesModified && (MouseWasPressed(MOUSE_LEFT) || MouseWasPressed(MOUSE_RIGHT)))
    {
        HistoryCommit();
        tilesModified = false;
    }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------
