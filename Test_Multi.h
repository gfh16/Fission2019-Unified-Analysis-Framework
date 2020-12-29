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

// Header file for the classes stored in the TTree if any.

class Test_Multi {

private :
  CSHINESSDCalibratedData       fSiEChCut;
  Double_t                                           fL1ResidulE;
  Double_t                                           fL2IncidentE;
  Int_t                                                    fFirstRun;
  Int_t                                                    fLastRun;

public :
  EnergyLossModule                      fLISEModule;
  CSHINETrackReconstruction   fPattern;
  CSHINETrackEvent                       fTrackEvent;
  TTree                                                *fChain;   //!pointer to the analyzed TTree or TChain
  Int_t                                                    fCurrent; //!current Tree number in a TChain

  //___________________________________________________
  // 定义变量，用于提取 LayerEvent Tree
  // Declaration of leaf types

  // for L1S
  Int_t                                           LayerEvent_fL1SMulti;
  std::vector<Int_t>                LayerEvent_fL1SSSDNum;     //[LayerEvent_fL1SMulti]
  std::vector<Int_t>                LayerEvent_fL1SNumStrip;   //[LayerEvent_fL1SMulti]
  std::vector<Double_t>       LayerEvent_fL1SEMeV;       //[LayerEvent_fL1SMulti]
  // for L2F
  Int_t                                           LayerEvent_fL2FMulti;
  std::vector<Int_t>                LayerEvent_fL2FSSDNum;    //[LayerEvent_fL2FMulti]
  std::vector<Int_t>                LayerEvent_fL2FNumStrip;  //[LayerEvent_fL2FMulti]
  std::vector<Double_t>       LayerEvent_fL2FEMeV;      //[LayerEvent_fL2FMulti]
  // for L2B
  Int_t                                            LayerEvent_fL2BMulti;
  std::vector<Int_t>                 LayerEvent_fL2BSSDNum;    //[LayerEvent_fL2BMulti]
  std::vector<Int_t>                 LayerEvent_fL2BNumStrip;  //[LayerEvent_fL2BMulti]
  std::vector<Double_t>        LayerEvent_fL2BEMeV;      //[LayerEvent_fL2BMulti]
  // CsI
  Int_t                                            LayerEvent_fCsIMulti;
  std::vector<Int_t>                 LayerEvent_fCsISSDNum;    //[LayerEvent_fCsIMulti]
  std::vector<Int_t>                 LayerEvent_fCsINum;       //[LayerEvent_fCsIMulti]
  std::vector<Int_t>                 LayerEvent_fCsIECh;       //[LayerEvent_fCsIMulti]

  std::vector<Int_t>                 LayerEvent_fSSDL1SMulti;  //[NUM_SSD]
  std::vector<Int_t>                 LayerEvent_fSSDL2FMulti;  //[NUM_SSD]
  std::vector<Int_t>                 LayerEvent_fSSDL2BMulti;  //[NUM_SSD]
  std::vector<Int_t>                 LayerEvent_fSSDCsIMulti;  //[NUM_SSD]


  Test_Multi(Int_t firstrun, Int_t lastrun);
  virtual ~Test_Multi();
  virtual void     Init(TTree *tree);


  //_________________________________________
  //user-defined function
  virtual void     TrackReconstructionAlgorithm(); // 生成临时的 trackevent,以方面模式识别
  virtual void     CheckTrackEvent();  //检查径迹查找算法得到的 trackevent 是否合理

  virtual void     CheckCutEffOnMulti();  // 测试约束条件对事件数的影响, 计算每个条件对每一层的贡献
  virtual void     CheckL2BL2FEnergyCorrelation();
  virtual void     DetermineL2BL2FEnergyErrRatio();
  virtual void     CheckEnergyLossL1L2();
  virtual void     CheckEnergyLossL1L2_Relationship();
  virtual void     CheckEnergyLossL1L2_Expdata();
  virtual void     CheckLayerMultiPercentage();
  virtual void     CheckGlobalMultiRatio(Int_t ssdindex);
  virtual void     GlobalMulti_ExtractData(Int_t globalmulti);
  virtual void     CheckParallelDraw(const char* fglobalmulti);
  virtual void     CheckL2L3DEEPlot(const char* fglobalmulti);
  virtual void     CheckL1L2DEEPlot(const char* fglobalmulti);
  virtual void     CheckChargeSharingEffect_L1(Int_t ssdindex);
  virtual void     ChangeModeToNumber(std::string mode, Int_t& l3a, Int_t& l2b, Int_t& l2f, Int_t& l1s);

  virtual Bool_t   IsModeMatched(Int_t mode_index, Int_t ch1, Int_t ch2);
};

#endif
