#include "include/CSHINETrackReconstruction.h"
#include "include/CSHINEEvent.h"
#include "include/CSHINEParticleIdentification.h"
#include "include/TimeAndPercentage.h"
#include "include/CSHINECsIEnergyCali.h"
#include "include/CSHINEAlphaCali.h"

using namespace std;

void test()
{
//  CSHINEAlphaCali alphacali;
//  alphacali.CalculateAlphaEnergy();
  //_______________________________________
  // step1: build layerevent
  CSHINEBuildEvent build;
//  build.BuildLayerEventTree(150, 400);

  //_______________________________________________
  // step2: 径迹重建前的一些测试与检查
//  Test_Multi test(120, 200);

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
//    test.CalcClusterSize_Equal2_ERatio();
//    test.ClusterSize_Equal2_CsI();
//    test.CheckCrossTalk_CsI();


//__________________________________________________
//  检查 DEE plot
   CSHINECheckDEEPlot deefitcheck(150, 160);
//   deefitcheck.CheckL2L3DEE();
//   deefitcheck.CheckL2L3PIDResults();

//   deefitcheck.CheckL1L2DEE();
//   deefitcheck.CheckL1L2DEE_Uncalibrate();
//   deefitcheck.CheckDEEL1L2_SiResolution();
     deefitcheck.CheckGoodPIDOfSSD3AndSSD4();
//     deefitcheck.CheckL1L2DEEPunchThrough();

//   deefitcheck.CheckCsIAlphaEnergyResolution();
//   deefitcheck.CheckL1L2StraighteningPIDResults();
//   deefitcheck.CheckL2L3StraighteningPIDResults();
//   deefitcheck.CheckPolyFitResults();

//__________________________________________________
//  使用 DEEFIT 方法进行粒子鉴别
    CSHINEDEEFITPID deefit(150, 160);
//   deefit.DEEFITGenerateData_L2L3();
//  deefit.DEEFITGenerateData_L1L2();
//   deefit.DEEFITRunCode();

//    CSHINEStraighteningPID straightdee;
//    straightdee.DoBananaCut();

//__________________________________________________________________
// CsI 刻度
  CSHINECsIEnergyCali csicali(150,250);
//  csicali.DoGraphicalCut();
//  csicali.GetProjectionCsIEnergPoints();
//  csicali.GetDEEFITCsIEnergyPoints();
//  csicali.DEEFITDrawAndFit_Z1();
//  csicali.DEEFITDrawAndFit_ZOver2();
//   csicali.CheckCsIEnergyCaliResults();
//    csicali.GetStraighteningCsIEnergyPoints();
//    csicali.StraighteningDrawAndFit_Z1();
//   csicali.StraighteningDrawAndFit_ZOver2();
}
