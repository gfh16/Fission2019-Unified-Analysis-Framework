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
