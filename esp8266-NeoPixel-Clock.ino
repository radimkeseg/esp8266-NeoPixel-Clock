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
#include <ESP8266HTTPUpdateServer.h>

// Helps with connecting to internet
#include <WiFiManager.h>

#include <Adafruit_NeoPixel.h>

// check settings.h for adapting to your needs
#include "settings.h"
#include "embHTML.h"
#include "ITimer.h"
#include "Clock.h"
#include "CuckooRainbowCycle.h"
#include "CuckooAlarm.h"

// HOSTNAME for OTA update
#define HOSTNAME "WSC-ESP8266-"

//WiFiManager
//Local intialization. Once its business is done, there is no need to keep it around
WiFiManager wifiManager;
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEOPIXEL_DATA_IN_PIN, NEO_GRB + NEO_KHZ800);

CustomSettings cs; 

Clock clock(&strip, 30);
CuckooRainbowCycle cuckoo(&strip);
CuckooAlarm buzzer(&strip);

/*prototypes*/
void updateData();

/* webserver handlers */
void handle_root()
{
  String content = FPSTR(PAGE_INDEX);

  content.replace("{timeoffset}", String(cs.settings.UTC_OFFSET).c_str() );
  
  if (cs.settings.DST) content.replace("{dst}", "checked='checked'");
  else    content.replace("{dst}", "");

  content.replace("{brightness}", String(cs.settings.brightness).c_str());
  content.replace("{brightness_night}", String(cs.settings.brightness_night).c_str());

  content.replace("{val-hand-hour}", cs.settings.color_hand_hour);
  content.replace("{val-hand-mins}", cs.settings.color_hand_mins);
  content.replace("{val-hand-secs}", cs.settings.color_hand_secs);
  content.replace("{val-segm-hour}", cs.settings.color_segm_hour);
  content.replace("{val-dots-hour}", cs.settings.color_dots_hour);
  content.replace("{val-dots-qrtr}", cs.settings.color_dots_qrtr);

  if (cs.settings.ALARM_SWITCH) content.replace("{alarm}", "checked='checked'");
  else    content.replace("{alarm}", "");

  content.replace("{alarmHour}", String(cs.settings.alarmHour).c_str());
  content.replace("{alarmMins}", String(cs.settings.alarmMins).c_str());
      
  server.send(200, "text/html", content);
}

static bool forceUpdateData = false;
void handle_store_settings(){
  if(server.arg("_dst")==NULL && server.arg("_timeoffset")==NULL ){
    Serial.println("setting page refreshed only, no params");      
  }else{
    Serial.println("settings changed");  
    cs.settings.UTC_OFFSET = atof(server.arg("_timeoffset").c_str());
    cs.settings.DST = server.arg("_dst").length()>0;
    cs.settings.brightness = atoi(server.arg("_brightness").c_str());
    cs.settings.brightness_night = atoi(server.arg("_brightness_night").c_str());
    strncpy(cs.settings.color_hand_hour, server.arg("_input-hand-hour").c_str(), 8);
    strncpy(cs.settings.color_hand_mins, server.arg("_input-hand-mins").c_str(), 8);
    strncpy(cs.settings.color_hand_secs, server.arg("_input-hand-secs").c_str(), 8);
    strncpy(cs.settings.color_segm_hour, server.arg("_input-segm-hour").c_str(), 8);
    strncpy(cs.settings.color_dots_hour, server.arg("_input-dots-hour").c_str(), 8);
    strncpy(cs.settings.color_dots_qrtr, server.arg("_input-dots-qrtr").c_str(), 8);
    cs.settings.ALARM_SWITCH = server.arg("_alarm").length()>0;
    cs.settings.alarmHour = atoi(server.arg("_alarmHour").c_str());
    cs.settings.alarmMins = atoi(server.arg("_alarmMins").c_str());
    
    cs.print();          
    cs.write();
    
    updateData();
    forceUpdateData = true;
  }
  clock.SetTimeOffset(cs.settings.UTC_OFFSET+cs.settings.DST);
  server.send(200, "text/html", "OK");
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
//  ArduinoOTA.setHostname((const char *)hostname.c_str());
//  ArduinoOTA.begin();
  
  cs.init();
  cs.read();   
  cs.print();

  clock.SetTimeOffset(cs.settings.UTC_OFFSET+cs.settings.DST);   

  MDNS.begin( (hostname+"-webupdate").c_str() );
  httpUpdater.setup(&server, "/firmware", update_username, update_password );


  //user setting handling
  server.on("/", handle_root);
  server.on("/offset", handle_store_settings);
  server.begin(); 
  Serial.println("HTTP server started");

  MDNS.addService("http", "tcp", 80);
  
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
//  ArduinoOTA.handle();


  
  clear = true;

  //effect by alarm
  if(cs.settings.ALARM_SWITCH)
  if(clock.getHourInt() == cs.settings.alarmHour && clock.getMinsInt() == cs.settings.alarmMins){
    buzzer.Show();
    clear = false;
  }

  // cuckoo effect by the full hour
  if(clock.getMinsInt() == 0 && clock.getSecsInt()<10){
    cuckoo.Show(); 
    clear = false;
  }

  // show clock
  stamp = millis();
  if (stamp - lastDrew > 500 || stamp < lastDrew || !clear) {
    
    //adjust brightness
    if(clock.getHourInt()>=22 || clock.getHourInt()<6) strip.setBrightness(cs.settings.brightness);
    else strip.setBrightness(cs.settings.brightness_night);

    //show clock hands
    clock.Show(clear, clear); delay(10);
    lastDrew = stamp;
  }

  if(stamp - lastUpdate > UPDATE_INTERVAL_SECS * 1000 || stamp < lastUpdate || forceUpdateData){
    updateData();
    lastUpdate = stamp;  
  }
}


void updateData(){
  clock.SetTimeOffset(cs.settings.UTC_OFFSET+cs.settings.DST);
  clock.SetUp(ITimer::hex2rgb(
    cs.settings.color_hand_hour), ITimer::hex2rgb(cs.settings.color_hand_mins), ITimer::hex2rgb(cs.settings.color_hand_secs), ITimer::hex2rgb(cs.settings.color_segm_hour), 
    ITimer::hex2rgb(cs.settings.color_dots_hour), ITimer::hex2rgb(cs.settings.color_dots_qrtr)
  );
  
  if(forceUpdateData) forceUpdateData=false;
}
