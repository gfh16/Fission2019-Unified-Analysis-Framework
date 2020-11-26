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

   virtual void     DecodefGlobalMulti1();
   virtual void     DecodefGlobalMulti2();
   virtual void     DecodefGlobalMulti3();

};

#endif

#ifdef Test_Multi_cxx
Test_Multi::Test_Multi(TTree *tree) : fChain(0)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/home/sea/Fission2019_Data/CSHINEEvent/EventTree_Run0120-Run0130.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/home/sea/Fission2019_Data/CSHINEEvent/EventTree_Run0120-Run0130.root");
      }
      f->GetObject("CSHINEEvent",tree);

   }
   Init(tree);
}

Test_Multi::~Test_Multi()
{
   if (!fChain) return;
  // delete fChain->GetCurrentFile();
}

Int_t Test_Multi::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Test_Multi::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Test_Multi::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("SSD1.fL1SMulti", &SSD1_fL1SMulti, &b_SSD1_fL1SMulti);
   fChain->SetBranchAddress("SSD1.fL1SNumStrip", SSD1_fL1SNumStrip, &b_SSD1_fL1SNumStrip);
   fChain->SetBranchAddress("SSD1.fL1SEMeV", SSD1_fL1SEMeV, &b_SSD1_fL1SEMeV);
   fChain->SetBranchAddress("SSD1.fL2FMulti", &SSD1_fL2FMulti, &b_SSD1_fL2FMulti);
   fChain->SetBranchAddress("SSD1.fL2FNumStrip", SSD1_fL2FNumStrip, &b_SSD1_fL2FNumStrip);
   fChain->SetBranchAddress("SSD1.fL2FEMeV", SSD1_fL2FEMeV, &b_SSD1_fL2FEMeV);
   fChain->SetBranchAddress("SSD1.fL2BMulti", &SSD1_fL2BMulti, &b_SSD1_fL2BMulti);
   fChain->SetBranchAddress("SSD1.fL2BNumStrip", SSD1_fL2BNumStrip, &b_SSD1_fL2BNumStrip);
   fChain->SetBranchAddress("SSD1.fL2BEMeV", SSD1_fL2BEMeV, &b_SSD1_fL2BEMeV);
   fChain->SetBranchAddress("SSD1.fCsIMulti", &SSD1_fCsIMulti, &b_SSD1_fCsIMulti);
   fChain->SetBranchAddress("SSD1.fCsINum", SSD1_fCsINum, &b_SSD1_fCsINum);
   fChain->SetBranchAddress("SSD1.fCsIECh", SSD1_fCsIECh, &b_SSD1_fCsIECh);
   fChain->SetBranchAddress("SSD2.fL1SMulti", &SSD2_fL1SMulti, &b_SSD2_fL1SMulti);
   fChain->SetBranchAddress("SSD2.fL1SNumStrip", SSD2_fL1SNumStrip, &b_SSD2_fL1SNumStrip);
   fChain->SetBranchAddress("SSD2.fL1SEMeV", SSD2_fL1SEMeV, &b_SSD2_fL1SEMeV);
   fChain->SetBranchAddress("SSD2.fL2FMulti", &SSD2_fL2FMulti, &b_SSD2_fL2FMulti);
   fChain->SetBranchAddress("SSD2.fL2FNumStrip", SSD2_fL2FNumStrip, &b_SSD2_fL2FNumStrip);
   fChain->SetBranchAddress("SSD2.fL2FEMeV", SSD2_fL2FEMeV, &b_SSD2_fL2FEMeV);
   fChain->SetBranchAddress("SSD2.fL2BMulti", &SSD2_fL2BMulti, &b_SSD2_fL2BMulti);
   fChain->SetBranchAddress("SSD2.fL2BNumStrip", SSD2_fL2BNumStrip, &b_SSD2_fL2BNumStrip);
   fChain->SetBranchAddress("SSD2.fL2BEMeV", SSD2_fL2BEMeV, &b_SSD2_fL2BEMeV);
   fChain->SetBranchAddress("SSD2.fCsIMulti", &SSD2_fCsIMulti, &b_SSD2_fCsIMulti);
   fChain->SetBranchAddress("SSD2.fCsINum", SSD2_fCsINum, &b_SSD2_fCsINum);
   fChain->SetBranchAddress("SSD2.fCsIECh", SSD2_fCsIECh, &b_SSD2_fCsIECh);
   fChain->SetBranchAddress("SSD3.fL1SMulti", &SSD3_fL1SMulti, &b_SSD3_fL1SMulti);
   fChain->SetBranchAddress("SSD3.fL1SNumStrip", SSD3_fL1SNumStrip, &b_SSD3_fL1SNumStrip);
   fChain->SetBranchAddress("SSD3.fL1SEMeV", SSD3_fL1SEMeV, &b_SSD3_fL1SEMeV);
   fChain->SetBranchAddress("SSD3.fL2FMulti", &SSD3_fL2FMulti, &b_SSD3_fL2FMulti);
   fChain->SetBranchAddress("SSD3.fL2FNumStrip", SSD3_fL2FNumStrip, &b_SSD3_fL2FNumStrip);
   fChain->SetBranchAddress("SSD3.fL2FEMeV", SSD3_fL2FEMeV, &b_SSD3_fL2FEMeV);
   fChain->SetBranchAddress("SSD3.fL2BMulti", &SSD3_fL2BMulti, &b_SSD3_fL2BMulti);
   fChain->SetBranchAddress("SSD3.fL2BNumStrip", SSD3_fL2BNumStrip, &b_SSD3_fL2BNumStrip);
   fChain->SetBranchAddress("SSD3.fL2BEMeV", SSD3_fL2BEMeV, &b_SSD3_fL2BEMeV);
   fChain->SetBranchAddress("SSD3.fCsIMulti", &SSD3_fCsIMulti, &b_SSD3_fCsIMulti);
   fChain->SetBranchAddress("SSD3.fCsINum", SSD3_fCsINum, &b_SSD3_fCsINum);
   fChain->SetBranchAddress("SSD3.fCsIECh", SSD3_fCsIECh, &b_SSD3_fCsIECh);
   fChain->SetBranchAddress("SSD4.fL1SMulti", &SSD4_fL1SMulti, &b_SSD4_fL1SMulti);
   fChain->SetBranchAddress("SSD4.fL1SNumStrip", SSD4_fL1SNumStrip, &b_SSD4_fL1SNumStrip);
   fChain->SetBranchAddress("SSD4.fL1SEMeV", SSD4_fL1SEMeV, &b_SSD4_fL1SEMeV);
   fChain->SetBranchAddress("SSD4.fL2FMulti", &SSD4_fL2FMulti, &b_SSD4_fL2FMulti);
   fChain->SetBranchAddress("SSD4.fL2FNumStrip", SSD4_fL2FNumStrip, &b_SSD4_fL2FNumStrip);
   fChain->SetBranchAddress("SSD4.fL2FEMeV", SSD4_fL2FEMeV, &b_SSD4_fL2FEMeV);
   fChain->SetBranchAddress("SSD4.fL2BMulti", &SSD4_fL2BMulti, &b_SSD4_fL2BMulti);
   fChain->SetBranchAddress("SSD4.fL2BNumStrip", SSD4_fL2BNumStrip, &b_SSD4_fL2BNumStrip);
   fChain->SetBranchAddress("SSD4.fL2BEMeV", SSD4_fL2BEMeV, &b_SSD4_fL2BEMeV);
   fChain->SetBranchAddress("SSD4.fCsIMulti", &SSD4_fCsIMulti, &b_SSD4_fCsIMulti);
   fChain->SetBranchAddress("SSD4.fCsINum", SSD4_fCsINum, &b_SSD4_fCsINum);
   fChain->SetBranchAddress("SSD4.fCsIECh", SSD4_fCsIECh, &b_SSD4_fCsIECh);
   fChain->SetBranchAddress("Global.fGlobalMulti", &Global_fGlobalMulti, &b_Global_fGlobalMulti);
   fChain->SetBranchAddress("Global.fGNumOfSSD", &Global_fGNumOfSSD, &b_Global_fGNumOfSSD);
   fChain->SetBranchAddress("Global.fGL1SNumStrip", &Global_fGL1SNumStrip, &b_Global_fGL1SNumStrip);
   fChain->SetBranchAddress("Global.fGL1SEMeV", &Global_fGL1SEMeV, &b_Global_fGL1SEMeV);
   fChain->SetBranchAddress("Global.fGL2FNumStrip", &Global_fGL2FNumStrip, &b_Global_fGL2FNumStrip);
   fChain->SetBranchAddress("Global.fGL2FEMeV", &Global_fGL2FEMeV, &b_Global_fGL2FEMeV);
   fChain->SetBranchAddress("Global.fGL2BNumStrip", &Global_fGL2BNumStrip, &b_Global_fGL2BNumStrip);
   fChain->SetBranchAddress("Global.fGL2BEMeV", &Global_fGL2BEMeV, &b_Global_fGL2BEMeV);
   fChain->SetBranchAddress("Global.fGCsINum", &Global_fGCsINum, &b_Global_fGCsINum);
   fChain->SetBranchAddress("Global.fGCsIEMeV", &Global_fGCsIEMeV, &b_Global_fGCsIEMeV);
   fChain->SetBranchAddress("Global.fGL2FTime", &Global_fGL2FTime, &b_Global_fGL2FTime);
   fChain->SetBranchAddress("Global.fTheta", &Global_fTheta, &b_Global_fTheta);
   fChain->SetBranchAddress("Global.fPhi", &Global_fPhi, &b_Global_fPhi);
   fChain->SetBranchAddress("Global.fTotKinEnergy", &Global_fTotKinEnergy, &b_Global_fTotKinEnergy);
   fChain->SetBranchAddress("Global.fCalKinEnergy", &Global_fCalKinEnergy, &b_Global_fCalKinEnergy);
   fChain->SetBranchAddress("Global.fMomentum", &Global_fMomentum, &b_Global_fMomentum);
   fChain->SetBranchAddress("Global.fBeta", &Global_fBeta, &b_Global_fBeta);
   fChain->SetBranchAddress("Global.fZ", &Global_fZ, &b_Global_fZ);
   fChain->SetBranchAddress("Global.fA", &Global_fA, &b_Global_fA);
   Notify();
}

Bool_t Test_Multi::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Test_Multi::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Test_Multi::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Test_Multi_cxx
