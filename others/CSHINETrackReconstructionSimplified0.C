#include "../include/CSHINETrackReconstructionSimplified.h"
using namespace std;


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//  将径迹重建实际用到的功能单独写在这里, 因为 CSHINETrackReconstruction 比较甬长不利于阅读
//  CSHINETrackReconstructionSimplified 的所有成员函数都是从 CSHINETrackReconstruction
//  中直接复制过来的, 没有作任何修改！
//
//  gfh, 2021-08
//______________________________________________________________________________
CSHINETrackReconstructionSimplified::CSHINETrackReconstructionSimplified()
{}


CSHINETrackReconstructionSimplified::~CSHINETrackReconstructionSimplified()
{}
//______________________________________________________________________________

//______________________________________________________________________________
Bool_t CSHINETrackReconstructionSimplified::IsGeoConstraint_L3A_L2B(Int_t csiindex, Int_t  stripl2b)
{
  Bool_t index = false;
  if ((csiindex/3)==0 && (stripl2b>=10 && stripl2b<=15)) index = true; // csiindex = 0, 1, 2
  if ((csiindex/3)==1 && (stripl2b>=5  && stripl2b<=10)) index = true; // csiindex = 3, 4, 5
  if ((csiindex/3)==2 && (stripl2b>=0  && stripl2b<=5))  index = true; // csiindex = 6, 7, 8
  return (index==true) ? true : false;
}

//______________________________________________________________________________
Bool_t CSHINETrackReconstructionSimplified::IsGeoConstraint_L3A_L2F(Int_t csiindex, Int_t stripl2f)
{
  Bool_t index = false;
  if ((csiindex%3)==0 && (stripl2f>=10 && stripl2f<=15)) index = true; // csiindex = 0, 3, 6
  if ((csiindex%3)==1 && (stripl2f>=5  && stripl2f<=10)) index = true; // csiindex = 1, 4, 7
  if ((csiindex%3)==2 && (stripl2f>=0  && stripl2f<=5))  index = true; // csiindex = 2, 5, 8
  return (index==true) ? true : false;
}

//______________________________________________________________________________
Bool_t CSHINETrackReconstructionSimplified::IsEneConstraint_L2B_L2F(Int_t ssdindex, Double_t El2b, Double_t El2f)
{
  // LowErrRatio 为负数, highErrRatio 为正数
  if (El2f<L2BL2F_ENERGYBOUNDARY[ssdindex]) {
    return ((El2b-El2f)/El2f>L2BL2F_LOWENERGYLOWCUT[ssdindex] && (El2b-El2f)/El2f<L2BL2F_LOWENERGYHIGHCUT[ssdindex]);
  }
  else {
    return ((El2b-El2f)/El2f>L2BL2F_HIGHENERGYLOWCUT[ssdindex] && (El2b-El2f)/El2f<L2BL2F_HIGHENERGYHIGHCUT[ssdindex]);
  }
}

//______________________________________________________________________________
Bool_t CSHINETrackReconstructionSimplified::IsGeoConstraint_L2B_L1S(Int_t stripl2b, Int_t stripl1s)
{
  return (TMath::Abs(stripl2b - stripl1s)<=1);
}

//_
Bool_t CSHINETrackReconstructionSimplified::IsEneConstraint_L1S_L2F(Int_t ssdindex, Double_t El1s, Double_t El2f)
{
  return ((El1s/El2f>L1L2_ENERGYLOWCUT[ssdindex]) && (El1s/El2f<L1L2_ENERGYHIGHCUT[ssdindex]));
}

//______________________________________________________________________________
Bool_t CSHINETrackReconstructionSimplified::IsInsideABananaCut(std::vector<TCutG*> cut, MBananaCut& mcut, Double_t e2, Double_t de1)
{
  Bool_t index = kFALSE;
  for (Int_t particle=0; particle<cut.size(); particle++) {
    if (cut[particle]->IsInside(e2, de1)) {
      index = kTRUE;
      mcut.CutName = cut[particle]->GetName();
      GetZAFromBananaCutName(mcut.CutName,mcut.Cut_Z,mcut.Cut_A);
      break;
    }
  }
  return index;
}

