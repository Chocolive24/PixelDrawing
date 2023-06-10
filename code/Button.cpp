#pragma once 

#include "Drawing.cpp"

struct Button
{
    Rect rect;
    Rect frame;
    uint32_t color = rect.color;
    uint32_t selectedColor;
    Text text;
    bool isSelected;
    void (*activateEffect)();

    // //Center the rect and frame
    // rect.xPos  -= rect.width   / 2;
    // rect.yPos  -= rect.height  / 2;
    // frame.xPos -= frame.width  / 2;
    // frame.yPos -= frame.height / 2;

    // Button()
    // {
    //     
    // };  

    // Button CreateButton(Rect rect, uint32_t color, uint32_t selectedColor, Text text, void (*actionFunc)())
    // {
    //     //Rect rect{xPos - width / 2, yPos - height / 2, width, height, color};

    //     Button button{rect, color, selectedColor, text, false, actionFunc};

    //     return button;
    // }

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

