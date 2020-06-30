////////////////////////////////////////////////////////////////////////////////
//
//  1. 能量刻度的准确性主要由三大因素决定:
//            (1) 脉冲刻度的线性 (2)alpha 能量的准确性 (3)alpha 拟合道址的准确性
//  2. 从结果看, 脉冲刻度的线性度很好. 因此，现在来考察alpha拟合道址的准确性对刻度的影响!
//
//  3.考察思路: 固定因素(1)(2)的不变, 对 alpha 拟合道址增加一个误差，如 +1ch，-1ch,
//    以此考察刻度曲线的变化
//
//  gfh, 2020-06-23
////////////////////////////////////////////////////////////////////////////////

# include "../../include/ReadFileModule.h"


Int_t SSDNum = 4;
Int_t CHNum  = 16;


//________________________________________________________
void EstimateAlphaCHEffectsOnFitPars(const char* layertag, Double_t Ch_Err);


//_________________________________________________
void SiEnergyCali_EstimateAlphaCHEffectsOnFitPars()
{
  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");

  Double_t Ch_Err_L1S = 0.5; // 对拟合的 alpha 峰的道址给一个误差
  Double_t Ch_Err_L2F = 0.3; // 对拟合的 alpha 峰的道址给一个误差
  Double_t Ch_Err_L2B = 0.3; // 对拟合的 alpha 峰的道址给一个误差

  EstimateAlphaCHEffectsOnFitPars(L1STag.c_str(),Ch_Err_L1S);
  EstimateAlphaCHEffectsOnFitPars(L2FTag.c_str(),Ch_Err_L2F);
  EstimateAlphaCHEffectsOnFitPars(L2BTag.c_str(),Ch_Err_L2B);
}


