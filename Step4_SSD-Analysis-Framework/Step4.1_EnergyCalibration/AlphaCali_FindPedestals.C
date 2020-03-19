/////////////////////////////////////////////////////////////////////////
//    This macro is used to find the peaks in alpha calibration data  //
//    Here, we use TSpectrum to find the peaks candidates              //
//                                                                     //
//    Author gfh                                                       //
//    Date Nov 11, 2019                                                //
/////////////////////////////////////////////////////////////////////////

#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TSpectrum.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TChain.h"
#include "TStyle.h"
#include "TMarker.h"
#include "TGraph.h"
#include "TMath.h"
#include "TFile.h"


//______________________________________________________________________________
void AlphaCali_FindPedestals()
{
/////////////////////////////////////////////////////////////////////////////////
//                        自动寻峰步骤                                          //
//   1.从文件中读取已有的的 hitograms                                            //
//   2.新建TSpectrum：TSpectrum * s = new TSpectrum();                         //
//   3.寻峰(寻找候选峰): Int_t nfound  = s->Search(PulserPeaks[i],3,"",0.2);    //
//   4.对所有峰进行排序                                                         //
//   5.遍历所有找到的峰，提取峰值的 X值、Y值，拟合                                 //
//   6.将寻峰得到的数据点画出来, 作线性拟合                                       //
//                                                                            //
//   PulserCali_AutoFindPeak()参数说明                                         //
//   LayerTag   : L1 or L2                                                    //
//   FileTag    : Switch / Height，We use this two methods for pulser cali    //
//   cans[4][16]: Canvas for to save all the figures                          //
//                                                                            //
//   Search()参数说明：                                                        //
//   Search(hist, sigma, option, threshold) \\ 0 < threshold < 1              //
//   sigma    : sigma of searched peaks， search                              //
//   threshold: peaks with amplitude < threshold*highest_peak are discarded   //
////////////////////////////////////////////////////////////////////////////////
  Int_t lowrange  = 0;
  Int_t highrange = 200;
  std::string FileTag("05_08");
  std::string LayerTag("L1S");
  /*
  std::string inputpath(Form("data/MapSSD_%s_AlphaCali%s.root", LayerTag.c_str(), FileTag.c_str()));
  std::string pdfpath(Form("figures/SSD_%s_AlphaCaliPedestals_%s.pdf",LayerTag.c_str(), FileTag.c_str()));
  std::string pdfpathbegin(Form("figures/SSD_%s_AlphaCaliPedestals_%s.pdf[",LayerTag.c_str(), FileTag.c_str()));
  std::string pdfpathend(Form("figures/SSD_%s_AlphaCaliPedestals_%s.pdf]",LayerTag.c_str(), FileTag.c_str()));
  std::string outputpath(Form("output/SSD_%s_AlphaCaliPedestals_%s.dat",LayerTag.c_str(), FileTag.c_str()));
  */
  for (Int_t i=0; i<9; i++) {
    std::string inputpath(Form("/data/EXPdata/Fission2019_Data/MapRoot/MapSSD_%s_AlphaCali%04d.root", LayerTag.c_str(), i));
    std::string pdfpath(Form("figures/MapSSD_%s_AlphaCali%04d.pdf", LayerTag.c_str(), i));
    std::string pdfpathbegin(Form("figures/MapSSD_%s_AlphaCali%04d.pdf[", LayerTag.c_str(), i));
    std::string pdfpathend(Form("figures/MapSSD_%s_AlphaCali%04d.pdf]", LayerTag.c_str(), i));
    std::string outputpath(Form("output/MapSSD_%s_AlphaCali%04d.dat", LayerTag.c_str(), i));

/*
  std::string inputpath("data/QC_MapSSD_PulserCali_Pedestal0000.root");
  std::string pdfpath("figures/SSD_Pedestals.pdf");
  std::string pdfpathbegin("figures/SSD_Pedestals.pdf[");
  std::string pdfpathend("figures/SSD_Pedestals.pdf]");
  std::string outputpath("output/SSD_Pedestals.dat");
*/
  FILE * FileOut = fopen(outputpath.c_str(),"w");
  fprintf(FileOut,"* SSDNum   CHNum  Mean  \n");

  TFile* FileIn = new TFile(inputpath.c_str());
  if (!FileIn->IsOpen())
  {
    cout<<"Open file "<< inputpath.c_str() << " failed"<<endl;
    return;
  }

  TCanvas* c1 = new TCanvas("c1","c1");
  TCanvas* c_begin = new TCanvas();
  TCanvas* c_end   = new TCanvas();
  c_begin->Print(pdfpathbegin.c_str());
  TH1D* Pedestals[4][16];
  for (Int_t SSDNum=0; SSDNum<4; SSDNum++)
  {
    // 读取root文件中的 Histograms
    for (Int_t CHNum=0; CHNum<16; CHNum++)
    {
      Pedestals[SSDNum][CHNum] = (TH1D*)FileIn->Get(Form("SSD%d_%s_E_CH%02d",SSDNum+1,LayerTag.c_str(),CHNum));
      Pedestals[SSDNum][CHNum]->GetXaxis()->SetRangeUser(lowrange, highrange);
      cout << Form("SSD%d_%sS_E_CH%d",SSDNum+1, LayerTag.c_str(), CHNum) << endl;
    }
    printf("Histograms loaded\n");

    //____________________________________________________________________________
    //                        BEGIN ANALYZE HERE !!!
    //
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
      printf("SSD%d_%sS_E_CH%d is analyzed,%2d peaks found\n",SSDNum+1,LayerTag.c_str(),CHNum,nfound);
      c1->Print(pdfpath.c_str());

      Int_t npeaks = 0;
      Double_t* xpeaks = s->GetPositionX();
      fprintf(FileOut, "%5d  %5d  %10.1f \n", SSDNum, CHNum, xpeaks[0]);
      fflush(FileOut);

      gPad->Modified();
      gPad->Update();
  //    getchar();
    }// ====  close loop of strip number
  } // =====  close loop of SSDNum
  c_end->Print(pdfpathend.c_str());

  fclose(FileOut);
  return;
  }
}
