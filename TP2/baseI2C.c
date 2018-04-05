#include "Driver_I2C.h"                 // ::CMSIS Driver:I2C
#include "stdio.h"
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"
#include "GPIO.h" 
#include "LPC17xx.h"
#include "math.h"
#include "Board_ADC.h"                  // ::Board Support:A/D Converter
#include "Board_DAC.h"                  // ::Board Support:D/A Converter
#include "CAN_CNA.h"

#define SLAVE_I2C_ADDR       0xXX			// Adresse esclave sur 7 bits

extern ARM_DRIVER_I2C Driver_I2C0;
extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

void Timer0_Init(unsigned int prescaler,unsigned int valeur);

void Init_I2C(void){   //initialisation de la transmission I2C
	Driver_I2C0.Initialize(NULL);
	Driver_I2C0.PowerControl(ARM_POWER_FULL);
	Driver_I2C0.Control(	ARM_I2C_BUS_SPEED,				// 2nd argument = débit
							ARM_I2C_BUS_SPEED_STANDARD  );	// 100 kHz
	Driver_I2C0.Control(	ARM_I2C_BUS_CLEAR,
							0 );
}
void  TIMER0_IRQHandler(void)
{
	float en;
	char n;
	en = 2048*sin(n*12500);
	
	n++;
}
int main (void){
	int i;
	ADC_Init ();
	DAC_Init();
	Init_I2C();
	GLCD_Initialize();
  GLCD_ClearScreen();
  GLCD_SetFont(&GLCD_Font_16x24);
	Timer0_Init(1, 624999);
	

	NVIC_EnableIRQ(TIMER0_IRQn);
  NVIC_SetPriority(TIMER0_IRQn,2);

  NVIC_SetPriority(I2C0_IRQn,0);
  NVIC_SetPriority(SSP1_IRQn,0);
	
	while (1){
		
	}
	
	return 0;
}

void Timer0_Init(unsigned int prescaler,unsigned int valeur)
{
		LPC_SC->PCONP |=(1<<1);//active timer 0
	
    LPC_TIM0->CTCR=0;// Mode timer
    LPC_TIM0->PR= prescaler;// fixer valeur du prescaler
    LPC_TIM0->MR0=valeur;// fixer valeur du registre MRO
    LPC_TIM0->MCR|=(3<<0); // RAZ du comptuer+déclenchement interruption
	
		LPC_TIM0->TCR = 1;
}
	