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
#pragma once

#include <FS.h> 

// Setup
const int UPDATE_INTERVAL_SECS = 24* 10 * 60;  // Update every 24 hrs

// pins for the touchscreen
#define NEOPIXEL_DATA_IN_PIN 2

// TimeClient settings
typedef struct
{
  float UTC_OFFSET;
  boolean DST; 
  uint8_t brightness;
  boolean ALARM_SWITCH; 
  uint8_t alarmHour;
  uint8_t alarmMins;
  char color_hand_hour[8];
  char color_hand_mins[8];
  char color_hand_secs[8];
  char color_segm_hour[8];
  char color_dots_hour[8];
  char color_dots_qrtr[8];
} settings_t;


/***************************
 * End Settings
 **************************/

class CustomSettings{
  private:
     const String CUSTOM_SETTINGS = "/settings/custom.txt";
  
  public:
    settings_t settings;

    CustomSettings(){
      settings.UTC_OFFSET = 1;
      settings.DST = false;
      settings.brightness = 10;
      settings.ALARM_SWITCH = false;
      settings.alarmHour = 8;
      settings.alarmMins = 0;
      strncpy(settings.color_hand_hour, "#ff0000", 8);
      strncpy(settings.color_hand_mins, "#00aa00", 8);
      strncpy(settings.color_hand_secs, "#220022", 8);
      strncpy(settings.color_segm_hour, "#222200", 8);
      strncpy(settings.color_dots_hour, "#000022", 8);
      strncpy(settings.color_dots_qrtr, "#000055", 8);
    }
    void init();

    void write();
    void read();
    void print();
}; 

