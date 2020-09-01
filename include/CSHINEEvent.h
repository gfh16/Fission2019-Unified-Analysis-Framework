#ifndef CSHINEEVENT_H
#define CSHINEEVENT_H

//______________________________________________________________________________
// 该类使用于各个子望远镜分开讨论的情况
class CSHINESSDEvent
{
public:
  Int_t        fSSDMulti;

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


  CSHINESSDEvent();

  virtual ~CSHINESSDEvent();

  ClassDef(CSHINESSDEvent, 1);

};


//______________________________________________________________________________
// 将四套硅条探测器合并起来讨论. 以总的粒子多重性为自变量
class CSHINEGlobalEvent
{
public:
  Int_t        fGlobalMulti;

  Int_t       *fGNumOfSSD;           //[fGlobalMulti]
  Int_t       *fGL1SNumStrip;        //[fGlobalMulti]
  Int_t       *fGL2FNumStrip;        //[fGlobalMulti]
  Int_t       *fGL2BNumStrip;        //[fGlobalMulti]
  Int_t       *fGCsINum;             //[fGlobalMulti]

  Double_t    *fGL1SEMeV;            //[fGlobalMulti]
  Double_t    *fGL2FEMeV;            //[fGlobalMulti]
  Double_t    *fGL2BEMeV;            //[fGlobalMulti]
  Double_t    *fGCsIEMeV;            //[fGlobalMulti]

  Int_t       *fGL2FTime;            //[fGlobalMulti]

  Double_t    *fTotKinEnergy;        //[fGlobalMulti]
  Double_t    *fCalKinEnergy;        //[fGlobalMulti]
  Double_t    *fBeta;                //[fGlobalMulti]
  Double_t    *fMomentum;            //[fGlobalMulti]
  Double_t    *fTheta;               //[fGlobalMulti]
  Double_t    *fPhi;                 //[fGlobalMulti]
  Int_t       *fZ;                   //[fGlobalMulti]
  Int_t       *fA;                   //[fGlobalMulti]

  CSHINEGlobalEvent();
  CSHINEGlobalEvent(Int_t);
  virtual ~CSHINEGlobalEvent();

  ClassDef(CSHINEGlobalEvent, 1);
};

#endif
