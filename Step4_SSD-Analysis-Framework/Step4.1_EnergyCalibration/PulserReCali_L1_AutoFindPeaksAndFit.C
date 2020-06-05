
/////////////////////////////////////////////////////////////////////////
//    This macro is used to find the peaks in pulser calibration data
//    Here, we use TSpectrum to find the peaks candidates
//
//    1.对 SSD1 与 SSD2 的第一层进行 重新刻度, 原因是放射源刻度时，这两层硅条
//      的看不到信号，因而增大了放大倍数
//
//    2.需要注意以下的不同：
//      (1) SSD1_L1: gain=4   => gain=20
//      (2) SSD2_L1: gain=7.4 => gain=20
//
//    3.Pulser 与 放射源刻度完以后, 需要将刻度曲线转换回到原来的放大倍数的情况
//
//    Author gfh
//    Date May 27, 2020
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
Double_t AttenFactor[11] = {0};     //衰减因子
Double_t Switch5[5]   = {1./4, 1./5, 1./10, 1./20, 1./40};
Double_t Switch8[8]   = {1., 1./2, 1./4, 1./5, 1./10, 1./20, 1./40, 1./50};
Double_t Switch9[9]   = {1., 1/2., 1/4., 1/5., 1/10., 1/20., 1/40., 1/50., 1/100.};
Double_t Height10[10] = {1./4, .9/4, .8/4, .7/4, .6/4, .5/4, .4/4, .3/4, .2/4, .1/4};

// for SSD_L1_PulserReCali_Switch
Double_t Switch6[6]  = {1., 1./2, 1./4, 1./5, 1./10, 1./20};      // for SSD2_L1_Gain7.4_Switch
Double_t Switch6_SSD1_Gain20[6]  = {1./4, 1./5, 1./10, 1./20, 1./40, 1./50};  // for SSD1_L1_Gain20_Switch


// SSD_L1_PulserReCali_Height
// 对 Height 进行归一, 相对幅度 10./10,9./10,8./10,....,0.5/10
Double_t Height11[11] = {1.,.9, .8, .7, .6, .5, .4, .3, .2, .1, 0.05};
Double_t Height11_SSD2_Gain20[11] = {1./4, .9/4, .8/4, .7/4, .6/4, .5/4, .4/4, .3/4, .2/4, .1/4, 0.05/4};  // for SSD2_L1_Gain20_Hight

Double_t RangeXLowCut = 120;   // for Gain74: Cut=120; Gain20: Cut=250(Switch) or 350(Height)
Double_t RangeXLowCutSSD2 = 120;
//_______________________________________________
//   实现数组的降序排序
bool compare(Int_t a, Int_t b)
{
  return a>b;
}


//______________________________________________________________________________
void PulserCali_AutoFindPeak(const char* L1Tag, const char* FileTag, const char* GainTag, TCanvas* cans[4][16]);


//______________________________________________________________________________
void PulserReCali_L1_AutoFindPeaksAndFit()
{
  gStyle->SetOptStat(0);

  std::string L1Tag("L1");
  std::string L1STag("L1S");
  std::string FileTag("Switch");   // "Height" or "Switch"
  std::string GainTag("Gain4");  // SSD1_L1: Gain4, Gain20; SSD2_L1: Gain7.4, Gain20

  std::string pathPDFOutput(Form("figures/SSD_%s_PulserCali_%s_%s.pdf", L1STag.c_str(),GainTag.c_str(),FileTag.c_str()));
  std::string pathPDFbegin(Form("figures/SSD_%s_PulserCali_%s_%s.pdf[", L1STag.c_str(),GainTag.c_str(),FileTag.c_str()));
  std::string pathPDFend(Form("figures/SSD_%s_PulserCali_%s_%s.pdf]",   L1STag.c_str(),GainTag.c_str(),FileTag.c_str()));

  TCanvas *cans[4][16];
  for(Int_t SSDNum=0; SSDNum<4; SSDNum++)
  {
    for(int CHNum=0; CHNum<16; CHNum++)
    {
      cans[SSDNum][CHNum] = new TCanvas(Form("C_SSD%d_CH%d",SSDNum+1,CHNum),Form("C_SSD%d_CH%d",SSDNum+1,CHNum),10,10,1000,900);
      cans[SSDNum][CHNum]->Close();
    }
  }
  PulserCali_AutoFindPeak(L1Tag.c_str(), FileTag.c_str(),GainTag.c_str(),cans);

  TCanvas *c_begin = new TCanvas("c_begin","");
  TCanvas *c_end   = new TCanvas("c_end","");
  c_end->Close();
  c_begin->Close();
  c_begin->Print(pathPDFbegin.c_str());

  for (Int_t SSDNum=0; SSDNum<4; SSDNum++)
  {
    for (int CHNum=0; CHNum<16; CHNum++)
    {
        cans[SSDNum][CHNum]->Print(pathPDFOutput.c_str());
    }
  }
  c_end->Print(pathPDFend.c_str());
  return;
}


