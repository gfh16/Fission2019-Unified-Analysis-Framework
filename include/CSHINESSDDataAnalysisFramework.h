#ifndef CSHINESSDDATAANALYSISFRAMEWORK_H
#define CSHINESSDDATAANALYSISFRAMEWORK_H

#include "ReadFileModule.h"
#include "CSHINEPedestals.h"
#include "CSHINEPulserCali.h"
#include "CSHINEAlphaCali.h"
#include "CSHINESiEnergyCali.h"
#include "CSHINESiPixelGeometry.h"
#include "CSHINEEvent.h"
#include "CSHINEHitPatternRecognition.h"


//______________________________________________________________________________
class CSHINESSDDataAnalysisFramework
{
public:
  CSHINESSDDataAnalysisFramework();
  ~CSHINESSDDataAnalysisFramework();

  void   Step4_SSDDataAnalysisFramework();
  void   Step41_PedestalCali();
  void   Step42_SiEnergyCali();
  void   Step43_SiPixellation();
  void   Step44_HitPatternRecognition();
  void   Step45_CsIEnergyCali();
  void   Step46_ParticleIdentification();
  void   Step47_EventTree();

private:
  void   Step411_AutoFindPedestals();
  void   Step412_ClickToFindPedestals();
  void   Step413_EstimateErrorsOfPedestals();

  void   Step421_PulserCali();
  void   Step422_AlphaCali();
  void   Step423_SiEnergyCali();

  void   InputLayerTagWithLabel(Int_t opt_layer, std::string& layertag);
  void   InputLayerAndFileTag(Int_t opt_layer, std::string& layertag,Int_t opt_file,std::string& filetag);
  void   InputLayerAngTwoFiles(Int_t opt_layer,std::string& layertag,Int_t opt_file,std::string& filetag1,std::string& filetag2);
  void   InputPuserTag(Int_t opt, std::string& pulserfiletag);
  void   InputLayerTag(Int_t opt_layer, std::string& layertag);
  void   InputLayerAndAlphaTag(Int_t opt_layer,std::string& layertag,Int_t opt_alphafile,std::string& alphafiletag);

  CSHINESiPixelGeometry   pixel;
  CSHINEBuildEvent        buildevent;
  CSHINEPedestals         pedestal;
  CSHINEPulserCali        pulsercali;
  CSHINEAlphaCali         alphacali;
  CSHINESiEnergyCali      energycali;
};

#endif
