////////////////////////////////////////////////////////////////////////////////
//
//    根据 pulser 刻度 与 alpha 能量点, 可求得硅条的能量刻度曲线 !
//
//    说明1: pulser 刻度同一使用 "Height" 刻度的结果, 因为点较多、且均匀分布
//    说明2：pulser 刻度时， SSD2_L2F_CH00,CH01 两道的道址只有其他道的一半，需要注意！
//    说明3: alpha 刻度时，
//          L1: SSD1,SSD2,SSD3 使用合并文件 L1_AlphaCali00_08.root
//              SSD4 使用合并文件 L1_AlphaCali05_08.root, 这是因为前后两次刻度有一个shift
//
//              此外, alpha 刻度时，SSD1_L1,SSD2_L1 改变了主放 gain，需要注意！
//
//
//          L2: SSD1,SSD2,SSD3,SSD4 均采用合并文件 L2_AlphaCali00_48.root
//              SSD4_L2F_CH13,CH14 两道有两组 alpha 峰, 暂使用较大一组进行刻度
//
//    说明4：三组分 alpha 源有 三个 alpha 峰， 但由于能量最大的峰计数小很多，在进行能量刻度时
//          将选取前两个 alpha 峰，分别进行刻度，再将两组刻度曲线进行比较, 最终再确定如何确定
//          能量刻度曲线
//
//   gfh, 2020-06-16
/////////////////////////////////////////////////////////////////////////////////

#include "../../include/ReadFileModule.h"


Int_t SSDNum = 4;
Int_t CHNum  = 16;


//__________________________________________________________
void SiEnergyCali_PulserAndAlpha_L1(const char* layertag);
void SiEnergyCali_PulserAndAlpha_L2(const char* layertag);


//___________________________________________________________
void SiEnergyCali_PulserAndAlpha()
{
  gStyle->SetPalette(1);

  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");

  SiEnergyCali_PulserAndAlpha_L1(L1STag.c_str());
  SiEnergyCali_PulserAndAlpha_L2(L2FTag.c_str());
  SiEnergyCali_PulserAndAlpha_L2(L2BTag.c_str());
}


