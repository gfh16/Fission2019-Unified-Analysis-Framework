#ifndef CSHINEDEELINES_H
#define CSHINEDEELINES_H

#include <TTree.h>


//________________________________
class CSHINEDEELines
{
public:
  TTree           *fChain;  //!pointer to the analyzed TTree or TChain

// Declaration of leaf types
  //CSHINESSDEvent  *SSD1_;
  Int_t           SSD1_fSSDMulti;
  Int_t           SSD1_fL1SMulti;
  Int_t           SSD1_fL1SNumStrip[16];   //[SSD1.fL1SMulti]
  Double_t        SSD1_fL1SEMeV[16];       //[SSD1.fL1SMulti]
  Int_t           SSD1_fL2FMulti;
  Int_t           SSD1_fL2FNumStrip[16];   //[SSD1.fL2FMulti]
  Double_t        SSD1_fL2FEMeV[16];       //[SSD1.fL2FMulti]
  Int_t           SSD1_fL2BMulti;
  Int_t           SSD1_fL2BNumStrip[16];   //[SSD1.fL2BMulti]
  Double_t        SSD1_fL2BEMeV[16];       //[SSD1.fL2BMulti]
  Int_t           SSD1_fCsIMulti;
  Int_t           SSD1_fCsINum[9];         //[SSD1.fCsIMulti]
  Int_t           SSD1_fCsIECh[9];         //[SSD1.fCsIMulti]
  //CSHINESSDEvent  *SSD2_;
  Int_t           SSD2_fSSDMulti;
  Int_t           SSD2_fL1SMulti;
  Int_t           SSD2_fL1SNumStrip[16];   //[SSD2.fL1SMulti]
  Double_t        SSD2_fL1SEMeV[16];       //[SSD2.fL1SMulti]
  Int_t           SSD2_fL2FMulti;
  Int_t           SSD2_fL2FNumStrip[16];   //[SSD2.fL2FMulti]
  Double_t        SSD2_fL2FEMeV[16];       //[SSD2.fL2FMulti]
  Int_t           SSD2_fL2BMulti;
  Int_t           SSD2_fL2BNumStrip[16];   //[SSD2.fL2BMulti]
  Double_t        SSD2_fL2BEMeV[16];       //[SSD2.fL2BMulti]
  Int_t           SSD2_fCsIMulti;
  Int_t           SSD2_fCsINum[9];         //[SSD2.fCsIMulti]
  Int_t           SSD2_fCsIECh[9];         //[SSD2.fCsIMulti]
  //CSHINESSDEvent  *SSD3_;
  Int_t           SSD3_fSSDMulti;
  Int_t           SSD3_fL1SMulti;
  Int_t           SSD3_fL1SNumStrip[16];   //[SSD3.fL1SMulti]
  Double_t        SSD3_fL1SEMeV[16];       //[SSD3.fL1SMulti]
  Int_t           SSD3_fL2FMulti;
  Int_t           SSD3_fL2FNumStrip[16];   //[SSD3.fL2FMulti]
  Double_t        SSD3_fL2FEMeV[16];       //[SSD3.fL2FMulti]
  Int_t           SSD3_fL2BMulti;
  Int_t           SSD3_fL2BNumStrip[16];   //[SSD3.fL2BMulti]
  Double_t        SSD3_fL2BEMeV[16];       //[SSD3.fL2BMulti]
  Int_t           SSD3_fCsIMulti;
  Int_t           SSD3_fCsINum[9];         //[SSD3.fCsIMulti]
  Int_t           SSD3_fCsIECh[9];         //[SSD3.fCsIMulti]
  //CSHINESSDEvent  *SSD4_;
  Int_t           SSD4_fSSDMulti;
  Int_t           SSD4_fL1SMulti;
  Int_t           SSD4_fL1SNumStrip[15];   //[SSD4.fL1SMulti]
  Double_t        SSD4_fL1SEMeV[15];       //[SSD4.fL1SMulti]
  Int_t           SSD4_fL2FMulti;
  Int_t           SSD4_fL2FNumStrip[16];   //[SSD4.fL2FMulti]
  Double_t        SSD4_fL2FEMeV[16];       //[SSD4.fL2FMulti]
  Int_t           SSD4_fL2BMulti;
  Int_t           SSD4_fL2BNumStrip[16];   //[SSD4.fL2BMulti]
  Double_t        SSD4_fL2BEMeV[16];       //[SSD4.fL2BMulti]
  Int_t           SSD4_fCsIMulti;
  Int_t           SSD4_fCsINum[9];         //[SSD4.fCsIMulti]
  Int_t           SSD4_fCsIECh[9];         //[SSD4.fCsIMulti]
  //CSHINEGlobalEvent *Global_;
  Int_t           Global_fGlobalMulti;
  Int_t           Global_fGNumOfSSD[1];    //[Global.fGlobalMulti]
  Int_t           Global_fGL1SNumStrip[1]; //[Global.fGlobalMulti]
  Int_t           Global_fGL2FNumStrip[1]; //[Global.fGlobalMulti]
  Int_t           Global_fL2BGNumStrip[1]; //[Global.fGlobalMulti]
  Int_t           Global_fGCsINum[1];      //[Global.fGlobalMulti]
  Double_t        Global_fGL1SEMeV[1];     //[Global.fGlobalMulti]
  Double_t        Global_fGL2FEMeV[1];     //[Global.fGlobalMulti]
  Double_t        Global_fGL2BEMeV[1];     //[Global.fGlobalMulti]
  Double_t        Global_fGCsIEMeV[1];     //[Global.fGlobalMulti]

