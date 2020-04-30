#ifndef __PROJECT_IMAGES_H__
#define __PROJECT_IMAGES_H__

#include <stdint.h>

typedef struct font_char
{
	uint8_t width;
	uint8_t height;
	int offset;
} FONT_CHAR_INFO;

extern const uint8_t consolas_14ptBitmaps[];
extern const FONT_CHAR_INFO consolas_14ptDescriptors[];

extern const uint8_t fighterjeticon26Bitmaps[];
extern const uint8_t fighterjeticon26WidthPixels;
extern const uint8_t fighterjeticon26HeightPixels;

extern const uint8_t fighterjetflipBitmaps[];
extern const uint8_t fighterjetflipWidthPixels;
extern const uint8_t fighterjetflipHeightPixels;

extern const uint8_t bossBitmaps[];
extern const uint8_t bossWidthPixels;
extern const uint8_t bossHeightPixels;

extern const uint8_t startBitmaps[];
extern const uint8_t startWidthPixels;
extern const uint8_t startHeightPixels;

extern const uint8_t bombBitmaps[];
extern const uint8_t bombWidthPixels;
extern const uint8_t bombHeightPixels;

extern const uint8_t clearBitmaps[];
extern const uint8_t clearWidthPixels;
extern const uint8_t clearHeightPixels;

#endif