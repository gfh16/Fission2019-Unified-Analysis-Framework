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

  //oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
  // "像素点"检查
  CSHINESiPixelGeometry pixel;
  //pixel.SiPixellation();


  //oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
  // 计算 Mylar 膜修正后的放射源 alpha 能量
  CSHINEAlphaCali alphacali;
  // alphacali.CalculateAlphaEnergy();


  //oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
  // 创建 LayerEvent
  CSHINEBuildEvent build;
  // build.BuildLayerEventTree(90, 430);


  //oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
  // 径迹重建前的一些测试与检查
  CSHINETrackReconstruction test(150, 160);
  // test.PrintUsage();

  //----------------------------------------
  // L2B && L2F 能量约束条件
  // test.CheckL2BL2FEnergyCorrelation(0);
  // test.CheckL2BL2FEnergyCorrelation(1);
  // test.CheckL2BL2FEnergyCorrelation(2);
  // test.CheckL2BL2FEnergyCorrelation(3);
  // test.DetermineL2BL2FEnergyErrRatio();

  //----------------------------------------
  // L1 && L2(L2F) 能量约束条件
  // test.CheckEnergyLossL1L2();
  // test.CheckEnergyLossL1L2_Relationship(1);
  // test.CheckEnergyLossL1L2_Expdata();

  //---------------------------------------
  // LayerEvent 中粒子多重性检查
  // test.CheckLayerMultiPercentage();

  //___________________________________________
  //           检查电荷分配效应
  //         ----------------
  //  test.CheckAlphaCaliChargeSharing();
  //  test.CheckClusterSize_Si();
  //  test.CalcClusterSize_Equal2_ERatio();
  //  test.CheckEnergyCondition_ExpData();

  //___________________________________________
  //          检查CsI 晶体的串扰
  //        ---------------------
  // test.ClusterSize_Equal2_CsI();
  // test.CheckCrossTalk_CsI();



  //————————————————————————————————————————————————————————————————————————————
  //                       径迹重建算法
  //                   -----------------
  //
  //              L2-L3 创建待重建的事件并提取数据
  //           ----------------------------------
  //  test.L2L3_TrackReconstructionAlgorithm();
  //  test.CheckGlobalMultiRatio("L2L3",12);
  //  for (Int_t i=3; i<=12; i++)  test.GlobalMulti_ExtractData("L2L3",i);
  //  test.L2L3_Mode_DrawClassification(2);
  //  test.L2L3_Mode_CalcRatio(2);
  //  test.L2L3_Mode_CalcRatio(3);
  //  test.L2L3_Mode_CalcRatio(4);
  //  test.L2L3_Mode_CalcRatio(6);
  //  test.L2L3_Mode_CalcRatio(8);
  //  test.L2L3_Mode_CalcRatio(12);

  //  test.L2L3_Mode_ExtractData(2);
  //test.L2L3_Mode_ExtractData(3);
  //  test.L2L3_Mode_ExtractData(4);
  //  test.L2L3_Mode_ExtractData(6);
  //  test.L2L3_Mode_ExtractData(8);
  //  test.L2L3_Mode_ExtractData(12);



  //____________________________________________________________________________
  //              L1-L2 创建待重建的事件并提取数据
  //           ----------------------------------
  // test.L1L2_TrackReconstructionAlgorithm();
  //  test.CheckGlobalMultiRatio("L1L2",6);
  //  for (Int_t i=2; i<=6; i++) { test.GlobalMulti_ExtractData("L1L2",i); }
  //  for (Int_t i=2; i<=6; i++) { test.L1L2_Mode_DrawClassification(i); }
  //  for (Int_t i=2; i<=6; i++) { test.L1L2_Mode_CalcRatio(i); }
  //  for (Int_t i=2; i<=6; i++) { test.L1L2_Mode_ExtractData(i); }



  //oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
  //                           L2L3-径迹识别算法与结果检验
  //                      -----------------------------------
  //
  // 模式识别
  L2L3_DecodefGlobalMulti1 l2l3track1(150, 160);
  // l2l3track1.GlobalMulti1_L2L3_Decode();


  L2L3_DecodefGlobalMulti2 l2l3track2(150, 160);
  //  l2l3track2.GlobalMulti2_Checks("L2L3");
  //  l2l3track2.GlobalMulti2_CalcModeRatio();

  //  l2l3track2.GlobalMulti2_Decode_1111();
  //  l2l3track2.GlobalMulti2_Decode_1101();
  //  l2l3track2.GlobalMulti2_Decode_1010();
  //  l2l3track2.GlobalMulti2_Decode_1000();
  //  l2l3track2.GlobalMulti2_Decode_0101();
  //  l2l3track2.GlobalMulti2_Decode_0100();
  //  l2l3track2.GlobalMulti2_Decode_0010();
  //  l2l3track2.GlobalMulti2_Decode_0001();


  L2L3_DecodefGlobalMulti3 l2l3track3(150, 160);
  //l2l3track3.GlobalMulti3_Checks();

  //  l2l3track3.GlobalMulti3_Decode_2222();
  //  l2l3track3.GlobalMulti3_Decode_2111();
  //  l2l3track3.GlobalMulti3_Decode_1211();
  //  l2l3track3.GlobalMulti3_Decode_1121();
  //  l2l3track3.GlobalMulti3_Decode_1112();
  //  l2l3track3.GlobalMulti3_Decode_1111();
  //  l2l3track3.GlobalMulti3_Decode_1110();
  //  l2l3track3.GlobalMulti3_Decode_1020();
  //  l2l3track3.GlobalMulti3_Decode_1010();
  //  l2l3track3.GlobalMulti3_Decode_0201();
  //  l2l3track3.GlobalMulti3_Decode_0102();
  //  l2l3track3.GlobalMulti3_Decode_0101();
  //  l2l3track3.GlobalMulti3_Decode_0020();
  //  l2l3track3.GlobalMulti3_Decode_0002();


  L2L3_DecodefGlobalMulti4 l2l3track4(150, 160);
  //l2l3track4.GlobalMulti4_Checks();

  //  l2l3track4.GlobalMulti4_Decode_2222();
  //  l2l3track4.GlobalMulti4_Decode_1111();
  //  l2l3track4.GlobalMulti4_Decode_1110();
  //  l2l3track4.GlobalMulti4_Decode_1011();
  //  l2l3track4.GlobalMulti4_Decode_0111();
  //  l2l3track4.GlobalMulti4_Decode_0110();
  //  l2l3track4.GlobalMulti4_Decode_0101();
  //  l2l3track4.GlobalMulti4_Decode_0011();


  L2L3_DecodefGlobalMulti6 l2l3track6(150, 160);
  //  l2l3track6.GlobalMulti6_Checks();

  //  l2l3track6.GlobalMulti6_Decode_2222();
  //  l2l3track6.GlobalMulti6_Decode_1211();
  //  l2l3track6.GlobalMulti6_Decode_1121();
  //  l2l3track6.GlobalMulti6_Decode_1112();
  //  l2l3track6.GlobalMulti6_Decode_1111();
  //  l2l3track6.GlobalMulti6_Decode_1110();
  //  l2l3track6.GlobalMulti6_Decode_1011();
  //  l2l3track6.GlobalMulti6_Decode_0211();
  //  l2l3track6.GlobalMulti6_Decode_0121();
  //  l2l3track6.GlobalMulti6_Decode_0120();
  //  l2l3track6.GlobalMulti6_Decode_0112();
  //  l2l3track6.GlobalMulti6_Decode_0111();
  //  l2l3track6.GlobalMulti6_Decode_0021();
  //  l2l3track6.GlobalMulti6_Decode_0012();



