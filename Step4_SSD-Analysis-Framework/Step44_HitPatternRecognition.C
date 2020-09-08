////////////////////////////////////////////////////////////////////////////////
//   此脚本用于模式识别
//
//   gfh, 2020-09-06
////////////////////////////////////////////////////////////////////////////////

#include "../include/CSHINEHitPatternRecognition.h"


//______________________________________________________________________________
void Step44_HitPatternRecognition()
{
  CSHINEHitPatternRecognition layermulti;

  Int_t FisrtRun = 200;
  Int_t LastRun  = 201;

  layermulti.EstimateLayerMulti(FisrtRun, LastRun);
}
