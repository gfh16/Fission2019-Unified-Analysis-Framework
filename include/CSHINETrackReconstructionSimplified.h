#ifndef CSHINETRACKRECONSTRUCTIONSIMPLIFIED_H
#define CSHINETRACKRECONSTRUCTIONSIMPLIFIED_H

#include "CSHINESiPixelGeometry.h"
#include "CSHINECsIEnergyCaliSimplified.h"
#include "PhysicalCalculator.h"
#include "Vector3.h"
#include "CSHINEParticleIdentificationSimplified.h"


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//  将径迹重建实际用到的功能单独写在这里, 因为 CSHINETrackReconstruction 比较甬长
//  不利于阅读
//
//  gfh, 2021-08
//____________________
struct MBananaCut
{
 std::string  CutName;
 Int_t        Cut_Z;
 Int_t        Cut_A;
};
//_______________________
class CSHINETrackReconstructionSimplified
{
public:
  CSHINETrackReconstructionSimplified();
  ~CSHINETrackReconstructionSimplified();

public:
  Bool_t    IsGeoConstraint_L3A_L2B(Int_t csiindex, Int_t  stripl2b); // L3A_L2B 几何约束
  Bool_t    IsGeoConstraint_L3A_L2F(Int_t csiindex, Int_t stripl2f);  // L3A_L2F 几何约束
  Bool_t    IsGeoConstraint_L2B_L1S(Int_t stripl2b, Int_t stripl1s);  // L2B_L1S 几何约束

  Bool_t    IsEneConstraint_L2B_L2F(Int_t ssdindex, Double_t El2b, Double_t El2f);
  Bool_t    IsEneConstraint_L1S_L2F(Int_t ssdindex, Double_t El1s, Double_t El2f);  // 对于能穿透 L2 的粒子, 通过 LISE 计算给出 L1, L2 的能损比例, 作为新的约束条件

  Bool_t    IsInsideABananaCut(std::vector<TCutG*> cut, MBananaCut& mcut, Double_t e2, Double_t de1);
  Int_t     GetCsINumFromPixel(Int_t stripl2f, Int_t stripl2b); // 根据 L2F, L2B 的位置, 确定对应的 CsI 位置

  void      GetZAFromBananaCutName(std::string cutname, Int_t& Z, Int_t& A);

  string    L2L3_CalcModeFromExpData(Int_t globalmultti, vector<Double_t> mcsi, vector<Double_t> ml2b, vector<Double_t> ml2f, vector<Double_t> ml1s);
  string    L1L2_CalcModeFromExpData(Int_t globalmultti, vector<Double_t> ml2b, vector<Double_t> ml2f, vector<Double_t> ml1s);

  Double_t  GetChargeCenter(Double_t strip1, Double_t strip2, Double_t e1, Double_t e2);

  Int_t     GetModeID(Int_t gmulti, Int_t mode_index) const;
};


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//  L2L3 径迹重建的算法摘自源程序 L2L3TrackFinding.C
//  这里将算法部分摘出来, 简化成单独的函数, 以便后续正式调用
//
//  gfh, 2021-08
//_______________________
class L2L3_TrackDecoded : public CSHINETrackReconstructionSimplified
{
public:
  L2L3_TrackDecoded();
  ~L2L3_TrackDecoded();

public:
  virtual void    L2L3_AllModes_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);


private:
  Int_t fgMulti;
  Int_t fSSDgMulti[NUM_CSI];


private:
  Double_t  GetEL1S(Int_t ssdindex, Double_t el1, Double_t el2, Int_t charge, Int_t mass);
  Double_t  GetEL1S(Int_t ssdindex, Double_t el2, Int_t charge, Int_t mass);

  void      FillGlobalEvent_L2L3(CSHINESSDEvent* globalevent, Int_t gmulti, Int_t modex_index,
                                 Int_t ssdindex, Int_t& ssdmulti, Int_t charge, Int_t mass,
                                 Double_t Stripl1s, Double_t El1s, Double_t Stripl2f, Double_t El2f,
                                 Double_t Stripl2b, Double_t El2b, Double_t csiindex, Double_t Ecsi, Int_t Timel2f);

