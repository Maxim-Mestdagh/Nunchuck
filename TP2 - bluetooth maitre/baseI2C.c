/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "cmsis_os.h"                       // CMSIS RTOS header file
#include "stdio.h"
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"
#include "GPIO.h" 
#include "LPC17xx.h"
#include "Driver_USART.h"
#include "stdlib.h"

void Thread2 (void const *argument);                             // thread function
osThreadId tid_Thread2;                                          // thread id
osThreadDef (Thread2, osPriorityNormal, 1, 0);                   // thread object

//void Thread_R (void const *argument);                             // thread function Receive
//osThreadId tid_Thread_R;                                          // thread id
//osThreadDef (Thread_R, osPriorityNormal, 1, 0);  

osMutexId mid_Thread_Mutex;                                     // mutex id
osMutexDef (SampleMutex);                                       // mutex name definition

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;
extern ARM_DRIVER_USART Driver_USART1;
int stringToInt(char *tab,int taille);

char retour[6], tab[10], maValeur, chaine[20], chaine1[20];

//void event_UART(uint32_t event)
//{
//	switch (event) {
//		
//		case ARM_USART_EVENT_RECEIVE_COMPLETE : 	osSignalSet(tid_Thread_R, 0x01);
//																							break;
//		
//		
//		default : break;
//	}
//}

//void Thread_R (void const *argument) {

//	char RxChar;
//	int ligne;
//	int i=0;	// i pour position colonne caractère
//	char RxBuf[200];
//	
//  while (1) {
//		Driver_USART1.Receive(&RxChar,1);		// A mettre ds boucle pour recevoir 
//		osSignalWait(0x01, osWaitForever);	// sommeil attente reception
//		
//		RxBuf[i]=RxChar;
//		i++;
//		//Suivant le caractère récupéré
//		switch(RxChar)
//		{
//			case 0x0D: 		//Un retour chariot? On ne le conserve pas...
//				i--;
//				break;
//			case 0x0A:										//Un saut de ligne?
//				RxBuf[i-1]=0;											//=> Fin de ligne, donc, on "cloture" la chaine de caractères
//				GLCD_DrawString(1,ligne,RxBuf);	//On l'affiche (peut etre trop long, donc perte des caractères suivants??)
//				ligne+=10;										//On "saute" une ligne de l'afficheur LCD
//			  if(ligne>240)
//				{
//					ligne=1;
//					GLCD_ClearScreen();
//					osDelay(2000);
//				}
//				i=0;													//On se remet au début du buffer de réception pour la prochaine ligne à recevoir
//				break;
//		}
//  }
//}

void Init_bluetooth(void)
{
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

//void GetBTString(char *ptab,char delimFin)
//{
//	char RxBt;
//	int i=0;
//	

//	//On remplit le tableau avec les caractères qui suivent...
//	do
//	{
//			while(Driver_USART1.GetRxCount()<1); //Attente et réception d'un caractère
//		Driver_USART1.Receive(&RxBt,1);


//		//Si on vient de recevoir de délimiteur de FIN  ...
//		if(RxBt==delimFin)
//		ptab[i]=NULL;	            //... on place le caractère NULL dans le tableau => on "cloture" la chaine de caractères...
//		else
//		ptab[i]=RxBt;            //... sinon on copie le caractère reçu dans le tableau

//		i++;//on fait évoluer l'index du tableau

//	}while(     RxBt!=delimFin        );//... Tant que le caractère reçu n'est pas le délimiteur de FIN
//}


int main (void){
	osKernelInitialize ();
	
	Init_UART();
	GLCD_Initialize();
  GLCD_ClearScreen();
  GLCD_SetFont(&GLCD_Font_16x24);
//	tid_Thread_R = osThreadCreate(osThread(Thread_R),NULL);
	tid_Thread2 = osThreadCreate (osThread(Thread2), NULL);
	Init_bluetooth();
	mid_Thread_Mutex = osMutexCreate (osMutex (SampleMutex));
	
	osKernelStart ();
}
	
	
void Thread2 (void const *argument) {

	char chaine[25];
	char data[25];
	
  while (1) {
   // while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
		Driver_USART1.Receive(data,10);
		while(Driver_USART1.GetRxCount()<4); 
		sprintf(chaine,"data %x %x",data[0],data[1]);
		GLCD_DrawString(1,24,(unsigned char*)chaine);
		
	// Ecriture vers registre esclave : START + ADDR(W) + 1W_DATA + 1W_DATA + STOP
	
	
		
		
		//Traitement donnees joystick axe X + Affichage
		if (data[0]=='X')
		{
			osMutexWait (mid_Thread_Mutex, osWaitForever);
			sprintf(chaine,"Joy X %01d%01d%01d", data[1],data[2],data[3] );
			GLCD_DrawString(1,12,(unsigned char*)chaine);
			osMutexRelease (mid_Thread_Mutex);
				
		}
		
		//Traitement donnees joystick axe Y + Affichage
		if (data[0]=='Y')
		{
			osMutexWait (mid_Thread_Mutex, osWaitForever);
			sprintf(chaine,"Joy Y %01d%01d%01d", data[1],data[2],data[3] );
			GLCD_DrawString(1,24,(unsigned char*)chaine);
			osMutexRelease (mid_Thread_Mutex);
				
		}
		
		
		//Traitement donnees bouton Z + Affichage
		if (data[0]=='Z')
		{
				if (data[1]=='1')
				{
					osMutexWait (mid_Thread_Mutex, osWaitForever);
					GLCD_DrawString(1,96,"Btn z : pressed");
					osMutexRelease (mid_Thread_Mutex);
				}
				else if (data[1]=='0')
				{
					osMutexWait (mid_Thread_Mutex, osWaitForever);
					GLCD_DrawString(1,96,"Btn z : released");
					osMutexRelease (mid_Thread_Mutex);
				}
		}
		
		//Traitement donnees bouton C + Affichage
		if (data[0]=='C')
		{
				if (data[1]=='1')
				{
					osMutexWait (mid_Thread_Mutex, osWaitForever);
					GLCD_DrawString(1,120,"Btn c : pressed");
					osMutexRelease (mid_Thread_Mutex);
				}
				else if (data[1]=='0')
				{
					osMutexWait (mid_Thread_Mutex, osWaitForever);
					GLCD_DrawString(1,120,"Btn c : released");
					osMutexRelease (mid_Thread_Mutex);
				}
		}
	}	
}

	