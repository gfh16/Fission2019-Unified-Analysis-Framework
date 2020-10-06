#ifndef MDEEFRAME_H
#define MDEEFRAME_H

///////////////////////////////////////////////////////////
// Class that implements the main program functions
// and manages the graphical interface and menu layout
// E.d.F. (2009-2010)   v 1.1 rev. 1.2
// rev. 1.3   03/2010
// rev. 1.31   2013  CheckMResults (Paolo)
///////////////////////////////////////////////////////////


#include "deefit.h"
#include "MEpax.h"
#include "MTree.h"
#include "MFitParam.h"

//Popup Menu commands
enum EDeeCommands {
 kFileOpen,
 kFileClose,
 kFileQuit,

 kLineOpen,
 kLineSave,
 kLineSaveAs,
 kLineSpline,
 kLineDraw,
 kLineCopy,
 kLineList,
 kLineAscii,
 kLineDelete,
 kLineDelgroup,
 kLineDelOne,

 kFitParam,
 kFitExec,
 kFitDrawOne,
 kFitDrawAll,
 kFitStandard,
 kFitEnhanced,
 kFitCharity,
 kFitEpax,
 kFitTwo,
 kFitSave,
 kFitLoad,
 kFitFile,

 kAnaIdent,
 kAnaDraw,
 kAnaMass,
 kAnaCheck,
 kAnaCheck2,

 kSetupReac
};


//The fit data structure
struct MPart {
 Double_t x,y;   //x,y points
 Double_t Z,A;   //Charge and mass
 Double_t e;     //Error
};

class MDEEFrame;                      //forward declaration
// Alias for the prototype function to fit (pointer to
// member function)
typedef Double_t (MDEEFrame::*FitPtr)(MPart &, Double_t *);


