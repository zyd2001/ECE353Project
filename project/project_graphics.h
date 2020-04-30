#ifndef __PROJECT_GRAPHICS_H__
#define __PROJECT_GRAPHICS_H__

#include "main.h"

void draw_rectangle_centered(uint16_t x_center, uint16_t x_len, uint16_t y_center, uint16_t y_len, uint16_t fg_color);
void draw_digit(Position pos, char *, uint16_t fcolor, uint16_t bcolor);
void erase_bullet(Position pos, BulletType t);
void draw_bullet(Position pos, Position old_pos, BulletType t, uint16_t fcolor);
void erase_jet(Position pos);
void draw_jet(Position pos, Position old_pos, uint16_t fcolor);
void erase_boss(Position pos);
void draw_boss(Position pos, Position old_pos, uint16_t fcolor);
void draw_enemy_jet(Position pos, Position old_pos, uint16_t fcolor);

#endif