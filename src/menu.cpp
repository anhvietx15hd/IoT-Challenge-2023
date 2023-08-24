/**********************************************************
 * INcludes
**********************************************************/
#include "menu.h"
/**********************************************************
 * Variables
**********************************************************/
static long lastUpdateDisplay = 0;
/**********************************************************
 * Prototypes
**********************************************************/

/**
 * @brief Update the parameters displayed on screen
 * 
 */
static void updateDisplay(void);
/**********************************************************
 * Codes
**********************************************************/
void display(int cursorLine0, int cursorLine1, String textLine0, String textLine1){
  LCD.clear();
  LCD.setCursor(cursorLine0, 0);
  LCD.print(textLine0);
  LCD.setCursor(cursorLine1, 1);
  LCD.print(textLine1);
}

void parametersDisplay(void){
  LCD.setCursor(8, 0);
  LCD.print("Tem:");
  LCD.setCursor(8, 1);
  LCD.print("Hum:");
  LCD.setCursor(0, 0);
  LCD.print("L1:");
  LCD.setCursor(0, 1);
  LCD.print("L2:");
  updateDisplay();
}

static void updateDisplay(void){
  long now = millis();
  if(now - lastUpdateDisplay > TIME_TO_UPDATE_DISPLAY){
    LCD.setCursor(12, 0);
    LCD.print("    ");
    LCD.setCursor(12, 1);
    LCD.print("    ");

    LCD.setCursor(12, 0);
    LCD.print(temperature);
    LCD.setCursor(12, 1);
    LCD.print(humidity);
    lastUpdateDisplay = now;
  }

  if (ceilingLightStatus == LIGHT_ON) {   // state On
    LCD.setCursor(4, 0);
    LCD.print("ON");
  }
  else  {
    LCD.setCursor(4, 0);
    LCD.print("OFF");
  }
  if (wallLightStatus == LIGHT_ON) {   // state On
    LCD.setCursor(4, 1);
    LCD.print("ON");
  }
  else  {
    LCD.setCursor(4, 1);
    LCD.print("OFF");
  }
  delay(100);
}

/**********************************************************
 * End of file
**********************************************************/
