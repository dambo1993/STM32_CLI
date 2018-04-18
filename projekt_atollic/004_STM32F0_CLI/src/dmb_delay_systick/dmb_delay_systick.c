/*
 * delay_systick.c
 *
 *  Created on: 25 wrz 2016
 *      Author: Przemek
 */

#include "dmb_delay_systick.h"
#include "stm32f0xx.h"
#include <stdint.h>

volatile uint32_t delay_counter;

volatile dmb_delay_systick_callback_t callback;

/*
 * Inicjalizuje Systick na przerwanie co 1ms.
 *
 * @param clock_speed predkosc taktowania timera SysTick
 */
void dmb_delay_systick_init( uint32_t clock_speed )
{
	SysTick_Config( clock_speed / 1000 );
}

/*
 *
 */
void dmb_delay_systick_register_callback(dmb_delay_systick_callback_t user_callback)
{
	callback = user_callback;
}

/*
 * Podstawowa funkcja delaya oczekujacego zadana ilosc milisekund
 * UWAGA - BLOKUJACA!
 */
void _delay_ms( unsigned int delay )
{
	delay_counter = delay;
	while( delay_counter )
	{
		__WFI(); // delikatne uspienie urzadzenia
	}
}

/*
 * Obsluga przerwania od timera SysTick
 */
void SysTick_Handler(void)
{
	// dekrementacja licznika odpowiadajacego za prosty delay
	if( delay_counter )
		delay_counter--;

	// jesli mamy podpiety callback - wywolujemy go
	if(callback)
	{
		callback();
	}
}