  Int_t           Global_fGL2FTime[1];     //[Global.fGlobalMulti]

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
  TBranch        *b_SSD1_fL1SMulti;        //!
  TBranch        *b_SSD1_fL1SNumStrip;     //!
  TBranch        *b_SSD1_fL1SEMeV;         //!
  TBranch        *b_SSD1_fL2FMulti;        //!
  TBranch        *b_SSD1_fL2FNumStrip;     //!
  TBranch        *b_SSD1_fL2FEMeV;         //!
  TBranch        *b_SSD1_fL2BMulti;        //!
  TBranch        *b_SSD1_fL2BNumStrip;     //!
  TBranch        *b_SSD1_fL2BEMeV;         //!
  TBranch        *b_SSD1_fCsIMulti;        //!
  TBranch        *b_SSD1_fCsINum;          //!
  TBranch        *b_SSD1_fCsIECh;          //!
  TBranch        *b_SSD2_fSSDMulti;        //!
  TBranch        *b_SSD2_fL1SMulti;        //!
  TBranch        *b_SSD2_fL1SNumStrip;     //!
  TBranch        *b_SSD2_fL1SEMeV;         //!
  TBranch        *b_SSD2_fL2FMulti;        //!
  TBranch        *b_SSD2_fL2FNumStrip;     //!
  TBranch        *b_SSD2_fL2FEMeV;         //!
  TBranch        *b_SSD2_fL2BMulti;        //!
  TBranch        *b_SSD2_fL2BNumStrip;     //!
  TBranch        *b_SSD2_fL2BEMeV;         //!
  TBranch        *b_SSD2_fCsIMulti;        //!
  TBranch        *b_SSD2_fCsINum;          //!
  TBranch        *b_SSD2_fCsIECh;          //!
  TBranch        *b_SSD3_fSSDMulti;        //!
  TBranch        *b_SSD3_fL1SMulti;        //!
  TBranch        *b_SSD3_fL1SNumStrip;     //!
  TBranch        *b_SSD3_fL1SEMeV;         //!
  TBranch        *b_SSD3_fL2FMulti;        //!
  TBranch        *b_SSD3_fL2FNumStrip;     //!
  TBranch        *b_SSD3_fL2FEMeV;         //!
  TBranch        *b_SSD3_fL2BMulti;        //!
  TBranch        *b_SSD3_fL2BNumStrip;     //!
  TBranch        *b_SSD3_fL2BEMeV;         //!
  TBranch        *b_SSD3_fCsIMulti;        //!
  TBranch        *b_SSD3_fCsINum;          //!
  TBranch        *b_SSD3_fCsIECh;          //!
  TBranch        *b_SSD4_fSSDMulti;        //!
  TBranch        *b_SSD4_fL1SMulti;        //!
  TBranch        *b_SSD4_fL1SNumStrip;     //!
  TBranch        *b_SSD4_fL1SEMeV;         //!
  TBranch        *b_SSD4_fL2FMulti;        //!
  TBranch        *b_SSD4_fL2FNumStrip;     //!
  TBranch        *b_SSD4_fL2FEMeV;         //!
  TBranch        *b_SSD4_fL2BMulti;        //!
  TBranch        *b_SSD4_fL2BNumStrip;     //!
  TBranch        *b_SSD4_fL2BEMeV;         //!
  TBranch        *b_SSD4_fCsIMulti;        //!
  TBranch        *b_SSD4_fCsINum;          //!
  TBranch        *b_SSD4_fCsIECh;          //!
  TBranch        *b_Global_fGlobalMulti;   //!
  TBranch        *b_Global_fGNumOfSSD;     //!
  TBranch        *b_Global_fGL1SNumStrip;  //!
  TBranch        *b_Global_fGL2FNumStrip;  //!
  TBranch        *b_Global_fGL2BNumStrip;  //!
  TBranch        *b_Global_fGCsINum;       //!
  TBranch        *b_Global_fGL1SEMeV;      //!
  TBranch        *b_Global_fGL2FEMeV;      //!
  TBranch        *b_Global_fGL2BEMeV;      //!
  TBranch        *b_Global_fGCsIEMeV;      //!
  TBranch        *b_Global_fGL2FTime;      //!
  TBranch        *b_Global_fTotKinEnergy;  //!
  TBranch        *b_Global_fCalKinEnergy;  //!
  TBranch        *b_Global_fBeta;          //!
  TBranch        *b_Global_fMomentum;      //!
  TBranch        *b_Global_fTheta;         //!
  TBranch        *b_Global_fPhi;           //!
  TBranch        *b_Global_fZ;             //!
  TBranch        *b_Global_fA;             //!


