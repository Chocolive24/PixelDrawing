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

void RunPlayMod()
{
    playerRef->isEditingLvl = false;
    playerRef->SelfReset();
}

void RunEditMod()
{
    playerRef->isEditingLvl = true;
    playerRef->SelfReset();
}

void InitEditorButtons()
{
    Rect playEditorRectB{ 180, 95, 30, 13, BLUE };
    Text playEditorTextB{ "play", 180, 95, WHITE };

    CreateEditorButton(playEditorRectB, BLUE, LIGHT_BLUE, playEditorTextB, &RunPlayMod);

    Rect editEditorRectB{ 180, 110, 30, 13, BLUE };
    Text editEditorTextB{ "edit", 180, 110, WHITE };

    CreateEditorButton(editEditorRectB, BLUE, LIGHT_BLUE, editEditorTextB, &RunEditMod);
}

void UpdateEditorButtons()
{
    if (playerRef->isEditingLvl)
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

void InitializeGUI(Player& p)
{
    playerRef = &p;

    InitEditorButtons();
}

void UpdateGUI()
{
    UpdateEditorButtons();
}
    