private:
  std::string     MODE_EXP[NUM_SSD];

  Double_t        g2_EL2B_Sum;
  Double_t        g2_EL2F_Sum;
  Double_t        g2_EL1S_Sum;
  Double_t        g2_ChargeCenter;

  Double_t        g3_EL2B_Sum_01, g3_EL2B_Sum_02, g3_EL2B_Sum_12;
  Double_t        g3_EL2F_Sum_01, g3_EL2F_Sum_02, g3_EL2F_Sum_12;
  Double_t        g3_EL1S_Sum_01, g3_EL1S_Sum_02, g3_EL1S_Sum_12;
  Double_t        g3_ChargeCenter_01, g3_ChargeCenter_12;

  Double_t        g4_EL2F_Sum_03, g4_EL2F_Sum_12, g4_EL2F_Sum_01, g4_EL2F_Sum_23;
  Double_t        g4_EL2B_Sum_02, g4_EL2B_Sum_03, g4_EL2B_Sum_13;
  Double_t        g4_ChargeCenter, g4_L2F_ChargeCenter, g4_L2B_ChargeCenter;

  Double_t        g6_EL2F_Sum_12, g6_EL2F_Sum_14, g6_EL2F_Sum_34;

private:
  MBananaCut             fMCut[10];
  std::vector<TCutG*>    BananaCut[NUM_SSD*NUM_CSI]; // for L2L3 track resconstruction
  std::string            pathBananaCut = Form("%sdata_ForPhysicsEvent/L2L3DEECuts.root",PATHDATAFOLDER);

private:
  Double_t          fL1SELoss;
  Double_t          fL2Einc;
  Double_t          fL2EPunchThrough;
  Double_t          fGetEL1S;
  Double_t          fEL2F;
  Double_t          fECsI;
  Double_t          fEtot;
  Double_t          fPID;
  Double_t          fMassd;
  Int_t             fModeID;
  Double_t          fMomentum;
  Double_t          fVelocity;

  const Int_t       fLISEModel = 1;
  const Double_t    fCondition_L2FElossCut = 0.1; // MeV
  const Double_t    fCondition_L2FEincCut  = 0.1; // MeV
  const Double_t    fLISE_Presision        = 0.2; // MeV

private:
  EnergyLossModule                        ELossModule;
  CSHINESiPixelGeometry                   pixel;
  CSHINECsIEnergyCali                     csicali;
  PhysicalCalculator                      physcalc;
  Vector3                                 vechitpoint;
  CSHINEParticleIdentificationSimplified  l2l3pid;

