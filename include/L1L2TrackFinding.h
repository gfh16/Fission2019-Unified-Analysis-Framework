#ifndef L1L2TRACKFINDING_H
#define L1L2TRACKFINDING_H

#include "EnergyLossModule.h"
#include "CSHINESSDCalibratedData.h"
#include "TimeAndPercentage.h"
#include "shared.h"
#include "CSHINEEvent.h"
# include "CSHINEParticleIdentification.h"

#include <iostream>
#include <TTree.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TMath.h>



//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//                           fglobalmulti == 1
//                        --------------------
class L1L2_DecodefGlobalMulti1
{
public:
  L1L2_DecodefGlobalMulti1();
  L1L2_DecodefGlobalMulti1(Int_t firstrun, Int_t lastrun);
  ~L1L2_DecodefGlobalMulti1();

  virtual void    L1L2_GlobalMulti1_Decode();

private:
  Int_t                       fFirstRun;
  Int_t                       fLastRun;
  TimeAndPercentage           timeper;
  CSHINETrackReconstruction  *trackreconstruct;
  CSHINEDEEFITPID             fPID;

  std::vector<TCutG*>         bananacut[NUM_SSD];


private:
  Double_t EL1Range[4] = {400., 320., 150., 100.};
	Int_t    NBinsL1 [4] = {4000, 3200, 1500, 1000};
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2 [4] = {3000, 2500, 1500, 800};

private:
  std::vector<Int_t>    L1L2_SSDNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L2BStripNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L2FStripNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L1SStripNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L2FTime[NUM_SSD];

  std::vector<Double_t> L1L2_L2BEMeV[NUM_SSD];
  std::vector<Double_t> L1L2_L2FEMeV[NUM_SSD];
  std::vector<Double_t> L1L2_L1SEMeV[NUM_SSD];
  std::vector<Double_t> L1L2_L1SEMeV_Corrected[NUM_SSD];

  Int_t       fZone_index;
  const Int_t fZone_length = 8;
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo



//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//                           fglobalmulti == 2
//                        --------------------
class L1L2_DecodefGlobalMulti2
{
public:
  L1L2_DecodefGlobalMulti2();
  L1L2_DecodefGlobalMulti2(Int_t firstrun, Int_t lastrun);
  ~L1L2_DecodefGlobalMulti2();

  virtual void    L1L2_GlobalMulti2_Decode_001();
  virtual void    L1L2_GlobalMulti2_Decode_010();
  virtual void    L1L2_GlobalMulti2_Decode_100();
  virtual void    L1L2_GlobalMulti2_Decode_101();

private:
  Int_t                       fFirstRun;
  Int_t                       fLastRun;
  TimeAndPercentage           timeper;
  CSHINETrackReconstruction  *trackreconstruct;
  CSHINEDEEFITPID             fPID;

  std::vector<TCutG*>         bananacut[NUM_SSD];


private:
  Double_t EL1Range[4] = {400., 320., 150., 100.};
	Int_t    NBinsL1 [4] = {4000, 3200, 1500, 1000};
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2 [4] = {3000, 2500, 1500, 800};

private:
  std::vector<Int_t>    L1L2_SSDNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L2BStripNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L2FStripNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L1SStripNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L2FTime[NUM_SSD];

  std::vector<Double_t> L1L2_L2BEMeV[NUM_SSD];
  std::vector<Double_t> L1L2_L2FEMeV[NUM_SSD];
  std::vector<Double_t> L1L2_L1SEMeV[NUM_SSD];
  std::vector<Double_t> L1L2_L1SEMeV_Corrected[NUM_SSD];

  Int_t       fZone_index;
  const Int_t fZone_length = 8;
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo



//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//                           fglobalmulti == 3
//                        --------------------
class L1L2_DecodefGlobalMulti3
{
public:
  L1L2_DecodefGlobalMulti3();
  L1L2_DecodefGlobalMulti3(Int_t firstrun, Int_t lastrun);
  ~L1L2_DecodefGlobalMulti3();

  virtual void    L1L2_GlobalMulti3_Decode_002();
  virtual void    L1L2_GlobalMulti3_Decode_020();
  virtual void    L1L2_GlobalMulti3_Decode_101();
  virtual void    L1L2_GlobalMulti3_Decode_102();
  virtual void    L1L2_GlobalMulti3_Decode_200();
  virtual void    L1L2_GlobalMulti3_Decode_201();
  virtual void    L1L2_GlobalMulti3_Decode_202();

private:
  Int_t                       fFirstRun;
  Int_t                       fLastRun;
  TimeAndPercentage           timeper;
  CSHINETrackReconstruction  *trackreconstruct;
  CSHINEDEEFITPID             fPID;

