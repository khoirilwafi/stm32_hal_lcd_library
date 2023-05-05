/*
 * lcd.c
 *
 * Created : 10/06/2018
 * Author  : Muhammad Khoiril Wafi
 * Company : Teknik Elektro Diponegoro University
 *
 */

#include <stdio.h>
#include "lcd.h"

const uint8_t ROW_16[] = {0x00, 0x40, 0x10, 0x50};
const uint8_t ROW_20[] = {0x00, 0x40, 0x14, 0x54};

// static declaration - only for internal function

static void lcd_write_data(uint8_t data);
static void lcd_write_command(uint8_t command);
static void lcd_write(uint8_t data, uint8_t len);

Lcd_HandleTypeDef hlcd;

// ---------- function implementation ----------

void Lcd_create(Lcd_PortType port[], Lcd_PinType pin[], Lcd_PortType rs_port, Lcd_PinType rs_pin, Lcd_PortType en_port, Lcd_PinType en_pin, Lcd_ModeTypeDef mode)
{
	hlcd.mode      = mode;
	hlcd.en_pin    = en_pin;
	hlcd.en_port   = en_port;
	hlcd.rs_pin    = rs_pin;
	hlcd.rs_port   = rs_port;
	hlcd.data_pin  = pin;
	hlcd.data_port = port;

	Lcd_init(hlcd);
}

// init lcd
void Lcd_init()
{
	if(hlcd.mode == LCD_4_BIT_MODE)
	{
		lcd_write_command(0x33);
		lcd_write_command(0x32);
		lcd_write_command(FUNCTION_SET | OPT_N);
	}
	else
	{
		lcd_write_command(FUNCTION_SET | OPT_DL | OPT_N);
	}

	lcd_write_command(CLEAR_DISPLAY);						// Clear screen
	lcd_write_command(DISPLAY_ON_OFF_CONTROL | OPT_D);		// Lcd-on, cursor-off, no-blink
	lcd_write_command(ENTRY_MODE_SET | OPT_INC);			// Increment cursor
}

// lcd print integer
void Lcd_int(int number)
{
	char buffer[11];
	sprintf(buffer, "%d", number);

	Lcd_string(buffer);
}

// lcd print string
void Lcd_string(char * string)
{
	for(uint8_t i = 0; i < strlen(string); i++)
	{
		lcd_write_data(string[i]);
	}
}

// set cursor position
void Lcd_cursor(uint8_t row, uint8_t col)
{
	#ifdef LCD20xN
		lcd_write_command(SET_DDRAM_ADDR + ROW_20[row] + col);
	#endif

	#ifdef LCD16xN
		lcd_write_command(SET_DDRAM_ADDR + ROW_16[row] + col);
	#endif
}

// clear the screen
void Lcd_clear() {
	lcd_write_command(CLEAR_DISPLAY);
}


// ---------- static function implementation -----------

// send command to lcd
void lcd_write_command(uint8_t command)
{
	HAL_GPIO_WritePin(hlcd.rs_port, hlcd.rs_pin, LCD_COMMAND_REG);

	if(hlcd.mode == LCD_4_BIT_MODE)
	{
		lcd_write(command >> 4, LCD_NIB);
		lcd_write(command & 0x0F, LCD_NIB);
	}
	else
	{
		lcd_write(command, LCD_BYTE);
	}

}

// send data to lcd
void lcd_write_data(uint8_t data)
{
	HAL_GPIO_WritePin(hlcd.rs_port, hlcd.rs_pin, LCD_DATA_REG);

	if(hlcd.mode == LCD_4_BIT_MODE)
	{
		lcd_write(data >> 4, LCD_NIB);
		lcd_write(data & 0x0F, LCD_NIB);
	}
	else
	{
		lcd_write(data, LCD_BYTE);
	}

}

// lcd write data pin helper
void lcd_write(uint8_t data, uint8_t len)
{
	for(uint8_t i = 0; i < len; i++)
	{
		HAL_GPIO_WritePin(hlcd.data_port[i], hlcd.data_pin[i], (data >> i) & 0x01);
	}

	HAL_GPIO_WritePin(hlcd.en_port, hlcd.en_pin, 1);
	DELAY(1);
	HAL_GPIO_WritePin(hlcd.en_port, hlcd.en_pin, 0);
}
