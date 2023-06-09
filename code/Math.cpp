#pragma once

#define GRAVITY 2.f
#define MAX_G_VEL 10.f

struct Vector2F
{
    float x, y;
};

// Variables 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------



// Functions 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Operators 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

Vector2F operator+(Vector2F a, Vector2F b)
{
    return Vector2F { a.x + b.x, a.y + b.y  };
}

Vector2F& operator+=(Vector2F& a, const Vector2F& b) 
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

Vector2F operator*(Vector2F v, float scale)
{
    return Vector2F { v.x * scale, v.y * scale };
};