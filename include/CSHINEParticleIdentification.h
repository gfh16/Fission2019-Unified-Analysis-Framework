#ifndef CSHINEPARTICLEIDENTIFICATION_H
#define CSHINEPARTICLEIDENTIFICATION_H

#include "shared.h"
#include "TimeAndPercentage.h"
#include "CSHINETrackReconstruction.h"
#include "ReadFileModule.h"

#include "TLatex.h"
#include "TRandom.h"


//******************************************************************************
// 定义 DEEFIT 的数据结构
struct DEEFITTreeData
{
// DEEFIT 原始数据格式
  UShort_t numtel;  // number of csi crystals
  Float_t  desipgf; // dE (MeV)
  UShort_t  fastpg;  // ECsI (ADC Chs)
};

// 定义粒子属性的数据结构
struct DEEFITParticle
{
  Double_t E, dE;
  Double_t Z, A; // charge and mass number, double type
  Double_t e;    // Error
};


//******************************************************************************
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
  CSHINEDEEFITPID(Int_t firstrun, Int_t lastrun);
  ~CSHINEDEEFITPID();

  // 初始化
  TTree     *fChain;   //!pointer to the analyzed TTree or TChain
  Int_t      fCurrent; //!current Tree number in a TChain
  void       Init(TTree *tree);

  // 功能实现
  void       DEEFITGenerateData_L2L3(); // 借助DEEFIT 工具进行CsI能量刻度, 需要先将实验数据存成DEEFIT格式
  void       DEEFITGenerateData_L1L2();
  void       DEEFITRunCode();           // 运行 DEEFIT 程序，手动选点，对 dE(ECsI,Z,A) 进行拟合

  // 需要用到的函数
  Double_t   DEEFITFunc14(DEEFITParticle& p, Double_t* par); // 定义 Func14 函数
  Double_t **DEEFITLoadPars(const char* pathParsFile);       // 加载已有 Func14 的拟合参数
  Int_t      DEEFITGetCharge(Double_t* par, Double_t de, Double_t fast, Double_t& zeta); // 提取电荷数: 拟合参数, dE, E, (Double_t)Z
  Double_t   DEEFITGetMass(Double_t* par, Int_t charge, Double_t de, Double_t fast, Int_t& imass);     // 提取质量数(double型): 拟合参数, 电荷数 Z, dE, E

  //___________________________________________________
  // 定义变量，用于提取 LayerEvent Tree
  // Declaration of leaf types
  // for L1S
  Int_t                       LayerEvent_fL1SMulti;
  std::vector<Int_t>          LayerEvent_fL1SSSDNum;     //[LayerEvent_fL1SMulti]
  std::vector<Int_t>          LayerEvent_fL1SNumStrip;   //[LayerEvent_fL1SMulti]
  std::vector<Double_t>       LayerEvent_fL1SEMeV;       //[LayerEvent_fL1SMulti]
  // for L2F
  Int_t                       LayerEvent_fL2FMulti;
  std::vector<Int_t>          LayerEvent_fL2FSSDNum;    //[LayerEvent_fL2FMulti]
  std::vector<Int_t>          LayerEvent_fL2FNumStrip;  //[LayerEvent_fL2FMulti]
  std::vector<Double_t>       LayerEvent_fL2FEMeV;      //[LayerEvent_fL2FMulti]
  std::vector<Int_t>          LayerEvent_fL2FTime;      //[LayerEvent_fL2FMulti]
  // for L2B
  Int_t                       LayerEvent_fL2BMulti;
  std::vector<Int_t>          LayerEvent_fL2BSSDNum;    //[LayerEvent_fL2BMulti]
  std::vector<Int_t>          LayerEvent_fL2BNumStrip;  //[LayerEvent_fL2BMulti]
  std::vector<Double_t>       LayerEvent_fL2BEMeV;      //[LayerEvent_fL2BMulti]
  // CsI
  Int_t                       LayerEvent_fCsIMulti;
  std::vector<Int_t>          LayerEvent_fCsISSDNum;    //[LayerEvent_fCsIMulti]
  std::vector<Int_t>          LayerEvent_fCsINum;       //[LayerEvent_fCsIMulti]
  std::vector<Int_t>          LayerEvent_fCsIECh;       //[LayerEvent_fCsIMulti]

  std::vector<Int_t>          LayerEvent_fSSDL1SMulti;  //[NUM_SSD]
  std::vector<Int_t>          LayerEvent_fSSDL2FMulti;  //[NUM_SSD]
  std::vector<Int_t>          LayerEvent_fSSDL2BMulti;  //[NUM_SSD]
  std::vector<Int_t>          LayerEvent_fSSDCsIMulti;  //[NUM_SSD]
};


//******************************************************************************
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

  Double_t      StraighteningGetMass(Double_t e2, Double_t de1, TCutG* cut[50],vector< vector<Double_t> > pars, vector<Int_t> icharge, vector<Int_t> imass, Int_t& geticharge, Int_t& getimass, Double_t& pidnum);
  Double_t      DoCalcdEMeV(Double_t Ex, Double_t* par, Int_t ParsNum);
  Double_t      StdPIDNumber(Int_t Z_charge, Int_t A_mass);
  Double_t      StdPIDNumberToMass(Double_t pidnumber, Int_t charge);
  void          ReadDataFile(const char* pathfile, vector< vector<Int_t> >& icharge, vector< vector<Int_t> >& imass, vector< vector< vector<Double_t> > >& pars);

};


//******************************************************************************
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
  CSHINECheckDEEPlot(Int_t firstrun, Int_t lastrun);
  ~CSHINECheckDEEPlot();

  void    CheckL2L3DEE();
  void    CheckL2L3PIDResults();
  void    CheckCsIAlphaEnergyResolution();

  void    CheckL1L2DEE();
  void    CheckL1L2DEE_Uncalibrated();
  void    CheckDEEL1L2_SiResolution();
  void    CheckL1L2StraighteningPIDResults();
};

#endif
