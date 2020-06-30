////////////////////////////////////////////////////////////////////////////////
//
//   硅条的能量刻度已基本完成，现在需要对异常的 Channels 进行评估、寻找原因.
//   异常的 Channels 分别是:
//   1. Pulser 刻度: SSD2_L2F_CH00,CH01  这两道脉冲峰值约是其他道的一半
//   2. Alpha 刻度:  SSD4_L2F_CH13,CH14   这两道有两组 alpha 峰，此前的刻度使用较大的一组 alpha 峰
//   3. Alpha 刻度:  SSD4_L1S_CH00-CH15   前后两次刻度有一个 shift, 此前的刻度使用 AlphaCali05_08
//
//   评估的内容包括：
//   (1) 分别修正 alpha 能量点的 ADC 道址 与 能量    => 结果: 没找到修正方法, 暂不作修正
//   (2) 将两组 alpha 峰峰值相加, 在对这两道进行刻度  => 结果: peak1Low + peak1Up - pedestal
//   (3) 对于 SSD4_L1S, 使用 AlphaCali00_04 再进行刻度, 与AlphaCali05_08进行对比 =>  => 结果: 没找到修正方法, 暂使用 AlphaCali05_08
//
//   gfh, 2020-06-18
////////////////////////////////////////////////////////////////////////////////

#include "../../include/ReadFileModule.h"


Int_t SSDNum = 4;
Int_t CHNum  = 16;


//___________________________________________
void Check_SSD2_L2F_CH00_CH01();
void Check_SSD4_L2F_CH13_CH14();
void Check_SSD4_L1S_CH00_CH15_AlphaCali00_04();



//___________________________________________
void SiEnergyCali_CheckAbnormalChannels()
{
  Check_SSD2_L2F_CH00_CH01();
  Check_SSD4_L2F_CH13_CH14();
  Check_SSD4_L1S_CH00_CH15_AlphaCali00_04();
}


