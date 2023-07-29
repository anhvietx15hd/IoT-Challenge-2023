/************************************************
 * Includes
************************************************/
#include <Arduino.h>
#include "HomeAutomation.h"
/************************************************
 * Variable
************************************************/
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

    if (strcmp(str, "ON_ceilingLight") == 0){

        ceilingLightStatus = LIGHT_ON;
    }
    else if (strcmp(str, "OFF_ceilingLight") == 0){
        ceilingLightStatus = LIGHT_OFF;
    }
    else if (strcmp(str, "ON_wallLight") == 0){
        wallLightStatus = LIGHT_ON;
    }
    else if(strcmp(str, "OFF_wallLight") == 0){
        wallLightStatus = LIGHT_OFF;    }
    else if (strcmp(str, "ON_yardLight") == 0){
        yardLightStatus = LIGHT_ON;
    }
    else if (strcmp (str, "OFF_yardLight") == 0){
        yardLightStatus = LIGHT_OFF;
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

void ReadSensors(void *pvParameters)
{
    (void) pvParameters;
    while(true){
        long now = millis();
        
        client.loop();
        client.subscribe(subscribe_topic.c_str());

        power = ina219.getPower_mW();
        humidity = dht.readHumidity();
        temperature = dht.readTemperature();
        lightSensor = !digitalRead(LIGHT_SENSOR);  // LightSensor
        gas = !digitalRead(GAS_SENSOR);    // co2_value: 1 -> có CO2 ; 0 -> ko có Co2
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
    doc["temperature"]  = temperature;
    doc["lightSensor"] = lightSensor;
    doc["gas"] = gas;
    doc["humanDetected"] = humanDetected;
    doc["humidity"] = humidity;
    doc["power"] = power;
    String message;
    serializeJson(doc, message);
    // Serial.println(message);
    //Send data to publish topic
    client.publish(publish_topic.c_str(), message.c_str());
    digitalWrite(STATUS_LED_GREEN, HIGH);
}

void buzzer(void){
    digitalWrite(BUZZER, 1);
    delay(100);
    digitalWrite(BUZZER, 0);
    delay(100);
}

void controlDevice(void){
    /*Update the light state with local switch*/
    StaticJsonDocument<256> state_light_Json;
    if(digitalRead(CEILING_LIGHT_SWITCH) != ceilingLightSwitchStatus){
        ceilingLightStatus = (! ceilingLightStatus);
        ceilingLightSwitchStatus = digitalRead(CEILING_LIGHT_SWITCH);
        sendLightStatus();

    }
    if(digitalRead(WALL_LIGHT_SWITCH) != wallLightSwitchStatus){
        wallLightStatus = (! wallLightStatus);
        wallLightSwitchStatus = digitalRead(WALL_LIGHT_SWITCH);
        sendLightStatus();
    }
    /*Update to the relay*/
    digitalWrite(CEILING_LIGHT, ceilingLightStatus);
    digitalWrite(WALL_LIGHT, wallLightStatus);
}

static void sendLightStatus(void){
    lightStatus["ceilingLightStatus"] = (! ceilingLightStatus);
    lightStatus["wallLightStatus"]    = (! wallLightStatus);
    
    String message;
    serializeJson(lightStatus, message);
    /*Send data to server*/
    client.publish("LightStatus", message.c_str());
    /*Set the new curSor*/
    LCD.setCursor(3, 1);
    LCD.print("     ");
}
/**********************************************************
 * End of file
**********************************************************/
