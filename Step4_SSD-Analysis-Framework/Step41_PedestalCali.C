#include "../include/CSHINEPedestals.h"


//______________________________________________________________________________
void Step411_AutoFindPedestals();
void Step412_ClickToFindPedestals();
void Step413_EstimateErrorsOfPedestals();

void InputLayerTag(Int_t opt_layer, std::string& layertag);
void InputLayerAndFileTag(Int_t opt_layer, std::string& layertag,Int_t opt_file,std::string& filetag);
void InputLayerAngTwoFiles(Int_t opt_layer,std::string& layertag,Int_t opt_file,std::string& filetag1,std::string& filetag2);


//______________________________________________________________________________
void Step41_PedestalCali()
{
  Int_t option41 = 1;
  Int_t option_step41;

  cout<<"****************************************************************"<<endl;
  cout<<"               欢迎来到 Step41_PedestalCali 界面 !               "<<endl;
  cout<<"****************************************************************"<<endl;
  cout<<endl;
  while (option41 == 1) {
    cout<<"请选择您需要进行的操作: "<<endl;
    cout<<"============================================================="<<endl;
    cout<<"411.AutoFindPedestals()         -- 自动寻峰"<<endl;
    cout<<"412.ClickToFindPedestals()      -- 手动拟合"<<endl;
    cout<<"413.EstimateErrorsOfPedestals() -- 评估不同刻度文件的pedestal差异"<<endl;
    cout<<"============================================================="<<endl;
    cout<<"请在此输入您的选项: 411, 412, 413"<<endl;
    cin >>option_step41;

    if (option_step41 == 411)  Step411_AutoFindPedestals();
    if (option_step41 == 412)  Step412_ClickToFindPedestals();
    if (option_step41 == 413)  Step413_EstimateErrorsOfPedestals();

    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step41,重新进行操作选择"<<endl;
    cout<<"2.操作完成, 结束程序"<<endl;
    cin >> option41;
    if (option41 == 2) {
      cout<<"***********************************************"<<endl;
      cout<<"          Exit Step41_PedestalCali!            "<<endl;
      cout<<"***********************************************"<<endl;
      return;
    }
  }
}

//______________________________________________________________________________
void Step411_AutoFindPedestals()
{
  Int_t option411 = 1;
  Int_t option_step411;
  Int_t option_layer;
  Int_t option_file;

  std::string LayerTag;
  std::string FileTag;

  CSHINEPedestals pedestal;

  cout<<"****************************************************************"<<endl;
  cout<<"     您正在进行 Pedestals 自动寻峰  Step411_AutoFindPedestals ！    "<<endl;
  cout<<"****************************************************************"<<endl;
  cout<<endl;
  while (option411 == 1) {
    InputLayerAndFileTag(option_layer, LayerTag, option_file, FileTag);
    pedestal.AutoFindPedestals(LayerTag.c_str(), FileTag.c_str());
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step411,重新进行操作选择"<<endl;
    cout<<"2.操作完成, 结束程序"<<endl;
    cin >>option411;
    if (option411 == 2) {
      cout<<"*************************************************"<<endl;
      cout<<"          Exit Step411_AutoFindPedestals!        "<<endl;
      cout<<"*************************************************"<<endl;
      return;
    }
  }
}

//______________________________________________________________________________
void Step412_ClickToFindPedestals()
{
  Int_t option412 = 1;
  Int_t option_step412;
  Int_t option_layer;
  Int_t option_file;

  std::string LayerTag;
  std::string FileTag;

  CSHINEPedestals pedestal;

  cout<<"****************************************************************"<<endl;
  cout<<"    您正在进行Pedestals自动寻峰  Step412_ClickToFindPedestals ！   "<<endl;
  cout<<"****************************************************************"<<endl;
  cout<<endl;
  while (option412 == 1) {
    InputLayerAndFileTag(option_layer, LayerTag, option_file, FileTag);
    pedestal.ClickToFindPedestals(LayerTag.c_str(), FileTag.c_str());
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step412,重新进行操作选择"<<endl;
    cout<<"2.操作完成, 结束程序"<<endl;
    cin >> option412;
    if (option412 == 2) {
      cout<<"****************************************************"<<endl;
      cout<<"          Exit Step412_ClickToFindPedestals!        "<<endl;
      cout<<"****************************************************"<<endl;
      return;
    }
  }
}

//______________________________________________________________________________
void Step413_EstimateErrorsOfPedestals()
{
  Int_t option413 = 1;
  Int_t option_step413;
  Int_t option_layer;
  Int_t option_file;

  std::string LayerTag;
  std::string FileTag1;
  std::string FileTag2;

  CSHINEPedestals pedestal;

  cout<<"*******************************************************************"<<endl;
  cout<<"   您正在进行Pedestals差异评估  Step413_EstimateErrorsOfPedestals !   "<<endl;
  cout<<"*******************************************************************"<<endl;
  cout<<endl;
  while (option413 == 1) {
    InputLayerAngTwoFiles(option_layer, LayerTag, option_file, FileTag1, FileTag2);
    pedestal.EstimateErrorsOfPedestals(LayerTag.c_str(), FileTag1.c_str(), FileTag2.c_str());
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step413,重新进行操作选择"<<endl;
    cout<<"2.操作完成, 结束程序"<<endl;
    cin >> option413;
    if (option413 == 2) {
      cout<<"**********************************************************"<<endl;
      cout<<"          Exit Step413_EstimateErrorsOfPedestals!         "<<endl;
      cout<<"**********************************************************"<<endl;
      return;
    }
  }
}

