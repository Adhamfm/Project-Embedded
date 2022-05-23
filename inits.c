#include "tm4c123gh6pm.h"
#include "stdint.h"

void PORTF_Init();
unsigned int SW_INPUT(void);
void RGB_OUTPUT (unsigned char data);

void SystemInit(){}

void PORTF_Init(){
 SYSCTL_RCGCGPIO_R|=0X20; //Enable clock for portf
 while((SYSCTL_PRGPIO_R & 0X20) == 0){}; //Wait until check that clock is enabled
 GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY; //Unlock PORTF
 GPIO_PORTF_CR_R = 0X1F;  //Allow change to PF4-0
 GPIO_PORTF_AFSEL_R = 0X0;	//Disable alternate function for PF4-0
 GPIO_PORTF_PCTL_R = ~0X000FFFFF;  //GPIO on PF4-0
 GPIO_PORTF_AMSEL_R = 0X0;	//Disable analog on PF4-0
 GPIO_PORTF_DIR_R = 0X0E;		//PF3-1 is output , (PF0,PF4) is input
 GPIO_PORTF_DEN_R = 0X1F;		//Enable Digital I/O for PF4-0
 GPIO_PORTF_PUR_R = 0X11;		//Enable pull-up resistor for PF0,PF4
 GPIO_PORTF_DATA_R = 0X11;	//Initalize PF4-0
}

unsigned int SW_INPUT(void) {
return GPIO_PORTF_DATA_R & 0x11;
}


void RGB_OUTPUT (unsigned char data) {
   GPIO_PORTF_DATA_R &=~ 0x0E;
	 GPIO_PORTF_DATA_R |= data;
}