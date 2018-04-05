/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "cmsis_os.h"                       // CMSIS RTOS header file
#include "Driver_I2C.h"                 // ::CMSIS Driver:I2C
#include "stdio.h"
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"
#include "GPIO.h" 
#include "LPC17xx.h"
#include "Driver_USART.h"
#include "stdlib.h"

#define SLAVE_I2C_ADDR       0xXX			// Adresse esclave sur 7 bits

void Thread1 (void const *argument);                             // thread function
osThreadId tid_Thread1;                                          // thread id
osThreadDef (Thread1, osPriorityNormal, 1, 0);                   // thread object
void Thread2 (void const *argument);                             // thread function
osThreadId tid_Thread2;                                          // thread id
osThreadDef (Thread2, osPriorityNormal, 1, 0);                   // thread object

osMutexId mid_Thread_Mutex;                                     // mutex id
osMutexDef (SampleMutex);                                       // mutex name definition

extern ARM_DRIVER_I2C Driver_I2C0;
extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;
extern ARM_DRIVER_USART Driver_USART1;
int stringToInt(char *tab,int taille);

uint8_t DeviceAddr;
char retour[6], tab[10], maValeur, chaine[20], chaine1[20];


void write1byte(unsigned char composant, unsigned char registre, unsigned char valeur);
void sendCommand(char * command, int tempo_ms);

void Init_I2C(void){   //initialisation de la transmission I2C
	Driver_I2C0.Initialize(NULL);
	Driver_I2C0.PowerControl(ARM_POWER_FULL);
	Driver_I2C0.Control(	ARM_I2C_BUS_SPEED,				// 2nd argument = débit
							ARM_I2C_BUS_SPEED_STANDARD  );	// 100 kHz
	Driver_I2C0.Control(	ARM_I2C_BUS_CLEAR,
							0 );
}

void Init_UART(void){
	Driver_USART1.Initialize(NULL);
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	Driver_USART1.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							38400);
	Driver_USART1.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART1.Control(ARM_USART_CONTROL_RX,1);
}

void GetBTString(char *ptab,char delimFin)
{
	char RxBt;
	int i=0;
	

	//On remplit le tableau avec les caractères qui suivent...
	do
	{
			while(Driver_USART1.GetRxCount()<1); //Attente et réception d'un caractère
		Driver_USART1.Receive(&RxBt,1);


		//Si on vient de recevoir de délimiteur de FIN  ...
		if(RxBt==delimFin)
		ptab[i]=NULL;	            //... on place le caractère NULL dans le tableau => on "cloture" la chaine de caractères...
		else
		ptab[i]=RxBt;            //... sinon on copie le caractère reçu dans le tableau

		i++;//on fait évoluer l'index du tableau

	}while(     RxBt!=delimFin        );//... Tant que le caractère reçu n'est pas le délimiteur de FIN
}


int main (void){
	osKernelInitialize ();
	
	Init_I2C();
	Init_UART();
	GLCD_Initialize();
  GLCD_ClearScreen();
  GLCD_SetFont(&GLCD_Font_16x24);
	
	DeviceAddr = 0x52;
	write1byte(DeviceAddr,0xf0,0x55); //init 1er registre
	write1byte(DeviceAddr,0xfb,0x00);	//init 2e registre

	tid_Thread1 = osThreadCreate (osThread(Thread1), NULL);
	//tid_Thread2 = osThreadCreate (osThread(Thread2), NULL);
	
	mid_Thread_Mutex = osMutexCreate (osMutex (SampleMutex));
	
	osKernelStart ();
}
	
