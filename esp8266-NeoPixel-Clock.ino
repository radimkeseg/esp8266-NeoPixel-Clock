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

/* webserver handlers */

void handle_root()
{
  String content = FPSTR(PAGE_INDEX);

  content.replace("{timeoffset}", String(timeoffset.UTC_OFFSET).c_str() );
  
  if (timeoffset.DST) content.replace("{dst}", "checked='checked'");
  else    content.replace("{dst}", "");
      
  server.send(200, "text/html", content);
}

static bool forceUpdateData = false;
void handle_store_settings(){
  if(server.arg("_dst")==NULL && server.arg("_timeoffset")==NULL ){
    Serial.println("setting page refreshed only, no params");      
  }else{
    Serial.println("settings changed");  
    timeoffset.UTC_OFFSET = atof(server.arg("_timeoffset").c_str());;
    timeoffset.DST = server.arg("_dst").length()>0;
    Serial.print("UTC TimeOffset: "); Serial.println(timeoffset.UTC_OFFSET);
    Serial.print("DST"); Serial.println(timeoffset.DST);
  
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
    
    //updateData(false);
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

void drawTime();
void updateData();
  
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

void loop() {
  // Handle web server
  server.handleClient();
  // Handle OTA update requests
  ArduinoOTA.handle();

  stamp = millis();
  if (stamp - lastDrew > 500 || stamp < lastDrew) {
    drawTime();
    lastDrew = stamp;
  }

  if(stamp - lastUpdate > UPDATE_INTERVAL_SECS * 1000 || stamp < lastUpdate || forceUpdateData){
    updateData();
    lastUpdate = stamp;  
  }
}

void drawTime(){
/* 
  Serial.print("Time ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());  
*/
  strip.clear();
  strip.setBrightness(10);
  strip.setPixelColor(timeClient.getSecondsInt(), strip.Color(100,20,20));
  strip.setPixelColor(timeClient.getMinutesInt(), strip.Color(20,100,20));
  strip.setPixelColor( ((timeClient.getHoursInt()%12)*5)+(timeClient.getMinutesInt()/12), strip.Color(20,20,100));

  strip.show();  
}

void updateData(){
  timeClient.updateTime();
  if(forceUpdateData) forceUpdateData=false;
}

