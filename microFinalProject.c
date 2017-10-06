/* Final Project */
/* 121044050     */
/* Duygu Bulut   */

// kullanilacak moduller
// portb , seven segment, pth , timer overflow, output compare 
// interrupt 
// lcd , buzzer

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//lcd.h
#define LCD_DAT PORTK	//portK drives LCD data pins E and RS
#define LCD_DIR DDRK
#define LCD_E 0x02		//LCD E signal
#define LCD_RS 0x01		//Lcd register select signal
#define CMD 0
#define DATA 1  // lcd 
/* keypad specific defines */
#define ROW_NUM 4
#define COL_NUM 4 
  
 //global variables
 int overflow = 0;
 /*defining user id*/
 unsigned char user_id_1=1;
 unsigned char user_id_2=2;
 unsigned char user_id_3=3;
 unsigned char user_id_4=4; 
 /* defining passwords */
 unsigned char password_1=2;
 unsigned char password_2=3;
 unsigned char password_3=4;
 unsigned char password_4=6;
 /* initializing balance */
 unsigned int balance_1 = 100;
 unsigned int balance_2 = 200;
 unsigned int balance_3 = 300;
 unsigned int balance_4 = 400;
 /*initialize money  */
 int decMoney = 150;  //decrement to draw money
 int incMoney = 300;  // increment to deposit money

 // function prototypes
void timer_initializer(void);          // initialize timer module
void pth_initializer(void);            // initialize porth for getting input
void keypad_initialize(void);          // initialize DDRA to use keypad
void printMessage(char string1[],char string2[]);   // to print several message to lcd
void MSDelay(unsigned int itime);      // milisecond delay
void printLine(char myString[]);              // to print a message to lcd
void openlcd(void);	                   // initialize LCD
void puts2lcd(char* str);              // write command or data to Lcd controller
void put2lcd(char c, char tyoe);       // write a string to LCD
void delay_50us(int n);		             // n*50 microsecond intervals
void menu(unsigned char user_id);      // ATM menu function
void BuzzerPlayer(void);                 // buzzer function     
int getKeyFromKeypad(void);            // to get input from keypad function

interrupt (((0x10000 - Vtimovf)/2)-1) void overflow_ISR(void);
interrupt (((0x10000 - Vporth)/2)-1) void prth_ISR(void);

void main(void) {
  
 DDRB = 0xFF;             /* make output PORTB */
 DDRP = 0xFF;             /* make output for using seven segment */
 DDRT = DDRT | 0x20;      /* for play buzzer */ 
 TIOS = TIOS | 0x20; 	    /* ch5 output compare */
 
 timer_initializer();
 pth_initializer();
 keypad_initialize();
 
 printMessage("Welcome to ATM !","Enter userNumber");
 MSDelay(3000); //user id girmesi icin  3 saniye bekliyorum. Eger hala girmezse programý bitiriyorum
  
 if(PTH == 1)  // if user is first_user
 {  
   printMessage("Welcome user1" , "Enter   password");
   MSDelay(4000); // 4 saniye bekliyorum sifre icin
   
   if (password_1== PTH)   // is it password true for user 1
   { 
      printMessage("True password", "Redirecting menu");
      MSDelay(1000);
      PTP = 0x07;            //1.bit enable 
      PORTB = 0b00000110 ;   // 1
      menu(user_id_1);
      
   } 
   else {
      printMessage("Wrong password","System closed");   // exit
      BuzzerPlayer();
      
   }
 }
  
   else if(PTH ==2)  // if user is second user
 { 
   printMessage("Welcome user2" , "Enter   password");
   MSDelay(4000); // 4 saniye bekle

   if (password_2== PTH)    // is it password true for user 2
   { 
      printMessage("True password", "Redirecting menu");
      PTP = 0x07;            //1.bit enable 
      PORTB = 0b11011011 ;         // 2
      menu(user_id_2);     
   } 
   else {
      printMessage("Wrong password","System closed");   //exit
      BuzzerPlayer();
   }
 }
 
 else if(PTH ==3) // if user is third user
 { 
   printMessage("Welcome user3" , "Enter   password");
   MSDelay(4000); // 4 saniye bekle
  
   if (password_3== PTH)  // is it password true for user 3
   {    
      printMessage("True password", "Redirecting menu");
      PTP = 0x07;      //1.bit enable
      PORTB = 0b01001111; // 3 
      menu(user_id_3);
   } 
   else {
      printMessage("Wrong password","System closed");  // exit
      BuzzerPlayer();
   }
  }
   
 else if(PTH == 4)    // if user is last user
 { 
   printMessage("Welcome user4" , "Enter   password");
   MSDelay(4000); // 4 saniye bekle
   
   if (password_4 == PTH )    // is it password true for user 4
   {     
      printMessage("True password", "Redirecting to menu");
      PTP = 0x07;      //1.bit enable
      PORTB = 0b01100110; // 4 
      menu(user_id_4);
   } 
   else {
      printMessage("Wrong password","System closed");  //exit
      BuzzerPlayer();
   }
 }
 else {
  printMessage("Wrong user id","System closed");
  BuzzerPlayer(); 
 }

	EnableInterrupts;   
 
}