  std::vector<TCutG*>         bananacut[NUM_SSD];


private:
  Double_t EL1Range[4] = {400., 320., 150., 100.};
	Int_t    NBinsL1 [4] = {4000, 3200, 1500, 1000};
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2 [4] = {3000, 2500, 1500, 800};

private:
  std::vector<Int_t>    L1L2_SSDNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L2BStripNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L2FStripNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L1SStripNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L2FTime[NUM_SSD];

  std::vector<Double_t> L1L2_L2BEMeV[NUM_SSD];
  std::vector<Double_t> L1L2_L2FEMeV[NUM_SSD];
  std::vector<Double_t> L1L2_L1SEMeV[NUM_SSD];
  std::vector<Double_t> L1L2_L1SEMeV_Corrected[NUM_SSD];

  Int_t       fZone_index;
  const Int_t fZone_length = 8;
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo



//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//                           fglobalmulti == 4
//                        --------------------
class L1L2_DecodefGlobalMulti4
{
public:
  L1L2_DecodefGlobalMulti4();
  L1L2_DecodefGlobalMulti4(Int_t firstrun, Int_t lastrun);
  ~L1L2_DecodefGlobalMulti4();

  virtual void    L1L2_GlobalMulti4_Decode_011();
  virtual void    L1L2_GlobalMulti4_Decode_101();
  virtual void    L1L2_GlobalMulti4_Decode_110();
  virtual void    L1L2_GlobalMulti4_Decode_111();

private:
  Int_t                       fFirstRun;
  Int_t                       fLastRun;
  TimeAndPercentage           timeper;
  CSHINETrackReconstruction  *trackreconstruct;
  CSHINEDEEFITPID             fPID;

  std::vector<TCutG*>         bananacut[NUM_SSD];


private:
  Double_t EL1Range[4] = {400., 320., 150., 100.};
	Int_t    NBinsL1 [4] = {4000, 3200, 1500, 1000};
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2 [4] = {3000, 2500, 1500, 800};

private:
  std::vector<Int_t>    L1L2_SSDNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L2BStripNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L2FStripNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L1SStripNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L2FTime[NUM_SSD];

  std::vector<Double_t> L1L2_L2BEMeV[NUM_SSD];
  std::vector<Double_t> L1L2_L2FEMeV[NUM_SSD];
  std::vector<Double_t> L1L2_L1SEMeV[NUM_SSD];
  std::vector<Double_t> L1L2_L1SEMeV_Corrected[NUM_SSD];

  Int_t       fZone_index;
  const Int_t fZone_length = 8;
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//                           fglobalmulti == 6
//                        --------------------
class L1L2_DecodefGlobalMulti6
{
public:
  L1L2_DecodefGlobalMulti6();
  L1L2_DecodefGlobalMulti6(Int_t firstrun, Int_t lastrun);
  ~L1L2_DecodefGlobalMulti6();

  virtual void    L1L2_GlobalMulti6_Decode_111();
  virtual void    L1L2_GlobalMulti6_Decode_112();
  virtual void    L1L2_GlobalMulti6_Decode_120();
  virtual void    L1L2_GlobalMulti6_Decode_121();
  virtual void    L1L2_GlobalMulti6_Decode_211();


private:
  Int_t                       fFirstRun;
  Int_t                       fLastRun;
  TimeAndPercentage           timeper;
  CSHINETrackReconstruction  *trackreconstruct;
  CSHINEDEEFITPID             fPID;

  std::vector<TCutG*>         bananacut[NUM_SSD];


private:
  Double_t EL1Range[4] = {400., 320., 150., 100.};
	Int_t    NBinsL1 [4] = {4000, 3200, 1500, 1000};
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2 [4] = {3000, 2500, 1500, 800};

private:
  std::vector<Int_t>    L1L2_SSDNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L2BStripNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L2FStripNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L1SStripNum[NUM_SSD];
  std::vector<Int_t>    L1L2_L2FTime[NUM_SSD];

  std::vector<Double_t> L1L2_L2BEMeV[NUM_SSD];
  std::vector<Double_t> L1L2_L2FEMeV[NUM_SSD];
  std::vector<Double_t> L1L2_L1SEMeV[NUM_SSD];
  std::vector<Double_t> L1L2_L1SEMeV_Corrected[NUM_SSD];

  Int_t       fZone_index;
  const Int_t fZone_length = 8;
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo

#endif
