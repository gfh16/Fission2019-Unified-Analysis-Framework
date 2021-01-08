#ifndef CSHINEPARTICLEIDENTIFICATION_H
#define CSHINEPARTICLEIDENTIFICATION_H

#include "shared.h"


//******************************************************************************
// 定义 DEEFIT 的数据结构
struct DEEFITTreeData
{
// DEEFIT 原始数据格式
  UShort_t numtel;  // number of csi crystals
  Float_t  desipgf; // dE (MeV)
  UShort_t fastpg;  // ECsI (ADC Chs)
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
public:
  CSHINEDEEFITPID();
  ~CSHINEDEEFITPID();

  void       DEEFITGenerateData(Int_t firstrun, Int_t lastrun); // 借助DEEFIT 工具进行CsI能量刻度, 需要先将实验数据存成DEEFIT格式
  void       DEEFITRunCode();  // 运行 DEEFIT 程序，手动选点，对 dE(ECsI,Z,A) 进行拟合

  Double_t   DEEFITFunc14(DEEFITParticle& p, Double_t* par); // 定义 Func14 函数
  Double_t **DEEFITLoadPars(const char* pathParsFile);       // 加载已有 Func14 的拟合参数
  Int_t      DEEFITGetCharge(Double_t* par, Double_t de, Double_t fast, Double_t* zeta); // 提取电荷数: 拟合参数, dE, E, (Double_t)Z
  Double_t   DEEFITGetMass(Double_t* par, Int_t charge, Double_t de, Double_t fast, Int_t* imass);     // 提取质量数(double型): 拟合参数, 电荷数 Z, dE, E

private:
  DEEFITTreeData  *fdeefitdata;
  const Int_t      fMAXFP = 14;  //  Func14 包含 14 个参数
};


//******************************************************************************
// "拉直法" : 将个带状结构 "拉直" 后再进行粒子鉴别
class CSHINEStraighteningPID
{
public:
  CSHINEStraighteningPID();
  ~CSHINEStraighteningPID();

  void          StraighteningBuildDEEPlot();
  void          StraighteningExtractPointsAndFit();
  void          StraighteningGetExpPIDNumber(const char* pathRootFile);

  Double_t      DoCalcdEMeV(Double_t Ex, Double_t* par, Int_t ParsNum);
  Double_t      StdPIDNumber(Int_t Z_charge, Int_t A_mass);

};


#endif