//______________________________________________________________________________
// 根据 L2F, L2B 的位置, 确定对应的 CsI 位置
Int_t CSHINETrackReconstructionSimplified::GetCsINumFromPixel(Int_t stripl2f, Int_t stripl2b)
{
  Int_t csinum = -99;

  if (0<=stripl2f && stripl2f<5) { // CsI 2,5,8
    if (0<=stripl2b && stripl2b<5)        csinum = 8;
    else if (5<=stripl2b && stripl2b<=10) csinum = 5;
    else                                  csinum = 2;
  }
  else if (5<=stripl2f && stripl2f<=10) { // CsI 1,4,7
    if (0<=stripl2b && stripl2b<5)        csinum = 7;
    else if (5<=stripl2b && stripl2b<=10) csinum = 4;
    else                                  csinum = 1;
  }
  else { // CsI 0,3,6
    if (0<=stripl2b && stripl2b<5)        csinum = 6;
    else if (5<=stripl2b && stripl2b<=10) csinum = 3;
    else                                  csinum = 0;
  }
  return csinum;
}

//______________________________________________________________________________
void CSHINETrackReconstructionSimplified::GetZAFromBananaCutName(std::string cutname, Int_t& Z, Int_t& A)
{
  Int_t Z1, Z2, A1, A2;
  Int_t len = std::strlen(cutname.c_str());

  A2 = cutname[len-1] - '0';
  A1 = cutname[len-2] - '0';

  Z2 = cutname[len-5] - '0';
  Z1 = cutname[len-6] - '0';

  Z = Z1*10 + Z2;
  A = A1*10 + A2;
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//  L2L3 径迹重建的算法摘自源程序 L2L3TrackFinding.C
//  这里将算法部分摘出来, 简化成单独的函数, 以便后续正式调用
//
//  gfh, 2021-08
//______________________________________________________________________________
L2L3_TrackDecoded::L2L3_TrackDecoded()
{
  std::string pathBananaCut(Form("%sStraighteningData/L2L3DEECuts.root",PATHROOTFILESFOLDER));
  TFile* myfile = TFile::Open(pathBananaCut.c_str());
  // 读取 bananacut
  for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
    TIter next(myfile->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*) next())) {
      TCutG* cut = (TCutG*) key->ReadObj();
      std::string cutname(cut->GetName());

      if (strstr(cutname.c_str(), Form("_Tel%02d_",numtel)) != NULL) {
        BananaCut[numtel].push_back(cut);
      //cout<<cut->GetName()<<endl;
      }
    }
  }
}

L2L3_TrackDecoded::~L2L3_TrackDecoded()
{}

//______________________________________________________________________________
// ModeID 定义： 用一个五位整数表示  abcde
//  a    : a=gloablmulti, 即只满足几何条件得到的候选径迹数
//  bcde : b,c,d,e, 分别表示 L3A, L2B, L2F, L1A 的多重性
//
Int_t L2L3_TrackDecoded::GetModeID(Int_t gmulti, Int_t mode_index) const
{
  return (gmulti*pow(10,4) + mode_index);
}
//______________________________________________________________________________


