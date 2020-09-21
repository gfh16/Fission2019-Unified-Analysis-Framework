#include "../include/CSHINEPedestals.h"


//______________________________________________________________________________
std::string L1STag("L1S");
std::string L2FTag("L2F");
std::string L2BTag("L2B");
std::string L3ATag("L3A");

std::string Pedestal0000("Pedestal0000");
std::string L1_AlphaCali00_04("AlphaCali00_04");
std::string L1_AlphaCali05_08("AlphaCali05_08");
std::string L1_AlphaCali00_08("AlphaCali00_08");
std::string L2_AlphaCali00_32("AlphaCali00_32");
std::string L2_AlphaCali33_48("AlphaCali33_48");
std::string L2_AlphaCali00_48("AlphaCali00_48");

//______________________________________________________________________________
void Step41  (CSHINEPedestals pedestal);
void Step411 (CSHINEPedestals pedestal);
void Step412 (CSHINEPedestals pedestal);
void Step413 (CSHINEPedestals pedestal);
void Step4111(CSHINEPedestals pedestal);
void Step4112(CSHINEPedestals pedestal);
void Step4113(CSHINEPedestals pedestal);
void Step4114(CSHINEPedestals pedestal);
void Step4121(CSHINEPedestals pedestal);
void Step4122(CSHINEPedestals pedestal);
void Step4123(CSHINEPedestals pedestal);
void Step4124(CSHINEPedestals pedestal);
void Step4131(CSHINEPedestals pedestal);
void Step4132(CSHINEPedestals pedestal);
void Step4133(CSHINEPedestals pedestal);


//______________________________________________________________________________
void Step41_PedestalCali()
{
  Int_t option41  = 1;

  CSHINEPedestals pedestal;

  while (option41 == 1) {
    Step41(pedestal);
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.继续进行第一轮操作."<<endl;
    cout<<"2.结束第一轮操作！"<<endl;
    cin >>option41;
    if (option41 == 2) break;
  }
}


//______________________________________________________________________________
void Step41(CSHINEPedestals pedestal)
{
  Int_t option_step;
  Int_t option411 = 1;
  Int_t option412 = 1;
  Int_t option413 = 1;

  cout<<"****************************************************************"<<endl;
  cout<<"               欢迎来到 Step4.1_PedestalCali 界面 !               "<<endl;
  cout<<"****************************************************************"<<endl;
  cout<<endl;
  cout<<"请选择您需要进行的操作: "<<endl;
  cout<<"411.运行 AutoFindPedestals(const char* layertag, const char* filetag)   - 自动寻峰"<<endl;
  cout<<"412.运行 ClickToFindPedestals(const char* layertag, const char* filetag)- 手动拟合"<<endl;
  cout<<"413.运行 EstimateErrorsOfPedestals(const char* layertag, const char* filetag1, const char* filetag2)"<<endl;
  cout<<"================================================================"<<endl;
  cout<<"请在此输入您的选项: 411, 412, 413"<<endl;
  cin >>option_step;

  if (option_step == 411) {
    while (option411 == 1) {
      Step411(pedestal);
      cout<<"请选择下一步操作: "<<endl;
      cout<<"1.继续进行第二轮操作."<<endl;
      cout<<"2.返回上一轮操作！"<<endl;
      cin >>option411;
      if (option411 == 2) break;
    }
  } else if (option_step == 412) {
    while (option412 == 1) {
      Step412(pedestal);
      cout<<"请选择下一步操作: "<<endl;
      cout<<"1.继续进行第二轮操作."<<endl;
      cout<<"2.返回上一轮操作！"<<endl;
      cin >>option412;
      if (option412 == 2) break;
    }
  } else {
    while (option413 == 1) {
      Step413(pedestal);
      cout<<"请选择下一步操作: "<<endl;
      cout<<"1.继续进行第二轮操作."<<endl;
      cout<<"2.返回上一轮操作！"<<endl;
      cin >>option413;
      if (option413 == 2) break;
    }
  }
}


