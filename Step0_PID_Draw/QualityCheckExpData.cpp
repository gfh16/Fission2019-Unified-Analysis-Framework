void QualityCheckExpData()
{
  const int FirstRun = 121;
  const int LastRun  = 200;

  TChain myData("KrPb");

/*
  for(int i=FirstRun; i<LastRun; i++)
  {
    myData.Add(Form("../../Fission2019_Data/MapRoot/MapFission2019_Kr_Pb%04d.root", i));
  }
*/
  myData.Add("../../Fission2019_Data/MapRoot/MapFission2019_Kr_Pb*.root");

  TCanvas *c1 = new TCanvas("c1","c1", 1200, 800);

  myData.Draw("SSD4_L2B_E[4]:SSD4_L3A_E[6]>>hist1(4096,0,4096,4096,0,4096)");
  TH2F * hist1 = (TH2F*)gDirectory->Get("hist1");
  hist1->Draw("COLZ");
/*
  char SSD_E[200];
  for(int i=0; i<16; i++)
  {
    sprintf(SSD_E,"SSD1_L1S_E[%d]",i);
    myData.Draw(SSD_E);

    gPad->Modified();
    gPad->Update();
    gPad->SetLogy();

    getchar();
  }
*/

}
