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

//______________________________________________________________________________
Double_t*** ReadData(const Char_t* datapath, Int_t& SSDNum, Int_t& CHNum, Int_t& ParNum);

//______________________________________________________________________________
void DeleteData(Double_t*** p, Int_t& SSDNum, Int_t& CHNum, Int_t& ParNum);

//___________________________________________________________
void SiEnergyCali()
{
   std::string LayerTag("L2F");
   std::string FileTag("Switch");
   std::string AlphaCaliTag("AlphaCali00_32");

   std::string pathPuserIn(Form("output/SSD_%s_PulserCali_%s.dat",LayerTag.c_str(),FileTag.c_str())); // Pulser fitting parameters
   std::string pathAlphaEIn("output/SSD_AlphaEnergies.dat");   // Alpha enegies in MeV
   std::string pathAlphaChIn(Form("output/SSD_%s_AlphaPeaks_%s.dat",LayerTag.c_str(), AlphaCaliTag.c_str())); // Channel of the alpha peaks
   std::string pathPulserPedestalIn(Form("output/SSD_%s_PulserPedestals.dat",LayerTag.c_str()));
   std::string pathAlphaCaliPedestalIn(Form("output/SSD_%s_AlphaCaliPedestals_%s.dat",LayerTag.c_str(),AlphaCaliTag.c_str()));
   std::string path3AlphaFitParIn(Form("output/SSD_%s_3AlphaFitPars_%s.dat",LayerTag.c_str(), AlphaCaliTag.c_str()));

   std::string pathSiEnergyCaliFitParOut(Form("output/SSD_%s_SiEnergyCaliPars_%s_%s.dat",LayerTag.c_str(), FileTag.c_str(), AlphaCaliTag.c_str()));

   std::string pathSiEnergyCaliFitPDF(Form("figures/SSD_%s_EnergyCali_%s_%s.pdf",LayerTag.c_str(), FileTag.c_str(), AlphaCaliTag.c_str()));
   std::string pathSiEnergyCaliFitPDFbegin(Form("figures/SSD_%s_EnergyCali_%s_%s.pdf[",LayerTag.c_str(), FileTag.c_str(), AlphaCaliTag.c_str()));
   std::string pathSiEnergyCaliFitPDFend(Form("figures/SSD_%s_EnergyCali_%s_%s.pdf]", LayerTag.c_str(), FileTag.c_str(), AlphaCaliTag.c_str()));
   std::string pathPedestalOffsetPDF(Form("figures/SSD_%s_PedestalOffset_%s_%s.pdf",LayerTag.c_str(), FileTag.c_str(), AlphaCaliTag.c_str()));
   std::string pathPedestalOffsetPNG(Form("figures/SSD_%s_PedestalOffset_%s_%s.png",LayerTag.c_str(), FileTag.c_str(), AlphaCaliTag.c_str()));

   ofstream EnergyCaliParOut(pathSiEnergyCaliFitParOut.c_str());
   EnergyCaliParOut<<"* Fit function: E_MeV = a * E_CH + b, then E_CH = 1/a * E_MeV - a/b \n";
   EnergyCaliParOut<<"* 1,2,3 represent using alpha peak1,peak2, and peak3 to calibrate the energy,respectively. \n";
   EnergyCaliParOut<<"* SSDNum"<<setw(7)<<"CHNum"<<setw(10)<<"a1"<<setw(10)<<"b1"<<setw(10)<<"1./a1"<<setw(12)
                   <<"-a1/b1"<<setw(10)<<"a2"<<setw(10)<<"b2"<<setw(10)<<"1./a2"<<setw(12)<<"-a2/b2"<<setw(10)
                   <<"a3"<<setw(10)<<"b3"<<setw(10)<<"1./a3"<<setw(12)<<"-a3/b3\n";

   Int_t numpar_PulserIn  = 4;  // 4 pars: a, err_a, b, err_b        (E = a*ch + b)
   Int_t numpar_AlphaEIn  = 3;  // 3 alpha energies: E1, E2, E3
   Int_t numpar_AlphaChIn = 3;  // 3 alpha channels: CH1, CH2, CH3
   Int_t numpar_3AlphaFit = 4;  // 4 pars: a', err_a', b', err_b'    (E' = a'*ch + b')
   Int_t numpar_PulserPedestalIn = 1; // 1 par
   Int_t numpar_AlphaCaliPedestalIn = 1; // 1 par

   Double_t*** PulserIn  = ReadData(pathPuserIn.c_str(),  SSDNum, CHNum, numpar_PulserIn);
   Double_t*** AlphaEIn  = ReadData(pathAlphaEIn.c_str(), SSDNum, CHNum, numpar_AlphaEIn);
   Double_t*** AlphaChIn = ReadData(pathAlphaChIn.c_str(),SSDNum, CHNum, numpar_AlphaChIn);
   Double_t*** AlphaFit  = ReadData(path3AlphaFitParIn.c_str(), SSDNum, CHNum, numpar_3AlphaFit);
   Double_t*** PulserPedestalIn = ReadData(pathPulserPedestalIn.c_str(), SSDNum, CHNum, numpar_PulserPedestalIn);
   Double_t*** AlphaCaliPedestalIn = ReadData(pathAlphaCaliPedestalIn.c_str(), SSDNum, CHNum, numpar_AlphaCaliPedestalIn);

   Int_t Num_PedestalOffset[SSDNum]; // TGrap* grap = new TGrap(Num_PedestalOffset, SSDCHNum, PedestalOffset);
   Double_t PulserPedestalOffset[SSDNum][CHNum];
   Double_t AlphaCaliPedestalOffset[SSDNum][CHNum];
   Double_t SSDCHNum[SSDNum][CHNum];
   for(Int_t i=0; i<SSDNum; i++)
   {
     Num_PedestalOffset[i] = CHNum;
     for(Int_t j=0; j<CHNum; j++)
     {
       SSDCHNum[i][j] = j+1;
       PulserPedestalOffset[i][j] = 0;
       AlphaCaliPedestalOffset[i][j] = 0;
     }
   }

   TCanvas* canvas = new TCanvas("c1","c1",1200,1000);
   canvas->Divide(2,2);

   TCanvas* cans_begin = new TCanvas();
   TCanvas* cans_end = new TCanvas();
   cans_begin->Close();
   cans_end->Close();
   cans_begin->Print(pathSiEnergyCaliFitPDFbegin.c_str());

   for(Int_t i=0; i<SSDNum; i++)
   {
     for(Int_t j=0; j<CHNum; j++)
     {
       // 计算比例系数
       Double_t a = PulserIn[i][j][0];
       Double_t b = PulserIn[i][j][2];
       Double_t E1_CH  = AlphaChIn[i][j][0];
       Double_t E2_CH  = AlphaChIn[i][j][1];
       Double_t E3_CH  = AlphaChIn[i][j][2];
       Double_t E1_MeV = AlphaEIn[i][j][0];
       Double_t E2_MeV = AlphaEIn[i][j][1];
       Double_t E3_MeV = AlphaEIn[i][j][2];
       Double_t PulserPedestal_CH = PulserPedestalIn[i][j][0];
       Double_t AlphaCaliPedestal_CH = AlphaCaliPedestalIn[i][j][0];

      // if(E1_CH<50.0) continue

       Double_t c1 = E1_MeV /(a * E1_CH + b);   // peak1 比例系数
       Double_t c2 = E2_MeV /(a * E2_CH + b);   // peak1 比例系数
       Double_t c3 = E3_MeV /(a * E3_CH + b);   // peak1 比例系数

       Double_t k1 = c1 * a;  // E1_MeV = k1 * E1_CH + h1
       Double_t k2 = c2 * a;  // E2_MeV = k2 * E2_CH + h2
       Double_t k3 = c3 * a;  // E3_MeV = k3 * E3_CH + h3

       Double_t h1 = c1 * b;
       Double_t h2 = c2 * b;
       Double_t h3 = c3 * b;

       Double_t k11 = 1./k1;  // E1_CH = k11 * E1_MeV + h11
       Double_t k22 = 1./k2;  // E2_CH = k22 * E2_MeV + h22
       Double_t k33 = 1./k3;  // E3_CH = k33 * E3_MeV + h33

       Double_t h11 = -h1/k1;
       Double_t h22 = -h2/k2;
       Double_t h33 = -h3/k3;

       Double_t a_3AlphaFit = AlphaFit[i][j][0]; // E = a_3AlphaFit*CH + b_3AlphaFit
       Double_t b_3AlphaFit = AlphaFit[i][j][2];

       Double_t a1_3AlphaFit = 1./a_3AlphaFit;
       Double_t b1_3AlphaFit = -b_3AlphaFit/a_3AlphaFit;

       PulserPedestalOffset[i][j] = h11 - PulserPedestal_CH; // Estimate the offset of pedestal
       AlphaCaliPedestalOffset[i][j] = h11 - AlphaCaliPedestal_CH; // Estimate the offset of pedestal

       EnergyCaliParOut<<setw(7)<< i <<setw(5)<< j <<setw(15)<< k1 <<setw(10)<< h1 <<setw(10)<< k11 <<setw(10)
                       << h11 <<setw(12)<< k2 <<setw(10)<< h2 <<setw(10)<< k22 <<setw(10)<< h22 <<setw(12)
                       << k3 <<setw(10)<< h3 <<setw(10)<< k33 <<setw(10)<< h33 << endl;

//       printf("k11=%.4f, k22=%.4f, k33=%.4f, a_3Alpha=%.4f, h11=%.4f, h22=%.4f, h33=%.4f, b_3Alpha=%.4f\n",
//               k11,k22,k33,a1_3AlphaFit,h11,h22,h33,b1_3AlphaFit);

       //  画出刻度曲线, 比较3-alpha峰拟合 与 1-alpha峰 + Pulser 两种拟合结果
       TF1* fPeak1Fit = new TF1("fPeak1Fit", Form("%.4f*x+%.4f", k1 ,h1), 0, 4096);
       TF1* fPeak2Fit = new TF1("fPeak2Fit", Form("%.4f*x+%.4f", k2 ,h2), 0, 4096);
       TF1* fPeak3Fit = new TF1("fPeak3Fit", Form("%.4f*x+%.4f", k3 ,h3), 0, 4096);
       TF1* f3AlphaFit = new TF1("f3AlphaFit", Form("%.4f*x+%.4f", a_3AlphaFit, b_3AlphaFit), 0, 4096);

       TF1* fPeak1FitClone1 = (TF1*)fPeak1Fit->Clone();
       TF1* fPeak1FitClone2 = (TF1*)fPeak1Fit->Clone();
       TF1* fPeak2FitClone1 = (TF1*)fPeak2Fit->Clone();
       TF1* fPeak2FitClone2 = (TF1*)fPeak2Fit->Clone();
       TF1* fPeak3FitClone1 = (TF1*)fPeak3Fit->Clone();
       TF1* fPeak3FitClone2 = (TF1*)fPeak3Fit->Clone();
       TF1* f3AlphaFitClone1 = (TF1*)f3AlphaFit->Clone();
       TF1* f3AlphaFitClone2 = (TF1*)f3AlphaFit->Clone();

       TLegend* legend1 = new TLegend(0.1,0.7,0.5,0.9);
       TLegend* legend2 = new TLegend(0.1,0.7,0.5,0.9);
       TLegend* legend3 = new TLegend(0.1,0.6,0.5,0.9);

       canvas->cd(1);
       Double_t y1_max = k1 * 4096 + h1;
       fPeak1Fit->SetTitle(Form("SSD%d_%s_%s_%s_CH%02d",i+1,LayerTag.c_str(),FileTag.c_str(),AlphaCaliTag.c_str(),j));
       fPeak1Fit->GetXaxis()->SetRangeUser(0,4096);
       fPeak1Fit->GetYaxis()->SetRangeUser(0,y1_max);
       fPeak1Fit->GetXaxis()->SetNdivisions(505,"true");
       fPeak1Fit->GetYaxis()->SetNdivisions(510,"true");
       fPeak1Fit->GetXaxis()->SetTitle("CH");
       fPeak1Fit->GetYaxis()->SetTitle("E/MeV");
       fPeak1Fit->SetLineColor(kRed);
       fPeak2Fit->SetLineColor(kGreen);
       fPeak3Fit->SetLineColor(kBlue);
       f3AlphaFit->SetLineColor(kBlack);

       legend1->AddEntry(fPeak1Fit, "Pulser+AlphaPeak1","l");
       legend1->AddEntry(fPeak2Fit, "Pulser+AlphaPeak2","l");
       legend1->AddEntry(fPeak3Fit, "Pulser+AlphaPeak3","l");
       legend1->AddEntry(f3AlphaFit,"3AlphaFit","l");

       fPeak1Fit->Draw();
       fPeak2Fit->Draw("SAME");
       fPeak3Fit->Draw("SAME");
       f3AlphaFit->Draw("SAME");
       legend1->Draw("SAME");
       gPad->Modified();
       gPad->Update();


       canvas->cd(2);
       Double_t x2_max = E3_CH + 10;
       Double_t y2_max = k1 * x2_max + h1;
//       printf("x2_max=%.4f\n", x2_max);
       fPeak1FitClone1->SetTitle(Form("SSD%d_%s_%s_%s_CH%02d",i+1,LayerTag.c_str(),FileTag.c_str(),AlphaCaliTag.c_str(),j));
       fPeak1FitClone1->GetXaxis()->SetRangeUser(0,x2_max);
       fPeak1FitClone1->GetYaxis()->SetRangeUser(0,y2_max);
       fPeak1FitClone1->GetXaxis()->SetNdivisions(510,"true");
       fPeak1FitClone1->GetYaxis()->SetNdivisions(510,"true");
       fPeak1FitClone1->GetXaxis()->SetTitle("CH");
       fPeak1FitClone1->GetYaxis()->SetTitle("E/MeV");
       fPeak1FitClone1->SetLineColor(kRed);
       fPeak2FitClone1->SetLineColor(kGreen);
       fPeak3FitClone1->SetLineColor(kBlue);
       f3AlphaFitClone1->SetLineColor(kBlack);

       TMarker* m0 = new TMarker(PulserPedestal_CH, 0, 20);
       TMarker* m1 = new TMarker(h11, 0, 20);
       TMarker* m2 = new TMarker(b1_3AlphaFit, 0, 20);
       m0->SetMarkerSize(1.5);
       m1->SetMarkerSize(1.5);
       m2->SetMarkerSize(1.5);
       m0->SetMarkerColor(7);
       m1->SetMarkerColor(kRed);
       m2->SetMarkerColor(6);

       legend2->AddEntry(fPeak1FitClone1, "Pulser+AlphaPeak1","l");
       legend2->AddEntry(fPeak2FitClone1, "Pulser+AlphaPeak2","l");
       legend2->AddEntry(fPeak3FitClone1, "Pulser+AlphaPeak3","l");
       legend2->AddEntry(f3AlphaFitClone1,"3AlphaFit","l");
       legend2->AddEntry(m0, "Pedestal","p");

       fPeak1FitClone1->Draw();
       fPeak2FitClone1->Draw("SAME");
       fPeak3FitClone1->Draw("SAME");
       f3AlphaFitClone1->Draw("SAME");
       m0->Draw("SAME");
       m1->Draw("SAME");
       m2->Draw("SAME");
       legend2->Draw("SAME");
       gPad->Modified();
       gPad->Update();


       canvas->cd(3);
       gPad->SetGridy();
       Double_t x3_min = E1_CH - 30.;
       Double_t x3_max = E3_CH;
       Double_t y3_min = k1 * x3_min + h1;
       Double_t y3_max = k1 * x3_max + h1;
  //     printf("x3_min=%.4f  x3_max=%.4f", x3_min, x3_max);
       fPeak1FitClone2->SetTitle(Form("SSD%d_%s_%s_%s_CH%02d",i+1,LayerTag.c_str(),FileTag.c_str(),AlphaCaliTag.c_str(),j));
       fPeak1FitClone2->GetXaxis()->SetRangeUser(x3_min, x3_max);
       fPeak1FitClone2->GetYaxis()->SetRangeUser(4.5, 6);
       fPeak1FitClone2->GetXaxis()->SetNdivisions(510,"true");
       fPeak1FitClone2->GetYaxis()->SetNdivisions(1005,"true");
       fPeak1FitClone2->GetXaxis()->SetTitle("CH");
       fPeak1FitClone2->GetYaxis()->SetTitle("E/MeV");
       fPeak1FitClone2->SetLineColor(kRed);
       fPeak2FitClone2->SetLineColor(kGreen);
       fPeak3FitClone2->SetLineColor(kBlue);
       f3AlphaFitClone2->SetLineColor(kBlack);

       TMarker *point1 = new TMarker(E1_CH, E1_MeV, 20);
       TMarker *point2 = new TMarker(E2_CH, E2_MeV, 20);
       TMarker *point3 = new TMarker(E3_CH, E3_MeV, 20);
       point1->SetMarkerSize(1.);
       point2->SetMarkerSize(1.);
       point3->SetMarkerSize(1.);
       point1->SetMarkerColor(kRed);
       point2->SetMarkerColor(kGreen);
       point3->SetMarkerColor(kBlue);

       legend3->AddEntry(fPeak1FitClone2, "Pulser+AlphaPeak1","l");
       legend3->AddEntry(fPeak2FitClone2, "Pulser+AlphaPeak2","l");
       legend3->AddEntry(fPeak3FitClone2, "Pulser+AlphaPeak3","l");
       legend3->AddEntry(f3AlphaFitClone2,"3AlphaFit","l");
       legend3->AddEntry(point1,"AlphaPeak1","p");
       legend3->AddEntry(point2,"AlphaPeak2","p");
       legend3->AddEntry(point3,"AlphaPeak3","p");

       fPeak1FitClone2->Draw();
       fPeak2FitClone2->Draw("SAME");
       fPeak3FitClone2->Draw("SAME");
       f3AlphaFitClone2->Draw("SAME");
       point1->Draw("SAME");
       point2->Draw("SAME");
       point3->Draw("SAME");
       legend3->Draw("SAME");

       gPad->Modified();
       gPad->Update();
    //   getchar();

       canvas->Print(pathSiEnergyCaliFitPDF.c_str());
     }
   }
   cans_end->Print(pathSiEnergyCaliFitPDFend.c_str());


   Double_t graphYmin[4] = {-10.,-25., -20, -10.};
   Double_t graphYmax[4] = { 10., -10., -5.,  5. };
   Double_t graphYbins[4] = {20, 15, 15, 15};
   TCanvas* cans_grap = new TCanvas("PedestalOffset","PedestalOffset", 1200,1000);
   cans_grap->Divide(2,2);
   for(int i=0; i<SSDNum; i++)
   {
     cans_grap->cd(i+1);
     gPad->SetGridx();

     // for Zooming the graph
     TH2D* HistforZoomedGraph = new TH2D("HistforZoomedGraph","HistforZoomedGraph",17,0,17,graphYbins[i],graphYmin[i],graphYmax[i]);
     HistforZoomedGraph->SetStats(0);
     HistforZoomedGraph->Draw();

     TGraph* graph_PulserOffset = new TGraph(Num_PedestalOffset[i], SSDCHNum[i], PulserPedestalOffset[i]);
     TGraph* graph_AlphaCaliOffset = new TGraph(Num_PedestalOffset[i], SSDCHNum[i], AlphaCaliPedestalOffset[i]);

     graph_PulserOffset->SetTitle(Form("SSD%d_%s_PedestalOffset_%s_%s",i+1, LayerTag.c_str(), FileTag.c_str(), AlphaCaliTag.c_str()));
     graph_PulserOffset->GetXaxis()->SetTitle("CH");
     graph_PulserOffset->GetYaxis()->SetTitle("PedestalOffset");
     graph_PulserOffset->SetMarkerStyle(20);
     graph_PulserOffset->SetMarkerColor(kBlue);
     graph_AlphaCaliOffset->SetMarkerStyle(21);
     graph_AlphaCaliOffset->SetMarkerColor(kRed);

     TLegend* legend = new TLegend(0.55,0.75,0.9 ,0.9);
     legend->AddEntry(graph_PulserOffset,"PulserOffset","p");
     legend->AddEntry(graph_AlphaCaliOffset,"AlphaCaliOffset","p");

     graph_AlphaCaliOffset->Draw("PL");
     graph_PulserOffset->Draw("PL");
     legend->Draw("SAME");

   }
   cans_grap->Print(pathPedestalOffsetPDF.c_str());
   cans_grap->Print(pathPedestalOffsetPNG.c_str());

   DeleteData(PulserIn,SSDNum, CHNum, numpar_PulserIn);
   DeleteData(AlphaEIn,SSDNum, CHNum, numpar_AlphaEIn);
   DeleteData(AlphaChIn,SSDNum,CHNum, numpar_AlphaChIn);

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
