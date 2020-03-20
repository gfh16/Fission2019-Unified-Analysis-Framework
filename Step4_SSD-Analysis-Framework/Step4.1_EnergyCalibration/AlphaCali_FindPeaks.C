////////////////////////////////////////////////////////////////////////////////////
//    THIS MACRO IS USED TO FIND THE PEAKS OF ALPHA SOURCE                        //
//    This macro enable us to set the fit range of each histogram by hand         //
//    Then the histogram is fitted using the user-defined function                //
//    The canva is saved as pdf                                                   //
//                                                                                //
//    Run this macro: root -l AlphaCali_FindPeak.cpp                                   //
//                                                                                //
//    Date: 2019-11-11                                                            //
//    Author: gfh                                                                 //
////////////////////////////////////////////////////////////////////////////////////

#include "TH1.h"
#include "TF1.h"
#include "TCanvas.h"

// 不考虑Mylar膜的能损,   239Pu, 241Am, 244Cm alpha加权能量
//double Ealpha[3] = {5.14718， 5.48038， 5.79521};

// 考虑alpha在 2um Mylar, 0.06um Al中的能损
double Ealpha[3] = {4.90407, 5.24768, 5.57147};  //MeV

Int_t SSDNum = 4;
Int_t CHNum  = 16;

Int_t Index = 0;
Int_t NPoints;
TMarker* m[6];
TLine* l[6];


//______________________________________________________________________________
Double_t*** ReadData(const Char_t* datapath, Int_t& SSDNum, Int_t& CHNum, Int_t& ParNum);

//______________________________________________________________________________
void DeleteData(Double_t*** p, Int_t& SSDNum, Int_t& CHNum, Int_t& ParNum);

