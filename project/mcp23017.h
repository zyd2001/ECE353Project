#ifndef __MCP23017_H__
#define __MCP23017_H__

#include <stdbool.h>
#include <stdint.h>

#define MCP23017_ID 0x27
#define BUTTON_UP 0
#define BUTTON_DOWN 1
#define BUTTON_LEFT 2
#define BUTTON_RIGHT 3
#define BUTTON_NULL 4


int read_button(void);
void set_led(uint8_t);

bool mcp23017_init(void);

#endif