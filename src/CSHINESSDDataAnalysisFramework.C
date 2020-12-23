#include "../include/CSHINESSDDataAnalysisFramework.h"


//******************************************************************************
CSHINESSDDataAnalysisFramework::CSHINESSDDataAnalysisFramework()
{
  cout<<"Enetr class CSHINESSDDataAnalysisFramework !" <<endl;
}

CSHINESSDDataAnalysisFramework::~CSHINESSDDataAnalysisFramework()
{
  cout<<"Exit class CSHINESSDDataAnalysisFramework !" <<endl;
}
//******************************************************************************


//******************************************************************************
void CSHINESSDDataAnalysisFramework::Step4_SSDDataAnalysisFramework()
{
  Int_t option4      = 1;
  Int_t option_step4 = 0;

  cout<<"****************************************************************"<<endl;
  cout<<"          欢迎来到 Step4_SSDDataAnalysisFramework 界面 !          "<<endl;
  cout<<"****************************************************************"<<endl;
  cout<<endl;
  while (option4 == 1) {
    cout<<"请选择您需要进行的操作: "<<endl;
    cout<<"============================================================="<<endl;
    cout<<"41.Step41_PedestalCali()           --  Pedestal 刻度"<<endl;
    cout<<"42.Step42_SiEnergyCali()           --  硅条能量刻度"<<endl;
    cout<<"43.Step43_SiPixellation()          --  硅条的像素化"<<endl;
    cout<<"44.Step44_TrackReconstruction()  --  SSD 径迹重建"<<endl;
    cout<<"45.Step45_CsIEnergyCali()          --  CsI 能量刻度"<<endl;
    cout<<"46.Step46_ParticleIdentification() --  PID"<<endl;
    cout<<"47.Step47_EventTree()              --  最终数据存储"<<endl;
    cout<<"============================================================="<<endl;
    cout<<"请在此输入您的选项: 41, 42, 43, 44, 45, 46, 47"<<endl;
    cin >>option_step4;

    if (option_step4 == 41)  Step41_PedestalCali();
    if (option_step4 == 42)  Step42_SiEnergyCali();
    if (option_step4 == 43)  Step43_SiPixellation();
    if (option_step4 == 44)  Step44_TrackReconstruction();
    if (option_step4 == 45)  Step45_CsIEnergyCali();
    if (option_step4 == 46)  Step46_ParticleIdentification();
    if (option_step4 == 47)  Step47_EventTree();

    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step4, 重新进行操作选择"<<endl;
    cout<<"2.操作完成, 结束程序"<<endl;
    cin >> option4;
    if (option4 == 2) {
      cout<<"*****************************************************"<<endl;
      cout<<"         Exit Step4_SSDDataAnalysisFramework!        "<<endl;
      cout<<"*****************************************************"<<endl;
      return;
    }
  }
}

