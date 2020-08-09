#include "../include/TimeAndPercentage.h"


//______________________________________________________________________________
void test2()
{

  TimeAndPercentage time;

  time.GetBeginTime();
  Long64_t nentries = 1e7;

  for (Long64_t i=0; i<nentries; i++)
  {
    ///cout<<gRandom->Gaus(10,5)<<endl;
    //cout<<i<<endl;
    time.PrintPercentageAndRemainingTime(i, nentries);
  }

  time.GetEndTime();
  time.GetRunTime();
}
