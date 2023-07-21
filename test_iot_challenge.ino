// import library
#include <ArduinoJson.h>
#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <SimpleKalmanFilter.h>
#include <LiquidCrystal_I2C.h>
#include "Adafruit_INA219.h"
#include <math.h>
#include "DHT.h"
// Notify: Adafruit_ADS1X15.h modified

// define pin Button
#define BUZZER_PIN 17
#define LightSensor_PIN 19
#define GasSensor_Pin 18
#define HumanDetect_Pin 4
#define Relay1 15
#define Relay2 16
#define DHTPIN 27
#define ledRed 25
#define ledGreen 33
#define ledBlue 26
#define btnConfig 35

#define DHTTYPE DHT22


const char* ssid = "Hoai Nam 2.4";
const char* password = "10101999";
// Add your MQTT Broker IP address, example:
const char* mqtt_server = "hungviet.hopto.org";
const int port_id = 1883;
const char* ssid_server = "vikings";
const char* pass_server = "19013005";

Adafruit_INA219 ina219;
StaticJsonDocument<256> JSONbuffer;
//JsonObject& JSONencoder = JSONbuffer.createObject();
JsonObject JSONencoder = JSONbuffer.to<JsonObject>();
WiFiClient espClient;
PubSubClient client(espClient);
SimpleKalmanFilter bo_loc(2, 2, 0.005);
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

// TẠO BIẾN TRỮ DỮ LIỆU GỬI ĐI

long lastMsg = 0;
long time_send_data = 0;

// CALIB CURRENT
float ofset_current1 = 2.5234;
float scale_current1 = 6.04773741406;
float ofset_current2 = 2.54;
float scale_current2 = 6.04773741406;

// create varible meter

float power_mW = 0;
bool LightSensor;
int co2_value = 0;
bool Human_detect = 0;
float humidity = 0;
float temperature = 0;
char temperature_str[10];
String temptest  = "NoHeat";

// CREATE VARIBLE SETTING

bool stateRelay1 = 1;   // 1 is off
bool stateRelay2 = 1;
bool security = 0;
bool enable_relay2 = 0;



// SETTING PHÉP ĐO SOC


// STATE OF BUTTON CONTROL

String state_str;   // STATE STRING OF

// END define varible

//void IRAM_ATTR isr1() {
//  pos = 1;
//
//}

// update Menu function
void updateMenu() {
  lcd.setCursor(12, 0);
  lcd.print("    ");
  lcd.setCursor(12, 1);
  lcd.print("    ");
  lcd.setCursor(3, 0);
  lcd.print("    ");
  lcd.setCursor(3, 1);
  lcd.print("    ");

  lcd.setCursor(12, 0);
  lcd.print(temperature_str);
  lcd.setCursor(12, 1);
  lcd.print(humidity);
  if (stateRelay1 == 0) {   // state On
    lcd.setCursor(4, 0);
    lcd.print("ON");
  }
  else  {
    lcd.setCursor(4, 0);

    lcd.print("OFF");
  }
  if (stateRelay2 == 0) {   // state On
    lcd.setCursor(4, 1);
    lcd.print("ON");
  }
  else  {
    lcd.setCursor(4, 1);

    lcd.print("OFF");
  }
  delay(100);
}
unsigned long time1 = 0;
void display_info()   // auto display parameter
{
  //   lcd.setCursor(10, 2);
  //   lcd.print("V");

}



void setup() {
  delay(1000);
  Serial.begin(115200);
  delay(500);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LightSensor_PIN, INPUT);
  pinMode(GasSensor_Pin, INPUT);
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  digitalWrite(Relay1, 1); // turn off relay2
  digitalWrite(Relay2, 1); // turn off relay2
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  digitalWrite(ledRed, 1);
  digitalWrite(ledGreen, 1);
  digitalWrite(ledBlue, 1);
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) {
      delay(10);
    }
  }
  dht.begin();

  lcd.init();
  lcd.backlight();



  lcd.print("Id:");
  lcd.setCursor(4, 0);
  lcd.print(ssid);
  lcd.setCursor(0, 1);
  lcd.print("Pass:");
  lcd.setCursor(6, 1);
  lcd.print(password);
  lcd.setCursor(0, 2);
  lcd.print("Connecting...");
  delay(1000);
  setup_wifi();