//____________________________________________
void Check_SSD2_L2F_CH00_CH01()
{
  std::string L2FTag("L2F");
  std::string PulserTag("Height");
  std::string AlphaCali00_48("AlphaCali00_48");

  std::string pathPulserIn(Form("output/SSD_%s_PulserCali_%s.dat",L2FTag.c_str(),PulserTag.c_str()));
  std::string pathAlphaCali00_48(Form("output/SSD_%s_AlphaPeaks_%s.dat",L2FTag.c_str(),AlphaCali00_48.c_str()));
  std::string pathAlphaEnergiesIn("output/SSD_AlphaEnergies.dat");
  std::string pathEnergyCaliPars(Form("output/SSD_%s_HeightAndAlphaFitPars.dat",L2FTag.c_str()));

  std::string pathpngPulserCali_SSD2_L2F_CH00_01(Form("figures/CheckPulserCali_SSD2_%s_CH00_CH01_%s.png",L2FTag.c_str(),PulserTag.c_str()));
  std::string pathPNGSSD2_L2F_CH00_01_Pars(Form("figures/Check_SSD2_%s_CH00_CH01_Peak1ParameterDistributions.png",L2FTag.c_str()));


  Int_t num_peaks             = 11;
  Int_t num_channel           = 2;
  Int_t numpar_PulserIn       = 15;  // a, err_a, b,err_b, pulser1...pulser11
  Int_t numpar_AlphaCali00_48 = 3;  // peak1, peak2, peak3
  Int_t numpar_AlphaEnergies  = 3;  // E1, E2, E3
  Int_t numpar_EnergyCaliPars = 2;  // E1, E2, E3

  // 手动设定系数 a 与 b 的范围
  Double_t YRangeLow = -22.;
  Double_t YRangeUp  = 22.;
  Double_t YScaleRangeLow = 2.;
  Double_t YScaleRangeUp  = -2.;
  Double_t SSD2_L2F_k_RangeLow = 0.05;
  Double_t SSD2_L2F_k_RangeUp  = 0.07;
  Double_t SSD2_L2F_h_RangeLow = -5.0;
  Double_t SSD2_L2F_h_RangeUp  = -2.5;
  Double_t E1_DeltaCH = 15.;
  Double_t E1_DeltaE  = 0.85;

  Double_t k_scalfactor = (YRangeUp-YScaleRangeLow) /(SSD2_L2F_k_RangeUp-SSD2_L2F_k_RangeLow);
  Double_t h_scalfactor = (YScaleRangeUp-YRangeLow) /(SSD2_L2F_h_RangeUp-SSD2_L2F_h_RangeLow);
  Double_t Height11[11] = {1./2, .9/2, .8/2, .7/2, .6/2, .5/2, .4/2, .3/2, .2/2, .1/2, 0.05/2};
  //Double_t Height11[11] = {1., .9, .8, .7, .6, .5, .4, .3, .2, .1, 0.05};

  ReadFileModule readfile;
  Double_t*** PulserIn       = readfile.ReadData(pathPulserIn.c_str(),       SSDNum, CHNum, numpar_PulserIn);
  Double_t*** AlphaCHIn00_48 = readfile.ReadData(pathAlphaCali00_48.c_str(), SSDNum, CHNum, numpar_AlphaCali00_48);
  Double_t*** AlphaEIn       = readfile.ReadData(pathAlphaEnergiesIn.c_str(),SSDNum, CHNum, numpar_AlphaEnergies);
  Double_t*** EnergyCaliPars = readfile.ReadData(pathEnergyCaliPars.c_str(), SSDNum, CHNum, numpar_EnergyCaliPars);

  Double_t k1_check[num_channel];
  Double_t h1_check[num_channel];
  Double_t k1_DeltaCh[num_channel];
  Double_t h1_DeltaCh[num_channel];
  Double_t k1_DeltaE[num_channel];
  Double_t h1_DeltaE[num_channel];

  Double_t peak_ch[num_channel][num_peaks];
  for (Int_t i=0; i<num_channel; i++)
  {
    for (Int_t j=0; j<num_peaks; j++)
    {
      peak_ch[i][j] = PulserIn[1][i][j+4];  // SSD2_L2F_CH00, peak1...peak11
    }
  }

  //_____________________________________
  // for the pulser calibrations
  TF1*    fit_ch[num_channel];
  TGraph* graph_ch[num_channel];
  TLatex* a_latex_ch[num_channel];
  TLatex* b_latex_ch[num_channel];
  for (Int_t i=0; i<num_channel; i++)
  {
    fit_ch[i] = new TF1(Form("fit_ch%02d",i),"pol1",0,2000);
    graph_ch[i] = new TGraph(num_peaks, peak_ch[i], Height11);
    graph_ch[i]->SetTitle(Form("PulserCali_SSD_%s_CH00_%s",L2FTag.c_str(),PulserTag.c_str()));
    graph_ch[i]->Fit(Form("fit_ch%02d",i));

    a_latex_ch[i] = new TLatex(600,0.8,Form("a = %.8f",fit_ch[i]->GetParameter(1)));
    b_latex_ch[i] = new TLatex(600,0.7,Form("b = %.8f",fit_ch[i]->GetParameter(0)));
  }


  // get all the pars of SSD2_L2F
  Double_t k[CHNum];
  Double_t h[CHNum];
  Double_t k_scale[CHNum];
  Double_t h_scale[CHNum];
  Double_t SSDCHNum[CHNum];

  Double_t k_scaleCH[num_channel];
  Double_t h_scaleCH[num_channel];
  Double_t k_scaleE[num_channel];
  Double_t h_scaleE[num_channel];

  Double_t k_sum = 0.;
  Double_t h_sum = 0.;

  Double_t k_average;
  Double_t h_average;
  Double_t k_average14;
  Double_t h_average14;
  Double_t k_averageDeltaCH;
  Double_t h_averageDeltaCH;
  Double_t k_averageDeltaE;
  Double_t h_averageDeltaE;

  Double_t k_average_scale;
  Double_t h_average_scale;
  Double_t k_average14_scale;
  Double_t h_average14_scale;
  Double_t k_averageDeltaCH_scale;
  Double_t h_averageDeltaCH_scale;
  Double_t k_averageDeltaE_scale;
  Double_t h_averageDeltaE_scale;
  //__________________________________________
  // for parameter distributions
  for (Int_t i=0; i<num_channel; i++)
  {
    Double_t a = fit_ch[i]->GetParameter(1);
    Double_t b = fit_ch[i]->GetParameter(0);

    Double_t E1_CH_00_48 = AlphaCHIn00_48[1][i][0]; // peak1 of SSD2_L2F_CH00,CH01
    Double_t E1_MeV      = AlphaEIn[1][i][0];       // energy of peak1

    Double_t c1 = E1_MeV / (a * E1_CH_00_48 + b);
    k1_check[i] = c1 * a;
    h1_check[i] = c1 * b;

    Double_t c1_DeltaCh = E1_MeV / (a * (E1_CH_00_48 + E1_DeltaCH)+ b);
    k1_DeltaCh[i] = c1_DeltaCh * a;
    h1_DeltaCh[i] = c1_DeltaCh * b;

    Double_t c1_DeltaE = (E1_MeV - E1_DeltaE)/ (a * E1_CH_00_48 + b);
    k1_DeltaE[i] = c1_DeltaE * a;
    h1_DeltaE[i] = c1_DeltaE * b;

    k_scaleCH[i] = (k1_DeltaCh[i] - SSD2_L2F_k_RangeLow) * k_scalfactor + 2;
    h_scaleCH[i] = (h1_DeltaCh[i] - SSD2_L2F_h_RangeUp)  * h_scalfactor - 2;

    k_scaleE[i] = (k1_DeltaE[i] - SSD2_L2F_k_RangeLow) * k_scalfactor + 2;
    h_scaleE[i] = (h1_DeltaE[i] - SSD2_L2F_h_RangeUp)  * h_scalfactor - 2;
  }


  for (Int_t i=0; i<CHNum; i++)
  {
    SSDCHNum[i] = i+1;   // 横坐标, strip number

    // for cans_distribution->cd(1);
    if (i<2) {
      k[i] = k1_check[i];
      h[i] = h1_check[i];    // 替换新的刻度参数, 得到的结果没有变！ 需要新的办法来 check！
    } else {
      k[i] = EnergyCaliPars[1][i][0];
      h[i] = EnergyCaliPars[1][i][1];
    }
    k_scale[i] = (k[i] - SSD2_L2F_k_RangeLow) * k_scalfactor + 2.;
    h_scale[i] = (h[i] - SSD2_L2F_h_RangeUp)  * h_scalfactor - 2.;

    k_sum += k[i];
    h_sum += h[i];
  }
  k_average = k_sum/CHNum;
  h_average = h_sum/CHNum;
  k_average_scale = (k_average - SSD2_L2F_k_RangeLow) * k_scalfactor + 2.;
  h_average_scale = (h_average - SSD2_L2F_h_RangeUp)  * h_scalfactor - 2.;

  k_average14 = (k_sum-k[0]-k[1])/14;
  h_average14 = (h_sum-h[0]-h[1])/14;
  k_average14_scale = (k_average14 - SSD2_L2F_k_RangeLow) * k_scalfactor + 2.;
  h_average14_scale = (h_average14 - SSD2_L2F_h_RangeUp)  * h_scalfactor - 2.;

  k_averageDeltaCH = (k_sum-k[0]-k[1] + k1_DeltaCh[0] + k1_DeltaCh[1])/CHNum;
  h_averageDeltaCH = (h_sum-h[0]-h[1] + h1_DeltaCh[0] + h1_DeltaCh[1])/CHNum;
  k_averageDeltaCH_scale = (k_averageDeltaCH - SSD2_L2F_k_RangeLow) * k_scalfactor + 2.;
  h_averageDeltaCH_scale = (h_averageDeltaCH - SSD2_L2F_h_RangeUp)  * h_scalfactor - 2.;

  k_averageDeltaE = (k_sum-k[0]-k[1] + k1_DeltaE[0] + k1_DeltaE[1])/CHNum;
  h_averageDeltaE = (h_sum-h[0]-h[1] + h1_DeltaE[0] + h1_DeltaE[1])/CHNum;
  k_averageDeltaE_scale = (k_averageDeltaE - SSD2_L2F_k_RangeLow) * k_scalfactor + 2.;
  h_averageDeltaE_scale = (h_averageDeltaE - SSD2_L2F_h_RangeUp)  * h_scalfactor - 2.;


  //____________________________________________________________________________
  // Draw the par Distributions
  TCanvas* cans_distribution = new TCanvas("cans_distribution","cans_distribution",1200,1000);
  cans_distribution->Divide(2,2);

  //________________________________________
  // 将脉冲高度取为一半，画出参数分布图
  cans_distribution->cd(1);

  TMultiGraph* mg = new TMultiGraph();
  TGraph* k_scale_distribution = new TGraph(CHNum, SSDCHNum, k_scale);
  TGraph* h_scale_distribution = new TGraph(CHNum, SSDCHNum, h_scale);
  k_scale_distribution->SetMarkerStyle(20);
  k_scale_distribution->SetMarkerColor(kBlue);
  h_scale_distribution->SetMarkerStyle(20);
  h_scale_distribution->SetMarkerColor(kRed);
  mg->Add(k_scale_distribution);
  mg->Add(h_scale_distribution);

  auto histk = k_scale_distribution->GetHistogram();
  histk->SetTitle(Form("SSD2_%s_Peak1ParameterDistributions",L2FTag.c_str()));
  histk->GetXaxis()->SetRangeUser(0.5,16.5);
  histk->GetXaxis()->SetNdivisions(117);
  histk->GetXaxis()->SetTitle("Channel");
  histk->GetXaxis()->CenterTitle(1);
  histk->GetXaxis()->SetTitleSize(0.04);
  histk->GetYaxis()->SetRangeUser(YRangeLow,YRangeUp);
  histk->Draw("Y+ H");
  gPad->SetGridx();
  gPad->Update();

  TGaxis* k_Yaxis = new TGaxis(gPad->GetUxmin(),YScaleRangeLow,gPad->GetUxmin(),YRangeUp,
                               SSD2_L2F_k_RangeLow,SSD2_L2F_k_RangeUp,1002,"-R");
  TGaxis* h_Yaxis = new TGaxis(gPad->GetUxmin(),YRangeLow,gPad->GetUxmin(),YScaleRangeUp,
                               SSD2_L2F_h_RangeLow,SSD2_L2F_h_RangeUp,1002,"-R");
  k_Yaxis->SetLineColor (kBlue);
  k_Yaxis->SetLabelColor(kBlue);
  h_Yaxis->SetLineColor (kRed);
  h_Yaxis->SetLabelColor(kRed);

  TLine* line_k_average = new TLine(gPad->GetUxmin(),k_average_scale,gPad->GetUxmax(),k_average_scale);
  TLine* line_h_average = new TLine(gPad->GetUxmin(),h_average_scale,gPad->GetUxmax(),h_average_scale);
  line_k_average->SetLineColor(kGreen);
  line_k_average->SetLineStyle(7);
  line_k_average->SetLineWidth(3);
  line_h_average->SetLineColor(kGreen);
  line_h_average->SetLineStyle(7);
  line_h_average->SetLineWidth(3);

  TLatex* latex_k = new TLatex(5,18.,Form("<a> = %.8f",k_average));
  TLatex* latex_h = new TLatex(5,-4.,Form("<b> = %.8f",h_average));
  TLatex* latex_average16 = new TLatex(5.5,14.,"Average (16 Channels)");
  latex_average16->SetTextColor(kMagenta);

  mg->Draw("PL");
  k_Yaxis->Draw();
  h_Yaxis->Draw();
  line_k_average->Draw();
  line_h_average->Draw();
  latex_h->Draw();
  latex_k->Draw();
  latex_average16->Draw();


  //____________________________________________
  // 将 SSD_L2F_CH00,CH01 取为其余 14 道的平均值
  cans_distribution->cd(2);
  histk->Draw("Y+ H");
  gPad->SetGridx();
  gPad->Update();

  TMultiGraph* mg14 = new TMultiGraph();
  TGraph* k14_scale_distribution = (TGraph*)k_scale_distribution->Clone();
  TGraph* h14_scale_distribution = (TGraph*)h_scale_distribution->Clone();
  k14_scale_distribution->SetPoint(0, 1., k_average14_scale);
  k14_scale_distribution->SetPoint(1, 2., k_average14_scale);
  mg14->Add(k14_scale_distribution);
  mg14->Add(h14_scale_distribution);

  TLine* line_k_average14 = new TLine(gPad->GetUxmin(),k_average14_scale,gPad->GetUxmax(),k_average14_scale);
  TLine* line_h_average14 = new TLine(gPad->GetUxmin(),h_average14_scale,gPad->GetUxmax(),h_average14_scale);
  line_k_average14->SetLineColor(kGreen);
  line_k_average14->SetLineStyle(7);
  line_k_average14->SetLineWidth(3);
  line_h_average14->SetLineColor(kGreen);
  line_h_average14->SetLineStyle(7);
  line_h_average14->SetLineWidth(3);

  TLatex* latex_k14 = new TLatex(5,18.,Form("<a> = %.8f",k_average14));
  TLatex* latex_h14 = new TLatex(5,-4.,Form("<b> = %.8f",h_average14));
  TLatex* latex_average14 = new TLatex(5.5,14.,"Average (14 Channels)");
  latex_average14->SetTextColor(kMagenta);

  mg14->Draw("PL");
  k_Yaxis->Draw();
  h_Yaxis->Draw();
  line_k_average14->Draw();
  line_h_average14->Draw();
  latex_k14->Draw();
  latex_h14->Draw();
  latex_average14->Draw();


  //__________________________________________________________________
  // 改变 alpha 刻度点的 ADC 道址, 将 SSD2_L2F_CH00-CH01 修正回到平均值附近
  cans_distribution->cd(3);
  histk->Draw("Y+ H");
  gPad->SetGridx();
  gPad->Update();

  TMultiGraph* mgch = new TMultiGraph();
  TGraph* kch_scale_distribution = (TGraph*)k_scale_distribution->Clone();
  TGraph* hch_scale_distribution = (TGraph*)h_scale_distribution->Clone();
  kch_scale_distribution->SetPoint(0, 1., k_scaleCH[0]);
  kch_scale_distribution->SetPoint(1, 2., k_scaleCH[1]);
  mgch->Add(kch_scale_distribution);
  mgch->Add(hch_scale_distribution);

  TLine* line_k_averagech = new TLine(gPad->GetUxmin(),k_averageDeltaCH_scale,gPad->GetUxmax(),k_averageDeltaCH_scale);
  TLine* line_h_averagech = new TLine(gPad->GetUxmin(),h_averageDeltaCH_scale,gPad->GetUxmax(),h_averageDeltaCH_scale);
  line_k_averagech->SetLineColor(kGreen);
  line_k_averagech->SetLineStyle(7);
  line_k_averagech->SetLineWidth(3);
  line_h_averagech->SetLineColor(kGreen);
  line_h_averagech->SetLineStyle(7);
  line_h_averagech->SetLineWidth(3);

  TLatex* latex_kch = new TLatex(5,18.,Form("<a> = %.8f",k_averageDeltaCH));
  TLatex* latex_hch = new TLatex(5,-4.,Form("<b> = %.8f",h_averageDeltaCH));
  TLatex* latex_DeltaCH = new TLatex(5.5,14.,Form("CH( #alpha_{1} ) + %.f",E1_DeltaCH));
  latex_DeltaCH->SetTextColor(kMagenta);

  mgch->Draw("PL");
  k_Yaxis->Draw();
  h_Yaxis->Draw();
  line_k_averagech->Draw();
  line_h_averagech->Draw();
  latex_kch->Draw();
  latex_hch->Draw();
  latex_DeltaCH->Draw();


  //__________________________________________________________________
  // 改变 alpha 刻度点的 ADC 道址, 将 SSD2_L2F_CH00-CH01 修正回到平均值附近
  cans_distribution->cd(4);
  histk->Draw("Y+ H");
  gPad->SetGridx();
  gPad->Update();

  TMultiGraph* mge = new TMultiGraph();
  TGraph* ke_scale_distribution = (TGraph*)k_scale_distribution->Clone();
  TGraph* he_scale_distribution = (TGraph*)h_scale_distribution->Clone();
  ke_scale_distribution->SetPoint(0, 1., k_scaleE[0]);
  ke_scale_distribution->SetPoint(1, 2., k_scaleE[1]);
  mge->Add(ke_scale_distribution);
  mge->Add(he_scale_distribution);

  TLine* line_k_averagee = new TLine(gPad->GetUxmin(),k_averageDeltaE_scale,gPad->GetUxmax(),k_averageDeltaE_scale);
  TLine* line_h_averagee = new TLine(gPad->GetUxmin(),h_averageDeltaE_scale,gPad->GetUxmax(),h_averageDeltaE_scale);
  line_k_averagee->SetLineColor(kGreen);
  line_k_averagee->SetLineStyle(7);
  line_k_averagee->SetLineWidth(3);
  line_h_averagee->SetLineColor(kGreen);
  line_h_averagee->SetLineStyle(7);
  line_h_averagee->SetLineWidth(3);

  TLatex* latex_ke = new TLatex(5,18.,Form("<a> = %.8f",k_averageDeltaE));
  TLatex* latex_he = new TLatex(5,-4.,Form("<b> = %.8f",h_averageDeltaE));
  TLatex* latex_DeltaE = new TLatex(5,14.,Form("E_{MeV}( #alpha_{1} ) - %.2f",E1_DeltaE));
  latex_DeltaE->SetTextColor(kMagenta);

  mge->Draw("PL");
  k_Yaxis->Draw();
  h_Yaxis->Draw();
  line_k_averagee->Draw();
  line_h_averagee->Draw();
  latex_ke->Draw();
  latex_he->Draw();
  latex_DeltaE->Draw();

  cans_distribution->Print(pathPNGSSD2_L2F_CH00_01_Pars.c_str());


  //____________________________________________________________________________
  // Draw the pulser calibrations
  TCanvas* cans_pulser = new TCanvas("cans_pulser","Check_SSD2_L2F_CH00_CH01",1200,600);
  cans_pulser->Divide(2,1);
  for (Int_t i=0; i<num_channel; i++)
  {
    cans_pulser->cd(i+1);
    gPad->SetGridx();
    gPad->SetGridy();
    graph_ch[i]->SetMarkerStyle(20);
    graph_ch[i]->SetMarkerSize(1.5);
    graph_ch[i]->SetMarkerColor(kBlue);
    graph_ch[i]->GetYaxis()->SetRangeUser(0.,1.);
    graph_ch[i]->GetYaxis()->SetNdivisions(511);
    graph_ch[i]->Draw("AP*");
    a_latex_ch[i]->Draw("SAME");
    b_latex_ch[i]->Draw("SAME");
  }
  cans_pulser->Print(pathpngPulserCali_SSD2_L2F_CH00_01.c_str());

}



