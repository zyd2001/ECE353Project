#include "main.h"

int read_button(void)
{
		uint8_t data;
		i2cSetSlaveAddr(EEPROM_I2C_BASE, MCP23017_ID, I2C_WRITE);
		i2cSendByte( EEPROM_I2C_BASE, 0x13, I2C_MCS_START | I2C_MCS_RUN);
		i2cSetSlaveAddr(EEPROM_I2C_BASE, MCP23017_ID, I2C_READ);
		i2cGetByte(EEPROM_I2C_BASE, &data, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
		if (~data & 0x1)
			return BUTTON_UP;
		else if (~data & 0x2)
			return BUTTON_DOWN;
		else if (~data & 0x4)
			return BUTTON_LEFT;
		else if (~data & 0x8)
			return BUTTON_RIGHT;
		else
			return BUTTON_NULL;
}

void set_led(uint8_t pins)
{
		i2cSetSlaveAddr(EEPROM_I2C_BASE, MCP23017_ID, I2C_WRITE);
		i2cSendByte( EEPROM_I2C_BASE, 0x12, I2C_MCS_START | I2C_MCS_RUN);
		i2cSendByte(EEPROM_I2C_BASE, pins, I2C_MCS_RUN | I2C_MCS_STOP);
}

/**
	same as eeprom
*/
bool mcp23017_init(void)
{
  
		if(gpio_enable_port(EEPROM_GPIO_BASE) == false)
		{
			return false;
		}
		
		// Configure SCL 
		if(gpio_config_digital_enable(EEPROM_GPIO_BASE, EEPROM_I2C_SCL_PIN)== false)
		{
			return false;
		}
			
		if(gpio_config_alternate_function(EEPROM_GPIO_BASE, EEPROM_I2C_SCL_PIN)== false)
		{
			return false;
		}
			
		if(gpio_config_port_control(EEPROM_GPIO_BASE, EEPROM_I2C_SCL_PCTL_M, EEPROM_I2C_SCL_PIN_PCTL)== false)
		{
			return false;
		}
			

		
		// Configure SDA 
		if(gpio_config_digital_enable(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PIN)== false)
		{
			return false;
		}
			
		if(gpio_config_open_drain(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PIN)== false)
		{
			return false;
		}
			
		if(gpio_config_alternate_function(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PIN)== false)
		{
			return false;
		}
			
		if(gpio_config_port_control(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PCTL_M, EEPROM_I2C_SDA_PIN_PCTL)== false)
		{
			return false;
		}
			
		//  Initialize the I2C peripheral
		if( initializeI2CMaster(EEPROM_I2C_BASE)!= I2C_OK)
		{
			return false;
		}
		
		i2cSetSlaveAddr(EEPROM_I2C_BASE, MCP23017_ID, I2C_WRITE);
		i2cSendByte(EEPROM_I2C_BASE, 0x00, I2C_MCS_START | I2C_MCS_RUN);
		i2cSendByte(EEPROM_I2C_BASE, 0x00, I2C_MCS_RUN | I2C_MCS_STOP); // porta output
		i2cSendByte(EEPROM_I2C_BASE, 0x01, I2C_MCS_START | I2C_MCS_RUN);
		i2cSendByte(EEPROM_I2C_BASE, 0xFF, I2C_MCS_RUN | I2C_MCS_STOP); // portb input
		i2cSendByte(EEPROM_I2C_BASE, 0x0D, I2C_MCS_START | I2C_MCS_RUN);
		i2cSendByte(EEPROM_I2C_BASE, 0x0F, I2C_MCS_RUN | I2C_MCS_STOP); // portb button pull up
		i2cSendByte(EEPROM_I2C_BASE, 0x05, I2C_MCS_START | I2C_MCS_RUN);
		i2cSendByte(EEPROM_I2C_BASE, 0x0F, I2C_MCS_RUN | I2C_MCS_STOP); // portb button interrupt
		i2cSendByte(EEPROM_I2C_BASE, 0x09, I2C_MCS_START | I2C_MCS_RUN);
		i2cSendByte(EEPROM_I2C_BASE, 0x0F, I2C_MCS_RUN | I2C_MCS_STOP); // intcon
		i2cSendByte(EEPROM_I2C_BASE, 0x07, I2C_MCS_START | I2C_MCS_RUN);
		i2cSendByte(EEPROM_I2C_BASE, 0x0F, I2C_MCS_RUN | I2C_MCS_STOP); // defval
				
		return true;
}