void printLine(char myString[])
{
   openlcd();          // initialize LCD
   puts2lcd(myString);		//send first line 
}

void printMessage(char string1[],char string2[])
{  
 	openlcd();	        //initalize LCD
 	puts2lcd(string1);		  //send first line
 	put2lcd(0xC0,CMD);	//move cursor to 2nd row, 1st colmn
 	puts2lcd(string2);		  //send second line	
}

void MSDelay(unsigned int itime)  //millisecond delay
{
  unsigned int i, j;
  for(i=0;i<itime;i++)
      for(j=0;j<4000;j++);
}

void openlcd(void)
{
	LCD_DIR = 0xFF;		     //configure LCD_DAT port for output
  MSDelay(100);		      //wait for LCD to be ready	
	put2lcd(0x28,CMD);	  //set 4bit data, 2line display, 5x7font
	put2lcd(0x0F,CMD);	  //turn on disply, cursor blinking
	put2lcd(0x06,CMD);	  //move cursor rigt
	put2lcd(0x01,CMD);	  //clear screen, move cursor home
	MSDelay(100);		      //wait until clear disply command complete
}

void puts2lcd(char* ptr)
{
	while(*ptr)            //while char to send
	{
		put2lcd(*ptr,DATA);	 //write data to LCD
		delay_50us(1);		   //wait for data to be written
		++ptr;		 		       //next char
	}
}

void put2lcd(char c, char type)
{
	char c_lo, c_hi;
	c_hi = (c & 0xF0) >> 2;		//upper 4 bits of c
	c_lo = (c & 0x0F) << 2;		//lower 4bits of c
	
	LCD_DAT &= (~LCD_RS);		//select LCD command register
	
	if(type == DATA)
		LCD_DAT = c_hi | LCD_RS;	//output upper bits, E, RS high
	else
		LCD_DAT = c_hi;				//output upper 4bits E RS low
		
	LCD_DAT |= LCD_E;				//pull E signal to high				
	__asm(nop);						//lengthen E
	__asm(nop);
	__asm(nop);
	
	LCD_DAT &= (~LCD_E);	//pull E to low
	
	if(type == DATA)
		LCD_DAT = c_lo | LCD_RS;	//output lower 4bits E Rs high
	else
		LCD_DAT = c_lo;				//output lower 4bits E RS low
		
	LCD_DAT |= LCD_E;			//pull E to high
	
	__asm(nop);						//lengthen E
	__asm(nop);
	__asm(nop);
	
	LCD_DAT &= (~LCD_E);	//pull E to low
	
	delay_50us(1);		//wait for command to execute
}

#define D50US 133 /* Inner loop takes 9 cycles; need 50x24 = 1200 cycles */

void delay_50us(int n)
{
	volatile int c;
	for (;n>0;n--)
	for (c=D50US;c>0;c--);
}

void timer_initializer()
{
    TSCR1 = 0x80;   //timer enable
    TSCR2 = 0x80;   //timer overflow interrupt enable, prescaler = 0    
    TFLG2 = TFLG2 | 0x80; //timer overflow flag clear
    __asm CLI; 
}

void pth_initializer()
{
    DDRH = 0x00; /* make input */
    PIEH = 0xFF;
    PPSH = 0xFF;    
    __asm CLI;  
}

