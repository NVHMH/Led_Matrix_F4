#include "command_line.h"
#include "led_matrix.h"
#include <string.h>

#include <stdint.h>

extern LED_MATRIX_DATA_t led_matrix_data;

uint8_t au8_uart_buf[255]={};
uint8_t u8_cnt_elt_uart_buf = 4;



void COMMAND_LINE_Receive(uint8_t u8_uart_rx)
{

	if (u8_uart_rx =='\n'){
		led_matrix_data.gu8_temp_cnt_elt_uart_buf = u8_cnt_elt_uart_buf;
		for (int i=0; i<led_matrix_data.gu8_temp_cnt_elt_uart_buf; i++){
			led_matrix_data.gau8_temp_uart_buf[i] = au8_uart_buf[i];
		}
		memset(led_matrix_data.gau8_temp_uart_buf,0x00,led_matrix_data.u8_num_led_matrix);	//create whitespace
		u8_cnt_elt_uart_buf = led_matrix_data.u8_num_led_matrix;
		led_matrix_data.gu8_cnt_display = 0;

	}
	else{
		au8_uart_buf[u8_cnt_elt_uart_buf] = u8_uart_rx;
		u8_cnt_elt_uart_buf++;

	}
}



