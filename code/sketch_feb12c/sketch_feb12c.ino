#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
//#include <Adafruit_HTU21DF.h>
#include "Adafruit_HTU21DF.h"
/* this can be run with an emulated server on host:
        cd esp8266-core-root-dir
        cd tests/host
        make ../../libraries/ESP8266WebServer/examples/PostServer/PostServer
        bin/PostServer/PostServer
   then put your PC's IP address in SERVER_IP below, port 9080 (instead of default 80):
*/
//#define SERVER_IP "10.0.1.7:9080" // PC address with emulation on host
#define SERVER_IP "http://192.168.71.115/api/atmos/poste.php"

#ifndef STASSID
#define STASSID "Bulkkot"
#define STAPSK  "supernet"
#endif
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_HTU21DF htu = Adafruit_HTU21DF();
void setup() {
   lcd.init();
  lcd.display();
  lcd.clear();
  lcd.backlight();
  delay(1000);
  pinMode(BUILTIN_LED, OUTPUT);
  //digitalWrite(BUILTIN_LED, HIGH);
  //Serial.begin(115200);
  Wire.begin(0, 2);

  //Serial.println();
  //Serial.println();
  //Serial.println();

  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(BUILTIN_LED, millis() / 400 % 2);
    delay(500);
    //Serial.print(".");
  }
  //Serial.println("");
  //Serial.print("Connected! IP address: ");
  //Serial.println(WiFi.localIP());

}

void loop() {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {
    digitalWrite(BUILTIN_LED, HIGH);
    WiFiClient client;
    HTTPClient http;

    //Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, SERVER_IP); //HTTP
    http.addHeader("Content-Type", "application/json");

    //Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    digitalWrite(BUILTIN_LED, HIGH);
    //int httpCode = http.POST("{\"valeur\":\""+WiFi.macAddress()+"\"}");
    int temp = htu.readTemperature();
    int humi = htu.readHumidity();
    String tempe = String(temp);
    String humie = String(humi);
    //Serial.println((String) tem+"Â°");
    //Serial.println((String) hum+"%");
    int httpCode = http.POST("{\"addr\":\"" + WiFi.macAddress() + "\",\"values\":{\"temperature\":\"" + tempe + "\",\"humidity\":\"" + humie + "\"}}");
    digitalWrite(BUILTIN_LED, LOW);
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      //Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        //Serial.println("received payload:\n<<");
        //Serial.println(payload);
          lcd.print("reponse :");
          lcd.setCursor(0, 1);
          lcd.print(payload);
        //Serial.println(">>");
        //digitalWrite(BUILTIN_LED, LOW);
      }
    } else {
      //Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }

  delay(10000);
}