// Main program class interface
class MDEEFrame : public TGMainFrame {
private:
// GUI layout members
 TGCompositeFrame *fhor1,*fhor2,*fhor3;              //Frame widgets
 TGLabel  *flzeta, *flmass, *ftlab, *flhist;         //Fixed labels
 TGNumberEntry *fZ,*fM,*ftel;                        //Number entry widgets
 TGTextButton *fdraw,*fplot,*frplot,*fusgraph;       //Text button widgets
 TGPictureButton *fphist;                            //Picture button widget
 TGPopupMenu *fMenuFile,*fMenuLine, *fMenuFit, *fMenuAna, *fMenuSetup;  //Menu widgets
 TGMenuBar *fMenuBar;                                //Menu bar widget
 TGLayoutHints *fMenuBarLayout, *fMenuBarItemLayout; //Layout widgets
 TGHProgressBar  *fProgressBar;                      //Progressbar widget
 TTimer *ftimer;                                     //Timer widget
 TGComboBox *fComboHisto;                            //Combo box widget
 TGStatusBar *fStatus;                               //Status bar widget
 TGFileInfo fi;                                      //TGFileInfo instance
// members
 MTree fmtree;               //Instance of the MTree class
 TList *fcklist;             //A dynamic list of clicks
 Int_t ftel_curr;            //The current telescope
 Int_t fNPoint;              //Dimension of fparray
 Int_t fNpar;                //Fit parameters
 TMinuit *fminuit;           //A pointer to TMinuit object
 Bool_t fdrawone;            //True if there is a request for the current A,Z draw fit
 Bool_t foffset;             //True if fitting, false during analysis
 MFitParam *fvpar[48];       //Table of fit parameter for all telescopes
 MEpax fepax2;               //Instance of MEpax class
 ecalcmass fmcalc;           //Mass calculation enum
 efitkind ffitkind;          //Fit function choice
 Int_t fLastAnalysis;        //Contains tha last telescope analyzed or -1 if any
 void PrintArray();          //List the array
public:
 static vector<MPart> fparray;     //A vector object for DEE fit
 FitPtr fptr;                      //Pointer to a fit function
 MDEEFrame(const TGWindow *p, UInt_t w, UInt_t h, MEpax &epax);  //Constructor
 virtual ~MDEEFrame();                                           //Destructor
 Bool_t AskToWrite();                           //Ask permission to write existing file
 int  CheckZResults(Int_t );                    //Check Z identification results
 int  CheckMResults(Int_t );                    //Check M identification results
 void CopyLines();                              //Copy a cut
 void CreateHistograms(Int_t );                 //Create histograms for data analysis
 void CreateMenu();                             //Create the menu structure
 void DeleteGroup(Int_t);                       //Delete a group of cuts/clicks
 void DeleteOne(Int_t);                         //Delete one cut/click
 void DisableFit();                             //Disable fit menu (slot)
 void DrawAllFit(Int_t);                        //Draw all fit lines
 void DrawHistogram();                          //Draw histogram (Slot)
 void DrawHistogram(Int_t );                    //Draw histograms given telescope number
 void DrawMasses(Int_t );                       //Draw mass distribution histograms
 void DrawOneFit(Int_t);                        //Draw just one fit line
 void DrawPoint();                              //Draw graphical cuts/lines (slot)
 void ExecuteFit();                             //Do fit
 void FitCurrentClicks();                       //Spline clicks lines
 Double_t func(MPart &, Double_t *);            //Standard fit function
 Double_t func14(MPart &, Double_t *);          //14 parameters fit function
 Bool_t GetAnaCoherence(Int_t );                //Check coherence
 Int_t Get_Charge(Int_t, Double_t, Double_t, Int_t *, Double_t *);  //Get charge (analysis)
 Int_t GetFitParameters(Int_t, Double_t *);     //Setup fit environment for one telescope
 Double_t Get_Mass(Int_t, Int_t, Double_t, Double_t, Int_t *);   //Get Mass analysis
 Int_t GetNPar() {return fNpar;}                //Get current fit parameters number
 void GetListClicks();                          //Load clicks using TGfileDialog to get file
 void GetListClicksFile(TFile *, Int_t);        //Retrieve clicks from file in memory
 string &GetListFromFile();                     //Return the file name of a fit data file
 Double_t GetMassCharity(Double_t);             //Get mass from Charity formula
 ecalcmass GetMassFormula() {return fmcalc;}    //Get current mas formula
 Bool_t GetOffset() {return foffset;}           //True if fitting, false during analysis
 Bool_t GetZ_A(TList *, Int_t &Z, Int_t &A);    //Extract mass and charge info from a cut
 void GraphMatrix();                            //Draw matrix (slot)
 FitPtr GetFPtr() {return fptr;}                //Get a fit function pointer
 Bool_t HandleMenu(Int_t );                     //Handler File menu
 Bool_t HandleMenuAna(Int_t );                  //Handler Analysis menu
 Bool_t HandleMenuFit(Int_t );                  //Handler fit menu
 Bool_t HandleMenuLine(Int_t );                 //Handler clicks menu
 Bool_t HandleMenuSetup(Int_t );                //Handler setup menu
 Bool_t HandleTimer(TTimer *);                  //Handler timer
 void InitFitFunction(Int_t );                  //Init a fit function
 void ListLines();                              //Print a list of cuts in memory
 void LoadFitParam(const char *);               //Load fit parameters
 Double_t operator()(Double_t *x, Double_t *par); //Overload operator ()
 void PrintAsciiLines();                        //Print points on std. output
 void PrintReaction();                          //Print reaction in statusbar;
 void ResetHistograms(Int_t );                  //Reset Histograms
 void ResetMatrix();                            //Clean matrix
 void SaveFitParam(const char *);               //Save fit parameters
 void SetFitMenu(Int_t);                        //Enable/disable the fit menu entries
 void SetMinuitParameters(Int_t, Double_t);     //Set minuit parameters
 void SetOffset(Bool_t val) {foffset=val;}      //Set foffset member
 void SetStatAZ();                              //Write status bar with Z and calculated A
 Int_t Si_CsiIdentification(Int_t );            //Si-CsI identification
 void UserPlot();                               //Start a panel for user defined matrix
 void Version(const char *);                    //Print current program version
 ClassDef(MDEEFrame, 0)
};

extern MDEEFrame *gfit;               //The unique global instance of this class permitted
#endif
