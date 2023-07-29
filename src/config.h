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
#define temperaturePin                  5u
#define BUZZER                          17u
#define LIGHT_SENSOR                    19u
#define GAS_SENSOR                      18u
#define HUMAN_DETECT                    4u
#define CEILING_LIGHT                   15u
#define WALL_LIGHT                      16u
#define DHT_SENSOR                      27u
#define DHT_TYPE                        DHT22
#define STATUS_LED_RED                  33u
#define STATUS_LED_GREEN                32u
#define STATUS_LED_BLUE                 25u
#define CONFIG_BUTTON                   35u
#define CEILING_LIGHT_SWITCH            13u
#define WALL_LIGHT_SWITCH               34u

#endif /* Config*/
/***********************************************
 * End of file
***********************************************/