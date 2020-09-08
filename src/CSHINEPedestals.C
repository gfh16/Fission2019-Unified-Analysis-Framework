#include "../include/CSHINEPedestals.h"


//******************************************************************************
// 构造函数
CSHINEPedestals::CSHINEPedestals()
{}

//析构函数
CSHINEPedestals::~CSHINEPedestals()
{}
//******************************************************************************


//******************************************************************************
//                          -------------------------
//                              自动提取 pedestals
//                          ------------------------
//    通过 TSpectrum 进行自动寻峰. 通过指定区间，可以快速进行自动寻峰.
//    自动寻峰有以下不足之处就是：
//      1. 只能得到峰值，得不到 sigma 以及其他参数. 如果需要知道其他参数, 则不能使用自动寻峰
//      2. 如果存在多个峰，自动寻峰是无序的，则峰值不按大小排列.
//
//    对于 pedestals，由于只需要得到峰位, 可以使用自动寻峰, 不过这样一来就得不到 sigma 值.
//
//                             -----------------
//                                自动寻峰步骤
//                             -----------------
//   1.从文件中读取已有的的 hitograms
//   2.新建TSpectrum：TSpectrum * s = new TSpectrum();
//   3.寻峰(寻找候选峰): Int_t nfound  = s->Search(PulserPeaks[i],3,"",0.2);
//   4.对所有峰进行排序
//   5.遍历所有找到的峰，提取峰值的 X值、Y值，拟合
//   6.将寻峰得到的数据点画出来, 作线性拟合
//
//   PulserCali_AutoFindPeak()参数说明
//   LayerTagWithOutLabel   : L1 or L2
//   AlphaTag    : Switch / Height，We use this two methods for pulser cali
//   cans[4][16]: Canvas for to save all the figures
//
//   Search()参数说明：
//   Search(hist, sigma, option, threshold) \\ 0 < threshold < 1
//   sigma    : sigma of searched peaks， search
//   threshold: peaks with amplitude < threshold*highest_peak are discarded
//
//    gfh, 2019-11-11
//    gfh, 2020-09-07 修改成类的形式
////////////////////////////////////////////////////////////////////////////////
void CSHINEPedestals::AutoFindPedestals(const char* layertag, const char* filetag)
{
  Int_t lowrange  = 0;
  Int_t highrange = 200;
  Int_t Num_CH;
  if (strcmp(layertag, "L3A") == 0) {
    Num_CH = NUM_CSI;
  } else {
    Num_CH = NUM_STRIP;
  }

  std::string LayerTagWithOutLabel;
  if (strcmp(filetag, "Pedestal0000") == 0) {
    LayerTagWithOutLabel = "PulserCali";
  } else if (strcmp(layertag,"L1S")==0) {
      LayerTagWithOutLabel = "L1";
  } else {
      LayerTagWithOutLabel = "L2";
  }

  std::string pathRootInFolder(Form("%sQualityCheck/", PATHROOTFILESFOLDER));
  std::string pathRootInput(Form("%sQC_MapSSD_%s_%s.root", pathRootInFolder.c_str(), LayerTagWithOutLabel.c_str(), filetag));
  std::string pathPDFOutput(Form("%sfigure_Pedestals/SSD_%s_AlphaCaliPedestals_%s.pdf" , PATHFIGURESFOLDER, layertag, filetag));
  std::string pathPDFbegin (Form("%sfigure_Pedestals/SSD_%s_AlphaCaliPedestals_%s.pdf[", PATHFIGURESFOLDER, layertag, filetag));
  std::string pathPDFend   (Form("%sfigure_Pedestals/SSD_%s_AlphaCaliPedestals_%s.pdf]", PATHFIGURESFOLDER, layertag, filetag));
  std::string pathAlphaCaliPedestalsOutput(Form("%sdata_Pedestals/SSD_%s_AlphaCaliPedestals_%s.dat",PATHDATAFOLDER,layertag, filetag));

  FILE * FileOut = fopen(pathAlphaCaliPedestalsOutput.c_str(),"w");
  fprintf(FileOut,"* SSDNum   CHNum  Mean  \n");

  TFile* FileIn = new TFile(pathRootInput.c_str());
  if (!FileIn->IsOpen()) {
    cout<<"Open file "<< pathRootInput.c_str() << " failed"<<endl;
    return;
  }

  TCanvas* c1 = new TCanvas("c1","c1");
  TCanvas* c_begin = new TCanvas();
  TCanvas* c_end   = new TCanvas();
  c_begin->Close();
  c_end->Close();
  c_begin->Print(pathPDFbegin.c_str());

  TH1D* Pedestals[NUM_SSD][Num_CH];
  std::string histname[NUM_SSD][Num_CH];
  for (Int_t SSDNum=0; SSDNum<NUM_SSD; SSDNum++)
  {
    // 读取root文件中的 Histograms
    for (Int_t CHNum=0; CHNum<Num_CH; CHNum++)
    {
      histname [SSDNum][CHNum] = Form("SSD%d_%s_E_CH%02d",SSDNum+1,layertag,CHNum);
      Pedestals[SSDNum][CHNum] = (TH1D*)FileIn->Get(histname[SSDNum][CHNum].c_str());
      Pedestals[SSDNum][CHNum]->GetXaxis()->SetRangeUser(lowrange, highrange);
      cout << histname[SSDNum][CHNum].c_str() << endl;
    }
    printf("Histograms loaded\n");

    //____________________________________________________________________________
    //                        BEGIN ANALYZE HERE !!!
    TSpectrum* s = new TSpectrum();
    for (Int_t CHNum=0; CHNum<Num_CH; CHNum++)
    {
      if (Pedestals[SSDNum][CHNum]==0) {
        printf("No data present for %s",histname[SSDNum][CHNum].c_str());
        continue;
      }
      c1->cd();
      Int_t nfound  = s->Search(Pedestals[SSDNum][CHNum],3,"",0.4);
      printf("%s is analyzed,%2d peaks found\n", histname[SSDNum][CHNum].c_str(), nfound);
      c1->Print(pathPDFOutput.c_str());

      Int_t npeaks = 0;
      Double_t* xpeaks = s->GetPositionX();
      fprintf(FileOut, "%5d  %5d  %10.1f \n", SSDNum, CHNum, xpeaks[0]);
      fflush(FileOut);

      gPad->Modified();
      gPad->Update();
    }
  }
  c_end->Print(pathPDFend.c_str());

  fclose(FileOut);
  return;
}
//******************************************************************************


