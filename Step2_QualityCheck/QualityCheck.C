
#include "../include/TimeAndPercentage.h"
#include "../include/ReadFileModule.h"
#include "../include/CSHINEQualityCheck.h"

#include <iostream>
#include <string.h>

using namespace std;


//______________________________________________________________________________
int main(int argc, char *argv[])
{
  Int_t option;
  std::string listfilenames;

  cout<<"请输入 listfilenames 文件: "<<endl;
  cin >>listfilenames;

  TString pathlistname(Form("/home/sea/Fission2019_Data/MapRoot/%s",listfilenames.c_str()));
  if (!pathlistname.IsNull()) {
    printf("文件 %s 不存在！\n", pathlistname.Data());
    return -1;
  }

  cout<<"请选择质检方式: (两种方式实质内容一样)"<<endl;
  cout<<"1.使用 TreeReader 方式(推荐)"<<endl;
  cout<<"2.使用 SetBranchAddress 方式"<<endl;
  cin >> option;

  CSHINEQualityCheck  quality;
  if (option == 1) quality.QC_TreeReader(pathlistname);
  if (option == 2) quality.QC_BranchAdress(pathlistname);

  return 0;
}
