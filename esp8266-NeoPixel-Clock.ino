/**The MIT License (MIT)
Copyright (c) 2017 by Radim Keseg
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

// Helps with connecting to internet
#include <WiFiManager.h>
#include <FS.h>

#include <Adafruit_NeoPixel.h>

// check settings.h for adapting to your needs
#include "settings.h"
#include "TimeClient.h"

#include "embHTML.h"

// HOSTNAME for OTA update
#define HOSTNAME "WSC-ESP8266-OTA-"

TimeClient timeClient(timeoffset.UTC_OFFSET+timeoffset.DST);

//WiFiManager
//Local intialization. Once its business is done, there is no need to keep it around
WiFiManager wifiManager;
ESP8266WebServer server(80);

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEOPIXEL_DATA_IN_PIN, NEO_GRB + NEO_KHZ800);

uint32_t color_hand_hour = 0;
uint32_t color_hand_mins = 0;
uint32_t color_hand_secs = 0;

/*prototypes*/
void drawTime();
void updateData();
uint32_t mixColors(uint32_t c1, uint32_t c2);

/* webserver handlers */
void handle_root()
{
  String content = FPSTR(PAGE_INDEX);

  content.replace("{timeoffset}", String(timeoffset.UTC_OFFSET).c_str() );
  
  if (timeoffset.DST) content.replace("{dst}", "checked='checked'");
  else    content.replace("{dst}", "");

  content.replace("{brightness}", String(timeoffset.brightness).c_str());

  content.replace("{val-hand-hour}", timeoffset.color_hand_hour);
  content.replace("{val-hand-mins}", timeoffset.color_hand_mins);
  content.replace("{val-hand-secs}", timeoffset.color_hand_secs);
      
  server.send(200, "text/html", content);
}

static bool forceUpdateData = false;
void handle_store_settings(){
  if(server.arg("_dst")==NULL && server.arg("_timeoffset")==NULL ){
    Serial.println("setting page refreshed only, no params");      
  }else{
    Serial.println("settings changed");  
    timeoffset.UTC_OFFSET = atof(server.arg("_timeoffset").c_str());
    timeoffset.DST = server.arg("_dst").length()>0;
    timeoffset.brightness = atoi(server.arg("_brightness").c_str());
    strncpy(timeoffset.color_hand_hour, server.arg("_input-hand-hour").c_str(), 8);
    strncpy(timeoffset.color_hand_mins, server.arg("_input-hand-mins").c_str(), 8);
    strncpy(timeoffset.color_hand_secs, server.arg("_input-hand-secs").c_str(), 8);
    Serial.print("UTC TimeOffset: "); Serial.println(timeoffset.UTC_OFFSET);
    Serial.print("DST"); Serial.println(timeoffset.DST);
    Serial.print("brightness"); Serial.println(timeoffset.brightness);
    Serial.print("color hand hour "); Serial.println(timeoffset.color_hand_hour);
    Serial.print("color hand mins "); Serial.println(timeoffset.color_hand_mins);
    Serial.print("color hand secs "); Serial.println(timeoffset.color_hand_secs);
          
    Serial.println("writing custom setting start");
    Serial.println("file: " + CUSTOM_SETTINGS);
    //write location to SPIFF
    File f = SPIFFS.open(CUSTOM_SETTINGS, "w");
    if (f){
      f.write((uint8_t*) &timeoffset, sizeof(timeoffset_t));
    }else{
      Serial.println("open file for writing failed: " + CUSTOM_SETTINGS);
    }
    f.flush();
    f.close();
    Serial.println("writing custom setting end");
    
    updateData();
    forceUpdateData = true;
  }
  timeClient.setTimeOffset(timeoffset.UTC_OFFSET+timeoffset.DST);
  server.send(200, "text/html", "OK");
}

void read_custom_settings(){
    //read setting from SPIFF
    Serial.println("reading custom setting start");
    File f = SPIFFS.open(CUSTOM_SETTINGS, "r");
    if(f){
       f.read((uint8_t*) &timeoffset, sizeof(timeoffset_t));
    }else{
      Serial.println("open file for reading failed: " + CUSTOM_SETTINGS);
    }
    f.close();
    Serial.println("reading custom setting end");

  timeClient.setTimeOffset(timeoffset.UTC_OFFSET+timeoffset.DST);
}
/**/

// Called if WiFi has not been configured yet
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Wifi Manager");
  Serial.println("Please connect to AP");
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.println("To setup Wifi Configuration");
}
 
