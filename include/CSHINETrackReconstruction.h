#ifndef CSHINETRACKRECONSTRUCTION_H
#define CSHINETRACKRECONSTRUCTION_H

#include "EnergyLossModule.h"
#include "CSHINESSDCalibratedData.h"
#include "TimeAndPercentage.h"
#include "shared.h"
#include "CSHINEEvent.h"

#include <iostream>
#include <TTree.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TMath.h>


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
struct BananaCut
{
 std::string  CutName;
 Int_t        Cut_Z;
 Int_t        Cut_A;
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
class CSHINETrackReconstruction
{
public:
  CSHINETrackReconstruction();
  CSHINETrackReconstruction(Int_t firstrun, Int_t lastrun);
  ~CSHINETrackReconstruction();
  void     InitLayerTree(TTree *tree);
  void     InitTrackTree(TTree *tree);

  //   在这里定义需要进行的操作
  // ------------------------
  // 计算每一层的多重数
  Int_t    LayerMultiplicity   (Int_t, const char*, Int_t*, Double_t*); // ssdindex, layertag, ech[], echcut[]

  // L3A_L2B 几何约束
  Bool_t   IsGeoConstraint_L3A_L2B(Int_t csiindex, Int_t  stripl2b);
  Bool_t   IsGeoConstraint_L3A_L2B(Int_t ssdindex, Int_t csinum, Int_t csissdnum,Int_t l2bmulti, Int_t* l2bstrip, Int_t* l2bssdnum);
  Bool_t   IsGeoConstraint_L3A_L2B(Int_t ssdindex, Int_t csimulti, Int_t* csinum, Int_t* csissdnum, Int_t l2bstrip, Int_t l2bssdnum);
  Bool_t   IsGeoConstraint_L3A_L2B(Int_t ssdindex, Int_t csimulti, Int_t* csinum,Int_t* csissdnum, Int_t l2bmulti, Int_t* l2bstrip, Int_t* l2bssdnum);
  // L3A_L2F 几何约束
  Bool_t   IsGeoConstraint_L3A_L2F(Int_t csiindex, Int_t stripl2f);
  Bool_t   IsGeoConstraint_L3A_L2F(Int_t ssdindex, Int_t csinum, Int_t csissdnum, Int_t l2fmulti, Int_t* l2fstrip, Int_t* l2fssdnum);
  Bool_t   IsGeoConstraint_L3A_L2F(Int_t ssdindex, Int_t csimulti, Int_t* csinum, Int_t* csissdnum, Int_t l2fstrip, Int_t l2fssdnum);
  Bool_t   IsGeoConstraint_L3A_L2F(Int_t ssdindex, Int_t csimulti, Int_t* csinum, Int_t* csissdnum, Int_t l2fmulti, Int_t* l2fstrip, Int_t* l2fssdnum);
  // L2B_L1S 几何约束
  Bool_t   IsGeoConstraint_L2B_L1S(Int_t stripl2b, Int_t stripl1s);
  Bool_t   IsGeoConstraint_L2B_L1S(Int_t stripl2b, Int_t stripl1s, Int_t deltastrip);
  Bool_t   IsGeoConstraint_L2B_L1S(Int_t ssdindex, Int_t l2bstrip, Int_t l2bssdnum, Int_t l1smulti, Int_t* l1sstrip, Int_t* l1sssdnum);
  Bool_t   IsGeoConstraint_L2B_L1S(Int_t ssdindex, Int_t l2bmulti, Int_t* l2bstrip, Int_t* l2bssdnum, Int_t l1sstrip, Int_t l1sssdnum);
  Bool_t   IsGeoConstraint_L2B_L1S(Int_t ssdindex, Int_t l2bmulti, Int_t* l2bstrip, Int_t* l2bssdnum, Int_t l1smulti, Int_t* l1sstrip, Int_t* l1sssdnum);
  // L2B_L2F 能量约束
  Bool_t   IsEneConstraint_L2B_L2F(Double_t El2b, Double_t El2f, Double_t ErrRatio);
  Bool_t   IsEneConstraint_L2B_L2F(Int_t ssdindex, Double_t El2b, Int_t l2bssdnum, Int_t l2fmulti, Double_t* El2f, Int_t* l2fssdnum, Double_t ErrRatio);
  Bool_t   IsEneConstraint_L2B_L2F(Int_t ssdindex, Int_t l2bmulti, Double_t* El2b, Int_t* l2bssdnum, Double_t El2f, Int_t l2fssdnum, Double_t ErrRatio);
  Bool_t   IsEneConstraint_L2B_L2F(Int_t ssdindex, Int_t l2bmulti, Double_t* El2b, Int_t* l2bssdnum, Int_t l2fmulti, Double_t* El2f, Int_t* l2fssdnum, Double_t ErrRatio);
  Bool_t   IsEneConstraint_L2B_L2F(Int_t ssdindex, Int_t l2bmulti, Double_t* El2b, Int_t* l2bssdnum, Int_t l2fmulti, Double_t* El2f, Int_t* l2fssdnum);
  Bool_t   IsEneConstraint_L2B_L2F(Int_t ssdindex, Double_t El2b, Double_t El2f);