private:
  const Int_t    GMULTI_1 = 1;
  const Int_t    GMULTI_2 = 2;
  const Int_t    GMULTI_3 = 3;
  const Int_t    GMULTI_4 = 4;
  const Int_t    GMULTI_6 = 6;

  // 模式说明:
  // 1.在径迹重建时, 模式定义中每层的多重性从 0 开始, 即 0 表示多重性为 1, 1 表示多重性为 2， 以此类推
  // 2.在存储数据时, mode index 多重性从 1 开始, 即 1 表示多重性为 1， 2 表示多重性为 2, 以此类推.
  // 这样一来可以区分 L2L3 与 L1L2. 因为对于 L1L2 的径迹, CsI层的多重性为 0.

  //
  Bool_t          SWITCH_L2L3_G1 = kTRUE;
  Bool_t          SWITCH_L2L3_G2 = kTRUE;
  Bool_t          SWITCH_L2L3_G3 = kTRUE;
  Bool_t          SWITCH_L2L3_G4 = kTRUE;
  Bool_t          SWITCH_L2L3_G6 = kTRUE;

  // globalmulti = 1
  const Int_t    MODEINDEX_G1      = 1111;

  // globalmulti = 2
  const Int_t    MODEINDEX_G2_0001 = 1112;   const char* MODE_G2_0001 = "0001";   Bool_t SWITCH_MODE_G2_0001 = kTRUE;
  const Int_t    MODEINDEX_G2_0010 = 1121;   const char* MODE_G2_0010 = "0010";   Bool_t SWITCH_MODE_G2_0010 = kTRUE;
  const Int_t    MODEINDEX_G2_0100 = 1211;   const char* MODE_G2_0100 = "0100";   Bool_t SWITCH_MODE_G2_0100 = kTRUE;
  const Int_t    MODEINDEX_G2_0101 = 1212;   const char* MODE_G2_0101 = "0101";   Bool_t SWITCH_MODE_G2_0101 = kTRUE;
  const Int_t    MODEINDEX_G2_1000 = 2111;   const char* MODE_G2_1000 = "1000";   Bool_t SWITCH_MODE_G2_1000 = kTRUE;
  const Int_t    MODEINDEX_G2_1010 = 2121;   const char* MODE_G2_1010 = "1010";   Bool_t SWITCH_MODE_G2_1010 = kTRUE;
  const Int_t    MODEINDEX_G2_1101 = 2212;   const char* MODE_G2_1101 = "1101";   Bool_t SWITCH_MODE_G2_1101 = kTRUE;
  const Int_t    MODEINDEX_G2_1111 = 2222;   const char* MODE_G2_1111 = "1111";   Bool_t SWITCH_MODE_G2_1111 = kTRUE;
  // globalmulti = 3
  const Int_t    MODEINDEX_G3_0002 = 1113;   const char* MODE_G3_0002 = "0002";   Bool_t SWITCH_MODE_G3_0002 = kTRUE;
  const Int_t    MODEINDEX_G3_0020 = 1131;   const char* MODE_G3_0020 = "0020";   Bool_t SWITCH_MODE_G3_0020 = kTRUE;
  const Int_t    MODEINDEX_G3_0101 = 1212;   const char* MODE_G3_0101 = "0101";   Bool_t SWITCH_MODE_G3_0101 = kTRUE;
  const Int_t    MODEINDEX_G3_0102 = 1213;   const char* MODE_G3_0102 = "0102";   Bool_t SWITCH_MODE_G3_0102 = kTRUE;
  const Int_t    MODEINDEX_G3_0201 = 1312;   const char* MODE_G3_0201 = "0201";   Bool_t SWITCH_MODE_G3_0201 = kTRUE;
  const Int_t    MODEINDEX_G3_1010 = 2121;   const char* MODE_G3_1010 = "1010";   Bool_t SWITCH_MODE_G3_1010 = kTRUE;
  const Int_t    MODEINDEX_G3_1020 = 2131;   const char* MODE_G3_1020 = "1020";   Bool_t SWITCH_MODE_G3_1020 = kTRUE;
  const Int_t    MODEINDEX_G3_1110 = 2221;   const char* MODE_G3_1110 = "1110";   Bool_t SWITCH_MODE_G3_1110 = kTRUE;
  const Int_t    MODEINDEX_G3_1111 = 2222;   const char* MODE_G3_1111 = "1111";   Bool_t SWITCH_MODE_G3_1111 = kTRUE;
  const Int_t    MODEINDEX_G3_1112 = 2223;   const char* MODE_G3_1112 = "1112";   Bool_t SWITCH_MODE_G3_1112 = kTRUE;
  const Int_t    MODEINDEX_G3_1121 = 2232;   const char* MODE_G3_1121 = "1121";   Bool_t SWITCH_MODE_G3_1121 = kTRUE;
  const Int_t    MODEINDEX_G3_1211 = 2322;   const char* MODE_G3_1211 = "1211";   Bool_t SWITCH_MODE_G3_1211 = kTRUE;
  const Int_t    MODEINDEX_G3_2111 = 3222;   const char* MODE_G3_2111 = "2111";   Bool_t SWITCH_MODE_G3_2111 = kTRUE;
  const Int_t    MODEINDEX_G3_2222 = 3333;   const char* MODE_G3_2222 = "2222";   Bool_t SWITCH_MODE_G3_2222 = kTRUE;
  // globalmulti = 4
  const Int_t    MODEINDEX_G4_0011 = 1122;   const char* MODE_G4_0011 = "0011";   Bool_t SWITCH_MODE_G4_0011 = kTRUE;
  const Int_t    MODEINDEX_G4_0101 = 1212;   const char* MODE_G4_0101 = "0101";   Bool_t SWITCH_MODE_G4_0101 = kTRUE;
  const Int_t    MODEINDEX_G4_0110 = 1221;   const char* MODE_G4_0110 = "0110";   Bool_t SWITCH_MODE_G4_0110 = kTRUE;
  const Int_t    MODEINDEX_G4_0111 = 1222;   const char* MODE_G4_0111 = "0111";   Bool_t SWITCH_MODE_G4_0111 = kTRUE;
  const Int_t    MODEINDEX_G4_1011 = 2122;   const char* MODE_G4_1011 = "1011";   Bool_t SWITCH_MODE_G4_1011 = kTRUE;
  const Int_t    MODEINDEX_G4_1110 = 2221;   const char* MODE_G4_1110 = "1110";   Bool_t SWITCH_MODE_G4_1110 = kTRUE;
  const Int_t    MODEINDEX_G4_1111 = 2222;   const char* MODE_G4_1111 = "1111";   Bool_t SWITCH_MODE_G4_1111 = kTRUE;
  const Int_t    MODEINDEX_G4_2222 = 3333;   const char* MODE_G4_2222 = "2222";   Bool_t SWITCH_MODE_G4_2222 = kTRUE;
  // globalmulti = 6
  const Int_t    MODEINDEX_G6_0012 = 1123;   const char* MODE_G6_0012 = "0012";   Bool_t SWITCH_MODE_G6_0012 = kTRUE;
  const Int_t    MODEINDEX_G6_0021 = 1132;   const char* MODE_G6_0021 = "0021";   Bool_t SWITCH_MODE_G6_0021 = kTRUE;
  const Int_t    MODEINDEX_G6_0111 = 1222;   const char* MODE_G6_0111 = "0111";   Bool_t SWITCH_MODE_G6_0111 = kTRUE;
  const Int_t    MODEINDEX_G6_0112 = 1223;   const char* MODE_G6_0112 = "0112";   Bool_t SWITCH_MODE_G6_0112 = kTRUE;
  const Int_t    MODEINDEX_G6_0120 = 1231;   const char* MODE_G6_0120 = "0120";   Bool_t SWITCH_MODE_G6_0120 = kTRUE;
  const Int_t    MODEINDEX_G6_0121 = 1232;   const char* MODE_G6_0121 = "0121";   Bool_t SWITCH_MODE_G6_0121 = kTRUE;
  const Int_t    MODEINDEX_G6_0211 = 1322;   const char* MODE_G6_0211 = "0211";   Bool_t SWITCH_MODE_G6_0211 = kTRUE;
  const Int_t    MODEINDEX_G6_1011 = 2122;   const char* MODE_G6_1011 = "1011";   Bool_t SWITCH_MODE_G6_1011 = kTRUE;
  const Int_t    MODEINDEX_G6_1110 = 2221;   const char* MODE_G6_1110 = "1110";   Bool_t SWITCH_MODE_G6_1110 = kTRUE;
  const Int_t    MODEINDEX_G6_1111 = 2222;   const char* MODE_G6_1111 = "1111";   Bool_t SWITCH_MODE_G6_1111 = kTRUE;
  const Int_t    MODEINDEX_G6_1112 = 2223;   const char* MODE_G6_1112 = "1112";   Bool_t SWITCH_MODE_G6_1112 = kTRUE;
  const Int_t    MODEINDEX_G6_1121 = 2232;   const char* MODE_G6_1121 = "1121";   Bool_t SWITCH_MODE_G6_1121 = kTRUE;
  const Int_t    MODEINDEX_G6_1211 = 2322;   const char* MODE_G6_1211 = "1211";   Bool_t SWITCH_MODE_G6_1211 = kTRUE;
  const Int_t    MODEINDEX_G6_2222 = 3333;   const char* MODE_G6_2222 = "2222";   Bool_t SWITCH_MODE_G6_2222 = kTRUE;


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
public:
  virtual void    L2L3_g1_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g2_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g3_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g4_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g6_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);

  //____________________________________
  //  以下是模式分类后的径迹重建算法

  // globalmulti = 2
  virtual void    L2L3_g2_0001_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g2_0010_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g2_0100_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g2_0101_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g2_1000_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g2_1010_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g2_1101_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g2_1111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);

  // globalmulti = 3
  virtual void    L2L3_g3_0002_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g3_0020_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g3_0101_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g3_0102_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g3_0201_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g3_1010_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g3_1020_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g3_1110_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g3_1111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g3_1112_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g3_1121_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g3_1211_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g3_2111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g3_2222_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);

  // globalmulti = 4
  virtual void    L2L3_g4_0011_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g4_0101_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g4_0110_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g4_0111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g4_1011_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g4_1110_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g4_1111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g4_2222_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);

  // globalmulti = 6
  virtual void    L2L3_g6_0012_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g6_0021_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g6_0111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g6_0112_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g6_0120_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g6_0121_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g6_0211_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g6_1011_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g6_1110_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g6_1111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g6_1112_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g6_1121_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g6_1211_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L2L3_g6_2222_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//  L1L2 径迹重建的算法摘自源程序 L1L2TrackFinding.C
