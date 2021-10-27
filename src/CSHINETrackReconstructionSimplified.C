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

//______________________________________________________________________________
Double_t CSHINETrackReconstructionSimplified::GetChargeCenter(Double_t strip1, Double_t strip2, Double_t e1, Double_t e2)
{
  Double_t chargecenter = 0;
  if (e1>0 && e2>0) {
    chargecenter = e1/(e1+e2)*strip1 + e2/(e1+e2)*strip2;
  }
  return chargecenter;
}

//______________________________________________________________________________
// ModeID 定义： 用一个五位整数表示  abcde
//  a    : a=gloablmulti, 即只满足几何条件得到的候选径迹数
//  bcde : b,c,d,e, 分别表示 L3A, L2B, L2F, L1A 的多重性
//
Int_t CSHINETrackReconstructionSimplified::GetModeID(Int_t gmulti, Int_t mode_index) const
{
  return (gmulti*pow(10,4) + mode_index);
}

//______________________________________________________________________________
string CSHINETrackReconstructionSimplified::L2L3_CalcModeFromExpData(Int_t globalmultti, vector<Double_t> mcsi, vector<Double_t> ml2b,
  vector<Double_t> ml2f, vector<Double_t> ml1s)
{
  if (globalmultti<2) { cout<<"globalmulti<2, no need for mode classification!"<<endl; return "error"; }
  if (globalmultti != mcsi.size()) { cout<<"Error: globalmutli != ecsi.size()"<<endl; return "error"; }

  string mode_here;
  Int_t candimulti_csi=0, candimulti_l2b=0, candimulti_l2f=0, candimulti_l1s=0;

  sort(mcsi.begin(), mcsi.end());
  sort(ml2b.begin(), ml2b.end());
  sort(ml2f.begin(), ml2f.end());
  sort(ml1s.begin(), ml1s.end());

  // 判断 CsI 中的径迹点
  for (Int_t itrackcsi=0; itrackcsi<(globalmultti-1); itrackcsi++) {
    if (mcsi[itrackcsi+1] != mcsi[itrackcsi]) candimulti_csi++;
  }
  // 判断 L2B 中的径迹点
  for (Int_t itrackl2b=0; itrackl2b<(globalmultti-1); itrackl2b++) {
    if (ml2b[itrackl2b+1] != ml2b[itrackl2b]) candimulti_l2b++;
  }
  // 判断 L2F 中的径迹点
  for (Int_t itrackl2f=0; itrackl2f<(globalmultti-1); itrackl2f++) {
    if (ml2f[itrackl2f+1] != ml2f[itrackl2f]) candimulti_l2f++;
  }
  // 判断 L1S 中的径迹点
  for (Int_t itrackl1s=0; itrackl1s<(globalmultti-1); itrackl1s++) {
    if (ml1s[itrackl1s+1] != ml1s[itrackl1s]) candimulti_l1s++;
  }

  mode_here = Form("%d%d%d%d",candimulti_csi,candimulti_l2b,candimulti_l2f,candimulti_l1s);
  return mode_here;
}


//______________________________________________________________________________
string CSHINETrackReconstructionSimplified::L1L2_CalcModeFromExpData(Int_t globalmultti, vector<Double_t> ml2b, vector<Double_t> ml2f, vector<Double_t> ml1s)
{
  if (globalmultti<2) { cout<<"globalmulti<2, no need for mode classification!"<<endl; return "error"; }
  if (globalmultti != ml2b.size()) { cout<<"Error: globalmutli != ml2b.size()"<<endl; return "error"; }

  string mode_exp;
  Int_t candimulti_l2b=0, candimulti_l2f=0, candimulti_l1s=0;

  sort(ml2b.begin(), ml2b.end());
  sort(ml2f.begin(), ml2f.end());
  sort(ml1s.begin(), ml1s.end());

  // 判断 L2B 中的径迹点
  for (Int_t itrackl2b=0; itrackl2b<(globalmultti-1); itrackl2b++) {
    if (ml2b[itrackl2b+1] != ml2b[itrackl2b]) candimulti_l2b++;
  }
  // 判断 L2F 中的径迹点
  for (Int_t itrackl2f=0; itrackl2f<(globalmultti-1); itrackl2f++) {
    if (ml2f[itrackl2f+1] != ml2f[itrackl2f]) candimulti_l2f++;
  }
  // 判断 L1S 中的径迹点
  for (Int_t itrackl1s=0; itrackl1s<(globalmultti-1); itrackl1s++) {
    if (ml1s[itrackl1s+1] != ml1s[itrackl1s]) candimulti_l1s++;
  }

  mode_exp = Form("%d%d%d",candimulti_l2b,candimulti_l2f,candimulti_l1s);
  return mode_exp;
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
    if (el2 > fCondition_L2FElossCut) {
      fL2Einc = ELossModule.GetIncidentEnergy(charge,mass,el2,"Si",SIL2THICKNESS[ssdindex]*1000,fLISE_Presision,fLISEModel);
      if (fL2Einc > el2) {
        EL1S = ELossModule.GetEnergyLossFromResidualEnergy(charge,mass,fL2Einc,"Si",SIL1THICKNESS[ssdindex]*1000,fLISE_Presision,fLISEModel);
      }
    }
  }
  return EL1S;
}

Double_t L2L3_TrackDecoded::GetEL1S(Int_t ssdindex, Double_t el2, Int_t charge, Int_t mass)
{
  Double_t EL1S = 0.;

  // 对于 L1S 双击的事件, 直接使用LISE++工具计算 EL1S
  if (el2 > fCondition_L2FElossCut) {
    fL2Einc = ELossModule.GetIncidentEnergy(charge,mass,el2,"Si",SIL2THICKNESS[ssdindex]*1000,fLISE_Presision,fLISEModel);
    if (fL2Einc > el2) {
      EL1S = ELossModule.GetEnergyLossFromResidualEnergy(charge,mass,fL2Einc,"Si",SIL1THICKNESS[ssdindex]*1000,fLISE_Presision,fLISEModel);
    }
  }

  return EL1S;
}
//______________________________________________________________________________

//______________________________________________________________________________
void  L2L3_TrackDecoded::FillGlobalEvent_L2L3(CSHINESSDEvent* globalevent, Int_t gmulti, Int_t mode_index,
  Int_t ssdindex, Int_t& ssdmulti, Int_t charge, Int_t mass, Double_t Stripl1s, Double_t El1s,
  Double_t Stripl2f, Double_t El2f, Double_t Stripl2b, Double_t El2b, Double_t csiindex, Double_t Ecsi, Int_t Timel2f)
{
  fGetEL1S    = GetEL1S(ssdindex,El1s,El2f,charge,mass);
  fEL2F       = El2f;
  fECsI       = csicali.CsICaliChToMeV(ssdindex,csiindex,charge,mass,Ecsi);
  fEtot       = fGetEL1S + fEL2F + fECsI;
  fMomentum   = physcalc.GetMomentumValue(fEtot,charge,mass);
  fVelocity   = physcalc.GetBeta(fEtot,charge,mass);
  fPID        = l2l3pid.L2L3_GetPIDNumFromExpData(ssdindex,csiindex,Ecsi,fEL2F,charge,mass);
  fMassd      = l2l3pid.GetMassFromPIDNum(fPID,charge);
  vechitpoint = pixel.StripNumToPosInLabFrame(ssdindex,Stripl2b,Stripl2f);
  fModeID     = GetModeID(gmulti,mode_index);

	ssdmulti++;
  globalevent->fGSSDNum.push_back(ssdindex);
  globalevent->fGL1SNumStrip.push_back(Stripl1s);
  globalevent->fGL1SEMeV.push_back(fGetEL1S);
  globalevent->fGL1SEMeVCorrected.push_back(fGetEL1S);
  globalevent->fGL2FNumStrip.push_back(Stripl2f);
  globalevent->fGL2FEMeV.push_back(El2f);
  globalevent->fGL2BNumStrip.push_back(Stripl2b);
  globalevent->fGL2BEMeV.push_back(El2b);
  globalevent->fGCsINum.push_back(csiindex);
  globalevent->fGCsIEMeV.push_back(fECsI);
  globalevent->fGL2FTime.push_back(Timel2f);
  globalevent->fDist.push_back(vechitpoint.R());
	globalevent->fTheta.push_back(vechitpoint.ThetaDeg());
	globalevent->fPhi.push_back(vechitpoint.PhiDeg());
	globalevent->fTotEkinc.push_back(fEtot);
	globalevent->fP.push_back(fMomentum);
	globalevent->fBeta.push_back(fVelocity);
	globalevent->fZ.push_back(charge);
  globalevent->fA.push_back(mass);
  globalevent->fAd.push_back(fMassd);
  globalevent->fPIDNum.push_back(fPID);
  globalevent->fTrackModeID.push_back(fModeID);
}
//______________________________________________________________________________


//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_AllModes_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭 L2L3_TrackDecoded 通道
  if (switchsetting == kFALSE) return;

  L2L3_g1_TrackDecoded(globalevent, trackevent, SWITCH_L2L3_G1);
  L2L3_g2_TrackDecoded(globalevent, trackevent, SWITCH_L2L3_G2);
  L2L3_g3_TrackDecoded(globalevent, trackevent, SWITCH_L2L3_G3);
  L2L3_g4_TrackDecoded(globalevent, trackevent, SWITCH_L2L3_G4);
  L2L3_g6_TrackDecoded(globalevent, trackevent, SWITCH_L2L3_G6);
}


//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g1_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭 L2L3_g1_TrackDecoded 通道
  if (switchsetting == kFALSE) return;

  fgMulti = 0;
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    if (trackevent->fCsINum[ssdindex].size() == GMULTI_1)
    {
      fSSDgMulti[ssdindex] = 0;

      if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
          IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]))
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_1, MODEINDEX_G1, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                             trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0],
                             trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                             trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                             trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                             trackevent->fL2FTime[ssdindex][0]);
      }
      //
  		fgMulti += fSSDgMulti[ssdindex];
  		globalevent->fSSDGlobalMulti.push_back(fSSDgMulti[ssdindex]);
    }
  }
  //
 	globalevent->fGlobalMulti = fgMulti;
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭 L2L3_g2_TrackDecoded 通道
  if (switchsetting == kFALSE) return;

  fgMulti = 0;
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    fSSDgMulti[ssdindex] = 0;
    //
    if (trackevent->fCsINum[ssdindex].size() == GMULTI_2)
    {
      MODE_EXP[ssdindex] = L2L3_CalcModeFromExpData(GMULTI_2,trackevent->fCsINum[ssdindex],trackevent->fL2BNumStrip[ssdindex],trackevent->fL2FNumStrip[ssdindex],trackevent->fL1SNumStrip[ssdindex]);
      L2L3_g2_0001_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G2_0001);
      L2L3_g2_0010_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G2_0010);
      L2L3_g2_0100_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G2_0100);
      L2L3_g2_0101_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G2_0101);
      L2L3_g2_1000_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G2_1000);
      L2L3_g2_1010_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G2_1010);
      L2L3_g2_1101_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G2_1101);
      L2L3_g2_1111_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G2_1111);
    }
    //
    fgMulti += fSSDgMulti[ssdindex];
    globalevent->fSSDGlobalMulti.push_back(fSSDgMulti[ssdindex]);
  }
  //
 	globalevent->fGlobalMulti = fgMulti;
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭 L2L3_g3_TrackDecoded 通道
  if (switchsetting == kFALSE) return;

  fgMulti = 0;
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    fSSDgMulti[ssdindex] = 0;
    //
    if (trackevent->fCsINum[ssdindex].size() == GMULTI_3)
    {
      MODE_EXP[ssdindex] = L2L3_CalcModeFromExpData(GMULTI_3,trackevent->fCsINum[ssdindex],trackevent->fL2BNumStrip[ssdindex],trackevent->fL2FNumStrip[ssdindex],trackevent->fL1SNumStrip[ssdindex]);
      L2L3_g3_0002_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_0002);
      L2L3_g3_0020_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_0020);
      L2L3_g3_0101_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_0101);
      L2L3_g3_0102_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_0102);
      L2L3_g3_0201_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_0201);
      L2L3_g3_1010_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_1010);
      L2L3_g3_1020_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_1020);
      L2L3_g3_1110_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_1110);
      L2L3_g3_1111_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_1111);
      L2L3_g3_1112_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_1112);
      L2L3_g3_1121_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_1121);
      L2L3_g3_1211_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_1211);
      L2L3_g3_2111_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_2111);
      L2L3_g3_2222_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_2222);
    }
    //
    fgMulti += fSSDgMulti[ssdindex];
    globalevent->fSSDGlobalMulti.push_back(fSSDgMulti[ssdindex]);
  }
  //
 	globalevent->fGlobalMulti = fgMulti;
}


//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭 L2L3_g2_TrackDecoded 通道
  if (switchsetting == kFALSE) return;

  fgMulti = 0;
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    fSSDgMulti[ssdindex] = 0;
    //
    if (trackevent->fCsINum[ssdindex].size() == GMULTI_4)
    {
      MODE_EXP[ssdindex] = L2L3_CalcModeFromExpData(GMULTI_4,trackevent->fCsINum[ssdindex],trackevent->fL2BNumStrip[ssdindex],trackevent->fL2FNumStrip[ssdindex],trackevent->fL1SNumStrip[ssdindex]);
      L2L3_g4_0011_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G4_0011);
      L2L3_g4_0101_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G4_0101);
      L2L3_g4_0110_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G4_0110);
      L2L3_g4_0111_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G4_0111);
      L2L3_g4_1011_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G4_1011);
      L2L3_g4_1110_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G4_1110);
      L2L3_g4_1111_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G4_1111);
      L2L3_g4_2222_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G4_2222);
    }
    //
    fgMulti += fSSDgMulti[ssdindex];
    globalevent->fSSDGlobalMulti.push_back(fSSDgMulti[ssdindex]);
  }
  //
 	globalevent->fGlobalMulti = fgMulti;
}


//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭 L2L3_g2_TrackDecoded 通道
  if (switchsetting == kFALSE) return;

  fgMulti = 0;
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    fSSDgMulti[ssdindex] = 0;
    //
    if (trackevent->fCsINum[ssdindex].size() == GMULTI_6)
    {
      MODE_EXP[ssdindex] = L2L3_CalcModeFromExpData(GMULTI_6,trackevent->fCsINum[ssdindex],trackevent->fL2BNumStrip[ssdindex],trackevent->fL2FNumStrip[ssdindex],trackevent->fL1SNumStrip[ssdindex]);
      L2L3_g6_0012_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_0012);
      L2L3_g6_0021_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_0021);
      L2L3_g6_0111_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_0111);
      L2L3_g6_0112_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_0112);
      L2L3_g6_0120_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_0120);
      L2L3_g6_0121_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_0121);
      L2L3_g6_0211_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_0211);
      L2L3_g6_1011_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_1011);
      L2L3_g6_1110_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_1110);
      L2L3_g6_1111_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_1111);
      L2L3_g6_1112_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_1112);
      L2L3_g6_1121_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_1121);
      L2L3_g6_1211_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_1211);
      L2L3_g6_2222_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_2222);
    }
    //
    fgMulti += fSSDgMulti[ssdindex];
    globalevent->fSSDGlobalMulti.push_back(fSSDgMulti[ssdindex]);
  }
  //
 	globalevent->fGlobalMulti = fgMulti;
}


