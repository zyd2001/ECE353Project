#include "project_graphics.h"


void draw_rectangle_centered
(
  uint16_t x_center, 
  uint16_t x_len, 
  uint16_t y_center, 
  uint16_t y_len,  
  uint16_t fg_color
)
{
  uint16_t x0;
  uint16_t x1;
  uint16_t y0;
  uint16_t y1;
  uint16_t i,j;
 
  x0 = x_center - (x_len/2);
  x1 = x_center + (x_len/2);
  if( (x_len & 0x01) == 0x00)
  {
    x1--;
  }
  
  y0 = y_center  - (y_len/2);
  y1 = y_center  + (y_len/2) ;
  if( (y_len & 0x01) == 0x00)
  {
    y1--;
  }
  
  lcd_set_pos(x0,x1, y0, y1);
  
  for (i=0;i< y_len ;i++)
  {
    for(j= 0; j < x_len; j++)
    {
      lcd_write_data_u16(fg_color);
    }
  }
}

void draw_digit(Position pos, char * str, uint16_t fcolor, uint16_t bcolor)
{
	char *p;
	uint16_t x = 0;
	FONT_CHAR_INFO info;
	p = str;
	while (*p != '\0')
	{
		info = consolas_14ptDescriptors[*p - 48];
		lcd_draw_image(pos.x + x, info.width, pos.y, info.height,
			consolas_14ptBitmaps + info.offset, fcolor, bcolor);
		p++;
		x += info.width + 2;
	}
}

void erase_bullet(Position pos, BulletType t)
{
	uint16_t w;
	switch (t)
	{
		case NORMAL:
			w = 5;
			break;
		case BIG:
			w = 7;
			break;
		case SCATTER:
			w = 3;
			break;
	}
	draw_rectangle_centered(pos.x, w, pos.y, w, LCD_COLOR_BLACK);
}

void draw_bullet(Position pos, Position old_pos, BulletType t, uint16_t fcolor)
{
	uint16_t w;
	switch (t)
	{
		case NORMAL:
			w = 5;
			break;
		case BIG:
			w = 7;
			break;
		case SCATTER:
			w = 3;
			break;
	}
	draw_rectangle_centered(old_pos.x, w, old_pos.y, w, LCD_COLOR_BLACK);
	draw_rectangle_centered(pos.x, w, pos.y, w, fcolor);
}

void erase_jet(Position pos)
{
	draw_rectangle_centered(pos.x, fighterjeticon26WidthPixels, pos.y, fighterjeticon26HeightPixels, LCD_COLOR_BLACK);
}

void draw_jet(Position pos, Position old_pos, uint16_t fcolor)
{
	if (old_pos.x != pos.x || old_pos.y != pos.y)
		erase_jet(old_pos);
	lcd_draw_image(pos.x, fighterjeticon26WidthPixels, pos.y, fighterjeticon26HeightPixels, fighterjeticon26Bitmaps, fcolor, LCD_COLOR_BLACK);
}

void erase_boss(Position pos)
{
	draw_rectangle_centered(pos.x, bossWidthPixels, pos.y, bossHeightPixels, LCD_COLOR_BLACK);
}

void draw_boss(Position pos, Position old_pos, uint16_t fcolor)
{
	if (old_pos.x != pos.x || old_pos.y != pos.y)
		erase_boss(old_pos);
	lcd_draw_image(pos.x, bossWidthPixels, pos.y, bossHeightPixels, bossBitmaps, fcolor, LCD_COLOR_BLACK);
}

void draw_enemy_jet(Position pos, Position old_pos, uint16_t fcolor)
{
	if (old_pos.x != pos.x || old_pos.y != pos.y)
		erase_jet(old_pos);
	lcd_draw_image(pos.x, fighterjeticon26WidthPixels, pos.y, fighterjeticon26HeightPixels, fighterjetflipBitmaps, fcolor, LCD_COLOR_BLACK);
}