///////////////////////////////////////////////
//   MDEEFrame class implementation
///////////////////////////////////////////////
#include "MDEEFrame.h"
#include "MGraphFrame.h"

MDEEFrame *gfit=0;                 //a global pointer and unique instance of MDEEFrame
vector<MPart> MDEEFrame::fparray;  //definition of static fparray

// Main file types
const char *filetypes[] = { "All files",     "*",
                            "ROOT files",    "*.root",
                            "ROOT macros",   "*.C",
                            "Text files",    "*.[tT][xX][tT]",
                            0,               0 };

const char *histotit[] = {" ZETA ALL ", "MASS Z=1 [H]", "MASS Z=2 [He]", "MASS Z=3 [Li]",
"MASS Z=4 [Be]", "MASS Z=5 [B]", "MASS Z=6 [C]", "MASS Z=7 [N]", "MASS Z=8 [O]",
"MASS Z=9 [F]", "MASS Z=10 [Ne]", "MASS Z=11 [Na]", "MASS Z=12 [Mg]", "MASS Z=13 [Al]",
"MASS Z=14 [Si]"};

//*********************************************************************
// MDEEFrame constructor
//*********************************************************************
MDEEFrame::MDEEFrame(const TGWindow *p, UInt_t w, UInt_t h, MEpax &epax) : TGMainFrame(p,w,h),
 fepax2(epax)
{
 if(gfit!=0) {
  cout<<"Only one instance of MDEEFrame permitted"<<endl;
  return;
 }
 gfit = this;
 ftimer = new TTimer(this, 20, kTRUE);
 fcklist = new TList;
 fNPoint = 0;
 fLastAnalysis = -1;
 InitFitFunction(kFitStandard);

 ftel_curr = 0;
 for(Int_t i=0; i<1192; i++) {
  fvpar[i]=0;
 }
 fminuit = new TMinuit(MAXFP); //maxfp is the max number of params for this object
 fdrawone = kFALSE;
 Version("1.3");
 CreateMenu();
// General Layout
 fhor1 = new TGHorizontalFrame(this, 400, 50, kFixedSize);
 ftlab = new TGLabel(fhor1,"TELESCOPE NUMBER:");
 fhor1->AddFrame(ftlab, new TGLayoutHints(kLHintsLeft|kLHintsTop,4,1,17,1));
 ftel = new TGNumberEntry(fhor1, 0, 5, 1, TGNumberFormat::kNESInteger,
                                          TGNumberFormat::kNEANonNegative,
                                          TGNumberFormat::kNELLimitMinMax,
                                               0, 1191);
 fhor1->AddFrame(ftel, new TGLayoutHints(kLHintsLeft | kLHintsTop,10,1,12,1));

 fplot = new TGTextButton(fhor1," PLOT ");
 fplot->SetToolTipText("PLOT DE-E Matrix");
 fhor1->AddFrame(fplot, new TGLayoutHints(kLHintsLeft|kLHintsTop,10,1,12,1));

 frplot = new TGTextButton(fhor1," CLEAR ");
 frplot->SetToolTipText("REDRAW DE-E MATRIX");
 fhor1->AddFrame(frplot, new TGLayoutHints(kLHintsLeft|kLHintsTop,10,1,12,1));

 fusgraph = new TGTextButton(fhor1," USER ");
 fusgraph->SetToolTipText("PLOT USER DEFINED\n COLOR MATRIX");
 fhor1->AddFrame(fusgraph, new TGLayoutHints(kLHintsLeft|kLHintsTop,10,1,12,1));

 TGHorizontal3DLine *fline1 = new TGHorizontal3DLine(fhor1);
 fhor1->AddFrame(fline1, new TGLayoutHints(kLHintsLeft|kLHintsTop|kLHintsExpandX,-400,1,48,1));
 AddFrame(fhor1,new TGLayoutHints(kLHintsLeft,1,1,1,1));



 fhor2 = new TGHorizontalFrame(this, 400, 70, kFixedSize);
 flzeta = new TGLabel(fhor2,"CHARGE Z:");
 fZ = new TGNumberEntry(fhor2, 1, 5, 1, TGNumberFormat::kNESInteger,
                                               TGNumberFormat::kNEANonNegative,
                                               TGNumberFormat::kNELLimitMinMax,
                                               1, 60);
 fhor2->AddFrame(flzeta, new TGLayoutHints(kLHintsLeft|kLHintsTop,4,1,17,1));
 fhor2->AddFrame(fZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,10,1,12,1));

 flmass = new TGLabel(fhor2,"MASS M:");
 fM = new TGNumberEntry(fhor2, 1, 5, 1, TGNumberFormat::kNESInteger,
                                               TGNumberFormat::kNEANonNegative,
                                               TGNumberFormat::kNELLimitMinMax,
                                               1, 150);
 fhor2->AddFrame(flmass, new TGLayoutHints(kLHintsLeft|kLHintsTop,30,1,17,1));
 fhor2->AddFrame(fM, new TGLayoutHints(kLHintsLeft|kLHintsTop,10,1,12,1));

 fdraw = new TGTextButton(fhor2," DRAW ");
 fdraw->SetToolTipText("PLEASE, DRAW CLICKS");
 fhor2->AddFrame(fdraw, new TGLayoutHints(kLHintsLeft|kLHintsTop,30,1,12,1));

 fProgressBar = new TGHProgressBar(fhor2);
 fProgressBar->SetBarColor("green");
 fProgressBar->SetFillType(TGProgressBar::kBlockFill);
 fhor2->AddFrame(fProgressBar, new
         TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, -376,4,46,3));
 AddFrame(fhor2,new TGLayoutHints(kLHintsLeft,1,1,1,1));

 fhor3 = new TGHorizontalFrame(this, 400, 42, kFixedSize);

 flhist = new TGLabel(fhor3,"HISTOGRAM SELECTION:");
 fhor3->AddFrame(flhist, new TGLayoutHints(kLHintsLeft|kLHintsTop,4,1,13,1));
 fComboHisto = new TGComboBox(fhor3,100,kHorizontalFrame|kSunkenFrame|kDoubleBorder|
                              kOwnBackground);

 const char **ptit = histotit;
 for(Int_t i=0; i<MAXMAS; i++) {
  fComboHisto->AddEntry(*ptit,i);
  ptit++;
 }
 fComboHisto->Resize(130,22);
 fComboHisto->Select(0);
 fhor3->AddFrame(fComboHisto, new
         TGLayoutHints(kLHintsTop | kLHintsLeft, 10,1,9,1));
 fphist = new TGPictureButton(fhor3, gClient->GetPicture("h1_s.xpm"));
 fphist->Resize(35,35);
 fphist->SetToolTipText("DRAW HISTOGRAMS");
 fhor3->AddFrame(fphist, new TGLayoutHints(kLHintsLeft|kLHintsTop,15,1,7,1));

 TGHorizontal3DLine *fline3 = new TGHorizontal3DLine(fhor3);
 fhor3->AddFrame(fline3, new TGLayoutHints(kLHintsLeft|kLHintsTop|kLHintsExpandX,-400,1,1,1));
 AddFrame(fhor3,new TGLayoutHints(kLHintsLeft,1,1,1,1));
 // Create status bar
 int parts[] = { 20,20,20,40};
 fStatus = new TGStatusBar(this, 10, 10);
 fStatus->SetParts(parts, 4);
 fStatus->SetText("A = 2*Z (A>4)",1);
 fStatus->SetText("Standard Fit",2);
 AddFrame(fStatus, new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 2, 2, 1, 1));

//connect buttons to slot
 fdraw->Connect("Clicked()","MDEEFrame",this,"DrawPoint()");
 fplot->Connect("Clicked()","MDEEFrame",this,"GraphMatrix()");
 frplot->Connect("Clicked()","MDEEFrame",this,"ResetMatrix()");
 fphist->Connect("Clicked()","MDEEFrame",this,"DrawHistogram()");
 fusgraph->Connect("Clicked()","MDEEFrame",this,"UserPlot()");
//connect charge value to status bar
 fZ->Connect("ValueSet(Long_t)", "MDEEFrame", this, "SetStatAZ()");
 (fZ->GetNumberEntry())->Connect("ReturnPressed()", "MDEEFrame", this, "SetStatAZ()");
 ftel->Connect("ValueSet(Long_t)","MDEEFrame", this, "DisableFit()");
 (ftel->GetNumberEntry())->Connect("ReturnPressed()", "MDEEFrame", this, "DisableFit()");

 SetCleanup(kDeepCleanup);
//set fixed size
 SetWMSizeHints(GetDefaultWidth(), GetDefaultHeight(), GetDefaultWidth(),
                GetDefaultHeight(), 0, 0);
 fProgressBar->SetPosition(0);
 fProgressBar->ShowPosition();
 Connect("CloseWindow()","TApplication",gApplication,"Terminate(=0)");
 SetWindowName("Chimera DEEFIT");
 MapSubwindows();
 Resize();
 MapWindow();
 PrintReaction();
}

// destructor
MDEEFrame::~MDEEFrame()
{
 cout<<"Cleaning up...bye"<<endl;
 Cleanup();
 gfit = 0;
}

// Ask to confirm if an existing file can be overwritten
Bool_t MDEEFrame::AskToWrite()
{
 int ret = 0;

 new TGMsgBox(gClient->GetRoot(), this,
              "Question", "File already exists. Overwrite ?",
              kMBIconQuestion, kMBYes | kMBNo, &ret);

 if (ret == kMBYes) return kTRUE;
 return kFALSE;
}


