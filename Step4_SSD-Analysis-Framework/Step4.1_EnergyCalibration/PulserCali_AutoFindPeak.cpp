//=============================================================================
//    This macro is used to find the peaks in pulser calibration data
//    Here, we use TSpectrum class in ROOT
//

#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TSpectrum.h"

void PulserCali_AutoFindPeak()
{
  std::string FileTag("L1");   // here to change layer1 (L1) or layer2 (L2)
  std::string path_to_file(Form("data/QC_MapSSD_%s_AlphaCali*.root"));

  TFile * FileIn = new TFile(path_to_file.c_str());
  if(!FileIn)
  {
    
  }

}