//  这里将算法部分摘出来, 简化成单独的函数, 以便后续正式调用
//
//  gfh, 2021-08
//_______________________
class L1L2_TrackDecoded : public CSHINETrackReconstructionSimplified
{
public:
  L1L2_TrackDecoded();
  ~L1L2_TrackDecoded();

private:
  Int_t           fgMulti;
  Int_t           fSSDgMulti[NUM_CSI];
  std::string     MODE_EXP[NUM_SSD];


public:
  void      FillGlobalEvent_L1L2(CSHINESSDEvent* globalevent, Int_t gmulti, Int_t modex_index,
                                 Int_t ssdindex, Int_t& ssdmulti, Int_t charge, Int_t mass,
                                 Double_t Stripl1s, Double_t El1s, Double_t El1s_corrected,
                                 Double_t Stripl2f, Double_t El2f,
                                 Double_t Stripl2b, Double_t El2b, Int_t Timel2f);


private:
  Double_t        g2_EL2B_Sum, g2_EL2F_Sum, g2_EL1S_Sum, g2_EL1S_Sum_Corrected;
  Double_t        g2_ChargeCenter, g2_L2B_ChargeCenter, g2_L1S_ChargeCenter;
  //
  Double_t        g3_EL2F_Sum01,  g3_EL2F_Sum12, g3_EL2B_Sum01, g3_EL2B_Sum02, g3_EL2B_Sum12, g3_EL2B_Sum012;
  Double_t        g3_ChargeCenter;
  //
  Double_t        g4_EL2F_Sum03, g4_EL2F_Sum12, g4_EL1S_Sum03, g4_EL1S_Sum03_Corrected, g4_EL2B_Sum03, g4_EL2B_Sum12;
  Double_t        g4_ChargeCenter, g4_L2B_ChargeCenter, g4_L2F_ChargeCenter;
  //
  Double_t        g6_EL2B_Sum03, g6_EL2B_Sum04, g6_EL2B_Sum15,g6_EL2F_Sum04, g6_EL2F_Sum15;
  Double_t        g6_EL2B_Sum24, g6_EL2B_Sum35, g6_EL2B_Sum02, g6_EL2B_Sum13;
  Double_t        g6_L2B_ChargeCenter, g6_L2F_ChargeCenter;

private:
  MBananaCut             fMCut[10];
  std::vector<TCutG*>    BananaCut[NUM_SSD]; // for L1L2 track resconstruction
  std::string            pathBananaCut = Form("%sdata_ForPhysicsEvent/L1L2DEECuts.root",PATHDATAFOLDER);

private:
  Double_t          fEtot;
  Double_t          fPID;
  Double_t          fMassd;
  Int_t             fModeID;
  Double_t          fMomentum;
  Double_t          fVelocity;

private:
  CSHINESiPixelGeometry                   pixel;
  PhysicalCalculator                      physcalc;
  Vector3                                 vechitpoint;
  CSHINEParticleIdentificationSimplified  l1l2pid;

private:
  const Int_t    GMULTI_1 = 1;
  const Int_t    GMULTI_2 = 2;
  const Int_t    GMULTI_3 = 3;
  const Int_t    GMULTI_4 = 4;
  const Int_t    GMULTI_6 = 6;