//******************************************************************************
//    相比于自动寻峰, 手动拟合的主要优点是： 可以得到拟合的所有参数！
//
//                          -----------------
//                             自动寻峰步骤
//                          -----------------
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
////////////////////////////////////////////////////////////////////////////////
void CSHINEPedestals::ClickToFindPedestals(const char* layertag, const char* filetag)
{
  std::string LayerTagWithOutLabel;
  if (strcmp(layertag,"L1S")==0) {
      LayerTagWithOutLabel = "L1";
  } else {
      LayerTagWithOutLabel = "L2";
  }

  std::string pathRootFolder(Form("%sQualityCheck/", PATHROOTFILESFOLDER));
  std::string FileOutTag;
  std::string pathRootInput;
  if (strcmp(filetag,"Pedestal0000")==0) {
   FileOutTag     = "PulserCaliPedestals";
   pathRootInput  = Form("%sQC_MapSSD_PulserCali_%s.root", pathRootFolder.c_str(), filetag);

  } else {
    FileOutTag    = "AlphaCaliPedestals";
    pathRootInput = Form("%sQC_MapSSD_%s_%s.root", pathRootFolder.c_str(), LayerTagWithOutLabel.c_str(), filetag);
  }
  std::string pathPedestalsOutput(Form("%sdata_Pedestals/SSD_%s_%s_%s.dat", PATHDATAFOLDER, layertag, FileOutTag.c_str(), filetag));
  std::string pathPDFOutput(Form("%sfigure_Pedestals/SSD_%s_%s_%s.pdf",  PATHFIGURESFOLDER, layertag, FileOutTag.c_str(), filetag));
  std::string pathPDFbegin (Form("%sfigure_Pedestals/SSD_%s_%s_%s.pdf[", PATHFIGURESFOLDER, layertag, FileOutTag.c_str(), filetag));
  std::string pathPDFend   (Form("%sfigure_Pedestals/SSD_%s_%s_%s.pdf]", PATHFIGURESFOLDER, layertag, FileOutTag.c_str(), filetag));

  ofstream FileOut(pathPedestalsOutput.c_str());
  FileOut << "*SSDNum" << setw(7) << "CHNum" << setw(8) << "mean"<< setw(10)<< "sigma"<< setw(14)
          <<"mean+5sigma"<< setw(10)<<"Chi2" << setw(10) <<"Xmin"<< setw(10)<<"Xmax" << endl;

  TFile* FileIn = new TFile(pathRootInput.c_str());
  if(!FileIn->IsOpen()) {
    cout<<"Open file "<< pathRootInput.c_str() << " failed"<<endl;
    return;
  }

  TH1D* PedestalHist  [NUM_SSD][NUM_STRIP];
  std::string histname[NUM_SSD][NUM_STRIP];
  for(Int_t i=0; i<NUM_SSD; i++) {
    for(Int_t j=0; j<NUM_STRIP; j++) {
      histname[i][j] = Form("SSD%d_%s_E_CH%02d",(i+1),layertag,j);
      PedestalHist[i][j]=(TH1D*)FileIn->Get(histname[i][j].c_str());
    }
  }
  printf("Histograms loaded\n");

  TCanvas *c1      = new TCanvas("c1","c1",800,600);
  TCanvas *c_begin = new TCanvas("c_begin","");
  c_begin->Close();
  TCanvas *c_end   = new TCanvas("c_end","");
  c_end->Close();

  //____________________________________________________________________________
  //                            BEGIN ANALYZE HERE !!!
  //
  // the pdf file begins here !!!
  c_begin->Print(pathPDFbegin.c_str());

  Float_t limit[2] = {0};   //定义limit[2]用于存储拟合范围
  for(Int_t i=0; i<NUM_SSD; i++)
  {
    for(Int_t j=0; j<NUM_STRIP; j++)
    {
      Int_t Option = 2;   // 设置一个标志, 当Option==2时，执行后面的while(Option==2)循环

      if (PedestalHist[i][j] == 0) {
        printf("No data present for SSD%d_%s_CH%02d\n",(i+1),layertag,j);
        continue;
      }
      while(Option == 2)
      {
        fIndex   = 0;
        fNPoints = 0;

        c1->cd();
        gPad->Modified();
        gPad->Update();
        PedestalHist[i][j]->GetXaxis()->SetRangeUser(50,160);
        PedestalHist[i][j]->GetYaxis()->SetRangeUser(0,(PedestalHist[i][j]->GetMaximum())*1.1);
        PedestalHist[i][j]->Draw();
        //____________________________________________________
        //   it is very important!!! very convenient!!!
        //   here to set the range and fit on the GUI by hand
        c1->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)","CSHINEPedestals",this,"SetPoints(Int_t,Int_t,Int_t,TObject*)");
        while(fIndex != 1) {
          usleep(100);
          gClient->HandleInput();
        }
        c1->Disconnect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)");

        if (fIndex == 1) {
          for(int ip=0; ip<fNPoints; ip++) {
            limit[ip] = m[ip]->GetX();
          }
        }
        //  对选取的范围进行拟合
        PedestalHist[i][j]->Fit("gaus","","same",limit[0],limit[1]);

        gPad->Modified();
        gPad->Update();

        cout<< "Options? 1.Save and next, 2.Retry the fit, 3.Exit"<< endl;
        cin >> Option;
        //  Option ==1，保存拟合结果
        if (Option==1) {
          TF1 *fit = PedestalHist[i][j]->GetFunction("gaus");
          Double_t Mean  = fit->GetParameter(1);    // Par[1] = Mean
          Double_t Sigma = fit->GetParameter(2);    // Par[2] = sigma
          Double_t Chi2  = fit->GetChisquare();

          cout.setf(ios::fixed);
          FileOut<<setw(5)<<std::right<<i<<setw(5)<<std::right<<j<<setw(15)
                 <<std::right<<setprecision(6)<<Mean<<setw(10)
                 <<std::right<<setprecision(6)<<Sigma<<setw(10)
                 <<std::right<<setprecision(6)<<Mean+5*Sigma<<setw(10)
                 <<std::right<<setprecision(6)<<Chi2<<setw(10)
                 <<std::right<<setprecision(6)<<limit[0]<<setw(10)
                 <<std::right<<setprecision(6)<<limit[1]
                 <<endl;

          c1->Print(pathPDFOutput.c_str());
        }
        // Option == 3, 中断跳出程序
        if (Option == 3) {
          c_end->Print(pathPDFend.c_str());
          FileOut.close();
          cout<<"File .dat closed"<<endl;

          delete c1;
          delete c_begin;
          delete c_end;
          return;
        }
      } // close while(Option==2)
    }   // close silicon strip  (j)
  }     // close silicon number (i)

  //  the pdf file ends here !!!
  c_end->Print(pathPDFend.c_str());
  FileOut.close();
  cout<<"File .dat closed"<<endl;

  delete c1;
  delete c_begin;
  delete c_end;
  return;
}
//******************************************************************************


