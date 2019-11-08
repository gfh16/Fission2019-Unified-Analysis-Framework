//=============================================================================
// \This macro is used to find the peaks in pulser calibration data
//  Here, we use TSpectrum class in ROOT
//
// \author gfh
// \date Nov.7, 2019
//==============================================================================

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

void PulserCali_AutoFindPeak()
{
  Int_t SSDNum = 1;           // here to change the telescope number
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

  //========================================
  //  here to change the output data file
  FILE * FileOut = fopen(Form("output/SSD%d_%s_PulserCali_%s.dat", SSDNum,FileTag1.c_str(),FileTag3.c_str()),"w");
  fprintf(FileOut,"# SSDNum    CHNum    peak1    peak2    peak3   peak4   peak5    peak6    peak7    peak8    peak9    peak10\n");
  fflush(FileOut); // 需要加上这句！！！ 否则由于缓存问题，内容不被写入

  //=============================
  // 读取root文件中的 Histograms
  TH1D * PulserPeaks[16];
  for(int i=0; i<16; i++)
  {
    PulserPeaks[i] = (TH1D*)FileIn->Get(Form("SSD%d_%s_E_CH%02d",SSDNum,FileTag2.c_str(),i));
    cout << Form("SSD%d_%s_E_CH%d",SSDNum,FileTag2.c_str(),i) << endl;
  }
  printf("Histograms loaded\n");

  //============================================================================
  //          begin analysis here !!!
  //
  TCanvas *c1 = new TCanvas("c1","c1",800,1000);
  c1->Divide(1,2);

  TSpectrum * s = new TSpectrum();
  for(int i=0; i<16; i++)
  {
    c1->cd(1);
    gPad->Modified();
    gPad->Update();
    getchar();
    //===============================
    // 如果某个Histogram为空,则跳过
    if(PulserPeaks[i]==0)
    {
      printf("No data present for SSD%d_%s_E_CH%02d\n",SSDNum,FileTag2.c_str(),i);
      continue;
    }
    // ==============================================================
    // find peaks candidates
    // Search(hist, sigma, option, threshold) \\ 0 < threshold < 1
    Int_t nfound  = s->Search(PulserPeaks[i],6,"",0.05);
    printf("SSD%d_%s_E_CH%d is analyzed, %02d peaks found\n",SSDNum,FileTag2.c_str(),i,nfound);

    //=========================================================
    //  Loop on all found peaks and fit
    c1->cd(2);
    gPad->Modified();
    gPad->Update();

    Double_t *xpeaks = s->GetPositionX();
    for(int i=0; i<16; i++)
    {
      Int_t npeaks = 0;
      for(Int_t p=0; p<nfound; p++)
      {
        Double_t xp = xpeaks[p];
        Int_t   bin = PulserPeaks[i]->GetXaxis()->FindBin(xp);
        Double_t yp = PulserPeaks[i]->GetBinContent(bin);
    //    if(yp - TMath::Sqrt(yp) < fline->Eval(xp)) continue; // 判选条件
        printf(" SSD%d_%s_E_CH%d peak %d is %f\n",SSDNum,FileTag2.c_str(),i,p,xp);
        npeaks++;
      }
      cout<<"有效的peak数目为:"<< npeaks << endl;
      cout<<"Now fitting: Be patient\n" <<endl;

      //TH1D *h = (TH1D*)PulserPeaks[i]->Clone("h");
    //  TF1 *fit = new TF1("fit",gaus,0,4095,3*npeaks);
    //  fit->SetNpx(1000);
    //  h->Fit("fit");
    }
  }

  delete c1;
  delete s;

  return;
}
