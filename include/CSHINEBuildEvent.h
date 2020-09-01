#ifndef CSHINEBUILDEVENT_H
#define CSHINEBUILDEVENT_H

#include "shared.h"
#include "CSHINESSDCalibratedData.h"
#include "CSHINEEvent.h"

#include <vector>


//__________________________________________
class CSHINEBuildEvent
{
public:
  CSHINEBuildEvent();
  virtual ~CSHINEBuildEvent();

  // 参数说明: CSHINESSDEvent class, ssdindex, ech_L1S[16], ech_L2F[16], ech_L2B[16], ech_L3A[9]
  void BuildSSDEvent(CSHINESSDEvent&, Int_t, Int_t*, Int_t*, Int_t*, Int_t*);
  void BuildGlobalEvent(CSHINEGlobalEvent&,  Int_t*, Int_t*, Int_t*, Int_t*);

private:

  Double_t               *fSiEChcutl1s;   // [NUM_SSD*NUM_STRIP]
  Double_t               *fSiEChcutl2f;   // [NUM_SSD*NUM_STRIP]
  Double_t               *fSiEChcutl2b;   // [NUM_SSD*NUM_STRIP]
  Double_t               *fCsIEChcutl3a;  // [NUM_SSD*NUM_CSI]

  Double_t               *fSlopel1s;      // [NUM_SSD*NUM_STRIP]
  Double_t               *fSlopel2f;      // [NUM_SSD*NUM_STRIP]
  Double_t               *fSlopel2b;      // [NUM_SSD*NUM_STRIP]
  Double_t               *fInterceptl1s;  // [NUM_SSD*NUM_STRIP]
  Double_t               *fInterceptl2f;  // [NUM_SSD*NUM_STRIP]
  Double_t               *fInterceptl2b;  // [NUM_SSD*NUM_STRIP]

  CSHINESSDCalibratedData fCSHINESSDCalibratedData;

};

#endif
