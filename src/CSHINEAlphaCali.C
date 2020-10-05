
#include "../include/CSHINEAlphaCali.h"
using namespace std;


//******************************************************************************
CSHINEAlphaCali::CSHINEAlphaCali()
{}

//______________________________________
CSHINEAlphaCali::~CSHINEAlphaCali()
{}
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//                  ALPHA CALIBRATION CALCULATION ENERGY
//    修正 alpha 的能量. 能量修正主要来源于两部分:
//     1. 硅条前面的镀铝 Mylar 膜: 2.0um Mylar + 0.06um Al
//     2. 硅条的死层
//    这里的计算先不考虑硅的死层，仅计算 alpha 在 镀铝 Mylar 膜中的能损
//
//    对于 alpha 的能量: 3 alpha 峰分别为 Pu239, Am241, Cm244, 每一种放射源都有 2-3
//    个不同你能量的 alpha, 这里按照分支比取其平均值
//
//    gfh, 2019-11-11
//    修改成类, 2020-10-03
////////////////////////////////////////////////////////////////////////////////
// 定义一个函数,用于计算两个数组元素乘积的求和
Double_t Eaverage(Double_t* arr1, Double_t* arr2, Int_t size)
{
  Double_t Esum = 0; // energy
  Double_t Rsum = 0; // energy branch ratio
  Double_t average = 0;
  for(Int_t i=0; i<size; i++) {
    Esum += arr1[i]*arr2[i];
    Rsum += arr2[i];
  }
  average = Esum/Rsum;
  return average;
}
//______________________________________________________________________________
void CSHINEAlphaCali::CalculateAlphaEnergy()
{
  // alpha 能量数据来源于: 刘运祚 《常用放射性核素衰变纲图（1982）》
  Double_t Energy1[3] = {5.155, 5.143, 5.103};   // Unit: MeV    Pu239
  Double_t Ratio1 [3] = {0.730, 0.151, 0.115};   // branch ratio
  Double_t Energy2[2] = {5.486, 5.443};          // Unit: MeV    Am241
  Double_t Ratio2 [2] = {0.852, 0.128};
  Double_t Energy3[2] = {5.805, 5.763};          // Unit: MeV    Cm244
  Double_t Ratio3 [2] = {0.767, 0.233};
  Double_t E1 = Eaverage(Energy1, Ratio1, 3);
  Double_t E2 = Eaverage(Energy2, Ratio2, 2);
  Double_t E3 = Eaverage(Energy3, Ratio3, 2);
  Double_t Ealpha[3] = {E1, E2, E3};  // MeV
  Double_t MylarThickness = 2.0;      // um
  Double_t AlThickness    = 0.06;      // um

  std::string pathCalculationResultsOut(Form("%sdata_AlphaCali/SSD_AlphaEnergies.dat",PATHDATAFOLDER));

  ofstream FileOut(pathCalculationResultsOut.c_str());
  FileOut<<setw(5)<<"* MylarThickness = 2.0um, AlThickness = 0.06um \n";
  FileOut<<setw(5)<<"* SSDNum"<<setw(7)<<"CHNum"<<setw(15)<<"E1_Residual"<<setw(12)<<"E2_Residual"
         <<setw(15)<<"E3_Residual"<<setw(7)<<"E1"<<setw(12)<<"E2"<<setw(15)<<"E3"
         <<setw(15)<<"E1_Loss"<<setw(12)<<"E2_Loss"<<setw(12)<<"E3_Loss"<<setw(5)<<"(MeV)\n";
  //____________________________________________________________________________
  EnergyLossModule LISEModule;
  Double_t Eloss1[3];
  Double_t Eloss2[3];
  Double_t Eresidual1[3];
  Double_t Eresidual2[3];
  for(Int_t i=0; i<3; i++) {
    Eloss1[i]     = LISEModule.GetEnergyLoss(2,4,Ealpha[i],"Mylar",MylarThickness);
    Eresidual1[i] = Ealpha[i] - Eloss1[i];
    Eloss2[i]     = LISEModule.GetEnergyLoss(2,4,Eresidual1[i],"Al",AlThickness);
    Eresidual2[i] = Eresidual1[i] - Eloss2[i];  //alpha粒子穿过Mylar膜之后的能量
    cout<<"E_Incident = "<< Ealpha[i] <<"  "<<"E_residual = "<< Eresidual2[i]<<"  "
        <<"ELoss_Total = "<<Ealpha[i]-Eresidual2[i]<<endl;
  }
  for(Int_t SSDNum=0; SSDNum<NUM_SSD; SSDNum++) {
    for(Int_t CHNum=0; CHNum<NUM_STRIP; CHNum++) {
      FileOut<<setw(5)<<SSDNum <<setw(7)<<CHNum
             <<setw(15)<<Eresidual2[0]<<setw(13)<<Eresidual2[1]<<setw(13)<<Eresidual2[2]
             <<setw(13)<<E1<<setw(13)<<E2<<setw(13)<<E3
             <<setw(13)<<Ealpha[0]-Eresidual2[0]
             <<setw(13)<<Ealpha[1]-Eresidual2[1]
             <<setw(13)<<Ealpha[2]-Eresidual2[2]<<endl;
    }
  }
  printf("A new file %s has been closed.\n", pathCalculationResultsOut.c_str());
  FileOut.close();
  return;
}
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//     刻度时， 一般需要对直方图进行拟合. 为方便起见，这里将刻度文件合并，累成直方图,
//     同时存成 root 文件.
//     根据实验条件，Layer1 与 Layer2 的 alpha 刻度都分别进行了两次:
//     (1) L1: 0-4,    5-8
//     (2) L2: 0-32,  33-48
//
//   gfh, 2019-11-11
//   修改成类, 2020-10-03
////////////////////////////////////////////////////////////////////////////////
void WriteHist(TH1D* hist[16], TFile* fileout) {
  for(Int_t CHNum=0; CHNum<NUM_STRIP; CHNum++) {
    fileout->WriteTObject(hist[CHNum],hist[CHNum]->GetName());
  }
}
//_____________________________________________
void CSHINEAlphaCali::MergeAlphaCaliFiles(const char* layertag, Int_t firstrun, Int_t lastrun)
{
  TimeAndPercentage time;

  std::string L1Tag("L1");
  std::string L2Tag("L2");
  std::string L1STag("L1S");
  std::string pathROOTFileOut;
  std::string pathROOTFileIn;
  pathROOTFileOut = Form("%sMapRoot/MapSSD_%s_AlphaCali%02d_%02d.root",PATHROOTFILESFOLDER,layertag,firstrun,lastrun);
  if (strcmp(layertag,L1STag.c_str())==0) {
    pathROOTFileIn = Form("%sMapRoot/MapSSD_%s",PATHROOTFILESFOLDER,L1Tag.c_str());
  } else {
    pathROOTFileIn = Form("%sMapRoot/MapSSD_%s",PATHROOTFILESFOLDER,L2Tag.c_str());
  }
  //____________________________________________________________________________
  TFile* FileOut = new TFile(pathROOTFileOut.c_str(), "RECREATE");
  auto*   myData = new TChain("KrPb");

  for (int i=firstrun; i<=lastrun; i++) {
    myData->Add(Form("%s_AlphaCali%04d.root",pathROOTFileIn.c_str(),i));
    printf("MapSSD_%s_AlphaCali%04d.root Loaded\n",layertag,i);
  }
  // I disable all the branches
  myData->SetBranchStatus("*",false);

  Int_t SSD1_L1S_E[NUM_STRIP];
  Int_t SSD2_L1S_E[NUM_STRIP];
  Int_t SSD3_L1S_E[NUM_STRIP];
  Int_t SSD4_L1S_E[NUM_STRIP];
  Int_t SSD1_L2F_E[NUM_STRIP];
  Int_t SSD1_L2B_E[NUM_STRIP];
  Int_t SSD2_L2F_E[NUM_STRIP];
  Int_t SSD2_L2B_E[NUM_STRIP];
  Int_t SSD3_L2F_E[NUM_STRIP];
  Int_t SSD3_L2B_E[NUM_STRIP];
  Int_t SSD4_L2F_E[NUM_STRIP];
  Int_t SSD4_L2B_E[NUM_STRIP];
  if (strcmp(L1Tag.c_str(), layertag)==0) {
    // I enable only the branches I need
    myData->SetBranchStatus ("SSD1_L1S_E",true);
    myData->SetBranchStatus ("SSD2_L1S_E",true);
    myData->SetBranchStatus ("SSD3_L1S_E",true);
    myData->SetBranchStatus ("SSD4_L1S_E",true);
    myData->SetBranchAddress("SSD1_L1S_E",SSD1_L1S_E);
    myData->SetBranchAddress("SSD2_L1S_E",SSD2_L1S_E);
    myData->SetBranchAddress("SSD3_L1S_E",SSD3_L1S_E);
    myData->SetBranchAddress("SSD4_L1S_E",SSD4_L1S_E);
  }
  if (strcmp(L2Tag.c_str(), layertag)==0) {
    myData->SetBranchStatus ("SSD1_L2F_E",true);
    myData->SetBranchStatus ("SSD1_L2B_E",true);
    myData->SetBranchStatus ("SSD2_L2F_E",true);
    myData->SetBranchStatus ("SSD2_L2B_E",true);
    myData->SetBranchStatus ("SSD3_L2F_E",true);
    myData->SetBranchStatus ("SSD3_L2B_E",true);
    myData->SetBranchStatus ("SSD4_L2F_E",true);
    myData->SetBranchStatus ("SSD4_L2B_E",true);
    myData->SetBranchAddress("SSD1_L2F_E",SSD1_L2F_E);
    myData->SetBranchAddress("SSD1_L2B_E",SSD1_L2B_E);
    myData->SetBranchAddress("SSD2_L2F_E",SSD2_L2F_E);
    myData->SetBranchAddress("SSD2_L2B_E",SSD2_L2B_E);
    myData->SetBranchAddress("SSD3_L2F_E",SSD3_L2F_E);
    myData->SetBranchAddress("SSD3_L2B_E",SSD3_L2B_E);
    myData->SetBranchAddress("SSD4_L2F_E",SSD4_L2F_E);
    myData->SetBranchAddress("SSD4_L2B_E",SSD4_L2B_E);
  }
  Char_t histname_L1S[100];
  Char_t histname_L2F[100];
  Char_t histname_L2B[100];
  TH1D* Hist_L1S[NUM_SSD][NUM_STRIP];
  TH1D* Hist_L2F[NUM_SSD][NUM_STRIP];
  TH1D* Hist_L2B[NUM_SSD][NUM_STRIP];
  for (int SSDNum=0; SSDNum<NUM_SSD; SSDNum++) {
    for (int CHNum=0; CHNum<NUM_STRIP; CHNum++) {
      strcpy(histname_L1S, Form("SSD%d_L1S_E_CH%02d",SSDNum+1,CHNum));
      strcpy(histname_L2F, Form("SSD%d_L2F_E_CH%02d",SSDNum+1,CHNum));
      strcpy(histname_L2B, Form("SSD%d_L2B_E_CH%02d",SSDNum+1,CHNum));
      Hist_L1S[SSDNum][CHNum] = new TH1D(histname_L1S,histname_L1S,4096,0,4096);
      Hist_L2F[SSDNum][CHNum] = new TH1D(histname_L2F,histname_L2F,4096,0,4096);
      Hist_L2B[SSDNum][CHNum] = new TH1D(histname_L2B,histname_L2B,4096,0,4096);
      Hist_L1S[SSDNum][CHNum]->GetXaxis()->SetRangeUser(10.,4096);
    }
  }
  // Loop on the data to fill the histograms. (event by event)
  unsigned long nentries = myData->GetEntries();
  cout<< "Found " << nentries <<" entries\n";
  for (unsigned long j=0; j<nentries; j++) {
    time.PrintPercentageAndRemainingTime(j, nentries);
    myData->GetEntry(j);

    if (strcmp(L1Tag.c_str(), layertag)==0) {
       for(int CHNum=0; CHNum<NUM_STRIP; CHNum++) {
         Hist_L1S[0][CHNum]->Fill(SSD1_L1S_E[CHNum],1.0);
         Hist_L1S[1][CHNum]->Fill(SSD2_L1S_E[CHNum],1.0);
         Hist_L1S[2][CHNum]->Fill(SSD3_L1S_E[CHNum],1.0);
         Hist_L1S[3][CHNum]->Fill(SSD4_L1S_E[CHNum],1.0);
       }
    }
    if (strcmp(L2Tag.c_str(), layertag)==0) {
      for (int CHNum=0; CHNum<NUM_STRIP; CHNum++) {
        Hist_L2F[0][CHNum]->Fill(SSD1_L2F_E[CHNum],1.0);
        Hist_L2B[0][CHNum]->Fill(SSD1_L2B_E[CHNum],1.0);
        Hist_L2F[1][CHNum]->Fill(SSD2_L2F_E[CHNum],1.0);
        Hist_L2B[1][CHNum]->Fill(SSD2_L2B_E[CHNum],1.0);
        Hist_L2F[2][CHNum]->Fill(SSD3_L2F_E[CHNum],1.0);
        Hist_L2B[2][CHNum]->Fill(SSD3_L2B_E[CHNum],1.0);
        Hist_L2F[3][CHNum]->Fill(SSD4_L2F_E[CHNum],1.0);
        Hist_L2B[3][CHNum]->Fill(SSD4_L2B_E[CHNum],1.0);
      }
    }
  }
  // Write the data into .root file
  if (strcmp(L1Tag.c_str(), layertag)==0) {
     WriteHist(Hist_L1S[0],FileOut);
     WriteHist(Hist_L1S[1],FileOut);
     WriteHist(Hist_L1S[2],FileOut);
     WriteHist(Hist_L1S[3],FileOut);
  }
  if (strcmp(L2Tag.c_str(), layertag)==0) {
    WriteHist(Hist_L2F[0],FileOut);
    WriteHist(Hist_L2B[0],FileOut);
    WriteHist(Hist_L2F[1],FileOut);
    WriteHist(Hist_L2B[1],FileOut);
    WriteHist(Hist_L2F[2],FileOut);
    WriteHist(Hist_L2B[2],FileOut);
    WriteHist(Hist_L2F[3],FileOut);
    WriteHist(Hist_L2B[3],FileOut);
  }
  FileOut->Close();
  delete myData;
}
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//    手动提取选取刻度区间，同时对 3 个 alpha 峰进行拟合.
//    这是因为: 手动可以自由选取拟合范围，同时方便将拟合区间保存下来.
//
//    说明: 手动选取 3 个拟合区间，同时画出拟合直线，根据拟合结果判断拟合效果，
//    如果 3 个 alpha 峰很好地在一条实现上，则进行下一个拟合; 否则，重复当前拟合.
//                      ----------------
//                         自动寻峰步骤
//                      ----------------
//   1. 读取Histograms
//   2. 通过c1->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",0,0,
//                     "SetPoints(Int_t,Int_t,Int_t,TObject*)")调用GUI
//   3. 定义一个正数Option，作为操作选项：
//     (1) Option 初始化为 2
//     (2) Option = 2: 调用 SetPoints(), 手动选点
//     (3) 提示输入Option：
//         <1> 输入 1, 进行拟合并保存;
//         <2> 输入 2, 在此执行while(Option==2)
//         <3> 输入 3，中断跳出
//  4. 三组分alpha源有3个能量峰,需要6个点来拟合
//
//  gfh, 2019-11-11
//  修改成类, 2020-10-03
////////////////////////////////////////////////////////////////////////////////
void CSHINEAlphaCali::ClickToFindAlphaPeaks(const char* layertag, const char* alphafiletag)
{
  Double_t Ealpha[3] = {4.90407, 5.24768, 5.57147};  //MeV

  std::string L1Tag("L1");
  std::string L2Tag("L2");
  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string AlphaFitTag("3AlphaFitPars");
  std::string AlphaPeaksTag("AlphaPeaks");

  std::string pathRootFileInput;
  if (strcmp(layertag,L1STag.c_str())==0) {
    pathRootFileInput= Form("%sQualityCheck/QC_MapSSD_%s_%s.root",PATHROOTFILESFOLDER,L1Tag.c_str(),alphafiletag);
  } else {
    pathRootFileInput= Form("%sQualityCheck/QC_MapSSD_%s_%s.root",PATHROOTFILESFOLDER,L2Tag.c_str(),alphafiletag);
  }
  std::string pathPedestalInput(Form("%sdata_Pedestals/SSD_%s_AlphaCaliPedestals_%s.dat",
                                PATHDATAFOLDER,layertag,alphafiletag));
  std::string pathAlphaPeaksOutput(Form("%sdata_AlphaCali/SSD_%s_%s_%s.dat", PATHDATAFOLDER,
                                layertag,AlphaPeaksTag.c_str(),alphafiletag));
  std::string pathAlphaFitResultsOutput(Form("%sdata_AlphaCali/SSD_%s_%s_%s.dat",PATHDATAFOLDER,
                                layertag,AlphaFitTag.c_str(),alphafiletag));
  std::string pathPDFOutput(Form("%sfigure_AlphaCali/SSD_%s_%s_%s.pdf",PATHFIGURESFOLDER,
                                layertag,AlphaPeaksTag.c_str(),alphafiletag));
  std::string pathPDFbegin(Form("%sfigure_AlphaCali/SSD_%s_%s_%s.pdf[",PATHFIGURESFOLDER,
                                layertag,AlphaPeaksTag.c_str(),alphafiletag));
  std::string pathPDFend(Form("%sfigure_AlphaCali/SSD_%s_%s_%s.pdf]",PATHFIGURESFOLDER,
                                layertag,AlphaPeaksTag.c_str(),alphafiletag));
  Double_t HistXLowRange[NUM_SSD];
  Double_t HistXUpRange [NUM_SSD];
  if (strcmp(layertag, L1STag.c_str())==0) {           // for L1S
      HistXLowRange[0] = 90.,  HistXUpRange[0] = 230.;
      HistXLowRange[1] = 90,   HistXUpRange[1] = 230.;
      HistXLowRange[2] = 110., HistXUpRange[2] = 210.;
      HistXLowRange[3] = 160., HistXUpRange[3] = 310.;
  } else if (strcmp(layertag, L2FTag.c_str())==0) {    // for L2F
      HistXLowRange[0] = 30.,  HistXUpRange[0] = 70.;
      HistXLowRange[1] = 60.,  HistXUpRange[1] = 130.;
      HistXLowRange[2] = 100., HistXUpRange[2] = 200.;
      HistXLowRange[3] = 250., HistXUpRange[3] = 350.;
  } else {
      HistXLowRange[0] = 40.,  HistXUpRange[0] = 100.; // for L2B
      HistXLowRange[1] = 60.,  HistXUpRange[1] = 130.;
      HistXLowRange[2] = 100., HistXUpRange[2] = 200.;
      HistXLowRange[3] = 250., HistXUpRange[3] = 360.;
  }
  // 在此修改输出文件路径
  ofstream FileOut(pathAlphaPeaksOutput.c_str());
  FileOut <<"* SSDNum" <<setw(7)<<"CHNum"<<setw(8)
          <<"mean1" << setw(10)<<"mean2" <<setw(10)<<"mean3" <<setw(10)
          <<"sigma1"<< setw(10)<<"sigma2"<<setw(10)<<"sigma3"<<setw(10)
          <<"Chi21" << setw(10)<<"Chi22" <<setw(10)<<"Chi23" <<setw(10)
          <<"Xmin1"<< setw(10)<<"Xmax1"<<setw(10)<<"Xmin2"<< setw(10)
          <<"Xmax2"<<setw(10)<<"Xmin3"<<setw(10)<<"Xmax3"<<endl;
  ofstream FileOutFit(pathAlphaFitResultsOutput.c_str());
  FileOutFit<<"* Fit function = par[0]+par[1]*x && y = a*x + b (y=Energy, x=Ch); so I define a = par[1], b = par[0]\n";
  FileOutFit<<"* SSDNum"<<setw(7)<<"CHNum"<<setw(10)<<"par_a"<<setw(15)<<"Errpar_a"<<setw(15)
            <<"par_b"<<setw(17)<<"Errpar_b\n";
  //____________________________________________________________________________
  TFile* FileIn = new TFile(pathRootFileInput.c_str());
  if (!FileIn->IsOpen()) {
    cout<<"Open file "<<pathRootFileInput.c_str()<<" failed."<<endl;
    return;
  } else {
    cout<<"file "<<pathRootFileInput.c_str()<< " loaded."<<endl;
  }
  // 定义、读取输入文件中的 Histograms
  Char_t histname[100];
  TH1D* AlphaCaliHist[NUM_SSD][NUM_STRIP];
  for (Int_t SSDNum=0; SSDNum<NUM_SSD; SSDNum++) {
    for (Int_t CHNum=0; CHNum<NUM_STRIP; CHNum++) {
      strcpy(histname, Form("SSD%d_%s_E_CH%02d",(SSDNum+1),layertag,CHNum));
      AlphaCaliHist[SSDNum][CHNum]=(TH1D*)FileIn->Get(histname);
    }
  }
  // read pedestal from pedestal file
  Int_t numpar_PedestalIn = 3; // mean, sigma, mean+5*sigma
  ReadFileModule readfile;
  Double_t*** PedestalIn = readfile.ReadData(pathPedestalInput.c_str(),NUM_SSD,NUM_STRIP,numpar_PedestalIn);

  TCanvas* c1 = new TCanvas("c1","c1",1000,1200);
  c1->Divide(1,2);
  TCanvas* c_begin = new TCanvas("c_begin","");
  TCanvas* c_end   = new TCanvas("c_end","");
  c_begin->Close();
  c_end->Close();
  c_begin->Print(pathPDFbegin.c_str());
  //____________________________________________________________________________
  //                       BEGIN ANALYZE HERE !!!
  Double_t limit[6] = {0};   //定义limit[2]用于存储拟合范围
//Double_t par[9]   = {40.,250.,15.,20.,260.,15.,10.,270.,15.}; // 拟合参数初始化
  for (Int_t SSDNum=0; SSDNum<NUM_SSD; SSDNum++) {
    for (Int_t CHNum=0; CHNum<NUM_STRIP; CHNum++) {
      Int_t Option = 2;   // 设置一个标志, 当Option==2时，执行后面的while(Option==2)循环
      if (AlphaCaliHist[SSDNum][CHNum]==0) {
        printf("No data present for SSD%d_%s_E_CH%02d\n",(SSDNum+1),layertag,CHNum);
        continue;
      }
      Double_t XLowRange = PedestalIn[SSDNum][CHNum][0]+HistXLowRange[SSDNum];
      Double_t XUpRange  = PedestalIn[SSDNum][CHNum][0]+HistXUpRange[SSDNum];
      AlphaCaliHist[SSDNum][CHNum]->GetXaxis()->SetRangeUser(XLowRange,XUpRange);
      Double_t HistYUpRange = (AlphaCaliHist[SSDNum][CHNum]->GetMaximum())*1.1;
      AlphaCaliHist[SSDNum][CHNum]->GetYaxis()->SetRangeUser(0,HistYUpRange);
      while (Option==2) {
        fIndex = 0;
        fNPoints = 0;
        c1->cd(1);
        gPad->Modified();
        gPad->Update();
        AlphaCaliHist[SSDNum][CHNum]->Draw();
        //________________________________________________________
        //   it is very important!!! very convenient!!!
        //   here to set the range and fit on the GUI by hand
        c1->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)","CSHINEAlphaCali",this,"SetPoints(Int_t,Int_t,Int_t,TObject*)");
        while (fIndex!=1) {
          usleep(100);
          gClient->HandleInput();
        }
        c1->Disconnect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)");
        if (fIndex==1) {
          for(Int_t ip=0; ip<fNPoints; ip++) {
            limit[ip] = m[ip]->GetX();
            cout<<limit[ip]<<endl;
          }
        }
        //  对选取的范围进行拟合
        //  ROOT中的高斯函数定义为: f(x) = p0*exp(-0.5*((x-p1)/p2)^2)
        TF1* FitPeak1 = new TF1("FitPeak1","gaus",limit[0],limit[1]);
        TF1* FitPeak2 = new TF1("FitPeak2","gaus",limit[2],limit[3]);
        TF1* FitPeak3 = new TF1("FitPeak3","gaus",limit[4],limit[5]);
        AlphaCaliHist[SSDNum][CHNum]->Fit(FitPeak1,"R");
        AlphaCaliHist[SSDNum][CHNum]->Fit(FitPeak2,"R+");
        AlphaCaliHist[SSDNum][CHNum]->Fit(FitPeak3,"R+");
      //Double_t Peak1_Height = FitPeak1->GetParameter(0);  // Par[1] = Height
        Double_t Peak1_Mean   = FitPeak1->GetParameter(1);  // Par[1] = Mean
        Double_t Peak1_Sigma  = FitPeak1->GetParameter(2);  // Par[2] = sigma
        Double_t Peak1_Chi2   = FitPeak1->GetChisquare();
      //Double_t Peak2_Height = FitPeak2->GetParameter(0);  // Par[1] = Height
        Double_t Peak2_Mean   = FitPeak2->GetParameter(1);  // Par[1] = Mean
        Double_t Peak2_Sigma  = FitPeak2->GetParameter(2);  // Par[2] = sigma
        Double_t Peak2_Chi2   = FitPeak2->GetChisquare();
      //Double_t Peak3_Height = FitPeak3->GetParameter(0);  // Par[1] = Height
        Double_t Peak3_Mean   = FitPeak3->GetParameter(1);  // Par[1] = Mean
        Double_t Peak3_Sigma  = FitPeak3->GetParameter(2);  // Par[2] = sigma
        Double_t Peak3_Chi2   = FitPeak3->GetChisquare();

        c1->cd(2);
        Double_t peaks[3] = {Peak1_Mean,Peak2_Mean,Peak3_Mean};
        TGraph* grap = new TGraph(3,peaks,Ealpha); // Energy vs Channel
        grap->SetMarkerStyle(20);
        grap->SetMarkerSize(1.5);
        grap->SetMarkerColor(kBlue);
        grap->SetTitle(Form("AlphaFits_SSD%d_%s_E_CH%02d",SSDNum+1,layertag,CHNum));
        grap->Draw("AP*");
        TF1* fit = new TF1("fit","pol1",100,4096);
        grap->Fit("fit");
        Double_t par0     = fit->GetParameter(0);
        Double_t err_par0 = fit->GetParError(0);
        Double_t par1     = fit->GetParameter(1);
        Double_t err_par1 = fit->GetParError(1);
        gPad->Modified();
        gPad->Update();
        // 提示输入操作选项
        cout<< "Options? 1.Save and next, 2.Retry the fit, 3.Exit"<< endl;
        cin >> Option;
        // Option ==1，保存拟合结果
        if (Option==1) {
          FileOut << SSDNum << setw(10) << CHNum << setw(12)
                  << Peak1_Mean <<setw(10)<<Peak2_Mean <<setw(10)<<Peak3_Mean <<setw(10)
                  << Peak1_Sigma<<setw(10)<<Peak2_Sigma<<setw(10)<<Peak3_Sigma<<setw(10)
                  << Peak1_Chi2 <<setw(10)<<Peak2_Chi2 <<setw(10)<<Peak3_Chi2 <<setw(10)
                  <<limit[0]<< setw(10)<<limit[1]<<setw(10)<<limit[2]<< setw(10)
                  <<limit[3]<<setw(10)<<limit[4]<<setw(10)<<limit[5]<<endl;
          FileOutFit<<setw(5)<<SSDNum<<setw(7)<<CHNum<<setw(15)<<par1<<setw(15)<<err_par1
                    <<setw(15)<<par0<<setw(15)<<err_par0<<endl;
          c1->Print(pathPDFOutput.c_str());
        }
        if (Option==3) {
          c_end->Print(pathPDFend.c_str());
          FileOut.close();
          cout<<"File .dat closed"<<endl;
          return;
        }
      } // close while(Option==2)
    }   // close silicon strip  (j)
  }     // close silicon number (i)
  //  the pdf file ends here !!!
  c_end->Print(pathPDFend.c_str());
  FileOut.close();
  FileOutFit.close();
  cout<<"File .dat closed"<<endl;
  return;
}
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//   1. 在 Fission2019 实验中, 对于 SSD_L1 的放射源刻度, 我们进行了两次, 这是因为
//      第一次刻度时, 有些条没有刻度到. 因此第一次刻度完, 又开了第二次靶室.
//   2. 第一次刻度文件 SSD_L1_AlphaCali0000-0004,
//      第二次刻度文件 SSD_L1_AlphaCali0005-0008
//      在合并两次刻度文件时发现：SSD4_L1 前后两次刻度的 alpha 峰位发生偏移,
//      因此需要分开两次刻度, 再对比差异
//   3. 在 Layer2 的 alpha 刻度时, SSD4_L2F_CH13 与 SSD4_L2F_CH14 两道
//      出现两组 alpha 峰, 需要分析一下
//   4. 这个 macro 的作用就是分析这两次刻度的差异
//
//   gfh, 2020-06-07
//   修改成类, 2020-10-03
////////////////////////////////////////////////////////////////////////////////
void CSHINEAlphaCali::CheckAbnormaleChannels_SSD4()
{
  AlphaCali_SSD4_L1S_CompareAlphaCali00_04And05_08();
  AlphaCali_SSD4_L2F_EstimateCH13_CH14();
}
//______________________________________________________________________________
void CSHINEAlphaCali::AlphaCali_SSD4_L1S_CompareAlphaCali00_04And05_08()
{
  gStyle->SetPalette(1);

  Int_t numpar  = 3;
  Float_t small = 0.001;

  std::string AlphaCali00_04("AlphaCali00_04");
  std::string AlphaCali05_08("AlphaCali05_08");
  std::string pathAlphaCali00_04(Form("%sdata_AlphaCali/SSD_L1S_AlphaPeaks_%s.dat",PATHDATAFOLDER,AlphaCali00_04.c_str()));
  std::string pathAlphaCali05_08(Form("%sdata_AlphaCali/SSD_L1S_AlphaPeaks_%s.dat",PATHDATAFOLDER,AlphaCali05_08.c_str()));
  std::string pathPNGOutput(Form("%sfigure_AlphaCali/SSD4_L1S_AlphaReCali.png",PATHFIGURESFOLDER));
  //____________________________________________________________________________
  Double_t CHIndex[NUM_STRIP];
  Double_t Peak1_00_04[NUM_STRIP];
  Double_t Peak2_00_04[NUM_STRIP];
  Double_t Peak3_00_04[NUM_STRIP];
  Double_t Peak1_05_08[NUM_STRIP];
  Double_t Peak2_05_08[NUM_STRIP];
  Double_t Peak3_05_08[NUM_STRIP];
  Double_t err_Peak1[NUM_STRIP];
  Double_t err_Peak2[NUM_STRIP];
  Double_t err_Peak3[NUM_STRIP];
  Double_t relative_err_Peak1[NUM_STRIP];
  Double_t relative_err_Peak2[NUM_STRIP];
  Double_t relative_err_Peak3[NUM_STRIP];

  ReadFileModule readfile;
  Double_t*** AlphaPeaks00_04 = readfile.ReadData(pathAlphaCali00_04.c_str(),NUM_SSD,NUM_STRIP,numpar);
  Double_t*** AlphaPeaks05_08 = readfile.ReadData(pathAlphaCali05_08.c_str(),NUM_SSD,NUM_STRIP,numpar);

  for (Int_t j=0; j<NUM_STRIP; j++) {
    CHIndex[j] = j+1;
    Peak1_00_04[j] = AlphaPeaks00_04[3][j][0];
    Peak2_00_04[j] = AlphaPeaks00_04[3][j][1];
    Peak3_00_04[j] = AlphaPeaks00_04[3][j][2];
    Peak1_05_08[j] = AlphaPeaks05_08[3][j][0];
    Peak2_05_08[j] = AlphaPeaks05_08[3][j][1];
    Peak3_05_08[j] = AlphaPeaks05_08[3][j][2];
    err_Peak1[j]   = Peak1_05_08[j] - Peak1_00_04[j];
    err_Peak2[j]   = Peak2_05_08[j] - Peak2_00_04[j];
    err_Peak3[j]   = Peak3_05_08[j] - Peak3_00_04[j];
    if (Peak1_00_04[j]==0 || Peak2_00_04[j]==0 || Peak3_00_04[j]==0) {
      relative_err_Peak1[j] = err_Peak1[j]/1.;
      relative_err_Peak2[j] = err_Peak2[j]/1.;
      relative_err_Peak3[j] = err_Peak3[j]/1.;
    } else {
      relative_err_Peak1[j] = err_Peak1[j]/Peak1_00_04[j] * 100;
      relative_err_Peak2[j] = err_Peak2[j]/Peak2_00_04[j] * 100;
      relative_err_Peak3[j] = err_Peak3[j]/Peak3_00_04[j] * 100;
    }
  }
  TGraph* gr1_00_04 = new TGraph(NUM_STRIP,CHIndex,Peak1_00_04);
  TGraph* gr2_00_04 = new TGraph(NUM_STRIP,CHIndex,Peak2_00_04);
  TGraph* gr3_00_04 = new TGraph(NUM_STRIP,CHIndex,Peak3_00_04);
  TGraph* gr1_05_08 = new TGraph(NUM_STRIP,CHIndex,Peak1_05_08);
  TGraph* gr2_05_08 = new TGraph(NUM_STRIP,CHIndex,Peak2_05_08);
  TGraph* gr3_05_08 = new TGraph(NUM_STRIP,CHIndex,Peak3_05_08);
  TGraph* gr1_err   = new TGraph(NUM_STRIP,CHIndex,err_Peak1);
  TGraph* gr2_err   = new TGraph(NUM_STRIP,CHIndex,err_Peak2);
  TGraph* gr3_err   = new TGraph(NUM_STRIP,CHIndex,err_Peak3);
  TGraph* gr1_relative_err = new TGraph(NUM_STRIP,CHIndex,relative_err_Peak1);
  TGraph* gr2_relative_err = new TGraph(NUM_STRIP,CHIndex,relative_err_Peak2);
  TGraph* gr3_relative_err = new TGraph(NUM_STRIP,CHIndex,relative_err_Peak3);
  TLegend* legend = new TLegend(0.55,0.25,0.88,0.4);

  gr1_00_04->GetXaxis()->SetRangeUser(-1,17);
  gr1_00_04->GetXaxis()->SetNdivisions(118);
  gr2_00_04->GetXaxis()->SetRangeUser(-1,17);
  gr2_00_04->GetXaxis()->SetNdivisions(118);
  gr3_00_04->GetXaxis()->SetRangeUser(-1,17);
  gr3_00_04->GetXaxis()->SetNdivisions(118);

  gr1_05_08->GetXaxis()->SetRangeUser(-1,17);
  gr1_05_08->GetXaxis()->SetNdivisions(118);
  gr1_05_08->GetXaxis()->SetRangeUser(-1,17);
  gr1_05_08->GetXaxis()->SetNdivisions(118);
  gr1_05_08->GetXaxis()->SetRangeUser(-1,17);
  gr1_05_08->GetXaxis()->SetNdivisions(118);

  gr1_err->GetXaxis()->SetRangeUser(-1,17);
  gr1_err->GetXaxis()->SetNdivisions(118);
  gr1_err->GetXaxis()->SetRangeUser(-1,17);
  gr1_err->GetXaxis()->SetNdivisions(118);
  gr1_err->GetXaxis()->SetRangeUser(-1,17);
  gr1_err->GetXaxis()->SetNdivisions(118);

  gr1_relative_err->GetXaxis()->SetRangeUser(-1,17);
  gr1_relative_err->GetXaxis()->SetNdivisions(118);
  gr2_relative_err->GetXaxis()->SetRangeUser(-1,17);
  gr2_relative_err->GetXaxis()->SetNdivisions(118);
  gr3_relative_err->GetXaxis()->SetRangeUser(-1,17);
  gr3_relative_err->GetXaxis()->SetNdivisions(118);

  gr1_00_04->SetMarkerStyle(20);
  gr1_00_04->SetMarkerColor(kRed);
  gr2_00_04->SetMarkerStyle(20);
  gr2_00_04->SetMarkerColor(kGreen);
  gr3_00_04->SetMarkerStyle(20);
  gr3_00_04->SetMarkerColor(kBlue);

  gr1_05_08->SetMarkerStyle(20);
  gr1_05_08->SetMarkerColor(kRed);
  gr2_05_08->SetMarkerStyle(20);
  gr2_05_08->SetMarkerColor(kGreen);
  gr3_05_08->SetMarkerStyle(20);
  gr3_05_08->SetMarkerColor(kBlue);

  gr1_err->SetMarkerStyle(20);
  gr1_err->SetMarkerColor(kRed);
  gr2_err->SetMarkerStyle(20);
  gr2_err->SetMarkerColor(kGreen);
  gr3_err->SetMarkerStyle(20);
  gr3_err->SetMarkerColor(kBlue);

  gr1_relative_err->SetMarkerStyle(20);
  gr1_relative_err->SetMarkerColor(kRed);
  gr2_relative_err->SetMarkerStyle(20);
  gr2_relative_err->SetMarkerColor(kGreen);
  gr3_relative_err->SetMarkerStyle(20);
  gr3_relative_err->SetMarkerColor(kBlue);

  TMultiGraph* mgr_00_04 = new TMultiGraph();
  TMultiGraph* mgr_05_08 = new TMultiGraph();
  TMultiGraph* mgr_err   = new TMultiGraph();
  TMultiGraph* mgr_relative_err = new TMultiGraph();
  mgr_00_04->Add(gr1_00_04);
  mgr_00_04->Add(gr2_00_04);
  mgr_00_04->Add(gr3_00_04);
  mgr_05_08->Add(gr1_05_08);
  mgr_05_08->Add(gr2_05_08);
  mgr_05_08->Add(gr3_05_08);
  mgr_err->Add(gr1_err);
  mgr_err->Add(gr2_err);
  mgr_err->Add(gr3_err);
  mgr_relative_err->Add(gr1_relative_err);
  mgr_relative_err->Add(gr2_relative_err);
  mgr_relative_err->Add(gr3_relative_err);

  legend->AddEntry(gr1_00_04,"peak1","lp");
  legend->AddEntry(gr2_00_04,"peak2","lp");
  legend->AddEntry(gr3_00_04,"peak3","lp");

  TCanvas* cans = new TCanvas("cans","cans",1000,1000);
  cans->Divide(2,2,small,small);
  cans->cd(1);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  mgr_00_04->SetTitle(AlphaCali00_04.c_str());
  mgr_00_04->GetXaxis()->SetRangeUser(-1,17);
  mgr_00_04->GetXaxis()->SetNdivisions(118);
  mgr_00_04->GetXaxis()->SetTitle("SSD Strip");
  mgr_00_04->GetXaxis()->SetTitleSize(0.04);
  mgr_00_04->GetXaxis()->CenterTitle(1);
  mgr_00_04->GetYaxis()->SetRangeUser(0.,400.);
  mgr_00_04->GetYaxis()->SetTitle("ADC Channel");
  mgr_00_04->GetYaxis()->SetTitleSize(0.04);
  mgr_00_04->GetYaxis()->CenterTitle(1);
  mgr_00_04->Draw("APL");
  legend->Draw("SAME");

  cans->cd(2);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  mgr_05_08->SetTitle(AlphaCali05_08.c_str());
  mgr_05_08->GetXaxis()->SetRangeUser(-1,17);
  mgr_05_08->GetXaxis()->SetNdivisions(118);
  mgr_05_08->GetXaxis()->SetTitle("SSD Strip");
  mgr_05_08->GetXaxis()->SetTitleSize(0.04);
  mgr_05_08->GetXaxis()->CenterTitle(1);
  mgr_05_08->GetYaxis()->SetRangeUser(0.,400.);
  mgr_05_08->GetYaxis()->SetTitle("ADC Channel");
  mgr_05_08->GetYaxis()->SetTitleSize(0.04);
  mgr_05_08->GetYaxis()->CenterTitle(1);
  mgr_05_08->Draw("APL");

  cans->cd(3);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  mgr_err->GetXaxis()->SetRangeUser(-1,17);
  mgr_err->GetXaxis()->SetNdivisions(118);
  mgr_err->GetXaxis()->SetTitle("SSD Strip");
  mgr_err->GetXaxis()->SetTitleSize(0.04);
  mgr_err->GetXaxis()->CenterTitle(1);
  mgr_err->GetYaxis()->SetRangeUser(0.,15.);
  mgr_err->GetYaxis()->SetNdivisions(515);
  mgr_err->GetYaxis()->SetTitle("Errors (Peak Shift)");
  mgr_err->GetYaxis()->SetTitleSize(0.04);
  mgr_err->GetYaxis()->CenterTitle(1);
  mgr_err->Draw("APL");

  cans->cd(4);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  mgr_relative_err->GetXaxis()->SetRangeUser(-1,17);
  mgr_relative_err->GetXaxis()->SetNdivisions(118);
  mgr_relative_err->GetXaxis()->SetTitle("SSD Strip");
  mgr_relative_err->GetXaxis()->SetTitleSize(0.04);
  mgr_relative_err->GetXaxis()->CenterTitle(1);
  mgr_relative_err->GetYaxis()->SetRangeUser(0.,5.);
  mgr_relative_err->GetYaxis()->SetNdivisions(1005);
  mgr_relative_err->GetYaxis()->SetTitle("Relative Errors (%)");
  mgr_relative_err->GetYaxis()->SetTitleSize(0.04);
  mgr_relative_err->GetYaxis()->CenterTitle(1);
  mgr_relative_err->Draw("APL");

  cans->Print(pathPNGOutput.c_str());
}

