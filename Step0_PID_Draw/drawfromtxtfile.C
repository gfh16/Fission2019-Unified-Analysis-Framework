void drawfromtxtfile()
{
   gROOT->Reset();
   gStyle->SetPalette(1);
   TH1F *h1 = new TH1F("h1","x distribution",100,-4,4);
   TTree *T = new TTree("ntuple","data from ascii file");
   Long64_t nlines = T->ReadFile("output/SSD2_L2BCH04_L3CH06.txt","x:y:z");
   printf(" found %lld pointsn",nlines);
   T->Draw("z:y");
}
