////////////////////////////////////////////////////////////////////////////////
//
//    自动提取 pedestals.
//    通过 TSpectrum 进行自动寻峰. 通过指定区间，可以快速进行自动寻峰.
//    自动寻峰有以下不足之处就是：
//      1. 只能得到峰值，得不到 sigma 以及其他参数. 如果需要知道其他参数, 则不能使用自动寻峰
//      2. 如果存在多个峰，自动寻峰是无序的，则峰值不按大小排列.
//
//    对于 pedestals，由于只要一个峰, 可以使用自动寻峰, 不过这样一来就得不到 sigma 值.
//
//    gfh, 2019-11-11
////////////////////////////////////////////////////////////////////////////////

#include "TCanvas.h"
#include "TH1.h"
#include "TSpectrum.h"


//______________________________________________________________________________
void Pedestals_AutoFindPedestals()
{
/////////////////////////////////////////////////////////////////////////////////
//                        自动寻峰步骤                                          //
//   1.从文件中读取已有的的 hitograms                                            //
//   2.新建TSpectrum：TSpectrum * s = new TSpectrum();                         //
//   3.寻峰(寻找候选峰): Int_t nfound  = s->Search(PulserPeaks[i],3,"",0.2);    //
//   4.对所有峰进行排序                                                          //
//   5.遍历所有找到的峰，提取峰值的 X值、Y值，拟合                                   //
//   6.将寻峰得到的数据点画出来, 作线性拟合                                         //
//                                                                            //
//   PulserCali_AutoFindPeak()参数说明                                         //
//   LayerTagWithOutLabel   : L1 or L2                                        //
//   AlphaTag    : Switch / Height，We use this two methods for pulser cali   //
//   cans[4][16]: Canvas for to save all the figures                          //
//                                                                            //
//   Search()参数说明：                                                         //
//   Search(hist, sigma, option, threshold) \\ 0 < threshold < 1              //
//   sigma    : sigma of searched peaks， search                              //
//   threshold: peaks with amplitude < threshold*highest_peak are discarded   //
////////////////////////////////////////////////////////////////////////////////

  Int_t lowrange  = 0;
  Int_t highrange = 200;
  std::string AlphaFileTag("AlphaCali00_32");
  std::string LayerTagWithOutLabel("L2");
  std::string LayerTag("L2F");

//  std::string pathRootInput(Form("/data/EXPdata/Fission2019_Data/MapRoot/MapSSD_%s_AlphaCali%04d.root", LayerTagWithOutLabel.c_str(), i));
  std::string pathRootInput(Form("/home/sea/Fission2019_Data/QualityCheck/QC_MapSSD_%s_%s.root", LayerTagWithOutLabel.c_str(), AlphaFileTag.c_str()));

  std::string pathAlphaCaliPedestalsOutput(Form("output/SSD_%s_AlphaCaliPedestals_%s.dat", LayerTag.c_str(), AlphaFileTag.c_str()));

  std::string pathPDFOutput(Form("figures/SSD_%s_AlphaCaliPedestals_%s.pdf", LayerTag.c_str(), AlphaFileTag.c_str()));
  std::string pathPDFbegin(Form("figures/SSD_%s_AlphaCaliPedestals_%s.pdf[", LayerTag.c_str(), AlphaFileTag.c_str()));
  std::string pathPDFend(Form("figures/SSD_%s_AlphaCaliPedestals_%s.pdf]", LayerTag.c_str(), AlphaFileTag.c_str()));


  FILE * FileOut = fopen(pathAlphaCaliPedestalsOutput.c_str(),"w");
  fprintf(FileOut,"* SSDNum   CHNum  Mean  \n");

  TFile* FileIn = new TFile(pathRootInput.c_str());
  if (!FileIn->IsOpen())
  {
    cout<<"Open file "<< pathRootInput.c_str() << " failed"<<endl;
    return;
  }

  TCanvas* c1 = new TCanvas("c1","c1");
  TCanvas* c_begin = new TCanvas();
  TCanvas* c_end   = new TCanvas();
  c_begin->Close();
  c_end->Close();
  c_begin->Print(pathPDFbegin.c_str());

  TH1D* Pedestals[4][16];
  for (Int_t SSDNum=0; SSDNum<4; SSDNum++)
  {
    // 读取root文件中的 Histograms
    for (Int_t CHNum=0; CHNum<16; CHNum++)
    {
      Pedestals[SSDNum][CHNum] = (TH1D*)FileIn->Get(Form("SSD%d_%s_E_CH%02d",SSDNum+1,LayerTag.c_str(),CHNum));
      Pedestals[SSDNum][CHNum]->GetXaxis()->SetRangeUser(lowrange, highrange);
      cout << Form("SSD%d_%s_E_CH%d",SSDNum+1, LayerTag.c_str(), CHNum) << endl;
    }
    printf("Histograms loaded\n");

    //____________________________________________________________________________
    //                        BEGIN ANALYZE HERE !!!
    TSpectrum* s = new TSpectrum();
    for (Int_t CHNum=0; CHNum<16; CHNum++)
    {
      if (Pedestals[SSDNum][CHNum]==0)
      {
        printf("No data present for SSD%d_%s_E_CH%02d\n",SSDNum+1,LayerTag.c_str(),CHNum);
        continue;
      }

      c1->cd();
      Int_t nfound  = s->Search(Pedestals[SSDNum][CHNum],3,"",0.4);
      printf("SSD%d_%s_E_CH%d is analyzed,%2d peaks found\n",SSDNum+1,LayerTag.c_str(),CHNum,nfound);
      c1->Print(pathPDFOutput.c_str());

      Int_t npeaks = 0;
      Double_t* xpeaks = s->GetPositionX();
      fprintf(FileOut, "%5d  %5d  %10.1f \n", SSDNum, CHNum, xpeaks[0]);
      fflush(FileOut);

      gPad->Modified();
      gPad->Update();
  //  getchar();
    }
  }
  c_end->Print(pathPDFend.c_str());

  fclose(FileOut);
  return;
}
