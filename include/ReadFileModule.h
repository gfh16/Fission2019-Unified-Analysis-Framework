#ifndef READFILEMODELU_H
#define READFILEMODELU_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TChain.h"
#include <TH2.h>
#include "TCanvas.h"

#include "shared.h"


using namespace std;

//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
class ReadFileModule
{
public:
  ReadFileModule();
  ~ReadFileModule();

  Double_t*** ReadData(const Char_t* datapath, Int_t SSDNum, Int_t CHNum, Int_t ParNum);
  void        DeleteData(Double_t*** p, Int_t SSDNum, Int_t CHNum, Int_t ParNum);
  void        AddChain(TChain *chain, const char *LayerTag, Int_t *dataarray, Int_t size, Int_t index);
  void        GetFileNamesFromFolder(const char* pathfolder, const char* pathfileout);
  void        GetFileNamesFromFile(const char* pathfilein, std::vector<string>& filelist);

  Bool_t      IsFileExists(const std::string& filename);

  void        CsICaliLoadPars(std::vector<Double_t> parlist[NUM_SSD*NUM_CSI], const char* pathFarsFile, Int_t npars);
  Double_t**  CsICaliLoadPars2(const char* pathFarsFile, Int_t npars);
};

#endif