int MDEEFrame::CheckZResults(Int_t numtel_curr)
{
 char retstr[256];
 int Zval;

 if(!fmtree.IsOpen()) {
  cout<<"A valid root file should be opened"<<endl;
  return -1;
 }
 TTree *t1 = fmtree.GetTree();
 if(t1==0) {
  cout<<"No valid tree is opened to read data"<<endl;
  return -1;
 }

 if(fLastAnalysis !=numtel_curr) {
  cout<<"No coherence between the current telescope and last identification analysis"<<endl;
  return -1;
 }

 new TGInputDialog(gClient->GetRoot(), this, "Charge Z to check:", "6",retstr);
 Zval = atoi(retstr);

 stringstream ss;
 ss<<"numtel=="<<numtel_curr<<" && charge=="<<Zval;
 stringstream nss;
 nss<<"numtel=="<<numtel_curr<<" && charge!="<<Zval;

 TFriendElement *frch = t1->AddFriend("treeide","./treeide.root");
 TCanvas *cc = new TCanvas("cc", "CHECK ZETA",700,600);
 cout<<"Please wait...this procedure takes time. [Z="<<Zval<<" checking]"<<endl;
 t1->SetMarkerColor(2);
 t1->Draw("desimev:ecsich",ss.str().c_str());
 cc->Update();
 t1->SetMarkerColor(4);
 t1->Draw("desimev:ecsich",nss.str().c_str(),"same");
 t1->SetMarkerColor(1);
}

//paolo->
int MDEEFrame::CheckMResults(Int_t numtel_curr)
{
 char Zretstr[256],Aretstr[256];
 int Zval,Aval;

 if(!fmtree.IsOpen()) {
  cout<<"A valid root file should be opened"<<endl;
  return -1;
 }
 TTree *t1 = fmtree.GetTree();
 if(t1==0) {
  cout<<"No valid tree is opened to read data"<<endl;
  return -1;
 }

 if(fLastAnalysis !=numtel_curr) {
  cout<<"No coherence between the current telescope and last identification analysis"<<endl;
  return -1;
 }

 new TGInputDialog(gClient->GetRoot(), this, "Charge Z to check:", "6",Zretstr);
 new TGInputDialog(gClient->GetRoot(), this, "Mass A to check:", "12",Aretstr);
 Zval = atoi(Zretstr);
 Aval = atoi(Aretstr);

 stringstream ss;
 ss<<"numtel=="<<numtel_curr<<" && charge=="<<Zval<<" && mass>="<<Aval-0.5<<" && mass<="<<Aval+0.5;
 cout << ss.str().c_str() << endl;
 stringstream nss;
 nss<<"numtel=="<<numtel_curr<<" && charge=="<<Zval<<" && (mass<="<<Aval-0.5<<"|| mass>="<<Aval+0.5<<")";
 cout << nss.str().c_str() << endl;

 stringstream nssp1;
 nssp1<<"numtel=="<<numtel_curr<<" && charge=="<<Zval+1;
 cout << nssp1.str().c_str() << endl;

 stringstream nssm1;
 nssm1<<"numtel=="<<numtel_curr<<" && charge=="<<Zval-1;
 cout << nssm1.str().c_str() << endl;

 TFriendElement *frch = t1->AddFriend("treeide","./treeide.root");
 TCanvas *cc = new TCanvas("cc", "CHECK ZETA and MASS",700,600);
 cout<<"Please wait...this procedure takes time. [Z A ="<<Zval<<" "<< Aval << " checking]"<<endl;
 t1->SetMarkerColor(2);
 t1->Draw("desimev:ecsich",ss.str().c_str());
 cc->Update();
 t1->SetMarkerColor(4);
 t1->Draw("desimev:ecsich",nss.str().c_str(),"same");
 t1->SetMarkerColor(1);

 t1->SetMarkerColor(3);
 t1->Draw("desimev:ecsich",nssp1.str().c_str(),"same");
 t1->SetMarkerColor(1);

 t1->SetMarkerColor(6);
 t1->Draw("desimev:ecsich",nssm1.str().c_str(),"same");
 t1->SetMarkerColor(1);
}
//paolo<-

// Copy a telescope cuts to the current one
void MDEEFrame::CopyLines()
{
 char retstr[256];
 Int_t newtel,n;
 stringstream ss,sn;
 const char *title;
 string oldname,newname,newtitle;
 TGraph *pl, *plnew=0;

 new TGInputDialog(gClient->GetRoot(), this, "Telescope number to copy:", "-1",retstr);
 if(strcmp(retstr,"-1")!=0) {
  newtel = atoi(retstr);
  Int_t numtel_curr = ftel->GetNumberEntry()->GetIntNumber();
  if(newtel>=0 && newtel<1192 && newtel!=numtel_curr) {
   sn<<"t"<<numtel_curr;
   ss<<"t"<<newtel;
   n = fcklist->GetEntries();
   for(Int_t i=0; i<n; i++) {
    pl = (TGraph*)fcklist->At(i);
    title = pl->GetTitle();
    if(strcmp(title,ss.str().c_str())==0) {
     oldname = pl->GetName();
     Int_t i=oldname.find("_");
     newname = oldname.replace(i+1,4,sn.str());
     newtitle=sn.str();
     plnew = dynamic_cast<TGraph*>(pl->Clone(newname.c_str()));
     plnew->SetTitle(newtitle.c_str());
     TGraph *res = (TGraph*)fcklist->FindObject(newname.c_str());
     if(res==0) {
      fcklist->Add(plnew);
     }
     else {
      cout<<"Object: "<<newname<<" already in the list. Replace refused"<<endl;
     }
    }
   }
  }
 }
 cout<<"There are "<<fcklist->GetEntries()<<" clicks objects in the list"<<endl;
}


//create histograms for data analysis
void MDEEFrame::CreateHistograms(int ntel)
{
 stringstream ss,ss1;
 ss1<<"hz"<<ntel;
 fvpar[ntel]->fhz[0] = new TH1F(ss1.str().c_str(),"Charge distribution",350,0.5,60.5);
 ss1.str("");
 for(Int_t i=1; i<MAXMAS; i++) {
  ss<<"hm"<<ntel<<"_"<<i;
  ss1<<"Mass [Z="<<i<<"]";
  if(i<=9) {
   fvpar[ntel]->fhz[i] = new TH1F(ss.str().c_str(),ss1.str().c_str(),200,0.5,30.5);
  }
  else {
   fvpar[ntel]->fhz[i] = new TH1F(ss.str().c_str(),ss1.str().c_str(),200,10.5,40.5);
  }
  ss.str("");
  ss1.str("");
 }
}

// create the main popup menu
void MDEEFrame::CreateMenu()
{
 fMenuFile = new TGPopupMenu(gClient->GetRoot());
 fMenuFile->AddEntry("&Open file",      kFileOpen);
 fMenuFile->AddEntry("&Close",          kFileClose);
 fMenuFile->AddEntry("&Quit ROOT",      kFileQuit);

 fMenuLine = new TGPopupMenu(gClient->GetRoot());
 fMenuLine->AddEntry("&Load file",      kLineOpen);
 fMenuLine->AddEntry("&Save",           kLineSave);
 fMenuLine->AddEntry("Save &As...",     kLineSaveAs);
 fMenuLine->AddEntry("S&pline",         kLineSpline);
 fMenuLine->AddEntry("&Graph",          kLineDraw);
 fMenuLine->AddEntry("&Copy",           kLineCopy);
 fMenuLine->AddEntry("L&ist",           kLineList);
 fMenuLine->AddEntry("&Print Points",   kLineAscii);
 fMenuLine->AddSeparator();
 fMenuLine->AddEntry("&Delete All",     kLineDelete);
 fMenuLine->AddEntry("D&elete Curr",    kLineDelgroup);
 fMenuLine->AddEntry("Delete &One",     kLineDelOne);

 fMenuFit = new TGPopupMenu(gClient->GetRoot());
 fMenuFit->AddEntry("&Init Fit",       kFitParam);
 fMenuFit->AddEntry("&Fit",            kFitExec);
 fMenuFit->AddEntry("&Graph One",      kFitDrawOne);
 fMenuFit->AddEntry("Graph &All",      kFitDrawAll);
 fMenuFit->AddSeparator();
 fMenuFit->AddEntry("S&tandard (9 param)",  kFitStandard);
 fMenuFit->AddEntry("E&nhanced (14 param)",  kFitEnhanced);
 fMenuFit->RCheckEntry(kFitStandard,kFitStandard,kFitEnhanced);
 ffitkind = kstandard;
 fMenuFit->AddSeparator();
 fMenuFit->AddEntry("Mass = &Charity", kFitCharity);
 fMenuFit->AddEntry("Mass = &EPAX2",   kFitEpax);
 fMenuFit->AddEntry("Mass = 2*&Zeta",  kFitTwo);
 fMenuFit->RCheckEntry(kFitTwo,kFitCharity,kFitTwo);
 fmcalc = k2;
 fMenuFit->AddSeparator();
 fMenuFit->AddEntry("&Save Param",   kFitSave);
 fMenuFit->AddEntry("&Load Param",   kFitLoad);
 fMenuFit->AddEntry("Load &From...",  kFitFile);


 for(Int_t i=kLineSave; i<=kLineSaveAs; i++) {
  fMenuLine->DisableEntry(i);
 }

 //disable the menu entries
 SetFitMenu(0);

 fMenuAna = new TGPopupMenu(gClient->GetRoot());
 fMenuAna->AddEntry("&Identification", kAnaIdent);
 fMenuAna->AddEntry("&Draw", kAnaDraw);
 fMenuAna->AddEntry("Draw all &Masses", kAnaMass);
 fMenuAna->AddEntry("&Check Z identification", kAnaCheck);
 fMenuAna->AddEntry("&Check Mass identification", kAnaCheck2); //paolo

 fMenuSetup = new TGPopupMenu(gClient->GetRoot());
 fMenuSetup->AddEntry("&System", kSetupReac);


// Menu bar layout
 fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
 fMenuBarLayout = new TGLayoutHints(kLHintsTop|kLHintsExpandX , 1, 1, 1, 1);
 fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
 fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
 fMenuBar->AddPopup("&Clicks", fMenuLine, fMenuBarItemLayout);
 fMenuBar->AddPopup("Fi&t", fMenuFit, fMenuBarItemLayout);
 fMenuBar->AddPopup("&Analysis", fMenuAna, fMenuBarItemLayout);
 fMenuBar->AddPopup("&Setup", fMenuSetup, fMenuBarItemLayout);
 AddFrame(fMenuBar, fMenuBarLayout);
 TGHorizontal3DLine *fline1 = new TGHorizontal3DLine(this);
 AddFrame(fline1, new TGLayoutHints(kLHintsLeft|kLHintsTop|kLHintsExpandX,1,1,3,1));
//connect file menu
 fMenuFile->Connect("Activated(Int_t)", "MDEEFrame", this,
                      "HandleMenu(Int_t)");
 fMenuLine->Connect("Activated(Int_t)", "MDEEFrame", this,
                      "HandleMenuLine(Int_t)");
 fMenuFit->Connect("Activated(Int_t)", "MDEEFrame", this,
                      "HandleMenuFit(Int_t)");
 fMenuAna->Connect("Activated(Int_t)", "MDEEFrame", this,
                      "HandleMenuAna(Int_t)");
 fMenuSetup->Connect("Activated(Int_t)", "MDEEFrame", this,
                      "HandleMenuSetup(Int_t)");
}


