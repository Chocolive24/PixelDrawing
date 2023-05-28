#include <random>
#include <stdio.h>

// Functions ----------------------------------------------------------------------------------------------------------------------------------------------------

#pragma region Utility Functions ---------------------------------------------------------------------------------------------------

int GetARandomNbr(int min, int max)
{
    // Seed the random number generator
    std::random_device rd;
    std::mt19937 generator(rd());

    // Create a distribution for the range
    std::uniform_int_distribution<int> distribution(min, max);

    int nbr = distribution(generator);

    printf("%i \n", nbr);

    return nbr;
};

#pragma endregion Utility Functions

int main()
{
    printf("%i ", GetARandomNbr(1, 5000));
}