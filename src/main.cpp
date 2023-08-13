#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WebServer.h>
#include "config.h"
#include "ConnectionConfig.h"
#include "menu.h"
#include "HomeAutomation.h"
#include <ESP32_Servo.h> 

/*Wifi*/
String wifi_ssid = "";
String wifi_password = "";
/*MQTT*/
String mqtt_server = "";
String mqtt_username = "";
String mqtt_password = "";
String mqtt_port = "";
String publish_topic = "";
String subscribe_topic = "";

WebServer server(80);
uint16_t statusCode;

WiFiClient espClient;
PubSubClient client(espClient);
Servo myservo;
Adafruit_INA219 ina219;

LiquidCrystal_I2C LCD(0X27,16,2);
// Create a JSON document to write sensor data
DynamicJsonDocument doc(256); 

bool isConfigButtonPressed = false;
/*The last time a message was published the server*/
long lastSentMsg = 0;

float power = 0;
bool lightSensor;
bool humanDetected = false;
/*Device status*/
bool ceilingLightStatus = LIGHT_OFF; //Normal Close
bool wallLightStatus = LIGHT_OFF; //Normal Close
bool yardLightStatus = LIGHT_OFF;
bool yardLightSwitchStatus;
bool hallwayLightSwitchStatus;
bool hallwayLightStatus;
bool Flag_wait;
bool state_Door;


bool warning_Security = WARNING_OFF;
long time_hallWall;

int16_t timeToUpdate = 3000; //Time to update the data by default (MiliSeconds)

void buttonPressedInterrupt(){
    isConfigButtonPressed = true;
}

void setup(){
    Serial.begin(115200);
    pinMode(STATUS_LED_RED, OUTPUT);
    pinMode(STATUS_LED_GREEN, OUTPUT);
    pinMode(HALLWAY_LIGHT, OUTPUT);
    pinMode(YARD_LIGHT, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    pinMode(LIGHT_SENSOR, INPUT);
    pinMode(HUMAN_DETECT, INPUT);


    /*Turn off the lights at the start*/
    digitalWrite(HALLWAY_LIGHT, LIGHT_OFF); 
    digitalWrite(YARD_LIGHT, LIGHT_OFF);
    digitalWrite(STATUS_LED_RED, HIGH);
    digitalWrite(STATUS_LED_GREEN, LOW);
    /*Update the switch status*/
    yardLightSwitchStatus = digitalRead(LIGHT_SENSOR);
    hallwayLightSwitchStatus = digitalRead(HUMAN_DETECT); 
    Flag_wait = 0;
    myservo.attach(DOOR, 500, 2400);
    /*Set up device*/
    while(! ina219.begin()){
        Serial.println("Failed to find INA219 chip");
        delay(10);
    }
    myservo.write(180);  
    LCD.init();      
    LCD.backlight(); 
    // WiFi.disconnect();

    EEPROM.begin(512);
    pinMode(CONFIG_BUTTON, INPUT);
    attachInterrupt(CONFIG_BUTTON, buttonPressedInterrupt, RISING);

    Serial.println("\n");
    Serial.println("Starting up");

    readEEPROM();
    setupWifi();

    client.setServer(mqtt_server.c_str(), mqtt_port.toInt()); 
    client.setCallback(callback);

    xTaskCreatePinnedToCore(
    ReadSensors
    ,  "Read_Sensor"   // A name just for humans
    ,  4000  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
    ,  1);
}

void loop(){

    if(isConfigButtonPressed == true){
        getConfig();
    }
    if (!client.connected()) {
        setupMQTTConnection();
    }
    controlDevice();
    parametersDisplay();
    client.loop();
}
/**********************************************************
 * End of file
**********************************************************/