void Thread1 (void const *argument) {

	char InitCmd=0;
		
  while (1) {
    Driver_I2C0.MasterTransmit (DeviceAddr, &InitCmd, 1, false);// commande de conversion
		while (Driver_I2C0.GetStatus().busy == 1); // attente fin transmission
	
		Driver_I2C0.MasterReceive (0x52 ,retour, 6, false);		// recupération des data false = avec stop 
		while (Driver_I2C0.GetStatus().busy == 1);		// attente fin transmission
//		//Affichage joystick axe X
//		osMutexWait (mid_Thread_Mutex, osWaitForever);
//		sprintf(chaine,"Joystick X = %03d", retour[0] ); 
//		GLCD_DrawString(1,1,(unsigned char*)chaine);// affichage nunchuk
//		sprintf(chaine,"X%03d", retour[0] );
//		Driver_USART1.Send(chaine,4);
//		
//		//Affichage joystick axe Y
//		sprintf(chaine,"Joystick Y = %03d",  retour[1]); 
//		GLCD_DrawString(1,24,(unsigned char*)chaine);
//		sprintf(chaine,"Y%03d", retour[1] );
//		Driver_USART1.Send(chaine,4);
//		osMutexRelease (mid_Thread_Mutex);
		
		if ((retour[5]&0x01)==0x00)
		{
				osMutexWait (mid_Thread_Mutex, osWaitForever);
				GLCD_DrawString(1,72,"Btn z : pressed");	
				while (Driver_I2C0.GetStatus().busy == 1);		// attente fin transmission
				Driver_USART1.Send("Z1",2);					//envoi UART vers module BT maitre
				osMutexRelease (mid_Thread_Mutex);
		}
		else
		{
				osMutexWait (mid_Thread_Mutex, osWaitForever);
				GLCD_DrawString(1,72,"Btn z : release");
				while (Driver_I2C0.GetStatus().busy == 1);		// attente fin transmission
				Driver_USART1.Send("Z0",2);					//envoi UART vers module BT maitre
				osMutexRelease (mid_Thread_Mutex);
		}
		if ((retour[5]&0x02)==0x00)
		{
				osMutexWait (mid_Thread_Mutex, osWaitForever);
				GLCD_DrawString(1,96,"Btn c : pressed");
				Driver_USART1.Send("C1",2);					//envoi UART vers module BT maitre
				osMutexRelease (mid_Thread_Mutex);
		}
		else
		{
				osMutexWait (mid_Thread_Mutex, osWaitForever);
				GLCD_DrawString(1,96,"Btn c : release");
				Driver_USART1.Send("C0",2);					//envoi UART vers module BT maitre
				osMutexRelease (mid_Thread_Mutex);
		}
	}
}
	
void Thread2 (void const *argument) {

	char chaine[20];
	char roll[50], pitch[50],joystick[50];
	char test;
	
  while (1) {
    while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
		Driver_USART1.Receive(&test,1);
		
	// Ecriture vers registre esclave : START + ADDR(W) + 1W_DATA + 1W_DATA + STOP
	
	while(Driver_USART1.GetRxCount()<1); 
		
		if(test=='A')
		{
			osMutexWait (mid_Thread_Mutex, osWaitForever);
			GLCD_DrawString(1,180,"Btn A : pressed");
			Driver_USART1.Send("*LR0G0B0",9);
			osMutexRelease (mid_Thread_Mutex);
		}
		
		else if(test=='a')
		{
			osMutexWait (mid_Thread_Mutex, osWaitForever);
	  	GLCD_DrawString(1,180,"Btn A : release");
			Driver_USART1.Send("*LR255G0B0",12);
			osMutexRelease (mid_Thread_Mutex);
		}
		
		if(test=='B')
		{
			osMutexWait (mid_Thread_Mutex, osWaitForever);
			GLCD_DrawString(1,210,"Btn B : pressed");
			Driver_USART1.Send("*LR0G0B0",12);
			osMutexRelease (mid_Thread_Mutex);
		}
		
		else if(test=='b')
		{
			osMutexWait (mid_Thread_Mutex, osWaitForever);
	  	GLCD_DrawString(1,210,"Btn B : release");
			Driver_USART1.Send("*LR255G255B0",12);
			osMutexRelease (mid_Thread_Mutex);
		}
		
		if(test=='1')
		{
			osMutexWait (mid_Thread_Mutex, osWaitForever);
			GLCD_DrawString(1,150,"Direction : Up");
			osMutexRelease (mid_Thread_Mutex);
		}
		else if(test=='2')
		{
			osMutexWait (mid_Thread_Mutex, osWaitForever);
			GLCD_DrawString(1,150,"Direction : Ri");
			osMutexRelease (mid_Thread_Mutex);
		}
		else if(test=='3')
		{
			osMutexWait (mid_Thread_Mutex, osWaitForever);
			GLCD_DrawString(1,150,"Direction : Do");
			osMutexRelease (mid_Thread_Mutex);
		}
		else if(test=='4')
		{
			osMutexWait (mid_Thread_Mutex, osWaitForever);
			GLCD_DrawString(1,150,"Direction : Le");
			osMutexRelease (mid_Thread_Mutex);
		}
	
	}	
}

void write1byte(unsigned char composant, unsigned char registre, unsigned char valeur)
{
	char tab[2];
		
	tab[0] = registre;
	tab[1] = valeur;
	
	Driver_I2C0.MasterTransmit (composant, tab, 2, false);// ecriture
	while (Driver_I2C0.GetStatus().busy == 1);// attente fin transmission
}

void sendCommand(char * command, int tempo_ms)
{
	int len;
	len = strlen (command);
	Driver_USART1.Send(command,len); // send the read character to the esp8266
	osSignalWait(0x02, osWaitForever);		// sommeil fin emission
	osDelay(tempo_ms);		// attente traitement retour
}
	