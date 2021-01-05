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
  f1_histo = 0; h2_DEEPlot = 0;
  TeleNo   = 0; TeleNo_Max = 0;
  ParticleNo = 0; ParticleNo_Max = 0;

  for(Int_t i=0; i<MAX_TELENO; i++) {
    for(Int_t j=0; j<MAX_PARTICLENO; j++) {
      for(Int_t k=0; k<NUM_POINTS; k++) {
        CutData[i][j][k][0] = -99; CutData[i][j][k][1] = -99;
        Marker_Cut[i][j][k] = 0;
      }
    }
  }
  DisplayDataXY = new Double_t* [NUM_POINTS];
  for(Int_t k=0; k<NUM_POINTS; k++) {
    Cur_Marker_Cut[k] = 0;
    DisplayDataXY[k] = new Double_t[2];
    DisplayDataXY[k][0] = -99;
    DisplayDataXY[k][1] = -99;
  }
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

  PointTable = new TGSimpleTable(this, 0, DisplayDataXY, NUM_POINTS , 2);
  AddFrame(PointTable,new TGLayoutHints(kLHintsExpandY));

  // Controls on right
  TGVerticalFrame *controls = new TGVerticalFrame(this);
  AddFrame(controls, new TGLayoutHints(kLHintsRight|kLHintsExpandY,5,5,0,5));

  // control margins of the detector ID and run Number
  margins = new TGGroupFrame(controls,"ID Panel");
  margins->SetTitlePos(TGGroupFrame::kCenter);
  Tele_Entry = new TextMargin(margins, "DEEPlotNo");
  Particle_Entry = new TextMargin(margins, "ParticleNo");
  Charge_Entry = new TextMargin(margins, "ChargeNo");
  Mass_Entry = new TextMargin(margins, "MassNo");
  margins->AddFrame(Tele_Entry, new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  margins->AddFrame(Particle_Entry, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,0,0,10,5));
  margins->AddFrame(Charge_Entry, new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  margins->AddFrame(Mass_Entry, new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  draw = new TGTextButton(margins, "Draw");
  margins->AddFrame(draw, new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  controls->AddFrame(margins, new TGLayoutHints(kLHintsExpandX,0,0,20,5));

  // for chosing the mode
   WorkMode_bg = new TGButtonGroup(controls,"Working Modes",kVerticalFrame);
   WorkMode_bg->SetTitlePos(TGButtonGroup::kCenter);
   WorkMode_bg_button[0] = new TGRadioButton(WorkMode_bg,new TGHotString("&Silence"));
   WorkMode_bg_button[1] = new TGRadioButton(WorkMode_bg,new TGHotString("&Marking"));
   WorkMode_bg_button[0]->SetState(kButtonDown);
   controls->AddFrame(WorkMode_bg, new TGLayoutHints(kLHintsExpandX,0,0,20,5));


 // record, delete, write cut markers
  TGGroupFrame* cut = new TGGroupFrame(controls,"Cut Markers");
  cut->SetTitlePos(TGGroupFrame::kCenter);
  record = new TGTextButton(cut, "Record");
  cut->AddFrame(record,new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  clear = new TGTextButton(cut, "Delete");
  cut->AddFrame(clear,new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  Write_File = new TGTextButton(cut, "&Save Markers");
  cut->AddFrame(Write_File,new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  controls->AddFrame(cut, new TGLayoutHints(kLHintsExpandX,0,0,20,5));

 // fit the current markers
  TGGroupFrame* fit = new TGGroupFrame(controls,"Fitting");
  fit->SetTitlePos(TGGroupFrame::kCenter);
  Fit_Markers = new TGTextButton(fit, "Fit-([0]/x+pol6)");
  fit->AddFrame(Fit_Markers,new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  Save_Pars = new TGTextButton(fit, "Save Fit Pars");
  fit->AddFrame(Save_Pars,new TGLayoutHints(kLHintsExpandX,0,0,10,5));
  controls->AddFrame(fit, new TGLayoutHints(kLHintsExpandX,0,0,20,5));

 // exit
  exit = new TGTextButton(controls, "&Exit");
  controls->AddFrame(exit, new TGLayoutHints(kLHintsExpandX,10,5,50,5));

  SetWindowName("Welcome to DE-E Points Extracting GUI! (Owned by Yan Zhang, modified by Fenhai Guan, Jan 2021)"); // Set a name to the main frame
  MapSubwindows(); // Map all subwindows of main frame
  Resize(this->GetDefaultSize()); // Initialize the layout algorithm
  MapWindow();   // Map main frame
}


//______________________________________________________________________________
void ExtractDEEPointsGUI::Set_CutFile_Name(string FileName_tem, string HistTile_tem)
{
  CutFile_Name = FileName_tem;
  HistTile = HistTile_tem;

  MarkersFileOut = ofstream(CutFile_Name.c_str(), ios::app);
  MarkersFileOut<<"#"<<"TeleNo"<<setw(15)<<"ParticleNo"<<setw(15)<<"Z"<<setw(15)<<"A"<<setw(15)
                <<"X(i),Y(i)"<<setw(10)<<"***"<<setw(20)<<Form("X(%d),Y(%d)",NUM_POINTS,NUM_POINTS)<<endl;

}

//______________________________________________________________________________
// write and save the coordinates of all the mrakers
void ExtractDEEPointsGUI::Write_CutFile()
{
  MarkersFileOut<<Tele_Entry->GetNumber()<<setw(5)<<Particle_Entry->GetNumber()<<setw(5)
         <<Charge_Entry->GetNumber()<<setw(5)<<Mass_Entry->GetNumber()<<setw(5);

  for(Int_t k=0; k<NUM_POINTS; k++) {
    if(CutData[TeleNo][ParticleNo][k][0]!=-99 || CutData[TeleNo][ParticleNo][k][1]!=-99) {
      MarkersFileOut<<setw(10)<<CutData[TeleNo][ParticleNo][k][0]<<setw(10)<<CutData[TeleNo][ParticleNo][k][1];
    }
  }
  MarkersFileOut<<endl;
  MarkersFileOut<<endl;
}

//______________________________________________________________________________
// signals and slots
void ExtractDEEPointsGUI::Initial_Slot()
{
  draw->Connect("Clicked()", "ExtractDEEPointsGUI", this, "DoDraw_ch()");
  Write_File->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Write_CutFile()");
  Fit_Markers->Connect("Clicked()", "ExtractDEEPointsGUI", this, "DoMarkersFit()");
  Save_Pars->Connect("Clicked()", "ExtractDEEPointsGUI", this, "SaveFitPars()");
  exit->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Is_Exit()");

  //for chosing the working mode
  WorkMode_bg_button[0]->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Silence_Mode()");
  WorkMode_bg_button[1]->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Marking_Mode()");
  Silence_Mode();
  record->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Record_Marker()");
  clear->Connect("Clicked()", "ExtractDEEPointsGUI", this, "Delete_Marker()");

  TCanvas* c1_tem = fEcanvas->GetCanvas();
  c1_tem->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "ExtractDEEPointsGUI",
                  this, "Record_Click(Int_t, Int_t, Int_t, TObject *)");
}

//______________________________________________________________________________
// Draw markers on the current histogram
void ExtractDEEPointsGUI::DoDraw_ch()
{
  TeleNo     = Tele_Entry->GetNumber();
  ParticleNo = Particle_Entry->GetNumber();
  ChargeNo   = Charge_Entry->GetNumber();
  MassNo     = Mass_Entry->GetNumber();
  cout<<"--> TeleNo: "<<TeleNo<<" , ParticleNo: "<<ParticleNo<<endl;
  if((TeleNo > TeleNo_Max) || (ParticleNo > ParticleNo_Max)) {
    cout<<"The Maximum for (TeleNo,ParticleNo) is: ("<<TeleNo_Max<<","<<ParticleNo_Max<<")"<<endl;
    cout<<"Please Press Enter key, then Re-Input"<<endl; getchar();
    return;
  }
  f1_histo->cd();

  TCanvas* c1_tem = fEcanvas->GetCanvas();
  c1_tem->cd();
  c1_tem->Clear();
  gPad->SetLeftMargin(0.16);
  //**************************************
  //  manage the histogram canvas here !!
  h2_DEEPlot = (TH2D*)f1_histo->Get(Form("%s%02d", HistTile.c_str(), TeleNo+1));
  h2_DEEPlot->GetXaxis()->SetNdivisions(1005);
  h2_DEEPlot->GetXaxis()->CenterTitle(1);
  h2_DEEPlot->GetYaxis()->CenterTitle(1);
  h2_DEEPlot->GetYaxis()->SetTitleOffset(1.5);
  h2_DEEPlot->Draw("COL");
  c1_tem->Modified();
  c1_tem->Update();

  for(Int_t k=0; k<NUM_POINTS; k++) {
    DisplayDataXY[k][0]=CutData[TeleNo][ParticleNo][k][0];
    DisplayDataXY[k][1]=CutData[TeleNo][ParticleNo][k][1];
  }

  for(Int_t i=0; i<NUM_POINTS; i++) {
    if(CutData[TeleNo][ParticleNo][i][0]!=-99 && CutData[TeleNo][ParticleNo][i][1]!=-99) {
      DrawMarker(TeleNo,ParticleNo,i,CutData[TeleNo][ParticleNo][i][0],CutData[TeleNo][ParticleNo][i][1]);
    }
    else if(TeleNo!=0 && Marker_Cut[TeleNo-1][ParticleNo][i]!=0) {
      Marker_Cut[TeleNo-1][ParticleNo][i]->Draw("same");
      Cur_Marker_Cut[i] = Marker_Cut[TeleNo-1][ParticleNo][i];
      Marker_Cut[TeleNo-1][ParticleNo][i]->SetMarkerColor(kMagenta);
    }
    else { Cur_Marker_Cut[i] = 0; }
  }
  c1_tem->Modified();
  c1_tem->Update();
  MapWindow();
}


//______________________________________________________________________________
void ExtractDEEPointsGUI::DrawMarker(Int_t TeleNo_tem, Int_t ParticleNo_tem, Int_t Edge_tem, Double_t x_tem, Double_t y_tem)
{
  TCanvas* c1_tem = fEcanvas->GetCanvas();
  c1_tem->cd();
  if(Marker_Cut[TeleNo_tem][ParticleNo_tem][Edge_tem]!=0) {
    Marker_Cut[TeleNo_tem][ParticleNo_tem][Edge_tem]->Delete();
    Marker_Cut[TeleNo_tem][ParticleNo_tem][Edge_tem] = 0;
  }
  Marker_Cut[TeleNo_tem][ParticleNo_tem][Edge_tem] = new TMarker(x_tem,y_tem,3);
  Marker_Cut[TeleNo_tem][ParticleNo_tem][Edge_tem]->SetMarkerColor(kRed);
  Marker_Cut[TeleNo_tem][ParticleNo_tem][Edge_tem]->SetMarkerSize(2.);
  Marker_Cut[TeleNo_tem][ParticleNo_tem][Edge_tem]->Draw("same");
  Cur_Marker_Cut[Edge_tem] = Marker_Cut[TeleNo_tem][ParticleNo_tem][Edge_tem];
}

//______________________________________________________________________________
// if the markers are satisfied, record them and the color of markers become red!
void ExtractDEEPointsGUI::Record_Marker()
{
  for(Int_t i=0; i<NUM_POINTS; i++) {
    if(Cur_Marker_Cut[i]!=0) {
      Marker_Cut[TeleNo][ParticleNo][i] = new TMarker(*(Cur_Marker_Cut[i]));

      CutData[TeleNo][ParticleNo][i][0] = Marker_Cut[TeleNo][ParticleNo][i]->GetX();
      CutData[TeleNo][ParticleNo][i][1] = Marker_Cut[TeleNo][ParticleNo][i]->GetY();

      DisplayDataXY[i][0]=CutData[TeleNo][ParticleNo][i][0];
      DisplayDataXY[i][1]=CutData[TeleNo][ParticleNo][i][1];

      ParticleZA[TeleNo][ParticleNo][i][0] = Charge_Entry->GetNumber();
      ParticleZA[TeleNo][ParticleNo][i][1] = Mass_Entry->GetNumber();

      Marker_Cut[TeleNo][ParticleNo][i]->SetMarkerColor(kRed);
      Marker_Cut[TeleNo][ParticleNo][i]->Draw("same");
      Cur_Marker_Cut[i]=Marker_Cut[TeleNo][ParticleNo][i];
    }
  }
  TCanvas* c1_tem = fEcanvas->GetCanvas();
  c1_tem->Update();
}

//______________________________________________________________________________
// delete markers
void ExtractDEEPointsGUI::Delete_Marker()
{
  for(Int_t i=0; i<NUM_POINTS; i++) {
    if(Cur_Marker_Cut[i]!=0) {
      Cur_Marker_Cut[i]->Delete(); Cur_Marker_Cut[i]=0;
      Marker_Cut[TeleNo][ParticleNo][i]=0;
    }
    CutData[TeleNo][ParticleNo][i][0] = -99;
    CutData[TeleNo][ParticleNo][i][1] = -99;
    DisplayDataXY[i][0]=CutData[TeleNo][ParticleNo][i][0];
    DisplayDataXY[i][1]=CutData[TeleNo][ParticleNo][i][1];
  }
  TCanvas* c1_tem = fEcanvas->GetCanvas();
  c1_tem->Update();
  Resize(GetDefaultSize());
}

//______________________________________________________________________________
// exit ot not ?
void ExtractDEEPointsGUI::Is_Exit()
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

//______________________________________________________________________________
// show markers on the canvas when cliking
void ExtractDEEPointsGUI::Record_Click(Int_t event, Int_t px, Int_t py, TObject *sel)
{
 //  Actions in reponse to mouse button events.
  TCanvas* c1_Actived = fEcanvas->GetCanvas();
  TPad* pad_tem = (TPad*) c1_Actived->GetSelectedPad();
  if(!pad_tem) { return; }

  Double_t x_tem = pad_tem->AbsPixeltoX(px);
  Double_t y_tem = pad_tem->AbsPixeltoY(py);
  x_tem = pad_tem->PadtoX(x_tem);
  y_tem = pad_tem->PadtoY(y_tem);

  if((event == 1) && (IsMarking == 1)) { // double left click
    for(Int_t i=0; i<NUM_POINTS; i++) {
      if(Cur_Marker_Cut[i] == 0) {
        DrawMarker(TeleNo,ParticleNo,i,x_tem,y_tem);
        Marker_Cut[TeleNo][ParticleNo][i]->SetMarkerColor(kMagenta);
        break;
      }
    }
  }
  Resize(GetDefaultSize());
}


//______________________________________________________________________________
// fit the markers for the current isotope
void ExtractDEEPointsGUI::DoMarkersFit()
{
  Double_t DeltaE[NUM_POINTS];
  Double_t Energy[NUM_POINTS];
  for (Int_t i=0; i<NUM_POINTS; i++) {
    DeltaE[i] = CutData[TeleNo][ParticleNo][i][0];
    Energy[i] = CutData[TeleNo][ParticleNo][i][1];
  }
  TCanvas* c1_tem = fEcanvas->GetCanvas();
  TGraph* g1 = new TGraph(NUM_POINTS, DeltaE, Energy);
  TF1* fit_func = new TF1("fit_func","[0]/x+[1]+[2]*x+[3]*x*x+[4]*x*x*x+[5]*x*x*x*x+[6]*x*x*x*x*x+[7]*x*x*x*x*x*x",0,4000);
  g1->Fit("fit_func","","", DeltaE[0]*0.9, DeltaE[NUM_POINTS-1]*1.1);
  fit_func->Draw("AL SAME");
  fit_func->SetLineColor(kOrange);
  fit_func->SetLineWidth(2);
  c1_tem->Update();

  TF1* f1 = (TF1*)g1->GetFunction("fit_func");
  for (Int_t i=0; i<NUM_FITPARS; i++) {
    MarkerFitPars[i] = f1->GetParameter(i);
  }
}

//______________________________________________________________________________
void ExtractDEEPointsGUI::Set_FitParsFile_Name(string pathFitParsOut_tem)
{
  pathFitParsOut = pathFitParsOut_tem;

  FitParsFileOut = ofstream(pathFitParsOut.c_str(), ios::app);
  FitParsFileOut<<"# Fitting the markers with func = [0]/x + pol6"<<endl;
  FitParsFileOut<<"#"<<"TeleNo"<<setw(15)<<"ParticleNo"<<setw(8)<<"Z"<<setw(10)<<"A"<<setw(12)
                <<"a0"<<setw(15)<<"a1"<<setw(15)<<"a2"<<setw(13)<<"a3"<<setw(15)
                <<"a4"<<setw(15)<<"a5"<<setw(15)<<"a6"<<setw(15)<<"a7"<<endl;
}

//______________________________________________________________________________
void ExtractDEEPointsGUI::SaveFitPars()
{
  FitParsFileOut<<setw(5)<<Tele_Entry->GetNumber()<<setw(15)<<Particle_Entry->GetNumber()<<setw(10)
         <<Charge_Entry->GetNumber()<<setw(10)<<Mass_Entry->GetNumber()<<setw(10);

  for (Int_t i=0; i<NUM_FITPARS; i++) {
    FitParsFileOut<<setw(15)<<MarkerFitPars[i];
  }
  FitParsFileOut<<endl;
}
