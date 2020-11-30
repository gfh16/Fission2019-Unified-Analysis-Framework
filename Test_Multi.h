//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Oct 13 17:01:27 2020 by ROOT version 6.18/00
// from TTree CSHINEEvent/CSHINEEvent Tree
// found on file: /home/sea/Fission2019_Data/CSHINEEvent/EventTree_Run0120-Run0130.root
//////////////////////////////////////////////////////////

#ifndef Test_Multi_h
#define Test_Multi_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include "include/CSHINEHitPatternRecognition.h"
#include "include/EnergyLossModule.h"

// Header file for the classes stored in the TTree if any.

class Test_Multi {
private :
  CSHINEHitPatternRecognition  fPattern;
  EnergyLossModule             fLISEModule;

  Double_t                     fL1ResidulE;
  Double_t                     fL2IncidentE;

public :
   TTree                      *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t                       fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.
   static constexpr Int_t kMaxSSD1 = 1;
   static constexpr Int_t kMaxSSD2 = 1;
   static constexpr Int_t kMaxSSD3 = 1;
   static constexpr Int_t kMaxSSD4 = 1;
   static constexpr Int_t kMaxGlobal = 1;

   // Declaration of leaf types
 //CSHINELayerEvent *SSD1_;
   Int_t           SSD1_fL1SMulti;
   Int_t           SSD1_fL1SNumStrip[11];   //[SSD1.fL1SMulti]
   Double_t        SSD1_fL1SEMeV[11];   //[SSD1.fL1SMulti]
   Int_t           SSD1_fL2FMulti;
   Int_t           SSD1_fL2FNumStrip[16];   //[SSD1.fL2FMulti]
   Double_t        SSD1_fL2FEMeV[16];   //[SSD1.fL2FMulti]
   Int_t           SSD1_fL2BMulti;
   Int_t           SSD1_fL2BNumStrip[16];   //[SSD1.fL2BMulti]
   Double_t        SSD1_fL2BEMeV[16];   //[SSD1.fL2BMulti]
   Int_t           SSD1_fCsIMulti;
   Int_t           SSD1_fCsINum[6];   //[SSD1.fCsIMulti]
   Int_t           SSD1_fCsIECh[6];   //[SSD1.fCsIMulti]
 //CSHINELayerEvent *SSD2_;
   Int_t           SSD2_fL1SMulti;
   Int_t           SSD2_fL1SNumStrip[11];   //[SSD2.fL1SMulti]
   Double_t        SSD2_fL1SEMeV[11];   //[SSD2.fL1SMulti]
   Int_t           SSD2_fL2FMulti;
   Int_t           SSD2_fL2FNumStrip[16];   //[SSD2.fL2FMulti]
   Double_t        SSD2_fL2FEMeV[16];   //[SSD2.fL2FMulti]
   Int_t           SSD2_fL2BMulti;
   Int_t           SSD2_fL2BNumStrip[16];   //[SSD2.fL2BMulti]
   Double_t        SSD2_fL2BEMeV[16];   //[SSD2.fL2BMulti]
   Int_t           SSD2_fCsIMulti;
   Int_t           SSD2_fCsINum[7];   //[SSD2.fCsIMulti]
   Int_t           SSD2_fCsIECh[7];   //[SSD2.fCsIMulti]
 //CSHINELayerEvent *SSD3_;
   Int_t           SSD3_fL1SMulti;
   Int_t           SSD3_fL1SNumStrip[9];   //[SSD3.fL1SMulti]
   Double_t        SSD3_fL1SEMeV[9];   //[SSD3.fL1SMulti]
   Int_t           SSD3_fL2FMulti;
   Int_t           SSD3_fL2FNumStrip[16];   //[SSD3.fL2FMulti]
   Double_t        SSD3_fL2FEMeV[16];   //[SSD3.fL2FMulti]
   Int_t           SSD3_fL2BMulti;
   Int_t           SSD3_fL2BNumStrip[16];   //[SSD3.fL2BMulti]
   Double_t        SSD3_fL2BEMeV[16];   //[SSD3.fL2BMulti]
   Int_t           SSD3_fCsIMulti;
   Int_t           SSD3_fCsINum[8];   //[SSD3.fCsIMulti]
   Int_t           SSD3_fCsIECh[8];   //[SSD3.fCsIMulti]
 //CSHINELayerEvent *SSD4_;
   Int_t           SSD4_fL1SMulti;
   Int_t           SSD4_fL1SNumStrip[15];   //[SSD4.fL1SMulti]
   Double_t        SSD4_fL1SEMeV[15];   //[SSD4.fL1SMulti]
   Int_t           SSD4_fL2FMulti;
   Int_t           SSD4_fL2FNumStrip[16];   //[SSD4.fL2FMulti]
   Double_t        SSD4_fL2FEMeV[16];   //[SSD4.fL2FMulti]
   Int_t           SSD4_fL2BMulti;
   Int_t           SSD4_fL2BNumStrip[16];   //[SSD4.fL2BMulti]
   Double_t        SSD4_fL2BEMeV[16];   //[SSD4.fL2BMulti]
   Int_t           SSD4_fCsIMulti;
   Int_t           SSD4_fCsINum[7];   //[SSD4.fCsIMulti]
   Int_t           SSD4_fCsIECh[7];   //[SSD4.fCsIMulti]
 //CSHINEGlobalEvent *Global_;
   Int_t           Global_fGlobalMulti;
   Int_t           Global_fGNumOfSSD[1];   //[Global.fGlobalMulti]
   Int_t           Global_fGL1SNumStrip[1];   //[Global.fGlobalMulti]
   Double_t        Global_fGL1SEMeV[1];   //[Global.fGlobalMulti]
   Int_t           Global_fGL2FNumStrip[1];   //[Global.fGlobalMulti]
   Double_t        Global_fGL2FEMeV[1];   //[Global.fGlobalMulti]
   Int_t           Global_fGL2BNumStrip[1];   //[Global.fGlobalMulti]
   Double_t        Global_fGL2BEMeV[1];   //[Global.fGlobalMulti]
   Int_t           Global_fGCsINum[1];   //[Global.fGlobalMulti]
   Double_t        Global_fGCsIEMeV[1];   //[Global.fGlobalMulti]
   Int_t           Global_fGL2FTime[1];   //[Global.fGlobalMulti]
   Double_t        Global_fTheta[1];   //[Global.fGlobalMulti]
   Double_t        Global_fPhi[1];   //[Global.fGlobalMulti]
   Double_t        Global_fTotKinEnergy[1];   //[Global.fGlobalMulti]
   Double_t        Global_fCalKinEnergy[1];   //[Global.fGlobalMulti]
   Double_t        Global_fMomentum[1];   //[Global.fGlobalMulti]
   Double_t        Global_fBeta[1];   //[Global.fGlobalMulti]
   Int_t           Global_fZ[1];   //[Global.fGlobalMulti]
   Int_t           Global_fA[1];   //[Global.fGlobalMulti]

