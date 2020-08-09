#ifndef CSHINEEVENTTREEREADER_H
#define CSHINEEVENTTREEREADER_H

#include <TTree.h>


//________________________________
class CSHINEEventTreeReader
{
public:
  TTree           *fChain;  //!pointer to the analyzed TTree or TChain

// Declaration of leaf types
  //CSHINESSDEvent  *SSD1_;
  Int_t           SSD1_fSSDMulti;
  Int_t           SSD1_fMultiL1S;
  Int_t           SSD1_fNumStripL1S[16];   //[SSD1.fMultiL1S]
  Double_t        SSD1_fEMeVL1S[16];       //[SSD1.fMultiL1S]
  Int_t           SSD1_fMultiL2F;
  Int_t           SSD1_fNumStripL2F[16];   //[SSD1.fMultiL2F]
  Double_t        SSD1_fEMeVL2F[16];       //[SSD1.fMultiL2F]
  Int_t           SSD1_fMultiL2B;
  Int_t           SSD1_fNumStripL2B[16];   //[SSD1.fMultiL2B]
  Double_t        SSD1_fEMeVL2B[16];       //[SSD1.fMultiL2B]
  Int_t           SSD1_fMultiCsI;
  Int_t           SSD1_fNumCsI[9];         //[SSD1.fMultiCsI]
  Int_t           SSD1_fEChCsI[9];         //[SSD1.fMultiCsI]
  //CSHINESSDEvent  *SSD2_;
  Int_t           SSD2_fSSDMulti;
  Int_t           SSD2_fMultiL1S;
  Int_t           SSD2_fNumStripL1S[16];   //[SSD2.fMultiL1S]
  Double_t        SSD2_fEMeVL1S[16];       //[SSD2.fMultiL1S]
  Int_t           SSD2_fMultiL2F;
  Int_t           SSD2_fNumStripL2F[16];   //[SSD2.fMultiL2F]
  Double_t        SSD2_fEMeVL2F[16];       //[SSD2.fMultiL2F]
  Int_t           SSD2_fMultiL2B;
  Int_t           SSD2_fNumStripL2B[16];   //[SSD2.fMultiL2B]
  Double_t        SSD2_fEMeVL2B[16];       //[SSD2.fMultiL2B]
  Int_t           SSD2_fMultiCsI;
  Int_t           SSD2_fNumCsI[9];         //[SSD2.fMultiCsI]
  Int_t           SSD2_fEChCsI[9];         //[SSD2.fMultiCsI]
  //CSHINESSDEvent  *SSD3_;
  Int_t           SSD3_fSSDMulti;
  Int_t           SSD3_fMultiL1S;
  Int_t           SSD3_fNumStripL1S[16];   //[SSD3.fMultiL1S]
  Double_t        SSD3_fEMeVL1S[16];       //[SSD3.fMultiL1S]
  Int_t           SSD3_fMultiL2F;
  Int_t           SSD3_fNumStripL2F[16];   //[SSD3.fMultiL2F]
  Double_t        SSD3_fEMeVL2F[16];       //[SSD3.fMultiL2F]
  Int_t           SSD3_fMultiL2B;
  Int_t           SSD3_fNumStripL2B[16];   //[SSD3.fMultiL2B]
  Double_t        SSD3_fEMeVL2B[16];       //[SSD3.fMultiL2B]
  Int_t           SSD3_fMultiCsI;
  Int_t           SSD3_fNumCsI[9];         //[SSD3.fMultiCsI]
  Int_t           SSD3_fEChCsI[9];         //[SSD3.fMultiCsI]
  //CSHINESSDEvent  *SSD4_;
  Int_t           SSD4_fSSDMulti;
  Int_t           SSD4_fMultiL1S;
  Int_t           SSD4_fNumStripL1S[15];   //[SSD4.fMultiL1S]
  Double_t        SSD4_fEMeVL1S[15];       //[SSD4.fMultiL1S]
  Int_t           SSD4_fMultiL2F;
  Int_t           SSD4_fNumStripL2F[16];   //[SSD4.fMultiL2F]
  Double_t        SSD4_fEMeVL2F[16];       //[SSD4.fMultiL2F]
  Int_t           SSD4_fMultiL2B;
  Int_t           SSD4_fNumStripL2B[16];   //[SSD4.fMultiL2B]
  Double_t        SSD4_fEMeVL2B[16];       //[SSD4.fMultiL2B]
  Int_t           SSD4_fMultiCsI;
  Int_t           SSD4_fNumCsI[9];         //[SSD4.fMultiCsI]
  Int_t           SSD4_fEChCsI[9];         //[SSD4.fMultiCsI]
  //CSHINEGlobalEvent *Global_;
  Int_t           Global_fGlobalMulti;
  Int_t           Global_fNumOfSSD[1];     //[Global.fGlobalMulti]
  Int_t           Global_fGNumStripL1S[1]; //[Global.fGlobalMulti]
  Int_t           Global_fGNumStripL2F[1]; //[Global.fGlobalMulti]
  Int_t           Global_fGNumStripL2B[1]; //[Global.fGlobalMulti]
  Int_t           Global_fGNumCsI[1];      //[Global.fGlobalMulti]
  Int_t           Global_fGTimeL2F[1];     //[Global.fGlobalMulti]
  Double_t        Global_fGEMeVL1S[1];     //[Global.fGlobalMulti]
  Double_t        Global_fGEMeVL2F[1];     //[Global.fGlobalMulti]
  Double_t        Global_fGEMeVL2B[1];     //[Global.fGlobalMulti]
  Double_t        Global_fGEMeVCsI[1];     //[Global.fGlobalMulti]
  Double_t        Global_fTotKinEnergy[1]; //[Global.fGlobalMulti]
  Double_t        Global_fCalKinEnergy[1]; //[Global.fGlobalMulti]
  Double_t        Global_fBeta[1];         //[Global.fGlobalMulti]
  Double_t        Global_fMomentum[1];     //[Global.fGlobalMulti]
  Double_t        Global_fTheta[1];        //[Global.fGlobalMulti]
  Double_t        Global_fPhi[1];          //[Global.fGlobalMulti]
  Int_t           Global_fZ[1];            //[Global.fGlobalMulti]
  Int_t           Global_fA[1];            //[Global.fGlobalMulti]