// Delete cuts for the current telescope
void MDEEFrame::DeleteGroup(Int_t curr_tel)
{
 stringstream ss,sn;
 const char *title;

 ss<<"t"<<curr_tel;
 TList *p,*q;
 TIter next(fcklist);
 while ((p = (TList*)next())) {
  title = p->GetTitle();
  if(strcmp(title,ss.str().c_str())==0) {
   q = (TList*)fcklist->Before(p);
   fcklist->Remove(p);
   delete p;
   p = q;
  }
 }
 cout<<"Group "<<ss.str()<<" deleted"<<endl;
 cout<<"There are "<<fcklist->GetEntries()<<" clicks objects in the list"<<endl;
}

// delete one cut from the current telescope
void MDEEFrame::DeleteOne(Int_t curr_tel)
{
 char retstr[256];
 stringstream ss;
 const char *title;

 new TGInputDialog(gClient->GetRoot(), this, "Cut name [es Z2A4_t10]:", "",retstr);
 if(strcmp(retstr,"")!=0) {
  ss<<"t"<<curr_tel;
  TGraph *p = (TGraph*)fcklist->FindObject(retstr);
  if(p==0) {
   cout<<"Cut not found"<<endl;
   return;
  }
  else {
   title = p->GetTitle();
   if(strcmp(title,ss.str().c_str())==0) {
    fcklist->Remove(p);
    delete p;
    cout<<"Cut "<<retstr<<" deleted"<<endl;
   }
   else {
    cout<<"You can not delete a cut not belonging to the current telescope"<<endl;
   }
  }
 }
 cout<<"There are "<<fcklist->GetEntries()<<" clicks objects in the list"<<endl;
}

//Slot function, disable menu fit
void MDEEFrame::DisableFit()
{
 fMenuFit->DisableEntry(kFitExec);
// fMenuFit->DisableEntry(kFitDrawOne);
// fMenuFit->DisableEntry(kFitDrawAll);
}

//Draw several fit-lines for a given Z and A
void MDEEFrame::DrawAllFit(Int_t ntel)
{
 Double_t param[MAXFP], err[MAXFP];
 Int_t i,bins = 60;
 Double_t xg[100], yg[100],x,fmin,step;
 MPart p;
 Int_t col[3] = {2,4,6};
 char retstr[256];
 Int_t zmax=20;
 Double_t *par;
 Int_t npar;

 new TGInputDialog(gClient->GetRoot(), this, "ZMAX:", "20",retstr);
 if(strcmp(retstr,"")!=0) {
  zmax = atoi(retstr);
 }

 fdrawone = kFALSE;
 foffset = kTRUE;
// for(Int_t i=0; i<fNpar; i++) {
//  fminuit->GetParameter(i,param[i],err[i]);
// }
//Take fit parameters from fvpar vector. A new fit overrides
//the values loaded from a file.
 npar = 0;
 if(fvpar[ntel]!=0) {
  npar = fvpar[ntel]->GetNpar();
  par = fvpar[ntel]->GetParam();
  for(Int_t i=0; i<npar; i++) {
   param[i] = par[i];
  }
 }

 if(npar == 0) {
  return;
 }

 fmin = param[npar-1]+5.0; //fNpar-1 = of_f (offset X, last fit parameter)
 step = (1500 - fmin)/bins;
 TGraph grfit(bins+1);
 grfit.SetLineWidth(3);
 grfit.SetName("fit");

 Int_t k=0;
 if(fmtree.IsCanvas()) {
  for(Int_t j=1; j<=zmax; j++) {
   p.Z = j;
   if(j<=4) {
    p.A = gLight[j-1];
   }
   else {
    if(fMenuFit->IsEntryRChecked(kFitCharity)) {
     p.A = GetMassCharity(p.Z);
    }
    else if(fMenuFit->IsEntryRChecked(kFitTwo)) {
     p.A = 2*j;
    }
    else if(fMenuFit->IsEntryRChecked(kFitEpax)) {
//     p.A = fepax2.GetMassEpax(p.Z);
     p.A = fepax2.GetEMass((Int_t)p.Z);
    }
   }
   cout<<"Z="<<j<<"  A="<<p.A<<endl;
   x=fmin;
   i=0;
   while(i<=bins) {
    p.x = x;
    xg[i] = x;
    yg[i] = (this->*fptr)(p,param);
//    cout<<i<<" "<<xg[i]<<" "<<yg[i]<<"   "<<p.Z<<" "<<p.A<<endl;
    x+=step;
    i++;
   }
   grfit.SetLineColor(col[k]);
   grfit.DrawGraph(bins+1,xg,yg,"L");
   k++;
   if(k>2)k=0;
   fmtree.GetCanvas()->Modified();
   fmtree.GetCanvas()->Update();
  }
 }
}


//Slot function for picture button
void MDEEFrame::DrawHistogram()
{
 Int_t numt = ftel->GetNumberEntry()->GetIntNumber();
 DrawHistogram(numt);
}


//Draw histogram for a given ntel
void MDEEFrame::DrawHistogram(int ntel)
{
 TH1F *p;
 TCanvas *ctemp;

 if(fvpar[ntel]==0)return;
 if(fvpar[ntel]->fhz[0]==0) {
  cout<<"No histogram found"<<endl;
  return;
 }
 Int_t n = fComboHisto->GetSelected();
 p = fvpar[ntel]->fhz[n];
 if((ctemp = (TCanvas*)gROOT->FindObject("c2"))!=0)delete ctemp;
 TCanvas *c2 = new TCanvas("c2", "DE-E Identification",170,140,700,500);
 c2->SetBorderSize(2);
 c2->SetFrameFillColor(0);
 c2->SetGridx();
 if(n!=0) {
  c2->SetLogy(kFALSE);
 }
 else {
  if(p->GetEntries() != 0.0)
   c2->SetLogy();
  else
   c2->SetLogy(kFALSE);
 }
 p->SetLineWidth(2);
 p->Draw();
}


//Draw mass spectra for each Z in a
//multipad canvas
void MDEEFrame::DrawMasses(int ntel)
{
 TH1F *p;
 TCanvas *cMass=0;
 Float_t xm,xM;

 if((cMass = (TCanvas *)gROOT->GetListOfCanvases()->FindObject("cMass")) != 0) {
  delete cMass;
 }
 if(fvpar[ntel]==0)return;
 if(fvpar[ntel]->fhz[0]==0) {
  cout<<"No histogram found"<<endl;
  return;
 }
 cMass = new TCanvas("cMass", "Mass Spectra",900,850);
 cMass->Divide(3,3);
 for(Int_t i=1; i<=9; i++) {
  TH1F *p = fvpar[ntel]->fhz[i];
  TVirtualPad *pwin = cMass->cd(i);
  pwin->SetGridx();
  p->SetLineWidth(3);
  p->SetLineColor(4);
  if(i<=3) {
   xm = 0;
   xM = 10;
  }
  else if(i>3 && i<=6) {
   xm = 5;
   xM = 15;
  }
  else {
   xm = 10;
   xM = 25;
  }
  p->GetXaxis()->SetRangeUser(xm,xM);
  p->Draw();
 }
 cMass->cd(1);
}