//___________________________________________
void Check_SSD4_L2F_CH13_CH14()
{
  std::string L2FTag("L2F");
  std::string PulserTag("Height");
  std::string AlphaCali00_48("AlphaCali00_48");

  std::string pathPulserIn(Form("output/SSD_%s_PulserCali_%s.dat",L2FTag.c_str(),PulserTag.c_str()));
  std::string pathAlphaCali00_48(Form("output/SSD_%s_AlphaPeaks_%s.dat",L2FTag.c_str(),AlphaCali00_48.c_str()));
  std::string pathAlphaEnergiesIn("output/SSD_AlphaEnergies.dat");
  std::string pathEnergyCaliPars(Form("output/SSD_%s_HeightAndAlphaFitPars.dat",L2FTag.c_str()));
  std::string pathPulserPedestalIn(Form("output/SSD_%s_PulserCaliPedestals_Pedestal0000.dat",L2FTag.c_str()));

  std::string pathPNGSSD4_L2F_CH13_14_Pars(Form("figures/Check_SSD4_%s_CH13_CH14_Peak1ParameterDistributions.png",L2FTag.c_str()));

  // 手动设定系数 a 与 b 的范围
  Double_t YRangeLow = -22.;
  Double_t YRangeUp  =  22.;
  Double_t YScaleRangeLow = 2.;
  Double_t YScaleRangeUp  = -2.;
  Double_t SSD4_L2F_k_RangeLow = 0.015;
  Double_t SSD4_L2F_k_RangeUp  = 0.022;
  Double_t SSD4_L2F_h_RangeLow = -2.0;
  Double_t SSD4_L2F_h_RangeUp  = -1.0;
  Double_t LowPeaks_SSD4_L2F_CH13_CH14[2] = {166.966, 168.828};

  Int_t num_channel           = 2;
  Int_t numpar_PulserIn       = 3;  // a, err_a, b,err_b, pulser1...pulser11
  Int_t numpar_AlphaCali00_48 = 3;  // peak1, peak2, peak3
  Int_t numpar_AlphaEnergies  = 3;  // E1, E2, E3
  Int_t numpar_EnergyCaliPars = 2;  // k1, h1
  Int_t numpar_PulserPedestal = 2;  // mean, sigma

  ReadFileModule readfile;
  Double_t*** PulserIn       = readfile.ReadData(pathPulserIn.c_str(),        SSDNum, CHNum, numpar_PulserIn);
  Double_t*** AlphaCHIn00_48 = readfile.ReadData(pathAlphaCali00_48.c_str(),  SSDNum, CHNum, numpar_AlphaCali00_48);
  Double_t*** AlphaEIn       = readfile.ReadData(pathAlphaEnergiesIn.c_str(), SSDNum, CHNum, numpar_AlphaEnergies);
  Double_t*** EnergyCaliPars = readfile.ReadData(pathEnergyCaliPars.c_str(),  SSDNum, CHNum, numpar_EnergyCaliPars);
  Double_t*** PulserPedestal = readfile.ReadData(pathPulserPedestalIn.c_str(),SSDNum, CHNum, numpar_PulserPedestal);

  Double_t k[CHNum];
  Double_t h[CHNum];
  Double_t k_scale[CHNum];
  Double_t h_scale[CHNum];
  Double_t SSDCHNum[CHNum];

  Double_t k_SumPeaks[num_channel];
  Double_t h_SumPeaks[num_channel];
  Double_t k_SumPeaks_scale[num_channel];
  Double_t h_SumPeaks_scale[num_channel];

  Double_t k_average;
  Double_t h_average;
  Double_t k_average_scale;
  Double_t h_average_scale;
  Double_t k_SumPeaks_average;
  Double_t h_SumPeaks_average;
  Double_t k_SumPeaks_average_scale;
  Double_t h_SumPeaks_average_scale;

  Double_t k_average14;
  Double_t h_average14;
  Double_t k_average14_scale;
  Double_t h_average14_scale;

  Double_t k_sum = 0.;
  Double_t h_sum = 0.;

  Double_t k_scalfactor = (YRangeUp-YScaleRangeLow) /(SSD4_L2F_k_RangeUp-SSD4_L2F_k_RangeLow);
  Double_t h_scalfactor = (YScaleRangeUp-YRangeLow) /(SSD4_L2F_h_RangeUp-SSD4_L2F_h_RangeLow);


  //___________________________________________
  // For cans_distribution_SSD4->cd(1);
  for (Int_t i=0; i<CHNum; i++)
  {
    SSDCHNum[i] = i+1;
    k[i] = EnergyCaliPars[3][i][0];
    h[i] = EnergyCaliPars[3][i][1];

    k_scale[i] = (k[i] - SSD4_L2F_k_RangeLow) * k_scalfactor + 2;
    h_scale[i] = (h[i] - SSD4_L2F_h_RangeUp)  * h_scalfactor - 2;

    k_sum += k[i];
    h_sum += h[i];
  }
  k_average = k_sum/CHNum;
  h_average = h_sum/CHNum;
  k_average_scale = (k_average - SSD4_L2F_k_RangeLow) * k_scalfactor + 2;
  h_average_scale = (h_average - SSD4_L2F_h_RangeUp)  * h_scalfactor - 2;

  k_average14 = (k_sum-k[13]-k[14])/14;
  h_average14 = (h_sum-h[13]-h[14])/14;
  k_average14_scale = (k_average14 - SSD4_L2F_k_RangeLow) * k_scalfactor + 2;
  h_average14_scale = (h_average14 - SSD4_L2F_h_RangeUp)  * h_scalfactor - 2;


  //__________________________________________
  // for cans_distribution_SSD4->cd(2);
  for (Int_t i=0; i<num_channel; i++)
  {
    Double_t a = PulserIn[3][13+i][0];  // a
    Double_t b = PulserIn[3][13+i][2];  // b

    Double_t LowAlphaPeak = LowPeaks_SSD4_L2F_CH13_CH14[i] - PulserPedestal[3][13+i][0];
    Double_t E1_CH_00_48  = AlphaCHIn00_48[3][13+i][0] + LowAlphaPeak;
    Double_t E1_MeV       = AlphaEIn[3][13+i][0];       // energy of peak1

    Double_t c1 = E1_MeV / (a * E1_CH_00_48 + b);
    k_SumPeaks[i] = c1 * a;
    h_SumPeaks[i] = c1 * b;

    k_SumPeaks_scale[i] = (k_SumPeaks[i] - SSD4_L2F_k_RangeLow) * k_scalfactor + 2;
    h_SumPeaks_scale[i] = (h_SumPeaks[i] - SSD4_L2F_h_RangeUp)  * h_scalfactor - 2;
  }
  k_SumPeaks_average = (k_sum-k[13]-k[14]+k_SumPeaks[0]+k_SumPeaks[1])/CHNum;
  h_SumPeaks_average = (h_sum-h[13]-h[14]+h_SumPeaks[0]+h_SumPeaks[1])/CHNum;
  k_SumPeaks_average_scale = (k_SumPeaks_average - SSD4_L2F_k_RangeLow) * k_scalfactor + 2;
  h_SumPeaks_average_scale = (h_SumPeaks_average - SSD4_L2F_h_RangeUp)  * h_scalfactor - 2;

  //____________________________________________________________________________
  // Draw the par Distributions
  TCanvas* cans_distribution_SSD4 = new TCanvas("cans_distribution_SSD4","cans_distribution_SSD4",1200,1000);
  cans_distribution_SSD4->Divide(2,2);

  //________________________________________
  // 直接画出 SSD4 刻度参数分布
  cans_distribution_SSD4->cd(1);

  TMultiGraph* mg = new TMultiGraph();
  TGraph* k_scale_distribution = new TGraph(CHNum, SSDCHNum, k_scale);
  TGraph* h_scale_distribution = new TGraph(CHNum, SSDCHNum, h_scale);
  k_scale_distribution->SetMarkerStyle(20);
  k_scale_distribution->SetMarkerColor(kBlue);
  h_scale_distribution->SetMarkerStyle(20);
  h_scale_distribution->SetMarkerColor(kRed);
  mg->Add(k_scale_distribution);
  mg->Add(h_scale_distribution);

  auto histk = k_scale_distribution->GetHistogram();
  histk->SetTitle(Form("SSD4_%s_Peak1ParameterDistributions",L2FTag.c_str()));
  histk->GetXaxis()->SetRangeUser(0.5,16.5);
  histk->GetXaxis()->SetNdivisions(117);
  histk->GetXaxis()->SetTitle("Channel");
  histk->GetXaxis()->CenterTitle(1);
  histk->GetXaxis()->SetTitleSize(0.04);
  histk->GetYaxis()->SetRangeUser(YRangeLow,YRangeUp);
  histk->Draw("Y+ H");
  gPad->SetGridx();
  gPad->Update();

  TGaxis* k_Yaxis = new TGaxis(gPad->GetUxmin(),YScaleRangeLow,gPad->GetUxmin(),YRangeUp,
                               SSD4_L2F_k_RangeLow,SSD4_L2F_k_RangeUp,1002,"-R");
  TGaxis* h_Yaxis = new TGaxis(gPad->GetUxmin(),YRangeLow,gPad->GetUxmin(),YScaleRangeUp,
                               SSD4_L2F_h_RangeLow,SSD4_L2F_h_RangeUp,1002,"-R");
  k_Yaxis->SetLineColor (kBlue);
  k_Yaxis->SetLabelColor(kBlue);
  h_Yaxis->SetLineColor (kRed);
  h_Yaxis->SetLabelColor(kRed);

  TLine* line_k_average = new TLine(gPad->GetUxmin(),k_average_scale,gPad->GetUxmax(),k_average_scale);
  TLine* line_h_average = new TLine(gPad->GetUxmin(),h_average_scale,gPad->GetUxmax(),h_average_scale);
  line_k_average->SetLineColor(kGreen);
  line_k_average->SetLineStyle(7);
  line_k_average->SetLineWidth(3);
  line_h_average->SetLineColor(kGreen);
  line_h_average->SetLineStyle(7);
  line_h_average->SetLineWidth(3);

  TLatex* latex_k = new TLatex(6,18.,Form("<a> = %.5f",k_average));
  TLatex* latex_h = new TLatex(6,-4.,Form("<b> = %.5f",h_average));
  TLatex* latex_average16 = new TLatex(4,14.,"Average( 16 Channels )");
  latex_average16->SetTextColor(kMagenta);

  mg->Draw("PL");
  k_Yaxis->Draw();
  h_Yaxis->Draw();
  line_k_average->Draw();
  line_h_average->Draw();
  latex_h->Draw();
  latex_k->Draw();
  latex_average16->Draw();

  //____________________________________________
  // 将 SSD_L2F_CH00,CH01 取为其余 14 道的平均值
  cans_distribution_SSD4->cd(2);
  histk->Draw("Y+ H");
  gPad->SetGridx();
  gPad->Update();

  TMultiGraph* mg14 = new TMultiGraph();
  TGraph* k14_scale_distribution = (TGraph*)k_scale_distribution->Clone();
  TGraph* h14_scale_distribution = (TGraph*)h_scale_distribution->Clone();
  k14_scale_distribution->SetPoint(0, 1., k_average14_scale);
  k14_scale_distribution->SetPoint(1, 2., k_average14_scale);
  mg14->Add(k14_scale_distribution);
  mg14->Add(h14_scale_distribution);

  TLine* line_k_average14 = new TLine(gPad->GetUxmin(),k_average14_scale,gPad->GetUxmax(),k_average14_scale);
  TLine* line_h_average14 = new TLine(gPad->GetUxmin(),h_average14_scale,gPad->GetUxmax(),h_average14_scale);
  line_k_average14->SetLineColor(kGreen);
  line_k_average14->SetLineStyle(7);
  line_k_average14->SetLineWidth(3);
  line_h_average14->SetLineColor(kGreen);
  line_h_average14->SetLineStyle(7);
  line_h_average14->SetLineWidth(3);

  TLatex* latex_k14 = new TLatex(5,18.,Form("<a> = %.8f",k_average14));
  TLatex* latex_h14 = new TLatex(5,-4.,Form("<b> = %.8f",h_average14));
  TLatex* latex_average14 = new TLatex(4,14.,"Average( 14 Channels )");
  latex_average14->SetTextColor(kMagenta);

  mg14->Draw("PL");
  k_Yaxis->Draw();
  h_Yaxis->Draw();
  line_k_average14->Draw();
  line_h_average14->Draw();
  latex_k14->Draw();
  latex_h14->Draw();
  latex_average14->Draw();


  //____________________________________________
  // 将 SSD_L2F_CH00,CH01 取为其余 14 道的平均值
  cans_distribution_SSD4->cd(3);
  histk->Draw("Y+ H");
  gPad->SetGridx();
  gPad->Update();

  TMultiGraph* mgsum = new TMultiGraph();
  TGraph* ksum_scale_distribution = (TGraph*)k_scale_distribution->Clone();
  TGraph* hsum_scale_distribution = (TGraph*)h_scale_distribution->Clone();
  ksum_scale_distribution->SetPoint(13, 14., k_SumPeaks_scale[0]);
  ksum_scale_distribution->SetPoint(14, 15., k_SumPeaks_scale[1]);
  hsum_scale_distribution->SetPoint(13, 14., h_SumPeaks_scale[0]);
  hsum_scale_distribution->SetPoint(14, 15., h_SumPeaks_scale[1]);
  mgsum->Add(ksum_scale_distribution);
  mgsum->Add(hsum_scale_distribution);

  TLine* line_k_averagesum = new TLine(gPad->GetUxmin(),k_SumPeaks_average_scale,gPad->GetUxmax(),k_SumPeaks_average_scale);
  TLine* line_h_averagesum = new TLine(gPad->GetUxmin(),h_SumPeaks_average_scale,gPad->GetUxmax(),h_SumPeaks_average_scale);
  line_k_averagesum->SetLineColor(kGreen);
  line_k_averagesum->SetLineStyle(7);
  line_k_averagesum->SetLineWidth(3);
  line_h_averagesum->SetLineColor(kGreen);
  line_h_averagesum->SetLineStyle(7);
  line_h_averagesum->SetLineWidth(3);

  TLatex* latex_ksum = new TLatex(5,18.,Form("<k> = %.8f",k_SumPeaks_average));
  TLatex* latex_hsum = new TLatex(5,-4.,Form("<h> = %.8f",h_SumPeaks_average));
  TLatex* latex_averageSumPeaks = new TLatex(3,14.,"HighPeak1 + LowPeak1 - Pedestal ");
  latex_averageSumPeaks->SetTextColor(kMagenta);

  mgsum->Draw("PL");
  k_Yaxis->Draw();
  h_Yaxis->Draw();
  line_k_averagesum->Draw();
  line_h_averagesum->Draw();
  latex_ksum->Draw();
  latex_hsum->Draw();
  latex_averageSumPeaks->Draw();

  cans_distribution_SSD4->Print(pathPNGSSD4_L2F_CH13_14_Pars.c_str());
}


