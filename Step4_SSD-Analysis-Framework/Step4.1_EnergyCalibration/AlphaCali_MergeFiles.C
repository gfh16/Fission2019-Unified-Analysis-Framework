////////////////////////////////////////////////////////////////////////////////
//          MERGE ALPHA CALIBRATION DATA FOR HIGHER STATISTICS                //
//                                                                            //
//     Use TChain to merge alpha calibtation data                             //
//                                                                            //
//     Author gfh                                                             //
//     Date Nov 11, 2019                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


//_____________________________________________________
void WriteHist(TH1D *hist[16], TFile * FileOut);


//______________________________________________________________________________
void AlphaCali_MergeFiles()
{

  const int FirstRun = 33;   // 0-32, 3-48 for L2
  const int LastRun  = 48;

  std::string LayerTag("L2");
  //std::string pathFolderRootInput("/data/EXPdata/Fission2019/MapRoot/");
  std::string pathFolderRootInput("/home/sea/Fission2019_Data/MapRoot/");

  std::string L1Tag("L1");
  std::string L2Tag("L2");

  TFile* FileOut = new TFile(Form("%sMapSSD_%s_AlphaCali%02d_%02d.root", pathFolderRootInput.c_str(),
                                   LayerTag.c_str(), FirstRun, LastRun), "RECREATE");

  auto* myData = new TChain("KrPb");

  for (int i=FirstRun; i<=LastRun; i++)
  {
    myData->Add(Form("%sMapSSD_%s_AlphaCali%04d.root",pathFolderRootInput.c_str(), LayerTag.c_str(),i));
    printf("MapSSD_%s_AlphaCali%04d.root Loaded\n",LayerTag.c_str(),i);
  }
  // I disable all the branches
  myData->SetBranchStatus("*",false);

  Int_t SSD1_L1S_E[16];
  Int_t SSD2_L1S_E[16];
  Int_t SSD3_L1S_E[16];
  Int_t SSD4_L1S_E[16];
  Int_t SSD1_L2F_E[16];
  Int_t SSD1_L2B_E[16];
  Int_t SSD2_L2F_E[16];
  Int_t SSD2_L2B_E[16];
  Int_t SSD3_L2F_E[16];
  Int_t SSD3_L2B_E[16];
  Int_t SSD4_L2F_E[16];
  Int_t SSD4_L2B_E[16];
  if (strcmp(L1Tag.c_str(), LayerTag.c_str())==0)
  {
    // I enable only the branches I need
    myData->SetBranchStatus("SSD1_L1S_E",true);
    myData->SetBranchStatus("SSD2_L1S_E",true);
    myData->SetBranchStatus("SSD3_L1S_E",true);
    myData->SetBranchStatus("SSD4_L1S_E",true);
    myData->SetBranchAddress("SSD1_L1S_E",SSD1_L1S_E);
    myData->SetBranchAddress("SSD2_L1S_E",SSD2_L1S_E);
    myData->SetBranchAddress("SSD3_L1S_E",SSD3_L1S_E);
    myData->SetBranchAddress("SSD4_L1S_E",SSD4_L1S_E);
  }
  if (strcmp(L2Tag.c_str(), LayerTag.c_str())==0)
  {
    myData->SetBranchStatus("SSD1_L2F_E",true);
    myData->SetBranchStatus("SSD1_L2B_E",true);
    myData->SetBranchStatus("SSD2_L2F_E",true);
    myData->SetBranchStatus("SSD2_L2B_E",true);
    myData->SetBranchStatus("SSD3_L2F_E",true);
    myData->SetBranchStatus("SSD3_L2B_E",true);
    myData->SetBranchStatus("SSD4_L2F_E",true);
    myData->SetBranchStatus("SSD4_L2B_E",true);
    myData->SetBranchAddress("SSD1_L2F_E",SSD1_L2F_E);
    myData->SetBranchAddress("SSD1_L2B_E",SSD1_L2B_E);
    myData->SetBranchAddress("SSD2_L2F_E",SSD2_L2F_E);
    myData->SetBranchAddress("SSD2_L2B_E",SSD2_L2B_E);
    myData->SetBranchAddress("SSD3_L2F_E",SSD3_L2F_E);
    myData->SetBranchAddress("SSD3_L2B_E",SSD3_L2B_E);
    myData->SetBranchAddress("SSD4_L2F_E",SSD4_L2F_E);
    myData->SetBranchAddress("SSD4_L2B_E",SSD4_L2B_E);
  }

  TH1D * Hist_L1S[4][16];
  TH1D * Hist_L2F[4][16];
  TH1D * Hist_L2B[4][16];
  for (int SSDNum=0; SSDNum<4; SSDNum++)
  {
    for (int CHNum=0; CHNum<16; CHNum++)
    {
      Hist_L1S[SSDNum][CHNum] = new TH1D(Form("SSD%d_L1S_E_CH%02d",SSDNum+1,CHNum),Form("SSD%d_L1S_E_CH%02d",SSDNum+1,CHNum),4096,0,4096);
      Hist_L2F[SSDNum][CHNum] = new TH1D(Form("SSD%d_L2F_E_CH%02d",SSDNum+1,CHNum),Form("SSD%d_L2F_E_CH%02d",SSDNum+1,CHNum),4096,0,4096);
      Hist_L2B[SSDNum][CHNum] = new TH1D(Form("SSD%d_L2B_E_CH%02d",SSDNum+1,CHNum),Form("SSD%d_L2B_E_CH%02d",SSDNum+1,CHNum),4096,0,4096);
    }
  }
  // Loop on the data to fill the histograms. (event by event)
  unsigned long nentries = myData->GetEntries();
  cout<< "Found " << nentries <<" entries\n";
  for (unsigned long j=0; j<nentries; j++)
  {
    myData->GetEntry(j);

    if (strcmp(L1Tag.c_str(), LayerTag.c_str())==0)
    {
       for(int CHNum=0; CHNum<16; CHNum++)
       {
         Hist_L1S[0][CHNum]->Fill(SSD1_L1S_E[CHNum],1.0);
         Hist_L1S[1][CHNum]->Fill(SSD2_L1S_E[CHNum],1.0);
         Hist_L1S[2][CHNum]->Fill(SSD3_L1S_E[CHNum],1.0);
         Hist_L1S[3][CHNum]->Fill(SSD4_L1S_E[CHNum],1.0);
       }
    }
    if (strcmp(L2Tag.c_str(), LayerTag.c_str())==0)
    {
      for (int CHNum=0; CHNum<16; CHNum++)
      {
        Hist_L2F[0][CHNum]->Fill(SSD1_L2F_E[CHNum],1.0);
        Hist_L2B[0][CHNum]->Fill(SSD1_L2B_E[CHNum],1.0);
        Hist_L2F[1][CHNum]->Fill(SSD2_L2F_E[CHNum],1.0);
        Hist_L2B[1][CHNum]->Fill(SSD2_L2B_E[CHNum],1.0);
        Hist_L2F[2][CHNum]->Fill(SSD3_L2F_E[CHNum],1.0);
        Hist_L2B[2][CHNum]->Fill(SSD3_L2B_E[CHNum],1.0);
        Hist_L2F[3][CHNum]->Fill(SSD4_L2F_E[CHNum],1.0);
        Hist_L2B[3][CHNum]->Fill(SSD4_L2B_E[CHNum],1.0);
      }
    }
  } //==== close for(unsigned long j=0; j<nentries; j++)
  // Write the data into .root file
  if (strcmp(L1Tag.c_str(), LayerTag.c_str())==0)
  {
     WriteHist(Hist_L1S[0],FileOut);
     WriteHist(Hist_L1S[1],FileOut);
     WriteHist(Hist_L1S[2],FileOut);
     WriteHist(Hist_L1S[3],FileOut);
  }
  if (strcmp(L2Tag.c_str(), LayerTag.c_str())==0)
  {
    WriteHist(Hist_L2F[0],FileOut);
    WriteHist(Hist_L2B[0],FileOut);
    WriteHist(Hist_L2F[1],FileOut);
    WriteHist(Hist_L2B[1],FileOut);
    WriteHist(Hist_L2F[2],FileOut);
    WriteHist(Hist_L2B[2],FileOut);
    WriteHist(Hist_L2F[3],FileOut);
    WriteHist(Hist_L2B[3],FileOut);
  }
  FileOut->Close();
  delete myData;
}

//______________________________________________________________________________
// WriteHist is to Write the histograms
void WriteHist(TH1D* hist[16], TFile* FileOut)
{
  for(int CHNum=0; CHNum<16; CHNum++)
  {
    FileOut->WriteTObject(hist[CHNum],hist[CHNum]->GetName());
  }
}