//Draw one fit-line for a given Z and A
void MDEEFrame::DrawOneFit(Int_t ntel)
{
 Double_t param[MAXFP], err[MAXFP];
 Double_t *par;
 Int_t npar;
 fdrawone = kTRUE;

// for(Int_t i=0; i<fNpar; i++) {
//  fminuit->GetParameter(i,param[i],err[i]);
// }
// take fit parameters from fvpar vector. A new fit overrides
// the values loaded from a file.

//delete an existing fitdee object
 TF1 *res = (TF1*)gROOT->FindObject("fitdee");
 if(res!=0) {
  delete res;
  fmtree.GetCanvas()->Update();
 }

 npar = 0;
 if(fvpar[ntel]!=0) {
  npar = fvpar[ntel]->GetNpar();
  par = fvpar[ntel]->GetParam();
  for(Int_t i=0; i<npar; i++) {
   param[i] = par[i];
  }
 }

 if(npar == 0) {
  fdrawone = kFALSE;
  return;
 }

 Bool_t IsCoherent = GetAnaCoherence(ntel);
 if(!IsCoherent) {
  cout<<"Warning: mismatch between current fit setting and data"<<endl;
  cout<<"Please, change fit setting (Standard or Enhanced) in fit menu"<<endl;
 }

 Double_t fmin = param[npar-1];  //minimun X offset to draw fit (of_f)
 if(fmtree.IsCanvas()) {
  TF1* funf = new TF1("fitdee",gfit,fmin,2000.,npar);
  funf->SetParameters(param);
  funf->SetLineColor(2);
  funf->Draw("same");
  fmtree.GetCanvas()->Update();
 }
 fdrawone = kFALSE;
}

// Draw graphical cuts. Slot functon
// for the DRAW button
void MDEEFrame::DrawPoint()
{
 Int_t Z,M;
 TGraph *pl=0;
 Int_t n=0;
 Double_t x,y;
 stringstream ss,stit;
 TCanvas *c1;

 if(!fmtree.IsCanvas())return;
 Z = fZ->GetNumberEntry()->GetIntNumber();
 M = fM->GetNumberEntry()->GetIntNumber();
 Int_t numtel = ftel->GetNumberEntry()->GetIntNumber();
 cout<<"Z= "<<Z<<"\tM= "<<M<<endl;
 stit<<"t"<<numtel;
 ss<<"Z"<<Z<<"A"<<M<<"_"<<stit.str();
 c1 = fmtree.GetCanvas();
 gStyle->SetLineColor(2);
 gStyle->SetLineWidth(2);
 gStyle->SetMarkerStyle(3);
 gStyle->SetMarkerColor(4);
 gStyle->SetMarkerSize(1.2);
 c1->WaitPrimitive("Graph","PolyLine");
 pl = (TGraph *)gPad->GetListOfPrimitives()->Last();
 pl->SetName(ss.str().c_str());
 pl->SetTitle(stit.str().c_str());
 n = pl->GetN();
 TGraph *res = (TGraph*)fcklist->FindObject(pl->GetName());
 pl->Draw("p");
 if(n>0) {
  for(Int_t i=0; i<n; i++){
   pl->GetPoint(i,x,y);
   cout<<stit.str()<<":"<<Z<<"  "<<M<<"  "<<x<<"  "<<y<<endl;
  }
 }
 if(res==0) {
  fcklist->Add(pl);   //add line to the list
 }
 else {
  cout<<"Sorry: cut with the same name is already in the list"<<endl;
  delete pl;
 }
 Int_t na = fcklist->GetEntries();
 cout<<"There are "<<na<<" clicks objects in the list"<<endl;
 if(na>0) {
  fMenuLine->EnableEntry(kLineSave);
  fMenuLine->EnableEntry(kLineSaveAs);
 }
}


// Do the fit using the Migrad procedure by Minuit
// and the chi2 minimization
void MDEEFrame::ExecuteFit()
{
 Double_t arglist[10];
 Int_t iflag=0;
 Double_t param[MAXFP], err[MAXFP];
 Double_t amin=0.0,edm=0.0,errdef=0.0,chisquare=0.0;
 Int_t nvpar,nparx,icstat;

 fminuit->SetPrintLevel(-1);

// Migrad fit
 arglist[0] = 10000;
 fminuit->mnexcm("SIMPLEX",arglist,1,iflag);
 fminuit->SetPrintLevel(0);
 fminuit->mnexcm("MIGRAD",arglist,1,iflag);
// fminuit->mnexcm("MINOS",arglist,1,iflag);
 for(Int_t i=0; i<fNpar; i++) {
  fminuit->GetParameter(i,param[i],err[i]);
//  cout<<param[i]<<" "<<err[i]<<endl;
 }
 Int_t n=fparray.size();
 foffset = kTRUE;
 for(Int_t i=0; i<n; i++) {
  Double_t th = (this->*fptr)(fparray[i],param);
  printf("%2.0f  %3.0f  %9.3f  %9.3f   %9.3f  [%9.3e]\n",fparray[i].Z,fparray[i].A,
        fparray[i].x,fparray[i].y, th, fparray[i].y - th);
 }
 fminuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
 chisquare = amin/(fparray.size()-fNpar-1);
 cout<<"\nChiSquare = "<<chisquare<<endl;
// fill gfpar global vector with current parameters relative
// to the current telescope

 if(fvpar[ftel_curr]!=0) {
  delete fvpar[ftel_curr];
  fvpar[ftel_curr]=0;
 }
 fvpar[ftel_curr] = new MFitParam(ftel_curr, fNpar, fmcalc, param, chisquare);
}


// the fitter function using chisquare minimization method
// root manual says that it can not be a member function of a class (?)
void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t flag)
{
 Double_t chisq = 0.0;
 Double_t fret;
 vector<MPart> data(MDEEFrame::fparray);
 FitPtr fp = gfit->GetFPtr();

 gfit->SetOffset(kTRUE);
 Int_t n=data.size();
 for(Int_t i=0; i<n; i++) {
  fret = data[i].y - (gfit->*fp)(data[i],par);
  chisq += fret*fret / (data[i].e*data[i].e);
 }
 f = chisq;
}



// Spline cuts fit a hyperbolic fit
void MDEEFrame::FitCurrentClicks()
{
 stringstream ss;
 TGraph *pl;
 const char *title;
 Double_t x,y,ny,par[4];

 if(!fcklist->IsEmpty()) {
  Int_t numtel_curr = ftel->GetNumberEntry()->GetIntNumber();
  ss<<"t"<<numtel_curr;
  TF1 fusp("fusp","[0]/(x**3)+[1]/(x**2)+[2]/x+[3]");
  for(Int_t i=0; i<fcklist->GetSize(); i++) {
   fusp.SetParameters(0,0,0,0);
   title = fcklist->At(i)->GetTitle();
   if(strcmp(title,ss.str().c_str())==0 && fmtree.IsCanvas()) {
    pl = (TGraph*)fcklist->At(i);
    pl->Fit("fusp","N");
    Int_t n = pl->GetN();

    if(n>0) {
     for(Int_t i=0; i<n; i++){
      pl->GetPoint(i,x,y);
      for(Int_t j=0; j<4; j++) {
       par[j] = fusp.GetParameter(j);
       ny = par[0]/(x*x*x) + par[1]/(x*x) + par[2]/x + par[3];
       pl->SetPoint(i,x,ny);
       pl->SetLineColor(4);
       pl->SetMarkerStyle(3);
       pl->SetMarkerColor(4);
       pl->SetMarkerSize(1.2);
      }
     }
     fmtree.GetCanvas()->Update();
    }

   }
  }
 }
}


// Function for DE-E identification based
// on formulas from N. Le Neindre et al. NIM A490 (2002) 251
// modified following the formula of W. Gawlikowicz
// for Light-Energy relation in CsI(Tl)
// NIM A491 (2002) 181. Adapted by Amalia Pop following
// ref.: J. Blicharska et al. LNS report 2005
// This formula needs 14 fit parameters

Double_t MDEEFrame::func(MPart &p, Double_t *par)
{
 Double_t X = p.x;
 Double_t Z = p.Z;
 Double_t A = p.A;
 Double_t xx,exp1,exp2,res=1.0;

 if(Z==0 && A==0) {
  return par[7];
 }
 xx = X-par[8];
 //if xx<0 never reached from data analysis
 if(xx<=0) {
 if(foffset) {
  xx=X;
 }
 else {
  return res;
  }
 }
 exp1 = par[2] + par[3] + 1;
 exp2 = 1./exp1;
 res = pow((par[5] * xx), exp1) +
       pow((par[6]*pow(Z,par[0]) * pow(A,par[1])),exp1) +
       par[4]*pow(Z,2) * pow(A,par[2]) * pow(xx*par[5],par[3]);
 res = pow(res,exp2) - par[5]*xx + par[7];
 return res;
}


// Function for DE-E identification based
// on formulas from N. Le Neindre et al. NIM A490 (2002) 251
// modified following the formula of W. Gawlikowicz
// for Light-Energy relation in CsI(Tl)
// NIM A491 (2002) 181. Adapted by Amalia Pop following
// ref.: J. Blicharska et al. LNS report 2005
// This formula needs 14 fit parameters
Double_t MDEEFrame::func14(MPart &p, Double_t *par)
{
 Double_t X = p.x;
 Double_t Z = p.Z;
 Double_t A = p.A;
 Double_t xx,exp1,exp2,res=1.0;
 Double_t reslog,ene;

 if(Z==0 && A==0) {
  return par[12];
 }
 xx = X-par[13];
 //if xx<0 never reached from data analysis
 if(xx<=0) {
 if(foffset) {
  xx=X;
 }
 else {
  return res;
  }
 }

 exp1 = par[2] + par[3] + 1;
 exp2 = 1./exp1;
 reslog = log(1.+ par[9]* Z)*(1.- exp(-par[10]*pow(Z,par[11])))/Z;
 ene = par[6] * reslog * xx ;
 ene = ene + par[7] * Z * sqrt(A) * log(1. + par[8] * reslog  * xx);
 res = pow(ene,exp1) +
       pow((par[4]* pow(Z,par[0]) * pow(A,par[1])),exp1) +
       par[5]*pow(Z,2) * pow(A,par[2]) * pow(ene,par[3]);
 res = pow(res,exp2) - ene + par[12];
 return res;
}

