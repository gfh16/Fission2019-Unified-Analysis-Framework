void QualityCheckExpData()
{
  const int FirstRun = 200;
  const int LastRun  = 300;

  TChain myData("KrPb");

  for(int i=FirstRun; i<=LastRun; i++)
  {
    myData.Add(Form("../../Fission2019_Data/MapRoot/MapFission2019_Kr_Pb%04d.root", i));
  }

  TCanvas *c1 = new TCanvas("c1","c1", 1200, 800);
  c1->Divide(2,2);

  myData.Draw("SSD1_L2B_E[4]:SSD1_L3A_E[6]>>hist1(4096,0,4096,4096,0,4096)");
  myData.Draw("SSD2_L2B_E[4]:SSD2_L3A_E[6]>>hist2(4096,0,4096,4096,0,4096)");
  myData.Draw("SSD3_L2B_E[4]:SSD3_L3A_E[6]>>hist3(4096,0,4096,4096,0,4096)");
  myData.Draw("SSD4_L2B_E[4]:SSD4_L3A_E[6]>>hist4(4096,0,4096,4096,0,4096)");

  TH2D *hist1 = (TH2D*)gDirectory->Get("hist1");
  TH2D *hist2 = (TH2D*)gDirectory->Get("hist2");
  TH2D *hist3 = (TH2D*)gDirectory->Get("hist3");
  TH2D *hist4 = (TH2D*)gDirectory->Get("hist4");

  c1->cd(1);
  hist1->Draw("COLZ");
  c1->cd(2);
  hist2->Draw("COLZ");
  c1->cd(3);
  hist3->Draw("COLZ");
  c1->cd(4);
  hist4->Draw("COLZ");

}
