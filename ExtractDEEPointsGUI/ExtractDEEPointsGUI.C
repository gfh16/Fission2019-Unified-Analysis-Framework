////////////////////////////////////////////////////////////////////////////////
// class ExtractDEEPointsGUI is used to extract dE-E points with GUI,
//  modified by gfh, based on the GUI of Fission2014
//
//  [**Note**] gfh just modified this class to be used for Fission2019,
//             the original one comes from Zhang Yan's code
//
// Jan 4, 2021
///////////////////////////////////////////////////////////////////////////////

#include "ExtractDEEPointsGUI.h"
ClassImp(ExtractDEEPointsGUI);


//______________________________________________________________________________
ExtractDEEPointsGUI::ExtractDEEPointsGUI(const TGWindow* p, UInt_t w, UInt_t h)
 : TGMainFrame(p, w, h, kHorizontalFrame)
{
  f1_histo = 0;   h2_DEEPlot = 0;
  TeleNo   = 0;   TeleNo_Max = 0;
  ParticleNo = 0; ParticleNo_Max = 0;

  MarkerFitPars = new Double_t[NUM_FITPARS];
  for (Int_t i=0; i<NUM_FITPARS; i++) { MarkerFitPars[i] = -99; }
}

//____________________________________________
ExtractDEEPointsGUI::~ExtractDEEPointsGUI()
{
 // Clean up used widgets: frames, buttons, layouthints
  this->Cleanup();
}

//______________________________________________________________________________
void ExtractDEEPointsGUI::Initial_RootFile(string histo_FilePath_tem)
{
   histo_FilePath = histo_FilePath_tem;
   f1_histo = new TFile(histo_FilePath.c_str());
}