//______________________________________________________________________________
void Step411(CSHINEPedestals pedestal)
{
  Int_t option_layer;
  Int_t option4111 = 1;
  Int_t option4112 = 1;
  Int_t option4113 = 1;
  Int_t option4114 = 1;

  cout<<"********************************************"<<endl;
  cout<<"           欢迎来到 ‘自动寻峰’ 界面 ！          "<<endl;
  cout<<"********************************************"<<endl;
  cout<<endl;
  cout<<"请您选择需要对哪一层进行 Pedestal 刻度: "<<endl;
  cout<<"4111.L1S - 第一层单面硅条"<<endl;
  cout<<"4112.L1S - 第二层双面硅条的 Front 面"<<endl;
  cout<<"4113.L1S - 第二层双面硅条的 Back  面"<<endl;
  cout<<"4114.L1S - 第三层CsI"<<endl;
  cout<<"=========================================================="<<endl;
  cout<<"请在此输入您的选项: 4111, 4112, 4113, 4114"<<endl;
  cin >>option_layer;

  if (option_layer == 4111) {
    while (option4111 == 1) {
      Step4111(pedestal);
      cout<<"请选择下一步操作: "<<endl;
      cout<<"1.继续进行第三轮操作."<<endl;
      cout<<"2.返回上一轮操作！"<<endl;
      cin >>option4111;
      if (option4111 == 2) return;
    }
  } else if (option_layer == 4112) {
    while (option4112 == 1) {
      Step4112(pedestal);
      cout<<"请选择下一步操作: "<<endl;
      cout<<"1.继续进行第三轮操作."<<endl;
      cout<<"2.返回上一轮操作！"<<endl;
      cin >>option4112;
      if (option4112 == 2) return;
    }
  } else if (option_layer == 4113) {
    while (option4113 == 1) {
      Step4113(pedestal);
      cout<<"请选择下一步操作: "<<endl;
      cout<<"1.继续进行第三轮操作."<<endl;
      cout<<"2.返回上一轮操作！"<<endl;
      cin >>option4113;
      if (option4113 == 2) return;
    }
  } else {
    while (option4114 == 1) {
      Step4114(pedestal);
      cout<<"请选择下一步操作: "<<endl;
      cout<<"1.继续进行第三轮操作."<<endl;
      cout<<"2.返回上一轮操作！"<<endl;
      cin >>option4114;
      if (option4114 == 2) return;
    }
  }
}


//______________________________________________________________________________
void Step412(CSHINEPedestals pedestal)
{
  Int_t option_layer;
  Int_t option4121 = 1;
  Int_t option4122 = 1;
  Int_t option4123 = 1;
  Int_t option4124 = 1;

  cout<<"*******************************************"<<endl;
  cout<<"           欢迎来到 ‘手动寻峰’ 界面 ！         "<<endl;
  cout<<"********************************************"<<endl;
  cout<<endl;
  cout<<"请您选择需要对哪一层进行 Pedestal 刻度: "<<endl;
  cout<<"4121.L1S - 第一层单面硅条"<<endl;
  cout<<"4122.L1S - 第二层双面硅条的 Front 面"<<endl;
  cout<<"4123.L1S - 第二层双面硅条的 Back  面"<<endl;
  cout<<"4124.L1S - 第三层CsI"<<endl;
  cout<<"=========================================================="<<endl;
  cout<<"请在此输入您的选项: 4121, 4122, 4123, 4124"<<endl;
  cin >>option_layer;

  if (option_layer == 4121) {
    while (option4121 == 1) {
      Step4121(pedestal);
      cout<<"请选择下一步操作: "<<endl;
      cout<<"1.继续进行第三轮操作."<<endl;
      cout<<"2.返回上一轮操作！"<<endl;
      cin >>option4121;
      if (option4121 == 2) return;
    }
  } else if (option_layer == 4122) {
    while (option4122 == 1) {
      Step4122(pedestal);
      cout<<"请选择下一步操作: "<<endl;
      cout<<"1.继续进行第三轮操作."<<endl;
      cout<<"2.返回上一轮操作！"<<endl;
      cin >>option4122;
      if (option4122 == 2) return;
    }
  } else if (option_layer == 4123) {
    while (option4123 == 1) {
      Step4123(pedestal);
      cout<<"请选择下一步操作: "<<endl;
      cout<<"1.继续进行第三轮操作."<<endl;
      cout<<"2.返回上一轮操作！"<<endl;
      cin >>option4123;
      if (option4123 == 2) return;
    }
  } else {
    while (option4124 == 1) {
      Step4124(pedestal);
      cout<<"请选择下一步操作: "<<endl;
      cout<<"1.继续进行第三轮操作."<<endl;
      cout<<"2.返回上一轮操作！"<<endl;
      cin >>option4124;
      if (option4124 == 2) return;
    }
  }
}

