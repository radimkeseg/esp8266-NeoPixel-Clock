#include "CuckooRainbowCycle.h"

//public 
void CuckooRainbowCycle::Show(boolean clear_background, boolean mix_colors){
  if(strip==NULL) return;

  if(j>255) j=0;
  for(int i=0; i< strip->numPixels(); i++) {
    strip->setPixelColor(strip->numPixels()-i-1, wheel(((i * 256 / strip->numPixels()) + j) & 255));
  }  
  strip->show();
  j++;
}

