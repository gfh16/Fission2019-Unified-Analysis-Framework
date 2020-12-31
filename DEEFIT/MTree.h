#ifndef MTREE_H
#define MTREE_H

#include "deefit.h"

// the original tree structure 
struct MIdent {
 UShort_t numtel;
// raw parameters 
// Int_t desilpg,desilgg;               //desilpg/desilgg
 Float_t desipgf;                     //float desilpg
// Int_t time,time90;                   //Tof 30% and 90%
 UShort_t fastpg;  //,slowpg;                 //fast/slow (pg)
// Int_t fastgg,slowgg;                 //fast/slow (gg)
};


// MTree class interface
class MTree  {
private:
 TFile *ftree;     //A TFile class
 TTree *ftr;       //A TTree class
 Bool_t fok;       //TRUE is a file is opened
 TCanvas *fc1;     //A TCanvas class
 TGraph *fgr;      //A graph class containing the DEE-E matrix
 Bool_t fcanvas;   //TRUE if a canvas is present  
 MIdent *fevt;     //The tree event 
public:
 MTree() : fok(kFALSE),fcanvas(kFALSE) {ftree=0; fc1=0; fgr=0; fevt=0;}  //Constructor
 virtual ~MTree() {}                           //Destructor
 void SetTree(TFile *f) {ftree = f;}           //Set the file identificator 
 void SetOpen(Bool_t ok) {fok=ok;}             //Set true if file opened 
 Bool_t IsOpen() {return fok;}                 //Return true if file is opened
 Bool_t IsCanvas() {return fcanvas;}           //Return true if Canvas is opened 
 TFile *GetFile() {return ftree;}              //Return a TFile instance
 TTree *GetTree() {return ftr;}                //Return a TTree instance
 TCanvas *GetCanvas() {return fc1;}            //Return a canvas instance
 MIdent *GetEvent() {return fevt;}             //The tree event structure instance
 TGraph *GetGraph() {return fgr;}              //Return a TGraph instance 
 void SetGraph(TGraph *g) {fgr=g;}             //Assign a graph instance to fgr member  
 void OpenTree();                              //Open tree
 void DrawCanvas();                            //Draw a canvas 
 void ClosedCanvas() {fcanvas=kFALSE;}         //fcanvas false if a canvas is closed 
 void InitTree();                              //Init a Tree
 ClassDef(MTree, 0);
};

#endif


