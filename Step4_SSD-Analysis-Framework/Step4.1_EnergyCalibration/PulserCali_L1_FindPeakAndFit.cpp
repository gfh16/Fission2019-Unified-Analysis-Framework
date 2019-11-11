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
#include "TMath.h"


//______________________________________________________________________________
//  const number definition
Double_t SwitchFactor[] = {0};
Double_t Switch5[5] = {1.0, 1/2.0, 1/4.0, 1/5.0, 1/10.0};
Double_t Switch6[6] = {1.0, 1/2.0, 1/4.0, 1/5.0, 1/10.0, 1/20.0};
Double_t Switch7[7] = {1.0, 1/2.0, 1/4.0, 1/5.0, 1/10.0, 1/20.0, 1/40.0};
Double_t Switch8[8] = {1.0, 1/2.0, 1/4.0, 1/5.0, 1/10.0, 1/20.0, 1/40.0, 1/50.0};

Double_t HeightFactor[] = {0};
Double_t Height10[10]   = {10.0, 9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0};
Double_t Height11[11]   = {10.0, 9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0, 0.5};

//_______________________________________________
//  实现数组的降序排序
bool compare(Int_t a, Int_t b)
{
  return a>b;
}

//______________________________________________________________________________
void PulserCali_AutoFindPeak(const char* LayerTag, const char* FileTag, FILE* FileOut, TCanvas* cans[4][16]);

//______________________________________________________________________________
void PulserCali_FindPeakAndFit()
{
  gStyle->SetOptStat(0);

  std::string LayerTag("L1");
  std::string FileTag("Switch");
  std::string pdfpath(Form("figures/SSD_%s_PulserCali_%s.pdf",LayerTag.c_str(),FileTag.c_str()));
  std::string FileOutTag(Form("output/SSD_%s_PulserCali_%s.dat",LayerTag.c_str(),FileTag.c_str()));
  FILE *FileOut = fopen(FileOutTag.c_str(),"w");
  fprintf(FileOut,"# Fiiting funtion = par[0] + par[1]*x && y=a*x+b, so a = par[1], b = par[0];  0.0 isn't a peak\n");
  fprintf(FileOut,"# SSDNum   CHNum    par1(a)   err_par0   par0(b)   err_par1    peak1    peak2    peak3    peak4    peak5    peak6     peak7    peak8     peak9     peak10     peak11\n");

  TCanvas *cans[4][16];
  for(Int_t SSDNum=0; SSDNum<4; SSDNum++)
  {
    for(int i=0; i<16; i++)
    {
      cans[SSDNum][i] = new TCanvas(Form("C_SSD%d_CH%d",SSDNum+1,i),Form("C_SSD%d_CH%d",SSDNum+1,i),10,10,1000,900);
      cans[SSDNum][i]->Close();
    }
  }
  PulserCali_AutoFindPeak(LayerTag.c_str(),FileTag.c_str(),FileOut,cans);

  std::string pdfpath_begin = pdfpath;
  pdfpath_begin += '[';
  std::string pdfpath_end = pdfpath;
  pdfpath_end += ']';

  TCanvas *c_begin = new TCanvas("c_begin","");
  TCanvas *c_end   = new TCanvas("c_end","");
  c_end->Close();
  c_begin->Close();
  c_end->Print(pdfpath_begin.c_str());
  for(Int_t SSDNum=0; SSDNum<4; SSDNum++)
  {
    for(int i=0; i<16; i++)
    {
        cans[SSDNum][i]->Print(pdfpath.c_str());
    }
  }
  c_end->Print(pdfpath_end.c_str());
  return;
}


