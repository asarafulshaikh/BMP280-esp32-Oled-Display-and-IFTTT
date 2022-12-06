
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP280.h>

#include <WiFi.h>
#include <HTTPClient.h>

/****************************************************/

const char * ssid = "31 Asaraful's Galaxy M12";
const char * password = "8928539402";


String server = "http://maker.ifttt.com";
String eventName = "bmp_280";
String IFTTT_Key = "llHLwb7Cs4fpuJzP4xeAdMghAuKLfsGNV-VZcBP075m";
String IFTTTUrl="https://maker.ifttt.com/trigger/bmp_280/with/key/llHLwb7Cs4fpuJzP4xeAdMghAuKLfsGNV-VZcBP075m";

int value1;  // value1 for temp.
int value2;  // value2 for pressure

/****************************************************/
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Adafruit_BMP280 bmp; 

void setup() {
  Serial.begin(115200);
  bmp.begin(0x76); 
  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
    }
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed or couldn't find a valid bme280"));
    //for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);  
  wifi_();
  


}

void loop() {
  delay(5000);
  value1= bmp.readTemperature();
  value2 = bmp.readPressure();
  if (isnan(value2 ) || isnan(value1)) {
    Serial.println("Failed to read from bmp sensor!");
  }
  else{
    Serial.print(F("Temperature = "));
    Serial.print(value1);
    Serial.println(" *C");
    Serial.print(F("Pressure = "));
    Serial.print(value2/1000);
    Serial.println(" kPa");
  }
  Display_();
  sendDataToSheet();
  
}

void sendDataToSheet(void)
{
  String url = server + "/trigger/" + eventName + "/with/key/" + IFTTT_Key + "?value1=" + String((int)value1) + "&value2="+String((int)value2);  
  Serial.println(url);
  //Start to send data to IFTTT
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  http.begin(url); //HTTP

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } 
  
  else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void wifi_(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Viola, Connected !!!");
}

void Display_(){
  display.clearDisplay();
  
  // display temperature
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.setTextSize(2);
  display.setCursor(30,10);
  display.print(value1);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");
  
  // display pressure
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Pressure: ");
  display.setTextSize(2);
  display.setCursor(30, 45);
  display.print(value2/1000);
  display.print("kPa"); 

  display.display();
}