  // 模式说明:
  // 1.在径迹重建时, 模式定义中每层的多重性从 0 开始, 即 0 表示多重性为 1, 1 表示多重性为 2， 以此类推
  // 2.在存储数据时, mode index 多重性从 1 开始, 即 1 表示多重性为 1， 2 表示多重性为 2, 以此类推.
  // 这样一来可以区分 L2L3 与 L1L2. 因为对于 L1L2 的径迹, CsI层的多重性为 0.

  //
  Bool_t          SWITCH_L1L2_G1 = kTRUE;
  Bool_t          SWITCH_L1L2_G2 = kTRUE;
  Bool_t          SWITCH_L1L2_G3 = kTRUE;
  Bool_t          SWITCH_L1L2_G4 = kTRUE;
  Bool_t          SWITCH_L1L2_G6 = kTRUE;

  // globalmulti = 1
  const Int_t    MODEINDEX_G1     = 111;

  // globalmulti = 2
  const Int_t    MODEINDEX_G2_001 = 112;   const char* MODE_G2_001 = "001";   Bool_t SWITCH_MODE_G2_001 = kTRUE;
  const Int_t    MODEINDEX_G2_010 = 121;   const char* MODE_G2_010 = "010";   Bool_t SWITCH_MODE_G2_010 = kTRUE;
  const Int_t    MODEINDEX_G2_100 = 211;   const char* MODE_G2_100 = "100";   Bool_t SWITCH_MODE_G2_100 = kTRUE;
  const Int_t    MODEINDEX_G2_101 = 212;   const char* MODE_G2_101 = "101";   Bool_t SWITCH_MODE_G2_101 = kTRUE;

