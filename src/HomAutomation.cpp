/************************************************
 * Definition
************************************************/
#include <Arduino.h>
#include "HomeAutomation.h"
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
        ceilingLightStatus = false;
        display(0, 0, "ON", "CeilingLight");
    }
    else if (strcmp(str, "OFF_ceilingLight") == 0){
        ceilingLightStatus = true;
        display(0, 0, "OFF", "CeilingLight");
    }
    else if (strcmp(str, "ON_wallLight") == 0){
        wallLightStatus = false;
        display(0, 0, "ON", "WallLight");
    }
    else if(strcmp(str, "OFF_wallLight") == 0){
        wallLightStatus = true;
        display(0, 0, "OFF", "OFFLight");
    }
    else if (str == "ON_security"){
        security = true;
        display(0, 0, "ON", "ON_security");
    }
    else if (str == "OFF_security"){
        security = false;
        display(0, 0, "OFF", "ON_security");
    }
    // else{
    //     getTimeToUpdate(message);
    // }
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
        power = ina219.getPower_mW();
        humidity = dht.readHumidity();
        temperature = dht.readTemperature();
        lightSensor = !digitalRead(LIGHT_SENSOR);  // LightSensor
        gas = !digitalRead(GAS_SENSOR);    // co2_value: 1 -> có CO2 ; 0 -> ko có Co2
        humanDetected = digitalRead(HUMAN_DETECT);
        vTaskDelay(500);
    }
}

void DataProcessing(void *pvParameters){
    (void) pvParameters;

    while(true){
        if (!client.connected()) {
            setupMQTTConnection();
        }
        client.loop();
        client.subscribe(subscribe_topic.c_str());
        
        long now = millis();
        if(now - lastSentMsg > timeToUpdate){
            sendSensorsData();
        }
        if (now - lastSentMsg >=1000){
        digitalWrite(STATUS_LED_GREEN, LOW);
        }
        vTaskDelay(1000);
    }
}

static void sendSensorsData(void){
    lastSentMsg = millis();
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
    
}
/**********************************************************
 * End of file
**********************************************************/
