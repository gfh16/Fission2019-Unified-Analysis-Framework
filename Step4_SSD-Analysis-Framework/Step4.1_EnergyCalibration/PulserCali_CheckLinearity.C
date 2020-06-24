////////////////////////////////////////////////////////////////////////////////
//
//   较定量考察 pulser 脉冲刻度的线性性！
//   虽然从 pulser 刻度的图片看到很好的线性, 但为保险期间，需要对其进行定量考察.
//   思路如下: 对于每个相同的 ADC 道址, 计算每个相对脉冲幅度与刻度曲线的偏差
//
//   gfh, 2020-06-20
////////////////////////////////////////////////////////////////////////////////

#include "../../include/ReadFileModule.h"


Int_t SSDNum = 4;
Int_t CHNum  = 16;

Double_t Height[11] = {1., .9, .8, .7, .6, .5, .4, .3, .2, .1, 0.05};


//________________________________________
void CheckLinearity(const char* layertag);


//___________________________________________________
void PulserCali_CheckLinearity()
{
  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");

  CheckLinearity(L1STag.c_str());
  CheckLinearity(L2FTag.c_str());
  CheckLinearity(L2BTag.c_str());
}


//___________________________________________________
void CheckLinearity(const char* layertag)
{
  std::string PulserTag("Height");
  std::string GainTag("Gain4");

  std::string pathPulserIn(Form("output/SSD_%s_PulserCali_%s.dat",layertag,PulserTag.c_str()));

  std::string pathPDFOutput(Form("figures/SSD_%s_CheckPulserLinearity.pdf",layertag));
  std::string pathPDFOutputBegin(Form("figures/SSD_%s_CheckPulserLinearity.pdf[",layertag));
  std::string pathPDFOutputEnd(Form("figures/SSD_%s_CheckPulserLinearity.pdf]",layertag));

  Int_t numpeaks11 = 11;  // 最多是 11 个点, 但有些是 10 个点
  Int_t numpeaks10 = 10;
  Int_t numpar_PulserIn  = 15;
  Int_t numpar_ParsInput = 4; // a, err_a, b, err_b

  ReadFileModule readfile;
  Double_t*** PulserIn  = readfile.ReadData(pathPulserIn.c_str(), SSDNum, CHNum, numpar_PulserIn);

  Double_t PulserPeaks11[SSDNum][CHNum][numpeaks11];
  Double_t PulserPeaks10[SSDNum][CHNum][numpeaks10];
  Double_t PulserHeight11[SSDNum][CHNum][numpeaks11];
  Double_t PulserHeight10[SSDNum][CHNum][numpeaks10];
  Double_t CaliPeaks[SSDNum][CHNum][numpeaks11];
  Double_t RelativeErr11[SSDNum][CHNum][numpeaks11];
  Double_t RelativeErr10[SSDNum][CHNum][numpeaks10];
  Double_t Sum_RelativeErr10[SSDNum][CHNum];
  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      Sum_RelativeErr10[i][j] = 0.;

      for (Int_t p=0; p<numpeaks11; p++)
      {
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

  TF1* fit[SSDNum][CHNum];
  TLatex* latex[SSDNum][CHNum];
  TLatex* latex_average[SSDNum][CHNum];
  TLine * line_averageErr[SSDNum][CHNum];

  TGraph* graphpeaks11[SSDNum][CHNum];
  TGraph* graphpeaks10[SSDNum][CHNum];
  TGraph* graph11[SSDNum][CHNum];
  TGraph* graph10[SSDNum][CHNum];
  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      fit[i][j] = new TF1(Form("fun_SSD%d_CH%02d",i+1,j),"pol1",0.,4000.);

      graphpeaks11[i][j] = new TGraph(numpeaks11,PulserPeaks11[i][j],PulserHeight11[i][j]);
      graphpeaks10[i][j] = new TGraph(numpeaks10,PulserPeaks10[i][j],PulserHeight10[i][j]);
      graph11[i][j]    = new TGraph(numpeaks11,PulserPeaks11[i][j],RelativeErr11[i][j]);
      graph10[i][j]    = new TGraph(numpeaks10,PulserPeaks10[i][j],RelativeErr10[i][j]);


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
                              Form("<Relative_Err>_{10Peaks} = %.2f",Sum_RelativeErr10[i][j]/numpeaks10));
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
      //getchar();
    }
  }
  cans_end->Print(pathPDFOutputEnd.c_str());
}