   // List of branches
   TBranch        *b_SSD1_fL1SMulti;   //!
   TBranch        *b_SSD1_fL1SNumStrip;   //!
   TBranch        *b_SSD1_fL1SEMeV;   //!
   TBranch        *b_SSD1_fL2FMulti;   //!
   TBranch        *b_SSD1_fL2FNumStrip;   //!
   TBranch        *b_SSD1_fL2FEMeV;   //!
   TBranch        *b_SSD1_fL2BMulti;   //!
   TBranch        *b_SSD1_fL2BNumStrip;   //!
   TBranch        *b_SSD1_fL2BEMeV;   //!
   TBranch        *b_SSD1_fCsIMulti;   //!
   TBranch        *b_SSD1_fCsINum;   //!
   TBranch        *b_SSD1_fCsIECh;   //!
   TBranch        *b_SSD2_fL1SMulti;   //!
   TBranch        *b_SSD2_fL1SNumStrip;   //!
   TBranch        *b_SSD2_fL1SEMeV;   //!
   TBranch        *b_SSD2_fL2FMulti;   //!
   TBranch        *b_SSD2_fL2FNumStrip;   //!
   TBranch        *b_SSD2_fL2FEMeV;   //!
   TBranch        *b_SSD2_fL2BMulti;   //!
   TBranch        *b_SSD2_fL2BNumStrip;   //!
   TBranch        *b_SSD2_fL2BEMeV;   //!
   TBranch        *b_SSD2_fCsIMulti;   //!
   TBranch        *b_SSD2_fCsINum;   //!
   TBranch        *b_SSD2_fCsIECh;   //!
   TBranch        *b_SSD3_fL1SMulti;   //!
   TBranch        *b_SSD3_fL1SNumStrip;   //!
   TBranch        *b_SSD3_fL1SEMeV;   //!
   TBranch        *b_SSD3_fL2FMulti;   //!
   TBranch        *b_SSD3_fL2FNumStrip;   //!
   TBranch        *b_SSD3_fL2FEMeV;   //!
   TBranch        *b_SSD3_fL2BMulti;   //!
   TBranch        *b_SSD3_fL2BNumStrip;   //!
   TBranch        *b_SSD3_fL2BEMeV;   //!
   TBranch        *b_SSD3_fCsIMulti;   //!
   TBranch        *b_SSD3_fCsINum;   //!
   TBranch        *b_SSD3_fCsIECh;   //!
   TBranch        *b_SSD4_fL1SMulti;   //!
   TBranch        *b_SSD4_fL1SNumStrip;   //!
   TBranch        *b_SSD4_fL1SEMeV;   //!
   TBranch        *b_SSD4_fL2FMulti;   //!
   TBranch        *b_SSD4_fL2FNumStrip;   //!
   TBranch        *b_SSD4_fL2FEMeV;   //!
   TBranch        *b_SSD4_fL2BMulti;   //!
   TBranch        *b_SSD4_fL2BNumStrip;   //!
   TBranch        *b_SSD4_fL2BEMeV;   //!
   TBranch        *b_SSD4_fCsIMulti;   //!
   TBranch        *b_SSD4_fCsINum;   //!
   TBranch        *b_SSD4_fCsIECh;   //!
   TBranch        *b_Global_fGlobalMulti;   //!
   TBranch        *b_Global_fGNumOfSSD;   //!
   TBranch        *b_Global_fGL1SNumStrip;   //!
   TBranch        *b_Global_fGL1SEMeV;   //!
   TBranch        *b_Global_fGL2FNumStrip;   //!
   TBranch        *b_Global_fGL2FEMeV;   //!
   TBranch        *b_Global_fGL2BNumStrip;   //!
   TBranch        *b_Global_fGL2BEMeV;   //!
   TBranch        *b_Global_fGCsINum;   //!
   TBranch        *b_Global_fGCsIEMeV;   //!
   TBranch        *b_Global_fGL2FTime;   //!
   TBranch        *b_Global_fTheta;   //!
   TBranch        *b_Global_fPhi;   //!
   TBranch        *b_Global_fTotKinEnergy;   //!
   TBranch        *b_Global_fCalKinEnergy;   //!
   TBranch        *b_Global_fMomentum;   //!
   TBranch        *b_Global_fBeta;   //!
   TBranch        *b_Global_fZ;   //!
   TBranch        *b_Global_fA;   //!

