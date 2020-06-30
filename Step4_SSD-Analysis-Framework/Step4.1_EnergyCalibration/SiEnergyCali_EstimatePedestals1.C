////////////////////////////////////////////////////////////////////////////////
//
//    考察能量刻度曲线的零点与 pedestal 之间的差异.
//    主要是检验两者之间是否存才某种关系: 比如, 两者是否在一定程度上相等?
//
//    这里, 能量刻度的零点是指: E_MeV = k * E_Ch + h 中, E_MeV = 0 时, E_CH = -h/k
//    pedetsal 使用的是 Pulser 的 pedestal
//
//    考察的内容包括: 两者的绝对误差 (ADC 道数) 与 相对误差 (百分比)
//
//    考察结果表明: 刻度曲线的截距总是比 pedestal 小, 这的合理的，因为 ADC 有阈值. 这里就
//    根据能量刻度曲线, 估算一下 ADC 阈值的大小
//
//
//    gfh, 2020-06-19
////////////////////////////////////////////////////////////////////////////////

#include "../../include/ReadFileModule.h"


Int_t SSDNum = 4;
Int_t CHNum  = 16;


//__________________________________________
void EstimatePedestals(const char* layertag);
void EstimateADCThreshold(const char* layertag);


//______________________________________________
void SiEnergyCali_EstimatePedestals()
{
  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");

  EstimatePedestals(L1STag.c_str());
  //EstimatePedestals(L2FTag.c_str());
  //EstimatePedestals(L2BTag.c_str());

  //EstimateADCThreshold(L1STag.c_str());
  //EstimateADCThreshold(L2FTag.c_str());
  EstimateADCThreshold(L2BTag.c_str());
}


