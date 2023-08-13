/************************************************
 * Includes
************************************************/
#include <Arduino.h>
#include "HomeAutomation.h"
/************************************************
 * Variable
************************************************/
long lastTimeBuzzer;
static DynamicJsonDocument lightStatus(256);
/***********************************************
 * Prototype
************************************************/

/**
 * @brief Thí function to extract server message
 * 
 * @param message 
 */
static void getActiveStatus(String &message);

/**
 * @brief Get time to update from message
 * 
 * @param message 
 */
static void getTimeToUpdate(String &message);

/**
 * @brief Collect and send sensors data
 * 
 */
static void sendSensorsData(void);

/**
 * @brief Enable Buzzer
 * 
 */
void buzzer(void);

/**
 * @brief doorOpen
 * 
 */
void doorOpen(void);

/**
 * @brief doorClose
 * 
 */
void doorClose(void);

/**
 * @brief Security
 * 
 */
void Security(void);

/**
 * @brief Collect and send light status to server as a JSON document
 * Relays are nomal close, so have to send inverse value of light status
 */
static void sendLightStatus(void);
/************************************************
 * Codes
************************************************/
void callback(char* topic, byte* payload, uint16_t length) {
    uint16_t i;
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    String message = "";
    for (i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
    }
    Serial.println();
    getActiveStatus(message);
}

static void getActiveStatus(String &message){
    const char* str = message.c_str();
    Serial.println(str);

    if (strcmp(str, "OpenDoor") == 0){
        doorOpen();
        state_Door = 1;
    }
    else if (strcmp(str, "OpenClose") == 0) {
        doorClose();
        state_Door = 0;
    }

    else{
        getTimeToUpdate(message);
    }
    Serial.println("______________________________________________");
}

static void getTimeToUpdate(String &message){
    StaticJsonDocument<250> sub_doc;
    deserializeJson(sub_doc, message.c_str());
    // Get the time to update the sensors data from server
    timeToUpdate = sub_doc["timeToUpdate"];
    Serial.println("Successfully set time to update the new data to server to " + String(timeToUpdate) + " miliSeconds");
    display(0, 4, "Time to update:", String(timeToUpdate) + " ms");
}
void Security(void) {
    if (warning_Security == WARNING_ON) {
    for (int i= 0; i<10; i++)
    buzzer();
    }
}

void ReadSensors(void *pvParameters)
{
    (void) pvParameters;
    while(true){
        long now = millis();
        
        client.loop();
        client.subscribe(subscribe_topic.c_str());
        hallwayLightSwitchStatus = digitalRead(HUMAN_DETECT);
        yardLightSwitchStatus = digitalRead(LIGHT_SENSOR);
        power = ina219.getPower_mW();
        lightSensor = digitalRead(LIGHT_SENSOR);  // LightSensor
        humanDetected = digitalRead(HUMAN_DETECT);

        if((now - lastSentMsg > timeToUpdate) && (client.connected())){
            sendSensorsData();
            lastSentMsg = now;
        }
        if (now - lastSentMsg >=1000){
        digitalWrite(STATUS_LED_GREEN, LOW);
        }
        vTaskDelay(1000);
    }
}

static void sendSensorsData(void){
    doc["lightSensor"] = lightSensor;
    doc["humanDetected"] = humanDetected;
    doc["power"] = power;
    doc["yardlight"] = yardLightSwitchStatus;
    doc["hallwaylight"] = hallwayLightSwitchStatus;
    String message;
    serializeJson(doc, message);
    // Serial.println(message);
    //Send data to publish topic
    client.publish("outdoor", message.c_str());
    if (digitalRead(STATUS_LED_RED == 0))
    digitalWrite(STATUS_LED_GREEN, HIGH);
}

void buzzer(void){
    long now = millis();
    if(now - lastTimeBuzzer > TIME_TO_DELAY_BUZZER){
    digitalWrite(BUZZER, !digitalRead(BUZZER));

    lastTimeBuzzer = now;
  }
}

void controlDevice(void){
    /*Update the light state with local switch*/
    if(digitalRead(YARD_LIGHT) != yardLightSwitchStatus){
        yardLightSwitchStatus = digitalRead(LIGHT_SENSOR);
        yardLightStatus = !yardLightSwitchStatus;
    }
    if(digitalRead(HALLWAY_LIGHT) != hallwayLightSwitchStatus){
        hallwayLightSwitchStatus = digitalRead(HUMAN_DETECT);
        hallwayLightStatus = !hallwayLightSwitchStatus;
        //if (digitalRead(HUMAN_DETECT) == 1) Flag_wait = 1;

    }
    /*Update to the relay*/
    if (digitalRead(HUMAN_DETECT) == 1)  {
        Flag_wait = 1;
        time_hallWall = millis();
    }
    if (Flag_wait == 1) {
        long now = millis();
        digitalWrite(HALLWAY_LIGHT, LIGHT_ON);
        if((now - time_hallWall > 15000) ){
        digitalWrite(HALLWAY_LIGHT, LIGHT_OFF);
        Flag_wait = 0;
        time_hallWall = now;
        }

    }
    digitalWrite(YARD_LIGHT, yardLightStatus);
    Security();
}
void doorOpen(void) {
    for (int pos =180; pos >= 90; pos -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

void doorClose(void) {
    for (int pos = 90; pos <= 179; pos += 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

/**********************************************************
 * End of file
**********************************************************/
