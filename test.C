#include "include/ReadFileModule.h"

Int_t SSDNum = 4;
Int_t CHNum  = 16;
Int_t PeakNum = 3;
Int_t PeakIndex = 10;


void test()
{
   ReadFileModule readfile;
   std::string LayerTag("L2F");
   std::string FileTag("Switch");  // Switch or Height
   std::string AlphaCaliTag("AlphaCali00_32");
   std::string DeadLayerTag("ChangingDeadLayer");

   std::string pathPuserIn(Form("Step4_SSD-Analysis-Framework/Step4.1_EnergyCalibration/output/SSD_%s_PulserCali_%s.dat",LayerTag.c_str(),FileTag.c_str())); // Pulser fitting parameters
   std::string pathAlphaChIn(Form("Step4_SSD-Analysis-Framework/Step4.1_EnergyCalibration/output/SSD_%s_AlphaPeaks_%s.dat",LayerTag.c_str(), AlphaCaliTag.c_str())); // Channel of the alpha peaks
   std::string pathAlphaE1In(Form("Step4_SSD-Analysis-Framework/Step4.1_EnergyCalibration/output/SSD_AlphaE1_%s.dat",DeadLayerTag.c_str()));   // Alpha enegies in MeV
   std::string pathAlphaE2In(Form("Step4_SSD-Analysis-Framework/Step4.1_EnergyCalibration/output/SSD_AlphaE2_%s.dat",DeadLayerTag.c_str()));   // Alpha enegies in MeV
   std::string pathAlphaE3In(Form("Step4_SSD-Analysis-Framework/Step4.1_EnergyCalibration/output/SSD_AlphaE3_%s.dat",DeadLayerTag.c_str()));   // Alpha enegies in MeV

   std::string pathSiEnergyCaliChangingDeadLayerPDF(Form("figures/SSD_%s_%s_%s_%s.pdf",
                                                    LayerTag.c_str(),DeadLayerTag.c_str(),FileTag.c_str(),AlphaCaliTag.c_str()));
   std::string pathSiEnergyCaliChangingDeadLayerPDFbegin(Form("figures/SSD_%s_%s_%s_%s.pdf[",
                                                    LayerTag.c_str(),DeadLayerTag.c_str(),FileTag.c_str(),AlphaCaliTag.c_str()));
   std::string pathSiEnergyCaliChangingDeadLayerPDFend(Form("figures/SSD_%s_%s_%s_%s.pdf]",
                                                    LayerTag.c_str(),DeadLayerTag.c_str(),FileTag.c_str(),AlphaCaliTag.c_str()));

   Int_t numpar_PulserIn  = 4;  // 4 pars: a, err_a, b, err_b        (E = a*ch + b)
   Int_t numpar_AlphaChIn = 3;  // 3 alpha channels: CH1, CH2, CH3
   Int_t numpar_AlphaE1In = 10;  //
   Int_t numpar_AlphaE2In = 10;  //
   Int_t numpar_AlphaE3In = 10;  //

   Double_t*** PulserIn  = readfile.ReadData(pathPuserIn.c_str(),  SSDNum, CHNum, numpar_PulserIn);
   Double_t*** AlphaChIn = readfile.ReadData(pathAlphaChIn.c_str(),SSDNum, CHNum, numpar_AlphaChIn);
   Double_t*** AlphaE1In = readfile.ReadData(pathAlphaE1In.c_str(), SSDNum, CHNum, numpar_AlphaE1In);
   Double_t*** AlphaE2In = readfile.ReadData(pathAlphaE2In.c_str(), SSDNum, CHNum, numpar_AlphaE2In);
   Double_t*** AlphaE3In = readfile.ReadData(pathAlphaE3In.c_str(), SSDNum, CHNum, numpar_AlphaE3In);

   Double_t k1[SSDNum][CHNum][PeakIndex];
   Double_t k2[SSDNum][CHNum][PeakIndex];
   Double_t k3[SSDNum][CHNum][PeakIndex];
   Double_t h1[SSDNum][CHNum][PeakIndex];
   Double_t h2[SSDNum][CHNum][PeakIndex];
   Double_t h3[SSDNum][CHNum][PeakIndex];
   Double_t k11[SSDNum][CHNum][PeakIndex];
   Double_t k22[SSDNum][CHNum][PeakIndex];
   Double_t k33[SSDNum][CHNum][PeakIndex];
   Double_t h11[SSDNum][CHNum][PeakIndex];
   Double_t h22[SSDNum][CHNum][PeakIndex];
   Double_t h33[SSDNum][CHNum][PeakIndex];
   Double_t peak_CH[SSDNum][CHNum][PeakIndex][PeakNum];
   Double_t peak_MeV[SSDNum][CHNum][PeakIndex][PeakNum];

   for (Int_t i=0; i<SSDNum; i++)
   {
     for (Int_t j=0; j<CHNum; j++)
     {
       cout<<PulserIn[i][j][0]<<endl;
     }
   }
}
