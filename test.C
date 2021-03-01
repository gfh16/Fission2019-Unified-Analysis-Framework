#include "include/CSHINETrackReconstruction.h"
#include "include/CSHINEEvent.h"
#include "include/CSHINEParticleIdentification.h"
#include "include/TimeAndPercentage.h"

using namespace std;

void test()
{
  //_______________________________________
  // step1: build layerevent
  CSHINEBuildEvent build;
  build.BuildLayerEventTree(120, 200);

  //_______________________________________________
  // step2: 径迹重建前的一些测试与检查
//  Test_Multi test(120, 130);

//  test.CheckL2BL2FEnergyCorrelation(0);
//  test.CheckL2BL2FEnergyCorrelation(1);
//  test.CheckL2BL2FEnergyCorrelation(2);
//  test.CheckL2BL2FEnergyCorrelation(3);

//  test.DetermineL2BL2FEnergyErrRatio();

//  test.CheckEnergyLossL1L2();
//  test.CheckEnergyLossL1L2_Relationship(1);
//  test.CheckEnergyLossL1L2_Expdata();

//  test.CheckLayerMultiPercentage();
//  test.TrackReconstructionAlgorithm();

//  test.CheckTrackReconstructionResult();

//  test.CheckGlobalMultiRatio();

//    test.CheckClusterSize_Si();
//   test.CalcClusterSize_Equal2_ERatio();

// 粒子鉴别
//   CSHINEDEEFITPID deefit(120, 130);
//   deefit.CheckL2L3EnergyCorrelation();

}
