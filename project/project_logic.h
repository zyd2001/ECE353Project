#ifndef __PROJECT_LOCIG_H__
#define __PROJECT_LOCIG_H__

#define SCREEN_X            240
#define SCREEN_Y            320
#define ENEMY_LIMIT	30
#define BULLET_LIMIT 100

typedef enum
{
	NORMAL,
	BIG,
	SCATTER
} BulletType;

typedef enum {
	D_LEFT,
	D_RIGHT,
	D_CENTER,
	D_UP_1,
	D_UP_2,
	D_UP,
	D_UP_3,
	D_UP_4,
	D_DOWN_1, // for bullets direction
	D_DOWN_2,
	D_DOWN,
	D_DOWN_3,
	D_DOWN_4,
	D_NULL
} Direction;

typedef struct position
{
	uint16_t x;
	uint16_t y;
} Position;

extern volatile int enemy_spawn_time;

void start(void);

#endif