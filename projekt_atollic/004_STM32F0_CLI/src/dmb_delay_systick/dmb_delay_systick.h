/*
 * delay_systick.h
 *
 *  Created on: 25 wrz 2016
 *      Author: Przemek
 */

#ifndef DMB_DELAY_SYSTICK_H_
#define DMB_DELAY_SYSTICK_H_

// podczas testow na STM32F030F4P6 na 48MHz dla ustawienia 500ms
// analizator saleae pokazywal 0,5001s
// dla 1500 - idealne 1,5s
// dla 50 -50,01ms

typedef void (*dmb_delay_systick_callback_t)();

void dmb_delay_systick_init( uint32_t );

void dmb_delay_systick_register_callback(dmb_delay_systick_callback_t);

void _delay_ms( unsigned int );

void SysTick_Handler(void);

#endif /* DMB_DELAY_SYSTICK_H_ */
