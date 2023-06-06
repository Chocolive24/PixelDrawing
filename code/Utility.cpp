#pragma once 

#include <iostream>
#include <random>

#define WINDOW_FAC 4

#define FRAME_BUFFER_WIDTH 200
#define FRAME_BUFFER_HEIGHT 120

#define WINDOW_WIDTH (FRAME_BUFFER_WIDTH * WINDOW_FAC)
#define WINDOW_HEIGHT (FRAME_BUFFER_HEIGHT * WINDOW_FAC)

#define WINDOW_MEMORY (WINDOW_WIDTH * WINDOW_HEIGHT * 4)

#define COLLISION_OFFSET 2 // Amount to subtract from collision detection pixels

// Variables 
// // ----------------------------------------------------------------------------------------------------------------------------------------------------

// std::random_device os_seed;
// uint_least32_t seed = os_seed();

// std::mt19937 generator(seed);

std::random_device os_seed;
uint_least32_t seed = os_seed();
std::mt19937 generator(seed);

// ----------------------------------------------------------------------------------------------------------------------------------------------------

// Functions 
// ----------------------------------------------------------------------------------------------------------------------------------------------------

int GetRandomInt(int min, int max)
{
    std::uniform_int_distribution<int> intDistribution(min, max);
    int randomInt = intDistribution(generator);
    std::cout << "Random Integer: " << randomInt << std::endl;

    return randomInt;
}

float GetRandomFloat(float min, float max)
{
    std::uniform_real_distribution<float> floatDistribution(min, max);
    float randomFloat = floatDistribution(generator);
    std::cout << "Random Float: " << randomFloat << std::endl;

    return randomFloat;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------