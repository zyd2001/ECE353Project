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
#include "project_interrupts.h"

volatile bool alert_button;
volatile bool alert;
volatile bool enemy_alert;
volatile bool bullet_alert;
volatile Direction ps2_direction;

//*****************************************************************************
// TIMER4 ISR
//*****************************************************************************
void TIMER4A_Handler(void)
{
	TIMER4->ICR |= TIMER_ICR_TATOCINT;
}

//*****************************************************************************
// TIMER5 ISR
//*****************************************************************************
void TIMER5A_Handler(void)
{
	static int count = 0;
	static int bullet_count = 0;
	static int enemy_count = 0;
	count++;
	bullet_count++;
	enemy_count++;
	if (count == 50)
	{
		if (enemy_count >= enemy_spawn_time)
		{
			enemy_alert = true;
			enemy_count = 0;
		}
		if (bullet_count >= 125)
		{
			bullet_alert = true;
			bullet_count = 0;
		}
		alert = true;
		count = 0;
	}
	ADC0->PSSI |= ADC_PSSI_SS1;
	TIMER5->ICR |= TIMER_ICR_TATOCINT;
}

//*****************************************************************************
// ADC0 SS1 ISR
//*****************************************************************************
void ADC0SS1_Handler(void)
{
	uint32_t x = ADC0->SSFIFO1;
	uint32_t y = ADC0->SSFIFO1;
	if (x > 3475)
		ps2_direction = D_LEFT;
	else if (x < 745)
		ps2_direction = D_RIGHT;
	else if (y > 3475)
		ps2_direction = D_UP;
	else if (y < 745)
		ps2_direction = D_DOWN;
	else 
		ps2_direction = D_CENTER;
	ADC0->ISC |= ADC_ISC_IN1;
}

void GPIOF_Handler(void)
{
	alert_button = true;
	GPIOF->ICR |= 0xF;
}