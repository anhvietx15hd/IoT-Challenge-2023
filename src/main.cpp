#include<Arduino.h>
#include<WiFi.h>
#include<WebServer.h>
#include<EEPROM.h>
#include<HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Wire.h>
#include<PubSubClient.h>
#include<OneWire.h>
#include<ArduinoJson.h>
#include<DallasTemperature.h>

int statusCode;

String st;
String content;
//WIFI
String essid = "";
String epass = "";
//MQTT
String mqtt_server = "";
String mqtt_username = "";
String mqtt_pass = "";
String mqtt_port = "";
String publish_topic = "";
String subscribe_topic = "";

#define RED_LED 33
#define YELLOW_LED 25
#define GREEN_LED 26
#define temperaturePin 5

// Address of the first memory cell
const int ssid_add = 0;
const int pass_add = 32;
const int mqtt_add = 64;
const int mqtt_username_add = 96;
const int mqtt_pass_add = 128;
const int mqtt_port_add = 160;
const int pub_topic_add = 192;
const int sub_topic_add = 224;

const int8_t buttonPin = 2;
bool isButtonPressed = false;
long last = 0;

void launchWeb(void);
void setupAP(void);

WebServer server(80);
LiquidCrystal_I2C LCD(0X27,16,2);
WiFiClient espClient;
PubSubClient client(espClient);

OneWire oneWire(temperaturePin); 
DallasTemperature temperatureSensor(&oneWire);
// Create a JSON document to write sensor data
DynamicJsonDocument doc(256); 

long lastSentMsg = 0;// The last theim a message was published the server

float waterTemperature = 0.0;

bool temperatureSensor_Active = true;

int16_t timeToUpdate = 3000; //Time to update the data by default (MiliSeconds)

void display(int cursorLine0, int cursorLine1, String textLine0, String textLine1){
  LCD.clear();
  LCD.setCursor(cursorLine0, 0);
  LCD.print(textLine0);
  LCD.setCursor(cursorLine1, 1);
  LCD.print(textLine1);
}

void setupWiFi(){
    delay(10);
    digitalWrite(YELLOW_LED, HIGH);
    Serial.println("\nConnecting to: " + essid);
    display(2, 5, "Connecting to ", "WIFI");
    WiFi.begin(essid.c_str(), epass.c_str());
    int count = 0;
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    count ++;
    if (count>20){
        Serial.println("False to set up WiFi connection");
        display(0, 4, "Failed to connect", "to WIFI");
        break;
        }
    }
    if(WiFi.status() == WL_CONNECTED){
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        display(3, 2, "IP Address", WiFi.localIP().toString());
        digitalWrite(YELLOW_LED, LOW);
    }
}

void setupMQTTConnection(){
    digitalWrite(YELLOW_LED, HIGH);
    int count = 0;
    while (!client.connected()) {
        Serial.println("Attempting MQTT connection...");
        if (!client.connect("ESP32Client", mqtt_username.c_str(), mqtt_pass.c_str())) {
        Serial.print("failed, rc=");
        Serial.print(client.state());                        
        Serial.println(" try again in 5 seconds");
        count ++;
        if (count >5){
            Serial.println("False to set up MQTT connection");
            display(0, 4, "Failed to connect", "to MQTT");
            break;
        }
        // wait 5s before reconnecting
        delay(500);
        }
        else{
        Serial.println("Successfully connect to mqtt server");
        display(2,2 , "Connected to", "MQTT server");
        digitalWrite(YELLOW_LED, LOW);
        }
    }
}

void getConfig(){
    long now = millis();
    while(digitalRead(buttonPin) == HIGH){
    if(millis() - now >=3000){
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(RED_LED, HIGH);
        Serial.println("Starting config");
        display(0, 0,"Starting config", "" );
        Serial.println("Turning the hotspot ON");
        //Create webserver for configuration
        setupAP();
        while (isButtonPressed){
            Serial.print(".");
            delay(100);
            server.handleClient();
        }
        break;
        }
    }
    isButtonPressed = false;
}

void buttonPressedInterrupt(){
    isButtonPressed = true;
}

void writeEEPROM(String index, int add){
    for (int i = 0; i < index.length(); ++i){
        EEPROM.write(i + add, index[i]);
        Serial.print("Wrote: ");
        Serial.println(index[i]);
    }
}