//______________________________________________________________________________
// 对于 L2L3 的径迹重建来说, EL1S 需要先判断是否满足 IsEneConstraint_L1S_L2F
// 1) 若满足, 则 EL1S 直接使用实验数据
// 2) 若不满足, 则使用 LISE++ 反推 EL1S
Double_t L2L3_TrackDecoded::GetEL1S(Int_t ssdindex, Double_t el1, Double_t el2, Int_t charge, Int_t mass)
{
  Double_t EL1S = 0.;
  // 如果满足 L1L2 能量约束, 直接取 EL1S
  if (IsEneConstraint_L1S_L2F(ssdindex,el1,el2)) {
    EL1S = el1;
  }
  else { // 否则, 使用LISE++工具计算 EL1S
    if (el2>fCondition_L2FElossCut) {
      fL2Einc = ELossModule.GetIncidentEnergy(charge,mass,el2,"Si",SIL2THICKNESS[ssdindex]*1000,fLISE_Presision,fLISEModel);
      if (fL2Einc > fCondition_L2FEincCut) {
        EL1S = ELossModule.GetEnergyLossFromResidualEnergy(charge,mass,fL2Einc,"Si",SIL1THICKNESS[ssdindex]*1000,fLISE_Presision,fLISEModel);
      }
    }
  }
  return EL1S;
}
//______________________________________________________________________________

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g1_TrackDecoed(CSHINEGlobalEvent& globalevent, CSHINETrackEvent2 trackevent, Bool_t switchsetting)
{
  if (switchsetting == kFALSE) return; // 关闭 L2L3_g1_TrackDecoed 通道

  const Int_t mode_index = 1111;

  fgMulti = 0;
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    if (trackevent.fCsINum[ssdindex].size() == globalmulti1)
    {
      fSSDgMulti[ssdindex] = 0;
      //________________________________________________________________________
      //               L2L3-globalomulti1 的径迹重建策略
      //            ------------------------------------
      if (IsEneConstraint_L2B_L2F(ssdindex,trackevent.fL2BEMeV[ssdindex][0],trackevent.fL2FEMeV[ssdindex][0]) &&
          IsInsideABananaCut(BananaCut[trackevent.fCutTelNum[ssdindex][0]],fMCut[0],trackevent.fCsIECh[ssdindex][0],trackevent.fL2FEMeV[ssdindex][0]))
      {
        fGetEL1S    = GetEL1S(ssdindex,trackevent.fL1SEMeV[ssdindex][0],trackevent.fL2FEMeV[ssdindex][0],fMCut[0].Cut_Z,fMCut[0].Cut_A);
        fEL2F       = trackevent.fL2FEMeV[ssdindex][0];
        fECsI       = csicali.CsICaliChToMeV(ssdindex,trackevent.fCsINum[ssdindex][0],fMCut[0].Cut_Z,fMCut[0].Cut_A,trackevent.fCsIECh[ssdindex][0]);
        fEtot       = fGetEL1S + fEL2F + fECsI;
        fPID        = l2l3pid.L2L3_GetPIDNumFromExpData(ssdindex,trackevent.fCsINum[ssdindex][0],trackevent.fCsIECh[ssdindex][0],fEL2F,fMCut[0].Cut_Z,fMCut[0].Cut_A);
        fMassd      = l2l3pid.GetMassFromPIDNum(fPID,fMCut[0].Cut_Z);
        vechitpoint = pixel.StripNumToPosInLabFrame(ssdindex,trackevent.fL2BNumStrip[ssdindex][0],trackevent.fL2FNumStrip[ssdindex][0]);
        fModeID     = GetModeID(globalmulti1,mode_index);

				fSSDgMulti[ssdindex]++;
        globalevent.fGSSDNum.push_back(ssdindex);
        globalevent.fGL1SNumStrip.push_back(trackevent.fL1SNumStrip[ssdindex][0]);
        globalevent.fGL1SEMeV.push_back(fGetEL1S);
        globalevent.fGL2FNumStrip.push_back(trackevent.fL2FNumStrip[ssdindex][0]);
        globalevent.fGL2FEMeV.push_back(trackevent.fL2FEMeV[ssdindex][0]);
        globalevent.fGL2BNumStrip.push_back(trackevent.fL2BNumStrip[ssdindex][0]);
        globalevent.fGL2BEMeV.push_back(trackevent.fL2BEMeV[ssdindex][0]);
        globalevent.fGCsINum.push_back(trackevent.fCsINum[ssdindex][0]);
        globalevent.fGCsIEMeV.push_back(fECsI);
        globalevent.fGL2FTime.push_back(trackevent.fL2FTime[ssdindex][0]);
        globalevent.fRadius.push_back(vechitpoint.R());
				globalevent.fTheta.push_back(vechitpoint.ThetaDeg());
				globalevent.fPhi.push_back(vechitpoint.PhiDeg());
				globalevent.fTotEkinc.push_back(fEtot);
				globalevent.fP.push_back(physcalc.GetMomentumValue(fEtot,fMCut[0].Cut_Z,fMCut[0].Cut_A));
				globalevent.fBeta.push_back(physcalc.GetBeta(fEtot,fMCut[0].Cut_Z,fMCut[0].Cut_A));
				globalevent.fZ.push_back(fMCut[0].Cut_Z);
        globalevent.fA.push_back(fMCut[0].Cut_A);
        globalevent.fAd.push_back(fMassd);
        globalevent.fPIDNum.push_back(fPID);
        globalevent.fTrackModeID.push_back(fModeID);
      }
      //
  		fgMulti += fSSDgMulti[ssdindex];
  		globalevent.fSSDGlobalMulti.push_back(fSSDgMulti[ssdindex]);
    }
  }
  //
 	globalevent.fGlobalMulti = fgMulti;
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_TrackDecoed()
{}