//______________________________________________________________________________
void CSHINESSDDataAnalysisFramework::Step41_PedestalCali()
{
  Int_t option41      = 1;
  Int_t option_step41 = 0;

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
    cout<<"1.返回 Step41, 重新进行操作选择"<<endl;
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
void CSHINESSDDataAnalysisFramework::Step42_SiEnergyCali()
{
  Int_t option42      = 1;
  Int_t option_step42 = 0;

  cout<<"***********************************************"<<endl;
  cout<<"      欢迎来到 Step42_SiEnergyCali 界面 !        "<<endl;
  cout<<"***********************************************"<<endl;
  cout<<endl;
  while (option42 == 1) {
    cout<<"请选择您需要进行的操作: "<<endl;
    cout<<"======================================="<<endl;
    cout<<"421 CSHINEPulserCali() -- 脉冲刻度 "<<endl;
    cout<<"422 CSHINEAlphaCali    -- 放射源刻度"<<endl;
    cout<<"423 CSHINESiEnergyCali -- 硅条能量线性刻度"<<endl;
    cout<<"======================================="<<endl;
    cout<<"请在此输入您的选项: 421, 422, 423"<<endl;
    cin >>option_step42;
    if (option_step42 == 421)  Step421_PulserCali();
    if (option_step42 == 422)  Step422_AlphaCali();
    if (option_step42 == 423)  Step423_SiEnergyCali();

    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step42, 重新进行操作选择"<<endl;
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
void CSHINESSDDataAnalysisFramework::Step43_SiPixellation()
{
  Int_t option43      = 1;
  Int_t option_step43 = 0;

  cout<<"********************************************"<<endl;
  cout<<"     欢迎来到 Step43_SiPixellation 界面 !    "<<endl;
  cout<<"********************************************"<<endl;
  cout<<endl;
  while (option43 == 1) {
    cout<<"请选择您需要进行的操作: "<<endl;
    cout<<"============================================================="<<endl;
    cout<<"431.运行 CalculateEffectiveThickness() - 计算每个双面硅条 pixel 的等效厚度"<<endl;
    cout<<"432.运行 SiPixellation()               - 计算每个像素点的 theta, phi"<<endl;
    cout<<"433.运行 CheckSiPixellation()          - 检查像素化结果是否正确"<<endl;
    cout<<"============================================================="<<endl;
    cout<<"请在此输入您的选项: 431, 432, 433"<<endl;
    cin >>option_step43;
    if (option_step43 == 431)  pixel.CalculateEffectiveThickness();
    if (option_step43 == 432)  pixel.SiPixellation();
    if (option_step43 == 433)  pixel.CheckSiPixellation();

    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step43, 重新进行操作选择"<<endl;
    cout<<"2.结束操作, 退出程序！"<<endl;
    cin >>option43;
    if (option43 == 2) {
      cout<<"***********************************************"<<endl;
      cout<<"          Exit Step43_SiPixellation!          "<<endl;
      cout<<"***********************************************"<<endl;
      return;
    }
  }
}

//______________________________________________________________________________
void CSHINESSDDataAnalysisFramework::Step44_TrackReconstruction()
{
  CSHINETrackReconstruction layermulti;

  Int_t FisrtRun = 200;
  Int_t LastRun  = 210;

  layermulti.EstimateLayerMulti(FisrtRun, LastRun);
}

//______________________________________________________________________________
void CSHINESSDDataAnalysisFramework::Step45_CsIEnergyCali()
{
  cout<<"Enter Step45_CsIEnergyCali"<<endl;
}

//______________________________________________________________________________
void CSHINESSDDataAnalysisFramework::Step46_ParticleIdentification()
{
  cout<<"Enter Step46_ParticleIdentification"<<endl;
}

//______________________________________________________________________________
void CSHINESSDDataAnalysisFramework::Step47_EventTree()
{
  Int_t option47      = 1;
  Int_t option_step47 = 0;

  Int_t FirstRun;
  Int_t LastRun;

  cout<<"********************************************"<<endl;
  cout<<"       欢迎来到 Step47_EventTree 界面 !      "<<endl;
  cout<<"********************************************"<<endl;
  cout<<endl;
  cout<<"请选择您需要进行的操作: "<<endl;
  cout<<"471.运行 BuildLayerEventTree (Int_t fisrtrun, Int_t lastlun)"<<endl;
  cout<<"472.运行 BuildSSDEventTree   (Int_t fisrtrun, Int_t lastlun)"<<endl;
  cout<<"473.运行 BuildGlobalEventTree(Int_t fisrtrun, Int_t lastlun)"<<endl;

  while (option47 == 1) {
    cout<<"============================================="<<endl;
    cout<<"请在此输入您的选项: 471, 472, 473"<<endl;
    cin >>option_step47;
    cout<<"请输入起始文件编号: (121 - 432)"<<endl;
    cin >>FirstRun;
    cout<<"请输入终止文件编号: (121 - 432)"<<endl;
    cin >>LastRun;
    if (FirstRun > LastRun) {
      cout<<"错误: 起止文件编号有误, 请重新输入:"<<endl;
      cout<<"请重新输入起始文件编号 firstrun: (121 - 432)"<<endl;
      cin >>FirstRun;
      cout<<"请重新输入终止文件编号  lastrun: (121 - 432)"<<endl;
      cin >>LastRun;
    }
    if (option_step47 == 471)  buildevent.BuildLayerEventTree (FirstRun, LastRun);
    if (option_step47 == 472)  buildevent.BuildSSDEventTree   (FirstRun, LastRun);
    if (option_step47 == 472)  buildevent.BuildGlobalEventTree(FirstRun, LastRun);

    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step47, 重新进行操作选择"<<endl;
    cout<<"2.结束操作, 退出程序！"<<endl;
    cin >>option47;
    if (option47 == 2) {
      cout<<"***********************************************"<<endl;
      cout<<"            Exit Step47_EventTree !           "<<endl;
      cout<<"***********************************************"<<endl;
      return;
    }
  }
}
//******************************************************************************


//******************************************************************************
void CSHINESSDDataAnalysisFramework::Step411_AutoFindPedestals()
{
  Int_t option411    = 1;
  Int_t option_layer = 0;
  Int_t option_file  = 0;

  std::string LayerTag;
  std::string FileTag;

  cout<<"****************************************************************"<<endl;
  cout<<"     您正在进行 Pedestals 自动寻峰  Step411_AutoFindPedestals ！    "<<endl;
  cout<<"****************************************************************"<<endl;
  cout<<endl;
  while (option411 == 1) {
    InputLayerAndFileTag(option_layer, LayerTag, option_file, FileTag);
    pedestal.AutoFindPedestals(LayerTag.c_str(), FileTag.c_str());
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step411, 重新进行操作选择"<<endl;
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
void CSHINESSDDataAnalysisFramework::Step412_ClickToFindPedestals()
{
  Int_t option412    = 1;
  Int_t option_layer = 0;
  Int_t option_file  = 0;

  std::string LayerTag;
  std::string FileTag;

  cout<<"****************************************************************"<<endl;
  cout<<"    您正在进行Pedestals自动寻峰  Step412_ClickToFindPedestals ！   "<<endl;
  cout<<"****************************************************************"<<endl;
  cout<<endl;
  while (option412 == 1) {
    InputLayerAndFileTag(option_layer, LayerTag, option_file, FileTag);
    pedestal.ClickToFindPedestals(LayerTag.c_str(), FileTag.c_str());
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step412, 重新进行操作选择"<<endl;
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
void CSHINESSDDataAnalysisFramework::Step413_EstimateErrorsOfPedestals()
{
  Int_t option413    = 1;
  Int_t option_layer = 0;
  Int_t option_file  = 0;

  std::string LayerTag;
  std::string FileTag1;
  std::string FileTag2;

  cout<<"*******************************************************************"<<endl;
  cout<<"   您正在进行Pedestals差异评估  Step413_EstimateErrorsOfPedestals !   "<<endl;
  cout<<"*******************************************************************"<<endl;
  cout<<endl;
  while (option413 == 1) {
    InputLayerAngTwoFiles(option_layer, LayerTag, option_file, FileTag1, FileTag2);
    pedestal.EstimateErrorsOfPedestals(LayerTag.c_str(), FileTag1.c_str(), FileTag2.c_str());
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step413, 重新进行操作选择"<<endl;
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
//******************************************************************************


//******************************************************************************
void CSHINESSDDataAnalysisFramework::Step421_PulserCali()
{
  Int_t option421      = 1;
  Int_t option_step421 = 0;
  Int_t option_pulser  = 0;
  Int_t option_gain    = 0;
  Int_t option_layer   = 0;

  std::string PulserFileTag;
  std::string GainTag;
  std::string LayerTag;
  std::string LowGain("LowGain");
  std::string HighGain("HighGain");
  std::string L1STag("L1S");

  cout<<"***************************************************************"<<endl;
  cout<<"           您现在进行硅条的脉冲刻度  Step421_PulserCali            "<<endl;
  cout<<"***************************************************************"<<endl;
  cout<<endl;
  while (option421 == 1) {
    cout<<"请选择您需要进行的操作: "<<endl;
    cout<<"============================================================="<<endl;
    printf("4211  L1_AutoFindPeaksAndFit()          -- L1自动寻峰\n");
    printf("4212  L2_AutoFindPeaksAndFit()          -- L2自动寻峰\n");
    printf("4213  L1_AutoFindPeaksAndFit_ReCali()   -- L1重新刻度\n");
    printf("4214  L1_GainEffectOnPulseCali_ReCali() -- L1前放放大倍数的关系\n");
    printf("4215  CaliCheck_SSD_L2F_CH00_01()       -- L2F检查CH00_01\n");
    printf("4216  EstimateErrorOfHightAndSwitch()   -- Hight和Switch偏差\n");
    printf("4217  CheckLinearity()                  -- 检查脉冲的线性\n");
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
      cout<<"====================="<<endl;
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

    } else {  //4217  检查脉冲的线性
      InputLayerTagWithLabel(option_layer, LayerTag);
      InputPuserTag(option_pulser,PulserFileTag);
      pulsercali.CheckLinearity(LayerTag.c_str(),PulserFileTag.c_str());
    }
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step421, 重新进行操作选择"<<endl;
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
void CSHINESSDDataAnalysisFramework::Step422_AlphaCali()
{
  Int_t option422        = 1;
  Int_t option_step422   = 0;
  Int_t option_layer     = 0;
  Int_t option_alphafile = 0;
  Int_t firstrun         = 0;
  Int_t lastrun          = 0;

  std::string LayerTag;
  std::string AlphaFileTag;

  cout<<"***************************************************************"<<endl;
  cout<<"           您现在进行硅条的脉冲刻度  Step422_AlphaCali             "<<endl;
  cout<<"***************************************************************"<<endl;
  cout<<endl;
  while (option422 == 1) {
    cout<<"============================================================="<<endl;
    cout<<"请选择您需要进行的操作: "<<endl;
    printf("4221  CalculateAlphaEnergy()        -- alpha 能量修正计算\n");
    printf("4222  MergeAlphaCaliFiles()         -- 合并 alpha 刻度文件\n");
    printf("4223  ClickToFindAlphaPeaks()       -- 手动拟合 alpha 峰\n");
    printf("4224  CheckAbnormaleChannels_SSD4() -- 检查异常信号\n");
    cout<<"============================================================="<<endl;
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
      alphacali.CheckAbnormaleChannels_SSD4();
    }
    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step422, 重新进行操作选择"<<endl;
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
void CSHINESSDDataAnalysisFramework::Step423_SiEnergyCali()
{
  Int_t option423         = 1;
  Int_t option_step423    = 0;
  Int_t option_layer      = 0;
  Int_t option_pulserfile = 0;

  std::string LayerTag;
  std::string PulserFileTag;

  cout<<"***************************************************************"<<endl;
  cout<<"          您现在进行硅条的脉冲刻度  Step423_SiEnergyCali           "<<endl;
  cout<<"***************************************************************"<<endl;
  cout<<endl;
  while (option423 == 1) {
    cout<<"请选择您需要进行的操作: "<<endl;
    cout<<"============================================================="<<endl;
    printf("4231  PulserAndAlphaCali()               -- 硅条能量刻度曲线\n");
    printf("4232  EstimatePedestals()                -- 考察刻度曲线与pedestal\n");
    printf("4233  EstimateFitPars()                  -- 初步评估刻度参数\n");
    printf("4234  CheckAbnormalChannels()            -- 检查异常信号\n");
    printf("4235  EstimateDeadlayerEffects()         -- 考察死层修正\n");
    printf("4236  EstimateAlphaChannelEffOnFitPars() -- 考察 alpha 道址效应\n");
    printf("4237  FinallyDeterminedFitPars()         -- 确定最终刻度参数\n");
    cout<<"============================================================="<<endl;
    cout<<"请在此输入您的选项: 4231, 4232, 4233, 4234, 4235, 4236, 4237"<<endl;
    cin >>option_step423;
    if (option_step423 == 4231) {
      InputLayerTagWithLabel(option_layer, LayerTag);
      InputPuserTag(option_pulserfile, PulserFileTag);
      energycali.PulserAndAlphaCali(LayerTag.c_str(),PulserFileTag.c_str());

    } else if (option_step423 == 4232) {
      InputLayerTagWithLabel(option_layer, LayerTag);
      InputPuserTag(option_pulserfile, PulserFileTag);
      energycali.EstimatePedestals(LayerTag.c_str(),PulserFileTag.c_str());

    } else if (option_step423 == 4233) {
      InputLayerTagWithLabel(option_layer, LayerTag);
      InputPuserTag(option_pulserfile, PulserFileTag);
      energycali.EstimateFitPars(LayerTag.c_str(),PulserFileTag.c_str());

    } else if (option_step423 == 4234) {
      InputPuserTag(option_pulserfile, PulserFileTag);
      energycali.CheckAbnormalChannels(PulserFileTag.c_str());

    } else if (option_step423 == 4235) {
      InputLayerTagWithLabel(option_layer, LayerTag);
      InputPuserTag(option_pulserfile, PulserFileTag);
      energycali.EstimateDeadlayerEffects(LayerTag.c_str(), PulserFileTag.c_str());

    } else if (option_step423 == 4236) {
      InputLayerTagWithLabel(option_layer, LayerTag);
      InputPuserTag(option_pulserfile, PulserFileTag);
      energycali.EstimateAlphaChannelEffOnFitPars(LayerTag.c_str(),PulserFileTag.c_str());

    } else {
      InputLayerTagWithLabel(option_layer, LayerTag);
      InputPuserTag(option_pulserfile, PulserFileTag);
      energycali.FinallyDeterminedFitPars(LayerTag.c_str(),PulserFileTag.c_str());
    }

    cout<<"请选择下一步操作: "<<endl;
    cout<<"1.返回 Step423, 重新进行操作选择"<<endl;
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
//******************************************************************************


//******************************************************************************
// for Step41_PedestalCali()
void CSHINESSDDataAnalysisFramework::InputLayerTagWithLabel(Int_t opt_layer, std::string& layertag)
{
  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string L3ATag("L3A");
  cout<<"请选择对哪一层硅条进行处理: "<<endl;
  cout<<"===================="<<endl;
  cout<<"1.L1S"<<endl;
  cout<<"2.L2F"<<endl;
  cout<<"3.L2B"<<endl;
  cout<<"4.L3A (only for PedestalCali)"<<endl;
  cout<<"====================="<<endl;
  cout<<"请输入您的选项:"<<endl;
  cin >>opt_layer;
  if (opt_layer == 1) layertag = L1STag;
  if (opt_layer == 2) layertag = L2FTag;
  if (opt_layer == 3) layertag = L2BTag;
  if (opt_layer == 4) layertag = L3ATag;
}

//______________________________________________________________________________
void CSHINESSDDataAnalysisFramework::InputLayerAndFileTag(Int_t opt_layer, std::string& layertag,
  Int_t opt_file,std::string& filetag)
{
  std::string L1STag("L1S");
  std::string Pedestal0000  ("Pedestal0000");
  std::string AlphaCali00_04("AlphaCali00_04");
  std::string AlphaCali05_08("AlphaCali05_08");
  std::string AlphaCali00_08("AlphaCali00_08");
  std::string AlphaCali00_32("AlphaCali00_32");
  std::string AlphaCali33_48("AlphaCali33_48");
  std::string AlphaCali00_48("AlphaCali00_48");

  InputLayerTagWithLabel(opt_layer, layertag);

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
    if (opt_file == 2) filetag = AlphaCali00_32;
    if (opt_file == 3) filetag = AlphaCali33_48;
    if (opt_file == 4) filetag = AlphaCali00_48;
  }
}

//______________________________________________________________________________
void CSHINESSDDataAnalysisFramework::InputLayerAngTwoFiles(Int_t opt_layer,std::string& layertag,
  Int_t opt_file,std::string& filetag1,std::string& filetag2)
{
  std::string L1STag("L1S");
  std::string Pedestal0000  ("Pedestal0000");
  std::string AlphaCali00_04("AlphaCali00_04");
  std::string AlphaCali05_08("AlphaCali05_08");
  std::string AlphaCali00_08("AlphaCali00_08");
  std::string AlphaCali00_32("AlphaCali00_32");
  std::string AlphaCali33_48("AlphaCali33_48");
  std::string AlphaCali00_48("AlphaCali00_48");

  InputLayerTagWithLabel(opt_layer, layertag);

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

//______________________________________________________________________________
// for Step42_SiEnergyCali()
void CSHINESSDDataAnalysisFramework::InputPuserTag(Int_t opt, std::string& pulserfiletag)
{
  std::string Height("Height");
  std::string Switch("Switch");
  cout<<"请选择脉冲刻度方式: 1.Height(等幅度法), 2.Switch(拨档法)"<<endl;
  cin >>opt;
  if (opt == 1) pulserfiletag = Height;
  if (opt == 2) pulserfiletag = Switch;
}

//______________________________________________________________________________
void CSHINESSDDataAnalysisFramework::InputLayerTag(Int_t opt_layer, std::string& layertag)
{
  std::string L1Tag("L1");
  std::string L2Tag("L2");
  cout<<"请选择对哪一层硅条进行处理: "<<endl;
  cout<<"========================"<<endl;
  cout<<"1.L1"<<endl;
  cout<<"2.L2"<<endl;
  cout<<"========================"<<endl;
  cout<<"请输入您的选项:"<<endl;
  cin >>opt_layer;
  if (opt_layer == 1) layertag = L1Tag;
  if (opt_layer == 2) layertag = L2Tag;
}

//______________________________________________________________________________
void CSHINESSDDataAnalysisFramework::InputLayerAndAlphaTag(Int_t opt_layer,std::string& layertag,
  Int_t opt_alphafile,std::string& alphafiletag)
{
  std::string L1STag("L1S");
  std::string AlphaCali00_04("AlphaCali00_04");
  std::string AlphaCali05_08("AlphaCali05_08");
  std::string AlphaCali00_08("AlphaCali00_08");
  std::string AlphaCali00_32("AlphaCali00_32");
  std::string AlphaCali33_48("AlphaCali33_48");
  std::string AlphaCali00_48("AlphaCali00_48");

  InputLayerTagWithLabel(opt_layer, layertag);

  if (strcmp(layertag.c_str(), L1STag.c_str())==0) {
    cout<<"请选择 L1 的 alpha 合并文件: "<<endl;
    cout<<"================================="<<endl;
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
    cout<<"==================================="<<endl;
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
//******************************************************************************
