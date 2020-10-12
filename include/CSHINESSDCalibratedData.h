#ifndef CSHINESSDCalibratedData_H
#define CSHINESSDCalibratedData_H

#include "ReadFileModule.h"
#include "shared.h"

#include <string>
#include <iostream>
#include <fstream>

//______________________________________________________________________________
class CSHINESSDCalibratedData
{
public:
  CSHINESSDCalibratedData();
  ~CSHINESSDCalibratedData();

  Double_t    *GetSiEChPedestals  (const char* layertag);                      // [NUM_SSD*NUM_STRIP]

  Double_t    *GetSiEChCut        (const char* layertag);                      // [NUM_SSD*NUM_STRIP]
  Double_t    *GetSiEChCut        (const char* layertag, Double_t numsigma);  // [NUM_SSD*NUM_STRIP]

  Double_t    *GetSiCaliSlope     (const char* layertag);                      // [NUM_SSD*NUM_STRIP]
  Double_t    *GetSiCaliIntercept (const char* layertag);                      // [NUM_SSD*NUM_STRIP]
  Double_t    *GetSiEMeVCut       (const char* layertag);                      // [NUM_SSD*NUM_STRIP]
  Double_t     GetSiEMeV          (Int_t ssdindex, const char* layertag, Int_t chindex, Int_t ech);

  Double_t    *GetCsIEChCut       (const char* layertag);                     // [NUM_SSD*NUM_CSI]
  Double_t    *GetCsIEChCut       (const char* layertag, Double_t numsigma);  // [NUM_SSD*NUM_CSI]

private:
  Double_t    *fL1SEChPedestals;   // [NUM_SSD*NUM_STRIP]
  Double_t    *fL2FEChPedestals;   // [NUM_SSD*NUM_STRIP]
  Double_t    *fL2BEChPedestals;   // [NUM_SSD*NUM_STRIP]

  Double_t    *fL1SEChCut;         // [NUM_SSD*NUM_STRIP]
  Double_t    *fL2FEChCut;         // [NUM_SSD*NUM_STRIP]
  Double_t    *fL2BEChCut;         // [NUM_SSD*NUM_STRIP]
  Double_t    *fCsIEChCut;         // [NUM_SSD*NUM_CSI]

  Double_t    *fL1SSlope;          // [NUM_SSD*NUM_STRIP]
  Double_t    *fL2FSlope;          // [NUM_SSD*NUM_STRIP]
  Double_t    *fL2BSlope;          // [NUM_SSD*NUM_STRIP]

  Double_t    *fL1SIntercept;      // [NUM_SSD*NUM_STRIP]
  Double_t    *fL2FIntercept;      // [NUM_SSD*NUM_STRIP]
  Double_t    *fL2BIntercept;      // [NUM_SSD*NUM_STRIP]

  Double_t    *fL1SEMeVCut;        // [NUM_SSD*NUM_STRIP]
  Double_t    *fL2FEMeVCut;        // [NUM_SSD*NUM_STRIP]
  Double_t    *fL2BEMeVCut;        // [NUM_SSD*NUM_STRIP]

  ReadFileModule  readfile;
};

#endif