//______________________________________________________________________________
//            L2L3_g2_TrackDecoded
//          ------------------------
void L2L3_TrackDecoded::L2L3_g2_0001_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_2) && (strcmp(mode,MODE_G2_0001)==0))
  {
    g2_EL1S_Sum = trackevent->fL1SEMeV[ssdindex][0] + trackevent->fL1SEMeV[ssdindex][1];
    //
    if (IsEneConstraint_L2B_L2F(ssdindex, trackevent->fL2BEMeV[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0]) &&
        IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]))
    {
      // 考虑 L1S charge sharing
      if ((TMath::Abs(trackevent->fL1SNumStrip[ssdindex][0]-trackevent->fL1SNumStrip[ssdindex][1])==1) &&
          IsEneConstraint_L1S_L2F(ssdindex, g2_EL1S_Sum, trackevent->fL2FEMeV[ssdindex][0]))
      {
        g2_ChargeCenter = GetChargeCenter(trackevent->fL1SNumStrip[ssdindex][0],trackevent->fL1SNumStrip[ssdindex][1],trackevent->fL1SEMeV[ssdindex][0],trackevent->fL1SEMeV[ssdindex][1]);
        FillGlobalEvent_L2L3(globalevent, GMULTI_2, MODEINDEX_G2_0001, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                             g2_ChargeCenter,                       g2_EL1S_Sum,
                             trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                             trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                             trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                             trackevent->fL2FTime[ssdindex][0]);
      }
      else // 0)1)为同一条径迹,只考虑第0)条候选径迹
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_2, MODEINDEX_G2_0001, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                             trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0],
                             trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                             trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                             trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                             trackevent->fL2FTime[ssdindex][0]);
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_0010_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_2) && (strcmp(mode,MODE_G2_0010)==0))
  {
    g2_EL2F_Sum = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][1];
    // 考虑 L2F sharing, 电荷重心法计算击中位置
    if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][1])==1) &&
         IsEneConstraint_L2B_L2F(ssdindex, trackevent->fL2BEMeV[ssdindex][0], g2_EL2F_Sum) &&
         IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],g2_EL2F_Sum))
    {
      g2_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][1],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][1]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_2, MODEINDEX_G2_0010, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0],
                      g2_ChargeCenter,                       g2_EL2F_Sum,
                      trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                      trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                      trackevent->fL2FTime[ssdindex][0]);
    }
    else // 考虑第 0）条或第 1）条候选径迹为有效径迹
    {
      for (Int_t itrack=0; itrack<GMULTI_2; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_2, MODEINDEX_G2_0010, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_0100_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_2) && (strcmp(mode,MODE_G2_0100)==0))
  {
    g2_EL2B_Sum = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][1];
    // 考虑 L2B sharing, 电荷重心法计算击中位置
    if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][1])==1) &&
         IsEneConstraint_L2B_L2F(ssdindex, g2_EL2B_Sum, trackevent->fL2FEMeV[ssdindex][0]) &&
         IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]))
    {
      g2_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][1]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_2, MODEINDEX_G2_0100, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0],
                           trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                           g2_ChargeCenter,                       g2_EL2B_Sum,
                           trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                           trackevent->fL2FTime[ssdindex][0]);
    }
    else // 考虑第 0）条或第 1）条候选径迹为有效径迹
    {
      for (Int_t itrack=0; itrack<GMULTI_2; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_2, MODEINDEX_G2_0100, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_0101_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_2) && (strcmp(mode,MODE_G2_0101)==0))
  {
    g2_EL2B_Sum = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][1];
    // 考虑 L2B sharing, 电荷重心法计算击中位置
    if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][1])==1) &&
         IsEneConstraint_L2B_L2F(ssdindex, g2_EL2B_Sum, trackevent->fL2FEMeV[ssdindex][0]) &&
         IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]))
    {
      g2_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][1]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_2, MODEINDEX_G2_0101, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0],
                           trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                           g2_ChargeCenter,                       g2_EL2B_Sum,
                           trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                           trackevent->fL2FTime[ssdindex][0]);
    }
    else // 考虑第 0）条或第 1）条候选径迹为有效径迹
    {
      for (Int_t itrack=0; itrack<GMULTI_2; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_2, MODEINDEX_G2_0101, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_1000_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_2) && (strcmp(mode,MODE_G2_1000)==0))
  { // 考虑第 0）条或第 1）条候选径迹为有效径迹
    for (Int_t itrack=0; itrack<GMULTI_2; itrack++)
    {
      if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
          IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_2, MODEINDEX_G2_1000, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                        trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                        trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                        trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                        trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                        trackevent->fL2FTime[ssdindex][itrack]);
        break;
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_1010_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_2) && (strcmp(mode,MODE_G2_1010)==0))
  {
    g2_EL2F_Sum = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][1];
    // 考虑 L2B 双击
    if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],g2_EL2F_Sum))
    {
      for (Int_t itrack=0; itrack<GMULTI_2; itrack++)
      {
        if (IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          fL1SELoss  = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
          FillGlobalEvent_L2L3(globalevent, GMULTI_2, MODEINDEX_G2_1010, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    else // 考虑第 0）条或第 1）条候选径迹为有效径迹
    {
      for (Int_t itrack=0; itrack<GMULTI_2; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_2, MODEINDEX_G2_1010, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_1101_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_2) && (strcmp(mode,MODE_G2_1101)==0))
  {
    g2_EL2B_Sum = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][1];
    // 考虑 L2F 双击, 此时使用 EL2B 代替第二层能量, 需要将 EL2B Scale 一下
    // 因为 EL2B != EL2F, 而 BananaCut 是根据 EL2F vs ECsI 作出来的
    if (IsEneConstraint_L2B_L2F(ssdindex,g2_EL2B_Sum,trackevent->fL2FEMeV[ssdindex][0]))
    {
      for (Int_t itrack=0; itrack<GMULTI_2; itrack++)
      {
        if (IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_2, MODEINDEX_G2_1101, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    else // 考虑第 0）条或第 1）条候选径迹为有效径迹
    {
      for (Int_t itrack=0; itrack<GMULTI_2; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_2, MODEINDEX_G2_1101, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g2_1111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_2) && (strcmp(mode,MODE_G2_1111)==0))
  {
    for (Int_t itrack=0; itrack<GMULTI_2; itrack++)
    {
      if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
          IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_2, MODEINDEX_G2_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                        trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                        trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                        trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                        trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                        trackevent->fL2FTime[ssdindex][itrack]);
      }
    }
  }
}


//______________________________________________________________________________
//             L2L3_g3_TrackDecoded
//          ------------------------
//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_0002_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_3) && (strcmp(mode,MODE_G3_0002)==0))
  {
    if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
				IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]))
    {
      if (IsEneConstraint_L1S_L2F(ssdindex, trackevent->fL1SEMeV[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0]))
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0002, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                        trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0],
                        trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                        trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                        trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                        trackevent->fL2FTime[ssdindex][0]);
      }
      else if (IsEneConstraint_L1S_L2F(ssdindex, trackevent->fL1SEMeV[ssdindex][1], trackevent->fL2FEMeV[ssdindex][1]))
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0002, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                        trackevent->fL1SNumStrip[ssdindex][1], trackevent->fL1SEMeV[ssdindex][1],
                        trackevent->fL2FNumStrip[ssdindex][1], trackevent->fL2FEMeV[ssdindex][1],
                        trackevent->fL2BNumStrip[ssdindex][1], trackevent->fL2BEMeV[ssdindex][1],
                        trackevent->fCsINum[ssdindex][1],      trackevent->fCsIECh[ssdindex][1],
                        trackevent->fL2FTime[ssdindex][1]);
      }
      else if (IsEneConstraint_L1S_L2F(ssdindex, trackevent->fL1SEMeV[ssdindex][2], trackevent->fL2FEMeV[ssdindex][2]))
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0002, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                        trackevent->fL1SNumStrip[ssdindex][2], trackevent->fL1SEMeV[ssdindex][2],
                        trackevent->fL2FNumStrip[ssdindex][2], trackevent->fL2FEMeV[ssdindex][2],
                        trackevent->fL2BNumStrip[ssdindex][2], trackevent->fL2BEMeV[ssdindex][2],
                        trackevent->fCsINum[ssdindex][2],      trackevent->fCsIECh[ssdindex][2],
                        trackevent->fL2FTime[ssdindex][2]);
      }
      else {
        fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][0],fMCut[0].Cut_Z, fMCut[0].Cut_A);
        FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0002, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                        trackevent->fL2BNumStrip[ssdindex][0], fL1SELoss,  // 认为 L1S 与 L2B 击中位置 strip 编号相同
                        trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                        trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                        trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                        trackevent->fL2FTime[ssdindex][0]);
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_0020_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_3) && (strcmp(mode,MODE_G3_0020)==0))
  {
    // 考虑 L2F sharing
    g3_EL2F_Sum_01 = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][1];
    g3_EL2F_Sum_12 = trackevent->fL2FEMeV[ssdindex][1] + trackevent->fL2FEMeV[ssdindex][2];
    // 考虑第0)条与第 1)条候选径迹在 L2F 产生 charge sharing
    if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][1])==1) &&
        IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],g3_EL2F_Sum_01) &&
        IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],g3_EL2F_Sum_01))
    {
      g3_ChargeCenter_01 = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][1],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][1]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0020, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0],
                      g3_ChargeCenter_01,                   g3_EL2F_Sum_01,
                      trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                      trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                      trackevent->fL2FTime[ssdindex][0]);
    }
    // 考虑第1)条与第2)条候选径迹在 L2F 产生 charge sharing
    else if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][1]-trackevent->fL2FNumStrip[ssdindex][2])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],g3_EL2F_Sum_12) &&
             IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],g3_EL2F_Sum_12))
    {
      g3_ChargeCenter_12 = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][1],trackevent->fL2FNumStrip[ssdindex][2],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][2]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0020, ssdindex, fSSDgMulti[ssdindex], fMCut[1].Cut_Z, fMCut[1].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][1], trackevent->fL1SEMeV[ssdindex][1],
                      g3_ChargeCenter_12,                   g3_EL2F_Sum_12,
                      trackevent->fL2BNumStrip[ssdindex][1], trackevent->fL2BEMeV[ssdindex][1],
                      trackevent->fCsINum[ssdindex][1],      trackevent->fCsIECh[ssdindex][1],
                      trackevent->fL2FTime[ssdindex][1]);
    }
    else
    {  // 考虑3条候选径迹中只有1条有效径迹
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
    				IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0020, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_0101_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_3) && (strcmp(mode,MODE_G3_0101)==0))
  {
    // 考虑 L2B sharing
    g3_EL2B_Sum_01 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][1];
    g3_EL2B_Sum_12 = trackevent->fL2BEMeV[ssdindex][1] + trackevent->fL2BEMeV[ssdindex][2];
    // 考虑第0)条与第1)条候选径迹在 L2B 产生 charge sharing
    if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][1])==1) &&
        IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum_01,trackevent->fL2FEMeV[ssdindex][0]) &&
        IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]))
    {
      g3_ChargeCenter_01 = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][1]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0101, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0],
                      trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                      g3_ChargeCenter_01,                    g3_EL2B_Sum_01,
                      trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                      trackevent->fL2FTime[ssdindex][0]);
    }
    // 考虑第1)条与第2)条候选径迹在 L2B 产生 charge sharing
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][1]-trackevent->fL2BNumStrip[ssdindex][2])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum_12,trackevent->fL2FEMeV[ssdindex][1]) &&
             IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]))
    {
      g3_ChargeCenter_12 = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BNumStrip[ssdindex][2],trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2BEMeV[ssdindex][2]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0101, ssdindex, fSSDgMulti[ssdindex], fMCut[1].Cut_Z, fMCut[1].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][1], trackevent->fL1SEMeV[ssdindex][1],
                      trackevent->fL2FNumStrip[ssdindex][1], trackevent->fL2FEMeV[ssdindex][1],
                      g3_ChargeCenter_12,                    g3_EL2B_Sum_12,
                      trackevent->fCsINum[ssdindex][1],      trackevent->fCsIECh[ssdindex][1],
                      trackevent->fL2FTime[ssdindex][1]);
    }
    else
    {  // 考虑3条候选径迹中只有1条有效径迹
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
    				IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0101, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_0102_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_3) && (strcmp(mode,MODE_G3_0102)==0))
  {
    // 考虑 L2B sharing
    g3_EL2B_Sum_01 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][1];
    g3_EL2B_Sum_12 = trackevent->fL2BEMeV[ssdindex][1] + trackevent->fL2BEMeV[ssdindex][2];
    // 考虑第0)条与第1)条候选径迹在 L2B 产生 charge sharing
    if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][1])==1) &&
        IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum_01,trackevent->fL2FEMeV[ssdindex][0]) &&
        IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]))
    {
      g3_ChargeCenter_01 = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][1]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0102, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0],
                      trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                      g3_ChargeCenter_01,                    g3_EL2B_Sum_01,
                      trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                      trackevent->fL2FTime[ssdindex][0]);
    }
    // 考虑第1)条与第2)条候选径迹在 L2B 产生 charge sharing
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][1]-trackevent->fL2BNumStrip[ssdindex][2])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum_12,trackevent->fL2FEMeV[ssdindex][1]) &&
             IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]))
    {
      g3_ChargeCenter_12 = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BNumStrip[ssdindex][2],trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2BEMeV[ssdindex][2]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0102, ssdindex, fSSDgMulti[ssdindex], fMCut[1].Cut_Z, fMCut[1].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][1], trackevent->fL1SEMeV[ssdindex][1],
                      trackevent->fL2FNumStrip[ssdindex][1], trackevent->fL2FEMeV[ssdindex][1],
                      g3_ChargeCenter_12,                    g3_EL2B_Sum_12,
                      trackevent->fCsINum[ssdindex][1],      trackevent->fCsIECh[ssdindex][1],
                      trackevent->fL2FTime[ssdindex][1]);
    }
    else
    {  // 考虑3条候选径迹中只有1条有效径迹
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
    				IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0102, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_0201_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_3) && (strcmp(mode,MODE_G3_0201)==0))
  {
    // 考虑 L2B sharing
    g3_EL2B_Sum_01 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][1];
    g3_EL2B_Sum_12 = trackevent->fL2BEMeV[ssdindex][1] + trackevent->fL2BEMeV[ssdindex][2];
    // 考虑第0)条与第1)条候选径迹在 L2B 产生 charge sharing
    if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][1])==1) &&
        IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum_01,trackevent->fL2FEMeV[ssdindex][0]) &&
        IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]))
    {
      g3_ChargeCenter_01 = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][1]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0201, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0],
                      trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                      g3_ChargeCenter_01,                    g3_EL2B_Sum_01,
                      trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                      trackevent->fL2FTime[ssdindex][0]);
    }
    // 考虑第1)条与第2)条候选径迹在 L2B 产生 charge sharing
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][1]-trackevent->fL2BNumStrip[ssdindex][2])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum_12,trackevent->fL2FEMeV[ssdindex][1]) &&
             IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]))
    {
      g3_ChargeCenter_12 = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BNumStrip[ssdindex][2],trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2BEMeV[ssdindex][2]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0201, ssdindex, fSSDgMulti[ssdindex], fMCut[1].Cut_Z, fMCut[1].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][1], trackevent->fL1SEMeV[ssdindex][1],
                      trackevent->fL2FNumStrip[ssdindex][1], trackevent->fL2FEMeV[ssdindex][1],
                      g3_ChargeCenter_12,                    g3_EL2B_Sum_12,
                      trackevent->fCsINum[ssdindex][1],      trackevent->fCsIECh[ssdindex][1],
                      trackevent->fL2FTime[ssdindex][1]);
    }
    else
    {  // 考虑3条候选径迹中只有1条有效径迹
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
    				IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_0201, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_1010_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_3) && (strcmp(mode,MODE_G3_1010)==0))
  {
    //
    g3_EL2F_Sum_01 = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][1];
    g3_EL2F_Sum_12 = trackevent->fL2FEMeV[ssdindex][1] + trackevent->fL2FEMeV[ssdindex][2];
    // 考虑 L2B 双击, 第0)条与第1)条候选径迹为有效径迹
    if (((trackevent->fCsIECh[ssdindex][0]!=trackevent->fCsIECh[ssdindex][1]) && (trackevent->fL2FEMeV[ssdindex][0]!=trackevent->fL2FEMeV[ssdindex][1])) &&
				(IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],g3_EL2F_Sum_01) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
				if (itrack==0 || itrack==1) {
          fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1010, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2B 双击, 第1)条与第2)条候选径迹为有效径迹, 不存在第0)条与第2)条候选径迹为有效径迹的情况！
    else if (((trackevent->fCsIECh[ssdindex][1]!=trackevent->fCsIECh[ssdindex][2]) && (trackevent->fL2FEMeV[ssdindex][1]!=trackevent->fL2FEMeV[ssdindex][2])) &&
				     (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],g3_EL2F_Sum_12) &&
				      IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
				      IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
				if (itrack==1 || itrack==2) {
          fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1010, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    else
    {  // 考虑3条候选径迹中只有1条有效径迹
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
    				IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1010, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_1020_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_3) && (strcmp(mode,MODE_G3_1020)==0))
  {
    //
    g3_EL2F_Sum_01 = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][1];
    g3_EL2F_Sum_02 = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][2];
    g3_EL2F_Sum_12 = trackevent->fL2FEMeV[ssdindex][1] + trackevent->fL2FEMeV[ssdindex][2];
    // 考虑 L2B 双击, 第0)条与第1)条候选径迹为有效径迹
    if ((trackevent->fCsIECh[ssdindex][0]!=trackevent->fCsIECh[ssdindex][1]) &&
				(IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],g3_EL2F_Sum_01) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
				if (itrack==0 || itrack==1) {
          fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1020, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2B 双击, 第0)条与第2)条候选径迹为有效径迹
    else if ((trackevent->fCsIECh[ssdindex][0]!=trackevent->fCsIECh[ssdindex][2]) &&
				     (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],g3_EL2F_Sum_02) &&
				      IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
				      IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
				if (itrack==0 || itrack==2) {
          fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1020, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2B 双击, 第1)条与第2)条候选径迹为有效径迹
    else if ((trackevent->fCsIECh[ssdindex][1]!=trackevent->fCsIECh[ssdindex][2]) &&
				     (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],g3_EL2F_Sum_12) &&
				      IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
				      IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
				if (itrack==1 || itrack==2) {
          fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1020, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    else
    {  // 考虑3条候选径迹中只有1条有效径迹
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
    				IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1020, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_1110_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_3) && (strcmp(mode,MODE_G3_1110)==0))
  {
    // 考虑第0)条与第1)条候选径迹为有效径迹
    if (((trackevent->fCsIECh[ssdindex][0]!=trackevent->fCsIECh[ssdindex][1]) && (trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][1]) && (trackevent->fL2FEMeV[ssdindex][0]!=trackevent->fL2FEMeV[ssdindex][1])) &&
        (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])) &&
        (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
         IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
				if (itrack==0 || itrack==1) {
          fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑第1)条与第2)条候选径迹为有效径迹, 不存在第0)条与第2)条候选径迹为有效径迹的情况！
    else if (((trackevent->fCsIECh[ssdindex][1]!=trackevent->fCsIECh[ssdindex][2]) && (trackevent->fL2BEMeV[ssdindex][1]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL2FEMeV[ssdindex][1]!=trackevent->fL2FEMeV[ssdindex][2])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
				      IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
				if (itrack==1 || itrack==2) {
          fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    else
    {  // 考虑3条候选径迹中只有1条有效径迹
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
    				IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_1111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_3) && (strcmp(mode,MODE_G3_1111)==0))
  {
    // 考虑第0)条与第1)条候选径迹为有效径迹
    if (((trackevent->fCsIECh[ssdindex][0]!=trackevent->fCsIECh[ssdindex][1]) && (trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][1]) && (trackevent->fL2FEMeV[ssdindex][0]!=trackevent->fL2FEMeV[ssdindex][1]) && (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][1])) &&
        (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])) &&
        (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
         IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
				if (itrack==0 || itrack==1) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑第1)条与第2)条候选径迹为有效径迹, 不存在第0)条与第2)条候选径迹为有效径迹的情况！
    else if (((trackevent->fCsIECh[ssdindex][1]!=trackevent->fCsIECh[ssdindex][2]) && (trackevent->fL2BEMeV[ssdindex][1]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL2FEMeV[ssdindex][1]!=trackevent->fL2FEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][1]!=trackevent->fL1SEMeV[ssdindex][2])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
				      IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==1 || itrack==2) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    else
    {  // 考虑3条候选径迹中只有1条有效径迹
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
    				IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_1112_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_3) && (strcmp(mode,MODE_G3_1112)==0))
  {
    fSSDgMulti[ssdindex] = 0;
    // 考虑第0)条与第1)条候选径迹为有效径迹
    if (((trackevent->fCsIECh[ssdindex][0]!=trackevent->fCsIECh[ssdindex][1]) && (trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][1]) && (trackevent->fL2FEMeV[ssdindex][0]!=trackevent->fL2FEMeV[ssdindex][1])) &&
        (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])) &&
        (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
         IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
				if (itrack==0 || itrack==1) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
		// 考虑第1)条与第2)条候选径迹为有效径迹, 不存在第0)条与第2)条候选径迹为有效径迹的情况！
    else if (((trackevent->fCsIECh[ssdindex][1]!=trackevent->fCsIECh[ssdindex][2]) && (trackevent->fL2BEMeV[ssdindex][1]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL2FEMeV[ssdindex][1]!=trackevent->fL2FEMeV[ssdindex][2])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
				      IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==1 || itrack==2) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    else
    {  // 考虑3条候选径迹中只有1条有效径迹
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
    				IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_1121_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_3) && (strcmp(mode,MODE_G3_1121)==0))
  {
    // 考虑 L2F sharing
    g3_EL2F_Sum_01 = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][1];
    g3_EL2F_Sum_12 = trackevent->fL2FEMeV[ssdindex][1] + trackevent->fL2FEMeV[ssdindex][2];
    // 考虑第0)条与第1)条候选径迹在 L2F 有 charge sharing
    if (((trackevent->fCsIECh[ssdindex][0]==trackevent->fCsIECh[ssdindex][1]) && (trackevent->fL2BEMeV[ssdindex][0]==trackevent->fL2BEMeV[ssdindex][1]) && (trackevent->fL1SEMeV[ssdindex][0]==trackevent->fL1SEMeV[ssdindex][1])) &&
				(TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][1])==1) &&
			  (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],g3_EL2F_Sum_01) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],g3_EL2F_Sum_01)) &&
				(IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
			  if (itrack==0) {
          g3_ChargeCenter_01 = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][1],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][1]);
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          g3_ChargeCenter_01,                         g3_EL2F_Sum_01,
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
        else if (itrack==2) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑第1)条与第2)条候选径迹在 L2F 有 charge sharing
    if (((trackevent->fCsIECh[ssdindex][1]==trackevent->fCsIECh[ssdindex][2]) && (trackevent->fL2BEMeV[ssdindex][1]==trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][1]==trackevent->fL1SEMeV[ssdindex][2])) &&
				(TMath::Abs(trackevent->fL2FNumStrip[ssdindex][1]-trackevent->fL2FNumStrip[ssdindex][2])==1) &&
			  (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],g3_EL2F_Sum_12) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],g3_EL2F_Sum_12)) &&
				(IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==0) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
			  else if (itrack==1) {
          g3_ChargeCenter_12 = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][1],trackevent->fL2FNumStrip[ssdindex][2],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][2]);
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          g3_ChargeCenter_12,                         g3_EL2F_Sum_12,
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑没有 charge sharing,第0)条和第1)条候选径迹为有效径迹
    else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
					    IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
          	  IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])) &&
						 ((trackevent->fCsIECh[ssdindex][0]!=trackevent->fCsIECh[ssdindex][1]) && (trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][1]) && (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][1])))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==0 || itrack==1) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑没有 charge sharing,第0)条和第2)条候选径迹为有效径迹
    else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
					    IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
          	  IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])) &&
						 ((trackevent->fCsIECh[ssdindex][0]!=trackevent->fCsIECh[ssdindex][2]) && (trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][2])))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==0 || itrack==2) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑没有 charge sharing,第1)条和第2)条候选径迹为有效径迹
    else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
					    IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
          	  IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])) &&
						 ((trackevent->fCsIECh[ssdindex][1]!=trackevent->fCsIECh[ssdindex][2]) && (trackevent->fL2BEMeV[ssdindex][1]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][1]!=trackevent->fL1SEMeV[ssdindex][2])))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==1 || itrack==2) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    else
    {  // 考虑3条候选径迹中只有1条有效径迹
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
    				IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_1211_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_3) && (strcmp(mode,MODE_G3_1211)==0))
  {
    // 考虑 L2B sharing
    g3_EL2B_Sum_01 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][1];
    g3_EL2B_Sum_12 = trackevent->fL2BEMeV[ssdindex][1] + trackevent->fL2BEMeV[ssdindex][2];
    // 考虑第0)条与第1)条候选径迹在 L2B 有 charge sharing
    if (((trackevent->fCsIECh[ssdindex][0]==trackevent->fCsIECh[ssdindex][1]) && (trackevent->fL2FEMeV[ssdindex][0]==trackevent->fL2FEMeV[ssdindex][1]) && (trackevent->fL1SEMeV[ssdindex][0]==trackevent->fL1SEMeV[ssdindex][1])) &&
				(TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][1])==1) &&
			  (IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum_01,trackevent->fL2FEMeV[ssdindex][0]) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])) &&
				(IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
			  if (itrack==0) {
          g3_ChargeCenter_01 = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][1]);
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          g3_ChargeCenter_01,                         g3_EL2B_Sum_01,
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
        else if (itrack==2) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑第1)条与第2)条候选径迹在 L2B 有 charge sharing
    if (((trackevent->fCsIECh[ssdindex][1]==trackevent->fCsIECh[ssdindex][2]) && (trackevent->fL2FEMeV[ssdindex][1]==trackevent->fL2FEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][1]==trackevent->fL1SEMeV[ssdindex][2])) &&
				(TMath::Abs(trackevent->fL2BNumStrip[ssdindex][1]-trackevent->fL2BNumStrip[ssdindex][2])==1) &&
			  (IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum_12,trackevent->fL2FEMeV[ssdindex][1]) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])) &&
				(IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==0) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
			  else if (itrack==1) {
          g3_ChargeCenter_12 = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BNumStrip[ssdindex][2],trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2BEMeV[ssdindex][2]);
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          g3_ChargeCenter_12,                         g3_EL2B_Sum_12,
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑没有 charge sharing,第0)条和第1)条候选径迹为有效径迹
    else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
					    IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
          	  IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])) &&
						 ((trackevent->fCsIECh[ssdindex][0]!=trackevent->fCsIECh[ssdindex][1]) && (trackevent->fL2FEMeV[ssdindex][0]!=trackevent->fL2FEMeV[ssdindex][1]) && (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][1])))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==0 || itrack==1) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑没有 charge sharing,第0)条和第2)条候选径迹为有效径迹
    else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
					    IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
          	  IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])) &&
						 ((trackevent->fCsIECh[ssdindex][0]!=trackevent->fCsIECh[ssdindex][2]) && (trackevent->fL2FEMeV[ssdindex][0]!=trackevent->fL2FEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][2])))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==0 || itrack==2) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑没有 charge sharing,第1)条和第2)条候选径迹为有效径迹
    else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
					    IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
          	  IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])) &&
						 ((trackevent->fCsIECh[ssdindex][1]!=trackevent->fCsIECh[ssdindex][2]) && (trackevent->fL2FEMeV[ssdindex][1]!=trackevent->fL2FEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][1]!=trackevent->fL1SEMeV[ssdindex][2])))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==1 || itrack==2) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    else
    {  // 考虑3条候选径迹中只有1条有效径迹
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
    				IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_2111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_3) && (strcmp(mode,MODE_G3_2111)==0))
  {
    // 考虑第0)条与第1)条候选径迹为有效径迹
    if ((IsEneConstraint_L2B_L2F(ssdindex, trackevent->fL2BEMeV[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0]) &&
				 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])) &&
        (IsEneConstraint_L2B_L2F(ssdindex, trackevent->fL2BEMeV[ssdindex][1], trackevent->fL2FEMeV[ssdindex][1]) &&
     		 IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])) &&
				((trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][1]) && (trackevent->fL2FEMeV[ssdindex][0]!=trackevent->fL2FEMeV[ssdindex][1]) &&
         (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][1])))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
				if (itrack==0 || itrack==1) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_2111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑第0)条与第2)条候选径迹为有效径迹
    else if ((IsEneConstraint_L2B_L2F(ssdindex, trackevent->fL2BEMeV[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0]) &&
				      IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])) &&
             (IsEneConstraint_L2B_L2F(ssdindex, trackevent->fL2BEMeV[ssdindex][2], trackevent->fL2FEMeV[ssdindex][2]) &&
     		      IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])) &&
				     ((trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL2FEMeV[ssdindex][0]!=trackevent->fL2FEMeV[ssdindex][2]) &&
              (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][2])))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
				if (itrack==0 || itrack==2) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_2111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑第1)条与第2)条候选径迹为有效径迹
    else if ((IsEneConstraint_L2B_L2F(ssdindex, trackevent->fL2BEMeV[ssdindex][1], trackevent->fL2FEMeV[ssdindex][1]) &&
				      IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])) &&
             (IsEneConstraint_L2B_L2F(ssdindex, trackevent->fL2BEMeV[ssdindex][2], trackevent->fL2FEMeV[ssdindex][2]) &&
     		      IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])) &&
				     ((trackevent->fL2BEMeV[ssdindex][1]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL2FEMeV[ssdindex][1]!=trackevent->fL2FEMeV[ssdindex][2]) &&
              (trackevent->fL1SEMeV[ssdindex][1]!=trackevent->fL1SEMeV[ssdindex][2])))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
				if (itrack==1 || itrack==2) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_2111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    else
    {  // 考虑3条候选径迹中只有1条有效径迹
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++)
      {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
    				IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_2111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g3_2222_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_3) && (strcmp(mode,MODE_G3_2222)==0))
  {
    //
    for (Int_t itrack=0; itrack<GMULTI_3; itrack++)
    {
      if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
          IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_3, MODEINDEX_G3_2222, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                        trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                        trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                        trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                        trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                        trackevent->fL2FTime[ssdindex][itrack]);
      }
    }
  }
}



