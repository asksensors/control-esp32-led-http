/*
 * Description:  Turn ON/OFF LED (https://asksensors.com) using an ESP32 dev board.
 *  Author: https://asksensors.com, 2020
 *  github: https://github.com/asksensors
 */
 
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

WiFiMulti WiFiMulti;
HTTPClient ask;
// TODO: user config
const char* ssid     = ".................."; //Wifi SSID
const char* password = ".................."; //Wifi Password
const char* apiKeyOut = "..................";      // Actuator API KEY OUT
const unsigned int readInterval = 5000;   // read interval (in ms)
const int LED_pin = 22;

// ASKSENSORS API host config
const char* host = "api.asksensors.com";  // API host name
const int httpPort = 80;      // port
  
void setup(){
  
  // open serial
  Serial.begin(115200);
  Serial.println("*****************************************************");
  Serial.println("********** Program Start : Turn ON/OFF LED with AskSensors.");
  Serial.println("Set LED as output");
  pinMode(LED_pin, OUTPUT);   // set led as output

  Serial.println("Wait for WiFi... ");

  // connecting to the WiFi network
  WiFiMulti.addAP(ssid, password);
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  // connected
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 
}

void loop(){

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }else {

    // Create a URL to read data from module 1
    String url = "http://api.asksensors.com/read/";
    url += apiKeyOut;
    url += "?module=module1";
    url += "&maxResults=1";
      
    Serial.print("********** requesting URL: ");
    Serial.println(url);
    // send data 
    ask.begin(url); //Specify the URL
  
    //Check for the returning code
    int httpCode = ask.GET();          
 
    if (httpCode > 0) { 
 
        String payload = ask.getString();
        String command_value;       
        Serial.println(httpCode);
        Serial.println(payload);

        Serial.print("********** Json parsing ");
        JSONVar askObject = JSON.parse(payload);
        Serial.println(askObject);

        // JSON.typeof(jsonVar) can be used to get the type of the var
        if (JSON.typeof(askObject[0]) == "undefined") {
          Serial.println("Parsing input failed!");
          return;
        }
 
          Serial.println("********** Read Actuator Command from AskSensors ");

      // read command value
        if (askObject[0].hasOwnProperty("value")) {          
          command_value = (const char*)askObject[0]["value"];
          Serial.print("Command value: ");
          Serial.println(command_value);
          if(command_value[0] == '1'){
            digitalWrite(LED_pin, 1);
            Serial.println("LED is ON");
          } else{
            digitalWrite(LED_pin, 0);
            Serial.println("LED is OFF");
         }
        }

      } else {
      Serial.println("Error on HTTP request");
    }
 
    ask.end(); //End 
    Serial.println("********** End ");
    Serial.println("*****************************************************");

  }

  client.stop();  // stop client
  
  delay(readInterval);    // delay
}
