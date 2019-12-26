#include "mef.h"
#include "lcd.h"
#include "teclado.h"
#include "chip.h"
#include "stdint.h"
#include "sapi.h"              // <= sAPI header


/*Definici�n de variables */


 statename state; /*Estado actual de la MEF */

extern  unsigned char entradaPresiono; // Variable de entrada que indica si se presiono una key
extern  unsigned char entradaKey; /* Variable de entrada que referencia a la key que se presiono */
extern  unsigned char entradaNumero; /* Variable de entrada que indica si se presiono un numero */

extern int ticksMin;

struct struct_Salida salidaString; //Salida del LCD
volatile unsigned char tecladoHabilitado=1;
struct struct_infoHumedad infoHumedad;
struct struct_infoRiego infoRiego;
struct struct_infoHumedadChar infoHumedadChar;
struct struct_infoRiegoChar infoRiegoChar;
volatile int ticksMef=0;
struct struct_datos datos;


static ADC_CLOCK_SETUP_T ADCSetup;

volatile int Tiempo;
volatile int Periodo;
volatile int Dia;
volatile int TMEF; /*Cada cuanto debe ejecutarse la ME */

int numeroMensaje = 0; /*Indica en algun estado que mensaje mostrar*/
int digito = 0;    	/*Indica que digito se esta ingresado */
int datosIngresados = 0; /*Indica que datos se estan ingresando de la informaci�n */
float tiempoRegando = 0;

unsigned char regar = 0 ;
unsigned char datosCorrectos = 0;
unsigned char modo; /* 0 modo riego, 1 modo humedad*/

/*Implementaci�n de funciones de la maquina de Estado */
void Init_MEF(void){
	state = apagado;
	tecladoHabilitado = 1;
	numeroMensaje = 0;
	limpiarInformacion();
	TMEF = 1;
	ticksMef = 0;
	Periodo = -1;
	Tiempo = -1;
	Dia = MS_X_DIA;
	Chip_SCU_PinMux( BOMBA_SCU_PORT, BOMBA_SCU_PIN, MD_PLN, FUNC0);
	Chip_GPIO_SetDir( LPC_GPIO_PORT, BOMBA_GPIO_PORT, (1<<BOMBA_GPIO_PIN), SALIDA);
	Chip_ADC_Init(LPC_ADC1,&ADCSetup);
	Chip_ADC_EnableChannel(LPC_ADC1,ADC_CH1,ENABLE);
	PinSetMEF( BOMBA_GPIO_PORT, BOMBA_GPIO_PIN, 0);
}


void Update_MEF(void){

	switch(state){
	case apagado:
		fApagado();
	break;
	case bienvenida:
		fBienvenida();
	break;
	case seleccionmodo:
		fSeleccionModo();
	break;
	case configuracionriego:
		fConfiguracionRiego();
	break;
	case configuracionhumedad:
		fConfiguracionHumedad();
	break;
	case confirmaciondatos:
		fConfirmacionDatos();
	break;
	case mostrardatos:
		fMostrar_datos();
	break;
	case regando:
		fRegando();
	break;

	}

	mostrarSalidaLCD();
}

void fApagado(){
	limpiarSalida();

	TMEF = -1;
	if (entradaPresiono && ( entradaKey == T_ENCENDIDO ) ){
		numeroMensaje = 0;
		TMEF = 1;
		state = bienvenida;
		limpiarInformacion();
	}

}

void fSeleccionModo(){
	ticksMef = 0;
	TMEF = 30; // Cada un segundo alternamos el mensaje
	salidaSeleccionModo(numeroMensaje);
	if (numeroMensaje == 1) {
		numeroMensaje = 0;
	} else {
		numeroMensaje++;
	}
	if (entradaPresiono){
		switch(entradaKey){

			case CANCELAR:
				Init_MEF();

			break;

			case T_SELECCIONAR_MODO_RIEGO:
				digito = 0;
				datosIngresados = 0;
				modo = 0;
				state = configuracionriego;
				TMEF = 1;
				ticksMef = 0;
			break;

			case T_SELECCIONAR_MODO_HUMEDAD:
				digito = 0;
				datosIngresados = 0;
				modo = 1;
				state = configuracionhumedad;
				TMEF = 1;
				ticksMef = 0;
			break;

		default: break;
		}
	}
}

