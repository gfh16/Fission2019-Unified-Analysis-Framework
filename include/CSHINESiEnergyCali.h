#ifndef CSHINESIENERGYCALI_H
#define CSHINESIENERGYCALI_H


//______________________________________________________________________________
class PulserCalibration
{
public:
  PulserCalibration();
  ~PulserCalibration();

  void   L1_AutoFindPeaksAndFit();
  void   L2_AutoFindPeaksAndFit();
  void   L1_AutoFindPeaksAndFit_ReCali();
  void   L1_GainEffectOnPulseCali();
  void   CaliCheck_SSD_L2F_CH00_01();
};


//______________________________________________________________________________
class AlphaCalibration
{
public:
  AlphaCalibration();
  ~AlphaCalibration();

  void   CalculateAlphaEnergy();
  void   MergeAlphaCaliFiles();
  void   CheckAphaPeaks();
  void   ClickToFindAlphaPeaks();
  void   CheckAbnormaleChannels_SSD4();

  void   SetPoints(Int_t event, Int_t x, Int_t y, TObject* selected); //*SIGNAL*

private:
  Int_t     fIndex = 0;
  Int_t     fNPoints;
  TMarker  *m[2];
  TLine    *l[2];
};


//______________________________________________________________________________
class SiEnergiCalibration
{
public:
  SiEnergiCalibration();
  ~SiEnergiCalibration();

  void   CheckAbnormalChannels();
  void   PulserAndAlphaCali();
  void   EstimatePedestals();
  void   EstimateFitPars();
  void   EstimateDeadlayerEffects();
  void   EstimateAlphaChannelEffOnFitPars();
  void   FinallyDeterminedFitPars();
};

#endif
