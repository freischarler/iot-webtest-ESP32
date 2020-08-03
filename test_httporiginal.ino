#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include "BH1750.h"
#include <Wire.h>

#define DHTTYPE DHT22

#define I2C_SDA 21
#define I2C_SCL 22
BH1750 lightMeter(0x23);

const char* ssid = "Fibertel WiFi225 2.4GHz";
const char* password = "0141845935";

//Your Domain name with URL path or IP address with path
const char* serverName = "https://iot-crud-angular-nodejs-mysql.herokuapp.com/api/values";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 1000*60*15;

// El Sensor DHT
uint8_t DHTPin = 4; 
// Inicializar sensor DHT.
DHT dht(DHTPin, DHTTYPE);                
 
float Temperature=-1;
float Humidity=-1;
uint16_t Lux=-1;
String postMessage;


void setup() {
  Serial.begin(115200);
  pinMode(DHTPin,INPUT);
  dht.begin();
  //Wire.begin (I2C_SDA, I2C_SCL);
  Wire.begin();
  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE_2);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void get_values(){
  delay(3000);
  Temperature=dht.readTemperature();
  Humidity=dht.readHumidity();
  Lux=lightMeter.readLightLevel();
}


void loop() {
  get_values();
  const int capacity = JSON_OBJECT_SIZE(4);
  //StaticJsonDocument<capacity> doc;

  postMessage="{\"sensor_id\":"; postMessage+="\"1\",";
  postMessage+="\"temperatura\":\""; postMessage+=Temperature; postMessage+="\",";
  postMessage+="\"humedad\":\""; postMessage+=Humidity; postMessage+="\",";
  postMessage+="\"luz\":\""; postMessage+=Lux; postMessage+="\"}";
  Serial.println(postMessage);
      
  //serializeJsonPretty(doc, postMessage);
  //Send an HTTP POST request every 10 minutes
  
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      //int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");
      int httpResponseCode=http.POST(postMessage);

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
