
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>


// DHT18B20 is on GPIO2
OneWire oneWire(2);
DallasTemperature tempSensors(&oneWire);

const char* wifi_ssid = "YOUR_SSID";
const char* wifi_pw = "YOUR_PASSWORD";

String loggerPublicKey = "YOUR_PUBLIC_KEY";
String loggerPrivateKey = "YOUR_PRIVATE_KEY";
String loggerUrl = "/input/" + loggerPublicKey + "?private_key=" + loggerPrivateKey; 

ESP8266WiFiMulti WiFiMulti;

void setup() {
//  Serial.begin(9600);

  // start the temperature sensors - there could theoretically be more than one connected
  // to the bus.
  tempSensors.begin();

  if(tempSensors.getDeviceCount() < 1) {
    // there's no sensor on the bus - we're doomed
    // TODO error handling
  }

  // start a WiFi connection
  WiFiMulti.addAP(wifi_ssid, wifi_pw);
}

void loop() {
  if((WiFiMulti.run() == WL_CONNECTED)) {
      tempSensors.requestTemperatures();
      
//    Serial.println("Connected.");
    HTTPClient http;   
    http.begin("data.sparkfun.com",
      80,
      loggerUrl
        + "&tempf="
        + tempSensors.getTempFByIndex(0));

    int rc = http.GET();
//    Serial.print("GET complete;");
//    Serial.println(rc);

    delay(60000);
  }
}