//  attachInterrupt(button_down, isr1, FALLING);  // ATTACH INTERRUPT FUNCTION

  client.setServer(mqtt_server, port_id);
  client.setCallback(callback);
  updateMenu();
  xTaskCreatePinnedToCore(
    Read_ADC
    ,  "Read_And_Send_Data"   // A name just for humans
    ,  4000  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
    ,  1);

  lcd.clear();
  delay(500);


}



long time_restart = 0;  // TIME TO AUTO RESTART WHEN CAN'T CONNECT AFTER A PERIOD OF TIME
void setup_wifi() {   // CONNECT TO WIFI
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  delay(100);
  int m = 13, n = 2;
  WiFi.begin(ssid, password);
  delay(1000);
  time_restart = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(m, n);
    lcd.print(".");
    m++;
    if (m  == 20) {
      m = 0;
      n++;
      if (n == 4) n = 0;
    }
    if (millis() - time_restart > 10000)

      ESP.restart();
    time_restart = millis();

  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(2, 1);
  lcd.clear();
  lcd.print("Connected!");
  delay(3000);
  lcd.clear();

}
// RECEIVED MESSAGE IN SERVER
void callback(char* topic, byte * message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (messageTemp == "OnRelay1")  //COMMAND OF SERVER
  {
    stateRelay1 = 0;
  }
  if (messageTemp == "OffRelay1")
  {
    stateRelay1 = 1;
  }
  if (messageTemp == "OnRelay2")
  {
    stateRelay2 = 0;
  }
  if (messageTemp == "OffRelay2")
  {
    stateRelay2 = 1;
  }
  if (messageTemp == "SecurityOn")
  {
    security = 1;
  }
  if (messageTemp == "SecurityOff")
  {
    security = 0;
  }
  if (messageTemp == "testwarning")
  {
    temptest  = "Heat";
  }


}

void xulyJson(String msgdata) {   // CONVERT JSON TO VARIBLE
  DynamicJsonDocument doc(1024);

  deserializeJson(doc, msgdata);
  JsonObject obj = doc.as<JsonObject>();
  //  setpoint_cc_cv_1 = float(obj["SETPOINT_CC_CV_1"]);
  //  ocv_chg_max_vol_1 = float(obj["OCV_CHG_MAX_VOL_1"]);
  //
  //
  //
  //  Serial.print("SETPOINT_CC_CV_1"); Serial.println(setpoint_cc_cv_1);
  //  Serial.print("OCV_CHG_MAX_VOL_1"); Serial.println(ocv_chg_max_vol_1);
  Serial.println(msgdata.length());


  /*
        "status": sst,
        "setcurr": setcurr,
        "setsetvolt": setsetvolt,
        "setstartstep": setstartstep,
        "setmaxcharvolt": setmaxcharvolt,
        "setcccvpoint": setcccvpoint,
        "setmincharcurr": setmincharcurr,
        "setdeadtime": setdeadtime,
        "setdcchrcurr": setdcchrcurr,
        "setcycle": setcycle
  */
}
// reconnect wifi when disconnect
long time_wifi = 0;   // time wait reconnect wifi
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {

    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client_Charge_Dchg_1", ssid_server, pass_server)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
      client.publish("esp32/status", "Connect Success");
      digitalWrite(ledRed, 1);
      digitalWrite(ledGreen, 0);
      digitalWrite(ledBlue, 1);
      delay(2000);
      client.publish("esp32/status", "Connect Failed");

    } else {

      long now = millis();
      if (now - time_wifi > 15000)
      {
        setup_wifi();
        time_wifi = now;
        digitalWrite(ledRed, 0);
        digitalWrite(ledGreen, 1);
        digitalWrite(ledBlue, 1);

        delay(5000);
      }

      Serial.print("failed, err_code:");
      Serial.println(client.state());
      delay(100);
      Serial.println(" try again in 5 seconds");
      client.disconnect();
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  lcd.clear();
  updateMenu();
  delay(100);
}

void buzzer()
{
  digitalWrite(17, 1);
  delay(100);
  digitalWrite(17, 0);
  delay(100);
}
unsigned long timer_led = 0;
unsigned long timer_warning = 0;