//______________________________________________________________________________
void Step413(CSHINEPedestals pedestal)
{
  Int_t option_layer;
  Int_t option4131 = 1;
  Int_t option4132 = 1;
  Int_t option4133 = 1;

  cout<<"*******************************************"<<endl;
  cout<<"           欢迎来到 ‘手动寻峰’ 界面 ！         "<<endl;
  cout<<"********************************************"<<endl;
  cout<<endl;
  cout<<"请您选择需要对哪一层进行 Pedestal 刻度: "<<endl;
  cout<<"4131.L1S - 第一层单面硅条"<<endl;
  cout<<"4132.L1S - 第二层双面硅条的 Front 面"<<endl;
  cout<<"4133.L1S - 第二层双面硅条的 Back  面"<<endl;
  cout<<"4134.L1S - 第三层CsI"<<endl;
  cout<<"=========================================================="<<endl;
  cout<<"请在此输入您的选项: 4131, 4132, 4133, 4134"<<endl;
  cin >>option_layer;

  if (option_layer == 4131) {
    while (option4131 == 1) {
      Step4131(pedestal);
      cout<<"请选择下一步操作: "<<endl;
      cout<<"1.继续进行第三轮操作."<<endl;
      cout<<"2.返回上一轮操作！"<<endl;
      cin >>option4131;
      if (option4131 == 2) return;
    }
  } else if (option_layer == 4132) {
    while (option4132 == 1) {
      Step4132(pedestal);
      cout<<"请选择下一步操作: "<<endl;
      cout<<"1.继续进行第三轮操作."<<endl;
      cout<<"2.返回上一轮操作！"<<endl;
      cin >>option4132;
      if (option4132 == 2) return;
    }
  } else {
    while (option4133 == 1) {
      Step4133(pedestal);
      cout<<"请选择下一步操作: "<<endl;
      cout<<"1.继续进行第三轮操作."<<endl;
      cout<<"2.返回上一轮操作！"<<endl;
      cin >>option4133;
      if (option4133 == 2) return;
    }
  }
}

//______________________________________________________________________________
void Step4111(CSHINEPedestals pedestal)
{
  Int_t option_file;
  cout<<"请您选择对哪个文件中的 L1S 进行 Pedestal 的自动寻峰: "<<endl;
  cout<<"41111.Pedestal0000"<<endl;
  cout<<"41112.AlphaCali00_04"<<endl;
  cout<<"41113.AlphaCali05_08"<<endl;
  cout<<"41114.AlphaCali00_08"<<endl;
  cout<<"=================================================="<<endl;
  cout<<"请在此输入您的选项: 41111, 41112, 41113, 41114"<<endl;
  cin >>option_file;
  if (option_file == 41111) {
    pedestal.AutoFindPedestals(L1STag.c_str(), Pedestal0000.c_str());
  } else if (option_file == 41112) {
    pedestal.AutoFindPedestals(L1STag.c_str(), L1_AlphaCali00_04.c_str());
  } else if (option_file == 41113) {
    pedestal.AutoFindPedestals(L1STag.c_str(), L1_AlphaCali05_08.c_str());
  } else {
    pedestal.AutoFindPedestals(L1STag.c_str(), L1_AlphaCali00_08.c_str());
  }
}

