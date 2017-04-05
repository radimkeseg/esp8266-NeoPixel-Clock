#pragma once

#include <Adafruit_NeoPixel.h>

class ITimer
{
    public:
      ITimer(Adafruit_NeoPixel *sTrip){
        strip = sTrip; 
      }
      virtual void Show(boolean clear_background, boolean mix_colors) = 0;

      virtual void Show(){ Show(false, false); }
      virtual ~ITimer() {}

      uint32_t static hex2rgb(char* hexstring);
      uint32_t static mixColors(uint32_t c1, uint32_t c2);

    protected:
      Adafruit_NeoPixel *strip;
            
      uint32_t wheel(byte WheelPos);
};
