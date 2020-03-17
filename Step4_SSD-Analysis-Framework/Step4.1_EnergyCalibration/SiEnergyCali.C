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

//______________________________________________________________________________
Double_t*** ReadData(const char* datapath, int& SSDNum, int& CHNum, int& ParNum);

//______________________________________________________________________________
void DeleteData(Double_t*** p, int& SSDNum, int& CHNum, int& ParNum);

//___________________________________________________________
void SiEnergyCali()
{
   std::string LayerTag("L1");
   std::string FileTag("Height");

   std::string pathPuserIn(Form("output/SSD_%s_PulserCali_%s.dat",LayerTag.c_str(),FileTag.c_str())); // Pulser fitting parameters
   std::string pathAlphaEIn("output/SSD_Alpha_Energy.dat");   // Alpha enegies in MeV
   std::string pathAlphaChIn(Form("output/SSD_Alpha_%sS_E_00_04.dat",LayerTag.c_str())); // Channel of the alpha peaks

   std::string pathAlphaFitParOut(Form("output/Fitpara_SSD_Alpha_%sS_E_00_04.dat",LayerTag.c_str()));

   int SSDNum = 4;
   int CHNum  = 16;
   int numpar_PulserIn  = 4;  // 4 pars: a, err_a, b, err_b        (E = a*ch + b)
   int numpar_AlphaEIn  = 3;  // 3 alpha energies: E1, E2, E3
   int numpar_AlphaChIn = 3;  // 3 alpha channels: CH1, CH2, CH3
   int numpar_3AlphaFit = 4;  // 4 pear: a', err_a', b', err_b'    (E' = a'*ch + b')

   Double_t*** PulserIn  = ReadData(pathPuserIn.c_str(),  SSDNum, CHNum, numpar_PulserIn);
   Double_t*** AlphaEIn  = ReadData(pathAlphaEIn.c_str(), SSDNum, CHNum, numpar_AlphaEIn);
   Double_t*** AlphaChIn = ReadData(pathAlphaChIn.c_str(),SSDNum, CHNum, numpar_AlphaChIn);
   Double_t*** AlphaFit  = ReadData(pathAlphaFitParOut.c_str(), SSDNum, CHNum, numpar_3AlphaFit);

   TCanvas * canvas1 = new TCanvas("canvas","Energy Linearity",1800,600);
   canvas1->Divide(3,1);

   for(int i=0; i<SSDNum; i++)
   {
     for(int j=0; j<CHNum; j++)
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

      // if(E1_CH<50.0) continue;

       Double_t c1 = E1_MeV /(a * E1_CH + b);   // peak1 比例系数
       Double_t c2 = E2_MeV /(a * E2_CH + b);   // peak1 比例系数
       Double_t c3 = E3_MeV /(a * E3_CH + b);   // peak1 比例系数

       Double_t k1 = c1 * a;  // E1_MeV = k1 * E1_CH + h1
       Double_t k2 = c2 * a;  // E1_MeV = k1 * E1_CH + h1
       Double_t k3 = c3 * a;  // E1_MeV = k1 * E1_CH + h1

       Double_t h1 = c1 * b;
       Double_t h2 = c2 * b;
       Double_t h3 = c3 * b;

       Double_t k11 = 1./k1;  // E1_CH = k11 * E1_MeV + h11
       Double_t k22 = 1./k2;  // E1_CH = k11 * E1_MeV + h11
       Double_t k33 = 1./k3;  // E1_CH = k11 * E1_MeV + h11

       Double_t h11 = -h1/k1;
       Double_t h22 = -h2/k2;
       Double_t h33 = -h3/k3;

       Double_t a_3AlphaFit = AlphaFit[i][j][0]; // E = a_3AlphaFit*CH + B_3AlphaFit
       Double_t b_3AlphaFit = AlphaFit[i][j][2];

       Double_t a1_3AlphaFit = 1./a_3AlphaFit;
       Double_t b1_3AlphaFit = -b_3AlphaFit/a_3AlphaFit;

       printf("k11=%.4f k22=%.4f  k33=%.4f a_3Alpha=%.4f h11=%.4f h22=%.4f h33=%.4f b_3Alpha=%.4f",
               k11,k22,k33,a1_3AlphaFit,h11,h22,h33,b1_3AlphaFit);

       //  画出刻度曲线, 比较3-alpha峰拟合 与 1-alpha峰 + Pulser 两种拟合结果
       TF1 * fPeak1Fit = new TF1("fPeak1Fit", Form("%.4f*x+%.4f", k1 ,h1), 0, 4096);
       TF1 * fPeak2Fit = new TF1("fPeak2Fit", Form("%.4f*x+%.4f", k2 ,h2), 0, 4096);
       TF1 * fPeak3Fit = new TF1("fPeak3Fit", Form("%.4f*x+%.4f", k3 ,h3), 0, 4096);
       TF1 * f3AlphaFit = new TF1("f3AlphaFit", Form("%.4f*x+%.4f", a_3AlphaFit, b_3AlphaFit), 0, 4096);

       fPeak1Fit->SetLineColor(kRed);
       fPeak2Fit->SetLineColor(kGreen);
       fPeak3Fit->SetLineColor(kBlue);
       f3AlphaFit->SetLineColor(kBlack);


       canvas1->cd(1);
       Double_t y1_max = k1 * 4096 + h1;
       fPeak1Fit->GetXaxis()->SetRangeUser(0,4096);
       fPeak1Fit->GetYaxis()->SetRangeUser(0,y1_max);
       fPeak1Fit->Draw();
       fPeak2Fit->Draw("SAME");
       fPeak3Fit->Draw("SAME");
       f3AlphaFit->Draw("SAME");
       gPad->Modified();
       gPad->Update();

       canvas1->cd(2);
       Double_t x2_max = E3_CH + 10;
       Double_t y2_max = k1 * x2_max + h1;
       fPeak1Fit->GetXaxis()->SetRangeUser(0,x2_max);
       fPeak1Fit->GetYaxis()->SetRangeUser(0,y2_max);
       fPeak1Fit->Draw();
       fPeak2Fit->Draw("SAME");
       fPeak3Fit->Draw("SAME");
       f3AlphaFit->Draw("SAME");
       gPad->Modified();
       gPad->Update();

       canvas1->cd(3);
       Double_t x3_min = E1_CH-10;
       Double_t x3_max = E3_CH-10;
       Double_t y3_min = k3 * x3_min + h3 - 0.5;
       Double_t y3_max = k1 * x3_max + h1 + 0.5;
       fPeak1Fit->GetXaxis()->SetRangeUser(x3_min,x3_max);
       fPeak1Fit->GetYaxis()->SetRangeUser(y3_min,y3_max);
       fPeak1Fit->Draw();
       fPeak2Fit->Draw("SAME");
       fPeak3Fit->Draw("SAME");
       f3AlphaFit->Draw("SAME");
       gPad->Modified();
       gPad->Update();

       getchar();
     }
   }

  DeleteData(PulserIn,SSDNum, CHNum, numpar_PulserIn);
  DeleteData(AlphaEIn,SSDNum, CHNum, numpar_AlphaEIn);
  DeleteData(AlphaChIn,SSDNum,CHNum, numpar_AlphaChIn);
  return;
}

