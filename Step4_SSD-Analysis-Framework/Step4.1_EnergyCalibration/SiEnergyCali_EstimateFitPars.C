////////////////////////////////////////////////////////////////////////////////
//
//  1.前面已经通过 Pulser(Height) 与 alpha 能量点拟合得到能量刻度曲线，发现用 peak1 与
//    peak2 两点刻度的结果吻合得比较好，peak3的结果稍微有点偏差. 需要对刻度曲线参数进行评估.
//
//  2. peak3的偏差主要是由其较低的计数引起的，因此后续的讨论中，只讨论 peak1 与 peak2 的结果.
//     评价内容包括：
//    (1) peak1 与 peak2 拟合结果的差异. 由此确定最终使用 peak1 的结果，还是采用 peak1
//        与 peak2 的平均值
//    (2) 评估每一个 SSD 每个 CH 的刻度参数分布，给出参数的平均值，评估同一个 SSD 不同 CH
//        之间的参数差异
//
//  gfh, 2020-06-17
////////////////////////////////////////////////////////////////////////////////

#include "../../include/ReadFileModule.h"

Int_t SSDNum = 4;
Int_t CHNum  = 16;


//________________________________________
void EstimateErrorOfPeak1AndPeak2(const char* layertag);
void EstimateParameterDistributions(const char* layertag);


//__________________________________________________________
void SiEnergyCali_EstimateFitPars()
{
  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");

  EstimateErrorOfPeak1AndPeak2(L1STag.c_str());
  EstimateErrorOfPeak1AndPeak2(L2FTag.c_str());
  EstimateErrorOfPeak1AndPeak2(L2BTag.c_str());

  EstimateParameterDistributions(L1STag.c_str());
  EstimateParameterDistributions(L2FTag.c_str());
  EstimateParameterDistributions(L2BTag.c_str());
}


//___________________________________________________________
void EstimateErrorOfPeak1AndPeak2(const char* layertag)
{
  std::string PulserTag("Height");
  std::string pathParsInput(Form("output/SSD_%s_%sAndAlphaFitPars.dat",layertag,PulserTag.c_str()));

  std::string pathPNGOutput(Form("figures/SSD_%s_ParErrorsOfPeak1AndPeak2.png",layertag));

  Int_t numpar_ParsInput = 6;

  ReadFileModule readfile;
  Double_t*** ParsInput = readfile.ReadData(pathParsInput.c_str(), SSDNum, CHNum, numpar_ParsInput);

  Double_t SSDCHNum[SSDNum][CHNum];
  Double_t a_RelativeErr[SSDNum][CHNum];
  Double_t b_RelativeErr[SSDNum][CHNum];
  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      SSDCHNum[i][j]=j+1;
      a_RelativeErr[i][j] =100*abs((ParsInput[i][j][2]-ParsInput[i][j][0])/ParsInput[i][j][0]); // 百分比%
      b_RelativeErr[i][j] =100*abs((ParsInput[i][j][3]-ParsInput[i][j][1])/ParsInput[i][j][1]); // 百分比%

      cout<< a_RelativeErr[i][j] - b_RelativeErr[i][j]<<endl;
    }
  }

  TGraph* graph_a_Err[SSDNum];
  TGraph* graph_b_Err[SSDNum];
  TMultiGraph* multigraph[SSDNum];

  TCanvas* cans = new TCanvas("cans","Pars Error of Peak1 and Peak2",1200,1000);
  cans->Divide(2,2);
  for (Int_t i=0; i<SSDNum; i++)
  {
    cans->cd(i+1);
    gPad->SetGridx();

    graph_a_Err[i] = new TGraph(CHNum, SSDCHNum[i], a_RelativeErr[i]);
    graph_b_Err[i] = new TGraph(CHNum, SSDCHNum[i], b_RelativeErr[i]);

    graph_a_Err[i]->SetMarkerStyle(20);
    graph_a_Err[i]->SetMarkerColor(kBlue);

    graph_b_Err[i]->SetMarkerStyle(20);
    graph_b_Err[i]->SetMarkerColor(kRed);

    multigraph[i] = new TMultiGraph();
    multigraph[i]->Add(graph_a_Err[i]);
    multigraph[i]->Add(graph_b_Err[i]);

    multigraph[i]->SetTitle(Form("SSD%d_%s_ParErrorOfPeak1AndPeak2",i+1,layertag));
    multigraph[i]->GetXaxis()->SetRangeUser(0,17);
    multigraph[i]->GetXaxis()->SetNdivisions(117);
    multigraph[i]->GetXaxis()->SetTitle("Channel");
    multigraph[i]->GetXaxis()->CenterTitle(1);
    multigraph[i]->GetXaxis()->SetTitleSize(0.04);
    multigraph[i]->GetYaxis()->SetRangeUser(0.,2.0);
    multigraph[i]->GetYaxis()->SetNdivisions(1004);
    multigraph[i]->GetYaxis()->SetTitle("Relative Errors (%)");
    multigraph[i]->GetYaxis()->CenterTitle(1);
    multigraph[i]->GetYaxis()->SetTitleSize(0.04);

    TLegend* legend = new TLegend(0.65,0.73,0.88,0.88);
    legend->AddEntry(graph_a_Err[i],"a_Err","lp");
    legend->AddEntry(graph_b_Err[i],"b_Err","lp");

    TLine* line = new TLine(1,1.0,16,1.0);
    line->SetLineColor(kGreen);
    line->SetLineStyle(7);
    line->SetLineWidth(3);

    multigraph[i]->Draw("APL");
    legend->Draw("SAME");
    line->Draw("SAME");
  }
  cans->Print(pathPNGOutput.c_str());

}


