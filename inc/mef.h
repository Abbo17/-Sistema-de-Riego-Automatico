#ifndef _MEF_H
#define _MEF_H


/* Definimos las teclas */
#define T_ENCENDIDO 'A'
#define CANCELAR 'D'
#define CONTINUAR 'C'
#define VOLVER 'A'
#define T_SELECCIONAR_MODO_RIEGO 'A'
#define T_SELECCIONAR_MODO_HUMEDAD 'B'
#define T_SELECCION_MODO 'D'

#define PERIODO_SENSADO 1
#define TIEMPO_RIEGO	1

#define MS_X_DIA 1728000

/*Pin de la bomba */
#define BOMBA_SCU_PORT 0
#define BOMBA_SCU_PIN 0
#define BOMBA_GPIO_PORT 0
#define BOMBA_GPIO_PIN 0

#define ENTRADA 0
#define SALIDA 1

/* Definimos los estados */
typedef enum {apagado, bienvenida, seleccionmodo, configuracionriego, configuracionhumedad, confirmaciondatos, mostrardatos, regando} statename;
/* Definici�n de tipos */

struct struct_Salida{
	 char Linea1[17];
	 char Linea2[17];
};

struct struct_infoRiego{
	 int Periodo;
	 int Tiempo;
};

struct struct_infoHumedad{
	int UmbralMaximo;
	int UmbralMinimo;
};

struct struct_infoRiegoChar{
	unsigned char Periodo[2];
	unsigned char Tiempo[2];
};

struct struct_infoHumedadChar{
	unsigned char UmbralMaximo[3];
	unsigned char UmbralMinimo[3];
};

struct struct_datos{
	int humedadActual;
	float cantLitros;
	int sigRiego;
};
/* Definimos funciones de la Maquina de estados */

void Init_MEF(void);
void Update_MEF(void);


void fApagado (void);
void fSeleccionModo(void);
void fConfiguracionRiego(void);
void fConfiguracionHumedad(void);
void fConfirmacionDatos(void);
void fRegando(void);
void fMostrar_datos(void);
void fBienvenida(void);


/*Definicion de funciones generales */

void limpiarSalida(void);
void limpiarInformacion(void);
void mostrarSalidaLCD(void);
void salidaSeleccionModo(int numeroMensaje);
void ActualizarEntrada();
void salidaConfiguracionRiego();
void salidaConfirmacionDatos();
void salidaConfiguracionHumedad();
void CastearInt();
unsigned char Comprobar();
void salidaMostrarDatos();
unsigned char hayQueRegar();
void salidaRegando();
void dejarDeRegar();
void iniciarRiego();
void apagarBomba();
void prenderBomba();
void medirHumedad();
void sensarHumedad();
void actualizarDia();
float cantLitrosPorRiego();
void PinSetMEF(unsigned char PORT, unsigned char PIN, unsigned char valor);
#endif
