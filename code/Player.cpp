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
    bool isEditingLvl = true;

    // Graphical attributes
    bitmap_t sprite = LoadImage("assets/bopy.png");

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

    void SelfReset()
    {
        SetupJumpVariables();
        position.x = startPos.x;
        position.y = startPos.y;
        velocity = Vector2F{0, 0};
        isJumping = false;
        isGrounded = false;
    }

    bool IsRightCollision()
    {
        for (int y = 0; y < TILEMAP_HEIGHT_PX; y++)
        {
            for (int x = 0; x < TILEMAP_WIDTH_PX; x++)
            {
                int tile_type = tiles[y * TILEMAP_WIDTH_PX + x];

                if (position.x - (size.x / 2) <  (x * TILE_PX) + TILE_PX &&
                    position.x + (size.x / 2) > (x * TILE_PX) &&
                    position.y - (size.y / 2) <  (y * TILE_PX) + TILE_PX &&
                    position.y + (size.y / 2) >  (y * TILE_PX) && tile_type == TILE_WALL)
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

                if (position.x - (size.x / 2) <  (x * TILE_PX) + TILE_PX &&
                    position.x + (size.x / 2) > (x * TILE_PX) &&
                    position.y - (size.y / 2) <  (y * TILE_PX) + TILE_PX &&
                    position.y + (size.y / 2) >  (y * TILE_PX) && tile_type != TILE_EMPTY)
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool IsBottomCollision(TileType type)
    {
        for (int y = 0; y < TILEMAP_HEIGHT_PX; y++)
        {
            for (int x = 0; x < TILEMAP_WIDTH_PX; x++)
            {
                int tile_type = tiles[y * TILEMAP_WIDTH_PX + x];

                if (position.x - (size.x / 2) <   (x * TILE_PX) + TILE_PX &&
                    position.x + (size.x / 2) >   (x * TILE_PX) &&
                    position.y - (size.y / 2) <   (y * TILE_PX) + TILE_PX &&
                    position.y + (size.y / 2) >=  (y * TILE_PX) && (tile_type == type) && position.y < (y * TILE_PX))
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool IsCollision(int x, int y)
    {
        return  position.x - (size.x / 2) <   (x * TILE_PX) + TILE_PX &&
                position.x + (size.x / 2) >   (x * TILE_PX) &&
                position.y - (size.y / 2) <   (y * TILE_PX) + TILE_PX &&
                position.y + (size.y / 2) >   (y * TILE_PX);
    }

    void CheckCollisions()
    {
        for (int y = 0; y < TILEMAP_HEIGHT_PX; y++)
        {
            for (int x = 0; x < TILEMAP_WIDTH_PX; x++)
            {
                int tileType = tiles[y * TILEMAP_WIDTH_PX + x];

                if (!IsCollision(x, y))
                {
                    continue;
                }

                switch (tileType)
                {
                    case TILE_EMPTY:
                        continue;
                        break;
                    case TILE_WALL:
                    case TILE_PLAYER_WALL:
                        // if (position.y < (y * TILE_PX))
                        // {
                        //     isGrounded = true;
                        //     printf("%f %i \n", position.y, (y * TILE_PX) + TILE_PX / 2);
                        // }

                        // if (IsCollision(x, y) && position.y > (y * TILE_PX))
                        // {
                        //     // Up Collision
                        // }

                        break;
                    case TILE_FIRE:
                        SelfReset();
                        break;
                    case TILE_FRUIT:
                        LOG("YOU WOON !!!!");
                        break;
                    
                    // Handle other tile types
                }
            }
        }

    }

    bool IsGrounded()
    {
        for (int y = 0; y < TILEMAP_HEIGHT_PX; y++)
        {
            for (int x = 0; x < TILEMAP_WIDTH_PX; x++)
            {
                int tileType = tiles[y * TILEMAP_WIDTH_PX + x];

                if (tileType == TILE_WALL || tileType == TILE_PLAYER_WALL)
                {
                    if (IsCollision(x, y) && position.y < (y * TILE_PX))
                    {
                        return true;
                    }
                }
            }
        }

        return false;
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
            isGrounded = false;
            velocity.y = -(initalJumpVelocity);
            jumpSound = PlaySound(1.0f, cScale[10], 2.0 / 44100, -0.01, false);
        }
    }

    void Update()
    {
        if (KeyWasPressed(KB_KEY_ENTER))
        {
            isEditingLvl = false;
        }

        if (KeyWasPressed(KB_KEY_R))
        {
            isEditingLvl = true;
            SelfReset();
        }

        if (!isEditingLvl)
        {   
            position += velocity * deltaTime;

            HandleMove();

            CheckCollisions();

            // if (!IsGrounded())
            // {
            //     velocity.y += isJumping ? jumpGravity * deltaTime : GRAVITY;
            // }
            // else
            // {
            //     printf("%f \n", velocity.x);
            //     isJumping = false;
            //     velocity.y = 0.f;
            //     position.y = ((int)position.y / TILE_PX * TILE_PX) + 3;
            // }

            if (!IsBottomCollision(TILE_WALL) && !IsBottomCollision(TILE_PLAYER_WALL))
            {
                velocity.y += isJumping ? jumpGravity * deltaTime : GRAVITY;
            }
            else if ((IsBottomCollision(TILE_WALL) || IsBottomCollision(TILE_PLAYER_WALL)) && !isGrounded && isJumping)
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

            if (IsBottomCollision(TILE_SPRING))
            {
                printf("yo");
                isJumping = true;
                isGrounded = false;
                velocity.y = -(initalJumpVelocity * 1.4f);

                jumpSound = PlaySound(1.0f, cScale[10], 2.0 / 44100, -0.03, false);
            }

            HandleJump();

            


            //printf("gravity %f\n ", velocity.y * deltaTime);
        }
       
        

        //DrawFullRect(position.x, position.y, TILE_PX, TILE_PX, GREEN ,true);

        DrawBitmap((unsigned char*)sprite.pixels, position.x, position.y, sprite.pixel_size_x, sprite.pixel_size_y, true);

    }
};

