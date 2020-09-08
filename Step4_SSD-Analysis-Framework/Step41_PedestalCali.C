#include "../include/CSHINEPedestals.h"


//______________________________________________________________________________
void Step41_PedestalCali()
{
  CSHINEPedestals pedestal;

  Int_t option_step;
  Int_t option_layer;
  Int_t option_file;

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

  cout<<"***********************************************"<<endl;
  cout<<"       欢迎来到 Step4.1_PedestalCali 界面 !      "<<endl;
  cout<<"***********************************************"<<endl;
  cout<<endl;
  cout<<"请选择您需要进行的操作: "<<endl;
  cout<<"411.运行 AutoFindPedestals(const char* layertag, const char* filetag)   - 自动寻峰"<<endl;
  cout<<"412.运行 ClickToFindPedestals(const char* layertag, const char* filetag)- 手动拟合"<<endl;
  cout<<"413.运行 EstimateErrorsOfPedestals(const char* layertag, const char* filetag1, const char* filetag2)"<<endl;
  cin >>option_step;

  cout<<"请您选择需要对哪一层进行 Pedestal 刻度: "<<endl;
  cout<<Form("%d1.L1S - 第一层单面硅条", option_step)<<endl;
  cout<<Form("%d2.L2F - 第二层双面硅条的 Front 面", option_step)<<endl;
  cout<<Form("%d3.L2B - 第二层双面硅条的 Back  面", option_step)<<endl;
  cout<<Form("%d4.L3A - 第三层CsI", option_step)<<endl;
  cin >>option_layer;

  switch(option_step) {
    //__________________________________________________________________________
    // 对 Pedestal 自动寻峰
    case 411:
      switch(option_layer) {
        case 4111: {  // for L1S
          cout<<"请您选择对哪个文件中的 L1S 进行 Pedestal 的自动寻峰: "<<endl;
          cout<<"41111.Pedestal0000"<<endl;
          cout<<"41112.AlphaCali00_04"<<endl;
          cout<<"41113.AlphaCali05_08"<<endl;
          cout<<"41114.AlphaCali00_08"<<endl;
          cin >>option_file;
          switch(option_file) {
            case 41111: pedestal.AutoFindPedestals(L1STag.c_str(), Pedestal0000.c_str());      break; // 跳出 option_file
            case 41112: pedestal.AutoFindPedestals(L1STag.c_str(), L1_AlphaCali00_04.c_str()); break; // 跳出 option_file
            case 41113: pedestal.AutoFindPedestals(L1STag.c_str(), L1_AlphaCali05_08.c_str()); break; // 跳出 option_file
            case 41114: pedestal.AutoFindPedestals(L1STag.c_str(), L1_AlphaCali00_08.c_str()); break; // 跳出 option_file
          }
          break; // 跳出 option_layer
        }

        case 4112: { // for L2F
          cout<<"请您选择对哪个文件中的 L2F 进行 Pedestal 的自动寻峰: "<<endl;
          cout<<"41121.Pedestal0000"<<endl;
          cout<<"41122.AlphaCali00_32"<<endl;
          cout<<"41123.AlphaCali33_48"<<endl;
          cout<<"41124.AlphaCali00_48"<<endl;
          cin >>option_file;
          switch(option_file) {
            case 41121: pedestal.AutoFindPedestals(L2FTag.c_str(), Pedestal0000.c_str());      break; // 跳出 option_file
            case 41122: pedestal.AutoFindPedestals(L2FTag.c_str(), L2_AlphaCali00_32.c_str()); break; // 跳出 option_file
            case 41123: pedestal.AutoFindPedestals(L2FTag.c_str(), L2_AlphaCali33_48.c_str()); break; // 跳出 option_file
            case 41124: pedestal.AutoFindPedestals(L2FTag.c_str(), L2_AlphaCali00_48.c_str()); break; // 跳出 option_file
          }
          break; // 跳出 option_layer
        }

        case 4113: {
          cout<<"请您选择对哪个文件中的 L2B 进行 Pedestal 的自动寻峰: "<<endl;
          cout<<"41131.Pedestal0000"<<endl;
          cout<<"41132.AlphaCali00_32"<<endl;
          cout<<"41133.AlphaCali33_48"<<endl;
          cout<<"41134.AlphaCali00_48"<<endl;
          cin >>option_file;
          switch(option_file) {
            case 41131: pedestal.AutoFindPedestals(L2BTag.c_str(), Pedestal0000.c_str());      break; // 跳出 option_file
            case 41132: pedestal.AutoFindPedestals(L2BTag.c_str(), L2_AlphaCali00_32.c_str()); break; // 跳出 option_file
            case 41133: pedestal.AutoFindPedestals(L2BTag.c_str(), L2_AlphaCali33_48.c_str()); break; // 跳出 option_file
            case 41134: pedestal.AutoFindPedestals(L2BTag.c_str(), L2_AlphaCali00_48.c_str()); break; // 跳出 option_file
          }
          break; // 跳出 option_layer
        }

        case 4114: {  // for L3A
          cout<<"正在对文件 'Pedestal0000' 中的 L3A 进行 Pedestal 的自动寻峰: "<<endl;
          pedestal.AutoFindPedestals(L3ATag.c_str(), Pedestal0000.c_str());
          break; // 跳出 option_layer
        }
        break; // 跳出 option_step
      }
    //__________________________________________________________________________

    //__________________________________________________________________________
    // 对 Pedestal 手动拟合
    case 412:
      switch(option_layer) {
        case 4121: {  // for L1S
          cout<<"请您选择对哪个文件中的 L1S 进行 Pedestal 的手动拟合: "<<endl;
          cout<<"41211.Pedestal0000"<<endl;
          cout<<"41212.AlphaCali00_04"<<endl;
          cout<<"41213.AlphaCali05_08"<<endl;
          cout<<"41214.AlphaCali00_08"<<endl;
          cin >>option_file;
          switch(option_file) {
            case 41211: pedestal.ClickToFindPedestals(L1STag.c_str(), Pedestal0000.c_str());      break; // 跳出 option_file
            case 41212: pedestal.ClickToFindPedestals(L1STag.c_str(), L1_AlphaCali00_04.c_str()); break; // 跳出 option_file
            case 41213: pedestal.ClickToFindPedestals(L1STag.c_str(), L1_AlphaCali05_08.c_str()); break; // 跳出 option_file
            case 41214: pedestal.ClickToFindPedestals(L1STag.c_str(), L1_AlphaCali00_08.c_str()); break; // 跳出 option_file
          }
          break; // 跳出 option_layer
        }

        case 4122: {  // for L2F
          cout<<"请您选择对哪个文件中的 L2F 进行 Pedestal 的手动拟合: "<<endl;
          cout<<"41221.Pedestal0000"<<endl;
          cout<<"41222.AlphaCali00_32"<<endl;
          cout<<"41223.AlphaCali33_48"<<endl;
          cout<<"41224.AlphaCali00_48"<<endl;
          cin >>option_file;
          switch(option_file) {
            case 41221: pedestal.ClickToFindPedestals(L2FTag.c_str(), Pedestal0000.c_str());      break; // 跳出 option_file
            case 41222: pedestal.ClickToFindPedestals(L2FTag.c_str(), L2_AlphaCali00_32.c_str()); break; // 跳出 option_file
            case 41223: pedestal.ClickToFindPedestals(L2FTag.c_str(), L2_AlphaCali33_48.c_str()); break; // 跳出 option_file
            case 41224: pedestal.ClickToFindPedestals(L2FTag.c_str(), L2_AlphaCali00_48.c_str()); break; // 跳出 option_file
          }
          break; // 跳出 option_layer
        }

        case 4123: {  // for L2B
          cout<<"请您选择对哪个文件中的 L2B 进行 Pedestal 的手动拟合: "<<endl;
          cout<<"41231.Pedestal0000"<<endl;
          cout<<"41232.AlphaCali00_32"<<endl;
          cout<<"41233.AlphaCali33_48"<<endl;
          cout<<"41234.AlphaCali00_48"<<endl;
          cin >>option_file;
          switch(option_file) {
            case 41231: pedestal.ClickToFindPedestals(L2BTag.c_str(), Pedestal0000.c_str());      break; // 跳出 option_file
            case 41232: pedestal.ClickToFindPedestals(L2BTag.c_str(), L2_AlphaCali00_32.c_str()); break; // 跳出 option_file
            case 41233: pedestal.ClickToFindPedestals(L2BTag.c_str(), L2_AlphaCali33_48.c_str()); break; // 跳出 option_file
            case 41234: pedestal.ClickToFindPedestals(L2BTag.c_str(), L2_AlphaCali00_48.c_str()); break; // 跳出 option_file
          }
          break; // 跳出 option_layer
        }

        case 4124: {
          cout<<"正在对文件 'Pedestal0000' 中的 L3A 进行 Pedestal 的手动拟合: "<<endl;
          pedestal.ClickToFindPedestals(L3ATag.c_str(), Pedestal0000.c_str());
          break; // 跳出 option_layer
        }
        break; // 跳出 option_step
      }
    //__________________________________________________________________________

    //__________________________________________________________________________
    // 比较不同文件之间的 pedestal 的差异
    case 413:
      switch(option_layer) {
        case 4131: {
          cout<<"请您选择对两个哪个文件中的 L1S_Pedestal 进行比较: "<<endl;
          cout<<"41311.Pedestal0000   vs  AlphaCali00_04"<<endl;
          cout<<"41312.Pedestal0000   vs  AlphaCali05_08"<<endl;
          cout<<"41313.Pedestal0000   vs  AlphaCali00_08"<<endl;
          cout<<"41314.AlphaCali00_04 vs  AlphaCali05_08"<<endl;
          cin >>option_file;
          switch(option_file) {
            case 41311: pedestal.EstimateErrorsOfPedestals(L1STag.c_str(), Pedestal0000.c_str(), L1_AlphaCali00_04.c_str()); break; // 跳出 option_file
            case 41312: pedestal.EstimateErrorsOfPedestals(L1STag.c_str(), Pedestal0000.c_str(), L1_AlphaCali05_08.c_str()); break; // 跳出 option_file
            case 41313: pedestal.EstimateErrorsOfPedestals(L1STag.c_str(), Pedestal0000.c_str(), L1_AlphaCali00_08.c_str()); break; // 跳出 option_file
            case 41314: pedestal.EstimateErrorsOfPedestals(L1STag.c_str(), L1_AlphaCali00_04.c_str(),L1_AlphaCali05_08.c_str()); break; // 跳出 option_file
          }
          break; // 跳出 option_layer
        }

        case 4132: {
          cout<<"请您选择对两个哪个文件中的 L2F_Pedestal 进行比较: "<<endl;
          cout<<"41321.Pedestal0000   vs  AlphaCali00_32"<<endl;
          cout<<"41322.Pedestal0000   vs  AlphaCali33_48"<<endl;
          cout<<"41323.Pedestal0000   vs  AlphaCali00_48"<<endl;
          cout<<"41324.AlphaCali00_32 vs  AlphaCali33_48"<<endl;
          cin >>option_file;
          switch(option_file) {
            case 41321: pedestal.EstimateErrorsOfPedestals(L2FTag.c_str(), Pedestal0000.c_str(), L2_AlphaCali00_32.c_str()); break; // 跳出 option_file
            case 41322: pedestal.EstimateErrorsOfPedestals(L2FTag.c_str(), Pedestal0000.c_str(), L2_AlphaCali33_48.c_str()); break; // 跳出 option_file
            case 41323: pedestal.EstimateErrorsOfPedestals(L2FTag.c_str(), Pedestal0000.c_str(), L2_AlphaCali00_48.c_str()); break; // 跳出 option_file
            case 41324: pedestal.EstimateErrorsOfPedestals(L2FTag.c_str(), L2_AlphaCali00_32.c_str(),L2_AlphaCali33_48.c_str()); break; // 跳出 option_file
          }
          break; // 跳出 option_layer
        }

        case 4133: {
          cout<<"请您选择对两个哪个文件中的 L2B_Pedestal 进行比较: "<<endl;
          cout<<"41331.Pedestal0000   vs  AlphaCali00_32"<<endl;
          cout<<"41332.Pedestal0000   vs  AlphaCali33_48"<<endl;
          cout<<"41333.Pedestal0000   vs  AlphaCali00_48"<<endl;
          cout<<"41334.AlphaCali00_32 vs  AlphaCali33_48"<<endl;
          cin >>option_file;
          switch(option_file) {
            case 41331: pedestal.EstimateErrorsOfPedestals(L2BTag.c_str(), Pedestal0000.c_str(),  L2_AlphaCali00_32.c_str()); break; // 跳出 option_file
            case 41332: pedestal.EstimateErrorsOfPedestals(L2BTag.c_str(), Pedestal0000.c_str(),  L2_AlphaCali33_48.c_str()); break; // 跳出 option_file
            case 41333: pedestal.EstimateErrorsOfPedestals(L2BTag.c_str(), Pedestal0000.c_str(),  L2_AlphaCali00_48.c_str()); break; // 跳出 option_file
            case 41334: pedestal.EstimateErrorsOfPedestals(L2BTag.c_str(), L2_AlphaCali00_32.c_str(),L2_AlphaCali33_48.c_str()); break; // 跳出 option_file
          }
          break; // 跳出 option_layer
        }
        break; // 跳出 option_step
      }
  }
}