//________________________________________
void EstimatePedestals(const char* layertag)
{
  std::string PulserTag("Height");

  std::string pathPedestalsInput(Form("output/SSD_%s_PulserCaliPedestals_Pedestal0000.dat",layertag));
  std::string pathParsInput(Form("output/SSD_%s_%sAndAlphaFitPars.dat",layertag,PulserTag.c_str()));

  std::string pathPNGOutput[SSDNum];
  for (Int_t i=0; i<SSDNum; i++)
  {
    pathPNGOutput[i] = Form("figures/SSD%d_%s_ErrorsOfPedestalsAndEnergyCaliIntercept.png",i+1,layertag);
  }

  Int_t index;
  if (strcmp(layertag,"L1S")==0) index = 0;
  if (strcmp(layertag,"L2F")==0) index = 1;
  if (strcmp(layertag,"L2B")==0) index = 2;

  Double_t PedestalRangeLow[3]    = { 50., 30.,  30. };
  Double_t PedestalRangeUp [3]    = { 130.,130., 160.};
  Double_t ErrorRangeLow[3]       = {-25., -35., -25.};
  Double_t ErrorRangeUp [3]       = { 0.,  -5.,   0.};
  Double_t RelativeErrRangeLow[3] = {-30., -40., -50.};
  Double_t RelativeErrRangeUp [3] = { 0.,  -5.,   0.};
  Double_t latex_PedestalUp    = 0.9 * (PedestalRangeUp[index]   - PedestalRangeLow[index])    + PedestalRangeLow[index];
  Double_t latex_PedestalLow   = 0.1 * (PedestalRangeUp[index]   - PedestalRangeLow[index])    + PedestalRangeLow[index];
  Double_t latex_ErrorUp       = 0.9 * (ErrorRangeUp[index]      - ErrorRangeLow[index])       + ErrorRangeLow[index];
  Double_t latex_RelativeErrUp = 0.9 * (RelativeErrRangeUp[index]- RelativeErrRangeLow[index]) + RelativeErrRangeLow[index];


  Int_t numpar_PedestalsIn = 2;
  Int_t numpar_ParsInput   = 8; // k1,h1,k2,h2,k3,h3,k11,h11... h11是截距

  ReadFileModule readfile;
  Double_t*** PedestalIn = readfile.ReadData(pathPedestalsInput.c_str(), SSDNum, CHNum, numpar_PedestalsIn);
  Double_t*** ParsInput  = readfile.ReadData(pathParsInput.c_str(), SSDNum, CHNum, numpar_ParsInput);

  Double_t SSDCHNum[SSDNum][CHNum];
  Double_t PulserPedestals[SSDNum][CHNum];
  Double_t EnergyCaliPars[SSDNum][CHNum];
  Double_t Errors[SSDNum][CHNum];
  Double_t Average_PulserPedestals[SSDNum];
  Double_t Average_EnergyCaliPars[SSDNum];
  Double_t Average_Errors[SSDNum];

  Double_t Sum_PulserPedestals[SSDNum];
  Double_t Sum_EnergyCaliPars[SSDNum];
  Double_t Sum_Errors[SSDNum];

  for (Int_t i=0; i<SSDNum; i++)
  {
     Sum_PulserPedestals[i]     = 0.;
     Sum_EnergyCaliPars [i]     = 0.;
     Sum_Errors[i]              = 0.;

    for (Int_t j=0; j<CHNum; j++)
    {
      SSDCHNum[i][j] = j + 1;
      PulserPedestals[i][j] = PedestalIn[i][j][0];
      EnergyCaliPars[i][j]  = ParsInput[i][j][7]; // h11
      Errors[i][j]          = EnergyCaliPars[i][j] - PulserPedestals[i][j];

      Sum_PulserPedestals[i] += PulserPedestals[i][j];
      Sum_EnergyCaliPars[i]  += EnergyCaliPars[i][j];
      Sum_Errors[i]          += Errors[i][j];
    }
    Average_PulserPedestals[i] = Sum_PulserPedestals[i]/CHNum;
    Average_EnergyCaliPars [i] = Sum_EnergyCaliPars[i]/CHNum;
    Average_Errors         [i] = Sum_Errors[i]/CHNum;
  }

  TMultiGraph* mg[SSDNum];
  TGraph* graph_pulser[SSDNum];
  TGraph* graph_energycali[SSDNum];
  TGraph* graph_errors[SSDNum];
  TLine* line_ave1[SSDNum];
  TLine* line_ave2[SSDNum];
  TLine* line_aveErrs[SSDNum];
  TLatex* latex1[SSDNum];
  TLatex* latex2[SSDNum];
  TLatex* latex_aveErr[SSDNum];
  for (Int_t i=0; i<SSDNum; i++)
  {
    mg[i] = new TMultiGraph();

    graph_pulser[i]       = new TGraph(CHNum, SSDCHNum[i], PulserPedestals[i]);
    graph_energycali[i]   = new TGraph(CHNum, SSDCHNum[i], EnergyCaliPars[i]);
    graph_errors[i]       = new TGraph(CHNum, SSDCHNum[i], Errors[i]);

    graph_pulser[i]->SetMarkerStyle(20);
    graph_pulser[i]->SetMarkerColor(kBlue);
    graph_energycali[i]->SetMarkerStyle(20);
    graph_energycali[i]->SetMarkerColor(kRed);

    graph_errors[i]->SetMarkerStyle(20);
    graph_errors[i]->SetMarkerColor(kMagenta);


    mg[i]->Add(graph_pulser[i]);
    mg[i]->Add(graph_energycali[i]);
  }

  TCanvas* cans[SSDNum];
  for (Int_t i=0; i<SSDNum; i++)
  {
    cans[i] = new TCanvas(Form("cans_%d",i+1),Form("cans_%d",i+1),1200,1000);
    cans[i]->Divide(2,2);


    // pedestal 分布
    cans[i]->cd(1);
    gPad->SetGridx();
    mg[i]->SetTitle(Form("SSD%d_%s_PedestalDistributions",i+1,layertag));
    mg[i]->GetXaxis()->SetRangeUser(0,17);
    mg[i]->GetXaxis()->SetNdivisions(117);
    mg[i]->GetXaxis()->SetTitle("Channel");
    mg[i]->GetXaxis()->CenterTitle(1);
    mg[i]->GetXaxis()->SetTitleSize(0.04);
    mg[i]->GetYaxis()->SetRangeUser(PedestalRangeLow[index],PedestalRangeUp[index]);
    mg[i]->GetYaxis()->SetNdivisions(1008);

    line_ave1[i] = new TLine(mg[i]->GetXaxis()->GetXmin(),Average_PulserPedestals[i],
                             mg[i]->GetXaxis()->GetXmax(),Average_PulserPedestals[i]);
    line_ave2[i] = new TLine(mg[i]->GetXaxis()->GetXmin(),Average_EnergyCaliPars[i],
                             mg[i]->GetXaxis()->GetXmax(),Average_EnergyCaliPars[i]);
    line_ave1[i]->SetLineColor(kBlue);
    line_ave1[i]->SetLineStyle(7);
    line_ave1[i]->SetLineWidth(3);
    line_ave2[i]->SetLineColor(kRed);
    line_ave2[i]->SetLineStyle(7);
    line_ave2[i]->SetLineWidth(3);

    latex1[i] = new TLatex(6.,latex_PedestalUp,  Form("<pedestal> = %.1f",Average_PulserPedestals[i]));
    latex2[i] = new TLatex(6.,latex_PedestalLow, Form("<intercept_{CH}> = %.1f",Average_EnergyCaliPars[i]));
    latex1[i]->SetTextColor(kBlue);
    latex2[i]->SetTextColor(kRed);

    mg[i]->Draw("APL");
    line_ave1[i]->Draw();
    line_ave2[i]->Draw();
    latex1[i]   ->Draw();
    latex2[i]   ->Draw();


    // 绝对误差分布
    cans[i]->cd(2);
    gPad->SetGridx();
    graph_errors[i]->SetTitle(Form("SSD%d_%s_PedestalErrors",i+1,layertag));
    graph_errors[i]->GetXaxis()->SetRangeUser(0.5,16.5);
    graph_errors[i]->GetXaxis()->SetNdivisions(118);
    graph_errors[i]->GetXaxis()->SetTitle("Channel");
    graph_errors[i]->GetXaxis()->CenterTitle(1);
    graph_errors[i]->GetXaxis()->SetTitleSize(0.04);
    graph_errors[i]->GetYaxis()->SetRangeUser(ErrorRangeLow[index],ErrorRangeUp[index]);
    graph_errors[i]->GetYaxis()->SetNdivisions(504);

    line_aveErrs[i] = new TLine(0.5, Average_Errors[i], 16.5, Average_Errors[i]);
    line_aveErrs[i]->SetLineColor(kGreen);
    line_aveErrs[i]->SetLineStyle(7);
    line_aveErrs[i]->SetLineWidth(3);

    latex_aveErr[i] = new TLatex(5.,latex_ErrorUp,Form("<Errors> = %.1f CH ",Average_Errors[i]));
    latex_aveErr[i]->SetTextColor(kMagenta);

    graph_errors[i]->Draw("APL");
    line_aveErrs[i]->Draw();
    latex_aveErr[i]->Draw();


    // 相对误差分布


    cans[i]->Print(pathPNGOutput[i].c_str());
  }
}


