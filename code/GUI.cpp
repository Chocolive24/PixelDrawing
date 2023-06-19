#pragma once 

#include "Drawing.cpp"
#include "Player.cpp"

// Structs 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

struct Button
{
    Rect rect;
    Rect frame;
    uint32_t color = rect.color;
    uint32_t selectedColor;
    Text text;
    bool isSelected;
    void (*activateEffect)();

    void Draw()
    {
        color = isSelected ? selectedColor : rect.color;

        DrawFullRect (rect.xPos, rect.yPos, rect.width, rect.height, color, true);
        DrawEmptyRect(rect.xPos, rect.yPos,  rect.width, rect.height, WHITE, true);

        DrawTextWithColor(text.literalString, text.xPos, text.yPos, text.color);
    }

    void Update()
    {
        // Buttons are always centered
        isSelected = IsMouseOverButton(rect.xPos - rect.width / 2, rect.yPos - rect.height / 2, rect.width, rect.height); 

        Draw();
    }
};

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Variables 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

Button editorButtons[2];
int editorButtonCount;

Button titleScreenButtons[3];
int  titleScrButtonCount = 0;

bool gameStarted, levelSelectOpen ,editorOpen;

Player* playerRef = nullptr;

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Functions 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

void CreateEditorButton(Rect rect, uint32_t color, uint32_t selectedColor, Text text, void (*activateEffect)())
{
    if (editorButtonCount >= 2)
    {
        return;
    }

    Rect frame {rect.xPos, rect.yPos, rect.width, rect.height, WHITE};

    Button button{rect, frame, color, selectedColor, text, false, activateEffect};

    editorButtons[editorButtonCount++] = button;
}

void RunPlayMode()
{
    playerRef->OnPlayMode();
}

void RunEditMode()
{
    playerRef->isEditingLvl = true;
    isPlayerEditing = true;
    playerRef->SelfReset();
}

void InitEditorButtons()
{
    Rect playEditorRectB{ 180, 95, 30, 13, BLUE };
    Text playEditorTextB{ "play", 180, 95, WHITE };

    CreateEditorButton(playEditorRectB, BLUE, LIGHT_BLUE, playEditorTextB, &RunPlayMode);

    Rect editEditorRectB{ 180, 110, 30, 13, BLUE };
    Text editEditorTextB{ "edit", 180, 110, WHITE };

    CreateEditorButton(editEditorRectB, BLUE, LIGHT_BLUE, editEditorTextB, &RunEditMode);
}



void RunGame()
{
    ClearTileButtons();

    CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tilePlayerWallSprite, TILE_PLAYER_WALL, 8);

    InitializeLevelEditor(true);
    LoadLevel("assets//save1.level");
    currentLvl = 1;
    
    playerRef->SelfReset();
    playerRef->isEditingLvl = true;
    isPlayerEditing = true;

    gameStarted = true;
}

void RunLevelSelection()
{
    levelSelectOpen = true;
    LOG("LEVEL SELECTION");
}

void RunLevelEditor()
{
    ClearTileButtons();

    CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tileWallSprite,    TILE_WALL, 8);
    CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tileFireSprite,    TILE_FIRE, 5);
    CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tileSpringSprite,  TILE_SPRING, 2);
    CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tileFruitSprite,   TILE_FRUIT, 9);
    CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tilePlayer,        TILE_PLAYER_SPAWN, 1);

    InitializeLevelEditor(false);
    ResetTilemap();
    editorOpen = true;
}

void CreateTitleScreenButton(int row, char* litteralString, void (*activateEffect)())
{
    if (titleScrButtonCount >= 3)
    {
        return;
    }

    Rect rect{ ((int)(FRAME_BUFFER_WIDTH / 2)), row * ((int)(FRAME_BUFFER_HEIGHT / 4)), 100, 20, BLUE };

    Text text{ litteralString, ((int)(FRAME_BUFFER_WIDTH / 2)), row * ((int)(FRAME_BUFFER_HEIGHT / 4)), WHITE };

    Rect frame {rect.xPos, rect.yPos, rect.width, rect.height, WHITE};

    Button button{rect, frame, BLUE, LIGHT_BLUE, text, false, activateEffect};

    titleScreenButtons[titleScrButtonCount++] = button;
}

void CreateTitleScreen()
{
    CreateTitleScreenButton(1.5, "start game",   &RunGame);

    //CreateTitleScreenButton(2, "select level", &RunLevelSelection);

    CreateTitleScreenButton(3, "level editor", &RunLevelEditor);
}



void InitializeGUI(Player& p)
{
    playerRef = &p;

    InitEditorButtons();
}

void UpdateTitleScreen()
{
    for (int i = 0; i < titleScrButtonCount; i++)
    {
        Button* b = &titleScreenButtons[i];

        b->Update();

        if (b->isSelected && MouseWasPressed(MOUSE_LEFT))
        {
            b->activateEffect();
        }
    }
}

void UpdateEditorButtons()
{
    if (playerRef->started)
    {
        for (int i = 0; i < editorButtonCount; i++)
        {
            Button* b = &editorButtons[i];

            b->Update();

            if (b->isSelected && MouseWasPressed(MOUSE_LEFT))
            {
                b->activateEffect();
            }
        }
    }
}



void UpdateGUI()
{
    UpdateEditorButtons();
}
    