void setup() {
  //pinMode(NEOPIXEL_DATA_IN_PIN, OUTPUT);
  Serial.begin(9600);

  // Uncomment for testing wifi manager
  //wifiManager.resetSettings();
  wifiManager.setAPCallback(configModeCallback);
  //or use this for auto generated name ESP + ChipID
  wifiManager.autoConnect();

  //Manual Wifi
  //WiFi.begin(WIFI_SSID, WIFI_PWD);


  // OTA Setup
  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  WiFi.hostname(hostname);
  ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.begin();
  SPIFFS.begin();
  read_custom_settings();   

  //user setting handling
  server.on("/", handle_root);
  server.on("/offset", handle_store_settings);
  server.begin(); 
  Serial.println("HTTP server started"); 
  
  //Uncomment if you want to format FS
  //SPIFFS.format();

  strip.begin();
  strip.clear();
  strip.show();
  
  updateData();
}

long lastDrew = 0;
long lastUpdate = 0;

long stamp = 0;
boolean clear = true;

void loop() {
  // Handle web server
  server.handleClient();
  // Handle OTA update requests
  ArduinoOTA.handle();

  if(timeClient.getMinutesInt() == 0 && timeClient.getSecondsInt()<5){
    rainbowCycle(5);
    clear = false;
  }else{
    clear = true;
  }

  stamp = millis();
  if (stamp - lastDrew > 500 || stamp < lastDrew || !clear) {
    drawTime(clear);
    lastDrew = stamp;
  }

  if(stamp - lastUpdate > UPDATE_INTERVAL_SECS * 1000 || stamp < lastUpdate || forceUpdateData){
    updateData();
    lastUpdate = stamp;  
  }
}

int offset = 30;
void drawTime(boolean clear){
/* 
  Serial.print("Time ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());  
*/
  uint32_t color = 0;
  int pos = 0;
  
  if(clear) strip.clear();
  strip.setBrightness(timeoffset.brightness);
  pos = timeClient.getSecondsInt();
  pos = (pos +30)%60;
  color = strip.getPixelColor(pos);
  strip.setPixelColor(pos, mixColors(color, color_hand_secs));

  pos = timeClient.getMinutesInt();
  pos = (pos +30)%60;
  color = strip.getPixelColor(pos);
  strip.setPixelColor(pos, mixColors(color, color_hand_mins));

  pos = ((timeClient.getHoursInt()%12)*5)+(timeClient.getMinutesInt()/12);
  pos = (pos +30)%60;
  color = strip.getPixelColor(pos);
  strip.setPixelColor( pos, mixColors(color, color_hand_hour));

  strip.show();  
}

void updateData(){
  timeClient.updateTime();

  color_hand_hour = hex2rgb(timeoffset.color_hand_hour);
  color_hand_mins = hex2rgb(timeoffset.color_hand_mins);
  color_hand_secs = hex2rgb(timeoffset.color_hand_secs);
  
  if(forceUpdateData) forceUpdateData=false;
}


//------------------------------------------------------------------------
//color functions
//------------------------------------------------------------------------

uint32_t mixColors(uint32_t c1, uint32_t c2){
  uint8_t w1 = c1>>24 & 0xFF;
  uint8_t r1 = c1>>16 & 0xFF;
  uint8_t g1 = c1>>8 & 0xFF;
  uint8_t b1 = c1 & 0xFF;
  
  uint8_t w2 = c2>>24 & 0xFF;
  uint8_t r2 = c2>>16 & 0xFF;
  uint8_t g2 = c2>>8 & 0xFF;
  uint8_t b2 = c2 & 0xFF;

  uint8_t w = w1/2+w2/2;
  uint8_t r = r1/2+r2/2;
  uint8_t g = g1/2+g2/2;
  uint8_t b = b1/2+b2/2;

  return ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

uint32_t hex2rgb(char* hexstring) {
    if(hexstring==NULL) return 0;

    Serial.println(hexstring);
    Serial.print("len ");
    Serial.println(strlen(hexstring));
    
    if(strlen(hexstring)==4){
      long number = (long) strtol( &hexstring[1], NULL, 16);
      int r = number >> 8;
      int g = number >> 4 & 0xF;
      int b = number & 0xF;
    Serial.print("r="); Serial.print(r);
    Serial.print(",g="); Serial.print(g);
    Serial.print(",b="); Serial.println(b);
      return strip.Color(r,g,b);
    }

    if(strlen(hexstring)==7){
      long number = (long) strtol( &hexstring[1], NULL, 16);
      int r = number >> 16;
      int g = number >> 8 & 0xFF;
      int b = number & 0xFF;
    Serial.print("r="); Serial.print(r);
    Serial.print(",g="); Serial.print(g);
    Serial.print(",b="); Serial.println(b);
      return strip.Color(r,g,b);
    }

    return 0;
}

//------------------------------------------------------------------------
//effects
//------------------------------------------------------------------------

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(strip.numPixels()-i-1, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

