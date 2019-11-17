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
#include "TFile.h"

//______________________________________________________________________________
//  const number definition
Double_t AttenFactorF[11] = {0};     //衰减因子
Double_t AttenFactorB[11] = {0};     //衰减因子
Double_t Switch5[5]   = {1.0, 1/2.0, 1/4.0, 1/5.0, 1/10.0};
Double_t Switch6[6]   = {1.0, 1/2.0, 1/4.0, 1/5.0, 1/10.0, 1/20.0};
Double_t Switch7[7]   = {1.0, 1/2.0, 1/4.0, 1/5.0, 1/10.0, 1/20.0, 1/40.0};
Double_t Switch8[8]   = {1.0, 1/2.0, 1/4.0, 1/5.0, 1/10.0, 1/20.0, 1/40.0, 1/50.0};
Double_t Height10[10] = {10.0, 9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0};
Double_t Height11[11] = {10.0, 9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0, 0.5};


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

  std::string LayerTag("L2");
  std::string FileTag("Height");   // "Height" or "Switch"
  std::string pdfpath(Form("figures/SSD_%s_PulserCali_%s.pdf",LayerTag.c_str(),FileTag.c_str()));

  TCanvas *cans[4][16];
  for(Int_t SSDNum=0; SSDNum<4; SSDNum++)
  {
    for(int CHNum=0; CHNum<16; CHNum++)
    {
      cans[SSDNum][CHNum] = new TCanvas(Form("C_SSD%d_CH%d",SSDNum+1,CHNum),Form("C_SSD%d_CH%d",SSDNum+1,CHNum),10,10,1000,900);
      cans[SSDNum][CHNum]->Close();
    }
  }
  PulserCali_AutoFindPeak(LayerTag.c_str(),FileTag.c_str(),cans);

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
    for(int CHNum=0; CHNum<16; CHNum++)
    {
        cans[SSDNum][CHNum]->Print(pdfpath.c_str());
    }
  }
  c_end->Print(pdfpath_end.c_str());
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
  std::string L2F_outputpath(Form("output/SSD_%sF_PulserCali_%s.dat",LayerTag,FileTag));
  std::string L2B_outputpath(Form("output/SSD_%sB_PulserCali_%s.dat",LayerTag,FileTag));
  FILE * FileOutF = fopen(L2F_outputpath.c_str(),"w");
  FILE * FileOutB = fopen(L2B_outputpath.c_str(),"w");
  fprintf(FileOutF,"* Fiiting funtion = par[0] + par[1]*x && y=a*x+b (y=Energy, x=Ch), so a = par[1], b = par[0];  0.0 isn't a peak\n");
  fprintf(FileOutF,"* SSDNum CHNum    par1(a)   err_par0     par0(b)      err_par1      peak1     peak2      peak3      peak4     peak5     peak6     peak7     peak8     peak9     peak10   peak11 \n");
  fprintf(FileOutB,"* Fiiting funtion = par[0] + par[1]*x && y=a*x+b, so a = par[1], b = par[0];  0.0 isn't a peak\n");
  fprintf(FileOutB,"* SSDNum CHNum    par1(a)   err_par0     par0(b)      err_par1      peak1     peak2      peak3      peak4     peak5     peak6     peak7     peak8     peak9     peak10   peak11 \n");

  TH1D * L2F_PulserPeaks[4][16];
  TH1D * L2B_PulserPeaks[4][16];
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
    for(int CHNum=0; CHNum<16; CHNum++)
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
      if(L2F_PulserPeaks[SSDNum][CHNum]==0)
      {
        printf("No data present for SSD%d_%sF_E_CH%02d\n",SSDNum+1,LayerTag,CHNum);
        continue;
      }
      if(L2B_PulserPeaks[SSDNum][CHNum]==0)
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
      cans[SSDNum][CHNum]->cd(3);
      Int_t nfoundB  = s_B->Search(L2B_PulserPeaks[SSDNum][CHNum],3,"",0.2);
      printf("SSD%d_%sF_E_CH%d is analyzed,%2d peaks found\n",SSDNum+1,LayerTag,CHNum,nfoundF);
      printf("SSD%d_%sB_E_CH%d is analyzed,%2d peaks found\n",SSDNum+1,LayerTag,CHNum,nfoundB);
      //  Loop on all found peaks

      Int_t npeaksF = 0;
      Int_t npeaksB = 0;
      Double_t *xpeaksF = s_F->GetPositionX();
      Double_t *xpeaksB = s_B->GetPositionX();
      // 对寻找到的峰值降序排序
      std::sort(xpeaksF,xpeaksF+nfoundF,compare);   // sort默认升序,compare使之变成降序
      std::sort(xpeaksB,xpeaksB+nfoundB,compare);
      for(Int_t p=0; p<nfoundF; p++)
      {
        if(xpeaksF[p]<120) continue;
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
        if(npeaksF==5)  AttenFactorF[i] = Switch5[i];
        if(npeaksF==6)  AttenFactorF[i] = Switch6[i];
        if(npeaksF==7)  AttenFactorF[i] = Switch7[i];
        if(npeaksF==8)  AttenFactorF[i] = Switch8[i];
        if(npeaksF==10) AttenFactorF[i] = Height10[i];
        if(npeaksF==11) AttenFactorF[i] = Height11[i];
      }
      for(Int_t i=0; i<npeaksB; i++)
      {
        if(npeaksB==5)  AttenFactorB[i] = Switch5[i];
        if(npeaksB==6)  AttenFactorB[i] = Switch6[i];
        if(npeaksB==7)  AttenFactorB[i] = Switch7[i];
        if(npeaksB==8)  AttenFactorB[i] = Switch8[i];
        if(npeaksB==10) AttenFactorB[i] = Height10[i];
        if(npeaksB==11) AttenFactorB[i] = Height11[i];
      }
      cans[SSDNum][CHNum]->cd(2);
      TGraph *grapF = new TGraph(npeaksF,xpeaksF,AttenFactorF); //Energy vs Ch (y = Enegry, x = channel)
      grapF->SetMarkerStyle(20);
      grapF->SetMarkerSize(1.5);
      grapF->SetMarkerColor(kBlue);
      grapF->SetTitle(Form("PulserFit_SSD%d_%sF_E_CH%02d",SSDNum+1,LayerTag,CHNum));
      grapF->Draw("AP*");
      TF1 * fitF = new TF1("fitF","pol1",100,4096);
      grapF->Fit("fitF");
      Double_t par0_F     = fitF->GetParameter(1);   // fitting function = par[0]+par[1]*x
      Double_t err_par0_F = fitF->GetParError(1);
      Double_t par1_F     = fitF->GetParameter(0);
      Double_t err_par1_F = fitF->GetParError(0);

      cans[SSDNum][CHNum]->cd(4);
      TGraph *grapB = new TGraph(npeaksB,xpeaksB,AttenFactorB); //Energy vs Ch (y = Enegry, x = channel)
      grapB->SetMarkerStyle(20);
      grapB->SetMarkerSize(1.5);
      grapB->SetMarkerColor(kBlue);
      grapB->SetTitle(Form("PulserFit_SSD%d_%sB_E_CH%02d",SSDNum+1,LayerTag,CHNum));
      grapB->Draw("AP*");
      TF1 * fitB = new TF1("fitB","pol1",100,4096);
      grapB->Fit("fitB");
      Double_t par0_B     = fitB->GetParameter(1);   // fitting function = par[0]+par[1]*x
      Double_t err_par0_B = fitB->GetParError(1);
      Double_t par1_B     = fitB->GetParameter(0);
      Double_t err_par1_B = fitB->GetParError(0);

      fprintf(FileOutF," %5d    %2d   %10.4f  %10.4f  %10.4f   %10.4f   %8.1f   %8.1f   %8.1f   %8.1f  %8.1f  %8.1f  %8.1f  %8.1f  %8.1f  %8.1f  %8.1f\n",
      SSDNum,CHNum,par0_F,err_par0_F,par1_F,err_par1_F,xpeaksF[0],xpeaksF[1],xpeaksF[2],xpeaksF[3],xpeaksF[4],xpeaksF[5],xpeaksF[6],xpeaksF[7],xpeaksF[8],xpeaksF[9],xpeaksF[10]);
      fprintf(FileOutB," %5d    %2d   %10.4f  %10.4f  %10.4f   %10.4f   %8.1f   %8.1f   %8.1f   %8.1f  %8.1f  %8.1f  %8.1f  %8.1f  %8.1f  %8.1f  %8.1f\n",
      SSDNum,CHNum,par0_B,err_par0_B,par1_B,err_par1_B,xpeaksB[0],xpeaksB[1],xpeaksB[2],xpeaksB[3],xpeaksB[4],xpeaksB[5],xpeaksB[6],xpeaksB[7],xpeaksB[8],xpeaksB[9],xpeaksB[10]);
      fflush(FileOutF); // 需要加上这句！！！ 否则由于缓存问题，内容不被写入
      fflush(FileOutB);

      gPad->Modified();
      gPad->Update();
  //    getchar();    */
    }// ====  close loop of strip number
  } // =====  close loop of SSDNum
  fclose(FileOutF);
  fclose(FileOutB);
  return;
}
