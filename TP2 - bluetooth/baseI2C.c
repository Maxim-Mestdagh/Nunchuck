#include "Driver_I2C.h"                 // ::CMSIS Driver:I2C
#include "stdio.h"
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"
#include "GPIO.h" 
#include "LPC17xx.h"
#include "Driver_USART.h"
#include "stdlib.h"

#define SLAVE_I2C_ADDR       0xXX			// Adresse esclave sur 7 bits

extern ARM_DRIVER_I2C Driver_I2C0;
extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;
extern ARM_DRIVER_USART Driver_USART1;
int stringToInt(char *tab,int taille);

uint8_t DeviceAddr;
char accX[2],accY[2], retour[6], tab[10], maValeur, chaine[20], chaine1[20];
short acX, acY, posX = 0, posY = 0, vitX = 0, vitY = 0;

void write1byte(unsigned char composant, unsigned char registre, unsigned char valeur);
unsigned char read1byte(unsigned char composant, unsigned char registre);
void Timer0_Init(unsigned int prescaler,unsigned int valeur);

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
							115200);
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
	char chaine[20];
	int i;
	char roll[50], pitch[50],joystick[50];
	char test;
	Init_I2C();
	Init_UART();
	GLCD_Initialize();
  GLCD_ClearScreen();
  GLCD_SetFont(&GLCD_Font_16x24);

	
	DeviceAddr = 0x52;
	write1byte(DeviceAddr,0xf0,0x55); //init 1er registre
	write1byte(DeviceAddr,0xfb,0x00);	//init 2e registre

	
	while (1){
		char InitCmd=0;

		while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
		Driver_USART1.Receive(&test,1);
		
	// Ecriture vers registre esclave : START + ADDR(W) + 1W_DATA + 1W_DATA + STOP
	
	while(Driver_USART1.GetRxCount()<1); 
		
//		if(test=='R')
//		{
//			GetBTString(roll,'*');
//			sprintf(chaine,"gdrd %d",atoi(roll));
//			GLCD_DrawString(1,120,chaine);
//		}
//		
//		if(test=='P')
//		{
//			GetBTString(pitch,'*');
//	  	sprintf(chaine,"yuuu %d",atoi(pitch));
//			GLCD_DrawString(1,150,chaine);
//		}
		
		if(test=='X')
		{
			GetBTString(joystick,'*');
	  	//sprintf(chaine," %d",atoi(joystick));
			GLCD_DrawString(1,150,joystick);
		}
		
		if(test=='A')
		{
			GLCD_DrawString(1,180,"Btn A : pressed");
		}
		
		else if(test=='a')
		{
	  	GLCD_DrawString(1,180,"Btn A : release");
		}
		
		if(test=='B')
		{
			GLCD_DrawString(1,210,"Btn B : pressed");
		}
		
		else if(test=='b')
		{
	  	GLCD_DrawString(1,210,"Btn B : release");
		}
		
		if(test=='1')
		{
			GLCD_DrawString(1,150,"Direction : Up");
		}
		else if(test=='2')
		{
			GLCD_DrawString(1,150,"Direction : Ri");
		}
		else if(test=='3')
		{
			GLCD_DrawString(1,150,"Direction : Do");
		}
		else if(test=='4')
		{
			GLCD_DrawString(1,150,"Direction : Le");
		}
	
		Driver_I2C0.MasterTransmit (DeviceAddr, &InitCmd, 1, false);// commande de conversion
		while (Driver_I2C0.GetStatus().busy == 1); // attente fin transmission
	
		Driver_I2C0.MasterReceive (0x52 ,retour, 6, false);		// recupération des data false = avec stop 
		while (Driver_I2C0.GetStatus().busy == 1);		// attente fin transmission
		sprintf(chaine,"Joystick X = %04d", retour[0] ); 
		GLCD_DrawString(1,1,(unsigned char*)chaine);// affichage nunchuk
		sprintf(chaine,"Joystick Y = %04d",  retour[1]); 
		GLCD_DrawString(1,24,(unsigned char*)chaine);
		if ((retour[5]&0x01)==0x00)
		{
				GLCD_DrawString(1,72,"Btn z : pressed");
		}
		else
		{
				GLCD_DrawString(1,72,"Btn z : release");
		}
		if ((retour[5]&0x02)==0x00)
		{
				GLCD_DrawString(1,96,"Btn c : pressed");
		}
		else
		{
				GLCD_DrawString(1,96,"Btn c : release");
		}
	}
	
	return 0;
}
void write1byte(unsigned char composant, unsigned char registre, unsigned char valeur)
{
	char tab[2];
		
	tab[0] = registre;
	tab[1] = valeur;
	
	Driver_I2C0.MasterTransmit (composant, tab, 2, false);// ecriture
	while (Driver_I2C0.GetStatus().busy == 1);// attente fin transmission
}
unsigned char read1byte(unsigned char composant, unsigned char registre)
{
	char retour;
	
	Driver_I2C0.MasterTransmit (composant, &registre, 1, false);// ecriture
	while (Driver_I2C0.GetStatus().busy == 1);// attente fin transmission
	
	Driver_I2C0.MasterReceive (composant, &retour, 1, false);		// lecture false = avec stop
	while (Driver_I2C0.GetStatus().busy == 1);	// attente fin transmission
	
	return retour;
	}

	