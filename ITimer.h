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
