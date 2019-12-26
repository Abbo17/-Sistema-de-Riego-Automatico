
#include "lcd.h"
#include "chip.h"
#include "stdint.h"

union ubyte
{
	char _byte;
	struct
	{
		unsigned char b0 : 1;
		unsigned char b1 : 1;
		unsigned char b2 : 1;
		unsigned char b3 : 1;
		unsigned char b4 : 1;
		unsigned char b5 : 1;
		unsigned char b6 : 1;
		unsigned char b7 : 1;
	} bit;
};

// Display configuration global variable
static char lcd_mode;

//**************************************************************************
//* A simple delay function (used by LCD functions)
//**************************************************************************
//* Calling arguments
//* unsigned char time: aproximate delay time in microseconds
//**************************************************************************
void LCD_delay_ms(unsigned char delay_ms) {
   volatile uint64_t i;
   volatile uint64_t delay;

   delay = INACCURATE_TO_MS * delay_ms;

   for( i=delay; i>0; i-- );
}
//**************************************************************************
//* Send a nibble to the LCD
//**************************************************************************
//* Calling arguments
//* char data : data to be sent to the display
//**************************************************************************
void LCD_send_nibble(char data)
{
	union ubyte my_union;
	my_union._byte = data;
	/* Output the four data bits
	LCD_D4 = my_union.bit.b0;
	LCD_D5 = my_union.bit.b1;
	LCD_D6 = my_union.bit.b2;
	LCD_D7 = my_union.bit.b3;
	*/
	PinSetLCD( LCD_D4_GPIO, LCD_D4_PIN, my_union.bit.b0);
	PinSetLCD( LCD_D5_GPIO, LCD_D5_PIN, my_union.bit.b1);
	PinSetLCD( LCD_D6_GPIO, LCD_D6_PIN, my_union.bit.b2);
	PinSetLCD( LCD_D7_GPIO, LCD_D7_PIN, my_union.bit.b3);

	// pulse the LCD enable line
	PinSetLCD( LCD_ENABLE_GPIO, LCD_ENABLE_PIN, 1);
	for (data=306; data; data--);
	PinSetLCD( LCD_ENABLE_GPIO, LCD_ENABLE_PIN, 0);
}

//**************************************************************************
//* Write a byte into the LCD
//**************************************************************************
//* Calling arguments:
//* char address : 0 for instructions, 1 for data
//* char data : command or data to be written
//**************************************************************************
void LCD_send_byte(char address, char data)
{
  unsigned int temp;
	if(address==0)
		PinSetLCD( LCD_RS_GPIO, LCD_RS_PIN, 0);               // config the R/S line
	else
		PinSetLCD( LCD_RS_GPIO, LCD_RS_PIN, 1);
	PinSetLCD( LCD_ENABLE_GPIO, LCD_ENABLE_PIN, 0);           // set LCD enable line to 0
	LCD_send_nibble(data >> 4);     // send the higher nibble
	LCD_send_nibble(data & 0x0f);   // send the lower nibble
	LCD_delay_ms(1);
}

//**************************************************************************
//* LCD initialization
//**************************************************************************
//* Calling arguments:
//* char mode1 : display mode (number of lines and character size)
//* char mode2 : display mode (cursor and display state)
//**************************************************************************
void LCD_init(char mode1, char mode2)
{
	char aux;

	ConfigurarPines();

	LCD_delay_ms(15);
	// LCD 4-bit mode initialization sequence
	// send three times 0x03 and then 0x02 to finish configuring the LCD
	for(aux=0;aux<3;++aux)
	{
	  LCD_send_nibble(3);
	  LCD_delay_ms(5);
	}
	LCD_send_nibble(2);
	// Now send the LCD configuration data
	LCD_send_byte(0,0x20 | mode1);
	LCD_send_byte(0,0x08 | mode2);
	lcd_mode = 0x08 | mode2;

	LCD_send_byte(0,1);
	LCD_delay_ms(5);
	LCD_send_byte(0,6); //entry mode set I/D=1 S=0
}

//**************************************************************************
//* LCD cursor position set
//**************************************************************************
//* Calling arguments:
//* char x : column (starting by 0)
//* char y : line (0 or 1)
//**************************************************************************
void LCD_pos_xy(char x, char y)
{
  char address;
  if (y) address = LCD_SEC_LINE; else address = 0;
  address += x;
  LCD_send_byte(0,0x80|address);
}

//**************************************************************************
//* Write a character on the display
//**************************************************************************
//* Calling arguments:
//* char c : character to be written
//**************************************************************************
//* Notes :
//* \f clear the display
//* \n and \r return the cursor to line 1 column 0
//**************************************************************************
void LCD_write_char(char c)
{
  switch (c)
	{
	  case '\f' :
	    LCD_send_byte(0,1);
	    LCD_delay_ms(5);
	    break;
	  case '\n' :
	  case '\r' :
	    LCD_pos_xy(0,1);
	    break;
	  default:
	    LCD_send_byte(1,c);
   }
}

