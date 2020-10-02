#include "include/shared.h"
#include "include/CSHINEPulserCali.h"


void test()
{
  CSHINEPulserCali pulsecali;
//  pulsecali.L1_AutoFindPeaksAndFit_ReCali("Height","LowGain");
//  pulsecali.L1_GainEffectOnPulseCali_ReCali("Height");
    pulsecali.CheckLinearity("L2F");
}
