#include <Arduino_JSON.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Adafruit_HTU21DF.h"

const char* ssid = "Bulkkot";
const char* password = "supernet";
//Your Domain name with URL path or IP address with path
const char* serverName = "http://192.168.71.115/api/atmos/poste.php";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

Adafruit_HTU21DF htu = Adafruit_HTU21DF();
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Wire.begin(0, 2);
  lcd.init();
  lcd.display();
  lcd.clear();
  lcd.backlight();
  delay(1000);

  Serial.begin(115200);

  if (!htu.begin()) {
    lcd.setCursor(0, 0);
    lcd.print("Sensor don't find");
    while (1);
  }

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  lcd.print(WiFi.localIP());
  lcd.setCursor(0, 1);
  lcd.print(WiFi.status());
  delay(10000);
  lcd.clear();
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  float temp = htu.readTemperature();
  float humi = htu.readHumidity();
  
  DynamicJsonDocument doc(1024);
  doc["temperature"] = temp;
  doc["humidite"]= humi;
  String json;
  serializeJson(doc, json);
  
  lcd.print("t°: ");
  lcd.print(temp);
  lcd.setCursor(0, 1);
  lcd.print("h%: ");
  lcd.print(humi);

  delay(2000);
  lcd.clear();
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // If you need an HTTP request with a content type: application/json, use the following:
      //http.addHeader("Content-Type", "application/json");
      http.addHeader("Content-Type", "application/json");
      //int httpResponseCode = http.POST("{\"temperature\":\"" + tempe +"\",\"humidite\":\""+ humie +"\"}");
      //int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");
        

        int httpResponseCode = http.POST(json);

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      lcd.print(httpResponseCode);
      delay(2000);
      lcd.clear();
      http.end();

      String payload = http.getString();
     
      lcd.setCursor(0, 1);
      lcd.print(payload);
      delay(2000);
      lcd.clear();
      Serial.println(payload);
    }
    else {
      Serial.println("WiFi Disconnected");
      lcd.print("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
