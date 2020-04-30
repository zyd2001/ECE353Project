#include "main.h"

int random(int min, int max)
{
	return rand() % (max + 1 - min) + min;
}

int compare(const void* a, const void* b)
{
	int arg1 = *(const int*)a;
	int arg2 = *(const int*)b;
	return (arg1 > arg2) - (arg1 < arg2); 
}

Direction accel_direction(int16_t o_x, int16_t o_y)
{
	int16_t x = accel_read_x();
	int16_t y = accel_read_y();
	
	if (x < o_x - 3000)
	{
		return D_RIGHT;
	}
	else if (x > o_x + 3000)
	{
		return D_LEFT;
	}
	else if (y < o_y - 3000)
	{
		return D_UP;
	}
	else if (y > o_y + 3000)
	{
		return D_DOWN;
	}
}

bool in_range(Position p1, Position p2, uint8_t width, uint8_t height)
{
	return p1.x < p2.x + width / 2 && p1.x > p2.x - width / 2 && 
		p1.y < p2.y + height / 2 && p1.y > p2.y - height / 2;
}