/**************************************************
 * Definition
***************************************************/
#include <Arduino.h>
#include "ConnectionConfig.h"
#include "menu.h"
#include "config.h"

/***************************************************
 * Variables
****************************************************/
/*Webserver content variable*/
static String st;
static String content;
/***************************************************
 * Codes
****************************************************/
void setupWifi(void){
    delay(10);
    uint8_t count;
    Serial.println("\nConnecting to: " + wifi_ssid);
    display(2, 5, "Connecting to ", "WIFI");
    /*EStablish wifi connection*/
    uint16_t m = 9, n = 1;
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
    while((WiFi.status() != WL_CONNECTED) && (count < 20)){
        digitalWrite(STATUS_LED_RED, HIGH);
        delay(500);
        Serial.print(".");
        LCD.setCursor(m, n);
        LCD.print(".");
        m++;
        if (m == 16) {
            m = 0;
            if (n == 1) n= 0;
            else n = 1;
        }
        count ++;
        if (count>20){
            Serial.println("False to set up WiFi connection");
            display(0, 4, "Failed to connect", "to WIFI");
            ESP.restart();
        }
    }
    digitalWrite(STATUS_LED_RED, LOW);
    if(WiFi.status() == WL_CONNECTED){
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        display(3, 2, "IP Address", WiFi.localIP().toString());
        digitalWrite(STATUS_LED_BLUE, LOW);
    }
        LCD.clear();
}

void setupMQTTConnection(void){
    digitalWrite(STATUS_LED_BLUE, HIGH);
    uint8_t count = 0;
    while (!client.connected()) {
        digitalWrite(STATUS_LED_RED, HIGH);
        Serial.println("Attempting MQTT connection...");
        if (!client.connect("ESP32Client", mqtt_username.c_str(), mqtt_password.c_str())) {
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
        digitalWrite(STATUS_LED_RED, LOW);
        digitalWrite(STATUS_LED_BLUE, LOW);
        }
    }
    digitalWrite(STATUS_LED_RED, LOW);
    LCD.clear();
}

void getConfig(void){
    long now = millis();
    while(digitalRead(CONFIG_BUTTON) == HIGH){
    if(millis() - now >= 3000){
        digitalWrite(STATUS_LED_BLUE, LOW);
        digitalWrite(STATUS_LED_RED, HIGH);
        Serial.println("Starting config");
        display(0, 0,"Starting config", "" );
        Serial.println("Turning the hotspot ON");
        //Create webserver for configuration
        setupAP();
        while (isConfigButtonPressed){
            Serial.print(".");
            delay(100);
            server.handleClient();
        }
        break;
        }
    }
    isConfigButtonPressed = false;
}

void writeEEPROM(String index, uint16_t add){
    uint16_t i;
    for (i = 0; i < index.length(); ++i){
        EEPROM.write(i + add, index[i]);
        Serial.print("Wrote: ");
        Serial.println(index[i]);
    }
}

void readEEPROM(void){
    //read ssid from EEPROM
    //const int ssid_add-> pass_add ->  mqtt_add -> mqtt_username_add -> mqtt_pass_ad -> mqtt_port_add -> pub_topic_add ->  sub_topic_add
    uint16_t i;
    Serial.println("Reading WiFi SSID from EEPROM");
    for (i = 0; i < WIFI_PASSWORD_ADDRESS; i++){
        wifi_ssid += char(EEPROM.read(i));
    }
    Serial.println("SSID: " + wifi_ssid);
    //read password from EEPROM
    Serial.println("\nReading WiFi PASSWORD from EEPROM");
    for (i = WIFI_PASSWORD_ADDRESS; i < MQTT_ADDRESS; i++){
        wifi_password += char(EEPROM.read(i));
    }
    Serial.println("PASSWORD: " + wifi_password);
    //read mqtt server address from EEPROM
    Serial.println("\nReading MQTT ADDRESS from EEPROM");
    for (i = MQTT_ADDRESS; i < MQTT_USERNAME_ADDRESS; i++){
        mqtt_server += char(EEPROM.read(i));
    }
    Serial.println("MQTT ADDRESS: " + mqtt_server);
    //read mqtt username from EEPROM
    Serial.println("\nReading MQTT USERNAME from EEPROM");
    for (i = MQTT_USERNAME_ADDRESS; i < MQTT_PASSWORD_ADDRESS; i++){
        mqtt_username += char(EEPROM.read(i));
    }
    Serial.println("MQTT USERNAME: " + mqtt_username);
    //read mqtt password from EEPROM
    Serial.println("\nReading MQTT PASSWORD from EEPROM");
    for (i = MQTT_PASSWORD_ADDRESS; i < MQTT_PORT_ADDRESS; i++){
        mqtt_password += char(EEPROM.read(i));
    }
    Serial.println("MQTT PASSWORD: " + mqtt_password);
    //read mqtt port from EEPROM
    Serial.println("\nReading MQTT PORT from EEPROM");
    for (i = MQTT_PORT_ADDRESS; i < MQTT_PUBLISH_TOPIC_ADDRESS; i++){
        mqtt_port += char(EEPROM.read(i));
    }
    Serial.println("MQTT PORT: " + mqtt_port);
    //read publish topic from EEPROM
    Serial.println("\nReading MQTT PUBLISH TOPIC from EEPROM");
    for (i = MQTT_PUBLISH_TOPIC_ADDRESS; i < MQTT_SUBSCRIBE_TOPIC_ADDRESS; i++){
        publish_topic += char(EEPROM.read(i));
    }
    Serial.println("MQTT PUBLISH TOPIC: " + publish_topic);
    //read subscribe topic from EEPROM
    Serial.println("\nReading MQTT SUBSCRIBE TOPIC from EEPROM");
    for (i = MQTT_SUBSCRIBE_TOPIC_ADDRESS; i < MQTT_SUBSCRIBE_TOPIC_ADDRESS + 32; i++){
        subscribe_topic += char(EEPROM.read(i));
    }
    Serial.println("MQTT SUBSCRIBE TOPIC: " + subscribe_topic);
}

