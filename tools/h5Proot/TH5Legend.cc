#include "TH5Legend.h"

//ClassImp(TH5Legend)

TH5Legend::TH5Legend() : TLegend() { }

TH5Legend::TH5Legend( Double_t x1, Double_t y1, Double_t x2, Double_t y2)
  : TLegend(x1,y1,x2,y2) {
  
  SetTextFont(62); 
  SetTextSize(0.035);
  SetFillColor(0);
  SetBorderSize(0);
 
}

TH5Legend::~TH5Legend() {} // Destructor


