#include "include/CSHINETrackReconstruction.h"
#include "include/CSHINEEvent.h"
#include "include/CSHINECsIEnergyCali.h"

using namespace std;

void test()
{
// Test_Multi test(120, 130);
//  test.TrackReconstructionAlgorithm();


//  DecodefGlobalMulti2 decode;
//  decode.GlobalMulti2_CalcModeRatio();


  //CSHINETrackReconstruction hit;
  //hit.GetPunchThroughEnergiesOfLCPs();
//  CSHINEBuildEvent build;
//  build.BuildLayerEventTree(120, 130);

 CSHINECsIEnergyCali csicali(120, 130);
 csicali.RunDEEFitCode();
}