void loop() {
  lcd.setCursor(8, 0);
  lcd.print("Tem:");
  lcd.setCursor(8, 1);
  lcd.print("Hum:");
  lcd.setCursor(0, 0);
  lcd.print("L1:");
  lcd.setCursor(0, 1);
  lcd.print("L2:");
  delay(500);
  updateMenu();
  delay(100);

  digitalWrite(Relay1, stateRelay1);
  digitalWrite(Relay2, stateRelay2);


  // @Thinh project cu
  //  if (security == 1 && Human_detect == 1) buzzer();
  //  if (security == 0 && Human_detect == 1)
  //  {
  //    digitalWrite(Relay1, 0);
  //    Serial.println("Relay on");
  //    stateRelay1 = 1;
  //
  //
  //    //    unsigned long current_led = millis();
  //    //    if (current_led - timer_led > 5000)
  //    //    {
  //    //      timer_led = current_led;
  //    //      digitalWrite(Relay2, 1);
  //    //      Serial.println("Relay off");
  //    //    }
  //    //    digitalWrite(Relay2, 0);
  //    //    Serial.println("Relay on");
  //    timer_led = millis();
  //    enable_relay2 = 1;
  //
  //
  //  }
  //  if (security == 0 && Human_detect == 0 && enable_relay2 == 1)
  //  {
  //    Serial.println("Relay on 2");
  //    digitalWrite(Relay1, 0);
  //    unsigned long current_led = millis();
  //    if (current_led - timer_led > 5000)
  //    {
  //      timer_led = current_led;
  //      digitalWrite(Relay1, 1);
  //      Serial.println("Relay off");
  //      enable_relay2 = 0;
  //    }
  //  }
  //Warning
  if (temptest  == "Heat")
  {
    Serial.println("Warning !!!");
    lcd.clear();
    lcd.print("Warning Heat !!!");
    digitalWrite(BUZZER_PIN, 1);

    unsigned long current_warning = millis();
    if (current_warning - timer_warning > 5000)
    {
      timer_warning = current_warning;
      digitalWrite(BUZZER_PIN, 1);
      delay(1000);
      digitalWrite(BUZZER_PIN, 0);
      delay(1000);
    }
  }
  else digitalWrite(BUZZER_PIN, 0);


}

// THIS IS A TASK READ_ADC AUTO
void Read_ADC(void *pvParameters)
{
  (void) pvParameters;

  /*
    Blink
    Turns on an LED on for one second, then off for one second, repeatedly.

    If you want to know what pin the on-board LED is connected to on your ESP32 model, check
    the Technical Specs of your board.
  */


  for (;;) // A Task shall never return or exit.
  {
    // READ sensor
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    power_mW = ina219.getPower_mW();
    humidity = dht.readHumidity();
    float temp = dht.readTemperature();
    temperature = round(temp * 100) / 100.0;


    sprintf(temperature_str, "%.2f", temperature);
    LightSensor = !digitalRead(LightSensor_PIN);  // LightSensor
    co2_value = !digitalRead(GasSensor_Pin);    // co2_value: 1 -> có CO2 ; 0 -> ko có Co2
    Human_detect = digitalRead(HumanDetect_Pin);
    long now = millis();
    if (now - lastMsg > 3000) {
      lastMsg = now;
      client.subscribe("esp32/doAn1/control");
      delay(10);
      if (now - time_send_data > 3000)
      {
        StaticJsonDocument<256> doc1;
        //        doc1["soc1"] = atoi(temp_soc_str1);
        //        doc1["current1"] = temp_curr_str1;

        doc1["power_mW"] = power_mW;
        doc1["LightSensor"] = LightSensor;
        doc1["co2_value"] = co2_value;
        doc1["Human_detect"] = Human_detect;
        doc1["humidity"] = humidity;
        doc1["temperature"] = temperature_str;
        Serial.print("power_mW: "); Serial.println(power_mW);
        Serial.print("LightSensor: "); Serial.println(LightSensor);
        Serial.print("co2_value: "); Serial.println(co2_value);
        Serial.print("Human_detect: "); Serial.println(Human_detect);
        Serial.print("humidity: "); Serial.println(humidity);
        Serial.print("temperature: "); Serial.println(temperature);
        Serial.println("###############################################");

        String Jdata;
        serializeJson(doc1, Jdata);
        //gửi json đến topic esp32/json
        client.publish("esp32/data", Jdata.c_str());
        Serial.println("MQTT Send Data");
        time_send_data = now;


      }
    }


    //      Serial.print("OCV: "); Serial.print(OCV_raw); Serial.print("("); Serial.print(OCV); Serial.println("V)");
    //      Serial.print("SOC: "); Serial.print(SOC); Serial.println("%");
    //      Serial.print("CURRENT: "); Serial.println(current);
    //Serial.print("stack_voltage: "); Serial.println(temperature_str);

    vTaskDelay(1000);



  }
}
//THIS IS A TASK AUTO SEND DATA AFTER A PERIOD OF TIME