//___________________________________________________________
void SiEnergyCali_PulserAndAlpha_L1(const char* layertag)
{
  std::string PulserTag("Height");   // 使用 "Height" 方式的脉冲刻度
  std::string AlphaCali00_08("AlphaCali00_08");
  std::string AlphaCali05_08("AlphaCali05_08");

  std::string pathPulserIn(Form("output/SSD_%s_PulserCali_%s.dat",layertag,PulserTag.c_str()));
  std::string pathPulserInGain20(Form("output/SSD_%s_PulserReCali_Gain20_%s.dat",layertag,PulserTag.c_str()));
  std::string pathAlphaCali00_08(Form("output/SSD_%s_AlphaPeaks_%s.dat",layertag,AlphaCali00_08.c_str()));
  std::string pathAlphaCali05_08(Form("output/SSD_%s_AlphaPeaks_%s.dat",layertag,AlphaCali05_08.c_str()));
  std::string pathAlphaEnergiesIn("output/SSD_AlphaEnergies.dat");

  std::string pathPDFOutput(Form("figures/SSD_%s_%sAndAlphaFitFunction.pdf",layertag,PulserTag.c_str()));
  std::string pathPDFBegin(Form("figures/SSD_%s_%sAndAlphaFitFunction.pdf[",layertag,PulserTag.c_str()));
  std::string pathPDFEnd(Form("figures/SSD_%s_%sAndAlphaFitFunction.pdf]",layertag,PulserTag.c_str()));
  std::string pathFitParsOutput(Form("output/SSD_%s_%sAndAlphaFitPars.dat",layertag,PulserTag.c_str()));

  ofstream SiEnergyCaliPars(pathFitParsOutput.c_str());
  SiEnergyCaliPars<<"* Fit function: E_MeV = k * E_CH + h, then E_CH = 1/k * E_MeV - h/k \n";
  SiEnergyCaliPars<<"* 1,2,3 represent using alpha peak1,peak2, and peak3 to calibrate the energy,respectively. \n";
  SiEnergyCaliPars<<"* SSDNum"<<setw(7)<<"CHNum"<<setw(8)<<"k1"<<setw(10)<<"h1"<<setw(12)<<"k2"<<setw(13)
                  <<"h2"<<setw(12)<<"k3"<<setw(12)<<"h3"<<setw(16)<<"k11=1./k1"<<setw(12)<<"h11=-h1/k1"<<setw(12)
                  <<"k22=1./k2"<<setw(12)<<"h22=-h2/k2"<<setw(12)<<"k33=1./k3"<<setw(13)<<"h33=-h3/k3\n";

  Int_t numpar_PulserIn       = 3;  // a, err_a, b
  Int_t numpar_PulserInGain20 = 3;  // a, err_a, b
  Int_t numpar_AlphaCali00_08 = 3;  // peak1, peak2, peak3
  Int_t numpar_AlphaCali05_08 = 3;  // peak1, peak2, peak3
  Int_t numpar_AlphaEnergies  = 3;  // E1, E2, E3

  ReadFileModule readfile;
  Double_t*** PulserIn       = readfile.ReadData(pathPulserIn.c_str(),       SSDNum, CHNum, numpar_PulserIn);
  Double_t*** PulserInGain20 = readfile.ReadData(pathPulserInGain20.c_str(), SSDNum, CHNum, numpar_PulserInGain20);
  Double_t*** AlphaCHIn00_08 = readfile.ReadData(pathAlphaCali00_08.c_str(), SSDNum, CHNum, numpar_AlphaCali00_08);
  Double_t*** AlphaCHIn05_08 = readfile.ReadData(pathAlphaCali05_08.c_str(), SSDNum, CHNum, numpar_AlphaCali05_08);
  Double_t*** AlphaEIn       = readfile.ReadData(pathAlphaEnergiesIn.c_str(),SSDNum, CHNum, numpar_AlphaEnergies);

  // 计算比例系数, 并进一步得到能量刻度曲线的洗漱
  Double_t k1[SSDNum][CHNum];
  Double_t h1[SSDNum][CHNum];   // E1_MeV = k1 * E1_CH + h1
  Double_t k2[SSDNum][CHNum];
  Double_t h2[SSDNum][CHNum];   // E2_MeV = k2 * E2_CH + h2
  Double_t k3[SSDNum][CHNum];
  Double_t h3[SSDNum][CHNum];   // E3_MeV = k3 * E3_CH + h3

  Double_t k11[SSDNum][CHNum];
  Double_t h11[SSDNum][CHNum];  // E1_CH = k11 * E1_MeV + h11
  Double_t k22[SSDNum][CHNum];
  Double_t h22[SSDNum][CHNum];  // E2_CH = k22 * E2_MeV + h22
  Double_t k33[SSDNum][CHNum];
  Double_t h33[SSDNum][CHNum];  // E3_CH = k33 * E3_MeV + h33
  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      Double_t a = PulserIn[i][j][0];
      Double_t b = PulserIn[i][j][2];
      Double_t a_gain20 = PulserInGain20[i][j][0];
      Double_t b_gain20 = PulserInGain20[i][j][2];

      Double_t E1_CH_00_08  = AlphaCHIn00_08[i][j][0];
      Double_t E2_CH_00_08  = AlphaCHIn00_08[i][j][1];
      Double_t E3_CH_00_08  = AlphaCHIn00_08[i][j][2];

      Double_t E1_CH_05_08  = AlphaCHIn05_08[i][j][0];
      Double_t E2_CH_05_08  = AlphaCHIn05_08[i][j][1];
      Double_t E3_CH_05_08  = AlphaCHIn05_08[i][j][2];

      Double_t E1_MeV = AlphaEIn[i][j][0];  // energy of peak1
      Double_t E2_MeV = AlphaEIn[i][j][1];  // energy of peak2
      Double_t E3_MeV = AlphaEIn[i][j][2];  // energy of peak3

      Double_t c1, c2, c3;
      if (i==0 || i==1) {  // for SSD1,SSD2, 改变了gain
        c1 = E1_MeV / (a_gain20 * E1_CH_00_08 + b_gain20);
        c2 = E2_MeV / (a_gain20 * E2_CH_00_08 + b_gain20);
        c3 = E3_MeV / (a_gain20 * E3_CH_00_08 + b_gain20);
      } else if (i==2) {   // for SSD3
        c1 = E1_MeV / (a * E1_CH_00_08 + b);
        c2 = E2_MeV / (a * E2_CH_00_08 + b);
        c3 = E3_MeV / (a * E3_CH_00_08 + b);
      } else {             // for SSD4,
        c1 = E1_MeV / (a * E1_CH_05_08 + b);
        c2 = E2_MeV / (a * E2_CH_05_08 + b);
        c3 = E3_MeV / (a * E3_CH_05_08 + b);
      }

      k1[i][j] = c1 * a; h1[i][j] = c1 * b;  // E1_MeV = k1 * E1_CH + h1
      k2[i][j] = c2 * a; h2[i][j] = c2 * b;  // E2_MeV = k2 * E3_CH + h2
      k3[i][j] = c3 * a; h3[i][j] = c3 * b;  // E3_MeV = k3 * E3_CH + h3

      k11[i][j] = 1./k1[i][j]; h11[i][j] = -h1[i][j]/k1[i][j]; // E1_CH = k11 * E1_MeV + h11
      k22[i][j] = 1./k2[i][j]; h22[i][j] = -h2[i][j]/k2[i][j]; // E2_CH = k22 * E2_MeV + h22
      k33[i][j] = 1./k3[i][j]; h33[i][j] = -h3[i][j]/k3[i][j]; // E3_CH = k33 * E3_MeV + h33

      SiEnergyCaliPars<<setw(5)<<i<<setw(8)<<j
                      <<setw(12)<<k1[i][j] <<setw(12)<<h1[i][j] <<setw(12)<<k2[i][j]<<setw(12)<<h2[i][j]
                      <<setw(12)<<k3[i][j] <<setw(12)<<h3[i][j] <<setw(12)<<k11[i][j]<<setw(12)<<h11[i][j]
                      <<setw(12)<<k22[i][j]<<setw(12)<<h22[i][j]<<setw(12)<<k33[i][j]<<setw(12)<<h33[i][j]
                      <<endl;
    }
  }

  // Draw the three fit functions
  TLegend* legend[SSDNum][CHNum];

  TF1* fPeak1[SSDNum][CHNum];
  TF1* fPeak2[SSDNum][CHNum];
  TF1* fPeak3[SSDNum][CHNum];

  TCanvas* cans = new TCanvas("cans","cans", 1000,600);
  cans->Divide(2,1);

  TCanvas* cans_begin = new TCanvas();
  TCanvas* cans_end = new TCanvas();
  cans_begin->Close();
  cans_end  ->Close();
  cans_begin->Print(pathPDFBegin.c_str());

  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      fPeak1[i][j] = new TF1(Form("fPeak1_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k1[i][j],h1[i][j]),0,4096);
      fPeak2[i][j] = new TF1(Form("fPeak2_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k2[i][j],h2[i][j]),0,4096);
      fPeak3[i][j] = new TF1(Form("fPeak3_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k3[i][j],h3[i][j]),0,4096);

      fPeak1[i][j]->SetTitle(Form("SSD%d_%s_CH%02d_FitFunction",i+1,layertag,j));
      fPeak1[i][j]->GetXaxis()->SetTitle("ADC CH");
      fPeak1[i][j]->GetXaxis()->CenterTitle(1);
      fPeak1[i][j]->GetYaxis()->SetTitle("Energy (MeV)");
      fPeak1[i][j]->GetYaxis()->CenterTitle(1);

      fPeak1[i][j]->SetLineColor(kRed);
      fPeak2[i][j]->SetLineColor(kGreen);
      fPeak3[i][j]->SetLineColor(kBlue);

      legend[i][j] = new TLegend(0.15,0.7,0.60,0.88);
      legend[i][j]->AddEntry(fPeak1[i][j],"Pulser+AlphaPeak1","l");
      legend[i][j]->AddEntry(fPeak2[i][j],"Pulser+AlphaPeak2","l");
      legend[i][j]->AddEntry(fPeak3[i][j],"Pulser+AlphaPeak3","l");

      cans->cd(1);
      gPad->SetLeftMargin(0.12);
      fPeak1[i][j]->Draw();
      fPeak2[i][j]->Draw("SAME");
      fPeak3[i][j]->Draw("SAME");
      legend[i][j]->Draw("SAME");

      cans->cd(2);
      gPad->SetLeftMargin(0.12);
      fPeak1[i][j]->Draw();
      fPeak2[i][j]->Draw("SAME");

      gPad->Modified();
      gPad->Update();

      cans->Print(pathPDFOutput.c_str());
    }
  }
  cans_end->Print(pathPDFEnd.c_str());

}



//___________________________________________________________
void SiEnergyCali_PulserAndAlpha_L2(const char* layertag)
{
  std::string PulserTag("Height");   // 使用 "Height" 方式的脉冲刻度
  std::string AlphaCali00_48("AlphaCali00_48");

  std::string pathPulserIn(Form("output/SSD_%s_PulserCali_%s.dat",layertag,PulserTag.c_str()));
  std::string pathAlphaCali00_48(Form("output/SSD_%s_AlphaPeaks_%s.dat",layertag,AlphaCali00_48.c_str()));
  std::string pathAlphaEnergiesIn("output/SSD_AlphaEnergies.dat");

  std::string pathPDFOutput(Form("figures/SSD_%s_%sAndAlphaFitFunction.pdf",layertag,PulserTag.c_str()));
  std::string pathPDFBegin(Form("figures/SSD_%s_%sAndAlphaFitFunction.pdf[",layertag,PulserTag.c_str()));
  std::string pathPDFEnd(Form("figures/SSD_%s_%sAndAlphaFitFunction.pdf]",  layertag,PulserTag.c_str()));
  std::string pathFitParsOutput(Form("output/SSD_%s_%sAndAlphaFitPars.dat", layertag,PulserTag.c_str()));

  ofstream SiEnergyCaliPars(pathFitParsOutput.c_str());
  SiEnergyCaliPars<<"* Fit function: E_MeV = k * E_CH + h, then E_CH = 1/k * E_MeV - h/k \n";
  SiEnergyCaliPars<<"* 1,2,3 represent using alpha peak1,peak2, and peak3 to calibrate the energy,respectively. \n";
  SiEnergyCaliPars<<"* SSDNum"<<setw(7)<<"CHNum"<<setw(8)<<"k1"<<setw(10)<<"h1"<<setw(12)<<"k2"<<setw(13)
                  <<"h2"<<setw(12)<<"k3"<<setw(12)<<"h3"<<setw(16)<<"k11=1./k1"<<setw(12)<<"h11=-h1/k1"<<setw(12)
                  <<"k22=1./k2"<<setw(12)<<"h22=-h2/k2"<<setw(12)<<"k33=1./k3"<<setw(13)<<"h33=-h3/k3\n";

  Int_t numpar_PulserIn       = 3;  // a, err_a, b
  Int_t numpar_AlphaCali00_48 = 3;  // peak1, peak2, peak3
  Int_t numpar_AlphaEnergies  = 3;  // E1, E2, E3

  ReadFileModule readfile;
  Double_t*** PulserIn       = readfile.ReadData(pathPulserIn.c_str(),       SSDNum, CHNum, numpar_PulserIn);
  Double_t*** AlphaCHIn00_48 = readfile.ReadData(pathAlphaCali00_48.c_str(), SSDNum, CHNum, numpar_AlphaCali00_48);
  Double_t*** AlphaEIn       = readfile.ReadData(pathAlphaEnergiesIn.c_str(),SSDNum, CHNum, numpar_AlphaEnergies);

  // 计算比例系数, 并进一步得到能量刻度曲线的洗漱
  Double_t k1[SSDNum][CHNum];
  Double_t h1[SSDNum][CHNum];   // E1_MeV = k1 * E1_CH + h1
  Double_t k2[SSDNum][CHNum];
  Double_t h2[SSDNum][CHNum];   // E2_MeV = k2 * E2_CH + h2
  Double_t k3[SSDNum][CHNum];
  Double_t h3[SSDNum][CHNum];   // E3_MeV = k3 * E3_CH + h3

  Double_t k11[SSDNum][CHNum];
  Double_t h11[SSDNum][CHNum];  // E1_CH = k11 * E1_MeV + h11
  Double_t k22[SSDNum][CHNum];
  Double_t h22[SSDNum][CHNum];  // E2_CH = k22 * E2_MeV + h22
  Double_t k33[SSDNum][CHNum];
  Double_t h33[SSDNum][CHNum];  // E3_CH = k33 * E3_MeV + h33
  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      Double_t a = PulserIn[i][j][0];
      Double_t b = PulserIn[i][j][2];

      Double_t E1_CH_00_48  = AlphaCHIn00_48[i][j][0];
      Double_t E2_CH_00_48  = AlphaCHIn00_48[i][j][1];
      Double_t E3_CH_00_48  = AlphaCHIn00_48[i][j][2];

      Double_t E1_MeV = AlphaEIn[i][j][0];  // energy of peak1
      Double_t E2_MeV = AlphaEIn[i][j][1];  // energy of peak2
      Double_t E3_MeV = AlphaEIn[i][j][2];  // energy of peak3

      Double_t c1 = E1_MeV / (a * E1_CH_00_48 + b);
      Double_t c2 = E2_MeV / (a * E2_CH_00_48 + b);
      Double_t c3 = E3_MeV / (a * E3_CH_00_48 + b);

      k1[i][j] = c1 * a; h1[i][j] = c1 * b;  // E1_MeV = k1 * E1_CH + h1
      k2[i][j] = c2 * a; h2[i][j] = c2 * b;  // E2_MeV = k2 * E3_CH + h2
      k3[i][j] = c3 * a; h3[i][j] = c3 * b;  // E3_MeV = k3 * E3_CH + h3

      k11[i][j] = 1./k1[i][j]; h11[i][j] = -h1[i][j]/k1[i][j]; // E1_CH = k11 * E1_MeV + h11
      k22[i][j] = 1./k2[i][j]; h22[i][j] = -h2[i][j]/k2[i][j]; // E2_CH = k22 * E2_MeV + h22
      k33[i][j] = 1./k3[i][j]; h33[i][j] = -h3[i][j]/k3[i][j]; // E3_CH = k33 * E3_MeV + h33

      SiEnergyCaliPars<<setw(5)<<i<<setw(8)<<j
                      <<setw(12)<<k1[i][j] <<setw(12)<<h1[i][j] <<setw(12)<<k2[i][j]<<setw(12)<<h2[i][j]
                      <<setw(12)<<k3[i][j] <<setw(12)<<h3[i][j] <<setw(12)<<k11[i][j]<<setw(12)<<h11[i][j]
                      <<setw(12)<<k22[i][j]<<setw(12)<<h22[i][j]<<setw(12)<<k33[i][j]<<setw(12)<<h33[i][j]
                      <<endl;
    }
  }

  // Draw the three fit functions
  TCanvas* cans = new TCanvas("cans","cans", 1000,600);
  cans->Divide(2,1);

  TCanvas* cans_begin = new TCanvas();
  TCanvas* cans_end = new TCanvas();
  cans_begin->Close();
  cans_end  ->Close();
  cans_begin->Print(pathPDFBegin.c_str());

  TLegend* legend[SSDNum][CHNum];

  TF1* fPeak1[SSDNum][CHNum];
  TF1* fPeak2[SSDNum][CHNum];
  TF1* fPeak3[SSDNum][CHNum];
  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      fPeak1[i][j] = new TF1(Form("fPeak1_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k1[i][j],h1[i][j]),0,4096);
      fPeak2[i][j] = new TF1(Form("fPeak2_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k2[i][j],h2[i][j]),0,4096);
      fPeak3[i][j] = new TF1(Form("fPeak3_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k3[i][j],h3[i][j]),0,4096);

      fPeak1[i][j]->SetTitle(Form("SSD%d_%s_CH%02d_FitFunction",i+1,layertag,j));
      fPeak1[i][j]->GetXaxis()->SetTitle("ADC CH");
      fPeak1[i][j]->GetXaxis()->CenterTitle(1);
      fPeak1[i][j]->GetYaxis()->SetTitle("Energy (MeV)");
      fPeak1[i][j]->GetYaxis()->CenterTitle(1);

      fPeak1[i][j]->SetLineColor(kRed);
      fPeak2[i][j]->SetLineColor(kGreen);
      fPeak3[i][j]->SetLineColor(kBlue);

      legend[i][j] = new TLegend(0.20,0.7,0.65,0.88);
      legend[i][j]->AddEntry(fPeak1[i][j],"Pulser+AlphaPeak1","l");
      legend[i][j]->AddEntry(fPeak2[i][j],"Pulser+AlphaPeak2","l");
      legend[i][j]->AddEntry(fPeak3[i][j],"Pulser+AlphaPeak3","l");

      cans->cd(1);
      gPad->SetLeftMargin(0.12);
      fPeak1[i][j]->Draw();
      fPeak2[i][j]->Draw("SAME");
      fPeak3[i][j]->Draw("SAME");
      legend[i][j]->Draw("SAME");

      cans->cd(2);
      gPad->SetLeftMargin(0.12);
      fPeak1[i][j]->Draw();
      fPeak2[i][j]->Draw("SAME");

      gPad->Modified();
      gPad->Update();

      cans->Print(pathPDFOutput.c_str());
    }
  }
  cans_end->Print(pathPDFEnd.c_str());
}
