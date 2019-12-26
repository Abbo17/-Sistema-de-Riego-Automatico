/* Copyright 2016, Eric Pernia.
 * All rights reserved.
 *
 * This file is part sAPI library for microcontrollers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * Date: 2016-07-28
 */

/*==================[inclusions]=============================================*/

//#include "keypad_7segment.h"   // <= own header (optional)
#include "sapi.h"                // <= sAPI header
#include "lcd.h"
#include "teclado.h"
#include "mef.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/


extern unsigned char entradaPresiono;
extern unsigned char tecladoHabilitado;
extern int TMEF;
extern int Tiempo;
extern int Periodo;
extern int Dia;

int ticksTiempo = 0;
volatile int ticksMin = 0;
int ticksSeg = 0;
int ticksPeriodo = 0;
int ticksTeclado = 0;
extern int ticksMef;

unsigned char flagTeclado = 0;
unsigned char flagTiempo = 0;
unsigned char flagPeriodo = 0;
unsigned char flagMef = 0;


void interrupt_handler(void *ptr){
	ticksTeclado++;
	ticksMef++;
	ticksMin++;
	ticksSeg++;


	if (ticksTeclado == 4) {
		ticksTeclado = 0;
		flagTeclado = 1;
	}

	if (ticksMef == (TMEF*2)) {
		flagMef = 1;
		ticksMef = 0;
	} else {
		if (TMEF == -1) {
			ticksMef = 0;
		}
	}

	if (ticksMin == 1200) {
		if (Periodo != -1) Periodo--;
		if (Periodo == 0) {
			Periodo = -1;
			flagMef = 1;
		}
		if (Dia != -1) Dia--;
		if (Dia == 0){
			Dia = -1;
			flagMef = 1;
		}
		ticksMin=0;
	}

	if (ticksSeg == 20) {
		if (Tiempo != -1) Tiempo--;
		if (Tiempo == 0) {
			Tiempo = -1;
			flagMef = 1;
		}
		ticksSeg = 0;
	}

}

/* FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE RESET. */
int main(void){

   /* ------------- INICIALIZACIONES ------------- */

   /* Inicializar la placa */
   boardConfig();
   Init_MEF();
   LCD_init(_2_LINES | DISPLAY_8X5,DISPLAY_ON);
   tickConfig( 50 );
   tickCallbackSet( interrupt_handler, (void*)LEDR );
   initKeypad();

   while (1){
	   if (flagTeclado ) {
			leerTeclado();
			flagTeclado = 0;
		}

		if (flagMef || entradaPresiono) {
			Update_MEF();
			if (entradaPresiono) entradaPresiono = 0;
			flagMef=0;
		}
   }
   /* NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa no es llamado
      por ningun S.O. */
   return 0 ;
}

/*==================[end of file]============================================*/
