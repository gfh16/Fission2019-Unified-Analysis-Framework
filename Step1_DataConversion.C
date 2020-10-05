#include "include/ReadFileModule.h"

#include <iostream>
#include <string.h>

using namespace std;


void Step11_GenerateFileLists();
void Step12_RawToRawRoot();
void Step13_RawRootToMapRoot();

void InputFileName(Int_t opt_filename, std::string& pathfolder, std::string& pathfile);

//______________________________________________________________________________
void Step1_DataConversion()
{
  Int_t option1 = 1;
  Int_t option_step1;

  cout<<"***********************************************"<<endl;
  cout<<"       欢迎来到 Step1_DataConversion 界面 !      "<<endl;
  cout<<"***********************************************"<<endl;
  cout<<endl;
  while (option1 == 1) {
    cout<<"请选择您需要进行的操作: "<<endl;
    cout<<"========================================================="<<endl;
    cout<<"11 Generate filelists  -- 生成filelists文件,以便后续批量处理 "<<endl;
    cout<<"12 raw2root            -- 将原始二进制文件转换为root格式"     <<endl;
    cout<<"13 ReadRootFile2D      -- 将RawRoot数据转换成MapRoot数据"   <<endl;
    cout<<"========================================================="<<endl;
    cout<<"请在此输入您的选项: 11, 12, 13"<<endl;
    cin >>option_step1;
    if (option_step1 == 11)  Step11_GenerateFileLists();
    if (option_step1 == 12)  Step12_RawToRawRoot();
    if (option_step1 == 13)  Step13_RawRootToMapRoot();
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step1,重新进行操作选择"<<endl;
    cout<<"2.操作结束, 退出程序！"<<endl;
    cin >>option1;
    if (option1 == 2) {
      cout<<"***********************************************"<<endl;
      cout<<"          Exit Step1_DataConversion!           "<<endl;
      cout<<"***********************************************"<<endl;
      return;
    }
  }
}

//______________________________________________________________________________
void Step11_GenerateFileLists()
{
  Int_t option11 = 1;
  Int_t option_file;

  std::string pathFolder;
  std::string pathFile;

  ReadFileModule readfile;

  cout<<"***************************************************************"<<endl;
  cout<<"         您正在生成 filelists  Step11_GenerateFileLists ！       "<<endl;
  cout<<"***************************************************************"<<endl;
  cout<<endl;
  while (option11 == 1) {
    InputFileName(option_file, pathFolder, pathFile);
    readfile.GetFileNamesFromFolder(pathFolder.c_str(), pathFile.c_str());
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step11,重新进行操作选择"<<endl;
    cout<<"2.操作完成, 结束程序"<<endl;
    cin >> option11;
    if (option11 == 2) {
      cout<<"*************************************************"<<endl;
      cout<<"          Exit Step11_GenerateFileLists!         "<<endl;
      cout<<"*************************************************"<<endl;
      return;
    }
  }
}

//______________________________________________________________________________
void Step12_RawToRawRoot()
{
  Int_t option12 = 1;
  Int_t option_step12;

  cout<<"***************************************************************"<<endl;
  cout<<"    您正在将原始二进制数据转换成root格式  Step12_RawToRawRoot ！     "<<endl;
  cout<<"***************************************************************"<<endl;
  cout<<endl;
  while (option12 == 1) {
    cout<<"请选择您需要进行的操作:"<<endl;
    cout<<"============================================================="<<endl;
    cout<<"提示: !!!一般不需要修改 RIBLLVMEDAQ/Raw2ROOT.cpp!!!"<<endl;
    cout<<"121  Check          -- 检查/vmedata/下 listrawfilename.dat 是否存在;若否,则先执行 Step11_GenerateFileLists()"<<endl;
    cout<<"122  make clean     -- make clean(可进入 Step1_DataConversion/ 下执行)"<<endl;
    cout<<"123  make           -- 生成可执行文件 Raw2ROOT"<<endl;
    cout<<"124  ./raw2root.sh  -- 批量文件转换"<<endl;
    cout<<"============================================================="<<endl;
    cout<<"请在此输入您的选项: 121, 122, 123, 124"<<endl;
    cin >>option_step12;

    if (option_step12 == 121) cout<<"请到/vmedata/文件夹检查文件 listrawfilename.dat 是否存在!"<<endl;
    if (option_step12 == 122) system("cd Step1_DataConversion/ && make clean");
    if (option_step12 == 123) system("cd Step1_DataConversion/ && make");
    if (option_step12 == 124) system("cd Step1_DataConversion/ && ./raw2root.sh listrawfilename.dat");

    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step12,重新进行操作选择"<<endl;
    cout<<"2.操作完成, 结束程序"<<endl;
    cin >> option12;
    if (option12 == 2) {
      cout<<"*************************************************"<<endl;
      cout<<"             Exit Step12_RawToRawRoot!           "<<endl;
      cout<<"*************************************************"<<endl;
      return;
    }
  }
}

