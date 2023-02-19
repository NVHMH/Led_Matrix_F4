#include "led_matrix_font.h"
#include "led_matrix.h"
#include <stdlib.h>
#include <stdint.h>


// Max7219's registers address
#define MAX7219_REG_ADD_DECODEMODE		0x09
#define MAX7219_REG_ADD_INTENSITY		0x0A
#define MAX7219_REG_ADD_SCANLIMIT		0x0B
#define MAX7219_REG_ADD_SHUTDOWN		0x0C
#define MAX7219_REG_ADD_DISPLAYTEST		0x0F

// Shutdown register's value
#define MAX7219_SHUTDOWN_MODE			0x00
#define MAX7219_NO_SHUTDOWN_MODE		0x01

// Decode-mode register's value
// what digits will be decoded
#define MAX7219_NO_DECODE				0x00
#define MAX7219_DECODE_DIGIT_0			0x01
#define MAX7219_DECODE_DIGIT_3_0		0x0F
#define MAX7219_DECODE_ALL_DIGIT		0xFF

// Intensity register's value
// Level of intensity
#define MAX7219_INTENSITY_1_32			0x00
#define MAX7219_INTENSITY_3_32			0x01
#define MAX7219_INTENSITY_5_32			0x02
#define MAX7219_INTENSITY_7_32			0x03
#define MAX7219_INTENSITY_9_32			0x04
#define MAX7219_INTENSITY_11_32			0x05
#define MAX7219_INTENSITY_13_32			0x06
#define MAX7219_INTENSITY_15_32			0x07
#define MAX7219_INTENSITY_17_32			0x08
#define MAX7219_INTENSITY_19_32			0x09
#define MAX7219_INTENSITY_21_32			0x0A
#define MAX7219_INTENSITY_23_32			0x0B
#define MAX7219_INTENSITY_25_32			0x0C
#define MAX7219_INTENSITY_27_32			0x0D
#define MAX7219_INTENSITY_29_32			0x0E
#define MAX7219_INTENSITY_31_32			0x0F

// Scan-limit register's value
// Number of displayed digit, start from digit 0
#define MAX7219_DISPLAY_1_DIGIT			0x00
#define MAX7219_DISPLAY_2_DIGIT			0x01
#define MAX7219_DISPLAY_3_DIGIT			0x02
#define MAX7219_DISPLAY_4_DIGIT			0x03
#define MAX7219_DISPLAY_5_DIGIT			0x04
#define MAX7219_DISPLAY_6_DIGIT			0x05
#define MAX7219_DISPLAY_7_DIGIT			0x06
#define MAX7219_DISPLAY_8_DIGIT			0x07

// Display-test register's value
#define MAX7219_DISPLAY_NORMAL			0x00
#define MAX7219_DISPLAY_TEST			0x01

/*
 * @brief	Push data to led matrix module
 * @param 	led_matrix_data		Pointer to LED_MATRX_DATA_t structure
 */
void PUSH_OUTPUT_Buf(LED_MATRIX_DATA_t *led_matrix_data)
{
	for (int i = 0; i<8; i++){
		uint8_t u8_temp_data[2*led_matrix_data->u8_num_led_matrix];
		for(int j = 0; j< led_matrix_data->u8_num_led_matrix;j++){
			u8_temp_data[2*j] = i+1;
			u8_temp_data[2*j+1] = led_matrix_data->gau8_output_buf[led_matrix_data->u8_num_led_matrix-1-j][7-i];
		}
		HAL_SPI_Transmit_DMA(led_matrix_data->hspi, u8_temp_data, 2*led_matrix_data->u8_num_led_matrix);
	}
}

/*
 * @brief	Update output_data_buffer value to create effect
 * @param 	led_matrix_data		Pointer to LED_MATRX_DATA_t structure
 */
void UPDATE_OUTPUT_Buf(LED_MATRIX_DATA_t *led_matrix_data, uint8_t *gu8_flag_cplt_update)
{
	uint8_t u8_current_char;		//contains the character being updated for the output_buf array
	*gu8_flag_cplt_update = 1;
	if(led_matrix_data->operation_param.display_direction == LED_MATRIX_DISPLAY_FORWARD)
	{
	// Shift left data of all led matrix, except 1st led matrix
		for (int i=0; i<led_matrix_data->u8_num_led_matrix-1; i++){
			for (int j=0; j<8; j++){
				led_matrix_data->gau8_output_buf[i][j] >>= 1;
				led_matrix_data->gau8_output_buf[i][j] |= (led_matrix_data->gau8_output_buf[i+1][j] << 7);
			}
		}
	// Get current character displaying in 1st led matrix
		if ((led_matrix_data->gu8_cnt_display/8) < led_matrix_data-> gu8_temp_cnt_elt_uart_buf ){
			 u8_current_char = led_matrix_data->gau8_temp_uart_buf[(led_matrix_data->gu8_cnt_display/8)];
		} else led_matrix_data->gu8_cnt_display = 0;
	// Update data for 1st led matrix
		for (int i=0; i<8; i++){
			led_matrix_data->gau8_output_buf[led_matrix_data->u8_num_led_matrix-1][i] >>= 1;
			led_matrix_data->gau8_output_buf[led_matrix_data->u8_num_led_matrix-1][i] |= 0x80 & (FONT[u8_current_char][i] << (7-(led_matrix_data->gu8_cnt_display%8)));
		}
	} else
	{
	// Shift right data of all led matrix, except last led matrix
		for (int i=0; i<led_matrix_data->u8_num_led_matrix-1; i++){
			for (int j=0; j<8; j++){
				led_matrix_data->gau8_output_buf[led_matrix_data->u8_num_led_matrix-1-i][j] <<= 1;
				led_matrix_data->gau8_output_buf[led_matrix_data->u8_num_led_matrix-1-i][j] |= (led_matrix_data->gau8_output_buf[led_matrix_data->u8_num_led_matrix-2-i][j] >> 7);
			}
		}
	// Get current character displaying in 1st led matrix
		if ((led_matrix_data->gu8_cnt_display/8) < led_matrix_data-> gu8_temp_cnt_elt_uart_buf ){
			 u8_current_char = led_matrix_data->gau8_temp_uart_buf[led_matrix_data-> gu8_temp_cnt_elt_uart_buf-1-(led_matrix_data->gu8_cnt_display/8)];
		} else led_matrix_data->gu8_cnt_display = 0;
	// Update data for last led matrix
		for (int i=0; i<8; i++){
			led_matrix_data->gau8_output_buf[0][i] <<= 1;
			led_matrix_data->gau8_output_buf[0][i] |= 0x01 & (FONT[u8_current_char][i] >> (7-(led_matrix_data->gu8_cnt_display%8)));
		}
	}
	led_matrix_data->gu8_cnt_display++;
}


