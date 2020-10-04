#include "../include/CSHINEPulserCali.h"
#include "../include/CSHINEAlphaCali.h"
#include "../include/CSHINESiEnergyCali.h"


void Step421_PulserCali();
void Step422_AlphaCali();
void Step423_SiEnergyCali();

void InputPuserTag(Int_t opt, std::string& pulserfiletag);
void InputLayerTag(Int_t opt_layer, std::string& layertag);
void InputLayerAndAlphaTag (Int_t opt_layer,std::string& layertag,Int_t opt_alphafile, std::string& alphafiletag);

//______________________________________________________________________________
void Step42_SiEnergyCali()
{
  Int_t option42 = 1;
  Int_t option_step42;

  cout<<"***********************************************"<<endl;
  cout<<"      欢迎来到 Step42_SiEnergyCali 界面 !        "<<endl;
  cout<<"***********************************************"<<endl;
  cout<<endl;
  cout<<"请选择您需要进行的操作: "<<endl;
  cout<<"421 CSHINEPulserCali() -- 脉冲刻度 "<<endl;
  cout<<"422 CSHINEAlphaCali    -- 放射源刻度"<<endl;
  cout<<"423 CSHINESiEnergyCali -- 硅条能量线性刻度"<<endl;

  while (option42 == 1) {
    cout<<"======================================="<<endl;
    cout<<"请在此输入您的选项: 421, 422, 423"<<endl;
    cin >>option_step42;
    if (option_step42 == 421) Step421_PulserCali();
    if (option_step42 == 422) Step422_AlphaCali();
    if (option_step42 == 423) Step423_SiEnergyCali();
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step42,重新进行操作选择"<<endl;
    cout<<"2.操作完成, 结束程序"<<endl;
    cin >> option42;
    if (option42 == 2) {
      cout<<"***********************************************"<<endl;
      cout<<"          Exit Step42_SiEnergyCali!            "<<endl;
      cout<<"***********************************************"<<endl;
      return;
    }
  }
}
//______________________________________________________________________________


//______________________________________________________________________________
void Step421_PulserCali()
{
  Int_t option421 = 1;
  Int_t option_step421;
  Int_t option_pulser;
  Int_t option_gain;
  Int_t option_layer;

  std::string PulserFileTag;
  std::string GainTag;
  std::string LayerTag;
  std::string LowGain("LowGain");
  std::string HighGain("HighGain");
  std::string L1STag("L1S");

  CSHINEPulserCali pulsercali;

  cout<<"***************************************************************"<<endl;
  cout<<"           您现在进行硅条的脉冲刻度  Step421_PulserCali            "<<endl;
  cout<<"***************************************************************"<<endl;
  cout<<endl;
  cout<<"请选择您需要进行的操作: "<<endl;
  printf("4211  L1_AutoFindPeaksAndFit()          -- L1自动寻峰\n");
  printf("4212  L2_AutoFindPeaksAndFit()          -- L2自动寻峰\n");
  printf("4213  L1_AutoFindPeaksAndFit_ReCali()   -- L1重新刻度\n");
  printf("4214  L1_GainEffectOnPulseCali_ReCali() -- L1前放放大倍数效应\n");
  printf("4215  CaliCheck_SSD_L2F_CH00_01()       -- L2F检查CH00_01\n");
  printf("4216  EstimateErrorOfHightAndSwitch()   -- Hight和Switch偏差\n");
  printf("4217  CheckLinearity()                  -- 检查脉冲的线性\n");

  while (option421 == 1) {
    cout<<"============================================================="<<endl;
    cout<<"请在此输入您的选项: 4211, 4212, 4213, 4214, 4215, 4216, 4217"<<endl;
    cin >>option_step421;
    if (option_step421 == 4211){           //4211  L1自动寻峰
      InputPuserTag(option_pulser,PulserFileTag);
      pulsercali.L1_AutoFindPeaksAndFit(PulserFileTag.c_str());

    } else if (option_step421 == 4212) {   //4212  L2自动寻峰
      InputPuserTag(option_pulser,PulserFileTag);
      pulsercali.L2_AutoFindPeaksAndFit(PulserFileTag.c_str());

    } else if (option_step421 == 4213) {   //4213  L1重新刻度
      InputPuserTag(option_pulser,PulserFileTag);
      cout<<"请选择前放放大倍数: "<<endl;
      cout<<"1.LowGain"<<endl;
      cout<<"2.HighGain"<<endl;
      cout<<"====================="<<endl;
      cout<<"请输入您的选项:"<<endl;
      cin >>option_gain;
      if (option_gain == 1) GainTag = LowGain;
      if (option_gain == 2) GainTag = HighGain;
      pulsercali.L1_AutoFindPeaksAndFit_ReCali(PulserFileTag.c_str(),GainTag.c_str());

    } else if (option_step421 == 4214) {  //4214  L1前放放大倍数效应
      InputPuserTag(option_pulser,PulserFileTag);
      pulsercali.L1_GainEffectOnPulseCali_ReCali(PulserFileTag.c_str());

    } else if (option_step421 == 4215) {  //4215  L2F检查CH00_01
      pulsercali.CaliCheck_SSD_L2F_CH00_01();

    } else if (option_step421 == 4216) {  //4216  Hight和Switch偏差
      pulsercali.EstimateErrorOfHightAndSwitch();

    } else if (option_step421 == 4217) {  //4217  检查脉冲的线性
      InputLayerTag(option_layer, LayerTag);
      InputPuserTag(option_pulser,PulserFileTag);
      pulsercali.CheckLinearity(LayerTag.c_str(),PulserFileTag.c_str());
    } else {
      cout<<"输入有误!"<<endl;
    }
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step421,重新进行操作选择"<<endl;
    cout<<"2.操作完成, 结束程序"<<endl;
    cin >> option421;
    if (option421 == 2) {
      cout<<"***********************************************"<<endl;
      cout<<"          Exit Step421_PulserCali!             "<<endl;
      cout<<"***********************************************"<<endl;
      return;
    }
  }
}
//______________________________________________________________________________


