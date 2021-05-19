#include "ExtractDEEPointsGUI.h"
void DoMarker()
{
  // Popup the GUI...
  ExtractDEEPointsGUI* Cut_analyser = new ExtractDEEPointsGUI(gClient->GetRoot(),20,20);

//  Cut_analyser->Initial_RootFile("/home/sea/Fission2019_Data/StraighteningData/L1L2_HistData_Run0150-0400.root"); // DEE-plot 输入文件路径
//  Cut_analyser->Set_MarkersFileName("../data/data_PID/L1L2PunchThrough_StraighteningGraphData.root", "h2_dE1_dE2_Tel"); // 选点输出文件路径,与直方图名称前缀
//  Cut_analyser->Set_FitParsFileName("../data/data_PID/L1L2PunchThrough_StraighteningFitPars.dat");   // PID 带子拟合参数输出路径
//  Cut_analyser->Set_BananaCutFileName("/home/sea/Fission2019_Data/StraighteningData/L1L2PunchThroughCuts.root");

//  Cut_analyser->Set_MarkersFileName("../data/data_PID/L2L3_StraighteningGraphData.root", "h2_dE2_ECsI_Tel"); // 选点输出文件路径,与直方图名称前缀
//  Cut_analyser->Set_FitParsFileName("../data/data_PID/L2L3_StraighteningFitPars.dat");   // PID 带子拟合参数输出路径
//  Cut_analyser->Set_BananaCutFileName("../data/L2L3BananaCut.root");

//  Cut_analyser->Initial_RootFile("/home/sea/Fission2019_Data/StraighteningData/L1L2_SSD3SSD4_HistData_Run0150-0400.root"); // DEE-plot 输入文件路径
//  Cut_analyser->Set_MarkersFileName("../data/data_PID/L1L2_SSD3SSD4_StraighteningGraphData.root", "h2_dE1_dE2_Tel"); // 选点输出文件路径,与直方图名称前缀
//  Cut_analyser->Set_FitParsFileName("../data/data_PID/L1L2_SSD3SSD4_StraighteningFitPars.dat");   // PID 带子拟合参数输出路径
//  Cut_analyser->Set_BananaCutFileName("/home/sea/Fission2019_Data/StraighteningData/L1L2_SSD3SSD4_DEECuts.root");

  Cut_analyser->Initial_GUI(800, 600);  // 设置 GUI 面板大小
  Cut_analyser->Initial_Slot();
}
