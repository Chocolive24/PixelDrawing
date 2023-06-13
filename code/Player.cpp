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
    Vector2F startPos = Vector2F{12, TILEMAP_HEIGHT - 12};
    Vector2F position = Vector2F{startPos.x, startPos.y};
    Vector2F velocity = Vector2F{0, 0};
    Vector2Int size   = Vector2Int{TILE_PX, TILE_PX};
    int top = position.y - size.y / 2, right = position.x + size.x / 2, left = position.x - size.x / 2, bottom = position.y + size.y / 2;
    float moveSpeed   = 80.f;
    float initalJumpVelocity = 0.f, maxJumpHeight = 18.f, maxJumpTime = 0.5f, jumpGravity = 0.f;
    bool isJumping, isGrounded;
    bool isEditingLvl = false;
    bool hasFinishTheLevel = false;
    bool started;

    int topTile, rigthTile, bottomTile, leftTile;
    

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
        started = true;
        SetupJumpVariables();
        position.x = startPos.x;
        position.y = startPos.y;
        velocity = Vector2F{0, 0};
        isJumping = false;
        isGrounded = false;
        hasFinishTheLevel = false;
    }

    void GetCollidingTiles()
    {
        topTile    = (position.y - (size.y / 2)) / TILE_PX;
        rigthTile  = (position.x + (size.x / 2)) / TILE_PX;
        bottomTile = (position.y + (size.x / 2)) / TILE_PX;
        leftTile   = (position.x - (size.x / 2)) / TILE_PX;

        if (topTile  < 0) topTile  = 0;
        if (rigthTile  > TILEMAP_WIDTH_PX)  rigthTile  = TILEMAP_WIDTH_PX;
        if (bottomTile > TILEMAP_HEIGHT_PX) bottomTile = TILEMAP_HEIGHT_PX;
        if (leftTile < 0) leftTile = 0;
        
        //printf("%i %i \n", topTile, rigthTile);
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
        for (int y = topTile; y <= bottomTile; y++)
        {
            for (int x = leftTile; x <= rigthTile; x++)
            {
                int tileType = tiles[y * TILEMAP_WIDTH_PX + x];

                if (!IsCollision(x, y))
                {
                    continue;
                }

                DrawEmptyRect(x * TILE_PX, y * TILE_PX, 8, 8, RED, false);

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
                            
                        //     return;
                        //     printf("%f %i \n", position.y, (y * TILE_PX) + TILE_PX / 2);
                        // }
                        if (position.y > (y * TILE_PX))
                        {
                            velocity.y = 0.f;
                        }

                        if (std::abs(position.x - x + 4) <= 2 || std::abs(position.y - y + 4) <= 2)
                        {
                            position.x = position.x / TILE_PX * TILE_PX;
                            position.y = position.y / TILE_PX * TILE_PX;
                            printf("DEBUG \n");
                        }

                        break;
                    case TILE_FIRE:
                        SelfReset();
                        break;
                    case TILE_FRUIT:
                        //hasFinishTheLevel = true;
                        if (!winTheme.samples)
                        {
                            winTheme = loadSoundClip("assets/winTheme.wav");
                            PlaySoundClip(winTheme, 1.f, 440, 0, 0, false);
                        }
                        break;
                    case TILE_SPRING:
                        if (position.y < (y * TILE_PX))
                        {
                            isJumping = true;
                            isGrounded = false;
                            velocity.y = -(initalJumpVelocity * 1.4f);
                            jumpSound = PlaySound(1.0f, cScale[10], 2.0 / 44100, -0.03, false);
                        }
                        break;
                    // Handle other tile types
                }
            }
        }
    }

    bool IsGrounded()
    {
        for (int y = topTile; y <= bottomTile; y++)
        {
            for (int x = leftTile; x <= rigthTile; x++)
            {
                int tileType = tiles[y * TILEMAP_WIDTH_PX + x];

                if (!IsCollision(x, y))
                {
                    continue;
                }

                if (tileType == TILE_WALL || tileType == TILE_PLAYER_WALL)
                {
                    if (position.y < (y * TILE_PX))
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
                velocity.x +=  (moveSpeed / 5);
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
                velocity.x -= (moveSpeed / 5);
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
        if (KeyWasPressed(KB_KEY_SPACE) && !isJumping && isGrounded)
        {
            isJumping = true;
            isGrounded = false;
            velocity.y = -(initalJumpVelocity);
            jumpSound = PlaySound(1.0f, cScale[10], 2.0 / 44100, -0.01, false);
        }
    }

    void OnPlayMode()
    {
        isEditingLvl = false;
        isPlayerEditing = false;
        SelfReset();
    }
    
    void OnEditMode()
    {
        isEditingLvl = true;
        SelfReset();
    }

    void Update()
    {
        if (KeyWasPressed(KB_KEY_ENTER))
        {
            OnPlayMode();
        }

        if (KeyWasPressed(KB_KEY_R))
        {
            OnEditMode();
        }

        if (!isEditingLvl)
        {   
            position += velocity * deltaTime;

            HandleMove();

            GetCollidingTiles();
            CheckCollisions();

            if (!IsBottomCollision(TILE_WALL) && !IsBottomCollision(TILE_PLAYER_WALL))
            {
                velocity.y += isJumping ? jumpGravity * deltaTime : GRAVITY;
            }
            else 
            {
                isJumping = false;
                isGrounded = true;
                velocity.y = 0.f;
                position.y = ((int)position.y / TILE_PX * TILE_PX) + 4;
            }
            // else if ((IsBottomCollision(TILE_WALL) || IsBottomCollision(TILE_PLAYER_WALL)) && !isGrounded && isJumping)
            // {
                
            // }
            // else if (!isJumping)
            // {
            //     isGrounded = true;
            //     velocity.y = 0.f;
            //     position.y = ((int)position.y / TILE_PX * TILE_PX) + 4;
            // }

            HandleJump();
        }

        DrawBitmap((unsigned char*)sprite.pixels, position.x, position.y, sprite.pixel_size_x, sprite.pixel_size_y, true);
    }
};