//______________________________________________________________________________
void CSHINEAlphaCali::AlphaCali_SSD4_L2F_EstimateCH13_CH14()
{
  Float_t  small   = 0.001;
  Int_t    PeakNum = 3;
  Double_t PeakIndex[3] = {1., 2., 3.};
  Double_t SSD4_CH13_LowPeaks [3] = {166.966, 172.312, 177.245};
  Double_t SSD4_CH13_HighPeaks[3] = {330.585, 347.711, 364.161};
  Double_t SSD4_CH14_LowPeaks [3] = {168.828, 174.303, 179.306};
  Double_t SSD4_CH14_HighPeaks[3] = {331.057, 348.248, 364.35 };

  std::string pathPNGOut(Form("%sfigure_AlphaCali/SSD4_L2F_CH13-14_AlphaCali00_48.png",PATHFIGURESFOLDER));
  //____________________________________________________________________________
  Double_t err_CH13[PeakNum];
  Double_t err_CH14[PeakNum];
  for (Int_t i=0; i<PeakNum; i++) {
    err_CH13[i] = 100*(SSD4_CH13_HighPeaks[i]-SSD4_CH13_LowPeaks[i])/(SSD4_CH13_LowPeaks[i]);
    err_CH14[i] = 100*(SSD4_CH14_HighPeaks[i]-SSD4_CH14_LowPeaks[i])/(SSD4_CH14_LowPeaks[i]);
  }

  TGraph* grap_CH13_LowPeaks  = new TGraph(PeakNum,PeakIndex,SSD4_CH13_LowPeaks);
  TGraph* grap_CH13_HighPeaks = new TGraph(PeakNum,PeakIndex,SSD4_CH13_HighPeaks);
  TGraph* grap_CH14_LowPeaks  = new TGraph(PeakNum,PeakIndex,SSD4_CH14_LowPeaks);
  TGraph* grap_CH14_HighPeaks = new TGraph(PeakNum,PeakIndex,SSD4_CH14_HighPeaks);
  TGraph* grap_err_CH13 = new TGraph(PeakNum,PeakIndex,err_CH13);
  TGraph* grap_err_CH14 = new TGraph(PeakNum,PeakIndex,err_CH14);
  TMultiGraph* mg_CH13  = new TMultiGraph();
  TMultiGraph* mg_CH14  = new TMultiGraph();

  grap_CH13_LowPeaks ->SetMarkerStyle(20);
  grap_CH13_LowPeaks ->SetMarkerColor(kRed);
  grap_CH13_LowPeaks ->SetMarkerSize(1.2);
  grap_CH13_HighPeaks->SetMarkerStyle(20);
  grap_CH13_HighPeaks->SetMarkerColor(kBlue);
  grap_CH13_HighPeaks->SetMarkerSize(1.2);

  grap_CH14_LowPeaks ->SetMarkerStyle(20);
  grap_CH14_LowPeaks ->SetMarkerColor(kRed);
  grap_CH14_LowPeaks ->SetMarkerSize(1.2);
  grap_CH14_HighPeaks->SetMarkerStyle(20);
  grap_CH14_HighPeaks->SetMarkerColor(kBlue);
  grap_CH14_HighPeaks ->SetMarkerSize(1.2);

  grap_err_CH13->SetMarkerStyle(20);
  grap_err_CH13->SetMarkerColor(kGreen);
  grap_err_CH13->SetMarkerSize(1.2);
  grap_err_CH14->SetMarkerStyle(20);
  grap_err_CH14->SetMarkerColor(kGreen);
  grap_err_CH14->SetMarkerSize(1.2);

  mg_CH13->Add(grap_CH13_LowPeaks);
  mg_CH13->Add(grap_CH13_HighPeaks);
  mg_CH14->Add(grap_CH14_LowPeaks);
  mg_CH14->Add(grap_CH14_HighPeaks);

  TCanvas* cans_CH13_14 = new TCanvas("cans_CH13_14","cans_CH13_14",1000,1000);
  cans_CH13_14->Divide(2,2,small,small);
  cans_CH13_14->cd(1);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  mg_CH13->SetTitle("SSD4_L2F_CH13_AlphaCali00_48");
  mg_CH13->GetXaxis()->SetRangeUser(0.,4.);
  mg_CH13->GetXaxis()->SetNdivisions(104);
  mg_CH13->GetXaxis()->SetTitle("Peak");
  mg_CH13->GetXaxis()->SetTitleSize(0.04);
  mg_CH13->GetXaxis()->CenterTitle(1);
  mg_CH13->GetYaxis()->SetRangeUser(100.,400.);
  mg_CH13->GetXaxis()->SetNdivisions(310);
  mg_CH13->GetYaxis()->SetTitle("ADC Channel");
  mg_CH13->GetYaxis()->SetTitleSize(0.04);
  mg_CH13->GetYaxis()->CenterTitle(1);
  mg_CH13->Draw("APL");

  cans_CH13_14->cd(2);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  mg_CH14->SetTitle("SSD4_L2F_CH13_AlphaCali00_48");
  mg_CH14->GetXaxis()->SetRangeUser(0.,4.);
  mg_CH14->GetXaxis()->SetNdivisions(104);
  mg_CH14->GetXaxis()->SetTitle("Peak");
  mg_CH14->GetXaxis()->SetTitleSize(0.04);
  mg_CH14->GetXaxis()->CenterTitle(1);
  mg_CH14->GetYaxis()->SetRangeUser(100.,400.);
  mg_CH14->GetXaxis()->SetNdivisions(310);
  mg_CH14->GetYaxis()->SetTitle("ADC Channel");
  mg_CH14->GetYaxis()->SetTitleSize(0.04);
  mg_CH14->GetYaxis()->CenterTitle(1);
  mg_CH14->Draw("APL");

  cans_CH13_14->cd(3);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  grap_err_CH13->SetTitle("SSD4_L2F_CH13_AlphaCali00_48");
  grap_err_CH13->GetXaxis()->SetRangeUser(0.,4.);
  grap_err_CH13->GetXaxis()->SetNdivisions(104);
  grap_err_CH13->GetXaxis()->SetTitle("Peak");
  grap_err_CH13->GetXaxis()->SetTitleSize(0.04);
  grap_err_CH13->GetXaxis()->CenterTitle(1);
  grap_err_CH13->GetYaxis()->SetRangeUser(90.,110.);
  grap_err_CH13->GetYaxis()->SetNdivisions(510);
  grap_err_CH13->GetYaxis()->SetTitle("Relative Errors (%)");
  grap_err_CH13->GetYaxis()->SetTitleSize(0.04);
  grap_err_CH13->GetYaxis()->CenterTitle(1);
  grap_err_CH13->Draw("APL");

  cans_CH13_14->cd(4);
  gPad->SetGridx();
  gPad->SetLeftMargin(0.13);
  grap_err_CH14->SetTitle("SSD4_L2F_CH14_AlphaCali00_48");
  grap_err_CH14->GetXaxis()->SetRangeUser(0.,4.);
  grap_err_CH14->GetXaxis()->SetNdivisions(104);
  grap_err_CH14->GetXaxis()->SetTitle("Peak");
  grap_err_CH14->GetXaxis()->SetTitleSize(0.04);
  grap_err_CH14->GetXaxis()->CenterTitle(1);
  grap_err_CH14->GetYaxis()->SetRangeUser(90.,110.);
  grap_err_CH14->GetYaxis()->SetNdivisions(510);
  grap_err_CH14->GetYaxis()->SetTitle("Relative Errors (%)");
  grap_err_CH14->GetYaxis()->SetTitleSize(0.04);
  grap_err_CH14->GetYaxis()->CenterTitle(1);
  grap_err_CH14->Draw("APL");

  cans_CH13_14->Print(pathPNGOut.c_str());
}
//******************************************************************************