  CSHINEDEELines(TTree* tree=0);
  virtual ~CSHINEDEELines();

  virtual void Init(TTree* tree);

  // 在这里定义需要进行的操作
  virtual void DEE_All      (const char* pathOutput);
  virtual void DEE_L2F_L3   (const char* pathOutput);
  virtual void DEE_L2B_L3   (const char* pathOutput);
  virtual void DEE_L1_L2F   (const char* pathOutput);
  virtual void DEE_L1_L2B   (const char* pathOutput);

  // 在这里进行一些检查
  virtual void Check_EMeVL2F_EMeVL2B_Correlation(const char* pathOutput);
//  virtual void Check_L1_L2F_Matching();
//  virtual void Check_L1_L2B_Matching();
//  virtual void Check_L2F_CsI_Matching();
//  virtual void Check_L2B_CsI_Matching();
};

#endif


//=============================
#ifdef CSHINEDEELines_cxx
//______________________________________________________________________________
CSHINEDEELines::CSHINEDEELines(TTree* tree) : fChain(0)
{
  Init(tree);
}

//______________________________________________________________________________
CSHINEDEELines::~CSHINEDEELines()
{
  if (!fChain) return;
//  delete fChain->GetCurrentFile();
}

//______________________________________________________________________________
void CSHINEDEELines::Init(TTree* tree)
{
  if (!tree) return;
  fChain = tree;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("SSD1.fSSDMulti",       &SSD1_fSSDMulti,       &b_SSD1_fSSDMulti);
  fChain->SetBranchAddress("SSD1.fL1SMulti",       &SSD1_fL1SMulti,       &b_SSD1_fL1SMulti);
  fChain->SetBranchAddress("SSD1.fL1SNumStrip",     SSD1_fL1SNumStrip,    &b_SSD1_fL1SNumStrip);
  fChain->SetBranchAddress("SSD1.fL1SEMeV",         SSD1_fL1SEMeV,        &b_SSD1_fL1SEMeV);
  fChain->SetBranchAddress("SSD1.fL2FMulti",       &SSD1_fL2FMulti,       &b_SSD1_fL2FMulti);
  fChain->SetBranchAddress("SSD1.fL2FNumStrip",     SSD1_fL2FNumStrip,    &b_SSD1_fL2FNumStrip);
  fChain->SetBranchAddress("SSD1.fL2FEMeV",         SSD1_fL2FEMeV,        &b_SSD1_fL2FEMeV);
  fChain->SetBranchAddress("SSD1.fL2BMulti",       &SSD1_fL2BMulti,       &b_SSD1_fL2BMulti);
  fChain->SetBranchAddress("SSD1.fL2BNumStrip",     SSD1_fL2BNumStrip,    &b_SSD1_fL2BNumStrip);
  fChain->SetBranchAddress("SSD1.fL2BEMeV",         SSD1_fL2BEMeV,        &b_SSD1_fL2BEMeV);
  fChain->SetBranchAddress("SSD1.fCsIMulti",       &SSD1_fCsIMulti,       &b_SSD1_fCsIMulti);
  fChain->SetBranchAddress("SSD1.fCsINum",          SSD1_fCsINum,         &b_SSD1_fCsINum);
  fChain->SetBranchAddress("SSD1.fCsIECh",          SSD1_fCsIECh,         &b_SSD1_fCsIECh);

  fChain->SetBranchAddress("SSD2.fSSDMulti",       &SSD2_fSSDMulti,       &b_SSD2_fSSDMulti);
  fChain->SetBranchAddress("SSD2.fL1SMulti",       &SSD2_fL1SMulti,       &b_SSD2_fL1SMulti);
  fChain->SetBranchAddress("SSD2.fL1SNumStrip",     SSD2_fL1SNumStrip,    &b_SSD2_fL1SNumStrip);
  fChain->SetBranchAddress("SSD2.fL1SEMeV",         SSD2_fL1SEMeV,        &b_SSD2_fL1SEMeV);
  fChain->SetBranchAddress("SSD2.fL2FMulti",       &SSD2_fL2FMulti,       &b_SSD2_fL2FMulti);
  fChain->SetBranchAddress("SSD2.fL2FNumStrip",     SSD2_fL2FNumStrip,    &b_SSD2_fL2FNumStrip);
  fChain->SetBranchAddress("SSD2.fL2FEMeV",         SSD2_fL2FEMeV,        &b_SSD2_fL2FEMeV);
  fChain->SetBranchAddress("SSD2.fL2BMulti",       &SSD2_fL2BMulti,       &b_SSD2_fL2BMulti);
  fChain->SetBranchAddress("SSD2.fL2BNumStrip",     SSD2_fL2BNumStrip,    &b_SSD2_fL2BNumStrip);
  fChain->SetBranchAddress("SSD2.fL2BEMeV",         SSD2_fL2BEMeV,        &b_SSD2_fL2BEMeV);
  fChain->SetBranchAddress("SSD2.fCsIMulti",       &SSD2_fCsIMulti,       &b_SSD2_fCsIMulti);
  fChain->SetBranchAddress("SSD2.fCsINum",          SSD2_fCsINum,         &b_SSD2_fCsINum);
  fChain->SetBranchAddress("SSD2.fCsIECh",          SSD2_fCsIECh,         &b_SSD2_fCsIECh);

  fChain->SetBranchAddress("SSD3.fSSDMulti",       &SSD3_fSSDMulti,       &b_SSD3_fSSDMulti);
  fChain->SetBranchAddress("SSD3.fL1SMulti",       &SSD3_fL1SMulti,       &b_SSD3_fL1SMulti);
  fChain->SetBranchAddress("SSD3.fL1SNumStrip",     SSD3_fL1SNumStrip,    &b_SSD3_fL1SNumStrip);
  fChain->SetBranchAddress("SSD3.fL1SEMeV",         SSD3_fL1SEMeV,        &b_SSD3_fL1SEMeV);
  fChain->SetBranchAddress("SSD3.fL2FMulti",       &SSD3_fL2FMulti,       &b_SSD3_fL2FMulti);
  fChain->SetBranchAddress("SSD3.fL2FNumStrip",     SSD3_fL2FNumStrip,    &b_SSD3_fL2FNumStrip);
  fChain->SetBranchAddress("SSD3.fL2FEMeV",         SSD3_fL2FEMeV,        &b_SSD3_fL2FEMeV);
  fChain->SetBranchAddress("SSD3.fL2BMulti",       &SSD3_fL2BMulti,       &b_SSD3_fL2BMulti);
  fChain->SetBranchAddress("SSD3.fL2BNumStrip",     SSD3_fL2BNumStrip,    &b_SSD3_fL2BNumStrip);
  fChain->SetBranchAddress("SSD3.fL2BEMeV",         SSD3_fL2BEMeV,        &b_SSD3_fL2BEMeV);
  fChain->SetBranchAddress("SSD3.fCsIMulti",       &SSD3_fCsIMulti,       &b_SSD3_fCsIMulti);
  fChain->SetBranchAddress("SSD3.fCsINum",          SSD3_fCsINum,         &b_SSD3_fCsINum);
  fChain->SetBranchAddress("SSD3.fCsIECh",          SSD3_fCsIECh,         &b_SSD3_fCsIECh);

  fChain->SetBranchAddress("SSD4.fSSDMulti",       &SSD4_fSSDMulti,       &b_SSD4_fSSDMulti);
  fChain->SetBranchAddress("SSD4.fL1SMulti",       &SSD4_fL1SMulti,       &b_SSD4_fL1SMulti);
  fChain->SetBranchAddress("SSD4.fL1SNumStrip",     SSD4_fL1SNumStrip,    &b_SSD4_fL1SNumStrip);
  fChain->SetBranchAddress("SSD4.fL1SEMeV",         SSD4_fL1SEMeV,        &b_SSD4_fL1SEMeV);
  fChain->SetBranchAddress("SSD4.fL2FMulti",       &SSD4_fL2FMulti,       &b_SSD4_fL2FMulti);
  fChain->SetBranchAddress("SSD4.fL2FNumStrip",     SSD4_fL2FNumStrip,    &b_SSD4_fL2FNumStrip);
  fChain->SetBranchAddress("SSD4.fL2FEMeV",         SSD4_fL2FEMeV,        &b_SSD4_fL2FEMeV);
  fChain->SetBranchAddress("SSD4.fL2BMulti",       &SSD4_fL2BMulti,       &b_SSD4_fL2BMulti);
  fChain->SetBranchAddress("SSD4.fL2BNumStrip",     SSD4_fL2BNumStrip,    &b_SSD4_fL2BNumStrip);
  fChain->SetBranchAddress("SSD4.fL2BEMeV",         SSD4_fL2BEMeV,        &b_SSD4_fL2BEMeV);
  fChain->SetBranchAddress("SSD4.fCsIMulti",       &SSD4_fCsIMulti,       &b_SSD4_fCsIMulti);
  fChain->SetBranchAddress("SSD4.fCsINum",          SSD4_fCsINum,         &b_SSD4_fCsINum);
  fChain->SetBranchAddress("SSD4.fCsIECh",          SSD4_fCsIECh,         &b_SSD4_fCsIECh);

  fChain->SetBranchAddress("Global.fGlobalMulti",  &Global_fGlobalMulti,  &b_Global_fGlobalMulti);
  fChain->SetBranchAddress("Global.fGNumOfSSD",    &Global_fGNumOfSSD,    &b_Global_fGNumOfSSD);
  fChain->SetBranchAddress("Global.fGL1SNumStrip", &Global_fGL1SNumStrip, &b_Global_fGL1SNumStrip);
  fChain->SetBranchAddress("Global.fGL2FNumStrip", &Global_fGL2FNumStrip, &b_Global_fGL2FNumStrip);
  fChain->SetBranchAddress("Global.fGL2BNumStrip", &Global_fGL2FNumStrip, &b_Global_fGL2BNumStrip);
  fChain->SetBranchAddress("Global.fGCsINum",      &Global_fGCsINum,      &b_Global_fGCsINum);
  fChain->SetBranchAddress("Global.fGL1SEMeV",     &Global_fGL1SEMeV,     &b_Global_fGL1SEMeV);
  fChain->SetBranchAddress("Global.fGL2FEMeV",     &Global_fGL2FEMeV,     &b_Global_fGL2FEMeV);
  fChain->SetBranchAddress("Global.fGL2BEMeV",     &Global_fGL2BEMeV,     &b_Global_fGL2BEMeV);
  fChain->SetBranchAddress("Global.fGCsIEMeV",     &Global_fGCsIEMeV,     &b_Global_fGCsIEMeV);
  fChain->SetBranchAddress("Global.fGL2FTime",     &Global_fGL2FTime,     &b_Global_fGL2FTime);
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