//______________________________________________________________________________
void Step4112(CSHINEPedestals pedestal)
{
  Int_t option_file;
  cout<<"请您选择对哪个文件中的 L2F 进行 Pedestal 的自动寻峰: "<<endl;
  cout<<"41121.Pedestal0000"<<endl;
  cout<<"41122.AlphaCali00_32"<<endl;
  cout<<"41123.AlphaCali33_48"<<endl;
  cout<<"41124.AlphaCali00_48"<<endl;
  cout<<"=================================================="<<endl;
  cout<<"请在此输入您的选项: 41121, 41122, 41123, 41124"<<endl;
  cin >>option_file;
  if (option_file == 41121) {
    pedestal.AutoFindPedestals(L2FTag.c_str(), Pedestal0000.c_str());
  } else if (option_file == 41122) {
    pedestal.AutoFindPedestals(L2FTag.c_str(), L2_AlphaCali00_32.c_str());
  } else if (option_file == 41123) {
    pedestal.AutoFindPedestals(L2FTag.c_str(), L2_AlphaCali33_48.c_str());
  } else {
    pedestal.AutoFindPedestals(L2FTag.c_str(), L2_AlphaCali00_48.c_str());
  }
}

//______________________________________________________________________________
void Step4113(CSHINEPedestals pedestal)
{
  Int_t option_file;
  cout<<"请您选择对哪个文件中的 L2B 进行 Pedestal 的自动寻峰: "<<endl;
  cout<<"41131.Pedestal0000"<<endl;
  cout<<"41132.AlphaCali00_32"<<endl;
  cout<<"41133.AlphaCali33_48"<<endl;
  cout<<"41134.AlphaCali00_48"<<endl;
  cout<<"=================================================="<<endl;
  cout<<"请在此输入您的选项: 41131, 41132, 41133, 41134"<<endl;
  cin >>option_file;
  if (option_file == 41131) {
    pedestal.AutoFindPedestals(L2BTag.c_str(), Pedestal0000.c_str());
  } else if (option_file == 41132) {
    pedestal.AutoFindPedestals(L2BTag.c_str(), L2_AlphaCali00_32.c_str());
  } else if (option_file == 41133) {
    pedestal.AutoFindPedestals(L2BTag.c_str(), L2_AlphaCali33_48.c_str());
  } else {
    pedestal.AutoFindPedestals(L2BTag.c_str(), L2_AlphaCali00_48.c_str());
  }
}

//______________________________________________________________________________
void Step4114(CSHINEPedestals pedestal)
{
  cout<<"正在对文件 'Pedestal0000' 中的 L3A 进行 Pedestal 的自动寻峰: "<<endl;
  pedestal.AutoFindPedestals(L3ATag.c_str(), Pedestal0000.c_str());
}

//______________________________________________________________________________
void Step4121(CSHINEPedestals pedestal)
{
  Int_t option_file;
  cout<<"请您选择对哪个文件中的 L1S 进行 Pedestal 的手动拟合: "<<endl;
  cout<<"41211.Pedestal0000"<<endl;
  cout<<"41212.AlphaCali00_04"<<endl;
  cout<<"41213.AlphaCali05_08"<<endl;
  cout<<"41214.AlphaCali00_08"<<endl;
  cout<<"=================================================="<<endl;
  cout<<"请在此输入您的选项: 41211, 41212, 41213, 41214"<<endl;
  cin >>option_file;
  if (option_file == 41211) {
    pedestal.ClickToFindPedestals(L1STag.c_str(), Pedestal0000.c_str());
  } else if (option_file == 41212) {
    pedestal.ClickToFindPedestals(L1STag.c_str(), L1_AlphaCali00_04.c_str());
  } else if (option_file == 41213) {
    pedestal.ClickToFindPedestals(L1STag.c_str(), L1_AlphaCali05_08.c_str());
  } else {
    pedestal.ClickToFindPedestals(L1STag.c_str(), L1_AlphaCali00_08.c_str());
  }
}

