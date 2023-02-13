/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdlib.h"
#include "string.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define USING_DMA						1		// using dma spi = 1
#define TEXT_MOVING_DIRECTION			1		// shift left = 1
#define NUM_MODULE_LED_MATRIX			4 		// number of horizontal modules
#define SPEED							70		// character running speed (ms)
#define NUM_CHAR						255		// maximum number of character

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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_tx;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */
//8px by 8px, row major, little endian
const uint8_t FONT [128][8] = {
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	// U+0000 (nul)
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0001
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0002
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0003
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0004
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0005
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0006
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0007
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0008
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0009
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+000A
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+000B
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+000C
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+000D
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+000E
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+000F
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0010
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0011
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0012
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0013
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0014
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0015
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0016
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0017
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0018
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0019
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+001A
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+001B
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+001C
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+001D
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+001E
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+001F
	    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	// U+0020 (space)
		{0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x04},			// U+0021 (!)
		{0x00,0x36,0x36,0x12,0x00,0x00,0x00,0x00},			// U+0022 (")
		{0x00,0x14,0x3E,0x14,0x14,0x3E,0x14,0x00},			// U+0023 (#)
		{0x00,0x04,0x1C,0x02,0x0C,0x10,0x0E,0x08},			// U+0024 ($)
		{0x00,0x26,0x26,0x10,0x08,0x04,0x32,0x32},			// U+0025 (%)
		{0x00,0x04,0x0A,0x0A,0x04,0x2A,0x12,0x2C},			// U+0026 (&)
		{0x00,0x0C,0x0C,0x04,0x00,0x00,0x00,0x00},			// U+0027 (')
		{0x00,0x08,0x04,0x04,0x04,0x04,0x04,0x08},			// U+0028 (()
		{0x00,0x04,0x08,0x08,0x08,0x08,0x08,0x04},			// U+0029 ())
		{0x00,0x00,0x2A,0x1C,0x3E,0x1C,0x2A,0x00},			// U+001A (*)
		{0x00,0x00,0x08,0x08,0x3E,0x08,0x08,0x00},			// U+001B (+)
		{0x00,0x00,0x00,0x00,0x00,0x0C,0x08,0x04},			// U+001C (,)
		{0x00,0x00,0x00,0x00,0x3E,0x00,0x00,0x00},			// U+001D (-)
		{0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x0C},			// U+001E (.)
		{0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x00},			// U+001E (/)
		{0x00,0x1C,0x22,0x32,0x2A,0x26,0x22,0x1C},   		// U+0030 (0)
		{0x00,0x08,0x0C,0x08,0x08,0x08,0x08,0x1C},   		// U+0031 (1)
	    {0x00,0x1C,0x22,0x20,0x18,0x04,0x02,0x3E},   		// U+0032 (2)
	    {0x00,0x1C,0x22,0x20,0x1C,0x20,0x22,0x1C},   		// U+0033 (3)
	    {0x00,0x10,0x18,0x14,0x12,0x3E,0x10,0x10},   		// U+0034 (4)
	    {0x00,0x3E,0x02,0x02,0x1E,0x20,0x22,0x1C},   		// U+0035 (5)
	    {0x00,0x18,0x04,0x02,0x1E,0x22,0x22,0x1C},   		// U+0036 (6)
	    {0x00,0x3E,0x20,0x10,0x08,0x04,0x04,0x04},   		// U+0037 (7)
	    {0x00,0x1C,0x22,0x22,0x1C,0x22,0x22,0x1C},   		// U+0038 (8)
	    {0x00,0x1C,0x22,0x22,0x3C,0x20,0x10,0x0C},   		// U+0039 (9)
		{0x00,0x00,0x00,0x0C,0x0C,0x00,0x0C,0x0C},   		// U+003A (:)
		{0x00,0x00,0x0C,0x0C,0x00,0x0C,0x08,0x04},   		// U+003B (;)
		{0x00,0x10,0x08,0x04,0x02,0x04,0x08,0x10},   		// U+003C (<)
		{0x00,0x00,0x00,0x3E,0x00,0x00,0x3E,0x00},   		// U+003D (=)
		{0x00,0x04,0x08,0x10,0x20,0x10,0x08,0x04},   		// U+003E (>)
		{0x00,0x1C,0x22,0x20,0x18,0x08,0x00,0x08},   		// U+003F (?)
		{0x00,0x1C,0x22,0x3A,0x2A,0x3A,0x02,0x1C},   		// U+0040 (@)
		{0x00,0x1C,0x22,0x22,0x22,0x3E,0x22,0x22},			// U+0041 (A)
		{0x00,0x1E,0x22,0x22,0x1E,0x22,0x22,0x1E},			// U+0042 (B)
		{0x00,0x1C,0x22,0x02,0x02,0x02,0x22,0x1C},			// U+0043 (C)
		{0x00,0x1E,0x22,0x22,0x22,0x22,0x22,0x1E},   		// U+0044 (D)
		{0x00,0x3E,0x02,0x02,0x1E,0x02,0x02,0x3E},   		// U+0045 (E)
		{0x00,0x3E,0x02,0x02,0x1E,0x02,0x02,0x02},   		// U+0046 (F)
		{0x00,0x1C,0x22,0x02,0x3A,0x22,0x22,0x3C},   		// U+0047 (G)
		{0x00,0x22,0x22,0x22,0x3E,0x22,0x22,0x22},   		// U+0048 (H)
		{0x00,0x1C,0x08,0x08,0x08,0x08,0x08,0x1C},   		// U+0049 (I)
		{0x00,0x20,0x20,0x20,0x20,0x22,0x22,0x1C},   		// U+004A (J)
		{0x00,0x22,0x12,0x0A,0x06,0x0A,0x12,0x22},   		// U+004B (K)
		{0x00,0x02,0x02,0x02,0x02,0x02,0x02,0x3E},   		// U+004C (L)
		{0x00,0x22,0x36,0x2A,0x22,0x22,0x22,0x22},   		// U+004D (M)
		{0x00,0x22,0x26,0x2A,0x32,0x22,0x22,0x22},   		// U+004E (N)
		{0x00,0x1C,0x22,0x22,0x22,0x22,0x22,0x1C},   		// U+004F (O)
		{0x00,0x1E,0x22,0x22,0x1E,0x02,0x02,0x02},   		// U+0050 (P)
		{0x00,0x1C,0x22,0x22,0x22,0x2A,0x12,0x2C},   		// U+0051 (Q)
		{0x00,0x1E,0x22,0x22,0x1E,0x12,0x22,0x22},   		// U+0052 (R)
		{0x00,0x1C,0x22,0x02,0x1C,0x20,0x22,0x1C},   		// U+0053 (S)
		{0x00,0x3E,0x08,0x08,0x08,0x08,0x08,0x08},   		// U+0054 (T)
		{0x00,0x22,0x22,0x22,0x22,0x22,0x22,0x1C},  		// U+0055 (U)
		{0x00,0x22,0x22,0x22,0x22,0x22,0x14,0x08},   		// U+0056 (V)
		{0x00,0x22,0x22,0x2A,0x2A,0x2A,0x2A,0x14}, 			// U+0057 (W)
		{0x00,0x22,0x22,0x14,0x08,0x14,0x22,0x22}, 	  		// U+0058 (X)
		{0x00,0x22,0x22,0x22,0x14,0x08,0x08,0x08},   		// U+0059 (Y)
		{0x00,0x1E,0x10,0x08,0x04,0x02,0x02,0x1E},   		// U+005A (Z)
		{0x00,0x0C,0x04,0x04,0x04,0x04,0x04,0x0C},   		// U+005B ([)
		{0x00,0x00,0x02,0x04,0x08,0x10,0x20,0x00},   		// U+005C (\)
		{0x00,0x18,0x10,0x10,0x10,0x10,0x10,0x18},   		// U+005D (])
		{0x00,0x08,0x14,0x22,0x00,0x00,0x00,0x00},   		// U+005E (^)
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3E},   		// U+005F (_)
		{0x00,0x02,0x04,0x00,0x00,0x00,0x00,0x00},   		// U+0060 (`)
		{0x00,0x00,0x00,0x1C,0x20,0x3C,0x22,0x3C},   		// U+0061 (a)
		{0x00,0x02,0x02,0x1E,0x22,0x22,0x22,0x1E},  		// U+0062 (b)
		{0x00,0x00,0x00,0x1C,0x02,0x02,0x02,0x1C},   		// U+0063 (c)
		{0x00,0x20,0x20,0x3C,0x22,0x22,0x22,0x3C},   		// U+0064 (d)
		{0x00,0x00,0x00,0x1C,0x22,0x1E,0x02,0x1C},   		// U+0065 (e)
		{0x00,0x18,0x04,0x04,0x1E,0x04,0x04,0x04},   		// U+0066 (f)
		{0x00,0x00,0x3C,0x22,0x22,0x3C,0x20,0x1C},   		// U+0067 (g)
		{0x00,0x02,0x02,0x0E,0x12,0x12,0x12,0x12},   		// U+0068 (h)
		{0x00,0x08,0x00,0x08,0x08,0x08,0x08,0x18},   		// U+0069 (i)
		{0x10,0x00,0x18,0x10,0x10,0x10,0x12,0x0C},   		// U+006A (j)
		{0x00,0x02,0x02,0x12,0x0A,0x06,0x0A,0x12},   		// U+006B (k)
		{0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x18},   		// U+006C (l)
		{0x00,0x00,0x00,0x16,0x2A,0x2A,0x22,0x22},   		// U+006D (m)
		{0x00,0x00,0x00,0x0E,0x12,0x12,0x12,0x12},   		// U+006E (n)
		{0x00,0x00,0x00,0x1C,0x22,0x22,0x22,0x1C},   		// U+006F (o)
		{0x00,0x00,0x1E,0x22,0x22,0x1E,0x02,0x02},   		// U+0070 (p)
		{0x00,0x00,0x3C,0x22,0x22,0x3C,0x20,0x20},   		// U+0071 (q)
		{0x00,0x00,0x00,0x1A,0x24,0x04,0x04,0x0E},   		// U+0072 (r)
		{0x00,0x00,0x00,0x1C,0x02,0x1C,0x20,0x1C},   		// U+0073 (s)
		{0x00,0x00,0x04,0x1E,0x04,0x04,0x14,0x08},  		// U+0074 (t)
		{0x00,0x00,0x00,0x12,0x12,0x12,0x1A,0x14},  		// U+0075 (u)
		{0x00,0x00,0x00,0x22,0x22,0x22,0x14,0x08},   		// U+0076 (v)
		{0x00,0x00,0x00,0x22,0x22,0x2A,0x3E,0x14},   		// U+0077 (w)
		{0x00,0x00,0x00,0x12,0x12,0x0C,0x12,0x12},   		// U+0078 (x)
		{0x00,0x00,0x12,0x12,0x12,0x1C,0x08,0x06},   		// U+0079 (y)
		{0x00,0x00,0x00,0x1E,0x10,0x0C,0x02,0x1E},   		// U+007A (z)
		{0x00,0x18,0x04,0x04,0x06,0x04,0x04,0x18},   		// U+007B ({)
		{0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x08},   		// U+007C (|)
		{0x00,0x0c,0x10,0x10,0x30,0x10,0x10,0x0c},   		// U+007D (})
		{0x00,0x14,0x0a,0x00,0x00,0x00,0x00,0x00},   		// U+007E (~)
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}    		// U+007F
};


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

