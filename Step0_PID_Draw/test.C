void test()
{
  TFile f("../../Fission2019_Data/MapRoot/MapFission2019_Kr_Pb0200.root");
  TTree *KrPb = (TTree*)f.Get("KrPb");
  Event *myEvent = 0;
  TBranch* brEvent = 0;
  KrPb->SetBranchAddress("SSD1_L2B_E", &myEvent, brEvent);
  KrPb->SetCacheSize(10000000);
  KrPb->AddBranchToCache("SSD1_L2B_E");
  Long64_t nbent = KrPb->GetEntries();

  for (Long64_t i = 0; i < nbent; ++i)
  {
    brEvent->GetEntry(i);
    myEvent->Analyze();
  }
}