void createWebServer(){
    uint16_t i;
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
        uint16_t i;
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
            /*Clear EEPROM*/
            for(i = 0; i< MQTT_ADDRESS; ++i){
                EEPROM.write(i, 0);
            }
            Serial.println("Received SSID: " + String(qssid));
            Serial.println("Received password: " +  String(qpassword));
            Serial.println("Writing new SSID to EEPROM");
            writeEEPROM(qssid, WIFI_SSID_ADDRESS);
            Serial.println("Writing new PASSWORD to EEPROM");
            writeEEPROM(qpassword, WIFI_PASSWORD_ADDRESS);
        }

        if(qmqtt_address.length()>0){
            isUpdated = true;
            Serial.println("Clearing MQTT from EEPROM");
            for(i = MQTT_ADDRESS; i < MQTT_PORT_ADDRESS; ++i){
                EEPROM.write(i, 0);
            }
            Serial.println("Received MQTT ADDRESS: " + String(qmqtt_address));
            Serial.println("Received MQTT PASSWORD: " +  String(qmqtt_password));
            Serial.println("Writing new MQTT ADDRESS to EEPROM");
            writeEEPROM(qmqtt_address, MQTT_ADDRESS);
            Serial.println("Writing new MQTT USERNAME to EEPROM");
            writeEEPROM(qmqtt_username, MQTT_USERNAME_ADDRESS);
            Serial.println("Writing new MQTT PASSWORD to EEPROM");
            writeEEPROM(qmqtt_password, MQTT_PASSWORD_ADDRESS);
        }
        
        if(qmqtt_port.length() > 0){
            isUpdated = true;
            Serial.println("Clearing MQTT PORT from EEPROM");
            for(i = MQTT_PORT_ADDRESS; i< MQTT_PUBLISH_TOPIC_ADDRESS; ++i){
                EEPROM.write(i, 0);
            }
            Serial.println("Received MQTT PORT: " + String(qmqtt_port));
            Serial.println("Writing new MQTT PORT to EEPROM");
            writeEEPROM(qmqtt_port, MQTT_PORT_ADDRESS);
        }

        if(qpub_topic.length() > 0){
            isUpdated = true;
            Serial.println("Clearing MQTT PUBLISH TOPIC from EEPROM");
            for(i = MQTT_PUBLISH_TOPIC_ADDRESS; i < MQTT_SUBSCRIBE_TOPIC_ADDRESS; ++i){
                EEPROM.write(i, 0);
            }
            Serial.println("Received MQTT PUBLISH TOPIC: " + String(qpub_topic));
            Serial.println("Writing new MQTT PUBLISH TOPIC to EEPROM");
            writeEEPROM(qpub_topic, MQTT_PUBLISH_TOPIC_ADDRESS);
        }

        if(qsub_topic.length()>0){
            isUpdated = true;
            Serial.println("Clearing MQTT SUBSCRIBE TOPIC from EEPROM");
            for(i = MQTT_SUBSCRIBE_TOPIC_ADDRESS; i < MQTT_SUBSCRIBE_TOPIC_ADDRESS + 32; ++i){
                Serial.println("i\n");
                EEPROM.write(i, 0);
            }
            Serial.println("Received MQTT SUBSCRIBE TOPIC: " + String(qsub_topic));
            Serial.println("Writing new MQTT SUBSCRIBE TOPIC to EEPROM");
            writeEEPROM(qsub_topic, MQTT_SUBSCRIBE_TOPIC_ADDRESS);
        }

        if(isUpdated){
            EEPROM.commit();

            content = "{\"Success\":\"saved to EEPROM... Starting new connection\"}";
            statusCode = 200;
            display(6, 0, "DONE", "RESTARTING ESP32");
            digitalWrite(STATUS_LED_RED, LOW);
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

void launchWeb(void){
    Serial.print("Please visit the Address: ");
    Serial.println(WiFi.softAPIP());

    createWebServer();

    server.begin();
    Serial.println("Server started");
    display(2, 2, "Please visit", WiFi.softAPIP().toString());
}

void setupAP(void){
    uint8_t i;
    uint8_t numberOfNetworks;
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    numberOfNetworks = WiFi.scanNetworks();
    Serial.println("Done Scanning");
    if(numberOfNetworks == 0){
        Serial.println("No network was found");
    }
    else{
        Serial.println(String(numberOfNetworks) + " networks was found");
        for ( i = 0; i < numberOfNetworks; i++){
            Serial.println(String(i + 1) + ": " + WiFi.SSID(i));
        }
    }

    st = "<ol>"; // save scanned networks
    for (int i = 0; i< numberOfNetworks ; i++){
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
/**********************************************************
 * End of file
**********************************************************/
