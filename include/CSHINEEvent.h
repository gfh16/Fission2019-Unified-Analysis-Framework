#ifndef CSHINEEVENT_H
#define CSHINEEVENT_H

//______________________________________________________________________________
// 该类使用于各个子望远镜分开讨论的情况
class CSHINESSDEvent 
{
public:
  Int_t        fSSDMulti;

  Int_t        fMultiL1S;
  Int_t       *fNumStripL1S; // [fMultiL1S]
  Double_t    *fEMeVL1S;     // [fMultiL1S]

  Int_t        fMultiL2F;
  Int_t       *fNumStripL2F; // [fMultiL2F]
  Double_t    *fEMeVL2F;     // [fMultiL2F]

  Int_t        fMultiL2B;
  Int_t       *fNumStripL2B; // [fMultiL2B]
  Double_t    *fEMeVL2B;     // [fMultiL2B]

  Int_t        fMultiCsI;
  Int_t       *fNumCsI;      // [fMultiCsI]
  Int_t       *fEChCsI;      // [fMultiCsI]


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

  Int_t       *fNumOfSSD;            //[fGlobalMulti]
  Int_t       *fGNumStripL1S;        //[fGlobalMulti]
  Int_t       *fGNumStripL2F;        //[fGlobalMulti]
  Int_t       *fGNumStripL2B;        //[fGlobalMulti]
  Int_t       *fGNumCsI;             //[fGlobalMulti]

  Int_t       *fGTimeL2F;            //[fGlobalMulti]

  Double_t    *fGEMeVL1S;            //[fGlobalMulti]
  Double_t    *fGEMeVL2F;            //[fGlobalMulti]
  Double_t    *fGEMeVL2B;            //[fGlobalMulti]
  Double_t    *fGEMeVCsI;            //[fGlobalMulti]

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