//______________________________________________________________________________
//             L2L3_g4_TrackDecoded
//          ------------------------
//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_0011_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_4) && (strcmp(mode,MODE_G4_0011)==0))
  {
    //  g4-0011 模式
    //   0) 0000
    //   1) 0001
    //   2) 0010
    //   3) 0011
    g4_EL2F_Sum_03 = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][3];
    //
    // 考虑 L2F sharing: 0)3) sharing
    if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][3])==1) &&
			  IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],g4_EL2F_Sum_03) &&
			  IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],g4_EL2F_Sum_03))
		{
      g4_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][3],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][3]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_0011, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0],
                      g4_ChargeCenter,                       g4_EL2F_Sum_03,
                      trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                      trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                      trackevent->fL2FTime[ssdindex][0]);
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_0011, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_0101_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_4) && (strcmp(mode,MODE_G4_0101)==0))
  {
    //  g4-0101 模式
    //  0) 0000
    //  1) 0001
    //  2) 0100
    //  3) 0101
    g4_EL2B_Sum_03 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][3];
    //
    // 考虑 L2B sharing: 0)3) sharing
    if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][3])==1) &&
			  IsEneConstraint_L2B_L2F(ssdindex,g4_EL2B_Sum_03,trackevent->fL2FEMeV[ssdindex][0]) &&
			  IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]))
		{
      g4_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][3],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][3]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_0101, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0],
                      trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                      g4_ChargeCenter,                       g4_EL2B_Sum_03,
                      trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                      trackevent->fL2FTime[ssdindex][0]);
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_0101, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_0110_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_4) && (strcmp(mode,MODE_G4_0110)==0))
  {
    //  g4-0110 模式
    //  0) 0000
    //  1) 0010
    //  2) 0100
    //  3) 0110
    g4_EL2F_Sum_01 = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][1];
    g4_EL2F_Sum_03 = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][3];
    g4_EL2F_Sum_23 = trackevent->fL2FEMeV[ssdindex][2] + trackevent->fL2FEMeV[ssdindex][3];

    g4_EL2B_Sum_02 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][2];
    g4_EL2B_Sum_03 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][3];
    g4_EL2B_Sum_13 = trackevent->fL2BEMeV[ssdindex][1] + trackevent->fL2BEMeV[ssdindex][3];
    //
    // 考虑 L2F sharing : 0)1) sharing
    if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][1])==1) &&
				IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],g4_EL2F_Sum_01) &&
			  IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],g4_EL2F_Sum_01))
		{
      g4_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][1],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][1]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_0110, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0],
                      g4_ChargeCenter,                       g4_EL2F_Sum_01,
                      trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                      trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                      trackevent->fL2FTime[ssdindex][0]);
    }
    // 考虑 L2F sharing : 2)3) sharing
    else if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][2]-trackevent->fL2FNumStrip[ssdindex][3])==1) &&
			       IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],g4_EL2F_Sum_23) &&
			       IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],g4_EL2F_Sum_23))
		{
      g4_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][2],trackevent->fL2FNumStrip[ssdindex][3],trackevent->fL2FEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][3]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_0110, ssdindex, fSSDgMulti[ssdindex], fMCut[2].Cut_Z, fMCut[2].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][2], trackevent->fL1SEMeV[ssdindex][2],
                      g4_ChargeCenter,                       g4_EL2F_Sum_23,
                      trackevent->fL2BNumStrip[ssdindex][2], trackevent->fL2BEMeV[ssdindex][2],
                      trackevent->fCsINum[ssdindex][2],      trackevent->fCsIECh[ssdindex][2],
                      trackevent->fL2FTime[ssdindex][2]);
    }
    // 考虑 L2B sharing: 0)2) sharing
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][2])==1) &&
			       IsEneConstraint_L2B_L2F(ssdindex,g4_EL2B_Sum_02,trackevent->fL2FEMeV[ssdindex][0]) &&
			       IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]))
		{
      g4_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][2],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][2]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_0110, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0],
                      trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                      g4_ChargeCenter,                       g4_EL2B_Sum_02,
                      trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                      trackevent->fL2FTime[ssdindex][0]);
    }
    // 考虑 L2B sharing: 1)3) sharing
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][1]-trackevent->fL2BNumStrip[ssdindex][3])==1) &&
			       IsEneConstraint_L2B_L2F(ssdindex,g4_EL2B_Sum_13,trackevent->fL2FEMeV[ssdindex][1]) &&
			       IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]))
		{
      g4_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BNumStrip[ssdindex][3],trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2BEMeV[ssdindex][3]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_0110, ssdindex, fSSDgMulti[ssdindex], fMCut[1].Cut_Z, fMCut[1].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][1], trackevent->fL1SEMeV[ssdindex][1],
                      trackevent->fL2FNumStrip[ssdindex][1], trackevent->fL2FEMeV[ssdindex][1],
                      g4_ChargeCenter,                       g4_EL2B_Sum_13,
                      trackevent->fCsINum[ssdindex][1],      trackevent->fCsIECh[ssdindex][1],
                      trackevent->fL2FTime[ssdindex][1]);
    }
    // 考虑 L2F，L2B 同时sharing: 0)3) sharing
    else if (((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][3])==1) && (TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][3])==1)) &&
			       IsEneConstraint_L2B_L2F(ssdindex,g4_EL2B_Sum_03,g4_EL2F_Sum_03) &&
			       IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],g4_EL2F_Sum_03))
		{
      g4_L2F_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][3],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][3]);
      g4_L2B_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][3],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][3]);
      FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_0110, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                      trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0],
                      g4_L2F_ChargeCenter,                   g4_EL2F_Sum_03,
                      g4_L2B_ChargeCenter,                   g4_EL2B_Sum_03,
                      trackevent->fCsINum[ssdindex][0],      trackevent->fCsIECh[ssdindex][0],
                      trackevent->fL2FTime[ssdindex][0]);
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_0110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_0111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_4) && (strcmp(mode,MODE_G4_0111)==0))
  {
    // 考虑只有一条有效径迹
    for (Int_t itrack=0; itrack<GMULTI_4; itrack++)
    {
      if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
          IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_0111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                        trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                        trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                        trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                        trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                        trackevent->fL2FTime[ssdindex][itrack]);
        break;
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_1011_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_4) && (strcmp(mode,MODE_G4_1011)==0))
  {
    // g4-1011 模式
    // 0) 0000
    // 1) 0001
    // 2) 1010
    // 3) 1011
    //
    g4_EL2F_Sum_03 = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][3];
    g4_EL2F_Sum_12 = trackevent->fL2FEMeV[ssdindex][1] + trackevent->fL2FEMeV[ssdindex][2];
    // 考虑 L2B 双击: 0)3)双击
    if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],g4_EL2F_Sum_03) &&
				IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
			  IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][3]],fMCut[3],trackevent->fCsIECh[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3]))
		{
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==0 || itrack==3) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_1011, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2B 双击: 1)2)双击
    else if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],g4_EL2F_Sum_12) &&
				     IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
			       IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]))
		{
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==1 || itrack==2) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_1011, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑只要有一条有效径迹
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_1011, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_1110_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_4) && (strcmp(mode,MODE_G4_1110)==0))
  {
    //   g4_1111 模式:
    //   0）0***
    //   1）0***
    //   2）1***
    //   3）1***
    // 考虑有两条有效径迹: 0)2)
    if ( ((trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL2FEMeV[ssdindex][0]!=trackevent->fL2FEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][2])) &&
          (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
           IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])) &&
          (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
           IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])))
		{
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==0 || itrack==2) {
          fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑有两条有效径迹: 0)3)
    else if ( ((trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][3]) && (trackevent->fL2FEMeV[ssdindex][0]!=trackevent->fL2FEMeV[ssdindex][3]) && (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][3])) &&
              (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
               IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])) &&
              (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3]) &&
               IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][3]],fMCut[3],trackevent->fCsIECh[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3])))
		{
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==0 || itrack==3) {
          fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    //考虑有两条有效径迹: 1)2)
    else if ( ((trackevent->fL2BEMeV[ssdindex][1]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL2FEMeV[ssdindex][1]!=trackevent->fL2FEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][1]!=trackevent->fL1SEMeV[ssdindex][2])) &&
              (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
               IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])) &&
              (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
               IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])))
		{
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==1 || itrack==2) {
          fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    //考虑有两条有效径迹: 1)3)
    else if ( ((trackevent->fL2BEMeV[ssdindex][1]!=trackevent->fL2BEMeV[ssdindex][3]) && (trackevent->fL2FEMeV[ssdindex][1]!=trackevent->fL2FEMeV[ssdindex][3]) && (trackevent->fL1SEMeV[ssdindex][1]!=trackevent->fL1SEMeV[ssdindex][3])) &&
              (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
               IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])) &&
              (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3]) &&
               IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][3]],fMCut[3],trackevent->fCsIECh[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3])))
		{
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==1 || itrack==3) {
          fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑只有一条有效径迹
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_1111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_4) && (strcmp(mode,MODE_G4_1111)==0))
  {
    //   g4_1111 模式:
    //   0）0***
    //   1）0***
    //   2）1***
    //   3）1***
    // 考虑有两条有效径迹: 0)2)
    if ( ((trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL2FEMeV[ssdindex][0]!=trackevent->fL2FEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][2])) &&
          (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
           IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])) &&
          (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
           IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])))
		{
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==0 || itrack==2) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑有两条有效径迹: 0)3)
    else if ( ((trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][3]) && (trackevent->fL2FEMeV[ssdindex][0]!=trackevent->fL2FEMeV[ssdindex][3]) && (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][3])) &&
              (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
               IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][0]],fMCut[0],trackevent->fCsIECh[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0])) &&
              (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3]) &&
               IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][3]],fMCut[3],trackevent->fCsIECh[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3])))
		{
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==0 || itrack==3) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    //考虑有两条有效径迹: 1)2)
    else if ( ((trackevent->fL2BEMeV[ssdindex][1]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL2FEMeV[ssdindex][1]!=trackevent->fL2FEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][1]!=trackevent->fL1SEMeV[ssdindex][2])) &&
              (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
               IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])) &&
              (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
               IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][2]],fMCut[2],trackevent->fCsIECh[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2])))
		{
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==1 || itrack==2) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    //考虑有两条有效径迹: 1)3)
    else if ( ((trackevent->fL2BEMeV[ssdindex][1]!=trackevent->fL2BEMeV[ssdindex][3]) && (trackevent->fL2FEMeV[ssdindex][1]!=trackevent->fL2FEMeV[ssdindex][3]) && (trackevent->fL1SEMeV[ssdindex][1]!=trackevent->fL1SEMeV[ssdindex][3])) &&
              (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
               IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][1]],fMCut[1],trackevent->fCsIECh[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1])) &&
              (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3]) &&
               IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][3]],fMCut[3],trackevent->fCsIECh[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3])))
		{
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==1 || itrack==3) {
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑只有一条有效径迹
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g4_2222_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_4) && (strcmp(mode,MODE_G4_2222)==0))
  {
    // --00-12--
    // 考虑四条候选径迹中, 第1条与第2条为同一条
    if (trackevent->fCsINum[ssdindex][0]==trackevent->fCsINum[ssdindex][1])
    {
      // 在第1条与第2条中找一条
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==0 || itrack==1) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_2222, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 第3,4条直接判选
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==2 || itrack==3) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_2222, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
    }
    // --0-11-2--
    // 考虑四条候选径迹中, 第2条与第3条为同一条
    else if (trackevent->fCsINum[ssdindex][1]==trackevent->fCsINum[ssdindex][2])
    {
      // 在第1条与第2条中找一条
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==1 || itrack==2) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_2222, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 第3,4条直接判选
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==0 || itrack==3) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_2222, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
    }
    // --0-1-22--
    // 考虑四条候选径迹中, 第3条与第4条为同一条
    else if (trackevent->fCsINum[ssdindex][2]==trackevent->fCsINum[ssdindex][3])
    {
      // 在第1条与第2条中找一条
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==2 || itrack==3) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_2222, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 第3,4条直接判选
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==0 || itrack==1) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_4, MODEINDEX_G4_2222, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
    }
  }
}



