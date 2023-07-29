#include <Arduino.h>
#include "menu.h"
void display(int cursorLine0, int cursorLine1, String textLine0, String textLine1){
  LCD.clear();
  LCD.setCursor(cursorLine0, 0);
  LCD.print(textLine0);
  LCD.setCursor(cursorLine1, 1);
  LCD.print(textLine1);
}



/**********************************************************
 * End of file
**********************************************************/
