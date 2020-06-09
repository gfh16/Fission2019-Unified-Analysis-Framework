////////////////////////////////////////////////////////////////////////////////
//
//   1. 在 Fission2019 实验中, 对于 SSD_L1 的放射源刻度, 我们进行了两次, 这是因为
//      第一次刻度时, 有些条没有刻度到. 因此第一次刻度完, 又开了第二次靶室.
//   2. 第一次刻度文件 SSD_L1_AlphaCali0000-0004,
//      第二次刻度文件 SSD_L1_AlphaCali0005-0008
//      在合并两次刻度文件时发现：SSD4_L1 前后两次刻度的 alpha 峰位发生偏移,
//      因此需要分开两次刻度, 再对比差异
//
//   3. 在 Layer2 的 alpha 刻度时, SSD4_L2F_CH13 与 SSD4_L2F_CH14 两道
//      出现两组 alpha 峰, 需要分析一下
//
//   4. 这个 macro 的作用就是分析这两次刻度的差异
//
//   gfh, 2020-06-07
////////////////////////////////////////////////////////////////////////////////

#include "include/ReadFileModule.h"

Int_t SSDNum = 4;
Int_t CHNum  = 16;
Int_t numpar = 3;

Float_t small = 0.001;


//______________________________________
void AlphaCali_SSD4_L1S_ReCali();
void AlphaCali_SSD4_L2F_ReCali();



//__________________________________
void AlphaCali_SSD4_ReCali()
{
  AlphaCali_SSD4_L1S_ReCali();
  AlphaCali_SSD4_L2F_ReCali();
}