// Retrieve information about coherence between
// the current fitting parameters for a telescope
// and the parameters in memory that could be obtained
// with a different kind of fit
Bool_t MDEEFrame::GetAnaCoherence(Int_t ntel)
{
 Bool_t res = kTRUE;
 Int_t np = fvpar[ntel]->GetNpar();
 if((np==14 && ffitkind==kstandard) || (np==9 && ffitkind==kenhanced)) {
  res = kFALSE;
 }
 return res;
}


// Charge Identification in Si-CsI
Int_t MDEEFrame::Get_Charge(Int_t ntel, Double_t de, Double_t fast, Int_t *iter, Double_t *zeta)
{
 const Int_t maxiter=500, zlim=(Int_t)(fepax2.GetZP()+2);
 Int_t cres = 1,izeta;
 Double_t zmin=1, zmax=zlim, ztest, atest=0, yy, dist;
 Double_t amass=0,amassp=0,amassm=0;
 Bool_t found = kFALSE;
 MPart p;

 foffset = kFALSE;
 *iter = 0;
 *zeta = 0.0;
 while(*iter < maxiter) {
  (*iter)++;
  ztest = (zmin + zmax) / 2.0;

  if(fmcalc == k2) {
   atest = 2.0*ztest;
  }
  else if(fmcalc == kepax) {
   atest = fepax2.GetEMass((Int_t)(ztest+0.5));
   if(atest==1)atest=2.;
  }
  else if(fmcalc == kcharity) {
   atest = GetMassCharity(Int_t(ztest+0.5));
   if(atest==1)atest=2.;
  }
  p.x = fast;
  p.Z = ztest;
  p.A = atest;
  yy = (this->*fptr)(p,fvpar[ntel]->GetParam());
  if(yy == 1.0) {
   cres = 0;
   return cres;
  }
  if((Int_t)(zmin+0.5) == (Int_t)(zmax+0.5)) {
   found = kTRUE;
   break;
  }
  if((de - yy)>=0) {
   zmin = ztest;
   continue;
  }
  else {
   zmax = ztest;
   continue;
  }
 }
 if(!found) {
  cout<<"Get_Charge>> Convergenze not reached"<<endl;
 }
 cres = (Int_t)(zmin+0.5);
 izeta = cres;
 if(cres>zlim) {
  *zeta = izeta;
  return cres;
 }

// dispersion around mean charge value
  p.x = fast;
  p.Z = izeta;
  if(fmcalc == k2) {
   amass = 2.0*izeta;
   amassp = 2.0*(izeta+1);
   amassm = 2.0*(izeta-1);
  }
  else if(fmcalc == kepax) {
   amass = fepax2.GetEMass(izeta);
   amassp = fepax2.GetEMass(izeta+1);
   amassm = fepax2.GetEMass(izeta-1);
  }
  else if(fmcalc == kcharity) {
   amass = GetMassCharity(izeta);
   amassp = GetMassCharity(izeta+1);
   amassm = GetMassCharity(izeta-1);
  }

  p.A = amass;
  yy = (this->*fptr)(p,fvpar[ntel]->GetParam());
  dist = de - yy;
  if(dist >= 0.0) {
   p.Z = izeta+1;
   p.A = amassp;  //z+1
   *zeta = (Double_t)izeta + dist/((this->*fptr)(p,fvpar[ntel]->GetParam())-yy);
  }
  else {
   p.Z = izeta-1;
   p.A = amassm;  //zeta-1
   *zeta = (Double_t)izeta + dist/(yy-(this->*fptr)(p,fvpar[ntel]->GetParam()));
  }
  if(*zeta>0 && *zeta<0.5)*zeta = 0.51;
 return cres;
}



// Setup the global fit environment for one telescope
Int_t MDEEFrame::GetFitParameters(Int_t numtel_curr, Double_t *xmin)
{
 Int_t n,j,Z,A;
 TGraph *pl;
 const char *title;
 stringstream ss;
 Double_t x,y;
 MPart mp;
 vector <MPart>::const_iterator it;

 *xmin = 4095.0;
 if(!fparray.empty()) {
  fparray.clear();
 }
 fNPoint = 0;

 if(fcklist->IsEmpty())return 0;
 n = fcklist->GetEntries();


 for(Int_t i=0; i<n; i++) {
  pl = (TGraph*)fcklist->At(i);
  title = pl->GetTitle();
  ss<<"t"<<numtel_curr;
  if(strcmp(title,ss.str().c_str())==0) {
   GetZ_A((TList*)pl,Z,A);
   for(j=0; j<pl->GetN(); j++) {
    pl->GetPoint(j,x,y);
    mp.x = x;
    mp.y = y;
    mp.Z = Z;
    mp.A = A;
    mp.e = 0.005*y;
    fparray.push_back(mp);
    if(x<=*xmin)*xmin=x;
   }
  }
  ss.str("");
 }
 fNPoint = fparray.size();
 return fNPoint;
}

//return file name of a fit list file
string &MDEEFrame::GetListFromFile()
{
 const char *outtypes[] = {"Data files",    "*.[oO][uU][tT]",
                           "All files",     "*",
                            0,               0 };

 static TString dir(".");
 TGFileInfo fi;
 static string name="";

 fi.fFileTypes = outtypes;
 fi.fIniDir    = StrDup(dir);
 new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fi);
 if(!fi.fFilename)return name;
 cout<<"Opening file: "<<fi.fFilename<<endl;
 dir = fi.fIniDir;
 name = gSystem->UnixPathName(fi.fFilename);
 return name;
}

// Mass Identification in Si-CsI
Double_t MDEEFrame::Get_Mass(Int_t ntel, Int_t charge, Double_t de, Double_t fast, Int_t *iter) {
 const Int_t maxiter=1000;
 Bool_t found = kFALSE;
 Double_t amin,amax;
 Double_t Amin[] = {0, 2, 4, 6, 7, 9, 11, 13, 14, 17, 19, 21, 22, 23};
 Double_t Amax[] = {6, 10, 11, 12, 13, 15, 18, 26, 29, 32, 35, 36, 38, 41};
 Double_t mass = 0.0, fmass, yy, atest, dist, dmin=0.10;
 MPart p;

 foffset = kFALSE;
 *iter = 0;
 amin = Amin[charge-1];
 amax = Amax[charge-1];
 p.x = fast;
 p.Z = charge;
 while(*iter < maxiter) {
  (*iter)++;
  atest = (amax + amin) / 2.0;
  p.A = atest;
  yy = (this->*fptr)(p,fvpar[ntel]->GetParam());
  if(yy == 1.0) {
   return 0.0;
  }
  dist = de - yy;
  if(fabs(dist) <= dmin) {
   found = kTRUE;
   break;
  }
  if((de - yy)>=0) {
   amin = atest;
   continue;
  }
  else {
   amax = atest;
   continue;
  }
 }  //end convergenze loop

 if(!found) {
  if(amin==amax) {
   return 0.0;
  }
  else {
   if(charge>1) {
    cout<<"Get_Mass>> Warning: charge convergenze failed:  Z="<<charge<<endl;
   }
  }
 }

 mass = Int_t(atest+0.5);
 p.A = mass;
 yy = (this->*fptr)(p,fvpar[ntel]->GetParam());
 if(yy == 1.0) {
  return 0.0;
 }

// calculate dispersion around mean mass value
 dist = de - yy;
 if(dist >= 0.0) {
   p.A = mass + 1.;
   fmass = mass + dist/((this->*fptr)(p,fvpar[ntel]->GetParam())-yy);
  }
  else {
   p.A = mass - 1.;
   fmass = mass + dist/(yy-(this->*fptr)(p,fvpar[ntel]->GetParam()));
  }
 if(int(fmass+0.5) != int(mass+0.5)) {
  if(dist>0)
   fmass = int(fmass+0.5) + 0.501;
  else
  fmass = int(fmass+0.5) + 0.499;
 }
 if(fmass>0 && fmass<0.5)fmass = 0.51;
 return fmass;
}


// Load cuts from a file
void MDEEFrame::GetListClicks()
{
 TFile *ft;
 TGFileInfo fld;
 int nf=0;

 static TString dir(".");
 fld.fFileTypes = filetypes;
 fld.fIniDir    = StrDup(dir);
 new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fld);


 if (fld.fMultipleSelection && fld.fFileNamesList) {
  TObjString *el;
  TIter next(fld.fFileNamesList);
  while ((el = (TObjString *) next())) {
   ft = new TFile(gSystem->UnixPathName(el->GetString()));
   nf++;
   if(!ft->IsZombie()) {
    cout<<"Opening file: "<<el->GetString()<<endl;
    if(nf==1)
     GetListClicksFile(ft,1);
    else
     GetListClicksFile(ft,0);
    ft->Close();
    delete ft;
   }
  }
  fcklist->ls();
 }
 else if (fld.fFilename) {
  ft = new TFile(gSystem->UnixPathName(fld.fFilename));
  cout<<"Opening file: "<<fld.fFilename<<endl;
  if(!ft->IsZombie()) {
   GetListClicksFile(ft,1);
   ft->Close();
   delete ft;
   fcklist->ls();
  }
 }
}

