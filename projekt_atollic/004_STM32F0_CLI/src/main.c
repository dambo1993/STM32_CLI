#include "dmb_delay_systick/dmb_delay_systick.h"
#include "stm32f0xx.h"
#include "DMB/rejestry.h"
#include "DMB/debug.h"
#include "RCC/RCC.h"
#include "stdlib.h"
#include <string.h>

#include "UART/uart.h"
#include "AT_parser/AT_parser.h"		// biblioteka parsera komend AT
#include "CLI_modules/CLI_modules.h" 	// tutaj znajduja sie funkcje obslugi GPIO i ADC
#include "common.h"						// tutaj znajduja sie "wspolne" odpowiedzi z ukladu typu OK/ERROR


//! glowna tablica z komendami AT - uzupelniamy ja naszymi komendami AT
//! a nastepnie przekazujemy do biblioteki parsera
const t_cmd AT_cmd_array[] = {
		{"GPIO_INIT",           CLI_GPIO_init},
		{"GPIO_OUT",          	CLI_GPIO_out},
		{"GPIO_IN",         	CLI_GPIO_in},
		{"ADC_INIT",			CLI_ADC_init},
		{"ADC_START",			CLI_ADC_start},
		{"ADC_START_V",			CLI_ADC_start_v},
		{"ADC_SET_A",			CLI_ADC_set_A},
		{"ADC_SET_B",			CLI_ADC_set_B},
		{"ADC_GET_AB",			CLI_ADC_get_AB},
};

//! glowna tablica z komendami "noAT"
const t_cmd_no_at no_AT_cmd_array[] = {
		{"AT",          przywitanie_no_AT},
};

int main(void)
{
	// inicjalizacja zegara na 48MHz z zewnetrznego kwarcu 8MHz
	RCC_init();

	// wlaczenie zegara dla portu A i B
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	// rejestracja tablic ze strukturami komend
    AT_register_AT_commands_table(AT_cmd_array, (sizeof(AT_cmd_array)/sizeof(AT_cmd_array[0])));
    AT_register_no_AT_commands_table(no_AT_cmd_array, (sizeof(no_AT_cmd_array)/sizeof(no_AT_cmd_array[0])));
    AT_register_error_function(error);

    // inicjalizacja uartu:
	init_uart( 115200 );

	// podpiecie callbacka do eventu od odebrania linii przez UART
	uart_register_rx_callback( AT_commands_decode );

	// inicjalizacja SysTicka z aktualna predkoscia zegara
	dmb_delay_systick_init(48000000);

	uart_wyslij_lancuch_rn("Witam w CLI dla STM32F0!");
	while(1)
	{
		uart_event_check();
	}
}
