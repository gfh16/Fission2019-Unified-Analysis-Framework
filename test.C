#include "include/CSHINEPulserCali.h"
#include "include/CSHINEAlphaCali.h"
#include "include/CSHINESiEnergyCali.h"
#include "include/EnergyLossModule.h"


void test()
{
  CSHINEPulserCali pulsecali;
//  pulsecali.L1_AutoFindPeaksAndFit_ReCali("Height","LowGain");
//  pulsecali.L1_GainEffectOnPulseCali_ReCali("Height");
//    pulsecali.CheckLinearity("L2F");

  CSHINEAlphaCali alphacali;
//  alphacali.CalculateAlphaEnergy();
//  alphacali.CheckAbnormaleChannels_SSD4();

  CSHINESiEnergyCali energycali;
  energycali.PulserAndAlphaCali("L1S","Height");
}