void readEEPROM(){
    //read ssid from EEPROM
    //const int ssid_add-> pass_add ->  mqtt_add -> mqtt_username_add -> mqtt_pass_ad -> mqtt_port_add -> pub_topic_add ->  sub_topic_add
    Serial.println("Reading WiFi SSID from EEPROM");
    for (int i = 0; i < pass_add; i++){
        essid += char(EEPROM.read(i));
    }
    Serial.println("SSID: " + essid);
    //read password from EEPROM
    Serial.println("\nReading WiFi PASSWORD from EEPROM");
    for (int i = pass_add; i < mqtt_add; i++){
        epass += char(EEPROM.read(i));
    }
    Serial.println("PASSWORD: " + epass);
    //read mqtt server address from EEPROM
    Serial.println("\nReading MQTT ADDRESS from EEPROM");
    for (int i = mqtt_add; i < mqtt_username_add; i++){
        mqtt_server += char(EEPROM.read(i));
    }
    Serial.println("MQTT ADDRESS: " + mqtt_server);
    //read mqtt username from EEPROM
    Serial.println("\nReading MQTT USERNAME from EEPROM");
    for (int i = mqtt_username_add; i < mqtt_pass_add; i++){
        mqtt_username += char(EEPROM.read(i));
    }
    //read mqtt password from EEPROM
    Serial.println("\nReading MQTT PASSWORD from EEPROM");
    for (int i = mqtt_pass_add; i < mqtt_port_add; i++){
        mqtt_pass += char(EEPROM.read(i));
    }
    Serial.println("MQTT PASSWORD: " + mqtt_pass);
    //read mqtt port from EEPROM
    Serial.println("\nReading MQTT PORT from EEPROM");
    for (int i = mqtt_port_add; i < pub_topic_add; i++){
        mqtt_port += char(EEPROM.read(i));
    }
    Serial.println("MQTT PORT: " + mqtt_port);
    //read publish topic from EEPROM
    Serial.println("\nReading MQTT PUBLISH TOPIC from EEPROM");
    for (int i = pub_topic_add; i < sub_topic_add; i++){
        publish_topic += char(EEPROM.read(i));
    }
    Serial.println("MQTT PUBLISH TOPIC: " + publish_topic);
    //read subscribe topic from EEPROM
    Serial.println("\nReading MQTT SUBSCRIBE TOPIC from EEPROM");
    for (int i = sub_topic_add; i < sub_topic_add+32; i++){
        subscribe_topic += char(EEPROM.read(i));
    }
    Serial.println("MQTT SUBSCRIBE TOPIC: " + subscribe_topic);
}