//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_TrackDecoed()
{}


//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_TrackDecoed()
{}


//______________________________________________________________________________
//            L2L3_g2_TrackDecoed
//          ------------------------
void L2L3_TrackDecoded::L2L3_g2_0001_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_0010_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_0100_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_0101_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_1000_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_1010_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_1101_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_1111_TrackDecoed()
{}


//______________________________________________________________________________
//             L2L3_g3_TrackDecoed
//          ------------------------
//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_0002_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_0020_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_0101_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_0102_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_0201_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_1010_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_1020_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_1110_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_1111_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_1112_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_1121_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_1211_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_2111_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_2222_TrackDecoed()
{}



//______________________________________________________________________________
//             L2L3_g4_TrackDecoed
//          ------------------------
//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_0011_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_0101_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_0110_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_0111_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_1011_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_1110_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_1111_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_2222_TrackDecoed()
{}



//______________________________________________________________________________
//             L2L3_g6_TrackDecoed
//          ------------------------
//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_0012_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_0021_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_0111_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_0112_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_0120_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_0121_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_0211_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_1011_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_1110_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_1111_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_1112_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_1121_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_1211_TrackDecoed()
{}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_2222_TrackDecoed()
{}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo



//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//  L1L2 径迹重建的算法摘自源程序 L1L2TrackFinding.C
//  这里将算法部分摘出来, 简化成单独的函数, 以便后续正式调用
//
//  gfh, 2021-08
//______________________________________________________________________________
L1L2_TrackDecoded::L1L2_TrackDecoded()
{}

L1L2_TrackDecoded::~L1L2_TrackDecoded()
{}
//______________________________________________________________________________

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g1_TrackDecoed()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g2_TrackDecoed()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g3_TrackDecoed()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g4_TrackDecoed()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g6_TrackDecoed()
{}


//______________________________________________________________________________
//            L1L2_g2_TrackDecoed
//          ------------------------
void L1L2_TrackDecoded::L1L2_g2_001_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g2_010_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g2_100_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g2_101_TrackDecoded()
{}


//______________________________________________________________________________
//            L1L2_g3_TrackDecoed
//          ------------------------
void L1L2_TrackDecoded::L1L2_g3_002_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g3_020_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g3_101_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g3_102_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g3_200_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g3_201_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g3_202_TrackDecoded()
{}



//______________________________________________________________________________
//            L1L2_g4_TrackDecoed
//          ------------------------
void L1L2_TrackDecoded::L1L2_g4_011_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g4_101_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g4_110_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g4_111_TrackDecoded()
{}



//______________________________________________________________________________
//            L1L2_g6_TrackDecoed
//          ------------------------
void L1L2_TrackDecoded::L1L2_g6_111_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g6_112_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g6_120_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g6_121_TrackDecoded()
{}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g6_211_TrackDecoded()
{}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
