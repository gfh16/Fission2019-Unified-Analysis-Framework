#ifndef L2L3TRACKFINDING_H
#define L2L3TRACKFINDING_H

#include "EnergyLossModule.h"
#include "CSHINESSDCalibratedData.h"
#include "TimeAndPercentage.h"
#include "shared.h"
#include "CSHINEEvent.h"
#include "CSHINETrackReconstruction.h"

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
class DecodefGlobalMulti1
{
public:
  DecodefGlobalMulti1();
  DecodefGlobalMulti1(Int_t firstrun, Int_t lastrun);
  ~DecodefGlobalMulti1();

  virtual void    GlobalMulti1_Checks();

  virtual void    GlobalMulti1_L2L3_Decode();

  virtual void    GlobalMulti_L2L3_Discard();
  virtual void    GlobalMulti_L2L3_EneL2BL2F_Discard();
  virtual void    GlobalMulti_L2L3_EneL1SL2F_Discard();

private:
  Int_t                       fFirstRun;
  Int_t                       fLastRun;
  TimeAndPercentage           timeper;
  CSHINETrackReconstruction  *trackreconstruct;

  std::vector<TCutG*>         bananacut[NUM_SSD*NUM_CSI];

private:
  Double_t EL1Range[4] = {400., 320., 100., 100.};
	Int_t    NBinsL1 [4] = {4000, 3200, 1000, 1000};
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2 [4] = {3000, 2500, 1500, 800};

private:
  std::vector<Int_t>    L2L3_SSDNum[NUM_SSD];
  std::vector<Int_t>    L2L3_CsINum[NUM_SSD];
  std::vector<Int_t>    L2L3_CSIECh[NUM_SSD];
  std::vector<Double_t> L2L3_L2BEMeV[NUM_SSD];
  std::vector<Double_t> L2L3_L2FEMeV[NUM_SSD];
  std::vector<Double_t> L2L3_L1SEMeV[NUM_SSD];
  std::vector<Int_t>    BananaCut_TelNum[NUM_SSD];

private:
  EnergyLossModule      fEloss;
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//                           fglobalmulti == 2
//                        --------------------
class DecodefGlobalMulti2
{
public:
  DecodefGlobalMulti2();
  DecodefGlobalMulti2(Int_t firstrun, Int_t lastrun);
  ~DecodefGlobalMulti2();

  virtual void    GlobalMulti2_Checks();

  virtual void    GlobalMulti2_Decode_1111();
  virtual void    GlobalMulti2_Decode_1101();
  virtual void    GlobalMulti2_Decode_1010();
  virtual void    GlobalMulti2_Decode_1000();
  virtual void    GlobalMulti2_Decode_0101();
  virtual void    GlobalMulti2_Decode_0100();
  virtual void    GlobalMulti2_Decode_0010();
  virtual void    GlobalMulti2_Decode_0001();


private:
  Int_t                       fFirstRun;
  Int_t                       fLastRun;
  TimeAndPercentage           timeper;
  CSHINETrackReconstruction  *trackreconstruct;
  std::vector<TCutG*>         bananacut[NUM_SSD*NUM_CSI];
  EnergyLossModule            fEloss;

private:
  Double_t EL1Range[4] = {400., 320., 100., 100.};
	Int_t    NBinsL1 [4] = {4000, 3200, 1000, 1000};
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2 [4] = {3000, 2500, 1500, 800};

  std::vector<Int_t>    L2L3_SSDNum[NUM_SSD];
  std::vector<Int_t>    L2L3_CsINum[NUM_SSD];
  std::vector<Int_t>    L2L3_L2BStripNum[NUM_SSD];
  std::vector<Int_t>    L2L3_L2FStripNum[NUM_SSD];
  std::vector<Int_t>    L2L3_L1SStripNum[NUM_SSD];
  std::vector<Int_t>    L2L3_CSIECh[NUM_SSD];
  std::vector<Double_t> L2L3_L2BEMeV[NUM_SSD];
  std::vector<Double_t> L2L3_L2FEMeV[NUM_SSD];
  std::vector<Double_t> L2L3_L1SEMeV[NUM_SSD];
  std::vector<Int_t>    BananaCut_TelNum[NUM_SSD];
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//                           fglobalmulti == 3
//                        --------------------
class DecodefGlobalMulti3
{
public:
	DecodefGlobalMulti3();
	DecodefGlobalMulti3(Int_t firstrun, Int_t lastrun);
	~DecodefGlobalMulti3();

public:
  virtual void    GlobalMulti3_Checks();

	virtual void    GlobalMulti3_Decode_2222();
	virtual void    GlobalMulti3_Decode_2111();
	virtual void    GlobalMulti3_Decode_1211();
	virtual void    GlobalMulti3_Decode_1121();
	virtual void    GlobalMulti3_Decode_1112();
	virtual void    GlobalMulti3_Decode_1111();
	virtual void    GlobalMulti3_Decode_1110();
	virtual void    GlobalMulti3_Decode_1020();
	virtual void    GlobalMulti3_Decode_1010();
	virtual void    GlobalMulti3_Decode_0201();
  virtual void    GlobalMulti3_Decode_0102();
	virtual void    GlobalMulti3_Decode_0101();
  virtual void    GlobalMulti3_Decode_0020();
	virtual void    GlobalMulti3_Decode_0002();

private:
  Int_t                       fFirstRun;
  Int_t                       fLastRun;
  TimeAndPercentage           timeper;
  CSHINETrackReconstruction  *trackreconstruct;
  std::vector<TCutG*>         bananacut[NUM_SSD*NUM_CSI];
  EnergyLossModule            fEloss;

private:
  Double_t EL1Range[4] = {400., 320., 100., 100.};
	Int_t    NBinsL1 [4] = {4000, 3200, 1000, 1000};
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2 [4] = {3000, 2500, 1500, 800};

