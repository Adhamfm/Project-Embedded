#include "tm4c123gh6pm.h"
#include "stdint.h"


//for keypad// we will use portc and porte 
//portC pins 4-7 represent rows  (outputs)
//portE pins 0-3 represent columns (inputs)

void ONE_MICRO_SEC_DELAY(void);
void DELAY_MICRO_SEC(int NUMBER_OF_MICRO_SEC);
void KeypadGpioInit();
unsigned char keypadInput();
unsigned char keypadInput_optionD();
int time_from_array(int timer[], int i);


/*KEYPAD SECTION*/
//for keypad// we will use portc and porte 
//portC pins 4-7 represent rows  (outputs)
//portE pins 0-3 represent columns (inputs)

void ONE_MICRO_SEC_DELAY(void){
	NVIC_ST_CTRL_R=0X00;  //turn off SysTick
	NVIC_ST_RELOAD_R = 15 ; // number of counts to wait
	NVIC_ST_CURRENT_R = 0; // any value written to CURRENT clears
	NVIC_ST_CTRL_R=0X00000005;  //enable SysTick with core clock
	while((NVIC_ST_CTRL_R&0x00010000)==0){} // wait for count flag
}

void DELAY_MICRO_SEC(int NUMBER_OF_MICRO_SEC)	
{
	int i ;
	for (i = 0 ; i<NUMBER_OF_MICRO_SEC ; i++)
	{
		ONE_MICRO_SEC_DELAY();
	}
	
}

void KeypadGpioInit(){
	SYSCTL_RCGCGPIO_R|=0X14;//ENABLE CLOCK FOR PORT C ,E
	while((SYSCTL_PRGPIO_R&0X14)==0){};//WAIT UNTIL ENABLED
	GPIO_PORTC_LOCK_R= GPIO_LOCK_KEY; //need to be checked if portC is locked or not?
	GPIO_PORTC_CR_R|=0XF0;//ENABLE CR FOR PC7-4
	
	GPIO_PORTE_LOCK_R= GPIO_LOCK_KEY; //need to be checked if portE is locked or not?
	GPIO_PORTE_CR_R|=0X0F;//ENABLE CR FOR PE3-0
	
	GPIO_PORTC_AFSEL_R&=~0XF0;//DISABLE ALTERNATE FUNCTION FOR PC7-4
	GPIO_PORTE_AFSEL_R&=~0X0F;//DISABLE ALTERNATE FUNCTION FOR PE3-0
		
	GPIO_PORTC_PCTL_R&=~0XFFFF0000;//GPIO FOR PC7-4
	GPIO_PORTE_PCTL_R&=~0X0000FFFF;//GPIO FOR PE3-0
		
	GPIO_PORTC_AMSEL_R&=~0XF0;//DISABLE ANALOG FOR PC7-4
	GPIO_PORTE_AMSEL_R&=~0X0F;//DISABLE ANALOG FOR PE3-0
			
	GPIO_PORTC_DIR_R|=0XF0;	//PC7-4 IS OUTPUT
	GPIO_PORTE_DIR_R&=~0X0F;//PE3-0 IS INPUT	
		
	GPIO_PORTC_DEN_R|=0XF0;//ENABLE DIGITAL FOR PC7-4 
	GPIO_PORTE_DEN_R|=0X0F;//ENABLE DIGITAL FOR PE3-0
	
	
	GPIO_PORTE_PDR_R|=0X0F;//ENABEL PULL DOWN FOR PE3-0 *****IMPORTANT WE ARE NOW WORKING POSITIVE LOGIC
}