//______________________________________________________________________________
void PulserCali_AutoFindPeak(const char* L1Tag, const char* FileTag, const char* GainTag, TCanvas* cans[4][16])
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
//   L1Tag   : L1 or L2                                                    //
//   FileTag    : Switch / Height，We use this two methods for pulser cali    //
//   cans[4][16]: Canvas for to save all the figures                          //
//                                                                            //
//   Search()参数说明：                                                        //
//   Search(hist, sigma, option, threshold) \\ 0 < threshold < 1              //
//   sigma    : sigma of searched peaks， search                              //
//   threshold: peaks with amplitude < threshold*highest_peak are discarded   //
////////////////////////////////////////////////////////////////////////////////

  std::string gain20tag("Gain20");

  std::string pathPulserCaliParsOutput(Form("output/SSD_%sS_PulserReCali_%s_%s.dat",L1Tag,GainTag,FileTag));

  FILE * FileOut = fopen(pathPulserCaliParsOutput.c_str(),"w");
  fprintf(FileOut,"* Fiiting funtion = par[0] + par[1]*x && y=a*x+b (y=Energy, x=Ch), so a = par[1], b = par[0];  0.0 isn't a peak\n");
  fprintf(FileOut,"* SSDNum CHNum    par0(a)   err_par0     par1(b)      err_par1      "
	    	  " peak1     peak2     peak3      peak4     peak5  "
		      " peak6     peak7     peak8      peak9     peak10    peak11 \n");

  TH1D* PulserPeaks[4][16];
  for(Int_t SSDNum=0; SSDNum<1; SSDNum++)
  {
    //std::string pathRootInput(Form("/data/EXPdata/Fission2019/QualityCheck/QC_MapSSD%d_%s_PulserCali_%s0000.root", SSDNum+1, L1Tag, FileTag));
    std::string pathRootInput(Form("rootinput/QC_MapSSD%d_%s_PulserReCali_%s_%s0000.root", SSDNum+1, L1Tag, GainTag, FileTag));

    TFile* FileIn = new TFile(pathRootInput.c_str());
    if (!FileIn->IsOpen())
    {  //  cans[SSDNum][i]->Close();
      cout<<"Open file "<< pathRootInput.c_str() << " failed"<<endl;
      return;
    }
    // 读取root文件中的 Histograms
    for(Int_t CHNum=0; CHNum<16; CHNum++)
    {
      PulserPeaks[SSDNum][CHNum] = (TH1D*)FileIn->Get(Form("SSD%d_%sS_E_CH%02d",SSDNum+1,L1Tag,CHNum));
      if ((SSDNum==1)&&(strcmp(GainTag,gain20tag.c_str())==0)) {
        PulserPeaks[SSDNum][CHNum]->GetXaxis()->SetRangeUser(RangeXLowCutSSD2,4095);
      } else {
        PulserPeaks[SSDNum][CHNum]->GetXaxis()->SetRangeUser(RangeXLowCut,4095);
      }

      if (PulserPeaks[SSDNum][CHNum]==0) continue;
      cout << Form("SSD%d_%sS_E_CH%d",SSDNum+1,L1Tag,CHNum) << endl;
    }
    printf("Histograms loaded\n");

    //============================================================================
    //                        BEGIN ANALYZE HERE !!!
    //============================================================================
    TSpectrum* s = new TSpectrum();
    for(Int_t CHNum=0; CHNum<16; CHNum++)
    {
      if (PulserPeaks[SSDNum][CHNum]==0)
      {
        printf("No data present for SSD%d_%sS_E_CH%02d\n",SSDNum+1,L1Tag,CHNum);
        continue;
      }
      // find peaks candidates
      // Search(hist, sigma, option, threshold) \\ 0 < threshold < 1
      cans[SSDNum][CHNum] = new TCanvas(Form("C_SSD%d_CH%d",SSDNum+1,CHNum),Form("C_SSD%d_CH%d",SSDNum+1,CHNum),10,10,1000,900);
      cans[SSDNum][CHNum]->Divide(1,2);
      cans[SSDNum][CHNum]->cd(1);
      Int_t nfound  = s->Search(PulserPeaks[SSDNum][CHNum],3,"",0.1);
      printf("SSD%d_%sS_E_CH%d is analyzed,%2d peaks found\n",SSDNum+1,L1Tag,CHNum,nfound);
      //  Loop on all found peaks
      Int_t npeaks = 0;
      Double_t* xpeaks = s->GetPositionX();
      // 对寻找到的峰值降序排序
      std::sort(xpeaks,xpeaks+nfound,compare);   // sort默认升序,compare使之变成降序
      for(Int_t p=0; p<nfound; p++)
      {
        if(xpeaks[p]<120) continue;
        Double_t xp = xpeaks[p];
        Int_t   bin = PulserPeaks[SSDNum][CHNum]->GetXaxis()->FindBin(xp);
        Double_t yp = PulserPeaks[SSDNum][CHNum]->GetBinContent(bin);

        printf(" SSD%d_%sS_E_CH%d peak %d is %.1f\n",SSDNum+1,L1Tag,CHNum,p,xp);
        npeaks++;
      }
      cout<<"有效的peak数目为:"<< npeaks << endl;
      cout<<"Now fitting: Be patient\n" <<endl;

      for(Int_t i=0; i<npeaks; i++)
      {
        if (npeaks==5)  AttenFactor[i] = Switch5[i];
        if (npeaks==6)  {
          if ((SSDNum==0)&&(strcmp(GainTag,gain20tag.c_str())==0)) {
            AttenFactor[i] = Switch6_SSD1_Gain20[i];
          } else {
            AttenFactor[i] = Switch6[i];
          }
        }
        if (npeaks==8)  AttenFactor[i] = Switch8[i];
        if (npeaks==9)  AttenFactor[i] = Switch9[i];
        if (npeaks==10) AttenFactor[i] = Height10[i];
        if (npeaks==11) {
          if ((SSDNum==1)&&(strcmp(GainTag,gain20tag.c_str())==0)) {
            AttenFactor[i] = Height11_SSD2_Gain20[i];
          } else {
            AttenFactor[i] = Height11[i];
          }
        }
      }
      cans[SSDNum][CHNum]->cd(2);
      gPad->SetGridx();
      gPad->SetGridy();
      TGraph *grap = new TGraph(npeaks,xpeaks,AttenFactor);  // Energy vs Ch (y = Enegry, x = channel)
      grap->SetMarkerStyle(20);
      grap->SetMarkerSize(1.5);
      grap->SetMarkerColor(kBlue);
      grap->SetTitle(Form("PulserFit_SSD%d_%sS_E_CH%02d",SSDNum+1,L1Tag,CHNum));
      grap->GetXaxis()->SetTitle("ADC Channel");
      grap->GetXaxis()->CenterTitle(1);
      grap->GetXaxis()->SetTitleSize(0.04);
      grap->GetYaxis()->SetTitle("Relative_Pulser_Height");
      grap->GetYaxis()->CenterTitle(1);
      grap->GetYaxis()->SetRangeUser(0.,1.1);
      grap->GetYaxis()->SetNdivisions(511);
      grap->Draw("AP*");

      TF1 * fit = new TF1("fit","pol1",100,4096);
      grap->Fit("fit");
      Double_t par0     = fit->GetParameter(1);   // fitting function = par[0]+par[1]*x
      Double_t err_par0 = fit->GetParError(1);
      Double_t par1     = fit->GetParameter(0);
      Double_t err_par1 = fit->GetParError(0);
      fprintf(FileOut,"%-5d    %-2d    %-10.8f  %-10.8f  %-10.8f  %-10.8f   "
		      "%-8.1f  %-8.1f  %-8.1f   %-8.1f   %-8.1f   %-8.1f   "
		      "%-8.1f  %-8.1f  %-8.1f   %-8.1f   %-8.1f \n ",
                      SSDNum,CHNum,par0,err_par0,par1,err_par1,
                      xpeaks[0],xpeaks[1],xpeaks[2],xpeaks[3],xpeaks[4],
                      (npeaks>5 ? xpeaks[5]:0.), (npeaks>6  ? xpeaks[6]:0.),
                      (npeaks>7 ? xpeaks[7]:0.), (npeaks>8  ? xpeaks[8]:0.),
                      (npeaks>9 ? xpeaks[9]:0.), (npeaks>10 ? xpeaks[10]:0.));
      fflush(FileOut);
      gPad->Modified();
      gPad->Update();
  //    getchar();
    }// ====  close loop of strip number
  } // =====  close loop of SSDNum
  fclose(FileOut);
  return;
}
