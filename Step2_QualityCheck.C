#include "include/ReadFileModule.h"


//______________________________________________________________________________
void Step2_QualityCheck()
{
  Int_t option_step;
  Int_t option_index = 1;

  std::string pathFolder("/home/sea/Fission2019_Data/MapRoot/");
  std::string pathFileout("/home/sea/Fission2019_Data/MapRoot/QC_listfiles.dat");

  ReadFileModule readfile;

  cout<<"***********************************************"<<endl;
  cout<<"       欢迎来到 Step2_QualityCheck 界面 !        "<<endl;
  cout<<"***********************************************"<<endl;
  cout<<endl;
  cout<<"请选择您需要进行的操作: ";
  cout<<"==============================================="<<endl;
  cout<<"211 生成 listfilename 文件, 文件内容是需要质检的文件名"<<endl;
  cout<<"221 在当前目录下, 执行编译命令: 先 make clean, 再 make"<<endl;
  cout<<"231 生成质检文件,包括 pdf 文件与 root 文件"<<endl;

  while (option_index == 1) {
    cout<<"请在此输入您的选项: 211, 221, 231"<<endl;
    cin >>option_step;

    if (option_step == 211) {
      readfile.GetFileNamesFromFolder(pathFolder.c_str(), pathFileout.c_str());
      printf("新生成的文件: %s \n", pathFileout.c_str());
    } else if (option_step == 221) {
      cout<<"请在当前目录下进行编译: 先 make clean, 后 make"<<endl;
    } else if (option_step == 231) {
      system("./bin/QualityCheck");
    } else {
      cout<<"输入的选项有误!"<<endl;
      return -1;
    }
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回上一层,重新进行操作选择"<<endl;
    cout<<"2.操作完成, 结束程序"<<endl;
    cin >>option_index;
    if (option_index == 2) return;
  }
}
