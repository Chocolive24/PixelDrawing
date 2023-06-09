#pragma once

#include "AudioManager.cpp"
#include "Drawing.cpp"
#include "Input.cpp"
#include "LevelEditor.cpp"
#include "Math.cpp"
#include "Timer.cpp"

typedef struct Player
{
    // Technical attributes
    Vector2F position;
    Vector2F velocity;
    Vector2F size;
    float moveSpeed;

    float GetBottomPos()
    {
        return position.y + TILE_PX / 2;
    }
}
Player;

enum CollisionType
{
    TOP,
    RIGHT,
    LEFT,
    BOTTOM
};

// Variables 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

Player player
{ 
    player.position = Vector2F{12, 12},
    player.velocity = Vector2F{0, 0},
    player.size     = Vector2F{TILE_PX, TILE_PX},
    player.moveSpeed = 100.f
};

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Functions 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

void HandleMove()
{
    if (KeyBeingPressed(KB_KEY_D))
    {
        if (player.velocity.x < player.moveSpeed * deltaTime)
        {
            player.velocity.x += (player.moveSpeed / 10) * deltaTime;
        }
        else 
        {
            player.velocity.x = player.moveSpeed * deltaTime;
        }
    }   
    else if (KeyBeingPressed(KB_KEY_A))
    {
        if (player.velocity.x > -(player.moveSpeed) * deltaTime)
        {
            player.velocity.x -= (player.moveSpeed / 10) * deltaTime;
        }
        else 
        {
            player.velocity.x = -(player.moveSpeed) * deltaTime;
        }
    } 
    else 
    {
        player.velocity.x = 0;
    }
}

bool IsCollisionDetected()
{
    for (int y = 0; y < TILEMAP_HEIGHT_PX; y++)
    {
        for (int x = 0; x < TILEMAP_WIDTH_PX; x++)
        {
            int tile_type = tiles[y * TILEMAP_WIDTH_PX + x];

            if (player.position.x - (player.size.x / 2) < x + TILE_PX && 
                player.position.x + (player.size.x / 2) > x           &&
                player.position.y + (player.size.y / 2) >= y + TILE_PX )
            {
                return true;
            }
        }
    }

}

void UpdatePlayer()
{
    HandleMove();

    if (player.GetBottomPos() < FRAME_BUFFER_HEIGHT && !IsCollisionDetected())
    {
        player.velocity.y += player.velocity.y < 2 ? GRAVITY * deltaTime : 0.f;
    }
    else 
    {
        player.velocity.y = 0.f;
    }

    player.position += player.velocity;
    printf("%f %f\n", player.position.x, player.position.y);
    

    DrawFullRect(player.position.x, player.position.y, TILE_PX, TILE_PX, GREEN ,true);

    //DrawBitmap((unsigned char*)player.sprite.pixels, player.xPos, player.yPos, player.sprite.pixel_size_x, player.sprite.pixel_size_y);
}