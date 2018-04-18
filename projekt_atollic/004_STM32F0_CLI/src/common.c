/*
 * common.c
 *
 *  Created on: 18.04.2018
 *      Author: Przemek
 */

#include "dmb_delay_systick/dmb_delay_systick.h"
#include "stm32f0xx.h"
#include "DMB/rejestry.h"
#include "DMB/debug.h"
#include "RCC/RCC.h"
#include "stdlib.h"
#include <string.h>

#include "UART/uart.h"

#include "UART/uart.h"
#include "common.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////
// Funkcje do odeslania statusu wykonania przez UART
////////////////////////////////////////////////////////////////////////////////////////////////////////
void odeslij_ok()
{
	uart_wyslij_lancuch_rn("OK!");
}

void odeslij_error()
{
	uart_wyslij_lancuch_rn("ERROR!");
}

//! Przywitanie sie z ukladem - test polaczenia
void przywitanie_no_AT()
{
	uart_wyslij_lancuch_rn("Dzien dobry!");
}

//! odeslanie bledu - blad w komendzie na etapie parsowania
void error(char *wsk)
{
	odeslij_error();
}
