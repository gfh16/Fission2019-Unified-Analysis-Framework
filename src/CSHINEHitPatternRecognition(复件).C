////////////////////////////////////////////////////////////////////////////////
//
//  此程序用于硅条望远镜的模式识别. 大致可以分为以下几部分:
//   1.SSD 各层的多重性
//   2.L2F 与 L2B 能量关联
//   3.入射粒子的几何位置判断 (主要判断是 hit>=2 的事件)
//   4.确定每个 SSD 的多重性
//   5.确定 SSD 阵列的多重性
//
//  gfh, 2020-08-27
////////////////////////////////////////////////////////////////////////////////

#include "../include/CSHINEHitPatternRecognition.h"
#include "../include/TimeAndPercentage.h"
#include "../include/shared.h"


//______________________________________________________________________________
CSHINEHitPatternRecognition::CSHINEHitPatternRecognition()
{
  cout<<"Enter class CSHINEHitPatternRecognition!"<<endl;
}


//______________________________________________________________________________
CSHINEHitPatternRecognition::~CSHINEHitPatternRecognition()
{
  cout<<"Exit class CSHINEHitPatternRecognition!"<<endl;
}

//______________________________________________________________________________
//                            1.SSD 各层的多重性
//______________________________________________________________________________
// 计算每一层的多重数
Int_t CSHINEHitPatternRecognition::LayerMultiplicity(Int_t ssdindex, const char* layertag,
  Int_t* ech, Double_t* echcut)
{
  Int_t multi = 0;
  Int_t NUM_CH;

  if (strcmp(layertag, "L3A") != 0) {
    NUM_CH = NUM_STRIP; // for Si
  } else {
    NUM_CH = NUM_CSI;   // for Si
  }

  for (Int_t i=0; i<NUM_CH; i++) {
    if (ech[i]>echcut[ssdindex*NUM_CH + i]) {
      multi++;
    }
  }
  return multi;
}


//______________________________________________________________________________
// 计算 多重数 >=1 的事件数, 即有效的事件数
Double_t CSHINEHitPatternRecognition::RatioOfMuiltiplicity(TChain* chain, Int_t ssdindex,
  const char* layertag, Int_t* ech, Double_t* echcut)
{
  Int_t multiplicity;
  Int_t CountofMulti_ge_one          = 0;  // 多重数大于等于 1 的事件数

  unsigned long nentries = chain->GetEntries();
  for (unsigned long jentry=0; jentry<nentries; jentry++)
  {
    chain->GetEntry(jentry);

    multiplicity = LayerMultiplicity(ssdindex, layertag, ech, echcut);
    if (multiplicity >= 1) {
      CountofMulti_ge_one ++;
    }
  }
  return CountofMulti_ge_one;
}


//______________________________________________________________________________
// 计算 1=< 多重数 <= multicut 的事件数
Double_t CSHINEHitPatternRecognition::GetNumOfEntriesWithMultiCut(TChain* chain,
  Int_t multicut, Int_t ssdindex, const char* layertag, Int_t* ech, Double_t* echcut)
{
  Int_t multiplicity;
  Int_t CountOfMulti_ge_one_le_multi = 0;  // 多重数大于等于 1, 小于等于 multi 的事件数

  unsigned long nentries = chain->GetEntries();
  for (unsigned long jentry=0; jentry<nentries; jentry++)
  {
    chain->GetEntry(jentry);

    multiplicity = LayerMultiplicity(ssdindex, layertag, ech, echcut);
    if ((multiplicity >= 1) && (multiplicity <= multicut)) {
      CountOfMulti_ge_one_le_multi ++;
    }
  }
  return CountOfMulti_ge_one_le_multi;
}


//______________________________________________________________________________
// 计算 1=< 多重数 <= multi 的事件占 多重数>=1 事件的比例
Double_t CSHINEHitPatternRecognition::RatioOfMuiltiplicity(TChain* chain, Int_t multicut,
  Int_t ssdindex, const char* layertag, Int_t* ech, Double_t* echcut)
{
  Int_t multiplicity;
  Int_t CountofMulti_ge_one          = 0;  // 多重数大于等于 1 的事件数
  Int_t CountOfMulti_ge_one_le_multi = 0;  // 多重数大于等于 1, 小于等于 multi 的事件数
  Double_t ratio;

  unsigned long nentries = chain->GetEntries();
  cout<< "Found " << nentries <<" entries"<<endl;
  for (unsigned long jentry=0; jentry<nentries; jentry++)
  {
    chain->GetEntry(jentry);

    multiplicity = LayerMultiplicity(ssdindex, layertag, ech, echcut);
    if (multiplicity >= 1) {
      CountofMulti_ge_one ++;
    }
    if ((multiplicity >= 1) && (multiplicity <= multi)) {
      CountOfMulti_ge_one_le_multi ++;
    }
  }

  if (CountofMulti_ge_one == 0) {
    cout<<"Multiplicity Error!"<<endl;
  } else {
    ratio = (Double_t) CountOfMulti_ge_one_le_multi/CountofMulti_ge_one;
  }

  cout<<setw(10)<<"Entries = "<<nentries<<setw(20)<<"(Multi >= 1) = "<<CountofMulti_ge_one
      <<setw(35)<<Form("(1 <= Multi <= %d) = %d", multi, CountOfMulti_ge_one_le_multi)
      <<setw(15)<<"Ratio = "<<ratio<<endl; */

  return ratio;
}
