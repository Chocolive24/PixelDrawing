#pragma once

#include "MiniFB.h"

// Variables 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

mfb_timer* timer = mfb_timer_create();
double deltaTime, totalTime;

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Functions 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

void TimerTick()
{
    deltaTime = mfb_timer_delta(timer);
    totalTime += deltaTime;
}