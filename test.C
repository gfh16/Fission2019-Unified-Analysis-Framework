#include "include/CSHINETrackReconstruction.h"
#include "include/CSHINEEvent.h"
#include "include/CSHINEParticleIdentification.h"
#include "include/TimeAndPercentage.h"
#include "include/CSHINECsIEnergyCali.h"
#include "include/CSHINEAlphaCali.h"

#include "include/L2L3TrackFinding.h"
#include "include/L1L2TrackFinding.h"


using namespace std;


//______________________________________________________________________________
void test()
{

  CSHINESiPixelGeometry pixel;
  //pixel.SiPixellation();


//  CSHINEAlphaCali alphacali;
//  alphacali.CalculateAlphaEnergy();
  //_______________________________________
  // step1: build layerevent
  CSHINEBuildEvent build;
//  build.BuildLayerEventTree(151, 151);

  //_______________________________________________
  //  径迹重建前的一些测试与检查
  CSHINETrackReconstruction test(150, 160);
//  test.PrintUsage();

//  test.CheckL2BL2FEnergyCorrelation(0);
//  test.CheckL2BL2FEnergyCorrelation(1);
//  test.CheckL2BL2FEnergyCorrelation(2);
//  test.CheckL2BL2FEnergyCorrelation(3);
//  test.DetermineL2BL2FEnergyErrRatio();

//  test.CheckEnergyLossL1L2();
//  test.CheckEnergyLossL1L2_Relationship(1);
//  test.CheckEnergyLossL1L2_Expdata();

  test.CheckLayerMultiPercentage();

//___________________________________________
//              电荷分配效应
//            ----------------
//  test.CheckAlphaCaliChargeSharing();
//  test.CheckClusterSize_Si();
//  test.CalcClusterSize_Equal2_ERatio();
//  test.CheckEnergyCondition_ExpData();

//___________________________________________
//              CsI 晶体的串扰
//           ------------------
//  test.ClusterSize_Equal2_CsI();
//  test.CheckCrossTalk_CsI();


//___________________________________________
//             径迹重建算法
//          -----------------
//            L2-L3 径迹重建
//           ---------------
//  test.L2L3_TrackReconstructionAlgorithm();
//  test.CheckGlobalMultiRatio("L2L3",6);
//  for (Int_t i=1; i<=6; i++)  test.GlobalMulti_ExtractData("L2L3",i);
//  test.L2L3_Mode_DrawClassification(2);
//  test.L2L3_Mode_CalcRatio(2);
//  test.L2L3_Mode_CalcRatio(3);
//  test.L2L3_Mode_CalcRatio(4);
//  test.L2L3_Mode_CalcRatio(6);

//  test.L2L3_Mode_ExtractData(2);


//          L1-L2 径迹重建
//       -------------------
// test.L1L2_TrackReconstructionAlgorithm();
//  test.CheckGlobalMultiRatio("L1L2",6);
  for (Int_t i=2; i<=6; i++)
  {
    // test.GlobalMulti_ExtractData("L1L2",i);
    // test.L1L2_Mode_DrawClassification(i);
    // test.L1L2_Mode_CalcRatio(i);
    // test.L1L2_Mode_ExtractData(i);
  }


//******************************************************************************
//                           径迹重建结果检查
//
// 模式识别
  DecodefGlobalMulti1 decode1(150, 160);
//  decode1.GlobalMulti1_Checks();
//  decode1.GlobalMulti_L2L3_Discard();
//  decode1.GlobalMulti_L2L3_EneL2BL2F_Discard();
 // decode1.GlobalMulti_L2L3_EneL1SL2F_Discard();

//  decode1.GlobalMulti1_L2L3_Decode();


  DecodefGlobalMulti2 decode2(150, 160);
//  decode2.GlobalMulti2_Checks("L2L3");
//  decode2.GlobalMulti2_CalcModeRatio();

//  decode2.GlobalMulti2_Decode_1111();
//  decode2.GlobalMulti2_Decode_1101();
//  decode2.GlobalMulti2_Decode_1010();
//  decode2.GlobalMulti2_Decode_1000();
//  decode2.GlobalMulti2_Decode_0101();
//  decode2.GlobalMulti2_Decode_0100();
//  decode2.GlobalMulti2_Decode_0010();
//  decode2.GlobalMulti2_Decode_0001();

//                           globalmulti = 3
//                         -------------------
  DecodefGlobalMulti3 trackg3(150, 160);
  //trackg3.GlobalMulti3_Checks();

//  trackg3.GlobalMulti3_Decode_2222();
//  trackg3.GlobalMulti3_Decode_2111();
//  trackg3.GlobalMulti3_Decode_1211();
//  trackg3.GlobalMulti3_Decode_1121();
//  trackg3.GlobalMulti3_Decode_1112();
//  trackg3.GlobalMulti3_Decode_1111();
//  trackg3.GlobalMulti3_Decode_1110();
//  trackg3.GlobalMulti3_Decode_1020();
//  trackg3.GlobalMulti3_Decode_1010();
//  trackg3.GlobalMulti3_Decode_0201();
//  trackg3.GlobalMulti3_Decode_0102();
//  trackg3.GlobalMulti3_Decode_0101();
//  trackg3.GlobalMulti3_Decode_0020();
//  trackg3.GlobalMulti3_Decode_0002();


  DecodefGlobalMulti4 trackg4(150, 160);
  //trackg4.GlobalMulti4_Checks();

//  trackg4.GlobalMulti4_Decode_2222();
//  trackg4.GlobalMulti4_Decode_1111();
//  trackg4.GlobalMulti4_Decode_1110();
//  trackg4.GlobalMulti4_Decode_1011();
//  trackg4.GlobalMulti4_Decode_0111();
//  trackg4.GlobalMulti4_Decode_0110();
//  trackg4.GlobalMulti4_Decode_0101();
//  trackg4.GlobalMulti4_Decode_0011();


  DecodefGlobalMulti6 trackg6(150, 160);
//  trackg6.GlobalMulti6_Checks();

//  trackg6.GlobalMulti6_Decode_2222();
//  trackg6.GlobalMulti6_Decode_1211();
//  trackg6.GlobalMulti6_Decode_1121();
//  trackg6.GlobalMulti6_Decode_1112();
//  trackg6.GlobalMulti6_Decode_1111();
//  trackg6.GlobalMulti6_Decode_1110();
//  trackg6.GlobalMulti6_Decode_1011();
//  trackg6.GlobalMulti6_Decode_0211();
//  trackg6.GlobalMulti6_Decode_0121();
//  trackg6.GlobalMulti6_Decode_0120();
//  trackg6.GlobalMulti6_Decode_0112();
//  trackg6.GlobalMulti6_Decode_0111();
//  trackg6.GlobalMulti6_Decode_0021();
//  trackg6.GlobalMulti6_Decode_0012();



//******************************************************************************
//                        L1L2 径迹重建
//                      ----------------
  L1L2_DecodefGlobalMulti1 l1l2track1(150, 160);
//  l1l2track1.L1L2_GlobalMulti1_Decode();


  L1L2_DecodefGlobalMulti2 l1l2track2(150, 160);
//  l1l2track2.L1L2_GlobalMulti2_Decode_001();
//  l1l2track2.L1L2_GlobalMulti2_Decode_010();
//  l1l2track2.L1L2_GlobalMulti2_Decode_100();
//  l1l2track2.L1L2_GlobalMulti2_Decode_101();


  L1L2_DecodefGlobalMulti3 l1l2track3(150, 160);
//  l1l2track3.L1L2_GlobalMulti3_Decode_002();
//  l1l2track3.L1L2_GlobalMulti3_Decode_020();
//  l1l2track3.L1L2_GlobalMulti3_Decode_101();
//  l1l2track3.L1L2_GlobalMulti3_Decode_102();
//  l1l2track3.L1L2_GlobalMulti3_Decode_200();
//  l1l2track3.L1L2_GlobalMulti3_Decode_201();
//  l1l2track3.L1L2_GlobalMulti3_Decode_202();


  L1L2_DecodefGlobalMulti4 l1l2track4(150, 160);
//  l1l2track4.L1L2_GlobalMulti4_Decode_011();
//  l1l2track4.L1L2_GlobalMulti4_Decode_101();
//  l1l2track4.L1L2_GlobalMulti4_Decode_110();
//  l1l2track4.L1L2_GlobalMulti4_Decode_111();


  L1L2_DecodefGlobalMulti6 l1l2track6(150, 160);
//  l1l2track6.L1L2_GlobalMulti6_Decode_111();
//  l1l2track6.L1L2_GlobalMulti6_Decode_112();
//  l1l2track6.L1L2_GlobalMulti6_Decode_120();
//  l1l2track6.L1L2_GlobalMulti6_Decode_121();
//  l1l2track6.L1L2_GlobalMulti6_Decode_211();


//*******************************************************************************
//                           粒子鉴别
//                     ------------------
//  使用 DEEFIT 方法进行粒子鉴别
    CSHINEDEEFITPID deefit(150, 250);
//  deefit.DEEFITGenerateData_L2L3();
//  deefit.DEEFITGenerateData_L1L2();
//  deefit.DEEFITRunCode();

//  CSHINEStraighteningPID straightdee;
//  straightdee.DoBananaCut();



//******************************************************************************
//                        粒子鉴别结果检查
//                     -------------------
//  检查 DEE plot
   CSHINECheckDEEPlot deefitcheck(150, 250);
//   deefitcheck.CheckL2L3DEE();
//   deefitcheck.CheckL2L3PIDResults();

//   deefitcheck.CheckL1L2DEE();
//   deefitcheck.CheckL1L2DEE_Uncalibrate();
//   deefitcheck.CheckDEEL1L2_SiResolution();
//   deefitcheck.CheckGoodPIDOfSSD3AndSSD4();
//   deefitcheck.CheckL1L2DEEPunchThrough();
//   deefitcheck.CheckL2L3DEECsIGapEffect();

//   deefitcheck.CheckCsIAlphaEnergyResolution();
//   deefitcheck.CheckL1L2StraighteningPIDResults();
//   deefitcheck.CheckL2L3StraighteningPIDResults();
//   deefitcheck.CheckPolyFitResults();
//   deefitcheck.L1S_FindThicknessUniformity();
//   deefitcheck.L1S_DetermineThicknessUniformity();

///  deefitcheck.L2L3DrawPIDECsIMap();

//__________________________________________________________________
//                        CsI 能量刻度
//                     -------------------
  CSHINECsIEnergyCali csicali(150,160);
//  csicali.DoGraphicalCut();
//  csicali.GetProjectionCsIEnergPoints();
//  csicali.GetDEEFITCsIEnergyPoints();
//  csicali.DEEFITDrawAndFit_Z1();
//  csicali.DEEFITDrawAndFit_ZOver2();
//  csicali.CheckCsIEnergyCaliResults();
//  csicali.GetStraighteningCsIEnergyPoints();
//  csicali.StraighteningDrawAndFit_Z1();
//  csicali.StraighteningDrawAndFit_ZOver2();
}
