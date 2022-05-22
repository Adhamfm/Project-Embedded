#include "tm4c123gh6pm.h"
#include "stdint.h"

//lcd//assume gpio portb(DATA) and 
//A(5,6,7 CONTROL)(RS 5, R/W 6 , E 7)

void LCD_Init();
void delay_1microOrmillisec(unsigned long delay);
void generic_delayMicro(unsigned long time);
void generic_delayMilli(unsigned long time);
void SysTick_Init(void);
void SysTick_Wait(uint32_t delay);
void SysTick_Wait10ms(uint32_t delay);
void LCD_DATA(unsigned char data);
void LCD_CMD(unsigned char command);
void LCD_STRING(char *str);
void LCD_WRITE(char* str);
	
	
#define RS 0x20				 
#define RW 0x40        
#define EN 0x80  		 	 	

void SysTick_Init(void) {
	NVIC_ST_CTRL_R    = 0;
	NVIC_ST_RELOAD_R  = 0x00FFFFFF;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R    = 0X00000005;
}

void SysTick_Wait(uint32_t delay) {
	NVIC_ST_RELOAD_R = delay - 1;
	NVIC_ST_CURRENT_R = 0;
	while ((NVIC_ST_CTRL_R&0x00010000)==0) {}
}

void SysTick_Wait10ms(uint32_t delay) {//1000
	unsigned long i;
	for (i = 0; i < delay; i++)
		SysTick_Wait(160000); //wait 10ms
}

void LCD_Init(){
	SYSCTL_RCGCGPIO_R|=0X3;
	while((SYSCTL_PRGPIO_R&0X3)==0){};
	GPIO_PORTA_LOCK_R= GPIO_LOCK_KEY;
	GPIO_PORTA_CR_R|=0XE0;

	GPIO_PORTB_LOCK_R= GPIO_LOCK_KEY;
	GPIO_PORTB_CR_R=0XFF;
		
	GPIO_PORTA_AFSEL_R&=~0XE0;
	GPIO_PORTB_AFSEL_R=0X0;
	
	GPIO_PORTA_PCTL_R&=~0XFFF00000;
	GPIO_PORTB_PCTL_R = 0X0;
	
	GPIO_PORTA_DIR_R|=0XE0;
	GPIO_PORTB_DIR_R=0XFF;	
		
	GPIO_PORTA_DEN_R|=0XE0;
	GPIO_PORTB_DEN_R=0XFF;
	
	GPIO_PORTA_AMSEL_R&=~0XE0;
	GPIO_PORTB_AMSEL_R=0X0;

		LCD_CMD(0x38);        
		LCD_CMD(0x06);          
		LCD_CMD(0x01);					
		LCD_CMD(0x0F);          

}

void delay_1microOrmillisec(unsigned long delay){
	NVIC_ST_CTRL_R=0X00;  //turn off SysTick
	NVIC_ST_RELOAD_R= delay-1; //no.of counts
	NVIC_ST_CURRENT_R=0X00; //clear the counter
	NVIC_ST_CTRL_R=0X00000005;  //enable SysTick with core clock
	while((NVIC_ST_CTRL_R&0X00010000)==0){}; //wait until count flag is set
}

void generic_delayMicro(unsigned long time){ //function for delay multiple of 1ms
	int i;
	for(i=0;i<time;i++){
		delay_1microOrmillisec(16); //delay of 1MICROSEC
	}
}
void generic_delayMilli(unsigned long time){ //function for delay multiple of 1ms
	int i;
	for(i=0;i<time;i++){
		delay_1microOrmillisec(16000); //delay of 1millisec
	}
}

void LCD_DATA(unsigned char data){

	GPIO_PORTA_DATA_R&=~(RW|EN);
	GPIO_PORTA_DATA_R|= RS;
	
	GPIO_PORTB_DATA_R= data;
	
	GPIO_PORTA_DATA_R|= EN;
	generic_delayMicro(10);
	GPIO_PORTA_DATA_R&= ~EN;
	
	GPIO_PORTB_DATA_R=0X0;
	generic_delayMicro(40);
	
	
}

void LCD_CMD(unsigned char command){
	GPIO_PORTA_DATA_R&=~(RW|EN|RS);
	
	GPIO_PORTB_DATA_R=command;
	
	GPIO_PORTA_DATA_R|= EN;
	generic_delayMicro(10);
	GPIO_PORTA_DATA_R&= ~EN;
	
	GPIO_PORTB_DATA_R=0X0;

	if(command < 4)
		generic_delayMilli(2);       //commands 1 and 2 need up to 1.64ms
	else
		generic_delayMicro(40);      //all others 40 us

}

void LCD_STRING(char *str){
	while(*str){
		LCD_DATA(*str);
		str++;	
	}
}

void LCD_WRITE(char* str)
{
	LCD_CMD(0x01);						
	LCD_CMD(0x80);   
	generic_delayMilli(50);						
	LCD_STRING(str);							
	generic_delayMilli(100);	
}
