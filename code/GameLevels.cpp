#pragma once 

#include "GUI.cpp"
#include "LevelEditor.cpp"

int currentLvl = 1;
const char* lvlNames[10] = 
{
    "assets//save1.level",
    "assets//save2.level",
    "assets//save3.level",
    "assets//save4.level",
    "assets//save5.level"
};

void RunNextLevel()
{
    currentLvl++;

    ClearTileButtons();

    switch(currentLvl)
    {
        case 2:
            CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tileSpringSprite,     TILE_SPRING, 2);
            break;
        case 3:
            CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tilePlayerWallSprite, TILE_PLAYER_WALL, 4);
            CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tileSpringSprite,     TILE_SPRING, 1);
            break;
        case 4:
            CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tilePlayerWallSprite, TILE_PLAYER_WALL, 7);
            CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tileSpringSprite,     TILE_SPRING, 1);
            break;
        case 5:
            CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tilePlayerWallSprite, TILE_PLAYER_WALL, 4);
            CreateTileButton(buttonPosX, buttonPosY += (2 * TILE_PX), tileSpringSprite,     TILE_SPRING, 1);
            break;
    }

    InitializeLevelEditor(true);

    LoadLevel(lvlNames[currentLvl - 1]);
}



// void CreateLevelSelectButton(int row, char* literalString, void (*activateEffect)())
// {
//     if (levelSelectButtonCount >= 5)
//     {
//         return;
//     }

//     Rect rect{row * 20,  FRAME_BUFFER_HEIGHT / 2, 20, 20, BLUE };

//     Text text{ literalString, row * 20,  FRAME_BUFFER_HEIGHT / 2, WHITE };

//     Rect frame {rect.xPos, rect.yPos, rect.width, rect.height, WHITE};

//     Button button{rect, frame, BLUE, LIGHT_BLUE, text, false, activateEffect};

//     levelSelectButtons[levelSelectButtonCount++] = button;
// }

// void InitializeLevelSelect()
// {
//     CreateLevelSelectButton(1, "1", &RunALevel);
// }

// void UpdateLevelSelection()
// {
    
// }