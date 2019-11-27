//___________________________________________________________________
void QC_ReadHistAndDraw()
{
  Int_t runnumber = 0;
  std::string fileinpath("");
  std::string pdfpath();
  std::string pdfbegin();
  std::string pdfend(); 
  
  TCanvas* c1      = new TCanvas("c1","c1",800,600);
  TCanvas* c_begin = new TCanvas("c_begin", "c_begin");
  TCanvas* c_end   = new TCanvas("c_end", "c_end");
  c_begin->Print(pdfpath.c_str());

  TFile* filein = new TFile(fileinpath.c_str());
  TIter* next(filein->GetListOfKeys());
  TKey* key;

  while ((key = (TKey*)next())) {
    TClass* cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TH1D")) continue;
    TH1D* hist = (TH1D*)key->ReadObj();
    hist->Draw()
    
  
  }



}
