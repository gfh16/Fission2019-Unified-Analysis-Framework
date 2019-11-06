// This macro is used to find the pedestals for Energy Channel of SSD

void FindPedestals()
{
   std::string path_to_file("data/QC_MapSSD_PulserCali_Pedestal0000.root");
   TFile * FileIn = new TFile(path_to_file.c_str());

   std::string FileOutTag("L1S_E");

   ofstream FileOut(Form("output/SSD_Pedestals_%s.dat", FileOutTag.c_str()));
   FileOut << "*SSDNum" << setw(10) << "CHNum" << setw(20) << "Pedestal Cut"<< endl;



   TIter next(FileIn->GetListOfKeys());
   TKey * key;

   while((key = (TKey*)next()))
   {
     TClass *cl = gROOT->GetClass(key->GetClassName());
     if(!cl->InheritsFrom("TH1D")) continue;
     TH1D * hist = (TH1D*)key->ReadObj();

     std::string HistName(hist->GetName());

     for(int i=0; i<4; i++)
     {
       for(int j=0; j<16; j++)
       {
         if(HistName != Form("SSD%d_%s_CH%d",(i+1),FileOutTag.c_str(),j)) continue;
         //printf("SSD%d_%s_CH%d\n", i+1,FileOutTag.c_str(),j);


       }
     }

   }
}


//=====================================================================
// click on the histograms by hand, and save the clicks
// as the valuea  pedestals cut
void SetClicks(Int_t event, Int_t x, TObject *selected)
{

}
