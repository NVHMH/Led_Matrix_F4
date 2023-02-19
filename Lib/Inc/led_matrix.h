#ifndef LED_MATRIX_H
#define LED_MATRIX_H

/* =====================================================================
 * 						Multiple led matrix circuit
 * =====================================================================
 * 							1st column					1st column
 * 								|							|
 * 				 _________________			 _________________
 * 	1st row --> | * * * * * * * * |			| * * * * * * * * |
 * 				| * * * * * * * * |			| * * * * * * * * |
 * 				| * * * * * * * * |			| * * * * * * * * |	--- VCC
 * 				| * * * * * * * * |			| * * * * * * * * |	--- GND
 * 				| * * * * * * * * |			| * * * * * * * * |	--- DIN
 * 				| * * * * * * * * |	 . . .	| * * * * * * * * |	--- CS
 * 				| * * * * * * * * |			| * * * * * * * * |	--- CLK
 * 				| * * * * * * * * |			| * * * * * * * * |
 * 				 -----------------			 -----------------
 *
 *				  Last led matrix			   1st led matrix
 * ======================================================================
 */
#include "main.h"
#include <stdint.h>

// text moving direction
typedef enum
{
	LED_MATRIX_DISPLAY_FORWARD,
	LED_MATRIX_DISPLAY_BACKWARD,
} LED_MATRIX_DISPLAY_DIRECTION_e;

// Parameters for displaying effect
typedef struct
{
	uint32_t u32_speed_run_text;
	LED_MATRIX_DISPLAY_DIRECTION_e display_direction;
} LED_MATRIX_OPERATION_PARAM_t;

// Led matrix's data for operation
typedef struct
{
	SPI_HandleTypeDef *hspi;
	GPIO_TypeDef *load_pin_port;
	uint16_t u16_load_pin;
	uint8_t u8_num_led_matrix;
	LED_MATRIX_OPERATION_PARAM_t operation_param;
	uint8_t gau8_temp_uart_buf[255];
	uint8_t gu8_temp_cnt_elt_uart_buf;
	uint8_t gau8_output_buf[4][8];
	uint8_t gu8_cnt_display;	//manage to update output_buf array to what character
								//in uart_buf array and what column
} LED_MATRIX_DATA_t;

void LED_MATRIX_Init(
		LED_MATRIX_DATA_t *led_matrix_data,
		SPI_HandleTypeDef *hspi,
		GPIO_TypeDef *load_pin_port,
		uint16_t u16_load_pin,
		uint8_t  u8_num_led_matrix
);
void PUSH_OUTPUT_Buf(LED_MATRIX_DATA_t *led_matrix_data);
void UPDATE_OUTPUT_Buf(LED_MATRIX_DATA_t *led_matrix_data, uint8_t *gu8_flag_cplt_update);
void LED_MATRIX_SPI_Handle(LED_MATRIX_DATA_t *led_matrix_data, SPI_HandleTypeDef *hspi);
#endif
