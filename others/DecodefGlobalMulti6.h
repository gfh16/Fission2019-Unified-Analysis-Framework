#ifndef DECODEFGLOBALMULTI6_H
#define DECODEFGLOBALMULTI6_H

#include "EnergyLossModule.h"
#include "CSHINESSDCalibratedData.h"
#include "TimeAndPercentage.h"
#include "shared.h"
#include "CSHINEEvent.h"


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


#endif
