#include "Driver_I2C.h"                 // ::CMSIS Driver:I2C
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "LPC17xx.h"                    // Device header
#include <stdio.h>

#define SLAVE_I2C_ADDR       0xa4
#define SLAVE_I2C_ADDR2				0xa5			// Adresse esclave sur 7 bits

extern ARM_DRIVER_I2C Driver_I2C0;
extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;


uint8_t DeviceAddr;

void Init_I2C(void){
	Driver_I2C0.Initialize(NULL);
	Driver_I2C0.PowerControl(ARM_POWER_FULL);
	Driver_I2C0.Control(	ARM_I2C_BUS_SPEED,				// 2nd argument = débit
							ARM_I2C_BUS_SPEED_STANDARD  );	// 100 kHz
	Driver_I2C0.Control(	ARM_I2C_BUS_CLEAR,
							0 );
}
void Init_Nunchuck (void)
{
	char tab[2];
	tab[0]=0xF0;
	tab[1]=0x55;
	Driver_I2C0.MasterTransmit (SLAVE_I2C_ADDR, tab, 2, false);		// false = avec stop
	while (Driver_I2C0.GetStatus().busy == 1);
	tab[0]=0xFB;
	tab[1]=0x00;
	Driver_I2C0.MasterTransmit (SLAVE_I2C_ADDR, tab, 2, false);		// false = avec stop
	while (Driver_I2C0.GetStatus().busy == 1);
	
	
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
int main (void){
	char chaine[30];
	char maValeur[6];
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetBackgroundColor(GLCD_COLOR_WHITE);
	GLCD_SetFont(&GLCD_Font_16x24);
	//GLCD_SetForegroundColor(GLCD_COLOR_BLUE);
	Init_I2C();
	Init_Nunchuck();
	
	
	
	
	while (1)
	{
		
		char tab[1],sub0[1]={0x00};
		tab[0]=0x00;
		
		Driver_I2C0.MasterTransmit (SLAVE_I2C_ADDR, tab, 1, false);		// conversion command 
		while (Driver_I2C0.GetStatus().busy == 1);
		
		Driver_I2C0.MasterTransmit (SLAVE_I2C_ADDR, sub0, 1, true);		//lire registre 0x00 joystick x 
		while (Driver_I2C0.GetStatus().busy == 1);
		Driver_I2C0.MasterReceive (SLAVE_I2C_ADDR2, maValeur, 6, false);		
		while (Driver_I2C0.GetStatus().busy == 1);	
		sprintf( chaine," Valeur : %d", maValeur[0]);
		GLCD_DrawString (2,3,chaine);
	}
	
	return 0;
}
