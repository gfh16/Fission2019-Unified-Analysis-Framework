#ifndef DEEFIT_H
#define DEEFIT_H

// Root include and globals
#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TApplication.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGMenu.h>
#include <TG3DLine.h>
#include <TGFileDialog.h>
#include <TGInputDialog.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TGButton.h>
#include <TGProgressBar.h>
#include <TGComboBox.h>
#include <TGStatusBar.h>
#include <TGMsgBox.h>
#include <TTimer.h>
#include <TFile.h>
#include <TTree.h>
#include <TKey.h>
#include <TGraph.h>
#include <TCut.h>
#include <TCanvas.h>
#include <TFrame.h>
#include <Riostream.h>
#include <TF1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TMinuit.h>
#include <sstream>
#include <cstdlib>
#include <vector>

enum efitkind {kstandard, kenhanced};   //Fit standard parameters or enhanced 
enum ecalcmass {k2, kcharity, kepax};   //Mass Z=2*A, Charity or Epax

const int MAXMAS = 15;  // Max number of Z for which Mass can be drawn 
const int MAXFP = 20;   // Max number of fit parameters   

// the fitter function have to be declared global
void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t flag);

#endif

