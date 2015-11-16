/*
 * File: LCD_SK1632.c
 * Author: Tony Ng
 * Software: MPLAB X IDE V2.15
 * Compiler: XC8 V1.32
 * Created on December 4, 2014
 */

#include <xc.h>

//========================================================//
                //Pin Declaration//
//========================================================//
#define LCD_RS      LATBbits.LATB2
#define LCD_E       LATBbits.LATB3          // LCD 4-bit
#define LCD_D4      LATBbits.LATB4
#define LCD_D5      LATBbits.LATB7
#define LCD_D6      LATBbits.LATB13
#define LCD_D7      LATBbits.LATB14
#define LCD_DATA    LATB


//========================================================//
                //Function Prototypes//
//========================================================//
unsigned char LCD_RS_bit = 0;
unsigned char LCD_RS_PIN = 0;

//===============================================================================
//	LCD Functions
//===============================================================================
//4-bit mode configuration
void lcd_init()
{
    TRISBbits.TRISB2 = 0;        // LCD Output
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB7 = 0;
    TRISBbits.TRISB13 = 0;
    TRISBbits.TRISB14 = 0;

    DelayMs(30);	//wait for 30ms after power ON for LCD internal controller to initialize itself
    LCD_E = 0;
    //Set lcd to configuration mode
    LCD_RS = 0;		//Selected command register
    DelayUs(1); 	//macro from HITECH compiler to generate code to delay for 1 microsecond base on _XTAL_FREQ value

    LCD_DATA = 0x80;     //make it in 4-bit mode
    lcd_e_clock();
    DelayMs(2);

    LCD_DATA = 0x80;
    lcd_e_clock();
    DelayMs(2);

    LCD_DATA = 0x80;
    lcd_e_clock();
    DelayMs(2);

    //start sending command in 4 bit mode
    //Function Set
    lcd_config(0b00101000);	 //0b 0 0 1 DL N F X X
                                                //Interface Data Length, ID= 4-bit
                                                //Number of line to display, N = 1 is 2 line display, N = 0 is 1 line display
                                                //Display Font, F = 0 is 5x 8 dots, F = 1 is 5 x 11 dots

    //Command Entry Mode
    lcd_config(0b00000110);	//0b 0 0 0 0 0 1 ID SH
                                                //ID  = 1, cursor automatic move to right, increase by 1
                                                //SH = 0,  shift of entire display is not perform

    //Display Control
    lcd_config(0b00001111);	//0b 0 0 0 0 1 D C B
                                                //D  = 1, Display is ON
                                                //C = 0,  Cursor is not display
                                                //B = 0. Cursor does not blink

    lcd_clear();	//clear LCD and move the cursor back to home position
}

//Routine to send data to LCD via 2 nibbles in 4-bit mode
void lcd_4bit_write(unsigned char data)
{
    unsigned  long data1,data2;

    if(LCD_RS_bit == 1)
        LCD_RS_PIN = 0b100;
    else LCD_RS_PIN = 0;

    //send out the Most Significant Nibble
    data1 = ((data & 0xF0) >> 4);
    data2  = (((data1 & 0b1000) | (data1 & 0b0100)) << 11);
    LCD_DATA = (data2 | (((data1 & 0b0010) << 6) | ((data1 & 0b0001) << 4))) + LCD_RS_PIN;
    lcd_e_clock();

    //send out the Least Significant Nibble
    data1 = (data & 0x0F);
    data2  = (((data1 & 0b1000) | (data1 & 0b0100)) << 11);
    LCD_DATA = (data2 | (((data1 & 0b0010) << 6) | ((data1 & 0b0001) << 4))) + LCD_RS_PIN;
    lcd_e_clock();
}

//Routine to send command to LCD
void lcd_config(unsigned char command)
{
    LCD_RS_bit = 0;   			//Selected command register
    DelayUs(5);
    lcd_4bit_write(command);	//Send command via 2 nibbles
    DelayMs(1);
}

//Routine to send display data (single character) to LCD
void lcd_putchar(unsigned char data)
{
    LCD_RS_bit = 1;        			//Selected data register
    DelayUs(5);
    lcd_4bit_write(data);		//Send display via 2 nibbles
    DelayMs(1);
}

//Routine to send string to LCD
void lcd_putstr(const char *str)
{
    while(*str != '\0')		//loop till string ends
    {
    lcd_putchar(*str++);  		//send characters to LCD one by one
    }
}

//Routine to clear the LCD
void lcd_clear(void)
{
	lcd_config(0x01);			//command to clear LCD
	DelayMs(1);
}
//function to move LCD cursor to home position
void lcd_home(void)
{
	lcd_config(0x02);			//command to move cursor to home position
  	DelayMs(1);
}
//Rountine to set cursor to the desired position base on coordinate, column and row
/*Place a string in the specified row and column of the screen.
* +--+--+--+--+--+---------------------+
* |0 |1 |2 |3 |4 |5 ...etc             | <- row 0
* +--+--+--+--+--+---------------------+
* |0 |1 |2 |3 |4 |5 ...etc             | <- row 1
* +--+--+--+--+--+---------------------+
*/
void lcd_set_cursor(unsigned char uc_column, unsigned char uc_row)
{
    if(uc_row == 0) lcd_config(0x80 + uc_column);	//command to move cursor to first row/line with offset of column
    else if(uc_row ==1 ) lcd_config(0xC0 + uc_column);	//command to move cursor to 2nd row/line with offset of column
}

//Rountine to set cursor to the desired position base on LCD DDRAM address
/*Place a string in the specified row and column of the screen.
* +--+--+--+--+--+---------------------+
* |0x00 |0x01 |0x02 |0x03 |0x04 |0x05  ...etc          |0x0F| <- 1st line
* +--+--+--+--+--+---------------------+
* |0x40 |0x41 |0x42 |0x43 |0x44 |0x45 ...etc           |0x4F| <- 2nd line
* +--+--+--+--+--+---------------------+
*/
void lcd_goto(unsigned char address)
{
     lcd_config(0x80 + address);    //command to move cursor to desire position base on the LCD DDRAM address
}

//function to output enable clock pulse to LCD
void lcd_e_clock(void)
{
	DelayUs(10);
	LCD_E = 1;	//pull the Enable pin high again
	DelayUs(100);
        LCD_E = 0;	//create a falling edge for Enable pin of LCD to process data
        DelayUs(100);
}