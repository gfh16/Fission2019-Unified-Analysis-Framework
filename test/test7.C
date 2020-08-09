void test7()
{
  std::string pathRootInput("EventTree_Run0200-Run0300.root");

  TFile f(pathRootInput.c_str());

  TTree* tree = (TTree*)f.Get("CSHINEEvent");

  tree->Draw("SSD2.fEMeVL2B[12]:SSD2.fEChCsI[6]","SSD2.fMultiL2B==1");
}