void fConfirmacionDatos(){
	TMEF = -1;
	if (entradaPresiono){
		switch(entradaKey){

			case CANCELAR:
				numeroMensaje = 0;
				state = seleccionmodo;
				limpiarInformacion();
				TMEF = 1;
				ticksMef = 0;
				break;

			case CONTINUAR:
				if (datosCorrectos){
					state = mostrardatos;
					if (modo){
						Periodo = -1;
						Tiempo = -1;
					} else {
						Periodo = infoRiego.Periodo;
						Tiempo = -1;
					}
					ticksMin = 0;
					numeroMensaje = 0;
					TMEF = 1;
					break;
				}

		}
	}
	CastearInt();
	datosCorrectos = Comprobar();
	salidaConfirmacionDatos();
}

void fConfiguracionHumedad(){
	TMEF = -1;
	if (entradaPresiono){
		if (entradaNumero){
			ActualizarEntrada(modo);
		} else {
			switch(entradaKey){

				case CANCELAR:
					numeroMensaje = 0;
					state = seleccionmodo;
					limpiarInformacion();
					TMEF = 1;
					ticksMef=0;
					break;

				case CONTINUAR:
					if (datosIngresados == 0){
						datosIngresados = 1;
						digito = 0;
					} else {
						state = confirmaciondatos;
						TMEF = 1;
						ticksMef=0;
					}
					break;

				case VOLVER:
					if (datosIngresados != 0){
						datosIngresados--;
						digito = 0;
					}
					break;

				default: break;
			}
		}
	}
	salidaConfiguracionHumedad();
}

void fConfiguracionRiego(){
	TMEF = -1;
	if (entradaPresiono){
		if (entradaNumero){
			ActualizarEntrada();
		} else {
			switch(entradaKey){

				case CANCELAR:
					numeroMensaje = 0;
					state = seleccionmodo;
					limpiarInformacion();
					TMEF = 1;
					ticksMef=0;
					break;

				case CONTINUAR:
					if (datosIngresados == 0){
						datosIngresados = 1;
						digito = 0;
					}else{
						state = confirmaciondatos;
						TMEF = 1;
						ticksMef=0;
					}
					break;

				case VOLVER:
					datosIngresados = 0;
					digito = 0;
					break;

				default: break;
			}
		}
	}
	salidaConfiguracionRiego();
}

void fMostrar_datos(){
	TMEF = 50;
	ticksMef=0;
	actualizarDia();
	medirHumedad();
	datos.sigRiego = Periodo;

	salidaMostrarDatos();
	numeroMensaje++;

	if (numeroMensaje > 2){
		numeroMensaje = 0;
	} else {
		if (numeroMensaje == 2 && modo == 1){
			numeroMensaje = 0;
		}
	}

	if (entradaPresiono){
		if (entradaKey == T_SELECCION_MODO){
			numeroMensaje = 0;
			state = seleccionmodo;
			limpiarInformacion();
			TMEF = 1;
			ticksMef=0;
		}
	} else {
		if (modo) {
			if (hayQueRegar()){
				iniciarRiego();
			}
		} else {
			if (Periodo == -1) {
				iniciarRiego();
			}
		}
	}
}

void iniciarRiego(){
	state = regando;
	tecladoHabilitado = 0;
	datosIngresados = 0;
	TMEF = 1;
	ticksMef = 0;
	if (modo){
		Tiempo = TIEMPO_RIEGO;
	} else {
		Tiempo = infoRiego.Tiempo;
	}
}

