#pragma once

#include<stdint.h>
#define HEIGHT 5
#define WIDTH 15


typedef struct map
{
    uint8_t pixels[HEIGHT][WIDTH];
} map;

typedef struct pixel
{
    uint8_t x;
    uint8_t y;
} pixel;

void drawLine(map* map,pixel startPosition,pixel endPosition);

void displayMap(map* map);

void clearDisplay();