//___________________________________________________________________________
void AlphaCali_FindPeaks()
{
  /////////////////////////////////////////////////////////////////////////////
  //                                                                         //
  //   1. 读取Histograms                                                     //
  //   2. 通过c1->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",0,0,  //
  //                     "SetPoints(Int_t,Int_t,Int_t,TObject*)")调用GUI     //
  //   3. 定义一个正数Option，作为操作选项：                                 //
  //     (1) Option 初始化为 2                                               //
  //     (2) Option = 2: 调用 SetPoints(), 手动选点                          //
  //     (3) 提示输入Option：                                                //
  //         <1> 输入 1, 进行拟合并保存;                                     //
  //         <2> 输入 2, 在此执行while(Option==2)                            //
  //         <3> 输入 3，中断跳出                                            //
  //  4. 三组分alpha源有3个能量峰,需要6个点来拟合                            //
  /////////////////////////////////////////////////////////////////////////////

   std::string LayerTag("L1S");                // "L1S" or "L2F" or "L2B"
   std::string LayerTagWithoutLabel("L1");     //  L1 or L2
   std::string AlphaFileTag("AlphaCali00_08"); //  for L1:00_04,00_08,05_08;  for L2: 00_32,33_48

   std::string L1Tag("L1");
   std::string L2FTag("L2F");
   std::string L2BTag("L2B");
   Double_t HistXLowRange[SSDNum];
   Double_t HistXUpRange[SSDNum];
   if (strcmp(L1Tag.c_str(), LayerTagWithoutLabel.c_str())==0) {
       HistXLowRange[0] = 90.,  HistXUpRange[0] = 230.;
       HistXLowRange[1] = 90,   HistXUpRange[1] = 230.;
       HistXLowRange[2] = 110., HistXUpRange[2] = 210.;
       HistXLowRange[3] = 160., HistXUpRange[3] = 310.;
   } else if (strcmp(L2FTag.c_str(), LayerTag.c_str())==0) {     // for L2F
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

   std::string AlphaFitTag("3AlphaFitPars");
   std::string AlphaPeaksTag("AlphaPeaks");

   //std::string pathFileInput(Form("/data/EXPdata/Fission2019_Data/MapRoot/MapSSD_L2_%s.root", AlphaFileTag.c_str()));
   //std::string pathFileInput(Form("rootinput/QC_MapSSD_L2_%s.root", AlphaFileTag.c_str()));
   std::string pathFileInput(Form("/home/sea/Fission2019_Data/MapRoot/MapSSD_%s_%s.root", LayerTagWithoutLabel.c_str(), AlphaFileTag.c_str()));
   std::string pathPedestalInput(Form("output/SSD_%s_Pedestals.dat",LayerTag.c_str()));

   std::string pathAlphaPeaksOutput(Form("output/SSD_%s_%s_%s.dat", LayerTag.c_str(), AlphaPeaksTag.c_str(), AlphaFileTag.c_str()));
   std::string pathAlphaFitResultsOutput(Form("output/SSD_%s_%s_%s.dat",LayerTag.c_str(), AlphaFitTag.c_str(), AlphaFileTag.c_str()));

   std::string pathPDFOutput(Form("figures/SSD_%s_%s_%s.pdf", LayerTag.c_str(), AlphaPeaksTag.c_str(), AlphaFileTag.c_str()));
   std::string pathPDFbegin(Form("figures/SSD_%s_%s_%s.pdf[", LayerTag.c_str(), AlphaPeaksTag.c_str(), AlphaFileTag.c_str()));
   std::string pathPDFend(Form("figures/SSD_%s_%s_%s.pdf]", LayerTag.c_str(), AlphaPeaksTag.c_str(), AlphaFileTag.c_str()));

   //   在此修改输出文件路径
   ofstream FileOut(pathAlphaPeaksOutput.c_str());
   FileOut <<"*SSDNum" <<setw(7)<<"CHNum"<<setw(8)
           <<"mean1" << setw(10)<<"mean2" <<setw(10)<<"mean3" <<setw(10)
           <<"sigma1"<< setw(10)<<"sigma2"<<setw(10)<<"sigma3"<<setw(10)
           <<"Chi21" << setw(10)<<"Chi22" <<setw(10)<<"Chi23" <<setw(10)
           <<"Xmin1"<< setw(10)<<"Xmax1"<<setw(10)<<"Xmin2"<< setw(10)<<"Xmax2"<<setw(10)<<"Xmin3"<<setw(10)<<"Xmax3"<<endl;

   ofstream FileOutFit(pathAlphaFitResultsOutput.c_str());
   FileOutFit<<" *Fit function = par[0]+par[1]*x && y = a*x + b (y=Energy, x=Ch);  so I define a = par[1], b = par[0]\n";
   FileOutFit<<" *SSDNum"<<setw(7)<<"CHNum"<<setw(10)<<"par_a"<<setw(15)<<"Errpar_a"<<setw(15)<<"par_b"<<setw(17)<<"Errpar_b\n";


   TFile* FileIn = new TFile(pathFileInput.c_str());

   if (!FileIn->IsOpen()) {
     cout<<"Open file "<< pathFileInput.c_str() << " failed. "<<endl;
     return;
   } else cout<<"file "<< pathFileInput.c_str() << " loaded. "<<endl;

   //    定义、读取输入文件中的 Histograms
   TH1D* AlphaCaliHist[4][16];
   for (Int_t SSDNum=0; SSDNum<4; SSDNum++) {
     for (Int_t CHNum=0; CHNum<16; CHNum++) {
       AlphaCaliHist[SSDNum][CHNum]=(TH1D*)FileIn->Get(Form("SSD%d_%s_E_CH%02d",(SSDNum+1),LayerTag.c_str(),CHNum));
     }
   }

   // read pedestal from pedestal file
   Int_t numpar_PedestalIn = 3; // mean, sigma, mean+5*sigma
   Double_t*** PedestalIn = ReadData(pathPedestalInput.c_str(), SSDNum, CHNum, numpar_PedestalIn);


   TCanvas* c1 = new TCanvas("c1","c1",1000,1200);
   c1->Divide(1,2);
   TCanvas* c_begin = new TCanvas("c_begin","");
   TCanvas* c_end   = new TCanvas("c_end","");
   c_begin->Close();
   c_end->Close();
   c_begin->Print(pathPDFbegin.c_str());


   //====================================================================================
   //                            BEGIN ANALYZE HERE !!!

   Double_t limit[6] = {0};   //定义limit[2]用于存储拟合范围
   Double_t par[9] = {40,250,15,20,260,15,10,270,15}; // 拟合参数初始化

   for (Int_t SSDNum=0; SSDNum<4; SSDNum++) {
     for (Int_t CHNum=0; CHNum<16; CHNum++) {

       Int_t Option = 2;   // 设置一个标志, 当Option==2时，执行后面的while(Option==2)循环

       if (AlphaCaliHist[SSDNum][CHNum]==0) {
         printf("No data present for SSD%d_%s_E_CH%02d\n",(SSDNum+1),LayerTag.c_str(),CHNum);
         continue;
       }
       while (Option==2) {
         Index = 0;
         NPoints = 0;

         c1->cd(1);
         gPad->Modified();
         gPad->Update();
         Double_t Pedestal_CH = PedestalIn[SSDNum][CHNum][2];
         AlphaCaliHist[SSDNum][CHNum]->GetXaxis()->SetRangeUser(Pedestal_CH+HistXLowRange[SSDNum], Pedestal_CH+HistXUpRange[SSDNum]);
         AlphaCaliHist[SSDNum][CHNum]->GetYaxis()->SetRangeUser(0,(AlphaCaliHist[SSDNum][CHNum]->GetMaximum())*1.1);
         AlphaCaliHist[SSDNum][CHNum]->Draw();
         //________________________________________________________
         //   it is very important!!! very convenient!!!
         //   here to set the range and fit on the GUI by hand
         c1->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",0,0,"SetPoints(Int_t,Int_t,Int_t,TObject*)");
         while (Index!=1) {
           usleep(100);
           gClient->HandleInput();
         }
         c1->Disconnect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)");

         if (Index==1) {
           for(Int_t ip=0; ip<NPoints; ip++) {
             limit[ip] = m[ip]->GetX();
             cout<< limit[ip] <<endl;
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

         Double_t Peak1_Height = FitPeak1->GetParameter(0);  // Par[1] = Height
         Double_t Peak1_Mean   = FitPeak1->GetParameter(1);  // Par[1] = Mean
         Double_t Peak1_Sigma  = FitPeak1->GetParameter(2);  // Par[2] = sigma
         Double_t Peak1_Chi2   = FitPeak1->GetChisquare();

         Double_t Peak2_Height = FitPeak2->GetParameter(0);  // Par[1] = Height
         Double_t Peak2_Mean   = FitPeak2->GetParameter(1);  // Par[1] = Mean
         Double_t Peak2_Sigma  = FitPeak2->GetParameter(2);  // Par[2] = sigma
         Double_t Peak2_Chi2   = FitPeak2->GetChisquare();

         Double_t Peak3_Height = FitPeak3->GetParameter(0);  // Par[1] = Height
         Double_t Peak3_Mean   = FitPeak3->GetParameter(1);  // Par[1] = Mean
         Double_t Peak3_Sigma  = FitPeak3->GetParameter(2);  // Par[2] = sigma
         Double_t Peak3_Chi2   = FitPeak3->GetChisquare();

         c1->cd(2);
         Double_t peaks[3] = {Peak1_Mean,Peak2_Mean,Peak3_Mean};
         TGraph* grap = new TGraph(3,peaks,Ealpha); // Energy vs Channel
         grap->SetMarkerStyle(20);
         grap->SetMarkerSize(1.5);
         grap->SetMarkerColor(kBlue);
         grap->SetTitle(Form("AlphaFits_SSD%d_%s_E_CH%02d",SSDNum+1,LayerTag.c_str(),CHNum));
         grap->Draw("AP*");
         TF1* fit = new TF1("fit","pol1",100,4096);
         grap->Fit("fit");
         Double_t par0     = fit->GetParameter(0);
         Double_t err_par0 = fit->GetParError(0);
         Double_t par1     = fit->GetParameter(1);
         Double_t err_par1 = fit->GetParError(1);

         gPad->Modified();
         gPad->Update();
         //  提示输入操作选项
         cout<< "Options? 1.Save and next, 2.Retry the fit, 3.Exit"<< endl;
         cin >> Option;

         //  Option ==1，保存拟合结果
         if (Option==1) {
           FileOut << SSDNum << setw(10) << CHNum << setw(12)
                   << Peak1_Mean <<setw(10)<<Peak2_Mean <<setw(10)<<Peak3_Mean <<setw(10)
                   << Peak1_Sigma<<setw(10)<<Peak2_Sigma<<setw(10)<<Peak3_Sigma<<setw(10)
                   << Peak1_Chi2 <<setw(10)<<Peak2_Chi2 <<setw(10)<<Peak3_Chi2 <<setw(10)
                   <<limit[0]<< setw(10)<<limit[1]<<setw(10)<<limit[2]<< setw(10)<<limit[3]<<setw(10)<<limit[4]<<setw(10)<<limit[5]<<endl;

           FileOutFit<<setw(5)<<SSDNum<<setw(7)<<CHNum<<setw(15)<<par1<<setw(15)<<err_par1<<setw(15)<<par0<<setw(15)<<err_par0<<endl;

           c1->Print(pathPDFOutput.c_str());
         }
         if (Option==3) {
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
   FileOutFit.close();
   cout<<"File .dat closed"<<endl;

   delete c1;
   delete c_begin;
   delete c_end;
   return;
} // end void FindPedestals()

//______________________________________________________________________________
void SetPoints(Int_t event, Int_t x, Int_t y, TObject* selected)
{
/////////////////////////////////////////////////////////////////////////////
//    This function is to set the fit range by hand on GUI                 //
//    Click the Central button of the mouse to get the range               //
//                                                                         //
//    Function parameters:                                                 //
//       event :   number of click points                                  //
//           x :   x value of the point                                    //
//           y :   y value of the point                                    //
//    selected :   a pointer to the curent point                           //
/////////////////////////////////////////////////////////////////////////////
  if (event == 2) {
    Float_t px = gPad->AbsPixeltoX(x); // Conversion of absolute pixel to X
    Float_t py = gPad->AbsPixeltoX(y); // CoYversion of absolute pixel to Y
    py = gPad->PadtoY(py);           // Convert y from pad to Y
    Float_t Uymin = gPad->GetUymin();  // Returns the minimum/maximum y-coordinate
    Float_t Uymax = gPad->GetUymax();  // value visible on the pad

    //  save the clicks as a marker
    if (px >= gPad->GetUxmin() && px <= gPad->GetUxmax()) {
      m[NPoints] = new TMarker(px,py,3);          // marker style = 3, means “*”
      l[NPoints] = new TLine(px,Uymin,px,Uymax);
      l[NPoints]->SetLineColor(kGreen);
      l[NPoints]->SetLineWidth(2);
      l[NPoints]->Draw();

      for(Int_t i=0; i<NPoints; i++) {
        l[i]->Draw();
      }
      gPad->Modified();
      gPad->Update();
      printf("x=%f\n", px);
      NPoints++;

      //  Important!!!  I get 6 points and then I return
      if (NPoints==6) {
        Index = 1;   // Index
        return;
      }
      cout<< "Click next point"<< endl;
    }
  }
  return;
}




//_______________________________________________
Double_t*** ReadData(const Char_t* datapath, Int_t& SSDNum, Int_t& CHNum, Int_t& ParNum)
{
///////////////////////////////////////////////////////
//     构建一个函数，返回类型是 三维数组(三级指针)          //
//     注意多级指针的使用！！！                          //
///////////////////////////////////////////////////////
   Double_t*** readpar = NULL;
   readpar = new Double_t** [SSDNum];
   for(Int_t i=0; i<SSDNum; i++)
   {
     readpar[i] = new Double_t* [CHNum];
     for(Int_t j=0; j<CHNum; j++)
     {
       readpar[i][j]= new Double_t[ParNum];
     }
   }
   ifstream in;
   in.open(datapath);
   if(!in.is_open())
   {
     printf("Error: file %s not found\n",datapath);
     return NULL;
   }
   while(in.good())
   {
     // 按行读取数据
     std::string LineRead;
     std::getline(in, LineRead);
     LineRead.assign(LineRead.substr(0, LineRead.find('*')));
     if(LineRead.empty()) continue;
     if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
     std::istringstream LineStream(LineRead);

     Int_t ssdnum;
     Int_t chnum;

     LineStream >> ssdnum >> chnum;
     for(Int_t i=0; i<ParNum; i++)
     {
       LineStream >>readpar[ssdnum][chnum][i];
     }
   }
   in.close();
   return readpar;
}

//______________________________________________________________________________
void DeleteData(Double_t*** p, Int_t& SSDNum, Int_t& CHNum, Int_t& ParNum)
{
/////////////////////////////////////////////////////////////
//    释放前面用 new方法给 Double_t*** p 动态分配的内存         //
//    多级指针需要逐层释放内存！！！                            //
/////////////////////////////////////////////////////////////
  for(Int_t i=0; i<SSDNum; i++)
  {
    for(Int_t j=0; j<CHNum; j++)
    {
      delete [] p[i][j];
    }
  }
  for(Int_t i=0; i<SSDNum; i++)
  {
    delete [] p[i];
  }
  delete [] p;
}
