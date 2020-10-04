
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
  std::string pathPedestalsInput(Form("%sdata_Pedestals/SSD_%s_PulserCaliPedestals_Pedestal0000.dat",
                                 PATHDATAFOLDER,layertag));
  std::string pathParsInput(Form("%sdata_SiEnergyCali/SSD_%s_%sAndAlphaFitPars.dat",PATHDATAFOLDER,
                                 layertag,pulserfiletag));
  std::string pathPNGOutput[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    pathPNGOutput[i] = Form("%sfigure_SiEnergyCali/SSD%d_%s_ErrorsOfPedestalsAndEnergyCaliIntercept.png",
                            PATHFIGURESFOLDER,i+1,layertag);
  }
  Int_t index;
  if (strcmp(layertag,"L1S")==0) index = 0;
  if (strcmp(layertag,"L2F")==0) index = 1;
  if (strcmp(layertag,"L2B")==0) index = 2;
  Double_t PedestalRangeLow   [3] = { 50., 30.,  30. };
  Double_t PedestalRangeUp    [3] = { 130.,130., 160.};
  Double_t ErrorRangeLow      [3] = {-25., -35., -25.};
  Double_t ErrorRangeUp       [3] = { 0.,  -5.,   0.};
//Double_t RelativeErrRangeLow[3] = {-30., -40., -50.};
//Double_t RelativeErrRangeUp [3] = { 0.,  -5.,   0.};
  Double_t latex_PedestalUp    = 0.9 * (PedestalRangeUp   [index] - PedestalRangeLow   [index]) + PedestalRangeLow[index];
  Double_t latex_PedestalLow   = 0.1 * (PedestalRangeUp   [index] - PedestalRangeLow   [index]) + PedestalRangeLow[index];
  Double_t latex_ErrorUp       = 0.9 * (ErrorRangeUp      [index] - ErrorRangeLow      [index]) + ErrorRangeLow[index];
