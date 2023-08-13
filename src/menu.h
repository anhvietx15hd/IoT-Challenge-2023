/**********************************************************
 * Inlcudes
**********************************************************/
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"
#ifndef _MENU_
#define __MENU_
/**********************************************************
 * APIs
**********************************************************/
extern LiquidCrystal_I2C LCD;
extern float humidity;
extern float temperature;
extern bool ceilingLightStatus ; //Normal Close
extern bool wallLightStatus ;
extern bool Flag_warning;
extern long time_warning;
/**
 * @brief This function to display data to LCD 16x2
 * 
 * @param cursorLine0 
 * @param cursorLine1 
 * @param textLine0 
 * @param textLine1 
 */
void display(int cursorLine0, int cursorLine1, String textLine0, String textLine1);

/**
 * @brief Display environment parameters
 * 
 */
void parametersDisplay(void);
#endif /*Menu*/
/**********************************************************
 * End of file
**********************************************************/
