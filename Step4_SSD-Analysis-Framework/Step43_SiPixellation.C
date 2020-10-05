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
  Int_t option2 = 1;

  cout<<"********************************************"<<endl;
  cout<<"     欢迎来到 Step43_SiPixellation 界面 !    "<<endl;
  cout<<"********************************************"<<endl;
  cout<<endl;
  while (option2 == 1) {
    cout<<"请选择您需要进行的操作: "<<endl;
    cout<<"============================================================="<<endl;
    cout<<"431.运行 CalculateEffectiveThickness() - 计算每个双面硅条 pixel 的等效厚度"<<endl;
    cout<<"432.运行 SiPixellation()               - 计算每个像素点的 theta, phi"<<endl;
    cout<<"433.运行 CheckSiPixellation()          - 检查像素化结果是否正确"<<endl;
    cout<<"============================================================="<<endl;
    cout<<"请在此输入您的选项: 431, 432, 433"<<endl;
    cin >>option;
    if (option == 431) pixel.CalculateEffectiveThickness();
    if (option == 432) pixel.SiPixellation();
    if (option == 433) pixel.CheckSiPixellation();
    }
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.继续输入操作选项."<<endl;
    cout<<"2.结束操作, 退出程序！"<<endl;
    cin >>option2;
    if (option2 == 2) {
      cout<<"***********************************************"<<endl;
      cout<<"          Exit Step43_SiPixellation!          "<<endl;
      cout<<"***********************************************"<<endl;
      return;
    }
  }
}
