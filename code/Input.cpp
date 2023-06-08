#pragma once

#include <MiniFB.h>

#include "Utility.cpp"

// Variables 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

bool keyStates[KB_KEY_LAST + 1];
bool previousKeyStates[KB_KEY_LAST + 1];

bool mouseButtonStates[MOUSE_BTN_7 + 1];
bool previousMouseButtonStates[MOUSE_BTN_7 + 1];

int mouseX, mouseY;

// Functions 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Keyboard Events
// -----------------------------------------------------------------------------------------------------------------------------------

void OnKeyboardEvent(struct mfb_window *window, mfb_key key, mfb_key_mod mod, bool isPressed)
{
    keyStates[key] = isPressed;
}

bool KeyBeingPressed(mfb_key key)
{
    return keyStates[key];
}

bool KeyWasPressed(mfb_key key)
{
    return !previousKeyStates[key] && keyStates[key];
}

bool KeyWasReleased(mfb_key key)
{
    return previousKeyStates[key] && !keyStates[key];
}

// -----------------------------------------------------------------------------------------------------------------------------------

// Mouse Events
// -----------------------------------------------------------------------------------------------------------------------------------

void OnMouseMove(struct mfb_window *window, int x, int y)
{
    mouseX = x / WINDOW_FAC / monitorScaleX;
    mouseY = y / WINDOW_FAC / monitorScaleY;
}

void OnMouseButtonEvent(struct mfb_window *window, mfb_mouse_button button, mfb_key_mod mod, bool isPressed)
{
    mouseButtonStates[button] = isPressed;
}

bool MouseBeingPressed(mfb_mouse_button button)
{
    return previousMouseButtonStates[button];
}

bool MouseWasPressed(mfb_mouse_button button)
{
    return previousMouseButtonStates[button] && !mouseButtonStates[button];
}

// -----------------------------------------------------------------------------------------------------------------------------------

void InitializeInputCallbacks(mfb_window* window)
{
    mfb_set_keyboard_callback(window, OnKeyboardEvent);
    mfb_set_mouse_move_callback(window, OnMouseMove);
    mfb_set_mouse_button_callback(window, OnMouseButtonEvent);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------
