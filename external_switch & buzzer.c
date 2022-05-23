#include "tm4c123gh6pm.h"
#include "string.h"
#include "stdint.h"

void external_switch_buzzer_init (void);
unsigned char read_external_switch(void);
void buzzer (unsigned char buzzer_state);
void ONE_MICRO_SEC_DELAY(void);
void DELAY_MICRO_SEC(int NUMBER_OF_MICRO_SEC);

void external_switch_buzzer_init (void)
{
	SYSCTL_RCGCGPIO_R |= 0X1;
	while((SYSCTL_PRGPIO_R&0x1) == 0){}
	GPIO_PORTA_LOCK_R=0x4C4F434B;
	GPIO_PORTA_CR_R |= 0X0C;
	GPIO_PORTA_DIR_R &=~0X04;   // PIN A2 AS INPUT FOR THE SWITCH
	GPIO_PORTA_DIR_R |=0X08;    // PIN A3 AS OUTPUT FOT BUZZER
	GPIO_PORTA_DEN_R |= 0X0C;   // DEN FOR PINS A2&A3
	GPIO_PORTA_AFSEL_R &= ~0X0C; 
	GPIO_PORTA_PCTL_R &= ~0X0000FF00;  // USE PINS A2&A3 AS GPIOS
	GPIO_PORTA_AMSEL_R &=~0X0C; // DISABLE THE ANALOG FUN 
	GPIO_PORTA_DATA_R &= ~0X08; // INITIALY BUZZER IS OFF
}

unsigned char SW3_Input(void)
{
	return (GPIO_PORTA_DATA_R&=0X04); // RETURN THE VALUE IN PIN A2
}

void buzzer (unsigned char buzzer_state)     // STATE_ACTIVE = 0X08 , STATE_INACTIVE = 0X00
{
	GPIO_PORTA_DATA_R &= ~0X08;
	GPIO_PORTA_DATA_R |= buzzer_state; // WRITE THE STATE OF THE BUZZER AT PIN A3
}
