// *********************************************************************
// MTree class implementation
// *********************************************************************
#include "MTree.h"

//Draw a canvas
void MTree::DrawCanvas()
{
 if((fc1 = (TCanvas*)gROOT->FindObject("c1"))!=0)delete fc1;
 fc1 = new TCanvas("c1","ExoChim",800,800);
 fcanvas = kTRUE;
 fc1->SetFillColor(0);
 fc1->Draw();
 gPad->Update();
// protect against canvas closing
 fc1->Connect("Closed()","MTree",this,"ClosedCanvas()");
}

//Init a Root Tree
void MTree::InitTree()
{
 if(IsOpen()) {   //if root file is opened
  OpenTree();     //assign ftr
  fevt = new MIdent;
  ftr->SetBranchAddress("numtel", &fevt->numtel);
//  ftr->SetBranchAddress("desilpg",&fevt->desilpg);
//  ftr->SetBranchAddress("desilgg",&fevt->desilgg);
  ftr->SetBranchAddress("desimev",&fevt->desimev);
//  ftr->SetBranchAddress("desipgf",&fevt->desipgf);
//  ftr->SetBranchAddress("time",   &fevt->time);
//  ftr->SetBranchAddress("time90", &fevt->time90);
  ftr->SetBranchAddress("ecsich", &fevt->ecsich);
//  ftr->SetBranchAddress("fastpg", &fevt->fastpg);
//  ftr->SetBranchAddress("fastgg", &fevt->fastgg);
//  ftr->SetBranchAddress("slowpg", &fevt->slowpg);
//  ftr->SetBranchAddress("slowgg", &fevt->slowgg);
 }
}


//Get a Tree file identificator. It is supposed the tree
//has the "h1" label
void MTree::OpenTree()
{
 ftr = (TTree*)ftree->Get("h1");
}
