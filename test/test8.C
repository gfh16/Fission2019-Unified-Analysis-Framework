
Int_t FirstRun = 200;
Int_t LastRun  = 250;

//______________________________________________________________________________
void test8()
{
  std::string pathRootEventTreeFolder("/home/sea/Fission2019_Data/CSHINEEvent/");
  std::string pathRootInput(Form("%sEventTree_Run%04d-Run%04d.root",pathRootEventTreeFolder.c_str(),
                                  FirstRun,LastRun));

 TString pathOutput(Form("EventTree_Run%04d-Run%04d.pdf",FirstRun,LastRun));

  TFile* file = new TFile(pathRootInput.c_str());
  if (!file->IsOpen()) {
    cout<<Form("*** Error: Open file %s error! ***", pathRootInput.c_str())<<endl;
  }

  TTree* mytree = (TTree*)file->Get("CSHINEEvent");

  CSHINEEventTreeReader treereader(mytree);
  treereader.DEE_All(pathOutput);
}