void rotate (uint8_t *output);
void data_to_module (uint8_t *output );
void init_max7219(void);
void ascii_to_matrix (uint8_t buf[], uint8_t pointer, uint8_t *output);

void init_max7219(void);
void u8_update_output_buf(void);
void u8_push_output_buf(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint8_t gu8_uart_rx;										// receive data from uart
uint8_t gau8_uart_buf[NUM_CHAR];
uint8_t gu8_cnt_elt_uart_buf = NUM_MODULE_LED_MATRIX;		// count the number of characters received from uart or the number of array elements uart_buf

uint8_t gau8_temp_uart_buf[NUM_CHAR];						// temporary array that stores the value of uart_buf
uint8_t gu8_temp_cnt_elt_uart_buf=NUM_MODULE_LED_MATRIX;	// temporary variable stores the value of the variable cnt_elt_uart_buf

uint8_t gau8_output_buf[NUM_MODULE_LED_MATRIX][8] = {};		// array [4x8] contains data to be pushed out to the led matrix module
uint8_t gu8_cnt_display = 0;								//manage to update output_buf array to what character in uart_buf array and what column


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == hspi2.Instance)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == huart2.Instance)
	{

		if (gu8_uart_rx =='\n'){
			uint8_t i;
			gu8_temp_cnt_elt_uart_buf = gu8_cnt_elt_uart_buf;
			for (i=0; i<gu8_temp_cnt_elt_uart_buf; i++){
				gau8_temp_uart_buf[i] = gau8_uart_buf[i];
			}
			memset(gau8_temp_uart_buf,0x00,4);		//create whitespace
			gu8_cnt_elt_uart_buf = NUM_MODULE_LED_MATRIX;
			gu8_cnt_display = 0;

		}
		else{
			gau8_uart_buf[gu8_cnt_elt_uart_buf] = gu8_uart_rx;
			gu8_cnt_elt_uart_buf++;

		}
		HAL_UART_Receive_DMA(&huart2, &gu8_uart_rx, 1);
	}
}

