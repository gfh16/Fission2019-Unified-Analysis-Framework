////////////////////////////////////////////////////////////////////////////////
//
// 此脚本用于确定硅条探测器像素点的几何位置
//
// gfh, 2020-09-05 重新修改
////////////////////////////////////////////////////////////////////////////////

#include "../include/CSHINESiPixelGeometry.h"


//______________________________________________________________________________
void Step43_SiPixellation()
{
  CSHINESiPixelGeometry pixel;

  Int_t option;

  cout<<"********************************************"<<endl;
  cout<<"     欢迎来到 Step4.3_SiPixellation 界面 !    "<<endl;
  cout<<"********************************************"<<endl;
  cout<<endl;
  cout<<"请选择您需要进行的操作: "<<endl;
  cout<<"431.运行 CalculateEffectiveThickness() - 计算每个双面硅条 pixel 的等效厚度"<<endl;
  cout<<"432.运行 SiPixellation()               - 计算每个像素点的 theta, phi"<<endl;
  cout<<"433.运行 CheckSiPixellation()          - 检查像素化结果是否正确"<<endl;

  cin>>option;

  switch(option) {
    case 431:
      pixel.CalculateEffectiveThickness();
      break;

    case 432:
      pixel.SiPixellation();
      break;

    case 433:
      pixel.CheckSiPixellation();
      break;
  }
}