//______________________________________________________________________________
void Step422_AlphaCali()
{
  Int_t option422 = 1;
  Int_t option_step422;
  Int_t option_layer;
  Int_t option_alphafile;
  Int_t firstrun;
  Int_t lastrun;

  std::string LayerTag;
  std::string AlphaFileTag;

  CSHINEAlphaCali alphacali;

  cout<<"***************************************************************"<<endl;
  cout<<"           您现在进行硅条的脉冲刻度  Step422_AlphaCali             "<<endl;
  cout<<"***************************************************************"<<endl;
  cout<<endl;
  cout<<"请选择您需要进行的操作: "<<endl;
  printf("4221  CalculateAlphaEnergy()        -- alpha 能量修正计算\n");
  printf("4222  MergeAlphaCaliFiles()         -- 合并 alpha 刻度文件\n");
  printf("4223  ClickToFindAlphaPeaks()       -- 手动拟合 alpha 峰\n");
  printf("4224  CheckAbnormaleChannels_SSD4() -- 检查异常信号\n");

  while (option422 == 1) {
    cout<<"================================================="<<endl;
    cout<<"请在此输入您的选项: 4221, 4222, 4223, 4224"<<endl;
    cin >>option_step422;
    if (option_step422 == 4221) {
      alphacali.CalculateAlphaEnergy();

    } else if (option_step422 == 4222) {
      InputLayerTag(option_layer, LayerTag);
      cout<<"L1:第一次刻度0-4,第二次刻度5-8; L2:第一次刻度0-32,第二次刻度33-48"<<endl;
      cout<<"请输入起始文件: L1:0-8, L2:0-48 "<<endl;
      cin >>firstrun;
      cout<<"请输入终止文件: L1:0-8, L2:0-48"<<endl;
      cin >>lastrun;
      alphacali.MergeAlphaCaliFiles(LayerTag.c_str(), firstrun, lastrun);

    } else if (option_step422 == 4223) {
      InputLayerAndAlphaTag(option_layer, LayerTag, option_alphafile, AlphaFileTag);
      alphacali.ClickToFindAlphaPeaks(LayerTag.c_str(), AlphaFileTag.c_str());

    } else {
      alphacali.AlphaCali_SSD4_L1S_CompareAlphaCali00_04And05_08();
    }
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step422,重新进行操作选择"<<endl;
    cout<<"2.操作完成, 结束程序"<<endl;
    cin >> option422;
    if (option422 == 2) {
      cout<<"***********************************************"<<endl;
      cout<<"          Exit Step422_AlphaCali!              "<<endl;
      cout<<"***********************************************"<<endl;
      return;
    }
  }
}
//______________________________________________________________________________