//______________________________________________________________________________
//             L2L3_g6_TrackDecoded
//          ------------------------
//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_0012_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_6) && (strcmp(mode,MODE_G6_0012)==0))
  {
    // 考虑只有一条有效径迹
    for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
    {
      if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
          IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_0012, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                        trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                        trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                        trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                        trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                        trackevent->fL2FTime[ssdindex][itrack]);
        break;
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_0021_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_6) && (strcmp(mode,MODE_G6_0021)==0))
  {
    // 考虑只有一条有效径迹
    for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
    {
      if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
          IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_0021, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                        trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                        trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                        trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                        trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                        trackevent->fL2FTime[ssdindex][itrack]);
        break;
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_0111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_6) && (strcmp(mode,MODE_G6_0111)==0))
  {
    // 考虑只有一条有效径迹
    for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
    {
      if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
          IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_0111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                        trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                        trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                        trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                        trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                        trackevent->fL2FTime[ssdindex][itrack]);
        break;
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_0112_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_6) && (strcmp(mode,MODE_G6_0112)==0))
  {
    // 考虑只有一条有效径迹
    for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
    {
      if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
          IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_0112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                        trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                        trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                        trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                        trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                        trackevent->fL2FTime[ssdindex][itrack]);
        break;
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_0120_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_6) && (strcmp(mode,MODE_G6_0120)==0))
  {
    // 考虑只有一条有效径迹
    for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
    {
      if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
          IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_0120, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                        trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                        trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                        trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                        trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                        trackevent->fL2FTime[ssdindex][itrack]);
        break;
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_0121_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_6) && (strcmp(mode,MODE_G6_0121)==0))
  {
    // 考虑只有一条有效径迹
    for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
    {
      if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
          IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_0121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                        trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                        trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                        trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                        trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                        trackevent->fL2FTime[ssdindex][itrack]);
        break;
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_0211_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_6) && (strcmp(mode,MODE_G6_0211)==0))
  {
    // 考虑只有一条有效径迹
    for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
    {
      if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
          IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
      {
        FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_0211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                        trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                        trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                        trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                        trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                        trackevent->fL2FTime[ssdindex][itrack]);
        break;
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_1011_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_6) && (strcmp(mode,MODE_G6_1011)==0))
  {
    // g6-1112 模式:
    //    case 1：    case 2:     case 3:     case 4:
    //    0) 0000    0) 0000     0) 0000     0) 0000
    //    1) 0001    1) 0001     1) 0001     1) 0001
    //    2) 0010    2) 0010     2) 1010     2) 1000
    //    3) 0011    3) 0011     3) 1011     3) 1001
    //    4) 1000    4) 1010     4) 1000     4) 1010
    //    5) 1001    5) 1011     5) 1001     5) 1011
    //
    // 考虑 L2B 双击
    g6_EL2F_Sum_12 = trackevent->fL2FEMeV[ssdindex][1] + trackevent->fL2FEMeV[ssdindex][2];
    g6_EL2F_Sum_14 = trackevent->fL2FEMeV[ssdindex][1] + trackevent->fL2FEMeV[ssdindex][4];
    g6_EL2F_Sum_34 = trackevent->fL2FEMeV[ssdindex][3] + trackevent->fL2FEMeV[ssdindex][4];

    // case 1: 3)4) L2B 双击
    if (((trackevent->fCsINum[ssdindex][3]!=trackevent->fCsINum[ssdindex][4]) && (trackevent->fL2FNumStrip[ssdindex][3]!=trackevent->fL2FNumStrip[ssdindex][4])) &&
        IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],g6_EL2F_Sum_34))
		{
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (itrack==3 || itrack==4) {
          if (IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1011, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
    }
    // case 2: 1)4) L2B 双击
    else if (((trackevent->fCsINum[ssdindex][3]!=trackevent->fCsINum[ssdindex][4]) && (trackevent->fL2FNumStrip[ssdindex][1]!=trackevent->fL2FNumStrip[ssdindex][4])) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],g6_EL2F_Sum_14))
		{
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (itrack==1 || itrack==4) {
          if (IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1011, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
    }
    // case 3: 1)2) L2B 双击
    else if (((trackevent->fCsINum[ssdindex][1]!=trackevent->fCsINum[ssdindex][2]) && (trackevent->fL2FNumStrip[ssdindex][1]!=trackevent->fL2FNumStrip[ssdindex][2])) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],g6_EL2F_Sum_12))
		{
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (itrack==1 || itrack==2) {
          if (IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1011, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
    }
    // case 4: 1)4) L2B 双击
    else if (((trackevent->fCsINum[ssdindex][1]!=trackevent->fCsINum[ssdindex][2]) && (trackevent->fL2FNumStrip[ssdindex][1]!=trackevent->fL2FNumStrip[ssdindex][4])) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],g6_EL2F_Sum_14))
		{
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (itrack==1 || itrack==4) {
          if (IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1011, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
    }
    // 考虑只有一条有效径迹
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
        {
          FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1011, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                          trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                          trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                          trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                          trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                          trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_1110_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_6) && (strcmp(mode,MODE_G6_1110)==0))
  {
    // g6-1112 模式:
    // ---0--12345---
    // ---01--2345---
    // ---012--345---
    // ---0123--45---
    // ---01234--5--
    //
    // ---0--12345---
    if (trackevent->fCsINum[ssdindex][0]!=trackevent->fCsINum[ssdindex][1])
    {
      // 在第0)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack==0) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第1)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=1) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---01--2345---
    else if (trackevent->fCsINum[ssdindex][1]!=trackevent->fCsINum[ssdindex][2])
    {
      // 在第0)-1)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<2) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第2)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=2) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---012--345---
    else if (trackevent->fCsINum[ssdindex][2]!=trackevent->fCsINum[ssdindex][3])
    {
      // 在第0)-2)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<3) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第3)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=3) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---0123--45---
    else if (trackevent->fCsINum[ssdindex][3]!=trackevent->fCsINum[ssdindex][4])
    {
      // 在第0)-3)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<4) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第4)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=4) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---01234--5--
    else if (trackevent->fCsINum[ssdindex][4]!=trackevent->fCsINum[ssdindex][5])
    {
      // 在第0)-4)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<5) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=5) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            fL1SELoss = GetEL1S(ssdindex,trackevent->fL2FEMeV[ssdindex][itrack],fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A);
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], fL1SELoss,
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_1111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_6) && (strcmp(mode,MODE_G6_1111)==0))
  {
    // g6-1112 模式:
    // ---0--12345---
    // ---01--2345---
    // ---012--345---
    // ---0123--45---
    // ---01234--5--
    //
    // ---0--12345---
    if (trackevent->fCsINum[ssdindex][0]!=trackevent->fCsINum[ssdindex][1])
    {
      // 在第0)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack==0) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第1)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=1) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---01--2345---
    else if (trackevent->fCsINum[ssdindex][1]!=trackevent->fCsINum[ssdindex][2])
    {
      // 在第0)-1)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<2) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第2-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=2) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---012--345---
    else if (trackevent->fCsINum[ssdindex][2]!=trackevent->fCsINum[ssdindex][3])
    {
      // 在第0)-2)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<3) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第3)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=3) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---0123--45---
    else if (trackevent->fCsINum[ssdindex][3]!=trackevent->fCsINum[ssdindex][4])
    {
      // 在第0)-3)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<4) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第4)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=4) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---01234--5--
    else if (trackevent->fCsINum[ssdindex][4]!=trackevent->fCsINum[ssdindex][5])
    {
      // 在第0)-4)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<5) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=5) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_1112_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_6) && (strcmp(mode,MODE_G6_1112)==0))
  {
    // g6-1112 模式:
    // ---0--12345---
    // ---01--2345---
    // ---012--345---
    // ---0123--45---
    // ---01234--5--
    //
    // ---0--12345---
    if (trackevent->fCsINum[ssdindex][0]!=trackevent->fCsINum[ssdindex][1])
    {
      // 在第0)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack==0) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第1)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=1) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---01--2345---
    else if (trackevent->fCsINum[ssdindex][1]!=trackevent->fCsINum[ssdindex][2])
    {
      // 在第0)-1)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<2) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第2)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=2) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---012--345---
    else if (trackevent->fCsINum[ssdindex][2]!=trackevent->fCsINum[ssdindex][3])
    {
      // 在第0)-2)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<3) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第3)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=3) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---0123--45---
    else if (trackevent->fCsINum[ssdindex][3]!=trackevent->fCsINum[ssdindex][4])
    {
      // 在第0)-3)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<4) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第4)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=4) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---01234--5--
    else if (trackevent->fCsINum[ssdindex][4]!=trackevent->fCsINum[ssdindex][5])
    {
      // 在第0)-4)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<5) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=5) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_1121_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_6) && (strcmp(mode,MODE_G6_1121)==0))
  {
    // g6-1121 模式:
    // ---0--12345---
    // ---01--2345---
    // ---012--345---
    // ---0123--45---
    // ---01234--5--
    //
    // ---0--12345---
    if (trackevent->fCsINum[ssdindex][0]!=trackevent->fCsINum[ssdindex][1])
    {
      // 在第0)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack==0) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第1)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=1) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---01--2345---
    else if (trackevent->fCsINum[ssdindex][1]!=trackevent->fCsINum[ssdindex][2])
    {
      // 在第0)-1)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<2) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第2)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=2) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---012--345---
    else if (trackevent->fCsINum[ssdindex][2]!=trackevent->fCsINum[ssdindex][3])
    {
      // 在第0)-2)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<3) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第3)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=3) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---0123--45---
    else if (trackevent->fCsINum[ssdindex][3]!=trackevent->fCsINum[ssdindex][4])
    {
      // 在第0)-3)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<4) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第4)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=4) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---01234--5--
    else if (trackevent->fCsINum[ssdindex][4]!=trackevent->fCsINum[ssdindex][5])
    {
      // 在第0)-4)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<5) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=5) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_1211_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_6) && (strcmp(mode,MODE_G6_1211)==0))
  {
    // g6-1211 模式:
    // ---0--12345---
    // ---01--2345---
    // ---012--345---
    // ---0123--45---
    // ---01234--5--
    //
    // ---0--12345---
    if (trackevent->fCsINum[ssdindex][0]!=trackevent->fCsINum[ssdindex][1])
    {
      // 在第0)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack==0) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第1)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=1) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---01--2345---
    else if (trackevent->fCsINum[ssdindex][1]!=trackevent->fCsINum[ssdindex][2])
    {
      // 在第0)-1)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<2) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第2)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=2) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---012--345---
    else if (trackevent->fCsINum[ssdindex][2]!=trackevent->fCsINum[ssdindex][3])
    {
      // 在第0)-2)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<3) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第3)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=3) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---0123--45---
    else if (trackevent->fCsINum[ssdindex][3]!=trackevent->fCsINum[ssdindex][4])
    {
      // 在第0)-3)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<4) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第4)-5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=4) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // ---01234--5--
    else if (trackevent->fCsINum[ssdindex][4]!=trackevent->fCsINum[ssdindex][5])
    {
      // 在第0)-4)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack<5) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第5)条候选径迹中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++)
      {
        if (itrack>=5) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_1211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
  }
}

//______________________________________________________________________________
void L2L3_TrackDecoded::L2L3_g6_2222_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if ((trackevent->fCsINum[ssdindex].size()==GMULTI_6) && (strcmp(mode,MODE_G6_2222)==0))
  {
    // g6-2222 模式, 只考虑:
    // ---01--23--45---
    // 策略: 在 01, 23, 45 条候选径迹中, 各自找出1条有效径迹
    //
    if ((trackevent->fCsINum[ssdindex][1]!=trackevent->fCsINum[ssdindex][2]) && (trackevent->fCsINum[ssdindex][3]!=trackevent->fCsINum[ssdindex][4]))
    {
      // 在第0)条与第1)条中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (itrack==0 || itrack==1) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_2222, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第2)条与第3)条中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (itrack==2 || itrack==3) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_2222, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      // 在第4)条与第5)条中找一条
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (itrack==4 || itrack==5) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[trackevent->fCutTelNum[ssdindex][itrack]],fMCut[itrack],trackevent->fCsIECh[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]))
          {
            FillGlobalEvent_L2L3(globalevent, GMULTI_6, MODEINDEX_G6_2222, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                            trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack],
                            trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                            trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                            trackevent->fCsINum[ssdindex][itrack],      trackevent->fCsIECh[ssdindex][itrack],
                            trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      //
    }
  }
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo




//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//  L1L2 径迹重建的算法摘自源程序 L1L2TrackFinding.C
//  这里将算法部分摘出来, 简化成单独的函数, 以便后续正式调用
//
//  gfh, 2021-08
//______________________________________________________________________________
L1L2_TrackDecoded::L1L2_TrackDecoded()
{
  TFile* myfile = TFile::Open(pathBananaCut.c_str());
  // 读取 bananacut
  for (Int_t numtel=0; numtel<NUM_SSD; numtel++) {
    TIter next(myfile->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*) next())) {
      TCutG* cut = (TCutG*) key->ReadObj();
      std::string cutname(cut->GetName());

      if (strstr(cutname.c_str(), Form("_Tel%02d_",numtel)) != NULL) {
        BananaCut[numtel].push_back(cut);
        // cout<<cut->GetName()<<endl;
      }
    }
  }
}

L1L2_TrackDecoded::~L1L2_TrackDecoded()
{}

//____________________________________
void L1L2_TrackDecoded::FillGlobalEvent_L1L2(CSHINESSDEvent* globalevent, Int_t gmulti, Int_t mode_index,
  Int_t ssdindex, Int_t& ssdmulti, Int_t charge, Int_t mass, Double_t Stripl1s, Double_t El1s, Double_t El1s_corrected,
  Double_t Stripl2f, Double_t El2f, Double_t Stripl2b, Double_t El2b, Int_t Timel2f)
{
  fEtot       = El1s + El2f;
  fMomentum   = physcalc.GetMomentumValue(fEtot,charge,mass);
  fVelocity   = physcalc.GetBeta(fEtot,charge,mass);
  fPID        = l1l2pid.L1L2_GetPIDNumFromExpData(ssdindex,El2f,El1s_corrected,charge,mass);
  fMassd      = l1l2pid.GetMassFromPIDNum(fPID,charge);
  vechitpoint = pixel.StripNumToPosInLabFrame(ssdindex,Stripl2b,Stripl2f);
  fModeID     = GetModeID(gmulti,mode_index);

	ssdmulti++;
  globalevent->fGSSDNum.push_back(ssdindex);
  globalevent->fGL1SNumStrip.push_back(Stripl1s);
  globalevent->fGL1SEMeV.push_back(El1s);
  globalevent->fGL1SEMeVCorrected.push_back(El1s_corrected);
  globalevent->fGL2FNumStrip.push_back(Stripl2f);
  globalevent->fGL2FEMeV.push_back(El2f);
  globalevent->fGL2BNumStrip.push_back(Stripl2b);
  globalevent->fGL2BEMeV.push_back(El2b);
  globalevent->fGCsINum.push_back(fCsINum_Init);
  globalevent->fGCsIEMeV.push_back(fCsIEMeV_Init);
  globalevent->fGL2FTime.push_back(Timel2f);
  globalevent->fDist.push_back(vechitpoint.R());
	globalevent->fTheta.push_back(vechitpoint.ThetaDeg());
	globalevent->fPhi.push_back(vechitpoint.PhiDeg());
	globalevent->fTotEkinc.push_back(fEtot);
	globalevent->fP.push_back(fMomentum);
	globalevent->fBeta.push_back(fVelocity);
	globalevent->fZ.push_back(charge);
  globalevent->fA.push_back(mass);
  globalevent->fAd.push_back(fMassd);
  globalevent->fPIDNum.push_back(fPID);
  globalevent->fTrackModeID.push_back(fModeID);
}
//______________________________________________________________________________

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g1_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭 L2L3_g1_TrackDecoded 通道
  if (switchsetting == kFALSE) return;

  fgMulti = 0;
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    if ((trackevent->fCsINum[ssdindex].size()==0) && (trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_1))
    {
      fSSDgMulti[ssdindex] = 0;

      if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
          IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0]))
      {
        FillGlobalEvent_L1L2(globalevent, GMULTI_1, MODEINDEX_G1, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                             trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0], trackevent->fL1SEMeV_Corrected[ssdindex][0],
                             trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                             trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                             trackevent->fL2FTime[ssdindex][0]);
      }
      //
  		fgMulti += fSSDgMulti[ssdindex];
  		globalevent->fSSDGlobalMulti.push_back(fSSDgMulti[ssdindex]);
    }
  }
  //
 	globalevent->fGlobalMulti = fgMulti;
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g2_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭 L2L3_g2_TrackDecoded 通道
  if (switchsetting == kFALSE) return;

  fgMulti = 0;
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    fSSDgMulti[ssdindex] = 0;
    //
    if ((trackevent->fCsINum[ssdindex].size()==0) && (trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_2))
    {
      MODE_EXP[ssdindex] = L1L2_CalcModeFromExpData(GMULTI_2,trackevent->fL2BNumStrip[ssdindex],trackevent->fL2FNumStrip[ssdindex],trackevent->fL1SNumStrip[ssdindex]);
      L1L2_g2_001_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G2_001);
      L1L2_g2_010_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G2_010);
      L1L2_g2_100_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G2_100);
      L1L2_g2_101_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G2_101);
      /*
      if (ssdindex==0) {
        cout<<"mode="<<MODE_EXP[ssdindex].c_str()<<setw(15)
            <<"l2b[0]="<<trackevent->fL2BNumStrip[ssdindex][0]<<setw(15)<<"l2b[1]="<<trackevent->fL2BNumStrip[ssdindex][1]<<setw(15)
            <<"l2f[0]="<<trackevent->fL2FNumStrip[ssdindex][0]<<setw(15)<<"l2f[1]="<<trackevent->fL2FNumStrip[ssdindex][1]<<setw(15)
            <<"l1s[0]="<<trackevent->fL1SNumStrip[ssdindex][0]<<setw(15)<<"l1s[1]="<<trackevent->fL1SNumStrip[ssdindex][1]
            <<endl;
      }
      */
    }
    //
    fgMulti += fSSDgMulti[ssdindex];
    globalevent->fSSDGlobalMulti.push_back(fSSDgMulti[ssdindex]);
  }
  //
 	globalevent->fGlobalMulti = fgMulti;
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g3_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭 L2L3_g3_TrackDecoded 通道
  if (switchsetting == kFALSE) return;

  fgMulti = 0;
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    fSSDgMulti[ssdindex] = 0;
    //
    if ((trackevent->fCsINum[ssdindex].size()==0) && (trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_3))
    {
      MODE_EXP[ssdindex] = L1L2_CalcModeFromExpData(GMULTI_3,trackevent->fL2BNumStrip[ssdindex],trackevent->fL2FNumStrip[ssdindex],trackevent->fL1SNumStrip[ssdindex]);
      L1L2_g3_002_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_002);
      L1L2_g3_020_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_020);
      L1L2_g3_101_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_101);
      L1L2_g3_102_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_102);
      L1L2_g3_200_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_200);
      L1L2_g3_201_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_201);
      L1L2_g3_202_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G3_202);
    }
    //
    fgMulti += fSSDgMulti[ssdindex];
    globalevent->fSSDGlobalMulti.push_back(fSSDgMulti[ssdindex]);
  }
  //
 	globalevent->fGlobalMulti = fgMulti;
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g4_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭 L2L3_g3_TrackDecoded 通道
  if (switchsetting == kFALSE) return;

  fgMulti = 0;
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    fSSDgMulti[ssdindex] = 0;
    //
    if ((trackevent->fCsINum[ssdindex].size()==0) && (trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_4))
    {
      MODE_EXP[ssdindex] = L1L2_CalcModeFromExpData(GMULTI_4,trackevent->fL2BNumStrip[ssdindex],trackevent->fL2FNumStrip[ssdindex],trackevent->fL1SNumStrip[ssdindex]);
      L1L2_g4_011_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G4_011);
      L1L2_g4_101_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G4_101);
      L1L2_g4_110_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G4_110);
      L1L2_g4_111_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G4_111);
    }
    //
    fgMulti += fSSDgMulti[ssdindex];
    globalevent->fSSDGlobalMulti.push_back(fSSDgMulti[ssdindex]);
  }
  //
 	globalevent->fGlobalMulti = fgMulti;
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g6_TrackDecoded(CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭 L2L3_g3_TrackDecoded 通道
  if (switchsetting == kFALSE) return;

  fgMulti = 0;
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    fSSDgMulti[ssdindex] = 0;
    //
    if ((trackevent->fCsINum[ssdindex].size()==0) && (trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_6))
    {
      MODE_EXP[ssdindex] = L1L2_CalcModeFromExpData(GMULTI_6,trackevent->fL2BNumStrip[ssdindex],trackevent->fL2FNumStrip[ssdindex],trackevent->fL1SNumStrip[ssdindex]);
      L1L2_g6_111_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_111);
      L1L2_g6_112_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_112);
      L1L2_g6_120_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_120);
      L1L2_g6_121_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_121);
      L1L2_g6_211_TrackDecoded(ssdindex, MODE_EXP[ssdindex].c_str(), globalevent, trackevent, SWITCH_MODE_G6_211);
    }
    //
    fgMulti += fSSDgMulti[ssdindex];
    globalevent->fSSDGlobalMulti.push_back(fSSDgMulti[ssdindex]);
  }
  //
 	globalevent->fGlobalMulti = fgMulti;
}


