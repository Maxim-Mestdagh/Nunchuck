#include "Driver_I2C.h"                 // ::CMSIS Driver:I2C
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "stdio.h"
#include "LPC17xx.h"

#define SLAVE_I2C_ADDR       0xa4			// Adresse esclave sur 7 bits

extern ARM_DRIVER_I2C Driver_I2C0;
extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

char read1byte(unsigned char composant, unsigned char registre);

uint8_t DeviceAddr;

void Init_I2C(void){
	Driver_I2C0.Initialize(NULL);
	Driver_I2C0.PowerControl(ARM_POWER_FULL);
	Driver_I2C0.Control(	ARM_I2C_BUS_SPEED,				// 2nd argument = débit
							ARM_I2C_BUS_SPEED_STANDARD  );	// 100 kHz
	Driver_I2C0.Control(	ARM_I2C_BUS_CLEAR,
							0 );
}


void write1byte (unsigned char composant, unsigned char registre, unsigned char valeur)
{
	uint8_t tab[2];
  tab[0]=registre;
  tab[1]=valeur;
	Driver_I2C0.MasterTransmit (composant, tab, 2, false);		// false = avec stop
	while (Driver_I2C0.GetStatus().busy == 1);	// attente fin transmission
}

char read1byte(unsigned char composant, unsigned char registre)
{
	char retour;
	Driver_I2C0.MasterTransmit (composant, &registre, 1, true);		// false = avec stop
	while (Driver_I2C0.GetStatus().busy == 1);	// attente fin transmission
	Driver_I2C0.MasterReceive (composant, &retour, 1, false);		// false = avec stop
	while (Driver_I2C0.GetStatus().busy == 1);	// attente fin transmission
	return retour;
}


int main (void){
	uint8_t tab[10], maValeur;
	char X1, X2;
	char Y1, Y2;
	char ax1, ax2, ay1, ay2;
	char chaine[40];
	Init_I2C();
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	
	// Adresse module esclave sur 7 bits
	DeviceAddr = 0x1D;
	write1byte (DeviceAddr,0x24,0x70);
	write1byte (DeviceAddr,0x25,0x20);
	write1byte (DeviceAddr,0x20,0x57);
	write1byte (DeviceAddr,0x21,0x00);
	write1byte (DeviceAddr,0x22,0x00);


	
	
	while (1)
	{
		

		
		
	}
	
	return 0;
}
