#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

int hIndex = 0;
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Home Wi-Fi"
#define WLAN_PASS       "zvw-0000-1984-1583#pk"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "nuviDev"
#define AIO_KEY         "aio_wxMt598ilb0CbXPo5gw8secJKJr0"

WiFiClient client;

// Adafruit IO feed object
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe temperature = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Temperature");
Adafruit_MQTT_Subscribe humidity = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Humidity");

void setup() {
  
  // LCD display configurations
  Wire.begin(0, 2);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  Serial.begin(115200);
  delay(10);

  // Connect to WiFi access point.
  Serial.println(); 
  Serial.println();
  Serial.print("Connecting to Wi-Fi");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to Adafruit IO MQTT server
  mqtt.connect();
  Serial.println("MQTT connected");

  // Subscribe to temperature and humidity feeds
  mqtt.subscribe(&temperature);
  mqtt.subscribe(&humidity);
  Serial.println("Subscribed to feed");

}

void loop() {

  mqtt.processPackets(5000);
  
  // Check if new temperature data is available
  Serial.print("Temperature: ");
  float floatTemp = atof((char*)temperature.lastread);
  Serial.println(floatTemp);
  
  // Check if new humidity data is available
  Serial.print("Humidity: ");
  float floatHumid = atof((char*)humidity.lastread);
  Serial.println(floatHumid);

  //Print Values on LCD Display
  lcd.setCursor(0, 0);
  lcd.print("Heat Index :");
  lcd.setCursor(0, 1);
  calcHeat(floatTemp, floatHumid);
  delay(10000);

  lcd.clear();

}

//Calculate HeatIndex Function
void calcHeat(float Tc, float RH) {
  float Tf=0, HI=0;

  //Calculate Heat Index
  Tf = Tc*1.8 + 32;
  HI =  -42.379 + 2.04901523*Tf + 10.14333127*RH - .22475541*Tf*RH - .00683783*Tf*Tf - .05481717*RH*RH + .00122874*Tf*Tf*RH + .00085282*Tf*RH*RH - .00000199*Tf*Tf*RH*RH;

  //Print Heat Index on Serial Monitor
  Serial.print("Heat Index : ");
  Serial.println(HI);

  //Classify heat index value into Caution, Extreme Caution, Danger, Extreme Danger
  if(HI>80.0 && HI<=90.0) {
    lcd.print("Caution");
  } else if(HI>90.0 && HI<=103.0) {
    lcd.print("Extreme Caution");
  } else if(HI>103.0 && HI<=124.0) {
    lcd.print("Danger");
  } else if(HI>124.0) {
    lcd.print("Extreme Danger");
  } else {
    lcd.print("Invalid Range");
  }
  
}
