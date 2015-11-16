/* 
 * File:   LCD_SK1632.h
 * Author: User
 *
 * Created on December 4, 2014, 10:16 AM
 */

#ifndef LCD_SK1632_H
#define	LCD_SK1632_H

//===============================================================================
//	LCD Functions
//===============================================================================
void lcd_init();
void lcd_4bit_write(unsigned char data);
void lcd_config(unsigned char command);
void lcd_putchar(unsigned char data);
void lcd_putstr(const char *str);
void lcd_clear(void);
void lcd_home(void);
void lcd_set_cursor(unsigned char uc_column, unsigned char uc_row);
void lcd_goto(unsigned char address);
void lcd_e_clock(void);

#endif	/* LCD_SK1632_H */