//______________________________________________________________________________
void PulserCali_AutoFindPeak(const char* LayerTag, const char* FileTag, FILE* FileOut, TCanvas* cans[4][16])
{
/////////////////////////////////////////////////////////////////////////////////
//                        自动寻峰步骤                                           //
//   1.从文件中读取已有的的 hitograms                                             //
//   2.新建TSpectrum：TSpectrum * s = new TSpectrum();                          //
//   3.寻峰(寻找候选峰): Int_t nfound  = s->Search(PulserPeaks[i],3,"",0.2);     //
//   4.对所有峰进行排序                                                          //
//   5.遍历所有找到的峰，提取峰值的 X值、Y值，拟合                                   //
//   6.将寻峰得到的数据点画出来, 作线性拟合                                         //
//                                                                            //
//   PulserCali_AutoFindPeak()参数说明                                         //
//   LayerTag   : L1 or L2                                                    //
//   FileTag    : Switch / Height，We use this two methods for pulser cali    //
//   cans[4][16]: Canvas for to save all the figures                          //
//                                                                            //
//   Search()参数说明：                                                         //
//   Search(hist, sigma, option, threshold) \\ 0 < threshold < 1              //
//   sigma    : sigma of searched peaks， search                              //
//   threshold: peaks with amplitude < threshold*highest_peak are discarded   //
////////////////////////////////////////////////////////////////////////////////
  TH1D * PulserPeaks[4][16];
  for(Int_t SSDNum=0; SSDNum<4; SSDNum++)
  {
    std::string path_to_file(Form("data/QC_MapSSD%d_%s_PulserCali_%s0000.root", SSDNum+1,LayerTag,FileTag));
    TFile * FileIn = new TFile(path_to_file.c_str());
    if(!FileIn->IsOpen())
    {  //  cans[SSDNum][i]->Close();
      cout<<"Open file "<< path_to_file.c_str() << " failed"<<endl;
      return;
    }
    // 读取root文件中的 Histograms
    for(int i=0; i<16; i++)
    {
      PulserPeaks[SSDNum][i] = (TH1D*)FileIn->Get(Form("SSD%d_%sS_E_CH%02d",SSDNum+1,LayerTag,i));
      PulserPeaks[SSDNum][i]->GetXaxis()->SetRangeUser(120,4095);
      cout << Form("SSD%d_%sS_E_CH%d",SSDNum+1,LayerTag,i) << endl;
    }
    printf("Histograms loaded\n");

    //============================================================================
    //                        BEGIN ANALYZE HERE !!!
    //============================================================================
    TSpectrum * s = new TSpectrum();
    for(int i=0; i<16; i++)
    {
      if(PulserPeaks[SSDNum][i]==0)
      {
        printf("No data present for SSD%d_%s_E_CH%02d\n",SSDNum+1,LayerTag,i);
        continue;
      }
      // find peaks candidates
      // Search(hist, sigma, option, threshold) \\ 0 < threshold < 1
      cans[SSDNum][i] = new TCanvas(Form("C_SSD%d_CH%d",SSDNum+1,i),Form("C_SSD%d_CH%d",SSDNum+1,i),10,10,1000,900);
      cans[SSDNum][i]->Divide(1,2);
      cans[SSDNum][i]->cd(1);
      Int_t nfound  = s->Search(PulserPeaks[SSDNum][i],3,"",0.2);
      printf("SSD%d_%sS_E_CH%d is analyzed,%2d peaks found\n",SSDNum+1,LayerTag,i,nfound);
      //  Loop on all found peaks
      Int_t npeaks = 0;
      Double_t *xpeaks = s->GetPositionX();
      // 对寻找到的峰值降序排序
      std::sort(xpeaks,xpeaks+nfound,compare);   // sort默认升序
      for(Int_t p=0; p<nfound; p++)
      {
        if(xpeaks[p]<120) continue;
        Double_t xp = xpeaks[p];
        Int_t   bin = PulserPeaks[SSDNum][i]->GetXaxis()->FindBin(xp);
        Double_t yp = PulserPeaks[SSDNum][i]->GetBinContent(bin);

        printf(" SSD%d_%sS_E_CH%d peak %d is %.1f\n",SSDNum+1,LayerTag,i,p,xp);
        npeaks++;
      }
      cout<<"有效的peak数目为:"<< npeaks << endl;
      cout<<"Now fitting: Be patient\n" <<endl;

      for(Int_t i=0; i<npeaks; i++)
      {
        if(npeaks==5)  SwitchFactor[i] = Switch5[i];
        if(npeaks==6)  SwitchFactor[i] = Switch6[i];
        if(npeaks==7)  SwitchFactor[i] = Switch7[i];
        if(npeaks==8)  SwitchFactor[i] = Switch8[i];
      }
      cans[SSDNum][i]->cd(2);
      TGraph *grap = new TGraph(npeaks,SwitchFactor,xpeaks);
      grap->SetMarkerStyle(20);
      grap->SetMarkerSize(1.5);
      grap->SetMarkerColor(kBlue);
      grap->SetTitle(Form("PulserFit_SSD%d_%sS_E_CH%02d",SSDNum+1,LayerTag,i));
      grap->Draw("AP*");

      TF1 * fit = new TF1("fit","pol1",100,4096);
      grap->Fit("fit");
      Double_t par0     = fit->GetParameter(1);   // fitting function = par[0]+par[1]*x
      Double_t err_par0 = fit->GetParError(1);
      Double_t par1     = fit->GetParameter(0);
      Double_t err_par1 = fit->GetParError(0);
      fprintf(FileOut," %5d  %7d   %8.4f  %8.4f  %8.4f  %8.4f   %8.1f  %8.1f %8.1f %8.1f %8.1f %8.1f %8.1f %8.1f %8.1f %8.1f %8.1f\n",
              SSDNum,i,par0,err_par0,par1,err_par1,xpeaks[0],xpeaks[1],xpeaks[2],xpeaks[3],xpeaks[4],xpeaks[5],xpeaks[6],xpeaks[7],xpeaks[8],xpeaks[9],xpeaks[10]);
      fflush(FileOut); // 需要加上这句！！！ 否则由于缓存问题，内容不被写入

      gPad->Modified();
      gPad->Update();
  //    getchar();
    }// ====  close loop of strip number
  } // =====  close loop of SSDNum
  return;
}