#ifndef Test_Multi_h
#define Test_Multi_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include "include/CSHINETrackReconstruction.h"
#include "include/EnergyLossModule.h"
#include "include/CSHINESSDCalibratedData.h"
#include "include/TimeAndPercentage.h"
#include "include/shared.h"


//******************************************************************************
class Test_Multi
{

private :
  CSHINESSDCalibratedData     fSiEChCut;
  Double_t                    fL1ResidulE;
  Double_t                    fL2IncidentE;
  Int_t                       fFirstRun;
  Int_t                       fLastRun;

public :
  TimeAndPercentage           timeper;
  EnergyLossModule            fLISEModule;
  CSHINETrackReconstruction   fPattern;
  CSHINETrackEvent            fTrackEvent;
  TTree                      *fChain;   //!pointer to the analyzed TTree or TChain
  Int_t                       fCurrent; //!current Tree number in a TChain

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


  Test_Multi(Int_t firstrun, Int_t lastrun);
  virtual ~Test_Multi();
  virtual void     Init(TTree *tree);

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

  //___________________________________________
  //           电荷分配效应
  //         --------------
  virtual void     CheckChargeSharingEffect_L1(Int_t ssdindex);


  //_________________________________________
  //          径迹重建 与 相关检测
  //       -----------------------
  // 径迹重建算法
  virtual void     TrackReconstructionAlgorithm(); // 生成临时的 trackevent,以方面模式识别
  // 径迹重建事件检查
  virtual void     CheckTrackReconstructionResult();  //检查径迹查找算法得到的 trackevent 是否合理
  virtual void     CheckGlobalMultiRatio(); // 检查不同 globalmulti 所占的比例
  virtual void     GlobalMulti_ExtractData(Int_t globalmulti);
  virtual void     CheckParallelDraw(const char* fglobalmulti);
  virtual void     CheckL2L3DEEPlot(const char* fglobalmulti);
  virtual void     CheckL1L2DEEPlot(const char* fglobalmulti);

  //_____________________________________
  //               模式识别
  //            -------------
  virtual void     ChangeModeToNumber(std::string mode, Int_t& l3a, Int_t& l2b, Int_t& l2f, Int_t& l1s);
  virtual Bool_t   IsModeMatched(Int_t mode_index, Int_t ch1, Int_t ch2);
};
//******************************************************************************

/*
//##############################################################################
//                           模式识别分类讨论
//                        --------------------
//_______________________________________________
// fglobalmulti == 1 的情况
class DecodefGlobalMulti1 : public Test_Multi
{
public:
  DecodefGlobalMulti1();
  ~DecodefGlobalMulti1();

  void    GlobalMulti1_ExtractData(const Int_t globalmulti=1);
  void    GlobalMulti1_Checks(const Int_t globalmulti=1);
};


//_______________________________________________
// fglobalmulti == 2 的情况
class DecodefGlobalMulti2 : public Test_Multi
{
public:
  DecodefGlobalMulti2(TTree* mytree=0);
  ~DecodefGlobalMulti2();

  TTree  *fChain;

  void    GlobalMulti2_ExtractData(const Int_t globalmulti=2);
  void    GlobalMulti2_Checks(const Int_t globalmulti=2);

  void    GlobalMulti2_CalcModeRatio(); // 计算不同模式所占比例

  void    GlobalMulti2_Decode_0000(const Int_t globalmulti=2, const char* mode="0000");
  void    GlobalMulti2_Decode_1110(const Int_t globalmulti=2, const char* mode="1110");
  void    GlobalMulti2_Decode_1000(const Int_t globalmulti=2, const char* mode="1000");
  void    GlobalMulti2_Decode_0111(const Int_t globalmulti=2, const char* mode="0111");
  void    GlobalMulti2_Decode_1101(const Int_t globalmulti=2, const char* mode="1101");
  void    GlobalMulti2_Decode_1001(const Int_t globalmulti=2, const char* mode="1001");
  void    GlobalMulti2_Decode_0001(const Int_t globalmulti=2, const char* mode="0001");
  void    GlobalMulti2_Decode_1010(const Int_t globalmulti=2, const char* mode="1010");
  void    GlobalMulti2_Decode_0101(const Int_t globalmulti=2, const char* mode="0101");
  void    GlobalMulti2_Decode_1011(const Int_t globalmulti=2, const char* mode="1011");
  void    GlobalMulti2_Decode_0010(const Int_t globalmulti=2, const char* mode="0010");

private:
  void    GlobalMulti2_DataClassification(std::string mode, Double_t& ratio); // index 用于区分不同的模式
  Bool_t  Is_Mode1110_Decoded(Double_t el10, Double_t el11, Double_t el2f);  // 判断是否满足 cut 条件
  Bool_t  Is_Mode0111_Decoded(Double_t el31, Double_t el32);
  Bool_t  Is_Mode1001_Decoded(Int_t l2bch1, Int_t l2bch2);

  Int_t SSD2_GlobalMulti;
  std::vector<Int_t>    SSD2_L1SNumStrip;
  std::vector<Double_t> SSD2_L1SEMeV;
  std::vector<Int_t>    SSD2_L2FNumStrip;
  std::vector<Double_t> SSD2_L2FEMeV;
  std::vector<Int_t>    SSD2_L2BNumStrip;
  std::vector<Double_t> SSD2_L2BEMeV;
  std::vector<Int_t>    SSD2_CsINum;
  std::vector<Int_t>    SSD2_CsIECh;
};
//##############################################################################
*/

#endif