//_______________________________________________
Double_t*** ReadData(const char* datapath, int& SSDNum, int& CHNum, int& ParNum)
{
///////////////////////////////////////////////////////
//     构建一个函数，返回类型是 三维数组(三级指针)          //
//     注意多级指针的使用！！！                          //
///////////////////////////////////////////////////////
   Double_t*** readpar = NULL;
   readpar = new Double_t** [SSDNum];
   for(int i=0; i<SSDNum; i++)
   {
     readpar[i] = new Double_t* [CHNum];
     for(int j=0; j<CHNum; j++)
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

     int ssdnum;
     int chnum;

     LineStream >> ssdnum >> chnum;
     for(int i=0; i<ParNum; i++)
     {
       LineStream >>readpar[ssdnum][chnum][i];
     }
   }
   in.close();
   return readpar;
}

//______________________________________________________________________________
void DeleteData(Double_t*** p, int& SSDNum, int& CHNum, int& ParNum)
{
/////////////////////////////////////////////////////////////
//    释放前面用 new方法给 Double_t*** p 动态分配的内存           //                                                    //
//    多级指针需要逐层释放内存！！！                            //
/////////////////////////////////////////////////////////////
  for(int i=0; i<SSDNum; i++)
  {
    for(int j=0; j<CHNum; j++)
    {
      delete [] p[i][j];
    }
  }
  for(int i=0; i<SSDNum; i++)
  {
    delete [] p[i];
  }
  delete [] p;
}
