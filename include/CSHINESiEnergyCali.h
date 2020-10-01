#ifndef CSHINESIENERGYCALI_H
#define CSHINESIENERGYCALI_H


//______________________________________________________________________________
class PulserCalibration
{
public:
  PulseCalibration();
  ~PulseCalibration();

  void    L1_AutoFindPeaksAndFit();
  void    L2_AutoFindPeaksAndFit();
  void    L1_AutoFindPeaksAndFit_ReCali();
  void    L1_GainEffectOnPulseCali();

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
};


//______________________________________________________________________________
class SiEnergiCalibration
{
public:
  SiEnergiCalibration();
  ~SiEnergiCalibration();

  void   CheckAbnormalChannels();
  void   PulseAndAlphaCali();
  void   EstimatePedestals();
  void   EstimateFitPars();
  void   EstimateDeadlayerEffects();
  void   EstimateAlphaChannelEffOnFitPars();
  void   FinallyDeterminedFitPars();
};

#endif
