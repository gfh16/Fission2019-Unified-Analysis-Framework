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
const int NUM_LCPs                  =    16;
const int Z_A_LCPs[16][2]           = { {1,1}, {1,2}, {1,3},   // H1,  H2,  H3
                                        {2,3}, {2,4}, {2,6},   // He3, He4, He6
                                        {3,6}, {3,7}, {3,8},   // Li6, Li7, Li8
                                        {4,7}, {4,9}, {4,10},  // Be7, Be9, Be10
                                        {5,8}, {5,10},{5,11},  // B8,  B10, B11
                                        {6,12}                 // C12
                                      };

//______________________________________________________________________________
const double SIL1THICKNESS[4]       =    {0.304, 0.305, 0.110, 0.070}; //mm
const double SIL2THICKNESS[4]       =    {1.010, 1.008, 0.526, 0.306}; //mm
const double CSITHICKNESS           =    50;     //mm

//______________________________________________________________________________
const double NUM_SIGMA_L1S[4]       =    {5.,  8.,  13., 20.};
const double NUM_SIGMA_L2F[4]       =    {5.,  5.,  5.,  6.};
const double NUM_SIGMA_L2B[4]       =    {5.,  5.,  5.,  6.};
const double NUM_SIGMA_L3A[4]       =    {5.,  8.,  10., 8.};

//______________________________________________________________________________
//const Double_t ECSICHCUT          =    150.;  // 手动添加
//const double ECSICHCUT            =    80.;   // 手动添加
//const double CSIPEDESTALSIGMA     =    4.;    // 手动添加

//______________________________________________________________________________
//本地使用
const char* PATHDATAFOLDER      = "/home/sea/Fission2019-Unified-Analysis-Framework/data/";
const char* PATHFIGURESFOLDER   = "/home/sea/Fission2019-Unified-Analysis-Framework/figures/";
const char* PATHROOTFILESFOLDER = "/home/sea/Fission2019_Data/";
const char* PATHHOMEFOLDER      = "/home/sea/Fission2019-Unified-Analysis-Framework/";

//______________________________________________________________________________
// 服务器上使用
/*
const char* PATHDATAFOLDER      = "/home/gfh/Fission2019-Unified-Analysis-Framework/data/";
const char* PATHFIGURESFOLDER   = "/home/gfh/Fission2019-Unified-Analysis-Framework/figures/";
const char* PATHROOTFILESFOLDER = "/data/EXPdata/Fission2019_Data/";
const char* PATHHOMEFOLDER      = "/home/gfh/Fission2019-Unified-Analysis-Framework/";
*/

const char* NAMERAWFILELIST     = "listRAWfilename.dat";  //原始文件 list的文件名, 用于 Rawdata->RawRoot
const char* NAMERAWROOTFILELIST = "listROOTfilename.dat"; //RawRoot文件 list 的文件名,用于 RawRoot->MapRoot
const char* NAMEMAPROOTFILELIST = "listQCfilename.dat";   //MapRoot文件 list 的文件名,用于 QualityCheck
