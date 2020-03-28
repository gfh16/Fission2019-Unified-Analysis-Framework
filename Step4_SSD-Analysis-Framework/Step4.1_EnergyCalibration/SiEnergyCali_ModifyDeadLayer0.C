////////////////////////////////////////////////////////////////////////////////
//     THIS MACRO IS USED TO CALCULATE THE SLOPE OF THE ENERGY LINEARITY      //
//                                                                            //
//   The function of pulser linearity is defined as:                          //
//        y = a * ch + b                                                      //
//   The function of energy linearity is defined as:                          //
//        E(MeV) = a * ch + c, so c = E(MeV)-a * ch                           //
//                                                                            //
// ***************************************************************************//                                                                           //
//   关于 C++ vector 作为参数的使用方式：                                         //
//   https://www.cnblogs.com/xiaoxi666/p/6843211.html                         //
//                                                                            //
//   C语言动态申请三维数组的三种方法                                               //
//   https://blog.csdn.net/qq_29545231/article/details/78662983               //
//   https://www.cnblogs.com/gaozihan/p/10759636.html                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_________________________________________
#include "iostream"
#include "string.h"
#include "fstream"
#include "vector"

using namespace std;

Int_t SSDNum = 4;
Int_t CHNum  = 16;
Int_t PeakNum = 3;
Int_t PeakIndex = 10;


//______________________________________________________________________________
Double_t*** ReadData(const Char_t* datapath, Int_t& SSDNum, Int_t& CHNum, Int_t& ParNum);

//______________________________________________________________________________
void DeleteData(Double_t*** p, Int_t& SSDNum, Int_t& CHNum, Int_t& ParNum);