//___________________________________________________________
void EstimateParameterDistributions(const char* layertag)
{
  std::string PulserTag("Height");
  std::string pathParsInput(Form("output/SSD_%s_%sAndAlphaFitPars.dat",layertag,PulserTag.c_str()));

  std::string pathPNGOutput(Form("figures/SSD_%s_Peak1ParameterDistributions.png",layertag));

  Int_t numpar_ParsInput = 6;

  // 手动设定系数 a 与 b 的范围
  Double_t YRangeLow = -22.;
  Double_t YRangeUp  = 22.;
  Double_t YScaleRangeLow = 2.;
  Double_t YScaleRangeUp  = -2.;

  Double_t L1S_a_rangeLow[4] = { 0.14, 0.073, 0.027, 0.018};
  Double_t L1S_a_rangeUp [4] = { 0.15, 0.085, 0.033, 0.024};
  Double_t L1S_b_rangeLow[4] = {-18., -7.,   -3.4,  -3.};
  Double_t L1S_b_rangeUp [4] = {-8.,  -3.,   -1.6,  -1.};

  Double_t L2F_a_rangeLow[4] = { 0.068, 0.05,  0.028, 0.015};
  Double_t L2F_a_rangeUp [4] = { 0.072, 0.07,  0.030, 0.022};
  Double_t L2F_b_rangeLow[4] = {-7.0, -5.0, -3.0, -2.0};
  Double_t L2F_b_rangeUp [4] = {-2.5, -2.5, -1.0, -1.0};

  Double_t L2B_a_rangeLow[4] = { 0.055, 0.045, 0.028, 0.015};
  Double_t L2B_a_rangeUp [4] = { 0.065, 0.050, 0.033, 0.017};
  Double_t L2B_b_rangeLow[4] = {-8.0,  -6.0,  -3.0,  -2.5};
  Double_t L2B_b_rangeUp [4] = {-3.0,  -2.0,   0.0,  -1.0};

  Double_t a_rangeLow[SSDNum];
  Double_t a_rangeUp[SSDNum];
  Double_t b_rangeLow[SSDNum];
  Double_t b_rangeUp[SSDNum];
  for (Int_t i=0; i<SSDNum; i++)
  {
    if (strcmp(layertag,"L1S")==0) {
      a_rangeLow[i] = L1S_a_rangeLow[i];
      a_rangeUp[i]  = L1S_a_rangeUp[i];
      b_rangeLow[i] = L1S_b_rangeLow[i];
      b_rangeUp[i]  = L1S_b_rangeUp[i];
    } else if (strcmp(layertag,"L2F")==0) {
      a_rangeLow[i] = L2F_a_rangeLow[i];
      a_rangeUp[i]  = L2F_a_rangeUp[i];
      b_rangeLow[i] = L2F_b_rangeLow[i];
      b_rangeUp[i]  = L2F_b_rangeUp[i];
    } else {
      a_rangeLow[i] = L2B_a_rangeLow[i];
      a_rangeUp[i]  = L2B_a_rangeUp[i];
      b_rangeLow[i] = L2B_b_rangeLow[i];
      b_rangeUp[i]  = L2B_b_rangeUp[i];
    }
  }

  ReadFileModule readfile;
  Double_t*** ParsInput = readfile.ReadData(pathParsInput.c_str(), SSDNum, CHNum, numpar_ParsInput);

  Double_t SSDCHNum[SSDNum][CHNum];
  Double_t a[SSDNum][CHNum];  // 能量刻度曲线的斜率
  Double_t b[SSDNum][CHNum];  // 能量刻度曲线的截距
  Double_t a_average[SSDNum];
  Double_t b_average[SSDNum];
  Double_t a_sum[SSDNum];
  Double_t b_sum[SSDNum];
  Double_t a_scalefactor[SSDNum];
  Double_t b_scalefactor[SSDNum];
  Double_t a_scale[SSDNum][CHNum];
  Double_t b_scale[SSDNum][CHNum];
  Double_t a_average_scale[SSDNum];
  Double_t b_average_scale[SSDNum];
  for (Int_t i=0; i<SSDNum; i++)
  {
    a_sum[i] = 0;
    b_sum[i] = 0;

    a_scalefactor[i] = (YRangeUp-YScaleRangeLow) /(a_rangeUp[i]-a_rangeLow[i]);
    b_scalefactor[i] = (YScaleRangeUp-YRangeLow)/(b_rangeUp[i]-b_rangeLow[i]);

    for (Int_t j=0; j<CHNum; j++)
    {
      SSDCHNum[i][j]=j+1;
      a[i][j] = ParsInput[i][j][0];  // a of Peak1
      b[i][j] = ParsInput[i][j][1];  // a of Peak1

      a_scale[i][j] = (a[i][j] - a_rangeLow[i]) * a_scalefactor[i] + 2;
      b_scale[i][j] = (b[i][j] - b_rangeUp[i])  * b_scalefactor[i] - 2;

      if (i==3 && j==0) continue; // SSD4_L1S_CH00 无信号, 应剔除该道
      a_sum[i] += a[i][j];
      b_sum[i] += b[i][j];
    }
    if (i!=3) {
      a_average[i] = a_sum[i]/CHNum;
      b_average[i] = b_sum[i]/CHNum;
    } else {
      a_average[i] = a_sum[i]/15;
      b_average[i] = b_sum[i]/15;
    }
    a_average_scale[i] = (a_average[i]-a_rangeLow[i]) * a_scalefactor[i] + 2;
    b_average_scale[i] = (b_average[i]-b_rangeUp[i])  * b_scalefactor[i] - 2;

    cout<<Form("a_average[%d] = ",i+1)<< a_average[i]<<setw(20)
        <<Form("b_average[%d] = ",i+1)<< b_average[i]<<endl;
  }

  TGraph* a_scale_distribution[SSDNum];
  TGraph* b_scale_distribution[SSDNum];
  TMultiGraph* mg[SSDNum];
  TGaxis* a_Yaxis[SSDNum];
  TGaxis* b_Yaxis[SSDNum];
  TLine* line_a_average[SSDNum];
  TLine* line_b_average[SSDNum];
  TLatex* latex_a[SSDNum];
  TLatex* latex_b[SSDNum];

  TCanvas* cans = new TCanvas("cans","Pars Distributions",1200,1000);
  cans->Divide(2,2);
  for (Int_t i=0; i<SSDNum; i++)
  {
    cans->cd(i+1);
    gPad->SetGridx();

    a_scale_distribution[i] = new TGraph(CHNum, SSDCHNum[i], a_scale[i]);
    b_scale_distribution[i] = new TGraph(CHNum, SSDCHNum[i], b_scale[i]);

    auto hista = a_scale_distribution[i]->GetHistogram();
    hista->SetTitle(Form("SSD%d_%s_Peak1ParameterDistributions",i+1,layertag));
    hista->GetXaxis()->SetRangeUser(0.5,16.5);
    hista->GetXaxis()->SetNdivisions(117);
    hista->GetXaxis()->SetTitle("Channel");
    hista->GetXaxis()->CenterTitle(1);
    hista->GetXaxis()->SetTitleSize(0.04);
    hista->GetYaxis()->SetRangeUser(YRangeLow,YRangeUp);
    hista->Draw("Y+ H");
    gPad->Update();

    a_scale_distribution[i]->SetMarkerStyle(20);
    a_scale_distribution[i]->SetMarkerColor(kBlue);

    b_scale_distribution[i]->SetMarkerStyle(20);
    b_scale_distribution[i]->SetMarkerColor(kRed);

    TLine* line0 = new TLine(gPad->GetUxmin(),0.,gPad->GetUxmax(),0);
    line0->SetLineStyle(7);
    line0->SetLineColor(kBlack);

    line_a_average[i] = new TLine(gPad->GetUxmin(),a_average_scale[i],gPad->GetUxmax(),a_average_scale[i]);
    line_a_average[i]->SetLineColor(kGreen);
    line_a_average[i]->SetLineStyle(7);
    line_a_average[i]->SetLineWidth(3);
    line_b_average[i] = new TLine(gPad->GetUxmin(),b_average_scale[i],gPad->GetUxmax(),b_average_scale[i]);
    line_b_average[i]->SetLineColor(kGreen);
    line_b_average[i]->SetLineStyle(7);
    line_b_average[i]->SetLineWidth(3);

    mg[i] = new TMultiGraph();
    mg[i]->Add(a_scale_distribution[i]);
    mg[i]->Add(b_scale_distribution[i]);


    a_Yaxis[i] = new TGaxis(gPad->GetUxmin(),YScaleRangeLow,gPad->GetUxmin(),YRangeUp,
                            a_rangeLow[i],a_rangeUp[i],1002,"-R");
    b_Yaxis[i] = new TGaxis(gPad->GetUxmin(),YRangeLow,gPad->GetUxmin(),YScaleRangeUp,
                            b_rangeLow[i],b_rangeUp[i],1002,"-R");

    a_Yaxis[i]->SetLineColor(kBlue);
    a_Yaxis[i]->SetLabelColor(kBlue);
    b_Yaxis[i]->SetLineColor(kRed);
    b_Yaxis[i]->SetLabelColor(kRed);

    latex_a[i] = new TLatex(6,18.,Form("<a> = %.5f",a_average[i]));
    latex_b[i] = new TLatex(6,-4.,Form("<b> = %.5f",b_average[i]));

    mg[i]->Draw("PL");
    a_Yaxis[i]->Draw();
    b_Yaxis[i]->Draw();
    line0->Draw();
    line_a_average[i]->Draw();
    line_b_average[i]->Draw();
    latex_a[i]->Draw();
    latex_b[i]->Draw();
  }
  cans->Print(pathPNGOutput.c_str());
}
