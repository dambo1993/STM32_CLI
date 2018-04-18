/*
 * CLI_modules.c
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
#include "../common.h"

// makro zwracajace rozmiar tablicy
#define ARRAY_LENGTH(x)		(sizeof(x)/sizeof(x[0]))

// makro zwracajace wskaznik na zadana tablice oraz jej rozmiar po przecinku do przekazania jako parametry funkcji
#define ARRAY_AND_LENGTH(x) x,(sizeof(x)/sizeof(x[0]))

char *(params_GPIO_ports[]) =
{
		"GPIOA",
		"GPIOB",
		"GPIOC",
		"GPIOD",
		"GPIOF"
};

//! tablica z wskaznikami na porty GPIO
GPIO_TypeDef *tablica_GPIO[] = {GPIOA,GPIOB,GPIOC,GPIOD,GPIOF};

char *(params_GPIO_mode[]) =
{
		"OUTPUT",
		"INPUT",
		"ALTERNATE",
		"ANALOG"
};

enum{
	gpio_mode_output,
	gpio_mode_input,
	gpio_mode_alternate,
	gpio_mode_analog,
};

// wyszukuje zadana wartosc w podanej tabeli parametrow, jesli jej nie ma - zwraca -1
int8_t wyszukaj_w_tabeli_parametrow(const char* txt, char** params_table, uint8_t params_number)
{
	int8_t pozycja = -1;
	for(int8_t i = 0; i < params_number; i++)
	{
		if(strcmp(txt,params_table[i]) == 0)
		{
			pozycja = i;
			break;
		}
	}
	return pozycja;
}

//! funkcja ustawiajaca stan zadanago pinu
void GPIO_set_output(uint8_t gpio_number, uint8_t gpio_pin_number, uint8_t state)
{
	if(state)
	{
		tablica_GPIO[gpio_number]->BSRR = 1 << gpio_pin_number;
	}
	else
	{
		tablica_GPIO[gpio_number]->BSRR = (1 << gpio_pin_number) << 16;
	}
}

//! funkcja odczytujaca stan zadanego pinu
uint8_t GPIO_read_input(uint8_t gpio_number, uint8_t gpio_pin_number)
{
	return tablica_GPIO[gpio_number]->IDR & ( 1 << gpio_pin_number) ? 1 : 0;
}

//! podstawowa konfiguracja ADC
void init_ADC_single( void )
{
	// standardowo - wlaczenie zegarow
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;

	// musimy wybrac z jakiego zegara bedzie taktowany nasz przetwornik
	// do dyspozycji mamy osobny zegar 14MHz, ktory sluzy tylko do taktowania ADC
	// oraz PCLK dzielone przez 2 lub 4
	// 14MHz jest domyslny, wiec go zostawmy

	ADC1->CR |= ADC_CR_ADEN; // start przetwornika
	//czekanie az sie ogarnie
	while( !(ADC1->ISR & ADC_ISR_ADRDY) );

	// ustawianie czasu konwersji:
	// Tconv = sampling time + 12,5 * ADC clock cycles
	// przyklad
	// With ADC_CLK = 14 MHz and a sampling time of 1.5 ADC clock cycles:
	// tCONV = 1.5 + 12.5 = 14 ADC clock cycles = 1 µs

	// ustawmy duzo probek - 72
	ADC1->SMPR = ADC_SMPR_SMP_2 | ADC_SMPR_SMP_1; // 72 probki
}

// funkcja wykonujaca pomiar na zadanym kanale
uint16_t CLI_ADC_pomiar( uint8_t kanal )
{
	// ustawiamy na ktorym kanale ma byc dokonany pomiar
	ADC1->CHSELR = ( 1 << kanal );

	// uruchamiamy pomiar
	ADC1->CR |= ADC_CR_ADSTART;

	// oczekujemy na jego zakonczenie
	while( ! ( ADC1->ISR & ADC_ISR_EOC) );

	// zwracamy wartosc pomiaru
	return ADC1->DR;
}

//! funkcja konfugurujaca GPIO
//! format komendy -> AT+GPIO_INIT=A,5,MODE,parameter
void CLI_GPIO_init(char **params_array, uint8_t params_cnt)
{
	// flaga bledu jest domyslnie zapalona - zgasimy ja, gdy wszystko bedzie ok
	uint8_t error_flag = 1;

	if(params_cnt == 4)
	{
		int8_t gpio_index; // index parametru GPIO

		gpio_index = wyszukaj_w_tabeli_parametrow(params_array[0],ARRAY_AND_LENGTH(params_GPIO_ports));

		if(gpio_index != -1)
		{
			// dekodujemy numer pinu (musi byc z przedzialu 0 - 15)
			uint8_t pin_index = atoi(params_array[1]);

			if(pin_index >= 0 && pin_index <= 15)
			{
				int8_t mode_index = wyszukaj_w_tabeli_parametrow(params_array[2],ARRAY_AND_LENGTH(params_GPIO_mode));

				if( mode_index != -1 )
				{
					uint8_t parametr = atoi(params_array[3]);

					// najpierw sprawdzmy, czy ostatni parametr jest poprawny
					switch(mode_index)
					{
						case gpio_mode_output:
						{
							if(parametr == 0 || parametr == 1)
							{
								error_flag = 0;
							}
							break;
						}
						case gpio_mode_input:
						{
							if(parametr < 4)
							{
								error_flag = 0;
							}
							break;
						}
						case gpio_mode_alternate:
						{
							if(parametr < 16)
							{
								error_flag = 0;
							}
							break;
						}
						case gpio_mode_analog:
						{
							error_flag = 0;
							break;
						}
					}
					if(error_flag == 0)
					{
						// zawsze czyscimy dzieki temu mamy pewnosc, ze nic nie jest dodatkowo ustawione
						tablica_GPIO[gpio_index]->MODER &= ~( 3 << ( 2 * pin_index ) );
						switch(mode_index)
						{
							case gpio_mode_output:
							{
								tablica_GPIO[gpio_index]->MODER |= ( 1 << ( 2 * pin_index ) );

								GPIO_set_output(gpio_index,pin_index,parametr);
								break;
							}
							case gpio_mode_input:
							{
								GPIO_set_output(gpio_index,pin_index,parametr);

								break;
							}
							case gpio_mode_alternate:
							{
								tablica_GPIO[gpio_index]->MODER |= ( 2 << ( 2 * pin_index ) );

								break;
							}
							case gpio_mode_analog:
							{
								tablica_GPIO[gpio_index]->MODER |= ( 3 << ( 2 * pin_index ) );
								break;
							}
						}
					}
				}
			}
		}
	}

	if(error_flag)
	{
		odeslij_error();
	}
	else
	{
		odeslij_ok();
	}
}

//! funkcja ustawiajaca wyjscie na zadany stan (oczywiscie wczesniej zakladamy jego ustawienie)
void CLI_GPIO_out(char **params_array, uint8_t params_cnt)
{
	// flaga bledu jest domyslnie zapalona - zgasimy ja, gdy wszystko bedzie ok
	uint8_t error_flag = 1;

	if(params_cnt == 3)
	{
		int8_t gpio_index; // index parametru GPIO

		gpio_index = wyszukaj_w_tabeli_parametrow(params_array[0],ARRAY_AND_LENGTH(params_GPIO_ports));

		if(gpio_index != -1)
		{
			// dekodujemy numer pinu (musi byc z przedzialu 0 - 15)
			uint8_t pin_index = atoi(params_array[1]);

			if(pin_index >= 0 && pin_index <= 15)
			{
				if(params_array[2][0] == '1')
				{
					GPIO_set_output(gpio_index,pin_index,1);
					error_flag = 0;
				}
				else
				{
					GPIO_set_output(gpio_index,pin_index,0);
					error_flag = 0;
				}
			}
		}
	}

	if(error_flag)
	{
		odeslij_error();
	}
	else
	{
		odeslij_ok();
	}
}

//! Pobranie stanu podanego pinu
void CLI_GPIO_in(char **params_array, uint8_t params_cnt)
{
	// flaga bledu jest domyslnie zapalona - zgasimy ja, gdy wszystko bedzie ok
	uint8_t error_flag = 1;

	if(params_cnt == 2)
	{
		int8_t gpio_index; // index parametru GPIO

		gpio_index = wyszukaj_w_tabeli_parametrow(params_array[0],ARRAY_AND_LENGTH(params_GPIO_ports));

		if(gpio_index != -1)
		{
			// dekodujemy numer pinu (musi byc z przedzialu 0 - 15)
			uint8_t pin_index = atoi(params_array[1]);

			if(pin_index >= 0 && pin_index <= 15)
			{
				uart_wyslij_lancuch("Stan pinu: ");
				if(GPIO_read_input(gpio_index,pin_index))
				{
					uart_wyslij_lancuch_rn("HIGH");
				}
				else
				{
					uart_wyslij_lancuch_rn("LOW");
				}
				error_flag = 0;
			}
		}
	}

	if(error_flag)
	{
		odeslij_error();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Obsluga ADCka
////////////////////////////////////////////////////////////////////////////////////////////////////////

//! wspolczynniki do przeliczania wartosci napiecia
//! przeliczane ze wzoru:
//! napiecie_volty = (ADC * A)/B
uint32_t A = 3300;
uint32_t B = 4096;

//! inicjalizacja ADC
void CLI_ADC_init(char **params_array, uint8_t params_cnt)
{
	if(params_cnt == 0)
	{
		init_ADC_single();
		odeslij_ok();
	}
	else
	{
		odeslij_error();
	}
}

//! Wykonanie pomiaru na zadanym kanale
void CLI_ADC_start(char **params_array, uint8_t params_cnt)
{
	uint8_t kanal = atoi(params_array[0]);

	if(kanal < 8 || kanal == 9)
	{
		uint16_t wynik = CLI_ADC_pomiar(kanal);

		uart_wyslij_lancuch("Wynik pomiaru ADC to: ");
		uart_wyslij_liczbe(wynik);
		uart_wyslij_lancuch_rn("");
	}
	else
	{
		odeslij_error();
	}
}

//! Wykonanie pomiaru na zadanym kanale + przeliczenie na wartosc napiecia na podstawie wspolczynnikow
//! ze zmiennych A i B
void CLI_ADC_start_v(char **params_array, uint8_t params_cnt)
{
	uint8_t kanal = atoi(params_array[0]);

	if(kanal < 8 || kanal == 9)
	{
		uint32_t wynik = CLI_ADC_pomiar(kanal);

		uart_wyslij_lancuch("Napiecie na ADC to: ");
		wynik = (wynik * A)/B;
		uart_wyslij_liczbe(wynik/1000);
		uart_wyslij_lancuch(",");
		uart_wyslij_liczbe(wynik%1000);
		uart_wyslij_lancuch("V");
		uart_wyslij_lancuch_rn("");
	}
	else
	{
		odeslij_error();
	}
}

//! Ustawienie parametru A
void CLI_ADC_set_A(char **params_array, uint8_t params_cnt)
{
	if(params_cnt == 1)
	{
		A = atoi(params_array[0]);
	}
	else
	{
		odeslij_error();
	}
}

//! Ustawienie parametru B
void CLI_ADC_set_B(char **params_array, uint8_t params_cnt)
{
	if(params_cnt == 1)
	{
		B = atoi(params_array[0]);
		odeslij_ok();
	}
	else
	{
		odeslij_error();
	}
}

//! Wyswietlenie parametrow A i B:
void CLI_ADC_get_AB(char **params_array, uint8_t params_cnt)
{
	if(params_cnt == 0)
	{
		uart_wyslij_lancuch("A=");
		uart_wyslij_liczbe(A);
		uart_wyslij_lancuch(" B=");
		uart_wyslij_liczbe(B);
		uart_wyslij_lancuch_rn("");
	}
	else
	{
		odeslij_error();
	}
}