//******************************************************************************
//   考察不同的实验条件下，硅条的 Pedestals 的差异.
//   考察的内容主要有:
//    1. 考察两次 alpha 刻度的 pedestals 的差异
//    2. 考察 alpha 刻度与脉冲刻度的 pedestals 差异
//
//   gfh, 2020-06-04
//   gfh, 2020-09-07 改写为类的形式
////////////////////////////////////////////////////////////////////////////////
void CSHINEPedestals::EstimateErrorsOfPedestals(const char* layertag, const char* filetag1,
  const char* filetag2)
{
  std::string CaliTag1;
  if (strcmp(filetag1, "Pedestal0000") == 0) {
      CaliTag1 = "PulserCaliPedestals";
  } else {
      CaliTag1 = "AlphaCaliPedestals";
  }
  std::string CaliTag2("AlphaCaliPedestals");
  std::string pathPedestal1(Form("%sdata_Pedestals/SSD_%s_%s_%s.dat", PATHDATAFOLDER, layertag, CaliTag1.c_str(), filetag1));
  std::string pathPedestal2(Form("%sdata_Pedestals/SSD_%s_%s_%s.dat", PATHDATAFOLDER, layertag, CaliTag2.c_str(), filetag2));
  std::string pathPNGOut(Form("%sfigure_Pedestals/SSD_%s_PedestalError_%s_%s.png", PATHFIGURESFOLDER, layertag, filetag1, filetag2));

   Int_t    numpar     =  2;    // mean, sigma
   Float_t  small      =  1e-5;
   Double_t XLow       =  0;
   Double_t XUp        =  17;
   Double_t LeftMargin =  0.13;

   ReadFileModule readfile;
   Double_t*** Pedestal1 = readfile.ReadData(pathPedestal1.c_str(), NUM_SSD, NUM_STRIP, numpar);
   Double_t*** Pedestal2 = readfile.ReadData(pathPedestal2.c_str(), NUM_SSD, NUM_STRIP, numpar);

   Double_t  SSDCHNum    [NUM_SSD][NUM_STRIP];
   Double_t  err_pedestal[NUM_SSD][NUM_STRIP];

   for (Int_t i=0; i<NUM_SSD; i++) {
     for (Int_t j=0; j<NUM_STRIP; j++) {
       SSDCHNum    [i][j]  =  j+1;
       err_pedestal[i][j]  =  TMath::Abs(Pedestal1[i][j][0]-Pedestal2[i][j][0]);
     }
   }

   // build graphs
   TGraph* graph[NUM_SSD];
   for (Int_t i=0; i<NUM_SSD; i++) {
     graph[i] = new TGraph(NUM_STRIP, SSDCHNum[i], err_pedestal[i]);
     graph[i]->SetTitle(Form("SSD%d_%s_PedestalError_%s_%s", i+1, layertag, filetag1, filetag2));
   }

   // draw
   TCanvas* cans = new TCanvas("cans","Error of Pedestals",1200,1000);
   cans->Divide(2,2,small,small);

   cans->cd(1);
   gPad->SetGridx();
   gPad->SetLeftMargin(LeftMargin);
   graph[0]->SetMarkerStyle(20);
   graph[0]->SetMarkerColor(kRed);
   graph[0]->GetXaxis()->SetRangeUser(XLow,XUp);
   graph[0]->GetXaxis()->SetNdivisions(117);
   graph[0]->GetXaxis()->SetTitle("Strip_Number");
   graph[0]->GetXaxis()->CenterTitle(1);
   graph[0]->GetXaxis()->SetTitleSize(0.04);
   graph[0]->GetYaxis()->SetTitle("PedestalError (Ch)");
   graph[0]->GetYaxis()->CenterTitle(1);
   graph[0]->GetYaxis()->SetTitleSize(0.04);
   graph[0]->Draw("APL");

   cans->cd(2);
   gPad->SetGridx();
   gPad->SetLeftMargin(LeftMargin);
   graph[1]->SetMarkerStyle(20);
   graph[1]->SetMarkerColor(kRed);
   graph[1]->GetXaxis()->SetRangeUser(XLow,XUp);
   graph[1]->GetXaxis()->SetNdivisions(117);
   graph[1]->GetXaxis()->SetTitle("Strip_Number");
   graph[1]->GetXaxis()->CenterTitle(1);
   graph[1]->GetXaxis()->SetTitleSize(0.04);
   graph[1]->GetYaxis()->SetTitle("PedestalError (Ch)");
   graph[1]->GetYaxis()->CenterTitle(1);
   graph[1]->GetYaxis()->SetTitleSize(0.04);
   graph[1]->Draw("APL");

   cans->cd(3);
   gPad->SetGridx();
   gPad->SetLeftMargin(LeftMargin);
   graph[2]->SetMarkerStyle(20);
   graph[2]->SetMarkerColor(kRed);
   graph[2]->GetXaxis()->SetRangeUser(XLow,XUp);
   graph[2]->GetXaxis()->SetNdivisions(117);
   graph[2]->GetXaxis()->SetTitle("Strip_Number");
   graph[2]->GetXaxis()->CenterTitle(1);
   graph[2]->GetXaxis()->SetTitleSize(0.04);
   graph[2]->GetYaxis()->SetTitle("PedestalError (Ch)");
   graph[2]->GetYaxis()->CenterTitle(1);
   graph[2]->GetYaxis()->SetTitleSize(0.04);
   graph[2]->Draw("APL");

   cans->cd(4);
   gPad->SetGridx();
   gPad->SetLeftMargin(LeftMargin);
   graph[3]->SetMarkerStyle(20);
   graph[3]->SetMarkerColor(kRed);
   graph[3]->GetXaxis()->SetRangeUser(XLow,XUp);
   graph[3]->GetXaxis()->SetNdivisions(117);
   graph[3]->GetXaxis()->SetTitle("Strip_Number");
   graph[3]->GetXaxis()->CenterTitle(1);
   graph[3]->GetXaxis()->SetTitleSize(0.04);
   graph[3]->GetYaxis()->SetTitle("PedestalError (Ch)");
   graph[3]->GetYaxis()->CenterTitle(1);
   graph[3]->GetYaxis()->SetTitleSize(0.04);
   graph[3]->Draw("APL");

   cans->Print(pathPNGOut.c_str());
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
void CSHINEPedestals::SetPoints(Int_t event, Int_t x, Int_t y, TObject* selected)
{
  if(event == 2)
  {
    Float_t px = gPad->AbsPixeltoX(x); // Conversion of absolute pixel to X
    Float_t py = gPad->AbsPixeltoX(y); // CoYversion of absolute pixel to Y
    py = gPad->PadtoY(py);             // Convert y from pad to Y
    Float_t Uymin = gPad->GetUymin();  // Returns the minimum/maximum y-coordinate
    Float_t Uymax = gPad->GetUymax();  // value visible on the pad

    //  save the clicks as a marker
    if(px>=gPad->GetUxmin() && px<=gPad->GetUxmax())
    {
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
      if (fNPoints == 2) {
        fIndex = 1;   // fIndex
        return;
      }
      cout<< "Click next point"<< endl;
    }
  }
  return;
}
//******************************************************************************
