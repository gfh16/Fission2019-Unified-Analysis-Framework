////////////////////////////////////////////////////////////////////////////////
//
//    DEEFIT Root data 数据格式有特殊的要求:
//    ** tree 中只有三个 branch
//       h1: tree 的名字
//        numtel:  CsI 编号,
//        desipgf: Si中能损(MeV)
//        fastpg:  CsI中的能量沉积(Ch)
//
//    ** 数据类型:
//        numtel/s     --> numtel/i
//        desipgf/f    --> desimev/d
//        fastpg/s     --> ecsich/i
//
//   现在要做的是: (只考虑 one-hit 事件)
//   1. 给 CsI 设定一个 Cut 值, 当 ECsI_CH > Cut 时, 认为 CsI 中是一个有效的 hit
//   2. 利用硅条的能量刻度曲线, 计算粒子在硅中的能量 ESi_MeV
//   3. 创建 tree h1，有三个 branch:
//      (1) numtel/s  : 从 0 - 35, 共 4 * 9 = 36 块 CsI
//      (2) desimev/d : ESi_MeV
//      (3) ecsich/d  : ECsI_CH
//    需要注意 branch 的数据类型!
//
////////////////////////////////////////////////////////////////////////////////

#include "../../include/EnergyLossModule.h"
#include "../../include/ReadFileModule.h"


//______________________________________________________________________________
const Int_t SSDNum    = 4;
const Int_t CHNum     = 16;
const Int_t CsINum    = 9;

const Int_t FirstRun  = 200;
const Int_t LastRun   = 220;

const Int_t ParNum    = 2;
const Int_t SigmaNum  = 5;
const Int_t NBinsX    = 4096;
const Int_t NBinsY    = 4096;

const Int_t kMaxMultiSi  = 5;
const Int_t kMaxMultiCsI = 5;


//______________________________________________________________________________
// 定义 SiCalibratedEvent 类
class SiCalibratedEvent
{
public:
  Int_t    fMulti;
  Int_t    fNumStripL1S  [kMaxMultiSi];
  Int_t    fNumStripL2F  [kMaxMultiSi];
  Int_t    fNumStripL2B  [kMaxMultiSi];
  Int_t    fNumCsI       [kMaxMultiCsI];

  Double_t fESiMeVL1S    [kMaxMultiSi];
  Double_t fESiMeVL2F    [kMaxMultiSi];
  Double_t fESiMeVL2B    [kMaxMultiSi];
  Int_t    fECsICH       [kMaxMultiCsI];  // ADC 道址以整数表示

  Double_t fThetaDeg     [kMaxMultiSi];
  Double_t fPhiDeg       [kMaxMultiSi];

  SiCalibratedEvent() {fMulti=0;}
  ClassDef(SiCalibratedEvent,1);
};



//______________________________________________________________________________
void SiCalibratedDataToDEEData()
{
  std::string pathExpDataFolder("/home/sea//Fission2019_Data/MapRoot/");

  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string L3ATag("L3A");

  EnergyLossModule lisecalculator;
  ReadFileModule   readfile;

  // 定义名为 CSHINE2019 的新的 tree, 用于存储数据，便于后续用于转换为 DEEFIT 数据
  SiCalibratedEvent event;
  TFile* rootfile = new TFile("rootfiles/SiCalibratedData.root","RECREATE");
  TTree* EXP2019 = new TTree("EXP2019","Tree data with Si calibrated results");
  EXP2019->Branch("fMulti",      &event.fMulti,       "fMulti/I");
  EXP2019->Branch("fNumStripL1S", event.fNumStripL1S, "fNumStripL1S[fMulti]/I");
  EXP2019->Branch("fNumStripL2F", event.fNumStripL2F, "fNumStripL2F[fMulti]/I");
  EXP2019->Branch("fNumStripL2B", event.fNumStripL2B, "fNumStripL2B[fMulti]/I");
  EXP2019->Branch("fNumCsI",      event.fNumCsI,      "fNumCsI[fMulti]/I");
  EXP2019->Branch("fESiMeVL1S",   event.fESiMeVL1S,   "fESiMeVL1S[fMulti]/D");
  EXP2019->Branch("fESiMeVL2F",   event.fESiMeVL2F,   "fESiMeVL2F[fMulti]/D");
  EXP2019->Branch("fESiMeVL2B",   event.fNumStripL2B, "fESiMeVL2B[fMulti]/D");
  EXP2019->Branch("fECsICH",      event.fECsICH,      "fECsICH[fMulti]/I");
  EXP2019->Branch("fThetaDeg",    event.fThetaDeg,    "fThetaDeg[fMulti]/D");
  EXP2019->Branch("fPhiDeg",      event.fPhiDeg,      "fPhiDeg[fMulti]/D");


  // Load experimental data
  TChain* myData = new TChain("KrPb");
  for (Int_t i=0; i< (LastRun+1); i++)
  {
    myData->Add(Form("%sMapFission2019_Kr_Pb%04d.root", pathExpDataFolder.c_str(), i));
  }
  myData->SetBranchStatus("*",false); // 先关闭所有的 branch

  for (Int_t i=0; i<SSDNum; i++)
  {

  }

}
