#ifndef DECODEFGLOBALMULTI3_H
#define DECODEFGLOBALMULTI3_H

#include "EnergyLossModule.h"
#include "CSHINESSDCalibratedData.h"
#include "TimeAndPercentage.h"
#include "shared.h"
#include "CSHINEEvent.h"


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


#endif
