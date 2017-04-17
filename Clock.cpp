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

#include "Clock.h"
    
//public:
void Clock::SetUp(uint32_t color_hand_hour, uint32_t color_hand_mins, uint32_t color_hand_secs, uint32_t color_segm_hour, uint32_t color_dots_hour, uint32_t color_dots_qrtr){
  this->color_hand_hour = color_hand_hour;
  this->color_hand_mins = color_hand_mins;
  this->color_hand_secs = color_hand_secs;
  this->color_segm_hour = color_segm_hour;
  this->color_dots_hour = color_dots_hour;
  this->color_dots_qrtr = color_dots_qrtr;
}
        
void Clock::Show(boolean clear_background, boolean mix_colors){
  uint32_t color = 0;
  int pos = 0;

  if(strip == NULL) return;
  
  long stamp = millis();
  if(stamp - last_time_update > time_update_interval || stamp < last_time_update ){
    last_time_update = stamp;
    timeClient.updateTime();
  }

  if(clear_background) strip->clear();
  
  int hour_pos = ((timeClient.getHoursInt()%12)*5);
  int mins_pos = timeClient.getMinutesInt();
  int secs_pos = timeClient.getSecondsInt();

  int count = strip->numPixels(); 
  for(int i=0; i<count; i++){
    pos=(i + pixel_offset)%count;
    if(i%5 == 0){ //full hour dots
      setPixel( pos, color_dots_hour ,mix_colors);
    }
    if(i%15 == 0){ // quarter dots
      setPixel( pos, color_dots_qrtr ,mix_colors);      
    }
    if(i > hour_pos && i < (hour_pos + 5)){ // hour segment
      setPixel( pos, color_segm_hour ,mix_colors);            
    }
    if(i == secs_pos){ // second hand
      setPixel( pos, color_hand_secs ,mix_colors);            
    }
    if(i == mins_pos){ // minute hand
      setPixel( pos, color_hand_mins ,false);            
    }
    if(i == hour_pos){ // hour hand
      setPixel( pos, color_hand_hour ,false);            
    }
  }
   
  strip->show();  
}

void Clock::SetTimeOffset(int time_offset){
  timeClient.setTimeOffset(time_offset);
  timeClient.updateTime();
}

//private
void Clock::setPixel(int pos, uint32_t color, boolean mix_color){
  if(mix_color){
    uint32_t bg_color = strip->getPixelColor(pos);
    if( bg_color == 0 ) //if bg color is black dont mix just draw
      strip->setPixelColor(pos, color);
    else
      strip->setPixelColor(pos, mixColors(bg_color, color));
  }else{
    strip->setPixelColor(pos, color);
  }
}

