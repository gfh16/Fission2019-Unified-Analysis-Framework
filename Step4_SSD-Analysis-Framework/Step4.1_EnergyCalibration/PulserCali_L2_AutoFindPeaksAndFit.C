////////////////////////////////////////////////////////////////////////////////
//
//    自动寻峰进行脉冲刻度. 刻度分两步完成:
//      1. 自动寻峰, 同时使用函数 sort() 对峰值进行倒序排序
//      2. 使用一次多项式对所有峰值进行拟合, 同时画出拟合直线, 保存拟合结果
//
//    由于脉冲刻度, 只需要知道峰值即可, 因此使用自动寻峰是最方便的. 但由于自动寻峰是乱序的,
//    需要认为对峰值进行排序. 使用方法 std::sort().
//
//    脉冲刻度时, 将最大脉冲归一成 1.0，然后按比例衰减
//
//    gfh, 2019-11-11
////////////////////////////////////////////////////////////////////////////////

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
Double_t AttenFactorF[11] = {0};     //衰减因子
Double_t AttenFactorB[11] = {0};     //衰减因子
Double_t Switch5[5]   = {1., 1/2., 1/4., 1/5., 1/10.};
Double_t Switch6[6]   = {1., 1/2., 1/4., 1/5., 1/10., 1/20.};
Double_t Switch8[8]   = {1., 1/2., 1/4., 1/5., 1/10., 1/20., 1/40., 1/50.};
Double_t Switch9[9]   = {1., 1/2., 1/4., 1/5., 1/10., 1/20., 1/40., 1/50., 1/100.};

Double_t Height10[10] = {1., .9, .8, .7, .6, .5, .4, .3, .2, .1};
Double_t Height11[11] = {1., .9, .8, .7, .6, .5, .4, .3, .2, .1, 0.05};


//_______________________________________________
//  实现数组的降序排序
bool compare(Int_t a, Int_t b)
{
  return a>b;
}

//______________________________________________________________________________
void PulserCali_AutoFindPeak(const char* LayerTag, const char* FileTag, TCanvas* cans[4][16]);

//______________________________________________________________________________
void PulserCali_L2_AutoFindPeaksAndFit()
{
  gStyle->SetOptStat(0);

  std::string L2Tag("L2");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string FileTag("Height");   // "Height" or "Switch"

  std::string pathPDFOutput(Form("figures/SSD_%s_PulserCali_%s.pdf",L2Tag.c_str(),FileTag.c_str()));
  std::string pathPDFbegin(Form("figures/SSD_%s_PulserCali_%s.pdf[",L2Tag.c_str(),FileTag.c_str()));
  std::string pathPDFend(Form("figures/SSD_%s_PulserCali_%s.pdf]",L2Tag.c_str(),FileTag.c_str()));

  TCanvas *cans[4][16];
  for(Int_t SSDNum=0; SSDNum<4; SSDNum++)
  {
    for(int CHNum=0; CHNum<16; CHNum++)
    {
      cans[SSDNum][CHNum] = new TCanvas(Form("C_SSD%d_CH%d",SSDNum+1,CHNum),Form("C_SSD%d_CH%d",SSDNum+1,CHNum),10,10,1000,900);
      cans[SSDNum][CHNum]->Close();
    }
  }
  // ！！！ 实现自动寻峰
  PulserCali_AutoFindPeak(L2Tag.c_str(),FileTag.c_str(),cans);

  TCanvas *c_begin = new TCanvas("c_begin","");
  TCanvas *c_end   = new TCanvas("c_end","");
  c_end->Close();
  c_begin->Close();
  c_begin->Print(pathPDFbegin.c_str());

  for(Int_t SSDNum=0; SSDNum<4; SSDNum++)
  {
    for(Int_t CHNum=0; CHNum<16; CHNum++)
    {
        cans[SSDNum][CHNum]->Print(pathPDFOutput.c_str());
    }
  }
  c_end->Print(pathPDFend.c_str());

  return;
}