//______________________________________
void AlphaCali_SSD4_L1S_ReCali()
{
  gStyle->SetPalette(1);

  Double_t CHIndex[CHNum];
  Double_t Peak1_00_04[CHNum];
  Double_t Peak2_00_04[CHNum];
  Double_t Peak3_00_04[CHNum];
  Double_t Peak1_05_08[CHNum];
  Double_t Peak2_05_08[CHNum];
  Double_t Peak3_05_08[CHNum];
  Double_t err_Peak1[CHNum];
  Double_t err_Peak2[CHNum];
  Double_t err_Peak3[CHNum];
  Double_t relative_err_Peak1[CHNum];
  Double_t relative_err_Peak2[CHNum];
  Double_t relative_err_Peak3[CHNum];

  std::string AlphaCali00_04("AlphaCali00_04");
  std::string AlphaCali05_08("AlphaCali05_08");

  std::string pathAlphaCali00_04(Form("output/SSD_L1S_AlphaPeaks_%s.dat",AlphaCali00_04.c_str()));
  std::string pathAlphaCali05_08(Form("output/SSD_L1S_AlphaPeaks_%s.dat",AlphaCali05_08.c_str()));

  std::string pathPNGOutput("figures/SSD4_L1S_AlphaReCali.png");

  ReadFileModule readfile;

  Double_t*** AlphaPeaks00_04 = readfile.ReadData(pathAlphaCali00_04.c_str(),SSDNum,CHNum,numpar);
  Double_t*** AlphaPeaks05_08 = readfile.ReadData(pathAlphaCali05_08.c_str(),SSDNum,CHNum,numpar);

  for (Int_t j=0; j<CHNum; j++)
  {
    CHIndex[j] = j+1;
    Peak1_00_04[j] = AlphaPeaks00_04[3][j][0];
    Peak2_00_04[j] = AlphaPeaks00_04[3][j][1];
    Peak3_00_04[j] = AlphaPeaks00_04[3][j][2];
    Peak1_05_08[j] = AlphaPeaks05_08[3][j][0];
    Peak2_05_08[j] = AlphaPeaks05_08[3][j][1];
    Peak3_05_08[j] = AlphaPeaks05_08[3][j][2];

    err_Peak1[j] = Peak1_05_08[j] - Peak1_00_04[j];
    err_Peak2[j] = Peak2_05_08[j] - Peak2_00_04[j];
    err_Peak3[j] = Peak3_05_08[j] - Peak3_00_04[j];

    if (Peak1_00_04[j]==0 || Peak2_00_04[j]==0 || Peak3_00_04[j]==0) {
      relative_err_Peak1[j] = err_Peak1[j]/1.;
      relative_err_Peak2[j] = err_Peak2[j]/1.;
      relative_err_Peak3[j] = err_Peak3[j]/1.;
    } else {
      relative_err_Peak1[j] = err_Peak1[j]/Peak1_00_04[j] * 100;
      relative_err_Peak2[j] = err_Peak2[j]/Peak2_00_04[j] * 100;
      relative_err_Peak3[j] = err_Peak3[j]/Peak3_00_04[j] * 100;
    }

  }

  TGraph* gr1_00_04 = new TGraph(CHNum,CHIndex,Peak1_00_04);
  TGraph* gr2_00_04 = new TGraph(CHNum,CHIndex,Peak2_00_04);
  TGraph* gr3_00_04 = new TGraph(CHNum,CHIndex,Peak3_00_04);
  TGraph* gr1_05_08 = new TGraph(CHNum,CHIndex,Peak1_05_08);
  TGraph* gr2_05_08 = new TGraph(CHNum,CHIndex,Peak2_05_08);
  TGraph* gr3_05_08 = new TGraph(CHNum,CHIndex,Peak3_05_08);
  TGraph* gr1_err = new TGraph(CHNum,CHIndex,err_Peak1);
  TGraph* gr2_err = new TGraph(CHNum,CHIndex,err_Peak2);
  TGraph* gr3_err = new TGraph(CHNum,CHIndex,err_Peak3);
  TGraph* gr1_relative_err = new TGraph(CHNum,CHIndex,relative_err_Peak1);
  TGraph* gr2_relative_err = new TGraph(CHNum,CHIndex,relative_err_Peak2);
  TGraph* gr3_relative_err = new TGraph(CHNum,CHIndex,relative_err_Peak3);
  TLegend* legend = new TLegend(0.55,0.25,0.88,0.4);

  gr1_00_04->GetXaxis()->SetRangeUser(-1,17);
  gr1_00_04->GetXaxis()->SetNdivisions(118);
  gr2_00_04->GetXaxis()->SetRangeUser(-1,17);
  gr2_00_04->GetXaxis()->SetNdivisions(118);
  gr3_00_04->GetXaxis()->SetRangeUser(-1,17);
  gr3_00_04->GetXaxis()->SetNdivisions(118);

  gr1_05_08->GetXaxis()->SetRangeUser(-1,17);
  gr1_05_08->GetXaxis()->SetNdivisions(118);
  gr1_05_08->GetXaxis()->SetRangeUser(-1,17);
  gr1_05_08->GetXaxis()->SetNdivisions(118);
  gr1_05_08->GetXaxis()->SetRangeUser(-1,17);
  gr1_05_08->GetXaxis()->SetNdivisions(118);

  gr1_err->GetXaxis()->SetRangeUser(-1,17);
  gr1_err->GetXaxis()->SetNdivisions(118);
  gr1_err->GetXaxis()->SetRangeUser(-1,17);
  gr1_err->GetXaxis()->SetNdivisions(118);
  gr1_err->GetXaxis()->SetRangeUser(-1,17);
  gr1_err->GetXaxis()->SetNdivisions(118);

  gr1_relative_err->GetXaxis()->SetRangeUser(-1,17);
  gr1_relative_err->GetXaxis()->SetNdivisions(118);
  gr2_relative_err->GetXaxis()->SetRangeUser(-1,17);
  gr2_relative_err->GetXaxis()->SetNdivisions(118);
  gr3_relative_err->GetXaxis()->SetRangeUser(-1,17);
  gr3_relative_err->GetXaxis()->SetNdivisions(118);

  gr1_00_04->SetMarkerStyle(20);
  gr1_00_04->SetMarkerColor(kRed);
  gr2_00_04->SetMarkerStyle(20);
  gr2_00_04->SetMarkerColor(kGreen);
  gr3_00_04->SetMarkerStyle(20);
  gr3_00_04->SetMarkerColor(kBlue);

  gr1_05_08->SetMarkerStyle(20);
  gr1_05_08->SetMarkerColor(kRed);
  gr2_05_08->SetMarkerStyle(20);
  gr2_05_08->SetMarkerColor(kGreen);
  gr3_05_08->SetMarkerStyle(20);
  gr3_05_08->SetMarkerColor(kBlue);

  gr1_err->SetMarkerStyle(20);
  gr1_err->SetMarkerColor(kRed);
  gr2_err->SetMarkerStyle(20);
  gr2_err->SetMarkerColor(kGreen);
  gr3_err->SetMarkerStyle(20);
  gr3_err->SetMarkerColor(kBlue);

  gr1_relative_err->SetMarkerStyle(20);
  gr1_relative_err->SetMarkerColor(kRed);
  gr2_relative_err->SetMarkerStyle(20);
  gr2_relative_err->SetMarkerColor(kGreen);
  gr3_relative_err->SetMarkerStyle(20);
  gr3_relative_err->SetMarkerColor(kBlue);

  TMultiGraph* mgr_00_04 = new TMultiGraph();
  TMultiGraph* mgr_05_08 = new TMultiGraph();
  TMultiGraph* mgr_err   = new TMultiGraph();
  TMultiGraph* mgr_relative_err   = new TMultiGraph();
  mgr_00_04->Add(gr1_00_04);
  mgr_00_04->Add(gr2_00_04);
  mgr_00_04->Add(gr3_00_04);
  mgr_05_08->Add(gr1_05_08);
  mgr_05_08->Add(gr2_05_08);
  mgr_05_08->Add(gr3_05_08);
  mgr_err->Add(gr1_err);
  mgr_err->Add(gr2_err);
  mgr_err->Add(gr3_err);
  mgr_relative_err->Add(gr1_relative_err);
  mgr_relative_err->Add(gr2_relative_err);
  mgr_relative_err->Add(gr3_relative_err);

  legend->AddEntry(gr1_00_04,"peak1","lp");
  legend->AddEntry(gr2_00_04,"peak2","lp");
  legend->AddEntry(gr3_00_04,"peak3","lp");

  TCanvas* cans = new TCanvas("cans","cans",1000,1000);
  cans->Divide(2,2,small,small);

  cans->cd(1);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  mgr_00_04->SetTitle(AlphaCali00_04.c_str());
  mgr_00_04->GetXaxis()->SetRangeUser(-1,17);
  mgr_00_04->GetXaxis()->SetNdivisions(118);
  mgr_00_04->GetXaxis()->SetTitle("SSD Strip");
  mgr_00_04->GetXaxis()->SetTitleSize(0.04);
  mgr_00_04->GetXaxis()->CenterTitle(1);
  mgr_00_04->GetYaxis()->SetRangeUser(0.,400.);
  mgr_00_04->GetYaxis()->SetTitle("ADC Channel");
  mgr_00_04->GetYaxis()->SetTitleSize(0.04);
  mgr_00_04->GetYaxis()->CenterTitle(1);
  mgr_00_04->Draw("APL");
  legend->Draw("SAME");

  cans->cd(2);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  mgr_05_08->SetTitle(AlphaCali05_08.c_str());
  mgr_05_08->GetXaxis()->SetRangeUser(-1,17);
  mgr_05_08->GetXaxis()->SetNdivisions(118);
  mgr_05_08->GetXaxis()->SetTitle("SSD Strip");
  mgr_05_08->GetXaxis()->SetTitleSize(0.04);
  mgr_05_08->GetXaxis()->CenterTitle(1);
  mgr_05_08->GetYaxis()->SetRangeUser(0.,400.);
  mgr_05_08->GetYaxis()->SetTitle("ADC Channel");
  mgr_05_08->GetYaxis()->SetTitleSize(0.04);
  mgr_05_08->GetYaxis()->CenterTitle(1);
  mgr_05_08->Draw("APL");

  cans->cd(3);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  mgr_err->GetXaxis()->SetRangeUser(-1,17);
  mgr_err->GetXaxis()->SetNdivisions(118);
  mgr_err->GetXaxis()->SetTitle("SSD Strip");
  mgr_err->GetXaxis()->SetTitleSize(0.04);
  mgr_err->GetXaxis()->CenterTitle(1);
  mgr_err->GetYaxis()->SetRangeUser(0.,15.);
  mgr_err->GetYaxis()->SetNdivisions(515);
  mgr_err->GetYaxis()->SetTitle("Errors (Peak Shift)");
  mgr_err->GetYaxis()->SetTitleSize(0.04);
  mgr_err->GetYaxis()->CenterTitle(1);
  mgr_err->Draw("APL");

  cans->cd(4);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  mgr_relative_err->GetXaxis()->SetRangeUser(-1,17);
  mgr_relative_err->GetXaxis()->SetNdivisions(118);
  mgr_relative_err->GetXaxis()->SetTitle("SSD Strip");
  mgr_relative_err->GetXaxis()->SetTitleSize(0.04);
  mgr_relative_err->GetXaxis()->CenterTitle(1);
  mgr_relative_err->GetYaxis()->SetRangeUser(0.,5.);
  mgr_relative_err->GetYaxis()->SetNdivisions(1005);
  mgr_relative_err->GetYaxis()->SetTitle("Relative Errors (%)");
  mgr_relative_err->GetYaxis()->SetTitleSize(0.04);
  mgr_relative_err->GetYaxis()->CenterTitle(1);
  mgr_relative_err->Draw("APL");

  cans->Print(pathPNGOutput.c_str());
}


