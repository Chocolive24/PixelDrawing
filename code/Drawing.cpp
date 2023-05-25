#include <stdio.h>
#include <iostream>
#include <MiniFB.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Structs 
//---------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct bitmap_t 
{
  int pixel_size_x;
  int pixel_size_y;
  uint32_t* pixels;
} 
bitmap_t;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Variables 
//---------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma region Colors

#define RED  0xFFFF0000
#define BLUE 0xFF0000FF
#define GREEN 0xFF00FF00
#define BLACK 0xFF000000
#define WHITE 0xFFFFFFFF

#pragma endregion Colors

#define CHAR_WIDTH 7
#define CHAR_HEIGHT 9

uint32_t* frameBuffer;
uint32_t* windowBuffer;

bitmap_t pixelFont;
bitmap_t penguinSprite;
bitmap_t enemySprite;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Functions
//---------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma region Pixel Handle Functions 

int GetPixelIndex(int x, int y)
{
    return y * FRAME_BUFFER_WIDTH + x;
};

void DrawPixel(int x, int y, uint32_t color)
{
    // Check coords in window bounds.
    if (x >= FRAME_BUFFER_WIDTH || x < 0)
    {
       return;
    }
    if (y >= FRAME_BUFFER_HEIGHT || y < 0)
    {
        return;
    }

    int pixelIndex = GetPixelIndex(x, y);
    uint32_t destColor = frameBuffer[pixelIndex];

    float  srcAlpha  = (uint8_t) (color >> 24) / 256.f;
    uint8_t srcRed   = (uint8_t) (color >> 16);
    uint8_t srcGreen = (uint8_t) (color >> 8);
    uint8_t srcBlue  = (uint8_t) (color);

    uint8_t destRed   = (uint8_t) (destColor >> 16);
    uint8_t destGreen = (uint8_t) (destColor >> 8);
    uint8_t destBlue  = (uint8_t) (destColor);

    uint8_t finalRed   = (uint8_t) ((srcRed * srcAlpha + destRed * (1.f * srcAlpha)));
    uint8_t finalGreen = (uint8_t) ((srcGreen * srcAlpha + destGreen * (1.f * srcAlpha)));
    uint8_t finalBlue  = (uint8_t) ((srcBlue * srcAlpha + destBlue * (1.f * srcAlpha)));

    frameBuffer[pixelIndex] = MFB_RGB(finalRed, finalGreen, finalBlue);
    
    //buffer[GetPixelIndex(x, y)] = color;
};

#pragma endregion Pixels Handle Functions

#pragma region Shape Drawing Functions 

void DrawHorizontalLine(int xStart, int xEnd, int y, uint32_t color)
{
    for (int i = xStart; i < xEnd; i++)
    {
        DrawPixel(i, y, color);
    }
};

void DrawVerticalLine(int x, int yStart, int yEnd, uint32_t color)
{
    for (int i = yStart; i < yEnd; i++)
    {
        DrawPixel(x, i, color);
    }
};

void DrawFullRect(int xStart, int yStart, int width, int height, uint32_t color)
{
    int xEnd = width + xStart;
    int yEnd = height + yStart;

    // y avant x car ca parcour ligne par ligne et les pixels en ligne sont plus proches en mémoire.
    for (int y = yStart; y < yEnd; y++)
    {
        for (int x = xStart; x < xEnd; x++)
        {
            DrawPixel(x, y, color);
        }
    }
};

void DrawEmptyRect(int xStart, int yStart, int width, int height, uint32_t color)
{
    int xEnd = width + xStart;
    int yEnd = height + yStart;

    DrawHorizontalLine(xStart, xEnd, yStart, color);
    DrawVerticalLine(xStart, yStart, yEnd, color);
    DrawVerticalLine(xEnd, yStart, yEnd, color);
    DrawHorizontalLine(xStart, xEnd, yEnd, color);
};

#pragma endregion Shape Drawing Functions

#pragma region Image Functions 

bitmap_t LoadImage(const char* filePath)
{
    bitmap_t img = {};

    int channels; // dummy (not used)

    img.pixels = (uint32_t*) stbi_load(filePath, &img.pixel_size_x, &img.pixel_size_y, &channels, 4);

    if(img.pixels == nullptr) 
    {
        printf("Error in loading the image %s \n", filePath);
        exit(1);
    }

    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", img.pixel_size_x, img.pixel_size_y, channels);

    return img;
}

void DrawBitmap(unsigned char* img, int xStart, int yStart, int imgWidth, int imgHeight)
{
    xStart = (int)(xStart - imgWidth / 2);
    yStart = (int)(yStart - imgHeight / 2);

    int xEnd = imgWidth + xStart;
    int yEnd = imgHeight + yStart;

    for (int y = yStart; y < yEnd; y++)
    {
        for (int x = xStart; x < xEnd; x++)
        {
            int idx = ((y - yStart) * imgWidth + (x - xStart)) * 4; 
            
            // MiniFB à la macro suivant qui fait la même chose : MFB_ARGB
            uint32_t pixelColor = (img[idx + 3] << 24 | img[idx] << 16 | img[idx + 1] << 8 | img[idx + 2]);

            if (img[idx + 3] != 0)
            {
                DrawPixel(x, y, pixelColor);
            }
        }
    }
};

