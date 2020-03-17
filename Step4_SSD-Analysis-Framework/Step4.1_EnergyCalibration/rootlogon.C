{
  gStyle->SetOptFit(1111111);
  gStyle->SetPalette(1);
  gStyle->SetOptFit(kTRUE);
  gSystem->Exec("date");
  gROOT->ProcessLine(".L Nuclear_Masses/nuclear_masses.cpp");
  gROOT->ProcessLine(".L EnergyLossModule.C");
}