void u8_update_output_buf(void){

	uint8_t i,j;
	uint8_t u8_current_char;	//contains the character being updated for the output_buf array
#if TEXT_MOVING_DIRECTION
	// Shift left data of all led matrix, except 1st led matrix
		for (i=0; i<NUM_MODULE_LED_MATRIX-1; i++){
			for (j=0; j<8; j++){
				gau8_output_buf[i][j] >>= 1;
				gau8_output_buf[i][j] |= (gau8_output_buf[i+1][j] << 7);
			}
		}
	// Get current character displaying in 1st led matrix
		if ((gu8_cnt_display/8) < gu8_temp_cnt_elt_uart_buf ){
			 u8_current_char = gau8_temp_uart_buf[(gu8_cnt_display/8)];
		} else gu8_cnt_display = 0;
	// Update data for 1st led matrix
		for (i=0; i<8; i++){
			 gau8_output_buf[NUM_MODULE_LED_MATRIX-1][i] >>= 1;
			 gau8_output_buf[NUM_MODULE_LED_MATRIX-1][i] |= 0x80 & (FONT[u8_current_char][i] << (7-(gu8_cnt_display%8)));
		}
#else
	// Shift right data of all led matrix, except last led matrix
		for (i=0; i<NUM_MODULE_LED_MATRIX-1; i++){
			for (j=0; j<8; j++){
				gau8_output_buf[NUM_MODULE_LED_MATRIX-1-i][j] <<= 1;
				gau8_output_buf[NUM_MODULE_LED_MATRIX-1-i][j] |= (gau8_output_buf[NUM_MODULE_LED_MATRIX-2-i][j] >> 7);
			}
		}
	// Get current character displaying in 1st led matrix
		if ((gu8_cnt_display/8) < gu8_temp_cnt_elt_uart_buf ){
			 u8_current_char = gau8_temp_uart_buf[gu8_temp_cnt_elt_uart_buf-1-(gu8_cnt_display/8)];
		} else gu8_cnt_display = 0;
	// Update data for last led matrix
		for (i=0; i<8; i++){
			 gau8_output_buf[0][i] <<= 1;
			 gau8_output_buf[0][i] |= 0x01 & (FONT[u8_current_char][i] >> (7-(gu8_cnt_display%8)));
		}
#endif
    gu8_cnt_display++;
}

