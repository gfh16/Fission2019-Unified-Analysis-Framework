#include "../include/shared.h"


//______________________________________________________________________________
const Double_t RadToDeg = 180./TMath::Pi();
const Double_t DegToRad = TMath::Pi()/180.;

//______________________________________________________________________________
const Int_t NUM_SSD                   =    4;
const Int_t NUM_STRIP                 =    16;
const Int_t NUM_CSI                   =    9;

const Int_t NUM_MAX_PARTICLES_PER_TEL =    5;

//______________________________________________________________________________
const Double_t NUM_SIGMA_L1S[4]       =    {5.,  7.,  9.,  20.};
const Double_t NUM_SIGMA_L2F[4]       =    {5.,  5.,  5.,  5.};
const Double_t NUM_SIGMA_L2B[4]       =    {5.,  5.,  5.,  5.};
const Double_t NUM_SIGMA_L3A[4]       =    {12., 5.,  12., 10.};

//______________________________________________________________________________
//const Double_t ECSICHCUT            =    150.;  // 手动添加
const Double_t ECSICHCUT              =    80.;   // 手动添加
const Double_t CSIPEDESTALSIGMA       =    4.;    // 手动添加

//______________________________________________________________________________
const char* PATHDATAFOLDER      = "/home/sea/Fission2019-Unified-Analysis-Framework/data/";
const char* PATHFIGURESFOLDER   = "/home/sea/Fission2019-Unified-Analysis-Framework/figures/";
const char* PATHROOTFILESFOLDER = "/home/sea/Fission2019_Data/";
