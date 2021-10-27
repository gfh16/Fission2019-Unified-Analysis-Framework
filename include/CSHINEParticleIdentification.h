#ifndef CSHINEPARTICLEIDENTIFICATION_H
#define CSHINEPARTICLEIDENTIFICATION_H

#include "shared.h"
#include "TimeAndPercentage.h"
#include "CSHINETrackReconstruction.h"
#include "ReadFileModule.h"

#include "TLatex.h"
#include "TRandom.h"

#include <iostream>
#include <vector>


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// 定义 DEEFIT 的数据结构
struct DEEFITTreeData
{
// DEEFIT 原始数据格式
  UShort_t  numtel;  // number of csi crystals
  Float_t   desipgf; // dE (MeV)
  UShort_t  fastpg;  // ECsI (ADC Chs)
};

// 定义粒子属性的数据结构
struct DEEFITParticle
{
  Double_t E, dE;
  Double_t Z, A; // charge and mass number, double type
  Double_t e;    // Error
};


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// “全局拟合法”: 使用 DEEFIT 工具
class CSHINEDEEFITPID
{
private:
  Int_t                       fFirstRun;
  Int_t                       fLastRun;
  DEEFITTreeData             *fdeefitdata;
  const Int_t                 fMAXFP = 14;  // Func14 包含 14 个参数
  TimeAndPercentage           timeper;
  CSHINETrackReconstruction   fPattern;

public:
  CSHINELayerEvent            fLayerEvent;

public:
  CSHINEDEEFITPID();
  CSHINEDEEFITPID(Int_t firstrun, Int_t lastrun);
  ~CSHINEDEEFITPID();

  // 初始化
  TTree     *fChain;   //!pointer to the analyzed TTree or TChain
  void       Init(TTree *tree);

  // 功能实现
  Int_t      GetZone(Int_t stripb, Int_t stripf, Int_t ZoneLength);
  void       DEEFITGenerateData_L2L3(); // 借助DEEFIT 工具进行CsI能量刻度, 需要先将实验数据存成DEEFIT格式
  void       DEEFITGenerateData_L1L2();
  void       DEEFITRunCode();           // 运行 DEEFIT 程序，手动选点，对 dE(ECsI,Z,A) 进行拟合

  // 需要用到的函数
  Double_t   DEEFITFunc14(DEEFITParticle& p, Double_t* par); // 定义 Func14 函数
  Double_t **DEEFITLoadPars(const char* pathParsFile);       // 加载已有 Func14 的拟合参数
  Int_t      DEEFITGetCharge(Double_t* par, Double_t de, Double_t fast, Double_t& zeta); // 提取电荷数: 拟合参数, dE, E, (Double_t)Z
  Double_t   DEEFITGetMass(Double_t* par, Int_t charge, Double_t de, Double_t fast, Int_t& imass);     // 提取质量数(double型): 拟合参数, 电荷数 Z, dE, E
};


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// "拉直法" : 将个带状结构 "拉直" 后再进行粒子鉴别
class CSHINEStraighteningPID
{
public:
  CSHINEStraighteningPID();
  ~CSHINEStraighteningPID();
  // 功能实现
  void          StraighteningBuildDEEPlot();
  void          DoBananaCut();
  void          StraighteningExtractPointsAndFit();
  void          StraighteningGetExpPIDNumber(const char* pathRootFile);

  Double_t      StraighteningGetMass(Double_t e2, Double_t de1, TCutG* cut[50],std::vector< std::vector<Double_t> > pars, std::vector<Int_t> icharge, std::vector<Int_t> imass, Int_t& geticharge, Int_t& getimass, Double_t& pidnum);
  Double_t      DoCalcdEMeV(Double_t Ex, Double_t* par, Int_t ParsNum);
  Double_t      StdPIDNumber(Int_t Z_charge, Int_t A_mass);
  Double_t      StdPIDNumberToMass(Double_t pidnumber, Int_t charge);
  void          ReadStraighteningFitPars(Int_t TelNum, const char* pathfile, std::vector< std::vector<Int_t> >& icharge, std::vector< std::vector<Int_t> >& imass, std::vector< std::vector< std::vector<Double_t> > >& pars);

};


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// 在进行粒子鉴别之前, 对 DEE plot 进行必要的检查,
// 以确定如何得到较好的 DEE plot
class CSHINECheckDEEPlot
{
private:
  Int_t                      fFirstRun;
  Int_t                      fLastRun;
  TimeAndPercentage          timeper;
  CSHINETrackReconstruction  fPattern;
  CSHINEDEEFITPID           *fDeefit;
  ReadFileModule             readfile;
  DEEFITParticle             fDeefitPartile;
  CSHINEStraighteningPID     fStraightening;

public:
  CSHINECheckDEEPlot();
  CSHINECheckDEEPlot(Int_t firstrun, Int_t lastrun);
  ~CSHINECheckDEEPlot();

  void    CheckL2L3DEE();
  // deefit results
  void    CheckL2L3PIDResults();
  void    CheckCsIAlphaEnergyResolution();
  // straigtening results
  void    CheckPolyFitResults();
  void    CheckL2L3StraighteningPIDResults();
  void    L2L3DrawPIDECsIMap();

  void    CheckL1L2DEE();
  void    CheckL1L2DEE_Uncalibrated();
  void    CheckDEEL1L2_SiResolution();
  void    CheckL1L2StraighteningPIDResults();

  void    DrawPID(TH2D* h2d, TH2D* h2d_cut, TH1D* h_pid, std::string path);
  Bool_t  IsPixelOnCsIGap(Int_t stripf, Int_t stripb);

  void    CheckGoodPIDOfSSD3AndSSD4();
  void    CheckL1L2DEEPunchThrough();
  void    CheckL2L3DEECsIGapEffect();

  void    L1S_FindThicknessUniformity();
  void    L1S_DetermineThicknessUniformity();

private:
  std::vector<Double_t> L1L2_EL1S;
  std::vector<Double_t> L1L2_EL2F;
	std::vector<Double_t> L1L2_EL2B;
  std::vector<Int_t>    L1L2_StripL1S;
  std::vector<Int_t>    L1L2_StripL2F;
	std::vector<Int_t>    L1L2_StripL2B;
	std::vector<Int_t>    L2FTime;

  std::vector<Double_t> L2L3_EL2F;
  std::vector<Double_t> L2L3_EL2B;
  std::vector<Double_t> L2L3_ECSI;
  std::vector<Int_t>    L2L3_StripL2F;
  std::vector<Int_t>    L2L3_StripL2B;
  std::vector<Int_t>    L2L3_NumCsI;
};

#endif