//________________________________________________________
void EstimateAlphaCHEffectsOnFitPars(const char* layertag, Double_t Ch_Err)
{
  std::string PulserTag("Height");   // 使用 "Height" 方式的脉冲刻度
  std::string AlphaCali00_08("AlphaCali00_08");
  std::string AlphaCali05_08("AlphaCali05_08");
  std::string AlphaCali00_48("AlphaCali00_48");

  std::string pathPDFOutput(Form("figures/SSD_%s_AlphaCHEffectsOnFitPar.pdf",layertag));
  std::string pathPDFBegin(Form("figures/SSD_%s_AlphaCHEffectsOnFitPar.pdf[",layertag));
  std::string pathPDFEnd(Form("figures/SSD_%s_AlphaCHEffectsOnFitPar.pdf]",  layertag));

  std::string pathPulserIn(Form("output/SSD_%s_PulserCali_%s.dat",layertag,PulserTag.c_str()));
  std::string pathAlphaEnergiesIn("output/SSD_AlphaEnergies.dat");
  std::string pathPulserInGain20;
  std::string pathAlphaCali00_08;
  std::string pathAlphaCali05_08;
  std::string pathAlphaCali00_48;

  Int_t numpar_PulserIn       = 3;  // a, err_a, b
  Int_t numpar_PulserInGain20 = 3;  // a, err_a, b
  Int_t numpar_AlphaCali00_08 = 3;  // peak1, peak2, peak3
  Int_t numpar_AlphaCali05_08 = 3;  // peak1, peak2, peak3
  Int_t numpar_AlphaCali00_48 = 3;  // peak1, peak2, peak3
  Int_t numpar_AlphaEnergies  = 3;  // E1, E2, E3

  ReadFileModule readfile;
  Double_t*** PulserIn = readfile.ReadData(pathPulserIn.c_str(),SSDNum,CHNum,numpar_PulserIn);
  Double_t*** AlphaEIn = readfile.ReadData(pathAlphaEnergiesIn.c_str(),SSDNum,CHNum,numpar_AlphaEnergies);
  Double_t*** PulserInGain20;
  Double_t*** AlphaCHIn00_08;
  Double_t*** AlphaCHIn05_08;
  Double_t*** AlphaCHIn00_48;

  if (strcmp(layertag,"L1S")==0) {
    pathPulserInGain20 = Form("output/SSD_%s_PulserReCali_Gain20_%s.dat",layertag,PulserTag.c_str());
    pathAlphaCali00_08 = Form("output/SSD_%s_AlphaPeaks_%s.dat",layertag,AlphaCali00_08.c_str());
    pathAlphaCali05_08 = Form("output/SSD_%s_AlphaPeaks_%s.dat",layertag,AlphaCali05_08.c_str());
    PulserInGain20 = readfile.ReadData(pathPulserInGain20.c_str(), SSDNum, CHNum, numpar_PulserInGain20);
    AlphaCHIn00_08 = readfile.ReadData(pathAlphaCali00_08.c_str(), SSDNum, CHNum, numpar_AlphaCali00_08);
    AlphaCHIn05_08 = readfile.ReadData(pathAlphaCali05_08.c_str(), SSDNum, CHNum, numpar_AlphaCali05_08);

  } else if (strcmp(layertag,"L2F")==0 || strcmp(layertag,"L2B")==0) {
    pathAlphaCali00_48 = Form("output/SSD_%s_AlphaPeaks_%s.dat",layertag,AlphaCali00_48.c_str());
    AlphaCHIn00_48 = readfile.ReadData(pathAlphaCali00_48.c_str(), SSDNum, CHNum, numpar_AlphaCali00_48);
  }


  // 计算比例系数, 并进一步得到能量刻度曲线的洗漱
  Double_t k1[SSDNum][CHNum];
  Double_t h1[SSDNum][CHNum];
  Double_t c1[SSDNum][CHNum];
  Double_t k1_Minus [SSDNum][CHNum];
  Double_t h1_Minus [SSDNum][CHNum];
  Double_t k1_Plus  [SSDNum][CHNum];
  Double_t h1_Plus  [SSDNum][CHNum];
  Double_t k1_2Minus[SSDNum][CHNum];
  Double_t h1_2Minus[SSDNum][CHNum];
  Double_t k1_2Plus [SSDNum][CHNum];
  Double_t h1_2Plus [SSDNum][CHNum];
  Double_t c1_Minus [SSDNum][CHNum];
  Double_t c1_Plus  [SSDNum][CHNum];
  Double_t c1_2Minus[SSDNum][CHNum];
  Double_t c1_2Plus [SSDNum][CHNum];
  Double_t SSDCHNum [SSDNum][CHNum];
  Double_t RelativeErr_Minus [SSDNum][CHNum];
  Double_t RelativeErr_Plus  [SSDNum][CHNum];
  Double_t RelativeErr_2Minus[SSDNum][CHNum];
  Double_t RelativeErr_2Plus [SSDNum][CHNum];

  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      SSDCHNum[i][j] = j+1;

      Double_t a_gain20;
      Double_t b_gain20;
      Double_t a      = PulserIn[i][j][0];
      Double_t b      = PulserIn[i][j][2];
      Double_t E1_MeV = AlphaEIn[i][j][0];  // energy of peak1

      if (strcmp(layertag,"L1S")==0) {  // 对与 L1S
        a_gain20 = PulserInGain20[i][j][0];
        b_gain20 = PulserInGain20[i][j][2];

        Double_t E1_CH_00_08  = AlphaCHIn00_08[i][j][0];
        Double_t E1_CH_05_08  = AlphaCHIn05_08[i][j][0];

        switch(i) {
          case 0:
          case 1:   // SSD1_L1S, SSD2_L1S 改变了 gain, SSD1，SSD2,SSD3 使用 AlphaCali00_08
              c1[i][j]        = E1_MeV / (a_gain20 *  E1_CH_00_08 + b_gain20);
              c1_Minus [i][j] = E1_MeV / (a_gain20 * (E1_CH_00_08 - Ch_Err)   + b_gain20);
              c1_Plus  [i][j] = E1_MeV / (a_gain20 * (E1_CH_00_08 + Ch_Err)   + b_gain20);
              c1_2Minus[i][j] = E1_MeV / (a_gain20 * (E1_CH_00_08 - 2*Ch_Err) + b_gain20);
              c1_2Plus [i][j] = E1_MeV / (a_gain20 * (E1_CH_00_08 + 2*Ch_Err) + b_gain20);
              break;
          case 2: // SSD1，SSD2,SSD3 使用 AlphaCali00_08
              c1[i][j]        = E1_MeV / (a *  E1_CH_00_08 + b);
              c1_Minus [i][j] = E1_MeV / (a * (E1_CH_00_08 - Ch_Err)   + b);
              c1_Plus  [i][j] = E1_MeV / (a * (E1_CH_00_08 + Ch_Err)   + b);
              c1_2Minus[i][j] = E1_MeV / (a * (E1_CH_00_08 - 2*Ch_Err) + b);
              c1_2Plus [i][j] = E1_MeV / (a * (E1_CH_00_08 + 2*Ch_Err) + b);
              break;
          case 3: // SSD4 使用 AlphaCali05_08
              c1[i][j]        = E1_MeV / (a *  E1_CH_05_08 + b);
              c1_Minus [i][j] = E1_MeV / (a * (E1_CH_05_08 - Ch_Err)   + b);
              c1_Plus  [i][j] = E1_MeV / (a * (E1_CH_05_08 + Ch_Err)   + b);
              c1_2Minus[i][j] = E1_MeV / (a * (E1_CH_05_08 - 2*Ch_Err) + b);
              c1_2Plus [i][j] = E1_MeV / (a * (E1_CH_05_08 + 2*Ch_Err) + b);
              break;
          default:
              cout<<Form("SSD%d_%s_%02d invalid!",i+1,layertag,j);
        }

      } else if (strcmp(layertag,"L2F")==0 || strcmp(layertag,"L2B")==0) {
        Double_t E1_CH_00_48  = AlphaCHIn00_48[i][j][0];
        c1[i][j]        = E1_MeV / (a *  E1_CH_00_48 + b);
        c1_Minus [i][j] = E1_MeV / (a * (E1_CH_00_48 - Ch_Err)   + b);
        c1_Plus  [i][j] = E1_MeV / (a * (E1_CH_00_48 + Ch_Err)   + b);
        c1_2Minus[i][j] = E1_MeV / (a * (E1_CH_00_48 - 2*Ch_Err) + b);
        c1_2Plus [i][j] = E1_MeV / (a * (E1_CH_00_48 + 2*Ch_Err) + b);
      }

      k1[i][j] = c1[i][j] * a;
      h1[i][j] = c1[i][j] * b;  // E1_MeV = k1 * E1_CH + h1
      k1_Minus [i][j] = c1_Minus [i][j] * a;
      h1_Minus [i][j] = c1_Minus [i][j] * b;
      k1_Plus  [i][j] = c1_Plus  [i][j] * a;
      h1_Plus  [i][j] = c1_Plus  [i][j] * b;
      k1_2Minus[i][j] = c1_2Minus[i][j] * a;
      h1_2Minus[i][j] = c1_2Minus[i][j] * b;
      k1_2Plus [i][j] = c1_2Plus [i][j] * a;
      h1_2Plus [i][j] = c1_2Plus [i][j] * b;

      RelativeErr_Minus[i][j]  = 100*(k1_Minus[i][j] -k1[i][j])/k1[i][j];
      RelativeErr_Plus[i][j]   = 100*(k1_Plus[i][j]  -k1[i][j])/k1[i][j];
      RelativeErr_2Minus[i][j] = 100*(k1_2Minus[i][j]-k1[i][j])/k1[i][j];
      RelativeErr_2Plus[i][j]  = 100*(k1_2Plus[i][j] -k1[i][j])/k1[i][j];

      cout<<setw(5)<<i<<setw(5)<<j<<setw(15)<<k1[i][j]<<setw(15)<<h1[i][j]<<endl;
    }
  }

  TF1* fPeak1[SSDNum][CHNum];
  TF1* fPeak1_Minus [SSDNum][CHNum];
  TF1* fPeak1_Plus  [SSDNum][CHNum];
  TF1* fPeak1_2Minus[SSDNum][CHNum];
  TF1* fPeak1_2Plus [SSDNum][CHNum];

  TLegend* legend_MinusPlus [SSDNum][CHNum];
  TLegend* legend_2MinusPlus[SSDNum][CHNum];

  TLatex* latex_Minus [SSDNum][CHNum];
  TLatex* latex_Plus  [SSDNum][CHNum];
  TLatex* latex_2Minus[SSDNum][CHNum];
  TLatex* latex_2Plus [SSDNum][CHNum];

  TCanvas* cans = new TCanvas("cans","cans",1200,800);
  TCanvas* cans_begin = new TCanvas();
  TCanvas* cans_end   = new TCanvas();
  cans->Divide(2,1);
  cans_begin->Close();
  cans_end  ->Close();
  cans_begin->Print(pathPDFBegin.c_str());

  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      fPeak1[i][j] = new TF1(Form("fPeak1_SSD%d_CH%02d",i+1,j),Form("%.5f*x+%.5f",k1[i][j],h1[i][j]),0,4096);
      fPeak1_Minus [i][j] = new TF1(Form("fPeak1Minus_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k1_Minus[i][j], h1_Minus[i][j]), 0,4096);
      fPeak1_Plus  [i][j] = new TF1(Form("fPeak1Plus_SSD%d_CH%d",i+1,j), Form("%.5f*x+%.5f",k1_Plus[i][j],  h1_Plus[i][j]),  0,4096);
      fPeak1_2Minus[i][j] = new TF1(Form("fPeak1Minus_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k1_2Minus[i][j],h1_2Minus[i][j]),0,4096);
      fPeak1_2Plus [i][j] = new TF1(Form("fPeak1Plus_SSD%d_CH%d",i+1,j), Form("%.5f*x+%.5f",k1_2Plus[i][j], h1_2Plus[i][j]), 0,4096);

      fPeak1[i][j]->SetTitle(Form("SSD%d_%s_CH%02d_AlphaCHEffectsOnFitPars",i+1,layertag,j));

      fPeak1[i][j]->SetLineColor(kRed);
      fPeak1_Minus [i][j]->SetLineColor(kBlue);
      fPeak1_Plus  [i][j]->SetLineColor(kGreen);
      fPeak1_2Minus[i][j]->SetLineColor(kBlue);
      fPeak1_2Plus [i][j]->SetLineColor(kGreen);

      legend_MinusPlus [i][j] = new TLegend(0.15,0.7,0.60,0.88);
      legend_2MinusPlus[i][j] = new TLegend(0.15,0.7,0.60,0.88);
      legend_MinusPlus [i][j]->AddEntry(fPeak1[i][j],"Pulser+AlphaPeak1","l");
      legend_MinusPlus [i][j]->AddEntry(fPeak1_Minus[i][j],Form("E_{CH}(#alpha1) - %.1f",Ch_Err),"l");
      legend_MinusPlus [i][j]->AddEntry(fPeak1_Plus[i][j], Form("E_{CH}(#alpha1) + %.1f",Ch_Err),"l");

      legend_2MinusPlus[i][j]->AddEntry(fPeak1[i][j],"Pulser+AlphaPeak1","l");
      legend_2MinusPlus[i][j]->AddEntry(fPeak1_2Minus[i][j],Form("E_{CH}(#alpha1) - %.1f",2*Ch_Err),"l");
      legend_2MinusPlus[i][j]->AddEntry(fPeak1_2Plus[i][j], Form("E_{CH}(#alpha1) + %.1f",2*Ch_Err),"l");

      latex_Minus [i][j] = new TLatex(1000.,0.15*fPeak1[i][j]->GetMaximum(),
                                      Form("a_Err(-%.1f) = %.3f(%%)",Ch_Err,RelativeErr_Minus[i][j]));
      latex_Plus  [i][j] = new TLatex(1000.,0.05*fPeak1[i][j]->GetMaximum(),
                                      Form("a_Err(+%.1f) = %.3f(%%)",Ch_Err,RelativeErr_Plus[i][j]));
      latex_2Minus[i][j] = new TLatex(1000.,0.15*fPeak1[i][j]->GetMaximum(),
                                      Form("a_Err(-%.1f) = %.3f(%%)",2*Ch_Err,RelativeErr_2Minus[i][j]));
      latex_2Plus [i][j] = new TLatex(1000.,0.05*fPeak1[i][j]->GetMaximum(),
                                      Form("a_Err(+%.1f) = %.3f(%%)",2*Ch_Err,RelativeErr_2Plus[i][j]));
      latex_Minus [i][j]->SetTextColor(kMagenta);
      latex_Plus  [i][j]->SetTextColor(kMagenta);
      latex_2Minus[i][j]->SetTextColor(kMagenta);
      latex_2Plus [i][j]->SetTextColor(kMagenta);


      // 画出-0.5,+0.5 CH 的效果图
      cans->cd(1);
      gPad->SetGridx();
      gPad->SetGridy();
      fPeak1[i][j]->Draw();
      fPeak1_Minus[i][j]->Draw("SAME");
      fPeak1_Plus [i][j]->Draw("SAME");
      latex_Minus [i][j]->Draw("SAME");
      latex_Plus  [i][j]->Draw("SAME");
      legend_MinusPlus[i][j]->Draw("SAME");


      // 画出-1.0,+1.0 CH 的效果图
      cans->cd(2);
      gPad->SetGridx();
      gPad->SetGridy();
      fPeak1[i][j]->Draw();
      fPeak1_2Minus[i][j]->Draw("SAME");
      fPeak1_2Plus [i][j]->Draw("SAME");
      latex_2Minus [i][j]->Draw("SAME");
      latex_2Plus  [i][j]->Draw("SAME");
      legend_2MinusPlus[i][j]->Draw("SAME");

      gPad->Modified();
      gPad->Update();
      //getchar();

      cans->Print(pathPDFOutput.c_str());
    }
  }
  cans_end->Print(pathPDFEnd.c_str());

}
