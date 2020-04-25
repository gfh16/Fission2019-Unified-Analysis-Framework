#ifndef MGRAPHFRAME_H
#define MGRAPHFRAME_H
//////////////////////////////////////////////////////////////////////////
// MGraphFrame
// Author: E. De Filippo (INFN-CT) 2010 for the Deefit program
//
// This class creates a selection dialog. The user can define a         
// a bidimensional matrix specifying bins and low and max channels     
// for both axis. Also the percentage of events that have to be read     
// from the tree root file. If a valid root tree is opened for the      
// current telescope a matrix is drawn in full colors and with the 
// the selection done. This is useful to draw matrices for light particles
// like Z=1 and Z=2 ones where usual black and white matrices and high 
// statistics can make difficult the click drawn in order to selects 
// charges or masses.   
//////////////////////////////////////////////////////////////////////////
#include "TGFrame.h"
#include "TGTextBuffer.h"
#include "TGTextEntry.h"
#include "TGSlider.h"
#include "TGButton.h"
#include "TGStatusBar.h"

static char oldbuffer[256] = "500 0 4000 500 0 4000";

class MGraphFrame;  //forward
//This helper class retains the MGraphFrame info
class MInfo {
private:
 char *fbuffer;
 Int_t fperc;
 Bool_t fok;
 friend class MGraphFrame;  
public:
 MInfo() : fbuffer(0), fok(false), fperc(0)  {}
 ~MInfo() {if(fbuffer) delete [] fbuffer; fbuffer=0;}
 Int_t GetPerc() {return fperc;}
 Bool_t IsDone() {return fok;}
 char *GetBuffer() {return fbuffer;}
};

//Create a Selection dialog
class MGraphFrame : public TGTransientFrame
{
 private:
  TGTextBuffer *fdata;           //data buffer from text-entry
  TGTextEntry *fhidim;           //text entry widget
  TGHSlider *fslperc;            //slider widget
  TGStatusBar *fStatus;          //Status bar
  TGTextButton *fdraw,*fexit;    //Draw button and exit buttons  
  MInfo *finfo;                  //Return info;     
 public:
 MGraphFrame(const TGWindow *p = 0, const TGWindow *main = 0, MInfo *info=0); //Constructor
 ~MGraphFrame();                                       //Destructor
 void GetData();                                       //Retrieve data
 void CloseWindow();                                   //Close Window
 void PrintSlider();                                   //Print slider value in status bar
 ClassDef(MGraphFrame,0)                               //spectra selection dialog
};
#endif
