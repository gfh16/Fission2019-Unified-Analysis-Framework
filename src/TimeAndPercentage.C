#include "../include/TimeAndPercentage.h"


//******************************************************************************
TimeAndPercentage::TimeAndPercentage()
{
  fBeginTime = 0;
  fEndTime   = 0;
}

//______________________________________________________________________________
TimeAndPercentage::~TimeAndPercentage()
{
  std::cout<<"\n";
  cout<<"The total computing time is "<<GetRunTime()<<" s"<<endl;
}
//******************************************************************************


//******************************************************************************
clock_t TimeAndPercentage::GetBeginTime()
{
  fBeginTime = clock();
  return fBeginTime;
}

//______________________________________________________________________________
clock_t TimeAndPercentage::GetEndTime()
{
  fEndTime = clock();
  return fEndTime;
}

//______________________________________________________________________________
Double_t TimeAndPercentage::GetRunTime()
{
  return (Double_t)(fEndTime-fBeginTime)/CLOCKS_PER_SEC;
}
//******************************************************************************


//******************************************************************************
void TimeAndPercentage::PrintPercentage(Long64_t ientry, Long64_t nentries)
{
  std::cout<<" Percentage = "<<std::fixed<<std::setprecision(1)<<std::setw(5)
           <<(100*Double_t(ientry)/nentries)<<" %";
  std::cout<<" [";
  for (Int_t index=0; index<Int_t(100*Double_t(ientry)/nentries); index+=5) {
    std::cout<<"=";
  }
  std::cout<<"]\r";
  std::cout.flush();
}
//******************************************************************************


//******************************************************************************
void TimeAndPercentage::PrintPercentageAndRemainingTime(Long64_t ientry, Long64_t nentries)
{
  Double_t time_elapsed = (Double_t)(clock()-fBeginTime)/CLOCKS_PER_SEC;
  std::cout<<" Percentage = "<<std::fixed<<std::setprecision(1)<<std::setw(5)
           <<(100*Double_t(ientry)/nentries)<<" %";
  std::cout<<" [";
  for (Int_t index=0; index<Int_t(100*Double_t(ientry)/nentries); index+=5) {
    std::cout<<"=";
  }
  std::cout<<"]   "<<"elapsed time "<<setprecision(1)
           <<(time_elapsed<60   ? time_elapsed :
             (time_elapsed<3600 ? time_elapsed/60 : time_elapsed/3600))
           <<(time_elapsed<60   ? "s; " : (time_elapsed<3600 ? "m; " : "h; "));
  if (ientry>0)
  {
    Double_t time_remaining = (time_elapsed/ientry)*(nentries-ientry);
    std::cout<<" estimated remaining time "<<setprecision(1)
             <<(time_remaining<60   ? time_remaining    :
               (time_remaining<3600 ? time_remaining/60 : time_remaining/3600))
             <<(time_remaining<60   ? "s; " : (time_remaining<3600 ? "m; " : "h; "));
  }
  std::cout<<"\r";
  std::cout.flush();
}
//******************************************************************************
