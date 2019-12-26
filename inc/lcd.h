/*
 * lcd.h
 *
 *  Created on: 30/07/2013
 *      Author: pc2
 */

#ifndef LCD_H_
#define LCD_H_


// The following defines set the default pins for the LCD display
#ifndef LCD_ENABLE			 // if lcd_enable is not defined

#define LCD_ENABLE_SCU_PORT 4
#define LCD_ENABLE_SCU_PIN 9
#define LCD_ENABLE_GPIO 5
#define LCD_ENABLE_PIN 13

#define LCD_RS_SCU_PORT 4
#define LCD_RS_SCU_PIN 8
#define LCD_RS_GPIO 5
#define LCD_RS_PIN 12

#define LCD_D7_SCU_PORT 4
#define LCD_D7_SCU_PIN 4
#define LCD_D7_GPIO 2
#define LCD_D7_PIN 4

#define LCD_D6_SCU_PORT 4
#define LCD_D6_SCU_PIN 5
#define LCD_D6_GPIO 2
#define LCD_D6_PIN 5

#define LCD_D5_SCU_PORT 4
#define LCD_D5_SCU_PIN 6
#define LCD_D5_GPIO 2
#define LCD_D5_PIN 6

#define LCD_D4_SCU_PORT 4
#define LCD_D4_SCU_PIN 10
#define LCD_D4_GPIO 5
#define LCD_D4_PIN 14

#define ENTRADAA 0
#define SALIDAA 1

#endif

#define LCD_SEC_LINE    0x40    // Address of the second line of the LCD

// LCD configuration constants
#define CURSOR_ON       2
#define CURSOR_OFF      0
#define CURSOR_BLINK    1
#define CURSOR_NOBLINK  0
#define DISPLAY_ON      4
#define DISPLAY_OFF     0
#define DISPLAY_8X5     0
#define DISPLAY_10X5    4
#define _2_LINES        8
#define _1_LINE         0


//**************************************************************************
//* Prototypes
//**************************************************************************
void LCD_delay_ms(unsigned char delay_ms);
void LCD_send_nibble(char data);
void LCD_send_byte(char address, char data);
void LCD_init(char mode1, char mode2);
void LCD_pos_xy(char x, char y);
void LCD_write_char(char c);
void LCD_write_string (char *c);
void LCD_display_on(void);
void LCD_display_off(void);
void LCD_cursor_on(void);
void LCD_cursor_off(void);
void LCD_cursor_blink_on(void);
void LCD_cursor_blink_off(void);
void PinSetLCD(unsigned char PORT, unsigned char PIN, unsigned char valor);
void ConfigurarPines(void);

#define INACCURATE_TO_MS 20400

#endif /* LCD_H_ */

/* Initializes GPIO */
// Chip_GPIO_Init(LPC_GPIO_PORT);
/* Config EDU-CIAA-NXP Led Pins as GPIOs */
// Chip_SCU_PinMux(LED1_P, LED1_P_, MD_PUP, FUNC0); /* P2_10, GPIO0[14], LED1 */
/* Config EDU-CIAA-NXP Led Pins as Outputs */
// Chip_GPIO_SetDir(LPC_GPIO_PORT, LED1_GPIO, (1<<LED1_PIN), OUTPUT); //OUTPUT=1
/* Init EDU-CIAA-NXP Led Pins OFF */
// Chip_GPIO_ClearValue(LPC_GPIO_PORT, LED1_GPIO, (1<<LED1_PIN));
/* Control EDU-CIAA-NXP Led Pins */
// Chip_GPIO_SetPinState( LPC_GPIO_PORT, LED1_GPIO, LED1_PIN, status); //0 o 1

