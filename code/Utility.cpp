#pragma once 

#include <random>

#define WINDOW_FAC 4

#define FRAME_BUFFER_WIDTH 200
#define FRAME_BUFFER_HEIGHT 120

#define WINDOW_WIDTH (FRAME_BUFFER_WIDTH * WINDOW_FAC)
#define WINDOW_HEIGHT (FRAME_BUFFER_HEIGHT * WINDOW_FAC)

#define WINDOW_MEMORY (WINDOW_WIDTH * WINDOW_HEIGHT * 4)

#define COLLISION_OFFSET 2 // Amount to subtract from collision detection pixels

// Functions ----------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Generate a rnd nbr.
/// @param min The starting point for the range of random numbers
/// @param maxMinusMin The number of values between first and the last possible random number including the limits.
/// @return 
int GetRandomNbr(int min, int maxMinusMin)
{
    int nbr = min + (rand() % maxMinusMin);
    //printf("%i \n", nbr);
    return nbr;
}
