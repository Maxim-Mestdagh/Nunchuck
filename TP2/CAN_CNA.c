
// Librairie des fonctions permettant l'initialisation des CAN/CNA (KEIL)
// IUT de Cachan - 03/12/2014 

#include "LPC17xx.h"

void ADC_Init (void) /* pour entrée GBF P0.24 */
{

	LPC_SC->PCONP |= (1<<12 ); 			// reveil peripherique CAN
	
	LPC_PINCON->PINSEL1 |= (1<<16); 	//P0.24 (AD0.1) en mode ADC
	LPC_PINCON->PINSEL1 &= ~(1<<17); 	//P0.24 (AD0.1) en mode ADC
		
	LPC_PINCON->PINMODE1 |= (1<<17);	/* pas de pull up ni de pull down */
	LPC_PINCON->PINMODE1 &= ~(1<<18);
	
	LPC_ADC->ADCR |= 	( 1 <<  1) |    /* select AD0.1 pin   */
										( 1 <<  8) |       	/* ADC clock is 25MHz/    */
										( 1 << 21);        	/* enable ADC     */

}


void DAC_Init(void)
{

LPC_PINCON->PINSEL1 &=  ~( 3 << 20); 	/* pin P0-26 = DAC*/
LPC_PINCON->PINSEL1 |=  ( 2 << 20);
LPC_PINCON->PINMODE1 &=  ~( 3 << 20);
LPC_PINCON->PINMODE1 |=  ( 2 << 20);  	/* pas de résistance */
}