//______________________________________________________________________________
//            L1L2_g2_TrackDecoded
//          ------------------------
void L1L2_TrackDecoded::L1L2_g2_001_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_2)) && (strcmp(mode,MODE_G2_001)==0))
  {
    // 考虑 L1S sharing
    g2_EL1S_Sum = trackevent->fL1SEMeV[ssdindex][0] + trackevent->fL1SEMeV[ssdindex][1];
    g2_EL1S_Sum_Corrected = trackevent->fL1SEMeV_Corrected[ssdindex][0] + trackevent->fL1SEMeV_Corrected[ssdindex][1];
    //
    if ((TMath::Abs(trackevent->fL1SNumStrip[ssdindex][0]-trackevent->fL1SNumStrip[ssdindex][1])==1) &&
				IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
				IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],g2_EL1S_Sum_Corrected))
    {
      g2_ChargeCenter = GetChargeCenter(trackevent->fL1SNumStrip[ssdindex][0],trackevent->fL1SNumStrip[ssdindex][1],trackevent->fL1SEMeV[ssdindex][0],trackevent->fL1SEMeV[ssdindex][1]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_2, MODEINDEX_G2_001, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           g2_ChargeCenter,                       g2_EL1S_Sum, g2_EL1S_Sum_Corrected,
                           trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                           trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                           trackevent->fL2FTime[ssdindex][0]);
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_2; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
				    IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_2, MODEINDEX_G2_001, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g2_010_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_2)) && (strcmp(mode,MODE_G2_010)==0))
  {
    // 考虑 L2F sharing
    g2_EL2F_Sum = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][1];
    //
    if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][1])==1) &&
				IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],g2_EL2F_Sum) &&
				IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],g2_EL2F_Sum,trackevent->fL1SEMeV_Corrected[ssdindex][0]))
    {
      g2_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][1],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][1]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_2, MODEINDEX_G2_010, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0], trackevent->fL1SEMeV_Corrected[ssdindex][0],
                           g2_ChargeCenter,                       g2_EL2F_Sum,
                           trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                           trackevent->fL2FTime[ssdindex][0]);
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_2; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
				    IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_2, MODEINDEX_G2_010, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g2_100_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_2)) && (strcmp(mode,MODE_G2_100)==0))
  {
    // 考虑 L2B sharing
    g2_EL2B_Sum = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][1];
    //
    if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][1])==1) &&
				IsEneConstraint_L2B_L2F(ssdindex,g2_EL2B_Sum,trackevent->fL2FEMeV[ssdindex][0]) &&
				IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0]))
    {
      g2_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][1]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_2, MODEINDEX_G2_100, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0], trackevent->fL1SEMeV_Corrected[ssdindex][0],
                           trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                           g2_ChargeCenter,                       g2_EL2B_Sum,
                           trackevent->fL2FTime[ssdindex][0]);
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_2; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
				    IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_2, MODEINDEX_G2_100, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g2_101_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_2)) && (strcmp(mode,MODE_G2_101)==0))
  {
    g2_EL2B_Sum = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][1];
    g2_EL1S_Sum = trackevent->fL1SEMeV[ssdindex][0] + trackevent->fL1SEMeV[ssdindex][1];
    g2_EL1S_Sum_Corrected = trackevent->fL1SEMeV_Corrected[ssdindex][0] + trackevent->fL1SEMeV_Corrected[ssdindex][1];
    //
    // 考虑有两条有效径迹: L2F 双击, L2能量用 L2B*EL2B_TO_EL2F_SCALE 代替
    if (IsEneConstraint_L2B_L2F(ssdindex,g2_EL2B_Sum,trackevent->fL2FEMeV[ssdindex][0]))
    {
      for (Int_t itrack=0; itrack<GMULTI_2; itrack++) {
        if (IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_2, MODEINDEX_G2_101, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2B sharing
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][1])==1) &&
				     IsEneConstraint_L2B_L2F(ssdindex,g2_EL2B_Sum,trackevent->fL2FEMeV[ssdindex][0]))
    {
      g2_L2B_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][1]);
      g2_L1S_ChargeCenter = GetChargeCenter(trackevent->fL1SNumStrip[ssdindex][0],trackevent->fL1SNumStrip[ssdindex][1],trackevent->fL1SEMeV[ssdindex][0],trackevent->fL1SEMeV[ssdindex][1]);
      // 进一步考虑 L1S sharing
      if ((TMath::Abs(trackevent->fL1SNumStrip[ssdindex][0]-trackevent->fL1SNumStrip[ssdindex][1])==1) &&
  				IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],g2_EL1S_Sum_Corrected))
      {
        FillGlobalEvent_L1L2(globalevent, GMULTI_2, MODEINDEX_G2_101, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                             g2_L1S_ChargeCenter,                   g2_EL1S_Sum, g2_EL1S_Sum_Corrected,
                             trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                             g2_L2B_ChargeCenter,                   g2_EL2B_Sum,
                             trackevent->fL2FTime[ssdindex][0]);
      }
      else
      {
        for (Int_t itrack=0; itrack<GMULTI_2; itrack++) {
          if (IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
          {
            FillGlobalEvent_L1L2(globalevent, GMULTI_2, MODEINDEX_G2_101, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_2; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
				    IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_2, MODEINDEX_G2_101, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}


//______________________________________________________________________________
//            L1L2_g3_TrackDecoded
//          ------------------------
void L1L2_TrackDecoded::L1L2_g3_002_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_3)) && (strcmp(mode,MODE_G3_002)==0))
  {
    // 直接判选
    for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
      if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
          IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
      {
        FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_002, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                             trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                             trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                             trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                             trackevent->fL2FTime[ssdindex][itrack]);
        break;
      }
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g3_020_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_3)) && (strcmp(mode,MODE_G3_020)==0))
  {
    // 考虑 L2F sharing
    g3_EL2F_Sum01 = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][1];
    g3_EL2F_Sum12 = trackevent->fL2FEMeV[ssdindex][1] + trackevent->fL2FEMeV[ssdindex][2];
    //
    // 考虑 0)1) sharing
    if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][1])==1) &&
				IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],g3_EL2F_Sum01) &&
				IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],g3_EL2F_Sum01,trackevent->fL1SEMeV_Corrected[ssdindex][0]))
    {
      g3_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][1],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][1]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_020, ssdindex, fSSDgMulti[0], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0], trackevent->fL1SEMeV_Corrected[ssdindex][0],
                           g3_ChargeCenter,                       g3_EL2F_Sum01,
                           trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                           trackevent->fL2FTime[ssdindex][0]);
    }
    // 考虑 1)2) sharing
    else if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][1]-trackevent->fL2FNumStrip[ssdindex][2])==1) &&
				     IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],g3_EL2F_Sum12) &&
				     IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],g3_EL2F_Sum12,trackevent->fL1SEMeV_Corrected[ssdindex][1]))
    {
      g3_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][1],trackevent->fL2FNumStrip[ssdindex][2],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][2]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_020, ssdindex, fSSDgMulti[1], fMCut[1].Cut_Z, fMCut[1].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][1], trackevent->fL1SEMeV[ssdindex][1], trackevent->fL1SEMeV_Corrected[ssdindex][1],
                           g3_ChargeCenter,                       g3_EL2F_Sum12,
                           trackevent->fL2BNumStrip[ssdindex][1], trackevent->fL2BEMeV[ssdindex][1],
                           trackevent->fL2FTime[ssdindex][1]);
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_020, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g3_101_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_3)) && (strcmp(mode,MODE_G3_101)==0))
  {
    //  g3-101 模式:
    //   case 1：          case 2:
    //  0)  000            0) 000
    //  1)  001            1) 100
    //  2)  101            2) 101
    //
    g3_EL2B_Sum01 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][1];
    g3_EL2B_Sum02 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][2];
    g3_EL2B_Sum12 = trackevent->fL2BEMeV[ssdindex][1] + trackevent->fL2BEMeV[ssdindex][2];
    //
    // 考虑 L2F 双击: 只需考虑 0)2)双击
    if (((trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][2])) &&
        IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum02,trackevent->fL2FEMeV[ssdindex][0]) &&
				(IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2BEMeV[ssdindex][0]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][0]) &&
         IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2BEMeV[ssdindex][2]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][2])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==0 || itrack==2) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_101, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2B sharing, 只需考虑 0)2) sharing
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][2])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum02,trackevent->fL2FEMeV[ssdindex][0]))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==0 || itrack==2) {
          if (IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
          {
            g3_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][2],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][2]);
            FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_101, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 g3_ChargeCenter,                            g3_EL2B_Sum02,
                                 trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_101, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g3_102_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_3)) && (strcmp(mode,MODE_G3_102)==0))
  {
    //  g3-101 模式:
    //   case 1：          case 2:
    //  0)  000            0) 000
    //  1)  001            1) 101
    //  2)  102            2) 102
    //
    g3_EL2B_Sum01 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][1];
    g3_EL2B_Sum02 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][2];
    g3_EL2B_Sum12 = trackevent->fL2BEMeV[ssdindex][1] + trackevent->fL2BEMeV[ssdindex][2];
    //
    // 考虑 L2F 双击: 0)1)双击
    if (((trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][1]) && (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][1])) &&
        IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum01,trackevent->fL2FEMeV[ssdindex][0]) &&
				(IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2BEMeV[ssdindex][0]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][0]) &&
         IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2BEMeV[ssdindex][1]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][1])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==0 || itrack==1) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_102, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2F 双击: 0)2)双击
    else if (((trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][2])) &&
             IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum02,trackevent->fL2FEMeV[ssdindex][0]) &&
				    (IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2BEMeV[ssdindex][0]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][0]) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2BEMeV[ssdindex][2]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][2])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==0 || itrack==2) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_102, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2F 双击: 1)2)双击
    else if (((trackevent->fL2BEMeV[ssdindex][1]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][1]!=trackevent->fL1SEMeV[ssdindex][2])) &&
             IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum12,trackevent->fL2FEMeV[ssdindex][1]) &&
				    (IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2BEMeV[ssdindex][1]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][1]) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2BEMeV[ssdindex][2]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][2])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==1 || itrack==2) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_102, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2B sharing, 0)1) sharing
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][1])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum01,trackevent->fL2FEMeV[ssdindex][0]))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==0 || itrack==1) {
          if (IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
          {
            g3_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][1]);
            FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_102, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 g3_ChargeCenter,                            g3_EL2B_Sum01,
                                 trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // 考虑 L2B sharing, 1)2) sharing
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][1]-trackevent->fL2BNumStrip[ssdindex][2])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum12,trackevent->fL2FEMeV[ssdindex][1]))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==1 || itrack==2) {
          if (IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
          {
            g3_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BNumStrip[ssdindex][2],trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2BEMeV[ssdindex][2]);
            FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_102, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 g3_ChargeCenter,                            g3_EL2B_Sum12,
                                 trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_102, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g3_200_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_3)) && (strcmp(mode,MODE_G3_200)==0))
  {
    g3_EL2B_Sum01 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][1];
    g3_EL2B_Sum12 = trackevent->fL2BEMeV[ssdindex][1] + trackevent->fL2BEMeV[ssdindex][2];
    //
    // 考虑 L2B sharing: 0)1) sharing
    if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][1])==1) &&
        IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum01,trackevent->fL2FEMeV[ssdindex][0]) &&
        IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0]))
    {
      g3_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][1]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_200, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0], trackevent->fL1SEMeV_Corrected[ssdindex][0],
                           trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                           g3_ChargeCenter,                       g3_EL2B_Sum01,
                           trackevent->fL2FTime[ssdindex][0]);
    }
    // 考虑 L2B sharing: 1)2) sharing
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][1]-trackevent->fL2BNumStrip[ssdindex][2])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum12,trackevent->fL2FEMeV[ssdindex][1]) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL1SEMeV_Corrected[ssdindex][1]))
    {
      g3_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BNumStrip[ssdindex][2],trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2BEMeV[ssdindex][2]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_200, ssdindex, fSSDgMulti[ssdindex], fMCut[1].Cut_Z, fMCut[1].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][1], trackevent->fL1SEMeV[ssdindex][1], trackevent->fL1SEMeV_Corrected[ssdindex][1],
                           trackevent->fL2FNumStrip[ssdindex][1], trackevent->fL2FEMeV[ssdindex][1],
                           g3_ChargeCenter,                       g3_EL2B_Sum12,
                           trackevent->fL2FTime[ssdindex][1]);
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_200, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g3_201_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_3)) && (strcmp(mode,MODE_G3_201)==0))
  {
    // g3-201 模式：
    //  0) 000    0)000
    //  1) 100    1)101
    //  2) 201    2)201
    //
    g3_EL2B_Sum01 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][1];
    g3_EL2B_Sum02 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][2];
    g3_EL2B_Sum12 = trackevent->fL2BEMeV[ssdindex][1] + trackevent->fL2BEMeV[ssdindex][2];
    //
    // 考虑 L2F 双击: 0)1)双击
    if (((trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][1]) && (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][1])) &&
        IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum01,trackevent->fL2FEMeV[ssdindex][0]) &&
				(IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2BEMeV[ssdindex][0]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][0]) &&
         IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2BEMeV[ssdindex][1]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][1])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==0 || itrack==1) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_201, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2F 双击: 0)2)双击
    else if (((trackevent->fL2BEMeV[ssdindex][0]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][0]!=trackevent->fL1SEMeV[ssdindex][2])) &&
             IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum02,trackevent->fL2FEMeV[ssdindex][0]) &&
				     (IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2BEMeV[ssdindex][0]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][0]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2BEMeV[ssdindex][2]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][2])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==0 || itrack==2) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_201, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2F 双击: 1)2)双击
    else if (((trackevent->fL2BEMeV[ssdindex][1]!=trackevent->fL2BEMeV[ssdindex][2]) && (trackevent->fL1SEMeV[ssdindex][1]!=trackevent->fL1SEMeV[ssdindex][2])) &&
             IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum12,trackevent->fL2FEMeV[ssdindex][1]) &&
				     (IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2BEMeV[ssdindex][1]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][1]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2BEMeV[ssdindex][2]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][2])))
		{
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==1 || itrack==2) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_201, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2B sharing: 0)1) sharing
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][1])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum01,trackevent->fL2FEMeV[ssdindex][0]))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==0 || itrack==1) {
          if (IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
          {
            g3_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][1]);
            FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_201, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 g3_ChargeCenter,                            g3_EL2B_Sum01,
                                 trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // 考虑 L2B sharing: 1)2) sharing
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][1]-trackevent->fL2BNumStrip[ssdindex][2])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum12,trackevent->fL2FEMeV[ssdindex][1]))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (itrack==1 || itrack==2) {
          if (IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
          {
            g3_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BNumStrip[ssdindex][2],trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2BEMeV[ssdindex][2]);
            FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_201, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 g3_ChargeCenter,                            g3_EL2B_Sum12,
                                 trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_102, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g3_202_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_3)) && (strcmp(mode,MODE_G3_202)==0))
  {
    g3_EL2B_Sum012 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][1] + trackevent->fL2BEMeV[ssdindex][2];
    //
    // 考虑 L2F 三击事件
    if (IsEneConstraint_L2B_L2F(ssdindex,g3_EL2B_Sum012,trackevent->fL2FEMeV[ssdindex][0]))
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_202, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_3; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_3, MODEINDEX_G3_202, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}



