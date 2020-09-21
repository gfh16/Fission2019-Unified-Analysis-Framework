#ifndef READFILEMODELU_H
#define READFILEMODELU_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <stdio.h>



//******************************************************************************
class ReadFileModule
{
public:
  ReadFileModule();
  ~ReadFileModule();

  Double_t*** ReadData(const Char_t* datapath, Int_t SSDNum, Int_t CHNum, Int_t ParNum);
  void        DeleteData(Double_t*** p, Int_t SSDNum, Int_t CHNum, Int_t ParNum);
  void        AddChain(TChain *chain, const char *LayerTag, Int_t *dataarray, Int_t size, Int_t index);
  void        GetFilesName(const char* pathfolder, const char* pathfileout);
};

#endif
