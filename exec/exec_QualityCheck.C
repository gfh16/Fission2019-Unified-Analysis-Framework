
#include "../include/TimeAndPercentage.h"
#include "../include/ReadFileModule.h"
#include "../include/CSHINEQualityCheck.h"
#include "../include/shared.h"

#include <iostream>
#include <string.h>

using namespace std;


//______________________________________________________________________________
int main(int argc, char *argv[])
{
  Int_t option;
  TString pathlistname;

  cout<<"请输入 pathlistname 文件: "<<endl;
  cout<<Form("提示: %sMapRoot/%s", PATHROOTFILESFOLDER, NAMEMAPROOTFILELIST)<<endl;
  cin >>pathlistname;

  cout<<"请选择质检方式: (两种方式实质内容一样)"<<endl;
  cout<<"1.使用 TreeReader 方式(推荐)"<<endl;
  cout<<"2.使用 SetBranchAddress 方式"<<endl;
  cin >> option;

  CSHINEQualityCheck  quality;
  if (option == 1) quality.QC_TreeReader(pathlistname);
  if (option == 2) quality.QC_BranchAdress(pathlistname);

  return 0;
}