void menu(unsigned char user_id)     // select action to perform
{
  int option;     // to select option which you want
  int castMoney;

  printLine("Enter 1 to query the balance");    
  MSDelay(2000);   // her option icin 2 saniye bekliyorum
  printMessage("Enter 2 to ", "draw money");
  MSDelay(2000);
  printMessage("Enter 3 to " , "deposit money");
  MSDelay(2000);
  printLine("Enter 4 to exit");
  MSDelay(2000);
  
  if(user_id == user_id_1)    // if user is first, taking option from keypad
  {
      MSDelay(3000); // secimini yapmasi icin 3 saniye bekliyorum
       
     // option = getKeyFromKeypad();    // taking option
     /*  PTP = 0x07; */
     /*  PORTB = option; 
        MSDelay(1000); 
       */  
       
       option = PTH;
      
      if(option == 1 ){
        printLine("balance is 100TL");  // query the balance
        MSDelay(1000);
      }
      else if(option == 2)  // draw money 
      {
      /* decMoney = getKeyFromKeypad(); */
        castMoney = decMoney;
        PTP = 0x07;       // seven segment
        PORTB = (unsigned char) castMoney; 
        MSDelay(1000);    
     
        if(balance_1 < decMoney){
          printMessage("You don't have ","enough money");
          MSDelay(1000);
        }
        else { 
          balance_1 = balance_1 - decMoney;
          castMoney = balance_1;
          printLine("sucessful draw");
          MSDelay(1000);
          
          PTP = 0x07;
          PORTB = (unsigned char) castMoney; 
          MSDelay(1000); 
        }
      } 
      else if(option == 3)  //deposit money
      {    
       // incMoney = getKeyFromKeypad();
       castMoney = incMoney; 
       PTP = 0x07;  
       PORTB = (unsigned char) castMoney;  
       MSDelay(1000);     
      
       balance_1 += incMoney;
       castMoney = balance_1;
       printLine("succesful action");
       MSDelay(1000);
       PTP = 0x07;
       PORTB = (unsigned char) castMoney;  
       MSDelay(1000); 
      } 
      else if(option == 4){
        printMessage("You want to exit.", "Good bye!");  //exit
        MSDelay(1000);
      }
      else {
        printMessage("There is no opt.", "Good bye!");
        MSDelay(1000);
        BuzzerPlayer();
      }     
  }
  else if(user_id == user_id_2) // if user is second, taking option from keypad
  {
     MSDelay(3000); // secimini yapmasi icin 3 saniye bekliyorum
      
    // option = getKeyFromKeypad(); // taking option
    /* PTP = 0x07; */
     /*PORTB = option; 
       MSDelay(100); */
       
      option = PTH;  
        
     if(option == 1 ) {
        printLine("balance is 200TL");  // query the balance
        MSDelay(1000);
     }
      else if(option == 2) // draw money
      {
        /* decMoney = getKeyFromKeypad(); */
        castMoney = decMoney;
        PTP = 0x07;
        PORTB = (unsigned char) castMoney; 
        MSDelay(1000);
        
        if(balance_2 < decMoney){
          printMessage("You don't have ","enough money");
          MSDelay(1000);
        }
        else{
         balance_2 = balance_2 - decMoney;
         castMoney = balance_2;
         printLine("sucessful draw");
         MSDelay(1000);
         PTP = 0x07;
         PORTB = (unsigned char) castMoney; 
         MSDelay(1000); 
        }
        
      } 
      else if(option == 3) // deposit money
      { 
       // incMoney = getKeyFromKeypad();
        // PTP = 0x07;   
       castMoney =incMoney;
       PORTB = (unsigned char) castMoney;  
       MSDelay(1000);     
      
       balance_2 += incMoney;
       castMoney = balance_2;
       
       printLine("succesful action");
       MSDelay(1000);
       PTP = 0x07;
       PORTB = (unsigned char) castMoney;  
       MSDelay(1000);
       
      }
      else if(option == 4){                                       
        printMessage("You want to exit.", "Good bye!");  //exit
        MSDelay(1000);
      }
      else{
         printMessage("There is no opt." ,"Good bye!");
         MSDelay(1000); 
         BuzzerPlayer();
      }
  
  } 
  else if(user_id == user_id_3)  // if user is third, taking option from keypad
  {
     MSDelay(3000); // secimini yapmasi icin 3 saniye bekliyorum 
     
    // option = getKeyFromKeypad();  // taking option
      //  PTP = 0x07; 
     /*  PORTB = option; 
         MSDelay(100); */
      
     
     option = PTH;
     
     if(option == 1 ){
        printLine("balance is 300TL");  // query the balance
        MSDelay(1000);
     }
     else if(option == 2)  // draw the money
     {
        //decMoney = getKeyFromKeypad();
        PTP = 0x07;
        castMoney = decMoney;
        PORTB = (unsigned char) castMoney;  
        MSDelay(1000);    
     
        if(balance_3 < decMoney){
          printMessage("You don't have ","enough money");
          MSDelay(1000);
        }
        else {
         balance_3 = balance_3 - decMoney;
         castMoney = balance_3;
         printLine("sucessful draw");
         MSDelay(1000);
         PTP = 0x07;
         PORTB = (unsigned char) castMoney;  
         MSDelay(1000); 
         
        }
     } 
     else if(option == 3) // deposit money
     {  
       //incMoney = getKeyFromKeypad();
       // PTP = 0x07;   
        castMoney = incMoney;
        PORTB = (unsigned char) castMoney;  
        MSDelay(1000); 
           
        balance_3 += incMoney;
        castMoney = balance_3;
        
        printLine("succesful action");
        MSDelay(1000);
        PTP = 0x07;
       
        PORTB = (unsigned char) castMoney;  
        MSDelay(1000); 
     }
      else if(option == 4) {
        printMessage("You want to exit.", "Good bye!");  //exit
        MSDelay(1000);
      }
      else { 
         printMessage("There is no opt." ,"Good bye!");
         MSDelay(1000);
         BuzzerPlayer();       
      }
   } 
  else  if(user_id == user_id_4) // if user is last, taking option from keypad
  {
     MSDelay(3000); // secimini yapmasi icin 3 saniye bekliyorum
      
    // option = getKeyFromKeypad();     // taking option
      /*  PTP = 0x07; */
     /*  PORTB = option; 
         MSDelay(100); */
          
     option = PTH;
     
     if(option == 1 )
     {
        printLine("balance is 400TL"); // query the balance
        MSDelay(1000);
     }
     else if(option == 2)  // draw money
     {
        //decMoney = getKeyFromKeypad();
        castMoney = decMoney; 
        PTP = 0x07;
        PORTB = (unsigned char) castMoney; 
        MSDelay(1000);    
     
        if(balance_2 < decMoney){
          printMessage("You don't have ","enough money");
          MSDelay(1000);
        }
        else{
        
         balance_4 = balance_4 - decMoney;
         castMoney = balance_4;
          
         printLine("sucessful draw");
         MSDelay(1000);
         PTP = 0x07;
         PORTB = (unsigned char) castMoney;  
         MSDelay(1000);
        }
        
     } 
     else if(option == 3) // deposit money 
     {
       //incMoney = getKeyFromKeypad(); 
       // PTP = 0x07; 
       castMoney = incMoney;
       PORTB = (unsigned char) castMoney; 
       MSDelay(1000);     
      
       balance_4 += incMoney;
       castMoney = balance_4;
       printLine("succesful action");
       MSDelay(1000);
       PTP = 0x07;
       PORTB = (unsigned char) castMoney; 
       MSDelay(1000);
     }
     
     else if(option == 4)
     {
        printMessage("You want to exit.", "Good bye!");  //exit
        MSDelay(1000);
     }
     
     else
     {
        printMessage("There is no opt." ,"Good bye!"); 
        MSDelay(1000);
        BuzzerPlayer();
     }
  }
  
}