//______________________________________________________________________________
void Step423_SiEnergyCali()
{
  Int_t option423 = 1;
  Int_t option_step423;
  Int_t option_layer;
  Int_t option_pulserfile;

  std::string LayerTag;
  std::string PulserFileTag;

  CSHINESiEnergyCali energycali;

  cout<<"***************************************************************"<<endl;
  cout<<"          您现在进行硅条的脉冲刻度  Step423_SiEnergyCali           "<<endl;
  cout<<"***************************************************************"<<endl;
  cout<<endl;
  cout<<"请选择您需要进行的操作: "<<endl;
  printf("4231  PulserAndAlphaCali()               -- 硅条能量刻度曲线\n");
  printf("4232  EstimatePedestals()                -- 考察刻度曲线与pedestal\n");
  printf("4233  EstimateFitPars()                  -- 初步评估刻度参数\n");
  printf("4234  CheckAbnormalChannels()            -- 检查异常信号\n");
  printf("4235  EstimateDeadlayerEffects()         -- 考察死层效应\n");
  printf("4236  EstimateAlphaChannelEffOnFitPars() -- 考察 alpha 道址效应\n");
  printf("4237  FinallyDeterminedFitPars()         -- 确定最终刻度参数\n");

  while (option423 == 1) {
    cout<<"========================================================="<<endl;
    cout<<"请在此输入您的选项: 4231, 4232, 4233, 4234, 4235, 4236, 4237"<<endl;
    cin >>option_step423;
    if (option_step423 == 4231) {
      InputLayerTag(option_layer, LayerTag);
      InputPuserTag(option_pulserfile, PulserFileTag);
      energycali.PulserAndAlphaCali(LayerTag.c_str(),PulserFileTag.c_str());

    } else if (option_step423 == 4232) {
      InputLayerTag(option_layer, LayerTag);
      InputPuserTag(option_pulserfile, PulserFileTag);
      energycali.EstimatePedestals(LayerTag.c_str(),PulserFileTag.c_str());

    } else if (option_step423 == 4233) {
      InputLayerTag(option_layer, LayerTag);
      InputPuserTag(option_pulserfile, PulserFileTag);
      energycali.EstimateFitPars(LayerTag.c_str(),PulserFileTag.c_str());

    } else if (option_step423 == 4234) {
      InputPuserTag(option_pulserfile, PulserFileTag);
      energycali.CheckAbnormalChannels(PulserFileTag.c_str());

    } else if (option_step423 == 4235) {
      InputPuserTag(option_pulserfile, PulserFileTag);
      energycali.CheckAbnormalChannels(PulserFileTag.c_str());

    } else if (option_step423 == 4236) {
      InputLayerTag(option_layer, LayerTag);
      InputPuserTag(option_pulserfile, PulserFileTag);
      energycali.EstimateDeadlayerEffects(LayerTag.c_str(),PulserFileTag.c_str());

    } else {
      InputLayerTag(option_layer, LayerTag);
      InputPuserTag(option_pulserfile, PulserFileTag);
      energycali.FinallyDeterminedFitPars(LayerTag.c_str(),PulserFileTag.c_str());
    }
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step423,重新进行操作选择"<<endl;
    cout<<"2.操作完成, 结束程序"<<endl;
    cin >> option423;
    if (option423 == 2) {
      cout<<"***********************************************"<<endl;
      cout<<"          Exit Step423_SiEnergyCali!             "<<endl;
      cout<<"***********************************************"<<endl;
      return;
    }
  }
}
//______________________________________________________________________________


//______________________________________________________________________________
void InputPuserTag(Int_t opt, std::string& pulserfiletag)
{
  std::string Height("Height");
  std::string Switch("Switch");
  cout<<"请选择脉冲刻度方式: 1.Height(等幅度法), 2.Switch(拨档法)"<<endl;
  cin >>opt;
  if (opt == 1) pulserfiletag = Height;
  if (opt == 2) pulserfiletag = Switch;
  return;
}
//___________________________________________________________
void InputLayerTag(Int_t opt_layer, std::string& layertag)
{
  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  cout<<"请选择对哪一层硅条进行处理: "<<endl;
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
void InputLayerAndAlphaTag(Int_t opt_layer,std::string& layertag,Int_t opt_alphafile,std::string& alphafiletag)
{
  std::string L1STag("L1S");
  std::string AlphaCali00_04("AlphaCali00_04");
  std::string AlphaCali05_08("AlphaCali05_08");
  std::string AlphaCali00_08("AlphaCali00_08");
  std::string AlphaCali00_32("AlphaCali00_32");
  std::string AlphaCali33_48("AlphaCali33_48");
  std::string AlphaCali00_48("AlphaCali00_48");

  InputLayerTag(opt_layer, layertag);

  if (strcmp(layertag.c_str(), L1STag.c_str())==0) {
    cout<<"请选择 L1 的 alpha 合并文件: "<<endl;
    cout<<"1.AlphaCali00_04"<<endl;
    cout<<"2.AlphaCali05_08"<<endl;
    cout<<"3.AlphaCali00_08"<<endl;
    cout<<"================================="<<endl;
    cout<<"请在此输入您的选择: "<<endl;
    cin >>opt_alphafile;
    if (opt_alphafile == 1) alphafiletag = AlphaCali00_04;
    if (opt_alphafile == 2) alphafiletag = AlphaCali05_08;
    if (opt_alphafile == 3) alphafiletag = AlphaCali00_08;
  } else {
    cout<<"请选择 L2 的 alpha 合并文件: "<<endl;
    cout<<"1.AlphaCali00_32"<<endl;
    cout<<"2.AlphaCali33_48"<<endl;
    cout<<"3.AlphaCali00_48"<<endl;
    cout<<"==================================="<<endl;
    cout<<"请在此输入您的选择: "<<endl;
    cin >>opt_alphafile;
    if (opt_alphafile == 1) alphafiletag = AlphaCali00_32;
    if (opt_alphafile == 2) alphafiletag = AlphaCali33_48;
    if (opt_alphafile == 3) alphafiletag = AlphaCali00_48;
  }
}
