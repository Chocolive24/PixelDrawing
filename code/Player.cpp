#pragma once

#include "AudioManager.cpp"
#include "Drawing.cpp"
#include "Input.cpp"
#include "LevelEditor.cpp"
#include "Math.cpp"
#include "Timer.cpp"

struct Player
{
    // Technical attributes
    Vector2F position;
    Vector2F velocity;
    Vector2F size;
    float moveSpeed;
    float initalJumpVelocity, maxJumpHeight, maxJumpTime;
    bool isJumping, isGrounded;

    float GetBottomPos()
    {
        return position.y + TILE_PX / 2;
    }
};

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
    player.moveSpeed = 100.f,
    player.initalJumpVelocity = 0.f, player.maxJumpHeight = 18.f, player.maxJumpTime = 0.5f

};

float jumpGravity;

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Functions 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

void SetupJumpVariables()
{
    // th = t / 2
    // The Apex (sommet) time of the jump parabola is the half max jump time, because a parabola is symetric.
    float timeToApex = player.maxJumpTime / 2;
    printf("timeToAPex %f \n", timeToApex);

    // g = -2h / th^2
    jumpGravity = (2 * player.maxJumpHeight) / (timeToApex * timeToApex);

    // v0 = 2h / th 
    player.initalJumpVelocity = (2 * player.maxJumpHeight) / timeToApex;

    printf("%f %f \n", jumpGravity, player.initalJumpVelocity);
}

bool IsRightCollision()
{
    for (int y = 0; y < TILEMAP_HEIGHT_PX; y++)
    {
        for (int x = 0; x < TILEMAP_WIDTH_PX; x++)
        {
            int tile_type = tiles[y * TILEMAP_WIDTH_PX + x];

            if (player.position.x - (player.size.x / 2) <  (x * TILE_PX) + TILE_PX &&
                player.position.x + (player.size.x / 2) >= (x * TILE_PX) &&
                player.position.y - (player.size.y / 2) <  (y * TILE_PX) + TILE_PX &&
                player.position.y + (player.size.y / 2) >  (y * TILE_PX) && tile_type == TILE_GROUND)
            {
                return true;
            }
        }
    }

    return false;
}

bool IsLeftCollision()
{
    for (int y = 0; y < TILEMAP_HEIGHT_PX; y++)
    {
        for (int x = 0; x < TILEMAP_WIDTH_PX; x++)
        {
            int tile_type = tiles[y * TILEMAP_WIDTH_PX + x];

            if (player.position.x - (player.size.x / 2) <=  (x * TILE_PX) + TILE_PX &&
                player.position.x + (player.size.x / 2) > (x * TILE_PX) &&
                player.position.y - (player.size.y / 2) <  (y * TILE_PX) + TILE_PX &&
                player.position.y + (player.size.y / 2) >  (y * TILE_PX) && tile_type == TILE_GROUND)
            {
                return true;
            }
        }
    }

    return false;
}

bool IsBottomCollision()
{
    for (int y = 0; y < TILEMAP_HEIGHT_PX; y++)
    {
        for (int x = 0; x < TILEMAP_WIDTH_PX; x++)
        {
            int tile_type = tiles[y * TILEMAP_WIDTH_PX + x];

            if (player.position.x - (player.size.x / 2) <  (x * TILE_PX) + TILE_PX &&
                player.position.x + (player.size.x / 2) >  (x * TILE_PX) &&
                player.position.y - (player.size.y / 2) <  (y * TILE_PX) + TILE_PX &&
                player.position.y + (player.size.y / 2) >=  (y * TILE_PX) && tile_type == TILE_GROUND)
            {
                return true;
            }
        }
    }

    return false;
}

void InitializePlayer()
{
    SetupJumpVariables();
}

void HandleMove()
{
    if (KeyBeingPressed(KB_KEY_D))
    {
        if (IsRightCollision())
        {
            player.velocity.x = 0;
            player.position.x = ((int)player.position.x / TILE_PX * TILE_PX) + 4;
            return;
        }
        if (player.velocity.x < player.moveSpeed)
        {
            player.velocity.x +=  (player.moveSpeed / 10);
        }
        else 
        {
            player.velocity.x = player.moveSpeed;
        }
    }   
    else if (KeyBeingPressed(KB_KEY_A))
    {
        if (IsLeftCollision())
        {
            player.velocity.x = 0;
            player.position.x = ((int)player.position.x / TILE_PX * TILE_PX) + 4;
            return;
        }

        if (player.velocity.x > -(player.moveSpeed))
        {
            player.velocity.x -= (player.moveSpeed / 10);
        }
        else 
        {
            player.velocity.x = -(player.moveSpeed);
        }
    } 
    else 
    {
        player.velocity.x = 0;
    }
}

void HandleJump()
{
    if (KeyWasPressed(KB_KEY_SPACE) && !player.isJumping)
    {
        player.isJumping = true;
        player.velocity.y = -(player.initalJumpVelocity);
        printf("%f, \n", player.initalJumpVelocity);
    }
}

void UpdatePlayer()
{
    player.position += player.velocity * deltaTime;

    HandleMove();
   
    //printf("jump %f ", player.velocity.y * deltaTime);

    if (!IsBottomCollision()) //&& !IsBottomCollision())
    {
        //player.velocity.y += player.velocity.y < MAX_G_VEL ? GRAVITY * deltaTime : 0.f;
        player.velocity.y += player.isJumping ? jumpGravity * deltaTime : GRAVITY;
        player.isGrounded = false;
    }
    else if (IsBottomCollision && !player.isGrounded && player.isJumping)
    {
        player.isJumping = false;
        player.isGrounded = true;
        player.velocity.y = 0.f;
        player.position.y = ((int)player.position.y / TILE_PX * TILE_PX) + 4;
    }
    else if (!player.isJumping)
    {
        player.isGrounded = true;
        player.velocity.y = 0.f;
        player.position.y = ((int)player.position.y / TILE_PX * TILE_PX) + 4;
    }

    HandleJump();

    

    //player.position += player.velocity * deltaTime;

    //printf("gravity %f\n ", player.velocity.y * deltaTime);
    

    DrawFullRect(player.position.x, player.position.y, TILE_PX, TILE_PX, GREEN ,true);

    //DrawBitmap((unsigned char*)player.sprite.pixels, player.xPos, player.yPos, player.sprite.pixel_size_x, player.sprite.pixel_size_y);
}