#ifndef CSHINEDATAANALYSISFRAMEWORK_H
#define CSHINEDATAANALYSISFRAMEWORK_H

#include "ReadFileModule.h"
#include "CSHINEPedestals.h"
#include "CSHINEPulserCali.h"
#include "CSHINEAlphaCali.h"
#include "CSHINESiEnergyCali.h"
#include "CSHINESiPixelGeometry.h"
#include "CSHINEEvent.h"
#include "CSHINESSDDataAnalysisFramework.h"

#include <iostream>


//______________________________________________________________________________
class CSHINEDataAnalysisFramework
{
public:
  CSHINEDataAnalysisFramework();
  ~CSHINEDataAnalysisFramework();

  void   CSHINEDataAnalysis();
  void   Step1_DataConversion();
  void   Step2_QualityChecks();
  void   Step3_PPACAnalysisFramework();
  void   Step4_SSDAnalysisFramework();

private:
  void   Step11_GenerateFileLists();
  void   Step12_RawToRawRoot();
  void   Step13_RawRootToMapRoot();

  // for Step1_DataConversion()
  void   InputFileName(Int_t opt_filename, std::string& pathfolder, std::string& pathfile);

  ReadFileModule                  readfile;
  CSHINESSDDataAnalysisFramework  SSDDataAnalysis;

};

#endif
