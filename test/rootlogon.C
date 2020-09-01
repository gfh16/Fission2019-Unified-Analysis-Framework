{
  gSystem->Exec("date");
  gROOT->ProcessLine(".L ../src/ReadFileModule.C");
  gROOT->ProcessLine(".L ../src/CSHINESSDCalibratedData.C");
  gROOT->ProcessLine(".L ../src/shared.C");
  gROOT->ProcessLine(".L ../src/CSHINEEvent.C");
  gROOT->ProcessLine(".L ../src/TimeAndPercentage.C");
  gROOT->ProcessLine(".L ../src/CSHINEBuildEvent.C ");
  gROOT->ProcessLine(".L ../src/CSHINEDEELines.C");
  gROOT->ProcessLine(".L ../src/CSHINEHitPatternRecognition.C");
}
