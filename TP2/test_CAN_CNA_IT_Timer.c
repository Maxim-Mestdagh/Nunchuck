// Programme test CAN/CNA/timer/IT (sans carte extension nécessaire)
// Auteur : XM - dec 2014

#include "LPC17xx.h"
#include "CAN_CNA.h"
#include "math.h"

int Conversion_AD()      /* lance la conversion et lit la valeur convertie */
{
	int entree, DONE;
	
	LPC_ADC->ADCR &= ~( 7 << 24);              /* stop conversion               */
	LPC_ADC->ADCR |=  ( 1 << 24);              /* start conversion              */
	
	do{
		DONE = LPC_ADC->ADGDR & 0x80000000; /* conversion prête ? */
	}
	while(DONE != 0x80000000);
	
	LPC_ADC->ADGDR &= 0x7FFFFFFF; 					/* CLEAR DONE bit N°31*/
	entree = (LPC_ADC->ADGDR >> 4)& 0x00000FFF;  	/* 12 bits décalage car le nombre converti est au milieu du mot*/
	entree = entree - 2048;
	return entree;
}




void Conversion_DA(int sortie)
{
sortie = (sortie + 2048)/4 ;   	/* 10 bits, division par 4 car ADC 12b et DAC 10b */
LPC_DAC->DACR = (sortie << 6);  /* decalage car le nombre à envoyer est au milieu du registre */
}



void Timer_Init(unsigned int prescaler, unsigned int valeur)
{
LPC_SC->PCONP |= (1<<1); //allume le timer 0 (facultatif, déjà allumé après un reset)

LPC_TIM0->PR =  prescaler;
LPC_TIM0->MR0 = valeur; 

LPC_TIM0->MCR = 3;		/*reset compteur si MR0=COUNTER + interruption*/

LPC_TIM0->TCR = 1; 		//démarre le comptage
}



// fonction IT - recopie
void TIMER0_IRQHandler(void)
{
	int en;
	char n;
	LPC_TIM0->IR |= 1<<0;   // drapeau IT Timer0
	
	// recopie : CAN -> CNA
	en = 2048*sin(n*0.13);
	Conversion_DA(en);
	n++;
}


int main (void)
{		
	ADC_Init();	
	DAC_Init();
	Timer_Init(0, 566); 		// 44.1 kHz

	NVIC_EnableIRQ(TIMER0_IRQn);
	
	while (1);
	
	return 0;
}