//main function of keypad which will detect whether key is preesed or not if yes then it will return this element from the keypad matrix
unsigned char keypadInput()
{//to remember portC (pins 4-7 are rows) , portE (pins 0-3 are columns)
unsigned char keypadButtons[4][4] = {{ '1', '2', '3', 'A'},
																		 { '4', '5', '6', 'B'},
																		 { '7', '8', '9', 'C'},
																		 { '*', '0', '#', 'D'},
																		};
int i,j;//counters
unsigned char pressed_button;//HOLD THE MATRIX ELEMENT THAT CORRESPONDS TO THE PREESED KEY
																		 
	
while(1)
{
//LETS CHECK FIRSTLY WHETHER THERE IS A KEY PRESSED OR NOT
GPIO_PORTC_DATA_R|=0XF0;//ENABLE ALL ROWS
if((GPIO_PORTE_DATA_R&0X0F)==0X00) {continue;}//HERE WE CHECK IF ANY KEY IS PRESSED ,IF ANY IS PRESSED THEN CORRESPONDING BIT IS SET


//IN CASE KEY IS PRESSED WE WILL START DETECTING IT 		
GPIO_PORTC_DATA_R&=~0XF0;//DISABLE ALL ROWS 
for(i=0 ;i<4;i++){
		if(i>0) {GPIO_PORTC_DATA_R&=~(0X10<<(i-1));} //TO AVOID 2 ROWS BEING ENABLE AT THE SAME TIME SO EACH ITERATION DISABLE THE PREVIOUS ITERATION ROW
		GPIO_PORTC_DATA_R|=(0X10<<i);//HERE WE ENABLE ROW BY ROW
		for(j=0 ; j<4; j++){//LOOP ON ALL COLUMNS IN ROW i TO SEE WHICH KEY IS PRESSED
				if((GPIO_PORTE_DATA_R&0X0F)==(0X01<<j)){
					pressed_button = keypadButtons[i][j];//GETTING THE ELEMENT CORRESPONDING TO THE PRESSED KEY AND PASSING TO PRESSED_BUTTON VARIABLE
				}				
			}
		}
	DELAY_MICRO_SEC(20);
return pressed_button;
	
	}
}

unsigned char keypadInput_optionD()
{//to remember portC (pins 4-7 are rows) , portE (pins 0-3 are columns)
unsigned char keypadButtons[4][4] = {{ '1', '2', '3', 'A'},
																		 { '4', '5', '6', 'B'},
																		 { '7', '8', '9', 'C'},
																		 { '*', '0', '#', 'D'},
																		};
int i,j;//counters
unsigned char pressed_button;//HOLD THE MATRIX ELEMENT THAT CORRESPONDS TO THE PREESED KEY
																		 
	
while(1)
{
//LETS CHECK FIRSTLY WHETHER THERE IS A KEY PRESSED OR NOT
GPIO_PORTC_DATA_R|=0XF0;//ENABLE ALL ROWS
if((GPIO_PORTE_DATA_R&0X0F)==0X00) {return 'z';}//HERE WE CHECK IF ANY KEY IS PRESSED ,IF ANY IS PRESSED THEN CORRESPONDING BIT IS SET


//IN CASE KEY IS PRESSED WE WILL START DETECTING IT 		
GPIO_PORTC_DATA_R&=~0XF0;//DISABLE ALL ROWS 
for(i=0 ;i<4;i++){
		if(i>0) {GPIO_PORTC_DATA_R&=~(0X10<<(i-1));} //TO AVOID 2 ROWS BEING ENABLE AT THE SAME TIME SO EACH ITERATION DISABLE THE PREVIOUS ITERATION ROW
		GPIO_PORTC_DATA_R|=(0X10<<i);//HERE WE ENABLE ROW BY ROW
		for(j=0 ; j<4; j++){//LOOP ON ALL COLUMNS IN ROW i TO SEE WHICH KEY IS PRESSED
				if((GPIO_PORTE_DATA_R&0X0F)==(0X01<<j)){
					pressed_button = keypadButtons[i][j];//GETTING THE ELEMENT CORRESPONDING TO THE PRESSED KEY AND PASSING TO PRESSED_BUTTON VARIABLE
				}				
			}
		}
	DELAY_MICRO_SEC(20);
return pressed_button;
	
	}
}

void copyArray(int arr[], int copy[], int size)
{
	int i;
  // loop to iterate through array
  for (i = 0; i < size; ++i)
  {
    copy[i] = arr[i];
  }
}

int time_from_array(int timer[], int i){
	int time = 0;
	int j;
		for (j =0; j <= 2-i; j++)
			{
				if (timer[3]==0)
				{
				timer[3] = timer[2];
				timer[2] = timer[1];
				timer[1] = timer[0];
				timer[0] = 0;
				}
			}
			//12:34
				time += timer[0]*10*60;
				time += timer[1]*60;
				time += timer[2]*10;
				time += timer[3];
			return time;
}
