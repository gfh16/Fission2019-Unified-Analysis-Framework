////////////////////////////////////////////////////////////////////////////////
// class ExtractDEEPointsGUI is used to extract dE-E points with GUI,
//  modified by gfh, based on the GUI of Fission2014
//
//  [**Note**] gfh just modified this class to be used for Fission2019,
//             the original one comes from Zhang Yan's code
//
// Jan 4, 2021
///////////////////////////////////////////////////////////////////////////////

#ifndef ExtractDEEPointsGUI_h
#define ExtractDEEPointsGUI_h

#include <TGClient.h>
#include <TCanvas.h>
#include "TPad.h"
#include <TF1.h>
#include "TFile.h"
#include "TH1D.h"
#include <TRandom.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGListBox.h>
#include <TGListView.h>
#include <TList.h>
#include <TGLabel.h>
#include <TGSimpleTable.h>
#include <TGFrame.h>
#include <TGNumberEntry.h>
#include <TG3DLine.h>
#include <TRootEmbeddedCanvas.h>
#include "TGFrame.h"
#include <TApplication.h>
#include "fstream"
#include <iomanip>
#include "TMarker.h"
#include "TH2I.h"
#include "TGraph.h"

using namespace std;

#define NUM_POINTS       20   // extract 20 points for DE-E plot of each isotope
#define NUM_FITPARS      8

#define MAX_TELENO       50 // max tele number (the index of DE-E plot)
#define MAX_PARTICLENO   50  // max channel number
#define MAX_CHARGENO     20   // max charge number
#define MAX_MASSNO       50   // max mass number



//******************************************************************************
//           text margins class
//        ------------------------
class TextMargin : public TGHorizontalFrame
{
protected:
  TGNumberEntry *fEntry;

public:
  TextMargin(const TGWindow* p, const char* name) : TGHorizontalFrame(p)
  {
    fEntry = new TGNumberEntry(this, 0, 6, -1, TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative);
    AddFrame(fEntry, new TGLayoutHints(kLHintsLeft));
    TGLabel* label = new TGLabel(this, name);
    AddFrame(label, new TGLayoutHints(kLHintsLeft, 10));
  }
  TGTextEntry* GetEntry() const { return fEntry->GetNumberEntry(); }
  Double_t GetNumber(){ return fEntry->GetNumber(); }

  ClassDef(TextMargin, 0)
};
//******************************************************************************


//______________________________________________________________________________
//                   GUI constructon
//                -----------------------
class ExtractDEEPointsGUI : public TGMainFrame
{
//the widgets for GUI
private:
  TRootEmbeddedCanvas   *fEcanvas, *ftext;
  TGTextButton          *record, *clear, *exit, *draw,  *Write_File, *refresh, *Fit_Markers, *Save_Pars;
  TGLabel               *xx[NUM_POINTS], *yy[NUM_POINTS];
  TGNumberEntryField    *addx[NUM_POINTS], *addy[NUM_POINTS];
  TGSimpleTable         *PointTable;
  TGGroupFrame          *margins;
  TextMargin            *Particle_Entry;
  TextMargin            *Tele_Entry;
  TextMargin            *Charge_Entry;
  TextMargin            *Mass_Entry;

  TGButtonGroup         *WorkMode_bg;
  TGRadioButton         *WorkMode_bg_button[2];

public:
  ExtractDEEPointsGUI(const TGWindow *p, UInt_t w, UInt_t h);
  ~ExtractDEEPointsGUI();


public:
  Int_t        TeleNo, TeleNo_Max;
  Int_t        ParticleNo, ParticleNo_Max;
  Int_t        ChargeNo, MassNo;
  Double_t     CutData[MAX_TELENO][MAX_PARTICLENO][NUM_POINTS][2];  // [lower,upper][x,y]
  Double_t     Current_Cut[NUM_POINTS][NUM_POINTS]; // [x,y][lower,upper]
  Int_t        ParticleZA[MAX_TELENO][MAX_PARTICLENO][NUM_POINTS][2]; // chargrno, massno
  string       histo_FilePath;
  string       CutFile_Name;
  string       HistTile;
  string       pathFitParsOut;
  ofstream     MarkersFileOut;
  ofstream     FitParsFileOut;
  Bool_t       IsMarking;           // =1, Marking; =0 silence
  TFile       *f1_histo;
  TH2D        *h2_DEEPlot;
// for the general member
  void         Initial_RootFile(string histo_FilePath_tem);
  void         Initial_GUI(int xPixel,int yPixel);
  void         Initial_Slot();
  void         DoDraw_ch();
// the data for Drawing the histogram
  void         SetTeleNo_Max(Int_t tem) { TeleNo_Max = tem; }
  void         SetParticleNo_Max(Int_t tem)  { ParticleNo_Max = tem; }
// for recording the cut value
  Double_t   **DisplayDataXY;
  void         Set_CutFile_Name(string FileName_tem, string HistTile_tem);
  void         Write_CutFile();
// deal with markers
  void         Record_Marker();
  void         Delete_Marker();
  void         Record_Click(Int_t event, Int_t px, Int_t py, TObject* sel);
// for Exit operation
  void         Is_Exit();
// for chosing the working mode
  void         Silence_Mode(){ IsMarking = 0; cout<<" The Canvas is locked! "<<endl; }
  void         Marking_Mode(){ IsMarking = 1; cout<<" The Canvas can be Marked! "<<endl; }
// for drawing something
  void         DrawMarker(Int_t RunNo_tem, Int_t ChNo_tem, Int_t Edge_tem, Double_t x_tem, Double_t y_tem);
  TMarker     *Marker_Cut[MAX_TELENO][MAX_PARTICLENO][NUM_POINTS];
  TMarker     *Cur_Marker_Cut[NUM_POINTS];

// fit the markers for each isotope
// added by gfh, Jan 2021
  Double_t    *MarkerFitPars;
  void         DoMarkersFit();
  void         Set_FitParsFile_Name(string pathFitParsOut_tem);
  void         SaveFitPars();

  ClassDef(ExtractDEEPointsGUI,0)
};

#endif