  // List of branches
  TBranch        *b_SSD1_fSSDMulti;        //!
  TBranch        *b_SSD1_fMultiL1S;        //!
  TBranch        *b_SSD1_fNumStripL1S;     //!
  TBranch        *b_SSD1_fEMeVL1S;         //!
  TBranch        *b_SSD1_fMultiL2F;        //!
  TBranch        *b_SSD1_fNumStripL2F;     //!
  TBranch        *b_SSD1_fEMeVL2F;         //!
  TBranch        *b_SSD1_fMultiL2B;        //!
  TBranch        *b_SSD1_fNumStripL2B;     //!
  TBranch        *b_SSD1_fEMeVL2B;         //!
  TBranch        *b_SSD1_fMultiCsI;        //!
  TBranch        *b_SSD1_fNumCsI;          //!
  TBranch        *b_SSD1_fEChCsI;          //!
  TBranch        *b_SSD2_fSSDMulti;        //!
  TBranch        *b_SSD2_fMultiL1S;        //!
  TBranch        *b_SSD2_fNumStripL1S;     //!
  TBranch        *b_SSD2_fEMeVL1S;         //!
  TBranch        *b_SSD2_fMultiL2F;        //!
  TBranch        *b_SSD2_fNumStripL2F;     //!
  TBranch        *b_SSD2_fEMeVL2F;         //!
  TBranch        *b_SSD2_fMultiL2B;        //!
  TBranch        *b_SSD2_fNumStripL2B;     //!
  TBranch        *b_SSD2_fEMeVL2B;         //!
  TBranch        *b_SSD2_fMultiCsI;        //!
  TBranch        *b_SSD2_fNumCsI;          //!
  TBranch        *b_SSD2_fEChCsI;          //!
  TBranch        *b_SSD3_fSSDMulti;        //!
  TBranch        *b_SSD3_fMultiL1S;        //!
  TBranch        *b_SSD3_fNumStripL1S;     //!
  TBranch        *b_SSD3_fEMeVL1S;         //!
  TBranch        *b_SSD3_fMultiL2F;        //!
  TBranch        *b_SSD3_fNumStripL2F;     //!
  TBranch        *b_SSD3_fEMeVL2F;         //!
  TBranch        *b_SSD3_fMultiL2B;        //!
  TBranch        *b_SSD3_fNumStripL2B;     //!
  TBranch        *b_SSD3_fEMeVL2B;         //!
  TBranch        *b_SSD3_fMultiCsI;        //!
  TBranch        *b_SSD3_fNumCsI;          //!
  TBranch        *b_SSD3_fEChCsI;          //!
  TBranch        *b_SSD4_fSSDMulti;        //!
  TBranch        *b_SSD4_fMultiL1S;        //!
  TBranch        *b_SSD4_fNumStripL1S;     //!
  TBranch        *b_SSD4_fEMeVL1S;         //!
  TBranch        *b_SSD4_fMultiL2F;        //!
  TBranch        *b_SSD4_fNumStripL2F;     //!
  TBranch        *b_SSD4_fEMeVL2F;         //!
  TBranch        *b_SSD4_fMultiL2B;        //!
  TBranch        *b_SSD4_fNumStripL2B;     //!
  TBranch        *b_SSD4_fEMeVL2B;         //!
  TBranch        *b_SSD4_fMultiCsI;        //!
  TBranch        *b_SSD4_fNumCsI;          //!
  TBranch        *b_SSD4_fEChCsI;          //!
  TBranch        *b_Global_fGlobalMulti;   //!
  TBranch        *b_Global_fNumOfSSD;      //!
  TBranch        *b_Global_fGNumStripL1S;  //!
  TBranch        *b_Global_fGNumStripL2F;  //!
  TBranch        *b_Global_fGNumStripL2B;  //!
  TBranch        *b_Global_fGNumCsI;       //!
  TBranch        *b_Global_fGTimeL2F;      //!
  TBranch        *b_Global_fGEMeVL1S;      //!
  TBranch        *b_Global_fGEMeVL2F;      //!
  TBranch        *b_Global_fGEMeVL2B;      //!
  TBranch        *b_Global_fGEMeVCsI;      //!
  TBranch        *b_Global_fTotKinEnergy;  //!
  TBranch        *b_Global_fCalKinEnergy;  //!
  TBranch        *b_Global_fBeta;          //!
  TBranch        *b_Global_fMomentum;      //!
  TBranch        *b_Global_fTheta;         //!
  TBranch        *b_Global_fPhi;           //!
  TBranch        *b_Global_fZ;             //!
  TBranch        *b_Global_fA;             //!


