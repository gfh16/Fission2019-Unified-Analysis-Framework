#include "include/CSHINEPedestals.h"

#include "include/CSHINESSDDataAnalysisFramework.h"
#include <iostream>

using namespace std;


void test()
{
  std::string pathfile("Makefile");
  std::string layertag("L1S");
  std::string filetag("Pedestal0000");

  CSHINESSDDataAnalysisFramework SSDDataAnalysis;
  SSDDataAnalysis.Step4_SSDDataAnalysisFramework();

}
