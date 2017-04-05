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
#include "embHTML.h"
#include "ITimer.h"
#include "Clock.h"
#include "CuckooRainbowCycle.h"

// HOSTNAME for OTA update
#define HOSTNAME "WSC-ESP8266-OTA-"

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

Clock clock(&strip);
CuckooRainbowCycle cuckoo(&strip);

/*prototypes*/
void updateData();

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
  clock.SetTimeOffset(timeoffset.UTC_OFFSET+timeoffset.DST);
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

  clock.SetTimeOffset(timeoffset.UTC_OFFSET+timeoffset.DST);
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

  if(clock.getMinsInt() == 0 && clock.getSecsInt()<5){
    cuckoo.Show(); delay(5);
    clear = false;
  }else{
    clear = true;
  }

  stamp = millis();
  if (stamp - lastDrew > 500 || stamp < lastDrew || !clear) {
    clock.Show(clear, clear);
    lastDrew = stamp;
  }

  if(stamp - lastUpdate > UPDATE_INTERVAL_SECS * 1000 || stamp < lastUpdate || forceUpdateData){
    updateData();
    lastUpdate = stamp;  
  }
}


void updateData(){
  strip.setBrightness(timeoffset.brightness);
  clock.SetTimeOffset(timeoffset.UTC_OFFSET+timeoffset.DST);
  clock.SetUp(ITimer::hex2rgb(timeoffset.color_hand_hour), ITimer::hex2rgb(timeoffset.color_hand_mins), ITimer::hex2rgb(timeoffset.color_hand_secs));
  
  if(forceUpdateData) forceUpdateData=false;
}
