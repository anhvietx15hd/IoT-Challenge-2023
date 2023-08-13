/**********************************************************
 * INcludes
**********************************************************/
#include "menu.h"
/**********************************************************
 * Variables
**********************************************************/
static long lastUpdateDisplay = 0;
static long lastRestartDisplay = 0;
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
  LCD.print("Lock:");
  LCD.setCursor(8, 1);
  LCD.print("Ser:");
  LCD.setCursor(0, 0);
  LCD.print("L1:");
  LCD.setCursor(0, 1);
  LCD.print("L2:");
  updateDisplay();
}

static void updateDisplay(void){
  long now = millis();
  if(now - lastUpdateDisplay > TIME_TO_UPDATE_DISPLAY){

    LCD.setCursor(13, 0);
    LCD.print("    ");
    LCD.setCursor(12, 1);
    LCD.print("    ");
    if (state_Door == 1) {
      LCD.setCursor(13, 0);
      LCD.print("ON");
    }
    else {
      LCD.setCursor(13, 0);
      LCD.print("OFF");
    }
    

    lastUpdateDisplay = now;
  }
    if(now - lastRestartDisplay > TIME_TO_RESTART_DISPLAY){

    LCD.clear();
    LCD.setCursor(13, 0);
    LCD.print("    ");
    LCD.setCursor(12, 1);
    LCD.print("    ");
    if (state_Door == 1) {
      LCD.setCursor(13, 0);
      LCD.print("ON");
    }
    else {
      LCD.setCursor(13, 0);
      LCD.print("OFF");
    }


    lastRestartDisplay = now;
  }

  if (yardLightStatus == LIGHT_ON) {   // state On

    LCD.setCursor(4, 0);
    LCD.print("ON");
    LCD.setCursor(3, 0);
    LCD.print(" ");
    LCD.setCursor(6, 0);
    LCD.print(" ");
  }
  else  {
    LCD.setCursor(4, 0);
    LCD.print("OFF");
    LCD.setCursor(3, 0);
    LCD.print(" ");
    LCD.setCursor(7, 0);
    LCD.print(" ");
  }
  if (digitalRead(HALLWAY_LIGHT) == LIGHT_ON) {   // state On
    LCD.setCursor(4, 1);
    LCD.print("ON");
    LCD.setCursor(3, 1);
    LCD.print(" ");
    LCD.setCursor(6, 1);
    LCD.print(" ");
  }
  else  {
    LCD.setCursor(4, 1);
    LCD.print("OFF");
    LCD.setCursor(3, 1);
    LCD.print(" ");
    LCD.setCursor(7, 1);
    LCD.print(" ");
  }
  delay(100);
}

/**********************************************************
 * End of file
**********************************************************/
