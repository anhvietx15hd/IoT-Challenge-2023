#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#ifndef _MENU_
#define __MENU_
extern LiquidCrystal_I2C LCD;
/**
 * @brief This function to display data to LCD 16x2
 * 
 * @param cursorLine0 
 * @param cursorLine1 
 * @param textLine0 
 * @param textLine1 
 */
void display(int cursorLine0, int cursorLine1, String textLine0, String textLine1);
#endif /*Menu*/
/**********************************************************
 * End of file
**********************************************************/