//___________________________________________
void Check_SSD4_L1S_CH00_CH15_AlphaCali00_04()
{
  std::string L1STag("L1S");
  std::string AlphaCali00_04("AlphaCali00_04");
  std::string AlphaCali05_08("AlphaCali05_08");

  std::string pathEnergyCaliPars05_08(Form("output/SSD_%s_HeightAndAlphaFitPars.dat",L1STag.c_str()));
  std::string pathEnergyCaliPars00_04(Form("output/SSD_%s_HeightAndAlphaFitPars_%s.dat",L1STag.c_str(),
                                      AlphaCali00_04.c_str()));

  std::string pathPNGOutput(Form("figures/Check_SSD4_%s_ParErrorsOf%sAnd%s.png",L1STag.c_str(),
                            AlphaCali00_04.c_str(),AlphaCali05_08.c_str()));

  Int_t numpar_ParsInput      = 2;
  Int_t numpar_ParsInput00_04 = 2;

  ReadFileModule readfile;
  Double_t*** ParsInput05_08 = readfile.ReadData(pathEnergyCaliPars05_08.c_str(), SSDNum, CHNum, numpar_ParsInput);
  Double_t*** ParsInput00_04 = readfile.ReadData(pathEnergyCaliPars00_04.c_str(), SSDNum, CHNum, numpar_ParsInput00_04);

  Double_t SSDCHNum[CHNum];
  Double_t k_05_08[CHNum];
  Double_t h_05_08[CHNum];
  Double_t k_00_04[CHNum];
  Double_t h_00_04[CHNum];
  Double_t k_RelativeErr[CHNum];
  Double_t h_RelativeErr[CHNum];
  for (Int_t i=0; i<CHNum; i++)
  {
    SSDCHNum[i] = i + 1;
    k_05_08[i]  = ParsInput05_08[3][i][0];
    h_05_08[i]  = ParsInput05_08[3][i][1];
    k_00_04[i]  = ParsInput00_04[3][i][0];
    h_00_04[i]  = ParsInput00_04[3][i][1];

    k_RelativeErr[i] = 100*abs((k_00_04[i]-k_05_08[i])/k_05_08[i]);
    h_RelativeErr[i] = 100*abs((h_00_04[i]-h_05_08[i])/h_05_08[i]);
  }

  TMultiGraph* mg_k   = new TMultiGraph();
  TMultiGraph* mg_h   = new TMultiGraph();
  TMultiGraph* mg_err = new TMultiGraph();
  TGraph* graph_k_05_08 = new TGraph(CHNum,SSDCHNum,k_05_08);
  TGraph* graph_h_05_08 = new TGraph(CHNum,SSDCHNum,h_05_08);
  TGraph* graph_k_00_04 = new TGraph(CHNum,SSDCHNum,k_00_04);
  TGraph* graph_h_00_04 = new TGraph(CHNum,SSDCHNum,h_00_04);
  TGraph* graph_k_RelativeErr = new TGraph(CHNum,SSDCHNum,k_RelativeErr);
  TGraph* graph_h_RelativeErr = new TGraph(CHNum,SSDCHNum,h_RelativeErr);

  graph_k_05_08->SetMarkerStyle(20);
  graph_k_05_08->SetMarkerColor(kBlue);
  graph_k_00_04->SetMarkerStyle(20);
  graph_k_00_04->SetMarkerColor(kRed);

  graph_h_05_08->SetMarkerStyle(20);
  graph_h_05_08->SetMarkerColor(kBlue);
  graph_h_00_04->SetMarkerStyle(20);
  graph_h_00_04->SetMarkerColor(kRed);

  graph_k_RelativeErr->SetMarkerStyle(20);
  graph_k_RelativeErr->SetMarkerColor(kBlue);

  graph_h_RelativeErr->SetMarkerStyle(20);
  graph_h_RelativeErr->SetMarkerColor(kRed);

  mg_k->Add(graph_k_05_08);
  mg_k->Add(graph_k_00_04);
  mg_h->Add(graph_h_05_08);
  mg_h->Add(graph_h_00_04);
  mg_err->Add(graph_k_RelativeErr);
  mg_err->Add(graph_h_RelativeErr);

  TCanvas* cans_SSD4 = new TCanvas("cans_SSD4","cans_SSD4",1200,1000);
  cans_SSD4->Divide(2,2);

  // k distributions
  cans_SSD4->cd(1);
  gPad->SetGridx();
  mg_k->SetTitle(Form("SSD4_%s_k_distributions",L1STag.c_str()));
  mg_k->GetXaxis()->SetRangeUser(0.5,16.5);
  mg_k->GetXaxis()->SetNdivisions(117);
  mg_k->GetXaxis()->SetTitle("Channel");
  mg_k->GetXaxis()->CenterTitle(1);
  mg_k->GetXaxis()->SetTitleSize(0.05);
  mg_k->GetYaxis()->SetRangeUser(0.015,0.025);
  mg_k->GetYaxis()->SetNdivisions(510);

  TLegend* legend_k = new TLegend(0.55,0.73,0.88,0.88);
  legend_k->AddEntry(graph_k_05_08,Form("k_%s",AlphaCali05_08.c_str()),"lp");
  legend_k->AddEntry(graph_k_00_04,Form("k_%s",AlphaCali00_04.c_str()),"lp");

  mg_k->Draw("APL");
  legend_k->Draw();


  // h distributions
  cans_SSD4->cd(2);
  gPad->SetGridx();
  mg_h->SetTitle(Form("SSD4_%s_h_distributions",L1STag.c_str()));
  mg_h->GetXaxis()->SetRangeUser(0.5,16.5);
  mg_h->GetXaxis()->SetNdivisions(117);
  mg_h->GetXaxis()->SetTitle("Channel");
  mg_h->GetXaxis()->CenterTitle(1);
  mg_h->GetXaxis()->SetTitleSize(0.05);
  mg_h->GetYaxis()->SetRangeUser(-2.5,-1.);
  mg_h->GetYaxis()->SetNdivisions(1003);

  TLegend* legend_h = new TLegend(0.55,0.73,0.88,0.88);
  legend_h->AddEntry(graph_h_05_08,Form("h_%s",AlphaCali05_08.c_str()),"lp");
  legend_h->AddEntry(graph_h_00_04,Form("h_%s",AlphaCali00_04.c_str()),"lp");

  mg_h->Draw("APL");
  legend_h->Draw();


  // err distributions
  cans_SSD4->cd(3);
  gPad->SetGridx();
  mg_err->SetTitle(Form("SSD4_%s_err_distributions",L1STag.c_str()));
  mg_err->GetXaxis()->SetRangeUser(0.5,16.5);
  mg_err->GetXaxis()->SetNdivisions(117);
  mg_err->GetXaxis()->SetTitle("Channel");
  mg_err->GetXaxis()->CenterTitle(1);
  mg_err->GetXaxis()->SetTitleSize(0.05);
  mg_err->GetYaxis()->SetRangeUser(0.,8.);
  mg_err->GetYaxis()->SetNdivisions(1008);
  mg_err->GetYaxis()->SetTitle("RelativeErr (%)");
  mg_err->GetYaxis()->CenterTitle(1);
  mg_err->GetYaxis()->SetTitleSize(0.05);

  TLegend* legend_err = new TLegend(0.65,0.73,0.88,0.88);
  legend_err->AddEntry(graph_k_RelativeErr,"k_Err","lp");
  legend_err->AddEntry(graph_h_RelativeErr,"h_Err","lp");

  TLatex* latex = new TLatex(6.,6.,"a_Err = b_Err"); // 这里 a=k, b=h, 为了统一
  latex->SetTextColor(kMagenta);

  mg_err->Draw("APL");
  legend_err->Draw();
  latex->Draw();

  cans_SSD4->Print(pathPNGOutput.c_str());
}