//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//                           L1L2-径迹识别算法与结果检验
//                      -----------------------------------

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



  //oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
  //                           粒子鉴别
  //                     ------------------
  //  使用 DEEFIT 方法进行粒子鉴别
  CSHINEDEEFITPID deefit(150, 250);
  //  deefit.DEEFITGenerateData_L2L3();
  //  deefit.DEEFITGenerateData_L1L2();
  //  deefit.DEEFITRunCode();

  //  CSHINEStraighteningPID straightdee;
  //  straightdee.DoBananaCut();



  //oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
  //                        粒子鉴别结果检查
  //                    ----------------------
  //  检查 DEE plot
  CSHINECheckDEEPlot deefitcheck(150, 250);
  //  deefitcheck.CheckL2L3DEE();
  //  deefitcheck.CheckL2L3PIDResults();

  // deefitcheck.CheckL1L2DEE();
  // deefitcheck.CheckL1L2DEE_Uncalibrate();
  // deefitcheck.CheckDEEL1L2_SiResolution();
  // deefitcheck.CheckGoodPIDOfSSD3AndSSD4();
  // deefitcheck.CheckL1L2DEEPunchThrough();
  // deefitcheck.CheckL2L3DEECsIGapEffect();

  // deefitcheck.CheckCsIAlphaEnergyResolution();
  // deefitcheck.CheckL1L2StraighteningPIDResults();
  // deefitcheck.CheckL2L3StraighteningPIDResults();
  // deefitcheck.CheckPolyFitResults();
  // deefitcheck.L1S_FindThicknessUniformity();
  // deefitcheck.L1S_DetermineThicknessUniformity();

  // deefitcheck.L2L3DrawPIDECsIMap();



  //oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
  //                     CsI 能量刻度
  //                   -------------------
  CSHINECsIEnergyCali csicali(150,160);
  // csicali.DoGraphicalCut();
  // csicali.GetProjectionCsIEnergPoints();
  // csicali.GetDEEFITCsIEnergyPoints();
  // csicali.DEEFITDrawAndFit_Z1();
  // csicali.DEEFITDrawAndFit_ZOver2();
  // csicali.CheckCsIEnergyCaliResults();
  // csicali.GetStraighteningCsIEnergyPoints();
  // csicali.StraighteningDrawAndFit_Z1();
  // csicali.StraighteningDrawAndFit_ZOver2();
}
