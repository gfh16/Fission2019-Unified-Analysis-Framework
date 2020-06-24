////////////////////////////////////////////////////////////////////////////////
//
//   确定硅条能量刻度的参数! 暂定两套参数:
//   刻度参数1. peak1 与 peak2 拟合参数的平均: peak1 与 peak2 的参数稍微有些差异,可能是
//            由 ADC 在较低 channel 处的非线性引起的
//   刻度参数2.考虑死层修正后, 使得 peak1 与 peak2 刻度参数误小于 0.5%, 再取平均
//
//   现在需要输出两套参数，同时画图检查刻度曲线的差异
//
//   gfh, 2020-06-22
////////////////////////////////////////////////////////////////////////////////

# include "../../include/ReadFileModule.h"


Int_t SSDNum = 4;
Int_t CHNum  = 16;

Double_t RelativeErrCut = 0.005;

//______________________________________________________________________
void FinallyDeterminedFitPars(const char* layertag);


//______________________________________
void SiEnergyCali_FinallyDeterminedFitPars()
{
  std::string Average("Average");
  std::string DeadLayer("AverageDeadLayerEffect");

  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");

  FinallyDeterminedFitPars(L1STag.c_str());
  FinallyDeterminedFitPars(L2FTag.c_str());
  FinallyDeterminedFitPars(L2BTag.c_str());
}



