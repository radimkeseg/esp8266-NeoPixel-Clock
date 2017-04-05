#pragma once

#include "ITimer.h"
#include "TimeClient.h"



class Clock : virtual public ITimer
{
    private:
      uint16_t color_hand_hour = 0;
      uint16_t color_hand_mins = 0; 
      uint16_t color_hand_secs = 0;

      uint8_t brightness = 10;
      
      TimeClient timeClient;
  
      const long time_update_interval = 12*60*60*1000;
      long last_time_update = time_update_interval;
    
    public:
      Clock(Adafruit_NeoPixel *strip) : ITimer(strip){}
    
      virtual void Show(boolean clear_background, boolean mix_colors);
      virtual void Show(){ Show(false, false); }

      virtual void SetUp(uint16_t color_Hand_hour, uint16_t color_Hand_mins, uint16_t color_Hand_secs);        
      virtual void SetTimeOffset(int time_offset);

      int getHourInt(){ return timeClient.getHoursInt();}
      int getMinsInt(){ return timeClient.getMinutesInt(); }
      int getSecsInt(){ return timeClient.getSecondsInt(); }      
};
