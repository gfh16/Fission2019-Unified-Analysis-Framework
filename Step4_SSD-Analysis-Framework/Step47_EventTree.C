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
  Int_t FirstRun;
  Int_t LastRun;

  cout<<"请选择您需要进行的操作: "<<endl;
  cout<<"1.运行 BuildLayerEventTree (Int_t fisrtrun, Int_t lastlun)"<<endl;
  cout<<"2.运行 BuildSSDEventTree   (Int_t fisrtrun, Int_t lastlun)"<<endl;
  cout<<"3.运行 BuildGlobalEventTree(Int_t fisrtrun, Int_t lastlun)"<<endl;

  cin >> option;

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
    case 1:
      buildevent.BuildLayerEventTree(FirstRun, LastRun);
      break;

/*
    case 2:
      buildevent.BuildSSDEventTree(FirstRun, LastRun);
      break;

    case 3:
      buildevent.BuildGlobalEventTree(FirstRun, LastRun);
      break;
*/
  }
}
