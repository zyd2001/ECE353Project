#ifndef __PROJECT_UTILITY_H__
#define __PROJECT_UTILITY_H__

#include "main.h"

int random(int min, int max);
int compare(const void* a, const void* b);
Direction accel_direction(int16_t o_x, int16_t o_y);
bool in_range(Position p1, Position p2, uint8_t width, uint8_t height);

#endif