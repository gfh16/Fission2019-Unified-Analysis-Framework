{
  gROOT->ProcessLine(".L ../Nuclear_Masses/nuclear_masses.C");
  gROOT->ProcessLine(".L ../EnergyLossModule.C");

  gROOT->ProcessLine(".L ../src/Vector3.C");
  gROOT->ProcessLine(".L ../src/TimeAndPercentage.C");
  gROOT->ProcessLine(".L ../src/shared.C");
  gROOT->ProcessLine(".L ../src/ReadFileModule.C");

  gROOT->ProcessLine(".L ../src/CSHINESSDCalibratedData.C");
  gROOT->ProcessLine(".L ../src/CSHINESiPixelGeometry.C");
  gROOT->ProcessLine(".L ../src/CSHINEHitPatternRecognition.C");
  gROOT->ProcessLine(".L ../src/CSHINEEvent.C");

  printf("Welcome to Fission2019-Unified-Analysis-Framework/\n");
}
