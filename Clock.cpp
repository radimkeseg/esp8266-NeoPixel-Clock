#include "Clock.h"
    
//public:
void Clock::SetUp(uint16_t color_Hand_hour, uint16_t color_Hand_mins, uint16_t color_Hand_secs){
  color_hand_hour = color_Hand_hour;
  color_hand_mins = color_Hand_mins;
  color_hand_secs = color_Hand_secs;
}
        
void Clock::Show(boolean clear_background, boolean mix_colors){
  uint32_t color = 0;
  int pos = 0;

  long stamp = millis();
  if(stamp - last_time_update > time_update_interval || stamp < last_time_update ){
    last_time_update = stamp;
    timeClient.updateTime();
  }

  if(strip == NULL) return;
  
  if(clear_background) strip->clear();
  
  pos = timeClient.getSecondsInt();
  pos = (pos +30)%60;
  if(mix_colors){
    color = strip->getPixelColor(pos);
    strip->setPixelColor(pos, mixColors(color, color_hand_secs));
  }else{
    strip->setPixelColor(pos, color_hand_secs);
  }
  
  pos = timeClient.getMinutesInt();
  pos = (pos +30)%60;
  if(mix_colors){
    color = strip->getPixelColor(pos);
    strip->setPixelColor(pos, mixColors(color, color_hand_mins));
  }else{
    strip->setPixelColor(pos, color_hand_mins);  
  }
  
  pos = ((timeClient.getHoursInt()%12)*5)+(timeClient.getMinutesInt()/12);
  pos = (pos +30)%60;
  if(mix_colors){
    color = strip->getPixelColor(pos);
    strip->setPixelColor( pos, mixColors(color, color_hand_hour));
  }else{
    strip->setPixelColor( pos, color_hand_hour);    
  }

  strip->show();  
}

void Clock::SetTimeOffset(int time_offset){
  timeClient.setTimeOffset(time_offset);
  timeClient.updateTime();
}