// Retrieve new list from file. If erase == 1 the old list
// is deleted before constructing the new one
void MDEEFrame::GetListClicksFile(TFile *ft, Int_t erase)
{
 if(erase==1)fcklist->Delete();
 TIter keys(ft->GetListOfKeys());
 TKey *key;
 while((key = (TKey*)(keys.Next()))) {
  TGraph *plcut = (TGraph*)ft->Get(key->GetName());
  plcut->SetName(key->GetName());
  plcut->SetTitle(key->GetTitle());
  fcklist->Add(plcut);
 }
}


//Return the mass by the Charity formula
//PRC 58(1998)1073
Double_t MDEEFrame::GetMassCharity(Double_t Z)
{
 Double_t A;

 if(Z<=4) {
  A = gLight[(Int_t)Z-1];
  return A;
 }
 else
  return (UInt_t)(2.072*Z + 2.32E-03 * Z*Z) + 1;
}


// extract the charge and mass information from cut title
Bool_t MDEEFrame::GetZ_A(TList *p, Int_t &Z, Int_t &A)
{
 Z=0;
 A=0;
 const char *name;
 const char *str;

 if(!p)return kFALSE;
 name = p->GetName();
 str = name+1;
 Z = atoi(str);
 str = strchr(name,'A');
 if(str)str++;
 A = atoi(str);
 return kTRUE;
}

// Draw matrix from a root tree. Slot function
// for the PLOT button
void MDEEFrame::GraphMatrix()
{
 Int_t numtel = ftel->GetNumberEntry()->GetIntNumber();
 TGraph *gr;
 TH2F *hnew;

if(fmtree.IsOpen()) {
  fmtree.OpenTree();
  TTree *tr = fmtree.GetTree();
  stringstream scut;
// define telescope selection
  scut<<"numtel=="<<numtel;
  TCut cut = scut.str().c_str();
  fmtree.DrawCanvas();
  TCanvas *c1 = fmtree.GetCanvas();

  tr->SetTimerInterval(300);
  ftimer->TurnOn();
  tr->Draw("desimev:ecsich", cut);
  HandleTimer(ftimer);
  ftimer->TurnOff();
  tr->SetTimerInterval(0);

  gr = (TGraph *)gPad->FindObject("Graph");
  if(gr!=0) {
   if(fmtree.GetGraph()!=0) {
    delete fmtree.GetGraph();
   }
   TGraph *mgraph = (TGraph*)gr->Clone();
   mgraph->SetName("current");
   mgraph->ResetBit(kCanDelete);
   fmtree.SetGraph(mgraph);
   hnew = (TH2F*)gPad->FindObject("htemp");
   hnew->GetYaxis()->SetLabelSize(0.025);
   hnew->GetXaxis()->SetLabelSize(0.025);
  }
//  c1->Modified();
  c1->Update();
  fProgressBar->SetPosition(0);
  fProgressBar->ShowPosition();
 }
}


// Handle the file menu
Bool_t MDEEFrame::HandleMenu(Int_t id)
{
 TFile *ft;
 static TString dir(".");
 TGFileInfo fi;

 switch (id) {
  case kFileOpen:
   fi.fFileTypes = filetypes;
   fi.fIniDir    = StrDup(dir);
   new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fi);
   if(!fi.fFilename)return kFALSE;
   cout<<"Opening file: "<<fi.fFilename<<endl;
   dir = fi.fIniDir;
   if(fmtree.IsOpen()) {
    cout<<"Please, close file first"<<endl;
   }
   else {
    ft = new TFile(gSystem->UnixPathName(fi.fFilename));
    if (!ft->IsZombie()) {
     fmtree.SetTree(ft);
     fmtree.SetOpen(kTRUE);
    }
    else {
     fmtree.SetOpen(kFALSE);
    }
   }
  break;

  case kFileClose:
   if(fmtree.IsOpen()) {
    ft = fmtree.GetFile();
    ft->Close();
    delete ft;
    fmtree.SetOpen(kFALSE);
    cout<<"Tree file closed"<<endl;
   }
  break;

  case kFileQuit:
   gApplication->Terminate(0);
  break;
 }
 return kTRUE;
}

// Handle the list menu
Bool_t MDEEFrame::HandleMenuLine(Int_t id)
{
 TFile *fc=0;
 TGFileInfo fsv;
 const char *title;
 stringstream ss;
 Int_t numtel_curr;
 static TString dir(".");
 Bool_t res = kTRUE;

 switch (id) {
  case kLineOpen:
   GetListClicks();
   fMenuLine->EnableEntry(kLineSave);
   fMenuLine->EnableEntry(kLineSaveAs);
   break;
  case kLineSave:
   fc = new TFile("graphcut.root","new");
   if(fc->IsZombie()) {
    res = AskToWrite();
    if(res)fc = new TFile("graphcut.root","recreate");
   }
   if(res) {
    fcklist->Write();
    fc->Close();
    cout<<"Default click file ""graphcut.root"" saved"<<endl;
   }
   break;
  case kLineSaveAs:
   fsv.fFileTypes = filetypes;
   fsv.fIniDir    = StrDup(dir);
   new TGFileDialog(gClient->GetRoot(), this, kFDSave, &fsv);
   if(!fsv.fFilename)return kFALSE;
   fc = new TFile(gSystem->UnixPathName(fsv.fFilename),"recreate");
   fcklist->Write();
   fc->Close();
   cout<<"Click file "<<fsv.fFilename<<" saved"<<endl;
   break;
  case kLineSpline:
   if(fcklist==0)return kTRUE;
   FitCurrentClicks();
   break;
  case kLineDraw:
   if(fcklist==0)return kTRUE;
    if(!fcklist->IsEmpty()) {
     numtel_curr = ftel->GetNumberEntry()->GetIntNumber();
     ss<<"t"<<numtel_curr;

     for(Int_t i=0; i<fcklist->GetSize(); i++) {
      title = fcklist->At(i)->GetTitle();
      if(strcmp(title,ss.str().c_str())==0 && fmtree.IsCanvas()) {
       TGraph *p = (TGraph*)fcklist->At(i);
       p->SetLineColor(2);
       p->SetMarkerStyle(3);
       p->SetMarkerColor(4);
       p->SetMarkerSize(1.2);
       p->Draw("pl");
      }
     }
     if(fmtree.IsCanvas()) {
      fmtree.GetCanvas()->Update();
     }
     fMenuLine->EnableEntry(kLineSave);
     fMenuLine->EnableEntry(kLineSaveAs);
    }
   break;
   case kLineCopy:
    if(fcklist==0)return kTRUE;
    CopyLines();
   break;
   case kLineList:
   if(!fcklist->IsEmpty()) {
    ListLines();
   }
   break;
   case kLineAscii:
    if(fcklist==0) {
     return kTRUE;
    }
    else {
     PrintAsciiLines();
    }
   break;
   case kLineDelete:
   if(!fcklist->IsEmpty()) {
    fcklist->Delete();
   }
   break;
   case kLineDelgroup:
    numtel_curr = ftel->GetNumberEntry()->GetIntNumber();
    DeleteGroup(numtel_curr);
    break;
   case kLineDelOne:
    numtel_curr = ftel->GetNumberEntry()->GetIntNumber();
    DeleteOne(numtel_curr);
    break;
 }
 return kTRUE;
}


// Handle the fit menu
Bool_t MDEEFrame::HandleMenuFit(Int_t id)
{
 Int_t numtel_curr = ftel->GetNumberEntry()->GetIntNumber();
 Int_t fp;
 Double_t xmin;
 string filename;

 ftel_curr = numtel_curr;
 switch (id) {
  case kFitParam:
  fp = GetFitParameters(numtel_curr, &xmin);
  if(fp>0) {
   SetMinuitParameters(fNpar, xmin);
   SetFitMenu(1);
  }
  else {
   SetFitMenu(0);
  }
  break;
  case kFitExec:
   ExecuteFit();
  break;
  case kFitDrawOne:
   DrawOneFit(numtel_curr);
  break;
  case kFitDrawAll:
   DrawAllFit(numtel_curr);
  break;
  case kFitStandard:
   fMenuFit->RCheckEntry(kFitStandard,kFitStandard,kFitEnhanced);
   fStatus->SetText("Standard Fit",2);
   InitFitFunction(kFitStandard);
   SetFitMenu(0);
   break;
  case kFitEnhanced:
   fMenuFit->RCheckEntry(kFitEnhanced,kFitStandard,kFitEnhanced);
   fStatus->SetText("14 params Fit",2);
   InitFitFunction(kFitEnhanced);
   SetFitMenu(0);
   break;
  case kFitCharity:
   fMenuFit->RCheckEntry(kFitCharity,kFitCharity,kFitTwo);
   fStatus->SetText("CHARITY",1);
   fmcalc = kcharity;
   SetStatAZ();
  break;
  case kFitTwo:
   fMenuFit->RCheckEntry(kFitTwo,kFitCharity,kFitTwo);
   fStatus->SetText("A = 2*Z (A>4)",1);
   fmcalc = k2;
   SetStatAZ();
  break;
  case kFitEpax:
   fMenuFit->RCheckEntry(kFitEpax,kFitCharity,kFitTwo);
   fStatus->SetText("EPAX2",1);
   fmcalc = kepax;
   SetStatAZ();
  break;
  case kFitSave:
   SaveFitParam("Fitparam_table.out");
  break;
  case kFitLoad:
   LoadFitParam("Fitparam_table.out");
  break;
  case kFitFile:
   filename = GetListFromFile();
   LoadFitParam(filename.c_str());
  break;
 }
 return kTRUE;
}