//______________________________________________________________________________
void Step4122(CSHINEPedestals pedestal)
{
  Int_t option_file;
  cout<<"请您选择对哪个文件中的 L2F 进行 Pedestal 的手动拟合: "<<endl;
  cout<<"41221.Pedestal0000"<<endl;
  cout<<"41222.AlphaCali00_32"<<endl;
  cout<<"41223.AlphaCali33_48"<<endl;
  cout<<"41224.AlphaCali00_48"<<endl;
  cout<<"=================================================="<<endl;
  cout<<"请在此输入您的选项: 41221, 41222, 41223, 41224"<<endl;
  cin >>option_file;
  if (option_file == 41221) {
    pedestal.ClickToFindPedestals(L2FTag.c_str(), Pedestal0000.c_str());
  } else if (option_file == 41222) {
    pedestal.ClickToFindPedestals(L2FTag.c_str(), L2_AlphaCali00_32.c_str());
  } else if (option_file == 41223) {
    pedestal.ClickToFindPedestals(L2FTag.c_str(), L2_AlphaCali33_48.c_str());
  } else {
    pedestal.ClickToFindPedestals(L2FTag.c_str(), L2_AlphaCali00_48.c_str());
  }
}

//______________________________________________________________________________
void Step4123(CSHINEPedestals pedestal)
{
  Int_t option_file;
  cout<<"请您选择对哪个文件中的 L2B 进行 Pedestal 的手动拟合: "<<endl;
  cout<<"41231.Pedestal0000"<<endl;
  cout<<"41232.AlphaCali00_32"<<endl;
  cout<<"41233.AlphaCali33_48"<<endl;
  cout<<"41234.AlphaCali00_48"<<endl;
  cout<<"=================================================="<<endl;
  cout<<"请在此输入您的选项: 41231, 41232, 41233, 41234"<<endl;
  cin >>option_file;
  if (option_file == 41231) {
    pedestal.ClickToFindPedestals(L2BTag.c_str(), Pedestal0000.c_str());
  } else if (option_file == 41232) {
    pedestal.ClickToFindPedestals(L2BTag.c_str(), L2_AlphaCali00_32.c_str());
  } else if (option_file == 41233) {
    pedestal.ClickToFindPedestals(L2BTag.c_str(), L2_AlphaCali33_48.c_str());
  } else {
    pedestal.ClickToFindPedestals(L2BTag.c_str(), L2_AlphaCali00_48.c_str());
  }
}

//______________________________________________________________________________
void Step4124(CSHINEPedestals pedestal)
{
  cout<<"正在对文件 'Pedestal0000' 中的 L3A 进行 Pedestal 的手动拟合: "<<endl;
  pedestal.ClickToFindPedestals(L3ATag.c_str(), Pedestal0000.c_str());
}