//______________________________________________________________________________
//            L1L2_g4_TrackDecoded
//          ------------------------
void L1L2_TrackDecoded::L1L2_g4_011_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_4)) && (strcmp(mode,MODE_G4_011)==0))
  {
    // g4-011 模式:
    //  0) 000
    //  1) 001
    //  2) 010
    //  3) 011
    g4_EL2F_Sum03 = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][3];
    g4_EL2F_Sum12 = trackevent->fL2FEMeV[ssdindex][1] + trackevent->fL2FEMeV[ssdindex][2];
    g4_EL1S_Sum03 = trackevent->fL1SEMeV[ssdindex][0] + trackevent->fL1SEMeV[ssdindex][3];
    g4_EL1S_Sum03_Corrected = trackevent->fL1SEMeV_Corrected[ssdindex][0] + trackevent->fL1SEMeV_Corrected[ssdindex][3];
    //
    // 考虑 L2B 双击: 0)3) 双击
    if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],g4_EL2F_Sum03) &&
				IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0]) &&
        IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],trackevent->fL2FEMeV[ssdindex][3],trackevent->fL1SEMeV_Corrected[ssdindex][3]))
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==0 || itrack==3) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_011, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2B 双击: 1)2) 双击
    else if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],g4_EL2F_Sum12) &&
				     IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL1SEMeV_Corrected[ssdindex][1]) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2FEMeV[ssdindex][2],trackevent->fL1SEMeV_Corrected[ssdindex][2]))
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==1 || itrack==2) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_011, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2F sharing : 0)3) sharing, 且第0)条候选径迹为有效径迹
    else if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][3])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],g4_EL2F_Sum03) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],g4_EL2F_Sum03,trackevent->fL1SEMeV_Corrected[ssdindex][0]))
    {
      g4_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][3],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][3]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_011, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0], trackevent->fL1SEMeV_Corrected[ssdindex][0],
                           g4_ChargeCenter,                       g4_EL2F_Sum03,
                           trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                           trackevent->fL2FTime[ssdindex][0]);
    }
    // 考虑 L2F sharing : 0)3) sharing, 且第3)条候选径迹为有效径迹
    else if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][3])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],g4_EL2F_Sum03) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],g4_EL2F_Sum03,trackevent->fL1SEMeV_Corrected[ssdindex][3]))
    {
      g4_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][3],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][3]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_011, ssdindex, fSSDgMulti[ssdindex], fMCut[3].Cut_Z, fMCut[3].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][3], trackevent->fL1SEMeV[ssdindex][3], trackevent->fL1SEMeV_Corrected[ssdindex][3],
                           g4_ChargeCenter,                       g4_EL2F_Sum03,
                           trackevent->fL2BNumStrip[ssdindex][3], trackevent->fL2BEMeV[ssdindex][3],
                           trackevent->fL2FTime[ssdindex][3]);
    }
    // 考虑 L1S sharing：0）3） sharing, 且第0)条候选径迹为有效径迹
    else if ((TMath::Abs(trackevent->fL1SNumStrip[ssdindex][0]-trackevent->fL1SNumStrip[ssdindex][3])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],g4_EL1S_Sum03))
    {
      g4_ChargeCenter = GetChargeCenter(trackevent->fL1SNumStrip[ssdindex][0],trackevent->fL1SNumStrip[ssdindex][3],trackevent->fL1SEMeV[ssdindex][0],trackevent->fL1SEMeV[ssdindex][3]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_011, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           g4_ChargeCenter,                       g4_EL1S_Sum03, g4_EL1S_Sum03_Corrected,
                           trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                           trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                           trackevent->fL2FTime[ssdindex][0]);
    }
    // 考虑 L1S sharing：0）3） sharing, 且第3)条候选径迹为有效径迹
    else if ((TMath::Abs(trackevent->fL1SNumStrip[ssdindex][0]-trackevent->fL1SNumStrip[ssdindex][3])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3]) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],trackevent->fL2FEMeV[ssdindex][3],g4_EL1S_Sum03))
    {
      g4_ChargeCenter = GetChargeCenter(trackevent->fL1SNumStrip[ssdindex][0],trackevent->fL1SNumStrip[ssdindex][3],trackevent->fL1SEMeV[ssdindex][0],trackevent->fL1SEMeV[ssdindex][3]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_011, ssdindex, fSSDgMulti[ssdindex], fMCut[3].Cut_Z, fMCut[3].Cut_A,
                           g4_ChargeCenter,                       g4_EL1S_Sum03, g4_EL1S_Sum03_Corrected,
                           trackevent->fL2FNumStrip[ssdindex][3], trackevent->fL2FEMeV[ssdindex][3],
                           trackevent->fL2BNumStrip[ssdindex][3], trackevent->fL2BEMeV[ssdindex][3],
                           trackevent->fL2FTime[ssdindex][3]);
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_011, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g4_101_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_4)) && (strcmp(mode,MODE_G4_101)==0))
  {
    // g4-101 模式:
    //  0) 000
    //  1) 001
    //  2) 100
    //  3) 101
    g4_EL2B_Sum03 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][3];
    g4_EL2B_Sum12 = trackevent->fL2BEMeV[ssdindex][1] + trackevent->fL2BEMeV[ssdindex][2];
    //
    // 考虑 L2F 双击: 0)3) 双击
    if (IsEneConstraint_L2B_L2F(ssdindex,g4_EL2B_Sum03,trackevent->fL2FEMeV[ssdindex][0]) &&
				IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2BEMeV[ssdindex][0]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][0]) &&
        IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],trackevent->fL2BEMeV[ssdindex][3]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][3]))
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==0 || itrack==3) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_101, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2F 双击: 1)2) 双击
    else if (IsEneConstraint_L2B_L2F(ssdindex,g4_EL2B_Sum12,trackevent->fL2FEMeV[ssdindex][1]) &&
			     	 IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2BEMeV[ssdindex][1]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][1]) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2BEMeV[ssdindex][2]*EL2B_TO_EL2F_SCALE[ssdindex],trackevent->fL1SEMeV_Corrected[ssdindex][2]))
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==1 || itrack==2) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_101, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑 L2B sharing : 0)3) sharing,且第0)条候选径迹为有效径迹
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][3])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,g4_EL2B_Sum03,trackevent->fL2FEMeV[ssdindex][0]) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0]))
    {
      g4_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][3],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][3]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_101, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0], trackevent->fL1SEMeV_Corrected[ssdindex][0],
                           trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                           g4_ChargeCenter,                       g4_EL2B_Sum03,
                           trackevent->fL2FTime[ssdindex][0]);
    }
    // 考虑 L2B sharing : 0)3) sharing,且第3)条候选径迹为有效径迹
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][3])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,g4_EL2B_Sum03,trackevent->fL2FEMeV[ssdindex][3]) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],trackevent->fL2FEMeV[ssdindex][3],trackevent->fL1SEMeV_Corrected[ssdindex][3]))
    {
      g4_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][3],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][3]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_101, ssdindex, fSSDgMulti[ssdindex], fMCut[3].Cut_Z, fMCut[3].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][3], trackevent->fL1SEMeV[ssdindex][3], trackevent->fL1SEMeV_Corrected[ssdindex][3],
                           trackevent->fL2FNumStrip[ssdindex][3], trackevent->fL2FEMeV[ssdindex][3],
                           g4_ChargeCenter,                       g4_EL2B_Sum03,
                           trackevent->fL2FTime[ssdindex][3]);
    }
    // 考虑 L1S sharing: 0)3) sharing, 且第0)条候选径迹为有效径迹
    else if ((TMath::Abs(trackevent->fL1SNumStrip[ssdindex][0]-trackevent->fL1SNumStrip[ssdindex][3])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],g4_EL1S_Sum03))
    {
      g4_ChargeCenter = GetChargeCenter(trackevent->fL1SNumStrip[ssdindex][0],trackevent->fL1SNumStrip[ssdindex][3],trackevent->fL1SEMeV[ssdindex][0],trackevent->fL1SEMeV[ssdindex][3]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_101, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           g4_ChargeCenter,                            g4_EL1S_Sum03, g4_EL1S_Sum03_Corrected,
                           trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                           trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                           trackevent->fL2FTime[ssdindex][0]);
    }
    // 考虑 L1S sharing: 0)3) sharing, 且第3)条候选径迹为有效径迹
    else if ((TMath::Abs(trackevent->fL1SNumStrip[ssdindex][0]-trackevent->fL1SNumStrip[ssdindex][3])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3]) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],trackevent->fL2FEMeV[ssdindex][3],g4_EL1S_Sum03))
    {
      g4_ChargeCenter = GetChargeCenter(trackevent->fL1SNumStrip[ssdindex][0],trackevent->fL1SNumStrip[ssdindex][3],trackevent->fL1SEMeV[ssdindex][0],trackevent->fL1SEMeV[ssdindex][3]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_101, ssdindex, fSSDgMulti[ssdindex], fMCut[3].Cut_Z, fMCut[3].Cut_A,
                           g4_ChargeCenter,                       g4_EL1S_Sum03, g4_EL1S_Sum03_Corrected,
                           trackevent->fL2FNumStrip[ssdindex][3], trackevent->fL2FEMeV[ssdindex][3],
                           trackevent->fL2BNumStrip[ssdindex][3], trackevent->fL2BEMeV[ssdindex][3],
                           trackevent->fL2FTime[ssdindex][3]);
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_101, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g4_110_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_4)) && (strcmp(mode,MODE_G4_110)==0))
  {
    // g4-110 模式:
    //  0) 000
    //  1) 010
    //  2) 100
    //  3) 110
    g4_EL2F_Sum03 = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][3];
    g4_EL2B_Sum03 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][3];
    //
    // 考虑 L2B sharing: 0)3) sharing, 且第0)条候选径迹为有效径迹
    if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][3])==1) &&
        IsEneConstraint_L2B_L2F(ssdindex,g4_EL2B_Sum03,trackevent->fL2FEMeV[ssdindex][0]) &&
        IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0]))
    {
      g4_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][3],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][3]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_110, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0], trackevent->fL1SEMeV_Corrected[ssdindex][0],
                           trackevent->fL2FNumStrip[ssdindex][0], trackevent->fL2FEMeV[ssdindex][0],
                           g4_ChargeCenter,                       g4_EL2B_Sum03,
                           trackevent->fL2FTime[ssdindex][0]);
    }
    // 考虑 L2B sharing: 0)3) sharing, 且第3)条候选径迹为有效径迹
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][3])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,g4_EL2B_Sum03,trackevent->fL2FEMeV[ssdindex][3]) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],trackevent->fL2FEMeV[ssdindex][3],trackevent->fL1SEMeV_Corrected[ssdindex][3]))
    {
      g4_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][3],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][3]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_110, ssdindex, fSSDgMulti[ssdindex], fMCut[3].Cut_Z, fMCut[3].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][3], trackevent->fL1SEMeV[ssdindex][3], trackevent->fL1SEMeV_Corrected[ssdindex][3],
                           trackevent->fL2FNumStrip[ssdindex][3], trackevent->fL2FEMeV[ssdindex][3],
                           g4_ChargeCenter,                       g4_EL2B_Sum03,
                           trackevent->fL2FTime[ssdindex][3]);
    }
    // 考虑 L2F sharing: 0)3) sharing, 且第0)条候选径迹为有效径迹
    else if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][3])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],g4_EL2F_Sum03) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],g4_EL2F_Sum03,trackevent->fL1SEMeV_Corrected[ssdindex][0]))
    {
      g4_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][3],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][3]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_110, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0], trackevent->fL1SEMeV_Corrected[ssdindex][0],
                           g4_ChargeCenter,                       g4_EL2F_Sum03,
                           trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                           trackevent->fL2FTime[ssdindex][0]);
    }
    // 考虑 L2F sharing: 0)3) sharing, 且第3)条候选径迹为有效径迹
    else if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][3])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],g4_EL2F_Sum03) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],g4_EL2F_Sum03,trackevent->fL1SEMeV_Corrected[ssdindex][3]))
    {
      g4_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][3],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][3]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_110, ssdindex, fSSDgMulti[ssdindex], fMCut[3].Cut_Z, fMCut[3].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][3], trackevent->fL1SEMeV[ssdindex][3], trackevent->fL1SEMeV_Corrected[ssdindex][3],
                           g4_ChargeCenter,                       g4_EL2F_Sum03,
                           trackevent->fL2BNumStrip[ssdindex][3], trackevent->fL2BEMeV[ssdindex][3],
                           trackevent->fL2FTime[ssdindex][3]);
    }
    // 考虑 L2B, L2F 同时 sharing
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][3])==1)&&
             (TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][3])==1)&&
             IsEneConstraint_L2B_L2F(ssdindex,g4_EL2B_Sum03,g4_EL2F_Sum03) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],g4_EL2F_Sum03,trackevent->fL1SEMeV_Corrected[ssdindex][0]))
    {
      g4_L2B_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][3],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][3]);
      g4_L2F_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][3],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][3]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_110, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][0],   trackevent->fL1SEMeV[ssdindex][0], trackevent->fL1SEMeV_Corrected[ssdindex][0],
                           g4_L2F_ChargeCenter,                     g4_EL2F_Sum03,
                           g4_L2B_ChargeCenter,                     g4_EL2B_Sum03,
                           trackevent->fL2FTime[ssdindex][0]);
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_110, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g4_111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_4)) && (strcmp(mode,MODE_G4_111)==0))
  {
    // g4-111 模式:
    //  0) 000
    //  1) 010
    //  2) 101
    //  3) 111
    //
    // 考虑有2条有效径迹: 0)3)
    if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
         IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0])) &&
        (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3]) &&
         IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],trackevent->fL2FEMeV[ssdindex][3],trackevent->fL1SEMeV_Corrected[ssdindex][3])))
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==0 || itrack==3) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑有2条有效径迹: 1)2)
    else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL1SEMeV_Corrected[ssdindex][1])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2FEMeV[ssdindex][2],trackevent->fL1SEMeV_Corrected[ssdindex][2])))
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (itrack==1 || itrack==2) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑有1条有效径迹: 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_4; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_4, MODEINDEX_G4_111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}


