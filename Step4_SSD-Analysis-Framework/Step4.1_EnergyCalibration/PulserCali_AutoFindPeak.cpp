/////////////////////////////////////////////////////////////////////////
//    This macro is used to find the peaks in pulser calibration data  //
//    Here, we use TSpectrum to find the peaks candidates              //
//                                                                     //
//    Author gfh                                                       //
//    Date Nov.7, 2019                                                 //
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

Int_t npeaks = 30;

//______________________________________________________________________________
Double_t SwitchFactor[] = {0};
Double_t Switch6[6] =                     {1.0/20.0, 1.0/10.0, 1.0/5.0, 1.0/4.0, 1.0/2.0, 1.0};
Double_t Switch7[7] =           {1.0/40.0, 1.0/20.0, 1.0/10.0, 1.0/5.0, 1.0/4.0, 1.0/2.0, 1.0};
Double_t Switch8[8] = {1.0/50.0, 1.0/40.0, 1.0/20.0, 1.0/10.0, 1.0/5.0, 1.0/4.0, 1.0/2.0, 1.0};

Double_t HeightFactor[] = {0};
Double_t Switch10[10] =     {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
Double_t Switch11[11] ={0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};

//______________________________________________________________________________
void PulserCali_AutoFindPeak()
{
///////////////////////////////////////////////////////////////////////////////
//                        自动寻峰步骤                                         //
//   1.从文件中读取已有的的 hitograms                                           //
//   2.新建TSpectrum：TSpectrum * s = new TSpectrum();                        //
//   3.寻峰(寻找候选峰): Int_t nfound  = s->Search(PulserPeaks[i],3,"",0.2);   //
//   4.对所有峰进行排序                                                         //
//   5.遍历所有找到的峰，提取峰值的 X值、Y值，拟合                                  //
//   6.将寻峰得到的数据点画出来, 作线性拟合                                        //
//                                                                           //
//   Search参数说明：                                                          //
//   Search(hist, sigma, option, threshold) \\ 0 < threshold < 1             //
//   sigma    : sigma of searched peaks， search                             //
//   threshold: peaks with amplitude < threshold*highest_peak are discarded  //
///////////////////////////////////////////////////////////////////////////////
  gStyle->SetOptStat(0);

  Int_t SSDNum = 1;               // here to change the telescope number
  std::string FileTag1("L1");     // here to change layer1 (L1) or layer2 (L2)
  std::string FileTag2("L1S");
  std::string FileTag3("Switch"); // here to change Switch or Height

  std::string path_to_file(Form("data/QC_MapSSD%d_%s_PulserCali_%s0000.root", SSDNum,FileTag1.c_str(),FileTag3.c_str()));

  TFile * FileIn = new TFile(path_to_file.c_str());
  if(!FileIn->IsOpen())
  {
    cout<<"Open file "<< path_to_file.c_str() << " failed"<<endl;
    return;
  }

  //  here to change the output data file
  FILE * FileOut = fopen(Form("output/SSD%d_%s_PulserCali_%s.dat", SSDNum,FileTag1.c_str(),FileTag3.c_str()),"w");
  fprintf(FileOut,"# SSDNum   CHNum    peak1    peak2    peak3    peak4    peak5    peak6     peak7    peak8     peak9     peak10  (0.0 isn't a peak)\n");

  // 读取root文件中的 Histograms
  TH1D * PulserPeaks[16];
  for(int i=0; i<16; i++)
  {
    PulserPeaks[i] = (TH1D*)FileIn->Get(Form("SSD%d_%s_E_CH%02d",SSDNum,FileTag2.c_str(),i));
    cout << Form("SSD%d_%s_E_CH%d",SSDNum,FileTag2.c_str(),i) << endl;
  }
  printf("Histograms loaded\n");

  //============================================================================
  //                        BEGIN ANALYZE HERE !!!
  //============================================================================
  TCanvas *c1 = new TCanvas("c1","c1",10,10,1000,900);
  c1->Divide(1,2);
  TCanvas *c_begin = new TCanvas("c_begin","");
  TCanvas *c_end = new TCanvas("c_end","");
  c_end->Close();
  c_begin->Close();
  c_begin->Print(Form("figures/SSD%d_%s_PulserCali_%s.pdf[", SSDNum,FileTag1.c_str(),FileTag3.c_str()));

  TSpectrum * s = new TSpectrum();
  for(int i=0; i<16; i++)
  {
    if(PulserPeaks[i]==0)
    {
      printf("No data present for SSD%d_%s_E_CH%02d\n",SSDNum,FileTag2.c_str(),i);
      continue;
    }
    // find peaks candidates
    // Search(hist, sigma, option, threshold) \\ 0 < threshold < 1
    c1->cd(1);
    Int_t nfound  = s->Search(PulserPeaks[i],3,"",0.2);

    printf("SSD%d_%s_E_CH%d is analyzed,%2d peaks found\n",SSDNum,FileTag2.c_str(),i,nfound);

    //  Loop on all found peaks
    npeaks = 0;
    Double_t *xpeaks = s->GetPositionX();
    sort(xpeaks,xpeaks+nfound);   // 对寻找到的峰值排序
    for(Int_t p=0; p<nfound; p++)
    {
      Double_t xp = xpeaks[p];
      Int_t   bin = PulserPeaks[i]->GetXaxis()->FindBin(xp);
      Double_t yp = PulserPeaks[i]->GetBinContent(bin);

      printf(" SSD%d_%s_E_CH%d peak %d is %.1f\n",SSDNum,FileTag2.c_str(),i,p,xp);

      npeaks++;
    }
    cout<<"有效的peak数目为:"<< npeaks << endl;
    cout<<"Now fitting: Be patient\n" <<endl;
    fprintf(FileOut," %5d  %7d  %8.1f %8.1f %8.1f %8.1f %8.1f  %8.1f  %8.1f %8.1f %8.1f  %8.1f\n",
              SSDNum,i,xpeaks[0],xpeaks[1],xpeaks[2],xpeaks[3],xpeaks[4],xpeaks[5],xpeaks[6],xpeaks[7],xpeaks[8],xpeaks[9]);
    fflush(FileOut); // 需要加上这句！！！ 否则由于缓存问题，内容不被写入

    for(Int_t i=0; i<npeaks; i++)
    {
      if(npeaks==6)  SwitchFactor[i] = Switch6[i];
      if(npeaks==7)  SwitchFactor[i] = Switch7[i];
      if(npeaks==8)  SwitchFactor[i] = Switch8[i];
    }
    c1->cd(2);
    TGraph *grap = new TGraph(npeaks,SwitchFactor,xpeaks);
    grap->SetMarkerStyle(20);
    grap->SetMarkerSize(1.5);
    grap->SetMarkerColor(kBlue);
    grap->Draw("AP*");

    TF1 * fit = new TF1("fit","pol1",0,4096);
    grap->Fit("fit");
    c1->Print(Form("figures/SSD%d_%s_PulserCali_%s.pdf", SSDNum,FileTag1.c_str(),FileTag3.c_str()));

    gPad->Modified();
    gPad->Update();
    getchar();
  }// ====  close loop of strip number i
  c_end->Print(Form("figures/SSD%d_%s_PulserCali_%s.pdf]", SSDNum,FileTag1.c_str(),FileTag3.c_str()));

  delete c1;
  delete s;
  return;
}
