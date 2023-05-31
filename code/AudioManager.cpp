#pragma once
#define SOKOL_IMPL
#include <sokol_log.h>
#include <sokol_audio.h>

#define _USE_MATH_DEFINES
#include <math.h>

// Structs 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct SFX
{
    float baseAmplitude, baseFrequency;
    float amplitude, frequency;
}
SFX;
 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Variables 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

float C_major_range[12]
{
    261.63f, // C or Bb
    277.18f, // C# or Db
    293.66f, // D
    311.13f, // D# or Eb
    329.63f, // E or Fb
    349.23f, // F or E#
    369.99f, // F# or Gb
    392.00f, // G
    415.30f, // G# or Ab
    440.00f, // A 
    466.16f, // A# or Bb
    493.88f  // B or Cb
};

double timeSound;
double timeCounter;

int C_rangeIdx = 0;

SFX sfx_jump 
{ 
    sfx_jump.baseAmplitude = 1, sfx_jump.baseFrequency = C_major_range[11],
    sfx_jump.amplitude = 1, sfx_jump.frequency = sfx_jump.baseFrequency
};

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Functions 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

void AudioCallback(float* buffer, int numFrames, int numChannels)
{
    for (int frame = 0; frame < numFrames; frame++)
    {
        // sfx_jump.amplitude -= 0.00001f;
        // sfx_jump.frequency -= 0.01f;

        // if (sfx_jump.amplitude < 0)
        // {
        //     sfx_jump.amplitude = 0;
        // }
        // if (sfx_jump.frequency  < 0)
        // {
        //     sfx_jump.frequency  = 0;
        // }

        // 2 * PI = un tour de cercle, donc une oscillation de sin
        buffer[frame] = sinf(timeSound * 2 * M_PI) * 0.2f * sfx_jump.amplitude; // * 0.n -> baisser le son.

        timeCounter += 1.0 / 44100;

        if (timeCounter >= 1)
        {
            timeCounter = 0;
            if (C_rangeIdx < 11)
            {
                C_rangeIdx++;
            }
            else 
            {
                C_rangeIdx = 0;
            }
        }

        timeSound += 1.0 / 44100 * C_major_range[C_rangeIdx]; // time between 2 sample
    }

    sinf(timeSound * M_PI);
}

void SetupSound()
{
    saudio_desc audioDesc = {0};
    audioDesc.stream_cb = AudioCallback;
    audioDesc.logger.func = slog_func;

    saudio_setup(audioDesc);
}

void PlaySFX(SFX& sfx)
{
    sfx.amplitude = sfx.baseAmplitude;
    sfx.frequency = sfx.baseFrequency;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------