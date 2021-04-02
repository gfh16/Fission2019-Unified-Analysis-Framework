#include "../include/CSHINEParticleIdentification.h"
#include <iostream>

using namespace std;


//________________________________
int main()
{
  CSHINECheckDEEPlot deefitcheck(150,160);
  deefitcheck.CheckCsIAlphaEnergyResolution();

  return 0;
}