// Handle the Analysis menu
Bool_t MDEEFrame::HandleMenuAna(Int_t id)
{
 Int_t numtel_curr = ftel->GetNumberEntry()->GetIntNumber();

 ftel_curr = numtel_curr;
 if(fvpar[numtel_curr]==0) {
  cout<<"Fit parameters not found for telescope: "<<numtel_curr<<endl;
  return kFALSE;
 }
 switch (id) {
  case kAnaIdent:
   Si_CsiIdentification(numtel_curr);
  break;
  case kAnaDraw:
   DrawHistogram(numtel_curr);
  break;
  case kAnaMass:
   DrawMasses(numtel_curr);
  break;
  case kAnaCheck:
   CheckZResults(numtel_curr);
  break;
  case kAnaCheck2://paolo
   CheckMResults(numtel_curr);//paolo
  break;//paolo
 }
 return kTRUE;
}

//Handle the setup menu
Bool_t MDEEFrame::HandleMenuSetup(Int_t id)
{
 char retstr[256];
 stringstream ss,ss1,ss2;
 Int_t zp,ap,zt,at;


 switch (id) {
  case kSetupReac:
  ss1<<fepax2.GetZP()<<" "<<fepax2.GetAP()<<" "
     <<fepax2.GetZT()<<" "<<fepax2.GetAT();
  new TGInputDialog(gClient->GetRoot(), this, "Reaction: proj(ZP AP) targ(ZT AT)",
                    ss1.str().c_str(),retstr);
  if(strcmp(retstr,"")!=0) {
   ss<<retstr;
   ss>>zp>>ap>>zt>>at;
   fepax2.SetReaction((Double_t)zp,(Double_t)ap,(Double_t)zt,(Double_t)at);
   fepax2.SetAZArray();
  }
  ss2<<"Proj ["<<fepax2.GetZP()<<" "<<fepax2.GetAP()<<
             "]  Targ ["<<fepax2.GetZT()<<" "<<fepax2.GetAT()<<"]";
  fStatus->SetText(ss2.str().c_str(),3);
  break;

 }
 return kTRUE;
}


// Handle for timer signal
Bool_t MDEEFrame::HandleTimer(TTimer *timer)
{
 TTree *tr = fmtree.GetTree();
// This function is called by the fTimer object.
 Float_t first = 0;
 Float_t last  = (Float_t)tr->GetEntries();
 Float_t current = (Float_t)tr->GetReadEntry();
 Float_t percent = (current-first+1)/(last-first+1);
 fProgressBar->SetPosition(100.*percent);
 fProgressBar->ShowPosition();
 timer->Reset();
 return kFALSE;
}


//Setup fit function
void MDEEFrame::InitFitFunction(Int_t id)
{
 if(id==kFitStandard) {
  fNpar = 9;
  fptr = &MDEEFrame::func;
  ffitkind = kstandard;
 }
 else if(id==kFitEnhanced) {
  fNpar = 14;
  fptr = &MDEEFrame::func14;
  ffitkind = kenhanced;
 }
}

// List all database cuts
void MDEEFrame::ListLines()
{
 TList *p;
 TIter next(fcklist);
 cout<<endl;
 while ((p = (TList*)next())) {
  p->ls();
 }
 cout<<"There are "<<fcklist->GetEntries()<<" clicks objects in the list"<<endl;
}



// Load an existing fit parameters table to memory
void MDEEFrame::LoadFitParam(const char *name)
{
 string line;
 Int_t ntel,np,lm;
 Double_t param[MAXFP], chi2;
 ecalcmass km;

 ifstream in(name,ios::in);
 if(in) {
  while(!in.eof()) {
   getline(in,line);
   if(line[0]=='*')continue;
   stringstream ss(line);
   ss>>ntel>>lm>>np;
   km = (ecalcmass )lm;
   if(!ss.fail()) {
    for(Int_t i=0; i<np; i++) {
     ss>>param[i];
    }
    ss>>chi2;
    if(fvpar[ntel]!=0) {
     delete fvpar[ntel];
     fvpar[ntel] = new MFitParam(ntel, np, km, param, chi2);
     cout<<"Data for tel: "<<ntel<<" replaced from file values"<<endl;
    }
    else {
     fvpar[ntel] = new MFitParam(ntel, np, km, param, chi2);
    }
   }
  }
  cout<<"Parameter table value loaded from file: "<<name<<endl;
  in.close();
 }
}



// functor for drawing fits
Double_t MDEEFrame::operator()(Double_t *x, Double_t *par)
{
 Int_t A=1,Z=1;
 Double_t res;
 MPart part;

 foffset = kTRUE;
 if(fdrawone) {
  Z = fZ->GetNumberEntry()->GetIntNumber();
  A = fM->GetNumberEntry()->GetIntNumber();
 }

 part.x = x[0];
 part.Z = Z;
 part.A = A;
 res = (this->*fptr)(part,par);
 return res;
}


// list the fparray vector
void MDEEFrame::PrintArray()
{
 vector <MPart>::const_iterator it;
 Int_t i=0;

 for(it=fparray.begin(); it<fparray.end(); it++) {
  cout<<"x["<<i<<"]="<<(*it).x<<
        ", y["<<i<<"]="<<(*it).y<<
        ", e["<<i<<"]="<<(*it).e<<
        ", Z["<<i<<"]="<<(*it).Z<<
        ", A["<<i<<"]="<<(*it).A<<endl;
  i++;
 }
}

//Print cut points on standard output
void MDEEFrame::PrintAsciiLines()
{
 stringstream ss;
 const char *title;
 Double_t x,y;

 if(!fcklist->IsEmpty()) {
  int numtel_curr = ftel->GetNumberEntry()->GetIntNumber();
  ss<<"t"<<numtel_curr;

  for(Int_t i=0; i<fcklist->GetSize(); i++) {
   title = fcklist->At(i)->GetTitle();
   if(strcmp(title,ss.str().c_str())==0) {
    TGraph *p = (TGraph*)fcklist->At(i);
    Int_t n = p->GetN();
    p->ls();
    if(n>0) {
     for(Int_t j=0; j<n; j++) {
      p->GetPoint(j,x,y);
      cout<<setw(2)<<j<<": "<<setprecision(3)<<fixed<<x<<"  "<<y<<" "<<endl;
     }
    }
   }
  }
 }
}

//Print reaction info in status bar
void MDEEFrame::PrintReaction()
{
 stringstream ss;
 ss<<"Proj ["<<fepax2.GetZP()<<" "<<fepax2.GetAP()<<
             "]  Targ ["<<fepax2.GetZT()<<" "<<fepax2.GetAT()<<"]";
 fStatus->SetText(ss.str().c_str(),3);
}

// Clear Matrix and draw again
void MDEEFrame::ResetMatrix()
{
 if(fmtree.IsCanvas()) {
  gPad->Clear();
  fmtree.GetGraph()->Draw("AP");
  fmtree.GetCanvas()->Update();
 }
}

//Save the table with the fit parameters to a file
void MDEEFrame::SaveFitParam(const char *name)
{
 ofstream out(name,ios::out);
 if(out) {
  out<<"*Fit parameters table created by DEEFIT"<<endl;
  out<<"*09| ntel mf npar alfa   mu  nu  csi  g  lambda  of_s  of_f  chi2"<<endl;
  out<<"*14| ntel mf npar alfa   mu  nu  lambda csi gap gb cp zl zi ze of_s of_f chi2"<<endl;
  out<<"*mf=0 2*Z; 1=Charity; 2=EPAX;"<<endl;
  for(Int_t i=0; i<1192; i++) {
   if(fvpar[i]!=0) {
    out<<*fvpar[i];          //stream parameters to file
   }
  }
  cout<<"Parameter table saved on file: "<<name<<endl;
  out.close();
 }
}

// enable/disable the fit menu entries
void MDEEFrame::SetFitMenu(Int_t status)
{
 if(status==0) {
  fMenuFit->DisableEntry(kFitExec);
//  modifica provvisoria
//  fMenuFit->DisableEntry(kFitDrawOne);
//  fMenuFit->DisableEntry(kFitDrawAll);
 }
 else {
  fMenuFit->EnableEntry(kFitExec);
  fMenuFit->EnableEntry(kFitDrawOne);
  fMenuFit->EnableEntry(kFitDrawAll);
 }
}