//_____________________________________________
void EstimateADCThreshold(const char* layertag)
{
  std::string PulserTag("Height");

  std::string pathPedestalsInput(Form("output/SSD_%s_PulserCaliPedestals_Pedestal0000.dat",layertag));
  std::string pathParsInput(Form("output/SSD_%s_%sAndAlphaFitPars.dat",layertag,PulserTag.c_str()));

  Double_t XMax = 4096.;
  Double_t VoltageMax = 4000.; // 单位：mV, ADC 量程是 4V

  Int_t numpar_PedestalsIn = 2;
  Int_t numpar_ParsInput   = 2; // k1,h1,k2,h2,k3,h3,k11,h11... h11是截距

  ReadFileModule readfile;
  Double_t*** PedestalIn = readfile.ReadData(pathPedestalsInput.c_str(), SSDNum, CHNum, numpar_PedestalsIn);
  Double_t*** ParsInput  = readfile.ReadData(pathParsInput.c_str(), SSDNum, CHNum, numpar_ParsInput);

  Double_t PulserPedestals[SSDNum][CHNum];
  Double_t k[SSDNum][CHNum];
  Double_t b[SSDNum][CHNum];  // E_MeV = k * E_CH + b
  Double_t Threshold_MeV    [CHNum][CHNum];
  Double_t SSDCHNum[CHNum][CHNum];

  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      SSDCHNum[i][j] = j+1;

      PulserPedestals[i][j] = PedestalIn[i][j][0];
      k[i][j] = ParsInput[i][j][0];
      b[i][j] = ParsInput[i][j][1];

      Threshold_MeV[i][j] = k[i][j] * PulserPedestals[i][j] + b[i][j];

      cout<<setw(5)<<i<<setw(5)<<j<<setw(15)<<k[i][j]<<setw(15)<<b[i][j]
          <<setw(15)<<Threshold_MeV[i][j]<<endl;
    }
  }
}
