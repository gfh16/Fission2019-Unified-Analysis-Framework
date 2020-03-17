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
double*** ReadData(const char* datapath, int& SSDNum, int& CHNum, int& ParNum);

//______________________________________________________________________________
void DeleteData(double*** p, int& SSDNum, int& CHNum, int& ParNum);

//___________________________________________________________
void SiEnergyCali()
{
   std::string LayerTag("L1");
   std::string FileTag("Height");
   std::string pathPuserIn(Form("output/SSD_%s_PulserCali_%s.dat",LayerTag.c_str(),FileTag.c_str()));
   std::string pathAlphaEIn("output/SSD_Alpha_Energy.dat");
   std::string pathAlphaChIn(Form("output/SSD_Alpha_%sS_E_00_04.dat",LayerTag.c_str()));
   std::string pathFitParAlpha(Form("output/Fitpara_SSD_Alpha_%sS_E_00_04.dat",LayerTag.c_str()));

   int SSDNum = 4;
   int CHNum  = 16;
   int numpar_PulserIn  = 4;
   int numpar_AlphaEIn  = 3;
   int numpar_AlphaChIn = 3;
   int numpar_AlphaFit  = 3;

   double*** PulserIn  = ReadData(pathPuserIn.c_str(),  SSDNum, CHNum, numpar_PulserIn);
   double*** AlphaEIn  = ReadData(pathAlphaEIn.c_str(), SSDNum, CHNum, numpar_AlphaEIn);
   double*** AlphaChIn = ReadData(pathAlphaChIn.c_str(),SSDNum, CHNum, numpar_AlphaChIn);
   double*** AlphaFit  = ReadData(pathFitParAlpha.c_str(), SSDNum, CHNum, numpar_AlphaFit);

   TCanvas * c1 = new TCanvas("c1","Energy Linearity",600,600);
   for(int i=0; i<SSDNum; i++)
   {
     for(int j=0; j<CHNum; j++)
     {
       // 计算截距 h
       //  Linear Pulser:   y = a * ch + b
       //  Linear Energy:   E(MeV) = k * ch + h, k = a, h = E(MeV)-k * ch

       double a = PulserIn[i][j][0];
       double b = PulserIn[i][j][2];      // energy of peak1
       double E_CH  = AlphaChIn[i][j][0];
       double E_MeV = AlphaEIn[i][j][0];
       if(E_CH<50.0) continue;
       double k = a;    // Slope of Linear Energy
       double h = E_MeV - k * E_CH;
       cout<<"Slope  = "<< k <<"    "<<"Ch0 = "<< h <<endl;

       double a_alphafit = AlphaFit[i][j][0];
       double b_alphafit = AlphaFit[i][j][2];

       //  画出刻度曲线, 比较3-alpha峰拟合 与 1-alpha峰 + Pulser 两种拟合结果
       TF1 * falphapulser = new TF1("falphapulser",Form("%.4f*x-%.4f",k,h),0,200);
    //   TF1 * f3alpha = new TF1("f3alpha",Form("%.4f*x+%.4f",a_alphafit,b_alphafit),0,4096);
       falphapulser->Draw();
    //   f3alpha->Draw("SAME");


     }
   }
  DeleteData(PulserIn,SSDNum, CHNum, numpar_PulserIn);
  DeleteData(AlphaEIn,SSDNum, CHNum, numpar_AlphaEIn);
  DeleteData(AlphaChIn,SSDNum,CHNum, numpar_AlphaChIn);
  return;
}

//_______________________________________________
double*** ReadData(const char* datapath, int& SSDNum, int& CHNum, int& ParNum)
{
///////////////////////////////////////////////////////
//     构建一个函数，返回类型是 三维数组(三级指针)          //
//     注意多级指针的使用！！！                          //
///////////////////////////////////////////////////////
   double*** readpar = NULL;
   readpar = new double** [SSDNum];
   for(int i=0; i<SSDNum; i++)
   {
     readpar[i] = new double* [CHNum];
     for(int j=0; j<CHNum; j++)
     {
       readpar[i][j]= new double[ParNum];
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
void DeleteData(double*** p, int& SSDNum, int& CHNum, int& ParNum)
{
/////////////////////////////////////////////////////////////
//    释放前面用 new方法给 double*** p 动态分配的内存           //                                                    //
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
