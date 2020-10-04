#include "../include/CSHINEPulserCali.h"
using namespace std;


//******************************************************************************
CSHINEPulserCali::CSHINEPulserCali()
{}

//_____________________________________________
CSHINEPulserCali::~CSHINEPulserCali()
{}

//______________________________________________________________________________
////////////////////////////////////////////////////////////////////////////////
//    自动寻峰进行脉冲刻度. 刻度分两步完成:
//      1. 自动寻峰, 同时使用函数 sort() 对峰值进行倒序排序
//      2. 使用一次多项式对所有峰值进行拟合, 同时画出拟合直线, 保存拟合结果
//
//    由于脉冲刻度, 只需要知道峰值即可, 因此使用自动寻峰是最方便的. 但由于自动寻峰是乱序的,
//    需要认为对峰值进行排序. 使用方法 std::sort().
//
//    脉冲刻度时, 将最大脉冲归一成 1.0，然后按比例衰减
//
//                           自动寻峰步骤
//                          -------------
//   1.从文件中读取已有的的 hitograms
//   2.新建TSpectrum：TSpectrum * s = new TSpectrum();
//   3.寻峰(寻找候选峰): Int_t nfound  = s->Search(PulserPeaks[i],3,"",0.2);
//   4.对所有峰进行排序
//   5.遍历所有找到的峰，提取峰值的 X值、Y值，拟合
//   6.将寻峰得到的数据点画出来, 作线性拟合
//
//   PulserCali_AutoFindPeak()参数说明
//   L1Tag   : L1 or L2
//   pulserfiletag : Switch / Height，We use this two methods for pulser cali
//   cans[4][16]: Canvas for to save all the figures
//
//   Search()参数说明：
//   Search(hist, sigma, option, threshold) \\ 0 < threshold < 1
//   sigma    : sigma of searched peaks， search
//   threshold: peaks with amplitude < threshold*highest_peak are discarded
//
// gfh, 2019-11-11
// 修改成类, 2020-10-02
////////////////////////////////////////////////////////////////////////////////

// 实现数组的降序排序
bool compare(Int_t a, Int_t b) {
  return a>b;
}

//******************************************************************************
void CSHINEPulserCali::L1_AutoFindPeaksAndFit(const char* pulserfiletag)
{
  gStyle->SetOptStat(0);

  //____________________________________________________________________________
  Double_t AttenFactor[11] = {0};     //衰减因子
  Double_t Switch5[5] = {1., 1/2., 1/4., 1/5., 1/10.};
  Double_t Switch6[6] = {1., 1/2., 1/4., 1/5., 1/10., 1/20.};
  Double_t Switch7[7] = {1/2., 1/4., 1/5., 1/10., 1/20., 1/40., 1/50.};
  Double_t Switch8[8] = {1., 1/2., 1/4., 1/5., 1/10., 1/20., 1/40., 1/50.};
  Double_t Switch9[9] = {1., 1/2., 1/4., 1/5., 1/10., 1/20., 1/40., 1/50., 1/100.};

  Double_t Height10[10] = {1., .9, .8, .7, .6, .5, .4, .3, .2, .1};
  Double_t Height11[11] = {1., .9, .8, .7, .6, .5, .4, .3, .2, .1, 0.05};
  Double_t Height10_SSD1_L1[10] = {.9, .8, .7, .6, .5, .4, .3, .2, .1,0.05};   // used for SSD1_L1, for ch>3500,
                                                                               // the 11th point should be omitted
  Double_t SSD_LowCut = 120;
  Double_t SSD1_L1_RangeXCut = 3500;

  std::string L1Tag("L1");
  std::string L1STag("L1S");

  std::string pathPDFOutput(Form("%sfigure_PulserCali/SSD_%s_PulserCali_%s.pdf",PATHFIGURESFOLDER,L1STag.c_str(),pulserfiletag));
  std::string pathPDFbegin(Form("%sfigure_PulserCali/SSD_%s_PulserCali_%s.pdf[",PATHFIGURESFOLDER,L1STag.c_str(),pulserfiletag));
  std::string pathPDFend(Form("%sfigure_PulserCali/SSD_%s_PulserCali_%s.pdf]"  ,PATHFIGURESFOLDER,L1STag.c_str(),pulserfiletag));

  std::string pathRootInput[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    pathRootInput[i] = Form("%sQualityCheck/QC_MapSSD%d_%s_PulserCali_%s0000.root",PATHROOTFILESFOLDER,i+1,L1Tag.c_str(),pulserfiletag);
  }

  std::string pathPulserCaliParsOutput(Form("%sdata_PulserCali/SSD_%s_PulserCali_%s.dat",PATHDATAFOLDER,L1STag.c_str(),pulserfiletag));
  FILE* FileOut = fopen(pathPulserCaliParsOutput.c_str(),"w");
  fprintf(FileOut,"* Fiiting funtion = par[0] + par[1]*x && y=a*x+b (y=Energy, x=Ch), so a = par[1], b = par[0];  0.0 isn't a peak\n");
  fprintf(FileOut,"* SSDNum CHNum    par0(a)   err_par0     par1(b)      err_par1      "
	    	  " peak1     peak2     peak3      peak4     peak5  "
		      " peak6     peak7     peak8      peak9     peak10    peak11 \n");

  Char_t cans_name[100];
  TCanvas* cans[NUM_SSD][NUM_STRIP];
  for(Int_t SSDNum=0; SSDNum<NUM_SSD; SSDNum++) {
    for(int CHNum=0; CHNum<NUM_STRIP; CHNum++) {
      strcpy(cans_name, Form("C_SSD%d_CH%d",SSDNum+1,CHNum));
      cans[SSDNum][CHNum] = new TCanvas(cans_name,cans_name,10,10,1000,900);
    }
  }
  //____________________________________________________________________________
  TH1D* PulserPeaks[NUM_SSD][NUM_STRIP];
  for(Int_t SSDNum=0; SSDNum<NUM_SSD; SSDNum++)
  {
    TFile* FileIn = new TFile(pathRootInput[SSDNum].c_str());
    if (!FileIn->IsOpen()) {
      cout<<"Open file "<< pathRootInput[SSDNum].c_str() << " failed"<<endl;
      return;
    }
    //________________________________
    // 读取root文件中的 Histograms
    for(Int_t CHNum=0; CHNum<NUM_STRIP; CHNum++)
    {
      PulserPeaks[SSDNum][CHNum] = (TH1D*)FileIn->Get(Form("SSD%d_%s_E_CH%02d",SSDNum+1,L1STag.c_str(),CHNum));
      if (SSDNum==0) {
        PulserPeaks[SSDNum][CHNum]->GetXaxis()->SetRangeUser(SSD_LowCut,SSD1_L1_RangeXCut);
      } else {
        PulserPeaks[SSDNum][CHNum]->GetXaxis()->SetRangeUser(SSD_LowCut,4096);
      }
      cout << Form("SSD%d_%s_E_CH%d",SSDNum+1,L1STag.c_str(),CHNum) << endl;
    }
    printf("Histograms loaded\n");

    //__________________________________________________________________________
    //                        BEGIN ANALYZE HERE !!!
    TSpectrum* s = new TSpectrum();
    for(Int_t CHNum=0; CHNum<NUM_STRIP; CHNum++)
    {
      if (PulserPeaks[SSDNum][CHNum]==0) {
        printf("No data present for SSD%d_%s_E_CH%02d\n",SSDNum+1,L1STag.c_str(),CHNum);
        continue;
      }
      //______________________________________________________________
      // find peaks candidates
      // Search(hist, sigma, option, threshold) \\ 0 < threshold < 1
      cans[SSDNum][CHNum]->Divide(1,2);
      cans[SSDNum][CHNum]->cd(1);
      Int_t nfound  = s->Search(PulserPeaks[SSDNum][CHNum],3,"",0.4);
      printf("SSD%d_%s_E_CH%d is analyzed,%2d peaks found\n",SSDNum+1,L1STag.c_str(),CHNum,nfound);
      Int_t npeaks = 0;
      Double_t* xpeaks = s->GetPositionX();
      std::sort(xpeaks,xpeaks+nfound,compare);   // sort默认升序,compare使之变成降序
      for(Int_t p=0; p<nfound; p++)
      {
        if(xpeaks[p]<SSD_LowCut) continue;
        Double_t xp = xpeaks[p];
        printf(" SSD%d_%s_E_CH%d peak %d is %.1f\n",SSDNum+1,L1STag.c_str(),CHNum,p,xp);
        npeaks++;
      }
      cout<<"有效的peak数目为:"<< npeaks << endl;
      cout<<"Now fitting: Be patient\n" <<endl;

      for(Int_t i=0; i<npeaks; i++)
      {
        if (npeaks==5)  AttenFactor[i] = Switch5[i];
        if (npeaks==6)  AttenFactor[i] = Switch6[i];
        if (npeaks==7)  AttenFactor[i] = Switch7[i];
        if (npeaks==8)  AttenFactor[i] = Switch8[i];
        if (npeaks==9)  AttenFactor[i] = Switch9[i];
        if (npeaks==11) AttenFactor[i] = Height11[i];
        if (npeaks==10) {
          if (SSDNum==0) {
            AttenFactor[i] = Height10_SSD1_L1[i];
          } else {
            AttenFactor[i] = Height10[i];
          }
        }
      }
      cans[SSDNum][CHNum]->cd(2);
      gPad->SetGridx();
      gPad->SetGridy();
      TGraph* grap = new TGraph(npeaks,xpeaks,AttenFactor);  // Energy vs Ch (y = Enegry, x = channel)
      grap->SetMarkerStyle(20);
      grap->SetMarkerSize(1.5);
      grap->SetMarkerColor(kBlue);
      grap->SetTitle(Form("PulserFit_SSD%d_%s_E_CH%02d",SSDNum+1,L1STag.c_str(),CHNum));
      grap->GetXaxis()->SetTitle("ADC Channel");
      grap->GetXaxis()->CenterTitle(1);
      grap->GetXaxis()->SetTitleSize(0.04);
      grap->GetYaxis()->SetRangeUser(0.,1.1);
      grap->GetYaxis()->SetNdivisions(511);
      grap->GetYaxis()->SetTitle("Relative_Pulser_Height");
      grap->GetYaxis()->CenterTitle(1);
      grap->GetYaxis()->SetTitleSize(0.04);
      grap->Draw("AP*");

      TF1* fit = new TF1("fit","pol1",100,3000);
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
    }
  }
  fclose(FileOut);

  TCanvas* c_begin = new TCanvas("c_begin","c_begin",800,600);
  TCanvas* c_end   = new TCanvas("c_end","c_begin",800,600);
  c_end->Close();
  c_begin->Close();
  c_begin->Print(pathPDFbegin.c_str());
  for (Int_t SSDNum=0; SSDNum<NUM_SSD; SSDNum++) {
    for (int CHNum=0; CHNum<NUM_STRIP; CHNum++) {
        cans[SSDNum][CHNum]->Print(pathPDFOutput.c_str());
    }
  }
  c_end->Print(pathPDFend.c_str());
  return;
}
//******************************************************************************