/*
 * @brief	Set and clear logic of load pin to latch data
 * @note	Call this in SPI receive complete callback interrupt function
 * @param 	led_matrix_data		Pointer to LED_MATRX_DATA_t structure
 */
void LED_MATRIX_SPI_Handle(LED_MATRIX_DATA_t *led_matrix_data, SPI_HandleTypeDef *hspi)
{
	if(led_matrix_data->hspi == hspi)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
	}
}


/*
 * @brief	Initialization function
 * @param 	led_matrix_data		Pointer to LED_MATRX_DATA_t structure
 * @param	hspi				Pointer to SPI_HandleTypeDef structure
 * @param	load_pin_port		Pointer to GPIO_TypeDef structure of load pin
 * @param	load_pin			Pin number of load pin
 * @param	num_led_matrix		Number of led matrix used in module
 * @param	speed_run_text
 */
void LED_MATRIX_Init(LED_MATRIX_DATA_t *led_matrix_data, SPI_HandleTypeDef *hspi, GPIO_TypeDef *load_pin_port, uint16_t u16_load_pin, uint8_t  u8_num_led_matrix)
{
	//Initialize data
	led_matrix_data->hspi = hspi;
	led_matrix_data->load_pin_port = load_pin_port;
	led_matrix_data->u16_load_pin = u16_load_pin;
	led_matrix_data->u8_num_led_matrix = u8_num_led_matrix;
	led_matrix_data->operation_param.u32_speed_run_text = 100;
	led_matrix_data->operation_param.display_direction = LED_MATRIX_DISPLAY_FORWARD;
	led_matrix_data->gu8_temp_cnt_elt_uart_buf = u8_num_led_matrix;
	led_matrix_data->gu8_cnt_display = 0;

	// Declare array use for transmit set up data
	uint8_t temp_data[u8_num_led_matrix * 2];

	// Set no use decode mode
	for(int i = 0; i < u8_num_led_matrix; i++)
	{
		temp_data[i*2] = MAX7219_REG_ADD_DECODEMODE;
		temp_data[i*2 + 1] = MAX7219_NO_DECODE;
	}
	HAL_SPI_Transmit_DMA(hspi, temp_data, 8);

	// Set max display intensity
	for(int i = 0; i < u8_num_led_matrix; i++)
	{
		temp_data[i*2] = MAX7219_REG_ADD_INTENSITY;
		temp_data[i*2 + 1] = MAX7219_INTENSITY_7_32;
	}
	HAL_SPI_Transmit_DMA(hspi, temp_data, 8);

	// Set displaying 8 digit
	for(int i = 0; i < u8_num_led_matrix; i++)
		{
			temp_data[i*2] = MAX7219_REG_ADD_SCANLIMIT;
			temp_data[i*2 + 1] = MAX7219_DISPLAY_8_DIGIT;
		}
	HAL_SPI_Transmit_DMA(hspi, temp_data, 8);

	// Set operation as normal
	for(int i = 0; i < u8_num_led_matrix; i++)
	{
		temp_data[i*2] = MAX7219_REG_ADD_SHUTDOWN;
		temp_data[i*2 + 1] = MAX7219_NO_SHUTDOWN_MODE;
	}
	HAL_SPI_Transmit_DMA(hspi, temp_data, 8);

	// Set no use display test
	for(int i = 0; i < u8_num_led_matrix; i++)
	{
		temp_data[i*2] = MAX7219_REG_ADD_DISPLAYTEST;
		temp_data[i*2 + 1] = MAX7219_DISPLAY_NORMAL;
	}
	HAL_SPI_Transmit_DMA(hspi, temp_data, 8);

	for(int i = 0; i < u8_num_led_matrix; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			led_matrix_data->gau8_output_buf[i][j] = 0x00;
		}
	}
	PUSH_OUTPUT_Buf(led_matrix_data);
}




