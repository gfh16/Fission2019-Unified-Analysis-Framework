#ifndef TIMEANDPERCENTAGE_H
#define TIMEANDPERCENTAGE_H

#include <TString.h>

#include <iostream>
#include <ctime>


//______________________________________________________________________________
class TimeAndPercentage
{
public:
  TimeAndPercentage();
  ~TimeAndPercentage();

  clock_t   GetBeginTime();
  clock_t   GetEndTime();
  Double_t  GetRunTime();

  void      PrintPercentage(Long64_t ientry, Long64_t nentries);
  void      PrintPercentageAndRemainingTime(Long64_t ientry, Long64_t nentries);

private:
  clock_t   fBeginTime;
  clock_t   fEndTime;
};

#endif