//******************************************************************************
//                        ------------------
//                             手动选点
//                        ------------------
//    This function is to set the fit range by hand on GUITObject *selected
//    Click the Central button of the mouse to get the range
//
//    Function parameters:
//       event :   default value = 2
//           x :   x value of the point
//           y :   y value of the point
//    selected :   a pointer to the curent point
////////////////////////////////////////////////////////////////////////////////
void CSHINEAlphaCali::SetPoints(Int_t event, Int_t x, Int_t y, TObject* selected)
{
  if(event == 2) {
    Float_t px = gPad->AbsPixeltoX(x); // Conversion of absolute pixel to X
    Float_t py = gPad->AbsPixeltoX(y); // CoYversion of absolute pixel to Y
    py = gPad->PadtoY(py);             // Convert y from pad to Y
    Float_t Uymin = gPad->GetUymin();  // Returns the minimum/maximum y-coordinate
    Float_t Uymax = gPad->GetUymax();  // value visible on the pad
    //  save the clicks as a marker
    if(px>=gPad->GetUxmin() && px<=gPad->GetUxmax()) {
      m[fNPoints] = new TMarker(px,py,3);  // marker style = 3, means “*”
      l[fNPoints] = new TLine(px,Uymin,px,Uymax);
      l[fNPoints] -> SetLineColor(2);
      l[fNPoints] -> SetLineWidth(2);
      l[fNPoints] -> Draw();
      for(Int_t i=0; i<fNPoints; i++) {
        l[i] -> Draw();
      }
      gPad->Modified();
      gPad->Update();
      printf("x=%f\n", px);
      fNPoints++;

      // Important!!!  I get only 2 points and then I return
      if (fNPoints == 6) {
        fIndex = 1;   // fIndex
        return;
      }
      cout<< "Click next point"<< endl;
    }
  }
  return;
}
//******************************************************************************