void fRegando(){
	TMEF = 10;
	salidaRegando();
	if (modo){
		tiempoRegando++;
		sensarHumedad();
	} else {
		regar = 1;
	}
	if (regar) {
		if (Tiempo == -1){
			if (modo){
				Tiempo = TIEMPO_RIEGO;
			} else {
				dejarDeRegar();
			}
		} else {
			prenderBomba();
		}
	} else {
		dejarDeRegar();
	}
}

void dejarDeRegar(){
	apagarBomba();
	state = mostrardatos;
	datos.cantLitros = datos.cantLitros + cantLitrosPorRiego();
	datosIngresados = 1;
	tecladoHabilitado = 1;
	numeroMensaje = 0;
	TMEF = 1;
	ticksMef=0;
	if (modo) {
		Periodo = PERIODO_SENSADO;
	} else {
		Periodo = infoRiego.Periodo;
	}
	ticksMin = 0;
}

/*Implementación de funciones generales */
void sensarHumedad(){
	medirHumedad();

	if (datos.humedadActual < infoHumedad.UmbralMinimo) {
		regar = 1;
	} else {
		if (datos.humedadActual < infoHumedad.UmbralMaximo) {
			regar = 1;
		} else {
			regar = 0;
		}
	}
}

unsigned char hayQueRegar(){
	medirHumedad();
	if (datos.humedadActual < infoHumedad.UmbralMinimo)
		return 1;
	else
		return 0;
}

void salidaRegando(){
	limpiarSalida();
	salidaString.Linea1[0] = 'R';
	salidaString.Linea1[1] = 'E';
	salidaString.Linea1[2] = 'G';
	salidaString.Linea1[3] = 'A';
	salidaString.Linea1[4] = 'N';
	salidaString.Linea1[5] = 'D';
	salidaString.Linea1[6] = 'O';
	salidaString.Linea1[15] = '\0';

	salidaString.Linea2[0] = 'H';
	salidaString.Linea2[1] = 'U';
	salidaString.Linea2[2] = 'M';
	salidaString.Linea2[3] = 'E';
	salidaString.Linea2[4] = 'D';
	salidaString.Linea2[5] = 'A';
	salidaString.Linea2[6] = 'D';
	salidaString.Linea2[8] = ((int)(datos.humedadActual)/10) % 10 + 48;
	salidaString.Linea2[9] = (datos.humedadActual % 10) + 48;
	salidaString.Linea2[10] = '%';
	salidaString.Linea2[11] = '\0';


}

void salidaMostrarDatos(){
	limpiarSalida();
	if (numeroMensaje == 0){
		salidaString.Linea1[0] = 'V';

		salidaString.Linea1[2] = 'D';
		salidaString.Linea1[3] = 'E';

		salidaString.Linea1[5] = 'H';
		salidaString.Linea1[6] = 'U';
		salidaString.Linea1[7] = 'M';
		salidaString.Linea1[8] = 'E';
		salidaString.Linea1[9] = 'D';
		salidaString.Linea1[10] = 'A';
		salidaString.Linea1[11] = 'D';
		salidaString.Linea1[15] = '\0';

		salidaString.Linea2[0] = ((int)(datos.humedadActual)/10) % 10 + 48;
		salidaString.Linea2[1] = (datos.humedadActual % 10) + 48;
		salidaString.Linea2[2] = '%';
		salidaString.Linea2[15] = '\0';
	}

	if (numeroMensaje == 1){
		salidaString.Linea1[0] = 'C';
		salidaString.Linea1[1] = 'A';
		salidaString.Linea1[2] = 'N';
		salidaString.Linea1[3] = 'T';

		salidaString.Linea1[5] = 'L';
		salidaString.Linea1[6] = 'I';
		salidaString.Linea1[7] = 'T';
		salidaString.Linea1[8] = 'R';
		salidaString.Linea1[9] = 'O';
		salidaString.Linea1[10] = 'S';

		salidaString.Linea1[12] = 'D';
		salidaString.Linea1[13] = 'I';
		salidaString.Linea1[14] = 'A';

		salidaString.Linea1[15] = '\0';

		salidaString.Linea2[0] =((int)(datos.cantLitros)/10) % 10 + 48;
		salidaString.Linea2[1] = ((int)datos.cantLitros % 10) + 48;
		salidaString.Linea2[2] = '.';
		salidaString.Linea2[3] = (int)(datos.cantLitros * 10) % 10 + 48;
		salidaString.Linea2[4] = (int)(datos.cantLitros * 100) % 10 + 48;
		salidaString.Linea2[5] = 'L';
		salidaString.Linea2[15] = '\0';
	}

	if (numeroMensaje == 2){
		salidaString.Linea1[0] = 'S';
		salidaString.Linea1[1] = 'I';
		salidaString.Linea1[2] = 'G';

		salidaString.Linea1[4] = 'R';
		salidaString.Linea1[5] = 'I';
		salidaString.Linea1[6] = 'E';
		salidaString.Linea1[7] = 'G';
		salidaString.Linea1[8] = 'O';

		salidaString.Linea1[10] = 'E';
		salidaString.Linea1[11] = 'N';
		salidaString.Linea1[15] = '\0';

		salidaString.Linea2[0] = ((int)(datos.sigRiego)/10) % 10 + 48;
		salidaString.Linea2[1] = (datos.sigRiego % 10) + 48;
		salidaString.Linea2[3] = 'M';
		salidaString.Linea2[4] = 'I';
		salidaString.Linea2[5] = 'N';
		salidaString.Linea2[15] = '\0';

	}

}