  // 对于能穿透 L2 的粒子, 通过 LISE 计算给出 L1, L2 的能损比例, 作为新的约束条件
  Bool_t   IsEneConstraint_L1S_L2F(Int_t ssdindex, Double_t El1, Double_t El2);
  Bool_t   IsEneConstraint_L1S_L2F(Double_t El1, Double_t El2, Double_t eratio);
  Bool_t   IsEneConstraint_L1S_L2F(Int_t ssdindex, Int_t l1smulti, Double_t* El1s, Int_t* l1sssdnum, Int_t l2fmulti, Double_t* El2f, Int_t* l2fssdnum);

  Bool_t   IsEneConstraint_EL1S_ELISECalc(Double_t el1s, Double_t eLISE, Double_t errratio);

  // 判断一个给定的能量点是否在某个 bannana cut 内
  Bool_t   IsInsideABananaCut(std::vector<TCutG*> cut, Double_t e2, Double_t de1);
  Bool_t   IsInsideABananaCut(std::vector<TCutG*> cut, std::string& cutname, Double_t e2, Double_t de1);
  Bool_t   IsInsideABananaCut(std::vector<TCutG*> cut, BananaCut& mcut, Double_t e2, Double_t de1);
  void     GetZAFromBananaCutName(std::string cutname, Int_t& Z, Int_t& A);

  // 根据 L2F, L2B 的位置, 确定对应的 CsI 位置
  Int_t    GetCsINumFromPixel(Int_t stripl2f, Int_t stripl2b);

  void     EstimateLayerMulti(Int_t firstrun, Int_t lastrun);
  void     GetPunchThroughEnergiesOfLCPs();

public:
  TTree                      *fChainLayerTree;   //!pointer to the analyzed TTree or TChain
  TTree                      *fChainTrackTree;
  CSHINETrackEvent            fTrackEvent;
  CSHINELayerEvent            fLayerEvent;

private:
  CSHINESSDCalibratedData     fCSHINESSDCalibratedData;
  EnergyLossModule            fLISEModule;
  TimeAndPercentage           timeper;

  Double_t                    fL1ResidulE;
  Double_t                    fL2IncidentE;
  Int_t                       fFirstRun;
  Int_t                       fLastRun;

public:
  //___________________________________________
  //         径迹重建前的一些测试与检查
  //        -----------------------
  // L2F-L2B 能量关联
  virtual void     CheckL2BL2FEnergyCorrelation(Int_t ssdindex);
  virtual void     DetermineL2BL2FEnergyErrRatio();
  // L1S-L2F 能量关联
  virtual void     CheckEnergyLossL1L2();
  virtual void     CheckEnergyLossL1L2_Relationship(Bool_t punchthrough);
  virtual void     CheckEnergyLossL1L2_Expdata();
  // 多重性测试
  virtual void     CheckLayerMultiPercentage();

public:
  //___________________________________________
  //           电荷分配效应
  //         --------------
  //
  virtual void     CheckAlphaCaliChargeSharing();
  // 以下是对束流实验数据进行电荷分配效应的分析
  virtual void     CheckClusterSize_Si();
  virtual void     CalcClusterSize_Equal2_ERatio();
  virtual void     CalcCluseterSize_SiLayer(Int_t layermulti, vector<Int_t> numstrip, Int_t& clustersize);
  virtual void     ClusterSize_Equal2_SiLayer(Int_t layermulti, vector<Int_t> numstrip, vector<Double_t> ene, Int_t* strip, Double_t* efound, Double_t& eratio);

