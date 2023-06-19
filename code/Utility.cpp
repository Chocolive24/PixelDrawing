#pragma once 

#include <iostream>
#include <random>

#define ANSI_RED "\e[31m"
#define ANSI_WHITE "\e[37m"
#define ANSI_RESET "\e[0m"

#define STRINGIFY(a) STRINGIFY_INTERNAL(a)
#define STRINGIFY_INTERNAL(a) #a

#define LOG(message) printf(message " (" __FILE__ ":" STRINGIFY(__LINE__) ")\n" ANSI_RESET)
#define LOG_ERROR(message) printf(ANSI_RED "ERROR " message " (" __FILE__ ":" STRINGIFY(__LINE__) ")\n" ANSI_RESET)

#define WINDOW_FAC 4

#define FRAME_BUFFER_WIDTH 200
#define FRAME_BUFFER_HEIGHT 120

#define WINDOW_WIDTH (FRAME_BUFFER_WIDTH * WINDOW_FAC)
#define WINDOW_HEIGHT (FRAME_BUFFER_HEIGHT * WINDOW_FAC)

#define WINDOW_MEMORY (WINDOW_WIDTH * WINDOW_HEIGHT * 4)

#define COLLISION_OFFSET 2 // Amount to subtract from collision detection pixels

#define ARR_LEN(arr) ((int) (sizeof(arr) / sizeof(*arr)))
#define MAX(a, b) ((a > b) ? a : b)
#define MIN(a, b) ((a < b) ? a : b)


//Structs
// ----------------------------------------------------------------------------------------------------------------------------------------------------

struct Span // Envergure / espace -> une tranche de mémoire.
{
    uint8_t* ptr;
    size_t size;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------

// Variables 
// ----------------------------------------------------------------------------------------------------------------------------------------------------

std::random_device os_seed;
uint_least32_t seed = os_seed();
std::mt19937 generator(seed);

float monitorScaleX, monitorScaleY;

// ----------------------------------------------------------------------------------------------------------------------------------------------------

// Functions 
// ----------------------------------------------------------------------------------------------------------------------------------------------------

Span loadEntireFile(const char* filePath)
{
    Span span{};

    FILE* file = fopen(filePath, "rb");
    fseek(file, 0, SEEK_END); // pose le curseur qui lit le fichier à la fin. 
    span.size = ftell(file); // retourne l'endroit ou est le curseur (ici à la fin car on cherche la taille du fichier).
    fseek(file, 0, SEEK_SET); // SEEK_SET set le curseur à l'endroit souhaité (ici 0).

    span.ptr = (uint8_t*)malloc(span.size);
    fread(span.ptr, span.size, 1, file);
    fclose(file);

    return span;
}

int GetRandomInt(int min, int max)
{
    std::uniform_int_distribution<int> intDistribution(min, max);
    int randomInt = intDistribution(generator);

    return randomInt;
}

float GetRandomFloat(float min, float max)
{
    std::uniform_real_distribution<float> floatDistribution(min, max);
    float randomFloat = floatDistribution(generator);

    return randomFloat;
}


char* Format()
{

}


void windows_enable_colors_in_command_prompt()
{
    #ifndef __EMSCRIPTEN__
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode;
    GetConsoleMode(hConsole, &consoleMode);
    consoleMode |= 0x0004; // ENABLE_ECHO_INPUT

    if (!SetConsoleMode(hConsole, consoleMode))
    {
        //auto e = GetLastError();
        printf("error\n");
        // could not enable colors (TODO: remember that and avoid printing control sequences)
    }
    else
    {
        // colors are enabled!!
        //printf("\x1b[32mgreen\n");
    }
    #endif 
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------