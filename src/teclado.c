#include "teclado.h"
#include "sapi.h"

keypad_t keypad;

uint8_t keypadRowPins1[4] = {
		T_COL0,
		T_FIL2,
		T_FIL3,
		T_FIL0
};

// Columnas --> Entradas con pull-up (MODO = GPIO_INPUT_PULLUP)
uint8_t keypadColPins1[4] = {
		T_COL1,
		CAN_TD,
		CAN_RD,
		RS232_TXD
};

// Vector de conversion entre indice de tecla presionada y el índice del
	// display 7 segmentos
unsigned char  keypadToDesplayKeys[16] = {
									   '1',    '2',    '3', 'A',
									   '4',    '5',    '6', 'B',
									   '7',    '8',    '9', 'C',
									'*',    '0', '#', 'D'
								  };

// Variable para guardar la tecla leida
volatile unsigned char entradaKey = 0;
volatile unsigned char entradaPresiono = 0;
volatile unsigned char entradaNumero = 0;
uint16_t valor;
void initKeypad(){
	keypadConfig( &keypad, keypadRowPins1, 4, keypadColPins1, 4 );
}

void leerTeclado(){

	entradaPresiono=keypadRead( &keypad, &valor );

	if (entradaPresiono){
		entradaKey = keypadToDesplayKeys[valor];

		switch (entradaKey){
		case '0': entradaNumero=1;
		break;
		case '1': entradaNumero=1;
				break;
		case '2': entradaNumero=1;
				break;
		case '3': entradaNumero=1;
				break;
		case '4': entradaNumero=1;
				break;
		case '5': entradaNumero=1;
				break;
		case '6': entradaNumero=1;
				break;
		case '7': entradaNumero=1;
				break;
		case '8': entradaNumero=1;
				break;
		case '9': entradaNumero=1;
				break;
		default :entradaNumero=0;
		break;
		}
	}

}