// Set the initial conditions for minuit fit
void MDEEFrame::SetMinuitParameters(Int_t npar, Double_t xmin)
{
 Double_t arglist[10];
 Int_t iflag =0;

 fminuit->SetPrintLevel(0);
 cout<<npar<<" parameters fit"<<endl;
 arglist[0] = 1;
 //for chisquare fit is 1
 fminuit->mnexcm("SET ERR",arglist,1,iflag);

 //parameter initialization
 if(ffitkind==kstandard) {
  Double_t start[]={1.,0.5,1.,0.5,200.,20.,500.,50.,xmin};
  Double_t step[]={0.001,0.001,0.001,0.001,0.1,0.5,1.,1.,1.};
  Double_t minv[]={0.,0.2,0.25,-1.,10.,0.1,0.,0.,10.};
  Double_t maxv[]={2.,1.,2.,2.,4000.,40.,2000.,2000.,2000.};

  fminuit->mnparm(0,  "alfa",start[0],step[0],minv[0],maxv[0],iflag);
  fminuit->mnparm(1,  "beta",start[1],step[1],minv[1],maxv[1],iflag);
  fminuit->mnparm(2,    "mu",start[2],step[2],minv[2],maxv[2],iflag);
  fminuit->mnparm(3,    "nu",start[3],step[3],minv[3],maxv[3],iflag);
  fminuit->mnparm(4,   "csi",start[4],step[4],minv[4],maxv[4],iflag);
  fminuit->mnparm(5,     "g",start[5],step[5],minv[5],maxv[5],iflag);
  fminuit->mnparm(6,"lambda",start[6],step[6],minv[6],maxv[6],iflag);
  fminuit->mnparm(7,  "of_s",start[7],step[7],minv[7],maxv[7],iflag);
  fminuit->mnparm(8,  "of_f",start[8],step[8],minv[8],maxv[8],iflag);
 }

 if(ffitkind==kenhanced) {
  Double_t start[]={1.,0.5,1.,0.5,20.,2.,10.,1.,500.,1.,1.,1.,50.,xmin};
  Double_t step[]={0.001,0.001,0.001,0.001,0.1,0.1,0.1,0.1,0.1,0.001,0.001,0.001,1.,1.};
  Double_t minv[]={0.,0.,0.2,-1.,0.,0.001,0.,0.,0.,0.,0.,0.,0.,10.};
  Double_t maxv[]={2.,1.,2.,2.,2000.,4000.,4000.,4000.,10000000.,20.,20.,20.,2000.,2000.};

  fminuit->mnparm(0,  "alfa",start[0],step[0],minv[0],maxv[0],iflag);
  fminuit->mnparm(1,  "beta",start[1],step[1],minv[1],maxv[1],iflag);
  fminuit->mnparm(2,    "mu",start[2],step[2],minv[2],maxv[2],iflag);
  fminuit->mnparm(3,    "nu",start[3],step[3],minv[3],maxv[3],iflag);
  fminuit->mnparm(4,"lambda",start[4],step[4],minv[4],maxv[4],iflag);
  fminuit->mnparm(5,   "csi",start[5],step[5],minv[5],maxv[5],iflag);
  fminuit->mnparm(6,     "gap",start[6],step[6],minv[6],maxv[6],iflag);
  fminuit->mnparm(7,   "gb",start[7],step[7],minv[7],maxv[7],iflag);
  fminuit->mnparm(8,     "cp",start[8],step[8],minv[8],maxv[8],iflag);
  fminuit->mnparm(9,   "zl",start[9],step[9],minv[9],maxv[9],iflag);
  fminuit->mnparm(10,     "zi",start[10],step[10],minv[10],maxv[10],iflag);
  fminuit->mnparm(11,"ze",start[11],step[11],minv[11],maxv[11],iflag);
  fminuit->mnparm(12,  "of_s",start[12],step[12],minv[12],maxv[12],iflag);
  fminuit->mnparm(13,  "of_f",start[13],step[13],minv[13],maxv[13],iflag);
 }
 //fitter function definition
 fminuit->SetFCN(fcn);
 //First step MonteCarlo minimization of the function
 //but the offset Fast (last param) remains fixed
 arglist[0] = 500;
 fminuit->FixParameter(npar-1);
 fminuit->mnexcm("SEEK",arglist,1,iflag);
 fminuit->Release(npar-1);
 cout<<"First step MonteCarlo minimization done"<<endl;
}



// calculate the mass starting from charge and
// write information in the status bar.
void MDEEFrame::SetStatAZ()
{
 Int_t Z=0,A=0;
 stringstream ss;

 Z = fZ->GetNumberEntry()->GetIntNumber();
 if(Z<=4) {
  A = (Int_t)gLight[Z-1];
 }
 else {
  if(fMenuFit->IsEntryRChecked(kFitTwo)) {
   A = 2*Z;
  }
  if(fMenuFit->IsEntryRChecked(kFitCharity)) {
   A = (Int_t)GetMassCharity((Double_t)Z);
  }
  if(fMenuFit->IsEntryRChecked(kFitEpax)) {
//   A = (Int_t)fepax2.GetMassEpax(Z);
   A = (Int_t)fepax2.GetEMass(Z);
  }
 }
 ss<<"Z="<<Z<<", A="<<A;
 fStatus->SetText(ss.str().c_str(),0);
}




//reset histograms for a given ntel
void MDEEFrame::ResetHistograms(int ntel)
{
 if(fvpar[ntel]==0)return;
 if(fvpar[ntel]->fhz[0]!=0) {
  for(Int_t i=0; i<MAXMAS; i++) {
   fvpar[ntel]->fhz[i]->Reset();
  }
 }
}


/////////////////////////////////////////////////////////////
//  general method for Si-Csi charge and mass Identification
//  based on the f95 version defined in the Isospin program
//  for Chimera data analysis
/////////////////////////////////////////////////////////////
Int_t MDEEFrame::Si_CsiIdentification(Int_t ntel)
{
 MIdent *evt=0;
 Int_t charge, iter;
 Double_t de, fast, zeta=0., mass=0.;
 Int_t numtel;

 if(!fmtree.IsOpen()) {
  cout<<"Probably you forgot to open a valid root file"<<endl;
  return -1;
 }
 if(fvpar[ntel]==0)return -1;
 fmtree.InitTree();
 TTree *t1 = fmtree.GetTree();
 if(t1==0) {
  cout<<"No valid tree is opened to read data"<<endl;
  return -1;
 }

 Bool_t IsCoherent = GetAnaCoherence(ntel);
 if(!IsCoherent) {
  cout<<"Tel "<<ntel<<" :Possible mismatch between current fit setting and data"<<endl;
  cout<<"Please change fit setting (Standard or Enhanced) in fit menu"<<endl;
  return -1;
 }


 evt = fmtree.GetEvent();
 Int_t nentries = (Int_t)t1->GetEntries();
 fProgressBar->SetPosition(0);
 fProgressBar->SetBarColor("cyan");
 Float_t first = 0;
 Float_t last  = (Float_t)nentries;

 Int_t nref = nentries/50+1;
 if(fvpar[ntel]->fhz[0]==0) {
  CreateHistograms(ntel);
 }

 ResetHistograms(ntel);


// Open a tree to store new data with Z identification
// for the current telescope for check purposes. The created
// tree can be opened as a "friend" of the original tree. It
// should be considered as a temporary file
  TFile *ide = new TFile("treeide.root","recreate");
  TTree *treeide = new TTree("treeide","Data analysis Friend Tree");
  treeide->Branch("charge",&charge,"charge/I");
  treeide->Branch("mass",&mass,"mass/D");

 for (Int_t i=0; i<nentries; i++) {
  t1->GetEntry(i);
  if(i%nref==0) {
   Float_t percent = (i-first+1)/(last-first+1);
   fProgressBar->SetPosition(100.*percent);
   gSystem->ProcessEvents();
  }
  charge = 0;

  if(evt->numtel != ntel) {
   treeide->Fill();
   continue;
  }
  de = (Double_t)evt->desimev;
  fast = (Double_t)evt->ecsich;

  if(de>0 && fast>0) {
// charge identification
   charge = Get_Charge(ntel, de, fast, &iter, &zeta);
   fvpar[ntel]->fhz[0]->Fill(zeta); //the first histo is the charge
// get mass for light fragments and particles
   if(charge>=1 && charge <MAXMAS) {
    mass = Get_Mass(ntel, charge, de, fast, &iter);
    fvpar[ntel]->fhz[charge]->Fill(mass);
   }
  }
  treeide->Fill();
 }
 fProgressBar->SetPosition(0);
 fProgressBar->ShowPosition();
 fProgressBar->SetBarColor("green");
//Write and close the analysis tree
 treeide->Write();
 ide->Close();
 delete ide;
 fLastAnalysis = ntel;
 return 0;
}

// Draw a user defined matrix in full colors
void MDEEFrame::UserPlot()
{
 MInfo info;
 Int_t binx,biny;
 Double_t xmin,xmax,ymin,ymax;
 TCanvas *cplot=0;
 TH2F *hpl=0;

 //check if a velid tree is already opened
 if(!fmtree.IsOpen()) {
  cout<<"UserPlot>> A valid root file should be opened"<<endl;
  return;
 }
 TTree *t1 = fmtree.GetTree();
 if(t1==0) {
  fmtree.OpenTree();
  t1 = fmtree.GetTree();
 }

 if(t1==0) {
  cout<<"UserPlot>> Unable to open tree file for data readout"<<endl;
  return;
 }

 //Get data from dialog panel
 MGraphFrame *p = new MGraphFrame(gClient->GetRoot(),this, &info);
 if(info.IsDone()==false)return;
 string sn = info.GetBuffer();
 stringstream ss(sn);
 ss>>binx>>xmin>>xmax>>biny>>ymin>>ymax;
 if(ss.fail()) {
  cout<<"UserPlot>> Wrong format string: "<<sn<<endl;
  return;
 }

 Int_t numtel = ftel->GetNumberEntry()->GetIntNumber();
 stringstream stel;
 stel<<"numtel=="<<numtel;

 Int_t perc = info.GetPerc();
 Int_t nentries = (Int_t)t1->GetEntries();
 Int_t nevt = nentries * perc / 100;
 cout<<"Processing "<<nevt<<" events ("<<perc<<"%)"<<endl;
 gStyle->SetPalette(1);
 fmtree.DrawCanvas();
// if((cplot = (TCanvas*)gROOT->FindObject("cplot"))!=0)delete cplot;
// cplot = new TCanvas("cplot", "User DE-E matrix",700,600);
 if((hpl = (TH2F*)gROOT->FindObject("hpl"))!=0)delete hpl;
 hpl = new TH2F("hpl","DE-E User Plot",binx,xmin,xmax,biny,ymin,ymax);
 t1->Draw("desimev:ecsich>>hpl",stel.str().c_str(),"colz",nevt);
}

//Display version
void MDEEFrame::Version(const char *ver)
{
 cout<<"*** DEEFIT ver. "<<ver<<"  E.d.F. 2009 ***"<<endl;
}
