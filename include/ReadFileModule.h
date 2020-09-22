#ifndef READFILEMODELU_H
#define READFILEMODELU_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <stdio.h>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TChain.h"
#include <TH2.h>
#include "TCanvas.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>

using namespace std;

//******************************************************************************
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
};

#endif
