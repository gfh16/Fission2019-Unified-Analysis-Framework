#include "../include/shared.h"


//______________________________________________________________________________
const double RadToDeg = 180./TMath::Pi();
const double DegToRad = TMath::Pi()/180.;

//______________________________________________________________________________
const int NUM_SSD                   =    4;
const int NUM_STRIP                 =    16;
const int NUM_CSI                   =    9;

const int NUM_MAX_PARTICLES_PER_TEL =    5;

//______________________________________________________________________________
const double NUM_SIGMA_L1S[4]       =    {5.,  7.,  9.,  20.};
const double NUM_SIGMA_L2F[4]       =    {5.,  5.,  5.,  5.};
const double NUM_SIGMA_L2B[4]       =    {5.,  5.,  5.,  5.};
const double NUM_SIGMA_L3A[4]       =    {12., 5.,  12., 10.};

//______________________________________________________________________________
//const Double_t ECSICHCUT          =    150.;  // 手动添加
const double ECSICHCUT              =    80.;   // 手动添加
const double CSIPEDESTALSIGMA       =    4.;    // 手动添加

//______________________________________________________________________________
const char* PATHDATAFOLDER      = "/home/sea/Fission2019-Unified-Analysis-Framework/data/";
const char* PATHFIGURESFOLDER   = "/home/sea/Fission2019-Unified-Analysis-Framework/figures/";
const char* PATHROOTFILESFOLDER = "/home/sea/Fission2019_Data/";
const char* PATHHOMEFOLDER      = "/home/sea/Fission2019-Unified-Analysis-Framework/";