void BuzzerPlayer() 
{
    unsigned int i,Tcount;
    TCTL1 = TCTL1 | 0x04; //toggle PT5 pin option
    for(i=0 ; i<10000 ; i++)
    {   
    	  Tcount = TCNT;
    	  Tcount += 6000;
    	  TC5 = Tcount;    	  
    	  while(!(TFLG1 & TFLG1_C5F_MASK));
    	  TFLG1 = TFLG1 | 0x20;	//clear ch5 flag
    }
    
    TCTL1 = 0x00; //toggle PT5 pin option
}

void keypad_initialize(void)
{
   DDRA = 0x0F; // enable keypad 
}


int getKeyFromKeypad(void) 
{
  int row, col;
               
  const char row_mask[ROW_NUM] = { 0xFE,0xFD,0xFB,0xF7 };
  const char col_mask[COL_NUM] = { 0x10,0x20,0x40,0x80 };        
  const unsigned int keypad[ROW_NUM][COL_NUM] = { 1,2,3,10,
                                                4,5,6,11,
                                                7,8,9,12,
                                                14,0,15,13 };
  for(;;)
  { // wait until getting input
      
    for(row=0 ; row < ROW_NUM ; ++row)
    {
      PORTA = 0xFF;// Reset PortA
      for(col=0; col < COL_NUM ;++col)
      {
                
        PORTA = PORTA & row_mask[row]; // Set specific row to 0
                                     
        if( (PORTA & col_mask[col]) == 0 )
        {
        // Check if any key is pressed      
          MSDelay(1);                      
          // Wait 1ms and check again for make sure key is pressed.
          if( (PORTA & col_mask[col]) == 0 )
          {
            return keypad[row][col]; 
          }                   
              
        }
           
      }
      
    }
  } 
}  
 
          
interrupt (((0x10000 - Vporth)/2)-1) void prth_ISR(void){
   
  PIFH = PIFH | 0xFF;
  
}       


interrupt (((0x10000 - Vtimovf)/2)-1) void overflow_ISR(void){
  
  
    if(overflow == 367) 
    {
        overflow = 0;
        
    } 
    else
    { 
        ++overflow;
    }
    
    TFLG2 = TFLG2 | 0x80; //clear
  
}       
