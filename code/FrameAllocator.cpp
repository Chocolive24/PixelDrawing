#pragma once 

#include "stdio.h"

uint8_t* frameAllocatorBuffer; // char* is the same size as uint8_t -> 1 byte == 8 bits
size_t   frameAllocatorUsed;
size_t   frameAllocatorCapacity;


void FrameAllocatorInit(size_t framAllocCapacity)
{
    frameAllocatorBuffer = (uint8_t*)malloc(framAllocCapacity);
    frameAllocatorCapacity = framAllocCapacity;
}

void* FrameAlloc(size_t size)
{
    size_t remainingBytes = frameAllocatorCapacity - frameAllocatorUsed;

    if(remainingBytes < size)
    {
        LOG_ERROR("Not enough memory in the frame allocator");
        exit(-1);
    }

    void* cursorInFrameAllocBuffer = frameAllocatorBuffer + frameAllocatorUsed;
    frameAllocatorUsed += size;

    return (void*)cursorInFrameAllocBuffer;
}

/// @brief Clear the allocator each frame
void FrameAllocatorTick()
{
    frameAllocatorUsed = 0;
}