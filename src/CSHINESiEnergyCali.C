#include "../include/CSHINESiEnergyCali.h"
using namespace std;


//******************************************************************************
PulserCalibration::PulserCalibration()
{}

//_____________________________________________
PulserCalibration::~PulserCalibration()
{}

//______________________________________________
void PulserCalibration::L1_AutoFindPeaksAndFit()
{

}

//______________________________________________
void PulserCalibration::L2_AutoFindPeaksAndFit()
{}

//_____________________________________________________
void PulserCalibration::L1_AutoFindPeaksAndFit_ReCali()
{}

//________________________________________________
void PulserCalibration::L1_GainEffectOnPulseCali()
{}

//_________________________________________________
void PulserCalibration::CaliCheck_SSD_L2F_CH00_01()
{}

//******************************************************************************



//******************************************************************************
AlphaCalibration::AlphaCalibration()
{}

//______________________________________
AlphaCalibration::~AlphaCalibration()
{}

//___________________________________________
void AlphaCalibration::CalculateAlphaEnergy()
{}

//___________________________________________
void AlphaCalibration::MergeAlphaCaliFiles()
{}

//________________________________________
void AlphaCalibration::CheckAphaPeaks()
{}

//_______________________________________________
void AlphaCalibration::ClickToFindAlphaPeaks()
{}

//__________________________________________________
void AlphaCalibration::CheckAbnormaleChannels_SSD4()
{}

void AlphaCalibration::SetPoints(Int_t event, Int_t x, Int_t y, TObject* selected)
{}
//******************************************************************************


//******************************************************************************
SiEnergiCalibration::SiEnergiCalibration()
{}

//__________________________________________
SiEnergiCalibration::~SiEnergiCalibration()
{}

//_______________________________________________
void SiEnergiCalibration::CheckAbnormalChannels()
{}

//_____________________________________________
void SiEnergiCalibration::PulserAndAlphaCali()
{}

//___________________________________________
void SiEnergiCalibration::EstimatePedestals()
{}

//__________________________________________
void SiEnergiCalibration::EstimateFitPars()
{}

//__________________________________________________
void SiEnergiCalibration::EstimateDeadlayerEffects()
{}

//__________________________________________________________
void SiEnergiCalibration::EstimateAlphaChannelEffOnFitPars()
{}

//__________________________________________________
void SiEnergiCalibration::FinallyDeterminedFitPars()
{}

//******************************************************************************
