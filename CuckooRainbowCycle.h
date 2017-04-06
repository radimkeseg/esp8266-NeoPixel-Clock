#pragma once

#include "ITimer.h"

class CuckooRainbowCycle : virtual public ITimer
{
    private:
      uint16_t j=0;

    public:
      CuckooRainbowCycle(Adafruit_NeoPixel *strip) : ITimer(strip){}
      
      virtual void Show(boolean clear_background, boolean mix_colors);
      virtual void Show(){ Show(false, false); }
};
