#ifndef SHARED_H
#define SHARED_H

#include "TMath.h"

//______________________________________________________________________________
extern const double RadToDeg;
extern const double DegToRad;

//______________________________________________________________________________
extern const int NUM_SSD;
extern const int NUM_STRIP;
extern const int NUM_CSI;
extern const int NUM_MAX_PARTICLES_PER_TEL;

//______________________________________________________________________________
// 定义轻粒子的质量
extern const int    NUM_LCPs;
extern const int    Z_A_LCPs[16][2];

//______________________________________________________________________________
extern const double SIL1THICKNESS[4]; //mm
extern const double SIL2THICKNESS[4]; //mm
extern const double CSITHICKNESS;     //mm

//______________________________________________________________________________
extern const double NUM_SIGMA_L1S[4];
extern const double NUM_SIGMA_L2F[4];
extern const double NUM_SIGMA_L2B[4];
extern const double NUM_SIGMA_L3A[4];

//______________________________________________________________________________
// Setting the Cut for CsI
//extern const double ECSICHCUT;
//extern const double CSIPEDESTALSIGMA;

//______________________________________________________________________________
extern const char* PATHDATAFOLDER;        // data 数据文件夹绝对路径
extern const char* PATHFIGURESFOLDER;     // figures 输出文件夹绝对路径
extern const char* PATHROOTFILESFOLDER;   // root 文件所在文件夹绝对路径
extern const char* PATHHOMEFOLDER;        // home 目录所在绝对路径 (在不同机器上使用时需要修改)

extern const char* NAMERAWFILELIST;       //原始文件 list的文件名, 用于 Rawdata->RawRoot
extern const char* NAMERAWROOTFILELIST;   //RawRoot文件 list 的文件名,用于 RawRoot->MapRoot
extern const char* NAMEMAPROOTFILELIST;   //MapRoot文件 list 的文件名,用于 QualityCheck

#endif