   Test_Multi(TTree *tree=0);
   virtual ~Test_Multi();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   //user-defined function
   virtual void     TrackFindingAlgorithm1();
   virtual void     TrackFindingAlgorithm2();

   virtual void     CheckCutEffOnMulti();
   virtual void     CheckL2BL2FEnergyCorrelation();
   virtual void     DetermineL2BL2FEnergyErrRatio();
   virtual void     CheckEnergyLossL1L2();
   virtual void     CheckEnergyLossL1L2_Relationship();
   virtual void     CheckLayerMultiPercentage();
   virtual void     CheckGlobalMultiRatio();

   virtual void     CheckParallelDraw(const char*);
};


//______________________________________________________________________________
// 定义 class Test_Multi 的子类 DecodefGlobalMulti1
class DecodefGlobalMulti1 : public Test_Multi
{
public:
  DecodefGlobalMulti1();
  ~DecodefGlobalMulti1();

  void    Check_fGlobalMulti1();
  void    ParaDraw_fGlobalMulti1();
};


//______________________________________________________________________________
// 定义 class Test_Multi 的子类 DecodefGlobalMulti2
class DecodefGlobalMulti2 : public Test_Multi
{
public:
  DecodefGlobalMulti2();
  ~DecodefGlobalMulti2();

  void    Check_fGlobalMulti2();
  void    ParaDraw_fGlobalMulti2();
};

#endif
