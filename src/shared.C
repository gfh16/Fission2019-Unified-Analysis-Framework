#include "../include/shared.h"


//______________________________________________________________________________
const double RadToDeg  =  180./TMath::Pi();
const double DegToRad  =  TMath::Pi()/180.;

//______________________________________________________________________________
const int NUM_SSD    =  4;
const int NUM_STRIP  =  16;
const int NUM_CSI    =  9;

const int NUM_MAX_PARTICLES_PER_TEL = 5;

//______________________________________________________________________________
// 定义轻粒子的原子序数(Z)与质量数(A)
const int NUM_LCPs         =  16;
const int Z_A_LCPs[16][2]  =  { {1,1}, {1,2}, {1,3},   // H1,  H2,  H3
                                {2,3}, {2,4}, {2,6},   // He3, He4, He6
                                {3,6}, {3,7}, {3,8},   // Li6, Li7, Li8
                                {4,7}, {4,9}, {4,10},  // Be7, Be9, Be10
                                {5,10},{5,11},{5,12},  // B10, B11, B12
                                {6,12}                 // C12
                              };

//______________________________________________________________________________
// 定义每一套硅条望远镜、每一层硅的厚度
const double SIL1THICKNESS[4]  =  {0.304, 0.305, 0.110, 0.070}; //mm
const double SIL2THICKNESS[4]  =  {1.010, 1.008, 0.526, 0.306}; //mm
const double CSITHICKNESS      =  50;     //mm

//______________________________________________________________________________
// 定义每一层的 Cut 值所使用的 pedestal sigma number
/*
const double NUM_SIGMA_L1S[4]  =  {12.,  8.,  13., 20.}; // used for check charge sharing
//const double NUM_SIGMA_L1S[4]=  {5.,  8.,  13., 20.};
const double NUM_SIGMA_L2F[4]  =  {5.,  5.,  5.,  6.};
const double NUM_SIGMA_L2B[4]  =  {5.,  5.,  5.,  6.};
const double NUM_SIGMA_L3A[4]  =  {5.,  8.,  10., 8.};
*/
const double NUM_SIGMA_L1S[4]  =  {10,  10,  20,  25};
const double NUM_SIGMA_L2F[4]  =  {10,  10,  10,  10};
const double NUM_SIGMA_L2B[4]  =  {10,  10,  10,  10};
const double NUM_SIGMA_L3A[4]  =  {20,  20,  20,  20};




//______________________________________________________________________________
// 定义每一层的 Muliplicity Cut
const double MULTICUT_L1S  =  6;
const double MULTICUT_L2F  =  5;
const double MULTICUT_L2B  =  5;
const double MULTICUT_L3A  =  4;

//______________________________________________________________________________
// 定义 L2B_L2F Energy Cut
// 根据实验数据 L2B-L2F 能量关联,决定对不同能区采取不同的 cut
const double L2BL2F_ENERGYBOUNDARY   [4] = {10., 10., 10., 10.};
const double L2BL2F_LOWENERGYLOWCUT  [4] = {-0.20, -0.20, -0.10, -0.10};
const double L2BL2F_LOWENERGYHIGHCUT [4] = { 0.15,  0.15,  0.10,  0.10};
const double L2BL2F_HIGHENERGYLOWCUT [4] = {-0.15, -0.20, -0.05, -0.05};
const double L2BL2F_HIGHENERGYHIGHCUT[4] = {-0.05, -0.05,  0.05,  0.05};


//______________________________________________________________________________
// 对于能穿透 L2 的粒子, 通过 LISE 计算给出 L1, L2 的能损比例, 作为新的约束条件
const double L1L2_ENERGYLOWCUT [4] = {0.05, 0.05, 0.05, 0.05};
const double L1L2_ENERGYHIGHCUT[4] = {0.50, 0.50, 0.50, 0.50};


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