//___________________________________________________________
void InputLayerTag(Int_t opt_layer, std::string& layertag)
{
  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  cout<<"请选择对哪一层硅条进行处理: "<<endl;
  cout<<"===================="<<endl;
  cout<<"1.L1S"<<endl;
  cout<<"2.L2F"<<endl;
  cout<<"3.L2B"<<endl;
  cout<<"====================="<<endl;
  cout<<"请输入您的选项:"<<endl;
  cin >>opt_layer;
  if (opt_layer == 1) layertag = L1STag;
  if (opt_layer == 2) layertag = L2FTag;
  if (opt_layer == 3) layertag = L2BTag;
}

//______________________________________________________________________________
void InputLayerAndFileTag(Int_t opt_layer, std::string& layertag, Int_t opt_file, std::string& filetag)
{
  std::string L1STag("L1S");
  std::string Pedestal0000  ("Pedestal0000");
  std::string AlphaCali00_04("AlphaCali00_04");
  std::string AlphaCali05_08("AlphaCali05_08");
  std::string AlphaCali00_08("AlphaCali00_08");
  std::string AlphaCali00_32("AlphaCali00_32");
  std::string AlphaCali33_48("AlphaCali33_48");
  std::string AlphaCali00_48("AlphaCali00_48");

  InputLayerTag(opt_layer, layertag);

  if (strcmp(layertag.c_str(), L1STag.c_str())==0) {
    cout<<"请选择对 L1 的哪个文件进行处理: "<<endl;
    cout<<"================================"<<endl;
    cout<<"1.Pedestal0000"  <<endl;
    cout<<"2.AlphaCali00_04"<<endl;
    cout<<"3.AlphaCali05_08"<<endl;
    cout<<"4.AlphaCali00_08"<<endl;
    cout<<"================================="<<endl;
    cout<<"请在此输入您的选择: "<<endl;
    cin >>opt_file;
    if (opt_file == 1) filetag = Pedestal0000;
    if (opt_file == 2) filetag = AlphaCali00_04;
    if (opt_file == 3) filetag = AlphaCali05_08;
    if (opt_file == 4) filetag = AlphaCali00_08;
  } else {
    cout<<"请选择对 L2 的哪个文件进行处理: "<<endl;
    cout<<"================================="<<endl;
    cout<<"1.Pedestal0000"  <<endl;
    cout<<"2.AlphaCali00_32"<<endl;
    cout<<"3.AlphaCali33_48"<<endl;
    cout<<"4.AlphaCali00_48"<<endl;
    cout<<"================================="<<endl;
    cout<<"请在此输入您的选择: "<<endl;
    cin >>opt_file;
    if (opt_file == 1) filetag = Pedestal0000;
    if (opt_file == 2) filetag = AlphaCali00_04;
    if (opt_file == 3) filetag = AlphaCali05_08;
    if (opt_file == 4) filetag = AlphaCali00_08;
  }
}

//______________________________________________________________________________
void InputLayerAngTwoFiles(Int_t opt_layer, std::string& layertag,
  Int_t opt_file, std::string& filetag1, std::string& filetag2)
{
  std::string L1STag("L1S");
  std::string Pedestal0000  ("Pedestal0000");
  std::string AlphaCali00_04("AlphaCali00_04");
  std::string AlphaCali05_08("AlphaCali05_08");
  std::string AlphaCali00_08("AlphaCali00_08");
  std::string AlphaCali00_32("AlphaCali00_32");
  std::string AlphaCali33_48("AlphaCali33_48");
  std::string AlphaCali00_48("AlphaCali00_48");

  InputLayerTag(opt_layer, layertag);

  if (strcmp(layertag.c_str(), L1STag.c_str())==0) {
    cout<<"请选择对 L1 的哪组文件进行比较: "<<endl;
    cout<<"========================================"<<endl;
    cout<<"1. Pedestal0000   vs  AlphaCali00_04"    <<endl;
    cout<<"2. Pedestal0000   vs  AlphaCali05_08"    <<endl;
    cout<<"3. Pedestal0000   vs  AlphaCali00_08"    <<endl;
    cout<<"4. AlphaCali00_04 vs  AlphaCali05_08"    <<endl;
    cout<<"========================================"<<endl;
    cout<<"请在此输入您的选择: "<<endl;
    cin >>opt_file;
    if (opt_file == 1) { filetag1 = Pedestal0000;   filetag2 = AlphaCali00_04; }
    if (opt_file == 2) { filetag1 = Pedestal0000;   filetag2 = AlphaCali05_08; }
    if (opt_file == 3) { filetag1 = Pedestal0000;   filetag2 = AlphaCali00_08; }
    if (opt_file == 4) { filetag1 = AlphaCali00_04; filetag2 = AlphaCali05_08; }
  } else {
    cout<<"请选择对 L2 的哪组文件进行比较: "<<endl;
    cout<<"========================================"<<endl;
    cout<<"1. Pedestal0000   vs  AlphaCali00_32"    <<endl;
    cout<<"2. Pedestal0000   vs  AlphaCali33_48"    <<endl;
    cout<<"3. Pedestal0000   vs  AlphaCali00_48"    <<endl;
    cout<<"4. AlphaCali00_32 vs  AlphaCali33_48"    <<endl;
    cout<<"========================================"<<endl;
    cout<<"请在此输入您的选择: "<<endl;
    cin >>opt_file;
    if (opt_file == 1) { filetag1 = Pedestal0000;   filetag2 = AlphaCali00_32; }
    if (opt_file == 2) { filetag1 = Pedestal0000;   filetag2 = AlphaCali33_48; }
    if (opt_file == 3) { filetag1 = Pedestal0000;   filetag2 = AlphaCali00_48; }
    if (opt_file == 4) { filetag1 = AlphaCali00_32; filetag2 = AlphaCali33_48; }
  }
}