void salidaConfirmacionDatos(){
	limpiarSalida();
	if (datosCorrectos){
		if (!modo){
			salidaString.Linea1[0] = 'P';
			salidaString.Linea1[1] = 'E';
			salidaString.Linea1[2] = 'R';
			salidaString.Linea1[3] = 'I';
			salidaString.Linea1[4] = 'O';
			salidaString.Linea1[5] = 'D';
			salidaString.Linea1[6] = 'O';

			salidaString.Linea1[8] = infoRiegoChar.Periodo[0];
			salidaString.Linea1[9] = infoRiegoChar.Periodo[1];

			salidaString.Linea1[11] = 'M';
			salidaString.Linea1[12] = 'I';
			salidaString.Linea1[13] = 'N';
			salidaString.Linea1[15] = '\0';

			salidaString.Linea2[0] = 'T';
			salidaString.Linea2[1] = 'I';
			salidaString.Linea2[2] = 'E';
			salidaString.Linea2[3] = 'M';
			salidaString.Linea2[4] = 'P';
			salidaString.Linea2[5] = 'O';

			salidaString.Linea2[7] = infoRiegoChar.Tiempo[0];
			salidaString.Linea2[8] = infoRiegoChar.Tiempo[1];

			salidaString.Linea2[10] = 'S';
			salidaString.Linea2[11] = 'E';
			salidaString.Linea2[12] = 'G';
			salidaString.Linea2[15] = '\0';
		}else{
			salidaString.Linea1[0] = 'M';
			salidaString.Linea1[1] = 'I';
			salidaString.Linea1[2] = 'N';

			salidaString.Linea1[4] = infoHumedadChar.UmbralMinimo[0];
			salidaString.Linea1[5] = infoHumedadChar.UmbralMinimo[1];
			salidaString.Linea1[6] = '%';
			salidaString.Linea1[15] = '\0';

			salidaString.Linea2[0] = 'M';
			salidaString.Linea2[1] = 'A';
			salidaString.Linea2[2] = 'X';

			salidaString.Linea2[4] = infoHumedadChar.UmbralMaximo[0];
			salidaString.Linea2[5] = infoHumedadChar.UmbralMaximo[1];
			salidaString.Linea2[6] = '%';
			salidaString.Linea2[15] = '\0';
		}

	}else{
		salidaString.Linea1[0] = 'L';
		salidaString.Linea1[1] = '0';
		salidaString.Linea1[2] = 'S';

		salidaString.Linea1[4] = 'D';
		salidaString.Linea1[5] = 'A';
		salidaString.Linea1[6] = 'T';
		salidaString.Linea1[7] = 'O';
		salidaString.Linea1[8] = 'S';

		salidaString.Linea1[10] = 'N';
		salidaString.Linea1[11] = 'O';
		salidaString.Linea1[15] = '\0';

		salidaString.Linea2[0] = 'S';
		salidaString.Linea2[1] = 'O';
		salidaString.Linea2[2] = 'N';

		salidaString.Linea2[4] = 'V';
		salidaString.Linea2[5] = 'A';
		salidaString.Linea2[6] = 'L';
		salidaString.Linea2[7] = 'I';
		salidaString.Linea2[8] = 'D';
		salidaString.Linea2[9] = 'O';
		salidaString.Linea2[10] = 'S';
		salidaString.Linea1[15] = '\0';
	}
}