void createWebServer(){
    server.on("/", [](){
        IPAddress IP = WiFi.softAPIP();
        String IPString = IP.toString();

        //Create html message
        content = "<!DOCTYPE HTML><html><head><style>";
        content += ".list-box { border: 1px solid #ccc;border-radius: 5px;background-color: #f2f2f2;padding: 10px; width: 300px;}";
        content += ".list-box li {border-bottom: 1px solid #752525; padding: 3px 0px 3px 10px; font-size: 18px; list-style: none;}";
        content += ".list-box li.highlight {background-color: #5ad1f294;}";
        content += ".submit-box {margin-top: 10px; text-align: center; width: 300px;}";
        content += ".submit-box input[type=\"submit\"] {height: 40px; width: 100px; background-color: #3c9b41; color: #fff; border: none; border-radius: 4px; cursor: pointer; font-size: 20px;} ";    
        content += ".submit-box input[type=\"submit\"]:hover {background-color: #86d188;}";
        content += ".config-box{ margin-top: 10px; border: 1px solid #ccc; border-radius: 5px; background-color: #f2f2f2; padding: 10px; width: 300px; }";
        content += ".data{ height: 30px; box-sizing: border-box; margin: 7px; }";
        content += "form label { display: inline-block; width: 90px; height: 100%;}";
        content += "form input { border: 1px solid #ccc; border-radius: 4px; width: 190px; height: 100%; box-sizing: border-box; padding-left: 10px; font-size: 15px;}";
        content += "p{ text-align: center;  font-size: 20px;  margin: 0;  font-weight: bold; width: 300px; }";
        content += "ol{ box-sizing: border-box; padding: 0px 10px 0px 0px; }";
        content += "body{ display: grid; place-items: center; }";
        content += "</style>";
        // Script
        content += "<script> function copyToSSID(li) { var ssidInput = document.getElementById('ssid'); ssidInput.value = li.innerHTML; var listItems = document.querySelectorAll('.list-box li'); listItems.forEach(function(item) { item.classList.remove('highlight'); }); li.classList.add('highlight'); } </script>";
        content += "</head>";

        content += "<body>";
        content += "<p style=\"margin: 10px;\">Welcome to Home Automation System Configuration</p>";
        content += "<div class=\"list-box\"> <p>Sellect network for ESP32</p>";
        // List scanned networks
        content += st;
        content += "</div>";
        // Config box
        content += "<form action=\"setting\" method=\"get\">";
        content += "<div class=\"config-box\"> <p>WIFI</p> <div class=\"data\"> <label>SSID </label> <input id=\"ssid\" name=\"ssid\" length=\"32\"> </div> <div class=\"data\"> <label>PASSWORD </label> <input id=\"password\" name=\"password\" length=\"64\">  </div> </div>";
        content += "<div class=\"config-box\"> <p>MQTT</p> <div class=\"data\"> <label>ADDRESS</label> <input name=\"mqtt_address\" length=\"32\"> </div> <div class=\"data\"> <label>USERNAME </label> <input name=\"mqtt_username\" length=\"32\">  </div> <div class=\"data\"> <label>PASSWORD </label> <input name=\"mqtt_password\" length=\"64\">  </div> <div class=\"data\"> <label>PORT </label> <input name=\"mqtt_port\" length=\"64\">  </div> <div class=\"data\"> <label>PUB TOPIC </label> <input name=\"pub_topic\" length=\"32\">  </div> <div class=\"data\"> <label> SUB TOPIC</label> <input name=\"sub_topic\" length=\"32\">  </div> </div>";
        // submit button
        content += "<div class = \"submit-box\"> <input type=\"submit\" value=\"SUBMIT\"> </div>";
        content += "</form> </body>";
        content += "</html>";
        server.send(200, "text/html", content);
    });

    server.on("/scan" ,[](){
        IPAddress IP = WiFi.softAPIP();
        String IPString = IP.toString();

        content = "<!DOCTYPE HTML>\r\n<html>go back";
        server.send(200, "text/html", content);
    });

    server.on ("/setting", [] (){
        String qssid = server.arg("ssid");
        String qpassword = server.arg("password");
        String qmqtt_address = server.arg("mqtt_address");
        String qmqtt_username = server.arg("mqtt_username");
        String qmqtt_password = server.arg("mqtt_password");
        String qmqtt_port = server.arg("mqtt_port");
        String qpub_topic = server.arg("pub_topic");
        String qsub_topic = server.arg("sub_topic");
        bool isUpdated = false;

        if(qssid.length()>0){
            isUpdated = true;
            Serial.println("Clearing WIFI from EEPROM");
            for(int i = 0; i<mqtt_add; ++i){
                EEPROM.write(i, 0);
            }
            Serial.println("Received SSID: " + String(qssid));
            Serial.println("Received password: " +  String(qpassword));
            Serial.println("Writing new SSID to EEPROM");
            writeEEPROM(qssid, ssid_add);
            Serial.println("Writing new PASSWORD to EEPROM");
            writeEEPROM(qpassword, pass_add);
        }

        if(qmqtt_address.length()>0){
            isUpdated = true;
            Serial.println("Clearing MQTT from EEPROM");
            for(int i = mqtt_add; i<mqtt_port_add; ++i){
                EEPROM.write(i, 0);
            }
            Serial.println("Received MQTT ADDRESS: " + String(qmqtt_address));
            Serial.println("Received MQTT PASSWORD: " +  String(qmqtt_password));
            Serial.println("Writing new MQTT ADDRESS to EEPROM");
            writeEEPROM(qmqtt_address, mqtt_add);
            Serial.println("Writing new MQTT USERNAME to EEPROM");
            writeEEPROM(qmqtt_username, mqtt_username_add);
            Serial.println("Writing new MQTT PASSWORD to EEPROM");
            writeEEPROM(qmqtt_password, mqtt_pass_add);
        }
        
        if(qmqtt_port.length()>0){
            isUpdated = true;
            Serial.println("Clearing MQTT PORT from EEPROM");
            for(int i = mqtt_port_add; i<pub_topic_add; ++i){
                EEPROM.write(i, 0);
            }
            Serial.println("Received MQTT PORT: " + String(qmqtt_port));
            Serial.println("Writing new MQTT PORT to EEPROM");
            writeEEPROM(qmqtt_port, mqtt_port_add);
        }

        if(qpub_topic.length()>0){
            isUpdated = true;
            Serial.println("Clearing MQTT PUBLISH TOPIC from EEPROM");
            for(int i = pub_topic_add; i<sub_topic_add; ++i){
                EEPROM.write(i, 0);
            }
            Serial.println("Received MQTT PUBLISH TOPIC: " + String(qpub_topic));
            Serial.println("Writing new MQTT PUBLISH TOPIC to EEPROM");
            writeEEPROM(qpub_topic, pub_topic_add);
        }

        if(qsub_topic.length()>0){
            isUpdated = true;
            Serial.println("Clearing MQTT PUBLISH TOPIC from EEPROM");
            for(int i = sub_topic_add; i<sub_topic_add+32; ++i){
                EEPROM.write(i, 0);
            }
            Serial.println("Received MQTT SUBSCRIBE TOPIC: " + String(qsub_topic));
            Serial.println("Writing new MQTT SUBSCRIBE TOPIC to EEPROM");
            writeEEPROM(qsub_topic, sub_topic_add);
        }

        if(isUpdated){
            EEPROM.commit();

            content = "{\"Success\":\"saved to EEPROM... Starting new connection\"}";
            statusCode = 200;
            display(6, 0, "DONE", "RESTARTING ESP32");
            digitalWrite(RED_LED, LOW);
            ESP.restart();
        }
        else{
            content = "{\"Error\":\"404 not found\"}";
            statusCode = 404;
            Serial.println("Sending 404");
        }
        server.sendHeader("Access-Control_Allow_Origin", "*");
        server.send(statusCode, "application/json", content);
    });
}