//______________________________________________________________________________
//            L1L2_g6_TrackDecoded
//          ------------------------
void L1L2_TrackDecoded::L1L2_g6_111_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_6)) && (strcmp(mode,MODE_G6_111)==0))
  {
    // g6-111 模式:
    //  0) 000            0) 000
		//  1）010            1) 001
		//  2）100            2) 010
		//  3）101            3) 011
		//  4）110            4) 101
		//  5）111            5) 111
    //
    // 考虑有两条有效径迹: 01-2345, 从 01 中找1条, 从 2345 中找1条
    if (trackevent->fL2BNumStrip[ssdindex][1]!=trackevent->fL2BNumStrip[ssdindex][2])
    {
      // 0)5) 为有效径迹
      if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
           IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0])) &&
          (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][5],trackevent->fL2FEMeV[ssdindex][5]) &&
           IsInsideABananaCut(BananaCut[ssdindex],fMCut[5],trackevent->fL2FEMeV[ssdindex][5],trackevent->fL1SEMeV_Corrected[ssdindex][5])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==0 || itrack==5) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 1)3) 为有效径迹
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL1SEMeV_Corrected[ssdindex][1])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],trackevent->fL2FEMeV[ssdindex][3],trackevent->fL1SEMeV_Corrected[ssdindex][3])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==1 || itrack==3) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 直接判断
      else
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
          {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // 考虑有两条有效径迹: 0123-45, 从 0123 中找1条, 从 45 中找1条
    else if (trackevent->fL2BNumStrip[ssdindex][3]!=trackevent->fL2BNumStrip[ssdindex][4])
    {
      // 0)5) 为有效径迹
      if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
           IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0])) &&
          (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][5],trackevent->fL2FEMeV[ssdindex][5]) &&
           IsInsideABananaCut(BananaCut[ssdindex],fMCut[5],trackevent->fL2FEMeV[ssdindex][5],trackevent->fL1SEMeV_Corrected[ssdindex][5])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==0 || itrack==5) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 2)4) 为有效径迹
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2FEMeV[ssdindex][2],trackevent->fL1SEMeV_Corrected[ssdindex][2])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][4],trackevent->fL2FEMeV[ssdindex][4]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[4],trackevent->fL2FEMeV[ssdindex][4],trackevent->fL1SEMeV_Corrected[ssdindex][4])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==2 || itrack==4) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 直接判断
      else
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
          {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_111, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      //
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g6_112_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_6)) && (strcmp(mode,MODE_G6_112)==0))
  {
    // g6-112 模式:
    //  0) 000            0) 000
		//  1）010            1) 001
		//  2）101            2) 010
		//  3）102            3) 011
		//  4）111            4) 102
		//  5）112            5) 112
    //
    // 考虑有两条有效径迹: 01-2345, 从 01 中找1条, 从 2345 中找1条
    if (trackevent->fL2BNumStrip[ssdindex][1]!=trackevent->fL2BNumStrip[ssdindex][2])
    {
      // 0)4)为有效径迹
      if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
           IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0])) &&
          (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][4],trackevent->fL2FEMeV[ssdindex][4]) &&
           IsInsideABananaCut(BananaCut[ssdindex],fMCut[4],trackevent->fL2FEMeV[ssdindex][4],trackevent->fL1SEMeV_Corrected[ssdindex][4])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==0 || itrack==4) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 0)5)为有效径迹
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][5],trackevent->fL2FEMeV[ssdindex][5]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[5],trackevent->fL2FEMeV[ssdindex][5],trackevent->fL1SEMeV_Corrected[ssdindex][5])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==0 || itrack==5) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 1)2)为有效径迹
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL1SEMeV_Corrected[ssdindex][1])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2FEMeV[ssdindex][2],trackevent->fL1SEMeV_Corrected[ssdindex][2])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==1 || itrack==2) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 1)3)为有效径迹
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL1SEMeV_Corrected[ssdindex][1])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],trackevent->fL2FEMeV[ssdindex][3],trackevent->fL1SEMeV_Corrected[ssdindex][3])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==1 || itrack==3) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 直接判选
      else
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
          {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // 考虑有两条有效径迹: 0123-45, 从 0123 中找1条, 从 45 中找1条
    else if (trackevent->fL2BNumStrip[ssdindex][3]!=trackevent->fL2BNumStrip[ssdindex][4])
    {
      // 0)5)为有效径迹
      if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
           IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0])) &&
          (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][5],trackevent->fL2FEMeV[ssdindex][5]) &&
           IsInsideABananaCut(BananaCut[ssdindex],fMCut[5],trackevent->fL2FEMeV[ssdindex][5],trackevent->fL1SEMeV_Corrected[ssdindex][5])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==0 || itrack==5) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 1)5)为有效径迹
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL1SEMeV_Corrected[ssdindex][1])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][5],trackevent->fL2FEMeV[ssdindex][5]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[5],trackevent->fL2FEMeV[ssdindex][5],trackevent->fL1SEMeV_Corrected[ssdindex][5])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==1 || itrack==5) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 2)4)为有效径迹
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2FEMeV[ssdindex][2],trackevent->fL1SEMeV_Corrected[ssdindex][2])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][4],trackevent->fL2FEMeV[ssdindex][4]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[4],trackevent->fL2FEMeV[ssdindex][4],trackevent->fL1SEMeV_Corrected[ssdindex][4])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==2 || itrack==4) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 3)4)为有效径迹
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],trackevent->fL2FEMeV[ssdindex][3],trackevent->fL1SEMeV_Corrected[ssdindex][3])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][4],trackevent->fL2FEMeV[ssdindex][4]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[4],trackevent->fL2FEMeV[ssdindex][4],trackevent->fL1SEMeV_Corrected[ssdindex][4])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==3 || itrack==4) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 直接判选
      else
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
          {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_112, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g6_120_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_6)) && (strcmp(mode,MODE_G6_120)==0))
  {
    // g6-120 模式:
    //  0)  000
		//  1)  010
		//  2)  020
		//  3)  100
		//  4)  110
		//  5)  120
    //
    g6_EL2B_Sum03 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][3];
    g6_EL2B_Sum04 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][4];
    g6_EL2B_Sum15 = trackevent->fL2BEMeV[ssdindex][1] + trackevent->fL2BEMeV[ssdindex][5];
    g6_EL2F_Sum04 = trackevent->fL2FEMeV[ssdindex][0] + trackevent->fL2FEMeV[ssdindex][4];
    g6_EL2F_Sum15 = trackevent->fL2FEMeV[ssdindex][1] + trackevent->fL2FEMeV[ssdindex][5];
    //
    // g6-120 仅考虑只有一条有效径迹的情况
    // L2B sharing: 0)3) sharing
    if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][3])==1) &&
				((IsEneConstraint_L2B_L2F(ssdindex,g6_EL2B_Sum03,trackevent->fL2FEMeV[ssdindex][0])) ||
         (IsEneConstraint_L2B_L2F(ssdindex,g6_EL2B_Sum03,trackevent->fL2FEMeV[ssdindex][1])) ||
         (IsEneConstraint_L2B_L2F(ssdindex,g6_EL2B_Sum03,trackevent->fL2FEMeV[ssdindex][2]))))
		{
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (itrack==0 || itrack==1 || itrack==2) {
          if (IsEneConstraint_L2B_L2F(ssdindex,g6_EL2B_Sum03,trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
          {
            g6_L2B_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][3],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][3]);
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_120, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 g6_L2B_ChargeCenter,                        g6_EL2B_Sum03,
                                 trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // L2F sharing: 0)4) sharing, 0)是有效径迹
    else if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][4])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],g6_EL2F_Sum04) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],g6_EL2F_Sum04,trackevent->fL1SEMeV_Corrected[ssdindex][0]))
    {
      g6_L2F_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][4],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][4]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_120, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0], trackevent->fL1SEMeV_Corrected[ssdindex][0],
                           g6_L2F_ChargeCenter,                   g6_EL2F_Sum04,
                           trackevent->fL2BNumStrip[ssdindex][0], trackevent->fL2BEMeV[ssdindex][0],
                           trackevent->fL2FTime[ssdindex][0]);
    }
    // L2F sharing: 0)4) sharing, 4)是有效径迹
    else if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][4])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][4],g6_EL2F_Sum04) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[4],g6_EL2F_Sum04,trackevent->fL1SEMeV_Corrected[ssdindex][4]))
    {
      g6_L2F_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][4],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][4]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_120, ssdindex, fSSDgMulti[ssdindex], fMCut[4].Cut_Z, fMCut[4].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][4], trackevent->fL1SEMeV[ssdindex][4], trackevent->fL1SEMeV_Corrected[ssdindex][4],
                           g6_L2F_ChargeCenter,                   g6_EL2F_Sum04,
                           trackevent->fL2BNumStrip[ssdindex][4], trackevent->fL2BEMeV[ssdindex][4],
                           trackevent->fL2FTime[ssdindex][4]);
    }
    // L2F sharing: 1)5) sharing, 1)是有效径迹
    else if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][1]-trackevent->fL2FNumStrip[ssdindex][5])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],g6_EL2F_Sum15) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],g6_EL2F_Sum15,trackevent->fL1SEMeV_Corrected[ssdindex][1]))
    {
      g6_L2F_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][1],trackevent->fL2FNumStrip[ssdindex][5],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][5]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_120, ssdindex, fSSDgMulti[ssdindex], fMCut[4].Cut_Z, fMCut[4].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][1], trackevent->fL1SEMeV[ssdindex][1], trackevent->fL1SEMeV_Corrected[ssdindex][1],
                           g6_L2F_ChargeCenter,                   g6_EL2F_Sum15,
                           trackevent->fL2BNumStrip[ssdindex][1], trackevent->fL2BEMeV[ssdindex][1],
                           trackevent->fL2FTime[ssdindex][1]);
    }
    // L2F sharing: 1)5) sharing, 5)是有效径迹
    else if ((TMath::Abs(trackevent->fL2FNumStrip[ssdindex][1]-trackevent->fL2FNumStrip[ssdindex][5])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][5],g6_EL2F_Sum15) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[5],g6_EL2F_Sum15,trackevent->fL1SEMeV_Corrected[ssdindex][5]))
    {
      g6_L2F_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][1],trackevent->fL2FNumStrip[ssdindex][5],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][5]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_120, ssdindex, fSSDgMulti[ssdindex], fMCut[5].Cut_Z, fMCut[5].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][5], trackevent->fL1SEMeV[ssdindex][5], trackevent->fL1SEMeV_Corrected[ssdindex][5],
                           g6_L2F_ChargeCenter,                   g6_EL2F_Sum15,
                           trackevent->fL2BNumStrip[ssdindex][5], trackevent->fL2BEMeV[ssdindex][5],
                           trackevent->fL2FTime[ssdindex][5]);
    }
    // L2B,L2F同时sharing: 0)4) sharing
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][4])==1) &&
             (TMath::Abs(trackevent->fL2FNumStrip[ssdindex][0]-trackevent->fL2FNumStrip[ssdindex][4])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,g6_EL2B_Sum04,g6_EL2F_Sum04) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],g6_EL2F_Sum04,trackevent->fL1SEMeV_Corrected[ssdindex][0]))
    {
      g6_L2F_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][0],trackevent->fL2FNumStrip[ssdindex][4],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][4]);
      g6_L2B_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][4],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][4]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_120, ssdindex, fSSDgMulti[ssdindex], fMCut[0].Cut_Z, fMCut[0].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][0], trackevent->fL1SEMeV[ssdindex][0], trackevent->fL1SEMeV_Corrected[ssdindex][0],
                           g6_L2F_ChargeCenter,                   g6_EL2F_Sum04,
                           g6_L2B_ChargeCenter,                   g6_EL2B_Sum04,
                           trackevent->fL2FTime[ssdindex][0]);
    }
    // L2B,L2F同时sharing: 1)5) sharing
    else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][1]-trackevent->fL2BNumStrip[ssdindex][5])==1) &&
             (TMath::Abs(trackevent->fL2FNumStrip[ssdindex][1]-trackevent->fL2FNumStrip[ssdindex][5])==1) &&
             IsEneConstraint_L2B_L2F(ssdindex,g6_EL2B_Sum15,g6_EL2F_Sum15) &&
             IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],g6_EL2F_Sum15,trackevent->fL1SEMeV_Corrected[ssdindex][1]))
    {
      g6_L2F_ChargeCenter = GetChargeCenter(trackevent->fL2FNumStrip[ssdindex][1],trackevent->fL2FNumStrip[ssdindex][5],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][5]);
      g6_L2B_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BNumStrip[ssdindex][5],trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2BEMeV[ssdindex][5]);
      FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_120, ssdindex, fSSDgMulti[ssdindex], fMCut[1].Cut_Z, fMCut[1].Cut_A,
                           trackevent->fL1SNumStrip[ssdindex][1], trackevent->fL1SEMeV[ssdindex][1], trackevent->fL1SEMeV_Corrected[ssdindex][1],
                           g6_L2F_ChargeCenter,                   g6_EL2F_Sum15,
                           g6_L2B_ChargeCenter,                   g6_EL2B_Sum15,
                           trackevent->fL2FTime[ssdindex][1]);
    }
    // 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_120, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g6_121_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_6)) && (strcmp(mode,MODE_G6_121)==0))
  {
    // g6-121 模式:
    //  0)  000
    //  1)  010
    //  2)  020
    //  3)  101
    //  4)  111
    //  5)  121
    //
    // 考虑两条有效径迹: 0)4)
    // 0)4)为有效径迹
    if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
         IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0])) &&
        (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][4],trackevent->fL2FEMeV[ssdindex][4]) &&
         IsInsideABananaCut(BananaCut[ssdindex],fMCut[4],trackevent->fL2FEMeV[ssdindex][4],trackevent->fL1SEMeV_Corrected[ssdindex][4])))
    {
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (itrack==0 || itrack==4) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑两条有效径迹: 0)5)
    else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][5],trackevent->fL2FEMeV[ssdindex][5]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[5],trackevent->fL2FEMeV[ssdindex][5],trackevent->fL1SEMeV_Corrected[ssdindex][5])))
    {
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (itrack==0 || itrack==5) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑两条有效径迹: 1)3)
    else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL1SEMeV_Corrected[ssdindex][1])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],trackevent->fL2FEMeV[ssdindex][3],trackevent->fL1SEMeV_Corrected[ssdindex][3])))
    {
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (itrack==1 || itrack==3) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑两条有效径迹: 1)5)
    else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL1SEMeV_Corrected[ssdindex][1])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][5],trackevent->fL2FEMeV[ssdindex][5]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[5],trackevent->fL2FEMeV[ssdindex][5],trackevent->fL1SEMeV_Corrected[ssdindex][5])))
    {
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (itrack==1 || itrack==5) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑两条有效径迹: 2)3)
    else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2FEMeV[ssdindex][2],trackevent->fL1SEMeV_Corrected[ssdindex][2])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],trackevent->fL2FEMeV[ssdindex][3],trackevent->fL1SEMeV_Corrected[ssdindex][3])))
    {
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (itrack==2 || itrack==3) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑两条有效径迹: 2)4)
    else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2FEMeV[ssdindex][2],trackevent->fL1SEMeV_Corrected[ssdindex][2])) &&
             (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][4],trackevent->fL2FEMeV[ssdindex][4]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[4],trackevent->fL2FEMeV[ssdindex][4],trackevent->fL1SEMeV_Corrected[ssdindex][4])))
    {
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (itrack==2 || itrack==4) {
          FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
        }
      }
    }
    // 考虑只有一条有效径迹: 直接判选
    else
    {
      for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
        if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
            IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
        {
          FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_121, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                               trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                               trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                               trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                               trackevent->fL2FTime[ssdindex][itrack]);
          break;
        }
      }
    }
  }
}

