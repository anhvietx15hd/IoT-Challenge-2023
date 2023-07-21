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

Adafruit_INA219 ina219;

LiquidCrystal_I2C LCD(0X27,16,2);
DHT dht(DHT_SENSOR, DHT_TYPE);
// Create a JSON document to write sensor data
DynamicJsonDocument doc(256); 

bool isConfigButtonPressed = false;
/*The last time a message was published the server*/
long lastSentMsg = 0;

float power = 0;
bool lightSensor;
uint8_t gas = 0;
bool humanDetected = false;
float humidity = 0;
float temperature = 0;
/*Device status*/
bool ceilingLightStatus = true; //Normal Close
bool wallLightStatus = true; //Normal Close
bool security = 0;

float waterTemperature = 0.0;

bool temperatureSensor_Active = true;

int16_t timeToUpdate = 3000; //Time to update the data by default (MiliSeconds)

void buttonPressedInterrupt(){
    isConfigButtonPressed = true;
}

void setup(){
    Serial.begin(115200);
    pinMode(STATUS_LED_RED, OUTPUT);
    pinMode(STATUS_LED_GREEN, OUTPUT);
    pinMode(STATUS_LED_BLUE, OUTPUT);

    pinMode(CEILING_LIGHT, OUTPUT);
    pinMode(WALL_LIGHT, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    pinMode(GAS_SENSOR, INPUT);
    pinMode(LIGHT_SENSOR, INPUT);
    /*Turn off the lights at the start*/
    digitalWrite(CEILING_LIGHT, HIGH); 
    digitalWrite(WALL_LIGHT, HIGH);

    /*Set up device*/
    while(! ina219.begin()){
        Serial.println("Failed to find INA219 chip");
        delay(10);
    }

    dht.begin();

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
  xTaskCreatePinnedToCore(
    DataProcessing
    ,  "Data_Processing"
    ,  4000  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL
    ,  1);
    delay(500);
}

void loop(){
    if(isConfigButtonPressed == true){
        getConfig();
    }
    controlDevice();
    client.loop();
}
/**********************************************************
 * End of file
**********************************************************/
