//___________________________________________________________________
void ReadHistToDraw(Int_t runnumber);

//__________________________________________________________________
void QC_ReadHistToDraw()
{
  for(Int_t i=0; i<=8; i++)
  {
    ReadHistToDraw(i);
  }
}

//____________________________________________________________________
void ReadHistToDraw(Int_t runnumber)
{
 // Int_t runnumber = 0;
  std::string LayerTag("L1");
  std::string fileinpath(Form("QC_HistRoot/QC_MapSSD_%s_AlphaCali%04d.root",LayerTag.c_str(),runnumber));
  std::string pdfpath(Form("QC_pdf/QC_MapSSD_%s_AlphaCali%04d.pdf", LayerTag.c_str(),runnumber));
  std::string pdfbegin(Form("QC_pdf/QC_MapSSD_%s_AlphaCali%04d.pdf[", LayerTag.c_str(),runnumber));
  std::string pdfend(Form("QC_pdf/QC_MapSSD_%s_AlphaCali%04d.pdf]", LayerTag.c_str(),runnumber)); 
  
  TCanvas* c1      = new TCanvas("c1","c1",800,600);
  TCanvas* c_begin = new TCanvas("c_begin", "c_begin");
  TCanvas* c_end   = new TCanvas("c_end", "c_end");
  c_begin->Print(pdfbegin.c_str());

  TFile* filein = new TFile(fileinpath.c_str());
  TIter next(filein->GetListOfKeys());
  TKey* key;

  while ((key = (TKey*)next())) {
    //TClass* cl = gROOT->GetClass(key->GetClassName());
    //if (!cl->InheritsFrom("TH1D")) continue;
    TH1D* hist = (TH1D*)key->ReadObj();
    std::string histname(hist->GetName());
    if(histname.find("SSD")==std::string::npos) continue;
    if(histname.find(LayerTag.c_str())==std::string::npos) continue;
    
    printf("histname = %s\n", histname.c_str());

    c1->cd();
    hist->GetXaxis()->SetRangeUser(150, 400);
    hist->GetYaxis()->SetRangeUser(0, (hist->GetMaximum())*1.1);
    hist->Draw();

    c1->Print(pdfpath.c_str()); 
    c1->Modified();
    c1->Update();
  }
  c_end->Print(pdfend.c_str());

}
