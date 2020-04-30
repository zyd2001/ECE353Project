// Copyright (c) 2015-19, Joe Krachey
// All rights reserved.
//
// Redistribution and use in source or binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in source form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "main.h"

static void project_timer4_init(void)
{
	gp_timer_config_32(TIMER4_BASE, TIMER_TAMR_TAMR_PERIOD, false, true);
	NVIC_EnableIRQ(TIMER4A_IRQn);
	NVIC_SetPriority(TIMER4A_IRQn, 0);
	TIMER4->TAILR = 1000000; //set time to 20ms
	TIMER4->CTL |= TIMER_CTL_TAEN;
}

//*****************************************************************************
// TIMER5 Init
// This function is used to configure TIMER5 based on the project requirements
//*****************************************************************************
static void project_timer5_init(void)
{
	gp_timer_config_32(TIMER5_BASE, TIMER_TAMR_TAMR_PERIOD, false, true);
	NVIC_EnableIRQ(TIMER5A_IRQn);
	NVIC_SetPriority(TIMER5A_IRQn, 1);
	TIMER5->TAILR = 50000; //set time to 1ms
	TIMER5->CTL |= TIMER_CTL_TAEN;
}

//*****************************************************************************
// ADC0 Init
// This function is used to configure ADC0 based on the project requirements.  
//
// DO NOT simply call the ADC0 initialization function from ICE-12.  That code
// configures the wrong sample sequencer and also does not use interrupts.
//*****************************************************************************
static void project_adc0_init(void)
{
	SYSCTL->RCGCADC |= SYSCTL_RCGCADC_R0;
	while(!(SYSCTL->PRADC & SYSCTL_PRADC_R0)){}
  ADC0->ACTSS &= ~ADC_ACTSS_ASEN1;
	ADC0->EMUX &= ~ADC_EMUX_EM1_M;
	ADC0->EMUX |= ADC_EMUX_EM1_PROCESSOR;
	ADC0->SSMUX1 = 0x10; //config sample sequnce, AIN0 and AIN1
	ADC0->SSCTL1 = ADC_SSCTL1_END1 | ADC_SSCTL1_IE1; // end at AIN1 and trigger interrupt
	ADC0->IM |= ADC_IM_MASK1;
	ADC0->ISC |= ADC_ISC_IN1;
	NVIC_EnableIRQ(ADC0SS1_IRQn);
	NVIC_SetPriority(ADC0SS1_IRQn, 2);
  ADC0->ACTSS |= ADC_ACTSS_ASEN1;
}

//*****************************************************************************
// GPIO Init
// This function is used to configure all of the GPIO pins used for project.  You 
// should initialize the pins for the LCD, PS2 Joystick, and SW1.
//*****************************************************************************
static void project_gpio_init(void)
{
	lcd_config_gpio();
	gpio_enable_port(PS2_GPIO_BASE);
	gpio_config_analog_enable(PS2_GPIO_BASE, 0xC);
	gpio_config_enable_input(PS2_GPIO_BASE, 0xC);
	gpio_config_alternate_function(PS2_GPIO_BASE, 0xC);
	
	// mcp23017 interrupt
	gpio_enable_port(GPIOF_BASE);
	gpio_config_digital_enable(GPIOF_BASE, 0x1);
	gpio_config_enable_input(GPIOF_BASE, 0x1);
	GPIOF->IM |= 0x1;
	NVIC_SetPriority(GPIOF_IRQn, 3);
	NVIC_EnableIRQ(GPIOF_IRQn);
}

//*****************************************************************************
// This function calls all of the routines used to configure the hardware
// for project.
//*****************************************************************************
void project_initialize_hardware(void)
{
	DisableInterrupts();
	init_serial_debug(true, true);
	project_gpio_init();
	project_adc0_init();
	project_timer4_init();
	project_timer5_init();
	lcd_config_screen();
	lcd_clear_screen(LCD_COLOR_BLACK);
	ft6x06_init();
	//accel_initialize();
	eeprom_init();
	mcp23017_init();
	EnableInterrupts();
}
