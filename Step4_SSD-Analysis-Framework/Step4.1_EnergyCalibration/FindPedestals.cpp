////////////////////////////////////////////////////////////////////////////////////
//    THIS MACRO IS USED TO FIND THE PEDESATLS FOR ENERGY CHANNEL OF SSD          //
//    This macro enable us to set the fit range of each histogram by hand         //
//    Then the histogram is fitted using the user-defined function                //
//    The canva is saved as pdf                                                   //
//                                                                                //
//    Date: 2019-11-06                                                            //
//    Author: gfh                                                                 //
////////////////////////////////////////////////////////////////////////////////////


#include "TH1.h"
#include "TF1.h"
#include "TCanvas.h"

int Index = 0;
int NPoints;
TMarker *m[2];
TLine   *l[2];

void FindPedestals()
{
////////////////////////////////////////////////////////////////////////////
//
//
//
//
////////////////////////////////////////////////////////////////////////////
    
   //   在此修改输入文件路径、文件名称
   std::string path_to_file("data/QCMapSSD_PulserCali_Pedestal0000.root");

   std::string FileOutTag1("Pedestals");  // here to change "Pedestals" or "Pulser" or "Alpha"
   std::string FileOutTag2("L1S_E");      // here to change "L1S_E" or "L2F_E" or "L2B_E" or "L3A_E"

   TFile * FileIn = new TFile(path_to_file.c_str());
   if(!FileIn->IsOpen())
   {
     cout<<"Open file "<< path_to_file.c_str() << " failed"<<endl;
     return;
   }

   //   在此修改输出文件路径
   ofstream FileOut(Form("output/SSD_%s_%s.dat", FileOutTag1.c_str(), FileOutTag2.c_str()));
   FileOut << "*SSDNum" << setw(7) << "CHNum" << setw(12) << "mean"<< setw(12)<< "sigma"<< setw(17)
           <<"mean+5sigma"<< setw(12)<<"Chi2" << setw(12) <<"Xmin"<< setw(12)<<"Xmax" << endl;

   //    定义、读取输入文件中的 Histograms
   TH1D * PedestalHist[4][16];
   for(int i=0; i<4; i++)
   {
     for(int j=0; j<16; j++)
     {
       PedestalHist[i][j]=(TH1D*)FileIn->Get(Form("SSD%d_%s_CH%02d",(i+1),FileOutTag2.c_str(),j));
     }
   }
   printf("Histograms loaded\n");

   //    定义 Canvas
   TCanvas *c1 = new TCanvas("c1","c1",800,600);
   TCanvas *c_begin = new TCanvas("c_begin","");
   c_begin->Close();
   TCanvas *c_end   = new TCanvas("c_end","");
   c_end->Close();


   //========================================================
   //                  behgen analysis here
   //========================================================
   // the pdf file begins here !!!
   c_begin->Print(Form("figures/SSD_%s_%s.pdf[", FileOutTag1.c_str(), FileOutTag2.c_str()));

   float limit[2] = {0};   //定义limit[2]用于存储拟合范围
   for(int i=0; i<4; i++)
   {
     for(int j=0; j<16; j++)
     {
       int Option = 2;   // 设置一个标志, 当Option==2时，执行后面的while(Option==2)循环

       c1->cd();
       gPad->Modified(); // Tell the canvas that an object it is displaying have changed
       gPad->Update();   //  Force the canvas to refresh

       // 如果某个Histogram为空,则跳过
       if(PedestalHist[i][j]==0)
       {
         printf("No data present for SSD%d_%s_CH%02d\n",(i+1),FileOutTag2.c_str(),j);
         continue;
       }
       PedestalHist[i][j]->GetXaxis()->SetRangeUser(50,150);
       PedestalHist[i][j]->GetYaxis()->SetRangeUser(0,7000);
       PedestalHist[i][j]->Draw();

       //====================================
       //  Option用于提供选项：
       //  (1) Option 初始化为 2
       //  (2) Option = 2 : 调用 SetPoints(), 手动选点
       //  (3) 提示输入Option：
       //      <1> 输入 1, 进行拟合并保存;
       //      <2> 输入 2, 在此执行while(Option==2)
       //      <3> 输入 3，中断跳出

       while(Option==2)
       {
         Index = 0;
         NPoints = 0;

         //   it is very important!!! very convenient!!!
         //   here to set the range and fit on the GUI by hand
         c1->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",0,0,"SetPoints(Int_t,Int_t,Int_t,TObject*)");
         while(Index!=1)
         {
           usleep(100);
           gClient->HandleInput();
         }
         c1->Disconnect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)");
          
         if(Index==1)
         {
           for(int ip=0; ip<NPoints; ip++)
           {
             limit[ip] = m[ip]->GetX();
           }
         }
         //  对选取的范围进行拟合
         PedestalHist[i][j]->Fit("gaus","","same",limit[0],limit[1]);
  
         //  提示输入操作选项
         cout<< "Options? 1.Save and next, 2.Retry the fit, 3.Exit"<< endl;
         cin >> Option;
         //  Option ==1，保存拟合结果
         if(Option==1)
         {
           TF1 *fit = PedestalHist[i][j]->GetFunction("gaus");
           double Mean  = fit->GetParameter(1);      // Par[1] = Mean
           double Sigma = fit->GetParameter(2);      // Par[2] = sigma
           double Chi2  = fit->GetChisquare();
           FileOut<<i+1<< setw(10) <<j<< setw(15)<<Mean<<setw(12)<<Sigma<< setw(17) <<Mean+5*Sigma
                  <<setw(12)<<Chi2<<setw(12)<<limit[0]<<setw(12)<<limit[1]<<endl;

           c1->Print(Form("figures/SSD_%s_%s.pdf", FileOutTag1.c_str(), FileOutTag2.c_str()));
         }
         // Option == 3, 中断跳出程序
         if(Option==3)
         {
           c_end->Print(Form("figures/SSD_%s_%s.pdf]", FileOutTag1.c_str(), FileOutTag2.c_str()));
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
   c_end->Print(Form("figures/SSD_%s_%s.pdf]", FileOutTag1.c_str(), FileOutTag2.c_str()));
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
/////////////////////////////////////////////////////////////////////////////
//    This function is to set the fit range by hand on GUI
//    Click the Central button of the mouse to get the range
//
//
//
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
      for(int i=0; i<NPoints; i++)
      {
        l[i] -> Draw();
      }
      gPad->Modified();    // Tell the canvas that an object it is displaying have changed
      gPad->Update();      //  Force the canvas to refresh
      printf("x=%f\n", px);
      NPoints++;

      //  I get only 2 points and then I return
      if(NPoints==2)
      {
        Index = 1;   // Index
        return;
      }
      cout<< "Click next point"<< endl;
    } // ==== close  if(px>=gPad->GetUxmin() && px<=gPad->GetUxmax())
  }   // ==== close if(event==2)
  return;
}
