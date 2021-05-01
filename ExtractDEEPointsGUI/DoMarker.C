#include "ExtractDEEPointsGUI.h"
void DoMarker()
{
  // Popup the GUI...
  ExtractDEEPointsGUI* Cut_analyser = new ExtractDEEPointsGUI(gClient->GetRoot(),20,20);
  Cut_analyser->Initial_RootFile("/home/sea/Fission2019_Data/StraighteningData/L2L3_HistData_Run0150-0400.root"); // DEE-plot 输入文件路径
  Cut_analyser->Set_CutFile_Name("../data/data_PID/L2L3_StraighteningGraphData.dat", "h2_dE2_ECsI_Tel"); // 选点输出文件路径,与直方图名称前缀
  Cut_analyser->Set_FitParsFile_Name("../data/data_PID/L2L3_StraighteningFitPars.dat");   // PID 带子拟合参数输出路径
  Cut_analyser->SetTeleNo_Max(100);     // 设置 DEE-plot 数目上限
  Cut_analyser->SetParticleNo_Max(50);  // 设置每个 DEE-plot 中粒子的数目上限
  Cut_analyser->Initial_GUI(800, 600);  // 设置 GUI 面板大小
  Cut_analyser->Initial_Slot();
}
