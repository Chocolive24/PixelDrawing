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
    Vector2F startPos = Vector2F{12, 12};
    Vector2F position = Vector2F{startPos.x, startPos.y};
    Vector2F velocity = Vector2F{0, 0};
    Vector2Int size   = Vector2Int{TILE_PX, TILE_PX};
    int top = position.y - size.y / 2, right = position.x + size.x / 2, left = position.x - size.x / 2, bottom = position.y + size.y / 2;
    float moveSpeed   = 100.f;
    float initalJumpVelocity = 0.f, maxJumpHeight = 18.f, maxJumpTime = 0.5f, jumpGravity = 0.f;
    bool isJumping, isGrounded;

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------

    // Functions 
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------

    float GetBottomPos()
    {
        return position.y + TILE_PX / 2;
    }

    void SetupJumpVariables()
    {
        // th = t / 2
        // The Apex (sommet) time of the jump parabola is the half max jump time, because a parabola is symetric.
        float timeToApex = maxJumpTime / 2;

        // g = -2h / th^2
        jumpGravity = (2 * maxJumpHeight) / (timeToApex * timeToApex);

        // v0 = 2h / th 
        initalJumpVelocity = (2 * maxJumpHeight) / timeToApex;
    }

    bool IsRightCollision()
    {
        for (int y = 0; y < TILEMAP_HEIGHT_PX; y++)
        {
            for (int x = 0; x < TILEMAP_WIDTH_PX; x++)
            {
                int tile_type = tiles[y * TILEMAP_WIDTH_PX + x];

                if (position.x - (size.x / 2) <  (x * TILE_PX) + TILE_PX &&
                    position.x + (size.x / 2) >= (x * TILE_PX) &&
                    position.y - (size.y / 2) <  (y * TILE_PX) + TILE_PX &&
                    position.y + (size.y / 2) >  (y * TILE_PX) && tile_type == TILE_GROUND)
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

                if (position.x - (size.x / 2) <=  (x * TILE_PX) + TILE_PX &&
                    position.x + (size.x / 2) > (x * TILE_PX) &&
                    position.y - (size.y / 2) <  (y * TILE_PX) + TILE_PX &&
                    position.y + (size.y / 2) >  (y * TILE_PX) && tile_type == TILE_GROUND)
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

                if (position.x - (size.x / 2) <   (x * TILE_PX) + TILE_PX &&
                    position.x + (size.x / 2) >   (x * TILE_PX) &&
                    position.y - (size.y / 2) <   (y * TILE_PX) + TILE_PX &&
                    position.y + (size.y / 2) >=  (y * TILE_PX) && tile_type == TILE_GROUND)
                {
                   
                    return true;
                }
            }
        }

        return false;
    }

    bool bottomCol(int x, int y)
    {
        return      position.x - (size.x / 2) <   (x * TILE_PX) + TILE_PX &&
                    position.x + (size.x / 2) >   (x * TILE_PX) &&
                    position.y - (size.y / 2) <   (y * TILE_PX) + TILE_PX &&
                    position.y + (size.y / 2) >=  (y * TILE_PX);
    }

    void COl()
    {
        for (int y = 0; y < TILEMAP_HEIGHT_PX; y++)
        {
            for (int x = 0; x < TILEMAP_WIDTH_PX; x++)
            {
                int tileType = tiles[y * TILEMAP_WIDTH_PX + x];

                switch (tileType)
                {
                    case TILE_EMPTY:
                        continue;
                        break;
                    case TILE_GROUND:
                        // Apply effect B
                        if (bottomCol(x, y))
                        {
                            //isGrounded = true;
                            printf("YOOOOO \n");
                        }
                        break;
                    // Handle other tile types
                }
            }
        }

    }

    void HandleMove()
    {
        if (KeyBeingPressed(KB_KEY_D))
        {
            if (IsRightCollision())
            {
                velocity.x = 0;
                position.x = ((int)position.x / TILE_PX * TILE_PX) + 4;
                return;
            }
            if (velocity.x < moveSpeed)
            {
                velocity.x +=  (moveSpeed / 10);
            }
            else 
            {
                velocity.x = moveSpeed;
            }
        }   
        else if (KeyBeingPressed(KB_KEY_A))
        {
            if (IsLeftCollision())
            {
                velocity.x = 0;
                position.x = ((int)position.x / TILE_PX * TILE_PX) + 4;
                return;
            }

            if (velocity.x > -(moveSpeed))
            {
                velocity.x -= (moveSpeed / 10);
            }
            else 
            {
                velocity.x = -(moveSpeed);
            }
        } 
        else 
        {
            velocity.x = 0;
        }
    }

    void HandleJump()
    {
        if (KeyWasPressed(KB_KEY_SPACE) && !isJumping)
        {
            isJumping = true;
            velocity.y = -(initalJumpVelocity);
        }
    }

    void Update()
    {
        position += velocity * deltaTime;

        //COl();

        HandleMove();
    
        //printf("jump %f ", velocity.y * deltaTime);

        if (!IsBottomCollision()) //&& !IsBottomCollision())
        {
            //velocity.y += velocity.y < MAX_G_VEL ? GRAVITY * deltaTime : 0.f;
            velocity.y += isJumping ? jumpGravity * deltaTime : GRAVITY;
            isGrounded = false;
        }
        else if (IsBottomCollision() && !isGrounded && isJumping)
        {
            isJumping = false;
            isGrounded = true;
            velocity.y = 0.f;
            position.y = ((int)position.y / TILE_PX * TILE_PX) + 4;
        }
        else if (!isJumping)
        {
            isGrounded = true;
            velocity.y = 0.f;
            position.y = ((int)position.y / TILE_PX * TILE_PX) + 4;
        }

        HandleJump();

        
        //printf("gravity %f\n ", velocity.y * deltaTime);
        

        DrawFullRect(position.x, position.y, TILE_PX, TILE_PX, GREEN ,true);

        //DrawBitmap((unsigned char*)sprite.pixels, xPos, yPos, sprite.pixel_size_x, sprite.pixel_size_y);
    }
};