//______________________________________________________________________________
void PulserCali_AutoFindPeak(const char* LayerTag, const char* FileTag, TCanvas* cans[4][16])
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

  std::string pathPulserCaliParsOutput_L2F(Form("output/SSD_%sF_PulserCali_%s.dat",LayerTag,FileTag));
  std::string pathPulserCaliParsOutput_L2B(Form("output/SSD_%sB_PulserCali_%s.dat",LayerTag,FileTag));

  FILE * FileOutF = fopen(pathPulserCaliParsOutput_L2F.c_str(),"w");
  FILE * FileOutB = fopen(pathPulserCaliParsOutput_L2B.c_str(),"w");
  fprintf(FileOutF,"* Fiiting funtion = par[0] + par[1]*x && y=a*x+b (y=Energy, x=Ch), so a = par[1], b = par[0]\n");
  fprintf(FileOutF,"* SSDNum CHNum    par1(a)   err_par0     par0(b)      err_par1     "
	           "  peak1     peak2      peak3      peak4     peak5  "
		   "  peak6     peak7     peak8     peak9     peak10   peak11 \n");
  fprintf(FileOutB,"* Fiiting funtion = par[0] + par[1]*x && y=a*x+b, so a = par[1], b = par[0]\n");
  fprintf(FileOutB,"* SSDNum CHNum    par1(a)   err_par0     par0(b)      err_par1    "
	           "  peak1     peak2     peak3     peak4     peak5  "
	           "  peak6     peak7     peak8     peak9     peak10    peak11 \n");

  TH1D * L2F_PulserPeaks[4][16];
  TH1D * L2B_PulserPeaks[4][16];
  for(Int_t SSDNum=0; SSDNum<4; SSDNum++)
  {

    std::string pathRootInput(Form("rootinput/QC_MapSSD%d_%s_PulserCali_%s0000.root", SSDNum+1,LayerTag,FileTag));

    TFile * FileIn = new TFile(pathRootInput.c_str());
    if (!FileIn->IsOpen())
    {  //  cans[SSDNum][i]->Close();
      cout<<"Open file "<< pathRootInput.c_str() << " failed"<<endl;
      return;
    }
    // 读取root文件中的 Histograms
    for(Int_t CHNum=0; CHNum<16; CHNum++)
    {
      L2F_PulserPeaks[SSDNum][CHNum] = (TH1D*)FileIn->Get(Form("SSD%d_%sF_E_CH%02d",SSDNum+1,LayerTag,CHNum));
      L2B_PulserPeaks[SSDNum][CHNum] = (TH1D*)FileIn->Get(Form("SSD%d_%sB_E_CH%02d",SSDNum+1,LayerTag,CHNum));
      L2F_PulserPeaks[SSDNum][CHNum]->GetXaxis()->SetRangeUser(0,4095);
      L2B_PulserPeaks[SSDNum][CHNum]->GetXaxis()->SetRangeUser(0,4095);
      cout << Form("SSD%d_%sF_E_CH%d",SSDNum+1,LayerTag,CHNum) << endl;
      cout << Form("SSD%d_%sB_E_CH%d",SSDNum+1,LayerTag,CHNum) << endl;
    }
    printf("Histograms loaded\n");

    //============================================================================
    //                        BEGIN ANALYZE HERE !!!
    //============================================================================
    TSpectrum * s_F = new TSpectrum();
    TSpectrum * s_B = new TSpectrum();
    for(Int_t CHNum=0; CHNum<16; CHNum++)
    {
      if (L2F_PulserPeaks[SSDNum][CHNum]==0)
      {
        printf("No data present for SSD%d_%sF_E_CH%02d\n",SSDNum+1,LayerTag,CHNum);
        continue;
      }
      if (L2B_PulserPeaks[SSDNum][CHNum]==0)
      {
        printf("No data present for SSD%d_%sB_E_CH%02d\n",SSDNum+1,LayerTag,CHNum);
        continue;
      }
      // find peaks candidates
      // Search(hist, sigma, option, threshold) \\ 0 < threshold < 1
      cans[SSDNum][CHNum] = new TCanvas(Form("C_SSD%d_CH%d",SSDNum+1,CHNum),Form("C_SSD%d_CH%d",SSDNum+1,CHNum),10,10,1000,900);
      cans[SSDNum][CHNum]->Divide(2,2);
      cans[SSDNum][CHNum]->cd(1);
      Int_t nfoundF  = s_F->Search(L2F_PulserPeaks[SSDNum][CHNum],3,"",0.2);
      cans[SSDNum][CHNum]->cd(2);
      Int_t nfoundB  = s_B->Search(L2B_PulserPeaks[SSDNum][CHNum],3,"",0.2);
      printf("SSD%d_%sF_E_CH%d is analyzed,%2d peaks found\n",SSDNum+1,LayerTag,CHNum,nfoundF);
      printf("SSD%d_%sB_E_CH%d is analyzed,%2d peaks found\n",SSDNum+1,LayerTag,CHNum,nfoundB);

      Int_t npeaksF = 0;
      Int_t npeaksB = 0;
      Double_t *xpeaksF = s_F->GetPositionX();
      Double_t *xpeaksB = s_B->GetPositionX();
      // 对寻找到的峰值降序排序
      std::sort(xpeaksF,xpeaksF+nfoundF,compare);   // sort默认升序,compare使之变成降序
      std::sort(xpeaksB,xpeaksB+nfoundB,compare);
      for(Int_t p=0; p<nfoundF; p++)
      {
        if (xpeaksF[p]<120) continue;
        Double_t xp_F = xpeaksF[p];
        Int_t   bin_F = L2F_PulserPeaks[SSDNum][CHNum]->GetXaxis()->FindBin(xp_F);
        Double_t yp_F = L2F_PulserPeaks[SSDNum][CHNum]->GetBinContent(bin_F);

        printf(" SSD%d_%sL_E_CH%d peak %d is %.1f\n",SSDNum+1,LayerTag,CHNum,p,xp_F);
        npeaksF++;
      }
      cout<<"SSD"<<SSDNum<<"_L2F 有效的peak数目为:"<< npeaksF << endl;
      for(Int_t p=0; p<nfoundB; p++)
      {
      //  if(xpeaksB[p]<120) continue;
        Double_t xp_B = xpeaksB[p];
        Int_t   bin_B = L2B_PulserPeaks[SSDNum][CHNum]->GetXaxis()->FindBin(xp_B);
        Double_t yp_B = L2B_PulserPeaks[SSDNum][CHNum]->GetBinContent(bin_B);

        printf(" SSD%d_%sL_E_CH%d peak %d is %.1f\n",SSDNum+1,LayerTag,CHNum,p,xp_B);
        npeaksB++;
      }
      cout<<"SSD"<<SSDNum <<"_L2B 有效的peak数目为:"<< npeaksB << endl;

      for(Int_t i=0; i<npeaksF; i++)
      {
        if (npeaksF==5)  AttenFactorF[i] = Switch5[i];
        if (npeaksF==6)  AttenFactorF[i] = Switch6[i];
        if (npeaksF==8)  AttenFactorF[i] = Switch8[i];
        if (npeaksF==9)  AttenFactorF[i] = Switch9[i];
        if (npeaksF==10) AttenFactorF[i] = Height10[i];
        if (npeaksF==11) AttenFactorF[i] = Height11[i];
      }
      for(Int_t i=0; i<npeaksB; i++)
      {
        if (npeaksB==5)  AttenFactorB[i] = Switch5[i];
        if (npeaksB==6)  AttenFactorB[i] = Switch6[i];
        if (npeaksB==8)  AttenFactorB[i] = Switch8[i];
        if (npeaksB==9)  AttenFactorB[i] = Switch9[i];
        if (npeaksB==10) AttenFactorB[i] = Height10[i];
        if (npeaksB==11) AttenFactorB[i] = Height11[i];
      }
      cans[SSDNum][CHNum]->cd(3);
      gPad->SetGridx();
      gPad->SetGridy();
      TGraph *grapF = new TGraph(npeaksF,xpeaksF,AttenFactorF); //Energy vs Ch (y = Enegry, x = channel)
      grapF->SetMarkerStyle(20);
      grapF->SetMarkerSize(1.5);
      grapF->SetMarkerColor(kBlue);
      grapF->SetTitle(Form("PulserFit_SSD%d_%sF_E_CH%02d",SSDNum+1,LayerTag,CHNum));
      grapF->GetXaxis()->SetTitle("ADC Channel");
      grapF->GetXaxis()->CenterTitle(1);
      grapF->GetXaxis()->SetTitleSize(0.04);
      grapF->GetYaxis()->SetTitle("Relative_Pulser_Height");
      grapF->GetYaxis()->CenterTitle(1);
      grapF->GetYaxis()->SetTitleSize(0.04);
      grapF->GetYaxis()->SetRangeUser(0.,1.1);
      grapF->GetYaxis()->SetNdivisions(511);
      grapF->Draw("AP*");
      TF1* fitF = new TF1("fitF","pol1",100,4096);
      grapF->Fit("fitF");
      Double_t par0_F     = fitF->GetParameter(1);   // fitting function = par[0]+par[1]*x
      Double_t err_par0_F = fitF->GetParError(1);
      Double_t par1_F     = fitF->GetParameter(0);
      Double_t err_par1_F = fitF->GetParError(0);

      cans[SSDNum][CHNum]->cd(4);
      gPad->SetGridx();
      gPad->SetGridy();
      TGraph *grapB = new TGraph(npeaksB,xpeaksB,AttenFactorB); //Energy vs Ch (y = Enegry, x = channel)
      grapB->SetMarkerStyle(20);
      grapB->SetMarkerSize(1.5);
      grapB->SetMarkerColor(kBlue);
      grapB->SetTitle(Form("PulserFit_SSD%d_%sB_E_CH%02d",SSDNum+1,LayerTag,CHNum));
      grapB->GetXaxis()->SetTitle("ADC Channel");
      grapB->GetXaxis()->CenterTitle(1);
      grapB->GetXaxis()->SetTitleSize(0.04);
      grapB->GetYaxis()->SetTitle("Relative_Pulser_Height");
      grapB->GetYaxis()->CenterTitle(1);
      grapB->GetYaxis()->SetTitleSize(0.04);
      grapB->GetYaxis()->SetRangeUser(0.,1.1);
      grapB->GetYaxis()->SetNdivisions(511);
      grapB->Draw("AP*");
      TF1* fitB = new TF1("fitB","pol1",100,4096);
      grapB->Fit("fitB");
      Double_t par0_B     = fitB->GetParameter(1);   // fitting function = par[0]+par[1]*x
      Double_t err_par0_B = fitB->GetParError(1);
      Double_t par1_B     = fitB->GetParameter(0);
      Double_t err_par1_B = fitB->GetParError(0);

      fprintf(FileOutF," %5d    %2d   %10.8f  %10.8f  %10.8f   %10.8f "
		       " %8.1f   %8.1f   %8.1f   %8.1f  %8.1f  %8.1f  "
		       " %8.1f  %8.1f  %8.1f  %8.1f  %8.1f\n",
                       SSDNum,CHNum,par0_F,err_par0_F,par1_F,err_par1_F,
		        xpeaksF[0],xpeaksF[1],xpeaksF[2],xpeaksF[3],xpeaksF[4],
		       (npeaksF>5 ? xpeaksF[5]:0.), (npeaksF>6 ? xpeaksF[6]:0.),
		       (npeaksF>7 ? xpeaksF[7]:0.), (npeaksF>8 ? xpeaksF[8]:0.),
		       (npeaksF>9 ? xpeaksF[9]:0.), (npeaksF>10? xpeaksF[10]:0.));
      fprintf(FileOutB," %5d    %2d   %10.8f  %10.8f  %10.8f   %10.8f  "
		       " %8.1f   %8.1f   %8.1f   %8.1f  %8.1f  %8.1f  "
		       " %8.1f  %8.1f  %8.1f  %8.1f  %8.1f\n",
                       SSDNum,CHNum,par0_B,err_par0_B,par1_B,err_par1_B,
		       xpeaksB[0],xpeaksB[1],xpeaksB[2],xpeaksB[3],xpeaksB[4],
		       (npeaksB>5 ? xpeaksB[5]:0.), (npeaksB>6 ? xpeaksB[6]:0.),
		       (npeaksB>7 ? xpeaksB[7]:0.), (npeaksB>8 ? xpeaksB[8]:0.),
		       (npeaksB>9 ? xpeaksB[9]:0.), (npeaksB>10? xpeaksB[10]:0.));
      fflush(FileOutF); // 需要加上这句！！！ 否则由于缓存问题，内容不被写入
      fflush(FileOutB);

      gPad->Modified();
      gPad->Update();
    }// ====  close loop of strip number
  } // =====  close loop of SSDNum
  fclose(FileOutF);
  fclose(FileOutB);
  return;
}