unsigned char Comprobar(){
	if (modo){
		if ((infoHumedad.UmbralMaximo != 0) && (infoHumedad.UmbralMinimo != 0)){
			if (infoHumedad.UmbralMaximo > infoHumedad.UmbralMinimo){
				return 1;
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	} else {
		if ((infoRiego.Tiempo != 0) && (infoRiego.Periodo !=0)){
			if (infoRiego.Tiempo < (infoRiego.Periodo * 60)){
				return 1;
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}
}

void CastearInt(){
	int aux1, aux2;
	if (modo){
		aux1 = infoHumedadChar.UmbralMaximo[0]-48;
		aux2 = infoHumedadChar.UmbralMaximo[1]-48;
		infoHumedad.UmbralMaximo = (aux1*10) + aux2;
		aux1 = infoHumedadChar.UmbralMinimo[0]-48;
		aux2 = infoHumedadChar.UmbralMinimo[1]-48;
		infoHumedad.UmbralMinimo = (aux1*10) + aux2;
	}else{
		aux1= infoRiegoChar.Tiempo[0] - 48;
		aux2 = infoRiegoChar.Tiempo[1] - 48;
		infoRiego.Tiempo = (aux1*10) + aux2;
		aux1 = infoRiegoChar.Periodo[0] - 48;
		aux2= infoRiegoChar.Periodo[1] - 48;
		infoRiego.Periodo = (aux1*10) + aux2;
	}
}

void ActualizarEntrada(){ //Tipo =0 es riego, tipo = 1 es humedad
	if (modo){
		if (datosIngresados == 0){
			infoHumedadChar.UmbralMaximo[digito] = entradaKey;
			digito++;
			if (digito > 1){
				digito = 0;
			}

		} else {
			infoHumedadChar.UmbralMinimo[digito] = entradaKey;
			digito++;
			if (digito > 1){
				digito = 0;
			}
		}
	} else {
		if (datosIngresados == 0){
			infoRiegoChar.Tiempo[digito] = entradaKey;
			if (digito == 1) {
				digito = 0;
			} else {
				digito++;
			}

		} else {
			infoRiegoChar.Periodo[digito] = entradaKey;
			if (digito == 1) {
				digito = 0;
			} else {
				digito++;
			}
		}
	}
}

void limpiarSalida(){
	int i;
	for(i=0;i<16;i++){
		salidaString.Linea1[i] = ' ';
		salidaString.Linea2[i] = ' ';
	}
}

void limpiarInformacion(){

	infoHumedad.UmbralMaximo = 0;
	infoHumedad.UmbralMinimo = 0;
	infoRiego.Periodo = -1;
	infoRiego.Tiempo = -1;

	infoHumedadChar.UmbralMaximo[0] = '0';
	infoHumedadChar.UmbralMaximo[1] = '0';
	infoHumedadChar.UmbralMinimo[0] = '0';
	infoHumedadChar.UmbralMinimo[1] = '0';


	infoRiegoChar.Periodo[0] = '0';
	infoRiegoChar.Periodo[1] = '0';
	infoRiegoChar.Tiempo[0] = '0';
	infoRiegoChar.Tiempo[1] = '0';

	datos.cantLitros = 0;
	datos.humedadActual = 0;
}

void salidaSeleccionModo(int numeroMensaje){
	limpiarSalida();
	salidaString.Linea1[0] = 'P';
	salidaString.Linea1[1] = 'R';
	salidaString.Linea1[2] = 'E';
	salidaString.Linea1[3] = 'S';
	salidaString.Linea1[4] = 'I';
	salidaString.Linea1[5] = 'O';
	salidaString.Linea1[6] = 'N';
	salidaString.Linea1[7] = 'E';

	salidaString.Linea1[11] = 'P';
	salidaString.Linea1[12] = 'A';
	salidaString.Linea1[13] = 'R';
 	salidaString.Linea1[14] = 'A';
 	salidaString.Linea1[15] = '\0';

	salidaString.Linea2[3] = 'M';
	salidaString.Linea2[4] = 'O';
	salidaString.Linea2[5] = 'D';
	salidaString.Linea2[6] = 'O';

	if (numeroMensaje){
		salidaString.Linea1[9] = 'A';

		salidaString.Linea2[8] = 'T';
		salidaString.Linea2[9] = 'I';
		salidaString.Linea2[10] = 'M';
		salidaString.Linea2[11] = 'E';
		salidaString.Linea2[12] = 'R';
		salidaString.Linea2[15] = '\0';
	} else {
		salidaString.Linea1[9] = 'B'
		;
		salidaString.Linea2[8] = 'H';
		salidaString.Linea2[9] = 'U';
		salidaString.Linea2[10] = 'M';
		salidaString.Linea2[11] = 'E';
		salidaString.Linea2[12] = 'D';
		salidaString.Linea2[13] = 'A';
		salidaString.Linea2[14] = 'D';
		salidaString.Linea2[15] = '\0';
	}
}

void salidaConfiguracionRiego(){
	limpiarSalida();
	if (datosIngresados == 0){
		salidaString.Linea1[0] = 'R';
		salidaString.Linea1[1] = 'I';
		salidaString.Linea1[2] = 'E';
		salidaString.Linea1[3] = 'G';
		salidaString.Linea1[4] = '0';

		salidaString.Linea1[6] = 'E';
		salidaString.Linea1[7] = 'N';

		salidaString.Linea1[9] = 'S';
		salidaString.Linea1[10] = 'E';
		salidaString.Linea1[11] = 'G';
		salidaString.Linea1[15] = '\0';

		salidaString.Linea2[0] = infoRiegoChar.Tiempo[0];
		salidaString.Linea2[1] = infoRiegoChar.Tiempo[1];
		salidaString.Linea2[15] = '\0';

	}else{
		salidaString.Linea1[0] = 'P';
		salidaString.Linea1[1] = 'E';
		salidaString.Linea1[2] = 'R';
		salidaString.Linea1[3] = 'I';
		salidaString.Linea1[4] = 'O';
		salidaString.Linea1[5] = 'D';
		salidaString.Linea1[6] = 'O';

		salidaString.Linea1[8] = 'E';
		salidaString.Linea1[9] = 'N';

		salidaString.Linea1[11] = 'M';
		salidaString.Linea1[12] = 'I';
		salidaString.Linea1[13] = 'N';
		salidaString.Linea1[15] = '\0';

		salidaString.Linea2[0] = infoRiegoChar.Periodo[0];
		salidaString.Linea2[1] = infoRiegoChar.Periodo[1];
		salidaString.Linea2[15] = '\0';


	}
}

void salidaConfiguracionHumedad(){
	limpiarSalida();
	salidaString.Linea1[0] = 'U';
	salidaString.Linea1[1] = 'M';
	salidaString.Linea1[2] = 'B';
	salidaString.Linea1[3] = 'R';
	salidaString.Linea1[4] = 'A';
	salidaString.Linea1[5] = 'L';
	if (datosIngresados == 0){
		salidaString.Linea1[7] = 'M';
		salidaString.Linea1[8] = 'A';
		salidaString.Linea1[9] = 'X';
		salidaString.Linea1[10] = 'I';
		salidaString.Linea1[11] = 'M';
		salidaString.Linea1[12] = 'O';
		salidaString.Linea1[15] = '\0';

		salidaString.Linea2[0]=infoHumedadChar.UmbralMaximo[0];
		salidaString.Linea2[1]=infoHumedadChar.UmbralMaximo[1];
		salidaString.Linea2[2]='%';
		salidaString.Linea2[15] = '\0';
	}else{
		salidaString.Linea1[7] = 'M';
		salidaString.Linea1[8] = 'I';
		salidaString.Linea1[9] = 'N';
		salidaString.Linea1[10] = 'I';
		salidaString.Linea1[11] = 'M';
		salidaString.Linea1[12] = 'O';
		salidaString.Linea1[15] = '\0';

		salidaString.Linea2[0]=infoHumedadChar.UmbralMinimo[0];
		salidaString.Linea2[1]=infoHumedadChar.UmbralMinimo[1];
		salidaString.Linea2[2]='%';
		salidaString.Linea2[15] = '\0';

	}
}

void mostrarSalidaLCD(){
	LCD_pos_xy(0,0);
	LCD_write_string(salidaString.Linea1);
	LCD_pos_xy(0,1);
	LCD_write_string(salidaString.Linea2);
}

void prenderBomba(){
	PinSetMEF(BOMBA_GPIO_PORT,BOMBA_GPIO_PIN,1);
}

void apagarBomba(){
	PinSetMEF(BOMBA_GPIO_PORT,BOMBA_GPIO_PIN,0);
}

void PinSetMEF(unsigned char PORT, unsigned char PIN, unsigned char valor){
   Chip_GPIO_SetPinState( LPC_GPIO_PORT, PORT, PIN, valor);
}

void medirHumedad(){

	unsigned short valorHumedad;
	Chip_ADC_SetStartMode(LPC_ADC1, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
	while (Chip_ADC_ReadStatus(LPC_ADC1,ADC_CH1,ADC_DR_DONE_STAT) != SET) {}
	Chip_ADC_ReadValue(LPC_ADC1,ADC_CH1, &valorHumedad);

	if (valorHumedad < 137) valorHumedad = 137;
	else if (valorHumedad > 1017) valorHumedad = 1017;

	datos.humedadActual = (-0.00114 * valorHumedad + 1.16) * 100;
}

void fBienvenida(){
	TMEF=30;
	limpiarSalida();
	salidaString.Linea1[2] = 'B';
	salidaString.Linea1[3] = 'I';
	salidaString.Linea1[4] = 'E';
	salidaString.Linea1[5] = 'N';
	salidaString.Linea1[6] = 'V';
	salidaString.Linea1[7] = 'E';
	salidaString.Linea1[8] = 'N';
	salidaString.Linea1[9] = 'I';
	salidaString.Linea1[10] = 'D';
	salidaString.Linea1[11] = 'O';

	salidaString.Linea1[13] = 'A';
	salidaString.Linea1[14] = '\0';

	salidaString.Linea2[6] = 'S';
	salidaString.Linea2[7] = 'D';
	salidaString.Linea2[8] = 'R';
	salidaString.Linea2[9] = 'A';
	salidaString.Linea2[10] = '\0';
	state=seleccionmodo;
}

float cantLitrosPorRiego(){
	if (modo){
		float aux = ( 0.0245 * tiempoRegando - 0.0217 );
		tiempoRegando = 0;
		return aux;
	} else {
		return ( 0.0245 * infoRiego.Tiempo - 0.0217);
	}
}

void actualizarDia(){
	if (Dia == -1){
		Dia = MS_X_DIA;
		datos.cantLitros = 0;
	}
}

