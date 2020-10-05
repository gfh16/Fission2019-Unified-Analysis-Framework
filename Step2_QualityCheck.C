#include "include/ReadFileModule.h"

#include <iostream>
#include <string.h>

using namespace std;


//______________________________________________________________________________
void Step2_QualityCheck()
{
  Int_t option2 = 1;
  Int_t option_step2;

  std::string FileName("QC_listfiles.dat");
  std::string pathFolder("/home/sea/Fission2019_Data/MapRoot/");
  std::string pathFileout(Form("%s%s",pathFolder.c_str(), FileName.c_str()));

  ReadFileModule readfile;

  cout<<"***********************************************"<<endl;
  cout<<"       欢迎来到 Step2_QualityCheck 界面 !        "<<endl;
  cout<<"***********************************************"<<endl;
  cout<<endl;
  while (option2 == 1) {
    cout<<"请选择您需要进行的操作: "<<endl;;
    cout<<"==============================================================="<<endl;
    cout<<"21  Generate filelists          -- 指定输出文件: QC_listfiles.dat"<<endl;
    cout<<"22  Modify CSHINEQualifyCheck.C -- 根据实际情况修改 CSHINEQualifyCheck.C"<<endl;
    cout<<"23  Modify Makefile             -- 批量处理质检文件,编译后速度更快"<<endl;
    cout<<"24  make clean                  -- make clean"<<endl;
    cout<<"25  make                        -- make"<<endl;
    cout<<"26  ./bin/QualityCheck          -- 批量生成质检文件 .pdf 与 .root"<<endl;
    cout<<"=============================================================="<<endl;
    cout<<"请在此输入您的选项: 21, 22, 23, 24, 25, 26"<<endl;
    cin >>option_step2;

    if (option_step2 == 21)  readfile.GetFileNamesFromFolder(pathFolder.c_str(), pathFileout.c_str());
    if (option_step2 == 22)  system("vim src/CSHINEQualityCheck.C");
    if (option_step2 == 23)  system("vim Makefile");
    if (option_step2 == 24)  system("make clean");
    if (option_step2 == 25)  system("make -j16");
    if (option_step2 == 26)  { cout<<pathFileout.c_str()<<endl; system("./bin/QualityCheck"); }

    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step2, 重新进行操作选择"<<endl;
    cout<<"2.操作完成, 结束程序"<<endl;
    cin >>option2;
    if (option2 == 2) {
      cout<<"***********************************************"<<endl;
      cout<<"          Exit Step2_QualityCheck!             "<<endl;
      cout<<"***********************************************"<<endl;
      return;
    }
  }
}
