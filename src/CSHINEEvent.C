#include "../include/CSHINEEvent.h"


//______________________________________________________________________________
CSHINESSDEvent::CSHINESSDEvent() :
fSSDMulti(0), // 初始化列表
fL1SMulti(0),
fL2FMulti(0),
fL2BMulti(0),
fCsIMulti(0)
{
  fL1SNumStrip  =   new Int_t    [NUM_STRIP];
  fL1SEMeV      =   new Double_t [NUM_STRIP];
  fL2FNumStrip  =   new Int_t    [NUM_STRIP];
  fL2FEMeV      =   new Double_t [NUM_STRIP];
  fL2BNumStrip  =   new Int_t    [NUM_STRIP];
  fL2BEMeV      =   new Double_t [NUM_STRIP];
  fCsINum       =   new Int_t    [NUM_CSI];
  fCsIECh       =   new Int_t    [NUM_CSI];
}


//______________________________________________________________________________
CSHINESSDEvent::~CSHINESSDEvent()
{
  delete []  fL1SNumStrip;
  delete []  fL1SEMeV;
  delete []  fL2FNumStrip;
  delete []  fL2FEMeV;
  delete []  fL2BNumStrip;
  delete []  fL2BEMeV;
  delete []  fCsINum;
  delete []  fCsIECh;
}


//______________________________________________________________________________
CSHINEGlobalEvent::CSHINEGlobalEvent() :
fGlobalMulti(0)
{
  fGNumOfSSD    =  new Int_t     [NUM_SSD*NUM_STRIP];
  fGL1SNumStrip =  new Int_t     [NUM_SSD*NUM_STRIP];
  fGL2FNumStrip =  new Int_t     [NUM_SSD*NUM_STRIP];
  fGL2BNumStrip =  new Int_t     [NUM_SSD*NUM_STRIP];
  fGCsINum      =  new Int_t     [NUM_SSD*NUM_CSI];

  fGL1SEMeV     =  new Double_t  [NUM_SSD*NUM_STRIP];
  fGL2FEMeV     =  new Double_t  [NUM_SSD*NUM_STRIP];
  fGL2BEMeV     =  new Double_t  [NUM_SSD*NUM_STRIP];
  fGCsIEMeV     =  new Double_t  [NUM_SSD*NUM_CSI];

  fGL2FTime     =  new Int_t     [NUM_SSD*NUM_STRIP];

  fTotKinEnergy =  new Double_t  [NUM_SSD*NUM_STRIP];
  fCalKinEnergy =  new Double_t  [NUM_SSD*NUM_STRIP];
  fBeta         =  new Double_t  [NUM_SSD*NUM_STRIP];
  fTheta        =  new Double_t  [NUM_SSD*NUM_STRIP];
  fPhi          =  new Double_t  [NUM_SSD*NUM_STRIP];

  fZ            =  new Int_t     [NUM_SSD*NUM_STRIP];
  fA            =  new Int_t     [NUM_SSD*NUM_STRIP];
}

//______________________________________________________________________________
CSHINEGlobalEvent::~CSHINEGlobalEvent()
{
  delete []  fGNumOfSSD;
  delete []  fGL1SNumStrip;
  delete []  fGL2FNumStrip;
  delete []  fGL2BNumStrip;
  delete []  fGCsINum;

  delete []  fGL1SEMeV;
  delete []  fGL2FEMeV;
  delete []  fGL2BEMeV;
  delete []  fGCsIEMeV;

  delete []  fGL2FTime;

  delete []  fTotKinEnergy;
  delete []  fCalKinEnergy;
  delete []  fBeta;
  delete []  fTheta;
  delete []  fPhi;

  delete []  fZ;
  delete []  fA;
}
