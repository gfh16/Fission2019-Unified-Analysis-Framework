////////////////////////////////////////////////////////////////////////////////
//
//   Pulser 刻度时， SSD2_L2F_CH00,CH01 两道峰值大约是其他道的一半,
//
//   怀疑可能的原因：由于2019年这次实验采取两路并一路的方式，可能是这两路
//   有断丝、或因为其他原因导致只有一路有信号.
//
//   为此，可以检查 alpha 刻度时，这两路的计数与其他道的计数是否存在差异
//
//   gfh, 2020-06-16
////////////////////////////////////////////////////////////////////////////////

#include "../../include/ReadFileModule.h"


Double_t L1_XRangeLow[4] = {110.,110.,110.,150.};
Double_t L1_XRangeUp [4] = {200.,200.,300.,400.};
Double_t L2_XRangeLow[4] = {100.,110.,150.,300.};
Double_t L2_XRangeUp [4] = {180.,200.,300.,480.};


//_______________________________________________
void PulserCaliCheck_SSD2_L2F_CH00_01()
{
  gStyle->SetOptStat(0);

  Int_t SSDNum = 4;
  Int_t CHNum  = 16;

  Int_t SSDIndex  = 2;
  Int_t YRangeCut = 4000;

  std::string LayerTag("L2");
  std::string LayerTagWithLabel("L2F");
  std::string AlphaCaliTag("AlphaCali00_48");
  std::string PulserCheckTag(Form("PulserCaliCheck_SSD%d_L2F",SSDIndex+1));


  std::string pathRootFileFolder("/home/sea/Fission2019_Data/QualityCheck/");
  std::string pathFileIn(Form("%sQC_MapSSD_%s_%s.root",pathRootFileFolder.c_str(),LayerTag.c_str(),
                         AlphaCaliTag.c_str()));
  std::string pathPNGOutput(Form("figures/%s_%s.png",PulserCheckTag.c_str(),AlphaCaliTag.c_str()));

  Double_t XRangeLow[SSDNum];
  Double_t XRangeUp[SSDNum];
  for (Int_t i=0; i<SSDNum; i++)
  {
    if (strcmp(LayerTag.c_str(),"L1")==0) {
      XRangeLow[i] = L1_XRangeLow[i];
      XRangeUp[i]  = L1_XRangeUp[i];
    } else {
      XRangeLow[i] = L2_XRangeLow[i];
      XRangeUp[i]  = L2_XRangeUp[i];
    }
  }

  TFile* FileIn = TFile::Open(pathFileIn.c_str());
  if (!FileIn->IsOpen()) {
    cout<< "Open file "<< pathFileIn.c_str() << " failed." <<endl;
    return;
  } else {
    cout<< "File "<< pathFileIn.c_str() << " loaded."<<endl;
  }

  TH1D* hist[SSDNum][CHNum];
  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      hist[i][j] = (TH1D*)FileIn->Get(Form("SSD%d_%s_E_CH%02d",i+1,LayerTagWithLabel.c_str(),j));
      if (hist[i][j]==0) {
        printf("No data present for SSD%d_%s_E_CH%02d\n",(i+1),LayerTagWithLabel.c_str(),j);
        continue;
      }
      hist[i][j]->GetXaxis()->SetRangeUser(XRangeLow[i],XRangeUp[i]);
      hist[i][j]->GetYaxis()->SetRangeUser(0,YRangeCut);
    }
  }

  TCanvas* cans = new TCanvas("cans","cans",1200,800);
  cans->Divide(3,2);

  cans->cd(1);
  hist[SSDIndex][0]->Draw();

  cans->cd(2);
  hist[SSDIndex][1]->Draw();

  cans->cd(3);
  hist[SSDIndex][2]->Draw();

  cans->cd(4);
  hist[SSDIndex][15]->Draw();

  cans->cd(5);
  hist[SSDIndex][14]->Draw();

  cans->cd(6);
  hist[SSDIndex][13]->Draw();

  cans->Print(pathPNGOutput.c_str());

}