  virtual void     ClusterSize_Equal2_CsI();
  virtual void     CheckCrossTalk_CsI();
  virtual Bool_t   IsAdj_CsI(Int_t n1, Int_t n2); //判断两块 CsI 是否相邻

public:
  //__________________________________________________
  //          L2_vs_L3 径迹重建 与 相关检测
  //       -----------------------
  // 径迹重建算法
  virtual void      L2L3_TrackReconstructionAlgorithm(); // 生成临时的 trackevent,以方面模式识别
  virtual void      L1L2_TrackReconstructionAlgorithm(); // 生成临时的 trackevent,以方面模式识别

  // 径迹重建事件检查; 根据 globalmulti 不同, 提取数据
  virtual void      CheckGlobalMultiRatio(const char* pidtag, Int_t multicut); // 检查不同 globalmulti 所占的比例
  virtual void      GlobalMulti_ExtractData(const char* pidtag, Int_t globalmulti);

public:
  // L2L3 的一些检查
  virtual void      L2L3_CheckParallelDraw(const char* gmultitag, Int_t gmulti);
  virtual void      L2L3_CheckL2L3DEEPlot(const char* gmultitag, Int_t gmulti);
  virtual void      L2L3_CheckL1L2DEEPlot(const char* gmultitag, Int_t gmulti);

public:
  // L2L3 径迹重建的模式定义
  virtual void     L2L3_GenerateModeFromGMulti(Int_t globalmulti, vector<string>& mode);
  virtual string   L2L3_CalcModeFromExpData(Int_t globalmultti, vector<Int_t> mcsi, vector<Int_t> ml2b, vector<Int_t> ml2f, vector<Int_t> ml1s);
  virtual void     L2L3_Mode_DrawClassification(Int_t globalmulti);
  virtual void     L2L3_Mode_CalcRatio(Int_t globalmulti);
  virtual void     L2L3_Mode_ExtractData(Int_t globalmulti);

public:
  // L1L2 径迹重建的模式定义
  virtual void     L1L2_GenerateModeFromGMulti(Int_t globalmulti, vector<string>& mode);
  virtual string   L1L2_CalcModeFromExpData(Int_t globalmultti, vector<Int_t> ml2b, vector<Int_t> ml2f, vector<Int_t> ml1s);
  virtual void     L1L2_Mode_DrawClassification(Int_t globalmulti);
  virtual void     L1L2_Mode_CalcRatio(Int_t globalmulti);
  virtual void     L1L2_Mode_ExtractData(Int_t globalmulti);

public:
  // 计算候选径迹的 submode
  virtual Int_t    L2L3_GetSubModeFromEneConstraints(Bool_t IsBananaCut, Bool_t IsEL2BEL2F, Bool_t IsEL1SEL2F); // for L2L3
  virtual Int_t    L1L2_GetSubModeFromEneConstraints(Bool_t IsEL2BEL2F, Bool_t IsBananaCut); // for L1L2

private:
  Double_t EL1Range[4] = {400., 320., 100., 100.};
	Int_t    NBinsL1 [4] = {4000, 3200, 1000, 1000};
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2 [4] = {3000, 2500, 1500, 800};

  std::vector<Int_t>    L2L3_SSDNum[NUM_SSD];
  std::vector<Int_t>    L2L3_CsINum[NUM_SSD];
  std::vector<Int_t>    L2L3_L2BStripNum[NUM_SSD];
  std::vector<Int_t>    L2L3_L2FStripNum[NUM_SSD];
  std::vector<Int_t>    L2L3_L1SStripNum[NUM_SSD];
  std::vector<Int_t>    L2L3_CSIECh[NUM_SSD];
  std::vector<Double_t> L2L3_L2BEMeV[NUM_SSD];
  std::vector<Double_t> L2L3_L2FEMeV[NUM_SSD];
  std::vector<Double_t> L2L3_L1SEMeV[NUM_SSD];
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo

#endif
