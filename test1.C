
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TChain.h"
#include "TNtuple.h"
#include <TH2.h>

#include "TSystem.h"
#include "TCanvas.h"

#include "include/TimeAndPercentage.h"
#include "include/ReadFileModule.h"
#include "include/CSHINEQualityCheck.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <map>

using namespace std;

int main(int argc, char *argv[])
{
  CSHINEQualityCheck  quality;

  std::string pathlistname("/home/sea/Fission2019_Data/MapRoot/alistfilename1");

  quality.QC_TreeReader(pathlistname.c_str());

  return 0;
}
