////////////////////////////////////////////////////////////////////////////////
//
//   在进行 alpha 峰拟合之前，先检查每一块硅条的情况, 确保没有问题再进行拟合
//   这是因为，一旦中途发现某一条信号异常，又要从头再来
//
//   gfh, 2020-06-04
////////////////////////////////////////////////////////////////////////////////

#include "../../include/ReadFileModule.h"


//______________________________________________________________________________
void AlphaCali_CheckAlphaPeaks()
{
  Int_t SSDNum = 4;
  Int_t CHNum  = 16;
  Int_t numpar = 1;
  Double_t XRangeLow[4] = {100.,100.,100.,150.};     // for L1
  Double_t XRangeUp[4]  = {300.,300.,350.,400.};     // for L1
  // Double_t XRangeLow[4] = {50.,50., 100.,200.};  // for L2
  // Double_t XRangeUp[4] = {250.,250.,350.,550.};   // for L2

  std::string LayerTag("L1S");                // "L1S" or "L2F" or "L2B"
  std::string LayerTagWithoutLabel("L1");     //  L1 or L2
  std::string AlphaFileTag("AlphaCali00_04"); //  for L1:00_04,00_08,05_08;  for L2: 00_32,33_48
  std::string AlphaCaliTag("AlphaCaliPedestals");

  std::string L1Tag("L1");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");

  std::string pathRootFileFolder("/home/sea/Fission2019_Data/QualityCheck/");
  std::string pathAlphaCaliIn(Form("%sQC_MapSSD_%s_%s.root", pathRootFileFolder.c_str(),
                              LayerTagWithoutLabel.c_str(), AlphaFileTag.c_str()));
  std::string pathRootFileOut(Form("rootinput/CheckAlphaPeaks_SSD_%s_%s.root",LayerTag.c_str(),
                              AlphaFileTag.c_str()));
  std::string pathPedestalInput(Form("output/SSD_%s_%s_%s.dat",LayerTag.c_str(),
                                AlphaCaliTag.c_str(), AlphaFileTag.c_str()));

  TFile* FileIn = TFile::Open(pathAlphaCaliIn.c_str());
  if (!FileIn->IsOpen()) {
    cout<<"Open file "<< pathAlphaCaliIn.c_str() << " failed. "<<endl;
    return;
  } else {
    cout<<"file "<< pathAlphaCaliIn.c_str() << " loaded. "<<endl;
  }

  ReadFileModule readfile;
  Double_t*** Pedestal = readfile.ReadData(pathPedestalInput.c_str(),SSDNum,CHNum,numpar);

  TH1D* hist[SSDNum][CHNum];
  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      hist[i][j] = (TH1D*)FileIn->Get(Form("SSD%d_%s_E_CH%02d",i+1,LayerTag.c_str(),j));
      if (hist[i][j]==0) {
        printf("No data present for SSD%d_%s_E_CH%02d\n",(i+1),LayerTag.c_str(),j);
        continue;
      }
      hist[i][j]->GetXaxis()->SetRangeUser(Pedestal[i][j][0]+XRangeLow[i],XRangeUp[i]);
    }
  }

  TFile* FileOut = new TFile(pathRootFileOut.c_str(),"RECREATE");
  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      hist[i][j]->Write();
    }
  }
  cout<<Form("%s have been created!",pathRootFileOut.c_str())<<endl;

  FileOut->Close();
  FileIn ->Close();
}