//__________________________________________
void FinallyDeterminedFitPars(const char* layertag)
{
  std::string pathHeightAndAlphaFitParsIn(Form("output/SSD_%s_HeightAndAlphaFitPars.dat",layertag));
  std::string pathDeadLayerFitParsIn(Form("output/SSD_%s_DeadLayerFitPars.dat",layertag));

  std::string AlphaCali00_08("AlphaCali00_08");
  std::string AlphaCali05_08("AlphaCali05_08");
  std::string AlphaCali00_48("AlphaCali00_48");
  std::string pathAlphaPeaks00_08;    // for SSD1_L1S,SSD2_L1S,SSD3_L1S
  std::string pathAlphaPeaks05_08;   // for SSD4_L1S
  std::string pathAlphaPeaks00_48;   // for L2F, L2B

  std::string pathPDFOutput(Form("figures/SSD_%s_CompareWithDeadLayerFitFunction.pdf",layertag));
  std::string pathPDFOutputBegin(Form("figures/SSD_%s_CompareWithDeadLayerFitFunction.pdf[",layertag));
  std::string pathPDFOutputEnd(Form("figures/SSD_%s_CompareWithDeadLayerFitFunction.pdf]",layertag));

  std::string pathSiEnergyCaliParsOutput(Form("output/SSD_%s_SiEnergyCaliParameters.dat",layertag));

  ofstream SiEnergyCaliPars(pathSiEnergyCaliParsOutput.c_str());
  SiEnergyCaliPars<<"* Silicon energy calibration parameters. Two sets of parameters: \n";
  SiEnergyCaliPars<<"* 1.Just the average of the peak1 and the peak2 parameters \n";
  SiEnergyCaliPars<<"* 2.Consider the dead layer effect, then the get the average of the peak1 and the peak2 parameters \n";
  SiEnergyCaliPars<<"* Fit function: E_MeV = k * E_CH + h, then E_CH = 1/k * E_MeV - h/k \n";
  SiEnergyCaliPars<<"* SSDNum"<<setw(7)<<"CHNum"<<setw(15)<<"k_average"<<setw(20)<<"h_average"<<setw(20)
                  <<"k_deadlayer"<<setw(20)<<"h_deadlayer\n";


  Int_t numpar_HeightAndAlphaFitPars = 4; // k1,h1,k2,h2
  Int_t numpar_DeadLayerFitPars      = 6; // k1,h1,k2,h2,<deadlayer>,deadlayer
  Int_t numpar_AlphapPeaks00_08      = 2; // peak1, peak2
  Int_t numpar_AlphapPeaks05_08      = 2; // peak1, peak2
  Int_t numpar_AlphapPeaks00_48      = 2; // peak1, peak2

  ReadFileModule readfile;
  Double_t*** HeightAndAlphaFitPars = readfile.ReadData(pathHeightAndAlphaFitParsIn.c_str(),
                                          SSDNum,CHNum,numpar_HeightAndAlphaFitPars);
  Double_t*** DeadLayerFitPars = readfile.ReadData(pathDeadLayerFitParsIn.c_str(),
                                          SSDNum,CHNum,numpar_DeadLayerFitPars);
  Double_t*** AlphaPeaks00_08;
  Double_t*** AlphaPeaks05_08;
  Double_t*** AlphaPeaks00_48;
  if (strcmp(layertag,"L1S")==0) {  // 用于提取 peak1,peak2 的 ADC 道址， 帮助判断会否落入ADC非线性区域
    pathAlphaPeaks00_08 = Form("output/SSD_%s_AlphaPeaks_%s.dat",layertag,AlphaCali00_08.c_str());
    pathAlphaPeaks05_08 = Form("output/SSD_%s_AlphaPeaks_%s.dat",layertag,AlphaCali05_08.c_str());
    AlphaPeaks00_08     = readfile.ReadData(pathAlphaPeaks00_08.c_str(),SSDNum,CHNum,numpar_AlphapPeaks00_08);
    AlphaPeaks05_08     = readfile.ReadData(pathAlphaPeaks05_08.c_str(),SSDNum,CHNum,numpar_AlphapPeaks05_08);
  } else {
    pathAlphaPeaks00_48 = Form("output/SSD_%s_AlphaPeaks_%s.dat",layertag,AlphaCali00_48.c_str());
    AlphaPeaks00_48     = readfile.ReadData(pathAlphaPeaks00_48.c_str(),SSDNum,CHNum,numpar_AlphapPeaks00_48);
  }

  Double_t k1[SSDNum][CHNum];
  Double_t h1[SSDNum][CHNum];   // E1_MeV = k1 * E1_CH + h1
  Double_t k2[SSDNum][CHNum];
  Double_t h2[SSDNum][CHNum];   // E2_MeV = k2 * E2_CH + h2
  Double_t k_ave[SSDNum][CHNum];
  Double_t h_ave[SSDNum][CHNum];
  Double_t k1_deadlayer[SSDNum][CHNum];
  Double_t h1_deadlayer[SSDNum][CHNum];
  Double_t k2_deadlayer[SSDNum][CHNum];
  Double_t h2_deadlayer[SSDNum][CHNum];
  Double_t k_deadlayer_ave[SSDNum][CHNum];
  Double_t h_deadlayer_ave[SSDNum][CHNum];
  Double_t deadlayer[SSDNum][CHNum];

  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      k1[i][j] = HeightAndAlphaFitPars[i][j][0];
      h1[i][j] = HeightAndAlphaFitPars[i][j][1];
      k2[i][j] = HeightAndAlphaFitPars[i][j][2];
      h2[i][j] = HeightAndAlphaFitPars[i][j][3];

      k_ave[i][j] = (k1[i][j]+k2[i][j])/2;
      h_ave[i][j] = (h1[i][j]+h2[i][j])/2;

      k1_deadlayer[i][j] = DeadLayerFitPars[i][j][0];
      h1_deadlayer[i][j] = DeadLayerFitPars[i][j][1];
      k2_deadlayer[i][j] = DeadLayerFitPars[i][j][2];
      h2_deadlayer[i][j] = DeadLayerFitPars[i][j][3];

      k_deadlayer_ave[i][j] = (k1_deadlayer[i][j]+k2_deadlayer[i][j])/2;
      h_deadlayer_ave[i][j] = (h1_deadlayer[i][j]+h2_deadlayer[i][j])/2;

      deadlayer[i][j] = DeadLayerFitPars[i][j][5];

      SiEnergyCaliPars<<setw(5)<<i<<setw(7)<<j<<setw(15)<<k_ave[i][j]<<setw(20)<<h_ave[i][j]<<setw(20)
                      <<k_deadlayer_ave[i][j]<<setw(20)<<h_deadlayer_ave[i][j]<<endl;
    }
  }

  TF1* fPeak1[SSDNum][CHNum];
  TF1* fPeak2[SSDNum][CHNum];
  TF1* fPeak1_deadlayer[SSDNum][CHNum];
  TF1* fPeak2_deadlayer[SSDNum][CHNum];
  TF1* fAverage[SSDNum][CHNum];
  TF1* fAverage_DeadLayer[SSDNum][CHNum];

  TLegend* legend_all[SSDNum][CHNum];
  TLegend* legend_peak1peak2[SSDNum][CHNum];
  TLegend* legend_peak1peak2_deadlayer[SSDNum][CHNum];
  TLatex*  latex[SSDNum][CHNum];
  TLatex*  latex0[SSDNum][CHNum];
  TLatex*  latex_peak1[SSDNum][CHNum];
  TLatex*  latex_peak2[SSDNum][CHNum];
  TLatex*  latex_k_Err[SSDNum][CHNum];

  TCanvas* cans = new TCanvas("cans","cans",1800,800);
  TCanvas* cans_begin = new TCanvas();
  TCanvas* cans_end   = new TCanvas();
  cans->Divide(3,1);
  cans_begin->Close();
  cans_end->Close();
  cans_begin->Print(pathPDFOutputBegin.c_str());

  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      fPeak1[i][j] = new TF1(Form("fPeak1_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k1[i][j],h1[i][j]),0,4096);
      fPeak2[i][j] = new TF1(Form("fPeak2_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k2[i][j],h2[i][j]),0,4096);
      fPeak1_deadlayer[i][j] = new TF1(Form("fPeak1_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k1_deadlayer[i][j],h1_deadlayer[i][j]),0,4096);
      fPeak2_deadlayer[i][j] = new TF1(Form("fPeak2_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k2_deadlayer[i][j],h2_deadlayer[i][j]),0,4096);
      fAverage[i][j] = new TF1(Form("fAverage_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k_ave[i][j],h_ave[i][j]),0,4096);
      fAverage_DeadLayer[i][j] = new TF1(Form("fAverage_DeadLayer_SSD%d_CH%d",i+1,j),
                                         Form("%.5f*x+%.5f",k_deadlayer_ave[i][j],h_deadlayer_ave[i][j]),0,4096);

      fPeak1[i][j]->SetTitle(Form("SSD%d_%s_CH%02d_FitFunctions",i+1,layertag,j));
      fPeak1[i][j]->GetXaxis()->SetTitle("ADC CH");
      fPeak1[i][j]->GetXaxis()->CenterTitle(1);
      fPeak1[i][j]->GetYaxis()->SetTitle("Energy (MeV)");
      fPeak1[i][j]->GetYaxis()->CenterTitle(1);

      fPeak1_deadlayer[i][j]->SetTitle(Form("SSD%d_%s_CH%02d_FitFunctions",i+1,layertag,j));
      fPeak1_deadlayer[i][j]->GetXaxis()->SetTitle("ADC CH");
      fPeak1_deadlayer[i][j]->GetXaxis()->CenterTitle(1);
      fPeak1_deadlayer[i][j]->GetYaxis()->SetTitle("Energy (MeV)");
      fPeak1_deadlayer[i][j]->GetYaxis()->CenterTitle(1);

      fPeak1[i][j]->SetLineColor(kGreen);
      fPeak2[i][j]->SetLineColor(kMagenta);
      fPeak1_deadlayer[i][j]->SetLineColor(kYellow);
      fPeak2_deadlayer[i][j]->SetLineColor(kCyan);
      fAverage[i][j]->SetLineColor(kBlue);
      fAverage_DeadLayer[i][j]->SetLineColor(kRed);

      legend_all[i][j] = new TLegend(0.15,0.65,0.65,0.88);
      legend_all[i][j]->AddEntry(fPeak1[i][j],"Pulser+#alpha_{1}","l");
      legend_all[i][j]->AddEntry(fPeak2[i][j],"Pulser+#alpha_{2}","l");
      legend_all[i][j]->AddEntry(fAverage[i][j],"<#alpha_{1}+#alpha_{2}>","l");
      legend_all[i][j]->AddEntry(fAverage_DeadLayer[i][j],"<#alpha_{1}+#alpha_{2}>(deadlayer)","l");

      legend_peak1peak2[i][j] = new TLegend(0.15,0.75,0.55,0.88);
      legend_peak1peak2[i][j]->AddEntry(fPeak1[i][j],"Pulser+#alpha_{1}","l");
      legend_peak1peak2[i][j]->AddEntry(fPeak2[i][j],"Pulser+#alpha_{2}","l");

      legend_peak1peak2_deadlayer[i][j] = new TLegend(0.15,0.75,0.65,0.88);
      legend_peak1peak2_deadlayer[i][j]->AddEntry(fPeak1_deadlayer[i][j],"Pulser+#alpha_{1}(deadlayer)","l");
      legend_peak1peak2_deadlayer[i][j]->AddEntry(fPeak2_deadlayer[i][j],"Pulser+#alpha_{2}(deadlayer","l");

      latex[i][j]  = new TLatex(1500,0.2*fPeak1[i][j]->GetMaximum(),
                                Form("deadlayer=%.2f(#mum)",deadlayer[i][j]));
      latex0[i][j] = new TLatex(1500,0.2*fPeak1[i][j]->GetMaximum(),
                                Form("deadlayer=%.2f(#mum)",0.));
      latex [i][j]->SetTextColor(kMagenta);
      latex0[i][j]->SetTextColor(kMagenta);


      latex_k_Err[i][j] = new TLatex(1500,0.05*fPeak1[i][j]->GetMaximum(),
                                     Form("k_Err < %.2f(%%)",100*RelativeErrCut));
      latex_k_Err[i][j]->SetTextColor(kMagenta);

      if (strcmp(layertag,"L1S")==0 && i<3) { // SSD1_L1S,SSD2_L1S,SSD3_L1S, 使用 AlphaCali00_08
        latex_peak1[i][j] = new TLatex(1500,0.12*fPeak1[i][j]->GetMaximum(),
                                       Form("CH(#alpha_{1})=%.2f",AlphaPeaks00_08[i][j][0]));
        latex_peak2[i][j] = new TLatex(1500,0.05*fPeak1[i][j]->GetMaximum(),
                                       Form("CH(#alpha_{2})=%.2f",AlphaPeaks00_08[i][j][1]));
      } else if (strcmp(layertag,"L1S")==0 && i==3) { // SSD4_L1S, 使用 AlphaCali05_08
        latex_peak1[i][j] = new TLatex(1500,0.12*fPeak1[i][j]->GetMaximum(),
                                       Form("CH(#alpha_{1})=%.2f",AlphaPeaks05_08[i][j][0]));
        latex_peak2[i][j] = new TLatex(1500,0.05*fPeak1[i][j]->GetMaximum(),
                                       Form("CH(#alpha_{2})=%.2f",AlphaPeaks05_08[i][j][1]));
      } else {
        latex_peak1[i][j] = new TLatex(1500,0.12*fPeak1[i][j]->GetMaximum(),
                                       Form("CH(#alpha_{1})=%.2f",AlphaPeaks00_48[i][j][0]));
        latex_peak2[i][j] = new TLatex(1500,0.05*fPeak1[i][j]->GetMaximum(),
                                       Form("CH(#alpha_{2})=%.2f",AlphaPeaks00_48[i][j][1]));
      }
      latex_peak1[i][j]->SetTextColor(kMagenta);
      latex_peak2[i][j]->SetTextColor(kMagenta);


      // peak1 与 peak2 刻度曲线
      cans->cd(1);
      gPad->SetGridx();
      gPad->SetGridy();
      gPad->SetLeftMargin(0.12);
      fPeak1[i][j]->Draw();
      fPeak2[i][j]->Draw("SAME");
      legend_peak1peak2[i][j]->Draw("SAME");
      latex0[i][j]->Draw("SAME");


      // peak1 与 peak2 死层修正后的刻度曲线
      cans->cd(2);
      gPad->SetGridx();
      gPad->SetGridy();
      gPad->SetLeftMargin(0.12);
      fPeak1_deadlayer[i][j]->Draw();
      fPeak2_deadlayer[i][j]->Draw("SAME");
      legend_peak1peak2_deadlayer[i][j]->Draw("SAME");
      latex[i][j]->Draw("SAME");
      latex_k_Err[i][j]->Draw("SAME");


      // peak1，peak2，<peak1+peak2>, <peak1+peak2>_deadlayer 刻度曲线
      cans->cd(3);
      gPad->SetGridx();
      gPad->SetGridy();
      gPad->SetLeftMargin(0.12);
      fPeak1[i][j]->Draw();
      fPeak2[i][j]->Draw("SAME");
      fAverage[i][j]->Draw("SAME");
      fAverage_DeadLayer[i][j]->Draw("SAME");
      legend_all[i][j]->Draw("SAME");
      latex[i][j]->Draw("SAME");
      latex_peak1[i][j]->Draw("SAME");
      latex_peak2[i][j]->Draw("SAME");

      gPad->Modified();
      gPad->Update();

      cans->Print(pathPDFOutput.c_str());
      //getchar();
    }
  }
  cans_end->Print(pathPDFOutputEnd.c_str());
}
