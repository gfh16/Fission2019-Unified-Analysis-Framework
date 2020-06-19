////////////////////////////////////////////////////////////////////////////////
//
//    相比于自动寻峰, 手动拟合的主要优点是： 可以得到拟合的所有参数！
//
//    gfh, 2019-11-11
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "TH1.h"
#include "TF1.h"
#include "TCanvas.h"

using namespace std;

int Index = 0;
int NPoints;
TMarker *m[2];
TLine   *l[2];

//___________________________________________________________________________
void Pedestals_ClickToFindPedestals()
{
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   1. 读取Histograms                                                      //
//   2. 通过c1->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",0,0,   //
//                     "SetPoints(Int_t,Int_t,Int_t,TObject*)")调用GUI      //
//   3. 定义一个正数Option，作为操作选项：                                      //
//     (1) Option 初始化为 2                                                 //
//     (2) Option = 2: 调用 SetPoints(), 手动选点                            //
//     (3) 提示输入Option：                                                  //
//         <1> 输入 1, 进行拟合并保存;                                        //
//         <2> 输入 2, 在此执行while(Option==2)                              //
//         <3> 输入 3，中断跳出                                              //
/////////////////////////////////////////////////////////////////////////////

   std::string LayerWithLabelTag("L2B"); // "L1S"  "L2F"  "L2B"
   std::string LayerTag("L2");           // "L1"   "L2"
   std::string CaliFileTag("AlphaCali33_48");       //  "AlphaCali05_08"  or "Pedestal0000"
                                                    //  L2: 00_48, 00_32, 33_48;  L1: 00_08, 00_04, 05_08
   std::string pathRootFolder("/home/sea/Fission2019_Data/QualityCheck/");

   std::string FileOutTag;
   std::string path_to_file;
   if (strcmp(CaliFileTag.c_str(),"Pedestal0000")==0) {
    FileOutTag   = "PulserCaliPedestals";
    path_to_file = Form("%sQC_MapSSD_PulserCali_%s.root", pathRootFolder.c_str(),CaliFileTag.c_str());

   } else {
     FileOutTag = "AlphaCaliPedestals";
     path_to_file = Form("%sQC_MapSSD_%s_%s.root", pathRootFolder.c_str(),LayerTag.c_str(), CaliFileTag.c_str());
   }
   std::string pathPedestalsOutput(Form("output/SSD_%s_%s_%s.dat", LayerWithLabelTag.c_str(),
                                        FileOutTag.c_str(), CaliFileTag.c_str()));

   std::string pathPDFOutput(Form("figures/SSD_%s_%s_%s.pdf", LayerWithLabelTag.c_str(),
                                  FileOutTag.c_str(), CaliFileTag.c_str()));
   std::string pathPDFbegin(Form("figures/SSD_%s_%s_%s.pdf[", LayerWithLabelTag.c_str(),
                                 FileOutTag.c_str(), CaliFileTag.c_str()));
   std::string pathPDFend(Form("figures/SSD_%s_%s_%s.pdf]", LayerWithLabelTag.c_str(),
                               FileOutTag.c_str(), CaliFileTag.c_str()));

   TFile * FileIn = new TFile(path_to_file.c_str());
   if(!FileIn->IsOpen())
   {
     cout<<"Open file "<< path_to_file.c_str() << " failed"<<endl;
     return;
   }

   ofstream FileOut(pathPedestalsOutput.c_str());
   FileOut << "*SSDNum" << setw(7) << "CHNum" << setw(8) << "mean"<< setw(10)<< "sigma"<< setw(14)
           <<"mean+5sigma"<< setw(10)<<"Chi2" << setw(10) <<"Xmin"<< setw(10)<<"Xmax" << endl;

   TH1D * PedestalHist[4][16];
   for(Int_t i=0; i<4; i++)
   {
     for(Int_t j=0; j<16; j++)
     {
       PedestalHist[i][j]=(TH1D*)FileIn->Get(Form("SSD%d_%s_E_CH%02d",(i+1),LayerWithLabelTag.c_str(),j));
     }
   }
   printf("Histograms loaded\n");

   TCanvas *c1 = new TCanvas("c1","c1",800,600);
   TCanvas *c_begin = new TCanvas("c_begin","");
   c_begin->Close();
   TCanvas *c_end   = new TCanvas("c_end","");
   c_end->Close();

   //====================================================================================
   //                            BEGIN ANALYZE HERE !!!
   //====================================================================================
   // the pdf file begins here !!!
   c_begin->Print(pathPDFbegin.c_str());

   float limit[2] = {0};   //定义limit[2]用于存储拟合范围
   for(Int_t i=0; i<4; i++)
   {
     for(Int_t j=0; j<16; j++)
     {
       int Option = 2;   // 设置一个标志, 当Option==2时，执行后面的while(Option==2)循环

       if (PedestalHist[i][j]==0)
       {
         printf("No data present for SSD%d_%s_CH%02d\n",(i+1),LayerWithLabelTag.c_str(),j);
         continue;
       }
       while(Option==2)
       {
         Index = 0;
         NPoints = 0;

         c1->cd();
         gPad->Modified();
         gPad->Update();
         PedestalHist[i][j]->GetXaxis()->SetRangeUser(50,160);
         PedestalHist[i][j]->GetYaxis()->SetRangeUser(0,(PedestalHist[i][j]->GetMaximum())*1.1);
         PedestalHist[i][j]->Draw();
         //____________________________________________________
         //   it is very important!!! very convenient!!!
         //   here to set the range and fit on the GUI by hand
         c1->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",0,0,"SetPoints(Int_t,Int_t,Int_t,TObject*)");
         while(Index!=1)
         {
           usleep(100);
           gClient->HandleInput();
         }
         c1->Disconnect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)");

         if (Index==1)
         {
           for(int ip=0; ip<NPoints; ip++)
           {
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
         if (Option==1)
         {
           TF1 *fit = PedestalHist[i][j]->GetFunction("gaus");
           double Mean  = fit->GetParameter(1);      // Par[1] = Mean
           double Sigma = fit->GetParameter(2);      // Par[2] = sigma
           double Chi2  = fit->GetChisquare();

           cout.setf(ios::fixed);
           FileOut<<setw(5)<<std::right<<i<<setw(5)<<std::right<<j<<setw(15)
                  <<std::right<<setprecision(6)<<Mean<<setw(10)<<std::right<<setprecision(6)<<Sigma<<setw(10)
                  <<std::right<<setprecision(6)<<Mean+5*Sigma<<setw(10)<<std::right<<setprecision(6)<<Chi2<<setw(10)
                  <<std::right<<setprecision(6)<<limit[0]<<setw(10)<<std::right<<setprecision(6)<<limit[1]<<endl;

           c1->Print(pathPDFOutput.c_str());
         }
         // Option == 3, 中断跳出程序
         if (Option==3)
         {
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
} // end void FindPedestals()

//______________________________________________________________________________
void SetPoints(Int_t event, Int_t x, Int_t y, TObject *selected)
{
//////////////////////////////////////////////////////////////////////////////
//    This function is to set the fit range by hand on GUITObject *selected //
//    Click the Central button of the mouse to get the range                //
//                                                                          //
//    Function parameters:                                                  //
//       event :                                                            //
//           x :   x value of the point                                     //
//           y :   y value of the point                                     //
//    selected :   a pointer to the curent point                            //
//////////////////////////////////////////////////////////////////////////////
  if(event == 2)
  {
    float px = gPad->AbsPixeltoX(x); // Conversion of absolute pixel to X
    float py = gPad->AbsPixeltoX(y); // CoYversion of absolute pixel to Y
    py = gPad->PadtoY(py);           // Convert y from pad to Y
    float Uymin = gPad->GetUymin();  // Returns the minimum/maximum y-coordinate
    float Uymax = gPad->GetUymax();  // value visible on the pad

    //  save the clicks as a marker
    if(px>=gPad->GetUxmin() && px<=gPad->GetUxmax())
    {
      m[NPoints] = new TMarker(px,py,3);          // marker style = 3, means “*”
      l[NPoints] = new TLine(px,Uymin,px,Uymax);
      l[NPoints] -> SetLineColor(2);
      l[NPoints] -> SetLineWidth(2);
      l[NPoints] -> Draw();
      for(Int_t i=0; i<NPoints; i++)
      {
        l[i] -> Draw();
      }
      gPad->Modified();
      gPad->Update();
      printf("x=%f\n", px);
      NPoints++;

      // !!! I get only 2 points and then I return
      if (NPoints==2)
      {
        Index = 1;   // Index
        return;
      }
      cout<< "Click next point"<< endl;
    }
  }
  return;
}