//**************************************************************************
//* Write a string on the display
//**************************************************************************
//* Calling arguments:
//* char *c : pointer to the string
//**************************************************************************
void LCD_write_string (char *c)
{
	while (*c)
	{
	  LCD_write_char(*c);
	  c++;
	}
}

//**************************************************************************
//* Turn the display on
//**************************************************************************
void LCD_display_on(void)
{
	lcd_mode |= 4;
	LCD_send_byte (0,lcd_mode);
}

//**************************************************************************
//* Turn the display off
//**************************************************************************
void LCD_display_off(void)
{
	lcd_mode &= 0b11111011;
	LCD_send_byte (0,lcd_mode);
}

//**************************************************************************
//* Turn the cursor on
//**************************************************************************
void LCD_cursor_on(void)
{
  lcd_mode |= 2;
	LCD_send_byte (0,lcd_mode);
}

//**************************************************************************
//* Turn the cursor off
//**************************************************************************
void LCD_cursor_off(void)
{
	lcd_mode &= 0b11111101;
	LCD_send_byte (0,lcd_mode);
}

//**************************************************************************
//* Turn on the cursor blink function
//**************************************************************************
void LCD_cursor_blink_on(void)
{
	lcd_mode |= 1;
	LCD_send_byte (0,lcd_mode);
}

//**************************************************************************
//* Turn off the cursor blink function
//**************************************************************************
void LCD_cursor_blink_off(void)
{
	lcd_mode &= 0b11111110;
	LCD_send_byte (0,lcd_mode);
}

void ConfigurarPines() {

//* Initializes GPIO */
 Chip_GPIO_Init(LPC_GPIO_PORT);

/* Config EDU-CIAA-NXP Pins as GPIOs */
 Chip_SCU_PinMux( LCD_ENABLE_SCU_PORT, LCD_ENABLE_SCU_PIN, MD_PLN, FUNC4); /* P4_9, GPIO5[13], LCD_ENABLE */
 Chip_SCU_PinMux( LCD_RS_SCU_PORT, LCD_RS_SCU_PIN, MD_PLN, FUNC4); /* P4_8, GPIO5[12], LCD_RS*/
 Chip_SCU_PinMux( LCD_D4_SCU_PORT, LCD_D4_SCU_PIN, MD_PLN, FUNC4); /* P4_4, GPIO2[4], LCD_D4 */
 Chip_SCU_PinMux( LCD_D5_SCU_PORT, LCD_D5_SCU_PIN, MD_PLN, FUNC0); /* P4_5, GPIO2[5], LCD_D5 */
 Chip_SCU_PinMux( LCD_D6_SCU_PORT, LCD_D6_SCU_PIN, MD_PLN, FUNC0); /* P4_6, GPIO2[6], LCD_D6 */
 Chip_SCU_PinMux( LCD_D7_SCU_PORT, LCD_D7_SCU_PIN, MD_PLN, FUNC0); /* P4_10, GPIO5[14], LCD_D7 */

/* Config EDU-CIAA-NXP Led Pins as Outputs */
 Chip_GPIO_SetDir( LPC_GPIO_PORT, LCD_ENABLE_GPIO, (1<<LCD_ENABLE_PIN), SALIDAA); //OUTPUT=1
 Chip_GPIO_SetDir( LPC_GPIO_PORT, LCD_RS_GPIO, (1<<LCD_RS_PIN), SALIDAA); //OUTPUT=1
 Chip_GPIO_SetDir( LPC_GPIO_PORT, LCD_D4_GPIO, (1<<LCD_D4_PIN), SALIDAA); //OUTPUT=1
 Chip_GPIO_SetDir( LPC_GPIO_PORT, LCD_D5_GPIO, (1<<LCD_D5_PIN), SALIDAA); //OUTPUT=1
 Chip_GPIO_SetDir( LPC_GPIO_PORT, LCD_D6_GPIO, (1<<LCD_D6_PIN), SALIDAA); //OUTPUT=1
 Chip_GPIO_SetDir( LPC_GPIO_PORT, LCD_D7_GPIO, (1<<LCD_D7_PIN), SALIDAA); //OUTPUT=1

/* Control EDU-CIAA-NXP Led Pins */
 PinSetLCD( LCD_ENABLE_GPIO, LCD_ENABLE_PIN, 0);
 PinSetLCD( LCD_RS_GPIO, LCD_RS_PIN, 0);
 PinSetLCD( LCD_D4_GPIO, LCD_D4_PIN, 0);
 PinSetLCD( LCD_D5_GPIO, LCD_D5_PIN, 0);
 PinSetLCD( LCD_D6_GPIO, LCD_D6_PIN, 0);
 PinSetLCD( LCD_D7_GPIO, LCD_D7_PIN, 0);



}

void PinSetLCD(unsigned char PORT, unsigned char PIN, unsigned char valor){
   Chip_GPIO_SetPinState( LPC_GPIO_PORT, PORT, PIN, valor);
}