//________________________________
void AlphaCali_SSD4_L2F_ReCali()
{

  Int_t PeakNum = 3;
  Double_t PeakIndex[3] = {1., 2., 3.};

  Double_t SSD4_CH13_LowPeaks[3]  = {166.966, 172.312, 177.245};
  Double_t SSD4_CH13_HighPeaks[3] = {330.585, 347.711, 364.161};
  Double_t SSD4_CH14_LowPeaks[3]  = {168.828, 174.303, 179.306};
  Double_t SSD4_CH14_HighPeaks[3] = {331.057, 348.248, 364.35};

  std::string pathPNGOut("figures/SSD4_L2F_CH13-14_AlphaCali00_48.png");

  Double_t err_CH13[PeakNum];
  Double_t err_CH14[PeakNum];
  for (Int_t i=0; i<PeakNum; i++)
  {
    err_CH13[i] = 100*(SSD4_CH13_HighPeaks[i]-SSD4_CH13_LowPeaks[i])/(SSD4_CH13_LowPeaks[i]);
    err_CH14[i] = 100*(SSD4_CH14_HighPeaks[i]-SSD4_CH14_LowPeaks[i])/(SSD4_CH14_LowPeaks[i]);
  }

  TCanvas* cans_CH13_14 = new TCanvas("cans_CH13_14","cans_CH13_14",1000,1000);
  cans_CH13_14->Divide(2,2,small,small);

  TGraph* grap_CH13_LowPeaks  = new TGraph(PeakNum,PeakIndex,SSD4_CH13_LowPeaks);
  TGraph* grap_CH13_HighPeaks = new TGraph(PeakNum,PeakIndex,SSD4_CH13_HighPeaks);
  TGraph* grap_CH14_LowPeaks  = new TGraph(PeakNum,PeakIndex,SSD4_CH14_LowPeaks);
  TGraph* grap_CH14_HighPeaks = new TGraph(PeakNum,PeakIndex,SSD4_CH14_HighPeaks);
  TGraph* grap_err_CH13 = new TGraph(PeakNum,PeakIndex,err_CH13);
  TGraph* grap_err_CH14 = new TGraph(PeakNum,PeakIndex,err_CH14);
  TMultiGraph* mg_CH13 = new TMultiGraph();
  TMultiGraph* mg_CH14 = new TMultiGraph();

  grap_CH13_LowPeaks ->SetMarkerStyle(20);
  grap_CH13_LowPeaks ->SetMarkerColor(kRed);
  grap_CH13_LowPeaks ->SetMarkerSize(1.2);
  grap_CH13_HighPeaks->SetMarkerStyle(20);
  grap_CH13_HighPeaks->SetMarkerColor(kBlue);
  grap_CH13_HighPeaks->SetMarkerSize(1.2);

  grap_CH14_LowPeaks ->SetMarkerStyle(20);
  grap_CH14_LowPeaks ->SetMarkerColor(kRed);
  grap_CH14_LowPeaks ->SetMarkerSize(1.2);
  grap_CH14_HighPeaks->SetMarkerStyle(20);
  grap_CH14_HighPeaks->SetMarkerColor(kBlue);
  grap_CH14_HighPeaks ->SetMarkerSize(1.2);

  grap_err_CH13->SetMarkerStyle(20);
  grap_err_CH13->SetMarkerColor(kGreen);
  grap_err_CH13->SetMarkerSize(1.2);
  grap_err_CH14->SetMarkerStyle(20);
  grap_err_CH14->SetMarkerColor(kGreen);
  grap_err_CH14->SetMarkerSize(1.2);

  mg_CH13->Add(grap_CH13_LowPeaks);
  mg_CH13->Add(grap_CH13_HighPeaks);
  mg_CH14->Add(grap_CH14_LowPeaks);
  mg_CH14->Add(grap_CH14_HighPeaks);

  cans_CH13_14->cd(1);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  mg_CH13->SetTitle("SSD4_L2F_CH13_AlphaCali00_48");
  mg_CH13->GetXaxis()->SetRangeUser(0.,4.);
  mg_CH13->GetXaxis()->SetNdivisions(104);
  mg_CH13->GetXaxis()->SetTitle("Peak");
  mg_CH13->GetXaxis()->SetTitleSize(0.04);
  mg_CH13->GetXaxis()->CenterTitle(1);
  mg_CH13->GetYaxis()->SetRangeUser(100.,400.);
  mg_CH13->GetXaxis()->SetNdivisions(310);
  mg_CH13->GetYaxis()->SetTitle("ADC Channel");
  mg_CH13->GetYaxis()->SetTitleSize(0.04);
  mg_CH13->GetYaxis()->CenterTitle(1);
  mg_CH13->Draw("APL");

  cans_CH13_14->cd(2);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  mg_CH14->SetTitle("SSD4_L2F_CH13_AlphaCali00_48");
  mg_CH14->GetXaxis()->SetRangeUser(0.,4.);
  mg_CH14->GetXaxis()->SetNdivisions(104);
  mg_CH14->GetXaxis()->SetTitle("Peak");
  mg_CH14->GetXaxis()->SetTitleSize(0.04);
  mg_CH14->GetXaxis()->CenterTitle(1);
  mg_CH14->GetYaxis()->SetRangeUser(100.,400.);
  mg_CH14->GetXaxis()->SetNdivisions(310);
  mg_CH14->GetYaxis()->SetTitle("ADC Channel");
  mg_CH14->GetYaxis()->SetTitleSize(0.04);
  mg_CH14->GetYaxis()->CenterTitle(1);
  mg_CH14->Draw("APL");

  cans_CH13_14->cd(3);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  grap_err_CH13->SetTitle("SSD4_L2F_CH13_AlphaCali00_48");
  grap_err_CH13->GetXaxis()->SetRangeUser(0.,4.);
  grap_err_CH13->GetXaxis()->SetNdivisions(104);
  grap_err_CH13->GetXaxis()->SetTitle("Peak");
  grap_err_CH13->GetXaxis()->SetTitleSize(0.04);
  grap_err_CH13->GetXaxis()->CenterTitle(1);
  grap_err_CH13->GetYaxis()->SetRangeUser(90.,110.);
  grap_err_CH13->GetYaxis()->SetNdivisions(510);
  grap_err_CH13->GetYaxis()->SetTitle("Relative Errors (%)");
  grap_err_CH13->GetYaxis()->SetTitleSize(0.04);
  grap_err_CH13->GetYaxis()->CenterTitle(1);
  grap_err_CH13->Draw("APL");

  cans_CH13_14->cd(4);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  grap_err_CH14->SetTitle("SSD4_L2F_CH14_AlphaCali00_48");
  grap_err_CH14->GetXaxis()->SetRangeUser(0.,4.);
  grap_err_CH14->GetXaxis()->SetNdivisions(104);
  grap_err_CH14->GetXaxis()->SetTitle("Peak");
  grap_err_CH14->GetXaxis()->SetTitleSize(0.04);
  grap_err_CH14->GetXaxis()->CenterTitle(1);
  grap_err_CH14->GetYaxis()->SetRangeUser(90.,110.);
  grap_err_CH14->GetYaxis()->SetNdivisions(510);
  grap_err_CH14->GetYaxis()->SetTitle("Relative Errors (%)");
  grap_err_CH14->GetYaxis()->SetTitleSize(0.04);
  grap_err_CH14->GetYaxis()->CenterTitle(1);
  grap_err_CH14->Draw("APL");

  cans_CH13_14->Print(pathPNGOut.c_str());

}
