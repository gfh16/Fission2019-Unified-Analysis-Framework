#ifndef CSHINEEVENT_H
#define CSHINEEVENT_H

#include "shared.h"
#include "CSHINESSDCalibratedData.h"
#include "ReadFileModule.h"
#include "TimeAndPercentage.h"

#include <vector>
#include <string>
#include "TChain.h"
#include "TCanvas.h"


//______________________________________________________________________________
// LayerEvent 是指: 对于每一个 entry, 确定各套硅条探测器各层的粒子多重性 LayerMulti，
// 并以此为变量, 记录每一层的事件信息
class CSHINELayerEvent
{
public:
  Int_t        fL1SMulti;
  Int_t       *fL1SNumStrip; // [fL1SMulti]
  Double_t    *fL1SEMeV;     // [fL1SMulti]

  Int_t        fL2FMulti;
  Int_t       *fL2FNumStrip; // [fL2FMulti]
  Double_t    *fL2FEMeV;     // [fL2FMulti]

  Int_t        fL2BMulti;
  Int_t       *fL2BNumStrip; // [fL2BMulti]
  Double_t    *fL2BEMeV;     // [fL2BMulti]

  Int_t        fCsIMulti;
  Int_t       *fCsINum;      // [fCsIMulti]
  Int_t       *fCsIECh;      // [fCsIMulti]

  CSHINELayerEvent();
  ~CSHINELayerEvent();
  ClassDef(CSHINELayerEvent, 1);
};
//______________________________________________________________________________


//______________________________________________________________________________
// SSDEvent 是指: 对于每一个 entry, 确定每套硅条探测器整体的粒子多重性 SSDMulti,
// 并以此为变量, 记录每一套硅条探测器的事件信息
class CSHINESSDEvent
{
public:
  Int_t        fSSDMulti;
  Int_t       *fL1SNumStrip;   //[fSSDMulti]
  Double_t    *fL1SEMeV;       //[fSSDMulti]
  Int_t       *fL2FNumStrip;   //[fSSDMulti]
  Double_t    *fL2FEMeV;       //[fSSDMulti]
  Int_t       *fL2BNumStrip;   //[fSSDMulti]
  Double_t    *fL2BEMeV;       //[fSSDMulti]
  Int_t       *fCsINum;        //[fSSDMulti]
  Int_t       *fCsIECh;        //[fSSDMulti]

  Double_t    *fTheta;         //[fSSDMulti]
  Double_t    *fPhi;           //[fSSDMulti]

  CSHINESSDEvent();
  ~CSHINESSDEvent();
  ClassDef(CSHINESSDEvent, 1);
};
//______________________________________________________________________________


//______________________________________________________________________________
// 将四套硅条探测器合并起来讨论. 以总的粒子多重性为自变量记录事件信息.
class CSHINEGlobalEvent
{
public:
  Int_t        fGlobalMulti;

  Int_t       *fGNumOfSSD;           //[fGlobalMulti]
  Int_t       *fGL1SNumStrip;        //[fGlobalMulti]
  Double_t    *fGL1SEMeV;            //[fGlobalMulti]
  Int_t       *fGL2FNumStrip;        //[fGlobalMulti]
  Double_t    *fGL2FEMeV;            //[fGlobalMulti]
  Int_t       *fGL2BNumStrip;        //[fGlobalMulti]
  Double_t    *fGL2BEMeV;            //[fGlobalMulti]
  Int_t       *fGCsINum;             //[fGlobalMulti]
  Double_t    *fGCsIEMeV;            //[fGlobalMulti]

  Int_t       *fGL2FTime;            //[fGlobalMulti]

  Double_t    *fTheta;               //[fGlobalMulti]
  Double_t    *fPhi;                 //[fGlobalMulti]
  Double_t    *fTotKinEnergy;        //[fGlobalMulti]
  Double_t    *fCalKinEnergy;        //[fGlobalMulti]
  Double_t    *fMomentum;            //[fGlobalMulti]
  Double_t    *fBeta;                //[fGlobalMulti]
  Int_t       *fZ;                   //[fGlobalMulti]
  Int_t       *fA;                   //[fGlobalMulti]

  CSHINEGlobalEvent();
  CSHINEGlobalEvent(Int_t);
  ~CSHINEGlobalEvent();

  ClassDef(CSHINEGlobalEvent, 1);
};
//______________________________________________________________________________


//______________________________________________________________________________
class CSHINEBuildEvent
{
public:
  CSHINEBuildEvent();
  ~CSHINEBuildEvent();

  // 参数说明: CSHINESSDEvent class, ssdindex, ech_L1S[16], ech_L2F[16], ech_L2B[16], ech_L3A[9]
  void BuildLayerEvent (CSHINELayerEvent&,  Int_t,  Int_t*, Int_t*, Int_t*, Int_t*);
  void BuildSSDEvent   (CSHINESSDEvent&,    Int_t,  Int_t*, Int_t*, Int_t*, Int_t*);
  void BuildGlobalEvent(CSHINEGlobalEvent&, Int_t*, Int_t*, Int_t*, Int_t*);

  void BuildLayerEventTree (Int_t fisrtrun, Int_t lastlun);
  void BuildSSDEventTree   (Int_t fisrtrun, Int_t lastlun);
  void BuildGlobalEventTree(Int_t fisrtrun, Int_t lastlun);

private:
  Double_t     *fSiEChcutl1s;   // [NUM_SSD*NUM_STRIP]
  Double_t     *fSiEChcutl2f;   // [NUM_SSD*NUM_STRIP]
  Double_t     *fSiEChcutl2b;   // [NUM_SSD*NUM_STRIP]
  Double_t     *fCsIEChcutl3a;  // [NUM_SSD*NUM_CSI]

  Double_t     *fSlopel1s;      // [NUM_SSD*NUM_STRIP]
  Double_t     *fSlopel2f;      // [NUM_SSD*NUM_STRIP]
  Double_t     *fSlopel2b;      // [NUM_SSD*NUM_STRIP]
  Double_t     *fInterceptl1s;  // [NUM_SSD*NUM_STRIP]
  Double_t     *fInterceptl2f;  // [NUM_SSD*NUM_STRIP]
  Double_t     *fInterceptl2b;  // [NUM_SSD*NUM_STRIP]

  CSHINESSDCalibratedData fCSHINESSDCalibratedData;
};
//______________________________________________________________________________

#endif