  // globalmulti = 3
  const Int_t    MODEINDEX_G3_002 = 113;   const char* MODE_G3_002 = "002";   Bool_t SWITCH_MODE_G3_002 = kTRUE;
  const Int_t    MODEINDEX_G3_020 = 131;   const char* MODE_G3_020 = "020";   Bool_t SWITCH_MODE_G3_020 = kTRUE;
  const Int_t    MODEINDEX_G3_101 = 212;   const char* MODE_G3_101 = "101";   Bool_t SWITCH_MODE_G3_101 = kTRUE;
  const Int_t    MODEINDEX_G3_102 = 213;   const char* MODE_G3_102 = "102";   Bool_t SWITCH_MODE_G3_102 = kTRUE;
  const Int_t    MODEINDEX_G3_200 = 311;   const char* MODE_G3_200 = "200";   Bool_t SWITCH_MODE_G3_200 = kTRUE;
  const Int_t    MODEINDEX_G3_201 = 312;   const char* MODE_G3_201 = "201";   Bool_t SWITCH_MODE_G3_201 = kTRUE;
  const Int_t    MODEINDEX_G3_202 = 313;   const char* MODE_G3_202 = "202";   Bool_t SWITCH_MODE_G3_202 = kTRUE;

  // globalmulti = 4
  const Int_t    MODEINDEX_G4_011 = 122;   const char* MODE_G4_011 = "011";   Bool_t SWITCH_MODE_G4_011 = kTRUE;
  const Int_t    MODEINDEX_G4_101 = 212;   const char* MODE_G4_101 = "101";   Bool_t SWITCH_MODE_G4_101 = kTRUE;
  const Int_t    MODEINDEX_G4_110 = 221;   const char* MODE_G4_110 = "110";   Bool_t SWITCH_MODE_G4_110 = kTRUE;
  const Int_t    MODEINDEX_G4_111 = 222;   const char* MODE_G4_111 = "111";   Bool_t SWITCH_MODE_G4_111 = kTRUE;

  // globalmulti = 6
  const Int_t    MODEINDEX_G6_111 = 222;   const char* MODE_G6_111 = "111";   Bool_t SWITCH_MODE_G6_111 = kTRUE;
  const Int_t    MODEINDEX_G6_112 = 223;   const char* MODE_G6_112 = "112";   Bool_t SWITCH_MODE_G6_112 = kTRUE;
  const Int_t    MODEINDEX_G6_120 = 231;   const char* MODE_G6_120 = "120";   Bool_t SWITCH_MODE_G6_120 = kTRUE;
  const Int_t    MODEINDEX_G6_121 = 232;   const char* MODE_G6_121 = "121";   Bool_t SWITCH_MODE_G6_121 = kTRUE;
  const Int_t    MODEINDEX_G6_211 = 322;   const char* MODE_G6_211 = "211";   Bool_t SWITCH_MODE_G6_211 = kTRUE;

//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
public:
  virtual void    L1L2_g1_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g2_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g3_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g4_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g6_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);


  //____________________________________
  //  以下是模式分类后的径迹重建算法

  // globalmulti = 2
  virtual void    L1L2_g2_001_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g2_010_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g2_100_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g2_101_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);

  // globalmulti = 3
  virtual void    L1L2_g3_002_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g3_020_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g3_101_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g3_102_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g3_200_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g3_201_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g3_202_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);

  // globalmulti = 4
  virtual void    L1L2_g4_011_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g4_101_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g4_110_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g4_111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);

  // globalmulti = 6
  virtual void    L1L2_g6_111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g6_112_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g6_120_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g6_121_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
  virtual void    L1L2_g6_211_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting);
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo

#endif
