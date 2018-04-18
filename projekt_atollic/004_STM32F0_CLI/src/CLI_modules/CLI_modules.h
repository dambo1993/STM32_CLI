/*
 * CLI_modules.h
 *
 *  Created on: 18.04.2018
 *      Author: Przemek
 */

#ifndef CLI_MODULES_CLI_MODULES_H_
#define CLI_MODULES_CLI_MODULES_H_

void CLI_ADC_get_AB(char **params_array, uint8_t params_cnt);
void CLI_ADC_init(char **params_array, uint8_t params_cnt);
uint16_t CLI_ADC_pomiar(uint8_t kanal );
void CLI_ADC_set_A(char **params_array, uint8_t params_cnt);
void CLI_ADC_set_B(char **params_array, uint8_t params_cnt);
void CLI_ADC_start_v(char **params_array, uint8_t params_cnt);
void CLI_ADC_start(char **params_array, uint8_t params_cnt);
void CLI_GPIO_init(char **params_array, uint8_t params_cnt);
void CLI_GPIO_in(char **params_array, uint8_t params_cnt);
void CLI_GPIO_out(char **params_array, uint8_t params_cnt);
uint8_t GPIO_read_input(uint8_t gpio_number, uint8_t gpio_pin_number);
void GPIO_set_output(uint8_t gpio_number, uint8_t gpio_pin_number, uint8_t state);
void init_ADC_single(void );
int8_t wyszukaj_w_tabeli_parametrow(const char* txt, char** params_table, uint8_t params_number);

#endif /* CLI_MODULES_CLI_MODULES_H_ */
