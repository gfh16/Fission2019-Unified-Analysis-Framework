
#include "../include/CSHINESiEnergyCali.h"
using namespace std;


//******************************************************************************
CSHINESiEnergyCali::CSHINESiEnergyCali()
{}

//__________________________________________
CSHINESiEnergyCali::~CSHINESiEnergyCali()
{}
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
// 根据 pulser 刻度 与 alpha 能量点, 可求得硅条的能量刻度曲线 !
//   说明1: pulser 刻度同一使用 "Height" 刻度的结果, 因为点较多、且均匀分布
//   说明2：pulser 刻度时， SSD2_L2F_CH00,CH01 两道的道址只有其他道的一半，需要注意！
//   说明3: alpha 刻度时，
//        L1: SSD1,SSD2,SSD3 使用合并文件 L1_AlphaCali00_08.root
//            SSD4 使用合并文件 L1_AlphaCali05_08.root, 这是因为前后两次刻度有一个shift
//           此外, alpha 刻度时，SSD1_L1,SSD2_L1 改变了主放 gain，需要注意！
//        L2: SSD1,SSD2,SSD3,SSD4 均采用合并文件 L2_AlphaCali00_48.root
//           SSD4_L2F_CH13,CH14 两道有两组 alpha 峰, 暂使用较大一组进行刻度
//   说明4：三组分 alpha 源有 三个 alpha 峰， 但由于能量最大的峰计数小很多，在进行能量刻度时
//         将选取前两个 alpha 峰，分别进行刻度，再将两组刻度曲线进行比较, 最终再确定如何确定
//         能量刻度曲线
//  gfh, 2020-06-16
//  修改成类, 2020-10-03
/////////////////////////////////////////////////////////////////////////////////
void CSHINESiEnergyCali::PulserAndAlphaCali(const char* layertag, const char* pulserfiletag)
{
  gStyle->SetPalette(1);

  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string AlphaCali00_08("AlphaCali00_08"); // for L1--SSD1,SSD2,SSD3
  std::string AlphaCali05_08("AlphaCali05_08"); // for L1--SSD4
  std::string AlphaCali00_48("AlphaCali00_48"); // for L2

  std::string pathPulserIn(Form("%sdata_PulserCali/SSD_%s_PulserCali_%s.dat",PATHDATAFOLDER,layertag,pulserfiletag));
  std::string pathAlphaEnergiesIn(Form("%sdata_AlphaCali/SSD_AlphaEnergies.dat",PATHDATAFOLDER));

  std::string pathPulserInGain20;
  std::string pathAlphaCali00_08;
  std::string pathAlphaCali05_08;
  std::string pathAlphaCali00_48;
  if (strcmp(layertag,L1STag.c_str())==0) {
    pathPulserInGain20 = Form("%sdata_PulserCali/SSD_%s_PulserReCali_Gain20_%s.dat",PATHDATAFOLDER,L1STag.c_str(),pulserfiletag);
    pathAlphaCali00_08 = Form("%sdata_AlphaCali/SSD_%s_AlphaPeaks_%s.dat",PATHDATAFOLDER,L1STag.c_str(),AlphaCali00_08.c_str());
    pathAlphaCali05_08 = Form("%sdata_AlphaCali/SSD_%s_AlphaPeaks_%s.dat",PATHDATAFOLDER,L1STag.c_str(),AlphaCali05_08.c_str());
  } else {
    pathAlphaCali00_48 = Form("%sdata_AlphaCali/SSD_%s_AlphaPeaks_%s.dat",PATHDATAFOLDER,layertag,AlphaCali00_48.c_str());
  }

  std::string pathPDFOutput(Form("%sfigure_SiEnergyCali/SSD_%s_%sAndAlphaFitFunc.pdf",PATHFIGURESFOLDER,layertag,pulserfiletag));
  std::string pathPDFBegin(Form("%sfigure_SiEnergyCali/SSD_%s_%sAndAlphaFitFunc.pdf[",PATHFIGURESFOLDER,layertag,pulserfiletag));
  std::string pathPDFEnd  (Form("%sfigure_SiEnergyCali/SSD_%s_%sAndAlphaFitFunc.pdf]",PATHFIGURESFOLDER,layertag,pulserfiletag));

  std::string pathFitParsOutput(Form("%sdata_SiEnergyCali/SSD_%s_%sAndAlphaFitPars.dat",PATHDATAFOLDER,layertag,pulserfiletag));
  ofstream SiEnergyCaliPars(pathFitParsOutput.c_str());
  SiEnergyCaliPars<<"* Fit function: E_MeV = k * E_CH + h, then E_CH = 1/k * E_MeV - h/k \n";
  SiEnergyCaliPars<<"* 1,2,3 represent using alpha peak1,peak2, and peak3 to calibrate the energy,respectively. \n";
  SiEnergyCaliPars<<"* SSDNum"<<setw(7)<<"CHNum"<<setw(8)<<"k1"<<setw(10)<<"h1"<<setw(12)<<"k2"<<setw(13)
                  <<"h2"<<setw(12)<<"k3"<<setw(12)<<"h3"<<setw(16)<<"k11=1./k1"<<setw(12)<<"h11=-h1/k1"<<setw(12)
                  <<"k22=1./k2"<<setw(12)<<"h22=-h2/k2"<<setw(12)<<"k33=1./k3"<<setw(13)<<"h33=-h3/k3\n";
  //____________________________________________________________________________
  Int_t numpar_PulserIn       = 3;  // a, err_a, b
  Int_t numpar_PulserInGain20 = 3;  // a, err_a, b
  Int_t numpar_AlphaCali00_08 = 3;  // peak1, peak2, peak3
  Int_t numpar_AlphaCali05_08 = 3;  // peak1, peak2, peak3
  Int_t numpar_AlphaCali00_48 = 3;  // peak1, peak2, peak3
  Int_t numpar_AlphaEnergies  = 3;  // E1, E2, E3

  ReadFileModule readfile;
  Double_t*** PulserIn = readfile.ReadData(pathPulserIn.c_str(),NUM_SSD,NUM_STRIP,numpar_PulserIn);
  Double_t*** AlphaEIn = readfile.ReadData(pathAlphaEnergiesIn.c_str(),NUM_SSD,NUM_STRIP,numpar_AlphaEnergies);
  Double_t*** PulserInGain20;
  Double_t*** AlphaCHIn00_08;
  Double_t*** AlphaCHIn05_08;
  Double_t*** AlphaCHIn00_48;
  if (strcmp(layertag,L1STag.c_str())==0) {
    PulserInGain20 = readfile.ReadData(pathPulserInGain20.c_str(), NUM_SSD,NUM_STRIP,numpar_PulserInGain20);
    AlphaCHIn00_08 = readfile.ReadData(pathAlphaCali00_08.c_str(), NUM_SSD,NUM_STRIP,numpar_AlphaCali00_08);
    AlphaCHIn05_08 = readfile.ReadData(pathAlphaCali05_08.c_str(), NUM_SSD,NUM_STRIP,numpar_AlphaCali05_08);
  } else {
    AlphaCHIn00_48 = readfile.ReadData(pathAlphaCali00_48.c_str(), NUM_SSD,NUM_STRIP,numpar_AlphaCali00_48);
  }
  // 计算比例系数, 并进一步得到能量刻度曲线的洗漱
  Double_t k1 [NUM_SSD][NUM_STRIP];
  Double_t h1 [NUM_SSD][NUM_STRIP];  // E1_MeV = k1 * E1_CH + h1
  Double_t k2 [NUM_SSD][NUM_STRIP];
  Double_t h2 [NUM_SSD][NUM_STRIP];  // E2_MeV = k2 * E2_CH + h2
  Double_t k3 [NUM_SSD][NUM_STRIP];
  Double_t h3 [NUM_SSD][NUM_STRIP];  // E3_MeV = k3 * E3_CH + h3
  Double_t k11[NUM_SSD][NUM_STRIP];
  Double_t h11[NUM_SSD][NUM_STRIP];  // E1_CH = k11 * E1_MeV + h11
  Double_t k22[NUM_SSD][NUM_STRIP];
  Double_t h22[NUM_SSD][NUM_STRIP];  // E2_CH = k22 * E2_MeV + h22
  Double_t k33[NUM_SSD][NUM_STRIP];
  Double_t h33[NUM_SSD][NUM_STRIP];  // E3_CH = k33 * E3_MeV + h33

  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
      Double_t a = PulserIn[i][j][0];
      Double_t b = PulserIn[i][j][2];
      Double_t E1_MeV = AlphaEIn[i][j][0];  // energy of peak1
      Double_t E2_MeV = AlphaEIn[i][j][1];  // energy of peak2
      Double_t E3_MeV = AlphaEIn[i][j][2];  // energy of peak3

      Double_t c1, c2, c3;
      if (strcmp(layertag,L1STag.c_str())==0) { // for l1S
        Double_t a_gain20 = PulserInGain20[i][j][0];
        Double_t b_gain20 = PulserInGain20[i][j][2];
        Double_t E1_CH_00_08  = AlphaCHIn00_08[i][j][0];
        Double_t E2_CH_00_08  = AlphaCHIn00_08[i][j][1];
        Double_t E3_CH_00_08  = AlphaCHIn00_08[i][j][2];
        Double_t E1_CH_05_08  = AlphaCHIn05_08[i][j][0];
        Double_t E2_CH_05_08  = AlphaCHIn05_08[i][j][1];
        Double_t E3_CH_05_08  = AlphaCHIn05_08[i][j][2];
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
      } else { // for L2F && L2B
        Double_t E1_CH_00_48  = AlphaCHIn00_48[i][j][0];
        Double_t E2_CH_00_48  = AlphaCHIn00_48[i][j][1];
        Double_t E3_CH_00_48  = AlphaCHIn00_48[i][j][2];
        c1 = E1_MeV / (a * E1_CH_00_48 + b);
        c2 = E2_MeV / (a * E2_CH_00_48 + b);
        c3 = E3_MeV / (a * E3_CH_00_48 + b);
      }
      k1[i][j]  = c1 * a; h1[i][j] = c1 * b;  // E1_MeV = k1 * E1_CH + h1
      k2[i][j]  = c2 * a; h2[i][j] = c2 * b;  // E2_MeV = k2 * E3_CH + h2
      k3[i][j]  = c3 * a; h3[i][j] = c3 * b;  // E3_MeV = k3 * E3_CH + h3
      k11[i][j] = 1./k1[i][j]; h11[i][j] = -h1[i][j]/k1[i][j]; // E1_CH = k11 * E1_MeV + h11
      k22[i][j] = 1./k2[i][j]; h22[i][j] = -h2[i][j]/k2[i][j]; // E2_CH = k22 * E2_MeV + h22
      k33[i][j] = 1./k3[i][j]; h33[i][j] = -h3[i][j]/k3[i][j]; // E3_CH = k33 * E3_MeV + h33
      SiEnergyCaliPars<<setw(5)<<i<<setw(8)<<j
                      <<setw(12)<<k1[i][j] <<setw(12)<<h1[i][j] <<setw(12)<<k2[i][j]<<setw(12)<<h2[i][j]
                      <<setw(12)<<k3[i][j] <<setw(12)<<h3[i][j] <<setw(12)<<k11[i][j]<<setw(12)<<h11[i][j]
                      <<setw(12)<<k22[i][j]<<setw(12)<<h22[i][j]<<setw(12)<<k33[i][j]<<setw(12)<<h33[i][j]
                      <<endl;
    } // close j
  } // close i
  // Draw the three fit functions
  TCanvas* cans = new TCanvas("cans","cans", 1000,600);
  cans->Divide(2,1);
  TCanvas* cans_begin = new TCanvas();
  TCanvas* cans_end = new TCanvas();
  cans_begin->Close();
  cans_end  ->Close();
  cans_begin->Print(pathPDFBegin.c_str());

  TLegend* legend[NUM_SSD][NUM_STRIP];
  TF1* fPeak1[NUM_SSD][NUM_STRIP];
  TF1* fPeak2[NUM_SSD][NUM_STRIP];
  TF1* fPeak3[NUM_SSD][NUM_STRIP];
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
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
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//  考察能量刻度曲线的零点与 pedestal 之间的差异.
//  主要是检验两者之间是否存才某种关系: 比如, 两者是否在一定程度上相等?
//  这里, 能量刻度的零点是指: E_MeV = k * E_Ch + h 中, E_MeV = 0 时, E_CH = -h/k
//     pedetsal 使用的是 Pulser 的 pedestal
//  考察的内容包括: 两者的绝对误差 (ADC 道数) 与 相对误差 (百分比)
//  考察结果表明: 刻度曲线的截距总是比 pedestal 小, 这的合理的，因为 ADC 有阈值. 这里就
//  根据能量刻度曲线, 估算一下 ADC 阈值的大小
//
//  gfh, 2020-06-19
//  修改成类, 2020-10-03
////////////////////////////////////////////////////////////////////////////////
void CSHINESiEnergyCali::EstimatePedestals(const char* layertag, const char* pulserfiletag)
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
  Int_t numpar_ParsInput   = 8; // a1,b1,a2,b2,a3,b3,a11,b11... b11是截距

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

  Double_t a1[SSDNum][CHNum];
  Double_t b1[SSDNum][CHNum];   // E_Mev = a1 * E_CH + b1
  Double_t Threshold_MeV[SSDNum][CHNum];

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

      a1[i][j] = ParsInput[i][j][0];
      b1[i][j] = ParsInput[i][j][1];
      Threshold_MeV[i][j] = a1[i][j] * PedestalIn[i][j][0] + b1[i][j];

      Sum_PulserPedestals[i] += PulserPedestals[i][j];
      Sum_EnergyCaliPars[i]  += EnergyCaliPars[i][j];
      Sum_Errors[i]          += Errors[i][j];
    }
    Average_PulserPedestals[i] = Sum_PulserPedestals[i]/CHNum;
    Average_EnergyCaliPars [i] = Sum_EnergyCaliPars[i]/CHNum;
    Average_Errors         [i] = Sum_Errors[i]/CHNum;
  }

  TMultiGraph* mg[SSDNum];
  TGraph* graph_pulser    [SSDNum];
  TGraph* graph_energycali[SSDNum];
  TGraph* graph_errors    [SSDNum];
  TGraph* graph_threshold [SSDNum];

  TLine* line_ave1   [SSDNum];
  TLine* line_ave2   [SSDNum];
  TLine* line_aveErrs[SSDNum];

  TLatex* latex1[SSDNum];
  TLatex* latex2[SSDNum];
  TLatex* latex_aveErr[SSDNum];
  TLatex* latex_threshold[SSDNum];

  for (Int_t i=0; i<SSDNum; i++)
  {
    mg[i] = new TMultiGraph();

    graph_pulser[i]     = new TGraph(CHNum, SSDCHNum[i], PulserPedestals[i]);
    graph_energycali[i] = new TGraph(CHNum, SSDCHNum[i], EnergyCaliPars[i]);
    graph_errors[i]     = new TGraph(CHNum, SSDCHNum[i], Errors[i]);
    graph_threshold[i]  = new TGraph(CHNum, SSDCHNum[i], Threshold_MeV[i]);

    graph_pulser[i]->SetMarkerStyle(20);
    graph_pulser[i]->SetMarkerColor(kBlue);
    graph_energycali[i]->SetMarkerStyle(20);
    graph_energycali[i]->SetMarkerColor(kRed);

    graph_errors[i]->SetMarkerStyle(20);
    graph_errors[i]->SetMarkerColor(kMagenta);

    graph_threshold[i]->SetMarkerStyle(20);
    graph_threshold[i]->SetMarkerColor(kCyan+1);

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


    // ADC 能量阈值分布
    cans[i]->cd(4);
    gPad->SetGridx();
    graph_threshold[i]->SetTitle(Form("SSD%d_%s_ADCThresholdCalculatedWithFitFunc",i+1,layertag));
    graph_threshold[i]->GetXaxis()->SetRangeUser(0,17);
    graph_threshold[i]->GetXaxis()->SetNdivisions(118);
    graph_threshold[i]->GetXaxis()->SetTitle("Channel");
    graph_threshold[i]->GetXaxis()->CenterTitle(1);
    graph_threshold[i]->GetXaxis()->SetTitleSize(0.04);
    graph_threshold[i]->GetYaxis()->SetRangeUser(0.,3.);
    graph_threshold[i]->GetYaxis()->SetNdivisions(1006);
    graph_threshold[i]->GetYaxis()->SetTitle("Threshold (MeV)");
    graph_threshold[i]->GetYaxis()->CenterTitle(1);
    graph_threshold[i]->GetYaxis()->SetTitleSize(0.04);

    latex_threshold[i] = new TLatex(4.,1.7,"Threshold = a * Pedestal + b");
    latex_threshold[i]->SetTextColor(kCyan+1);

    graph_threshold[i]->Draw("APL");
    latex_threshold[i]->Draw("SAME");


    cans[i]->Print(pathPNGOutput[i].c_str());
  }
}
//******************************************************************************


//******************************************************************************
void CSHINESiEnergyCali::EstimateFitPars()
{}
//******************************************************************************


//******************************************************************************
void CSHINESiEnergyCali::CheckAbnormalChannels()
{}
//******************************************************************************


//******************************************************************************
void CSHINESiEnergyCali::EstimateDeadlayerEffects()
{}
//******************************************************************************


//******************************************************************************
void CSHINESiEnergyCali::EstimateAlphaChannelEffOnFitPars()
{}
//******************************************************************************


//******************************************************************************
void CSHINESiEnergyCali::FinallyDeterminedFitPars()
{}
//******************************************************************************
