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
enum {A, B, C, D, InvalidInput} choices;


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
//return time in 10ms
int cooking_setup(unsigned char button_1, unsigned char button_2, unsigned char button_3){
	char weight = 'z';
	int weight_int;
	char user_input = 'z';
	int cooking_time = 0;
	// for option D
	int timer[4] = {0,0,0,0};
	int timer_copy[4] = {0,0,0,0};
	int i;
	int time = 0;
	int broke = 0;
	char* lcd_output;
	
	LCD_WRITE("Enter Option");
	user_input = keypadInput();
	if (user_input != 'A' && user_input != 'B' && user_input != 'C' && user_input != 'D')
	{
		choices = InvalidInput;
	}
	if (user_input == 'A') choices = A;
	if (user_input == 'B') choices = B;
	if (user_input == 'C') choices = C;
	if (user_input == 'D') choices = D;
	
	while(1)
	{
		switch (choices){
			case A:
				LCD_WRITE("Popcorn");
				SysTick_Wait10ms(200);
				cooking_time =6000;
				return cooking_time;
			break;
			
			case B:
				LCD_WRITE("BEEF WEIGHT?");
				// user enters between 1 and 9
				while (weight =='z')
				{
					lightup(all);
					weight = keypadInput();
					LCD_WRITE("");
					LCD_DATA(weight);
					SysTick_Wait10ms(100);
				}
				// Invalid Inputs
				if (weight == 'A' || weight == 'B' || weight == 'C' || weight == 'D' || weight == '*' || weight == '#' || weight == '0'){
					LCD_WRITE("Invalid Input");
					SysTick_Wait10ms(200);
					weight = 'z';
					continue;
				}
				weight_int = weight - '0';
				cooking_time = weight_int*0.5*60*100;
				return cooking_time;
			break;
			
			case C:
				LCD_WRITE("CHICKEN WEIGHT?");
				SysTick_Wait10ms(100);
				// user enters between 1 and 9
					while (weight =='z')
				{
					weight = keypadInput();
					LCD_WRITE("");
					LCD_DATA(weight);
					SysTick_Wait10ms(100);
					continue;
				}
				
				// Invalid Inputs
				if (weight == 'A' || weight == 'B' || weight == 'C' || weight == 'D' || weight == '*' || weight == '#' || weight == '0'){
					LCD_WRITE("Invalid Input");
					SysTick_Wait10ms(200);
					weight = 'z';
					continue;
				}
				weight_int = weight - '0';
				//time in 10ms, 0.2min per 1kilo weight
				cooking_time = weight_int*100 *0.2 *60;
				return cooking_time;
			break;
			
			case D:
				LCD_WRITE("Cooking Time? ");
				SysTick_Wait10ms(200);
						
				// Save User Inputs
				for (i=0; i<4; i++){
					broke = 0;
					user_input = 'z';
					if (i != 0)
					{
						LCD_WRITE(lcd_output);
						SysTick_Wait10ms(100);
					}
					while(user_input ==  'z')
					{
						if (SW_INPUT() == button_2)
						{
							lightup(0x8);
							time = 0;
							broke = 1;
							i++;
							time = time_from_array(timer_copy, i);
							break; 
						}
						user_input = keypadInput_optionD();
					}
				if (broke == 1) break;

						if (user_input == 'A' || user_input == 'B' || user_input == 'C' || user_input == 'D' || user_input == '*' || user_input == '#'){
							LCD_WRITE("Invalid Input");
							SysTick_Wait10ms(100);
							LCD_WRITE("");
							user_input = 'z';
							// Loop doesnt count
							i--;
							continue;
						}
						timer[i] = user_input - '0';
						copyArray(timer, timer_copy,4);
						time = time_from_array(timer_copy, i);
						lcd_output = formatTime(time);
						
						
			}
				//ex: [1,2,0,0] -> [0,0,1,2]
				copyArray(timer, timer_copy,4);
				lcd_output = formatTime(time);
				LCD_WRITE(lcd_output);
				SysTick_Wait10ms(50);
				cooking_time = time*100;
				return cooking_time;
			break;
			
			case InvalidInput:
				LCD_WRITE("Invalid Input");
				SysTick_Wait10ms(200);
				return 0;
			break;
		}
	}
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
				cooking_time_10ms = cooking_setup(button_1, button_2, button_3);
				if (cooking_time_10ms!=0)
				{
					delay = cooking_time_10ms;
					time_consumed = delay;
					state = cooking;
				}
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
