#include "CuckooRainbowCycle.h"

//public 
void CuckooRainbowCycle::Show(boolean clear_background, boolean mix_colors){
  if(strip==NULL) return;

  if(j>=256*5) j=0;
  if(i>=strip->numPixels()) i=0;

  strip->setPixelColor(strip->numPixels()-i-1, wheel(((i * 256 / strip->numPixels()) + j) & 255));
  strip->show();

  j++; i++;
}