void u8_push_output_buf(void){
	uint32_t i;
	int32_t j;
	for (i = 0; i<8; i++){
		uint8_t u8_temp_data[2*NUM_MODULE_LED_MATRIX];
		for(j = 0; j< NUM_MODULE_LED_MATRIX;j++){
			u8_temp_data[2*j] = i+1;
			u8_temp_data[2*j+1] = gau8_output_buf[NUM_MODULE_LED_MATRIX-1-j][7-i];
		}
#if USING_DMA
		HAL_SPI_Transmit_DMA(&hspi2, u8_temp_data, 2*NUM_MODULE_LED_MATRIX);
#else
		HAL_SPI_Transmit(&hspi2, u8_temp_data, 2*NUM_MODULE_LED_MATRIX, 1000);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
#endif

	}
}
void init_max7219(void){
	uint8_t i;
	for (i = 0; i < NUM_MODULE_LED_MATRIX; i++){
		uint8_t u8_temp_data[2] = {MAX7219_REG_ADD_DECODEMODE, MAX7219_NO_DECODE};
#if USING_DMA
		HAL_SPI_Transmit_DMA(&hspi2, u8_temp_data, 2);
#else
		HAL_SPI_Transmit(&hspi2, u8_temp_data, 2, 1000);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
#endif
	}
	for (i = 0; i < NUM_MODULE_LED_MATRIX; i++){
		uint8_t u8_temp_data[2] = {MAX7219_REG_ADD_INTENSITY, MAX7219_INTENSITY_7_32};
#if USING_DMA
		HAL_SPI_Transmit_DMA(&hspi2, u8_temp_data, 2);
#else
		HAL_SPI_Transmit(&hspi2, u8_temp_data, 2, 1000);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
#endif
	}
	for (i = 0; i < NUM_MODULE_LED_MATRIX; i++){
		uint8_t u8_temp_data[2] = {MAX7219_REG_ADD_SCANLIMIT, MAX7219_DISPLAY_8_DIGIT};
#if USING_DMA
		HAL_SPI_Transmit_DMA(&hspi2, u8_temp_data, 2);
#else
		HAL_SPI_Transmit(&hspi2, u8_temp_data, 2, 1000);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
#endif
	}
	for (i = 0; i < NUM_MODULE_LED_MATRIX; i++){
		uint8_t u8_temp_data[2] = {MAX7219_REG_ADD_SHUTDOWN, MAX7219_NO_SHUTDOWN_MODE};
#if USING_DMA
		HAL_SPI_Transmit_DMA(&hspi2, u8_temp_data, 2);
#else
		HAL_SPI_Transmit(&hspi2, u8_temp_data, 2, 1000);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
#endif
	}
	for (i = 0; i < NUM_MODULE_LED_MATRIX; i++){
		uint8_t u8_temp_data[2] = {MAX7219_REG_ADD_DISPLAYTEST, MAX7219_DISPLAY_NORMAL};
#if USING_DMA
		HAL_SPI_Transmit_DMA(&hspi2, u8_temp_data, 2);
#else
		HAL_SPI_Transmit(&hspi2, u8_temp_data, 2, 1000);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
#endif
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  init_max7219();
  HAL_UART_Receive_DMA(&huart2, &gu8_uart_rx, 1);
  uint32_t timer = 0;
  u8_push_output_buf();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  if (HAL_GetTick()-timer >= SPEED){
	  		  u8_update_output_buf();
	  		  u8_push_output_buf();
	  		  timer=HAL_GetTick();
	  	 }


  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
