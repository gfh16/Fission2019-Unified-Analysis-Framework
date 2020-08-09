#include "../include/CSHINEEvent.h"


//______________________________________________________________________________
CSHINESSDEvent::CSHINESSDEvent() :
fSSDMulti(0), // 初始化列表
fMultiL1S(0),
fMultiL2F(0),
fMultiL2B(0),
fMultiCsI(0)
{
  fNumStripL1S  =   new Int_t    [NUM_STRIP];
  fEMeVL1S      =   new Double_t [NUM_STRIP];
  fNumStripL2F  =   new Int_t    [NUM_STRIP];
  fEMeVL2F      =   new Double_t [NUM_STRIP];
  fNumStripL2B  =   new Int_t    [NUM_STRIP];
  fEMeVL2B      =   new Double_t [NUM_STRIP];
  fNumCsI       =   new Int_t    [NUM_CSI];
  fEChCsI       =   new Int_t    [NUM_CSI];
}


//______________________________________________________________________________
CSHINESSDEvent::~CSHINESSDEvent()
{
  delete []  fNumStripL1S;
  delete []  fEMeVL1S;
  delete []  fNumStripL2F;
  delete []  fEMeVL2F;
  delete []  fNumStripL2B;
  delete []  fEMeVL2B;
  delete []  fNumCsI;
  delete []  fEChCsI;
}


//______________________________________________________________________________
CSHINEGlobalEvent::CSHINEGlobalEvent() :
fGlobalMulti(0)
{
  fNumOfSSD     =  new Int_t    [NUM_SSD*NUM_STRIP];
  fGNumStripL1S =  new Int_t    [NUM_SSD*NUM_STRIP];
  fGNumStripL2F =  new Int_t    [NUM_SSD*NUM_STRIP];
  fGNumStripL2B =  new Int_t    [NUM_SSD*NUM_STRIP];
  fGNumCsI      =  new Int_t    [NUM_SSD*NUM_CSI];

  fGTimeL2F     =  new Int_t    [NUM_SSD*NUM_STRIP];

  fGEMeVL1S     = new Double_t  [NUM_SSD*NUM_STRIP];
  fGEMeVL2F     = new Double_t  [NUM_SSD*NUM_STRIP];
  fGEMeVL2B     = new Double_t  [NUM_SSD*NUM_STRIP];
  fGEMeVCsI     = new Double_t  [NUM_SSD*NUM_CSI];

  fTotKinEnergy = new Double_t  [NUM_SSD*NUM_STRIP];
  fCalKinEnergy = new Double_t  [NUM_SSD*NUM_STRIP];
  fBeta         = new Double_t  [NUM_SSD*NUM_STRIP];
  fTheta        = new Double_t  [NUM_SSD*NUM_STRIP];
  fPhi          = new Double_t  [NUM_SSD*NUM_STRIP];

  fZ            = new Int_t     [NUM_SSD*NUM_STRIP];
  fA            = new Int_t     [NUM_SSD*NUM_STRIP];
}

//______________________________________________________________________________
CSHINEGlobalEvent::~CSHINEGlobalEvent()
{
  delete []  fNumOfSSD;
  delete []  fGNumStripL1S;
  delete []  fGNumStripL2F;
  delete []  fGNumStripL2B;
  delete []  fGNumCsI;
  delete []  fGTimeL2F;

  delete []  fGEMeVL1S;
  delete []  fGEMeVL2F;
  delete []  fGEMeVL2B;
  delete []  fGEMeVCsI;

  delete []  fTotKinEnergy;
  delete []  fCalKinEnergy;
  delete []  fBeta;
  delete []  fTheta;
  delete []  fPhi;

  delete []  fZ;
  delete []  fA;
}
