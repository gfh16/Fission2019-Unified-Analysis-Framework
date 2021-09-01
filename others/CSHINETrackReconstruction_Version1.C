#include "../include/CSHINETrackReconstruction.h"

//__________________________________________________
// 经过以上 CheckClusterSize_Si() 的分析, charge charing
// 只需考虑 clustersize=2 的情况即可 ！
Double_t CalcRatio(Double_t e1, Double_t e2)
{
  return (e1 > e2 ? e2/e1 : e1/e2);
}

////////////////////////////////////////////////////////////////////////////////
//
//  此程序用于硅条望远镜的模式识别. 大致可以分为以下几部分:
//   1.SSD 各层的多重性
//   2.确定粒子入射位置的约束条件
//   3.L2F 与 L2B 能量关联
//   4.确定每个 SSD 的多重性
//   5.确定 SSD 阵列的多重性
//
//   a)添加函数 EstimateLayerMulti(Int_t fisrtrun, Int_t lastrun);
//
//
//  gfh, 2020-09-06
////////////////////////////////////////////////////////////////////////////////


//******************************************************************************
CSHINETrackReconstruction::CSHINETrackReconstruction()
{
  fFirstRun = 150;
  fLastRun  = 160;
}

CSHINETrackReconstruction::CSHINETrackReconstruction(Int_t firstrun, Int_t lastrun) : fChainLayerTree(0),fChainTrackTree(0)
{
  fFirstRun = firstrun;
  fLastRun = lastrun;

  std::string pathLayerRootFile(Form("%sCSHINEEvent/EventTree_Run%04d-Run%04d.root",PATHROOTFILESFOLDER,fFirstRun,fLastRun));
  std::string pathTrackRootFile(Form("%sL2L3_TrackReconstructionEvent_Run%04d-Run%04d.root",PATHROOTFILESFOLDER,fFirstRun,fLastRun));

  TTree* layertree = 0;
  TTree* tracktree = 0;;
  TFile* filelayertree = (TFile*) gROOT->GetListOfFiles()->FindObject(pathLayerRootFile.c_str());
  TFile* filetracktree = (TFile*) gROOT->GetListOfFiles()->FindObject(pathTrackRootFile.c_str());

  // for LayerEvent
  if (!filelayertree || !filelayertree->IsOpen()) {
    filelayertree = new TFile(pathLayerRootFile.c_str());
  }
  filelayertree->GetObject("LayerEvent",layertree);
  InitLayerTree(layertree);

  // for TrackEvent
  if (!filetracktree || !filetracktree->IsOpen()) {
    filetracktree = new TFile(pathTrackRootFile.c_str());
  }
  filetracktree->GetObject("TrackEvent",tracktree);
  InitTrackTree(tracktree);
}

//______________________________________________________________________________
CSHINETrackReconstruction::~CSHINETrackReconstruction()
{
  if (!fChainLayerTree) return;
  if (!fChainTrackTree) return;
  //cout<<"Exit class CSHINETrackReconstruction!"<<endl;
}

// init layer tree
void CSHINETrackReconstruction::InitLayerTree(TTree* tree)
{
  // Set branch addresses and branch pointers
  if (!tree) return;
  fChainLayerTree = tree;
  fChainLayerTree->SetMakeClass(1);

  fChainLayerTree->SetBranchAddress("LayerEvent.fL1SMulti",    &fLayerEvent.fL1SMulti);
  fChainLayerTree->SetBranchAddress("LayerEvent.fL1SSSDNum",   &fLayerEvent.fL1SSSDNum);
  fChainLayerTree->SetBranchAddress("LayerEvent.fL1SNumStrip", &fLayerEvent.fL1SNumStrip);
  fChainLayerTree->SetBranchAddress("LayerEvent.fL1SEMeV",     &fLayerEvent.fL1SEMeV);
  fChainLayerTree->SetBranchAddress("LayerEvent.fL2FMulti",    &fLayerEvent.fL2FMulti);
  fChainLayerTree->SetBranchAddress("LayerEvent.fL2FSSDNum",   &fLayerEvent.fL2FSSDNum);
  fChainLayerTree->SetBranchAddress("LayerEvent.fL2FNumStrip", &fLayerEvent.fL2FNumStrip);
  fChainLayerTree->SetBranchAddress("LayerEvent.fL2FEMeV",     &fLayerEvent.fL2FEMeV);
  fChainLayerTree->SetBranchAddress("LayerEvent.fL2FTime",     &fLayerEvent.fL2FTime);
  fChainLayerTree->SetBranchAddress("LayerEvent.fL2BMulti",    &fLayerEvent.fL2BMulti);
  fChainLayerTree->SetBranchAddress("LayerEvent.fL2BSSDNum",   &fLayerEvent.fL2BSSDNum);
  fChainLayerTree->SetBranchAddress("LayerEvent.fL2BNumStrip", &fLayerEvent.fL2BNumStrip);
  fChainLayerTree->SetBranchAddress("LayerEvent.fL2BEMeV",     &fLayerEvent.fL2BEMeV);
  fChainLayerTree->SetBranchAddress("LayerEvent.fCsIMulti",    &fLayerEvent.fCsIMulti);
  fChainLayerTree->SetBranchAddress("LayerEvent.fCsISSDNum",   &fLayerEvent.fCsISSDNum);
  fChainLayerTree->SetBranchAddress("LayerEvent.fCsINum",      &fLayerEvent.fCsINum);
  fChainLayerTree->SetBranchAddress("LayerEvent.fCsIECh",      &fLayerEvent.fCsIECh);
  fChainLayerTree->SetBranchAddress("LayerEvent.fSSDL1SMulti", &fLayerEvent.fSSDL1SMulti);
  fChainLayerTree->SetBranchAddress("LayerEvent.fSSDL2FMulti", &fLayerEvent.fSSDL2FMulti);
  fChainLayerTree->SetBranchAddress("LayerEvent.fSSDL2BMulti", &fLayerEvent.fSSDL2BMulti);
  fChainLayerTree->SetBranchAddress("LayerEvent.fSSDCsIMulti", &fLayerEvent.fSSDCsIMulti);
}

// init track tree
void CSHINETrackReconstruction::InitTrackTree(TTree* tree)
{
  // Set branch addresses and branch pointers
  if (!tree) return;
  fChainTrackTree = tree;
  fChainTrackTree->SetMakeClass(1);

  fChainTrackTree->SetBranchAddress("TrackEvent.fGlobalMulti",   &fTrackEvent.fGlobalMulti);
  fChainTrackTree->SetBranchAddress("TrackEvent.fSSDGlobalMulti",&fTrackEvent.fSSDGlobalMulti);
  fChainTrackTree->SetBranchAddress("TrackEvent.fGSSDNum",       &fTrackEvent.fGSSDNum);
  fChainTrackTree->SetBranchAddress("TrackEvent.fGL1SNumStrip",  &fTrackEvent.fGL1SNumStrip);
  fChainTrackTree->SetBranchAddress("TrackEvent.fGL1SEMeV",      &fTrackEvent.fGL1SEMeV);
  fChainTrackTree->SetBranchAddress("TrackEvent.fGL2FNumStrip",  &fTrackEvent.fGL2FNumStrip);
  fChainTrackTree->SetBranchAddress("TrackEvent.fGL2FEMeV",      &fTrackEvent.fGL2FEMeV);
  fChainTrackTree->SetBranchAddress("TrackEvent.fGL2BNumStrip",  &fTrackEvent.fGL2BNumStrip);
  fChainTrackTree->SetBranchAddress("TrackEvent.fGL2BEMeV",      &fTrackEvent.fGL2BEMeV);
  fChainTrackTree->SetBranchAddress("TrackEvent.fGCsINum",       &fTrackEvent.fGCsINum);
  fChainTrackTree->SetBranchAddress("TrackEvent.fGCsIECh",       &fTrackEvent.fGCsIECh);
  fChainTrackTree->SetBranchAddress("TrackEvent.fGL2FTime",      &fTrackEvent.fGL2FTime);
}
//******************************************************************************



//******************************************************************************
//                            1. SSD 各层的多重性
//
// 计算每一层的多重数
Int_t CSHINETrackReconstruction::LayerMultiplicity(Int_t ssdindex, const char* layertag,
  Int_t* ech, Double_t* echcut)
{
  Int_t multi = 0;
  Int_t NUM_CH;

  if (strcmp(layertag, "L3A") == 0) {
    NUM_CH = NUM_CSI;     // for CsI
  } else {
    NUM_CH = NUM_STRIP;   // for Si
  }

  for (Int_t i=0; i<NUM_CH; i++) {
    if (ech[i]>echcut[ssdindex*NUM_CH + i]) {
      multi++;
    }
  }
  return multi;
}
//******************************************************************************



//******************************************************************************
//                          2. 确定粒子入射位置的约束条件
//                        -----------------------------
// 几何判据一: GeoConstraint_L3A_L2B
//---------------------------------
// One_CsI <---> One_L2BStrip
Bool_t CSHINETrackReconstruction::IsGeoConstraint_L3A_L2B(Int_t csiindex, Int_t stripl2b)
{
  Bool_t index = false;
  if ((csiindex/3)==0 && (stripl2b>=10 && stripl2b<=15)) index = true; // csiindex = 0, 1, 2
  if ((csiindex/3)==1 && (stripl2b>=5  && stripl2b<=10)) index = true; // csiindex = 3, 4, 5
  if ((csiindex/3)==2 && (stripl2b>=0  && stripl2b<=5))  index = true; // csiindex = 6, 7, 8
  return (index==true) ? true : false;
}

// One_CsI <---> L2B_Surface
Bool_t CSHINETrackReconstruction::IsGeoConstraint_L3A_L2B(Int_t ssdindex, Int_t csinum,
  Int_t csissdnum, Int_t l2bmulti, Int_t* l2bstrip, Int_t* l2bssdnum)
{
  Int_t index = 0;
  for (Int_t l2b=0; l2b<l2bmulti; l2b++) {
    if (csissdnum==ssdindex && l2bssdnum[l2b]==ssdindex) {
      if (csinum/3==0 && (l2bstrip[l2b]>=10 && l2bstrip[l2b]<=15)) index++;
      if (csinum/3==1 && (l2bstrip[l2b]>=5  && l2bstrip[l2b]<=10)) index++;
      if (csinum/3==2 && (l2bstrip[l2b]>=0  && l2bstrip[l2b]<=5))  index++;
    }
  }
  return (index != 0) ? true : false;
}

// CsI_Array <---> One_L2BStrip
Bool_t CSHINETrackReconstruction::IsGeoConstraint_L3A_L2B(Int_t ssdindex, Int_t csimulti,
  Int_t* csinum, Int_t* csissdnum, Int_t l2bstrip, Int_t l2bssdnum)
{
  Int_t index = 0;
  for (Int_t csi=0; csi<csimulti; csi++) {
    if (csissdnum[csi]==ssdindex && l2bssdnum==ssdindex) {
      if (csinum[csi]/3==0 && (l2bstrip>=10 && l2bstrip<=15)) index++;
      if (csinum[csi]/3==1 && (l2bstrip>=5  && l2bstrip<=10)) index++;
      if (csinum[csi]/3==2 && (l2bstrip>=0  && l2bstrip<=5))  index++;
    }
  }
  return (index != 0) ? true : false;
}


// Csi_Array <---> L2B_Surface
Bool_t CSHINETrackReconstruction::IsGeoConstraint_L3A_L2B(Int_t ssdindex, Int_t csimulti,
  Int_t* csinum, Int_t* csissdnum, Int_t l2bmulti, Int_t* l2bstrip, Int_t* l2bssdnum)
{
  Int_t index = 0;
  for (Int_t csi=0; csi<csimulti; csi++) {
    for (Int_t l2b=0; l2b<l2bmulti; l2b++) {
      if (csissdnum[csi]==ssdindex && l2bssdnum[l2b]==ssdindex) {
        if (csinum[csi]/3==0 && (l2bstrip[l2b]>=10 && l2bstrip[l2b]<=15)) index++;
        if (csinum[csi]/3==1 && (l2bstrip[l2b]>=5  && l2bstrip[l2b]<=10)) index++;
        if (csinum[csi]/3==2 && (l2bstrip[l2b]>=0  && l2bstrip[l2b]<=5))  index++;
      }
    }
  }
  return (index != 0) ? true : false;
}

//______________________________________________________________________________
// 几何判据二: GeoConstraint_L3A_L2F
//---------------------------------
// One_CsI <---> One_L2FStrip
Bool_t CSHINETrackReconstruction::IsGeoConstraint_L3A_L2F(Int_t csiindex, Int_t stripl2f)
{
  Bool_t index = false;
  if ((csiindex%3)==0 && (stripl2f>=10 && stripl2f<=15)) index = true; // csiindex = 0, 3, 6
  if ((csiindex%3)==1 && (stripl2f>=5  && stripl2f<=10)) index = true; // csiindex = 1, 4, 7
  if ((csiindex%3)==2 && (stripl2f>=0  && stripl2f<=5))  index = true; // csiindex = 2, 5, 8
  return (index==true) ? true : false;
}

// CsI_Array <---> One_L2FStrip
Bool_t CSHINETrackReconstruction::IsGeoConstraint_L3A_L2F(Int_t ssdindex, Int_t csimulti,
  Int_t* csinum, Int_t* csissdnum, Int_t l2fstrip, Int_t l2fssdnum)
{
  Int_t index = 0;
  for (Int_t csi=0; csi<csimulti; csi++) {
    if (csissdnum[csi]==ssdindex && l2fssdnum==ssdindex) {
      if (csinum[csi]%3==0 && (l2fstrip>=10 && l2fstrip<=15)) index++;
      if (csinum[csi]%3==1 && (l2fstrip>=5  && l2fstrip<=10)) index++;
      if (csinum[csi]%3==2 && (l2fstrip>=0  && l2fstrip<=5))  index++;
    }
  }
  return (index != 0) ? true : false;
}

// One_CsI <---> L2F_Surface
Bool_t CSHINETrackReconstruction::IsGeoConstraint_L3A_L2F(Int_t ssdindex, Int_t csinum,
  Int_t csissdnum, Int_t l2fmulti, Int_t* l2fstrip, Int_t* l2fssdnum)
{
  Int_t index = 0;
  for (Int_t l2f=0; l2f<l2fmulti; l2f++) {
    if (csissdnum==ssdindex && l2fssdnum[l2f]==ssdindex) {
      if (csinum%3==0 && (l2fstrip[l2f]>=10 && l2fstrip[l2f]<=15)) index++;
      if (csinum%3==1 && (l2fstrip[l2f]>=5  && l2fstrip[l2f]<=10)) index++;
      if (csinum%3==2 && (l2fstrip[l2f]>=0  && l2fstrip[l2f]<=5))  index++;
    }
  }
  return (index != 0) ? true : false;
}

// CsI_Array <---> L2F_Surface
Bool_t CSHINETrackReconstruction::IsGeoConstraint_L3A_L2F(Int_t ssdindex, Int_t csimulti,
  Int_t* csinum, Int_t* csissdnum, Int_t l2fmulti, Int_t* l2fstrip, Int_t* l2fssdnum)
{
  Int_t index = 0;
  // 注意: 逆向查找时, 先对 L3A 作循环, 还是对 L2B作循环, 结果稍有差异
  for (Int_t csi=0; csi<csimulti; csi++) {
    for (Int_t l2f=0; l2f<l2fmulti; l2f++) {
      if (csissdnum[csi]==ssdindex && l2fssdnum[l2f]==ssdindex) {
        if (csinum[csi]%3==0 && (l2fstrip[l2f]>=10 && l2fstrip[l2f]<=15)) index++;
        if (csinum[csi]%3==1 && (l2fstrip[l2f]>=5  && l2fstrip[l2f]<=10)) index++;
        if (csinum[csi]%3==2 && (l2fstrip[l2f]>=0  && l2fstrip[l2f]<=5))  index++;
      }
    }
  }
  return (index != 0) ? true : false;
}

//______________________________________________________________________________
// 几何判据三: GeoConstraint_L2B_L1S
//-----------------------------------
// One_L2BStrip <---> One_L1SStrip
Bool_t CSHINETrackReconstruction::IsGeoConstraint_L2B_L1S(Int_t stripl2b, Int_t stripl1s)
{
  return (TMath::Abs(stripl2b - stripl1s)<=1);
}

// One_L2BStrip <---> One_L1SStrip
Bool_t CSHINETrackReconstruction::IsGeoConstraint_L2B_L1S(Int_t stripl2b, Int_t stripl1s, Int_t deltastrip)
{
  return (TMath::Abs(stripl2b - stripl1s)==deltastrip);
}

// One_L2BStrip <---> L1S_Surface
Bool_t CSHINETrackReconstruction::IsGeoConstraint_L2B_L1S(Int_t ssdindex, Int_t l2bstrip,
  Int_t l2bssdnum, Int_t l1smulti, Int_t* l1sstrip, Int_t* l1sssdnum)
{
  Int_t index = 0;
  for (Int_t l1s=0; l1s<l1smulti; l1s++) {
    if (l2bssdnum==ssdindex && l1sssdnum[l1s]==ssdindex) {
      if (TMath::Abs(l2bstrip - l1sstrip[l1s])<=1) index++ ;
    }
  }
  return (index != 0) ? true : false;
}

// L2B_Surface <---> One_L1SStrip
Bool_t CSHINETrackReconstruction::IsGeoConstraint_L2B_L1S(Int_t ssdindex, Int_t l2bmulti,
  Int_t* l2bstrip, Int_t* l2bssdnum, Int_t l1sstrip, Int_t l1sssdnum)
{
  Int_t index = 0;
  for (Int_t l2b=0; l2b<l2bmulti; l2b++) {
    if (l2bssdnum[l2b]==ssdindex && l1sssdnum==ssdindex) {
      if (TMath::Abs(l2bstrip[l2b] - l1sstrip)<=1) index++ ;
    }
  }
  return (index != 0) ? true : false;
}

// L2B_Surface <---> L1S_Surface
Bool_t CSHINETrackReconstruction::IsGeoConstraint_L2B_L1S(Int_t ssdindex, Int_t l2bmulti,
  Int_t* l2bstrip, Int_t* l2bssdnum, Int_t l1smulti, Int_t* l1sstrip, Int_t* l1sssdnum)
{
  Int_t index = 0;
  for (Int_t l2b=0; l2b<l2bmulti; l2b++) {
    for (Int_t l1s=0; l1s<l1smulti; l1s++) {
      if (l2bssdnum[l2b]==ssdindex && l1sssdnum[l1s]==ssdindex) {
        if (TMath::Abs(l2bstrip[l2b] - l1sstrip[l1s])<=1) index++ ;
      }
    }
  }
  return (index != 0) ? true : false;
}


//______________________________________________________________________________
// 能量判据四: 考虑 L2B 与 L2F 的能量关联
//-------------------------------------
// One_L2BStrip <---> One_L2FStrip
Bool_t CSHINETrackReconstruction::IsEneConstraint_L2B_L2F(Double_t El2b, Double_t El2f, Double_t ErrRatio)
{
  return (TMath::Abs(El2b - El2f)/El2f < ErrRatio);
}

// One_L2BStrip <---> L2F_Surface
Bool_t CSHINETrackReconstruction::IsEneConstraint_L2B_L2F(Int_t ssdindex, Double_t El2b,
  Int_t l2bssdnum, Int_t l2fmulti, Double_t* El2f, Int_t* l2fssdnum, Double_t ErrRatio)
{
  Int_t index = 0;
  for (Int_t l2f=0; l2f<l2fmulti; l2f++) {
    if (l2bssdnum==ssdindex && l2fssdnum[l2f]==ssdindex) {
      if (TMath::Abs(El2b - El2f[l2f])/El2f[l2f] < ErrRatio)  index++ ;
    }
  }
  return (index != 0) ? true : false;
}

// L2B_Surface <---> One_L2FStrip
Bool_t CSHINETrackReconstruction::IsEneConstraint_L2B_L2F(Int_t ssdindex, Int_t l2bmulti,
  Double_t* El2b, Int_t* l2bssdnum, Double_t El2f, Int_t l2fssdnum, Double_t ErrRatio)
{
  Int_t index = 0;
  for (Int_t l2b=0; l2b<l2bmulti; l2b++) {
    if (l2bssdnum[l2b]==ssdindex && l2fssdnum==ssdindex) {
      if (TMath::Abs(El2b[l2b] - El2f)/El2f < ErrRatio)  index++ ;
    }
  }
  return (index != 0) ? true : false;
}

// L2B_Surface <---> L2F_Surface
Bool_t CSHINETrackReconstruction::IsEneConstraint_L2B_L2F(Int_t ssdindex, Int_t l2bmulti,
  Double_t* El2b, Int_t* l2bssdnum, Int_t l2fmulti, Double_t* El2f, Int_t* l2fssdnum, Double_t ErrRatio)
{
  Int_t index = 0;
  for (Int_t l2b=0; l2b<l2bmulti; l2b++) {
    for (Int_t l2f=0; l2f<l2fmulti; l2f++) {
      if (l2bssdnum[l2b]==ssdindex && l2fssdnum[l2f]==ssdindex) {
        if (TMath::Abs(El2b[l2b] - El2f[l2f])/El2f[l2f] < ErrRatio)  index++ ;
      }
    }
  }
  return (index != 0) ? true : false;
}

// L2B_Surface <---> L2F_Surface
Bool_t CSHINETrackReconstruction::IsEneConstraint_L2B_L2F(Int_t ssdindex, Int_t l2bmulti,
  Double_t* El2b, Int_t* l2bssdnum, Int_t l2fmulti, Double_t* El2f, Int_t* l2fssdnum)
{
  Int_t index = 0;
  for (Int_t l2b=0; l2b<l2bmulti; l2b++) {
    for (Int_t l2f=0; l2f<l2fmulti; l2f++) {
      if (l2bssdnum[l2b]==ssdindex && l2fssdnum[l2f]==ssdindex)
      {
        if (El2f[l2f]<L2BL2F_ENERGYBOUNDARY[ssdindex])
        {
          if ((El2b[l2b]-El2f[l2f])/El2f[l2f]>L2BL2F_LOWENERGYLOWCUT[ssdindex] &&
              (El2b[l2b]-El2f[l2f])/El2f[l2f]<L2BL2F_LOWENERGYHIGHCUT[ssdindex])
          {
            index++ ;
          }
        }
        else
        {
          if ((El2b[l2b]-El2f[l2f])/El2f[l2f]>L2BL2F_HIGHENERGYLOWCUT[ssdindex] &&
              (El2b[l2b]-El2f[l2f])/El2f[l2f]<L2BL2F_HIGHENERGYHIGHCUT[ssdindex])
          {
            index++ ;
          }
        }
      }
    }
  }
  return (index != 0) ? true : false;
}

// 根据前面的测试结果, 确定每个 SSD 的 L2B-L2F 能量关联
Bool_t CSHINETrackReconstruction::IsEneConstraint_L2B_L2F(Int_t ssdindex, Double_t El2b, Double_t El2f)
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
// 能量判据 五 : 对于能穿透 L2 的粒子, 通过 LISE 计算给出 L1, L2 的能损比例, 作为新的约束条件
//--------------------------------
// One_L1SStrip <---> One_L2FStrip
Bool_t CSHINETrackReconstruction::IsEneConstraint_L1S_L2F(Int_t ssdindex, Double_t El1s, Double_t El2f)
{
  return ((El1s/El2f>L1L2_ENERGYLOWCUT[ssdindex]) && (El1s/El2f<L1L2_ENERGYHIGHCUT[ssdindex]));
}

//
Bool_t CSHINETrackReconstruction::IsEneConstraint_L1S_L2F(Double_t El1s, Double_t El2f, Double_t eratio)
{
  Bool_t index = kFALSE;
  if ((El1s/El2f>0) && (El1s/El2f<eratio)) index = kTRUE;
  return index;
}

// L1S_Surface <---> L2F_Surface
Bool_t CSHINETrackReconstruction::IsEneConstraint_L1S_L2F(Int_t ssdindex, Int_t l1smulti,
  Double_t* El1s, Int_t* l1sssdnum, Int_t l2fmulti, Double_t* El2f, Int_t* l2fssdnum)
{
  Int_t index = 0;
  for (Int_t l2f=0; l2f<l2fmulti; l2f++) {
    for (Int_t l1s=0; l1s<l1smulti; l1s++) {
      if (l2fssdnum[l2f]==ssdindex && l1sssdnum[l1s]==ssdindex) {
        if ((El1s[l1s]/El2f[l2f]>L1L2_ENERGYLOWCUT[ssdindex]) && (El1s[l1s]/El2f[l2f]<L1L2_ENERGYHIGHCUT[ssdindex]))
        {
          index++;
        }
      }
    }
  }
  return (index != 0) ? true : false;
}


//______________________________________________________________________________
// 判断 EL1S 与使用 LISE 反推的 EL1S_LISE是否小于给定的误差
Bool_t CSHINETrackReconstruction::IsEneConstraint_EL1S_ELISECalc(Double_t el1s, Double_t eLISE, Double_t errratio)
{
  Bool_t index;
  if (TMath::Abs(el1s-eLISE)/el1s < errratio) { index = kTRUE; }
  else { index = kFALSE; }

  return index;
}


//______________________________________________________________________________
// 判断一个给定的能量点是否在某个 bannana cut 内
Bool_t CSHINETrackReconstruction::IsInsideABananaCut(std::vector<TCutG*> cut, Double_t e2, Double_t de1)
{
  Bool_t index = kFALSE;
  for (Int_t particle=0; particle<cut.size(); particle++) {
    if (cut[particle]->IsInside(e2, de1)) {
      index = kTRUE;
      break;
    }
  }
  return index;
}

Bool_t CSHINETrackReconstruction::IsInsideABananaCut(std::vector<TCutG*> cut, std::string& cutname, Double_t e2, Double_t de1)
{
  Bool_t index = kFALSE;
  for (Int_t particle=0; particle<cut.size(); particle++) {
    if (cut[particle]->IsInside(e2, de1)) {
      index = kTRUE;
      cutname = cut[particle]->GetName();
      break;
    }
  }
  return index;
}

void CSHINETrackReconstruction::GetZAFromBananaCutName(std::string cutname, Int_t& Z, Int_t& A)
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
// 根据 L2F, L2B 的位置, 确定对应的 CsI 位置
Int_t CSHINETrackReconstruction::GetCsINumFromPixel(Int_t stripl2f, Int_t stripl2b)
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
//******************************************************************************



//******************************************************************************
// 创建函数 EstimateLayerMulti(Int_t fisrtrun, Int_t lastrun)
void AddNumOfCount(Int_t multi, Int_t multicut, Int_t& emptyentries, Int_t& effentries, Int_t& entrieswithmulticut)
{
  if (multi == 0) {          // 空的事件数
    emptyentries++;
  } else {
    effentries++;            // 有效事件数 (multi >= 1)
    if (multi <= multicut) {
      entrieswithmulticut++; // 1 <= 多重数 <= multi 的事件数
    }
  }
}
//______________________________________________________________________________
Double_t EventRatio(Int_t ssdindex, const char* layertag, Int_t entries_large, Int_t entries_small)
{
  Double_t ratio;
  if (entries_large == 0) {
    cout<<Form("Error: Multiplicity of SSD%d_%s is 0!",ssdindex+1, layertag)<<endl;
  } else {
    ratio = (Double_t) entries_small/entries_large;
  }
  return ratio;
}
//______________________________________________________________________________
////////////////////////////////////////////////////////////////////////////////
//
//  考察、确定每一层的粒子多重性,
//  实质上是，确定每一层的 cut 取值，思路如下:
//  1. 对于每一层，考虑 cut = pedestal + N * sigma
//  2. 画出不同 N下, 1 <= multi <= MultiCut 的事件数占有效 multi >= 1 事件数的比率
//  3. 根据比率的分布，确定 N 的取值
//
//  gfh, 2020-09-02
////////////////////////////////////////////////////////////////////////////////
void CSHINETrackReconstruction::EstimateLayerMulti(Int_t firstrun, Int_t lastrun)
{

  gStyle->SetPalette(1);

  TimeAndPercentage time;
  time.GetBeginTime();

  const Int_t NPoints    =  15;
  Int_t       MultiCut   =  2;
  Int_t       Color[4]   =  {kRed-4, kMagenta, kViolet+7, kOrange-3};
  Int_t       Marker[4]  =  {20, 21, 22, 33};
  Double_t    MarkerSize =  1.2;
  Double_t    SigmaIndex[NPoints] =  {1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.};

  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string L3ATag("L3A");

  std::string pathRootInputFolder(Form("%sMapRoot/", PATHROOTFILESFOLDER));
  std::string pathLayerMultiPNDOut(Form("%sfigure_TrackReconstruction/Layer_MultiRatio_Run%03d-Run%03d.png",PATHFIGURESFOLDER,firstrun,lastrun));
  std::string pathSSDMultiPNDOut  (Form("%sfigure_TrackReconstruction/SSD_MultiRatio_Run%03d-Run%03d.png",  PATHFIGURESFOLDER,firstrun,lastrun));
  std::string pathMultiCheckOut[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    pathMultiCheckOut[i] = Form("%sfigure_TrackReconstruction/SSD%d_MultiCheck_Run%03d-Run%03d.png",PATHFIGURESFOLDER,i+1,firstrun,lastrun);
  }

  TChain* mychain = new TChain("KrPb");
  for (Int_t i=firstrun; i<(lastrun+1); i++) {
    mychain->Add(Form("%sMapFission2019_Kr_Pb%04d.root",pathRootInputFolder.c_str(),i));
  }
  mychain->SetBranchStatus("*",false);

  Int_t L1SMulti[NUM_SSD][NPoints]  =  {{0}};
  Int_t L2FMulti[NUM_SSD][NPoints]  =  {{0}};
  Int_t L2BMulti[NUM_SSD][NPoints]  =  {{0}};
  Int_t L3AMulti[NUM_SSD][NPoints]  =  {{0}};

  Int_t L1SNumOfEmptyEntries[NUM_SSD][NPoints]         =  {{0}};
  Int_t L2FNumOfEmptyEntries[NUM_SSD][NPoints]         =  {{0}};
  Int_t L2BNumOfEmptyEntries[NUM_SSD][NPoints]         =  {{0}};
  Int_t L3ANumOfEmptyEntries[NUM_SSD][NPoints]         =  {{0}};

  Int_t L1SNumOfEffEntries[NUM_SSD][NPoints]           =  {{0}};
  Int_t L2FNumOfEffEntries[NUM_SSD][NPoints]           =  {{0}};
  Int_t L2BNumOfEffEntries[NUM_SSD][NPoints]           =  {{0}};
  Int_t L3ANumOfEffEntries[NUM_SSD][NPoints]           =  {{0}};

  Int_t L1SNumOfEntriesWithMultiCut[NUM_SSD][NPoints]  =  {{0}};
  Int_t L2FNumOfEntriesWithMultiCut[NUM_SSD][NPoints]  =  {{0}};
  Int_t L2BNumOfEntriesWithMultiCut[NUM_SSD][NPoints]  =  {{0}};
  Int_t L3ANumOfEntriesWithMultiCut[NUM_SSD][NPoints]  =  {{0}};

  Double_t L1SMultiRatio[NUM_SSD][NPoints]             =  {{0}};
  Double_t L2FMultiRatio[NUM_SSD][NPoints]             =  {{0}};
  Double_t L2BMultiRatio[NUM_SSD][NPoints]             =  {{0}};
  Double_t L3AMultiRatio[NUM_SSD][NPoints]             =  {{0}};

  Double_t L1SEffRatio[NUM_SSD][NPoints]               =  {{0}};
  Double_t L2FEffRatio[NUM_SSD][NPoints]               =  {{0}};
  Double_t L2BEffRatio[NUM_SSD][NPoints]               =  {{0}};
  Double_t L3AEffRatio[NUM_SSD][NPoints]               =  {{0}};

  Double_t L1SEmptyRatio[NUM_SSD][NPoints]             =  {{0}};
  Double_t L2FEmptyRatio[NUM_SSD][NPoints]             =  {{0}};
  Double_t L2BEmptyRatio[NUM_SSD][NPoints]             =  {{0}};
  Double_t L3AEmptyRatio[NUM_SSD][NPoints]             =  {{0}};

  Double_t L1SSumRatio[NUM_SSD][NPoints]               =  {{0}};
  Double_t L2FSumRatio[NUM_SSD][NPoints]               =  {{0}};
  Double_t L2BSumRatio[NUM_SSD][NPoints]               =  {{0}};
  Double_t L3ASumRatio[NUM_SSD][NPoints]               =  {{0}};

  Int_t SSD_L1S_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2F_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2B_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L3A_E[NUM_SSD][NUM_CSI];

  Double_t* L1SEChCut[NPoints];
  Double_t* L2FEChCut[NPoints];
  Double_t* L2BEChCut[NPoints];
  Double_t* L3AEChCut[NPoints];

  CSHINESSDCalibratedData   calidata[NPoints];

  // 计算每一层的 EChCut
  for (Int_t np=0; np<NPoints; np++) {
    L1SEChCut[np] = calidata[np].GetSiEChCut (L1STag.c_str(), SigmaIndex[np]);
    L2FEChCut[np] = calidata[np].GetSiEChCut (L2FTag.c_str(), SigmaIndex[np]);
    L2BEChCut[np] = calidata[np].GetSiEChCut (L2BTag.c_str(), SigmaIndex[np]);
    L3AEChCut[np] = calidata[np].GetCsIEChCut(L3ATag.c_str(), SigmaIndex[np]);
  }

  // set branchadress
  ReadFileModule readfile;
  for (Int_t i=0; i<NUM_SSD; i++) {
    readfile.AddChain(mychain, L1STag.c_str(), SSD_L1S_E[i], NUM_STRIP, i);
    readfile.AddChain(mychain, L2FTag.c_str(), SSD_L2F_E[i], NUM_STRIP, i);
    readfile.AddChain(mychain, L2BTag.c_str(), SSD_L2B_E[i], NUM_STRIP, i);
    readfile.AddChain(mychain, L3ATag.c_str(), SSD_L3A_E[i], NUM_CSI,   i);
  }

  //____________________________________________________________________________
  //                       逐一读取事件进行分析
  unsigned long nentries = mychain->GetEntries();
  cout<< "Found " << nentries <<" entries"<<endl;
  for (unsigned long jentry=0; jentry<nentries; jentry++) {

    mychain->GetEntry(jentry);

   for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
     for (Int_t np=0; np<NPoints; np++) {
       L1SMulti[ssdindex][np] = LayerMultiplicity(ssdindex, L1STag.c_str(), SSD_L1S_E[ssdindex], L1SEChCut[np]);
       L2FMulti[ssdindex][np] = LayerMultiplicity(ssdindex, L2FTag.c_str(), SSD_L2F_E[ssdindex], L2FEChCut[np]);
       L2BMulti[ssdindex][np] = LayerMultiplicity(ssdindex, L2BTag.c_str(), SSD_L2B_E[ssdindex], L2BEChCut[np]);
       L3AMulti[ssdindex][np] = LayerMultiplicity(ssdindex, L3ATag.c_str(), SSD_L3A_E[ssdindex], L3AEChCut[np]);

       AddNumOfCount(L1SMulti[ssdindex][np], MultiCut, L1SNumOfEmptyEntries[ssdindex][np], L1SNumOfEffEntries[ssdindex][np], L1SNumOfEntriesWithMultiCut[ssdindex][np]);
       AddNumOfCount(L2FMulti[ssdindex][np], MultiCut, L2FNumOfEmptyEntries[ssdindex][np], L2FNumOfEffEntries[ssdindex][np], L2FNumOfEntriesWithMultiCut[ssdindex][np]);
       AddNumOfCount(L2BMulti[ssdindex][np], MultiCut, L2BNumOfEmptyEntries[ssdindex][np], L2BNumOfEffEntries[ssdindex][np], L2BNumOfEntriesWithMultiCut[ssdindex][np]);
       AddNumOfCount(L3AMulti[ssdindex][np], MultiCut, L3ANumOfEmptyEntries[ssdindex][np], L3ANumOfEffEntries[ssdindex][np], L3ANumOfEntriesWithMultiCut[ssdindex][np]);
     }
   }
   time.PrintPercentageAndRemainingTime(jentry, nentries);
  }

  //____________________________________________________________________________
  // 计算 MultiRatio
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t np=0; np<NPoints; np++) {
      // multi ratio
      L1SMultiRatio[ssdindex][np] = EventRatio(ssdindex,L1STag.c_str(),L1SNumOfEffEntries[ssdindex][np],
                                      L1SNumOfEntriesWithMultiCut[ssdindex][np]);
      L2FMultiRatio[ssdindex][np] = EventRatio(ssdindex,L2FTag.c_str(),L2FNumOfEffEntries[ssdindex][np],
                                      L2FNumOfEntriesWithMultiCut[ssdindex][np]);
      L2BMultiRatio[ssdindex][np] = EventRatio(ssdindex,L2BTag.c_str(),L2BNumOfEffEntries[ssdindex][np],
                                      L2BNumOfEntriesWithMultiCut[ssdindex][np]);
      L3AMultiRatio[ssdindex][np] = EventRatio(ssdindex,L3ATag.c_str(),L3ANumOfEffEntries[ssdindex][np],
                                      L3ANumOfEntriesWithMultiCut[ssdindex][np]);
      // effective event ratio
      L1SEffRatio[ssdindex][np]   = EventRatio(ssdindex,L1STag.c_str(),nentries,L1SNumOfEffEntries[ssdindex][np]);
      L2FEffRatio[ssdindex][np]   = EventRatio(ssdindex,L2FTag.c_str(),nentries,L2FNumOfEffEntries[ssdindex][np]);
      L2BEffRatio[ssdindex][np]   = EventRatio(ssdindex,L2BTag.c_str(),nentries,L2BNumOfEffEntries[ssdindex][np]);
      L3AEffRatio[ssdindex][np]   = EventRatio(ssdindex,L3ATag.c_str(),nentries,L3ANumOfEffEntries[ssdindex][np]);

      // empty event ratio
      L1SEmptyRatio[ssdindex][np] = EventRatio(ssdindex,L1STag.c_str(),nentries,L1SNumOfEmptyEntries[ssdindex][np]);
      L2FEmptyRatio[ssdindex][np] = EventRatio(ssdindex,L2FTag.c_str(),nentries,L2FNumOfEmptyEntries[ssdindex][np]);
      L2BEmptyRatio[ssdindex][np] = EventRatio(ssdindex,L2BTag.c_str(),nentries,L2BNumOfEmptyEntries[ssdindex][np]);
      L3AEmptyRatio[ssdindex][np] = EventRatio(ssdindex,L3ATag.c_str(),nentries,L3ANumOfEmptyEntries[ssdindex][np]);

      // sum ratio
      L1SSumRatio[ssdindex][np]   = L1SEffRatio[ssdindex][np] + L1SEmptyRatio[ssdindex][np];
      L2FSumRatio[ssdindex][np]   = L2FEffRatio[ssdindex][np] + L2FEmptyRatio[ssdindex][np];
      L2BSumRatio[ssdindex][np]   = L2BEffRatio[ssdindex][np] + L2BEmptyRatio[ssdindex][np];
      L3ASumRatio[ssdindex][np]   = L3AEffRatio[ssdindex][np] + L3AEmptyRatio[ssdindex][np];
    }
  }

  //____________________________________________________________________________
  // 画图
  TCanvas* cans_mutliratio_layer = new TCanvas("cans_multi_layer", "cans_multi_layer", 1200, 1200);
  TCanvas* cans_mutliratio_ssd   = new TCanvas("cans_multi_ssd",   "cans_multi_ssd",   1200, 1200);
  TCanvas* cans_multiratio_check[NUM_SSD];

  // 画出 ratio = 1 作为基准线
  TLine* line_one = new TLine(1, 1.0, 15., 1.0);
  line_one->SetLineColor(kTeal+2);
  line_one->SetLineWidth(2);
  line_one->SetLineStyle(2);

  TLegend* legend_L1S  =  new TLegend(0.6, 0.12, 0.88, 0.32);
  TLegend* legend_L2F  =  new TLegend(0.6, 0.12, 0.88, 0.32);
  TLegend* legend_L2B  =  new TLegend(0.6, 0.12, 0.88, 0.32);
  TLegend* legend_L3A  =  new TLegend(0.6, 0.12, 0.88, 0.32);
  TLegend* legend_SSD[NUM_SSD];
  TLegend* legend_SSD_MultiCheck[NUM_SSD][NUM_SSD]; // [layer][ssdindex]

  TGraph* gr_L1SMultiRatio[NUM_SSD];
  TGraph* gr_L2FMultiRatio[NUM_SSD];
  TGraph* gr_L2BMultiRatio[NUM_SSD];
  TGraph* gr_L3AMultiRatio[NUM_SSD];

  TGraph* gr_L1SMultiRatio_Clone[NUM_SSD];
  TGraph* gr_L2FMultiRatio_Clone[NUM_SSD];
  TGraph* gr_L2BMultiRatio_Clone[NUM_SSD];
  TGraph* gr_L3AMultiRatio_Clone[NUM_SSD];

  TGraph* gr_L1SMultiRatio_Clone2[NUM_SSD];
  TGraph* gr_L2FMultiRatio_Clone2[NUM_SSD];
  TGraph* gr_L2BMultiRatio_Clone2[NUM_SSD];
  TGraph* gr_L3AMultiRatio_Clone2[NUM_SSD];

  TGraph* gr_L1SEffRatio[NUM_SSD];
  TGraph* gr_L2FEffRatio[NUM_SSD];
  TGraph* gr_L2BEffRatio[NUM_SSD];
  TGraph* gr_L3AEffRatio[NUM_SSD];

  TGraph* gr_L1SEmptyRatio[NUM_SSD];
  TGraph* gr_L2FEmptyRatio[NUM_SSD];
  TGraph* gr_L2BEmptyRatio[NUM_SSD];
  TGraph* gr_L3AEmptyRatio[NUM_SSD];

  TGraph* gr_L1SSumRatio[NUM_SSD];  // SumRatio = EffRatio + EmptyRatio
  TGraph* gr_L2FSumRatio[NUM_SSD];  // SumRatio = EffRatio + EmptyRatio
  TGraph* gr_L2BSumRatio[NUM_SSD];  // SumRatio = EffRatio + EmptyRatio
  TGraph* gr_L3ASumRatio[NUM_SSD];  // SumRatio = EffRatio + EmptyRatio

  TMultiGraph* mg_L1S  =  new TMultiGraph();
  TMultiGraph* mg_L2F  =  new TMultiGraph();
  TMultiGraph* mg_L2B  =  new TMultiGraph();
  TMultiGraph* mg_L3A  =  new TMultiGraph();
  TMultiGraph* mg_SSD[NUM_SSD];
  TMultiGraph* mg_SSD_MultiCheck[NUM_SSD][NUM_SSD];  // [layer][ssdindex]

  for (Int_t i=0; i<NUM_SSD; i++) {
    legend_SSD[i] = new TLegend(0.6, 0.12, 0.88, 0.32);
    mg_SSD    [i] = new TMultiGraph();

    cans_multiratio_check[i] = new TCanvas(Form("SSD%d_multiratio_check", i+1), Form("SSD%d_multiratio_check", i+1), 1200, 1200);

    for (Int_t j=0; j<NUM_SSD; j++){
      mg_SSD_MultiCheck    [i][j] = new TMultiGraph();
      legend_SSD_MultiCheck[i][j] = new TLegend(0.56, 0.10, 0.88, 0.32);
    }
  }

  //____________________________________________________________________________
  //                     将不同 SSD 的同一层画在一起
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    gr_L1SMultiRatio[ssdindex] = new TGraph(NPoints, SigmaIndex, L1SMultiRatio[ssdindex]);
    gr_L2FMultiRatio[ssdindex] = new TGraph(NPoints, SigmaIndex, L2FMultiRatio[ssdindex]);
    gr_L2BMultiRatio[ssdindex] = new TGraph(NPoints, SigmaIndex, L2BMultiRatio[ssdindex]);
    gr_L3AMultiRatio[ssdindex] = new TGraph(NPoints, SigmaIndex, L3AMultiRatio[ssdindex]);

    gr_L1SMultiRatio[ssdindex]->SetMarkerStyle(Marker[ssdindex]);
    gr_L2FMultiRatio[ssdindex]->SetMarkerStyle(Marker[ssdindex]);
    gr_L2BMultiRatio[ssdindex]->SetMarkerStyle(Marker[ssdindex]);
    gr_L3AMultiRatio[ssdindex]->SetMarkerStyle(Marker[ssdindex]);

    gr_L1SMultiRatio[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2FMultiRatio[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2BMultiRatio[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L3AMultiRatio[ssdindex]->SetMarkerSize(MarkerSize);

    gr_L1SMultiRatio[ssdindex]->SetMarkerColor(Color[ssdindex]);
    gr_L2FMultiRatio[ssdindex]->SetMarkerColor(Color[ssdindex]);
    gr_L2BMultiRatio[ssdindex]->SetMarkerColor(Color[ssdindex]);
    gr_L3AMultiRatio[ssdindex]->SetMarkerColor(Color[ssdindex]);

    mg_L1S->Add(gr_L1SMultiRatio[ssdindex]);
    mg_L2F->Add(gr_L2FMultiRatio[ssdindex]);
    mg_L2B->Add(gr_L2BMultiRatio[ssdindex]);
    mg_L3A->Add(gr_L3AMultiRatio[ssdindex]);

    legend_L1S->AddEntry(gr_L1SMultiRatio[ssdindex],Form("SSD%d_L1S", ssdindex+1));
    legend_L2F->AddEntry(gr_L2FMultiRatio[ssdindex],Form("SSD%d_L2F", ssdindex+1));
    legend_L2B->AddEntry(gr_L2BMultiRatio[ssdindex],Form("SSD%d_L2B", ssdindex+1));
    legend_L3A->AddEntry(gr_L3AMultiRatio[ssdindex],Form("SSD%d_L3A", ssdindex+1));
  }

  cans_mutliratio_layer->Divide(2,2);
  cans_mutliratio_layer->cd(1);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_L1S->SetTitle(Form("L1S_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_L1S->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_L1S->GetXaxis()->SetTitleSize(0.04);
  mg_L1S->GetXaxis()->CenterTitle(1);
  mg_L1S->GetXaxis()->SetRangeUser(0,15.);
  mg_L1S->GetXaxis()->SetNdivisions(115);
  mg_L1S->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_L1S->GetYaxis()->SetTitleSize(0.04);
  mg_L1S->GetYaxis()->CenterTitle(1);
  mg_L1S->GetYaxis()->SetRangeUser(0,1.1);
  mg_L1S->GetYaxis()->SetNdivisions(511);
  mg_L1S->Draw("APL");
  legend_L1S->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_layer->cd(2);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_L2F->SetTitle(Form("L2F_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_L2F->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_L2F->GetXaxis()->SetTitleSize(0.04);
  mg_L2F->GetXaxis()->CenterTitle(1);
  mg_L2F->GetXaxis()->SetRangeUser(0,15.);
  mg_L2F->GetXaxis()->SetNdivisions(115);
  mg_L2F->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_L2F->GetYaxis()->SetTitleSize(0.04);
  mg_L2F->GetYaxis()->CenterTitle(1);
  mg_L2F->GetYaxis()->SetRangeUser(0,1.1);
  mg_L2F->GetYaxis()->SetNdivisions(511);
  mg_L2F->Draw("APL");
  legend_L2F->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_layer->cd(3);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_L2B->SetTitle(Form("L2B_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_L2B->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_L2B->GetXaxis()->SetTitleSize(0.04);
  mg_L2B->GetXaxis()->CenterTitle(1);
  mg_L2B->GetXaxis()->SetRangeUser(0,15.);
  mg_L2B->GetXaxis()->SetNdivisions(115);
  mg_L2B->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_L2B->GetYaxis()->SetTitleSize(0.04);
  mg_L2B->GetYaxis()->CenterTitle(1);
  mg_L2B->GetYaxis()->SetRangeUser(0,1.1);
  mg_L2B->GetYaxis()->SetNdivisions(511);
  mg_L2B->Draw("APL");
  legend_L2B->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_layer->cd(4);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_L3A->SetTitle(Form("L3A_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_L3A->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_L3A->GetXaxis()->SetTitleSize(0.04);
  mg_L3A->GetXaxis()->CenterTitle(1);
  mg_L3A->GetXaxis()->SetRangeUser(0,15.);
  mg_L3A->GetXaxis()->SetNdivisions(115);
  mg_L3A->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_L3A->GetYaxis()->SetTitleSize(0.04);
  mg_L3A->GetYaxis()->CenterTitle(1);
  mg_L3A->GetYaxis()->SetRangeUser(0,1.1);
  mg_L3A->GetYaxis()->SetNdivisions(511);
  mg_L3A->Draw("APL");
  legend_L3A->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_layer->Print(pathLayerMultiPNDOut.c_str());

  //____________________________________________________________________________
  //                     将同一个 SSD 的不同层画在一起
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {

    gr_L1SMultiRatio_Clone[ssdindex] = (TGraph*) gr_L1SMultiRatio[ssdindex]->Clone();
    gr_L2FMultiRatio_Clone[ssdindex] = (TGraph*) gr_L2FMultiRatio[ssdindex]->Clone();
    gr_L2BMultiRatio_Clone[ssdindex] = (TGraph*) gr_L2BMultiRatio[ssdindex]->Clone();
    gr_L3AMultiRatio_Clone[ssdindex] = (TGraph*) gr_L3AMultiRatio[ssdindex]->Clone();

    gr_L1SMultiRatio_Clone[ssdindex]->SetMarkerStyle(Marker[0]);
    gr_L2FMultiRatio_Clone[ssdindex]->SetMarkerStyle(Marker[1]);
    gr_L2BMultiRatio_Clone[ssdindex]->SetMarkerStyle(Marker[2]);
    gr_L3AMultiRatio_Clone[ssdindex]->SetMarkerStyle(Marker[3]);

    gr_L1SMultiRatio_Clone[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2FMultiRatio_Clone[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2BMultiRatio_Clone[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L3AMultiRatio_Clone[ssdindex]->SetMarkerSize(MarkerSize);

    gr_L1SMultiRatio_Clone[ssdindex]->SetMarkerColor(Color[0]);
    gr_L2FMultiRatio_Clone[ssdindex]->SetMarkerColor(Color[1]);
    gr_L2BMultiRatio_Clone[ssdindex]->SetMarkerColor(Color[2]);
    gr_L3AMultiRatio_Clone[ssdindex]->SetMarkerColor(Color[3]);

    mg_SSD[ssdindex]->Add(gr_L1SMultiRatio_Clone[ssdindex]);
    mg_SSD[ssdindex]->Add(gr_L2FMultiRatio_Clone[ssdindex]);
    mg_SSD[ssdindex]->Add(gr_L2BMultiRatio_Clone[ssdindex]);
    mg_SSD[ssdindex]->Add(gr_L3AMultiRatio_Clone[ssdindex]);

    legend_SSD[ssdindex]->AddEntry(gr_L1SMultiRatio_Clone[ssdindex],Form("SSD%d_L1S", ssdindex+1));
    legend_SSD[ssdindex]->AddEntry(gr_L2FMultiRatio_Clone[ssdindex],Form("SSD%d_L2F", ssdindex+1));
    legend_SSD[ssdindex]->AddEntry(gr_L2BMultiRatio_Clone[ssdindex],Form("SSD%d_L2B", ssdindex+1));
    legend_SSD[ssdindex]->AddEntry(gr_L3AMultiRatio_Clone[ssdindex],Form("SSD%d_L3A", ssdindex+1));
  }

  cans_mutliratio_ssd->Divide(2,2);
  cans_mutliratio_ssd->cd(1);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_SSD[0]->SetTitle(Form("SSD1_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_SSD[0]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_SSD[0]->GetXaxis()->SetTitleSize(0.04);
  mg_SSD[0]->GetXaxis()->CenterTitle(1);
  mg_SSD[0]->GetXaxis()->SetRangeUser(0,15.);
  mg_SSD[0]->GetXaxis()->SetNdivisions(115);
  mg_SSD[0]->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_SSD[0]->GetYaxis()->SetTitleSize(0.04);
  mg_SSD[0]->GetYaxis()->CenterTitle(1);
  mg_SSD[0]->GetYaxis()->SetRangeUser(0,1.1);
  mg_SSD[0]->GetYaxis()->SetNdivisions(511);
  mg_SSD[0]->Draw("APL");
  legend_SSD[0]->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_ssd->cd(2);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_SSD[1]->SetTitle(Form("SSD2_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_SSD[1]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_SSD[1]->GetXaxis()->SetTitleSize(0.04);
  mg_SSD[1]->GetXaxis()->CenterTitle(1);
  mg_SSD[1]->GetXaxis()->SetRangeUser(0,15.);
  mg_SSD[1]->GetXaxis()->SetNdivisions(115);
  mg_SSD[1]->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_SSD[1]->GetYaxis()->SetTitleSize(0.04);
  mg_SSD[1]->GetYaxis()->CenterTitle(1);
  mg_SSD[1]->GetYaxis()->SetRangeUser(0,1.1);
  mg_SSD[1]->GetYaxis()->SetNdivisions(511);
  mg_SSD[1]->Draw("APL");
  legend_SSD[1]->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_ssd->cd(3);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_SSD[2]->SetTitle(Form("SSD3_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_SSD[2]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_SSD[2]->GetXaxis()->SetTitleSize(0.04);
  mg_SSD[2]->GetXaxis()->CenterTitle(1);
  mg_SSD[2]->GetXaxis()->SetRangeUser(0,15.);
  mg_SSD[2]->GetXaxis()->SetNdivisions(115);
  mg_SSD[2]->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_SSD[2]->GetYaxis()->SetTitleSize(0.04);
  mg_SSD[2]->GetYaxis()->CenterTitle(1);
  mg_SSD[2]->GetYaxis()->SetRangeUser(0,1.1);
  mg_SSD[2]->GetYaxis()->SetNdivisions(511);
  mg_SSD[2]->Draw("APL");
  legend_SSD[2]->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_ssd->cd(4);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_SSD[3]->SetTitle(Form("SSD4_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_SSD[3]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_SSD[3]->GetXaxis()->SetTitleSize(0.04);
  mg_SSD[3]->GetXaxis()->CenterTitle(1);
  mg_SSD[3]->GetXaxis()->SetRangeUser(0,15);
  mg_SSD[3]->GetXaxis()->SetNdivisions(115);
  mg_SSD[3]->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_SSD[3]->GetYaxis()->SetTitleSize(0.04);
  mg_SSD[3]->GetYaxis()->CenterTitle(1);
  mg_SSD[3]->GetYaxis()->SetRangeUser(0,1.1);
  mg_SSD[3]->GetYaxis()->SetNdivisions(511);
  mg_SSD[3]->Draw("APL");
  legend_SSD[3]->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_ssd->Print(pathSSDMultiPNDOut.c_str());


  //____________________________________________________________________________
  //            将 EffRatio, EmptyRatio 画出来, 检查多重性计算是否准确
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    gr_L1SMultiRatio_Clone2[ssdindex]  =  (TGraph*) gr_L1SMultiRatio[ssdindex]->Clone();
    gr_L2FMultiRatio_Clone2[ssdindex]  =  (TGraph*) gr_L2FMultiRatio[ssdindex]->Clone();
    gr_L2BMultiRatio_Clone2[ssdindex]  =  (TGraph*) gr_L2BMultiRatio[ssdindex]->Clone();
    gr_L3AMultiRatio_Clone2[ssdindex]  =  (TGraph*) gr_L3AMultiRatio[ssdindex]->Clone();

    gr_L1SEffRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L1SEffRatio[ssdindex]);
    gr_L2FEffRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L2FEffRatio[ssdindex]);
    gr_L2BEffRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L2BEffRatio[ssdindex]);
    gr_L3AEffRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L3AEffRatio[ssdindex]);

    gr_L1SEmptyRatio       [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L1SEmptyRatio[ssdindex]);
    gr_L2FEmptyRatio       [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L2FEmptyRatio[ssdindex]);
    gr_L2BEmptyRatio       [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L2BEmptyRatio[ssdindex]);
    gr_L3AEmptyRatio       [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L3AEmptyRatio[ssdindex]);

    gr_L1SSumRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L1SSumRatio[ssdindex]);
    gr_L2FSumRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L2FSumRatio[ssdindex]);
    gr_L2BSumRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L2BSumRatio[ssdindex]);
    gr_L3ASumRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L3ASumRatio[ssdindex]);

    gr_L1SMultiRatio_Clone2[ssdindex]->SetMarkerStyle(Marker[0]);
    gr_L2FMultiRatio_Clone2[ssdindex]->SetMarkerStyle(Marker[0]);
    gr_L2BMultiRatio_Clone2[ssdindex]->SetMarkerStyle(Marker[0]);
    gr_L3AMultiRatio_Clone2[ssdindex]->SetMarkerStyle(Marker[0]);
    gr_L1SEffRatio         [ssdindex]->SetMarkerStyle(Marker[1]);
    gr_L2FEffRatio         [ssdindex]->SetMarkerStyle(Marker[1]);
    gr_L2BEffRatio         [ssdindex]->SetMarkerStyle(Marker[1]);
    gr_L3AEffRatio         [ssdindex]->SetMarkerStyle(Marker[1]);
    gr_L1SEmptyRatio       [ssdindex]->SetMarkerStyle(Marker[2]);
    gr_L2FEmptyRatio       [ssdindex]->SetMarkerStyle(Marker[2]);
    gr_L2BEmptyRatio       [ssdindex]->SetMarkerStyle(Marker[2]);
    gr_L3AEmptyRatio       [ssdindex]->SetMarkerStyle(Marker[2]);
    gr_L1SSumRatio         [ssdindex]->SetMarkerStyle(Marker[3]);
    gr_L2FSumRatio         [ssdindex]->SetMarkerStyle(Marker[3]);
    gr_L2BSumRatio         [ssdindex]->SetMarkerStyle(Marker[3]);
    gr_L3ASumRatio         [ssdindex]->SetMarkerStyle(Marker[3]);

    gr_L1SMultiRatio_Clone2[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2FMultiRatio_Clone2[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2BMultiRatio_Clone2[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L3AMultiRatio_Clone2[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L1SEffRatio         [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2FEffRatio         [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2BEffRatio         [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L3AEffRatio         [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L1SEmptyRatio       [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2FEmptyRatio       [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2BEmptyRatio       [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L3AEmptyRatio       [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L1SSumRatio         [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2FSumRatio         [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2BSumRatio         [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L3ASumRatio         [ssdindex]->SetMarkerSize(MarkerSize);

    gr_L1SMultiRatio_Clone2[ssdindex]->SetMarkerColor(Color[0]);
    gr_L2FMultiRatio_Clone2[ssdindex]->SetMarkerColor(Color[0]);
    gr_L2BMultiRatio_Clone2[ssdindex]->SetMarkerColor(Color[0]);
    gr_L3AMultiRatio_Clone2[ssdindex]->SetMarkerColor(Color[0]);
    gr_L1SEffRatio         [ssdindex]->SetMarkerColor(Color[1]);
    gr_L2FEffRatio         [ssdindex]->SetMarkerColor(Color[1]);
    gr_L2BEffRatio         [ssdindex]->SetMarkerColor(Color[1]);
    gr_L3AEffRatio         [ssdindex]->SetMarkerColor(Color[1]);
    gr_L1SEmptyRatio       [ssdindex]->SetMarkerColor(Color[2]);
    gr_L2FEmptyRatio       [ssdindex]->SetMarkerColor(Color[2]);
    gr_L2BEmptyRatio       [ssdindex]->SetMarkerColor(Color[2]);
    gr_L3AEmptyRatio       [ssdindex]->SetMarkerColor(Color[2]);
    gr_L1SSumRatio         [ssdindex]->SetMarkerColor(Color[3]);
    gr_L2FSumRatio         [ssdindex]->SetMarkerColor(Color[3]);
    gr_L2BSumRatio         [ssdindex]->SetMarkerColor(Color[3]);
    gr_L3ASumRatio         [ssdindex]->SetMarkerColor(Color[3]);

    mg_SSD_MultiCheck[0][ssdindex]->Add(gr_L1SMultiRatio_Clone2[ssdindex]);
    mg_SSD_MultiCheck[0][ssdindex]->Add(gr_L1SEffRatio[ssdindex]);
    mg_SSD_MultiCheck[0][ssdindex]->Add(gr_L1SEmptyRatio[ssdindex]);
    mg_SSD_MultiCheck[0][ssdindex]->Add(gr_L1SSumRatio[ssdindex]);
    mg_SSD_MultiCheck[1][ssdindex]->Add(gr_L2FMultiRatio_Clone2[ssdindex]);
    mg_SSD_MultiCheck[1][ssdindex]->Add(gr_L2FEffRatio[ssdindex]);
    mg_SSD_MultiCheck[1][ssdindex]->Add(gr_L2FEmptyRatio[ssdindex]);
    mg_SSD_MultiCheck[1][ssdindex]->Add(gr_L2FSumRatio[ssdindex]);
    mg_SSD_MultiCheck[2][ssdindex]->Add(gr_L2BMultiRatio_Clone2[ssdindex]);
    mg_SSD_MultiCheck[2][ssdindex]->Add(gr_L2BEffRatio[ssdindex]);
    mg_SSD_MultiCheck[2][ssdindex]->Add(gr_L2BEmptyRatio[ssdindex]);
    mg_SSD_MultiCheck[2][ssdindex]->Add(gr_L2BSumRatio[ssdindex]);
    mg_SSD_MultiCheck[3][ssdindex]->Add(gr_L3AMultiRatio_Clone2[ssdindex]);
    mg_SSD_MultiCheck[3][ssdindex]->Add(gr_L3AEffRatio[ssdindex]);
    mg_SSD_MultiCheck[3][ssdindex]->Add(gr_L3AEmptyRatio[ssdindex]);
    mg_SSD_MultiCheck[3][ssdindex]->Add(gr_L3ASumRatio[ssdindex]);

    legend_SSD_MultiCheck[0][ssdindex]->AddEntry(gr_L1SMultiRatio_Clone2[ssdindex], Form("SSD%d_L1S_MultiRatio", ssdindex+1));
    legend_SSD_MultiCheck[0][ssdindex]->AddEntry(gr_L1SEffRatio[ssdindex],          Form("SSD%d_L1S_EffRatio"  , ssdindex+1));
    legend_SSD_MultiCheck[0][ssdindex]->AddEntry(gr_L1SEmptyRatio[ssdindex],        Form("SSD%d_L1S_EmptyRatio", ssdindex+1));
    legend_SSD_MultiCheck[0][ssdindex]->AddEntry(gr_L1SSumRatio[ssdindex],          Form("SSD%d_L1S_SumRatio"  , ssdindex+1));
    legend_SSD_MultiCheck[1][ssdindex]->AddEntry(gr_L2FMultiRatio_Clone2[ssdindex], Form("SSD%d_L2F_MultiRatio", ssdindex+1));
    legend_SSD_MultiCheck[1][ssdindex]->AddEntry(gr_L2FEffRatio[ssdindex],          Form("SSD%d_L2F_EffRatio"  , ssdindex+1));
    legend_SSD_MultiCheck[1][ssdindex]->AddEntry(gr_L2FEmptyRatio[ssdindex],        Form("SSD%d_L2F_EmptyRatio", ssdindex+1));
    legend_SSD_MultiCheck[1][ssdindex]->AddEntry(gr_L2FSumRatio[ssdindex],          Form("SSD%d_L2F_SumRatio"  , ssdindex+1));
    legend_SSD_MultiCheck[2][ssdindex]->AddEntry(gr_L2BMultiRatio_Clone2[ssdindex], Form("SSD%d_L2B_MultiRatio", ssdindex+1));
    legend_SSD_MultiCheck[2][ssdindex]->AddEntry(gr_L2BEffRatio[ssdindex],          Form("SSD%d_L2B_EffRatio"  , ssdindex+1));
    legend_SSD_MultiCheck[2][ssdindex]->AddEntry(gr_L2BEmptyRatio[ssdindex],        Form("SSD%d_L2B_EmptyRatio", ssdindex+1));
    legend_SSD_MultiCheck[2][ssdindex]->AddEntry(gr_L2BSumRatio[ssdindex],          Form("SSD%d_L2B_SumRatio"  , ssdindex+1));
    legend_SSD_MultiCheck[3][ssdindex]->AddEntry(gr_L3AMultiRatio_Clone2[ssdindex], Form("SSD%d_L3A_MultiRatio", ssdindex+1));
    legend_SSD_MultiCheck[3][ssdindex]->AddEntry(gr_L3AEffRatio[ssdindex],          Form("SSD%d_L3A_EffRatio"  , ssdindex+1));
    legend_SSD_MultiCheck[3][ssdindex]->AddEntry(gr_L3AEmptyRatio[ssdindex],        Form("SSD%d_L3A_EmptyRatio", ssdindex+1));
    legend_SSD_MultiCheck[3][ssdindex]->AddEntry(gr_L3ASumRatio[ssdindex],          Form("SSD%d_L3A_SumRatio"  , ssdindex+1));
  }

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    cans_multiratio_check[ssdindex]->Divide(2,2);

    cans_multiratio_check[ssdindex]->cd(1);
    gPad->SetGridx();
    gPad->SetGridy();
    mg_SSD_MultiCheck[0][ssdindex]->SetTitle(Form("SSD%d_L1S_MultiRatio", ssdindex+1));
    mg_SSD_MultiCheck[0][ssdindex]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
    mg_SSD_MultiCheck[0][ssdindex]->GetXaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[0][ssdindex]->GetXaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[0][ssdindex]->GetXaxis()->SetRangeUser(0,15.);
    mg_SSD_MultiCheck[0][ssdindex]->GetXaxis()->SetNdivisions(115);
    mg_SSD_MultiCheck[0][ssdindex]->GetYaxis()->SetTitle("Ratio");
    mg_SSD_MultiCheck[0][ssdindex]->GetYaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[0][ssdindex]->GetYaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[0][ssdindex]->GetYaxis()->SetRangeUser(0,1.1);
    mg_SSD_MultiCheck[0][ssdindex]->GetYaxis()->SetNdivisions(511);
    mg_SSD_MultiCheck[0][ssdindex]->Draw("APL");
    legend_SSD_MultiCheck[0][ssdindex]->Draw("SAME");
    line_one->Draw("SAME");

    cans_multiratio_check[ssdindex]->cd(2);
    gPad->SetGridx();
    gPad->SetGridy();
    mg_SSD_MultiCheck[1][ssdindex]->SetTitle(Form("SSD%d_L2F_MultiRatio", ssdindex+1));
    mg_SSD_MultiCheck[1][ssdindex]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
    mg_SSD_MultiCheck[1][ssdindex]->GetXaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[1][ssdindex]->GetXaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[1][ssdindex]->GetXaxis()->SetRangeUser(0,15.);
    mg_SSD_MultiCheck[1][ssdindex]->GetXaxis()->SetNdivisions(115);
    mg_SSD_MultiCheck[1][ssdindex]->GetYaxis()->SetTitle("Ratio");
    mg_SSD_MultiCheck[1][ssdindex]->GetYaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[1][ssdindex]->GetYaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[1][ssdindex]->GetYaxis()->SetRangeUser(0,1.1);
    mg_SSD_MultiCheck[1][ssdindex]->GetYaxis()->SetNdivisions(511);
    mg_SSD_MultiCheck[1][ssdindex]->Draw("APL");
    legend_SSD_MultiCheck[1][ssdindex]->Draw("SAME");
    line_one->Draw("SAME");

    cans_multiratio_check[ssdindex]->cd(3);
    gPad->SetGridx();
    gPad->SetGridy();
    mg_SSD_MultiCheck[2][ssdindex]->SetTitle(Form("SSD%d_L2B_MultiRatio", ssdindex+1));
    mg_SSD_MultiCheck[2][ssdindex]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
    mg_SSD_MultiCheck[2][ssdindex]->GetXaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[2][ssdindex]->GetXaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[2][ssdindex]->GetXaxis()->SetRangeUser(0,15.);
    mg_SSD_MultiCheck[2][ssdindex]->GetXaxis()->SetNdivisions(115);
    mg_SSD_MultiCheck[2][ssdindex]->GetYaxis()->SetTitle("Ratio");
    mg_SSD_MultiCheck[2][ssdindex]->GetYaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[2][ssdindex]->GetYaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[2][ssdindex]->GetYaxis()->SetRangeUser(0,1.1);
    mg_SSD_MultiCheck[2][ssdindex]->GetYaxis()->SetNdivisions(511);
    mg_SSD_MultiCheck[2][ssdindex]->Draw("APL");
    legend_SSD_MultiCheck[2][ssdindex]->Draw("SAME");
    line_one->Draw("SAME");

    cans_multiratio_check[ssdindex]->cd(4);
    gPad->SetGridx();
    gPad->SetGridy();
    mg_SSD_MultiCheck[3][ssdindex]->SetTitle(Form("SSD%d_L3A_MultiRatio", ssdindex+1));
    mg_SSD_MultiCheck[3][ssdindex]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
    mg_SSD_MultiCheck[3][ssdindex]->GetXaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[3][ssdindex]->GetXaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[3][ssdindex]->GetXaxis()->SetRangeUser(0,15.);
    mg_SSD_MultiCheck[3][ssdindex]->GetXaxis()->SetNdivisions(510);
    mg_SSD_MultiCheck[3][ssdindex]->GetYaxis()->SetTitle("Ratio");
    mg_SSD_MultiCheck[3][ssdindex]->GetYaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[3][ssdindex]->GetYaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[3][ssdindex]->GetYaxis()->SetRangeUser(0,1.1);
    mg_SSD_MultiCheck[3][ssdindex]->GetYaxis()->SetNdivisions(511);
    mg_SSD_MultiCheck[3][ssdindex]->Draw("APL");
    legend_SSD_MultiCheck[3][ssdindex]->Draw("SAME");
    line_one->Draw("SAME");

    cans_multiratio_check[ssdindex]->Print(pathMultiCheckOut[ssdindex].c_str());
  }
  time.GetEndTime();
  time.GetRunTime();
}
//******************************************************************************


//******************************************************************************
void CSHINETrackReconstruction::GetPunchThroughEnergiesOfLCPs()
{
  Int_t NUM_LCPs = 16;

  std::string SiTag("Si");
  std::string CsITag("CsI");
  const Int_t LiseModel = 1;
  //LiseModel=1 is good for low energy， J.F.Ziegler et al, Pergamon Press, NY (low energy)

  std::string pathPunchThroughOut(Form("%sdata_TrackReconstruction/PunchThroughEnergies_of_LCPs.dat", PATHDATAFOLDER));
  ofstream FileOut(pathPunchThroughOut.c_str());
  FileOut<<setw(10)<<"*SSDNum"<<setw(20)<<"material"<<setw(15)<<"thickness/um"<<setw(15)<<"Charge(Z)"
         <<setw(15)<<"Mass(A)"<<setw(25)<<"PunchThroughEnergy/MeV"<<endl;

  for (Int_t ssdnum=0; ssdnum<NUM_SSD; ssdnum++) {
    for (Int_t lcp=0; lcp<NUM_LCPs; lcp++) {
      fLISEModule.GetRangeFromEnergy(Z_A_LCPs[lcp][0], Z_A_LCPs[lcp][1], 10., SiTag.c_str(), 1); // 只为了加载文件
      Double_t PunchThroughL1 = fLISEModule.GetEnergyFromRange(Z_A_LCPs[lcp][0],Z_A_LCPs[lcp][1],SIL1THICKNESS[ssdnum]*1000,SiTag.c_str(),LiseModel);
      FileOut<<setw(10)<<ssdnum<<setw(15)<<SiTag.c_str()<<setw(15)<<SIL1THICKNESS[ssdnum]*1000<<setw(15)
             <<Z_A_LCPs[lcp][0]<<setw(15)<<Z_A_LCPs[lcp][1]<<setw(15)<<PunchThroughL1<<endl;
    }
    for (Int_t lcp=0; lcp<NUM_LCPs; lcp++) {
      fLISEModule.GetRangeFromEnergy(Z_A_LCPs[lcp][0], Z_A_LCPs[lcp][1], 10., SiTag.c_str(), 1); // 只为了加载文件
      Double_t PunchThroughL2 = fLISEModule.GetEnergyFromRange(Z_A_LCPs[lcp][0],Z_A_LCPs[lcp][1],SIL2THICKNESS[ssdnum]*1000,SiTag.c_str(),LiseModel);
      FileOut<<setw(10)<<ssdnum<<setw(15)<<SiTag.c_str()<<setw(15)<<SIL2THICKNESS[ssdnum]*1000<<setw(15)
             <<Z_A_LCPs[lcp][0]<<setw(15)<<Z_A_LCPs[lcp][1]<<setw(15)<<PunchThroughL2<<endl;
    }
    for (Int_t lcp=0; lcp<NUM_LCPs; lcp++) {
      fLISEModule.GetRangeFromEnergy(Z_A_LCPs[lcp][0], Z_A_LCPs[lcp][1], 10., CsITag.c_str(), 1); // 只为了加载文件
      Double_t PunchThroughL3 = fLISEModule.GetEnergyFromRange(Z_A_LCPs[lcp][0],Z_A_LCPs[lcp][1],CSITHICKNESS*1000,CsITag.c_str(),LiseModel);
      FileOut<<setw(10)<<ssdnum<<setw(15)<<CsITag.c_str()<<setw(15)<<CSITHICKNESS*1000<<setw(15)
             <<Z_A_LCPs[lcp][0]<<setw(15)<<Z_A_LCPs[lcp][1]<<setw(15)<<PunchThroughL3<<endl;
    }
  }
  FileOut.close();
}

//******************************************************************************


//******************************************************************************
//______________________________________________________________________________
//               径迹重建前的一些测试与检查
//             ---------------------------
// L2F-L2B 能量关联
//------------------
void CSHINETrackReconstruction::CheckL2BL2FEnergyCorrelation(Int_t ssdindex)
{
  gStyle->SetOptStat(1);
  gStyle->SetPalette(1);

  std::string pathL2BL2FEneCorr_Cut(Form("%sfigure_TrackReconstruction/SSD%d_L2BL2F_EneCorrCut_Run%04d-Run%04d.png", PATHFIGURESFOLDER,ssdindex+1,fFirstRun,fLastRun));
  std::string pathL2BL2FEneCorr_EnEratioMeV(Form("%sfigure_TrackReconstruction/SSD%d_L2BL2F_EneCorrEnEratioMeV_Run%04d-Run%04d.png", PATHFIGURESFOLDER,ssdindex+1,fFirstRun,fLastRun));
  std::string pathL2BL2FEneCorr_EnEratioPer(Form("%sfigure_TrackReconstruction/SSD%d_L2BL2F_EneCorrEnEratioPer_Run%04d-Run%04d.png", PATHFIGURESFOLDER,ssdindex+1,fFirstRun,fLastRun));

  Double_t L2FL2B_EnergyDiff_MeV;
  Double_t L2FL2B_EnergyDiff_Per;

  Double_t EnergyCut[4] = {0.05, 0.10, 0.15, 0.20};
  TH2D* hist_L2B_L2F[4];
  for (Int_t i=0; i<4; i++) {
    hist_L2B_L2F[i] = new TH2D(Form("hist_L2B_L2F_Cut%.2lf",EnergyCut[i]),Form("hist_L2B_L2F_Cut%.2lf",EnergyCut[i]),3000,0.,300.,3000,0.,300.);
  }

  TH2D* hist2_EnergyDifference_MeV = new TH2D("hist2_L2FL2B_EnEratio_MeV","",3000,0.,300.,1000,-50.,50.);
  TH2D* hist2_EnergyDifference_Per = new TH2D("hist2_L2FL2B_EnEratio_Per","",3000,0.,300.,600, -30., 30.);

  Long64_t nentries = fChainLayerTree->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {

    fChainLayerTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    if (fLayerEvent.fSSDCsIMulti[ssdindex]==0) continue;
    if (fLayerEvent.fSSDL2BMulti[ssdindex]==0) continue;
    if (fLayerEvent.fSSDL2FMulti[ssdindex]==0) continue;
    if (fLayerEvent.fSSDL1SMulti[ssdindex]==0) continue;

    for (Int_t i=0; i<4; i++) {
      for (Int_t l2b=0; l2b<fLayerEvent.fL2BMulti; l2b++) {
        for (Int_t l2f=0; l2f<fLayerEvent.fL2FMulti; l2f++) {
          if (IsGeoConstraint_L3A_L2B(ssdindex, fLayerEvent.fCsIMulti, fLayerEvent.fCsINum.data(), fLayerEvent.fCsISSDNum.data(),fLayerEvent.fL2BNumStrip[l2b], fLayerEvent.fL2BSSDNum[l2b]) &&
              IsGeoConstraint_L3A_L2F(ssdindex, fLayerEvent.fCsIMulti, fLayerEvent.fCsINum.data(), fLayerEvent.fCsISSDNum.data(),fLayerEvent.fL2FNumStrip[l2f], fLayerEvent.fL2FSSDNum[l2f]) &&
              IsGeoConstraint_L2B_L1S(ssdindex, fLayerEvent.fL2BNumStrip[l2b], fLayerEvent.fL2BSSDNum[l2b],fLayerEvent.fL1SMulti, fLayerEvent.fL1SNumStrip.data(), fLayerEvent.fL1SSSDNum.data()) &&
              IsEneConstraint_L2B_L2F(fLayerEvent.fL2BEMeV[l2b], fLayerEvent.fL2FEMeV[l2f], EnergyCut[i]))
          {
            hist_L2B_L2F[i]->Fill(fLayerEvent.fL2FEMeV[l2f], fLayerEvent.fL2BEMeV[l2b]);

            L2FL2B_EnergyDiff_MeV = fLayerEvent.fL2BEMeV[l2b] - fLayerEvent.fL2FEMeV[l2f];
            hist2_EnergyDifference_MeV->Fill(fLayerEvent.fL2FEMeV[l2f], L2FL2B_EnergyDiff_MeV);

            L2FL2B_EnergyDiff_Per = L2FL2B_EnergyDiff_MeV/fLayerEvent.fL2FEMeV[l2f] * 100; // %
            hist2_EnergyDifference_Per->Fill(fLayerEvent.fL2FEMeV[l2f], L2FL2B_EnergyDiff_Per);
          }
        }
      }
    }
  }

  TF1* func_L2B_eq_L2F = new TF1("func", "x", 0, 300);
  func_L2B_eq_L2F->SetLineColor(kRed);
  func_L2B_eq_L2F->SetLineStyle(7);
  func_L2B_eq_L2F->SetLineWidth(4);

  //________________________________________________________
  // 画出不同 EnergyCut 下的 L2B_L2F 能量关联
  TCanvas* cans = new TCanvas("cans", "cans", 1000, 1000);
  cans->Divide(2,2);
  for (Int_t i=0; i<4; i++) {
    cans->cd(i+1);
    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.12);
    gPad->SetTopMargin(0.12);
    hist_L2B_L2F[i]->GetXaxis()->SetTitle(Form("SSD%d_EL2F (MeV)", ssdindex+1));
    hist_L2B_L2F[i]->GetXaxis()->CenterTitle(1);
    hist_L2B_L2F[i]->GetXaxis()->SetTitleSize(0.05);
    hist_L2B_L2F[i]->GetXaxis()->SetLabelSize(0.05);

    hist_L2B_L2F[i]->GetYaxis()->SetTitle(Form("SSD%d_EL2B (MeV)", ssdindex+1));
    hist_L2B_L2F[i]->GetYaxis()->CenterTitle(1);
    hist_L2B_L2F[i]->GetYaxis()->SetTitleSize(0.05);
    hist_L2B_L2F[i]->GetYaxis()->SetLabelSize(0.05);

    hist_L2B_L2F[i]->Draw("COL");
    func_L2B_eq_L2F->Draw("SAME");
    gPad->Update();
    TPaveStats* st = (TPaveStats*)hist_L2B_L2F[i]->GetListOfFunctions()->FindObject("stats");
    st->SetX1NDC(0.55);
    st->SetX2NDC(0.85);
    st->SetY1NDC(0.15);
    st->SetY2NDC(0.45);
    gPad->Modified();
  }
  cans->Print(pathL2BL2FEneCorr_Cut.c_str());

  //_______________________________________________________________
  // 画出实验数据中的 EL2B-EL2F(MeV)的分布
  TCanvas* c_diff = new TCanvas("c_diff", "c_diff", 1200, 600);
  c_diff->Divide(2,1);
  c_diff->cd(1);
  gPad->SetLogz();
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.12);
  gPad->SetRightMargin(0.15);
  hist2_EnergyDifference_MeV->GetXaxis()->SetTitle(Form("SSD%d_EL2F (MeV)", ssdindex+1));
  hist2_EnergyDifference_MeV->GetXaxis()->CenterTitle(1);
  hist2_EnergyDifference_MeV->GetXaxis()->SetTitleSize(0.05);
  hist2_EnergyDifference_MeV->GetXaxis()->SetLabelSize(0.05);

  hist2_EnergyDifference_MeV->GetYaxis()->SetTitle("EL2B - EL2F (MeV)");
  hist2_EnergyDifference_MeV->GetYaxis()->CenterTitle(1);
  hist2_EnergyDifference_MeV->GetYaxis()->SetTitleSize(0.05);
  hist2_EnergyDifference_MeV->GetYaxis()->SetLabelSize(0.05);

  hist2_EnergyDifference_MeV->GetZaxis()->SetLabelSize(0.05);

  hist2_EnergyDifference_MeV->Draw("COLZ");
  gPad->Update();
  TPaveStats* st = (TPaveStats*)hist2_EnergyDifference_MeV->GetListOfFunctions()->FindObject("stats");
  st->SetX1NDC(0.16);
  st->SetX2NDC(0.37);
  st->SetY1NDC(0.65);
  st->SetY2NDC(0.88);
  st->Draw("SAME");
  gPad->Modified();

  TH2D* hist_clone = (TH2D*)hist2_EnergyDifference_MeV->Clone();
  TPad* subpad = new TPad("subpad", "", 0.15, 0.125, 0.42, 0.33);
  subpad->Draw();
  subpad->cd();
  gPad->SetFillStyle(0);
  gPad->SetLogz();
  gStyle->SetOptStat(0);
  hist_clone->GetXaxis()->SetTitle("");
  hist_clone->GetXaxis()->SetRangeUser(0., 50.);
  hist_clone->GetXaxis()->SetNdivisions(0);
  hist_clone->GetYaxis()->SetTitle("");
  hist_clone->GetYaxis()->SetRangeUser(-10, 10.);
  hist_clone->GetYaxis()->SetNdivisions(0);
  hist_clone->Draw("COL");

  // projectionY
  c_diff->cd(2);
  gPad->SetBottomMargin(0.12);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.15);
  TH1D* hist1_EnergyDifference_MeV = (TH1D*)hist2_EnergyDifference_MeV->ProjectionY();
  hist1_EnergyDifference_MeV->GetXaxis()->SetRangeUser(-20.,10);
  hist1_EnergyDifference_MeV->GetYaxis()->SetLabelSize(0.05);
  hist1_EnergyDifference_MeV->Draw();

  c_diff->Print(pathL2BL2FEneCorr_EnEratioMeV.c_str());


  //_______________________________________________________________
  // 画出实验数据中的 EL2B-EL2F(%)的分布
  TCanvas* c_per = new TCanvas("c_per", "c_per", 1200, 600);
  c_per->Divide(2,1);
  c_per->cd(1);
  gPad->SetLogz();
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.12);
  gPad->SetRightMargin(0.15);
  hist2_EnergyDifference_Per->GetXaxis()->SetTitle(Form("SSD%d_EL2F (MeV)", ssdindex+1));
  hist2_EnergyDifference_Per->GetXaxis()->CenterTitle(1);
  hist2_EnergyDifference_Per->GetXaxis()->SetTitleSize(0.05);
  hist2_EnergyDifference_Per->GetXaxis()->SetLabelSize(0.05);
  hist2_EnergyDifference_Per->GetXaxis()->SetNdivisions(508);
  hist2_EnergyDifference_Per->GetYaxis()->SetTitle("(EL2B - EL2F)/EL2F (%)");
  hist2_EnergyDifference_Per->GetYaxis()->CenterTitle(1);
  hist2_EnergyDifference_Per->GetYaxis()->SetTitleSize(0.05);
  hist2_EnergyDifference_Per->GetYaxis()->SetLabelSize(0.05);
  hist2_EnergyDifference_Per->GetZaxis()->SetLabelSize(0.05);
  hist2_EnergyDifference_Per->Draw("COLZ");

  TH2D* histper_clone = (TH2D*)hist2_EnergyDifference_Per->Clone();
  TPad* subpad_per = new TPad("subpad_per", "", 0.45, 0.45, 0.88, 0.93);
  subpad_per->Draw();
  subpad_per->cd();
  gPad->SetLogz();
  gPad->SetFillStyle(0);
  gStyle->SetOptStat(0);
  histper_clone->GetXaxis()->SetTitle("");
  histper_clone->GetXaxis()->SetRangeUser(0., 50.);
  histper_clone->GetXaxis()->SetNdivisions(0);
  histper_clone->GetYaxis()->SetTitle("");
  histper_clone->GetYaxis()->SetRangeUser(-20, 20.);
  histper_clone->GetYaxis()->SetNdivisions(0);
  histper_clone->Draw("COL");

  // projectionY
  TH1D* hist1_EnergyDifference_Per = (TH1D*)hist2_EnergyDifference_Per->ProjectionY();
  c_per->cd(2);
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.12);
  gPad->SetRightMargin(0.15);
  hist1_EnergyDifference_Per->GetYaxis()->SetLabelSize(0.05);
  hist1_EnergyDifference_Per->Draw();

  c_per->Print(pathL2BL2FEneCorr_EnEratioPer.c_str());
}

//______________________________________________________________________________
// 画出 L2B_L2F 能量关联的效果
void CSHINETrackReconstruction::DetermineL2BL2FEnergyErrRatio()
{
  gStyle->SetOptStat(1);
  gStyle->SetPalette(1);

  std::string pathL2BL2FEnergyCut(Form("%sfigure_TrackReconstruction/SSD_L2BL2F_EnergyCut_Run%04d-Run%04d.png",PATHFIGURESFOLDER,fFirstRun,fLastRun));

  TH2D* hist_L2B_L2F[4];
  for (Int_t i=0; i<4; i++) {
    hist_L2B_L2F[i] = new TH2D(Form("SSD%d_L2BL2F_EnergyCut",i+1),Form("SSD%d_L2BL2F_EnergyCut",i+1),3000,0.,300.,3000,0.,300.);
  }

  Long64_t nentries = fChainLayerTree->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries;ientry++) {

    fChainLayerTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      if (fLayerEvent.fSSDCsIMulti[ssdindex]==0) continue;
      if (fLayerEvent.fSSDL2BMulti[ssdindex]==0) continue;
      if (fLayerEvent.fSSDL2FMulti[ssdindex]==0) continue;
      if (fLayerEvent.fSSDL1SMulti[ssdindex]==0) continue;

      for (Int_t l2b=0; l2b<fLayerEvent.fL2BMulti; l2b++) {
        for (Int_t l2f=0; l2f<fLayerEvent.fL2FMulti; l2f++) {
          if (IsGeoConstraint_L3A_L2B(ssdindex, fLayerEvent.fCsIMulti, fLayerEvent.fCsINum.data(), fLayerEvent.fCsISSDNum.data(),
                                      fLayerEvent.fL2BNumStrip[l2b], fLayerEvent.fL2BSSDNum[l2b]) &&
              IsGeoConstraint_L3A_L2F(ssdindex, fLayerEvent.fCsIMulti, fLayerEvent.fCsINum.data(), fLayerEvent.fCsISSDNum.data(),
                                      fLayerEvent.fL2FNumStrip[l2f], fLayerEvent.fL2FSSDNum[l2f]) &&
              IsGeoConstraint_L2B_L1S(ssdindex, fLayerEvent.fL2BNumStrip[l2b], fLayerEvent.fL2BSSDNum[l2b],
                                      fLayerEvent.fL1SMulti, fLayerEvent.fL1SNumStrip.data(), fLayerEvent.fL1SSSDNum.data()) &&
              IsEneConstraint_L2B_L2F(ssdindex, fLayerEvent.fL2BEMeV[l2b], fLayerEvent.fL2FEMeV[l2f]))
          {
            hist_L2B_L2F[ssdindex]->Fill(fLayerEvent.fL2FEMeV[l2f], fLayerEvent.fL2BEMeV[l2b]);
          }
        }
      }
    }
  }

  TF1* func_L2B_eq_L2F = new TF1("func", "x", 0, 300);
  func_L2B_eq_L2F->SetLineColor(kRed);
  func_L2B_eq_L2F->SetLineStyle(7);
  func_L2B_eq_L2F->SetLineWidth(4);

  //________________________________________________________
  // 画出不同 EnergyCut 下的 L2B_L2F 能量关联
  TCanvas* cans = new TCanvas("cans", "cans", 1000, 1000);
  cans->Divide(2,2);

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    cans->cd(ssdindex+1);
    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.12);
    gPad->SetTopMargin(0.12);
    hist_L2B_L2F[ssdindex]->GetXaxis()->SetTitle(Form("SSD%d_EL2F (MeV)", ssdindex+1));
    hist_L2B_L2F[ssdindex]->GetXaxis()->CenterTitle(1);
    hist_L2B_L2F[ssdindex]->GetXaxis()->SetTitleSize(0.05);
    hist_L2B_L2F[ssdindex]->GetXaxis()->SetLabelSize(0.05);

    hist_L2B_L2F[ssdindex]->GetYaxis()->SetTitle(Form("SSD%d_EL2B (MeV)", ssdindex+1));
    hist_L2B_L2F[ssdindex]->GetYaxis()->CenterTitle(1);
    hist_L2B_L2F[ssdindex]->GetYaxis()->SetTitleSize(0.05);
    hist_L2B_L2F[ssdindex]->GetYaxis()->SetLabelSize(0.05);

    hist_L2B_L2F[ssdindex]->Draw("COL");
    func_L2B_eq_L2F->Draw("SAME");
    gPad->Update();
    TPaveStats* st1 = (TPaveStats*)hist_L2B_L2F[ssdindex]->GetListOfFunctions()->FindObject("stats");
    st1->SetX1NDC(0.55);
    st1->SetX2NDC(0.85);
    st1->SetY1NDC(0.15);
    st1->SetY2NDC(0.45);
    gPad->Modified();
  }
  cans->Print(pathL2BL2FEnergyCut.c_str());
}


//______________________________________________________________________________
// L1S-L2F 能量关联
//------------------
// LISE++ 计算不同粒子在每套硅望远镜两层硅中的能损
void CSHINETrackReconstruction::CheckEnergyLossL1L2()
{
  Double_t Eloss_L1[16];
  Double_t Eloss_L2[16];
  Double_t Eres_L2 [16];

  for (Int_t issd=0; issd<4; issd++) {
    std::string pathDataOut(Form("%sdata_Test_Multi/SSD%d_L1L2ELossTest.dat", PATHDATAFOLDER,issd+1));
    ofstream fileout(pathDataOut.c_str());
    fileout<<setw(5)<<"Z"<<setw(5)<<"A"<<setw(10)<<"Einc(MeV)"<<setw(12)<<"ELossL1"
           <<setw(12)<<"ELossL2"<<setw(12)<<"ERes"<<endl;

    for (Int_t ip=0; ip<16; ip++) {
      for (Double_t einc=0.; einc<400.; einc+=0.1) {
        Eloss_L1[ip] = fLISEModule.GetEnergyLoss(Z_A_LCPs[ip][0], Z_A_LCPs[ip][1], einc, "Si", SIL1THICKNESS[issd]*1000, 1);
        Eloss_L2[ip] = fLISEModule.GetEnergyLoss(Z_A_LCPs[ip][0], Z_A_LCPs[ip][1], einc-Eloss_L1[ip], "Si", SIL2THICKNESS[issd]*1000, 1);
        Eres_L2 [ip] = einc - Eloss_L1[ip] - Eloss_L2[ip];
        fileout<<setw(5)<<Z_A_LCPs[ip][0]<<setw(5)<<Z_A_LCPs[ip][1]<<setw(10)<<einc
               <<setw(12)<<Eloss_L1[ip]<<setw(12)<<Eloss_L2[ip]<<setw(12)<<Eres_L2[ip]<<endl;

        cout<<setw(5)<<Z_A_LCPs[ip][0]<<setw(5)<<Z_A_LCPs[ip][1]<<setw(10)<<einc
            <<setw(12)<<Eloss_L1[ip]<<setw(12)<<Eloss_L2[ip]<<setw(12)<<Eres_L2[ip]<<endl;
      }
    }
  }
}


//______________________________________________________________________________
// 提取 LISE++ 计算的数据, 画出 DE-E Plot
// 目的: 为了确定粒子穿透 L2 到达 L3 时, EL1 与 EL2 的比值
void CSHINETrackReconstruction::CheckEnergyLossL1L2_Relationship(Bool_t punchthrough)
{
  gStyle->SetOptStat(0);

  std::string pathE1E2Ratio = Form("%sfigure_TrackReconstruction/SSD_E1E2Ratio.png",PATHFIGURESFOLDER);
  std::string pathE1E2Correlation;
  if (punchthrough) pathE1E2Correlation = Form("%sfigure_TrackReconstruction/SSD_E1E2Correlation_punchthrough.png",PATHFIGURESFOLDER);
  else pathE1E2Correlation = Form("%sfigure_TrackReconstruction/SSD_E1E2Correlation.png",PATHFIGURESFOLDER);

  std::vector<Double_t> dE1_SSD1;
  std::vector<Double_t> dE2_SSD1;
  std::vector<Double_t> dE1_SSD2;
  std::vector<Double_t> dE2_SSD2;
  std::vector<Double_t> dE1_SSD3;
  std::vector<Double_t> dE2_SSD3;
  std::vector<Double_t> dE1_SSD4;
  std::vector<Double_t> dE2_SSD4;

  std::vector<Double_t> EL1EL2Ratio_SSD1;
  std::vector<Double_t> EL1EL2Ratio_SSD2;
  std::vector<Double_t> EL1EL2Ratio_SSD3;
  std::vector<Double_t> EL1EL2Ratio_SSD4;

  std::string pathSSDELossInput[4];
  for (Int_t i=0; i<4; i++) {
    pathSSDELossInput[i] = Form("%sdata_Test_Multi/SSD%d_L1L2ELossTest.dat", PATHDATAFOLDER,i+1);

    ifstream in;
    in.open(pathSSDELossInput[i].c_str());
    if(!in.is_open()) {
      printf("Error: file %s not found\n",pathSSDELossInput[i].c_str());
      return NULL;
    }
    std::vector<Double_t> dE1;
    std::vector<Double_t> dE2;
    std::vector<Double_t> EL1EL2Ratio;
    while(in.good()) {
      // 按行读取数据
      std::string LineRead;
      std::getline(in, LineRead);
      LineRead.assign(LineRead.substr(0, LineRead.find('*')));
      if(LineRead.empty()) continue;
      if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
      std::istringstream LineStream(LineRead);

      Int_t Z;
      Int_t A;
      Double_t Einc;
      Double_t dEL1;
      Double_t dEL2;
      Double_t Eres;
      LineStream>>Z>>A>>Einc>>dEL1>>dEL2>>Eres;
      if (punchthrough==kTRUE && Eres==0) continue;  // 只考虑能穿透第二层硅的情况
      dE1.push_back(dEL1);
      dE2.push_back(dEL2);
      EL1EL2Ratio.push_back(dEL1/dEL2 * 100);
    }
    if (i==0) { dE1_SSD1=dE1;  dE2_SSD1=dE2; EL1EL2Ratio_SSD1=EL1EL2Ratio; };
    if (i==1) { dE1_SSD2=dE1;  dE2_SSD2=dE2; EL1EL2Ratio_SSD2=EL1EL2Ratio; };
    if (i==2) { dE1_SSD3=dE1;  dE2_SSD3=dE2; EL1EL2Ratio_SSD3=EL1EL2Ratio; };
    if (i==3) { dE1_SSD4=dE1;  dE2_SSD4=dE2; EL1EL2Ratio_SSD4=EL1EL2Ratio; };
  }

  TH2D* hist2_SSD_dEE[NUM_SSD];
  TH1D* hist1_SSD_Eratio[NUM_SSD];
  for (Int_t ssd=0; ssd<NUM_SSD; ssd++) {
    hist2_SSD_dEE[ssd] = new TH2D(Form("hist2_SSD%d_DEE_LISE++", ssd+1), Form("hist2_SSD%d_DEE_LISE++", ssd+1),3000,0.,300.,1500,0.,150.);
    hist1_SSD_Eratio[ssd] = new TH1D(Form("hist1_SSD%d_Eratio_LISE++", ssd+1), Form("hist1_SSD%d_Eratio_LISE++", ssd+1), 500, 0., 50.);
  }

  for (Int_t i=0; i<dE1_SSD1.size(); i++) {
    hist2_SSD_dEE[0]->Fill(dE2_SSD1[i], dE1_SSD1[i]);
    hist2_SSD_dEE[1]->Fill(dE2_SSD2[i], dE1_SSD2[i]);
    hist2_SSD_dEE[2]->Fill(dE2_SSD3[i], dE1_SSD3[i]);
    hist2_SSD_dEE[3]->Fill(dE2_SSD4[i], dE1_SSD4[i]);
    cout<<EL1EL2Ratio_SSD1[i]<<endl;
    hist1_SSD_Eratio[0]->Fill(EL1EL2Ratio_SSD1[i]);
    hist1_SSD_Eratio[1]->Fill(EL1EL2Ratio_SSD2[i]);
    hist1_SSD_Eratio[2]->Fill(EL1EL2Ratio_SSD3[i]);
    hist1_SSD_Eratio[3]->Fill(EL1EL2Ratio_SSD4[i]);
  }

  TF1* func_L2B_eq_L2F = new TF1("func", "x", 0, 300);
  func_L2B_eq_L2F->SetLineColor(kRed);
  func_L2B_eq_L2F->SetLineStyle(7);
  func_L2B_eq_L2F->SetLineWidth(4);

  //________________________
  // 画出 DEE 曲线
  TCanvas* cans = new TCanvas("cans","cans",1000,1000);
  cans->Divide(2,2);
  //________________________
  // 画出 (EL2-EL1)/EL2 分布
  TCanvas* c_Eratio = new TCanvas("c_Eratio","c_Eratio",1000,1000);
  c_Eratio->Divide(2,2);

  for (Int_t index=0; index<4; index++) {
    cans->cd(index+1);
    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.12);
    hist2_SSD_dEE[index]->Draw("COL");
    hist2_SSD_dEE[index]->GetXaxis()->SetTitle("dE_{2} (MeV)");
    hist2_SSD_dEE[index]->GetXaxis()->SetTitleSize(0.05);
    hist2_SSD_dEE[index]->GetXaxis()->CenterTitle(1);
    hist2_SSD_dEE[index]->GetXaxis()->SetLabelSize(0.05);
    hist2_SSD_dEE[index]->GetXaxis()->SetNdivisions(508);
    hist2_SSD_dEE[index]->GetYaxis()->SetTitle("dE_{1} (MeV)");
    hist2_SSD_dEE[index]->GetYaxis()->SetTitleSize(0.05);
    hist2_SSD_dEE[index]->GetYaxis()->CenterTitle(1);
    hist2_SSD_dEE[index]->GetYaxis()->SetLabelSize(0.05);
    hist2_SSD_dEE[index]->GetYaxis()->SetNdivisions(508);
    func_L2B_eq_L2F->Draw("SAME");

    c_Eratio->cd(index+1);
    gPad->SetLeftMargin(0.17);
    gPad->SetBottomMargin(0.14);
    hist1_SSD_Eratio[index]->Draw();
    hist1_SSD_Eratio[index]->GetXaxis()->SetTitle("dE1/dE2 (%)");
    hist1_SSD_Eratio[index]->GetXaxis()->SetTitleSize(0.05);
    hist1_SSD_Eratio[index]->GetXaxis()->CenterTitle(1);
    hist1_SSD_Eratio[index]->GetXaxis()->SetLabelSize(0.05);
    hist1_SSD_Eratio[index]->GetXaxis()->SetNdivisions(505);
    hist1_SSD_Eratio[index]->GetYaxis()->SetTitle("Counts");
    hist1_SSD_Eratio[index]->GetYaxis()->SetTitleSize(0.05);
    hist1_SSD_Eratio[index]->GetYaxis()->CenterTitle(1);
    hist1_SSD_Eratio[index]->GetYaxis()->SetLabelSize(0.05);
    hist1_SSD_Eratio[index]->GetYaxis()->SetNdivisions(505);
  }
  c_Eratio->Print(pathE1E2Ratio.c_str());
  cans->Print(pathE1E2Correlation.c_str());
}


//______________________________________________________________________________
// 根据实验数据画出 L1-L2 的能量关联
void CSHINETrackReconstruction::CheckEnergyLossL1L2_Expdata()
{
  Double_t EnergyCut = 0.20;
  Double_t ratio_E1E2[4];

  std::string pathPNGOut(Form("%sfigure_TrackReconstruction/SSD2_DEL1DEL2_Ratio_Expdata.png",PATHFIGURESFOLDER));

  TH1D* hist[4];
  for (Int_t i=0; i<4; i++) {
    hist[i] = new TH1D(Form("hist_SSD%d",i+1),"",100,0.,100.);
  }

  Long64_t nentries = fChainLayerTree->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries;ientry++) {

    fChainLayerTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      if (fLayerEvent.fSSDCsIMulti[ssdindex]==0 || fLayerEvent.fSSDCsIMulti[ssdindex]>MULTICUT_L3A) continue;
      if (fLayerEvent.fSSDL2BMulti[ssdindex]==0 || fLayerEvent.fSSDL2BMulti[ssdindex]>MULTICUT_L2B) continue;
      if (fLayerEvent.fSSDL2FMulti[ssdindex]==0 || fLayerEvent.fSSDL2FMulti[ssdindex]>MULTICUT_L2F) continue;
      if (fLayerEvent.fSSDL1SMulti[ssdindex]==0 || fLayerEvent.fSSDL1SMulti[ssdindex]>MULTICUT_L1S) continue;

      for (Int_t l2f=0; l2f<fLayerEvent.fL2FMulti; l2f++) {
        for (Int_t l1s=0; l1s<fLayerEvent.fL1SMulti; l1s++) {
          if (IsGeoConstraint_L3A_L2B(ssdindex, fLayerEvent.fCsIMulti, fLayerEvent.fCsINum.data(), fLayerEvent.fCsISSDNum.data(),
                                      fLayerEvent.fL2BMulti, fLayerEvent.fL2BNumStrip.data(), fLayerEvent.fL2BSSDNum.data()) &&
              IsGeoConstraint_L3A_L2F(ssdindex, fLayerEvent.fCsIMulti, fLayerEvent.fCsINum.data(), fLayerEvent.fCsISSDNum.data(),
                                      fLayerEvent.fL2FNumStrip[l2f], fLayerEvent.fL2FSSDNum[l2f]) &&
              IsEneConstraint_L2B_L2F(ssdindex, fLayerEvent.fL2BMulti, fLayerEvent.fL2BEMeV.data(), fLayerEvent.fL2BSSDNum.data(),
                                      fLayerEvent.fL2FNumStrip[l2f], fLayerEvent.fL2BSSDNum[l2f], EnergyCut) &&
              IsGeoConstraint_L2B_L1S(ssdindex, fLayerEvent.fL2BMulti, fLayerEvent.fL2BNumStrip.data(), fLayerEvent.fL2BSSDNum.data(),
                                      fLayerEvent.fL1SNumStrip[l1s], fLayerEvent.fL1SSSDNum[l1s]))
          {
            //if (ssdindex==3 && (fLayerEvent.fL2FNumStrip[l2f]==0 || fLayerEvent.fL2FNumStrip[l2f]==1)) continue;
            ratio_E1E2[ssdindex] = fLayerEvent.fL1SEMeV[l1s]/fLayerEvent.fL2FEMeV[l2f]*100;  // 单位: %
            hist[ssdindex]->Fill(ratio_E1E2[ssdindex]);
          }
        }
      }
    }
  }

  TCanvas* cans = new TCanvas("cans","cans",1200,1000);
  cans->Divide(2,2);

  for (Int_t index=0; index<4; index++) {
    cans->cd(index+1);
    gPad->SetLeftMargin(0.13);
    gPad->SetBottomMargin(0.12);
    hist[index]->Draw();
    hist[index]->GetXaxis()->SetTitle("EL1S/EL2F (%)");
    hist[index]->GetXaxis()->SetTitleSize(0.05);
    hist[index]->GetXaxis()->CenterTitle(kTRUE);
    hist[index]->GetXaxis()->SetLabelSize(0.05);
    hist[index]->GetXaxis()->SetNdivisions(505);
    hist[index]->GetYaxis()->SetTitle("Counts");
    hist[index]->GetYaxis()->SetTitleSize(0.05);
    hist[index]->GetYaxis()->CenterTitle(kTRUE);
    hist[index]->GetYaxis()->SetTitleOffset(1.2);
    hist[index]->GetYaxis()->SetLabelSize(0.05);
    hist[index]->GetYaxis()->SetNdivisions(1005);
  }
  cans->Print(pathPNGOut.c_str());
}


//______________________________________________________________________________
// 多重性测试
void PrintMulti(Int_t ssdindex, Int_t num_multi, string layertag, std::ofstream& fileout,
  Double_t* multiratio, Double_t sumratio)
{
  cout<<setw(5)<<Form("SSD%d_%s",ssdindex+1, layertag.c_str());
  fileout<<setw(5)<<Form("SSD%d_%s",ssdindex+1, layertag.c_str());
  for (Int_t multi=0; multi<num_multi; multi++) {
    cout<<setw(15)<<multiratio[multi]<<setw(15);
    fileout<<setw(15)<<multiratio[multi]<<setw(15);
  }
  cout<<sumratio<<endl;
  fileout<<sumratio<<endl;
}

void CSHINETrackReconstruction::CheckLayerMultiPercentage()
{
  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string L3ATag("L3A");

  Int_t Num_Multi = 7; // hit = 0, 1, 2, 3, 4, 5, 6

  std::string pathDataOut(Form("%sdata_TrackReconstruction/SSD_CheckLayerMultiPercentage.dat",PATHDATAFOLDER));

  ofstream fileout(pathDataOut.c_str());
  fileout<<setw(5)<<"SSD_Layer"<<setw(14)<<"Multi=0"<<setw(14)<<"Multi=1"
         <<setw(15)<<"Multi=2"<<setw(15)<<"Multi=3"<<setw(15)<<"Multi=4"
         <<setw(15)<<"Multi=5"<<setw(15)<<"Multi=6"<<setw(12)<<"Sum"<<endl;
  fileout<<""<<endl;

  Int_t Entries_L1S[NUM_SSD][Num_Multi];
  Int_t Entries_L2F[NUM_SSD][Num_Multi];
  Int_t Entries_L2B[NUM_SSD][Num_Multi];
  Int_t Entries_L3A[NUM_SSD][Num_Multi];
  Double_t MultiRatio_L1S[NUM_SSD][Num_Multi];
  Double_t MultiRatio_L2F[NUM_SSD][Num_Multi];
  Double_t MultiRatio_L2B[NUM_SSD][Num_Multi];
  Double_t MultiRatio_L3A[NUM_SSD][Num_Multi];
  Double_t Sum_L1S[NUM_SSD];
  Double_t Sum_L2F[NUM_SSD];
  Double_t Sum_L2B[NUM_SSD];
  Double_t Sum_L3A[NUM_SSD];

  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<Num_Multi; j++) {
      Entries_L1S[i][j] = 0;  MultiRatio_L1S[i][j] = 0.;
      Entries_L2F[i][j] = 0;  MultiRatio_L2F[i][j] = 0.;
      Entries_L2B[i][j] = 0;  MultiRatio_L2B[i][j] = 0.;
      Entries_L3A[i][j] = 0;  MultiRatio_L3A[i][j] = 0.;
    }
    Sum_L1S[i] = 0.;
    Sum_L2F[i] = 0.;
    Sum_L2B[i] = 0.;
    Sum_L3A[i] = 0.;
  }

  Long64_t nentries = fChainLayerTree->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries;ientry++) {

    fChainLayerTree->GetEntry(ientry);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      for (Int_t multi=0; multi<Num_Multi; multi++) {
        if (fLayerEvent.fSSDL1SMulti[ssdindex]==multi)  Entries_L1S[ssdindex][multi]++;
        if (fLayerEvent.fSSDL2FMulti[ssdindex]==multi)  Entries_L2F[ssdindex][multi]++;
        if (fLayerEvent.fSSDL2BMulti[ssdindex]==multi)  Entries_L2B[ssdindex][multi]++;
        if (fLayerEvent.fSSDCsIMulti[ssdindex]==multi)  Entries_L3A[ssdindex][multi]++;
      }
    }
  }
  // 计算各种情况的比例
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t multi=0; multi<Num_Multi; multi++) {
      MultiRatio_L1S[ssdindex][multi] = (Double_t) Entries_L1S[ssdindex][multi]/nentries;
      Sum_L1S[ssdindex] += MultiRatio_L1S[ssdindex][multi];

      MultiRatio_L2F[ssdindex][multi] = (Double_t) Entries_L2F[ssdindex][multi]/nentries;
      Sum_L2F[ssdindex] += MultiRatio_L2F[ssdindex][multi];

      MultiRatio_L2B[ssdindex][multi] = (Double_t) Entries_L2B[ssdindex][multi]/nentries;
      Sum_L2B[ssdindex] += MultiRatio_L2B[ssdindex][multi];

      MultiRatio_L3A[ssdindex][multi] = (Double_t) Entries_L3A[ssdindex][multi]/nentries;
      Sum_L3A[ssdindex] += MultiRatio_L3A[ssdindex][multi];
    }
  }
  // 输入结果
  cout<<setw(5)<<"SSD_Layer"<<setw(14)<<"Multi=0"<<setw(14)<<"Multi=1"
      <<setw(15)<<"Multi=2"<<setw(15)<<"Multi=3"<<setw(15)<<"Multi=4"
      <<setw(15)<<"Multi=5"<<setw(15)<<"Multi=6"<<setw(12)<<"Sum"<<endl;
  cout<<endl;

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    PrintMulti(ssdindex, Num_Multi, L1STag.c_str(), fileout, MultiRatio_L1S[ssdindex], Sum_L1S[ssdindex]);
    PrintMulti(ssdindex, Num_Multi, L2FTag.c_str(), fileout, MultiRatio_L2F[ssdindex], Sum_L2F[ssdindex]);
    PrintMulti(ssdindex, Num_Multi, L2BTag.c_str(), fileout, MultiRatio_L2B[ssdindex], Sum_L2B[ssdindex]);
    PrintMulti(ssdindex, Num_Multi, L3ATag.c_str(), fileout, MultiRatio_L3A[ssdindex], Sum_L3A[ssdindex]);
    cout<<endl;
    fileout<<endl;
  }
  fileout.close();
}


//******************************************************************************
//                电荷分配效应
//             -----------------
// 检查 alpha 刻度数据中的 charge sharing
void DrawChargeSharing(std::string pathpdf, TH2D* h2d[4][15], Double_t latxe_ratio[4][15])
{
  TCanvas* cans = new TCanvas("cans","cans",1500,900);
  cans->Divide(5,3);

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t strip=0; strip<(NUM_STRIP-1); strip++) {
      cans->cd(strip+1);
      gPad->SetBottomMargin(0.12);
      gPad->SetLeftMargin(0.12);
      h2d[ssdindex][strip]->Draw("COL");
      h2d[ssdindex][strip]->GetXaxis()->SetLabelSize(0.07);
      h2d[ssdindex][strip]->GetXaxis()->SetNdivisions(505);
      h2d[ssdindex][strip]->GetYaxis()->SetLabelSize(0.07);
      h2d[ssdindex][strip]->GetYaxis()->SetNdivisions(505);

      TLatex* latex = new TLatex(120.,400.,Form("R(Sharing)=%.03f (%%)",latxe_ratio[ssdindex][strip]));
      latex->SetTextSize(0.06);
      latex->SetTextColor(kMagenta);
      latex->Draw();

      gPad->Modified();
      gPad->Update();
    }
    cans->Print(pathpdf.c_str());
  }
}

void DrawChargeSharingERatio(std::string pathpdf, TH1D* h1[4][15])
{
  TCanvas* cans = new TCanvas("cans","cans",1500,900);
  cans->Divide(5,3);

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t strip=0; strip<(NUM_STRIP-1); strip++) {
      cans->cd(strip+1);
      gPad->SetBottomMargin(0.12);
      gPad->SetLeftMargin(0.12);
      h1[ssdindex][strip]->Draw();
      h1[ssdindex][strip]->GetXaxis()->SetLabelSize(0.07);
      h1[ssdindex][strip]->GetXaxis()->SetNdivisions(505);
      h1[ssdindex][strip]->GetYaxis()->SetLabelSize(0.07);
      h1[ssdindex][strip]->GetYaxis()->SetNdivisions(505);

      gPad->Modified();
      gPad->Update();
    }
    cans->Print(pathpdf.c_str());
  }
}


void CSHINETrackReconstruction::CheckAlphaCaliChargeSharing()
{
  gStyle->SetOptStat(kFALSE);

  std::string pathAlphaCaliChargeSharing(Form("%sfigure_TrackReconstruction/AlphaCaliChargeSharing.pdf",PATHFIGURESFOLDER));
  std::string pathAlphaCaliChargeSharingBegin(Form("%sfigure_TrackReconstruction/AlphaCaliChargeSharing.pdf[",PATHFIGURESFOLDER));
  std::string pathAlphaCaliChargeSharingEnd(Form("%sfigure_TrackReconstruction/AlphaCaliChargeSharing.pdf]",PATHFIGURESFOLDER));

  Double_t range = 500;
  Int_t NBins = 500;

  Int_t conut_hit_L1S[4][15] = {{0}};
  Int_t conut_hit_L2F[4][15] = {{0}};
  Int_t conut_hit_L2B[4][15] = {{0}};

  Int_t conut_cs_L1S[4][15] = {{0}};
  Int_t conut_cs_L2F[4][15] = {{0}};
  Int_t conut_cs_L2B[4][15] = {{0}};

  Double_t ene_cs_L1S[4][15] = {{0.}};
  Double_t ene_cs_L2F[4][15] = {{0.}};
  Double_t ene_cs_L2B[4][15] = {{0.}};

  Double_t ratio_L1S[4][15] = {{0.}};
  Double_t ratio_L2F[4][15] = {{0.}};
  Double_t ratio_L2B[4][15] = {{0.}};

  Double_t* L1S_SiEChCut = fCSHINESSDCalibratedData.GetSiEChCut ("L1S");
  Double_t* L2F_SiEChCut = fCSHINESSDCalibratedData.GetSiEChCut ("L2F");
  Double_t* L2B_SiEChCut = fCSHINESSDCalibratedData.GetSiEChCut ("L2B");

  TH2D* h2_L1S_ChargeSharing[NUM_SSD][NUM_STRIP-1];
  TH2D* h2_L2F_ChargeSharing[NUM_SSD][NUM_STRIP-1];
  TH2D* h2_L2B_ChargeSharing[NUM_SSD][NUM_STRIP-1];
  TH1D* h1_L1S_ERatio[NUM_SSD][NUM_STRIP-1];
  TH1D* h1_L2F_ERatio[NUM_SSD][NUM_STRIP-1];
  TH1D* h1_L2B_ERatio[NUM_SSD][NUM_STRIP-1];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t strip=0; strip<(NUM_STRIP-1); strip++) {
      h2_L1S_ChargeSharing[ssdindex][strip] = new TH2D(Form("SSD%d_L1S_Strip%02dvsStrip%02d",ssdindex+1,strip,strip+1),
                                                       Form("SSD%d_L1S_Strip%02dvsStrip%02d",ssdindex+1,strip,strip+1),
                                                       NBins,0,range,NBins,0,range);

      h2_L2F_ChargeSharing[ssdindex][strip] = new TH2D(Form("SSD%d_L2F_Strip%02dvsStrip%02d",ssdindex+1,strip,strip+1),
                                                       Form("SSD%d_L2F_Strip%02dvsStrip%02d",ssdindex+1,strip,strip+1),
                                                       NBins,0,range,NBins,0,range);

      h2_L2B_ChargeSharing[ssdindex][strip] = new TH2D(Form("SSD%d_L2B_Strip%02dvsStrip%02d",ssdindex+1,strip,strip+1),
                                                       Form("SSD%d_L2B_Strip%02dvsStrip%02d",ssdindex+1,strip,strip+1),
                                                       NBins,0,range,NBins,0,range);

      h1_L1S_ERatio[ssdindex][strip] = new TH1D(Form("SSD%d_L1S_Strip%02dvsStrip%02d_ERatio",ssdindex+1,strip,strip+1),
                                                Form("SSD%d_L1S_Strip%02dvsStrip%02d_ERatio",ssdindex+1,strip,strip+1),1000,0.,1.);

      h1_L2F_ERatio[ssdindex][strip] = new TH1D(Form("SSD%d_L2F_Strip%02dvsStrip%02d_ERatio",ssdindex+1,strip,strip+1),
                                                Form("SSD%d_L2F_Strip%02dvsStrip%02d_ERatio",ssdindex+1,strip,strip+1),1000,0.,1.);

      h1_L2B_ERatio[ssdindex][strip] = new TH1D(Form("SSD%d_L2B_Strip%02dvsStrip%02d_ERatio",ssdindex+1,strip,strip+1),
                                                Form("SSD%d_L2B_Strip%02dvsStrip%02d_ERatio",ssdindex+1,strip,strip+1),1000,0.,1.);
    }
  }

  // reading tree data
  Int_t SSD_L1S_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2F_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2B_E[NUM_SSD][NUM_STRIP];
  // for L1
  TChain* mychain_L1 = new TChain("KrPb");
  for (Int_t i=1; i<9; i++) {
    mychain_L1->Add(Form("%sMapRoot/MapSSD_L1_AlphaCali%04d.root",PATHROOTFILESFOLDER,i));
  }
  mychain_L1->SetBranchStatus("*",false);
//  mychain_L1->SetEntries(500000);

  // for L2
  TChain* mychain_L2 = new TChain("KrPb");
  for (Int_t i=0; i<49; i++) {
    mychain_L2->Add(Form("%sMapRoot/MapSSD_L2_AlphaCali%04d.root",PATHROOTFILESFOLDER,i));
  }
  mychain_L2->SetBranchStatus("*",false);
//  mychain_L2->SetEntries(500000);

  //
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    mychain_L1->SetBranchStatus (Form("SSD%d_L1S_E",ssdindex+1),true);
    mychain_L1->SetBranchAddress(Form("SSD%d_L1S_E",ssdindex+1),SSD_L1S_E[ssdindex]);

    mychain_L2->SetBranchStatus (Form("SSD%d_L2F_E",ssdindex+1),true);
    mychain_L2->SetBranchStatus (Form("SSD%d_L2B_E",ssdindex+1),true);
    mychain_L2->SetBranchAddress(Form("SSD%d_L2F_E",ssdindex+1),SSD_L2F_E[ssdindex]);
    mychain_L2->SetBranchAddress(Form("SSD%d_L2B_E",ssdindex+1),SSD_L2B_E[ssdindex]);
  }

  // for L1S:
  cout<<"*** Processing L1S ........."<<endl;
  Long64_t nentries1 = mychain_L1->GetEntries();
  cout<<"nentries = "<<nentries1<<endl;
  for (Long64_t ientry=0; ientry<nentries1; ientry++) {
    mychain_L1->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries1);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      for (Int_t strip=0; strip<(NUM_STRIP-1); strip++) {
        if (SSD_L1S_E[ssdindex][strip]>L1S_SiEChCut[ssdindex*NUM_STRIP+strip] || SSD_L1S_E[ssdindex][strip+1]>L1S_SiEChCut[ssdindex*NUM_STRIP+strip+1]) {
          h2_L1S_ChargeSharing[ssdindex][strip]->Fill(SSD_L1S_E[ssdindex][strip],SSD_L1S_E[ssdindex][strip+1]);
          conut_hit_L1S[ssdindex][strip]++;
        }
        if (SSD_L1S_E[ssdindex][strip]>L1S_SiEChCut[ssdindex*NUM_STRIP+strip] && SSD_L1S_E[ssdindex][strip+1]>L1S_SiEChCut[ssdindex*NUM_STRIP+strip+1]) {
          h1_L1S_ERatio[ssdindex][strip]->Fill(CalcRatio(SSD_L1S_E[ssdindex][strip],SSD_L1S_E[ssdindex][strip+1]));
          conut_cs_L1S[ssdindex][strip]++;
        }
      }
    }
  }

  // for L2:
  cout<<"*** Processing L2 ........."<<endl;
  Long64_t nentries2 = mychain_L2->GetEntries();
  cout<<"nentries = "<<nentries2<<endl;
  for (Long64_t ientry=0; ientry<nentries2; ientry++) {
    mychain_L2->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries2);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      for (Int_t strip=0; strip<(NUM_STRIP-1); strip++) {
        // for L2F
        if (SSD_L2F_E[ssdindex][strip]>L2F_SiEChCut[ssdindex*NUM_STRIP+strip] || SSD_L2F_E[ssdindex][strip+1]>L2F_SiEChCut[ssdindex*NUM_STRIP+strip+1]) {
          h2_L2F_ChargeSharing[ssdindex][strip]->Fill(SSD_L2F_E[ssdindex][strip],SSD_L2F_E[ssdindex][strip+1]);
          conut_hit_L2F[ssdindex][strip]++;
        }
        if (SSD_L2F_E[ssdindex][strip]>L2F_SiEChCut[ssdindex*NUM_STRIP+strip] && SSD_L2F_E[ssdindex][strip+1]>L2F_SiEChCut[ssdindex*NUM_STRIP+strip+1]) {
          h1_L2F_ERatio[ssdindex][strip]->Fill(CalcRatio(SSD_L2F_E[ssdindex][strip],SSD_L2F_E[ssdindex][strip+1]));
          conut_cs_L2F[ssdindex][strip]++;
        }
        // for L2B
        if (SSD_L2B_E[ssdindex][strip]>L2B_SiEChCut[ssdindex*NUM_STRIP+strip] || SSD_L2B_E[ssdindex][strip+1]>L2B_SiEChCut[ssdindex*NUM_STRIP+strip+1]) {
          h2_L2B_ChargeSharing[ssdindex][strip]->Fill(SSD_L2B_E[ssdindex][strip],SSD_L2B_E[ssdindex][strip+1]);
          conut_hit_L2B[ssdindex][strip]++;
        }
        if (SSD_L2B_E[ssdindex][strip]>L2B_SiEChCut[ssdindex*NUM_STRIP+strip] && SSD_L2B_E[ssdindex][strip+1]>L2B_SiEChCut[ssdindex*NUM_STRIP+strip+1]) {
          h1_L2B_ERatio[ssdindex][strip]->Fill(CalcRatio(SSD_L2B_E[ssdindex][strip],SSD_L2B_E[ssdindex][strip+1]));
          conut_cs_L2B[ssdindex][strip]++;
        }
      }
    }
  }

  // 计算电荷分配的占比
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t strip=0; strip<(NUM_STRIP-1); strip++) {
      ratio_L1S[ssdindex][strip] = (Double_t) conut_cs_L1S[ssdindex][strip]/conut_hit_L1S[ssdindex][strip] * 100; // %
      ratio_L2F[ssdindex][strip] = (Double_t) conut_cs_L2F[ssdindex][strip]/conut_hit_L2F[ssdindex][strip] * 100; // %
      ratio_L2B[ssdindex][strip] = (Double_t) conut_cs_L2B[ssdindex][strip]/conut_hit_L2B[ssdindex][strip] * 100; // %
    }
  }

  // 画图
  TCanvas* c_begin = new TCanvas("c_begin","c_begin",600,600);
  TCanvas* c_end = new TCanvas("c_end","c_end",600,600);

  c_begin->Print(pathAlphaCaliChargeSharingBegin.c_str());
  DrawChargeSharing(pathAlphaCaliChargeSharing, h2_L1S_ChargeSharing, ratio_L1S);
  DrawChargeSharing(pathAlphaCaliChargeSharing, h2_L2F_ChargeSharing, ratio_L2F);
  DrawChargeSharing(pathAlphaCaliChargeSharing, h2_L2B_ChargeSharing, ratio_L2B);
  DrawChargeSharingERatio(pathAlphaCaliChargeSharing, h1_L1S_ERatio);
  DrawChargeSharingERatio(pathAlphaCaliChargeSharing, h1_L2F_ERatio);
  DrawChargeSharingERatio(pathAlphaCaliChargeSharing, h1_L2B_ERatio);
  c_end->Print(pathAlphaCaliChargeSharingEnd.c_str());
}


//____________________________________
// 检查实验数据中: 如果硅条中有相邻的条点火, 则定义这样相邻条的数目为 ClusterSize
/* //这一段用于检查每种 clustersize 的情况
if (ClusterSize_L2F==2) {
  cout<<"SSD"<<ssdindex<<setw(10)<<fLayerEvent.fSSDL2FMulti[ssdindex]<<setw(10)<<ClusterSize_L2F<<setw(10);
  for (Int_t i=0; i<fLayerEvent.fL2FMulti; i++) {
    if (fLayerEvent.fL2FSSDNum[i]==ssdindex) {
      cout<<setw(10)<<fLayerEvent.fL2FNumStrip[i];
    }
  }
  cout<<endl;
} */
void CSHINETrackReconstruction::CheckClusterSize_Si()
{
  Int_t NBins = 2000;
  Int_t Count_ClusterSize_L1S;
  Int_t Count_ClusterSize_L2F;
  Int_t Count_ClusterSize_L2B;
  Int_t CountSum_ClusterSize_L1S;
  Int_t CountSum_ClusterSize_L2F;
  Int_t CountSum_ClusterSize_L2B;

  Double_t Ratio_ClusterSize_L1S;
  Double_t Ratio_ClusterSize_L2F;
  Double_t Ratio_ClusterSize_L2B;
  Double_t RatioSum_ClusterSize_L1S;
  Double_t RatioSum_ClusterSize_L2F;
  Double_t RatioSum_ClusterSize_L2B;

  Int_t Count_L1SMulti_ge1[4] = {0};
  Int_t Count_L2FMulti_ge1[4] = {0};
  Int_t Count_L2BMulti_ge1[4] = {0};

  Int_t ClusterSize_L1S = -99;
  Int_t ClusterSize_L2F = -99;
  Int_t ClusterSize_L2B = -99;

  Int_t count_L1S[4] = {0};
  Int_t count_L2F[4] = {0};
  Int_t count_L2B[4] = {0};

  std::vector<Int_t> NumStrip_L1S;
  std::vector<Int_t> NumStrip_L2F;
  std::vector<Int_t> NumStrip_L2B;

  std::string HistName_L1S[NUM_SSD];
  std::string HistName_L2F[NUM_SSD];
  std::string HistName_L2B[NUM_SSD];
  TH1I* hist_ClusterSize_L1S[NUM_SSD];
  TH1I* hist_ClusterSize_L2F[NUM_SSD];
  TH1I* hist_ClusterSize_L2B[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    HistName_L1S[ssdindex] = Form("SSD%d_L1S_ClusterSize",ssdindex+1);
    HistName_L2F[ssdindex] = Form("SSD%d_L2F_ClusterSize",ssdindex+1);
    HistName_L2B[ssdindex] = Form("SSD%d_L2B_ClusterSize",ssdindex+1);
    hist_ClusterSize_L1S[ssdindex] = new TH1I(HistName_L1S[ssdindex].c_str(), HistName_L1S[ssdindex].c_str(), NBins, 0, NBins);
    hist_ClusterSize_L2F[ssdindex] = new TH1I(HistName_L2F[ssdindex].c_str(), HistName_L2F[ssdindex].c_str(), NBins, 0, NBins);
    hist_ClusterSize_L2B[ssdindex] = new TH1I(HistName_L2B[ssdindex].c_str(), HistName_L2B[ssdindex].c_str(), NBins, 0, NBins);
  }

  Long64_t nentries = fChainLayerTree->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries;ientry++) {

    fChainLayerTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      if (fLayerEvent.fSSDL1SMulti[ssdindex]>=1) Count_L1SMulti_ge1[ssdindex]++;
      if (fLayerEvent.fSSDL2FMulti[ssdindex]>=1) Count_L2FMulti_ge1[ssdindex]++;
      if (fLayerEvent.fSSDL2BMulti[ssdindex]>=1) Count_L2BMulti_ge1[ssdindex]++;

      // for L1S
      if (fLayerEvent.fSSDL1SMulti[ssdindex]>=2 && fLayerEvent.fSSDL1SMulti[ssdindex]<=MULTICUT_L1S) {
        for (Int_t l1smulti=0; l1smulti<fLayerEvent.fL1SMulti; l1smulti++) {
          if (fLayerEvent.fL1SSSDNum[l1smulti]==ssdindex) {
            NumStrip_L1S.push_back(fLayerEvent.fL1SNumStrip[l1smulti]);
          }
        }
        CalcCluseterSize_SiLayer(fLayerEvent.fSSDL1SMulti[ssdindex], NumStrip_L1S, ClusterSize_L1S);
        hist_ClusterSize_L1S[ssdindex]->Fill(ClusterSize_L1S);
      }

      // for L2F
      if (fLayerEvent.fSSDL2FMulti[ssdindex]>=2 && fLayerEvent.fSSDL2FMulti[ssdindex]<=MULTICUT_L2F) {
        for (Int_t l2fmulti=0; l2fmulti<fLayerEvent.fL2FMulti; l2fmulti++) {
          if (fLayerEvent.fL2FSSDNum[l2fmulti]==ssdindex) {
            NumStrip_L2F.push_back(fLayerEvent.fL2FNumStrip[l2fmulti]);
          }
        }
        CalcCluseterSize_SiLayer(fLayerEvent.fSSDL2FMulti[ssdindex], NumStrip_L2F, ClusterSize_L2F);
        hist_ClusterSize_L2F[ssdindex]->Fill(ClusterSize_L2F);
      }
      // for L2B
      if (fLayerEvent.fSSDL2BMulti[ssdindex]>=2 && fLayerEvent.fSSDL2BMulti[ssdindex]<=MULTICUT_L2B) {
        for (Int_t l2bmulti=0; l2bmulti<fLayerEvent.fL2BMulti; l2bmulti++) {
          if (fLayerEvent.fL2BSSDNum[l2bmulti]==ssdindex) {
            NumStrip_L2B.push_back(fLayerEvent.fL2BNumStrip[l2bmulti]);
          }
        }
        CalcCluseterSize_SiLayer(fLayerEvent.fSSDL2BMulti[ssdindex], NumStrip_L2B, ClusterSize_L2B);
        hist_ClusterSize_L2B[ssdindex]->Fill(ClusterSize_L2B);
      }
      NumStrip_L1S.clear();
      NumStrip_L2F.clear();
      NumStrip_L2B.clear();
    }
  }

  // 统计 ClusterSize 的取值与比例
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    CountSum_ClusterSize_L1S = 0;
    CountSum_ClusterSize_L2F = 0;
    CountSum_ClusterSize_L2B = 0;
    RatioSum_ClusterSize_L1S = 0.;
    RatioSum_ClusterSize_L2F = 0.;
    RatioSum_ClusterSize_L2B = 0.;
    // L1S
    cout<<endl;
    cout<<Form("SSD%d_L1S\n", ssdindex+1);
    for (Int_t nbin=1; nbin<NBins+1; nbin++) {
      Count_ClusterSize_L1S = hist_ClusterSize_L1S[ssdindex]->GetBinContent(nbin);
      if (Count_ClusterSize_L1S!=0) {
        Ratio_ClusterSize_L1S = (Double_t) Count_ClusterSize_L1S/Count_L1SMulti_ge1[ssdindex];
        cout<<left<<setw(30)<<Form("ClusterSize = %d", nbin-1)<<left<<setw(30)<<Form("count = %d",Count_ClusterSize_L1S);
        cout<<setprecision(4)<<left<<setw(10)<<"Ratio = "<<Ratio_ClusterSize_L1S<<endl;
        CountSum_ClusterSize_L1S += Count_ClusterSize_L1S;
        RatioSum_ClusterSize_L1S += Ratio_ClusterSize_L1S;
      }
    }
    cout<<"Count_L1SMulti_ge1 = "<<Count_L1SMulti_ge1[ssdindex]<<endl;
    cout<<"CountSum_L1S = "<<CountSum_ClusterSize_L1S<<endl;
    cout<<"RatioSum_L1S = "<<RatioSum_ClusterSize_L1S<<endl;

    // L2F
    cout<<endl;
    cout<<Form("SSD%d_L2F\n", ssdindex+1);
    for (Int_t nbin=1; nbin<NBins+1; nbin++) {
      Count_ClusterSize_L2F = hist_ClusterSize_L2F[ssdindex]->GetBinContent(nbin);
      if (Count_ClusterSize_L2F!=0) {
        Ratio_ClusterSize_L2F = (Double_t) Count_ClusterSize_L2F/Count_L2FMulti_ge1[ssdindex];
        cout<<left<<setw(30)<<Form("ClusterSize = %d", nbin-1)<<left<<setw(30)<<Form("count = %d",Count_ClusterSize_L2F);
        cout<<setprecision(4)<<left<<setw(10)<<"Ratio = "<<Ratio_ClusterSize_L2F<<endl;
        CountSum_ClusterSize_L2F += Count_ClusterSize_L2F;
        RatioSum_ClusterSize_L2F += Ratio_ClusterSize_L2F;
      }
    }
    cout<<"Count_L2FMulti_ge1 = "<<Count_L2FMulti_ge1[ssdindex]<<endl;
    cout<<"CountSum_L2F = "<<CountSum_ClusterSize_L2F<<endl;
    cout<<"RatioSum_L2F = "<<RatioSum_ClusterSize_L2F<<endl;
    // L2B
    cout<<endl;
    cout<<Form("SSD%d_L2B\n", ssdindex+1);
    for (Int_t nbin=1; nbin<NBins+1; nbin++) {
      Count_ClusterSize_L2B = hist_ClusterSize_L2B[ssdindex]->GetBinContent(nbin);
      if (Count_ClusterSize_L2B!=0) {
        Ratio_ClusterSize_L2B = (Double_t) Count_ClusterSize_L2B/Count_L2BMulti_ge1[ssdindex];
        cout<<left<<setw(30)<<Form("ClusterSize = %d", nbin-1)<<left<<setw(30)<<Form("count = %d",Count_ClusterSize_L2B);
        cout<<setprecision(4)<<left<<setw(10)<<"Ratio = "<<Ratio_ClusterSize_L2B<<endl;
        CountSum_ClusterSize_L2B += Count_ClusterSize_L2B;
        RatioSum_ClusterSize_L2B += Ratio_ClusterSize_L2B;
      }
    }
    cout<<"Count_L2BMulti_ge1 = "<<Count_L2BMulti_ge1[ssdindex]<<endl;
    cout<<"CountSum_L2B = "<<CountSum_ClusterSize_L2B<<endl;
    cout<<"RatioSum_L2B = "<<RatioSum_ClusterSize_L2B<<endl;
  }

  TCanvas* cans_L1S = new TCanvas("cans_L1S", "cans_L1S", 1000, 1000);
  TCanvas* cans_L2F = new TCanvas("cans_L2F", "cans_L2F", 1000, 1000);
  TCanvas* cans_L2B = new TCanvas("cans_L2B", "cans_L2B", 1000, 1000);
  cans_L1S->Divide(2,2);
  cans_L2F->Divide(2,2);
  cans_L2B->Divide(2,2);
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    cans_L1S->cd(ssdindex+1);
    gPad->SetLogy();
    hist_ClusterSize_L1S[ssdindex]->Draw();

    cans_L2F->cd(ssdindex+1);
    gPad->SetLogy();
    hist_ClusterSize_L2F[ssdindex]->Draw();

    cans_L2B->cd(ssdindex+1);
    gPad->SetLogy();
    hist_ClusterSize_L2B[ssdindex]->Draw();
  }
}

//_________________________
// 对 cluster 进行分类
void CSHINETrackReconstruction::CalcCluseterSize_SiLayer(Int_t layermulti, vector<Int_t> numstrip, Int_t& clustersize)
{
  // layermulti = 2 的情况
  if (layermulti==2) {
    if (numstrip[0]-numstrip[1] == -1)        clustersize = 2;  // ---**------
    else                                      clustersize = 0;  // ---*--*----
  }
  // layermulti = 3 的情况:
  if (layermulti==3) {
    if (numstrip[0]-numstrip[2] == -2)        clustersize = 3; //----***------
    else if (numstrip[0]-numstrip[1] == -1)   clustersize = 2; //--**---*-----
    else if (numstrip[1]-numstrip[2] == -1)   clustersize = 2; //--*---**-----
    else                                      clustersize = 0; //---*--*----*--
  }
  // layermulti = 4 的情况
  if (layermulti==4) {
    if (numstrip[0]-numstrip[3] == -3)        clustersize = 4; //----****------
    else if (numstrip[0]-numstrip[2] == -2)   clustersize = 3; //---***--*-----
    else if (numstrip[1]-numstrip[3] == -2)   clustersize = 3; //---*---***----
    else if (numstrip[0]-numstrip[1] == -1) {
      if (numstrip[2]-numstrip[3] == -1)      clustersize = 22;//---**---**---
      else                                    clustersize = 2; //---**--*--*--
    }
    else if (numstrip[1]-numstrip[2] == -1)   clustersize = 2; //---*--**---*--
    else if (numstrip[2]-numstrip[3] == -1)   clustersize = 2; //---*--*--**---
    else                                      clustersize = 0; //--*--*--*--*--
  }
  // layermulti = 5 的情况
  if (layermulti==5) {
    if (numstrip[0]-numstrip[4] == -4)        clustersize = 5; //---*****-----
    else if (numstrip[0]-numstrip[3] == -3)   clustersize = 4; //---****--*---
    else if (numstrip[1]-numstrip[4] == -3)   clustersize = 4; //---*--****---
    else if (numstrip[0]-numstrip[2] == -2) {
      if (numstrip[3]-numstrip[4] == -1)      clustersize = 23;//---***--**---
      else                                    clustersize = 3; //--***--*--*--
    }
    else if (numstrip[1]-numstrip[3] == -2)   clustersize = 3; //--*--***-*---
    else if (numstrip[2]-numstrip[4] == -2) {
      if (numstrip[0]-numstrip[1] == -1)      clustersize = 23;//--**--***----
      else                                    clustersize = 3; //--*--*--***--
    }
    else if (numstrip[0]-numstrip[1] == -1) {
      if ((numstrip[2]-numstrip[3] == -1) || (numstrip[3]-numstrip[4] == -1))  clustersize = 122; //---**--**--*-- or ---**--*--**---
      else                                    clustersize = 2; //---**--*--*--*--
    }
    else if (numstrip[1]-numstrip[2] == -1) {
      if (numstrip[3]-numstrip[4] == -1)      clustersize = 122;  //--*--**---**--
      else                                    clustersize = 2; //--*--**--*--*--
    }
    else if (numstrip[2]-numstrip[3] == -1)   clustersize = 2; //--*--*--**--*--
    else if (numstrip[3]-numstrip[4] == -1)   clustersize = 2; //--*--*--*--**--
    else                                      clustersize = 0; //--*--*--*--*--*--
  }
  // layermulti = 6 的情况
  if (layermulti==6) {
    if (numstrip[0]-numstrip[5] == -5)        clustersize = 6; //---******----
    else if (numstrip[0]-numstrip[4] == -4)   clustersize = 5; //---*****--*--
    else if (numstrip[1]-numstrip[5] == -4)   clustersize = 5; //---*--*****--
    else if (numstrip[0]-numstrip[3] == -3) {
      if (numstrip[4]-numstrip[5] == -1)      clustersize = 24;//---****--**--
      else                                    clustersize = 4; //---****--*--*--
    }
    else if (numstrip[1]-numstrip[4] == -3)   clustersize = 4; //---*--****--*--
    else if (numstrip[2]-numstrip[5] == -3) {
      if (numstrip[0]-numstrip[1] == -1)      clustersize = 24;//--**--****--
      else                                    clustersize = 4; //--*--*--****--
    }
    else if (numstrip[0]-numstrip[2] == -2) {
      if (numstrip[3]-numstrip[5] == -2)      clustersize = 33; //---***---***--
      else if ((numstrip[3]-numstrip[4] == -1) || (numstrip[3]-numstrip[4] == -1)) clustersize = 123; //--***--**--*-- or --***--*--**--
      else                                    clustersize = 3; //--***--*--*--*--
    }
    else if (numstrip[1]-numstrip[3] == -2) {
      if (numstrip[4]-numstrip[5] == -1)      clustersize = 123; //--*--***--**--
      else                                    clustersize = 3; //---*--***--*--*--
    }
    else if (numstrip[2]-numstrip[4] == -2) {
      if (numstrip[0]-numstrip[1] == -1)      clustersize = 123; //--**---***--*--
      else                                    clustersize = 3; //--*--*--***--*--
    }
    else if (numstrip[3]-numstrip[5] == -2) {
      if ((numstrip[0]-numstrip[1] == -1) || (numstrip[1]-numstrip[2] == -1))  clustersize = 123; //--**--*--***-- or --*--**--***--
      else                                    clustersize = 3; //--*--*--*--***--
    }
    else if (numstrip[0]-numstrip[1] == -1) {
      if (numstrip[2]-numstrip[3] == -1) {
        if (numstrip[4]-numstrip[5] == -1)   clustersize = 222; //--**--**--**--
        else                                 clustersize = 1122;//--**--**--*--*--
      }
      else if (numstrip[3]-numstrip[4] == -1)  clustersize = 1122; //--**--*--**-*--
      else if (numstrip[4]-numstrip[5] == -1)  clustersize = 1122; //--**--*--*--**--
      else                                     clustersize = 2; //--**--*--*--*--*--
    }
    else if  (numstrip[1]-numstrip[2] == -1) {
      if (numstrip[3]-numstrip[4] == -1)       clustersize = 1122; //--*--**--**--*--
      else if (numstrip[4]-numstrip[5] == -1)  clustersize = 1122; //--*--**--*--**--
      else                                     clustersize = 2; //--*--**--*--*--*--
    }
    else if (numstrip[2]-numstrip[3] == -1) {
      if (numstrip[4]-numstrip[5] == -1)      clustersize = 1122; //--*--*--**--**--
      else                                    clustersize = 2; //--*--*--**--*--*--
    }
    else if (numstrip[3]-numstrip[4] == -1)   clustersize = 2; //--*--*--*--**--*--
    else if (numstrip[4]-numstrip[5] == -1)   clustersize = 2; //--*--*--*--*--**--
    else                                      clustersize = 0; //--*--*--*--*--*--*--
  }
}

//__________________________________________________
// 经过以上 CheckClusterSize_Si() 的分析, charge charing
// 只需考虑 clustersize=2 的情况即可 ！
void CSHINETrackReconstruction::CalcClusterSize_Equal2_ERatio()
{
  Int_t strip[2];
  Double_t efound[2];
  Int_t ClusterSize_L2F = -99;

  Double_t ERatio_L1S = -99.;
  Double_t ERatio_L2F = -99.;
  Double_t ERatio_L2B = -99.;
  std::vector<Int_t> NumStrip_L1S;
  std::vector<Int_t> NumStrip_L2F;
  std::vector<Int_t> NumStrip_L2B;
  std::vector<Double_t> EMeV_L1S;
  std::vector<Double_t> EMeV_L2F;
  std::vector<Double_t> EMeV_L2B;

  std::string L1S_ERatio[NUM_SSD];
  std::string L2F_ERatio[NUM_SSD];
  std::string L2B_ERatio[NUM_SSD];
  std::string L1S_ECorr[NUM_SSD];
  std::string L2F_ECorr[NUM_SSD];
  std::string L2B_ECorr[NUM_SSD];

  TH1D* hist_ERatio_L1S[NUM_SSD];
  TH1D* hist_ERatio_L2F[NUM_SSD];
  TH1D* hist_ERatio_L2B[NUM_SSD];
  TH2D* hist_ECorr_L1S[NUM_SSD];
  TH2D* hist_ECorr_L2F[NUM_SSD];
  TH2D* hist_ECorr_L2B[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    L1S_ERatio[ssdindex] = Form("SSD%d_L1S_ERatio",ssdindex+1);
    L2F_ERatio[ssdindex] = Form("SSD%d_L2F_ERatio",ssdindex+1);
    L2B_ERatio[ssdindex] = Form("SSD%d_L2B_ERatio",ssdindex+1);
    L1S_ECorr [ssdindex] = Form("SSD%d_L1S_ECorr",ssdindex+1);
    L2F_ECorr [ssdindex] = Form("SSD%d_L2F_ECorr",ssdindex+1);
    L2B_ECorr [ssdindex] = Form("SSD%d_L2B_ECorr",ssdindex+1);

    hist_ERatio_L1S[ssdindex] = new TH1D(L1S_ERatio[ssdindex].c_str(), L1S_ERatio[ssdindex].c_str(), 100, 0, 1);
    hist_ERatio_L2F[ssdindex] = new TH1D(L2F_ERatio[ssdindex].c_str(), L2F_ERatio[ssdindex].c_str(), 100, 0, 1);
    hist_ERatio_L2B[ssdindex] = new TH1D(L2B_ERatio[ssdindex].c_str(), L2B_ERatio[ssdindex].c_str(), 100, 0, 1);
    hist_ECorr_L1S [ssdindex] = new TH2D(L1S_ECorr[ssdindex].c_str(), L1S_ECorr[ssdindex].c_str(),3000,0,300,3000,0,300);
  }

  Long64_t nentries = fChainLayerTree->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries;ientry++) {

    fChainLayerTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {

      // for L1S
      if (fLayerEvent.fSSDL1SMulti[ssdindex]>=2 && fLayerEvent.fSSDL1SMulti[ssdindex]<=MULTICUT_L1S) {
        for (Int_t l1smulti=0; l1smulti<fLayerEvent.fL1SMulti; l1smulti++) {
          if (fLayerEvent.fL1SSSDNum[l1smulti]==ssdindex) {
            NumStrip_L1S.push_back(fLayerEvent.fL1SNumStrip[l1smulti]);
            EMeV_L1S.push_back(fLayerEvent.fL1SEMeV[l1smulti]);
          }
        }
        ClusterSize_Equal2_SiLayer(fLayerEvent.fSSDL1SMulti[ssdindex], NumStrip_L1S, EMeV_L1S, strip, efound, ERatio_L1S);
        hist_ERatio_L1S[ssdindex]->Fill(ERatio_L1S);
        hist_ECorr_L1S [ssdindex]->Fill(efound[0], efound[1]);
      }
      // for L2F
      if (fLayerEvent.fSSDL2FMulti[ssdindex]>=2 && fLayerEvent.fSSDL2FMulti[ssdindex]<=MULTICUT_L2F) {
        for (Int_t l2fmulti=0; l2fmulti<fLayerEvent.fL2FMulti; l2fmulti++) {
          NumStrip_L2F.push_back(fLayerEvent.fL2FNumStrip[l2fmulti]);
          EMeV_L2F.push_back(fLayerEvent.fL2FEMeV[l2fmulti]);
        }
        ClusterSize_Equal2_SiLayer(fLayerEvent.fSSDL2FMulti[ssdindex], NumStrip_L2F, EMeV_L2F, strip, efound, ERatio_L2F);
        hist_ERatio_L2F[ssdindex]->Fill(ERatio_L2F);
      }
      // for L2B
      if (fLayerEvent.fSSDL2BMulti[ssdindex]>=2 && fLayerEvent.fSSDL2BMulti[ssdindex]<=MULTICUT_L2B) {
        for (Int_t l2bmulti=0; l2bmulti<fLayerEvent.fL2BMulti; l2bmulti++) {
          if (fLayerEvent.fL2BSSDNum[l2bmulti]==ssdindex) {
            NumStrip_L2B.push_back(fLayerEvent.fL2BNumStrip[l2bmulti]);
            EMeV_L2B.push_back(fLayerEvent.fL2BEMeV[l2bmulti]);
          }
        }
        ClusterSize_Equal2_SiLayer(fLayerEvent.fSSDL2BMulti[ssdindex], NumStrip_L2B, EMeV_L2B, strip, efound, ERatio_L2B);
        hist_ERatio_L2B[ssdindex]->Fill(ERatio_L2B);
      }
      NumStrip_L1S.clear();
      NumStrip_L2F.clear();
      NumStrip_L2B.clear();
      EMeV_L1S.clear();
      EMeV_L2F.clear();
      EMeV_L2B.clear();
    }
  }

  TCanvas* cans_L1S = new TCanvas("cans_L1S", "cans_L1S", 1000, 1000);
  TCanvas* cans_L2F = new TCanvas("cans_L2F", "cans_L2F", 1000, 1000);
  TCanvas* cans_L2B = new TCanvas("cans_L2B", "cans_L2B", 1000, 1000);
  TCanvas* cans_L1S_ECorr = new TCanvas("cans_L1S_ECorr", "cans_L1S_ECorr", 1000, 1000);
  cans_L1S->Divide(2,2);
  cans_L2F->Divide(2,2);
  cans_L2B->Divide(2,2);
  cans_L1S_ECorr->Divide(2,2);
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    cans_L1S->cd(ssdindex+1);
    hist_ERatio_L1S[ssdindex]->Draw();

    cans_L2F->cd(ssdindex+1);
    hist_ERatio_L2F[ssdindex]->Draw();

    cans_L2B->cd(ssdindex+1);
    hist_ERatio_L2B[ssdindex]->Draw();

    cans_L1S_ECorr->cd(ssdindex+1);
    hist_ECorr_L1S[ssdindex]->Draw("COLZ");
  }
}


//______________________________
// 计算 clustersize = 2 时两个能量的比值
void CSHINETrackReconstruction::ClusterSize_Equal2_SiLayer(Int_t layermulti, vector<Int_t> numstrip,
  vector<Double_t> ene, Int_t* strip, Double_t* efound, Double_t& eratio)
{
  if (layermulti==2 && (numstrip[0]-numstrip[1] == -1)) {
    eratio = CalcRatio(ene[0], ene[1]); //---**------
    strip[0] = numstrip[0]; strip[1] = numstrip[1];
    efound[0] = ene[0]; efound[1] = ene[1];
  }
  // multi = 3
  if (layermulti==3) {
    if ((numstrip[0]-numstrip[2]!=-2)&&(numstrip[0]-numstrip[1]==-1)) {
      eratio = CalcRatio(ene[0], ene[1]); //--**---*-----
      strip[0] = numstrip[0]; strip[1] = numstrip[1];
      efound[0] = ene[0]; efound[1] = ene[1];
    }
    if ((numstrip[0]-numstrip[2]!=-2)&&(numstrip[1]-numstrip[2]==-1)) {
      eratio = CalcRatio(ene[1], ene[2]);  //--*---**-----
      strip[0] = numstrip[1]; strip[1] = numstrip[2];
      efound[0] = ene[1]; efound[1] = ene[2];
    }
  }
  // multi = 4
  if (layermulti==4) {
    if ((numstrip[0]-numstrip[1]==-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]!=-1)) {
      eratio = CalcRatio(ene[0], ene[1]); //---**--*--*--
      strip[0] = numstrip[0]; strip[1] = numstrip[1];
      efound[0] = ene[0]; efound[1] = ene[1];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]==-1)&&(numstrip[2]-numstrip[3]!=-1)) {
      eratio = CalcRatio(ene[1], ene[2]); //---*--**---*--
      strip[0] = numstrip[1]; strip[1] = numstrip[2];
      efound[0] = ene[1]; efound[1] = ene[2];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]==-1)) {
      eratio = CalcRatio(ene[2], ene[3]); //---*--*--**---
      strip[0] = numstrip[2]; strip[1] = numstrip[3];
      efound[0] = ene[2]; efound[1] = ene[3];
    }
  }
  // multi = 5
  if (layermulti==5) {
    if ((numstrip[0]-numstrip[1]==-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]!=-1)&&(numstrip[3]-numstrip[4]!=-1)) {
      eratio = CalcRatio(ene[0], ene[1]); //---**--*--*--*--
      strip[0] = numstrip[0]; strip[1] = numstrip[1];
      efound[0] = ene[0]; efound[1] = ene[1];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]==-1)&&(numstrip[2]-numstrip[3]!=-1)&&(numstrip[3]-numstrip[4]!=-1)) {
      eratio = CalcRatio(ene[1], ene[2]); //--*--**--*--*--
      strip[0] = numstrip[1]; strip[1] = numstrip[2];
      efound[0] = ene[1]; efound[1] = ene[2];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]==-1)&&(numstrip[3]-numstrip[4]!=-1)) {
      eratio = CalcRatio(ene[2], ene[3]); //--*--*--**--*--
      strip[0] = numstrip[2]; strip[1] = numstrip[3];
      efound[0] = ene[2]; efound[1] = ene[3];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]!=-1)&&(numstrip[3]-numstrip[4]==-1)) {
      eratio = CalcRatio(ene[3], ene[4]); //--*--*--*--**--
      strip[0] = numstrip[3]; strip[1] = numstrip[4];
      efound[0] = ene[3]; efound[1] = ene[4];
    }
  }
  // mulit = 6
  if (layermulti==6) {
    if ((numstrip[0]-numstrip[1]==-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]!=-1)&&
        (numstrip[3]-numstrip[4]!=-1)&&(numstrip[4]-numstrip[5]!=-1)) {
      eratio = CalcRatio(ene[0], ene[1]); //--**--*--*--*--*--
      strip[0] = numstrip[0]; strip[1] = numstrip[1];
      efound[0] = ene[0]; efound[1] = ene[1];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]==-1)&&(numstrip[2]-numstrip[3]!=-1)&&
        (numstrip[3]-numstrip[4]!=-1)&&(numstrip[4]-numstrip[5]!=-1)) {
      eratio = CalcRatio(ene[1], ene[2]); //--*--**--*--*--*--
      strip[0] = numstrip[1]; strip[1] = numstrip[2];
      efound[0] = ene[1]; efound[1] = ene[2];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]==-1)&&
        (numstrip[3]-numstrip[4]!=-1)&&(numstrip[4]-numstrip[5]!=-1)) {
      eratio = CalcRatio(ene[2], ene[3]); //--*--*--**--*--*--
      strip[0] = numstrip[2]; strip[1] = numstrip[3];
      efound[0] = ene[2]; efound[1] = ene[3];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]!=-1)&&
        (numstrip[3]-numstrip[4]==-1)&&(numstrip[4]-numstrip[5]!=-1)) {
      eratio = CalcRatio(ene[3], ene[4]); //--*--*--*--**--*--
      strip[0] = numstrip[3]; strip[1] = numstrip[4];
      efound[0] = ene[3]; efound[1] = ene[4];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]!=-1)&&
        (numstrip[3]-numstrip[4]!=-1)&&(numstrip[4]-numstrip[5]==-1)) {
      eratio = CalcRatio(ene[4], ene[5]); //--*--*--*--*--**--
      strip[0] = numstrip[4]; strip[1] = numstrip[5];
      efound[0] = ene[4]; efound[1] = ene[5];
    }
  }
}


//_________________________________________
void CSHINETrackReconstruction::ClusterSize_Equal2_CsI()
{
  std::vector<Int_t> csinum;
  std::vector<Double_t>  csiech;

  Int_t count_eff[4] = {0};
  Int_t size_2[4]    = {0};
  Int_t size_1213[4] = {0};
  Int_t size_1223[4] = {0};
  Int_t size_1323[4] = {0};
  Int_t size_12[4]   = {0};
  Int_t size_13[4]   = {0};
  Int_t size_23[4]   = {0};

  Double_t ration_sum[4] = {0.};
  Double_t r_size_2[4]   = {0.};
  Double_t r_size_1213[4] = {0.};
  Double_t r_size_1223[4] = {0.};
  Double_t r_size_1323[4] = {0.};
  Double_t r_size_12[4]   = {0.};
  Double_t r_size_13[4]   = {0.};
  Double_t r_size_23[4]   = {0.};

  TH2D* hist_CsI_ECorr[NUM_SSD];
  TH2D* hist_CsI_Array[NUM_SSD][NUM_CSI];
  for (Int_t i=0; i<NUM_SSD; i++) {
    hist_CsI_ECorr[i] = new TH2D(Form("SSD%d_CsIECorr",i+1), Form("SSD%d_CsIECorr",i+1), 4000,0,4000,4000,0,4000);

    for (Int_t j=0; j<NUM_CSI; j++) {
      hist_CsI_Array[i][j] = new TH2D(Form("SSD%d_CsI%d", i+1, j), Form("SSD%d_CsI%d", i+1, j), 4000,0,4000,4000,0,4000);
    }
  }

  Long64_t nentries = fChainLayerTree->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries;ientry++) {

    fChainLayerTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {

      if (fLayerEvent.fSSDCsIMulti[ssdindex] >= 1) count_eff[ssdindex]++;

      // for multi = 2
      if (fLayerEvent.fSSDCsIMulti[ssdindex] == 2) {
        for (Int_t csimulti=0; csimulti<fLayerEvent.fCsIMulti; csimulti++) {
          if (fLayerEvent.fCsISSDNum[csimulti] == ssdindex) {
            csinum.push_back(fLayerEvent.fCsINum[csimulti]);
            csiech.push_back(fLayerEvent.fCsIECh[csimulti]);
          }
        }
        if (IsAdj_CsI(csinum[0],csinum[1])) {
          size_2[ssdindex]++;
          hist_CsI_ECorr[ssdindex]->Fill(csiech[0], csiech[1]);
          hist_CsI_Array[ssdindex][csinum[0]]->Fill(csiech[0], csiech[1]);
          // cout<<size_2[ssdindex]<<setw(20)<<csinum[0]<<setw(20)<<csinum[1]<<endl;
        }
      }
      // for multi = 3
      if (fLayerEvent.fSSDCsIMulti[ssdindex] == 3) {
        for (Int_t csimulti=0; csimulti<fLayerEvent.fCsIMulti; csimulti++) {
          if (fLayerEvent.fCsISSDNum[csimulti] == ssdindex) {
            csinum.push_back(fLayerEvent.fCsINum[csimulti]);
          }
        }
        if (IsAdj_CsI(csinum[0],csinum[1]) && IsAdj_CsI(csinum[0],csinum[2])) size_1213[ssdindex]++;
        if (IsAdj_CsI(csinum[0],csinum[1]) && IsAdj_CsI(csinum[1],csinum[2])) size_1223[ssdindex]++;
        if (IsAdj_CsI(csinum[0],csinum[2]) && IsAdj_CsI(csinum[1],csinum[2])) size_1323[ssdindex]++;
        if (IsAdj_CsI(csinum[0],csinum[1])  && !IsAdj_CsI(csinum[0],csinum[2]) && !IsAdj_CsI(csinum[1],csinum[2])) size_12[ssdindex]++;
        if (!IsAdj_CsI(csinum[0],csinum[1]) &&  IsAdj_CsI(csinum[0],csinum[2]) && !IsAdj_CsI(csinum[1],csinum[2])) size_13[ssdindex]++;
        if (!IsAdj_CsI(csinum[0],csinum[1]) && !IsAdj_CsI(csinum[0],csinum[2]) &&  IsAdj_CsI(csinum[1],csinum[2])) size_23[ssdindex]++;
      }
      csinum.clear();
      csiech.clear();
    }
  }

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    r_size_2[ssdindex]    = (Double_t) size_2[ssdindex]/count_eff[ssdindex];
    r_size_1213[ssdindex] = (Double_t) size_1213[ssdindex]/count_eff[ssdindex];
    r_size_1223[ssdindex] = (Double_t) size_1223[ssdindex]/count_eff[ssdindex];
    r_size_1323[ssdindex] = (Double_t) size_1323[ssdindex]/count_eff[ssdindex];
    r_size_12[ssdindex]   = (Double_t) size_12[ssdindex]/count_eff[ssdindex];
    r_size_13[ssdindex]   = (Double_t) size_13[ssdindex]/count_eff[ssdindex];
    r_size_23[ssdindex]   = (Double_t) size_23[ssdindex]/count_eff[ssdindex];

    ration_sum[ssdindex] = r_size_2[ssdindex]+r_size_1213[ssdindex]+r_size_1223[ssdindex]+
                           r_size_1323[ssdindex]+r_size_12[ssdindex]+r_size_13[ssdindex]+r_size_23[ssdindex];
  }

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    printf("SSD%d_CsI\n", ssdindex+1);
    cout<<"count_eff = "<<count_eff[ssdindex]<<endl;
    cout<<setprecision(4);
    cout<<setw(10)<<"size_2    = "<<size_2[ssdindex]<<setw(15)<<r_size_2[ssdindex]<<endl;
    cout<<setw(10)<<"size_1213 = "<<size_1213[ssdindex]<<setw(15)<<r_size_1213[ssdindex]<<endl;
    cout<<setw(10)<<"size_1223 = "<<size_1223[ssdindex]<<setw(15)<<r_size_1223[ssdindex]<<endl;
    cout<<setw(10)<<"size_1323 = "<<size_1323[ssdindex]<<setw(15)<<r_size_1323[ssdindex]<<endl;
    cout<<setw(10)<<"size_12   = "<<size_12[ssdindex]<<setw(15)<<r_size_12[ssdindex]<<endl;
    cout<<setw(10)<<"size_13   = "<<size_13[ssdindex]<<setw(15)<<r_size_13[ssdindex]<<endl;
    cout<<setw(10)<<"size_23   = "<<size_23[ssdindex]<<setw(15)<<r_size_23[ssdindex]<<endl;
    cout<<setw(10)<<"sum       = "<<ration_sum[ssdindex]<<endl;
    cout<<endl;
  }

  TCanvas* cans_CsI_ECorr = new TCanvas("cans_CsI_ECorr", "cans_CsI_ECorr", 1000, 1000);
  cans_CsI_ECorr->Divide(2,2);
  TCanvas* cans_CsI_Array[NUM_SSD];

  for (Int_t i=0; i<NUM_SSD; i++) {
    cans_CsI_ECorr->cd(i+1);
    hist_CsI_ECorr[i]->Draw("COLZ");

    cans_CsI_Array[i] = new TCanvas(Form("cans_SSD%d", i+1), Form("cans_SSD%d", i+1), 1200, 1200);
    cans_CsI_Array[i]->Divide(3,3);
    for (Int_t j=0; j<NUM_CSI; j++) {
      cans_CsI_Array[i]->cd(j+1);
      hist_CsI_Array[i][j]->Draw("COLZ");
    }
  }
}


//_________________________________________
//        检查 CsI 之间的串扰
// 选取 L2F, L2B 都只有一个粒子的事件, 然后对 CsI 中的情况进行分类:
// 如果点火的 CsI 数目大于1, 且至少有那块 CsI 相邻，则认为该事件
// “可能”存在串扰. 以此通过实验数据给出串扰的概率上限
//
void CSHINETrackReconstruction::CheckCrossTalk_CsI()
{
  gStyle->SetPalette(1);
//  gStyle->SetOptStat(0);

  TimeAndPercentage time;

  Int_t firstrun  = 120;
  Int_t lastrun   = 200;

  Double_t eneratio = 0.15;
  Int_t csiech_cut = 60;

  Int_t count_csimulti_ge1[4] = {0};
  Int_t count_csimulti_ge2[4] = {0};
  Int_t count_multi2_crosstalk[4] = {0};
  Int_t count_multi3_crosstalk[4] = {0};

  std::vector<Int_t> csinum;
  std::vector<Int_t> csiene;

  std::string pathFolder("/home/sea/Fission2019_Data/CSHINEEvent/");
  std::string pathROOT(Form("%sEventTree_Run%04d-Run%04d.root", pathFolder.c_str(),firstrun,lastrun));
  std::string pathPNGout[4];

  TH1D* hist_CsI[4];
  TH2D* h2_crosstalk[4];
  for (Int_t i=0; i<4; i++) {
    pathPNGout[i] = Form("%sfigure_TrackReconstruction/SSD%d_CsI_CrossTalk.png", PATHFIGURESFOLDER,i+1);
    hist_CsI[i] = new TH1D(Form("SSD%d_CsIMulti",i+1),Form("SSD%d_CsIMulti",i+1),10,0,10);
    h2_crosstalk[i] = new TH2D(Form("SSD%d_crosstalk",i+1),Form("SSD%d_crosstalk",i+1),4000,0,4000,4000,0,4000);
  }

  Long64_t nentries = fChainLayerTree->GetEntries();
  cout<<"Found "<<nentries<<" entries."<<endl;

  Int_t ientry = 0;
  for (Long64_t ientry=0; ientry<nentries; ientry++)
  {
    fChainLayerTree->GetEntry(ientry);
    time.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<4; ssdindex++) {
      //_______________________
      // for L2L3
      if (fLayerEvent.fSSDL1SMulti[ssdindex]>0 && fLayerEvent.fSSDL2BMulti[ssdindex]==1 &&
          fLayerEvent.fSSDL2FMulti[ssdindex]==1 && fLayerEvent.fSSDCsIMulti[ssdindex]>0)
      {
        hist_CsI[ssdindex]->Fill(fLayerEvent.fSSDCsIMulti[ssdindex]);

        if (fLayerEvent.fSSDCsIMulti[ssdindex]>=1) count_csimulti_ge1[ssdindex]++;
        if (fLayerEvent.fSSDCsIMulti[ssdindex]>=2) count_csimulti_ge2[ssdindex]++;

        // for multi = 2
        if (fLayerEvent.fSSDCsIMulti[ssdindex] == 2) {
          for (Int_t csimulti=0; csimulti<fLayerEvent.fCsIMulti; csimulti++) {
            if (fLayerEvent.fCsISSDNum[csimulti] == ssdindex) {
              csinum.push_back(fLayerEvent.fCsINum[csimulti]);
              csiene.push_back(fLayerEvent.fCsIECh[csimulti]);
            }
          }
          if (csinum.size()>0) {
            if (IsAdj_CsI(csinum[0],csinum[1])) {
              if (csiene[0]>csiech_cut && csiene[1]>csiech_cut) {
                count_multi2_crosstalk[ssdindex]++;

                // SSD4_CsI[4] 与 CsI[5] 信号异常, 需要扔掉
                if (ssdindex==3 && ((csinum[0]==4 || csinum[0]==5) || (csinum[1]==4 || csinum[1]==5))) continue;
                h2_crosstalk[ssdindex]->Fill(csiene[0], csiene[1]);
                //  cout<<"csi no. = "<<csinum[0]<<setw(15)<<"csi no. = "<<csinum[1]<<setw(15)
                //      <<"csi ene = "<<csiene[0]<<setw(15)<<"csi ene = "<<csiene[1]<<endl;
              }
            }
          }
        }

        // for multi = 3
        if (fLayerEvent.fSSDCsIMulti[ssdindex] == 3) {
          for (Int_t csimulti=0; csimulti<fLayerEvent.fCsIMulti; csimulti++) {
            if (fLayerEvent.fCsISSDNum[csimulti] == ssdindex) {
              csinum.push_back(fLayerEvent.fCsINum[csimulti]);
            }
          }
          if ( (IsAdj_CsI(csinum[0],csinum[1]) && IsAdj_CsI(csinum[0],csinum[2])) ||
               (IsAdj_CsI(csinum[0],csinum[1]) && IsAdj_CsI(csinum[1],csinum[2])) ||
               (IsAdj_CsI(csinum[0],csinum[2]) && IsAdj_CsI(csinum[1],csinum[2])) ||
               (IsAdj_CsI(csinum[0],csinum[1]) && IsAdj_CsI(csinum[0],csinum[2]) && IsAdj_CsI(csinum[1],csinum[2])) ||
               (IsAdj_CsI(csinum[0],csinum[1]) && IsAdj_CsI(csinum[0],csinum[2]) && IsAdj_CsI(csinum[1],csinum[2])) ||
               (IsAdj_CsI(csinum[0],csinum[1] )&& IsAdj_CsI(csinum[0],csinum[2]) && IsAdj_CsI(csinum[1],csinum[2])) )
          {
            count_multi3_crosstalk[ssdindex]++;
          }
        }
        csinum.clear();
        csiene.clear();
      }
    }
  }
  for (Int_t i=0; i<4; i++) {
    cout<<endl;
    cout<<"for SSD"<<i<<endl;
    cout<<"count_csimulti_ge1 = "<<count_csimulti_ge1[i]<<endl;
    cout<<"count_csimulti_ge2 = "<<count_csimulti_ge2[i]<<endl;
    cout<<"count_multi2_crosstalk = "<<count_multi2_crosstalk[i]<<endl;
    cout<<"count_multi3_crosstalk = "<<count_multi3_crosstalk[i]<<endl;

    Double_t ratio = (Double_t) count_csimulti_ge2[i]/count_csimulti_ge1[i];
    Double_t ratio_crosstalk = (Double_t) (count_multi2_crosstalk[i]+count_multi3_crosstalk[i])/count_csimulti_ge1[i];
    cout<<setprecision(6)<<"multi>=2 / multi>=1 = "<<ratio<<endl;
    cout<<setprecision(6)<<"ratio_crosstalk = "<<ratio_crosstalk<<endl;
  }


  TCanvas* cans[4];
  for (Int_t i=0; i<4; i++) {
    cans[i] = new TCanvas(Form("c_SSD%d",i+1), Form("c_SSD%d",i+1), 1200, 500);
    cans[i]->Divide(2,1);
    cans[i]->cd(1);
    hist_CsI[i]->Draw();
    cans[i]->cd(2);
    h2_crosstalk[i]->Draw("COL");
    cans[i]->Print(pathPNGout[i].c_str());
  }
}

//______________________________________________________________________________
// 判断两块 CsI 晶体是否相邻. 以下是 Fission2019 实验中 CsI 阵列的排布:
//  -----------
//   2   5   8
//   1   4   7
//   0   3   6
//  -----------
Bool_t CSHINETrackReconstruction::IsAdj_CsI(Int_t n1, Int_t n2)
{
  //（ 列相邻 || 行相邻 ）
  return (((n1/3 == n2/3)&&(n1-n2 == -1)) || ((n1%3 == n2%3)&&(n1-n2 == -3)));
}
//******************************************************************************


//******************************************************************************
//               径迹重建 与 相关检测
//            ----------------------
// 径迹重建算法
//______________________________________________________________________
// 自定义径迹查找算法1 : 循环-循环-判断-循环-判断-循环-判断
// 自定义径迹查找算法2 : 循环-循环-循环-循环-判断-判断-判断-判断
// 经验证, 算法2 与 算法1 等效 !
// 算法2 更加简洁明了，为方便起见，使用算法2进行后续分析
// 这一步生成 TrackEvent，便于后续的模式识别使用
//
// 为了不引起混乱, 本函数仅用于生成 Track 事件, 对于径迹重建的检查
// 将由下一个函数 CheckTrackReconstructionResult() 来完成
void CSHINETrackReconstruction::L2L3_TrackReconstructionAlgorithm()
{
  Int_t count_satisfiedmulticut[4] = {0}; // 满足初步 multi cut 计数
  Int_t count_effentries[4] = {0};        // globalmulti>0 的计数

  std::string LayerTag("L2L3");

  TimeAndPercentage timeper;

  std::string pathTrackEventRoot(Form("%s%s_TrackReconstructionEvent_Run%04d-Run%04d.root", PATHROOTFILESFOLDER,LayerTag.c_str(),fFirstRun,fLastRun));

  ofstream  FileOut[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    FileOut[i].open(Form("%sdata_Test_Multi/SSD%d_%s_Test_Multi_L1L2EnergyCut.dat",PATHDATAFOLDER,i+1,LayerTag.c_str()));

    FileOut[i]<<setw(7)<<"# EntryID"<<setw(15)<<"MultiGlobal"
              <<setw(11)<<"M_L3A"<<setw(11)<<"M_L2B"<<setw(11)<<"M_L2F"<<setw(11)<<"M_L1S"
              <<setw(11)<<"CSI"<<setw(11)<<"L2B"<<setw(11)<<"L2F"<<setw(11)<<"L1S"
              <<setw(13)<<"EL1S"<<setw(13)<<"EL2F"<<setw(13)<<"EL2B"<<setw(13)<<"ECsI"
              <<endl;
  }

  TFile* myfile = new TFile(pathTrackEventRoot.c_str(),"RECREATE");
  TTree* mytree = new TTree("TrackEvent","TrackEvent Tree");
  mytree->Branch("TrackEvent.", "CSHINETrackEvent", &fTrackEvent, 32000, 2);

  Long64_t nentries = fChainLayerTree->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;
  cout<<Form("Building TrackEvent for %s: Run%04d-Run%04d ......\n",LayerTag.c_str(),fFirstRun,fLastRun);

  for (Long64_t ientry=0; ientry<nentries; ientry++) {

    fChainLayerTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    Int_t globalmulti = 0 ;
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      Int_t ssdglobalmulti = 0;

      if ((fLayerEvent.fSSDCsIMulti[ssdindex]>0 && fLayerEvent.fSSDCsIMulti[ssdindex]<=MULTICUT_L3A) &&
          (fLayerEvent.fSSDL2BMulti[ssdindex]>0 && fLayerEvent.fSSDL2BMulti[ssdindex]<=MULTICUT_L2B) &&
          (fLayerEvent.fSSDL2FMulti[ssdindex]>0 && fLayerEvent.fSSDL2FMulti[ssdindex]<=MULTICUT_L2F) &&
          (fLayerEvent.fSSDL1SMulti[ssdindex]>0 && fLayerEvent.fSSDL1SMulti[ssdindex]<=MULTICUT_L1S)) // 给定初步的 multi cut
      {
        count_satisfiedmulticut[ssdindex]++;

        for (Int_t csimulti=0; csimulti<fLayerEvent.fCsIMulti; csimulti++) {
          for (Int_t l2bmulti=0; l2bmulti<fLayerEvent.fL2BMulti; l2bmulti++) {
            for (Int_t l2fmulti=0; l2fmulti<fLayerEvent.fL2FMulti; l2fmulti++) {
              for (Int_t l1smulti=0; l1smulti<fLayerEvent.fL1SMulti; l1smulti++) { // 逐层循环,遍历所有可能的组合

                if (fLayerEvent.fCsISSDNum[csimulti]==ssdindex && fLayerEvent.fL2BSSDNum[l2bmulti]==ssdindex &&
                    fLayerEvent.fL2FSSDNum[l2fmulti]==ssdindex && fLayerEvent.fL1SSSDNum[l1smulti]==ssdindex)  // 对每个SSD分开讨论
                {
                  if (IsGeoConstraint_L3A_L2B(fLayerEvent.fCsINum[csimulti], fLayerEvent.fL2BNumStrip[l2bmulti]) &&
                      IsGeoConstraint_L3A_L2F(fLayerEvent.fCsINum[csimulti], fLayerEvent.fL2FNumStrip[l2fmulti]) &&
                    //  IsEneConstraint_L2B_L2F(ssdindex, fLayerEvent.fL2BEMeV[l2bmulti], fLayerEvent.fL2FEMeV[l2fmulti]) &&
                      IsGeoConstraint_L2B_L1S(fLayerEvent.fL2BNumStrip[l2bmulti], fLayerEvent.fL1SNumStrip[l1smulti]))
                    //  IsEneConstraint_L1S_L2F(ssdindex, fLayerEvent.fL1SEMeV[l1smulti], fLayerEvent.fL2FEMeV[l2fmulti]) )
                  {
                    globalmulti++;
                    ssdglobalmulti++;

                    fTrackEvent.fGSSDNum.push_back(ssdindex);
                    fTrackEvent.fGCsINum.push_back(fLayerEvent.fCsINum[csimulti]);
                    fTrackEvent.fGCsIECh.push_back(fLayerEvent.fCsIECh[csimulti]);
                    fTrackEvent.fGL2BNumStrip.push_back(fLayerEvent.fL2BNumStrip[l2bmulti]);
                    fTrackEvent.fGL2BEMeV.push_back(fLayerEvent.fL2BEMeV[l2bmulti]);
                    fTrackEvent.fGL2FNumStrip.push_back(fLayerEvent.fL2FNumStrip[l2fmulti]);
                    fTrackEvent.fGL2FEMeV.push_back(fLayerEvent.fL2FEMeV[l2fmulti]);
                    fTrackEvent.fGL1SNumStrip.push_back(fLayerEvent.fL1SNumStrip[l1smulti]);
                    fTrackEvent.fGL1SEMeV.push_back(fLayerEvent.fL1SEMeV[l1smulti]);

                    fTrackEvent.fGL2FTime.push_back(fLayerEvent.fL2FTime[l2fmulti]);

                    FileOut[ssdindex]<<setw(7)<<ientry<<setw(10)<<ssdglobalmulti
                    <<setw(13)<<fLayerEvent.fSSDCsIMulti[ssdindex]
                    <<setw(11)<<fLayerEvent.fSSDL2BMulti[ssdindex]
                    <<setw(11)<<fLayerEvent.fSSDL2FMulti[ssdindex]
                    <<setw(11)<<fLayerEvent.fSSDL1SMulti[ssdindex]
                    <<setw(11)<<fLayerEvent.fCsINum[csimulti]
                    <<setw(11)<<fLayerEvent.fL2BNumStrip[l2bmulti]
                    <<setw(11)<<fLayerEvent.fL2FNumStrip[l2fmulti]
                    <<setw(11)<<fLayerEvent.fL1SNumStrip[l1smulti]
                    <<setw(14)<<fLayerEvent.fL1SEMeV[l1smulti]
                    <<setw(14)<<fLayerEvent.fL2FEMeV[l2fmulti]
                    <<setw(14)<<fLayerEvent.fL2BEMeV[l2bmulti]
                    <<setw(14)<<fLayerEvent.fCsIECh[csimulti]
                    <<endl;
                  }
                }
              }
            }
          }
        }
      }
      fTrackEvent.fSSDGlobalMulti.push_back(ssdglobalmulti);
      if (ssdglobalmulti>0) count_effentries[ssdindex]++;
    }
    fTrackEvent.fGlobalMulti = globalmulti;
    mytree->Fill();
    fTrackEvent.Swap();
  }
  mytree->Write();
  myfile->Close();

  cout<<endl;
  for (Int_t i=0; i<NUM_SSD; i++) {
    cout<<Form("count_satisfiedmulticut[%d] = ",i+1)<<count_satisfiedmulticut[i]<<setw(30)
        <<Form("count_effentries[%d] = ",i+1)<<count_effentries[i]<<setw(20)
        <<std::setprecision(4)<<(Double_t) count_effentries[i]/count_satisfiedmulticut[i]
        <<endl;
  }
}


//******************************************************************************
// 确定 L1-L2 的 globalmulti event
void CSHINETrackReconstruction::L1L2_TrackReconstructionAlgorithm()
{
  Int_t count_satisfiedmulticut[4] = {0}; // 满足初步 multi cut 计数
  Int_t count_effentries[4] = {0};        // globalmulti>0 的计数

  Int_t CsINum = -1;
  Int_t ECh_CsI = 0;

  std::string LayerTag("L1L2");

  TimeAndPercentage timeper;

  std::string pathTrackEventRoot(Form("%s%s_TrackReconstructionEvent_Run%04d-Run%04d.root", PATHROOTFILESFOLDER,LayerTag.c_str(),fFirstRun,fLastRun));

  ofstream  FileOut[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    FileOut[i].open(Form("%sdata_Test_Multi/SSD%d_%s_Test_Multi_L1L2EnergyCut.dat",PATHDATAFOLDER,i+1,LayerTag.c_str()));

    FileOut[i]<<setw(7)<<"# EntryID"<<setw(15)<<"MultiGlobal"
              <<setw(11)<<"M_L3A"<<setw(11)<<"M_L2B"<<setw(11)<<"M_L2F"<<setw(11)<<"M_L1S"
              <<setw(11)<<"CSI"<<setw(11)<<"L2B"<<setw(11)<<"L2F"<<setw(11)<<"L1S"
              <<setw(13)<<"EL1S"<<setw(13)<<"EL2F"<<setw(13)<<"EL2B"<<setw(13)<<"ECsI"
              <<endl;
  }

  TFile* myfile = new TFile(pathTrackEventRoot.c_str(),"RECREATE");
  TTree* mytree = new TTree("TrackEvent","TrackEvent Tree");
  mytree->Branch("TrackEvent.", "CSHINETrackEvent", &fTrackEvent, 32000, 2);

  Long64_t nentries = fChainLayerTree->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;
  cout<<Form("Building TrackEvent for %s: Run%04d-Run%04d ......\n",LayerTag.c_str(),fFirstRun,fLastRun);

  for (Long64_t ientry=0; ientry<nentries; ientry++) {

    fChainLayerTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    Int_t globalmulti = 0 ;
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      Int_t ssdglobalmulti = 0;

      if ((fLayerEvent.fSSDCsIMulti[ssdindex] == 0) &&
          (fLayerEvent.fSSDL2BMulti[ssdindex]>0 && fLayerEvent.fSSDL2BMulti[ssdindex]<=MULTICUT_L2B) &&
          (fLayerEvent.fSSDL2FMulti[ssdindex]>0 && fLayerEvent.fSSDL2FMulti[ssdindex]<=MULTICUT_L2F) &&
          (fLayerEvent.fSSDL1SMulti[ssdindex]>0 && fLayerEvent.fSSDL1SMulti[ssdindex]<=MULTICUT_L1S)) // 给定初步的 multi cut
      {
        count_satisfiedmulticut[ssdindex]++;

        for (Int_t l2bmulti=0; l2bmulti<fLayerEvent.fL2BMulti; l2bmulti++) {
          for (Int_t l2fmulti=0; l2fmulti<fLayerEvent.fL2FMulti; l2fmulti++) {
            for (Int_t l1smulti=0; l1smulti<fLayerEvent.fL1SMulti; l1smulti++) { // 逐层循环,遍历所有可能的组合

              if (fLayerEvent.fL2BSSDNum[l2bmulti]==ssdindex &&
                  fLayerEvent.fL2FSSDNum[l2fmulti]==ssdindex && fLayerEvent.fL1SSSDNum[l1smulti]==ssdindex)  // 对每个SSD分开讨论
              {
                if (IsGeoConstraint_L2B_L1S(fLayerEvent.fL2BNumStrip[l2bmulti], fLayerEvent.fL1SNumStrip[l1smulti]))
                    //  IsEneConstraint_L2B_L2F(ssdindex, fLayerEvent.fL2BEMeV[l2bmulti], fLayerEvent.fL2FEMeV[l2fmulti]) &&
                    //  IsEneConstraint_L1S_L2F(ssdindex, fLayerEvent.fL1SEMeV[l1smulti], fLayerEvent.fL2FEMeV[l2fmulti]) )
                {
                  globalmulti++;
                  ssdglobalmulti++;

                  fTrackEvent.fGSSDNum.push_back(ssdindex);
                  fTrackEvent.fGCsINum.push_back(CsINum);
                  fTrackEvent.fGCsIECh.push_back(ECh_CsI);
                  fTrackEvent.fGL2BNumStrip.push_back(fLayerEvent.fL2BNumStrip[l2bmulti]);
                  fTrackEvent.fGL2BEMeV.push_back(fLayerEvent.fL2BEMeV[l2bmulti]);
                  fTrackEvent.fGL2FNumStrip.push_back(fLayerEvent.fL2FNumStrip[l2fmulti]);
                  fTrackEvent.fGL2FEMeV.push_back(fLayerEvent.fL2FEMeV[l2fmulti]);
                  fTrackEvent.fGL1SNumStrip.push_back(fLayerEvent.fL1SNumStrip[l1smulti]);
                  fTrackEvent.fGL1SEMeV.push_back(fLayerEvent.fL1SEMeV[l1smulti]);

                  fTrackEvent.fGL2FTime.push_back(fLayerEvent.fL2FTime[l2fmulti]);

                  FileOut[ssdindex]<<setw(7)<<ientry<<setw(10)<<ssdglobalmulti
                  <<setw(13)<<fLayerEvent.fSSDCsIMulti[ssdindex]
                  <<setw(11)<<fLayerEvent.fSSDL2BMulti[ssdindex]
                  <<setw(11)<<fLayerEvent.fSSDL2FMulti[ssdindex]
                  <<setw(11)<<fLayerEvent.fSSDL1SMulti[ssdindex]
                  <<setw(11)<<CsINum
                  <<setw(11)<<fLayerEvent.fL2BNumStrip[l2bmulti]
                  <<setw(11)<<fLayerEvent.fL2FNumStrip[l2fmulti]
                  <<setw(11)<<fLayerEvent.fL1SNumStrip[l1smulti]
                  <<setw(14)<<fLayerEvent.fL1SEMeV[l1smulti]
                  <<setw(14)<<fLayerEvent.fL2FEMeV[l2fmulti]
                  <<setw(14)<<fLayerEvent.fL2BEMeV[l2bmulti]
                  <<setw(14)<<ECh_CsI
                  <<endl;
                }
              }
            }
          }
        }
      }
      fTrackEvent.fSSDGlobalMulti.push_back(ssdglobalmulti);
      if (ssdglobalmulti>0) count_effentries[ssdindex]++;
    }
    fTrackEvent.fGlobalMulti = globalmulti;
    mytree->Fill();
    fTrackEvent.Swap();
  }
  mytree->Write();
  myfile->Close();

  cout<<endl;
  for (Int_t i=0; i<NUM_SSD; i++) {
    cout<<Form("count_satisfiedmulticut[%d] = ",i+1)<<count_satisfiedmulticut[i]<<setw(30)
        <<Form("count_effentries[%d] = ",i+1)<<count_effentries[i]<<setw(20)
        <<std::setprecision(4)<<(Double_t) count_effentries[i]/count_satisfiedmulticut[i]
        <<endl;
  }
}
//******************************************************************************

//______________________________________________________________________________
// 分析不同 fGlobalMulti 所占的比例
void CSHINETrackReconstruction::CheckGlobalMultiRatio(const char* pidtag, Int_t multicut)
{
  // 定义变量， 以用于计算不同 fGlobalMulti 的比例
  Int_t    globalmulti[NUM_SSD][multicut+1];
  Double_t multiratio_from0[NUM_SSD][multicut+1];
  Double_t multiratio_from1[NUM_SSD][multicut+1];
  Double_t ratiosum_from0[NUM_SSD];
  Double_t ratiosum_from1[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    ratiosum_from0[ssdindex] = 0;
    ratiosum_from1[ssdindex] = 0;
    for (Int_t multi=0; multi<=multicut; multi++) {
      globalmulti[ssdindex][multi] = 0;
      multiratio_from0[ssdindex][multi] = 0;
      multiratio_from1[ssdindex][multi] = 0;
    }
  }

  std::string pathDataGlobalMultiRatio(Form("%sdata_Test_Multi/%s_GlobalMultiRatio.dat",PATHDATAFOLDER,pidtag));
  std::string pathTrackEventRoot = Form("%s%s_TrackReconstructionEvent_Run%04d-Run%04d.root",PATHROOTFILESFOLDER,pidtag,fFirstRun,fLastRun);
  std::string pathMultiRatioOut(Form("%sfigure_TrackReconstruction/%s_MultiRatio_Run%04d-Run%04d.png",PATHFIGURESFOLDER,pidtag,fFirstRun,fLastRun));

  ofstream FileOut(pathDataGlobalMultiRatio.c_str());

  TFile* myfile = new TFile(pathTrackEventRoot.c_str(), "READONLY");
  if (!myfile || !myfile->IsOpen()) {
    cout<<Form("File %s not founded.",  myfile->GetName());
    return;
  }
  TTree* mytree = (TTree*)myfile->Get("TrackEvent");
  Long64_t nentries = mytree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  std::vector<Int_t>      TrackEvent_fSSDGlobalMulti;
  mytree->SetMakeClass(1);  // 如果 tree 的 branch 使用了自定义的类, 则这条语句不能省略！！！
  mytree->SetBranchAddress("TrackEvent.fSSDGlobalMulti", &TrackEvent_fSSDGlobalMulti);

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    mytree->GetEntry(ientry);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      for (Int_t i=0; i<=multicut; i++) {
        if (TrackEvent_fSSDGlobalMulti[ssdindex]==i)   globalmulti[ssdindex][i]++;
      }
    }
  }

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    FileOut<<"------------------"<<endl;
    FileOut<<Form("For SSD%d :\n",ssdindex+1)<<endl;

    // 计算比例，从 globalmulti = 0 算起
    for (Int_t i=0; i<=multicut; i++) {
      multiratio_from0[ssdindex][i] = (Double_t) globalmulti[ssdindex][i]/nentries;
      ratiosum_from0[ssdindex] += multiratio_from0[ssdindex][i];
      FileOut<<Form("multi%d = ", i)<<multiratio_from0[ssdindex][i]<<setw(12);
    }
    FileOut<<ratiosum_from0[ssdindex]<<endl;

    // 计算比例，从 globalmulti = 1 算起
    for (Int_t i=1; i<=multicut; i++) {
      multiratio_from1[ssdindex][i] = (Double_t) globalmulti[ssdindex][i]/(nentries-globalmulti[ssdindex][0]); // 剔除 fGlobalMulti=0 的情况
      ratiosum_from1[ssdindex] += multiratio_from1[ssdindex][i];
      FileOut<<Form("multi%d = ", i)<<multiratio_from1[ssdindex][i]<<setw(12);
    }
    FileOut<<ratiosum_from1[ssdindex]<<endl;
  }
  myfile->Close();
  FileOut.close();

  // 画图
  TH1D* h_multi[NUM_SSD];
  TCanvas* cans = new TCanvas("cans","cans",1000,1000);
  cans->Divide(2,2);
  for (Int_t issd=0; issd<NUM_SSD; issd++) {
    h_multi[issd] = new TH1D(Form("%s_SSD%d_multi",pidtag,issd+1),Form("%s_SSD%d_multi",pidtag,issd+1),multicut+1,-0.5,multicut+0.5);
    for (Int_t ibin=1; ibin<=(multicut+1); ibin++) {
      h_multi[issd]->SetBinContent(ibin, multiratio_from0[issd][ibin-1]);
    }
    cans->cd(issd+1);
    gPad->SetLogy();
    h_multi[issd]->Draw();
    h_multi[issd]->GetXaxis()->SetNdivisions(100+multicut+1);
  }
  cans->Print(pathMultiRatioOut.c_str());
}


//______________________________________________________________________________
// 提取不同 fGlobalMulti 的数据，存成单独的文件，以便于对不同的模式进行分类
void CSHINETrackReconstruction::GlobalMulti_ExtractData(const char* pidtag, Int_t globalmulti)
{
  Int_t gmulti_index[NUM_SSD];
  ifstream FileIn[NUM_SSD];
  ofstream FileOut[NUM_SSD];

  std::string pathFileOut[NUM_SSD];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    pathFileOut[ssdindex] = Form("%sdata_Test_Multi/%s/globalmulti%d/SSD%d_%s_Test_Multi_fGlobalMulti%d.dat",PATHDATAFOLDER,pidtag,globalmulti,ssdindex+1,pidtag,globalmulti);
    FileOut[ssdindex].open(pathFileOut[ssdindex].c_str());
    FileOut[ssdindex]<<setw(10)<<"# EntryNo."<<setw(10)<<" MultiGlobal"
                     <<setw(8)<<"CSI"<<setw(10)<<"L2B"<<setw(10)<<"L2F"<<setw(10)<<"L1S"
                     <<setw(10)<<"EL1S"<<setw(10)<<"EL2F"<<setw(10)<<"EL2B"<<setw(10)<<"ECsI"
                     <<endl;
  }

  fChainTrackTree->SetEntries(10000000);
  cout<<Form("Processing globalmulti = %d ......",globalmulti)<<endl;
  Long64_t nentries = fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry,nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      if (fTrackEvent.fSSDGlobalMulti[ssdindex]==globalmulti) {
        gmulti_index[ssdindex] = 0;
        for (Int_t gmulti=0; gmulti<fTrackEvent.fGlobalMulti; gmulti++) {
          if (fTrackEvent.fGSSDNum[gmulti]==ssdindex) {
            gmulti_index[ssdindex]++;
            FileOut[ssdindex]<<setw(7)<<ientry
              <<setw(10)<<gmulti_index[ssdindex]
              <<setw(12)<<fTrackEvent.fGCsINum[gmulti]
              <<setw(10)<<fTrackEvent.fGL2BNumStrip[gmulti]
              <<setw(10)<<fTrackEvent.fGL2FNumStrip[gmulti]
              <<setw(10)<<fTrackEvent.fGL1SNumStrip[gmulti]
              <<setw(13)<<fTrackEvent.fGL1SEMeV[gmulti]
              <<setw(10)<<fTrackEvent.fGL2FEMeV[gmulti]
              <<setw(10)<<fTrackEvent.fGL2BEMeV[gmulti]
              <<setw(10)<<fTrackEvent.fGCsIECh[gmulti]
              <<endl;
          }
        }
      }
    }
  }
}


//______________________________________________________________________________
void CSHINETrackReconstruction::L2L3_CheckParallelDraw(const char* gmultitag, Int_t gmulti)
{
  gROOT->cd();

  std::string pathDataFileIn[NUM_SSD];
  std::string pidtag("L2L3");

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/%s_%s_ParallelDraw.pdf",PATHFIGURESFOLDER,pidtag.c_str(),gmultitag));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/%s_%s_ParallelDraw.pdf[",PATHFIGURESFOLDER,pidtag.c_str(),gmultitag));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/%s_%s_ParallelDraw.pdf]",PATHFIGURESFOLDER,pidtag.c_str(),gmultitag));

  std::string pathTrackRootFile(Form("%s%s_TrackReconstructionEvent_Run%04d-Run%04d.root",PATHROOTFILESFOLDER,pidtag.c_str(),fFirstRun,fLastRun));
  TFile* myfile = TFile::Open(pathTrackRootFile.c_str());
  TTree* mytree = (TTree*) myfile->Get("TrackEvent");
  mytree->SetEntries(10000);

  TCanvas* c_begin = new TCanvas("c_begin","c_begin",600,600);
  TCanvas* c_end = new TCanvas("c_end","c_end",600,600);
  TCanvas* cans[NUM_SSD][4];

  c_begin->Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    cans[ssdindex][0] = new TCanvas(Form("SSD%d_Cans%d",ssdindex+1,1),Form("SSD%d_Cans%d",ssdindex+1,1),800,600);
    cans[ssdindex][0]->cd();
    mytree->Draw("fGL1SNumStrip:fGL2FNumStrip:fGL2BNumStrip:fGCsINum",Form("fGSSDNum==%d && fSSDGlobalMulti[%d]==%d",ssdindex,ssdindex,gmulti),"PARA");

    cans[ssdindex][1] = new TCanvas(Form("SSD%d_Cans%d",ssdindex+1,2),Form("SSD%d_Cans%d",ssdindex+1,2),800,600);
    cans[ssdindex][1]->cd();
    mytree->Draw("fGL1SNumStrip:fGL2BNumStrip:fGCsINum",Form("fGSSDNum==%d && fSSDGlobalMulti[%d]==%d",ssdindex,ssdindex,gmulti),"PARA");

    cans[ssdindex][2] = new TCanvas(Form("SSD%d_Cans%d",ssdindex+1,3),Form("SSD%d_Cans%d",ssdindex+1,3),800,600);
    cans[ssdindex][2]->cd();
    mytree->Draw("fGL1SNumStrip:fGL2FNumStrip:fGCsINum",Form("fGSSDNum==%d && fSSDGlobalMulti[%d]==%d",ssdindex,ssdindex,gmulti),"PARA");

    cans[ssdindex][3] = new TCanvas(Form("SSD%d_Cans%d",ssdindex+1,4),Form("SSD%d_Cans%d",ssdindex+1,4),800,600);
    cans[ssdindex][3]->cd();
    mytree->Draw("fGL1SEMeV:fGL2FEMeV:fGL2BEMeV:fGCsIECh",Form("fGSSDNum==%d && fSSDGlobalMulti[%d]==%d",ssdindex,ssdindex,gmulti),"PARA");

    cans[ssdindex][0]->Print(pathPDFOut.c_str());
    cans[ssdindex][1]->Print(pathPDFOut.c_str());
    cans[ssdindex][2]->Print(pathPDFOut.c_str());
    cans[ssdindex][3]->Print(pathPDFOut.c_str());
  }
  c_end->Print(pathPDFOutEnd.c_str());
}

//______________________________________________________________________________
void CSHINETrackReconstruction::L2L3_CheckL2L3DEEPlot(const char* gmultitag, Int_t gmulti)
{
  gROOT->cd();
//  gStyle->SetOptStat(kFALSE);

  Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2[4]  = {3000, 2500, 1500, 800};

  std::string pidtag("L2L3");
  std::string pathFileIn[NUM_SSD];
  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/%s_EL2FvsECsI_%s.pdf",PATHFIGURESFOLDER,pidtag.c_str(),gmultitag));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/%s_EL2FvsECsI_%s.pdf[",PATHFIGURESFOLDER,pidtag.c_str(),gmultitag));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/%s_EL2FvsECsI_%s.pdf]",PATHFIGURESFOLDER,pidtag.c_str(),gmultitag));

  std::string pathTrackRootFile(Form("%s%s_TrackReconstructionEvent_Run%04d-Run%04d.root",PATHROOTFILESFOLDER,pidtag.c_str(),fFirstRun,fLastRun));
  TFile* myfile = TFile::Open(pathTrackRootFile.c_str());
  TTree* mytree = (TTree*) myfile->Get("TrackEvent");

  TH2D* h2d_DEE[NUM_SSD][NUM_CSI];
  TCanvas* cans = new TCanvas("cans_L2L3","",800,800);
  TCanvas* c_begin = new TCanvas("c_begin","c_begin",600,600);
  TCanvas* c_end = new TCanvas("c_end","c_end",600,600);

  c_begin->Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h2d_DEE[ssdindex][csiindex] = new TH2D(Form("SSD%d_CsI%d_EL2FvsECsI",ssdindex+1,csiindex),Form("SSD%d_CsI%d_EL2FvsECsI",
                                             ssdindex+1,csiindex),4096,0,4096,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

      mytree->Draw(Form("fGL2FEMeV:fGCsIECh>>SSD%d_CsI%d_EL2FvsECsI",ssdindex+1,csiindex),Form("fGSSDNum==%d && fSSDGlobalMulti[%d]==%d && fGCsINum==%d",ssdindex,ssdindex,gmulti,csiindex));

      cans->cd();
      gPad->SetBottomMargin(0.12);
      gPad->SetLeftMargin(0.12);
      h2d_DEE[ssdindex][csiindex]->SetTitle(Form("SSD%d_CsI%d_EL2FvsECsI",ssdindex+1,csiindex));
      h2d_DEE[ssdindex][csiindex]->SetTitleSize(0.05);
      h2d_DEE[ssdindex][csiindex]->GetXaxis()->SetTitle("ECsI (Ch)");
      h2d_DEE[ssdindex][csiindex]->GetXaxis()->SetTitleSize(0.05);
      h2d_DEE[ssdindex][csiindex]->GetXaxis()->CenterTitle(kTRUE);
      h2d_DEE[ssdindex][csiindex]->GetXaxis()->SetLabelSize(0.05);
      h2d_DEE[ssdindex][csiindex]->GetXaxis()->SetNdivisions(505);
      h2d_DEE[ssdindex][csiindex]->GetYaxis()->SetTitle("EL2F (MeV)");
      h2d_DEE[ssdindex][csiindex]->GetYaxis()->SetTitleSize(0.05);
      h2d_DEE[ssdindex][csiindex]->GetYaxis()->SetTitleOffset(1.2);
      h2d_DEE[ssdindex][csiindex]->GetYaxis()->CenterTitle(kTRUE);
      h2d_DEE[ssdindex][csiindex]->GetYaxis()->SetLabelSize(0.05);
      h2d_DEE[ssdindex][csiindex]->GetYaxis()->SetNdivisions(510);
      h2d_DEE[ssdindex][csiindex]->Draw("COL");

      gPad->Modified();
      gPad->Update();

      cans->Print(pathPDFOut.c_str());
    }
  }
  c_end->Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void CSHINETrackReconstruction::L2L3_CheckL1L2DEEPlot(const char* gmultitag, Int_t gmulti)
{
  gROOT->cd();

  std::string pidtag("L2L3");
  std::string pathDataInput[NUM_SSD];
  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/%s_EL1vsEL2F_%s.pdf",PATHFIGURESFOLDER,pidtag.c_str(),gmultitag));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/%s_EL1vsEL2F_%s.pdf[",PATHFIGURESFOLDER,pidtag.c_str(),gmultitag));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/%s_EL1vsEL2F_%s.pdf]",PATHFIGURESFOLDER,pidtag.c_str(),gmultitag));

  std::string pathTrackRootFile(Form("%s%s_TrackReconstructionEvent_Run%04d-Run%04d.root",PATHROOTFILESFOLDER,pidtag.c_str(),fFirstRun,fLastRun));
  TFile* myfile = TFile::Open(pathTrackRootFile.c_str());
  TTree* mytree = (TTree*) myfile->Get("TrackEvent");

  TCanvas* cans_L1L2 = new TCanvas("cans_L1L2","cans_L1L2",800,600);
  TCanvas* c_begin = new TCanvas("c_begin","c_begin",600,600);
  TCanvas* c_end = new TCanvas("c_end","c_end",600,600);
  c_begin->Print(pathPDFOutBegin.c_str());

  TH2D* hist2[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    pathDataInput[ssdindex] = Form("%sdata_Test_Multi/globalmulti%d/SSD%d_%s_Test_Multi_%s.dat",PATHDATAFOLDER,gmulti,ssdindex+1,pidtag.c_str(),gmultitag);
    hist2[ssdindex] = new TH2D(Form("SSD%d_EL1SvsEL2F",ssdindex+1),Form("SSD%d_EL1SvsEL2F",ssdindex+1),
                               NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);

    mytree->Draw(Form("fGL1SEMeV:fGL2FEMeV>>SSD%d_EL1SvsEL2F",ssdindex+1),Form("fGSSDNum==%d && fSSDGlobalMulti[%d]==%d",ssdindex,ssdindex,gmulti));

    cans_L1L2->cd();
    gPad->SetBottomMargin(0.12);
    gPad->SetLeftMargin(0.12);
    hist2[ssdindex]->Draw("COL");
    hist2[ssdindex]->SetTitle(Form("SSD%d_EL1SvsEL2F_%s",ssdindex+1,gmultitag));
    hist2[ssdindex]->SetTitleSize(0.05);
    hist2[ssdindex]->GetXaxis()->SetTitle("EL2F (MeV)");
    hist2[ssdindex]->GetXaxis()->SetTitleSize(0.05);
    hist2[ssdindex]->GetYaxis()->SetTitleOffset(1.1);
    hist2[ssdindex]->GetXaxis()->CenterTitle(kTRUE);
    hist2[ssdindex]->GetXaxis()->SetLabelSize(0.05);
    hist2[ssdindex]->GetXaxis()->SetNdivisions(505);
    hist2[ssdindex]->GetYaxis()->SetTitle("EL1 (MeV)");
    hist2[ssdindex]->GetYaxis()->SetTitleSize(0.05);
    hist2[ssdindex]->GetYaxis()->CenterTitle(kTRUE);
    hist2[ssdindex]->GetYaxis()->SetLabelSize(0.05);
    hist2[ssdindex]->GetYaxis()->SetNdivisions(505);

    gPad->Modified();
    gPad->Update();

    cans_L1L2->Print(pathPDFOut.c_str());
  }
  c_end->Print(pathPDFOutEnd.c_str());
}

//******************************************************************************


//******************************************************************************
// L2L3 径迹重建模式定义： 根据 globalmulti 定义模式
void CSHINETrackReconstruction::L2L3_GenerateModeFromGMulti(Int_t globalmulti, vector<string>& mode)
{
  if (globalmulti<1) { cout<<"Error: globalmulti<1"<<endl; return; }
  for (Int_t ml3a=0; ml3a<globalmulti; ml3a++) {
    for (Int_t ml2b=0; ml2b<globalmulti; ml2b++) {
      for (Int_t ml2f=0; ml2f<globalmulti; ml2f++) {
        for (Int_t ml1s=0; ml1s<globalmulti; ml1s++) {
          mode.push_back(Form("%d%d%d%d",ml3a,ml2b,ml2f,ml1s));
        }
      }
    }
  }
}

//____________________________
// 根据实验数据进行模式匹配
string CSHINETrackReconstruction::L2L3_CalcModeFromExpData(Int_t globalmultti, vector<Int_t> mcsi, vector<Int_t> ml2b,
  vector<Int_t> ml2f, vector<Int_t> ml1s)
{
  if (globalmultti<2) { cout<<"globalmulti<2, no need for mode classification!"<<endl; return "error"; }
  if (globalmultti != mcsi.size()) { cout<<"Error: globalmutli != ecsi.size()"<<endl; return "error"; }

  string mode_exp;
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

  mode_exp = Form("%d%d%d%d",candimulti_csi,candimulti_l2b,candimulti_l2f,candimulti_l1s);
  return mode_exp;
}

//___________________________________
// 画出不同 globalmulti 下的模式分布
void CSHINETrackReconstruction::L2L3_Mode_DrawClassification(Int_t globalmulti)
{
  Int_t nummode = pow(globalmulti, 4);
  Int_t gssdnum = -1;

  string pidtag("L2L3");

  string mode_exp;
  vector<string> mode;
  L2L3_GenerateModeFromGMulti(globalmulti, mode);

  std::string pathModeDistribution(Form("%sfigure_TrackReconstruction/%s_GlobalMulti%d_ModeStatistic.png", PATHFIGURESFOLDER, pidtag.c_str(), globalmulti));

  TH1I* h_modecount[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h_modecount[ssdindex] = new TH1I(Form("SSD%d_gmulti%d_modes",ssdindex+1,globalmulti),Form("SSD%d_gmulti%d_modes",ssdindex+1,globalmulti),nummode,-0.5,nummode-0.5);
  }

  // fChainTrackTree->SetEntries(100000);
  Long64_t nentries = fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing DrawClassification globalmulti = %d ......", globalmulti)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    // fill data
    if (fTrackEvent.fGlobalMulti > 0) {
      for (Int_t gmulti=0; gmulti<fTrackEvent.fGlobalMulti; gmulti++) {
        gssdnum = fTrackEvent.fGSSDNum[gmulti];
        L2L3_CsINum[gssdnum].push_back(fTrackEvent.fGCsINum[gmulti]);
        L2L3_L2BStripNum[gssdnum].push_back(fTrackEvent.fGL2BNumStrip[gmulti]);
        L2L3_L2FStripNum[gssdnum].push_back(fTrackEvent.fGL2FNumStrip[gmulti]);
        L2L3_L1SStripNum[gssdnum].push_back(fTrackEvent.fGL1SNumStrip[gmulti]);
      }
    }
    // save data
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      if (fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        mode_exp = L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
        for (Int_t imode=0; imode<nummode; imode++) {
          if (strcmp(mode_exp.c_str(), mode[imode].c_str()) == 0) {
            h_modecount[ssdindex]->Fill(imode);
          }
        }
      }
      // clear the vectors
      L2L3_CsINum[ssdindex].clear();
      L2L3_L2BStripNum[ssdindex].clear();
      L2L3_L2FStripNum[ssdindex].clear();
      L2L3_L1SStripNum[ssdindex].clear();
    }
  }
  // 画图
  TCanvas* cans = new TCanvas("cans", "cans", 1200, 1200);
  cans->Divide(2,2);

  cans->cd(1);
  h_modecount[0]->Draw();
//  h_modecount[0]->GetXaxis()->SetNdivisions(100+nummode);
  h_modecount[0]->GetYaxis()->SetRangeUser(0, 1.1*h_modecount[0]->GetMaximum());

  cans->cd(2);
  h_modecount[1]->Draw();
//  h_modecount[1]->GetXaxis()->SetNdivisions(100+nummode);
  h_modecount[1]->GetYaxis()->SetRangeUser(0, 1.1*h_modecount[1]->GetMaximum());

  cans->cd(3);
  h_modecount[2]->Draw();
//  h_modecount[2]->GetXaxis()->SetNdivisions(100+nummode);
  h_modecount[2]->GetYaxis()->SetRangeUser(0, 1.1*h_modecount[2]->GetMaximum());

  cans->cd(4);
  h_modecount[3]->Draw();
//  h_modecount[3]->GetXaxis()->SetNdivisions(100+nummode);
  h_modecount[3]->GetYaxis()->SetRangeUser(0, 1.1*h_modecount[3]->GetMaximum());

  cans->Print(pathModeDistribution.c_str());
}

//_________________________________________
// 计算不同模式的占比
void CSHINETrackReconstruction::L2L3_Mode_CalcRatio(Int_t globalmulti)
{
  Int_t nummode = pow(globalmulti, 4);
  Int_t gssdnum = -1;

  Double_t ratio_sum[NUM_SSD];
  Double_t ratio_mode[NUM_SSD][nummode];

  Int_t entry_globalmulti[NUM_SSD];
  Int_t entry_mode[NUM_SSD][nummode];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    ratio_sum[ssdindex] = 0.;
    entry_globalmulti[ssdindex] = 0;
    for (Int_t imode=0; imode<nummode; imode++) {
      entry_mode[ssdindex][imode] = 0;
    }
  }

  string mode_exp;
  vector<string> mode;
  L2L3_GenerateModeFromGMulti(globalmulti, mode);

  std::string pidtag("L2L3");
  std::string pathModeRatioOut(Form("%sdata_Test_Multi/%s_Test_Multi_fGlobalMulti%d_ModeRatio.dat",PATHDATAFOLDER,pidtag.c_str(),globalmulti));
  ofstream FileOut(pathModeRatioOut.c_str());
  FileOut<<"* Ratio of different modes of events with globalmulti=2"<<endl;

//  fChainTrackTree->SetEntries(10000);
  Long64_t nentries = fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing CaliRatio globalmulti = %d ......", globalmulti)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    // fill data
    if (fTrackEvent.fGlobalMulti > 0) {
      for (Int_t gmulti=0; gmulti<fTrackEvent.fGlobalMulti; gmulti++) {
        gssdnum = fTrackEvent.fGSSDNum[gmulti];
        L2L3_CsINum[gssdnum].push_back(fTrackEvent.fGCsINum[gmulti]);
        L2L3_L2BStripNum[gssdnum].push_back(fTrackEvent.fGL2BNumStrip[gmulti]);
        L2L3_L2FStripNum[gssdnum].push_back(fTrackEvent.fGL2FNumStrip[gmulti]);
        L2L3_L1SStripNum[gssdnum].push_back(fTrackEvent.fGL1SNumStrip[gmulti]);
      }
    }
    // save data
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      if (fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        entry_globalmulti[ssdindex]++;
        mode_exp = L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
        for (Int_t imode=0; imode<nummode; imode++) {
          if (strcmp(mode_exp.c_str(), mode[imode].c_str()) == 0) {
            entry_mode[ssdindex][imode]++;
          }
        }
      }
      // clear the vectors
      L2L3_CsINum[ssdindex].clear();
      L2L3_L2BStripNum[ssdindex].clear();
      L2L3_L2FStripNum[ssdindex].clear();
      L2L3_L1SStripNum[ssdindex].clear();
    }
  }
  // 计算模式比例
  for (Int_t ssdindex=0; ssdindex<4; ssdindex++) {
    cout<<endl;
    for (Int_t imode=0; imode<nummode; imode++) {
      ratio_mode[ssdindex][imode] = (Double_t) entry_mode[ssdindex][imode]/entry_globalmulti[ssdindex];
      ratio_sum[ssdindex] += ratio_mode[ssdindex][imode];

      cout<<"SSD"<<ssdindex+1<<setw(10)<<Form("mode[%d] = %s",imode,mode[imode].c_str())<<setw(10)<<setprecision(6)<<"ratio = "<<ratio_mode[ssdindex][imode]<<endl;
      FileOut<<"SSD"<<ssdindex+1<<setw(10)<<Form("mode[%d] = %s",imode,mode[imode].c_str())<<setw(10)<<setprecision(6)<<"ratio = "<<ratio_mode[ssdindex][imode]<<endl;
    }
    cout<<"SSD"<<ssdindex+1<<setw(15)<<"ratio_sum = "<<ratio_sum[ssdindex]<<endl;
    FileOut<<"SSD"<<ssdindex+1<<setw(15)<<"ratio_sum = "<<ratio_sum[ssdindex]<<endl;
    cout<<endl;
  }
}

//__________________________________
// 根据 globalmulti 提取不同模式的数据
void CSHINETrackReconstruction::L2L3_Mode_ExtractData(Int_t globalmulti)
{
  Int_t nummode = pow(globalmulti, 4);
  Int_t gssdnum = -1;

  std::string pidtag("L2L3");

  Int_t entry_mode[NUM_SSD][nummode];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t imode=0; imode<nummode; imode++) {
      entry_mode[ssdindex][imode] = 0;
    }
  }

  string mode_exp;
  vector<string> mode;
  L2L3_GenerateModeFromGMulti(globalmulti, mode);

  std::string pathModeDistribution(Form("%sfigure_TrackReconstruction/%s_GlobalMulti%d_ModeStatistic.png", PATHFIGURESFOLDER, pidtag.c_str(), globalmulti));

  std::string pathDataOutput[NUM_SSD][nummode];
  ofstream FileOut[NUM_SSD][nummode];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t imode=0; imode<nummode; imode++) {
      pathDataOutput[ssdindex][imode] = Form("%sdata_Test_Multi/%s/globalmulti%d/SSD%d_%s_Test_Multi_fGlobalMulti%d_%s.dat", PATHDATAFOLDER, pidtag.c_str(), globalmulti, ssdindex+1, pidtag.c_str(), globalmulti, mode[imode].c_str());
      FileOut[ssdindex][imode].open(pathDataOutput[ssdindex][imode].c_str());
      FileOut[ssdindex][imode]<<setw(10)<<"# EntryNo."<<setw(10)<<"MultiGlobal"<<setw(8)<<"CSI"<<setw(10)<<"L2B"
                              <<setw(10)<<"L2F"<<setw(10)<<"L1S"<<setw(10)<<"EL1S"
                              <<setw(10)<<"EL2F"<<setw(10)<<"EL2B"<<setw(10)<<"ECsI"
                              <<endl;
    }
  }

  // fChainTrackTree->SetEntries(10000);
  Long64_t nentries = fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing ExtractData globalmulti = %d ......", globalmulti)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    // fill data
    if (fTrackEvent.fGlobalMulti > 0) {
      for (Int_t gmulti=0; gmulti<fTrackEvent.fGlobalMulti; gmulti++) {
        gssdnum = fTrackEvent.fGSSDNum[gmulti];
        L2L3_CSIECh [gssdnum].push_back(fTrackEvent.fGCsIECh[gmulti]);
        L2L3_L2BEMeV[gssdnum].push_back(fTrackEvent.fGL2BEMeV[gmulti]);
        L2L3_L2FEMeV[gssdnum].push_back(fTrackEvent.fGL2FEMeV[gmulti]);
        L2L3_L1SEMeV[gssdnum].push_back(fTrackEvent.fGL1SEMeV[gmulti]);
        L2L3_CsINum[gssdnum].push_back(fTrackEvent.fGCsINum[gmulti]);
        L2L3_L2BStripNum[gssdnum].push_back(fTrackEvent.fGL2BNumStrip[gmulti]);
        L2L3_L2FStripNum[gssdnum].push_back(fTrackEvent.fGL2FNumStrip[gmulti]);
        L2L3_L1SStripNum[gssdnum].push_back(fTrackEvent.fGL1SNumStrip[gmulti]);
      }
    }
    // save data
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      if (fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        mode_exp = L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
        for (Int_t imode=0; imode<nummode; imode++) {
          if (strcmp(mode_exp.c_str(), mode[imode].c_str()) == 0) {
            for (Int_t ssdgmulti=0; ssdgmulti<globalmulti; ssdgmulti++) {
              entry_mode[ssdindex][imode]++;

              FileOut[ssdindex][imode]<<setw(7)<<ientry
                <<setw(10)<<ssdgmulti+1
                <<setw(12)<<L2L3_CsINum[ssdindex][ssdgmulti]
                <<setw(10)<<L2L3_L2BStripNum[ssdindex][ssdgmulti]
                <<setw(10)<<L2L3_L2FStripNum[ssdindex][ssdgmulti]
                <<setw(10)<<L2L3_L1SStripNum[ssdindex][ssdgmulti]
                <<setw(13)<<L2L3_L1SEMeV[ssdindex][ssdgmulti]
                <<setw(10)<<L2L3_L2FEMeV[ssdindex][ssdgmulti]
                <<setw(10)<<L2L3_L2BEMeV[ssdindex][ssdgmulti]
                <<setw(10)<<L2L3_CSIECh[ssdindex][ssdgmulti]
                <<endl;
            }
          }
        }
      }
      // clear the vectors
      L2L3_CSIECh [ssdindex].clear();
      L2L3_L2BEMeV[ssdindex].clear();
      L2L3_L2FEMeV[ssdindex].clear();
      L2L3_L1SEMeV[ssdindex].clear();
      L2L3_CsINum[ssdindex].clear();
      L2L3_L2BStripNum[ssdindex].clear();
      L2L3_L2FStripNum[ssdindex].clear();
      L2L3_L1SStripNum[ssdindex].clear();
    }
  }
  // 删除空的文件
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t imode=0; imode<nummode; imode++) {
      if (entry_mode[ssdindex][imode] == 0) system(Form("rm %s",pathDataOutput[ssdindex][imode].c_str()));  //删除空文件
    }
  }
}
//******************************************************************************


//******************************************************************************
// L1L2 径迹重建模式定义： 根据 globalmulti 定义模式
void CSHINETrackReconstruction::L1L2_GenerateModeFromGMulti(Int_t globalmulti, vector<string>& mode)
{
  if (globalmulti<1) { cout<<"Error: globalmulti<1"<<endl; return; }
  for (Int_t ml2b=0; ml2b<globalmulti; ml2b++) {
    for (Int_t ml2f=0; ml2f<globalmulti; ml2f++) {
      for (Int_t ml1s=0; ml1s<globalmulti; ml1s++) {
        mode.push_back(Form("%d%d%d",ml2b,ml2f,ml1s));
      }
    }
  }
}

//____________________________
// 根据实验数据进行模式匹配
string CSHINETrackReconstruction::L1L2_CalcModeFromExpData(Int_t globalmultti, vector<Int_t> ml2b,
  vector<Int_t> ml2f, vector<Int_t> ml1s)
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

//___________________________________
// 画出不同 globalmulti 下的模式分布
void CSHINETrackReconstruction::L1L2_Mode_DrawClassification(Int_t globalmulti)
{
  Int_t nummode = pow(globalmulti, 3);
  Int_t gssdnum = -1;

  std::string pidtag("L1L2");

  string mode_exp;
  vector<string> mode;
  L1L2_GenerateModeFromGMulti(globalmulti, mode);

  std::string pathModeDistribution(Form("%sfigure_TrackReconstruction/%s_GlobalMulti%d_ModeStatistic.png", PATHFIGURESFOLDER, pidtag.c_str(), globalmulti));

  TH1I* h_modecount[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h_modecount[ssdindex] = new TH1I(Form("SSD%d_gmulti%d_modes",ssdindex+1,globalmulti),Form("SSD%d_gmulti%d_modes",ssdindex+1,globalmulti),nummode,-0.5,nummode-0.5);
  }

  fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing DrawClassification globalmulti = %d ......", globalmulti)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    // fill data
    if (fTrackEvent.fGlobalMulti > 0) {
      for (Int_t gmulti=0; gmulti<fTrackEvent.fGlobalMulti; gmulti++) {
        gssdnum = fTrackEvent.fGSSDNum[gmulti];
        L2L3_CsINum[gssdnum].push_back(fTrackEvent.fGCsINum[gmulti]);
        L2L3_L2BStripNum[gssdnum].push_back(fTrackEvent.fGL2BNumStrip[gmulti]);
        L2L3_L2FStripNum[gssdnum].push_back(fTrackEvent.fGL2FNumStrip[gmulti]);
        L2L3_L1SStripNum[gssdnum].push_back(fTrackEvent.fGL1SNumStrip[gmulti]);
      }
    }
    // save data
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      if (fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        mode_exp = L1L2_CalcModeFromExpData(globalmulti,L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
        for (Int_t imode=0; imode<nummode; imode++) {
          if (strcmp(mode_exp.c_str(), mode[imode].c_str()) == 0) {
            h_modecount[ssdindex]->Fill(imode);
          }
        }
      }
      // clear the vectors
      L2L3_CsINum[ssdindex].clear();
      L2L3_L2BStripNum[ssdindex].clear();
      L2L3_L2FStripNum[ssdindex].clear();
      L2L3_L1SStripNum[ssdindex].clear();
    }
  }
  // 画图
  TCanvas* cans = new TCanvas("cans", "cans", 1200, 1200);
  cans->Divide(2,2);

  cans->cd(1);
  h_modecount[0]->Draw();
//  h_modecount[0]->GetXaxis()->SetNdivisions(100+nummode);
  h_modecount[0]->GetYaxis()->SetRangeUser(0, 1.1*h_modecount[0]->GetMaximum());

  cans->cd(2);
  h_modecount[1]->Draw();
//  h_modecount[1]->GetXaxis()->SetNdivisions(100+nummode);
  h_modecount[1]->GetYaxis()->SetRangeUser(0, 1.1*h_modecount[1]->GetMaximum());

  cans->cd(3);
  h_modecount[2]->Draw();
//  h_modecount[2]->GetXaxis()->SetNdivisions(100+nummode);
  h_modecount[2]->GetYaxis()->SetRangeUser(0, 1.1*h_modecount[2]->GetMaximum());

  cans->cd(4);
  h_modecount[3]->Draw();
//  h_modecount[3]->GetXaxis()->SetNdivisions(100+nummode);
  h_modecount[3]->GetYaxis()->SetRangeUser(0, 1.1*h_modecount[3]->GetMaximum());

  cans->Print(pathModeDistribution.c_str());
}

//_________________________________________
// 计算不同模式的占比
void CSHINETrackReconstruction::L1L2_Mode_CalcRatio(Int_t globalmulti)
{
  Int_t nummode = pow(globalmulti, 3);
  Int_t gssdnum = -1;

  Double_t ratio_sum[NUM_SSD];
  Double_t ratio_mode[NUM_SSD][nummode];

  Int_t entry_globalmulti[NUM_SSD];
  Int_t entry_mode[NUM_SSD][nummode];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    ratio_sum[ssdindex] = 0.;
    entry_globalmulti[ssdindex] = 0;
    for (Int_t imode=0; imode<nummode; imode++) {
      entry_mode[ssdindex][imode] = 0;
    }
  }

  string mode_exp;
  vector<string> mode;
  L1L2_GenerateModeFromGMulti(globalmulti, mode);

  std::string pidtag("L1L2");
  std::string pathModeRatioOut(Form("%sdata_Test_Multi/%s_Test_Multi_fGlobalMulti%d_ModeRatio.dat",PATHDATAFOLDER,pidtag.c_str(),globalmulti));
  ofstream FileOut(pathModeRatioOut.c_str());
  FileOut<<"* Ratio of different modes of events with globalmulti=2"<<endl;

  fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing CaliRatio globalmulti = %d ......", globalmulti)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    // fill data
    if (fTrackEvent.fGlobalMulti > 0) {
      for (Int_t gmulti=0; gmulti<fTrackEvent.fGlobalMulti; gmulti++) {
        gssdnum = fTrackEvent.fGSSDNum[gmulti];
        L2L3_CsINum[gssdnum].push_back(fTrackEvent.fGCsINum[gmulti]);
        L2L3_L2BStripNum[gssdnum].push_back(fTrackEvent.fGL2BNumStrip[gmulti]);
        L2L3_L2FStripNum[gssdnum].push_back(fTrackEvent.fGL2FNumStrip[gmulti]);
        L2L3_L1SStripNum[gssdnum].push_back(fTrackEvent.fGL1SNumStrip[gmulti]);
      }
    }
    // save data
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      if (fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        entry_globalmulti[ssdindex]++;
        mode_exp = L1L2_CalcModeFromExpData(globalmulti,L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
        for (Int_t imode=0; imode<nummode; imode++) {
          if (strcmp(mode_exp.c_str(), mode[imode].c_str()) == 0) {
            entry_mode[ssdindex][imode]++;
          }
        }
      }
      // clear the vectors
      L2L3_CsINum[ssdindex].clear();
      L2L3_L2BStripNum[ssdindex].clear();
      L2L3_L2FStripNum[ssdindex].clear();
      L2L3_L1SStripNum[ssdindex].clear();
    }
  }
  // 计算模式比例
  for (Int_t ssdindex=0; ssdindex<4; ssdindex++) {
    cout<<endl;
    for (Int_t imode=0; imode<nummode; imode++) {
      ratio_mode[ssdindex][imode] = (Double_t) entry_mode[ssdindex][imode]/entry_globalmulti[ssdindex];
      ratio_sum[ssdindex] += ratio_mode[ssdindex][imode];

      cout<<"SSD"<<ssdindex+1<<setw(10)<<Form("mode[%d] = %s",imode,mode[imode].c_str())<<setw(10)<<setprecision(6)<<"ratio = "<<ratio_mode[ssdindex][imode]<<endl;
      FileOut<<"SSD"<<ssdindex+1<<setw(10)<<Form("mode[%d] = %s",imode,mode[imode].c_str())<<setw(10)<<setprecision(6)<<"ratio = "<<ratio_mode[ssdindex][imode]<<endl;
    }
    cout<<"SSD"<<ssdindex+1<<setw(15)<<"ratio_sum = "<<ratio_sum[ssdindex]<<endl;
    FileOut<<"SSD"<<ssdindex+1<<setw(15)<<"ratio_sum = "<<ratio_sum[ssdindex]<<endl;
    cout<<endl;
  }
}

//__________________________________
// 根据 globalmulti 提取不同模式的数据
void CSHINETrackReconstruction::L1L2_Mode_ExtractData(Int_t globalmulti)
{
  Int_t nummode = pow(globalmulti, 3);
  Int_t gssdnum = -1;

  std::string pidtag("L1L2");

  Int_t entry_mode[NUM_SSD][nummode];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t imode=0; imode<nummode; imode++) {
      entry_mode[ssdindex][imode] = 0;
    }
  }

  string mode_exp;
  vector<string> mode;
  L1L2_GenerateModeFromGMulti(globalmulti, mode);

  std::string pathModeDistribution(Form("%sfigure_TrackReconstruction/%s_GlobalMulti%d_ModeStatistic.png",PATHFIGURESFOLDER,pidtag.c_str(),globalmulti));

  std::string pathDataOutput[NUM_SSD][nummode];
  ofstream FileOut[NUM_SSD][nummode];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t imode=0; imode<nummode; imode++) {
      pathDataOutput[ssdindex][imode] = Form("%sdata_Test_Multi/%s/globalmulti%d/SSD%d_%s_Test_Multi_fGlobalMulti%d_%s.dat", PATHDATAFOLDER, pidtag.c_str(), globalmulti, ssdindex+1, pidtag.c_str(), globalmulti, mode[imode].c_str());
      FileOut[ssdindex][imode].open(pathDataOutput[ssdindex][imode].c_str());
      FileOut[ssdindex][imode]<<setw(10)<<"# EntryNo."<<setw(10)<<"MultiGlobal"<<setw(8)<<"CSI"<<setw(10)<<"L2B"
                              <<setw(10)<<"L2F"<<setw(10)<<"L1S"<<setw(10)<<"EL1S"
                              <<setw(10)<<"EL2F"<<setw(10)<<"EL2B"<<setw(10)<<"ECsI"
                              <<endl;
    }
  }

  fChainTrackTree->SetEntries(1000);
  Long64_t nentries = fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing ExtractData globalmulti = %d ......", globalmulti)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    // fill data
    if (fTrackEvent.fGlobalMulti > 0) {
      for (Int_t gmulti=0; gmulti<fTrackEvent.fGlobalMulti; gmulti++) {
        gssdnum = fTrackEvent.fGSSDNum[gmulti];
        L2L3_CSIECh [gssdnum].push_back(fTrackEvent.fGCsIECh[gmulti]);
        L2L3_L2BEMeV[gssdnum].push_back(fTrackEvent.fGL2BEMeV[gmulti]);
        L2L3_L2FEMeV[gssdnum].push_back(fTrackEvent.fGL2FEMeV[gmulti]);
        L2L3_L1SEMeV[gssdnum].push_back(fTrackEvent.fGL1SEMeV[gmulti]);
        L2L3_CsINum[gssdnum].push_back(fTrackEvent.fGCsINum[gmulti]);
        L2L3_L2BStripNum[gssdnum].push_back(fTrackEvent.fGL2BNumStrip[gmulti]);
        L2L3_L2FStripNum[gssdnum].push_back(fTrackEvent.fGL2FNumStrip[gmulti]);
        L2L3_L1SStripNum[gssdnum].push_back(fTrackEvent.fGL1SNumStrip[gmulti]);
      }
    }
    // save data
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      if (fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        mode_exp = L1L2_CalcModeFromExpData(globalmulti,L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
        for (Int_t imode=0; imode<nummode; imode++) {
          if (strcmp(mode_exp.c_str(), mode[imode].c_str()) == 0) {
            for (Int_t ssdgmulti=0; ssdgmulti<globalmulti; ssdgmulti++) {
              entry_mode[ssdindex][imode]++;

              FileOut[ssdindex][imode]<<setw(7)<<ientry
                <<setw(10)<<ssdgmulti+1
                <<setw(12)<<L2L3_CsINum[ssdindex][ssdgmulti]
                <<setw(10)<<L2L3_L2BStripNum[ssdindex][ssdgmulti]
                <<setw(10)<<L2L3_L2FStripNum[ssdindex][ssdgmulti]
                <<setw(10)<<L2L3_L1SStripNum[ssdindex][ssdgmulti]
                <<setw(13)<<L2L3_L1SEMeV[ssdindex][ssdgmulti]
                <<setw(10)<<L2L3_L2FEMeV[ssdindex][ssdgmulti]
                <<setw(10)<<L2L3_L2BEMeV[ssdindex][ssdgmulti]
                <<setw(10)<<L2L3_CSIECh[ssdindex][ssdgmulti]
                <<endl;
            }
          }
        }
      }
      // clear the vectors
      L2L3_CSIECh [ssdindex].clear();
      L2L3_L2BEMeV[ssdindex].clear();
      L2L3_L2FEMeV[ssdindex].clear();
      L2L3_L1SEMeV[ssdindex].clear();
      L2L3_CsINum[ssdindex].clear();
      L2L3_L2BStripNum[ssdindex].clear();
      L2L3_L2FStripNum[ssdindex].clear();
      L2L3_L1SStripNum[ssdindex].clear();
    }
  }
  // 删除空的文件
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t imode=0; imode<nummode; imode++) {
      if (entry_mode[ssdindex][imode] == 0) system(Form("rm %s",pathDataOutput[ssdindex][imode].c_str()));  //删除空文件
    }
  }
}


//___________________________________
// 通过能量条件定义 sub-mode (L2L3)
Int_t CSHINETrackReconstruction::L2L3_GetSubModeFromEneConstraints(Bool_t IsBananaCut, Bool_t IsEL2BEL2F, Bool_t IsEL1SEL2F)
{
  Int_t index;
  Int_t index_BananaCut, index_EL2BEL2F, index_EL1SEL2F;
  if (IsBananaCut) { index_BananaCut = 1; }  else { index_BananaCut = 0; }
  if (IsEL2BEL2F)  { index_EL2BEL2F  = 1; }  else { index_EL2BEL2F  = 0; }
  if (IsEL1SEL2F)  { index_EL1SEL2F  = 1; }  else { index_EL1SEL2F  = 0; }

  index = index_BananaCut*pow(2,2) + index_EL2BEL2F*pow(2,1) + index_EL1SEL2F*1;
  return index;
}


//___________________________________
// 通过能量条件定义 sub-mode (L1L2)
Int_t CSHINETrackReconstruction::L1L2_GetSubModeFromEneConstraints(Bool_t IsEL2BEL2F, Bool_t IsBananaCut)
{
  Int_t index;
  Int_t index_EL2BEL2F, index_BananaCut;
  if (IsEL2BEL2F)  { index_EL2BEL2F  = 1; }  else { index_EL2BEL2F  = 0; }
  if (IsBananaCut) { index_BananaCut = 1; }  else { index_BananaCut = 0; }

  index = index_EL2BEL2F*pow(2,1) + index_BananaCut*1;
  return index;
}
//******************************************************************************


//******************************************************************************
//                        fGlobalMulti = 1
//                    ------------------------
// 解码 fGlobalMulit = 1的事件, 即检查是否有异常事件
DecodefGlobalMulti1::DecodefGlobalMulti1(Int_t firstrun, Int_t lastrun)
{
  fFirstRun = firstrun;
  fLastRun  = lastrun;

  trackreconstruct = new CSHINETrackReconstruction(fFirstRun, fLastRun);

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
        bananacut[numtel].push_back(cut);
      //  cout<<cut->GetName()<<endl;
      }
    }
  }
}

DecodefGlobalMulti1::~DecodefGlobalMulti1()
{}

//_______________________________________________
// 对 globalmulti = 1 的事件进行综合检查
// (1)平行坐标表示, 检验层间关系是否有误
// (2)检查 fGlobalMulit = 1 事件的 DE-E plot
void DecodefGlobalMulti1::GlobalMulti1_Checks()
{
  gROOT->cd();

  Int_t gmulti = 1;
  const char* gmultitag = "fGlobalMulti1";
  trackreconstruct->L2L3_CheckParallelDraw(gmultitag, gmulti);
  trackreconstruct->L2L3_CheckL2L3DEEPlot (gmultitag, gmulti);
  trackreconstruct->L2L3_CheckL1L2DEEPlot (gmultitag, gmulti);
}


//_______________________________________________________________
// 对 globalmulti = 1 的情况进行解析
// 解析内容包括:
//  增加约束条件 4: IsEneConstraint_L2B_L2F()
//  增加约束条件 5: IsEneConstraint_L1S_L2F()
//  增加约束条件 6: BananaCut.IsInside(EL2F, ECsI);
void DecodefGlobalMulti1::GlobalMulti1_L2L3_Decode()
{
  Int_t globalmulti = 1;
  Int_t ssdnum, csinum, bananacut_telnum;
  Double_t el1s, el2f, el2b, ecsi;
  Int_t MAXPARTICLES = 20;

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode.pdf",PATHFIGURESFOLDER,globalmulti));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode.pdf[",PATHFIGURESFOLDER,globalmulti));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode.pdf]",PATHFIGURESFOLDER,globalmulti));

  TH2F h2_L2L3_Geo[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_Geo_EneL2BL2F[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_Geo_EneL2BL2F_EneL12L2F[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_Geo_EneL2BL2F_EneL12L2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_Geo_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_Geo[NUM_SSD];
  TH2F h2_L1L2_Geo_EneL2BL2F[NUM_SSD];
  TH2F h2_L1L2_Geo_EneL2BL2F_EneL12L2F[NUM_SSD];
  TH2F h2_L1L2_Geo_EneL2BL2F_EneL12L2F_PID[NUM_SSD];
  TH2F h2_L1L2_Geo_PID[NUM_SSD];
  TH2F h2_L1L2_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h2_L2L3_Geo[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_Geo",ssdindex+1,csiindex),Form("SSD%d_CsI%d_Geo",ssdindex+1,csiindex),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_Geo_EneL2BL2F[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_Geo_EnenL2BL2F",ssdindex+1,csiindex),Form("SSD%d_CsI%d_Geo_EnenL2BL2F",ssdindex+1,csiindex),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_Geo_EneL2BL2F_EneL12L2F[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_Geo_EnenL2BL2F_EneL1SL2F",ssdindex+1,csiindex),Form("SSD%d_CsI%d_Geo_EnenL2BL2F_EneL1SL2F",ssdindex+1,csiindex),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_Geo_EnenL2BL2F_EneL1SL2F_PID",ssdindex+1,csiindex),Form("SSD%d_CsI%d_Geo_EnenL2BL2F_EneL1SL2F_PID",ssdindex+1,csiindex),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_Geo_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_Geo_PID",ssdindex+1,csiindex),Form("SSD%d_CsI%d_Geo_PID",ssdindex+1,csiindex),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_Discard",ssdindex+1,csiindex),Form("SSD%d_CsI%d_Discard",ssdindex+1,csiindex),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
    }
    h2_L1L2_Geo[ssdindex] = TH2F(Form("SSD%d_Geo",ssdindex+1),Form("SSD%d_Geo",ssdindex+1),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    h2_L1L2_Geo_EneL2BL2F[ssdindex] = TH2F(Form("SSD%d_Geo_EnenL2BL2F",ssdindex+1),Form("SSD%d_Geo_EnenL2BL2F",ssdindex+1),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    h2_L1L2_Geo_EneL2BL2F_EneL12L2F[ssdindex] = TH2F(Form("SSD%d_Geo_EnenL2BL2F_EneL1SL2F",ssdindex+1),Form("SSD%d_Geo_EnenL2BL2F_EneL1SL2F",ssdindex+1),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    h2_L1L2_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex] = TH2F(Form("SSD%d_Geo_EnenL2BL2F_EneL1SL2F_PID",ssdindex+1),Form("SSD%d_Geo_EnenL2BL2F_EneL1SL2F_PID",ssdindex+1),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    h2_L1L2_Geo_PID[ssdindex] = TH2F(Form("SSD%d_Geo_PID",ssdindex+1),Form("SSD%d_Geo_PID",ssdindex+1),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    h2_L1L2_Discard[ssdindex] = TH2F(Form("SSD%d_Discard",ssdindex+1),Form("SSD%d_Discard",ssdindex+1),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

//  trackreconstruct->fChainTrackTree->SetEntries(1000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L2L3_SSDNum [ssdindex].clear();
      L2L3_CsINum [ssdindex].clear();
      L2L3_CSIECh [ssdindex].clear();
      L2L3_L2BEMeV[ssdindex].clear();
      L2L3_L2FEMeV[ssdindex].clear();
      L2L3_L1SEMeV[ssdindex].clear();
      BananaCut_TelNum[ssdindex].clear();

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
            L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
            L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
            L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
            BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
          }
        }
      }
      // 填充数据
      if (L2L3_CsINum[ssdindex].size() == globalmulti) {
        h2_L2L3_Geo[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
        h2_L1L2_Geo[ssdindex].Fill(L2L3_L2FEMeV[ssdindex][0], L2L3_L1SEMeV[ssdindex][0]);

        // 考虑L2B_L2F能量约束条件
        if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0])) {
          h2_L2L3_Geo_EneL2BL2F[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
          h2_L1L2_Geo_EneL2BL2F[ssdindex].Fill(L2L3_L2FEMeV[ssdindex][0], L2L3_L1SEMeV[ssdindex][0]);
          // 考虑 L1S_L2F 能量约束条件
          if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0])) {
            h2_L2L3_Geo_EneL2BL2F_EneL12L2F[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
            h2_L1L2_Geo_EneL2BL2F_EneL12L2F[ssdindex].Fill(L2L3_L2FEMeV[ssdindex][0], L2L3_L1SEMeV[ssdindex][0]);
            // 在已考虑能量约束下, 再考虑 PID 约束条件
            if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0])) {
              h2_L2L3_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
              h2_L1L2_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex].Fill(L2L3_L2FEMeV[ssdindex][0], L2L3_L1SEMeV[ssdindex][0]);
            }
          }
        }

        // 考虑 PID 约束
        if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0])) {
          h2_L2L3_Geo_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
          h2_L1L2_Geo_PID[ssdindex].Fill(L2L3_L2FEMeV[ssdindex][0], L2L3_L1SEMeV[ssdindex][0]);
        }
        // 考察丢掉的事件
        if (!(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]) &&
              trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]) &&
              trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0])))
        {
          h2_L2L3_Discard[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
          h2_L1L2_Discard[ssdindex].Fill(L2L3_L2FEMeV[ssdindex][0], L2L3_L1SEMeV[ssdindex][0]);
        }
      }
    }
  }
  // 画图
  TCanvas* c_begin = new TCanvas("c_begin","c_begin",600,600);
  TCanvas* c_end   = new TCanvas("c_end","c_end",600,600);
  TCanvas* cans_L2L3 = new TCanvas("cans_L2L3","cans_L2L3",1500,1000);
  cans_L2L3->Divide(3,2);
  TCanvas* cans_L1L2 = new TCanvas("cans_L1L2","cans_L1L2",1500,1000);
  cans_L1L2->Divide(3,2);

  c_begin->Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    cans_L1L2->cd(1);
    h2_L1L2_Geo[ssdindex].Draw("COL");
    h2_L1L2_Geo[ssdindex].GetXaxis()->SetNdivisions(506);
    h2_L1L2_Geo[ssdindex].GetYaxis()->SetNdivisions(506);

    cans_L1L2->cd(2);
    h2_L1L2_Geo_EneL2BL2F[ssdindex].Draw("COL");
    h2_L1L2_Geo_EneL2BL2F[ssdindex].GetXaxis()->SetNdivisions(506);
    h2_L1L2_Geo_EneL2BL2F[ssdindex].GetYaxis()->SetNdivisions(506);

    cans_L1L2->cd(3);
    h2_L1L2_Geo_EneL2BL2F_EneL12L2F[ssdindex].Draw("COL");
    h2_L1L2_Geo_EneL2BL2F_EneL12L2F[ssdindex].GetXaxis()->SetNdivisions(506);
    h2_L1L2_Geo_EneL2BL2F_EneL12L2F[ssdindex].GetYaxis()->SetNdivisions(506);

    cans_L1L2->cd(4);
    h2_L1L2_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex].Draw("COL");
    h2_L1L2_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex].GetXaxis()->SetNdivisions(506);
    h2_L1L2_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex].GetYaxis()->SetNdivisions(506);

    cans_L1L2->cd(5);
    h2_L1L2_Geo_PID[ssdindex].Draw("COL");
    h2_L1L2_Geo_PID[ssdindex].GetXaxis()->SetNdivisions(506);
    h2_L1L2_Geo_PID[ssdindex].GetYaxis()->SetNdivisions(506);

    cans_L1L2->cd(6);
    h2_L1L2_Discard[ssdindex].Draw("COL");
    h2_L1L2_Discard[ssdindex].GetXaxis()->SetNdivisions(506);
    h2_L1L2_Discard[ssdindex].GetYaxis()->SetNdivisions(506);

    gPad->Modified();
    gPad->Update();

    cans_L1L2->Print(pathPDFOut.c_str());

    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      cans_L2L3->cd(1);
      h2_L2L3_Geo[ssdindex][csiindex].Draw("COL");
      h2_L2L3_Geo[ssdindex][csiindex].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo[ssdindex][csiindex].GetYaxis()->SetNdivisions(506);

      cans_L2L3->cd(2);
      h2_L2L3_Geo_EneL2BL2F[ssdindex][csiindex].Draw("COL");
      h2_L2L3_Geo_EneL2BL2F[ssdindex][csiindex].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL2BL2F[ssdindex][csiindex].GetYaxis()->SetNdivisions(506);

      cans_L2L3->cd(3);
      h2_L2L3_Geo_EneL2BL2F_EneL12L2F[ssdindex][csiindex].Draw("COL");
      h2_L2L3_Geo_EneL2BL2F_EneL12L2F[ssdindex][csiindex].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL2BL2F_EneL12L2F[ssdindex][csiindex].GetYaxis()->SetNdivisions(506);

      cans_L2L3->cd(4);
      h2_L2L3_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex][csiindex].Draw("COL");
      h2_L2L3_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex][csiindex].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex][csiindex].GetYaxis()->SetNdivisions(506);

      cans_L2L3->cd(5);
      h2_L2L3_Geo_PID[ssdindex][csiindex].Draw("COL");
      h2_L2L3_Geo_PID[ssdindex][csiindex].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_PID[ssdindex][csiindex].GetYaxis()->SetNdivisions(506);

      cans_L2L3->cd(6);
      h2_L2L3_Discard[ssdindex][csiindex].Draw("COL");
      h2_L2L3_Discard[ssdindex][csiindex].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Discard[ssdindex][csiindex].GetYaxis()->SetNdivisions(506);

      gPad->Modified();
      gPad->Update();

      cans_L2L3->Print(pathPDFOut.c_str());
    }
  }
  c_end->Print(pathPDFOutEnd.c_str());
}

//______________________________________________________________________________
// 查看径迹重建中扔掉的事件，看看是否有 “好的”事件被丢掉了
void DecodefGlobalMulti1::GlobalMulti_L2L3_Discard()
{
  Int_t globalmulti = 1;
  Int_t ssdnum, csinum, bananacut_telnum;
  Double_t el1s, el2f, el2b, ecsi;
  Int_t MAXPARTICLES = 20;

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Discard.pdf",PATHFIGURESFOLDER,globalmulti));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Discard.pdf[",PATHFIGURESFOLDER,globalmulti));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Discard.pdf]",PATHFIGURESFOLDER,globalmulti));

  TH2F h2_L2L3_Geo[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_Geo_EneL2BL2F_EneL12L2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_Geo_EneL2BL2F_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_Geo_EneL1SL2F_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_Geo_PID_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_Geo[NUM_SSD];
  TH2F h2_L1L2_Geo_EneL2BL2F_EneL12L2F_PID[NUM_SSD];
  TH2F h2_L1L2_Geo_EneL2BL2F_Discard[NUM_SSD];
  TH2F h2_L1L2_Geo_EneL1SL2F_Discard[NUM_SSD];
  TH2F h2_L1L2_Geo_PID_Discard[NUM_SSD];
  TH2F h2_L1L2_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h2_L2L3_Geo[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_Geo",ssdindex+1,csiindex),Form("SSD%d_CsI%d_Geo",ssdindex+1,csiindex),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_Geo_EnenL2BL2F_EneL1SL2F_PID",ssdindex+1,csiindex),Form("SSD%d_CsI%d_Geo_EnenL2BL2F_EneL1SL2F_PID",ssdindex+1,csiindex),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_Geo_EneL2BL2F_Discard",ssdindex+1,csiindex),Form("SSD%d_CsI%d_Geo_EneL2BL2F_Discard",ssdindex+1,csiindex),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_Geo_EneL1SL2F_Discard",ssdindex+1,csiindex),Form("SSD%d_CsI%d_Geo_EneL1SL2F_Discard",ssdindex+1,csiindex),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_Geo_PID_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_Geo_PID_Discard",ssdindex+1,csiindex),Form("SSD%d_CsI%d_Geo_PID_Discard",ssdindex+1,csiindex),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_Discard",ssdindex+1,csiindex),Form("SSD%d_CsI%d_Discard",ssdindex+1,csiindex),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
    }
    h2_L1L2_Geo[ssdindex] = TH2F(Form("SSD%d_Geo",ssdindex+1),Form("SSD%d_Geo",ssdindex+1),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    h2_L1L2_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex] = TH2F(Form("SSD%d_Geo_EneL2BL2F_EneL1SL2F_PID",ssdindex+1),Form("SSD%d_Geo_EneL2BL2F_EneL1SL2F_PID",ssdindex+1),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    h2_L1L2_Geo_EneL2BL2F_Discard[ssdindex] = TH2F(Form("SSD%d_Geo_EneL2BL2F_Discard",ssdindex+1),Form("SSD%d_Geo_EneL2BL2F_Discard",ssdindex+1),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    h2_L1L2_Geo_EneL1SL2F_Discard[ssdindex] = TH2F(Form("SSD%d_Geo_EneL12L2F_Discard",ssdindex+1),Form("SSD%d_Geo_EneL12L2F_Discard",ssdindex+1),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    h2_L1L2_Geo_PID_Discard[ssdindex] = TH2F(Form("SSD%d_Geo_PID_Discard",ssdindex+1),Form("SSD%d_Geo_PID_Discard",ssdindex+1),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    h2_L1L2_Discard[ssdindex] = TH2F(Form("SSD%d_Discard",ssdindex+1),Form("SSD%d_Discard",ssdindex+1),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  //trackreconstruct->fChainTrackTree->SetEntries(1000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L2L3_CsINum [ssdindex].clear();
      L2L3_CSIECh [ssdindex].clear();
      L2L3_L2BEMeV[ssdindex].clear();
      L2L3_L2FEMeV[ssdindex].clear();
      L2L3_L1SEMeV[ssdindex].clear();
      BananaCut_TelNum[ssdindex].clear();

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
            L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
            L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
            L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
            BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
          }
        }
      }

      // 填充数据
      if (L2L3_CsINum[ssdindex].size() == globalmulti) {
        h2_L2L3_Geo[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
        h2_L1L2_Geo[ssdindex].Fill(L2L3_L2FEMeV[ssdindex][0], L2L3_L1SEMeV[ssdindex][0]);

        // 考虑能量约束条件+PID cut
        if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]) &&
            trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]) &&
            trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]], L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]))
        {
          h2_L2L3_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
          h2_L1L2_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex].Fill(L2L3_L2FEMeV[ssdindex][0], L2L3_L1SEMeV[ssdindex][0]);
        }

        // 不满足 L2B_L2F 能量关联
        if (!(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]))) {
          h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
          h2_L1L2_Geo_EneL2BL2F_Discard[ssdindex].Fill(L2L3_L2FEMeV[ssdindex][0], L2L3_L1SEMeV[ssdindex][0]);
        }
        // 不满足 L2B_L2F 能量关联
        if (!(trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]))) {
          h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
          h2_L1L2_Geo_EneL1SL2F_Discard[ssdindex].Fill(L2L3_L2FEMeV[ssdindex][0], L2L3_L1SEMeV[ssdindex][0]);
        }
        // 不满足 PID 约束
        if (!(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]], L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]))) {
          h2_L2L3_Geo_PID_Discard[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
          h2_L1L2_Geo_PID_Discard[ssdindex].Fill(L2L3_L2FEMeV[ssdindex][0], L2L3_L1SEMeV[ssdindex][0]);
        }
        // 考察所有丢掉的事件
        if (!(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]) &&
              trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]) &&
              trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0])))
        {
          h2_L2L3_Discard[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
          h2_L1L2_Discard[ssdindex].Fill(L2L3_L2FEMeV[ssdindex][0], L2L3_L1SEMeV[ssdindex][0]);
        }
      }
    }
  }
  // 画图
  TCanvas* c_begin = new TCanvas("c_begin","c_begin",600,600);
  TCanvas* c_end   = new TCanvas("c_end","c_end",600,600);
  TCanvas* cans_L2L3 = new TCanvas("cans_L2L3","cans_L2L3",1500,1000);
  cans_L2L3->Divide(3,2);
  TCanvas* cans_L1L2 = new TCanvas("cans_L1L2","cans_L1L2",1500,1000);
  cans_L1L2->Divide(3,2);

  c_begin->Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    cans_L1L2->cd(1);
    h2_L1L2_Geo[ssdindex].Draw("COL");
    h2_L1L2_Geo[ssdindex].GetXaxis()->SetNdivisions(506);
    h2_L1L2_Geo[ssdindex].GetYaxis()->SetNdivisions(506);

    cans_L1L2->cd(2);
    h2_L1L2_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex].Draw("COL");
    h2_L1L2_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex].GetXaxis()->SetNdivisions(506);
    h2_L1L2_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex].GetYaxis()->SetNdivisions(506);

    cans_L1L2->cd(3);
    h2_L1L2_Discard[ssdindex].Draw("COL");
    h2_L1L2_Discard[ssdindex].GetXaxis()->SetNdivisions(506);
    h2_L1L2_Discard[ssdindex].GetYaxis()->SetNdivisions(506);

    cans_L1L2->cd(4);
    h2_L1L2_Geo_EneL2BL2F_Discard[ssdindex].Draw("COL");
    h2_L1L2_Geo_EneL2BL2F_Discard[ssdindex].GetXaxis()->SetNdivisions(506);
    h2_L1L2_Geo_EneL2BL2F_Discard[ssdindex].GetYaxis()->SetNdivisions(506);

    cans_L1L2->cd(5);
    h2_L1L2_Geo_EneL1SL2F_Discard[ssdindex].Draw("COL");
    h2_L1L2_Geo_EneL1SL2F_Discard[ssdindex].GetXaxis()->SetNdivisions(506);
    h2_L1L2_Geo_EneL1SL2F_Discard[ssdindex].GetYaxis()->SetNdivisions(506);

    cans_L1L2->cd(6);
    h2_L1L2_Geo_PID_Discard[ssdindex].Draw("COL");
    h2_L1L2_Geo_PID_Discard[ssdindex].GetXaxis()->SetNdivisions(506);
    h2_L1L2_Geo_PID_Discard[ssdindex].GetYaxis()->SetNdivisions(506);

    gPad->Modified();
    gPad->Update();

    cans_L1L2->Print(pathPDFOut.c_str());

    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      cans_L2L3->cd(1);
      h2_L2L3_Geo[ssdindex][csiindex].Draw("COL");
      h2_L2L3_Geo[ssdindex][csiindex].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo[ssdindex][csiindex].GetYaxis()->SetNdivisions(506);

      cans_L2L3->cd(2);
      h2_L2L3_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex][csiindex].Draw("COL");
      h2_L2L3_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex][csiindex].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL2BL2F_EneL12L2F_PID[ssdindex][csiindex].GetYaxis()->SetNdivisions(506);

      cans_L2L3->cd(3);
      h2_L2L3_Discard[ssdindex][csiindex].Draw("COL");
      h2_L2L3_Discard[ssdindex][csiindex].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Discard[ssdindex][csiindex].GetYaxis()->SetNdivisions(506);

      cans_L2L3->cd(4);
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex].Draw("COL");
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex].GetYaxis()->SetNdivisions(506);

      cans_L2L3->cd(5);
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex].Draw("COL");
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex].GetYaxis()->SetNdivisions(506);

      cans_L2L3->cd(6);
      h2_L2L3_Geo_PID_Discard[ssdindex][csiindex].Draw("COL");
      h2_L2L3_Geo_PID_Discard[ssdindex][csiindex].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_PID_Discard[ssdindex][csiindex].GetYaxis()->SetNdivisions(506);

      gPad->Modified();
      gPad->Update();

      cans_L2L3->Print(pathPDFOut.c_str());
    }
  }
  c_end->Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
// 考察 EneL2BL2F 能量约束条件对 "丢弃"事件的影响
void DecodefGlobalMulti1::GlobalMulti_L2L3_EneL2BL2F_Discard()
{

  Int_t globalmulti = 1;
  Int_t ssdnum, csinum, bananacut_telnum;
  Double_t el1s, el2f, el2b, ecsi;
  Int_t MAXPARTICLES = 20;

  Int_t nratio = 5;
  Double_t EL2BL2FRatio[5] = {0.20,0.30,0.40,0.50,0.60};
  Double_t lost_ratio[NUM_SSD][NUM_CSI][5] = {{{0}}};
  Double_t lost_ratio_ave[NUM_SSD][NUM_CSI] = {{0}};
  Double_t lost_ratio_sum[NUM_SSD][NUM_CSI] = {{0}};
  Double_t lost_ratio_csi_sum[NUM_CSI][5] = {{0}};
  Double_t lost_ratio_csi_ave[NUM_CSI][5] = {{0}};

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_EneL2BL2F_GlobalMulti%d_Discard.pdf",PATHFIGURESFOLDER,globalmulti));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_EneL2BL2F_GlobalMulti%d_Discard.pdf[",PATHFIGURESFOLDER,globalmulti));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_EneL2BL2F_GlobalMulti%d_Discard.pdf]",PATHFIGURESFOLDER,globalmulti));

  TH2F h2_L2L3_Geo[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_Geo_EneL2BL2F_Discard[NUM_SSD][NUM_CSI][5];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h2_L2L3_Geo[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_Geo",ssdindex+1,csiindex),Form("SSD%d_CsI%d_Geo",ssdindex+1,csiindex),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      for (Int_t iratio=0; iratio<nratio; iratio++) {
          h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][iratio] = TH2F(Form("SSD%d_CsI%d_Geo_EneL2BL2F%.2f_Discard",ssdindex+1,csiindex,EL2BL2FRatio[iratio]),
            Form("SSD%d_CsI%d_Geo_EneL2BL2F%.2f_Discard",ssdindex+1,csiindex,EL2BL2FRatio[iratio]),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      }
    }
  }

  // trackreconstruct->fChainTrackTree->SetEntries(1000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L2L3_SSDNum [ssdindex].clear();
      L2L3_CsINum [ssdindex].clear();
      L2L3_CSIECh [ssdindex].clear();
      L2L3_L2BEMeV[ssdindex].clear();
      L2L3_L2FEMeV[ssdindex].clear();
      L2L3_L1SEMeV[ssdindex].clear();
      BananaCut_TelNum[ssdindex].clear();

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
            L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
            L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
            L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
            BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
          }
        }
      }
      // 填充数据
      if (L2L3_CsINum[ssdindex].size() == globalmulti) {
        h2_L2L3_Geo[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
        // 不满足 L2B_L2F 能量关联
        for (Int_t iratio=0; iratio<nratio; iratio++) {
          if (!(trackreconstruct->IsEneConstraint_L2B_L2F(L2L3_L2BEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0], EL2BL2FRatio[iratio]))) {
            h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][L2L3_CsINum[ssdindex][0]][iratio].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
          }
        }
      }
    }
  }

  // 计算“丢失”事件的比例
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      for (Int_t iratio=0; iratio<nratio; iratio++) {
        lost_ratio[ssdindex][csiindex][iratio] = (Double_t) h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][iratio].GetEntries()/h2_L2L3_Geo[ssdindex][csiindex].GetEntries()*100;
        lost_ratio_sum[ssdindex][csiindex] += lost_ratio[ssdindex][csiindex][iratio];
      }
      lost_ratio_ave[ssdindex][csiindex] = lost_ratio_sum[ssdindex][csiindex]/nratio;
    }
  }

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t iratio=0; iratio<nratio; iratio++) {
      for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
        lost_ratio_csi_sum[ssdindex][iratio] += lost_ratio[ssdindex][csiindex][iratio];
      }
      lost_ratio_csi_ave[ssdindex][iratio] = lost_ratio_csi_sum[ssdindex][iratio]/NUM_CSI;
      cout<<Form("SSD%d_L2L3_Ratio%.2f = %.4f",ssdindex+1,EL2BL2FRatio[iratio],lost_ratio_csi_ave[ssdindex][iratio])<<endl;
    }
  }

  // 定义 latex
  TLatex* latex_ratio[NUM_SSD][NUM_CSI][nratio];
  TLatex* latex_ratio_ave[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      Double_t x1 = 0.3 * h2_L2L3_Geo[ssdindex][csiindex].GetXaxis()->GetXmax();
      Double_t y1 = 0.9 * h2_L2L3_Geo[ssdindex][csiindex].GetYaxis()->GetXmax();
      Double_t x2 = 0.3 * h2_L2L3_Geo[ssdindex][csiindex].GetXaxis()->GetXmax();
      Double_t y2 = 0.8 * h2_L2L3_Geo[ssdindex][csiindex].GetYaxis()->GetXmax();
      latex_ratio_ave[ssdindex][csiindex] = new TLatex(x1, y1, Form("#bar{r_{lost}} = %.4f%%",lost_ratio_ave[ssdindex][csiindex]));
      latex_ratio_ave[ssdindex][csiindex]->SetTextSize(0.05);
      latex_ratio_ave[ssdindex][csiindex]->SetTextColor(kRed);

      for (Int_t iratio=0; iratio<nratio; iratio++) {
        latex_ratio[ssdindex][csiindex][iratio] = new TLatex(x2, y2, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex][csiindex][iratio]));
        latex_ratio[ssdindex][csiindex][iratio]->SetTextSize(0.05);
        latex_ratio[ssdindex][csiindex][iratio]->SetTextColor(kRed);
      }
    }
  }

  // 画图
  TCanvas* c_begin = new TCanvas("c_begin","c_begin",600,600);
  TCanvas* c_end   = new TCanvas("c_end","c_end",600,600);
  TCanvas* cans_L2L3 = new TCanvas("cans_L2L3","cans_L2L3",1500,1000);
  cans_L2L3->Divide(3,2);

  c_begin->Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      cans_L2L3->cd(1);
      h2_L2L3_Geo[ssdindex][csiindex].Draw("COL");
      h2_L2L3_Geo[ssdindex][csiindex].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo[ssdindex][csiindex].GetYaxis()->SetNdivisions(506);

      cans_L2L3->cd(2);
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][0].Draw("COL");
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][0].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][0].GetYaxis()->SetNdivisions(506);
      latex_ratio_ave[ssdindex][csiindex]->Draw("SAME");
      latex_ratio[ssdindex][csiindex][0]->Draw("SAME");

      cans_L2L3->cd(3);
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][1].Draw("COL");
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][1].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][1].GetYaxis()->SetNdivisions(506);
      latex_ratio_ave[ssdindex][csiindex]->Draw("SAME");
      latex_ratio[ssdindex][csiindex][1]->Draw("SAME");

      cans_L2L3->cd(4);
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][2].Draw("COL");
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][2].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][2].GetYaxis()->SetNdivisions(506);
      latex_ratio_ave[ssdindex][csiindex]->Draw("SAME");
      latex_ratio[ssdindex][csiindex][2]->Draw("SAME");

      cans_L2L3->cd(5);
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][3].Draw("COL");
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][3].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][3].GetYaxis()->SetNdivisions(506);
      latex_ratio_ave[ssdindex][csiindex]->Draw("SAME");
      latex_ratio[ssdindex][csiindex][3]->Draw("SAME");

      cans_L2L3->cd(6);
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][4].Draw("COL");
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][4].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL2BL2F_Discard[ssdindex][csiindex][4].GetYaxis()->SetNdivisions(506);
      latex_ratio_ave[ssdindex][csiindex]->Draw("SAME");
      latex_ratio[ssdindex][csiindex][4]->Draw("SAME");

      gPad->Modified();
      gPad->Update();

      cans_L2L3->Print(pathPDFOut.c_str());
    }
  }
  c_end->Print(pathPDFOutEnd.c_str());

}


//______________________________________________________________________________
// 考察 EneL1SL2F 能量约束条件对 "丢弃"事件的影响
void DecodefGlobalMulti1::GlobalMulti_L2L3_EneL1SL2F_Discard()
{
  Int_t globalmulti = 1;
  Int_t ssdnum, csinum, bananacut_telnum;
  Double_t el1s, el2f, el2b, ecsi;
  Int_t MAXPARTICLES = 20;

  Int_t nratio = 5;
  Double_t EL1SL2FRatio[5] = {0.50,0.60,0.70,0.80,0.90};
  Double_t lost_ratio[NUM_SSD][NUM_CSI][5] = {{{0}}};
  Double_t lost_ratio_ave[NUM_SSD][NUM_CSI] = {{0}};
  Double_t lost_ratio_sum[NUM_SSD][NUM_CSI] = {{0}};
  Double_t lost_ratio_csi_sum[NUM_CSI][5] = {{0}};
  Double_t lost_ratio_csi_ave[NUM_CSI][5] = {{0}};

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_EneL1SL2F_GlobalMulti%d_Discard.pdf",PATHFIGURESFOLDER,globalmulti));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_EneL1SL2F_GlobalMulti%d_Discard.pdf[",PATHFIGURESFOLDER,globalmulti));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_EneL1SL2F_GlobalMulti%d_Discard.pdf]",PATHFIGURESFOLDER,globalmulti));

  TH2F h2_L2L3_Geo[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_Geo_EneL1SL2F_Discard[NUM_SSD][NUM_CSI][5];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h2_L2L3_Geo[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_Geo",ssdindex+1,csiindex),Form("SSD%d_CsI%d_Geo",ssdindex+1,csiindex),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      for (Int_t iratio=0; iratio<nratio; iratio++) {
          h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][iratio] = TH2F(Form("SSD%d_CsI%d_Geo_EneL1SL2F%.2f_Discard",ssdindex+1,csiindex,EL1SL2FRatio[iratio]),
            Form("SSD%d_CsI%d_Geo_EneL1SL2F%.2f_Discard",ssdindex+1,csiindex,EL1SL2FRatio[iratio]),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      }
    }
  }

//  trackreconstruct->fChainTrackTree->SetEntries(1000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L2L3_SSDNum [ssdindex].clear();
      L2L3_CsINum [ssdindex].clear();
      L2L3_CSIECh [ssdindex].clear();
      L2L3_L2BEMeV[ssdindex].clear();
      L2L3_L2FEMeV[ssdindex].clear();
      L2L3_L1SEMeV[ssdindex].clear();
      BananaCut_TelNum[ssdindex].clear();

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
            L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
            L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
            L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
            BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
          }
        }
      }

      // 填充数据
      if (L2L3_CsINum[ssdindex].size() == globalmulti) {
        h2_L2L3_Geo[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
        // 不满足 L1S_L2F 能量关联
        for (Int_t iratio=0; iratio<nratio; iratio++) {
          if (!(trackreconstruct->IsEneConstraint_L1S_L2F(L2L3_L1SEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0], EL1SL2FRatio[iratio]))) {
            h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][L2L3_CsINum[ssdindex][0]][iratio].Fill(L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]);
          }
        }
      }
    }
  }

  // 计算“丢失”事件的比例
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      for (Int_t iratio=0; iratio<nratio; iratio++) {
        lost_ratio[ssdindex][csiindex][iratio] = (Double_t) h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][iratio].GetEntries()/h2_L2L3_Geo[ssdindex][csiindex].GetEntries()*100;
        lost_ratio_sum[ssdindex][csiindex] += lost_ratio[ssdindex][csiindex][iratio];
      }
      lost_ratio_ave[ssdindex][csiindex] = lost_ratio_sum[ssdindex][csiindex]/nratio;
    }
  }

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t iratio=0; iratio<nratio; iratio++) {
      for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
        lost_ratio_csi_sum[ssdindex][iratio] += lost_ratio[ssdindex][csiindex][iratio];
      }
      lost_ratio_csi_ave[ssdindex][iratio] = lost_ratio_csi_sum[ssdindex][iratio]/NUM_CSI;
      cout<<Form("SSD%d_L1L2_Ratio%.2f = %.4f",ssdindex+1,EL1SL2FRatio[iratio],lost_ratio_csi_ave[ssdindex][iratio])<<endl;
    }
  }

  // 定义 latex
  TLatex* latex_ratio[NUM_SSD][NUM_CSI][nratio];
  TLatex* latex_ratio_ave[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      Double_t x1 = 0.3 * h2_L2L3_Geo[ssdindex][csiindex].GetXaxis()->GetXmax();
      Double_t y1 = 0.9 * h2_L2L3_Geo[ssdindex][csiindex].GetYaxis()->GetXmax();
      Double_t x2 = 0.3 * h2_L2L3_Geo[ssdindex][csiindex].GetXaxis()->GetXmax();
      Double_t y2 = 0.8 * h2_L2L3_Geo[ssdindex][csiindex].GetYaxis()->GetXmax();
      latex_ratio_ave[ssdindex][csiindex] = new TLatex(x1, y1, Form("#bar{r_{lost}} = %.4f%%",lost_ratio_ave[ssdindex][csiindex]));
      latex_ratio_ave[ssdindex][csiindex]->SetTextSize(0.05);
      latex_ratio_ave[ssdindex][csiindex]->SetTextColor(kRed);

      for (Int_t iratio=0; iratio<nratio; iratio++) {
        latex_ratio[ssdindex][csiindex][iratio] = new TLatex(x2, y2, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex][csiindex][iratio]));
        latex_ratio[ssdindex][csiindex][iratio]->SetTextSize(0.05);
        latex_ratio[ssdindex][csiindex][iratio]->SetTextColor(kRed);
      }
    }
  }

  // 画图
  TCanvas* c_begin = new TCanvas("c_begin","c_begin",600,600);
  TCanvas* c_end   = new TCanvas("c_end","c_end",600,600);
  TCanvas* cans_L2L3 = new TCanvas("cans_L2L3","cans_L2L3",1500,1000);
  cans_L2L3->Divide(3,2);

  c_begin->Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      cans_L2L3->cd(1);
      h2_L2L3_Geo[ssdindex][csiindex].Draw("COL");
      h2_L2L3_Geo[ssdindex][csiindex].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo[ssdindex][csiindex].GetYaxis()->SetNdivisions(506);

      cans_L2L3->cd(2);
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][0].Draw("COL");
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][0].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][0].GetYaxis()->SetNdivisions(506);
      latex_ratio_ave[ssdindex][csiindex]->Draw("SAME");
      latex_ratio[ssdindex][csiindex][0]->Draw("SAME");

      cans_L2L3->cd(3);
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][1].Draw("COL");
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][1].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][1].GetYaxis()->SetNdivisions(506);
      latex_ratio_ave[ssdindex][csiindex]->Draw("SAME");
      latex_ratio[ssdindex][csiindex][1]->Draw("SAME");

      cans_L2L3->cd(4);
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][2].Draw("COL");
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][2].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][2].GetYaxis()->SetNdivisions(506);
      latex_ratio_ave[ssdindex][csiindex]->Draw("SAME");
      latex_ratio[ssdindex][csiindex][2]->Draw("SAME");

      cans_L2L3->cd(5);
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][3].Draw("COL");
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][3].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][3].GetYaxis()->SetNdivisions(506);
      latex_ratio_ave[ssdindex][csiindex]->Draw("SAME");
      latex_ratio[ssdindex][csiindex][3]->Draw("SAME");

      cans_L2L3->cd(6);
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][4].Draw("COL");
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][4].GetXaxis()->SetNdivisions(506);
      h2_L2L3_Geo_EneL1SL2F_Discard[ssdindex][csiindex][4].GetYaxis()->SetNdivisions(506);
      latex_ratio_ave[ssdindex][csiindex]->Draw("SAME");
      latex_ratio[ssdindex][csiindex][4]->Draw("SAME");

      gPad->Modified();
      gPad->Update();

      cans_L2L3->Print(pathPDFOut.c_str());
    }
  }
  c_end->Print(pathPDFOutEnd.c_str());
}

//******************************************************************************



//******************************************************************************
//                        fGlobalMulti = 2
//                    ------------------------
// 解码 fGlobalMulit = 2的事件
DecodefGlobalMulti2::DecodefGlobalMulti2(Int_t firstrun, Int_t lastrun)
{
  fFirstRun = firstrun;
  fLastRun = lastrun;

  trackreconstruct = new CSHINETrackReconstruction(fFirstRun, fLastRun);

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
        bananacut[numtel].push_back(cut);
      //  cout<<cut->GetName()<<endl;
      }
    }
  }
}

DecodefGlobalMulti2::~DecodefGlobalMulti2()
{}
//______________________________________________________________________________


//______________________________________________________________________________
void DecodefGlobalMulti2::GlobalMulti2_Checks()
{
  gROOT->cd();

  Int_t gmulti = 2;
  const char* gmultitag = "fGlobalMulti2";
  trackreconstruct->L2L3_CheckParallelDraw(gmultitag, gmulti);
  trackreconstruct->L2L3_CheckL2L3DEEPlot (gmultitag, gmulti);
  trackreconstruct->L2L3_CheckL1L2DEEPlot (gmultitag, gmulti);
}

//______________________________________________________________________________
// 计算不同模式所占比例
void DecodefGlobalMulti2::GlobalMulti2_CalcModeRatio()
{
  std::string pidtag("L2L3");
  std::string pathModeRatioOut(Form("%sdata_Test_Multi/%s_Test_Multi_fGlobalMulti2_ModeRatio.dat",PATHDATAFOLDER,pidtag.c_str()));
  ofstream FileOut(pathModeRatioOut.c_str());
  FileOut<<"* Ratio of different modes of events with globalmulti=2"<<endl;

  Int_t mode_count[16][4] = {{0}};
  Double_t ratio[16][4] = {{0.}};
  Double_t ratio_sum[4] = {0.};
  string mode[16] = { "1111","1110","1101","1100","1011","1010","1001","1000",
                      "0111","0110","0101","0100","0011","0010","0001","0000" };

  for (Int_t modeindex=0; modeindex<16; modeindex++) {
    GlobalMulti2_DataClassification(pidtag, mode[modeindex],ratio[modeindex], mode_count[modeindex]);
  }

  TH1I* h_modecount[4];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h_modecount[ssdindex] = new TH1I(Form("SSD%d",ssdindex+1),Form("SSD%d",ssdindex+1),16,-0.5,15.5);
    for (Int_t modeindex=0; modeindex<16; modeindex++) {
      h_modecount[ssdindex]->SetBinContent(16-modeindex, mode_count[modeindex][ssdindex]);
    }
  }

  for (Int_t ssdindex=0; ssdindex<4; ssdindex++) {
    cout<<endl;
    for (Int_t modeindex=0; modeindex<16; modeindex++) {
      ratio_sum[ssdindex] += ratio[modeindex][ssdindex];
      cout<<"SSD"<<ssdindex+1<<setw(10)<<"mode = "<<mode[modeindex].c_str()<<setw(10)<<setprecision(6)<<"ratio = "<<ratio[modeindex][ssdindex]<<endl;

      FileOut<<"SSD"<<ssdindex+1<<setw(10)<<"mode = "<<mode[modeindex].c_str()<<setw(10)<<setprecision(6)<<"ratio = "<<ratio[modeindex][ssdindex]<<endl;
    }
    cout<<"SSD"<<ssdindex+1<<setw(15)<<"ratio_sum = "<<ratio_sum[ssdindex]<<endl;
    FileOut<<"SSD"<<ssdindex+1<<setw(15)<<"ratio_sum = "<<ratio_sum[ssdindex]<<endl;
    cout<<endl;
  }

  // 画图
  TCanvas* cans = new TCanvas("cans", "cans", 1000, 1000);
  cans->Divide(2,2);
  cans->cd(1);
  h_modecount[0]->Draw();
  h_modecount[0]->GetXaxis()->SetNdivisions(116);
  cans->cd(2);
  h_modecount[1]->Draw();
  h_modecount[1]->GetXaxis()->SetNdivisions(116);
  cans->cd(3);
  h_modecount[2]->Draw();
  h_modecount[2]->GetXaxis()->SetNdivisions(116);
  cans->cd(4);
  h_modecount[3]->Draw();
  h_modecount[3]->GetXaxis()->SetNdivisions(116);
  cans->Print(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti2_ModeStatistic.png", PATHFIGURESFOLDER));
}


//______________________________________________________________________________
// 用一个四位数字符串 index 来区分不同的模式
// 4个数从左往右,分别表示 L3A,L2B,L2F,L1S 的情况
// 每个数取值为 0 或 1: 0 表示该层径迹点为 1， 1 表示该层径迹点为 2.
// index 的形式如: 1011, 0101
void DecodefGlobalMulti2::GlobalMulti2_DataClassification(std::string pidtag, std::string mode, Double_t ratio[4], Int_t mode_count[4])
{
  const Int_t globalmulti = 2;

  gROOT->cd();
//  gStyle->SetOptStat(kFALSE);

  Int_t nlines_globalmulti2[4] = {0};
  Int_t nlines_classifi    [4] = {0};
  Int_t gssdnum = -1;

  Int_t L3A_index, L2B_index, L2F_index, L1S_index;
  ChangeModeToNumber(mode, L3A_index, L2B_index, L2F_index, L1S_index);
  cout<<L3A_index<<setw(10)<<L2B_index<<setw(10)<<L2F_index<<setw(10)<<L1S_index<<endl;

  std::string pathDataOutput[NUM_SSD];
  ofstream FileOut[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    mode_count[ssdindex] = 0;
    pathDataOutput[ssdindex] = Form("%sdata_Test_Multi/globalmulti%d/SSD%d_%s_Test_Multi_fGlobalMulti%d_%s.dat", PATHDATAFOLDER, globalmulti, ssdindex+1, pidtag.c_str(), globalmulti, mode.c_str());
    FileOut[ssdindex].open(pathDataOutput[ssdindex].c_str());
    FileOut[ssdindex]<<setw(10)<<"# EntryNo."<<setw(10)<<"MultiGlobal"<<setw(8)<<"CSI"<<setw(10)<<"L2B"
                     <<setw(10)<<"L2F"<<setw(10)<<"L1S"<<setw(10)<<"EL1S"
                     <<setw(10)<<"EL2F"<<setw(10)<<"EL2B"<<setw(10)<<"ECsI"
                     <<endl;
  }

  trackreconstruct->fChainTrackTree->SetEntries(5000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<"Processing mode = "<<mode.c_str()<<"......"<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    // fill data
    if (trackreconstruct->fTrackEvent.fGlobalMulti > 0) {
      for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
        gssdnum = trackreconstruct->fTrackEvent.fGSSDNum[gmulti];
        L2L3_CSIECh [gssdnum].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
        L2L3_L2BEMeV[gssdnum].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
        L2L3_L2FEMeV[gssdnum].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
        L2L3_L1SEMeV[gssdnum].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
        L2L3_CsINum[gssdnum].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
        L2L3_L2BStripNum[gssdnum].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
        L2L3_L2FStripNum[gssdnum].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
        L2L3_L1SStripNum[gssdnum].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
      }
    }
    // save data
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        nlines_globalmulti2[ssdindex]++;
        if (IsModeMatched(L3A_index, L2L3_CsINum[ssdindex][0],      L2L3_CsINum[ssdindex][1])      &&
            IsModeMatched(L2B_index, L2L3_L2BStripNum[ssdindex][0], L2L3_L2BStripNum[ssdindex][1]) &&
            IsModeMatched(L2F_index, L2L3_L2FStripNum[ssdindex][0], L2L3_L2FStripNum[ssdindex][1]) &&
            IsModeMatched(L1S_index, L2L3_L1SStripNum[ssdindex][0], L2L3_L1SStripNum[ssdindex][1]))
        {
          mode_count[ssdindex]++;
          nlines_classifi[ssdindex]++;
          for (Int_t ssdgmulti=0; ssdgmulti<globalmulti; ssdgmulti++) {
            FileOut[ssdindex]<<setw(7)<<ientry
                             <<setw(10)<<ssdgmulti+1
                             <<setw(12)<<L2L3_CsINum[ssdindex][ssdgmulti]
                             <<setw(10)<<L2L3_L2BStripNum[ssdindex][ssdgmulti]
                             <<setw(10)<<L2L3_L2FStripNum[ssdindex][ssdgmulti]
                             <<setw(10)<<L2L3_L1SStripNum[ssdindex][ssdgmulti]
                             <<setw(13)<<L2L3_L1SEMeV[ssdindex][ssdgmulti]
                             <<setw(10)<<L2L3_L2FEMeV[ssdindex][ssdgmulti]
                             <<setw(10)<<L2L3_L2BEMeV[ssdindex][ssdgmulti]
                             <<setw(10)<<L2L3_CSIECh[ssdindex][ssdgmulti]
                             <<endl;
          }
        }
      }
      // clear the vectors
      L2L3_CSIECh [ssdindex].clear();
      L2L3_L2BEMeV[ssdindex].clear();
      L2L3_L2FEMeV[ssdindex].clear();
      L2L3_L1SEMeV[ssdindex].clear();
      L2L3_CsINum[ssdindex].clear();
      L2L3_L2BStripNum[ssdindex].clear();
      L2L3_L2FStripNum[ssdindex].clear();
      L2L3_L1SStripNum[ssdindex].clear();
    }
  }

  // 计算每种 globalmulti = 2 情况下每种模式的比例
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    Double_t per = (Double_t) nlines_classifi[ssdindex]/nlines_globalmulti2[ssdindex];
    ratio[ssdindex] = per;
    if (nlines_classifi[ssdindex]==0) system(Form("rm %s",pathDataOutput[ssdindex].c_str()));  //删除空文件
  }
}


//______________________________________________________________________________
// Decode 策略:
// 直接判断
void DecodefGlobalMulti2::GlobalMulti2_Decode_1111()
{
  const Int_t globalmulti = 2;
  const char* mode = "1111";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Int_t charge, mass;
  std::string cutname;

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 3, -0.5, 2.5);
    }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

      h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    }
  }

  trackreconstruct->fChainTrackTree->SetEntries(2000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    // fill data
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L2L3_SSDNum [ssdindex].clear();
      L2L3_CsINum [ssdindex].clear();
      L2L3_CSIECh [ssdindex].clear();
      L2L3_L2BEMeV[ssdindex].clear();
      L2L3_L2FEMeV[ssdindex].clear();
      L2L3_L1SEMeV[ssdindex].clear();
      L2L3_CsINum[ssdindex].clear();
      L2L3_L2BStripNum[ssdindex].clear();
      L2L3_L2FStripNum[ssdindex].clear();
      L2L3_L1SStripNum[ssdindex].clear();
      BananaCut_TelNum[ssdindex].clear();

      // 初始化
      candimulti_sum[ssdindex] = 0;
      for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
            L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
            L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
            L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
            L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
            BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
          }
        }
      }
      // 填充数据
      if (L2L3_CsINum[ssdindex].size() == globalmulti) {
        ExpDataToMode(mode_exp,L2L3_CsINum[ssdindex].data(),L2L3_L2BStripNum[ssdindex].data(),L2L3_L2FStripNum[ssdindex].data(),L2L3_L1SStripNum[ssdindex].data());
        if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
            h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

            // 填充 mode statistic
            bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
            h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
            //__________________________________________________________________
            //        以下部分为当前 L2L3-globalomulti2-1111 模式的径迹重建策略！！！
            //       ---------------------------------------------------------
            if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname,L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])  &&
                trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
            {
              candimulti_sum[ssdindex]++;
              candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
              //
              h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              // L2L3_DEE 直接填充
              h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
              // 利用 LISE++ 反推 L1S_ELoss
              trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
              if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
                L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                if (L2F_Einc > Condition_L2FEincCut) {
                  L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
                }
              }
            }
            else { // 不满足 BananaCut 或 不满足 EL2B_EL2F
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
            }
          }
          //____________________________________________________________________
          // 填充每块 CsI 的 candidate multi
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
            h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
          }
          // 填充每一个 SSD 的 candidate multi
          h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
        }
      }
    }
  }
  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
      else {
        entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
        entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
      }
    }
    lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * globalmulti;
    y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      //
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
      //
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
    }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      cans.cd(1);
      h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
      if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
        for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
        }
      }

      cans.cd(2);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(3);
      h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(4);
      h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
      h1_modestat[ssdindex][csiindex]->SetLineWidth(2);
      h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
      h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
      h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
      //
      legend_SubMode[ssdindex][csiindex]->Draw("SAME");

      cans.cd(5);
      h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
      cans.cd(6);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(7);
      h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(8);
      // candimulti_sum
      h1_candimulti_sum[ssdindex]->Draw();
      h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
      h1_candimulti_sum[ssdindex]->SetLineWidth(2);
      h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(103);
      h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
      // candimulti_merged
      h1_candimulti_merged[ssdindex]->Draw("SAME");
      h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
      h1_candimulti_merged[ssdindex]->SetLineWidth(2);
      // candimulti
      h1_candimulti[ssdindex][csiindex]->Draw("SAME");
      h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
      h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
      // latex_ratio
      latex_ratio[ssdindex]->Draw();
      //
      legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

      gPad->Modified();
      gPad->Update();

      cans.Print(pathPDFOut.c_str());
    }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
// Decode 策略:
//  case 1) candimulti = 1 : 直接判选
//  case 2) candimulti = 0 : 考虑 L2F 双击
//  case 3) candimulti = 2 : 舍弃
void DecodefGlobalMulti2::GlobalMulti2_Decode_1101()
{
  const Int_t globalmulti = 2;
  const char* mode = "1101";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];

  Double_t EL2B_Sum;

  Int_t charge, mass;
  std::string cutname;

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_L2FDoubleHit[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_L2FDoubleHit[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_L2FDoubleHit_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_L2FDoubleHit_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 3, -0.5, 2.5);
    }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

      h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    }
  }

  trackreconstruct->fChainTrackTree->SetEntries(2000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    // fill data
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L2L3_SSDNum [ssdindex].clear();
      L2L3_CsINum [ssdindex].clear();
      L2L3_CSIECh [ssdindex].clear();
      L2L3_L2BEMeV[ssdindex].clear();
      L2L3_L2FEMeV[ssdindex].clear();
      L2L3_L1SEMeV[ssdindex].clear();
      L2L3_CsINum[ssdindex].clear();
      L2L3_L2BStripNum[ssdindex].clear();
      L2L3_L2FStripNum[ssdindex].clear();
      L2L3_L1SStripNum[ssdindex].clear();
      BananaCut_TelNum[ssdindex].clear();

      candimulti_sum[ssdindex] = 0;
      for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }
      for (Int_t imulti=0; imulti<globalmulti; imulti++) { bin_index[imulti] = -1; }

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
            L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
            L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
            L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
            L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
            BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
          }
        }
      }
      // 填充数据
      if (L2L3_CsINum[ssdindex].size() == globalmulti) {
        ExpDataToMode(mode_exp,L2L3_CsINum[ssdindex].data(),L2L3_L2BStripNum[ssdindex].data(),L2L3_L2FStripNum[ssdindex].data(),L2L3_L1SStripNum[ssdindex].data());
        if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
            h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

            // 填充 mode statistic
            bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
            h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);

            if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
            {
              candimulti_sum[ssdindex]++;
              candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
            }
          }
          //____________________________________________________________________
          //           以下部分为当前 L2L3-globalomulti2-1101 模式的径迹重建策略！！！
          //          -------------------------------------------------------
          // 存在 1 个“理想事件“的的情况: 直接判选！
          if (candimulti_sum[ssdindex] == 1) {
            for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              //
              h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              //
              if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname,L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
              {
                h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);

                trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
                   L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  if (L2F_Einc > Condition_L2FEincCut) {
                    L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                    if (trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][itrack],L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
                    }
                    else {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
                    }
                  }
                }
              }
            }
          }
          if (candimulti_sum[ssdindex] == 0) {
            EL2B_Sum = L2L3_L2BEMeV[ssdindex][0] + L2L3_L2BEMeV[ssdindex][1];
            // 考虑 L2F 双击, 同时至少有一条径迹满足 BananaCut
            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum,L2L3_L2FEMeV[ssdindex][0]) &&
                (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],cutname,L2L3_CSIECh[ssdindex][0],L2L3_L2BEMeV[ssdindex][0]) ||
                 trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][1]],cutname,L2L3_CSIECh[ssdindex][1],L2L3_L2BEMeV[ssdindex][1]))) {
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                //
                h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                h1_L2FDoubleHit[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                //
                if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname,L2L3_CSIECh[ssdindex][itrack],L2L3_L2BEMeV[ssdindex][itrack])) {
                  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2BEMeV[ssdindex][itrack]);
                  // 利用 LISE++ 反推 L1S_ELoss
                  trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                  if (L2L3_L2BEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
                    L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2BEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                    if (L2F_Einc > Condition_L2FEincCut) {
                      L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2BEMeV[ssdindex][itrack],L1S_ELoss);
                    }
                  }
                }
              }
            }
            else { // 两条径迹都不满足 BananaCut
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
                h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
              }
            }
          }
          // 舍弃存在 2 个“理想事件“的的情况：舍弃
          if (candimulti_sum[ssdindex] == 2) {
            for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
            }
          }
          //____________________________________________________________________
          // 填充每块 CsI 的 candidate multi
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
            h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
          }
          // 填充每一个 SSD 的 candidate multi
          h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
        }
      }
    }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
      else {
        entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
        entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
      }
    }
    lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.3 * globalmulti;
    y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      //
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_L2FDoubleHit[ssdindex][csiindex],"2hit/sharing","l");
      //
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
    }
  }


  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      cans.cd(1);
      h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
      if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
        for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
        }
      }

      cans.cd(2);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(3);
      h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(4);
      h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
      h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
      h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
      h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
      // candidatemulri sharing
      h1_L2FDoubleHit[ssdindex][csiindex]->Draw("SAME");
      h1_L2FDoubleHit[ssdindex][csiindex]->SetLineColor(kGreen-3);
      h1_L2FDoubleHit[ssdindex][csiindex]->SetLineWidth(2);
      //
      legend_SubMode[ssdindex][csiindex]->Draw("SAME");


      cans.cd(5);
      h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
      cans.cd(6);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(7);
      h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(8);
      // candimulti_sum
      h1_candimulti_sum[ssdindex]->Draw();
      h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
      h1_candimulti_sum[ssdindex]->SetLineWidth(2);
      h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(103);
      h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
      // candimulti_merged
      h1_candimulti_merged[ssdindex]->Draw("SAME");
      h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
      h1_candimulti_merged[ssdindex]->SetLineWidth(2);
      // candimulti
      h1_candimulti[ssdindex][csiindex]->Draw("SAME");
      h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
      h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
      // latex_ratio
      latex_ratio[ssdindex]->Draw();
      //
      legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

      gPad->Modified();
      gPad->Update();

      cans.Print(pathPDFOut.c_str());
    }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
// Decode 策略:
//  case 1) candimulti = 1   : 直接判选
//  case 2) candimulti = 0/2 : 舍弃
void DecodefGlobalMulti2::GlobalMulti2_Decode_1010()
{
  const Int_t globalmulti = 2;
  const char* mode = "1010";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];

  Double_t EL2F_Sum;

  std::string cutname;
  Int_t charge, mass;

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_L2FSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_L2FSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_L2FSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_L2FSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 3, -0.5, 2.5);
    }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

      h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    }
  }

  trackreconstruct->fChainTrackTree->SetEntries(2000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    // fill data
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L2L3_SSDNum [ssdindex].clear();
      L2L3_CsINum [ssdindex].clear();
      L2L3_CSIECh [ssdindex].clear();
      L2L3_L2BEMeV[ssdindex].clear();
      L2L3_L2FEMeV[ssdindex].clear();
      L2L3_L1SEMeV[ssdindex].clear();
      L2L3_CsINum[ssdindex].clear();
      L2L3_L2BStripNum[ssdindex].clear();
      L2L3_L2FStripNum[ssdindex].clear();
      L2L3_L1SStripNum[ssdindex].clear();
      BananaCut_TelNum[ssdindex].clear();

      candimulti_sum[ssdindex] = 0;
      for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
            L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
            L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
            L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
            L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
            BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
          }
        }
      }
      // 填充数据
      if (L2L3_CsINum[ssdindex].size() == globalmulti) {
        ExpDataToMode(mode_exp,L2L3_CsINum[ssdindex].data(),L2L3_L2BStripNum[ssdindex].data(),L2L3_L2FStripNum[ssdindex].data(),L2L3_L1SStripNum[ssdindex].data());
        if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
            h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

            // 填充 mode statistic
            bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
            h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);

            if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
            {
              candimulti_sum[ssdindex]++;
              candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
            }
          }
          //____________________________________________________________________
          //        以下部分为当前 L2L3-globalomulti2-1010 模式的径迹重建策略！！！
          //      -----------------------------------------------------------
          // 对于存在 1 个“理想事件“的的情况 : 直接判选
          if (candimulti_sum[ssdindex] == 1) {
            for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              //
              h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              //
              if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname,L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
              {
                h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);

                trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
                   L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  if (L2F_Einc > Condition_L2FEincCut) {
                    L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                    if (trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][itrack],L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
                    }
                    else {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
                    }
                  }
                }
              }
            }
          }
          // 存在 0 或 2 个“理想事件“的的情况 : 进一步分析
          if (candimulti_sum[ssdindex]==0 || candimulti_sum[ssdindex]==2) {
            EL2F_Sum = L2L3_L2FEMeV[ssdindex][0] + L2L3_L2FEMeV[ssdindex][1];
            // 满足 L2B 双击情况，且至少有一条径迹满足 BananaCut
            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][0],EL2F_Sum) &&
                (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],cutname,L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]) ||
                 trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][1]],cutname,L2L3_CSIECh[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]))) {
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                //
                h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                h1_L2FSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                //
                if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname,L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])) {
                  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
                  // 利用 LISE++ 反推 L1S_ELoss
                  trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                  if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
                    L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                    if (L2F_Einc > Condition_L2FEincCut) {
                      L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
                    }
                  }
                }
              }
            }
            // 其中一条径迹满足 EL2B_EL2F
            else if (!trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][0],EL2F_Sum) &&
                      trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]) &&
                      trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],cutname,L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0])) {
              //
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              }
              //
              h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]);
              // 利用 LISE++ 反推 L1S_ELoss
              trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
              if (L2L3_L2FEMeV[ssdindex][0]>Condition_L2FElossCut) {
                L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][0],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                if (L2F_Einc > Condition_L2FEincCut) {
                  L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L1S_ELoss);
                }
              }
            }
            // 其中一条径迹满足 EL2B_EL2F
            else if (!trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][0],EL2F_Sum) &&
                      trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]) &&
                      trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][1]],cutname,L2L3_CSIECh[ssdindex][1],L2L3_L2FEMeV[ssdindex][1])) {
              //
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              }
              //
              h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_CSIECh[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]);
              // 利用 LISE++ 反推 L1S_ELoss
              trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
              if (L2L3_L2FEMeV[ssdindex][1]>Condition_L2FElossCut) {
                L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][1],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                if (L2F_Einc > Condition_L2FEincCut) {
                  L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_L2FEMeV[ssdindex][1],L1S_ELoss);
                }
              }
            }
            else { // 其他情况, 舍弃
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
                h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
              }
            }
          }
          //____________________________________________________________________
          // 填充每块 CsI 的 candidate multi
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
            h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
          }
          // 填充每一个 SSD 的 candidate multi
          h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
        }
      }
    }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
      else {
        entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
        entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
      }
    }
    lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.3 * globalmulti;
    y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      //
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_L2FSharing[ssdindex][csiindex],"2hit","l");
      //
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
    }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      cans.cd(1);
      h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
      if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
        for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
        }
      }

      cans.cd(2);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(3);
      h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(4);
      h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
      h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
      h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
      h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
      // candidatemulri sharing
      h1_L2FSharing[ssdindex][csiindex]->Draw("SAME");
      h1_L2FSharing[ssdindex][csiindex]->SetLineColor(kGreen-3);
      h1_L2FSharing[ssdindex][csiindex]->SetLineWidth(2);
      //
      legend_SubMode[ssdindex][csiindex]->Draw("SAME");

      cans.cd(5);
      h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
      cans.cd(6);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(7);
      h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(8);
      // candimulti_sum
      h1_candimulti_sum[ssdindex]->Draw();
      h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
      h1_candimulti_sum[ssdindex]->SetLineWidth(2);
      h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(103);
      h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
      // candimulti_merged
      h1_candimulti_merged[ssdindex]->Draw("SAME");
      h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
      h1_candimulti_merged[ssdindex]->SetLineWidth(2);
      // candimulti
      h1_candimulti[ssdindex][csiindex]->Draw("SAME");
      h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
      h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
      // latex_ratio
      latex_ratio[ssdindex]->Draw();
      //
      legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

      gPad->Modified();
      gPad->Update();

      cans.Print(pathPDFOut.c_str());
    }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
// Decode 策略:
//  case 1) candimulti = 1   : 直接判选
//  case 2) candimulti = 0/2 : 进一步判选
void DecodefGlobalMulti2::GlobalMulti2_Decode_1000()
{
  const Int_t globalmulti = 2;
  const char* mode = "1000";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];

  Int_t charge, mass;
  std::string cutname;

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 3, -0.5, 2.5);
    }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

      h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    }
  }

  trackreconstruct->fChainTrackTree->SetEntries(2000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    // fill data
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L2L3_SSDNum [ssdindex].clear();
      L2L3_CsINum [ssdindex].clear();
      L2L3_CSIECh [ssdindex].clear();
      L2L3_L2BEMeV[ssdindex].clear();
      L2L3_L2FEMeV[ssdindex].clear();
      L2L3_L1SEMeV[ssdindex].clear();
      L2L3_CsINum[ssdindex].clear();
      L2L3_L2BStripNum[ssdindex].clear();
      L2L3_L2FStripNum[ssdindex].clear();
      L2L3_L1SStripNum[ssdindex].clear();
      BananaCut_TelNum[ssdindex].clear();

      candimulti_sum[ssdindex] = 0;
      for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
            L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
            L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
            L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
            L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
            BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
          }
        }
      }
      // 填充数据
      if (L2L3_CsINum[ssdindex].size() == globalmulti) {
        ExpDataToMode(mode_exp,L2L3_CsINum[ssdindex].data(),L2L3_L2BStripNum[ssdindex].data(),L2L3_L2FStripNum[ssdindex].data(),L2L3_L1SStripNum[ssdindex].data());
        if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
            h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

            // 填充 mode statistic
            bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
            h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);

            if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
            {
              candimulti_sum[ssdindex]++;
              candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
            }
          }
          //____________________________________________________________________
          //          以下部分为当前 L2L3-globalomulti2-1000 模式的径迹重建策略！！！
          //         -------------------------------------------------------
          // 存在 1 个“理想事件“的的情况 : 直接判选
          if (candimulti_sum[ssdindex] == 1) {
            for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              //
              h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              //
              if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname,L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
              {
                h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);

                trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
                   L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  if (L2F_Einc > Condition_L2FEincCut) {
                    L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                    if (trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][itrack],L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
                    }
                    else {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
                    }
                  }
                }
              }
            }
          }
          // 存在 0 或 2 个“理想事件“的的情况 :
          if (candimulti_sum[ssdindex] == 0 || candimulti_sum[ssdindex] == 2) {
            // 如果只有一条径迹满足 BananaCut
            if ( trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],cutname,L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]) &&
                !trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][1]],cutname,L2L3_CSIECh[ssdindex][1],L2L3_L2FEMeV[ssdindex][1])) {
              // 如果满足 EL2B_EL2F
              if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0])) {
                //
                for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                }
                //
                h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]);

                trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                if (L2L3_L2FEMeV[ssdindex][0]>Condition_L2FElossCut) {
                   L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][0],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  if (L2F_Einc > Condition_L2FEincCut) {
                    L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L1S_ELoss);
                  }
                }
              }
              else { // 不满足 EL2B_EL2F
                for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                  h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
                  h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
                }
              }
            }
            // 如果只有另一条径迹满足 BananaCut
            else if (!trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],cutname,L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]) &&
                      trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][1]],cutname,L2L3_CSIECh[ssdindex][1],L2L3_L2FEMeV[ssdindex][1])) {
              // 如果满足 EL2B_EL2F
              if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][1], L2L3_L2FEMeV[ssdindex][1])) {
                //
                for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                }
                //
                h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_CSIECh[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]);

                trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                if (L2L3_L2FEMeV[ssdindex][1]>Condition_L2FElossCut) {
                   L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][1],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  if (L2F_Einc > Condition_L2FEincCut) {
                    L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_L2FEMeV[ssdindex][1],L1S_ELoss);
                  }
                }
              }
              else { // 不满足 EL2B_EL2F
                for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                  h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
                  h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
                }
              }
            }
            else { // 如果两条径迹都满足/都不满足 BananaCut, 则舍弃
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
                h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
              }
            }
          }
          //____________________________________________________________________
          // 填充每块 CsI 的 candidate multi
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
            h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
          }
          // 填充每一个 SSD 的 candidate multi
          h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
        }
      }
    }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
      else {
        entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
        entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
      }
    }
    lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.3 * globalmulti;
    y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      //
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
      //
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
    }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      cans.cd(1);
      h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
      if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
        for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
        }
      }

      cans.cd(2);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(3);
      h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(4);
      h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
      h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
      h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
      h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
      //
      legend_SubMode[ssdindex][csiindex]->Draw("SAME");

      cans.cd(5);
      h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
      cans.cd(6);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(7);
      h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(8);
      // candimulti_sum
      h1_candimulti_sum[ssdindex]->Draw();
      h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
      h1_candimulti_sum[ssdindex]->SetLineWidth(2);
      h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(103);
      h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
      // candimulti_merged
      h1_candimulti_merged[ssdindex]->Draw("SAME");
      h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
      h1_candimulti_merged[ssdindex]->SetLineWidth(2);
      // candimulti
      h1_candimulti[ssdindex][csiindex]->Draw("SAME");
      h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
      h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
      // latex_ratio
      latex_ratio[ssdindex]->Draw();
      //
      legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

      gPad->Modified();
      gPad->Update();

      cans.Print(pathPDFOut.c_str());
    }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
// Decode 策略:
//  case 1) candimulti = 1   : 直接判选
//  case 2) candimulti = 0/2 ：考虑 L2B sharing
void DecodefGlobalMulti2::GlobalMulti2_Decode_0101()
{
  const Int_t globalmulti = 2;
  const char* mode = "0101";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;

  Int_t mode_index;
  Int_t fgmulti, ssdnum;
  Int_t bin_index[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];

  Double_t EL2B_Sum;

  Int_t charge, mass;
  std::string cutname;

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_L2BSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_L2BSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_L2BSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_L2BSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 3, -0.5, 2.5);
    }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

      h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    }
  }

  trackreconstruct->fChainTrackTree->SetEntries(1000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    // fill data
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L2L3_SSDNum [ssdindex].clear();
      L2L3_CsINum [ssdindex].clear();
      L2L3_CSIECh [ssdindex].clear();
      L2L3_L2BEMeV[ssdindex].clear();
      L2L3_L2FEMeV[ssdindex].clear();
      L2L3_L1SEMeV[ssdindex].clear();
      L2L3_CsINum[ssdindex].clear();
      L2L3_L2BStripNum[ssdindex].clear();
      L2L3_L2FStripNum[ssdindex].clear();
      L2L3_L1SStripNum[ssdindex].clear();
      BananaCut_TelNum[ssdindex].clear();

      candimulti_sum[ssdindex] = 0;
      for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }
      for (Int_t imulti=0; imulti<globalmulti; imulti++) { bin_index[imulti] = -1; }

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
            L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
            L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
            L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
            L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
            BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
          }
        }
      }
      // 填充数据
      if (L2L3_CsINum[ssdindex].size() == globalmulti) {
        ExpDataToMode(mode_exp,L2L3_CsINum[ssdindex].data(),L2L3_L2BStripNum[ssdindex].data(),L2L3_L2FStripNum[ssdindex].data(),L2L3_L1SStripNum[ssdindex].data());
        if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
            h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

            // 填充 mode statistic
            bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
            h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);

            if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
            {
              candimulti_sum[ssdindex]++;
              candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
            }
          }
          //____________________________________________________________________
          //           以下部分为当前 L2L3-globalomulti2-0101 模式的径迹重建策略！！！
          //          -------------------------------------------------------
          // 只保留存在 1 个“理想事件“的的情况
          if (candimulti_sum[ssdindex] == 1) {
            for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              //
              h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              //
              if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname,L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
              {
                h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);

                trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
                   L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  if (L2F_Einc > Condition_L2FEincCut) {
                    L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                    if (trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][itrack],L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
                    }
                    else {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
                    }
                  }
                }
              }
            }
          }
          // 存在 0/2 个“理想事件“的的情况 :
          if (candimulti_sum[ssdindex]==0 || candimulti_sum[ssdindex]==2) {
            EL2B_Sum = L2L3_L2BEMeV[ssdindex][0] + L2L3_L2BEMeV[ssdindex][1];
            if ((TMath::Abs(L2L3_L2BStripNum[ssdindex][0]-L2L3_L2BStripNum[ssdindex][1])==1) &&
                 trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, EL2B_Sum, L2L3_L2FEMeV[ssdindex][0]) &&
                 trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],cutname,L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]))
            {
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                h1_L2BSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              }
              // L2L3_DEE 直接填充
              h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]);
              // 利用 LISE++ 反推 L1S_ELoss
              trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
              if (L2L3_L2FEMeV[ssdindex][0]>Condition_L2FElossCut) {
                L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][0],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                if (L2F_Einc > Condition_L2FEincCut) {
                  L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L1S_ELoss);
                }
              }
            }
            else if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]) &&
                     trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],cutname,L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]))
            {
              //
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              }
              // L2L3_DEE 直接填充
              h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]);
              // 利用 LISE++ 反推 L1S_ELoss
              trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
              if (L2L3_L2FEMeV[ssdindex][0]>Condition_L2FElossCut) {
                L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][0],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                if (L2F_Einc > Condition_L2FEincCut) {
                  L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L1S_ELoss);
                }
              }
            }
            else if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][1]],cutname,L2L3_CSIECh[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]) &&
                     trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]))
            {
              //
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              }
              // L2L3_DEE 直接填充
              h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_CSIECh[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]);
              // 利用 LISE++ 反推 L1S_ELoss
              trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
              if (L2L3_L2FEMeV[ssdindex][1]>Condition_L2FElossCut) {
                L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][1],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                if (L2F_Einc > Condition_L2FEincCut) {
                  L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_L2FEMeV[ssdindex][1],L1S_ELoss);
                }
              }
            }
            else {
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
                h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
              }
            }
          }


            /*
            // 满足 BananaCut
            if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],cutname,L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0])) {
              // 两条径迹都不满足 EL2B_EL2F, 考虑 L2B charge sharing
              if ((TMath::Abs(L2L3_L2BStripNum[ssdindex][0]-L2L3_L2BStripNum[ssdindex][1])==1) &&
                   trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, EL2B_Sum, L2L3_L2FEMeV[ssdindex][0]))
              {
                for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                  h1_L2BSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                }
                // L2L3_DEE 直接填充
                h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]);
                // 利用 LISE++ 反推 L1S_ELoss
                trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                if (L2L3_L2FEMeV[ssdindex][0]>Condition_L2FElossCut) {
                  L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][0],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  if (L2F_Einc > Condition_L2FEincCut) {
                    L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L1S_ELoss);
                  }
                }
              }
              // 其中一条径迹满足 EL2B_EL2F, 保留
              else if ( trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]) &&
                       !trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]))
              {
                //
                for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                }
                // L2L3_DEE 直接填充
                h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]);
                // 利用 LISE++ 反推 L1S_ELoss
                trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                if (L2L3_L2FEMeV[ssdindex][0]>Condition_L2FElossCut) {
                  L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][0],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  if (L2F_Einc > Condition_L2FEincCut) {
                    L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L1S_ELoss);
                  }
                }
              }
              // 另一条径迹满足 EL2B_EL2F, 保留
              else if (!trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]) &&
                        trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]))
              {
                //
                for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                }
                // L2L3_DEE 直接填充
                h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_CSIECh[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]);
                // 利用 LISE++ 反推 L1S_ELoss
                trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                if (L2L3_L2FEMeV[ssdindex][1]>Condition_L2FElossCut) {
                  L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][1],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  if (L2F_Einc > Condition_L2FEincCut) {
                    L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_L2FEMeV[ssdindex][1],L1S_ELoss);
                  }
                }
              }
              else { // 两条径迹都满足 EL2B_EL2F， 舍弃
                for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                  h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
                  h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
                }
              }
            }
            else { // 不满足 BananaCut, 舍弃
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              //  h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              //  h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
              }
            }
          }
          */
          //____________________________________________________________________
          // 填充每块 CsI 的 candidate multi
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
            h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
          }
          // 填充每一个 SSD 的 candidate multi
          h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
        }
      }
    }
  }
  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
      else {
        entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries()/2;
        entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries()/2;
      }
    }
    lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.3 * globalmulti;
    y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      //
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_L2BSharing[ssdindex][csiindex],"sharing","l");
      //
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
    }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      cans.cd(1);
      h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
      if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
        for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
        }
      }

      cans.cd(2);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(3);
      h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(4);
      h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
      h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
      h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
      h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
      // candidatemulri sharing
      h1_L2BSharing[ssdindex][csiindex]->Draw("SAME");
      h1_L2BSharing[ssdindex][csiindex]->SetLineColor(kGreen-3);
      h1_L2BSharing[ssdindex][csiindex]->SetLineWidth(2);
      //
      legend_SubMode[ssdindex][csiindex]->Draw("SAME");


      cans.cd(5);
      h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
      cans.cd(6);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(7);
      h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(8);
      // candimulti_sum
      h1_candimulti_sum[ssdindex]->Draw();
      h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
      h1_candimulti_sum[ssdindex]->SetLineWidth(2);
      h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(103);
      h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
      // candimulti_merged
      h1_candimulti_merged[ssdindex]->Draw("SAME");
      h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
      h1_candimulti_merged[ssdindex]->SetLineWidth(2);
      // candimulti
      h1_candimulti[ssdindex][csiindex]->Draw("SAME");
      h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
      h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
      // latex_ratio
      latex_ratio[ssdindex]->Draw();
      //
      legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

      gPad->Modified();
      gPad->Update();

      cans.Print(pathPDFOut.c_str());
    }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}



//______________________________________________________________________________
// Decode 策略:
//  case 1) candimulti = 1 : 直接判选
//  case 2) candimulti = 0 : 考虑 L2F sharing
//  case 3) candimulti = 2 : 舍弃
void DecodefGlobalMulti2::GlobalMulti2_Decode_0100()
{
  const Int_t globalmulti = 2;
  const char* mode = "0100";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;

  Int_t mode_index;
  Int_t fgmulti, ssdnum;
  Int_t bin_index[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];

  Double_t EL2B_sharing;

  Int_t charge, mass;
  std::string cutname;

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_L2BSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_L2BSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_L2BSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_L2BSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 3, -0.5, 2.5);
    }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

      h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    }
  }

  trackreconstruct->fChainTrackTree->SetEntries(2000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    // fill data
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L2L3_SSDNum [ssdindex].clear();
      L2L3_CsINum [ssdindex].clear();
      L2L3_CSIECh [ssdindex].clear();
      L2L3_L2BEMeV[ssdindex].clear();
      L2L3_L2FEMeV[ssdindex].clear();
      L2L3_L1SEMeV[ssdindex].clear();
      L2L3_CsINum[ssdindex].clear();
      L2L3_L2BStripNum[ssdindex].clear();
      L2L3_L2FStripNum[ssdindex].clear();
      L2L3_L1SStripNum[ssdindex].clear();
      BananaCut_TelNum[ssdindex].clear();

      candimulti_sum[ssdindex] = 0;
      for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }
      for (Int_t imulti=0; imulti<globalmulti; imulti++) { bin_index[imulti] = -1; }

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
            L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
            L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
            L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
            L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
            BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
          }
        }
      }
      // 填充数据
      if (L2L3_CsINum[ssdindex].size() == globalmulti) {
        ExpDataToMode(mode_exp,L2L3_CsINum[ssdindex].data(),L2L3_L2BStripNum[ssdindex].data(),L2L3_L2FStripNum[ssdindex].data(),L2L3_L1SStripNum[ssdindex].data());
        if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
            h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

            // 填充 mode statistic
            bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
            h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);

            if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
            {
              candimulti_sum[ssdindex]++;
              candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
            }
          }
          //____________________________________________________________________
          //           以下部分为当前 L2L3-globalomulti2-0100 模式的径迹重建策略！！！
          //          --------------------------------------------------------
          // 存在 1 个“理想事件“的的情况 : 直接判选
          if (candimulti_sum[ssdindex] == 1) {
            for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              //
              h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              //
              if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname,L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
              {
                h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);

                trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
                   L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  if (L2F_Einc > Condition_L2FEincCut) {
                    L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                    if (trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][itrack],L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
                    }
                    else {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
                    }
                  }
                }
              }
            }
          }
          // 存在 2 个“理想事件“的的情况 : 舍弃
          if (candimulti_sum[ssdindex] == 2) {
            for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
            }
          }
          // 存在 0 个“理想事件“的的情况 : 考虑 L2B 的 charge sharing
          if (candimulti_sum[ssdindex] == 0) {
            EL2B_sharing = L2L3_L2BEMeV[ssdindex][0] + L2L3_L2BEMeV[ssdindex][1];
            // 满足 BananaCut
            if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]], cutname, L2L3_CSIECh[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]))
            {
              //  考虑 L2B sharing
              if ((TMath::Abs(L2L3_L2BStripNum[ssdindex][0]-L2L3_L2BStripNum[ssdindex][1])==1) &&
                   trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, EL2B_sharing, L2L3_L2FEMeV[ssdindex][0]))
              {
                for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                  h1_L2BSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                }
                //
                h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]);
                // 反推 EL1S
                trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                if (L2L3_L2FEMeV[ssdindex][0]>Condition_L2FElossCut) {
                  L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][0],"Si",SIL2THICKNESS[ssdindex]*1000,1);
                  if (L2F_Einc > Condition_L2FEincCut) {
                    L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,1);
                    if (trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][0],L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L2L3_L1SEMeV[ssdindex][0]);
                    }
                    else {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L1S_ELoss);
                    }
                  }
                }
              }
              // 其中一条候选径迹满足判选条件
              else if ( trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]) &&
                       !trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][1], L2L3_L2FEMeV[ssdindex][1]))
              {
                //
                for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                }
                //
                h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]);
                // 反推 EL1S
                trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                if (L2L3_L2FEMeV[ssdindex][0]>Condition_L2FElossCut) {
                  L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][0],"Si",SIL2THICKNESS[ssdindex]*1000,1);
                  if (L2F_Einc > Condition_L2FEincCut) {
                    L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,1);
                    if (trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][0],L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L2L3_L1SEMeV[ssdindex][0]);
                    }
                    else {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L1S_ELoss);
                    }
                  }
                }
              }
              // 其中另一条候选径迹满足判选条件
              else if (!trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0]) &&
                        trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][1], L2L3_L2FEMeV[ssdindex][1]))
              {
                //
                for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                }
                //
                h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_CSIECh[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]);
                // 反推 EL1S
                trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                if (L2L3_L2FEMeV[ssdindex][1]>Condition_L2FElossCut) {
                  L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][1],"Si",SIL2THICKNESS[ssdindex]*1000,1);
                  if (L2F_Einc > Condition_L2FEincCut) {
                    L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,1);
                    if (trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][1],L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_L2FEMeV[ssdindex][1],L2L3_L1SEMeV[ssdindex][1]);
                    }
                    else {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_L2FEMeV[ssdindex][1],L1S_ELoss);
                    }
                  }
                }
              }
              else { // 两条候选径迹同时满足条件, 舍弃
                for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                  h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
                  h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
                }
              }
            }
            else { // 不满足 banana cut
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
                h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
              }
            }
          }
          //____________________________________________________________________
          // 填充每块 CsI 的 candidate multi
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
            h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
          }
          // 填充每一个 SSD 的 candidate multi
          h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
        }
      }
    }
  }
  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
      else {
        entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries()/2;
        entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries()/2;
      }
    }
    lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.3 * globalmulti;
    y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      //
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_L2BSharing[ssdindex][csiindex],"sharing","l");
      //
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
    }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      cans.cd(1);
      h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
      if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
        for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
        }
      }

      cans.cd(2);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(3);
      h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(4);
      h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
      h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
      h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
      h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
      // candidatemulri sharing
      h1_L2BSharing[ssdindex][csiindex]->Draw("SAME");
      h1_L2BSharing[ssdindex][csiindex]->SetLineColor(kGreen-3);
      h1_L2BSharing[ssdindex][csiindex]->SetLineWidth(2);
      //
      legend_SubMode[ssdindex][csiindex]->Draw("SAME");


      cans.cd(5);
      h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
      cans.cd(6);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(7);
      h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(8);
      // candimulti_sum
      h1_candimulti_sum[ssdindex]->Draw();
      h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
      h1_candimulti_sum[ssdindex]->SetLineWidth(2);
      h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(103);
      h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
      // candimulti_merged
      h1_candimulti_merged[ssdindex]->Draw("SAME");
      h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
      h1_candimulti_merged[ssdindex]->SetLineWidth(2);
      // candimulti
      h1_candimulti[ssdindex][csiindex]->Draw("SAME");
      h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
      h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
      // latex_ratio
      latex_ratio[ssdindex]->Draw();
      //
      legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

      gPad->Modified();
      gPad->Update();

      cans.Print(pathPDFOut.c_str());
    }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
// Decode 策略:
//  case 1) candimulti = 1 : 直接判选
//  case 2) candimulti = 0 : 考虑 L2F sharing
//  case 3) candimulti = 2 : 舍弃
void DecodefGlobalMulti2::GlobalMulti2_Decode_0010()
{
  const Int_t globalmulti = 2;
  const char* mode = "0010";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];

  Double_t EL2F_sharing;

  Int_t charge, mass;
  std::string cutname;

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_L2FSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_L2FSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_L2FSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_L2FSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 3, -0.5, 2.5);
    }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

      h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    }
  }

  trackreconstruct->fChainTrackTree->SetEntries(2000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    // fill data
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L2L3_SSDNum [ssdindex].clear();
      L2L3_CsINum [ssdindex].clear();
      L2L3_CSIECh [ssdindex].clear();
      L2L3_L2BEMeV[ssdindex].clear();
      L2L3_L2FEMeV[ssdindex].clear();
      L2L3_L1SEMeV[ssdindex].clear();
      L2L3_CsINum[ssdindex].clear();
      L2L3_L2BStripNum[ssdindex].clear();
      L2L3_L2FStripNum[ssdindex].clear();
      L2L3_L1SStripNum[ssdindex].clear();
      BananaCut_TelNum[ssdindex].clear();

      candimulti_sum[ssdindex] = 0;
      for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }
      for (Int_t imulti=0; imulti<globalmulti; imulti++) { bin_index[imulti] = -1; }

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
            L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
            L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
            L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
            L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
            BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
          }
        }
      }
      // 填充数据
      if (L2L3_CsINum[ssdindex].size() == globalmulti) {
        ExpDataToMode(mode_exp,L2L3_CsINum[ssdindex].data(),L2L3_L2BStripNum[ssdindex].data(),L2L3_L2FStripNum[ssdindex].data(),L2L3_L1SStripNum[ssdindex].data());
        if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
            h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

            // 填充 mode statistic
            bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
            h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);

            if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
            {
              candimulti_sum[ssdindex]++;
              candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
            }
          }
          //____________________________________________________________________
          //           以下部分为当前 L2L3-globalomulti2-0010 模式的径迹重建策略！！！
          //          --------------------------------------------------------
          // 只保留存在 1 个“理想事件“的的情况
          if (candimulti_sum[ssdindex] == 1) {
            for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              //
              h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              //
              if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname,L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
              {
                h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);

                trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
                   L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  if (L2F_Einc > Condition_L2FEincCut) {
                    L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                    if (trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][itrack],L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
                    }
                    else {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
                    }
                  }
                }
              }
            }
          }
          // 对于 0 个理想事件的情况, 考虑 charge sharing
          if (candimulti_sum[ssdindex] == 0) {
            EL2F_sharing = L2L3_L2FEMeV[ssdindex][0] + L2L3_L2FEMeV[ssdindex][1];
            // 考虑 L2F charge sharing
            if (TMath::Abs(L2L3_L2FStripNum[ssdindex][0]-L2L3_L2FStripNum[ssdindex][1])==1 &&
                trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]], cutname, L2L3_CSIECh[ssdindex][0], EL2F_sharing) &&
                trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][0], EL2F_sharing)) {
              //
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                h1_L2FSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              }
              //
              h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0],EL2F_sharing);

              trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
              if (EL2F_sharing>Condition_L2FElossCut) {
                L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,EL2F_sharing,"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                if (L2F_Einc > Condition_L2FEincCut) {
                  L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  // 如果 EL1S 计算值与测量值相差较小, 则填充测量值
                  if (trackreconstruct->IsEneConstraint_EL1S_ELISECalc(EL2F_sharing,L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(EL2F_sharing,L2L3_L1SEMeV[ssdindex][0]);
                  }
                  else { // 否则, 填充计算值
                    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(EL2F_sharing,L1S_ELoss);
                  }
                }
              }
            }
            // 其中一条候选径迹满足 Banana Cut
            else if ((trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],cutname,L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]) &&
                      trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0])) &&
                     (!trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][1], L2L3_L2FEMeV[ssdindex][1])))
            {
              //
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              }
              //
              h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]);
              // 反推 EL1S
              trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
              if (L2L3_L2FEMeV[ssdindex][0]>Condition_L2FElossCut) {
                L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][0],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                if (L2F_Einc > Condition_L2FEincCut) {
                  L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  // 如果 EL1S 计算值与测量值相差较小, 则填充测量值
                  if (trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][0],L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L2L3_L1SEMeV[ssdindex][0]);
                  }
                  else { // 否则, 填充计算值
                    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L1S_ELoss);
                  }
                }
              }
            }
            // 其中另一条候选径迹满足 Banana Cut
            else if ((!trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0])) &&
                      (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][1]],cutname,L2L3_CSIECh[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]) &&
                       trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex, L2L3_L2BEMeV[ssdindex][1], L2L3_L2FEMeV[ssdindex][1])))
            {
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              }
              //
              h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_CSIECh[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]);
              // 反推 EL1S
              trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
              if (L2L3_L2FEMeV[ssdindex][1]>Condition_L2FElossCut) {
                L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][1],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                if (L2F_Einc > Condition_L2FEincCut) {
                  L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  // 如果 EL1S 计算值与测量值相差较小, 则填充测量值
                  if (trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][1],L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_L2FEMeV[ssdindex][1],L2L3_L1SEMeV[ssdindex][1]);
                  }
                  else { // 否则, 填充计算值
                    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_L2FEMeV[ssdindex][1],L1S_ELoss);
                  }
                }
              }
            }
            // 其他情况, 舍弃
            else {
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
                h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
              }
            }
          }
          //____________________________________________________________________
          // 填充每块 CsI 的 candidate multi
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
            h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
          }
          // 填充每一个 SSD 的 candidate multi
          h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
        }
      }
    }
  }
  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
      else {
        entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries()/2;
        entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries()/2;
      }
    }
    lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.3 * globalmulti;
    y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      //
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_L2FSharing[ssdindex][csiindex],"sharing","l");
      //
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
    }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      cans.cd(1);
      h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
      if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
        for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
        }
      }

      cans.cd(2);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(3);
      h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(4);
      h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
      h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
      h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
      h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
      // candidatemulri sharing
      h1_L2FSharing[ssdindex][csiindex]->Draw("SAME");
      h1_L2FSharing[ssdindex][csiindex]->SetLineColor(kGreen-3);
      h1_L2FSharing[ssdindex][csiindex]->SetLineWidth(2);
      //
      legend_SubMode[ssdindex][csiindex]->Draw("SAME");

      cans.cd(5);
      h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
      cans.cd(6);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(7);
      h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(8);
      // candimulti_sum
      h1_candimulti_sum[ssdindex]->Draw();
      h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
      h1_candimulti_sum[ssdindex]->SetLineWidth(2);
      h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(103);
      h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
      // candimulti_merged
      h1_candimulti_merged[ssdindex]->Draw("SAME");
      h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
      h1_candimulti_merged[ssdindex]->SetLineWidth(2);
      // candimulti
      h1_candimulti[ssdindex][csiindex]->Draw("SAME");
      h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
      h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
      // latex_ratio
      latex_ratio[ssdindex]->Draw();
      //
      legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

      gPad->Modified();
      gPad->Update();

      cans.Print(pathPDFOut.c_str());
    }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
// Decode 策略:
//  case 1) candimulti = 1   : 直接判选
//  case 2) candimulti = 0/2 : 考虑 L1S sharing
void DecodefGlobalMulti2::GlobalMulti2_Decode_0001()
{
  const Int_t globalmulti = 2;
  const char* mode = "0001";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;


  Int_t mode_index;
  Int_t fgmulti, ssdnum;
  Int_t bin_index[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];

  Double_t EL1S_sharing;

  Int_t charge, mass;
  std::string cutname;

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_L1SSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 3, -0.5, 2.5);
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_L1SSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_L1SSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_L1SSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
      h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 3, -0.5, 2.5);
    }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
      h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

      h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
      h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
    }
  }

  trackreconstruct->fChainTrackTree->SetEntries(2000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    // fill data
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L2L3_SSDNum [ssdindex].clear();
      L2L3_CsINum [ssdindex].clear();
      L2L3_CSIECh [ssdindex].clear();
      L2L3_L2BEMeV[ssdindex].clear();
      L2L3_L2FEMeV[ssdindex].clear();
      L2L3_L1SEMeV[ssdindex].clear();
      L2L3_CsINum[ssdindex].clear();
      L2L3_L2BStripNum[ssdindex].clear();
      L2L3_L2FStripNum[ssdindex].clear();
      L2L3_L1SStripNum[ssdindex].clear();
      BananaCut_TelNum[ssdindex].clear();

      candimulti_sum[ssdindex] = 0;
      for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }
      for (Int_t imulti=0; imulti<globalmulti; imulti++) { bin_index[imulti] = -1; }

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
            L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
            L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
            L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
            L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
            BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
          }
        }
      }

      // 填充数据
      if (L2L3_CsINum[ssdindex].size() == globalmulti) {
        ExpDataToMode(mode_exp,L2L3_CsINum[ssdindex].data(),L2L3_L2BStripNum[ssdindex].data(),L2L3_L2FStripNum[ssdindex].data(),L2L3_L1SStripNum[ssdindex].data());
        if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
            h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

            // 填充 mode statistic
            bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
                                                                                    trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
            h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);

            if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
            {
              candimulti_sum[ssdindex]++;
              candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
            }
          }
          //____________________________________________________________________
          //           以下部分为当前 L2L3-globalomulti2-0001 模式的径迹重建策略！！！
          //          --------------------------------------------------------
          // candidatemulti = 1 : 直接判选
          if (candimulti_sum[ssdindex] == 1) {
            for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              //
              h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              //
              if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname,L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
                  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
              {
                h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);

                trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
                if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
                   L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  if (L2F_Einc > Condition_L2FEincCut) {
                    L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                    if (trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][itrack],L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
                    }
                    else {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
                    }
                  }
                }
              }
            }
          }
          // candidatemulti = 2 : 考虑 L1S charge sharing
          if (candimulti_sum[ssdindex] == 2) {
            EL1S_sharing = L2L3_L1SEMeV[ssdindex][0] + L2L3_L1SEMeV[ssdindex][1];
            //
            for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
            }
            // L2L3_DEE 直接填充
            h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]);

            // 判断粒子种类, 对于 candimulti_sum[ssdindex] == 2 的情况, IsInsideABananaCut()条件恒成立
            if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],cutname,L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0])) {
              // 利用 LISE++ 反推 L1S_ELoss
              trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
              if (L2L3_L2FEMeV[ssdindex][0]>Condition_L2FElossCut) {
                L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][0],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                if (L2F_Einc > Condition_L2FEincCut) {
                  L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  // 如果其中一个 EL1S 与 计算值 L1S_ELoss 相差较小, 则填充
                  if ( trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][0],L1S_ELoss,L1S_ECalc_EReal_ErrCut) &&
                      !trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][1],L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L1S_ELoss);
                  }
                  // 如果另外一个 EL1S 与 计算值 L1S_ELoss 相差较小, 则填充
                  else if (!trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][0],L1S_ELoss,L1S_ECalc_EReal_ErrCut) &&
                            trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][1],L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_L2FEMeV[ssdindex][1],L1S_ELoss);
                  }
                  // 如果两个 EL1S 与 计算值 L1S_ELoss 相差较小, 则使用 L1S_ELoss
                  else if (trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][0],L1S_ELoss,L1S_ECalc_EReal_ErrCut) &&
                           trackreconstruct->IsEneConstraint_EL1S_ELISECalc(L2L3_L1SEMeV[ssdindex][1],L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L1S_ELoss);
                  }
                  // 如果两个 EL1S 与 计算值 L1S_ELoss 相差较大, 则考虑 L1S charge sharing
                  else {
                    // 考虑 L1S sharing 后, 如果 EL1S_sharing 与 计算值 L1S_ELoss 相差不大, 则为有效的 sharing 事件
                    if (TMath::Abs(L2L3_L1SStripNum[ssdindex][0]-L2L3_L1SStripNum[ssdindex][1])==1 &&
                        trackreconstruct->IsEneConstraint_EL1S_ELISECalc(EL1S_sharing,L1S_ELoss,L1S_ECalc_EReal_ErrCut)) {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],EL1S_sharing);
                      //
                      for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                        h1_L1SSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
                      }
                    }
                    // 否则使用计算值 L1S_ELoss
                    else {
                      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L1S_ELoss);
                    }
                  }
                }
              }
            }
          }
          // 对于 candidatemulti = 0 的情况: 即 EL1S[0]>EL2F[0] && EL2S[1]>EL2F[1], 舍弃
          if (candimulti_sum[ssdindex] == 0) {
            // 若同时满足 IsInsideABananaCut() && IsEneConstraint_L2B_L2F()
            if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],cutname,L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]) &&
                trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]))
            {
              //
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
              }
              // L2L3_DEE 直接填充
              h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]);
              // 利用 LISE++ 反推 L1S_ELoss
              trackreconstruct->GetZAFromBananaCutName(cutname,charge,mass);
              if (L2L3_L2FEMeV[ssdindex][0]>Condition_L2FElossCut) {
                L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][0],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
                if (L2F_Einc > Condition_L2FEincCut) {
                  L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
                  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L1S_ELoss);
                }
              }
            }
            else { // 舍弃
              for (Int_t itrack=0; itrack<globalmulti; itrack++) {
                h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
                h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
              }
            }
          }
          //____________________________________________________________________
          // 填充每块 CsI 的 candidate multi
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
            h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
            h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
          }
          // 填充每一个 SSD 的 candidate multi
          h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
        }
      }
    }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
      else {
        entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries()/2;
        entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries()/2;
      }
    }
    lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.3 * globalmulti;
    y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
      //
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
      legend_SubMode[ssdindex][csiindex]->AddEntry(h1_L1SSharing[ssdindex][csiindex],"sharing","l");
      //
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
      legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
    }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
      cans.cd(1);
      h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
      if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
        for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
          bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
        }
      }

      cans.cd(2);
      h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(3);
      h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(4);
      h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
      h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
      h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
      h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
      h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
      // candidatemulri sharing
      h1_L1SSharing[ssdindex][csiindex]->Draw("SAME");
      h1_L1SSharing[ssdindex][csiindex]->SetLineColor(kGreen-3);
      h1_L1SSharing[ssdindex][csiindex]->SetLineWidth(2);
      //
      legend_SubMode[ssdindex][csiindex]->Draw("SAME");

      cans.cd(5);
      h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
      cans.cd(6);
      h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
      cans.cd(7);
      h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

      cans.cd(8);
      // candimulti_sum
      h1_candimulti_sum[ssdindex]->Draw();
      h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
      h1_candimulti_sum[ssdindex]->SetLineWidth(2);
      h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(103);
      h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
      // candimulti_merged
      h1_candimulti_merged[ssdindex]->Draw("SAME");
      h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
      h1_candimulti_merged[ssdindex]->SetLineWidth(2);
      // candimulti
      h1_candimulti[ssdindex][csiindex]->Draw("SAME");
      h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
      h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
      // latex_ratio
      latex_ratio[ssdindex]->Draw();
      //
      legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

      gPad->Modified();
      gPad->Update();

      cans.Print(pathPDFOut.c_str());
    }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}



//******************************************************************************
//                        fGlobalMulti = 3
//                    ------------------------
DecodefGlobalMulti3::DecodefGlobalMulti3(Int_t firstrun, Int_t lastrun)
{
  fFirstRun = firstrun;
  fLastRun  = lastrun;

  trackreconstruct = new CSHINETrackReconstruction(fFirstRun, fLastRun);

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
        bananacut[numtel].push_back(cut);
      //  cout<<cut->GetName()<<endl;
      }
    }
  }
}

DecodefGlobalMulti3::~DecodefGlobalMulti3()
{}

//_______________________________________________
// 对 globalmulti = 3 的事件进行综合检查
// (1)平行坐标表示, 检验层间关系是否有误
// (2)检查 fGlobalMulit = 3 事件的 DE-E plot
void DecodefGlobalMulti3::GlobalMulti3_Checks()
{
  gROOT->cd();

  Int_t gmulti = 3;
  const char* gmultitag = "fGlobalMulti3";
  trackreconstruct->L2L3_CheckParallelDraw(gmultitag, gmulti);
  trackreconstruct->L2L3_CheckL2L3DEEPlot (gmultitag, gmulti);
  trackreconstruct->L2L3_CheckL1L2DEEPlot (gmultitag, gmulti);
}
//******************************************************************************


//******************************************************************************
//                        fGlobalMulti = 4
//                    ------------------------
DecodefGlobalMulti4::DecodefGlobalMulti4(Int_t firstrun, Int_t lastrun)
{
  fFirstRun = firstrun;
  fLastRun  = lastrun;

  trackreconstruct = new CSHINETrackReconstruction(fFirstRun, fLastRun);

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
        bananacut[numtel].push_back(cut);
      //  cout<<cut->GetName()<<endl;
      }
    }
  }
}

DecodefGlobalMulti4::~DecodefGlobalMulti4()
{}

//_______________________________________________
// 对 globalmulti = 4 的事件进行综合检查
// (1)平行坐标表示, 检验层间关系是否有误
// (2)检查 fGlobalMulit = 4 事件的 DE-E plot
void DecodefGlobalMulti4::GlobalMulti4_Checks()
{
  gROOT->cd();

  Int_t gmulti = 4;
  const char* gmultitag = "fGlobalMulti4";
  trackreconstruct->L2L3_CheckParallelDraw(gmultitag, gmulti);
  trackreconstruct->L2L3_CheckL2L3DEEPlot (gmultitag, gmulti);
  trackreconstruct->L2L3_CheckL1L2DEEPlot (gmultitag, gmulti);
}
//******************************************************************************



//******************************************************************************
//                        fGlobalMulti = 6
//                    ------------------------
DecodefGlobalMulti6::DecodefGlobalMulti6(Int_t firstrun, Int_t lastrun)
{
  fFirstRun = firstrun;
  fLastRun  = lastrun;

  trackreconstruct = new CSHINETrackReconstruction(fFirstRun, fLastRun);

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
        bananacut[numtel].push_back(cut);
      //  cout<<cut->GetName()<<endl;
      }
    }
  }
}

DecodefGlobalMulti6::~DecodefGlobalMulti6()
{}

//_______________________________________________
// 对 globalmulti = 6 的事件进行综合检查
// (1)平行坐标表示, 检验层间关系是否有误
// (2)检查 fGlobalMulit = 6 事件的 DE-E plot
void DecodefGlobalMulti6::GlobalMulti6_Checks()
{
  gROOT->cd();

  Int_t gmulti = 6;
  const char* gmultitag = "fGlobalMulti6";
  trackreconstruct->L2L3_CheckParallelDraw(gmultitag, gmulti);
  trackreconstruct->L2L3_CheckL2L3DEEPlot (gmultitag, gmulti);
  trackreconstruct->L2L3_CheckL1L2DEEPlot (gmultitag, gmulti);
}
//******************************************************************************





//******************************************************************************
//                  模式识别
//               --------------
// 定义 hit 的模式
// 用一个四位数字符串 index 来区分不同的模式
// 4个数从左往右,分别表示 L3A,L2B,L2F,L1S 的情况
// 每个数取值为 0 或 1
// index 的形式如: 1011, 0101
void DecodefGlobalMulti2::ChangeModeToNumber(std::string mode, Int_t& l3a, Int_t& l2b, Int_t& l2f, Int_t& l1s)
{
  // 将字符串 mode 转成成 int 数字
  Int_t index;
  std::stringstream ss;
  ss<<mode;
  ss>>index;

  l3a =  index/1000;
  l2b = (index%1000)/100;
  l2f = (index%100)/10;
  l1s =  index%10;
}

//______________________________________________________________________________
// 判断 ch1 与 ch2 是否满足模式规则
Bool_t DecodefGlobalMulti2::IsModeMatched(Int_t mode_index, Int_t ch1, Int_t ch2)
{
  // 当 ch1 != ch2 时, mode_index = 1， 表示有两个不同位置点火
  return ((ch1!=ch2) == mode_index);
}

//______________________________________________________________________________
void DecodefGlobalMulti2::ExpDataToMode(std::string& mode, Int_t l3a[2], Int_t l2b[2], Int_t l2f[2], Int_t l1s[2])
{
  Int_t l3a_index, l2b_index, l2f_index, l1s_index;
  if (l3a[0] != l3a[1]) { l3a_index = 1; }  else { l3a_index = 0; }
  if (l2b[0] != l2b[1]) { l2b_index = 1; }  else { l2b_index = 0; }
  if (l2f[0] != l2f[1]) { l2f_index = 1; }  else { l2f_index = 0; }
  if (l1s[0] != l1s[1]) { l1s_index = 1; }  else { l1s_index = 0; }

  mode = Form("%d%d%d%d",l3a_index,l2b_index,l2f_index,l1s_index);
}
//******************************************************************************
