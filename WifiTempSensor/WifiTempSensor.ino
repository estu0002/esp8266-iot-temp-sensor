
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SSD1306.h>

const char* wifi_ssid = "YOUR_SSID";
const char* wifi_pw = "YOUR_PASSWORD";

String loggerPublicKey = "YOUR_PUBLIC_KEY";
String loggerPrivateKey = "YOUR_PRIVATE_KEY";
String loggerUrl = "/input/" + loggerPublicKey + "?private_key=" + loggerPrivateKey; 

// DHT18B20 is on GPIO3 / RX
OneWire oneWire(3);
DallasTemperature tempSensors(&oneWire);

// Defaulting the OLED connection to GPIO0 and GPIO2 pins
#define SDA_PIN 0
#define SDC_PIN 2

// define the OLED display
SSD1306 display(0x3c, SDA_PIN, SDC_PIN);

// global variables (yeah, yeah, I know better...)
float currentTemp;
String ipAddress;
int nextUpdateCounter;

// function prototypes 
void updateDisplay();
int logTemp();
String ipToString(IPAddress ip);

void setup() {

  // start the temperature sensors - there could theoretically be more than one connected
  // to the bus.
  tempSensors.begin();

  display.init();
  display.flipScreenVertically();
  
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);

  if(tempSensors.getDeviceCount() < 1) {
    // there's no sensor on the bus - we're doomed
    display.drawString(0,0,"TEMP SENSOR FAIL!");
    display.display();
    return;
  }

  display.drawString(0,52,"WiFi connecting...");
  display.display();
  
  // start a WiFi connection
  WiFi.begin(wifi_ssid, wifi_pw);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  display.clear();
  display.drawString(0,52,"WiFi CONNECTED");
  display.display();
  delay(500); // let user see the connected message

  // initialize variables for loop
  ipAddress = ipToString(WiFi.localIP());
  nextUpdateCounter = 1;
}


void loop() {
  
  if(--nextUpdateCounter == 0) {
    tempSensors.requestTemperatures();
    currentTemp = tempSensors.getTempFByIndex(0);
    logTemp();
    nextUpdateCounter = 60;
  }
  
  updateDisplay();
  
  delay(950); // swag of 50ms required to update the OLED
}

/*
 * Draw the current temperature, next update countdown timer, and IP address
 */
void updateDisplay() {
  display.clear();

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  display.drawString(0,0,String(currentTemp));
  display.display();

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0,42,"Next update in: "+String(nextUpdateCounter));

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0,52,"IP: "+ipAddress);

  display.display();
}

/*
 * Log the temperature off to data.sparkfun.com
 * returns HTTP response code
 */
int logTemp() {
  HTTPClient http;   
  http.begin("data.sparkfun.com",
    80,
    loggerUrl
      + "&tempf="
      + currentTemp
  );

  return http.GET();
}

/*
 * Convert an IPAddress to a String
 */
String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
} 