  CSHINEEventTreeReader(TTree* tree=0);
  virtual ~CSHINEEventTreeReader();

  virtual void Init(TTree* tree);

  // 在这里定义需要进行的操作
  virtual void DEE_All      (const char* pathOutput);
  virtual void DEE_L2F_L3   (const char* pathOutput);
  virtual void DEE_L2B_L3   (const char* pathOutput);
  virtual void DEE_L1_L2F   (const char* pathOutput);
  virtual void DEE_L1_L2B   (const char* pathOutput);

  // 在这里进行一些检查
//  virtual void EMeVL2F_EMeVL2B_Correlation();
//  virtual void Check_L1_L2F_Matching();
//  virtual void Check_L1_L2B_Matching();
//  virtual void Check_L2F_CsI_Matching();
//  virtual void Check_L2B_CsI_Matching();
};

#endif


//=============================
#ifdef CSHINEEventTreeReader_cxx
//______________________________________________________________________________
CSHINEEventTreeReader::CSHINEEventTreeReader(TTree* tree) : fChain(0)
{
  Init(tree);
}

//______________________________________________________________________________
CSHINEEventTreeReader::~CSHINEEventTreeReader()
{
  if (!fChain) return;
//  delete fChain->GetCurrentFile();
}

//______________________________________________________________________________
void CSHINEEventTreeReader::Init(TTree* tree)
{
  if (!tree) return;
  fChain = tree;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("SSD1.fSSDMulti",       &SSD1_fSSDMulti,       &b_SSD1_fSSDMulti);
  fChain->SetBranchAddress("SSD1.fMultiL1S",       &SSD1_fMultiL1S,       &b_SSD1_fMultiL1S);
  fChain->SetBranchAddress("SSD1.fNumStripL1S",     SSD1_fNumStripL1S,    &b_SSD1_fNumStripL1S);
  fChain->SetBranchAddress("SSD1.fEMeVL1S",         SSD1_fEMeVL1S,        &b_SSD1_fEMeVL1S);
  fChain->SetBranchAddress("SSD1.fMultiL2F",       &SSD1_fMultiL2F,       &b_SSD1_fMultiL2F);
  fChain->SetBranchAddress("SSD1.fNumStripL2F",     SSD1_fNumStripL2F,    &b_SSD1_fNumStripL2F);
  fChain->SetBranchAddress("SSD1.fEMeVL2F",         SSD1_fEMeVL2F,        &b_SSD1_fEMeVL2F);
  fChain->SetBranchAddress("SSD1.fMultiL2B",       &SSD1_fMultiL2B,       &b_SSD1_fMultiL2B);
  fChain->SetBranchAddress("SSD1.fNumStripL2B",     SSD1_fNumStripL2B,    &b_SSD1_fNumStripL2B);
  fChain->SetBranchAddress("SSD1.fEMeVL2B",         SSD1_fEMeVL2B,        &b_SSD1_fEMeVL2B);
  fChain->SetBranchAddress("SSD1.fMultiCsI",       &SSD1_fMultiCsI,       &b_SSD1_fMultiCsI);
  fChain->SetBranchAddress("SSD1.fNumCsI",          SSD1_fNumCsI,         &b_SSD1_fNumCsI);
  fChain->SetBranchAddress("SSD1.fEChCsI",          SSD1_fEChCsI,         &b_SSD1_fEChCsI);
  fChain->SetBranchAddress("SSD2.fSSDMulti",       &SSD2_fSSDMulti,       &b_SSD2_fSSDMulti);
  fChain->SetBranchAddress("SSD2.fMultiL1S",       &SSD2_fMultiL1S,       &b_SSD2_fMultiL1S);
  fChain->SetBranchAddress("SSD2.fNumStripL1S",     SSD2_fNumStripL1S,    &b_SSD2_fNumStripL1S);
  fChain->SetBranchAddress("SSD2.fEMeVL1S",         SSD2_fEMeVL1S,        &b_SSD2_fEMeVL1S);
  fChain->SetBranchAddress("SSD2.fMultiL2F",       &SSD2_fMultiL2F,       &b_SSD2_fMultiL2F);
  fChain->SetBranchAddress("SSD2.fNumStripL2F",     SSD2_fNumStripL2F,    &b_SSD2_fNumStripL2F);
  fChain->SetBranchAddress("SSD2.fEMeVL2F",         SSD2_fEMeVL2F,        &b_SSD2_fEMeVL2F);
  fChain->SetBranchAddress("SSD2.fMultiL2B",       &SSD2_fMultiL2B,       &b_SSD2_fMultiL2B);
  fChain->SetBranchAddress("SSD2.fNumStripL2B",     SSD2_fNumStripL2B,    &b_SSD2_fNumStripL2B);
  fChain->SetBranchAddress("SSD2.fEMeVL2B",         SSD2_fEMeVL2B,        &b_SSD2_fEMeVL2B);
  fChain->SetBranchAddress("SSD2.fMultiCsI",       &SSD2_fMultiCsI,       &b_SSD2_fMultiCsI);
  fChain->SetBranchAddress("SSD2.fNumCsI",          SSD2_fNumCsI,         &b_SSD2_fNumCsI);
  fChain->SetBranchAddress("SSD2.fEChCsI",          SSD2_fEChCsI,         &b_SSD2_fEChCsI);
  fChain->SetBranchAddress("SSD3.fSSDMulti",       &SSD3_fSSDMulti,       &b_SSD3_fSSDMulti);
  fChain->SetBranchAddress("SSD3.fMultiL1S",       &SSD3_fMultiL1S,       &b_SSD3_fMultiL1S);
  fChain->SetBranchAddress("SSD3.fNumStripL1S",     SSD3_fNumStripL1S,    &b_SSD3_fNumStripL1S);
  fChain->SetBranchAddress("SSD3.fEMeVL1S",         SSD3_fEMeVL1S,        &b_SSD3_fEMeVL1S);
  fChain->SetBranchAddress("SSD3.fMultiL2F",       &SSD3_fMultiL2F,       &b_SSD3_fMultiL2F);
  fChain->SetBranchAddress("SSD3.fNumStripL2F",     SSD3_fNumStripL2F,    &b_SSD3_fNumStripL2F);
  fChain->SetBranchAddress("SSD3.fEMeVL2F",         SSD3_fEMeVL2F,        &b_SSD3_fEMeVL2F);
  fChain->SetBranchAddress("SSD3.fMultiL2B",       &SSD3_fMultiL2B,       &b_SSD3_fMultiL2B);
  fChain->SetBranchAddress("SSD3.fNumStripL2B",     SSD3_fNumStripL2B,    &b_SSD3_fNumStripL2B);
  fChain->SetBranchAddress("SSD3.fEMeVL2B",         SSD3_fEMeVL2B,        &b_SSD3_fEMeVL2B);
  fChain->SetBranchAddress("SSD3.fMultiCsI",       &SSD3_fMultiCsI,       &b_SSD3_fMultiCsI);
  fChain->SetBranchAddress("SSD3.fNumCsI",          SSD3_fNumCsI,         &b_SSD3_fNumCsI);
  fChain->SetBranchAddress("SSD3.fEChCsI",          SSD3_fEChCsI,         &b_SSD3_fEChCsI);
  fChain->SetBranchAddress("SSD4.fSSDMulti",       &SSD4_fSSDMulti,       &b_SSD4_fSSDMulti);
  fChain->SetBranchAddress("SSD4.fMultiL1S",       &SSD4_fMultiL1S,       &b_SSD4_fMultiL1S);
  fChain->SetBranchAddress("SSD4.fNumStripL1S",     SSD4_fNumStripL1S,    &b_SSD4_fNumStripL1S);
  fChain->SetBranchAddress("SSD4.fEMeVL1S",         SSD4_fEMeVL1S,        &b_SSD4_fEMeVL1S);
  fChain->SetBranchAddress("SSD4.fMultiL2F",       &SSD4_fMultiL2F,       &b_SSD4_fMultiL2F);
  fChain->SetBranchAddress("SSD4.fNumStripL2F",     SSD4_fNumStripL2F,    &b_SSD4_fNumStripL2F);
  fChain->SetBranchAddress("SSD4.fEMeVL2F",         SSD4_fEMeVL2F,        &b_SSD4_fEMeVL2F);
  fChain->SetBranchAddress("SSD4.fMultiL2B",       &SSD4_fMultiL2B,       &b_SSD4_fMultiL2B);
  fChain->SetBranchAddress("SSD4.fNumStripL2B",     SSD4_fNumStripL2B,    &b_SSD4_fNumStripL2B);
  fChain->SetBranchAddress("SSD4.fEMeVL2B",         SSD4_fEMeVL2B,        &b_SSD4_fEMeVL2B);
  fChain->SetBranchAddress("SSD4.fMultiCsI",       &SSD4_fMultiCsI,       &b_SSD4_fMultiCsI);
  fChain->SetBranchAddress("SSD4.fNumCsI",          SSD4_fNumCsI,         &b_SSD4_fNumCsI);
  fChain->SetBranchAddress("SSD4.fEChCsI",          SSD4_fEChCsI,         &b_SSD4_fEChCsI);
  fChain->SetBranchAddress("Global.fGlobalMulti",  &Global_fGlobalMulti,  &b_Global_fGlobalMulti);
  fChain->SetBranchAddress("Global.fNumOfSSD",     &Global_fNumOfSSD,     &b_Global_fNumOfSSD);
  fChain->SetBranchAddress("Global.fGNumStripL1S", &Global_fGNumStripL1S, &b_Global_fGNumStripL1S);
  fChain->SetBranchAddress("Global.fGNumStripL2F", &Global_fGNumStripL2F, &b_Global_fGNumStripL2F);
  fChain->SetBranchAddress("Global.fGNumStripL2B", &Global_fGNumStripL2B, &b_Global_fGNumStripL2B);
  fChain->SetBranchAddress("Global.fGNumCsI",      &Global_fGNumCsI,      &b_Global_fGNumCsI);
  fChain->SetBranchAddress("Global.fGTimeL2F",     &Global_fGTimeL2F,     &b_Global_fGTimeL2F);
  fChain->SetBranchAddress("Global.fGEMeVL1S",     &Global_fGEMeVL1S,     &b_Global_fGEMeVL1S);
  fChain->SetBranchAddress("Global.fGEMeVL2F",     &Global_fGEMeVL2F,     &b_Global_fGEMeVL2F);
  fChain->SetBranchAddress("Global.fGEMeVL2B",     &Global_fGEMeVL2B,     &b_Global_fGEMeVL2B);
  fChain->SetBranchAddress("Global.fGEMeVCsI",     &Global_fGEMeVCsI,     &b_Global_fGEMeVCsI);
  fChain->SetBranchAddress("Global.fTotKinEnergy", &Global_fTotKinEnergy, &b_Global_fTotKinEnergy);
  fChain->SetBranchAddress("Global.fCalKinEnergy", &Global_fCalKinEnergy, &b_Global_fCalKinEnergy);
  fChain->SetBranchAddress("Global.fBeta",         &Global_fBeta,         &b_Global_fBeta);
  fChain->SetBranchAddress("Global.fMomentum",     &Global_fMomentum,     &b_Global_fMomentum);
  fChain->SetBranchAddress("Global.fTheta",        &Global_fTheta,        &b_Global_fTheta);
  fChain->SetBranchAddress("Global.fPhi",          &Global_fPhi,          &b_Global_fPhi);
  fChain->SetBranchAddress("Global.fZ",            &Global_fZ,            &b_Global_fZ);
  fChain->SetBranchAddress("Global.fA",            &Global_fA,            &b_Global_fA);
}

#endif
