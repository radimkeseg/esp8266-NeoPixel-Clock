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

#include "ITimer.h"

uint32_t ITimer::hex2rgb(char* hexstring) {
    if(hexstring==NULL) return 0;

    if(strlen(hexstring)==4){
      long number = (long) strtol( &hexstring[1], NULL, 16);
      int r = number >> 8;
      int g = number >> 4 & 0xF;
      int b = number & 0xF;
      return Adafruit_NeoPixel::Color(r+r>>4,g+g>>4,b+b>>4);
   }

    if(strlen(hexstring)==7){
      long number = (long) strtol( &hexstring[1], NULL, 16);
      int r = number >> 16;
      int g = number >> 8 & 0xFF;
      int b = number & 0xFF;
      return Adafruit_NeoPixel::Color(r,g,b);
    }

    return 0;
}      

uint32_t ITimer::mixColors(uint32_t c1, uint32_t c2){
    uint8_t w1 = c1>>24 & 0xFF;
    uint8_t r1 = c1>>16 & 0xFF;
    uint8_t g1 = c1>>8 & 0xFF;
    uint8_t b1 = c1 & 0xFF;
    
    uint8_t w2 = c2>>24 & 0xFF;
    uint8_t r2 = c2>>16 & 0xFF;
    uint8_t g2 = c2>>8 & 0xFF;
    uint8_t b2 = c2 & 0xFF;
  
    uint8_t w = w1/2+w2/2;
    uint8_t r = r1/2+r2/2;
    uint8_t g = g1/2+g2/2;
    uint8_t b = b1/2+b2/2;
  
    return ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}  

uint32_t ITimer::wheel(byte WheelPos) {
  if(strip==NULL) return 0;
  
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return Adafruit_NeoPixel::Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return Adafruit_NeoPixel::Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return Adafruit_NeoPixel::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

