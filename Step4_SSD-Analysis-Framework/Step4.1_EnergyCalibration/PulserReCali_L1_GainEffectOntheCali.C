////////////////////////////////////////////////////////////////////////////////
//
//   由于前放设置的缘故, 在放射源刻度时, SSD1_L1 与 SSD2_L1 看不到信号，
//   于是，改变了放大倍数：
//     SSD1_L1: gain = 2*4    => 2*20
//     SSD2_L1: gain = 2*7.4  => 2*20
//
//   因此，在脉冲刻度时, 也要相应改变放大倍数, 现在来考察不同 gain 情况下,
//   刻度曲线之间的关系
//
//   gfh, 2020-05-30
////////////////////////////////////////////////////////////////////////////////

#include "../../include/ReadFileModule.h"


//_________________________________________
void PulserReCali_L1_GainEffectOntheCali()
{
  gStyle->SetPalette(1);
  gROOT->Reset();

  Int_t SSDNum = 4;
  Int_t CHNum  = 16;
  Int_t numpar_LowGainIn  = 3;
  Int_t numpar_HighGainIn = 3;
  Int_t FitFuncNum = 5;
  Int_t color[5]   = {kBlue,kMagenta,kBlack,kOrange,kGreen};

  Double_t LowGain  = 7.3;        // SSD1_L1: 4.0 ;   SSD2_L1: 7.3
  Double_t HighGain = 20;
  Double_t GainFacRatio = 0.043;  // SSD1_L1: 0.006;  SSD2_L1: 0.043

  Double_t MinCh = 0.;
  Double_t MaxCh = 4096.;
  Double_t RangeXLow = 3000.;
  Double_t RangeXUp  = 4000.;
  Double_t RangeYUp  = 1.2;

  std::string LayerTag("L1S");
  std::string CaliTag("PulserReCali");
  std::string FileOutTag("GainEffectOntheCali");
  std::string LowGainTag("Gain74");         // for SSD1_L1: Gain1=4,  Gain2=20
  std::string HighGainTag("Gain20");       // for SSD2_L1: Gain1=7.4, Gain2=20
  std::string CaliMethodTag("Height");     // "Height" or "Switch"

  std::string pathPDFOutput(Form("figures/SSD_%s_%s_%s_%s_%s.pdf",  LayerTag.c_str(),CaliTag.c_str(),
                                 CaliMethodTag.c_str(),LowGainTag.c_str(),FileOutTag.c_str()));
  std::string pathPDFOutputBegin(Form("figures/SSD_%s_%s_%s_%s_%s.pdf[",  LayerTag.c_str(),CaliTag.c_str(),
                                 CaliMethodTag.c_str(),LowGainTag.c_str(),FileOutTag.c_str()));
  std::string pathPDFOutputEnd(Form("figures/SSD_%s_%s_%s_%s_%s.pdf]",  LayerTag.c_str(),CaliTag.c_str(),
                                 CaliMethodTag.c_str(),LowGainTag.c_str(),FileOutTag.c_str()));
  std::string pathLowGainFileIn(Form("output/SSD_%s_%s_%s_%s.dat",LayerTag.c_str(),CaliTag.c_str(),
                                 LowGainTag.c_str(),CaliMethodTag.c_str()));
  std::string pathHighGainFileIn(Form("output/SSD_%s_%s_%s_%s.dat",LayerTag.c_str(),CaliTag.c_str(),
                                 HighGainTag.c_str(),CaliMethodTag.c_str()));

  ReadFileModule readfile;
  Double_t*** LowGainIn  = readfile.ReadData(pathLowGainFileIn.c_str(), SSDNum,CHNum,numpar_LowGainIn);
  Double_t*** HighGainIn = readfile.ReadData(pathHighGainFileIn.c_str(),SSDNum,CHNum,numpar_HighGainIn);

  Double_t a_LowGain[SSDNum][CHNum];
  Double_t b_LowGain[SSDNum][CHNum];
  Double_t a_HighGain[SSDNum][CHNum];
  Double_t b_HighGain[SSDNum][CHNum];
  Double_t a_GainFactor[SSDNum][CHNum][FitFuncNum];
  Double_t b_GainFactor[SSDNum][CHNum][FitFuncNum];
  Double_t AmpFactor[SSDNum][CHNum][FitFuncNum];

  // 换底公式  loga(n)/loga(m) = logm(n)
  Int_t powLowGain  = Int_t (log(LowGain)/log(1.22) + 0.5); //
  Int_t powHighGain = Int_t (log(HighGain)/log(1.22) + 0.5); //
  Double_t GainFactor = pow(1.22,(powHighGain-powLowGain));
  cout<< "im = " << powHighGain-powLowGain <<setw(20)<<"GainFactor = "<<GainFactor << endl;

  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      a_LowGain[i][j]  = LowGainIn[i][j][0];
      b_LowGain[i][j]  = LowGainIn[i][j][2];
      a_HighGain[i][j] = HighGainIn[i][j][0];
      b_HighGain[i][j] = HighGainIn[i][j][2];
      if (a_LowGain[i][j]<1e-10) continue;

      cout<<setw(5)<<std::right<<i<<setw(10)<<j<<setw(15)<<std::right
          <<"a_LowGain = " <<setw(10)<<std::right<<a_LowGain[i][j] <<setw(15)<<std::right
          <<"b_LowGain = " <<setw(10)<<std::right<<b_LowGain[i][j] <<setw(15)<<std::right
          <<"a_HighGain = "<<setw(10)<<std::right<<a_HighGain[i][j]<<setw(15)<<std::right
          <<"b_HighGain = "<<setw(10)<<std::right<<b_HighGain[i][j]<<endl;

      for (Int_t k=0; k<FitFuncNum; k++)
      {
        AmpFactor[i][j][k] = GainFactor*(1+(k-2)*GainFacRatio);
        a_GainFactor[i][j][k] = a_LowGain[i][j]/AmpFactor[i][j][k];
        b_GainFactor[i][j][k] = b_LowGain[i][j]/AmpFactor[i][j][k];
      }
    }
  }

  TCanvas* cans_begin = new TCanvas();
  TCanvas* cans_end   = new TCanvas();
  TCanvas* cans = new TCanvas("cans1","cans1_draw",800,800);
  cans->Divide(1,2);
  cans_begin->Close();
  cans_end->Close();

  cans_begin->Print(pathPDFOutputBegin.c_str());
  for (Int_t i=0; i<2; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      if (a_LowGain[i][j]<1e-10) continue;

      cans->cd(1);
      gPad->SetGridx();
      gPad->SetGridy();

      TLegend* legend1 = new TLegend(0.12,0.48,0.4,0.88);

      TF1* FuncLowGain  = new TF1("fLowGain", Form("%.11f*x+%.11f",a_LowGain[i][j], b_LowGain[i][j]), MinCh,MaxCh);
      TF1* FuncHighGain = new TF1("fHighGain",Form("%.11f*x+%.11f",a_HighGain[i][j],b_HighGain[i][j]),MinCh,MaxCh);

      FuncLowGain->SetTitle(Form("SSD%d_%s_CH%02d_%s_%s",i+1,LayerTag.c_str(),j,CaliMethodTag.c_str(),FileOutTag.c_str()));
      FuncLowGain->SetLineColor(kRed);
      FuncLowGain->GetXaxis()->SetRangeUser(0.,MaxCh);
      FuncLowGain->GetXaxis()->SetTitle("ADC Channel");
      FuncLowGain->GetXaxis()->CenterTitle(1);
      FuncLowGain->GetXaxis()->SetTitleSize(0.05);
      FuncLowGain->GetXaxis()->SetNdivisions(1005,"true");
      FuncLowGain->GetYaxis()->SetRangeUser(0.,RangeYUp);
      FuncLowGain->GetYaxis()->SetNdivisions(1003,"true");
      FuncLowGain->GetYaxis()->SetTitle("Relative_Pulser_Height");
      FuncLowGain->GetYaxis()->SetTitleSize(0.05);
      FuncLowGain->GetYaxis()->CenterTitle(1);

      FuncHighGain->SetLineColor(kCyan);

      FuncLowGain->Draw();
      FuncHighGain->Draw("SAME");

      legend1->AddEntry(FuncLowGain,LowGainTag.c_str(),"l");
      legend1->AddEntry(FuncHighGain,HighGainTag.c_str(),"l");

      for (Int_t k=0; k<FitFuncNum; k++)
      {
        TF1* FuncGainFactor = new TF1("fGainFactor", Form("%.11f*x+%.11f",a_GainFactor[i][j][k],
                                      b_GainFactor[i][j][k]),MinCh,MaxCh);
        FuncGainFactor->SetLineColor(color[k]);

        Double_t ratio = 100*(k-2)*GainFacRatio;
        legend1->AddEntry(FuncGainFactor,Form("%s * %.04f[%.2f%%]",HighGainTag.c_str(),AmpFactor[i][j][k],ratio),"l");

        FuncGainFactor->Draw("SAME");
      }
      legend1->Draw("SAME");

      gPad->Modified();
      gPad->Update();
    }
  }
  for (Int_t i=0; i<2; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      if (a_LowGain[i][j]<1e-10) continue;

      cans->cd(2);
      gPad->SetGridx();
      gPad->SetGridy();

      TF1* FuncLowGain  = new TF1("fLowGain", Form("%.11f*x+%.11f",a_LowGain[i][j], b_LowGain[i][j]), MinCh,MaxCh);
      TF1* FuncHighGain = new TF1("fHighGain",Form("%.11f*x+%.11f",a_HighGain[i][j],b_HighGain[i][j]),MinCh,MaxCh);

      FuncLowGain->SetTitle(Form("SSD%d_%s_CH%02d_%s_%s",i+1,LayerTag.c_str(),j,CaliMethodTag.c_str(),FileOutTag.c_str()));
      FuncLowGain->SetLineColor(kRed);
      FuncLowGain->GetXaxis()->SetRangeUser(RangeXLow,RangeXUp);
      FuncLowGain->GetXaxis()->SetTitle("ADC Channel");
      FuncLowGain->GetXaxis()->CenterTitle(1);
      FuncLowGain->GetXaxis()->SetTitleSize(0.05);
      FuncLowGain->GetXaxis()->SetNdivisions(1005,"true");
      FuncLowGain->GetYaxis()->SetRangeUser(0.99*FuncHighGain->Eval(RangeXLow),1.01*FuncHighGain->Eval(RangeXUp));
      FuncLowGain->GetYaxis()->SetNdivisions(1005,"true");
      FuncLowGain->GetYaxis()->SetTitle("Relative_Pulser_Height");
      FuncLowGain->GetYaxis()->SetTitleSize(0.05);
      FuncLowGain->GetYaxis()->CenterTitle(1);

      FuncHighGain->SetLineColor(kCyan);

      FuncLowGain->Draw();
      FuncHighGain->Draw("SAME");

      for (Int_t k=0; k<FitFuncNum; k++)
      {
        TF1* FuncGainFactor = new TF1("fGainFactor", Form("%.11f*x+%.11f",a_GainFactor[i][j][k],
                                      b_GainFactor[i][j][k]),MinCh,MaxCh);
        FuncGainFactor->SetLineColor(color[k]);

        Double_t ratio = 100*((k-2)*GainFacRatio);

        FuncGainFactor->Draw("SAME");
      }
      cans->Print(pathPDFOutput.c_str());

      gPad->Modified();
      gPad->Update();
    }
  }
  cans_end->Print(pathPDFOutputEnd.c_str());
}
