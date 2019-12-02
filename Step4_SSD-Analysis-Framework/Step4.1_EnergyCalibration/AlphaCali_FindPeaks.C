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

Int_t Index = 0;
Int_t NPoints;
TMarker* m[6];
TLine* l[6];

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
   std::string InputTag("00_08");
   std::string FileOutTag1("Alpha");
   std::string FileOutTag2("L1S_E");  // "L1S_E" or "L2F_E" or "L2B_E"
   std::string path_to_file(Form("data/MapSSD_L1_AlphaCali%s.root", InputTag.c_str()));
   std::string FileOutdatapath(Form("output/SSD_%s_%s_%s.dat", FileOutTag1.c_str(), FileOutTag2.c_str(), InputTag.c_str()));
   std::string FileOutfitresult(Form("output/Fitpara_SSD_%s_%s_%s.dat", FileOutTag1.c_str(), FileOutTag2.c_str(), InputTag.c_str()));
   std::string FileOutpdfpath(Form("figures/SSD_%s_%s_%s.pdf", FileOutTag1.c_str(), FileOutTag2.c_str(), InputTag.c_str()));
   std::string pdfbegin(Form("figures/SSD_%s_%s_%s.pdf[", FileOutTag1.c_str(), FileOutTag2.c_str(), InputTag.c_str()));
   std::string pdfend(Form("figures/SSD_%s_%s_%s.pdf]", FileOutTag1.c_str(), FileOutTag2.c_str(), InputTag.c_str()));

   TFile* FileIn = new TFile(path_to_file.c_str());
   if (!FileIn->IsOpen()) {
     cout<<"Open file "<< path_to_file.c_str() << " failed"<<endl;
     return;
   }

   //   在此修改输出文件路径
   ofstream FileOut(FileOutdatapath.c_str());
   FileOut <<"*SSDNum" <<setw(7)<<"CHNum"<<setw(8)
           <<"mean1" << setw(10)<<"mean2" <<setw(10)<<"mean3" <<setw(10)
           <<"sigma1"<< setw(10)<<"sigma2"<<setw(10)<<"sigma3"<<setw(10)
           <<"Chi21" << setw(10)<<"Chi22" <<setw(10)<<"Chi23" <<setw(10)
           <<"Xmin1"<< setw(10)<<"Xmax1"<<setw(10)<<"Xmin2"<< setw(10)<<"Xmax2"<<setw(10)<<"Xmin3"<<setw(10)<<"Xmax3"<<endl;

   ofstream FileOutFit(FileOutfitresult.c_str());
   FileOutFit<<" Fit function = par[0]+par[1]*x && y = a*x + b (y=Energy, x=Ch);  so I define a = par[1], b = par[0]\n";
   FileOutFit<<" *SSDNum"<<setw(7)<<"CHNum"<<setw(10)<<"par_a"<<setw(15)<<"Errpar_a"<<setw(15)<<"par_b"<<setw(17)<<"Errpar_b\n";

   //    定义、读取输入文件中的 Histograms
   TH1D* PedestalHist[4][16];
   for (Int_t SSDNum=0; SSDNum<4; SSDNum++) {
     for (Int_t CHNum=0; CHNum<16; CHNum++) {
       PedestalHist[SSDNum][CHNum]=(TH1D*)FileIn->Get(Form("SSD%d_%s_CH%02d",(SSDNum+1),FileOutTag2.c_str(),CHNum));
     }
   }
   printf("Histograms loaded\n");

   TCanvas* c1 = new TCanvas("c1","c1",1000,1200);
   c1->Divide(1,2);
   TCanvas* c_begin = new TCanvas("c_begin","");
   TCanvas* c_end   = new TCanvas("c_end","");
   //====================================================================================
   //                            BEGIN ANALYZE HERE !!!
   c_begin->Print(pdfbegin.c_str());

   Double_t limit[6] = {0};   //定义limit[2]用于存储拟合范围
   Double_t par[9] = {40,250,15,20,260,15,10,270,15};
   for (Int_t SSDNum=0; SSDNum<4; SSDNum++) {
     for (Int_t CHNum=0; CHNum<16; CHNum++) {

       Int_t Option = 2;   // 设置一个标志, 当Option==2时，执行后面的while(Option==2)循环

       if (PedestalHist[SSDNum][CHNum]==0) {
         printf("No data present for SSD%d_%s_CH%02d\n",(SSDNum+1),FileOutTag2.c_str(),CHNum);
         continue;
       }
       while (Option==2) {
         Index = 0;
         NPoints = 0;

         c1->cd(1);
         gPad->Modified();
         gPad->Update();
         PedestalHist[SSDNum][CHNum]->GetXaxis()->SetRangeUser(150,400);
         PedestalHist[SSDNum][CHNum]->GetYaxis()->SetRangeUser(0,(PedestalHist[SSDNum][CHNum]->GetMaximum())*1.1);
         PedestalHist[SSDNum][CHNum]->Draw();
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
         PedestalHist[SSDNum][CHNum]->Fit(FitPeak1,"R");
         PedestalHist[SSDNum][CHNum]->Fit(FitPeak2,"R+");
         PedestalHist[SSDNum][CHNum]->Fit(FitPeak3,"R+");

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
         grap->SetTitle(Form("AlphaFits_SSD%d_%s_E_CH%02d",SSDNum+1,FileOutTag2.c_str(),CHNum));
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

           c1->Print(FileOutpdfpath.c_str());
         }
         if (Option==3) {
           c_end->Print(pdfend.c_str());
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
   c_end->Print(pdfend.c_str());
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