//______________________________________________________________________________
void L1L2_TrackDecoded::L1L2_g6_211_TrackDecoded(Int_t ssdindex, const char* mode, CSHINESSDEvent* globalevent, CSHINETrackEvent2* trackevent, Bool_t switchsetting)
{
  // 关闭当前通道
  if (switchsetting == kFALSE) return;

  if (((trackevent->fCsINum[ssdindex].size()==0)&&(trackevent->fL2BNumStrip[ssdindex].size()==GMULTI_6)) && (strcmp(mode,MODE_G6_211)==0))
  {
    // g6-211 模式:
    //  0)  000         0) 000
    //  1)  010         1) 010
    //  2)  101         2) 100
    //  3)  111         3) 110
    //  4)  201         4) 201
    //  5)  211         5) 211
    // 根据 L1S 分成两类: 01-2345, 0123-45
    //
    g6_EL2B_Sum24 = trackevent->fL2BEMeV[ssdindex][2] + trackevent->fL2BEMeV[ssdindex][4];
    g6_EL2B_Sum35 = trackevent->fL2BEMeV[ssdindex][3] + trackevent->fL2BEMeV[ssdindex][5];
    g6_EL2B_Sum02 = trackevent->fL2BEMeV[ssdindex][0] + trackevent->fL2BEMeV[ssdindex][2];
    g6_EL2B_Sum13 = trackevent->fL2BEMeV[ssdindex][1] + trackevent->fL2BEMeV[ssdindex][3];
    //
    // L1S: 01-2345
    if (trackevent->fL1SNumStrip[ssdindex][1]!=trackevent->fL2BNumStrip[ssdindex][2])
    {
      // L2B sharing: 2)4) sharing, 2)为其中一条有效径迹, 1)为另一条有效径迹
      if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][2]-trackevent->fL2BNumStrip[ssdindex][4])==1) &&
          (IsEneConstraint_L2B_L2F(ssdindex,g6_EL2B_Sum24,trackevent->fL2FEMeV[ssdindex][2]) &&
           IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2FEMeV[ssdindex][2],trackevent->fL1SEMeV_Corrected[ssdindex][2])) &&
          (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
           IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL1SEMeV_Corrected[ssdindex][1])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==1) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
          if (itrack==2) {
            g6_L2B_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][2],trackevent->fL2BNumStrip[ssdindex][4],trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2BEMeV[ssdindex][4]);
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 g6_L2B_ChargeCenter,                        g6_EL2B_Sum24,
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // L2B sharing: 3)5) sharing, 3)为其中一条有效径迹, 0)为另一条有效径迹
      else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][3]-trackevent->fL2BNumStrip[ssdindex][5])==1) &&
               (IsEneConstraint_L2B_L2F(ssdindex,g6_EL2B_Sum35,trackevent->fL2FEMeV[ssdindex][3]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],trackevent->fL2FEMeV[ssdindex][3],trackevent->fL1SEMeV_Corrected[ssdindex][3])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==0) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
          if (itrack==3) {
            g6_L2B_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][3],trackevent->fL2BNumStrip[ssdindex][5],trackevent->fL2BEMeV[ssdindex][3],trackevent->fL2BEMeV[ssdindex][5]);
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 g6_L2B_ChargeCenter,                        g6_EL2B_Sum35,
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 不考虑 sharing 且两条有效径迹: 0)3)
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],trackevent->fL2FEMeV[ssdindex][3],trackevent->fL1SEMeV_Corrected[ssdindex][3])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==0 || itrack==3) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 不考虑 sharing 且两条有效径迹: 0)5)
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][5],trackevent->fL2FEMeV[ssdindex][5]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[5],trackevent->fL2FEMeV[ssdindex][5],trackevent->fL1SEMeV_Corrected[ssdindex][5])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==0 || itrack==5) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 不考虑 sharing 且两条有效径迹: 1)2)
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL1SEMeV_Corrected[ssdindex][1])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2FEMeV[ssdindex][2],trackevent->fL1SEMeV_Corrected[ssdindex][2])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==1 || itrack==2) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 不考虑 sharing 且两条有效径迹: 1)4)
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL1SEMeV_Corrected[ssdindex][1])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][4],trackevent->fL2FEMeV[ssdindex][4]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[4],trackevent->fL2FEMeV[ssdindex][4],trackevent->fL1SEMeV_Corrected[ssdindex][4])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==1 || itrack==4) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 只有一条径迹: 直接判选
      else
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
          {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
    }
    // L1S: 0123-45
    else if (trackevent->fL1SNumStrip[ssdindex][3]!=trackevent->fL2BNumStrip[ssdindex][4])
    {
      // L2B sharing: 0)2) sharing, 0)为其中一条有效径迹, 5)为另一条有效径迹
      if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][0]-trackevent->fL2BNumStrip[ssdindex][2])==1) &&
          (IsEneConstraint_L2B_L2F(ssdindex,g6_EL2B_Sum02,trackevent->fL2FEMeV[ssdindex][0]) &&
           IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0])) &&
          (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][5],trackevent->fL2FEMeV[ssdindex][5]) &&
           IsInsideABananaCut(BananaCut[ssdindex],fMCut[5],trackevent->fL2FEMeV[ssdindex][5],trackevent->fL1SEMeV_Corrected[ssdindex][5])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==0) {
            g6_L2B_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][0],trackevent->fL2BNumStrip[ssdindex][2],trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2BEMeV[ssdindex][2]);
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 g6_L2B_ChargeCenter,                        g6_EL2B_Sum02,
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
          if (itrack==5) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // L2B sharing: 1)3) sharing, 1)为其中一条有效径迹, 4)为另一条有效径迹
      else if ((TMath::Abs(trackevent->fL2BNumStrip[ssdindex][1]-trackevent->fL2BNumStrip[ssdindex][3])==1) &&
               (IsEneConstraint_L2B_L2F(ssdindex,g6_EL2B_Sum13,trackevent->fL2FEMeV[ssdindex][1]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL1SEMeV_Corrected[ssdindex][1])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][4],trackevent->fL2FEMeV[ssdindex][4]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[4],trackevent->fL2FEMeV[ssdindex][4],trackevent->fL1SEMeV_Corrected[ssdindex][4])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==1) {
            g6_L2B_ChargeCenter = GetChargeCenter(trackevent->fL2BNumStrip[ssdindex][1],trackevent->fL2BNumStrip[ssdindex][3],trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2BEMeV[ssdindex][3]);
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 g6_L2B_ChargeCenter,                        g6_EL2B_Sum13,
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
          if (itrack==4) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 不考虑 sharing 且两条有效径迹: 0)5)
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][0],trackevent->fL2FEMeV[ssdindex][0]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[0],trackevent->fL2FEMeV[ssdindex][0],trackevent->fL1SEMeV_Corrected[ssdindex][0])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][5],trackevent->fL2FEMeV[ssdindex][5]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[5],trackevent->fL2FEMeV[ssdindex][5],trackevent->fL1SEMeV_Corrected[ssdindex][5])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==0 || itrack==5) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 不考虑 sharing 且两条有效径迹: 1)4)
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][1],trackevent->fL2FEMeV[ssdindex][1]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[1],trackevent->fL2FEMeV[ssdindex][1],trackevent->fL1SEMeV_Corrected[ssdindex][1])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][4],trackevent->fL2FEMeV[ssdindex][4]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[4],trackevent->fL2FEMeV[ssdindex][4],trackevent->fL1SEMeV_Corrected[ssdindex][4])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==1 || itrack==4) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 不考虑 sharing 且两条有效径迹: 2)5)
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][2],trackevent->fL2FEMeV[ssdindex][2]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[2],trackevent->fL2FEMeV[ssdindex][2],trackevent->fL1SEMeV_Corrected[ssdindex][2])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][5],trackevent->fL2FEMeV[ssdindex][5]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[5],trackevent->fL2FEMeV[ssdindex][5],trackevent->fL1SEMeV_Corrected[ssdindex][5])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==2 || itrack==5) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 不考虑 sharing 且两条有效径迹: 3)4)
      else if ((IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][3],trackevent->fL2FEMeV[ssdindex][3]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[3],trackevent->fL2FEMeV[ssdindex][3],trackevent->fL1SEMeV_Corrected[ssdindex][3])) &&
               (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][4],trackevent->fL2FEMeV[ssdindex][4]) &&
                IsInsideABananaCut(BananaCut[ssdindex],fMCut[4],trackevent->fL2FEMeV[ssdindex][4],trackevent->fL1SEMeV_Corrected[ssdindex][4])))
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (itrack==3 || itrack==4) {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
          }
        }
      }
      // 只有一条径迹: 直接判选
      else
      {
        for (Int_t itrack=0; itrack<GMULTI_6; itrack++) {
          if (IsEneConstraint_L2B_L2F(ssdindex,trackevent->fL2BEMeV[ssdindex][itrack],trackevent->fL2FEMeV[ssdindex][itrack]) &&
              IsInsideABananaCut(BananaCut[ssdindex],fMCut[itrack],trackevent->fL2FEMeV[ssdindex][itrack],trackevent->fL1SEMeV_Corrected[ssdindex][itrack]))
          {
            FillGlobalEvent_L1L2(globalevent, GMULTI_6, MODEINDEX_G6_211, ssdindex, fSSDgMulti[ssdindex], fMCut[itrack].Cut_Z, fMCut[itrack].Cut_A,
                                 trackevent->fL1SNumStrip[ssdindex][itrack], trackevent->fL1SEMeV[ssdindex][itrack], trackevent->fL1SEMeV_Corrected[ssdindex][itrack],
                                 trackevent->fL2FNumStrip[ssdindex][itrack], trackevent->fL2FEMeV[ssdindex][itrack],
                                 trackevent->fL2BNumStrip[ssdindex][itrack], trackevent->fL2BEMeV[ssdindex][itrack],
                                 trackevent->fL2FTime[ssdindex][itrack]);
            break;
          }
        }
      }
      //
    }
  }
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
