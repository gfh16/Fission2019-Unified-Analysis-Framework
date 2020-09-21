////////////////////////////////////////////////////////////////////////////////
//   此程序用于创建 LayerEventTree, SSDEventTree, GlobalEventTree
//
//    gfh, 2020-09-06
////////////////////////////////////////////////////////////////////////////////

#include "../include/CSHINEEvent.h"


//______________________________________________________________________________
void Step47_EventTree()
{
  CSHINEBuildEvent   buildevent;

  Int_t option;
  Int_t option2 = 1;

  Int_t FirstRun;
  Int_t LastRun;

  cout<<"********************************************"<<endl;
  cout<<"       欢迎来到 Step4.7_EventTree 界面 !      "<<endl;
  cout<<"********************************************"<<endl;
  cout<<endl;
  cout<<"请选择您需要进行的操作: "<<endl;
  cout<<"471.运行 BuildLayerEventTree (Int_t fisrtrun, Int_t lastlun)"<<endl;
  cout<<"472.运行 BuildSSDEventTree   (Int_t fisrtrun, Int_t lastlun)"<<endl;
  cout<<"473.运行 BuildGlobalEventTree(Int_t fisrtrun, Int_t lastlun)"<<endl;

  while (option2 == 1) {
    cout<<"============================================================="<<endl;
    cout<<"请在此输入您的选项: 471, 472, 473"<<endl;
    cin >>option;

    cout<<"请输入起始文件编号: (121 - 432)"<<endl;
    cin >>FirstRun;
    cout<<"请输入终止文件编号: (121 - 432)"<<endl;
    cin >>LastRun;
    if (FirstRun > LastRun) {
      cout<<"错误: 起止文件编号有误, 请重新输入:"<<endl;
      cout<<"请重新输入起始文件编号 firstrun: (121 - 432)"<<endl;
      cin >>FirstRun;
      cout<<"请重新输入终止文件编号  lastrun: (121 - 432)"<<endl;
      cin >>LastRun;
    }

    switch(option) {
      case 471:
        buildevent.BuildLayerEventTree(FirstRun, LastRun);
        break;
     /*
      case 472:
        buildevent.BuildSSDEventTree(FirstRun, LastRun);
        break;

      case 473:
        buildevent.BuildGlobalEventTree(FirstRun, LastRun);
        break;
    */
    }
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.继续输入操作选项."<<endl;
    cout<<"2.结束操作, 退出程序！"<<endl;
    cin >>option2;
    if (option2 == 2) {
      cout<<"***********************************************"<<endl;
      cout<<"            Exit Step4.7_EventTree !           "<<endl;
      cout<<"***********************************************"<<endl;
      return;
    }
  }
}