//______________________________________________________________________________
void Step13_RawRootToMapRoot()
{
  Int_t option13 = 1;
  Int_t option_step13;

  cout<<"*********************************************************************"<<endl;
  cout<<"   您正在将 RawRoot 格式转换成 MapRoot 格式  Step13_RawRootToMapRoot ！  "<<endl;
  cout<<"*********************************************************************"<<endl;
  cout<<endl;
  while (option13 == 1) {
    cout<<"请选择您需要进行的操作:"<<endl;
    cout<<"============================================================="<<endl;
    cout<<"*************************************************************"<<endl;
    cout<<"提示: !!!需要修改 RIBLLVMEDAQ/ReadRootFile2D.cpp!!!"<<endl;
    cout<<"注意: !!!Fission2019实验中, 0089-0120 与 0121-0438 需要分开转换, 因为Au-Si探测器有改变!!!"<<endl;
    cout<<"*************************************************************"<<endl;
    cout<<"131  Modify ReadRootFile2D.cpp"<<endl;
    cout<<"     (a) 根据 ADC/TDC 与探测器的对应关系修改 ReadRootFile2D.cpp"<<endl;
    cout<<"     (b) 根据需要,修改输入路径(datapath)与输出路径(pathoutfile)"<<endl;
    cout<<endl;
    cout<<"132  Check            -- 检查 datapath 路径下是否包含 listrootfilename.dat;若否, 则先执行 Step11_GenerateFileLists()"<<endl;
    cout<<"133  make clean       -- make clean(可进入 Step1_DataConversion/ 下执行)"<<endl;
    cout<<"134  make             -- 生成可执行文件 ReadRootFile2D"<<endl;
    cout<<"135  ./ReadRootFile2D -- 批量文件转换"<<endl;
    cout<<"============================================================="<<endl;
    cout<<"请在此输入您的选项: 131, 132, 133, 134, 135"<<endl;
    cin >>option_step13;

    if (option_step13 == 131) system("vim Step1_DataConversion/RIBLLVMEDAQ/ReadRootFile2D.cpp");
    if (option_step13 == 132) cout<<"请到RawRoot文件夹检查文件 listrootfilename.dat 是否存在!"<<endl;
    if (option_step13 == 133) system("cd Step1_DataConversion && make clean");
    if (option_step13 == 134) system("cd Step1_DataConversion && make");
    if (option_step13 == 135) system("cd Step1_DataConversion && ./ReadRootFile2D listrootfilename.dat");

    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step13,重新进行操作选择"<<endl;
    cout<<"2.操作完成, 结束程序"<<endl;
    cin >> option13;
    if (option13 == 2) {
      cout<<"***************************************************"<<endl;
      cout<<"             Exit Step13_RawRootToMapRoot!         "<<endl;
      cout<<"***************************************************"<<endl;
      return;
    }
  }
}

//______________________________________________________________________________
void InputFileName(Int_t opt_filename, std::string& pathfolder, std::string& pathfile)
{
  std::string pathRawData("Step1_DataConversion/vmedata/");
  std::string pathRawDataFilelist("Step1_DataConversion/vmedata/listrawfilename.dat");

  std::string pathRootData(Form("%s/RawRoot/", PATHROOTFILESFOLDER));
  std::string pathRootDataFilelist(From("%slistrootfilename.dat", pathRootData.c_str()));

  cout<<"请选择读取下面哪个文件夹下的所有文件名: "<<endl;
  cout<<"======================================================================="<<endl;
  cout<<"1.RawData: vmedata/                  -- 指定输出文件: listrawfilename.dat"<<endl;
  cout<<"2.RawRoot: Fission2019_Data/RawRoot/ -- 指定输出文件: listrootfilename.dat"<<endl;
  cout<<"========================================================================"<<endl;
  cout<<"请输入您的选项:"<<endl;
  cin >>opt_filename;
  if (opt_filename == 1) { pathfolder = pathRawData;  pathfile = pathRawDataFilelist; }
  if (opt_filename == 2) { pathfolder = pathRootData; pathfile = pathRootDataFilelist; }
}