//______________________________________________________________________________
void Step4131(CSHINEPedestals pedestal)
{
  Int_t option_file;
  cout<<"请您选择对两个哪个文件中的 L1S_Pedestal 进行比较: "<<endl;
  cout<<"41311.Pedestal0000   vs  AlphaCali00_04"<<endl;
  cout<<"41312.Pedestal0000   vs  AlphaCali05_08"<<endl;
  cout<<"41313.Pedestal0000   vs  AlphaCali00_08"<<endl;
  cout<<"41314.AlphaCali00_04 vs  AlphaCali05_08"<<endl;
  cout<<"=================================================="<<endl;
  cout<<"请在此输入您的选项: 41311, 41312, 41313, 41314"<<endl;
  cin >>option_file;
  if (option_file == 41311) {
    pedestal.EstimateErrorsOfPedestals(L1STag.c_str(), Pedestal0000.c_str(), L1_AlphaCali00_04.c_str());
  } else if (option_file == 41312) {
    pedestal.EstimateErrorsOfPedestals(L1STag.c_str(), Pedestal0000.c_str(), L1_AlphaCali05_08.c_str());
  } else if (option_file == 41313) {
    pedestal.EstimateErrorsOfPedestals(L1STag.c_str(), Pedestal0000.c_str(), L1_AlphaCali00_08.c_str());
  } else {
    pedestal.EstimateErrorsOfPedestals(L1STag.c_str(), L1_AlphaCali00_04.c_str(),L1_AlphaCali05_08.c_str());
  }
}

//______________________________________________________________________________
void Step4132(CSHINEPedestals pedestal)
{
  Int_t option_file;
  cout<<"请您选择对两个哪个文件中的 L2F_Pedestal 进行比较: "<<endl;
  cout<<"41321.Pedestal0000   vs  AlphaCali00_32"<<endl;
  cout<<"41322.Pedestal0000   vs  AlphaCali33_48"<<endl;
  cout<<"41323.Pedestal0000   vs  AlphaCali00_48"<<endl;
  cout<<"41324.AlphaCali00_32 vs  AlphaCali33_48"<<endl;
  cout<<"=================================================="<<endl;
  cout<<"请在此输入您的选项: 41321, 41322, 41323, 41324"<<endl;
  cin >>option_file;
  if (option_file == 41311) {
    pedestal.EstimateErrorsOfPedestals(L2FTag.c_str(), Pedestal0000.c_str(), L2_AlphaCali00_32.c_str());
  } else if (option_file == 41312) {
    pedestal.EstimateErrorsOfPedestals(L2FTag.c_str(), Pedestal0000.c_str(), L2_AlphaCali33_48.c_str());
  } else if (option_file == 41313) {
    pedestal.EstimateErrorsOfPedestals(L2FTag.c_str(), Pedestal0000.c_str(), L2_AlphaCali00_48.c_str());
  } else {
    pedestal.EstimateErrorsOfPedestals(L2FTag.c_str(), L2_AlphaCali00_32.c_str(),L2_AlphaCali33_48.c_str());
  }
}

//______________________________________________________________________________
void Step4133(CSHINEPedestals pedestal)
{
  Int_t option_file;
  cout<<"请您选择对两个哪个文件中的 L2B_Pedestal 进行比较: "<<endl;
  cout<<"41331.Pedestal0000   vs  AlphaCali00_32"<<endl;
  cout<<"41332.Pedestal0000   vs  AlphaCali33_48"<<endl;
  cout<<"41333.Pedestal0000   vs  AlphaCali00_48"<<endl;
  cout<<"41334.AlphaCali00_32 vs  AlphaCali33_48"<<endl;
  cout<<"=================================================="<<endl;
  cout<<"请在此输入您的选项: 41331, 41332, 41333, 41334"<<endl;
  cin >>option_file;
  if (option_file == 41311) {
    pedestal.EstimateErrorsOfPedestals(L2BTag.c_str(), Pedestal0000.c_str(), L2_AlphaCali00_32.c_str());
  } else if (option_file == 41312) {
    pedestal.EstimateErrorsOfPedestals(L2BTag.c_str(), Pedestal0000.c_str(), L2_AlphaCali33_48.c_str());
  } else if (option_file == 41313) {
    pedestal.EstimateErrorsOfPedestals(L2BTag.c_str(), Pedestal0000.c_str(), L2_AlphaCali00_48.c_str());
  } else {
    pedestal.EstimateErrorsOfPedestals(L2BTag.c_str(), L2_AlphaCali00_32.c_str(),L2_AlphaCali33_48.c_str());
  }
}
