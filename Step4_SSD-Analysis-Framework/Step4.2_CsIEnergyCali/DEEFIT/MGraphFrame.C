////////////////////////////////
// MGraphFrame implementation 
// Author: Enrico De Filippo (2010)
// v0.2 for deefit 1.1
////////////////////////////////

#include "MGraphFrame.h"
#include "TGLabel.h"
#include "TG3DLine.h"


//Constructor
//Define the graphical interface 
MGraphFrame::MGraphFrame(const TGWindow *p, const TGWindow *main, MInfo *info) :
 TGTransientFrame(p, main, 10, 10, kVerticalFrame)
{
 finfo = info;
 SetCleanup(kDeepCleanup);
 Connect("CloseWindow()", "MGraphFrame", this, "CloseWindow()");
 DontCallClose();

 TGHorizontalFrame *htop = new TGHorizontalFrame(this, 10, 10);
 TGLabel *label = new TGLabel(htop,"HISTOGRAM: ");
 fdata = new TGTextBuffer(128);
 fhidim = new TGTextEntry(htop, fdata, -1);
 fhidim->SetWidth(200);
 fhidim->SetText(oldbuffer);
 fhidim->SetToolTipText("BinX XMin XMax BinY YMin YMax");

 htop->AddFrame(label, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 5, 2, 2));
 htop->AddFrame(fhidim, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 5, 2, 2));
 AddFrame(htop, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 4, 4, 3, 3));

 TGHorizontalFrame *hmiddle = new TGHorizontalFrame(this, 10, 10);
 TGLabel *labper = new TGLabel(hmiddle,"Event's percentage:");
 fslperc = new TGHSlider(hmiddle,178, kSlider2 | kScaleBoth);
 fslperc->SetRange(0,100);
 fslperc->SetPosition(10); 
 TGHorizontal3DLine *fline = new TGHorizontal3DLine(hmiddle);

 hmiddle->AddFrame(labper, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 5, 2, 2));
 hmiddle->AddFrame(fslperc, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 5, 2, 2));
 hmiddle->AddFrame(fline, new TGLayoutHints(kLHintsLeft | kLHintsTop | 
                   kLHintsExpandX, -310, 5, 50, 2));
 AddFrame(hmiddle, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 4, 4, 3, 3));
 
 TGHorizontalFrame *hbottom = new TGHorizontalFrame(this, 10, 10);
 TGTextButton *fdraw = new TGTextButton(hbottom,"  DONE  ");
 TGTextButton *fexit = new TGTextButton(hbottom," CANCEL ");
 hbottom->AddFrame(fdraw, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 10, 2));
 hbottom->AddFrame(fexit, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 35, 5, 10, 2));
 AddFrame(hbottom, new TGLayoutHints(kLHintsTop | kLHintsCenterX, 0, 0, 3, 3)); 

 // Create status bar
 Int_t parts[] = { 80, 20 };
 fStatus = new TGStatusBar(this, 10, 10);
 fStatus->SetParts(parts, 2);
 fStatus->SetText("COLZ",1);
 AddFrame(fStatus, new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 2, 2, 1, 1));


 fexit->Connect("Clicked()","MGraphFrame",this,"CloseWindow()"); 
 fdraw->Connect("Clicked()","MGraphFrame",this,"GetData()");
 fslperc->Connect("Released()","MGraphFrame",this,"PrintSlider()");

 MapSubwindows();
 TGDimension size = GetDefaultSize();
 Resize(size);
 CenterOnParent();

 SetWMSize(size.fWidth, size.fHeight);
 SetWMSizeHints(size.fWidth, size.fHeight, size.fWidth, size.fHeight, 0, 0);

 const char *wname = "User Matrix Setup";
 SetWindowName(wname);
 SetMWMHints(kMWMDecorAll | kMWMDecorResizeH  | kMWMDecorMaximize |
                            kMWMDecorMinimize | kMWMDecorMenu,
              kMWMFuncAll | kMWMFuncResize    | kMWMFuncMaximize |
                            kMWMFuncMinimize,
               kMWMInputModeless);
 

 MapWindow();
 PrintSlider();
 fClient->WaitFor(this);
}

//Destructor
MGraphFrame::~MGraphFrame()
{ 
 
}

void MGraphFrame::CloseWindow()
{
 finfo->fperc = 0;
 finfo->fok = false;
 DeleteWindow(); 
}

void MGraphFrame::GetData()
{
 int len = fdata->GetTextLength();
 if(finfo->fbuffer) {
  delete [] finfo->fbuffer;
  finfo->fbuffer = 0;
 }
 finfo->fbuffer = new char[len+1];
 strcpy(finfo->fbuffer, fdata->GetString());
 strcpy(oldbuffer, fdata->GetString());
 finfo->fperc = fslperc->GetPosition();
 finfo->fok = true;
 DeleteWindow(); 
}

//Print the current selection in status bar
void MGraphFrame::PrintSlider()
{
 char msg[64];
 sprintf(msg, "Event's Percentage %d (%%)",fslperc->GetPosition());
 fStatus->SetText(msg,0);
}



