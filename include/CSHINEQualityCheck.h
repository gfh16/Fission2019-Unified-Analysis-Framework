#ifndef CSHINEQUALITYCHECK_H
#define CSHINEQUALITYCHECK_H

#include "ReadFileModule.h"
#include "TimeAndPercentage.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TChain.h"
#include <TH2.h>
#include "TCanvas.h"
#include "TKey.h"

#include "shared.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>

//______________________________________________________________________________
class CSHINEQualityCheck
{
public:
    CSHINEQualityCheck();
    ~CSHINEQualityCheck();

    void    QC_TreeReader(const char* pathlistfiles);
    void    QC_BranchAdress(const char* pathlistfiles);
    void    QC_ReadHistToDraw(Int_t runnumber, const char* layertag);

    void    TreeReaderMethod(TChain* mychain, const char* pathrootout, const char* pathpdfout);
    void    WriteHistforSSD(TFile* FileOut, TH1D* hist[],  const Int_t numtel);
    void    DrawSSD(const Char_t* pdfpath, TCanvas* canvas[], TH1D* hist[], const Int_t numtel);
    void    DrawSSDLogy(const Char_t* pdfpath, TCanvas* canvas[], TH1D* hist[], const Int_t numtel);

    void    ReadBranchMethod(TChain* chain, const char* pathrootout, const char* pathpdfout);

};

#endif