//___________________________________________________________
void SiEnergyCali_ModifyDeadLayer()
{
   std::string LayerTag("L2B");
   std::string FileTag("Switch");  // Switch or Height
   std::string AlphaCaliTag("AlphaCali33_48");
   std::string DeadLayerTag("ChangingDeadLayer");

   std::string L1STag("L1S");
   std::string L2FTag("L2F");
   std::string L2BTag("L2B");

   std::string pathPuserIn(Form("output/SSD_%s_PulserCali_%s.dat",LayerTag.c_str(),FileTag.c_str())); // Pulser fitting parameters
   std::string pathAlphaChIn(Form("output/SSD_%s_AlphaPeaks_%s.dat",LayerTag.c_str(), AlphaCaliTag.c_str())); // Channel of the alpha peaks
   std::string pathAlphaE1In(Form("output/SSD_AlphaE1_%s.dat",DeadLayerTag.c_str()));   // Alpha enegies in MeV
   std::string pathAlphaE2In(Form("output/SSD_AlphaE2_%s.dat",DeadLayerTag.c_str()));   // Alpha enegies in MeV
   std::string pathAlphaE3In(Form("output/SSD_AlphaE3_%s.dat",DeadLayerTag.c_str()));   // Alpha enegies in MeV

   std::string pathSiEnergyCaliChangingDeadLayerPDF(Form("figures/SSD_%s_%s_%s_%s.pdf",
                                                    LayerTag.c_str(),DeadLayerTag.c_str(),FileTag.c_str(),AlphaCaliTag.c_str()));
   std::string pathSiEnergyCaliChangingDeadLayerPDFbegin(Form("figures/SSD_%s_%s_%s_%s.pdf[",
                                                    LayerTag.c_str(),DeadLayerTag.c_str(),FileTag.c_str(),AlphaCaliTag.c_str()));
   std::string pathSiEnergyCaliChangingDeadLayerPDFend(Form("figures/SSD_%s_%s_%s_%s.pdf]",
                                                    LayerTag.c_str(),DeadLayerTag.c_str(),FileTag.c_str(),AlphaCaliTag.c_str()));

   Int_t numpar_PulserIn  = 4;  // 4 pars: a, err_a, b, err_b        (E = a*ch + b)
   Int_t numpar_AlphaChIn = 3;  // 3 alpha channels: CH1, CH2, CH3
   Int_t numpar_AlphaE1In = 10;  //
   Int_t numpar_AlphaE2In = 10;  //
   Int_t numpar_AlphaE3In = 10;  //

   Double_t*** PulserIn  = ReadData(pathPuserIn.c_str(),  SSDNum, CHNum, numpar_PulserIn);
   Double_t*** AlphaChIn = ReadData(pathAlphaChIn.c_str(),SSDNum, CHNum, numpar_AlphaChIn);
   Double_t*** AlphaE1In = ReadData(pathAlphaE1In.c_str(), SSDNum, CHNum, numpar_AlphaE1In);
   Double_t*** AlphaE2In = ReadData(pathAlphaE2In.c_str(), SSDNum, CHNum, numpar_AlphaE2In);
   Double_t*** AlphaE3In = ReadData(pathAlphaE3In.c_str(), SSDNum, CHNum, numpar_AlphaE3In);

   Double_t k1[SSDNum][CHNum][PeakIndex];
   Double_t k2[SSDNum][CHNum][PeakIndex];
   Double_t k3[SSDNum][CHNum][PeakIndex];
   Double_t h1[SSDNum][CHNum][PeakIndex];
   Double_t h2[SSDNum][CHNum][PeakIndex];
   Double_t h3[SSDNum][CHNum][PeakIndex];
   Double_t k11[SSDNum][CHNum][PeakIndex];
   Double_t k22[SSDNum][CHNum][PeakIndex];
   Double_t k33[SSDNum][CHNum][PeakIndex];
   Double_t h11[SSDNum][CHNum][PeakIndex];
   Double_t h22[SSDNum][CHNum][PeakIndex];
   Double_t h33[SSDNum][CHNum][PeakIndex];
   Double_t peak_CH[SSDNum][CHNum][PeakIndex][PeakNum];
   Double_t peak_MeV[SSDNum][CHNum][PeakIndex][PeakNum];

   for (Int_t i=0; i<SSDNum; i++)
   {
     for (Int_t j=0; j<CHNum; j++)
     {
       for (Int_t index=0; index<PeakIndex; index++)
       {
         Double_t a = PulserIn[i][j][0];
         Double_t b = PulserIn[i][j][2];
         Double_t E1_CH  = AlphaChIn[i][j][0];
         Double_t E2_CH  = AlphaChIn[i][j][1];
         Double_t E3_CH  = AlphaChIn[i][j][2];
         Double_t E1_MeV = AlphaE1In[i][j][index];
         Double_t E2_MeV = AlphaE2In[i][j][index];
         Double_t E3_MeV = AlphaE3In[i][j][index];

         Double_t c1 = E1_MeV /(a * E1_CH + b);   // peak1 比例系数
         Double_t c2 = E2_MeV /(a * E2_CH + b);   // peak1 比例系数
         Double_t c3 = E3_MeV /(a * E3_CH + b);   // peak1 比例系数

         k1[i][j][index] = c1 * a;  // E1_MeV = k1 * E1_CH + h1
         k2[i][j][index] = c2 * a;  // E2_MeV = k2 * E2_CH + h2
         k3[i][j][index] = c3 * a;  // E3_MeV = k3 * E3_CH + h3

         h1[i][j][index] = c1 * b;
         h2[i][j][index] = c2 * b;
         h3[i][j][index] = c3 * b;

         k11[i][j][index] = 1./k1[i][j][index];  // E1_CH = k11 * E1_MeV + h11
         k22[i][j][index] = 1./k2[i][j][index];  // E2_CH = k22 * E2_MeV + h22
         k33[i][j][index] = 1./k3[i][j][index];  // E3_CH = k33 * E3_MeV + h33

         h11[i][j][index] = -h1[i][j][index]/k1[i][j][index];
         h22[i][j][index] = -h2[i][j][index]/k2[i][j][index];
         h33[i][j][index] = -h3[i][j][index]/k3[i][j][index];

         peak_CH[i][j][index][0] = E1_CH;
         peak_CH[i][j][index][1] = E2_CH;
         peak_CH[i][j][index][2] = E3_CH;
         peak_MeV[i][j][index][0] = E1_MeV;
         peak_MeV[i][j][index][1] = E2_MeV;
         peak_MeV[i][j][index][2] = E3_MeV;
       }
     }
   }

   // 3 alpha fit
   TGraph* graph[SSDNum][CHNum][PeakIndex];
   TF1* fitfunc[SSDNum][CHNum][PeakIndex];
   TH2D* HistforZoomedGraph[SSDNum][CHNum][PeakIndex];
   for (Int_t i=0; i<SSDNum; i++)
   {
     for (Int_t j=0; j<CHNum; j++)
     {
       for (Int_t index=0; index<PeakIndex; index++)
       {
         graph[i][j][index] = new TGraph(PeakNum,peak_CH[i][j][index],peak_MeV[i][j][index]);
         graph[i][j][index]->SetTitle(Form("3AlphaFit_SSD%d_%s_%s_%s_CH%02d_Index%d",i+1,LayerTag.c_str(),
                                           FileTag.c_str(),AlphaCaliTag.c_str(),j,index));
         graph[i][j][index]->SetMarkerStyle(20);
         graph[i][j][index]->SetMarkerColor(kBlue);
         graph[i][j][index]->Draw("AP*");

         fitfunc[i][j][index] = new TF1("fitfunc","pol1",0,4096);
         graph[i][j][index]->Fit("fitfunc");
       }
     }
   }

   // draw together
   TCanvas* cans_draw = new TCanvas("c1","cans_draw",1500,800);
   cans_draw->Divide(5,2);
   TCanvas* cans_drawbegin = new TCanvas();
   TCanvas* cans_drawend = new TCanvas();
   cans_drawbegin->Close();
   cans_drawend->Close();
   cans_drawbegin->Print(pathSiEnergyCaliChangingDeadLayerPDFbegin.c_str());
   for (Int_t i=0; i<SSDNum; i++)
   {
     for (Int_t j=0; j<CHNum; j++)
     {
       for (Int_t index=0; index<PeakIndex; index++)
       {
         cans_draw->cd(index+1);
         Double_t a_3AlphaFit = fitfunc[i][j][index]->GetParameter(1); // pol1: E_MeV= a[0]+a[1]*E_CH
         Double_t b_3AlphaFit = fitfunc[i][j][index]->GetParameter(0);
         TF1* fPeak1Fit = new TF1("fPeak1Fit", Form("%.4f*x+%.4f",k1[i][j][index],h1[i][j][index]),0,4096);
         TF1* fPeak2Fit = new TF1("fPeak2Fit", Form("%.4f*x+%.4f",k2[i][j][index],h2[i][j][index]),0,4096);
         TF1* fPeak3Fit = new TF1("fPeak3Fit", Form("%.4f*x+%.4f",k3[i][j][index],h3[i][j][index]),0,4096);
         TF1* f3AlphaFit = new TF1("f3AlphaFit", Form("%.4f*x+%.4f",a_3AlphaFit,b_3AlphaFit),0,4096);

         fPeak1Fit->SetTitle(Form("SSD%d_%s_CH%02d_Index%d",i+1,LayerTag.c_str(),j,index));

         fPeak1Fit->GetXaxis()->SetRangeUser(0,4096);
         fPeak1Fit->GetXaxis()->SetNdivisions(505,"true");
         fPeak1Fit->GetYaxis()->SetNdivisions(510,"true");
         fPeak1Fit->GetXaxis()->SetTitle("CH");
         fPeak1Fit->GetYaxis()->SetTitle("E/MeV");
         fPeak1Fit->SetLineColor(kRed);
         fPeak2Fit->SetLineColor(kGreen);
         fPeak3Fit->SetLineColor(kBlue);
         f3AlphaFit->SetLineColor(kBlack);

         fPeak1Fit->Draw();
         fPeak2Fit->Draw("SAME");
         fPeak3Fit->Draw("SAME");
         f3AlphaFit->Draw("SAME");

         gPad->Modified();
         gPad->Update();
       }
       cans_draw->Print(pathSiEnergyCaliChangingDeadLayerPDF.c_str());
     }
   }
   cans_drawend->Print(pathSiEnergyCaliChangingDeadLayerPDFend.c_str());

   DeleteData(PulserIn,SSDNum, CHNum, numpar_PulserIn);
   DeleteData(AlphaChIn,SSDNum,CHNum, numpar_AlphaChIn);
   DeleteData(AlphaE1In,SSDNum, CHNum, numpar_AlphaE1In);
   DeleteData(AlphaE2In,SSDNum, CHNum, numpar_AlphaE2In);
   DeleteData(AlphaE3In,SSDNum,CHNum, numpar_AlphaE3In);

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
