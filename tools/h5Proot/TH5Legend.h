#ifndef __TH5Legend__
#define __TH5Legend__

#include <TLegend.h>

#include<string>

// a slightly customized version of TLegend

class TH5Legend : public TLegend
{
 public:
  TH5Legend();
  TH5Legend( Double_t x1, Double_t y1, Double_t x2, Double_t y2);
  ~TH5Legend(); // Destructor
  
  ClassDef(TH5Legend,1);
    
 private:
  
};

#endif