void launchWeb(){
    Serial.print("Please visit the Address: ");
    Serial.println(WiFi.softAPIP());

    createWebServer();

    server.begin();
    Serial.println("Server started");
    display(2, 2, "Please visit", WiFi.softAPIP().toString());
}

void setupAP(void){
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    int n = WiFi.scanNetworks();
    Serial.println("Done Scanning");
    if(n == 0){
        Serial.println("No network was found");
    }
    else{
        Serial.println(String(n) + " networks was found");
        for (int i = 0; i<n; i++){
            Serial.println(String(i+1) + ": " + WiFi.SSID(i));
        }
    }

    st = "<ol>"; // save scanned networks
    for (int i = 0; i<n; i++){
        st += "<li onclick=\"copyToSSID(this)\">";
        st += WiFi.SSID(i);
        st += "</li>";
    }
    st += "</ol>";
    // Start the Wifi Access point
    WiFi.softAP("Home Automation System", "");
    Serial.println("Initializing SoftAP for WiFi config");
    //Start the webserver
    launchWeb();
    Serial.println("over");
}

void loopFunction(){
    Serial.println("Ham loop dang chay");
    digitalWrite(GREEN_LED, HIGH);
    last = millis();
}
void JSONProcessing(String &message){
    StaticJsonDocument<250> sub_doc;
    deserializeJson(sub_doc, message.c_str());
    // Get the time to update the sensors data from server
    timeToUpdate = sub_doc["timeToUpdate"];
    Serial.println("Successfully set time to update the new data to server to " + String(timeToUpdate) + " miliSeconds");
    display(0, 4, "Time to update:", String(timeToUpdate) + " ms");
}

void getSensorsActiveStatus(String &message){
    const char* str = message.c_str();
    // Update sensor operating status
    if (strcmp(str, "ON_temperatureSensor") == 0){
        temperatureSensor_Active = true;
        Serial.println("Successfully TURN ON the water temperature sensor");
        display(3, 1, "Turned on", "temperature ss");
    }
    else if (strcmp(str, "OFF_temperatureSensor") == 0){
        temperatureSensor_Active = false;
        Serial.println("Successfully TURN OFF the water temperature sensor");
        display(3, 1, "Turned off", "temperature ss");
    }
    else{
        JSONProcessing(message);
    }
    Serial.println("______________________________________________");
}

float getTemperature(){
    // Request temperatures
    temperatureSensor.requestTemperatures();
    float temperature = temperatureSensor.getTempCByIndex(0);

    return temperature;
}

void sendSensorsData(){
    long now = millis();
    if (now - lastSentMsg > timeToUpdate){
        lastSentMsg = now;
        
        //Add data to the Json document
        //  Temperature sensor
        if(temperatureSensor_Active == true)
        doc["temperature"]  = getTemperature();
        String message;
        serializeJson(doc, message);
        // Serial.println(message);
        //Send data to publish topic
        client.publish(publish_topic.c_str(), message.c_str());
        digitalWrite(GREEN_LED, HIGH);
    }
    if (now - lastSentMsg >=1000){
        digitalWrite(GREEN_LED, LOW);
    }
}

void callback(char* topic, byte* payload, int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println();
  getSensorsActiveStatus(message);
}


void setup(){
    Serial.begin(115200);
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    LCD.init();                    
    LCD.backlight(); 
    Serial.println("\n Disconnecting current WiFi connection");
    WiFi.disconnect();

    EEPROM.begin(512);
    pinMode(buttonPin, INPUT);
    attachInterrupt(buttonPin, buttonPressedInterrupt, RISING);

    Serial.println("\n");
    Serial.println("Starting up");

    readEEPROM();
    setupWiFi();

    client.setServer(mqtt_server.c_str(), mqtt_port.toInt()); 
    client.setCallback(callback);

    temperatureSensor.begin();
}

void loop(){
    if(isButtonPressed == true){
        getConfig();
    }
    if (!client.connected()) {
    setupMQTTConnection();
    }
    
    client.loop();
    client.subscribe(subscribe_topic.c_str());

    sendSensorsData();
}
// Test
