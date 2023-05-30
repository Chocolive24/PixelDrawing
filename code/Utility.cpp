#pragma once 

#include <random>

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