void DrawScaledBitmap(unsigned char* img, int xStart, int yStart, int imgWidth, int imgHeight, float scale)
{
    int scaledWidth = imgWidth * scale;
    int scaledHeight = imgHeight * scale;
    
    int xEnd = xStart + scaledWidth;
    int yEnd = yStart + scaledHeight;
    
    for (int y = yStart; y < yEnd; y++)
    {
        for (int x = xStart; x < xEnd; x++)
        {
            int srcX = (int)((x - xStart) / scale);
            int srcY = (int)((y - yStart) / scale);
            
            if (srcX >= 0 && srcX < imgWidth && srcY >= 0 && srcY < imgHeight)
            {
                int srcIdx = (srcY * imgWidth + srcX) * 4;
                uint32_t pixelColor = (img[srcIdx + 3] << 24 | img[srcIdx] << 16 | img[srcIdx + 1] << 8 | img[srcIdx + 2]);
                
                if (img[srcIdx + 3] != 0)
                {
                    DrawPixel(x, y, pixelColor);
                }
            }
            else
            {
                // Handle cases when the rotated pixel falls outside the source image
                // You can set a default color or leave it as per your requirements.
                DrawPixel(x, y, 0x00000000); // Assuming black color (alpha = 0) for pixels outside the image
            }
        }
    }
}

void DrawChar(unsigned char* fontMap, int xStart, int yStart, int imgWidth, int xIdx, int yIdx)
{
    int xEnd = CHAR_WIDTH + xStart;
    int yEnd = CHAR_HEIGHT + yStart;

    for (int y = yStart + yIdx; y < yEnd + yIdx; y++)
    {
        for (int x = xStart + xIdx; x < xEnd + xIdx; x++)
        {
            int idx = ((y - yStart) * imgWidth + (x - xStart)) * 4; 
            
            // MiniFB à la macro suivant qui fait la même chose : MFB_ARGB
            uint32_t pixelColor = (fontMap[idx + 3] << 24 | fontMap[idx] << 16 | fontMap[idx + 1] << 8 | fontMap[idx + 2]);

            if (fontMap[idx + 3] != 0)
            {
                DrawPixel(x - xIdx, y - yIdx, pixelColor);
            }
        }
    }
}

void resize_bitmap(uint32_t* dest, int dest_sx, int dest_sy, uint32_t* src, int src_sx, int src_sy)
{
    for (int y = 0; y < dest_sy; y++) {
        for (int x = 0; x < dest_sx; x++) {
            int src_x = x * src_sx / dest_sx;
            int src_y = y * src_sy / dest_sy;
            dest[y*dest_sx + x] = src[src_y*src_sx + src_x];
        }
    }
}

#pragma endregion Image Functions 

#pragma region Font Functions

void DrawText(const char* literalString, int xStart, int yStart)
{
    int charIdx = 0;
    char c;

    do
    {
        c = literalString[charIdx];

        if ((int)c >= 65 && (int)c <= 90)
        {
            int x = ((int)c - 65) * CHAR_WIDTH;
            int y = 0;

            DrawChar((unsigned char*)pixelFont.pixels, xStart += 7, yStart, pixelFont.pixel_size_x, x, y);
        }
        else if ((int)c >= 97 && (int)c <= 122)
        {
            int x = ((int)c - 97) * CHAR_WIDTH;
            int y = 12;

            DrawChar((unsigned char*)pixelFont.pixels, xStart += 7, yStart, pixelFont.pixel_size_x, x, y);
        }
        else if ((int)c >= 33 && (int)c <= 63)
        {
            int x = ((int)c - 33) * CHAR_WIDTH;
            int y = 22;

            DrawChar((unsigned char*)pixelFont.pixels, xStart += 7, yStart, pixelFont.pixel_size_x, x, y);
        }
        else if ((int)c >= 1 && (int)c <= 5)
        {
            int x = ((int)c - 1) * CHAR_WIDTH;
            int y = 32;

            DrawChar((unsigned char*)pixelFont.pixels, xStart += 7, yStart, pixelFont.pixel_size_x, x, y);
        }
        else if ((int)c == 32)
        {
            xStart += CHAR_WIDTH;
        }
        else if ((int)c == 13)
        {
            yStart  += CHAR_HEIGHT;
        }

        charIdx++;

    } while (literalString[charIdx] != '\0');
}

#pragma endregion Font Functions

void LoadAllImages()
{
    pixelFont = LoadImage("assets/font_map.png");
    penguinSprite = LoadImage("assets/Penguin.png");
    enemySprite = LoadImage("assets/MaskedOrc.png");
}
