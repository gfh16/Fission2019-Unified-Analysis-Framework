{
  gROOT->ProcessLine(".L src/Nuclear_masses.C");
  gROOT->ProcessLine(".L src/EnergyLossModule.C");

  gROOT->ProcessLine(".L src/Vector3.C");
  gROOT->ProcessLine(".L src/TimeAndPercentage.C");
  gROOT->ProcessLine(".L src/shared.C");
  gROOT->ProcessLine(".L src/ReadFileModule.C");
  gROOT->ProcessLine(".L src/CSHINESSDCalibratedData.C");
  gROOT->ProcessLine(".L src/CSHINESiPixelGeometry.C");
  gROOT->ProcessLine(".L src/CSHINETrackReconstruction.C");
  gROOT->ProcessLine(".L src/CSHINEEvent.C");
  gROOT->ProcessLine(".L src/CSHINEPedestals.C");
  gROOT->ProcessLine(".L src/CSHINEQualityCheck.C");
  gROOT->ProcessLine(".L src/CSHINESiEnergyCali.C");
  gROOT->ProcessLine(".L src/CSHINEAlphaCali.C");
  gROOT->ProcessLine(".L src/CSHINEPulserCali.C");
  gROOT->ProcessLine(".L src/CSHINECsIEnergyCali.C");
  gROOT->ProcessLine(".L src/CSHINEParticleIdentification.C");
  gROOT->ProcessLine(".L src/CSHINECsIEnergyCali.C");
  gROOT->ProcessLine(".L src/CSHINEDataAnalysisFramework.C");
  gROOT->ProcessLine(".L src/CSHINESSDDataAnalysisFramework.C");
  gROOT->ProcessLine(".L Test_Multi.C");
  gROOT->ProcessLine(".L ExtractDEEPointsGUI/ExtractDEEPointsGUI.C+");

  printf("Welcome to Fission2019-Unified-Analysis-Framework/\n");
}
