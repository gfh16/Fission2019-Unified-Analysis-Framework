#include "include/ReadFileModule.h"


//______________________________________________________________________________
void Step1_DataConversion()
{
  Int_t option1;
  Int_t option2 = 1;

  ReadFileModule readfile;

  std::string pathRawData("Step1_DataConversion/vmedata/");
  std::string pathRawDataFilelist("Step1_DataConversion/vmedata/listrawfilename.dat");

  std::string pathRootData("/home/sea/Fission2019_Data/RawRoot/");
  std::string pathRootDataFilelist("/home/sea/Fission2019_Data/RawRoot/listrootfilename.dat");

  cout<<"***********************************************"<<endl;
  cout<<"       欢迎来到 Step1_DataConversion 界面 !      "<<endl;
  cout<<"***********************************************"<<endl;
  cout<<endl;
  cout<<"请选择您需要进行的操作: "<<endl;
  cout<<endl;

  cout<<"1.1 将原始文件转换为root格式"<<endl;
  cout<<"111 原始文件都放在/vmedata/文件夹下"<<endl;
  cout<<"112 在/vmedata/文件夹下添加 listrawfilename.dat. 执行 13 直接生成 listrawfilename.dat."<<endl;
  cout<<"113 编译(make), 然后执行 ./raw2root.sh listrawfilename.dat"<<endl;
  cout<<endl;

  cout<<"1.2 将RawRoot数据住转换成MapRoot数据"<<endl;
  cout<<"121 根据 ADC/TDC 与探测器的对应关系, 修改/RIBLLVMEDAQ/ReadRootFile2D.cpp"<<endl;
  cout<<"122 根据需要,修改输入路径(datapath)与输出路径(pathoutfile)."<<endl;
  cout<<"123 在输入路径(datapath)文件夹下添加 listrootfilename.dat. 执行 14 直接生成 listrootfilename.dat."<<endl;
  cout<<"124 编译(make), 然后执行 ./ReadRootFile2D listrootfilename.dat"<<endl;
  cout<<endl;

  cout<<"1.3 生成 listrawfilename"<<endl;
  cout<<"13 调用 ReadFileModule::GetFileNames(),生成 listrawfilename.dat"<<endl;
  cout<<endl;

  cout<<"1.4 生成 listrootfilename"<<endl;
  cout<<"14 调用 ReadFileModule::GetFileNames(),生成 listrootfilename.dat"<<endl;

  while (option2 == 1) {
    cout<<"============================================================="<<endl;
    cout<<"请在此输入您的选项: 13 or 14"<<endl;
    cin >>option1;

    switch(option1) {
      case 13 :
        readfile.GetFileNamesFromFolder(pathRawData.c_str(), pathRawDataFilelist.c_str());
        break;

      case 14 :
        readfile.GetFileNamesFromFolder(pathRootData.c_str(), pathRootDataFilelist.c_str());
        break;
    }
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.继续输入操作选项."<<endl;
    cout<<"2.操作结束, 退出程序！"<<endl;
    cin >>option2;
    if (option2 == 2) {
      cout<<"***********************************************"<<endl;
      cout<<"          Exit Step1_DataConversion!           "<<endl;
      cout<<"***********************************************"<<endl;
      return;
    }
  }
}
