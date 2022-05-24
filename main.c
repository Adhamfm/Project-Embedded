#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"
#include "external_switch_buzzer.h"
#include "KEYPAD.h"
#include "LCD.h"
#include "inits.h"


#define none 0x11
#define all 0x0E

char* formatTime(int time_seconds);

enum {init, setup, cooking, cleared, completed, interrupted, paused} state;



//============================= Microwave Functions =============================//
char* formatTime(int time_seconds){
	// Change time from seconds to 00:00 format
	char time[5] = "00:00";
	int mins = floor((time_seconds / 60) % 60) ;
	int secs = floor(time_seconds % 60) ;
	
	char ones = (mins %10) +'0';
	char tens = ((mins /10) )+'0';
	char secones = (secs %10)+'0';
	char sectens = ((secs /10))+'0';
	
	
	time[0]=tens;
	time[1]=ones;
	time[3]=sectens;
	time[4] = secones;
	return time;
}

void lightup(char ledcode){
	int i;
	RGB_OUTPUT(ledcode);
	SysTick_Wait10ms(50);
	//for (i = 0; i < 50; i++)
	//	SysTick_Wait(160000);
	RGB_OUTPUT(none);
	SysTick_Wait10ms(100);
	}
	
void Init(){
	char button_1 = 0x01; 
	char button_2 = 0x10;
	char button_3 = 0x04;
	SysTick_Init(); //might be edited
	PORTF_Init();
	LCD_Init();
	KeypadGpioInit();
	external_switch_buzzer_init();
}
//=================================== Main Function ===========================//

unsigned char button_1 = 0x01;
unsigned char button_2 = 0x10;
unsigned char button_3 = 0x04;
int remaining_time = 10;
int cooking_time_10ms =0;
int doorOpened = 1;
int cooking_time = 0;
uint32_t delay;
unsigned long i;
int remaining_seconds;
char* remaining_seconds_str;
int time_consumed;
	
int main(){

	state = init;
	while(1)
	{
		switch(state){
			case init:
				Init();
				LCD_WRITE("System On");
				SysTick_Wait10ms(100);
				// Check if Door Opened before operation
				while(doorOpened == 1)
				{
					if(SW3_Input() == button_3){
						LCD_WRITE("CLOSE DOOR");
						SysTick_Wait10ms(200);
					} else {
						doorOpened = 0;
					}
				}
				state = setup;
				break;
			
			case setup:
				
			break;
			
			case cooking:
				delay = time_consumed;
				//LED ON
				RGB_OUTPUT(all);

				for (i=0; i < delay; i++){
					SysTick_Wait(160000); //wait 10ms
					time_consumed--;
					remaining_seconds = time_consumed/100;
					// Change LCD screen after 1 second
					if (time_consumed%100 == 0){
						remaining_seconds_str = formatTime(remaining_seconds);
						LCD_WRITE(remaining_seconds_str);
					}
					// button_1 paused & button_3 for door inturrpt
					if (SW_INPUT() ==button_1){
						state = paused;
						break;
					}
					if (SW3_Input() == button_3){
						state = interrupted;
						break;
					}
				}
				if (time_consumed == 0) state = completed;
			break;
			
			
			case paused:
				lightup(0x02);
				if (SW_INPUT() ==button_1)
				{
					state = cleared;
					break;
				}
				if (SW_INPUT() ==button_2)
				{
					state = cooking;
					break;
				}
				state = paused;
			break;
			
			case cleared:
				LCD_WRITE("Cleared");
				SysTick_Wait10ms(300);
				cooking_time_10ms = 0;
				time_consumed = 0;
				state = init;
			break;
			
			case interrupted:
				LCD_WRITE("CLOSE THE DOOR");
				SysTick_Wait10ms(100);
				if (SW3_Input() == 0) state = cooking;
				if (SW_INPUT() == button_1) state = cleared;
			break;
			
			case completed:
				buzzer(0x08);
				for ( i = 0 ; i < 3 ; i++)
				{
					RGB_OUTPUT(none);
					SysTick_Wait10ms(100);
					RGB_OUTPUT(all);
					SysTick_Wait10ms(100);
				}	
				buzzer(0);
				RGB_OUTPUT(none);
				state = init;
			break;
		}
		
	}
}