  std::vector<Int_t>    L2L3_SSDNum[NUM_SSD];
  std::vector<Int_t>    L2L3_CsINum[NUM_SSD];
  std::vector<Int_t>    L2L3_L2BStripNum[NUM_SSD];
  std::vector<Int_t>    L2L3_L2FStripNum[NUM_SSD];
  std::vector<Int_t>    L2L3_L1SStripNum[NUM_SSD];
  std::vector<Int_t>    L2L3_CSIECh[NUM_SSD];
  std::vector<Double_t> L2L3_L2BEMeV[NUM_SSD];
  std::vector<Double_t> L2L3_L2FEMeV[NUM_SSD];
  std::vector<Double_t> L2L3_L1SEMeV[NUM_SSD];
  std::vector<Int_t>    BananaCut_TelNum[NUM_SSD];
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//                           fglobalmulti == 4
//                        --------------------
class DecodefGlobalMulti4
{
public:
	DecodefGlobalMulti4();
	DecodefGlobalMulti4(Int_t firstrun, Int_t lastrun);
	~DecodefGlobalMulti4();

public:
  virtual void    GlobalMulti4_Checks();

  virtual void    GlobalMulti4_Decode_2222();
	virtual void    GlobalMulti4_Decode_1111();
	virtual void    GlobalMulti4_Decode_1110();
	virtual void    GlobalMulti4_Decode_1011();
	virtual void    GlobalMulti4_Decode_0111();
	virtual void    GlobalMulti4_Decode_0110();
	virtual void    GlobalMulti4_Decode_0101();
	virtual void    GlobalMulti4_Decode_0011();


private:
  Int_t                       fFirstRun;
  Int_t                       fLastRun;
  TimeAndPercentage           timeper;
  CSHINETrackReconstruction  *trackreconstruct;
  std::vector<TCutG*>         bananacut[NUM_SSD*NUM_CSI];
  EnergyLossModule            fEloss;

private:
  Double_t EL1Range[4] = {400., 320., 100., 100.};
	Int_t    NBinsL1 [4] = {4000, 3200, 1000, 1000};
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2 [4] = {3000, 2500, 1500, 800};

  std::vector<Int_t>    L2L3_SSDNum[NUM_SSD];
  std::vector<Int_t>    L2L3_CsINum[NUM_SSD];
  std::vector<Int_t>    L2L3_L2BStripNum[NUM_SSD];
  std::vector<Int_t>    L2L3_L2FStripNum[NUM_SSD];
  std::vector<Int_t>    L2L3_L1SStripNum[NUM_SSD];
  std::vector<Int_t>    L2L3_CSIECh[NUM_SSD];
  std::vector<Double_t> L2L3_L2BEMeV[NUM_SSD];
  std::vector<Double_t> L2L3_L2FEMeV[NUM_SSD];
  std::vector<Double_t> L2L3_L1SEMeV[NUM_SSD];
  std::vector<Int_t>    BananaCut_TelNum[NUM_SSD];
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//                           fglobalmulti == 6
//                        --------------------
class DecodefGlobalMulti6
{
public:
	DecodefGlobalMulti6();
	DecodefGlobalMulti6(Int_t firstrun, Int_t lastrun);
	~DecodefGlobalMulti6();

public:
  virtual void     GlobalMulti6_Checks();


//______________________________________________________________________________
  virtual void    GlobalMulti6_Decode_2222();
  virtual void    GlobalMulti6_Decode_1211();
	virtual void    GlobalMulti6_Decode_1121();
	virtual void    GlobalMulti6_Decode_1112();
	virtual void    GlobalMulti6_Decode_1111();
	virtual void    GlobalMulti6_Decode_1110();
	virtual void    GlobalMulti6_Decode_1011();
	virtual void    GlobalMulti6_Decode_0211();
	virtual void    GlobalMulti6_Decode_0121();
	virtual void    GlobalMulti6_Decode_0120();
	virtual void    GlobalMulti6_Decode_0112();
  virtual void    GlobalMulti6_Decode_0111();
	virtual void    GlobalMulti6_Decode_0021();
	virtual void    GlobalMulti6_Decode_0012();


//______________________________________________________________________________
private:
  Int_t                       fFirstRun;
  Int_t                       fLastRun;
  TimeAndPercentage           timeper;
  CSHINETrackReconstruction  *trackreconstruct;
  std::vector<TCutG*>         bananacut[NUM_SSD*NUM_CSI];
  EnergyLossModule            fEloss;

private:
  Double_t EL1Range[4] = {400., 320., 100., 100.};
	Int_t    NBinsL1 [4] = {4000, 3200, 1000, 1000};
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2 [4] = {3000, 2500, 1500, 800};

  std::vector<Int_t>    L2L3_SSDNum[NUM_SSD];
  std::vector<Int_t>    L2L3_CsINum[NUM_SSD];
  std::vector<Int_t>    L2L3_L2BStripNum[NUM_SSD];
  std::vector<Int_t>    L2L3_L2FStripNum[NUM_SSD];
  std::vector<Int_t>    L2L3_L1SStripNum[NUM_SSD];
  std::vector<Int_t>    L2L3_CSIECh[NUM_SSD];
  std::vector<Double_t> L2L3_L2BEMeV[NUM_SSD];
  std::vector<Double_t> L2L3_L2FEMeV[NUM_SSD];
  std::vector<Double_t> L2L3_L1SEMeV[NUM_SSD];
  std::vector<Int_t>    BananaCut_TelNum[NUM_SSD];
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo

#endif
