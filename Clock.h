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

#include "ITimer.h"
#include "TimeClient.h"



class Clock : virtual public ITimer
{
    private:
      uint32_t color_hand_hour = 0;
      uint32_t color_hand_mins = 0; 
      uint32_t color_hand_secs = 0;
      uint32_t color_segm_hour = 0; //hour segment
      uint32_t color_dots_hour = 0; //hour segment

      uint8_t brightness = 10;
      
      TimeClient timeClient;
  
      const long time_update_interval = 12*60*60*1000; //update every 12 hrs
      long last_time_update = time_update_interval;
    
    public:
      Clock(Adafruit_NeoPixel *strip) : ITimer(strip){}
    
      virtual void Show(boolean clear_background, boolean mix_colors);
      virtual void Show(){ Show(false, false); }

      virtual void SetUp(uint32_t color_hand_hour, uint32_t color_hand_mins, uint32_t color_hand_secs){ SetUp(color_hand_hour, color_hand_mins, color_hand_secs, 0, 0); }        
      virtual void SetUp(uint32_t color_hand_hour, uint32_t color_hand_mins, uint32_t color_hand_secs, uint32_t color_segm_hour, uint32_t color_dots_hour);        
      virtual void SetTimeOffset(int time_offset);

      int getHourInt(){ return timeClient.getHoursInt();}
      int getMinsInt(){ return timeClient.getMinutesInt(); }
      int getSecsInt(){ return timeClient.getSecondsInt(); }      
};