//______________________________________________________________________________
void ExtractDEEPointsGUI::Initial_GUI(Int_t xPixel, Int_t yPixel)
{
  // Create canvas widget
  fEcanvas = new TRootEmbeddedCanvas("Ecanvas", this, xPixel, yPixel);
  AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,5,5,5,5));

  // Controls on right
  TGVerticalFrame *controls = new TGVerticalFrame(this);
  AddFrame(controls, new TGLayoutHints(kLHintsRight|kLHintsExpandY,5,5,0,5));

  // control margins of drawing
  margins_draw = new TGGroupFrame(controls,"Draw Panel");
  margins_draw->SetTitlePos(TGGroupFrame::kCenter);
  Tele_Entry = new TextMargin(margins_draw, "DEEPlotNo");
  margins_draw->AddFrame(Tele_Entry, new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  drawhist = new TGTextButton(margins_draw, "Draw");
  margins_draw->AddFrame(drawhist, new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  controls->AddFrame(margins_draw, new TGLayoutHints(kLHintsExpandX,0,0,20,5));

  // control margins of particle ID
  margins_id = new TGGroupFrame(controls,"ID Panel");
  margins_id->SetTitlePos(TGGroupFrame::kCenter);
  Particle_Entry = new TextMargin(margins_id, "ParticleNo");
  Charge_Entry = new TextMargin(margins_id, "ChargeNo");
  Mass_Entry = new TextMargin(margins_id, "MassNo");
  margins_id->AddFrame(Particle_Entry, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,0,0,10,5));
  margins_id->AddFrame(Charge_Entry, new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  margins_id->AddFrame(Mass_Entry, new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  controls->AddFrame(margins_id, new TGLayoutHints(kLHintsExpandX,0,0,20,5));


 // record, delete, write cut markers
  TGGroupFrame* cut = new TGGroupFrame(controls,"Markers");
  cut->SetTitlePos(TGGroupFrame::kCenter);
  drawmarkers = new TGTextButton(cut, "Draw");
  cut->AddFrame(drawmarkers,new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  deletemakers = new TGTextButton(cut, "Delete");
  cut->AddFrame(deletemakers,new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  writefile = new TGTextButton(cut, "&Save");
  cut->AddFrame(writefile,new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  controls->AddFrame(cut, new TGLayoutHints(kLHintsExpandX,0,0,20,5));

 // fit the current markers
  TGGroupFrame* fit = new TGGroupFrame(controls,"Fitting");
  fit->SetTitlePos(TGGroupFrame::kCenter);
  fitmarkers = new TGTextButton(fit, "Fit-([0]/x+pol6)");
  fit->AddFrame(fitmarkers,new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  savepars = new TGTextButton(fit, "Save Fit Pars");
  fit->AddFrame(savepars,new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  controls->AddFrame(fit, new TGLayoutHints(kLHintsExpandX,0,0,20,5));

  // do banana cut for all the isotopes
  TGGroupFrame* bananacut = new TGGroupFrame(controls,"BananaCut");
  bananacut->SetTitlePos(TGGroupFrame::kCenter);
  drawbananacut = new TGTextButton(bananacut, "Draw");
  bananacut->AddFrame(drawbananacut,new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  deletebananacut = new TGTextButton(bananacut, "Delete");
  bananacut->AddFrame(deletebananacut,new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  savebananacut = new TGTextButton(bananacut, "&Save");
  bananacut->AddFrame(savebananacut,new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  controls->AddFrame(bananacut, new TGLayoutHints(kLHintsExpandX,0,0,20,5));

 // exit
  exit = new TGTextButton(controls, "&Exit");
  controls->AddFrame(exit, new TGLayoutHints(kLHintsExpandX,10,5,40,5));

  SetWindowName("Welcome to DE-E Points Extracting GUI! (Owned by Yan Zhang, modified by Fenhai Guan, Jan 2021)"); // Set a name to the main frame
  MapSubwindows(); // Map all subwindows of main frame
  Resize(this->GetDefaultSize()); // Initialize the layout algorithm
  MapWindow();   // Map main frame
}

//______________________________________________________________________________
// signals and slots
void ExtractDEEPointsGUI::Initial_Slot()
{
  drawhist    ->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Draw_Hist()");

  drawmarkers ->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Draw_Markers()");
  deletemakers->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Delete_Markers()");
  writefile   ->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Write_MarkersCut()");

  fitmarkers  ->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Fit_Markers()");
  savepars    ->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Save_FitPars()");

  drawbananacut  ->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Draw_BananaCut()");
  deletebananacut->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Delete_BananaCut()");
  savebananacut  ->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Save_BananaCut()");

  exit->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Exit_GUI()");
}


//______________________________________________________________________________
// Draw markers on the current histogram
void ExtractDEEPointsGUI::Draw_Hist()
{
  TeleNo     = Tele_Entry->GetNumber();
  ParticleNo = Particle_Entry->GetNumber();
  ChargeNo   = Charge_Entry->GetNumber();
  MassNo     = Mass_Entry->GetNumber();
  cout<<"--> TeleNo: "<<TeleNo<<" , ParticleNo: "<<ParticleNo<<endl;

  f1_histo->cd();

  TCanvas* c1_tem = fEcanvas->GetCanvas();
  c1_tem->cd();
  c1_tem->Clear();
  gPad->SetLeftMargin(0.16);
  //
  //  manage the histogram canvas here !!
  h2_DEEPlot = (TH2D*)f1_histo->Get(Form("%s%02d", HistTile.c_str(), TeleNo));
  h2_DEEPlot->GetXaxis()->SetNdivisions(1005);
  h2_DEEPlot->GetXaxis()->CenterTitle(1);
  h2_DEEPlot->GetYaxis()->CenterTitle(1);
  h2_DEEPlot->GetYaxis()->SetTitleOffset(1.5);
  h2_DEEPlot->Draw("COL");
  c1_tem->Modified();
  c1_tem->Update();

  MapWindow();
}


//______________________________________________________________________________
void ExtractDEEPointsGUI::Set_MarkersFileName(string FileName_tem, string HistTile_tem)
{
  CutFile_Name = FileName_tem;
  HistTile = HistTile_tem;

  MarkersFileOut = new TFile(CutFile_Name.c_str(),"UPDATE");
}


//______________________________________________________________________________
// show markers on the canvas when cliking
void ExtractDEEPointsGUI::Draw_Markers()
{
  ChargeNo   = Charge_Entry->GetNumber();
  MassNo     = Mass_Entry->GetNumber();

  TCanvas* c1_tem = fEcanvas->GetCanvas();
  gPad->SetLeftMargin(0.16);

  std::string HistName = h2_DEEPlot->GetName();
  std::string GraphName;
  GraphName.assign("gr_"+HistName+Form("_Z%02d_A%02d",ChargeNo,MassNo));

  c1_tem->WaitPrimitive("Graph","PolyLine");
  fgraph = (TGraph*) gPad->GetListOfPrimitives()->Last();

  fgraph->SetName(GraphName.c_str());
  fgraph->Draw("* SAME");
  fgraph->SetLineColor(kMagenta);
  fgraph->SetLineWidth(2);
  fgraph->SetMarkerStyle(3);
  fgraph->SetMarkerColor(kMagenta);
  fgraph->SetMarkerSize(2);

  fNMarkers = fgraph->GetN();
  for (Int_t i=0; i<fNMarkers; i++) {
    fgraph->GetPoint(i, fMarkerX, fMarkerY);
    cout<<"Z = "<<ChargeNo<<setw(10)<<"A = "<<MassNo<<setw(10)<<fMarkerX<<setw(15)<<fMarkerY<<endl;
  }
  c1_tem->Modified();
  c1_tem->Update();

  Resize(GetDefaultSize());
  MapWindow();
}

//______________________________________________________________________________
// delete markers
void ExtractDEEPointsGUI::Delete_Markers()
{
  cout<<"delete markers: "<<fgraph->GetName()<<endl;
  fgraph->Delete();
//  ffitfunc->Delete();

  TCanvas* c1_tem = fEcanvas->GetCanvas();
  c1_tem->Modified();
  c1_tem->Update();

  Resize(GetDefaultSize());
}

//______________________________________________________________________________
// write and save the coordinates of all the mrakers
void ExtractDEEPointsGUI::Write_MarkersCut()
{
  MarkersFileOut->WriteTObject(fgraph, fgraph->GetName());
  cout<<"write markers: "<<fgraph->GetName()<<endl;
}


//______________________________________________________________________________
void ExtractDEEPointsGUI::Set_FitParsFileName(string pathFitParsOut_tem)
{
  pathFitParsOut = pathFitParsOut_tem;

  FitParsFileOut = ofstream(pathFitParsOut.c_str(), ios::app);
  FitParsFileOut<<"* Fitting the markers with func = [0]/x + pol6"<<endl;
  FitParsFileOut<<"*"<<"TeleNo"<<setw(15)<<"ParticleNo"<<setw(8)<<"Z"<<setw(10)<<"A"<<setw(12)
                <<"a0"<<setw(15)<<"a1"<<setw(15)<<"a2"<<setw(13)<<"a3"<<setw(15)
                <<"a4"<<setw(15)<<"a5"<<setw(15)<<"a6"<<setw(15)<<"a7"<<endl;
}


//______________________________________________________________________________
// fit the markers for the current isotope
void ExtractDEEPointsGUI::Fit_Markers()
{
  fNMarkers = fgraph->GetN();
  Double_t DeltaE[fNMarkers];
  Double_t Energy[fNMarkers];
  for (Int_t i=0; i<fNMarkers; i++) {
    fgraph->GetPoint(i, Energy[i], DeltaE[i]);
  }
  TCanvas* c1_tem = fEcanvas->GetCanvas();
  TGraph* g1 = new TGraph(fNMarkers, Energy, DeltaE);
  TF1* fit = new TF1("fit","[0]/x+[1]+[2]*x+[3]*x*x+[4]*x*x*x+[5]*x*x*x*x+[6]*x*x*x*x*x+[7]*x*x*x*x*x*x",Energy[0]*0.9,Energy[fNMarkers-1]*1.1);
  g1->Fit("fit","","", Energy[0]*0.9, Energy[fNMarkers-1]*1.1);
  fit->Draw("L SAME");
  fit->SetLineColor(kOrange);
  fit->SetLineWidth(2);
  c1_tem->Update();

  ffitfunc = (TF1*) gPad->GetListOfPrimitives()->Last();

  TF1* f1 = (TF1*) g1->GetFunction("fit");
  for (Int_t i=0; i<NUM_FITPARS; i++) {
    MarkerFitPars[i] = f1->GetParameter(i);
  }
}


//______________________________________________________________________________
void ExtractDEEPointsGUI::Save_FitPars()
{
  cout<<"save fit parameters."<<endl;
  FitParsFileOut<<setw(5)<<Tele_Entry->GetNumber()<<setw(15)<<Particle_Entry->GetNumber()<<setw(10)
         <<Charge_Entry->GetNumber()<<setw(10)<<Mass_Entry->GetNumber()<<setw(10);

  for (Int_t i=0; i<NUM_FITPARS; i++) {
    FitParsFileOut<<setw(15)<<MarkerFitPars[i];
  }
  FitParsFileOut<<endl;
}


//______________________________________________________________________________
void ExtractDEEPointsGUI::Set_BananaCutFileName(string BananaCutName_tem)
{
  BananaCut_FileName = BananaCutName_tem;
  BananaCutFileOut = new TFile(BananaCut_FileName.c_str(),"UPDATE");
}


//______________________________________________________________________________
void ExtractDEEPointsGUI::Draw_BananaCut()
{
  ChargeNo   = Charge_Entry->GetNumber();
  MassNo     = Mass_Entry->GetNumber();

  std::string HistoName = h2_DEEPlot->GetName();
  std::string CutGName;
  CutGName.assign(HistoName+Form("_Z%02d_A%02d",ChargeNo,MassNo));

  TCanvas* c1_tem = fEcanvas->GetCanvas();
  gPad->SetLeftMargin(0.16);

  c1_tem->WaitPrimitive("CUTG","CutG");
  fcutg = (TCutG*) gPad->GetListOfPrimitives()->Last();

  fcutg->SetName(CutGName.c_str());
  fcutg->Draw("*L");
  fcutg->SetLineColor(kRed-7);
  fcutg->SetLineWidth(2);
  fcutg->SetMarkerStyle(3);
  fcutg->SetMarkerColor(kRed-7);
  fcutg->SetMarkerSize(2);

  c1_tem->Modified();
  c1_tem->Update();

  cout<<"current cut : "<<fcutg->GetName()<<" is done !"<<endl;

  Resize(GetDefaultSize());
  MapWindow();
}


//______________________________________________________________________________
void ExtractDEEPointsGUI::Delete_BananaCut()
{
  cout<<"delete cut "<<fcutg->GetName()<<endl;
  fcutg->Delete();

  TCanvas* c1_tem = fEcanvas->GetCanvas();
  c1_tem->Modified();
  c1_tem->Update();
  Resize(GetDefaultSize());
}


//______________________________________________________________________________
void ExtractDEEPointsGUI::Save_BananaCut()
{
  BananaCutFileOut->WriteTObject(fcutg, fcutg->GetName());
  cout<<"write cut "<<fcutg->GetName()<<endl;
}


//______________________________________________________________________________
// exit ot not ?
void ExtractDEEPointsGUI::Exit_GUI()
{
  cout<<"Are you ready to Exit, you have clicked the button-Write File, Right?(y/n)"<<endl;
  string Is_Written_tem = "n";
  cin>>Is_Written_tem;

  if (Is_Written_tem=="y" || Is_Written_tem=="Y") { gApplication->Terminate(0); }
  else if ( Is_Written_tem=="n" || Is_Written_tem=="N" ) {
    cout<<"Click that button, or you will lose all your input"<<endl;
  }
  else { cout<<"Are you kidding me ? Only y/Y/n/N is accepted!"<<endl; }
}