//Double_t latex_RelativeErrUp = 0.9 * (RelativeErrRangeUp[index] - RelativeErrRangeLow[index]) + RelativeErrRangeLow[index];
  //____________________________________________________________________________
  Int_t numpar_PedestalsIn = 2;
  Int_t numpar_ParsInput   = 8; // a1,b1,a2,b2,a3,b3,a11,b11... b11是截距
  ReadFileModule readfile;
  Double_t*** PedestalIn = readfile.ReadData(pathPedestalsInput.c_str(),NUM_SSD,NUM_STRIP,numpar_PedestalsIn);
  Double_t*** ParsInput  = readfile.ReadData(pathParsInput.c_str(),NUM_SSD,NUM_STRIP,numpar_ParsInput);

  Double_t SSDCHNum[NUM_SSD][NUM_STRIP];
  Double_t PulserPedestals[NUM_SSD][NUM_STRIP];
  Double_t EnergyCaliPars[NUM_SSD][NUM_STRIP];
  Double_t Errors[NUM_SSD][NUM_STRIP];
  Double_t Average_PulserPedestals[NUM_SSD];
  Double_t Average_EnergyCaliPars[NUM_SSD];
  Double_t Average_Errors[NUM_SSD];
  Double_t Sum_PulserPedestals[NUM_SSD];
  Double_t Sum_EnergyCaliPars[NUM_SSD];
  Double_t Sum_Errors[NUM_SSD];
  Double_t a1[NUM_SSD][NUM_STRIP];
  Double_t b1[NUM_SSD][NUM_STRIP]; // E_Mev = a1 * E_CH + b1
  Double_t Threshold_MeV[NUM_SSD][NUM_STRIP];
  for (Int_t i=0; i<NUM_SSD; i++) {
     Sum_PulserPedestals[i]     = 0.;
     Sum_EnergyCaliPars [i]     = 0.;
     Sum_Errors[i]              = 0.;
    for (Int_t j=0; j<NUM_STRIP; j++) {
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
    Average_PulserPedestals[i] = Sum_PulserPedestals[i]/NUM_STRIP;
    Average_EnergyCaliPars [i] = Sum_EnergyCaliPars[i]/NUM_STRIP;
    Average_Errors         [i] = Sum_Errors[i]/NUM_STRIP;
  }
  TMultiGraph* mg[NUM_SSD];
  TGraph* graph_pulser    [NUM_SSD];
  TGraph* graph_energycali[NUM_SSD];
  TGraph* graph_errors    [NUM_SSD];
  TGraph* graph_threshold [NUM_SSD];

  TLine* line_ave1   [NUM_SSD];
  TLine* line_ave2   [NUM_SSD];
  TLine* line_aveErrs[NUM_SSD];

  TLatex* latex1[NUM_SSD];
  TLatex* latex2[NUM_SSD];
  TLatex* latex_aveErr[NUM_SSD];
  TLatex* latex_threshold[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    mg[i] = new TMultiGraph();
    graph_pulser[i]     = new TGraph(NUM_STRIP, SSDCHNum[i], PulserPedestals[i]);
    graph_energycali[i] = new TGraph(NUM_STRIP, SSDCHNum[i], EnergyCaliPars[i]);
    graph_errors[i]     = new TGraph(NUM_STRIP, SSDCHNum[i], Errors[i]);
    graph_threshold[i]  = new TGraph(NUM_STRIP, SSDCHNum[i], Threshold_MeV[i]);

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
  TCanvas* cans[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
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
////////////////////////////////////////////////////////////////////////////////
//  1.前面已经通过 Pulser(Height) 与 alpha 能量点拟合得到能量刻度曲线，发现用 peak1 与
//    peak2 两点刻度的结果吻合得比较好，peak3的结果稍微有点偏差. 需要对刻度曲线参数进行评估.
//  2. peak3的偏差主要是由其较低的计数引起的，因此后续的讨论中，只讨论 peak1 与 peak2 的结果.
//     评价内容包括：
//    (1) peak1 与 peak2 拟合结果的差异. 由此确定最终使用 peak1 的结果，还是采用 peak1
//        与 peak2 的平均值
//    (2) 评估每一个 SSD 每个 CH 的刻度参数分布，给出参数的平均值，评估同一个 SSD 不同 CH
//        之间的参数差异
//
//  gfh, 2020-06-17
//  修改成类, 2020-10-03
////////////////////////////////////////////////////////////////////////////////
void CSHINESiEnergyCali::EstimateFitPars(const char* layertag, const char* pulserfiletag)
{
  EstimateErrorOfPeak1AndPeak2(layertag, pulserfiletag);
  EstimateParameterDistributions(layertag, pulserfiletag);
}

//______________________________________________________________________________
void CSHINESiEnergyCali::EstimateErrorOfPeak1AndPeak2(const char* layertag, const char* pulserfiletag)
{
  std::string pathParsInput(Form("%sdata_SiEnergyCali/SSD_%s_%sAndAlphaFitPars.dat",
                            PATHDATAFOLDER,layertag,pulserfiletag));
  std::string pathPNGOutput(Form("%sfigure_SiEnergyCali/SSD_%s_ParErrorsOfPeak1AndPeak2.png",
                            PATHFIGURESFOLDER,layertag));
  //____________________________________________________________________________
  Int_t numpar_ParsInput = 6;
  ReadFileModule readfile;
  Double_t*** ParsInput = readfile.ReadData(pathParsInput.c_str(),NUM_SSD,NUM_STRIP,numpar_ParsInput);

  Double_t SSDCHNum[NUM_SSD][NUM_STRIP];
  Double_t a_RelativeErr[NUM_SSD][NUM_STRIP];
  Double_t b_RelativeErr[NUM_SSD][NUM_STRIP];
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
      SSDCHNum[i][j]=j+1;
      a_RelativeErr[i][j] =100*abs((ParsInput[i][j][2]-ParsInput[i][j][0])/ParsInput[i][j][0]); // 百分比%
      b_RelativeErr[i][j] =100*abs((ParsInput[i][j][3]-ParsInput[i][j][1])/ParsInput[i][j][1]); // 百分比%
    }
  }
  TGraph* graph_a_Err[NUM_SSD];
  TGraph* graph_b_Err[NUM_SSD];
  TMultiGraph* multigraph[NUM_SSD];
  TCanvas* cans = new TCanvas("cans","Pars Error of Peak1 and Peak2",1200,1000);
  cans->Divide(2,2);
  for (Int_t i=0; i<NUM_SSD; i++) {
    cans->cd(i+1);
    gPad->SetGridx();
    graph_a_Err[i] = new TGraph(NUM_STRIP, SSDCHNum[i], a_RelativeErr[i]);
    graph_b_Err[i] = new TGraph(NUM_STRIP, SSDCHNum[i], b_RelativeErr[i]);

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
    multigraph[i]->GetYaxis()->SetTitle("Relative Errors (%%)");
    multigraph[i]->GetYaxis()->CenterTitle(1);
    multigraph[i]->GetYaxis()->SetTitleSize(0.04);

    TLegend* legend = new TLegend(0.65,0.73,0.88,0.88);
    legend->AddEntry(graph_a_Err[i],"a_Err","lp");
    legend->AddEntry(graph_b_Err[i],"b_Err","lp");

    TLatex* latex = new TLatex(3.,1.5,"a_Err = b_Err = #left|#frac{c_{2}-c_{1}}{c_{1}}#right|");
    latex->SetTextColor(kMagenta);

    TLine* line = new TLine(1,1.0,16,1.0);
    line->SetLineColor(kGreen);
    line->SetLineStyle(7);
    line->SetLineWidth(3);

    multigraph[i]->Draw("APL");
    legend->Draw("SAME");
    line->Draw("SAME");
    latex->Draw();
  }
  cans->Print(pathPNGOutput.c_str());
}

//______________________________________________________________________________
void CSHINESiEnergyCali::EstimateParameterDistributions(const char* layertag, const char* pulserfiletag)
{
  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string pathParsInput(Form("%sdata_SiEnergyCali/SSD_%s_%sAndAlphaFitPars.dat",
                            PATHDATAFOLDER,layertag,pulserfiletag));
  std::string pathPNGOutput(Form("%sfigure_SiEnergyCali/SSD_%s_Peak1ParameterDistributions.png",
                            PATHFIGURESFOLDER,layertag));
  // 手动设定系数 a 与 b 的范围
  Double_t YRangeLow      = -22.;
  Double_t YRangeUp       =  22.;
  Double_t YScaleRangeLow =  2.;
  Double_t YScaleRangeUp  = -2.;

  Double_t L1S_a_rangeLow[4] = { 0.14, 0.073, 0.027, 0.018};
  Double_t L1S_a_rangeUp [4] = { 0.15, 0.085, 0.033, 0.024};
  Double_t L1S_b_rangeLow[4] = {-18., -7.,   -3.4,  -3.};
  Double_t L1S_b_rangeUp [4] = {-8.,  -3.,   -1.6,  -1.};

  Double_t L2F_a_rangeLow[4] = { 0.068, 0.05,  0.028, 0.015};
  Double_t L2F_a_rangeUp [4] = { 0.072, 0.07,  0.030, 0.022};
  Double_t L2F_b_rangeLow[4] = {-7.0,  -5.0,  -3.0,  -2.0};
  Double_t L2F_b_rangeUp [4] = {-2.5,  -2.5,  -1.0,  -1.0};

  Double_t L2B_a_rangeLow[4] = { 0.055, 0.045, 0.028, 0.015};
  Double_t L2B_a_rangeUp [4] = { 0.065, 0.050, 0.033, 0.017};
  Double_t L2B_b_rangeLow[4] = {-8.0,  -6.0,  -3.0,  -2.5};
  Double_t L2B_b_rangeUp [4] = {-3.0,  -2.0,   0.0,  -1.0};
  //____________________________________________________________________________
  Double_t a_rangeLow[NUM_SSD];
  Double_t a_rangeUp [NUM_SSD];
  Double_t b_rangeLow[NUM_SSD];
  Double_t b_rangeUp [NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    if (strcmp(layertag, L1STag.c_str())==0) {
      a_rangeLow[i] = L1S_a_rangeLow[i];
      a_rangeUp [i] = L1S_a_rangeUp [i];
      b_rangeLow[i] = L1S_b_rangeLow[i];
      b_rangeUp [i] = L1S_b_rangeUp [i];
    } else if (strcmp(layertag, L2FTag.c_str())==0) {
      a_rangeLow[i] = L2F_a_rangeLow[i];
      a_rangeUp [i] = L2F_a_rangeUp [i];
      b_rangeLow[i] = L2F_b_rangeLow[i];
      b_rangeUp [i] = L2F_b_rangeUp [i];
    } else {
      a_rangeLow[i] = L2B_a_rangeLow[i];
      a_rangeUp [i] = L2B_a_rangeUp [i];
      b_rangeLow[i] = L2B_b_rangeLow[i];
      b_rangeUp [i] = L2B_b_rangeUp [i];
    }
  }
  Int_t numpar_ParsInput = 6;
  ReadFileModule readfile;
  Double_t*** ParsInput = readfile.ReadData(pathParsInput.c_str(),NUM_SSD,NUM_STRIP,numpar_ParsInput);

  Double_t SSDCHNum[NUM_SSD][NUM_STRIP];
  Double_t a[NUM_SSD][NUM_STRIP];  // 能量刻度曲线的斜率
  Double_t b[NUM_SSD][NUM_STRIP];  // 能量刻度曲线的截距
  Double_t a_average[NUM_SSD];
  Double_t b_average[NUM_SSD];
  Double_t a_sum[NUM_SSD];
  Double_t b_sum[NUM_SSD];
  Double_t a_scalefactor[NUM_SSD];
  Double_t b_scalefactor[NUM_SSD];
  Double_t a_scale[NUM_SSD][NUM_STRIP];
  Double_t b_scale[NUM_SSD][NUM_STRIP];
  Double_t a_average_scale[NUM_SSD];
  Double_t b_average_scale[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    a_sum[i] = 0;
    b_sum[i] = 0;
    a_scalefactor[i] = (YRangeUp-YScaleRangeLow) /(a_rangeUp[i]-a_rangeLow[i]);
    b_scalefactor[i] = (YScaleRangeUp-YRangeLow)/(b_rangeUp[i]-b_rangeLow[i]);
    for (Int_t j=0; j<NUM_STRIP; j++) {
      SSDCHNum[i][j]=j+1;
      a[i][j] = ParsInput[i][j][0];  // a of Peak1
      b[i][j] = ParsInput[i][j][1];  // a of Peak1
      a_scale[i][j] = (a[i][j] - a_rangeLow[i]) * a_scalefactor[i] + 2;
      b_scale[i][j] = (b[i][j] - b_rangeUp[i])  * b_scalefactor[i] - 2;
      if (i==3 && j==0 && (strcmp(layertag,L1STag.c_str())==0)) continue; // SSD4_L1S_CH00 无信号, 应剔除该道
      a_sum[i] += a[i][j];
      b_sum[i] += b[i][j];
    }
    if ((i==3) && (strcmp(layertag,L1STag.c_str())==0)) {
      a_average[i] = a_sum[i]/15;
      b_average[i] = b_sum[i]/15;
    } else {
      a_average[i] = a_sum[i]/NUM_STRIP;
      b_average[i] = b_sum[i]/NUM_STRIP;
    }
    a_average_scale[i] = (a_average[i]-a_rangeLow[i]) * a_scalefactor[i] + 2;
    b_average_scale[i] = (b_average[i]-b_rangeUp [i]) * b_scalefactor[i] - 2;
    printf("a_average[%d] = %6f, b_average[%d] = %6f\n",i+1,a_average[i],i+1,b_average[i]);
  }
  TGraph*      a_scale_distribution[NUM_SSD];
  TGraph*      b_scale_distribution[NUM_SSD];
  TMultiGraph* mg[NUM_SSD];
  TGaxis*      a_Yaxis[NUM_SSD];
  TGaxis*      b_Yaxis[NUM_SSD];
  TLine*       line_a_average[NUM_SSD];
  TLine*       line_b_average[NUM_SSD];
  TLatex*      latex_a[NUM_SSD];
  TLatex*      latex_b[NUM_SSD];

  TCanvas* cans = new TCanvas("cans","Pars Distributions",1200,1000);
  cans->Divide(2,2);
  for (Int_t i=0; i<NUM_SSD; i++) {
    cans->cd(i+1);
    gPad->SetGridx();

    a_scale_distribution[i] = new TGraph(NUM_STRIP, SSDCHNum[i], a_scale[i]);
    b_scale_distribution[i] = new TGraph(NUM_STRIP, SSDCHNum[i], b_scale[i]);

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
    mg[i] = new TMultiGraph();
    mg[i]->Add(a_scale_distribution[i]);
    mg[i]->Add(b_scale_distribution[i]);

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

    a_Yaxis[i] = new TGaxis(gPad->GetUxmin(),YScaleRangeLow,gPad->GetUxmin(),YRangeUp,
                            a_rangeLow[i],a_rangeUp[i],1002,"-R");
    b_Yaxis[i] = new TGaxis(gPad->GetUxmin(),YRangeLow,gPad->GetUxmin(),YScaleRangeUp,
                            b_rangeLow[i],b_rangeUp[i],1002,"-R");
    a_Yaxis[i]->SetLineColor (kBlue);
    a_Yaxis[i]->SetLabelColor(kBlue);
    b_Yaxis[i]->SetLineColor (kRed);
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
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//
// 硅条的能量刻度已基本完成，现在需要对异常的 Channels 进行评估、寻找原因.
// 异常的 Channels 分别是:
//  1. Pulser 刻度: SSD2_L2F_CH00,CH01  这两道脉冲峰值约是其他道的一半
//  2. Alpha 刻度:  SSD4_L2F_CH13,CH14  这两道有两组 alpha 峰，此前的刻度使用较大的一组 alpha 峰
//  3. Alpha 刻度:  SSD4_L1S_CH00-CH15  前后两次刻度有一个 shift, 此前的刻度使用 AlphaCali05_08
//
// 评估的内容包括：
//  (1) 分别修正 alpha 能量点的 ADC 道址 与 能量    => 结果: 没找到修正方法, 暂不作修正
//  (2) 将两组 alpha 峰峰值相加, 在对这两道进行刻度  => 结果: peak1Low + peak1Up - pedestal
//  (3) 对于 SSD4_L1S, 使用 AlphaCali00_04 再进行刻度, 与AlphaCali05_08进行对比
//      => 结果: 没找到修正方法, 暂使用 AlphaCali05_08
//
// gfh, 2020-06-18
// 修改成类, 2020-10-04
////////////////////////////////////////////////////////////////////////////////
void CSHINESiEnergyCali::CheckAbnormalChannels(const char* pulserfiletag)
{
  Check_SSD2_L2F_CH00_CH01(pulserfiletag);
  Check_SSD4_L2F_CH13_CH14(pulserfiletag);
  Check_SSD4_L1S_CH00_CH15_AlphaCali00_04();
}

//______________________________________________________________________________
void CSHINESiEnergyCali::Check_SSD2_L2F_CH00_CH01(const char* pulserfiletag)
{
  std::string L2FTag("L2F");
  std::string AlphaCali00_48("AlphaCali00_48");

  std::string pathPulserIn(Form("%sdata_PulserCali/SSD_%s_PulserCali_%s.dat",PATHDATAFOLDER,L2FTag.c_str(),pulserfiletag));
  std::string pathAlphaCali00_48(Form("%sdata_AlphaCali/SSD_%s_AlphaPeaks_%s.dat",PATHDATAFOLDER,L2FTag.c_str(),AlphaCali00_48.c_str()));
  std::string pathAlphaEnergiesIn(Form("%sdata_AlphaCali/SSD_AlphaEnergies.dat",PATHDATAFOLDER));
  std::string pathEnergyCaliPars(Form("%sdata_SiEnergyCali/SSD_%s_HeightAndAlphaFitPars.dat",PATHDATAFOLDER,L2FTag.c_str()));
  std::string pathpngPulserCali_SSD2_L2F_CH00_01(Form("%sfigure_SiEnergyCali/CheckPulserCali_SSD2_%s_CH00_CH01_%s.png",PATHFIGURESFOLDER,L2FTag.c_str(),pulserfiletag));
  std::string pathPNGSSD2_L2F_CH00_01_Pars(Form("%sfigure_SiEnergyCali/Check_SSD2_%s_CH00_CH01_Peak1ParameterDistributions.png",PATHFIGURESFOLDER,L2FTag.c_str()));

  Int_t num_peaks             =  11;
  Int_t num_channel           =  2;
  Int_t numpar_PulserIn       =  15;  // a, err_a, b,err_b, pulser1...pulser11
  Int_t numpar_AlphaCali00_48 =  3;  // peak1, peak2, peak3
  Int_t numpar_AlphaEnergies  =  3;  // E1, E2, E3
  Int_t numpar_EnergyCaliPars =  2;  // E1, E2, E3

  // 手动设定系数 a 与 b 的范围
  Double_t YRangeLow           = -22.;
  Double_t YRangeUp            =  22.;
  Double_t YScaleRangeLow      =  2.;
  Double_t YScaleRangeUp       = -2.;
  Double_t SSD2_L2F_k_RangeLow =  0.05;
  Double_t SSD2_L2F_k_RangeUp  =  0.07;
  Double_t SSD2_L2F_h_RangeLow = -5.0;
  Double_t SSD2_L2F_h_RangeUp  = -2.5;
  Double_t E1_DeltaCH          =  15.;
  Double_t E1_DeltaE           =  0.85;

  Double_t k_scalfactor = (YRangeUp-YScaleRangeLow) /(SSD2_L2F_k_RangeUp-SSD2_L2F_k_RangeLow);
  Double_t h_scalfactor = (YScaleRangeUp-YRangeLow) /(SSD2_L2F_h_RangeUp-SSD2_L2F_h_RangeLow);
  Double_t Height11[11] = {1./2, .9/2, .8/2, .7/2, .6/2, .5/2, .4/2, .3/2, .2/2, .1/2, 0.05/2};
  //Double_t Height11[11] = {1., .9, .8, .7, .6, .5, .4, .3, .2, .1, 0.05};
  //____________________________________________________________________________
  ReadFileModule readfile;
  Double_t*** PulserIn       = readfile.ReadData(pathPulserIn.c_str(),       NUM_SSD,NUM_STRIP,numpar_PulserIn);
  Double_t*** AlphaCHIn00_48 = readfile.ReadData(pathAlphaCali00_48.c_str(), NUM_SSD,NUM_STRIP,numpar_AlphaCali00_48);
  Double_t*** AlphaEIn       = readfile.ReadData(pathAlphaEnergiesIn.c_str(),NUM_SSD,NUM_STRIP,numpar_AlphaEnergies);
  Double_t*** EnergyCaliPars = readfile.ReadData(pathEnergyCaliPars.c_str(), NUM_SSD,NUM_STRIP,numpar_EnergyCaliPars);

  Double_t k1_check[num_channel];
  Double_t h1_check[num_channel];
  Double_t k1_DeltaCh[num_channel];
  Double_t h1_DeltaCh[num_channel];
  Double_t k1_DeltaE[num_channel];
  Double_t h1_DeltaE[num_channel];

  Double_t peak_ch[num_channel][num_peaks];
  for (Int_t i=0; i<num_channel; i++) {
    for (Int_t j=0; j<num_peaks; j++) {
      peak_ch[i][j] = PulserIn[1][i][j+4];  // SSD2_L2F_CH00, peak1...peak11
    }
  }
  //_____________________________________
  // for the pulser calibrations
  TF1*    fit_ch[num_channel];
  TGraph* graph_ch  [num_channel];
  TLatex* a_latex_ch[num_channel];
  TLatex* b_latex_ch[num_channel];
  for (Int_t i=0; i<num_channel; i++) {
    fit_ch[i]   = new TF1(Form("fit_ch%02d",i),"pol1",0,2000);
    graph_ch[i] = new TGraph(num_peaks, peak_ch[i], Height11);
    graph_ch[i]->SetTitle(Form("PulserCali_SSD_%s_CH00_%s",L2FTag.c_str(),pulserfiletag));
    graph_ch[i]->Fit(Form("fit_ch%02d",i));
    a_latex_ch[i] = new TLatex(600,0.8,Form("a = %.8f",fit_ch[i]->GetParameter(1)));
    b_latex_ch[i] = new TLatex(600,0.7,Form("b = %.8f",fit_ch[i]->GetParameter(0)));
  }
  // get all the pars of SSD2_L2F
  Double_t k[NUM_STRIP];
  Double_t h[NUM_STRIP];
  Double_t k_scale [NUM_STRIP];
  Double_t h_scale [NUM_STRIP];
  Double_t SSDCHNum[NUM_STRIP];

  Double_t k_scaleCH[num_channel];
//Double_t h_scaleCH[num_channel];
  Double_t k_scaleE [num_channel];
//Double_t h_scaleE [num_channel];

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
  for (Int_t i=0; i<num_channel; i++) {
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
  //h_scaleCH[i] = (h1_DeltaCh[i] - SSD2_L2F_h_RangeUp)  * h_scalfactor - 2;

    k_scaleE[i] = (k1_DeltaE[i] - SSD2_L2F_k_RangeLow) * k_scalfactor + 2;
  //h_scaleE[i] = (h1_DeltaE[i] - SSD2_L2F_h_RangeUp)  * h_scalfactor - 2;
  }
  for (Int_t i=0; i<NUM_STRIP; i++) {
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
  k_average = k_sum/NUM_STRIP;
  h_average = h_sum/NUM_STRIP;
  k_average_scale = (k_average - SSD2_L2F_k_RangeLow) * k_scalfactor + 2.;
  h_average_scale = (h_average - SSD2_L2F_h_RangeUp)  * h_scalfactor - 2.;

  k_average14 = (k_sum-k[0]-k[1])/14;
  h_average14 = (h_sum-h[0]-h[1])/14;
  k_average14_scale = (k_average14 - SSD2_L2F_k_RangeLow) * k_scalfactor + 2.;
  h_average14_scale = (h_average14 - SSD2_L2F_h_RangeUp)  * h_scalfactor - 2.;

  k_averageDeltaCH = (k_sum-k[0]-k[1] + k1_DeltaCh[0] + k1_DeltaCh[1])/NUM_STRIP;
  h_averageDeltaCH = (h_sum-h[0]-h[1] + h1_DeltaCh[0] + h1_DeltaCh[1])/NUM_STRIP;
  k_averageDeltaCH_scale = (k_averageDeltaCH - SSD2_L2F_k_RangeLow) * k_scalfactor + 2.;
  h_averageDeltaCH_scale = (h_averageDeltaCH - SSD2_L2F_h_RangeUp)  * h_scalfactor - 2.;

  k_averageDeltaE = (k_sum-k[0]-k[1] + k1_DeltaE[0] + k1_DeltaE[1])/NUM_STRIP;
  h_averageDeltaE = (h_sum-h[0]-h[1] + h1_DeltaE[0] + h1_DeltaE[1])/NUM_STRIP;
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
  TGraph* k_scale_distribution = new TGraph(NUM_STRIP, SSDCHNum, k_scale);
  TGraph* h_scale_distribution = new TGraph(NUM_STRIP, SSDCHNum, h_scale);
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
  for (Int_t i=0; i<num_channel; i++) {
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

//______________________________________________________________________________
void CSHINESiEnergyCali::Check_SSD4_L2F_CH13_CH14(const char* pulserfiletag)
{
  std::string L2FTag("L2F");
  std::string AlphaCali00_48("AlphaCali00_48");

  std::string pathPulserIn(Form("%sdata_PulserCali/SSD_%s_PulserCali_%s.dat",PATHDATAFOLDER,L2FTag.c_str(),pulserfiletag));
  std::string pathAlphaCali00_48(Form("%sdata_AlphaCali/SSD_%s_AlphaPeaks_%s.dat",PATHDATAFOLDER,L2FTag.c_str(),AlphaCali00_48.c_str()));
  std::string pathAlphaEnergiesIn(Form("%sdata_AlphaCali/SSD_AlphaEnergies.dat",PATHDATAFOLDER));
  std::string pathEnergyCaliPars(Form("%sdata_SiEnergyCali/SSD_%s_HeightAndAlphaFitPars.dat",PATHDATAFOLDER,L2FTag.c_str()));
  std::string pathPulserPedestalIn(Form("%sdata_PulserCali/SSD_%s_PulserCaliPedestals_Pedestal0000.dat",PATHDATAFOLDER,L2FTag.c_str()));
  std::string pathPNGSSD4_L2F_CH13_14_Pars(Form("%sfigure_SiEnergyCali/Check_SSD4_%s_CH13_CH14_Peak1ParameterDistributions.png",PATHFIGURESFOLDER,L2FTag.c_str()));

  // 手动设定系数 a 与 b 的范围
  Double_t YRangeLow           = -22.;
  Double_t YRangeUp            =  22.;
  Double_t YScaleRangeLow      =  2.;
  Double_t YScaleRangeUp       = -2.;
  Double_t SSD4_L2F_k_RangeLow =  0.015;
  Double_t SSD4_L2F_k_RangeUp  =  0.022;
  Double_t SSD4_L2F_h_RangeLow = -2.0;
  Double_t SSD4_L2F_h_RangeUp  = -1.0;
  Double_t LowPeaks_SSD4_L2F_CH13_CH14[2] = {166.966, 168.828};

  Int_t num_channel           =  2;
  Int_t numpar_PulserIn       =  3;  // a, err_a, b,err_b, pulser1...pulser11
  Int_t numpar_AlphaCali00_48 =  3;  // peak1, peak2, peak3
  Int_t numpar_AlphaEnergies  =  3;  // E1, E2, E3
  Int_t numpar_EnergyCaliPars =  2;  // k1, h1
  Int_t numpar_PulserPedestal =  2;  // mean, sigma

  ReadFileModule readfile;
  Double_t*** PulserIn       = readfile.ReadData(pathPulserIn.c_str(),        NUM_SSD,NUM_STRIP,numpar_PulserIn);
  Double_t*** AlphaCHIn00_48 = readfile.ReadData(pathAlphaCali00_48.c_str(),  NUM_SSD,NUM_STRIP,numpar_AlphaCali00_48);
  Double_t*** AlphaEIn       = readfile.ReadData(pathAlphaEnergiesIn.c_str(), NUM_SSD,NUM_STRIP,numpar_AlphaEnergies);
  Double_t*** EnergyCaliPars = readfile.ReadData(pathEnergyCaliPars.c_str(),  NUM_SSD,NUM_STRIP,numpar_EnergyCaliPars);
  Double_t*** PulserPedestal = readfile.ReadData(pathPulserPedestalIn.c_str(),NUM_SSD,NUM_STRIP,numpar_PulserPedestal);

  Double_t k[NUM_STRIP];
  Double_t h[NUM_STRIP];
  Double_t k_scale [NUM_STRIP];
  Double_t h_scale [NUM_STRIP];
  Double_t SSDCHNum[NUM_STRIP];

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
  for (Int_t i=0; i<NUM_STRIP; i++) {
    SSDCHNum[i] = i+1;
    k[i] = EnergyCaliPars[3][i][0];
    h[i] = EnergyCaliPars[3][i][1];

    k_scale[i] = (k[i] - SSD4_L2F_k_RangeLow) * k_scalfactor + 2;
    h_scale[i] = (h[i] - SSD4_L2F_h_RangeUp)  * h_scalfactor - 2;

    k_sum += k[i];
    h_sum += h[i];
  }
  k_average = k_sum/NUM_STRIP;
  h_average = h_sum/NUM_STRIP;
  k_average_scale = (k_average - SSD4_L2F_k_RangeLow) * k_scalfactor + 2;
  h_average_scale = (h_average - SSD4_L2F_h_RangeUp)  * h_scalfactor - 2;

  k_average14 = (k_sum-k[13]-k[14])/14;
  h_average14 = (h_sum-h[13]-h[14])/14;
  k_average14_scale = (k_average14 - SSD4_L2F_k_RangeLow) * k_scalfactor + 2;
  h_average14_scale = (h_average14 - SSD4_L2F_h_RangeUp)  * h_scalfactor - 2;
  //__________________________________________
  // for cans_distribution_SSD4->cd(2);
  for (Int_t i=0; i<num_channel; i++) {
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
  k_SumPeaks_average = (k_sum-k[13]-k[14]+k_SumPeaks[0]+k_SumPeaks[1])/NUM_STRIP;
  h_SumPeaks_average = (h_sum-h[13]-h[14]+h_SumPeaks[0]+h_SumPeaks[1])/NUM_STRIP;
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
  TGraph* k_scale_distribution = new TGraph(NUM_STRIP, SSDCHNum, k_scale);
  TGraph* h_scale_distribution = new TGraph(NUM_STRIP, SSDCHNum, h_scale);
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

//______________________________________________________________________________
void CSHINESiEnergyCali::Check_SSD4_L1S_CH00_CH15_AlphaCali00_04()
{
  std::string L1STag("L1S");
  std::string AlphaCali00_04("AlphaCali00_04");
  std::string AlphaCali05_08("AlphaCali05_08");
  std::string pathEnergyCaliPars05_08(Form("%sdata_SiEnergyCali/SSD_%s_HeightAndAlphaFitPars.dat",PATHDATAFOLDER,L1STag.c_str()));
  std::string pathEnergyCaliPars00_04(Form("%sdata_SiEnergyCali/SSD_%s_HeightAndAlphaFitPars_%s.dat",PATHDATAFOLDER,L1STag.c_str(),AlphaCali00_04.c_str()));
  std::string pathPNGOutput(Form("%sfigure_SiEnergyCali/Check_SSD4_%s_ParErrorsOf%sAnd%s.png",PATHFIGURESFOLDER,L1STag.c_str(),AlphaCali00_04.c_str(),AlphaCali05_08.c_str()));
  //____________________________________________________________________________
  Int_t numpar_ParsInput05_08 = 2;
  Int_t numpar_ParsInput00_04 = 2;
  ReadFileModule readfile;
  Double_t*** ParsInput05_08 = readfile.ReadData(pathEnergyCaliPars05_08.c_str(),NUM_SSD,NUM_STRIP,numpar_ParsInput05_08);
  Double_t*** ParsInput00_04 = readfile.ReadData(pathEnergyCaliPars00_04.c_str(),NUM_SSD,NUM_STRIP,numpar_ParsInput00_04);

  Double_t SSDCHNum[NUM_STRIP];
  Double_t k_05_08 [NUM_STRIP];
  Double_t h_05_08 [NUM_STRIP];
  Double_t k_00_04 [NUM_STRIP];
  Double_t h_00_04 [NUM_STRIP];
  Double_t k_RelativeErr[NUM_STRIP];
  Double_t h_RelativeErr[NUM_STRIP];
  for (Int_t i=0; i<NUM_STRIP; i++) {
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
  TGraph* graph_k_05_08 = new TGraph(NUM_STRIP,SSDCHNum,k_05_08);
  TGraph* graph_h_05_08 = new TGraph(NUM_STRIP,SSDCHNum,h_05_08);
  TGraph* graph_k_00_04 = new TGraph(NUM_STRIP,SSDCHNum,k_00_04);
  TGraph* graph_h_00_04 = new TGraph(NUM_STRIP,SSDCHNum,h_00_04);
  TGraph* graph_k_RelativeErr = new TGraph(NUM_STRIP,SSDCHNum,k_RelativeErr);
  TGraph* graph_h_RelativeErr = new TGraph(NUM_STRIP,SSDCHNum,h_RelativeErr);

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
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//  1. 能量刻度的准确性主要由三大因素决定:
//            (1) 脉冲刻度的线性 (2)alpha 能量的准确性 (3)alpha 拟合道址的准确性
//  2. 从结果看, 脉冲刻度的线性性很好. 因此，现在来考察alpha能量的准确性对刻度的影响!
//  3. alpha 能量的准确性来源于两部分:
//     (1) alpha 标准能量的准确性. 需要查表, 目前采用的是 刘运祚老师的
//        《常用放射性核素衰变纲图（1982）》. 年代较久远, 数据可能不准确, 不过与其他数据对比,
//         差异仅出现在小数点后第3位. 应该比较准确. 这里将不对此因素进行分析！
//     (2)即使 alpha 标准能量是准确的, 但需要考虑硅条的死层的影响, 这是影响 alpha 能量的最
//        主要因素. 这里将重点考虑死层对 alpha 能量修正对刻度的影响
//  4. 考察思路:
//     (a) 调整死层的厚度, 使用 LiseModule 计算 alpha 的能损, 从而确定 alpha 入射能量;
//     (b) 计算不同 alpha 能量下, peak1 与 peak2 刻度系数的误差;
//     (c) 当误差小于某一设定值时, 认为当前死层厚度为真实的死层厚度
//     (d) 记录当前的死层厚度, 以及 alpha 能损等信息
//  5. 在正式分析死层效应之前, 先回过头看看 pulser 刻度的线性性, 即考察每个 pulser 点相对
//     pulser 刻度曲线的偏离情况 (PulserCali_CheckLinearity.C)
//
//  gfh, 2020-06-20
//  修改成类, 2020-10-04
////////////////////////////////////////////////////////////////////////////////
void CSHINESiEnergyCali::EstimateDeadlayerEffects(const char* layertag, const char* pulserfiletag)
{
  //___________________________________
  // 进过镀铝 Mylar 膜之后的 alpha 能量
  // 数据直接从 SSD_AlphaEnergies.dat 中复制过来
  Double_t E1 = 4.90407; // MeV
  Double_t E2 = 5.24768; // MeV

  Int_t Z_Alpha = 2;
  Int_t A_Alpha = 4;
  Int_t Deadlayer_num       = 500;
  Double_t Sideadlayer_step = 0.01; // um
  Double_t RelativeErrCut   = 0.005;

  std::string pathPNDOutput(Form("%sfigure_SiEnergyCali/SSD_%s_DeadLayerEffectsOntheParErrs.png",PATHFIGURESFOLDER,layertag));
  std::string pathFitFuncPDFOutput(Form("%sfigure_SiEnergyCali/SSD_%s_DeadLayerEffectsOnFitFunc.pdf",PATHFIGURESFOLDER,layertag));
  std::string pathFitFuncPDFOutputBegin(Form("%sfigure_SiEnergyCali/SSD_%s_DeadLayerEffectsOnFitFunc.pdf[",PATHFIGURESFOLDER,layertag));
  std::string pathFitFuncPDFOutputEnd(Form("%sfigure_SiEnergyCali/SSD_%s_DeadLayerEffectsOnFitFunc.pdf]",PATHFIGURESFOLDER,layertag));
  std::string pathDeadLayerParsOut(Form("%sdata_SiEnergyCali/SSD_%s_DeadLayerFitPars.dat",PATHDATAFOLDER,layertag));

  ofstream SiEnergyCaliPars(pathDeadLayerParsOut.c_str());
  SiEnergyCaliPars<<"* Consider the deadlayer effects on the fit parameters of peak1 and peak2 \n";
  SiEnergyCaliPars<<"* 1,2 represent using alpha peak1,peak2, respectively. \n";
  SiEnergyCaliPars<<"* SSDNum"<<setw(7)<<"CHNum"<<setw(10)<<"k1"<<setw(15)<<"h1"<<setw(15)<<"k2"<<setw(13)
                  <<"h2"<<setw(25)<<"<deadlayer>(um)"<<setw(18)<<"deadlayer(um)"<<setw(15)
                  <<"E1_ELoss"<<setw(20)<<"E2_ELoss\n";

  Double_t YRangeLow = 0.;
  Double_t YRangeUp  = 45.;
  Double_t YScaleRangeUp_Err         = 20.;
  Double_t YScaleRangeLow_DeadLayer  = 25.;

  Double_t DeadLayer_RangeLow   = 0.;
  Double_t DeadLayer_RangeUp    = 4.;
  Double_t RelativeErr_RangeLow = 0.;
  Double_t RelativeErr_RangeUp  = 1.5*RelativeErrCut*100;
  Double_t RelativeErr_RangeLow_L2F = 0.005*100;
  Double_t RelativeErr_RangeUp_L2F  = 0.015*100;

  Double_t ScaleFactor_Err       = (YScaleRangeUp_Err-YRangeLow)/(RelativeErr_RangeUp-RelativeErr_RangeLow);
  Double_t ScaleFactor_DeadLayer = (YRangeUp-YScaleRangeLow_DeadLayer)/(DeadLayer_RangeUp-DeadLayer_RangeLow);
  Double_t ScaleFactor_Err_L2F   = (YScaleRangeUp_Err-YRangeLow)/(RelativeErr_RangeUp_L2F-RelativeErr_RangeLow_L2F);
  //____________________________________________________________________________
  Double_t k1[NUM_SSD][NUM_STRIP];
  Double_t k2[NUM_SSD][NUM_STRIP];
  Double_t h1[NUM_SSD][NUM_STRIP];
  Double_t h2[NUM_SSD][NUM_STRIP];
  Double_t c1[NUM_SSD][NUM_STRIP];
  Double_t c2[NUM_SSD][NUM_STRIP];

  Double_t SSDCHNum[NUM_SSD][NUM_STRIP];

  Double_t k_RelativeErr[NUM_SSD][NUM_STRIP];
  Double_t k_RelativeErr_Sum[NUM_SSD];
  Double_t k_RelativeErr_Ave[NUM_SSD];
  Double_t k_RelativeErr_scale[NUM_SSD][NUM_STRIP];
  Double_t k_RelativeErr_Ave_scale[NUM_SSD];

  Double_t deadlayer[NUM_SSD][NUM_STRIP];
  Double_t deadlayer_Sum[NUM_SSD];
  Double_t deadlayer_Ave[NUM_SSD];
  Double_t deadlayer_scale[NUM_SSD][NUM_STRIP];
  Double_t deadlayer_Ave_scale[NUM_SSD];

  Double_t energyloss1[NUM_SSD][NUM_STRIP];
  Double_t energyloss2[NUM_SSD][NUM_STRIP];

  Double_t E1_ELoss[Deadlayer_num];
  Double_t E2_ELoss[Deadlayer_num];
  Double_t E1_Residual[Deadlayer_num];
  Double_t E2_Residual[Deadlayer_num];

  Double_t RelativeErr[NUM_SSD][NUM_STRIP][Deadlayer_num];
  //_____________________________________
  // 计算不同死层厚度下的能损与剩余能量,
  EnergyLossModule LISEModule;
  for (Int_t i=0; i<Deadlayer_num; i++) {
    E1_ELoss[i] = LISEModule.GetEnergyLoss(Z_Alpha,A_Alpha,E1,"Si",i*Sideadlayer_step);
    E2_ELoss[i] = LISEModule.GetEnergyLoss(Z_Alpha,A_Alpha,E2,"Si",i*Sideadlayer_step);
    E1_Residual[i] = E1 - E1_ELoss[i];
    E2_Residual[i] = E2 - E2_ELoss[i];
  }
  //__________________________________________
  // 计算不同能量下, peak1 与 peak2 刻度参数的差异
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
      k1[i][j] = 0.; h1[i][j] = 0.;
      k2[i][j] = 0.; h2[i][j] = 0.;
      c1[i][j] = 0.; c2[i][j] = 0.;
      deadlayer  [i][j] = 0.;
      energyloss1[i][j] = 0.;
      energyloss2[i][j] = 0.;

      if (((i<2) && strcmp(layertag,"L2F")==0) ||
      ((i==1) && (j==15) && strcmp(layertag,"L2B")==0)) {                  // SSD1_L2F,SSD2_L2F peak2 刻度的斜率
        GetSiEnergyCaliPars(layertag,pulserfiletag,k1[i][j],h1[i][j],c1[i][j],i,j,E1,1); // 比 peak1 的大;
        GetSiEnergyCaliPars(layertag,pulserfiletag,k2[i][j],h2[i][j],c2[i][j],i,j,E2,2); // 死层修正无效, 因此不考虑死层的修正
        deadlayer[i][j]   = Sideadlayer_step * 0;                          // SSD2_L2B_CH15 同样无法考虑死层的修正
        cout<<setw(5)<<i<<setw(5)<<j<<setw(5)<<0<<setw(10)<<deadlayer[i][j]<<endl;
      } else {
        for (Int_t k=0; k<Deadlayer_num; k++) {
          GetSiEnergyCaliPars(layertag,pulserfiletag,k1[i][j],h1[i][j],c1[i][j],i,j,E1_Residual[k],1);
          GetSiEnergyCaliPars(layertag,pulserfiletag,k2[i][j],h2[i][j],c2[i][j],i,j,E2_Residual[k],2);
          RelativeErr[i][j][k] = abs((c1[i][j]-c2[i][j])/c1[i][j]);
          if (RelativeErr[i][j][k]<RelativeErrCut) {
            deadlayer[i][j]   = Sideadlayer_step * k;
            energyloss1[i][j] = E1_ELoss[k];
            energyloss2[i][j] = E2_ELoss[k];
            break;
          }
        }
      } // close "else"
    }  // close CHNum
  }  //  close NUM_SSD
  // 计算系数误差与平均deadlayer厚度
  for (Int_t i=0; i<NUM_SSD; i++) {
    deadlayer_Sum[i]     = 0.;
    k_RelativeErr_Sum[i] = 0.;
    for (Int_t j=0; j<NUM_STRIP; j++) {
      SSDCHNum[i][j] = j+1;
      k_RelativeErr[i][j]  = 100*abs((c1[i][j]-c2[i][j])/c1[i][j]); // 以 % 为单位
      k_RelativeErr_Sum[i]+= k_RelativeErr[i][j];
      deadlayer_Sum[i]    += deadlayer[i][j];
      // 计算用于 scale 画图所用的数据
      if  ((i<2) && strcmp(layertag,"L2F")==0) {
        k_RelativeErr_scale[i][j]  = (k_RelativeErr[i][j]-RelativeErr_RangeLow_L2F) *ScaleFactor_Err_L2F;
      } else {
        k_RelativeErr_scale[i][j]  = (k_RelativeErr[i][j]-RelativeErr_RangeLow) *ScaleFactor_Err;
      }
      deadlayer_scale[i][j] = (deadlayer[i][j]-DeadLayer_RangeLow)*ScaleFactor_DeadLayer + YScaleRangeLow_DeadLayer;
      cout<<setw(5)<<i<<setw(5)<<j<<setw(15)<<deadlayer[i][j]
          <<setw(15)<<abs((c1[i][j]-c2[i][j])/c1[i][j])
          <<setw(15)<<k1[i][j]<<setw(15)<<k2[i][j]
          <<setw(15)<<h1[i][j]<<setw(15)<<h2[i][j]<<endl;
    }
    // 计算每层硅条刻度参数相对误差与死层厚度的平均值
    if (i==3 && strcmp(layertag,"L1S")==0) { //SSD4_L1S_CH00为空
      k_RelativeErr_Ave[i] = (k_RelativeErr_Sum[i]-k_RelativeErr[i][0])/(NUM_STRIP-1);
      deadlayer_Ave[i]     = (deadlayer_Sum[i]-deadlayer[i][0])/(NUM_STRIP-1);
    } else {
      k_RelativeErr_Ave[i] = k_RelativeErr_Sum[i]/NUM_STRIP;
      deadlayer_Ave[i]     = deadlayer_Sum[i]/NUM_STRIP;
    }
    // 计算 scale 后的值
    if ((i<2) && strcmp(layertag,"L2F")==0) { // SSD1_L2F,SSD2_L2F 不考虑死层, 单独画
      k_RelativeErr_Ave_scale[i] = (k_RelativeErr_Ave[i]-RelativeErr_RangeLow_L2F)*ScaleFactor_Err_L2F;
    } else {
      k_RelativeErr_Ave_scale[i] = (k_RelativeErr_Ave[i]-RelativeErr_RangeLow)*ScaleFactor_Err;
    }
    deadlayer_Ave_scale[i] = (deadlayer_Ave[i]-DeadLayer_RangeLow)*ScaleFactor_DeadLayer + YScaleRangeLow_DeadLayer;
    // 输出经死层修正后的刻度参数
    for (Int_t j=0; j<NUM_STRIP; j++) {
      SiEnergyCaliPars<<setw(5) <<i<<setw(7)<<j<<setw(15)<<k1[i][j]<<setw(15)<<h1[i][j]
                      <<setw(15)<<k2[i][j]<<setw(15)<<h2[i][j]<<setw(15)<<deadlayer_Ave[i]
                      <<setw(18)<<deadlayer[i][j]<<setw(20)<<energyloss1[i][j]
                      <<setw(20)<<energyloss2[i][j]<<endl;
    }
  }
  //_______________________________________________________
  //  画图
  TMultiGraph* mg_scale[NUM_SSD];
  TGraph* graph_k_Err_scale[NUM_SSD];
  TGraph* graph_deadlayer_scale[NUM_SSD];

  TGaxis* k_Err_Yaxis[NUM_SSD];
  TGaxis* deadlayer_Yaxis[NUM_SSD];

  TLine*  line_Err_average[NUM_SSD];
  TLine*  line_deadlayer_average[NUM_SSD];
  TLine*  line0_Err[NUM_SSD];
  TLine*  line0_deadlayer[NUM_SSD];

  TLatex* latex_Err[NUM_SSD];
  TLatex* latex_deadlayer[NUM_SSD];

  TCanvas* cans = new TCanvas(Form("cans_%s",layertag),Form("cans_%s",layertag),1200,1000);
  cans->Divide(2,2);

  for (Int_t i=0; i<NUM_SSD; i++) {
    mg_scale[i] = new TMultiGraph();
    graph_k_Err_scale[i]     = new TGraph(NUM_STRIP, SSDCHNum[i], k_RelativeErr_scale[i]);
    graph_deadlayer_scale[i] = new TGraph(NUM_STRIP, SSDCHNum[i], deadlayer_scale[i]);

    graph_k_Err_scale[i]->SetMarkerStyle(20);
    graph_k_Err_scale[i]->SetMarkerColor(kBlue);
    graph_deadlayer_scale[i]->SetMarkerStyle(20);
    graph_deadlayer_scale[i]->SetMarkerColor(kRed);

    mg_scale[i]->Add(graph_k_Err_scale[i]);
    mg_scale[i]->Add(graph_deadlayer_scale[i]);

    cans->cd(i+1);
    gPad->SetGridx();
    mg_scale[i]->SetTitle(Form("SSD%d_%s_Peak1AndPeak2ParErrs_DeadLayer",i+1,layertag));
    mg_scale[i]->GetXaxis()->SetRangeUser(0.,17.);
    mg_scale[i]->GetXaxis()->SetNdivisions(117);

    auto hist_scale = graph_k_Err_scale[i]->GetHistogram();
    hist_scale->SetTitle(Form("SSD%d_%s_Peak1Peak2ParErrsWithDeadLayer",i+1,layertag));
    hist_scale->GetXaxis()->SetRangeUser(0.5,16.5);
    hist_scale->GetXaxis()->SetNdivisions(117);
    hist_scale->GetXaxis()->SetTitle("Channel");
    hist_scale->GetXaxis()->CenterTitle(1);
    hist_scale->GetXaxis()->SetTitleSize(0.04);
    hist_scale->GetYaxis()->SetRangeUser(YRangeLow,YRangeUp);
    hist_scale->Draw("Y+ H");
    gPad->Update();

    if  ((i<2) && strcmp(layertag,"L2F")==0) {
      k_Err_Yaxis[i]     = new TGaxis(gPad->GetUxmin(),YRangeLow,gPad->GetUxmin(),YScaleRangeUp_Err,
                                      RelativeErr_RangeLow_L2F,RelativeErr_RangeUp_L2F,1002,"-R");
    } else {
      k_Err_Yaxis[i]     = new TGaxis(gPad->GetUxmin(),YRangeLow,gPad->GetUxmin(),YScaleRangeUp_Err,
                                      RelativeErr_RangeLow,RelativeErr_RangeUp,1003,"-R");
    }
    deadlayer_Yaxis[i] = new TGaxis(gPad->GetUxmin(),YScaleRangeLow_DeadLayer,gPad->GetUxmin(),YRangeUp,
                                    DeadLayer_RangeLow,DeadLayer_RangeUp,1003,"-R");
    k_Err_Yaxis[i]->SetTitle("k_{RelativeErr} (%%)");
    k_Err_Yaxis[i]->CenterTitle(1);
    k_Err_Yaxis[i]->SetTitleSize(0.05);
    k_Err_Yaxis[i]->SetTitleColor(kBlue);
    k_Err_Yaxis[i]->SetLineColor (kBlue);
    k_Err_Yaxis[i]->SetLabelColor(kBlue);
    deadlayer_Yaxis[i]->SetTitle("Thickness(#mum)");
    deadlayer_Yaxis[i]->CenterTitle(1);
    deadlayer_Yaxis[i]->SetTitleSize(0.05);
    deadlayer_Yaxis[i]->SetTitleColor(kRed);
    deadlayer_Yaxis[i]->SetLineColor (kRed);
    deadlayer_Yaxis[i]->SetLabelColor(kRed);

    line_Err_average[i]       = new TLine(gPad->GetUxmin(),k_RelativeErr_Ave_scale[i],gPad->GetUxmax(),k_RelativeErr_Ave_scale[i]);
    line_deadlayer_average[i] = new TLine(gPad->GetUxmin(),deadlayer_Ave_scale[i],gPad->GetUxmax(),deadlayer_Ave_scale[i]);
    line0_Err[i]              = new TLine(gPad->GetUxmin(),YScaleRangeUp_Err,gPad->GetUxmax(),YScaleRangeUp_Err);
    line0_deadlayer[i]        = new TLine(gPad->GetUxmin(),YScaleRangeLow_DeadLayer,gPad->GetUxmax(),YScaleRangeLow_DeadLayer);
    line_Err_average[i]->SetLineColor(kGreen);
    line_Err_average[i]->SetLineStyle(7);
    line_Err_average[i]->SetLineWidth(3);
    line_deadlayer_average[i]->SetLineColor(kGreen);
    line_deadlayer_average[i]->SetLineStyle(7);
    line_deadlayer_average[i]->SetLineWidth(3);
    line0_Err[i]->SetLineColor(kBlue);
    line0_Err[i]->SetLineStyle(2);
    line0_deadlayer[i]->SetLineColor(kRed);
    line0_deadlayer[i]->SetLineStyle(2);

    latex_Err[i]       = new TLatex(3,15.,Form("<a_{RelativeErr}> = %.3f (%%)<%.2f (%%)",k_RelativeErr_Ave[i],RelativeErrCut*100));
    latex_deadlayer[i] = new TLatex(3,40.,Form("<deadlayer> = %.3f (#mum)",deadlayer_Ave[i]));
    latex_Err[i]->SetTextColor(kBlue);
    latex_deadlayer[i]->SetTextColor(kRed);

    mg_scale[i]->Draw("PL");
    k_Err_Yaxis[i]->Draw();
    deadlayer_Yaxis[i]->Draw();
    line_Err_average[i]->Draw();
    line_deadlayer_average[i]->Draw();
    line0_Err[i]->Draw();
    line0_deadlayer[i]->Draw();
    latex_Err[i]->Draw();
    latex_deadlayer[i]->Draw();
  }
  cans->Print(pathPNDOutput.c_str());

  //______________________________________
  // 画出死层修正后的能量刻度曲线
  TF1* fPeak1_deadlayer[NUM_SSD][NUM_STRIP];
  TF1* fPeak2_deadlayer[NUM_SSD][NUM_STRIP];

  TLegend* legend[NUM_SSD][NUM_STRIP];
  TLatex*  latex [NUM_SSD][NUM_STRIP];

  TCanvas* cans_func = new TCanvas("cans_func","cans_func",800,600);
  TCanvas* cans_begin = new TCanvas();
  TCanvas* cans_end   = new TCanvas();
  cans_begin->Close();
  cans_end->Close();

  cans_begin->Print(pathFitFuncPDFOutputBegin.c_str());
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
      fPeak1_deadlayer[i][j] = new TF1(Form("fPeak1_DeadLayer_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k1[i][j],h1[i][j]),0,4096);
      fPeak2_deadlayer[i][j] = new TF1(Form("fPeak2_DeadLayer_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k2[i][j],h2[i][j]),0,4096);
      fPeak1_deadlayer[i][j]->SetTitle(Form("SSD%d_%s_CH%02d_FitFunctionDeadLayer",i+1,layertag,j));
      fPeak1_deadlayer[i][j]->GetXaxis()->SetTitle("ADC CH");
      fPeak1_deadlayer[i][j]->GetXaxis()->CenterTitle(1);
      fPeak1_deadlayer[i][j]->GetYaxis()->SetTitle("Energy (MeV)");
      fPeak1_deadlayer[i][j]->GetYaxis()->CenterTitle(1);

      fPeak1_deadlayer[i][j]->SetLineColor(kRed);
      fPeak2_deadlayer[i][j]->SetLineColor(kGreen);

      legend[i][j] = new TLegend(0.15,0.7,0.55,0.85);
      legend[i][j]->AddEntry(fPeak1_deadlayer[i][j],"Pulser+AlphaPeak1","l");
      legend[i][j]->AddEntry(fPeak2_deadlayer[i][j],"Pulser+AlphaPeak2","l");

      latex[i][j] = new TLatex(1500.,0.2*fPeak1_deadlayer[i][j]->GetMaximum(),Form("deadlayer=%.5f(#mum)",deadlayer[i][j] ));
      latex[i][j]->SetTextColor(kMagenta);

      cans_func->cd();
      fPeak1_deadlayer[i][j]->Draw();
      fPeak2_deadlayer[i][j]->Draw("SAME");
      legend[i][j]->Draw("SAME");
      latex[i][j] ->Draw("SAME");

      gPad->Modified();
      gPad->Update();
      cans_func->Print(pathFitFuncPDFOutput.c_str());
    }
  }
  cans_end->Print(pathFitFuncPDFOutputEnd.c_str());
}

//______________________________________________________________________________
void CSHINESiEnergyCali::GetSiEnergyCaliPars(const char* layertag,const char* pulserfiletag,Double_t& k,
  Double_t& h,Double_t& c,Int_t ssdindex,Int_t chindex,Double_t E_MeV,Int_t peakindex)
{
  std::string AlphaCali00_08("AlphaCali00_08");
  std::string AlphaCali05_08("AlphaCali05_08");
  std::string AlphaCali00_48("AlphaCali00_48");

  std::string pathPulserIn(Form("%sdata_PulserCali/SSD_%s_PulserCali_%s.dat",PATHDATAFOLDER,layertag,pulserfiletag));
  std::string pathPulserInGain20;
  std::string pathAlphaCali00_08;
  std::string pathAlphaCali05_08;
  std::string pathAlphaCali00_48;

  Int_t numpar_PulserIn       = 3;  // a, err_a, b
  Int_t numpar_PulserInGain20 = 3;  // a, err_a, b
  Int_t numpar_AlphaCali00_08 = 3;  // peak1, peak2, peak3
  Int_t numpar_AlphaCali05_08 = 3;  // peak1, peak2, peak3
  Int_t numpar_AlphaCali00_48 = 3;  // peak1, peak2, peak3
  ReadFileModule readfile;
  Double_t*** PulserIn = readfile.ReadData(pathPulserIn.c_str(),NUM_SSD,NUM_STRIP,numpar_PulserIn);
  Double_t a = PulserIn[ssdindex][chindex][0];
  Double_t b = PulserIn[ssdindex][chindex][2];
  Double_t E_CH_00_08;
  Double_t E_CH_05_08;
  Double_t E_CH_00_48;
  //______________________________
  //  对 L1S 进行刻度
  if (strcmp(layertag,"L1S")==0) {
    pathPulserInGain20 = (Form("%sdata_PulserCali/SSD_%s_PulserReCali_Gain20_%s.dat",PATHDATAFOLDER,layertag,pulserfiletag));
    pathAlphaCali00_08 = (Form("%sdata_AlphaCali/SSD_%s_AlphaPeaks_%s.dat",PATHDATAFOLDER,layertag,AlphaCali00_08.c_str()));
    pathAlphaCali05_08 = (Form("%sdata_AlphaCali/SSD_%s_AlphaPeaks_%s.dat",PATHDATAFOLDER,layertag,AlphaCali05_08.c_str()));
    Double_t*** PulserInGain20 = readfile.ReadData(pathPulserInGain20.c_str(),NUM_SSD,NUM_STRIP,numpar_PulserInGain20);
    Double_t*** AlphaCHIn00_08 = readfile.ReadData(pathAlphaCali00_08.c_str(),NUM_SSD,NUM_STRIP,numpar_AlphaCali00_08);
    Double_t*** AlphaCHIn05_08 = readfile.ReadData(pathAlphaCali05_08.c_str(),NUM_SSD,NUM_STRIP,numpar_AlphaCali05_08);
    Double_t a_gain20 = PulserInGain20[ssdindex][chindex][0];
    Double_t b_gain20 = PulserInGain20[ssdindex][chindex][2];

    switch(peakindex) {
      case 1:   // 使用 peak1 进行刻度
          E_CH_00_08 = AlphaCHIn00_08[ssdindex][chindex][0];
          E_CH_05_08 = AlphaCHIn05_08[ssdindex][chindex][0];
          break;
      case 2:  // 使用 peak2 进行刻度
          E_CH_00_08 = AlphaCHIn00_08[ssdindex][chindex][1];
          E_CH_05_08 = AlphaCHIn05_08[ssdindex][chindex][1];
          break;
      default:
          cout<<Form("SSD%d_%s_%02d peak%d invalid!",ssdindex+1,layertag,chindex,peakindex);
    }
    // SSD1,SSD2,SSD3,SSD4 alpha 刻度文件使用不一样
    switch(ssdindex) {
      case 0:  // for SSD1,SSD2, 改变了gain
      case 1:
          c = E_MeV / (a_gain20 * E_CH_00_08 + b_gain20);
          break;
      case 2:  // SSD1,SSD2,SSD3 使用 AlphaCali00_08
          c = E_MeV / (a * E_CH_00_08 + b);
          break;
      case 3:   // SSD1,SSD2,SSD3 使用 AlphaCali00_08
          c = E_MeV / (a * E_CH_05_08 + b);
          break;
    }
    k = c * a;
    h = c * b;
  }
  //_______________________
  //  对 L2F, L2B 进行刻度
  if (strcmp(layertag,"L2F")==0 || strcmp(layertag,"L2B")==0) {
    pathAlphaCali00_48 = (Form("%sdata_AlphaCali/SSD_%s_AlphaPeaks_%s.dat",PATHDATAFOLDER,layertag,AlphaCali00_48.c_str()));
    Double_t*** AlphaCHIn00_48 = readfile.ReadData(pathAlphaCali00_48.c_str(),NUM_SSD,NUM_STRIP,numpar_AlphaCali00_48);
    switch(peakindex) {
      case 1:   // 使用 peak1 进行刻度
          E_CH_00_48 = AlphaCHIn00_48[ssdindex][chindex][0];
          break;
      case 2:  // 使用 peak2 进行刻度
          E_CH_00_48 = AlphaCHIn00_48[ssdindex][chindex][1];
          break;
      default:
          cout<<Form("SSD%d_%s_%02d peak%d invalid!",ssdindex+1,layertag,chindex,peakindex);
    }
    c = E_MeV / (a * E_CH_00_48 + b);
    k = c * a;
    h = c * b;
  }
}
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//  1. 能量刻度的准确性主要由三大因素决定:
//            (1) 脉冲刻度的线性 (2)alpha 能量的准确性 (3)alpha 拟合道址的准确性
//  2. 从结果看, 脉冲刻度的线性度很好. 因此，现在来考察alpha拟合道址的准确性对刻度的影响!
//  3.考察思路: 固定因素(1)(2)的不变, 对 alpha 拟合道址增加一个误差，如 +1ch，-1ch,
//    以此考察刻度曲线的变化
//
//  gfh, 2020-06-23
//  修改成类, 2020-10-04
////////////////////////////////////////////////////////////////////////////////
void CSHINESiEnergyCali::EstimateAlphaChannelEffOnFitPars(const char* layertag, const char* pulserfiletag)
{
  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string AlphaCali00_08("AlphaCali00_08");
  std::string AlphaCali05_08("AlphaCali05_08");
  std::string AlphaCali00_48("AlphaCali00_48");

  std::string pathPDFOutput(Form("%sfigure_SiEnergyCali/SSD_%s_AlphaCHEffectsOnFitPar.pdf",PATHFIGURESFOLDER,layertag));
  std::string pathPDFBegin(Form("%sfigure_SiEnergyCali/SSD_%s_AlphaCHEffectsOnFitPar.pdf[",PATHFIGURESFOLDER,layertag));
  std::string pathPDFEnd(Form("%sfigure_SiEnergyCali/SSD_%s_AlphaCHEffectsOnFitPar.pdf]"  ,PATHFIGURESFOLDER,layertag));

  std::string pathPulserIn(Form("%sdata_PulserCali/SSD_%s_PulserCali_%s.dat",PATHDATAFOLDER,layertag,pulserfiletag));
  std::string pathAlphaEnergiesIn(Form("%sdata_AlphaCali/SSD_AlphaEnergies.dat",PATHDATAFOLDER));
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
  Double_t*** PulserIn = readfile.ReadData(pathPulserIn.c_str(),NUM_SSD,NUM_STRIP,numpar_PulserIn);
  Double_t*** AlphaEIn = readfile.ReadData(pathAlphaEnergiesIn.c_str(),NUM_SSD,NUM_STRIP,numpar_AlphaEnergies);
  Double_t*** PulserInGain20;
  Double_t*** AlphaCHIn00_08;
  Double_t*** AlphaCHIn05_08;
  Double_t*** AlphaCHIn00_48;

  Double_t Ch_Err;
  if (strcmp(layertag,L1STag.c_str())==0) {
    Ch_Err = 0.5;  // 对拟合的 alpha 峰的道址给一个误差
    pathPulserInGain20 = Form("%sdata_PulserCali/SSD_%s_PulserReCali_Gain20_%s.dat",PATHDATAFOLDER,layertag,pulserfiletag);
    pathAlphaCali00_08 = Form("%sdata_AlphaCali/SSD_%s_AlphaPeaks_%s.dat",PATHDATAFOLDER,layertag,AlphaCali00_08.c_str());
    pathAlphaCali05_08 = Form("%sdata_AlphaCali/SSD_%s_AlphaPeaks_%s.dat",PATHDATAFOLDER,layertag,AlphaCali05_08.c_str());
    PulserInGain20 = readfile.ReadData(pathPulserInGain20.c_str(),NUM_SSD,NUM_STRIP,numpar_PulserInGain20);
    AlphaCHIn00_08 = readfile.ReadData(pathAlphaCali00_08.c_str(),NUM_SSD,NUM_STRIP,numpar_AlphaCali00_08);
    AlphaCHIn05_08 = readfile.ReadData(pathAlphaCali05_08.c_str(),NUM_SSD,NUM_STRIP,numpar_AlphaCali05_08);
  } else if (strcmp(layertag,L2FTag.c_str())==0 || strcmp(layertag,L2BTag.c_str())==0) {
    Ch_Err = 0.3;  // 对拟合的 alpha 峰的道址给一个误差
    pathAlphaCali00_48 = Form("%sdata_AlphaCali/SSD_%s_AlphaPeaks_%s.dat",PATHDATAFOLDER,layertag,AlphaCali00_48.c_str());
    AlphaCHIn00_48 = readfile.ReadData(pathAlphaCali00_48.c_str(),NUM_SSD,NUM_STRIP,numpar_AlphaCali00_48);
  }
  //____________________________________________________________________________
  // 计算比例系数, 并进一步得到能量刻度曲线的洗漱
  Double_t k1[NUM_SSD][NUM_STRIP];
  Double_t h1[NUM_SSD][NUM_STRIP];
  Double_t c1[NUM_SSD][NUM_STRIP];
  Double_t k1_Minus [NUM_SSD][NUM_STRIP];
  Double_t h1_Minus [NUM_SSD][NUM_STRIP];
  Double_t k1_Plus  [NUM_SSD][NUM_STRIP];
  Double_t h1_Plus  [NUM_SSD][NUM_STRIP];
  Double_t k1_2Minus[NUM_SSD][NUM_STRIP];
  Double_t h1_2Minus[NUM_SSD][NUM_STRIP];
  Double_t k1_2Plus [NUM_SSD][NUM_STRIP];
  Double_t h1_2Plus [NUM_SSD][NUM_STRIP];
  Double_t c1_Minus [NUM_SSD][NUM_STRIP];
  Double_t c1_Plus  [NUM_SSD][NUM_STRIP];
  Double_t c1_2Minus[NUM_SSD][NUM_STRIP];
  Double_t c1_2Plus [NUM_SSD][NUM_STRIP];
//Double_t SSDCHNum [NUM_SSD][NUM_STRIP];
  Double_t RelativeErr_Minus [NUM_SSD][NUM_STRIP];
  Double_t RelativeErr_Plus  [NUM_SSD][NUM_STRIP];
  Double_t RelativeErr_2Minus[NUM_SSD][NUM_STRIP];
  Double_t RelativeErr_2Plus [NUM_SSD][NUM_STRIP];
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
    //SSDCHNum[i][j] = j+1;
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
      } else if (strcmp(layertag,L2FTag.c_str())==0 || strcmp(layertag,L2BTag.c_str())==0) {
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
  TF1* fPeak1       [NUM_SSD][NUM_STRIP];
  TF1* fPeak1_Minus [NUM_SSD][NUM_STRIP];
  TF1* fPeak1_Plus  [NUM_SSD][NUM_STRIP];
  TF1* fPeak1_2Minus[NUM_SSD][NUM_STRIP];
  TF1* fPeak1_2Plus [NUM_SSD][NUM_STRIP];
  TLegend* legend_MinusPlus [NUM_SSD][NUM_STRIP];
  TLegend* legend_2MinusPlus[NUM_SSD][NUM_STRIP];
  TLatex*  latex_Minus [NUM_SSD][NUM_STRIP];
  TLatex*  latex_Plus  [NUM_SSD][NUM_STRIP];
  TLatex*  latex_2Minus[NUM_SSD][NUM_STRIP];
  TLatex*  latex_2Plus [NUM_SSD][NUM_STRIP];

  TCanvas* cans = new TCanvas("cans","cans",1200,800);
  TCanvas* cans_begin = new TCanvas();
  TCanvas* cans_end   = new TCanvas();
  cans->Divide(2,1);
  cans_begin->Close();
  cans_end  ->Close();
  cans_begin->Print(pathPDFBegin.c_str());
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
      fPeak1[i][j]        = new TF1(Form("fPeak1_SSD%d_CH%02d",i+1,j),Form("%.5f*x+%.5f",k1[i][j],h1[i][j]),0,4096);
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

      latex_Minus [i][j] = new TLatex(1000.,0.15*fPeak1[i][j]->GetMaximum(),Form("a_Err(-%.1f) = %.3f(%%)",Ch_Err,RelativeErr_Minus[i][j]));
      latex_Plus  [i][j] = new TLatex(1000.,0.05*fPeak1[i][j]->GetMaximum(),Form("a_Err(+%.1f) = %.3f(%%)",Ch_Err,RelativeErr_Plus[i][j]));
      latex_2Minus[i][j] = new TLatex(1000.,0.15*fPeak1[i][j]->GetMaximum(),Form("a_Err(-%.1f) = %.3f(%%)",2*Ch_Err,RelativeErr_2Minus[i][j]));
      latex_2Plus [i][j] = new TLatex(1000.,0.05*fPeak1[i][j]->GetMaximum(),Form("a_Err(+%.1f) = %.3f(%%)",2*Ch_Err,RelativeErr_2Plus[i][j]));
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
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//   确定硅条能量刻度的参数! 暂定两套参数:
//   刻度参数1. peak1 与 peak2 拟合参数的平均: peak1 与 peak2 的参数稍微有些差异,可能是
//            由 ADC 在较低 channel 处的非线性引起的
//   刻度参数2.考虑死层修正后, 使得 peak1 与 peak2 刻度参数误小于 0.5%, 再取平均
//   现在需要输出两套参数，同时画图检查刻度曲线的差异
//
//  gfh, 2020-06-22
//  修改成类, 2020-10-04
////////////////////////////////////////////////////////////////////////////////
void CSHINESiEnergyCali::FinallyDeterminedFitPars(const char* layertag, const char* pulserfiletag)
{
  Double_t RelativeErrCut = 0.005;

  std::string L1STag("L1S");
  std::string pathPulserAndAlphaFitParsIn(Form("%sdata_SiEnergyCali/SSD_%s_%sAndAlphaFitPars.dat",PATHDATAFOLDER,layertag,pulserfiletag));
  std::string pathDeadLayerFitParsIn(Form("%sdata_SiEnergyCali/SSD_%s_DeadLayerFitPars.dat",PATHDATAFOLDER,layertag));
  std::string pathSiEnergyCaliParsOutput(Form("%sdata_SiEnergyCali/SSD_%s_SiEnergyCaliParameters.dat",PATHDATAFOLDER,layertag));
  std::string pathPDFOutput(Form("%sfigure_SiEnergyCali/SSD_%s_CompareWithDeadLayerFitFunction.pdf",PATHFIGURESFOLDER,layertag));
  std::string pathPDFOutputBegin(Form("%sfigure_SiEnergyCali/SSD_%s_CompareWithDeadLayerFitFunction.pdf[",PATHFIGURESFOLDER,layertag));
  std::string pathPDFOutputEnd(Form("%sfigure_SiEnergyCali/SSD_%s_CompareWithDeadLayerFitFunction.pdf]",PATHFIGURESFOLDER,layertag));

  ofstream SiEnergyCaliPars(pathSiEnergyCaliParsOutput.c_str());
  SiEnergyCaliPars<<"* Silicon energy calibration parameters. Two sets of parameters: \n";
  SiEnergyCaliPars<<"* 1.Just the average of the peak1 and the peak2 parameters \n";
  SiEnergyCaliPars<<"* 2.Consider the dead layer effect, then the get the average of the peak1 and the peak2 parameters \n";
  SiEnergyCaliPars<<"* Fit function: E_MeV = k * E_CH + h, then E_CH = 1/k * E_MeV - h/k \n";
  SiEnergyCaliPars<<"* SSDNum"<<setw(7)<<"CHNum"<<setw(15)<<"k_average"<<setw(20)<<"h_average"<<setw(20)
                  <<"k_deadlayer"<<setw(20)<<"h_deadlayer"<<setw(20)<<"k_average_ave"<<setw(20)
                  <<"h_average_ave"<<setw(20)<<"k_deadlayer_ave"<<setw(20)<<"h_deadlayer_ave\n";

  std::string AlphaCali00_08("AlphaCali00_08");
  std::string AlphaCali05_08("AlphaCali05_08");
  std::string AlphaCali00_48("AlphaCali00_48");
  std::string pathAlphaPeaks00_08;    // for SSD1_L1S,SSD2_L1S,SSD3_L1S
  std::string pathAlphaPeaks05_08;    // for SSD4_L1S
  std::string pathAlphaPeaks00_48;    // for L2F, L2B
  Int_t numpar_HeightAndAlphaFitPars = 4; // k1,h1,k2,h2
  Int_t numpar_DeadLayerFitPars      = 6; // k1,h1,k2,h2,<deadlayer>,deadlayer
  Int_t numpar_AlphapPeaks00_08      = 2; // peak1, peak2
  Int_t numpar_AlphapPeaks05_08      = 2; // peak1, peak2
  Int_t numpar_AlphapPeaks00_48      = 2; // peak1, peak2
  ReadFileModule readfile;
  Double_t*** HeightAndAlphaFitPars = readfile.ReadData(pathPulserAndAlphaFitParsIn.c_str(),NUM_SSD,NUM_STRIP,numpar_HeightAndAlphaFitPars);
  Double_t*** DeadLayerFitPars      = readfile.ReadData(pathDeadLayerFitParsIn.c_str(),NUM_SSD,NUM_STRIP,numpar_DeadLayerFitPars);
  Double_t*** AlphaPeaks00_08;
  Double_t*** AlphaPeaks05_08;
  Double_t*** AlphaPeaks00_48;
  if (strcmp(layertag, L1STag.c_str())==0) {  // 用于提取 peak1,peak2 的 ADC 道址， 帮助判断会否落入ADC非线性区域
    pathAlphaPeaks00_08 = Form("%sdata_AlphaCali/SSD_%s_AlphaPeaks_%s.dat",PATHDATAFOLDER,layertag,AlphaCali00_08.c_str());
    pathAlphaPeaks05_08 = Form("%sdata_AlphaCali/SSD_%s_AlphaPeaks_%s.dat",PATHDATAFOLDER,layertag,AlphaCali05_08.c_str());
    AlphaPeaks00_08     = readfile.ReadData(pathAlphaPeaks00_08.c_str(),NUM_SSD,NUM_STRIP,numpar_AlphapPeaks00_08);
    AlphaPeaks05_08     = readfile.ReadData(pathAlphaPeaks05_08.c_str(),NUM_SSD,NUM_STRIP,numpar_AlphapPeaks05_08);
  } else {
    pathAlphaPeaks00_48 = Form("%sdata_AlphaCali/SSD_%s_AlphaPeaks_%s.dat",PATHDATAFOLDER,layertag,AlphaCali00_48.c_str());
    AlphaPeaks00_48     = readfile.ReadData(pathAlphaPeaks00_48.c_str(),NUM_SSD,NUM_STRIP,numpar_AlphapPeaks00_48);
  }
  //____________________________________________________________________________
  Double_t k1[NUM_SSD][NUM_STRIP];
  Double_t h1[NUM_SSD][NUM_STRIP];   // E1_MeV = k1 * E1_CH + h1
  Double_t k2[NUM_SSD][NUM_STRIP];
  Double_t h2[NUM_SSD][NUM_STRIP];   // E2_MeV = k2 * E2_CH + h2
  Double_t k_ave[NUM_SSD][NUM_STRIP];
  Double_t h_ave[NUM_SSD][NUM_STRIP];
  Double_t k1_deadlayer[NUM_SSD][NUM_STRIP];
  Double_t h1_deadlayer[NUM_SSD][NUM_STRIP];
  Double_t k2_deadlayer[NUM_SSD][NUM_STRIP];
  Double_t h2_deadlayer[NUM_SSD][NUM_STRIP];
  Double_t k_deadlayer_ave[NUM_SSD][NUM_STRIP];
  Double_t h_deadlayer_ave[NUM_SSD][NUM_STRIP];
  Double_t deadlayer[NUM_SSD][NUM_STRIP];

  Double_t k_ave_sum[NUM_SSD];
  Double_t h_ave_sum[NUM_SSD];
  Double_t k_ave_ave[NUM_SSD];
  Double_t h_ave_ave[NUM_SSD];

  Double_t k_deadlayer_ave_sum[NUM_SSD];
  Double_t h_deadlayer_ave_sum[NUM_SSD];
  Double_t k_deadlayer_ave_ave[NUM_SSD];
  Double_t h_deadlayer_ave_ave[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    k_ave_sum[i] = 0.0;
    h_ave_sum[i] = 0.0;
    k_ave_ave[i] = 0.0;
    h_ave_ave[i] = 0.0;
    k_deadlayer_ave_sum[i] = 0.0;
    h_deadlayer_ave_sum[i] = 0.0;
    k_deadlayer_ave_ave[i] = 0.0;
    h_deadlayer_ave_ave[i] = 0.0;
    for (Int_t j=0; j<NUM_STRIP; j++) {
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

      k_ave_sum  [i] += k_ave[i][j];
      h_ave_sum  [i] += h_ave[i][j];
      k_deadlayer_ave_sum[i] += k_deadlayer_ave[i][j];
      h_deadlayer_ave_sum[i] += h_deadlayer_ave[i][j];
    }
    if (i==3 && strcmp(layertag, L1STag.c_str())==0) {
      k_ave_ave  [i] = (k_ave_sum[i]-k_ave[i][0])/15;
      h_ave_ave  [i] = (h_ave_sum[i]-h_ave[i][0])/15;
      k_deadlayer_ave_ave[i] = (k_deadlayer_ave_sum[i]-k_deadlayer_ave[i][0])/15;
      h_deadlayer_ave_ave[i] = (h_deadlayer_ave_sum[i]-h_deadlayer_ave[i][0])/15;
    } else {
      k_ave_ave[i] = k_ave_sum[i]/NUM_STRIP;
      h_ave_ave[i] = h_ave_sum[i]/NUM_STRIP;
      k_deadlayer_ave_ave[i] = k_deadlayer_ave_sum[i]/NUM_STRIP;
      h_deadlayer_ave_ave[i] = h_deadlayer_ave_sum[i]/NUM_STRIP;
    }
  }
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
      SiEnergyCaliPars<<setw(5)<<i<<setw(7)<<j<<setw(15)<<k_ave[i][j]<<setw(20)<<h_ave[i][j]<<setw(20)
                      <<k_deadlayer_ave[i][j]<<setw(20)<<h_deadlayer_ave[i][j]<<setw(20)
                      <<k_ave_ave[i]<<setw(20)<<h_ave_ave[i]<<setw(20)
                      <<k_deadlayer_ave_ave[i]<<setw(20)<<h_deadlayer_ave_ave[i]<<endl;
    }
  }
  TF1* fPeak1[NUM_SSD][NUM_STRIP];
  TF1* fPeak2[NUM_SSD][NUM_STRIP];
  TF1* fPeak1_deadlayer[NUM_SSD][NUM_STRIP];
  TF1* fPeak2_deadlayer[NUM_SSD][NUM_STRIP];
  TF1* fAverage[NUM_SSD][NUM_STRIP];
  TF1* fAverage_DeadLayer[NUM_SSD][NUM_STRIP];
  TLegend* legend_all[NUM_SSD][NUM_STRIP];
  TLegend* legend_peak1peak2[NUM_SSD][NUM_STRIP];
  TLegend* legend_peak1peak2_deadlayer[NUM_SSD][NUM_STRIP];
  TLatex*  latex[NUM_SSD][NUM_STRIP];
  TLatex*  latex0[NUM_SSD][NUM_STRIP];
  TLatex*  latex_peak1[NUM_SSD][NUM_STRIP];
  TLatex*  latex_peak2[NUM_SSD][NUM_STRIP];
  TLatex*  latex_k_Err[NUM_SSD][NUM_STRIP];

  TCanvas* cans = new TCanvas("cans","cans",1800,800);
  TCanvas* cans_begin = new TCanvas();
  TCanvas* cans_end   = new TCanvas();
  cans->Divide(3,1);
  cans_begin->Close();
  cans_end->Close();
  cans_begin->Print(pathPDFOutputBegin.c_str());
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
      fPeak1[i][j] = new TF1(Form("fPeak1_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k1[i][j],h1[i][j]),0,4096);
      fPeak2[i][j] = new TF1(Form("fPeak2_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k2[i][j],h2[i][j]),0,4096);
      fPeak1_deadlayer[i][j] = new TF1(Form("fPeak1_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k1_deadlayer[i][j],h1_deadlayer[i][j]),0,4096);
      fPeak2_deadlayer[i][j] = new TF1(Form("fPeak2_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k2_deadlayer[i][j],h2_deadlayer[i][j]),0,4096);
      fAverage[i][j] = new TF1(Form("fAverage_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k_ave[i][j],h_ave[i][j]),0,4096);
      fAverage_DeadLayer[i][j] = new TF1(Form("fAverage_DeadLayer_SSD%d_CH%d",i+1,j),Form("%.5f*x+%.5f",k_deadlayer_ave[i][j],h_deadlayer_ave[i][j]),0,4096);
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

      latex[i][j]  = new TLatex(1500,0.2*fPeak1[i][j]->GetMaximum(),Form("deadlayer=%.2f(#mum)",deadlayer[i][j]));
      latex0[i][j] = new TLatex(1500,0.2*fPeak1[i][j]->GetMaximum(),Form("deadlayer=%.2f(#mum)",0.));
      latex [i][j]->SetTextColor(kMagenta);
      latex0[i][j]->SetTextColor(kMagenta);

      latex_k_Err[i][j] = new TLatex(1500,0.05*fPeak1[i][j]->GetMaximum(),Form("a_Err < %.2f(%%)",100*RelativeErrCut));
      latex_k_Err[i][j]->SetTextColor(kMagenta);

      if (strcmp(layertag,"L1S")==0 && i<3) { // SSD1_L1S,SSD2_L1S,SSD3_L1S, 使用 AlphaCali00_08
        latex_peak1[i][j] = new TLatex(1500,0.12*fPeak1[i][j]->GetMaximum(),Form("CH(#alpha_{1})=%.2f",AlphaPeaks00_08[i][j][0]));
        latex_peak2[i][j] = new TLatex(1500,0.05*fPeak1[i][j]->GetMaximum(),Form("CH(#alpha_{2})=%.2f",AlphaPeaks00_08[i][j][1]));
      } else if (strcmp(layertag,"L1S")==0 && i==3) { // SSD4_L1S, 使用 AlphaCali05_08
        latex_peak1[i][j] = new TLatex(1500,0.12*fPeak1[i][j]->GetMaximum(),Form("CH(#alpha_{1})=%.2f",AlphaPeaks05_08[i][j][0]));
        latex_peak2[i][j] = new TLatex(1500,0.05*fPeak1[i][j]->GetMaximum(),Form("CH(#alpha_{2})=%.2f",AlphaPeaks05_08[i][j][1]));
      } else {
        latex_peak1[i][j] = new TLatex(1500,0.12*fPeak1[i][j]->GetMaximum(),Form("CH(#alpha_{1})=%.2f",AlphaPeaks00_48[i][j][0]));
        latex_peak2[i][j] = new TLatex(1500,0.05*fPeak1[i][j]->GetMaximum(),Form("CH(#alpha_{2})=%.2f",AlphaPeaks00_48[i][j][1]));
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
//******************************************************************************