//******************************************************************************
void CSHINEPulserCali::L2_AutoFindPeaksAndFit(const char* pulserfiletag)
{
  gStyle->SetOptStat(0);
  //____________________________________________________________________________
  Double_t AttenFactorF[11] = {0};     //衰减因子
  Double_t AttenFactorB[11] = {0};     //衰减因子
  Double_t Switch5[5] = {1., 1/2., 1/4., 1/5., 1/10.};
  Double_t Switch6[6] = {1., 1/2., 1/4., 1/5., 1/10., 1/20.};
  Double_t Switch8[8] = {1., 1/2., 1/4., 1/5., 1/10., 1/20., 1/40., 1/50.};
  Double_t Switch9[9] = {1., 1/2., 1/4., 1/5., 1/10., 1/20., 1/40., 1/50., 1/100.};

  Double_t Height10[10] = {1., .9, .8, .7, .6, .5, .4, .3, .2, .1};
  Double_t Height11[11] = {1., .9, .8, .7, .6, .5, .4, .3, .2, .1, 0.05};

  std::string L2Tag("L2");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");

  std::string pathPDFOutput(Form("%sfigure_PulserCali/SSD_%s_PulserCali_%s.pdf", PATHFIGURESFOLDER,L2Tag.c_str(),pulserfiletag));
  std::string pathPDFbegin (Form("%sfigure_PulserCali/SSD_%s_PulserCali_%s.pdf[",PATHFIGURESFOLDER,L2Tag.c_str(),pulserfiletag));
  std::string pathPDFend   (Form("%sfigure_PulserCali/SSD_%s_PulserCali_%s.pdf]",PATHFIGURESFOLDER,L2Tag.c_str(),pulserfiletag));

  std::string pathPulserCaliParsOutput_L2F(Form("%sdata_PulserCali/SSD_%s_PulserCali_%s.dat",PATHDATAFOLDER,L2FTag.c_str(),pulserfiletag));
  std::string pathPulserCaliParsOutput_L2B(Form("%sdata_PulserCali/SSD_%s_PulserCali_%s.dat",PATHDATAFOLDER,L2BTag.c_str(),pulserfiletag));

  std::string pathRootInput[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    pathRootInput[i] = Form("%sQualityCheck/QC_MapSSD%d_%s_PulserCali_%s0000.root",PATHROOTFILESFOLDER,i+1,L2Tag.c_str(),pulserfiletag);
  }
  FILE* FileOutF = fopen(pathPulserCaliParsOutput_L2F.c_str(),"w");
  FILE* FileOutB = fopen(pathPulserCaliParsOutput_L2B.c_str(),"w");
  fprintf(FileOutF,"* Fiiting funtion = par[0] + par[1]*x && y=a*x+b (y=Energy, x=Ch), so a = par[1], b = par[0]\n");
  fprintf(FileOutF,"* SSDNum CHNum    par1(a)   err_par0     par0(b)      err_par1     "
	                 "  peak1     peak2      peak3      peak4     peak5  "
		               "  peak6     peak7     peak8     peak9     peak10   peak11 \n");
  fprintf(FileOutB,"* Fiiting funtion = par[0] + par[1]*x && y=a*x+b, so a = par[1], b = par[0]\n");
  fprintf(FileOutB,"* SSDNum CHNum    par1(a)   err_par0     par0(b)      err_par1    "
	                 "  peak1     peak2     peak3     peak4     peak5  "
	                 "  peak6     peak7     peak8     peak9     peak10    peak11 \n");

  Char_t cans_name[100];
  TCanvas* cans[NUM_SSD][NUM_STRIP];
  for(Int_t SSDNum=0; SSDNum<NUM_SSD; SSDNum++) {
    for(int CHNum=0; CHNum<NUM_STRIP; CHNum++) {
      strcpy(cans_name, Form("C_SSD%d_CH%d",SSDNum+1,CHNum));
      cans[SSDNum][CHNum] = new TCanvas(cans_name,cans_name,10,10,1000,900);
    }
  }
  //____________________________________________________________________________
  TH1D* L2F_PulserPeaks[NUM_SSD][NUM_STRIP];
  TH1D* L2B_PulserPeaks[NUM_SSD][NUM_STRIP];
  for(Int_t SSDNum=0; SSDNum<NUM_SSD; SSDNum++)
  {
    TFile* FileIn = new TFile(pathRootInput[SSDNum].c_str());
    if (!FileIn->IsOpen()) {
      cout<<"Open file "<< pathRootInput[SSDNum].c_str() << " failed"<<endl;
      return;
    }
    //_________________________________________
    //    读取root文件中的 Histograms
    for(Int_t CHNum=0; CHNum<NUM_STRIP; CHNum++) {
      L2F_PulserPeaks[SSDNum][CHNum] = (TH1D*)FileIn->Get(Form("SSD%d_%s_E_CH%02d",SSDNum+1,L2FTag.c_str(),CHNum));
      L2B_PulserPeaks[SSDNum][CHNum] = (TH1D*)FileIn->Get(Form("SSD%d_%s_E_CH%02d",SSDNum+1,L2BTag.c_str(),CHNum));
      L2F_PulserPeaks[SSDNum][CHNum]->GetXaxis()->SetRangeUser(0,4095);
      L2B_PulserPeaks[SSDNum][CHNum]->GetXaxis()->SetRangeUser(0,4095);
      cout << Form("SSD%d_%s_E_CH%d",SSDNum+1,L2FTag.c_str(),CHNum) << endl;
      cout << Form("SSD%d_%s_E_CH%d",SSDNum+1,L2BTag.c_str(),CHNum) << endl;
    }
    printf("Histograms loaded\n");

    //__________________________________________________________________________
    //                        BEGIN ANALYZE HERE !!!
    TSpectrum* s_F = new TSpectrum();
    TSpectrum* s_B = new TSpectrum();
    for(Int_t CHNum=0; CHNum<NUM_STRIP; CHNum++)
    {
      if (L2F_PulserPeaks[SSDNum][CHNum]==0) {
        printf("No data present for SSD%d_%s_E_CH%02d\n",SSDNum+1,L2FTag.c_str(),CHNum);
        continue;
      }
      if (L2B_PulserPeaks[SSDNum][CHNum]==0) {
        printf("No data present for SSD%d_%s_E_CH%02d\n",SSDNum+1,L2BTag.c_str(),CHNum);
        continue;
      }
      //________________________________________________________________________
      // find peaks candidates
      // Search(hist, sigma, option, threshold) \\ 0 < threshold < 1
      cans[SSDNum][CHNum]->Divide(2,2);
      cans[SSDNum][CHNum]->cd(1);
      Int_t nfoundF  = s_F->Search(L2F_PulserPeaks[SSDNum][CHNum],3,"",0.2);
      cans[SSDNum][CHNum]->cd(2);
      Int_t nfoundB  = s_B->Search(L2B_PulserPeaks[SSDNum][CHNum],3,"",0.2);
      printf("SSD%d_%s_E_CH%d is analyzed,%2d peaks found\n",SSDNum+1,L2FTag.c_str(),CHNum,nfoundF);
      printf("SSD%d_%s_E_CH%d is analyzed,%2d peaks found\n",SSDNum+1,L2BTag.c_str(),CHNum,nfoundB);

      Int_t npeaksF = 0;
      Int_t npeaksB = 0;
      Double_t* xpeaksF = s_F->GetPositionX();
      Double_t* xpeaksB = s_B->GetPositionX();
      std::sort(xpeaksF,xpeaksF+nfoundF,compare);   // sort默认升序,compare使之变成降序
      std::sort(xpeaksB,xpeaksB+nfoundB,compare);
      for(Int_t p=0; p<nfoundF; p++) {
        if (xpeaksF[p]<120) continue;
        Double_t xp_F = xpeaksF[p];
        printf(" SSD%d_%s_E_CH%d peak %d is %.1f\n",SSDNum+1,L2FTag.c_str(),CHNum,p,xp_F);
        npeaksF++;
      }
      cout<<"SSD"<<SSDNum<<"_L2F 有效的peak数目为:"<< npeaksF << endl;
      for(Int_t p=0; p<nfoundB; p++) {
        Double_t xp_B = xpeaksB[p];
        printf(" SSD%d_%s_E_CH%d peak %d is %.1f\n",SSDNum+1,L2BTag.c_str(),CHNum,p,xp_B);
        npeaksB++;
      }
      cout<<"SSD"<<SSDNum <<"_L2B 有效的peak数目为:"<< npeaksB << endl;

      for(Int_t i=0; i<npeaksF; i++) {
        if (npeaksF==5)  AttenFactorF[i] = Switch5[i];
        if (npeaksF==6)  AttenFactorF[i] = Switch6[i];
        if (npeaksF==8)  AttenFactorF[i] = Switch8[i];
        if (npeaksF==9)  AttenFactorF[i] = Switch9[i];
        if (npeaksF==10) AttenFactorF[i] = Height10[i];
        if (npeaksF==11) AttenFactorF[i] = Height11[i];
      }
      for(Int_t i=0; i<npeaksB; i++) {
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
      grapF->SetTitle(Form("PulserFit_SSD%d_%s_E_CH%02d",SSDNum+1,L2FTag.c_str(),CHNum));
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
      TGraph* grapB = new TGraph(npeaksB,xpeaksB,AttenFactorB); //Energy vs Ch (y = Enegry, x = channel)
      grapB->SetMarkerStyle(20);
      grapB->SetMarkerSize(1.5);
      grapB->SetMarkerColor(kBlue);
      grapB->SetTitle(Form("PulserFit_SSD%d_%s_E_CH%02d",SSDNum+1,L2BTag.c_str(),CHNum));
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
    }
  }
  fclose(FileOutF);
  fclose(FileOutB);
  //____________________________________________________________________________
  TCanvas* c_begin = new TCanvas("c_begin","c_begin",800,600);
  TCanvas* c_end   = new TCanvas("c_end","c_end",800,600);
  c_end->Close();
  c_begin->Close();
  c_begin->Print(pathPDFbegin.c_str());
  for(Int_t SSDNum=0; SSDNum<NUM_SSD; SSDNum++) {
    for(Int_t CHNum=0; CHNum<NUM_STRIP; CHNum++) {
        cans[SSDNum][CHNum]->Print(pathPDFOutput.c_str());
    }
  }
  c_end->Print(pathPDFend.c_str());
  return;
}
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//    1.对 SSD1 与 SSD2 的第一层进行 重新刻度, 原因是放射源刻度时，这两层硅条
//      的看不到信号，因而增大了放大倍数
//    2.需要注意以下的不同：
//      (1) SSD1_L1: gain=4   => gain=20
//      (2) SSD2_L1: gain=7.4 => gain=20
//    3.Pulser 与 放射源刻度完以后, 需要将刻度曲线转换回到原来的放大倍数的情况
//
//    gfh, 2020-05-27
//    修改成类, 2020-10-02
////////////////////////////////////////////////////////////////////////////////
void CSHINEPulserCali::L1_AutoFindPeaksAndFit_ReCali(const char* pulserfiletag,const char* GainTag)
{
  gStyle->SetOptStat(0);
  //____________________________________________________________________________
  std::string L1Tag("L1");
  std::string L1STag("L1S");
  std::string Gain20("Gain20");
  std::string Gain4 ("Gain4");
  std::string Gain74("Gain74");
  std::string LowGain("LowGain");
  std::string HighGain("HighGain");
  std::string Height("Height");

  Int_t ReCaliSSDNum = 2;

  Double_t AttenFactor[11] = {0};     //衰减因子
  Double_t Switch5[5]   = {1./4, 1./5, 1./10, 1./20, 1./40};
  Double_t Switch8[8]   = {1., 1./2, 1./4, 1./5, 1./10, 1./20, 1./40, 1./50};
  Double_t Switch9[9]   = {1., 1/2., 1/4., 1/5., 1/10., 1/20., 1/40., 1/50., 1/100.};
  Double_t Height10[10] = {1./4, .9/4, .8/4, .7/4, .6/4, .5/4, .4/4, .3/4, .2/4, .1/4};

  Double_t Switch6[6]  = {1., 1./2, 1./4, 1./5, 1./10, 1./20};      // for SSD2_L1_Gain7.4_Switch
  Double_t Switch6_SSD1_Gain20[6]  = {1./4, 1./5, 1./10, 1./20, 1./40, 1./50};  // for SSD1_L1_Gain20_Switch

  Double_t Height11[11] = {1.,.9, .8, .7, .6, .5, .4, .3, .2, .1, 0.05};
  Double_t Height11_SSD2_Gain20[11] = {1./4, .9/4, .8/4, .7/4, .6/4, .5/4, .4/4, .3/4, .2/4, .1/4, 0.05/4};  // for SSD2_L1_Gain20_Hight

  Double_t RangeXLowCut[ReCaliSSDNum];
  if (strcmp(GainTag,LowGain.c_str())==0) {
    RangeXLowCut[0] = 120.;
    RangeXLowCut[1] = 120.;
  } else if ((strcmp(GainTag,HighGain.c_str())==0) && (strcmp(pulserfiletag,Height.c_str())==0)) {
    RangeXLowCut[0] = 350.;
    RangeXLowCut[1] = 120.;
  } else {
    RangeXLowCut[0] = 250.;
    RangeXLowCut[1] = 120.;
  }

  std::string pathPDFOutput(Form("%sfigure_PulserCali/SSD_%s_PulserReCali_%s_%s.pdf",PATHFIGURESFOLDER,L1STag.c_str(),GainTag,pulserfiletag));
  std::string pathPDFbegin(Form("%sfigure_PulserCali/SSD_%s_PulserReCali_%s_%s.pdf[",PATHFIGURESFOLDER,L1STag.c_str(),GainTag,pulserfiletag));
  std::string pathPDFend(Form("%sfigure_PulserCali/SSD_%s_PulserReCali_%s_%s.pdf]",  PATHFIGURESFOLDER,L1STag.c_str(),GainTag,pulserfiletag));

  std::string pathPulserCaliParsOutput(Form("%sdata_PulserCali/SSD_%s_PulserReCali_%s_%s.dat",PATHDATAFOLDER,L1STag.c_str(),GainTag,pulserfiletag));

  std::string pathRootInput[ReCaliSSDNum];
  if (strcmp(GainTag,HighGain.c_str())==0) {
    pathRootInput[0] = Form("%sQualityCheck/QC_MapSSD%d_%s_PulserReCali_%s_%s0000.root",PATHROOTFILESFOLDER,1,L1Tag.c_str(),Gain20.c_str(),pulserfiletag);
    pathRootInput[1] = Form("%sQualityCheck/QC_MapSSD%d_%s_PulserReCali_%s_%s0000.root",PATHROOTFILESFOLDER,2,L1Tag.c_str(),Gain20.c_str(),pulserfiletag);
  }
  if (strcmp(GainTag,LowGain.c_str())==0) {
    pathRootInput[0] = Form("%sQualityCheck/QC_MapSSD%d_%s_PulserReCali_%s_%s0000.root",PATHROOTFILESFOLDER,1,L1Tag.c_str(),Gain4.c_str(),pulserfiletag);
    pathRootInput[1] = Form("%sQualityCheck/QC_MapSSD%d_%s_PulserReCali_%s_%s0000.root",PATHROOTFILESFOLDER,2,L1Tag.c_str(),Gain74.c_str(),pulserfiletag);
  }

  FILE * FileOut = fopen(pathPulserCaliParsOutput.c_str(),"w");
  fprintf(FileOut,"* Fiiting funtion = par[0] + par[1]*x && y=a*x+b (y=Energy, x=Ch), so a = par[1], b = par[0];  0.0 isn't a peak\n");
  fprintf(FileOut,"* SSDNum CHNum    par0(a)   err_par0     par1(b)      err_par1      "
	    	  " peak1     peak2     peak3      peak4     peak5  "
		      " peak6     peak7     peak8      peak9     peak10    peak11 \n");

  Char_t cans_name[100];
  TCanvas* cans[ReCaliSSDNum][NUM_STRIP];
  for(Int_t SSDNum=0; SSDNum<ReCaliSSDNum; SSDNum++) {
    for(int CHNum=0; CHNum<NUM_STRIP; CHNum++) {
      strcpy(cans_name, Form("C_SSD%d_CH%d",SSDNum+1,CHNum));
      cans[SSDNum][CHNum] = new TCanvas(cans_name,cans_name,10,10,1000,900);
    }
  }
  //____________________________________________________________________________
  TH1D* PulserPeaks[NUM_SSD][NUM_STRIP];
  for(Int_t SSDNum=0; SSDNum<ReCaliSSDNum; SSDNum++)
  {
    TFile* FileIn = new TFile(pathRootInput[SSDNum].c_str());
    if (!FileIn->IsOpen()) {
      cout<<"Open file "<< pathRootInput[SSDNum].c_str() << " failed"<<endl;
      return;
    }
    //_______________________________________
    // 读取root文件中的 Histograms
    for(Int_t CHNum=0; CHNum<NUM_STRIP; CHNum++) {
      PulserPeaks[SSDNum][CHNum] = (TH1D*)FileIn->Get(Form("SSD%d_%s_E_CH%02d",SSDNum+1,L1STag.c_str(),CHNum));
      PulserPeaks[SSDNum][CHNum]->GetXaxis()->SetRangeUser(RangeXLowCut[SSDNum],4095);
    }
    printf("Histograms loaded\n");
    //__________________________________________________________________________
    //                        BEGIN ANALYZE HERE !!!
    TSpectrum* s = new TSpectrum();
    for(Int_t CHNum=0; CHNum<NUM_STRIP; CHNum++)
    {
      if (PulserPeaks[SSDNum][CHNum]==0) {
        printf("No data present for SSD%d_%s_E_CH%02d\n",SSDNum+1,L1STag.c_str(),CHNum);
        continue;
      }
      //____________________________
      // find peaks candidates
      // Search(hist, sigma, option, threshold) \\ 0 < threshold < 1
      cans[SSDNum][CHNum]->Divide(1,2);
      cans[SSDNum][CHNum]->cd(1);
      Int_t nfound  = s->Search(PulserPeaks[SSDNum][CHNum],3,"",0.1);
      printf("SSD%d_%s_E_CH%d is analyzed,%2d peaks found\n",SSDNum+1,L1STag.c_str(),CHNum,nfound);
      Int_t npeaks = 0;
      Double_t* xpeaks = s->GetPositionX();
      // 对寻找到的峰值降序排序
      std::sort(xpeaks,xpeaks+nfound,compare);   // sort默认升序,compare使之变成降序
      for(Int_t p=0; p<nfound; p++) {
        if(xpeaks[p]<120) continue;
        Double_t xp = xpeaks[p];
        printf(" SSD%d_%s_E_CH%d peak %d is %.1f\n",SSDNum+1,L1STag.c_str(),CHNum,p,xp);
        npeaks++;
      }
      cout<<"有效的peak数目为:"<< npeaks << endl;
      cout<<"Now fitting: Be patient\n" <<endl;

      for(Int_t i=0; i<npeaks; i++) {
        if (npeaks==5)  AttenFactor[i] = Switch5[i];
        if (npeaks==6)  {
          if ((SSDNum==0)&&(strcmp(GainTag,HighGain.c_str())==0)) {
            AttenFactor[i] = Switch6_SSD1_Gain20[i];
          } else {
            AttenFactor[i] = Switch6[i];
          }
        }
        if (npeaks==8)  AttenFactor[i] = Switch8[i];
        if (npeaks==9)  AttenFactor[i] = Switch9[i];
        if (npeaks==10) AttenFactor[i] = Height10[i];
        if (npeaks==11) {
          if ((SSDNum==1)&&(strcmp(GainTag,HighGain.c_str())==0)) {
            AttenFactor[i] = Height11_SSD2_Gain20[i];
          } else {
            AttenFactor[i] = Height11[i];
          }
        }
      }
      cans[SSDNum][CHNum]->cd(2);
      gPad->SetGridx();
      gPad->SetGridy();
      TGraph* grap = new TGraph(npeaks,xpeaks,AttenFactor);  // Energy vs Ch (y = Enegry, x = channel)
      grap->SetMarkerStyle(20);
      grap->SetMarkerSize(1.5);
      grap->SetMarkerColor(kBlue);
      grap->SetTitle(Form("PulserFit_SSD%d_%s_E_CH%02d",SSDNum+1,L1STag.c_str(),CHNum));
      grap->GetXaxis()->SetTitle("ADC Channel");
      grap->GetXaxis()->CenterTitle(1);
      grap->GetXaxis()->SetTitleSize(0.04);
      grap->GetYaxis()->SetTitle("Relative_Pulser_Height");
      grap->GetYaxis()->CenterTitle(1);
      grap->GetYaxis()->SetRangeUser(0.,1.1);
      grap->GetYaxis()->SetNdivisions(511);
      grap->Draw("AP*");

      TF1* fit = new TF1("fit","pol1",100,4096);
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
    }
  }
  fclose(FileOut);
  //____________________________________________________________________________
  TCanvas* c_begin = new TCanvas("c_begin","c_begin",800,600);
  TCanvas* c_end   = new TCanvas("c_end","c_end",800,600);
  c_end->Close();
  c_begin->Close();
  c_begin->Print(pathPDFbegin.c_str());
  for(Int_t SSDNum=0; SSDNum<ReCaliSSDNum; SSDNum++) {
    for(Int_t CHNum=0; CHNum<NUM_STRIP; CHNum++) {
        cans[SSDNum][CHNum]->Print(pathPDFOutput.c_str());
    }
  }
  c_end->Print(pathPDFend.c_str());
  return;
}
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//   由于前放设置的缘故, 在放射源刻度时, SSD1_L1 与 SSD2_L1 看不到信号，
//   于是，改变了放大倍数：
//        SSD1_L1: gain = 2*4    => 2*20
//        SSD2_L1: gain = 2*7.4  => 2*20
//   因此，在脉冲刻度时, 也要相应改变放大倍数, 现在来考察不同 gain 情况下,
//   刻度曲线之间的关系
//
//  gfh, 2020-05-30
// 修改成类, 2020-10-02
////////////////////////////////////////////////////////////////////////////////
void CSHINEPulserCali::L1_GainEffectOnPulseCali_ReCali(const char* pulserfiletag)
{
  gStyle->SetPalette(1);
  //____________________________________________________________________________
  Double_t MinCh     = 0.;
  Double_t MaxCh     = 4096.;
  Double_t RangeXLow = 3000.;
  Double_t RangeXUp  = 4000.;
  Double_t RangeYUp  = 1.2;

  Int_t ReCaliSSDNum      = 2;
  Int_t numpar_LowGainIn  = 3;
  Int_t numpar_HighGainIn = 3;
  Int_t FitFuncNum = 5;
  Int_t color[5]   = {kBlue,kMagenta,kBlack,kOrange,kGreen};

  Double_t HighGain  [ReCaliSSDNum];
  Double_t LowGain   [ReCaliSSDNum];
  Double_t GainStep  [ReCaliSSDNum];
  Double_t GainFactor[ReCaliSSDNum];
  Int_t IntLowGain   [ReCaliSSDNum];
  Int_t IntHighGain  [ReCaliSSDNum];
  LowGain [0] = 4.0;    LowGain [1] = 7.3;
  HighGain[0] = 20.;    HighGain[1] = 20.;
  GainStep[0] = 0.006;  GainStep[1] = 0.043;
  // 换底公式  loga(n)/loga(m) = logm(n)
  for (Int_t i=0; i<ReCaliSSDNum; i++) {
    IntLowGain[i]  = Int_t (log(LowGain [i])/log(1.22) + 0.5); //取整
    IntHighGain[i] = Int_t (log(HighGain[i])/log(1.22) + 0.5); //取整
    GainFactor[i]  = pow(1.22,(IntHighGain[i]-IntLowGain[i]));
    cout<<"im = "<<IntHighGain[i]-IntLowGain[i]<<setw(20)<<"GainFactor = "<<GainFactor[i]<< endl;
  }

  std::string L1STag("L1S");
  std::string CaliTag("PulserReCali");
  std::string FileOutTag("GainEffectOntheCali");
  std::string HighGainTag("HighGain");
  std::string LowGainTag("LowGain");

  std::string pathPDFOutput(Form("%sfigure_PulserCali/SSD_%s_%s_%s_%s.pdf",PATHFIGURESFOLDER,L1STag.c_str(),
                            CaliTag.c_str(),pulserfiletag,FileOutTag.c_str()));
  std::string pathPDFOutputBegin(Form("%sfigure_PulserCali/SSD_%s_%s_%s_%s.pdf[",PATHFIGURESFOLDER,L1STag.c_str(),
                            CaliTag.c_str(),pulserfiletag,FileOutTag.c_str()));
  std::string pathPDFOutputEnd(Form("%sfigure_PulserCali/SSD_%s_%s_%s_%s.pdf]",PATHFIGURESFOLDER,L1STag.c_str(),
                            CaliTag.c_str(),pulserfiletag,FileOutTag.c_str()));
  std::string pathLowGainFileIn(Form("%sdata_PulserCali/SSD_%s_%s_%s_%s.dat",PATHDATAFOLDER,L1STag.c_str(),CaliTag.c_str(),
                            LowGainTag.c_str(),pulserfiletag));
  std::string pathHighGainFileIn(Form("%sdata_PulserCali/SSD_%s_%s_%s_%s.dat",PATHDATAFOLDER,L1STag.c_str(),CaliTag.c_str(),
                            HighGainTag.c_str(),pulserfiletag));
  //____________________________________________________________________________

  ReadFileModule readfile;
  Double_t*** LowGainIn  = readfile.ReadData(pathLowGainFileIn.c_str(), NUM_SSD,NUM_STRIP,numpar_LowGainIn);
  Double_t*** HighGainIn = readfile.ReadData(pathHighGainFileIn.c_str(),NUM_SSD,NUM_STRIP,numpar_HighGainIn);

  Double_t a_LowGain   [NUM_SSD][NUM_STRIP];
  Double_t b_LowGain   [NUM_SSD][NUM_STRIP];
  Double_t a_HighGain  [NUM_SSD][NUM_STRIP];
  Double_t b_HighGain  [NUM_SSD][NUM_STRIP];
  Double_t a_GainFactor[NUM_SSD][NUM_STRIP][FitFuncNum];
  Double_t b_GainFactor[NUM_SSD][NUM_STRIP][FitFuncNum];
  Double_t AmpFactor   [NUM_SSD][NUM_STRIP][FitFuncNum];

  for (Int_t i=0; i<ReCaliSSDNum; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
      a_LowGain[i][j]  = LowGainIn [i][j][0];
      b_LowGain[i][j]  = LowGainIn [i][j][2];
      a_HighGain[i][j] = HighGainIn[i][j][0];
      b_HighGain[i][j] = HighGainIn[i][j][2];
      if (a_LowGain[i][j]<1e-10) continue;

      cout<<setw(5)<<std::right<<i<<setw(10)<<j<<setw(15)<<std::right
          <<"a_LowGain = " <<setw(10)<<std::right<<a_LowGain[i][j] <<setw(15)<<std::right
          <<"b_LowGain = " <<setw(10)<<std::right<<b_LowGain[i][j] <<setw(15)<<std::right
          <<"a_HighGain = "<<setw(10)<<std::right<<a_HighGain[i][j]<<setw(15)<<std::right
          <<"b_HighGain = "<<setw(10)<<std::right<<b_HighGain[i][j]<<endl;

      for (Int_t k=0; k<FitFuncNum; k++) {
        AmpFactor[i][j][k]    = GainFactor[i]*(1+(k-2)*GainStep[i]);
        a_GainFactor[i][j][k] = a_LowGain[i][j]/AmpFactor[i][j][k];
        b_GainFactor[i][j][k] = b_LowGain[i][j]/AmpFactor[i][j][k];
      }
    }
  }
  TCanvas* cans_begin = new TCanvas();
  TCanvas* cans_end   = new TCanvas();
  TCanvas* cans = new TCanvas("cans1","cans1_draw",800,800);
  cans->Divide(1,2);
  cans_begin->Close();
  cans_end->Close();
  cans_begin->Print(pathPDFOutputBegin.c_str());
  for (Int_t i=0; i<ReCaliSSDNum; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
      if (a_LowGain[i][j]<1e-10) continue;
      TF1* FuncLowGain  = new TF1("fLowGain", Form("%.11f*x+%.11f",a_LowGain[i][j], b_LowGain[i][j]), MinCh,MaxCh);
      TF1* FuncHighGain = new TF1("fHighGain",Form("%.11f*x+%.11f",a_HighGain[i][j],b_HighGain[i][j]),MinCh,MaxCh);

      cans->cd(1);
      gPad->SetGridx();
      gPad->SetGridy();
      FuncLowGain->SetTitle(Form("SSD%d_%s_CH%02d_%s_%s",i+1,L1STag.c_str(),j,pulserfiletag,FileOutTag.c_str()));
      FuncLowGain->SetLineColor(kRed);
      FuncLowGain->GetXaxis()->SetRangeUser(0.,MaxCh);
      FuncLowGain->GetXaxis()->SetTitle("ADC Channel");
      FuncLowGain->GetXaxis()->CenterTitle(1);
      FuncLowGain->GetXaxis()->SetTitleSize(0.05);
      FuncLowGain->GetXaxis()->SetNdivisions(1005,"true");
      FuncLowGain->GetYaxis()->SetRangeUser(0.,RangeYUp);
      FuncLowGain->GetYaxis()->SetNdivisions(1003,"true");
      FuncLowGain->GetYaxis()->SetTitle("Relative_Pulser_Height");
      FuncLowGain->GetYaxis()->SetTitleSize(0.05);
      FuncLowGain->GetYaxis()->CenterTitle(1);
      FuncHighGain->SetLineColor(kCyan);
      FuncLowGain->Draw();
      FuncHighGain->Draw("SAME");

      TLegend* legend1 = new TLegend(0.12,0.48,0.4,0.88);
      legend1->AddEntry(FuncLowGain,LowGainTag.c_str(),"l");
      legend1->AddEntry(FuncHighGain,HighGainTag.c_str(),"l");
      TF1* FuncGainFactor[FitFuncNum];
      for (Int_t k=0; k<FitFuncNum; k++) {
        FuncGainFactor[k] = new TF1("fGainFactor", Form("%.11f*x+%.11f",a_GainFactor[i][j][k],
                                    b_GainFactor[i][j][k]),MinCh,MaxCh);
        FuncGainFactor[k]->SetLineColor(color[k]);
        Double_t ratio = 100*(k-2)*GainStep[i];
        legend1->AddEntry(FuncGainFactor[k],Form("%s * %.04f[%.2f%%]",HighGainTag.c_str(),AmpFactor[i][j][k],ratio),"l");
        FuncGainFactor[k]->Draw("SAME");
      }
      legend1->Draw("SAME");

      cans->cd(2);
      gPad->SetGridx();
      gPad->SetGridy();
      TF1* FuncLowGain_clone  = (TF1*)FuncLowGain->Clone();
      TF1* FuncHighGain_clone = (TF1*)FuncHighGain->Clone();
      TF1* FuncGainFactor_clone[FitFuncNum];
      FuncLowGain_clone->GetXaxis()->SetRangeUser(RangeXLow, RangeXUp);
      FuncLowGain_clone->GetYaxis()->SetRangeUser(0.99*FuncHighGain->Eval(RangeXLow),1.01*FuncHighGain->Eval(RangeXUp));
      FuncLowGain_clone->Draw();
      FuncHighGain_clone->Draw("SAME");
      for (Int_t k=0; k<FitFuncNum; k++) {
        FuncGainFactor_clone[k] = (TF1*)FuncGainFactor[k]->Clone();
        FuncGainFactor_clone[k]->Draw("SAME");
      }
      cans->Print(pathPDFOutput.c_str());
      gPad->Modified();
      gPad->Update();
    }
  }
  cans_end->Print(pathPDFOutputEnd.c_str());
  return;
}
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//
//   Pulser 刻度时， SSD2_L2F_CH00,CH01 两道峰值大约是其他道的一半,
//   怀疑可能的原因：由于2019年这次实验采取两路并一路的方式，可能是这两路
//   有断丝、或因为其他原因导致只有一路有信号.
//   为此，可以检查 alpha 刻度时，这两路的计数与其他道的计数是否存在差异
//  gfh, 2020-06-16
// 修改成类, 2020-10-02
////////////////////////////////////////////////////////////////////////////////
void CSHINEPulserCali::CaliCheck_SSD_L2F_CH00_01()
{
  gStyle->SetOptStat(0);
  //____________________________________________________________________________
  Double_t L2_XRangeLow[4] = {100.,110.,150.,300.};
  Double_t L2_XRangeUp [4] = {180.,200.,300.,480.};

  Int_t YRangeCut = 4000;

  std::string L2Tag("L2");
  std::string L2FTag("L2F");
  std::string AlphaCaliTag("AlphaCali00_48");
  std::string pathFileIn(Form("%sQualityCheck/QC_MapSSD_%s_%s.root",PATHROOTFILESFOLDER,
                         L2Tag.c_str(),AlphaCaliTag.c_str()));
  std::string pathPNGOutput[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    std::string PulserCheckTag(Form("CheckPulserCali_SSD%d_L2F_CH00_CH01",i+1));
    pathPNGOutput[i] = Form("%sfigure_PulserCali/%s_%s.png",PATHFIGURESFOLDER,PulserCheckTag.c_str(),AlphaCaliTag.c_str());
  }

  Double_t XRangeLow[NUM_SSD];
  Double_t XRangeUp [NUM_STRIP];
  for (Int_t i=0; i<NUM_SSD; i++) {
    XRangeLow[i] = L2_XRangeLow[i];
    XRangeUp[i]  = L2_XRangeUp[i];
  }
  //____________________________________________________________________________
  TFile* FileIn = TFile::Open(pathFileIn.c_str());
  if (!FileIn->IsOpen()) {
    cout<< "Open file "<< pathFileIn.c_str() << " failed." <<endl;
    return;
  } else {
    cout<< "File "<< pathFileIn.c_str() << " loaded."<<endl;
  }

  TH1D* hist[NUM_SSD][NUM_STRIP];
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
      hist[i][j] = (TH1D*)FileIn->Get(Form("SSD%d_%s_E_CH%02d",i+1,L2FTag.c_str(),j));
      if (hist[i][j]==0) {
        printf("No data present for SSD%d_%s_E_CH%02d\n",(i+1),L2FTag.c_str(),j);
        continue;
      }
      hist[i][j]->GetXaxis()->SetRangeUser(XRangeLow[i],XRangeUp[i]);
      hist[i][j]->GetYaxis()->SetRangeUser(0,YRangeCut);
    }
  }
  TCanvas* cans[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    cans[i] = new TCanvas("cans","cans",1200,800);
    cans[i]->Divide(3,2);
    cans[i]->cd(1);
    hist[i][0]->Draw();
    cans[i]->cd(2);
    hist[i][1]->Draw();
    cans[i]->cd(3);
    hist[i][2]->Draw();
    cans[i]->cd(4);
    hist[i][15]->Draw();
    cans[i]->cd(5);
    hist[i][14]->Draw();
    cans[i]->cd(6);
    hist[i][13]->Draw();
    cans[i]->Print(pathPNGOutput[i].c_str());
  }
  return;
}
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//   脉冲刻度时, 我们使用了 "拨档法 (Switch)" 与 "旋钮法 (Height)"
//   从拟合数据看，两者的差别很小，< 1%
//   现在来定量考察一下两种方法的拟合参数的差异, 来决定后续的数据处理采取哪种刻度方式
//
//   by gfh, 2020-05-29
//   修改成类, 2020-10-02
////////////////////////////////////////////////////////////////////////////////
void CSHINEPulserCali::EstimateErrorOfHightAndSwitch()
{
  //______________________________________________
  Int_t numpar_HeightIn = 3;
  Int_t numpar_SwitchIn = 3;

  std::string LayerTag("L2F");
  std::string HeightTag("Height");
  std::string SwitchTag("Switch");
  std::string CaliTag("PulserCali");
  std::string FileOutTag("ErrorOfHightAndSwitch");

  std::string pathHeightFileIn(Form("%sdata_PulserCali/SSD_%s_%s_%s.dat",PATHDATAFOLDER,
                               LayerTag.c_str(),CaliTag.c_str(),HeightTag.c_str()));
  std::string pathSwitchFileIn(Form("%sdata_PulserCali/SSD_%s_%s_%s.dat",PATHDATAFOLDER,
                               LayerTag.c_str(),CaliTag.c_str(),SwitchTag.c_str()));
  std::string pathPNGOutput(Form("%sfigure_PulserCali/SSD_%s_%s_%s.png",PATHFIGURESFOLDER,
                               LayerTag.c_str(),CaliTag.c_str(),FileOutTag.c_str()));
  //____________________________________________________________________________
  ReadFileModule readfile;
  Double_t*** HeightIn = readfile.ReadData(pathHeightFileIn.c_str(),NUM_SSD,NUM_STRIP,numpar_HeightIn);
  Double_t*** SwitchIn = readfile.ReadData(pathSwitchFileIn.c_str(),NUM_SSD,NUM_STRIP,numpar_SwitchIn);

  Double_t SSDCHNum[NUM_SSD][NUM_STRIP];
  Double_t a_ErrofHightOverSwitch[NUM_SSD][NUM_STRIP];
  Double_t b_ErrofHightOverSwitch[NUM_SSD][NUM_STRIP];
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
      SSDCHNum[i][j]=j+1;
      if (HeightIn[i][j][0]<1e-6) continue;
      a_ErrofHightOverSwitch[i][j]=100*abs((HeightIn[i][j][0]-SwitchIn[i][j][0])/SwitchIn[i][j][0]); // 结果是百分比
      b_ErrofHightOverSwitch[i][j]=100*abs((HeightIn[i][j][2]-SwitchIn[i][j][2])/SwitchIn[i][j][2]); // 结果是百分比
      cout.setf(ios::fixed);
      cout<<setw(5)<<std::right<<i<<setw(5)<<std::right<<j<<setw(15)<<std::right
          <<"a_err = "<<setw(10)<<std::right<<setprecision(11)<<a_ErrofHightOverSwitch[i][j]<<setw(15)<<std::right
          <<"b_err = "<<setw(10)<<std::right<<setprecision(11)<<b_ErrofHightOverSwitch[i][j]<<endl;
    }
  }
  TCanvas* cans = new TCanvas("cansa","Error of Hight and Switch",1200,1000);
  cans->Divide(2,2);
  for (Int_t i=0; i<NUM_SSD; i++) {
    cans->cd(i+1);
    gPad->SetGridx();
    gPad->SetLeftMargin(0.13);
    TGraph* a_Error_grap = new TGraph(NUM_STRIP,SSDCHNum[i],a_ErrofHightOverSwitch[i]);
    TGraph* b_Error_grap_scale = new TGraph(NUM_STRIP,SSDCHNum[i],b_ErrofHightOverSwitch[i]);

    auto hista = a_Error_grap->GetHistogram();
    a_Error_grap->SetTitle(Form("SSD%d_%s_%s_%s",i+1,LayerTag.c_str(),CaliTag.c_str(),FileOutTag.c_str()));
    a_Error_grap->GetXaxis()->SetNdivisions(117);
    a_Error_grap->GetYaxis()->SetRangeUser(0.5*hista->GetMinimum(),1.2*hista->GetMaximum());
    hista->Draw();
    gPad->Update();

    auto histb = b_Error_grap_scale->GetHistogram();
    Double_t rightmax = histb->GetMaximum();
    Double_t scale = gPad->GetUymax()/rightmax;

    for (Int_t j=0; j<NUM_STRIP; j++) {
      b_ErrofHightOverSwitch[i][j] = b_ErrofHightOverSwitch[i][j]*scale;
    }
    TGraph* b_Error_grap = new TGraph(NUM_STRIP,SSDCHNum[i],b_ErrofHightOverSwitch[i]);

    a_Error_grap->SetMarkerStyle(20);
    a_Error_grap->SetMarkerColor(kBlue);
    a_Error_grap->GetXaxis()->SetTitle("Channel");
    a_Error_grap->GetXaxis()->CenterTitle(1);
    a_Error_grap->GetXaxis()->SetTitleSize(0.04);
    a_Error_grap->GetYaxis()->SetTitle("Relative Errors (%)");
    a_Error_grap->GetYaxis()->CenterTitle(1);
    a_Error_grap->GetYaxis()->SetTitleSize(0.04);

    b_Error_grap->SetMarkerStyle(20);
    b_Error_grap->SetMarkerColor(kRed);
    b_Error_grap->GetXaxis()->SetTitle("Channel");
    b_Error_grap->GetXaxis()->CenterTitle(1);
    b_Error_grap->GetXaxis()->SetTitleSize(0.04);
    b_Error_grap->GetYaxis()->SetTitle("Relative Errors (%)");
    b_Error_grap->GetYaxis()->CenterTitle(1);
    b_Error_grap->GetYaxis()->SetTitleSize(0.04);

    TMultiGraph* mg = new TMultiGraph();
    mg->Add(a_Error_grap);
    mg->Add(b_Error_grap);
    TGaxis *axis = new TGaxis(gPad->GetUxmax(),gPad->GetUymin(),
                              gPad->GetUxmax(),gPad->GetUymax(),
                              0,rightmax,510,"+L");
    axis->SetLineColor(kRed);
    axis->SetLabelColor(kRed);
    TLegend* legend = new TLegend(0.70,0.80,0.9,0.9);
    legend->AddEntry(a_Error_grap,"a_Err(Slope)","lp");
    legend->AddEntry(b_Error_grap,"b_Err(Intercept)","lp");
    mg->Draw("PL");
    axis->Draw();
    legend->Draw("SAME");
  }
  cans->Print(pathPNGOutput.c_str());

  readfile.DeleteData(HeightIn, NUM_SSD, NUM_STRIP, numpar_HeightIn);
  readfile.DeleteData(SwitchIn, NUM_SSD, NUM_STRIP, numpar_SwitchIn);
  return;
}
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//   较定量考察 pulser 脉冲刻度的线性性！
//   虽然从 pulser 刻度的图片看到很好的线性, 但为保险起见，需要对其进行定量考察.
//   思路如下: 对于每个相同的 ADC 道址, 计算每个相对脉冲幅度与刻度曲线的偏差
//
//   gfh, 2020-06-20
//   修改成类, 2020-10-02
////////////////////////////////////////////////////////////////////////////////
void CSHINEPulserCali::CheckLinearity(const char* layertag, const char* pulserfiletag)
{
  Double_t Height[11] = {1., .9, .8, .7, .6, .5, .4, .3, .2, .1, 0.05};
  Int_t numpeaks11 = 11;  // 最多是 11 个点, 但有些是 10 个点
  Int_t numpeaks10 = 10;
  Int_t numpar_PulserIn  = 15;

  std::string pathPulserIn(Form("%sdata_PulserCali/SSD_%s_PulserCali_%s.dat",PATHDATAFOLDER,layertag,pulserfiletag));
  std::string pathPDFOutput(Form("%sfigure_PulserCali/SSD_%s_CheckPulserLinearity.pdf",PATHFIGURESFOLDER,layertag));
  std::string pathPDFOutputBegin(Form("%sfigure_PulserCali/SSD_%s_CheckPulserLinearity.pdf[",PATHFIGURESFOLDER,layertag));
  std::string pathPDFOutputEnd(Form("%sfigure_PulserCali/SSD_%s_CheckPulserLinearity.pdf]",PATHFIGURESFOLDER,layertag));
  //____________________________________________________________________________
  ReadFileModule readfile;
  Double_t*** PulserIn  = readfile.ReadData(pathPulserIn.c_str(),NUM_SSD,NUM_STRIP,numpar_PulserIn);
  Double_t PulserPeaks11[NUM_SSD][NUM_STRIP][numpeaks11];
  Double_t PulserPeaks10[NUM_SSD][NUM_STRIP][numpeaks10];
  Double_t PulserHeight11[NUM_SSD][NUM_STRIP][numpeaks11];
  Double_t PulserHeight10[NUM_SSD][NUM_STRIP][numpeaks10];
  Double_t CaliPeaks[NUM_SSD][NUM_STRIP][numpeaks11];
  Double_t RelativeErr11[NUM_SSD][NUM_STRIP][numpeaks11];
  Double_t RelativeErr10[NUM_SSD][NUM_STRIP][numpeaks10];
  Double_t Sum_RelativeErr10[NUM_SSD][NUM_STRIP];
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
      Sum_RelativeErr10[i][j] = 0.;
      for (Int_t p=0; p<numpeaks11; p++) {
        PulserPeaks11[i][j][p]  = PulserIn[i][j][4+p]; // peak1...peak11
        PulserHeight11[i][j][p] = Height[p];
        CaliPeaks[i][j][p]      = PulserIn[i][j][0]*PulserPeaks11[i][j][p]+PulserIn[i][j][2];
        RelativeErr11[i][j][p]  = 100*(Height[p]-CaliPeaks[i][j][p])/CaliPeaks[i][j][p];
        if (p<numpeaks10) {  // 去掉最小的点
          PulserHeight10[i][j][p] = Height[p];
          PulserPeaks10[i][j][p]  = PulserIn[i][j][4+p]; // peak1...peak10
          RelativeErr10[i][j][p]  = 100*(Height[p]-CaliPeaks[i][j][p])/CaliPeaks[i][j][p];
          Sum_RelativeErr10[i][j] += RelativeErr10[i][j][p];
        }
      }
    }
  }
  TCanvas* cans = new TCanvas("cans","cans",1200,500);
  TCanvas* cans_begin = new TCanvas();
  TCanvas* cans_end   = new TCanvas();
  cans->Divide(2,1);
  cans_begin->Close();
  cans_end->Close();
  cans_begin->Print(pathPDFOutputBegin.c_str());

  TF1*    fit[NUM_SSD][NUM_STRIP];
  TLatex* latex[NUM_SSD][NUM_STRIP];
  TLatex* latex_average[NUM_SSD][NUM_STRIP];
  TLine * line_averageErr[NUM_SSD][NUM_STRIP];
  TGraph* graphpeaks11[NUM_SSD][NUM_STRIP];
  TGraph* graphpeaks10[NUM_SSD][NUM_STRIP];
  TGraph* graph11[NUM_SSD][NUM_STRIP];
  TGraph* graph10[NUM_SSD][NUM_STRIP];
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
      fit[i][j] = new TF1(Form("fun_SSD%d_CH%02d",i+1,j),"pol1",0.,4000.);
      graphpeaks11[i][j] = new TGraph(numpeaks11,PulserPeaks11[i][j],PulserHeight11[i][j]);
      graphpeaks10[i][j] = new TGraph(numpeaks10,PulserPeaks10[i][j],PulserHeight10[i][j]);
      graph11[i][j]      = new TGraph(numpeaks11,PulserPeaks11[i][j],RelativeErr11[i][j]);
      graph10[i][j]      = new TGraph(numpeaks10,PulserPeaks10[i][j],RelativeErr10[i][j]);
      // 画出全部的 11 个点
      cans->cd(1);
      gPad->SetLeftMargin(0.13);
      gPad->SetGridx();
      gPad->SetGridy();
      graphpeaks11[i][j]->SetTitle(Form("SSD%d_%s_%02d_PulserCali",i+1,layertag,j));
      graphpeaks11[i][j]->SetMarkerStyle(20);
      graphpeaks11[i][j]->SetMarkerColor(kBlue);
      graphpeaks11[i][j]->GetXaxis()->SetTitle("ADC_Channel");
      graphpeaks11[i][j]->GetXaxis()->SetTitleSize(0.04);
      graphpeaks11[i][j]->GetXaxis()->CenterTitle(1);
      graphpeaks11[i][j]->GetXaxis()->SetRangeUser(0.,4096);
      graphpeaks11[i][j]->GetXaxis()->SetNdivisions(510);
      graphpeaks11[i][j]->GetYaxis()->SetTitle("Relative_PulserHeight11");
      graphpeaks11[i][j]->GetYaxis()->SetTitleSize(0.04);
      graphpeaks11[i][j]->GetYaxis()->CenterTitle(1);
      graphpeaks11[i][j]->GetYaxis()->SetRangeUser(0.,1.1);
      graphpeaks11[i][j]->GetYaxis()->SetNdivisions(511);
      graphpeaks10[i][j]->SetTitle(Form("SSD%d_%s_%02d_PulserCali",i+1,layertag,j));
      graphpeaks10[i][j]->SetMarkerStyle(20);
      graphpeaks10[i][j]->SetMarkerColor(kBlue);
      graphpeaks10[i][j]->GetXaxis()->SetTitle("ADC_Channel");
      graphpeaks10[i][j]->GetXaxis()->SetTitleSize(0.04);
      graphpeaks10[i][j]->GetXaxis()->CenterTitle(1);
      graphpeaks10[i][j]->GetXaxis()->SetRangeUser(0.,4096);
      graphpeaks10[i][j]->GetXaxis()->SetNdivisions(505);
      graphpeaks10[i][j]->GetYaxis()->SetTitle("Relative_PulserHeight11");
      graphpeaks10[i][j]->GetYaxis()->SetTitleSize(0.04);
      graphpeaks10[i][j]->GetYaxis()->CenterTitle(1);
      graphpeaks10[i][j]->GetYaxis()->SetRangeUser(0.,1.1);
      graphpeaks10[i][j]->GetYaxis()->SetNdivisions(511);
      fit[i][j]->SetLineColor(kRed);
      if (i>1 && strcmp(layertag,"L1S")==0) {
        graphpeaks10[i][j]->Fit(fit[i][j]);
        graphpeaks10[i][j]->Draw("AP");
      } else {
        graphpeaks11[i][j]->Fit(fit[i][j]);
        graphpeaks11[i][j]->Draw("AP");
      }

      // 画出全部的 11 个点
      cans->cd(2);
      gPad->SetLeftMargin(0.13);
      gPad->SetGridx();
      gPad->SetGridy();
      graph11[i][j]->SetTitle(Form("SSD%d_%s_%02d_ErrBetweenPointsAndCurve",i+1,layertag,j));
      graph11[i][j]->SetMarkerStyle(20);
      graph11[i][j]->SetMarkerColor(kBlue);
      graph11[i][j]->GetXaxis()->SetTitle("ADC_Channel");
      graph11[i][j]->GetXaxis()->SetTitleSize(0.04);
      graph11[i][j]->GetXaxis()->CenterTitle(1);
      graph11[i][j]->GetXaxis()->SetRangeUser(0.,4096);
      graph11[i][j]->GetXaxis()->SetNdivisions(510);
      graph11[i][j]->GetYaxis()->SetRangeUser(-1.5,1.5);
      graph11[i][j]->GetYaxis()->SetNdivisions(1006);
      graph11[i][j]->GetYaxis()->SetTitle("Relative_Errors (%)");
      graph11[i][j]->GetYaxis()->SetTitleSize(0.04);
      graph11[i][j]->GetYaxis()->CenterTitle(1);

      graph10[i][j]->SetTitle(Form("SSD%d_%s_%02d_ErrBetweenPointsAndCurve",i+1,layertag,j));
      graph10[i][j]->SetMarkerStyle(20);
      graph10[i][j]->SetMarkerColor(kBlue);
      graph10[i][j]->GetXaxis()->SetTitle("ADC_Channel");
      graph10[i][j]->GetXaxis()->SetTitleSize(0.04);
      graph10[i][j]->GetXaxis()->CenterTitle(1);
      graph10[i][j]->GetXaxis()->SetRangeUser(0.,4096);
      graph10[i][j]->GetXaxis()->SetNdivisions(505);
      graph10[i][j]->GetYaxis()->SetRangeUser(-1.5,1.5);
      graph10[i][j]->GetYaxis()->SetNdivisions(1006);
      graph10[i][j]->GetYaxis()->SetTitle("Relative_Errors (%)");
      graph10[i][j]->GetYaxis()->SetTitleSize(0.04);
      graph10[i][j]->GetYaxis()->CenterTitle(1);

      latex[i][j] = new TLatex(0.2*graph11[i][j]->GetXaxis()->GetXmax(),1.2,
                              "Relative_Err = #frac{Pulser-Cali}{Cali}");
      latex_average[i][j] = new TLatex(0.20*graph11[i][j]->GetXaxis()->GetXmax(),0.7,
                              Form("<Relative_Err>_{10Peaks} = %.2f (%%)",Sum_RelativeErr10[i][j]/numpeaks10));
      latex[i][j]->SetTextColor(kMagenta);
      latex_average[i][j]->SetTextColor(kBlue);

      if ((i>1 && strcmp(layertag,"L1S")==0)) {
        graph10[i][j]->Draw("APL");
        line_averageErr[i][j] = new TLine(graph10[i][j]->GetXaxis()->GetXmin(),
                                 Sum_RelativeErr10[i][j]/numpeaks10,
                                 graph10[i][j]->GetXaxis()->GetXmax(),
                                 Sum_RelativeErr10[i][j]/numpeaks11);
      } else {
        graph11[i][j]->Draw("APL");
        line_averageErr[i][j] = new TLine(graph11[i][j]->GetXaxis()->GetXmin(),
                                 Sum_RelativeErr10[i][j]/numpeaks10,
                                 graph11[i][j]->GetXaxis()->GetXmax(),
                                 Sum_RelativeErr10[i][j]/numpeaks11);
      }
      line_averageErr[i][j]->SetLineColor(kGreen);
      line_averageErr[i][j]->SetLineStyle(7);
      line_averageErr[i][j]->SetLineWidth(3);
      line_averageErr[i][j]->Draw();
      latex[i][j]->Draw();
      latex_average[i][j]->Draw();
      cans->Print(pathPDFOutput.c_str());
      gPad->Modified();
      gPad->Update();
    }
  }
  cans_end->Print(pathPDFOutputEnd.c_str());
  return;
}
//******************************************************************************
