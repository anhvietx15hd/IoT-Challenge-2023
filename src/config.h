#ifndef _CONFIG_
#define _CONFIG_
#include <Arduino.h>
#include <PubSubClient.h>
#include <WebServer.h>
#include <DHT.h>
/*************************************************
 * Config for project
*************************************************/
/*Address of the first memory cell in EEPROM*/
#define WIFI_SSID_ADDRESS               0u
#define WIFI_PASSWORD_ADDRESS           32u
#define MQTT_ADDRESS                    64u
#define MQTT_USERNAME_ADDRESS           96u
#define MQTT_PASSWORD_ADDRESS           128u
#define MQTT_PORT_ADDRESS               160u
#define MQTT_PUBLISH_TOPIC_ADDRESS      192u
#define MQTT_SUBSCRIBE_TOPIC_ADDRESS    224u

/*Device pin*/
#define BUZZER                          17u
#define LIGHT_SENSOR                    19u
#define HUMAN_DETECT                    4u
#define HALLWAY_LIGHT                   15u
#define YARD_LIGHT                      16u
#define STATUS_LED_RED                  26u
#define STATUS_LED_GREEN                25u
#define STATUS_LED_BLUE                 32u
#define CONFIG_BUTTON                   27u
#define CEILING_LIGHT_SWITCH            13u
#define DOOR                            33u
/*Light state*/
#define LIGHT_ON                        LOW
#define LIGHT_OFF                       HIGH
#define WARNING_ON                      HIGH
#define WARNING_OFF                     LOW
/*Time to update display*/
#define TIME_TO_UPDATE_DISPLAY          5000u
#define TIME_TO_RESTART_DISPLAY         62500u    
#define TIME_TO_DELAY_BUZZER            100U

#endif /* Config*/
/***********************************************
 * End of file
***********************************************/