#ifndef KRPBREADTREE_H
#define KRPBREADTREE_H

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

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <map>

class KrPbReadTree
{
    public:
    KrPbReadTree();
    ~KrPbReadTree();

    void WriteHist(TChain* myChain,  const char* rootpath);
    void SaveCanvas(TChain* myChain, const char* pdfpath);

};

#endif
