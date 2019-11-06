void Step1_DEEPoints_1st_CutsForDEELines()
{
  std::string path_to_file ("RootData/HiRA_DECal_CsIRaw_Pixelization.root");
  TFile * FileIn = new TFile(path_to_file.c_str());

  TFile * fileout = new TFile("RootData/DECal_CsIRaw_Cut.root", "UPDATE");

  TIter next(FileIn->GetListOfKeys());
  TKey *key;

  while ((key = (TKey*)next()))
  {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TH2D")) continue;
    TH2D *hist = (TH2D*)key->ReadObj();

    std::string HistName(hist->GetName());
    if(HistName.find("DECal_ERaw")==std::string::npos) continue;

    for(int i=0; i<8; i++)
    {
    std::string CutName;
    if(i==0 && fileout->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),1,1))==0) {
      printf("Please Draw cut on protons\n");
      CutName.assign(HistName+Form("_Z%02d_A%02d",1,1));
    }
    if(i==1 && fileout->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),1,2))==0) {
      printf("Please Draw cut on deuterons\n");
      CutName.assign(HistName+Form("_Z%02d_A%02d",1,2));
    }
    if(i==2 && fileout->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),1,3))==0) {
      printf("Please Draw cut on tritons\n");
      CutName.assign(HistName+Form("_Z%02d_A%02d",1,3));
    }
    if(i==3 && fileout->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),2,3))==0) {
      printf("Please Draw cut on 3He\n");
      CutName.assign(HistName+Form("_Z%02d_A%02d",2,3));
    }
    if(i==4 && fileout->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),2,4))==0) {
      printf("Please Draw cut on 4He\n");
      CutName.assign(HistName+Form("_Z%02d_A%02d",2,4));
    }
    if(i==5 && fileout->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),3,6))==0) {
      continue;
      printf("Please Draw cut on 6Li\n");
      CutName.assign(HistName+Form("_Z%02d_A%02d",3,6));
    }
    if(i==6 && fileout->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),3,7))==0) {
      continue;
      printf("Please Draw cut on 7Li\n");
      CutName.assign(HistName+Form("_Z%02d_A%02d",3,7));
    }
    if(i==7 && fileout->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),4,7))==0) {
      continue;
      printf("Please Draw cut on 7Be\n");
      CutName.assign(HistName+Form("_Z%02d_A%02d",4,7));
    }
    if(fileout->Get(CutName.c_str()) || CutName.empty()) continue; //cut already existing

    hist->Draw("colz");
    gPad->SetLogz(kTRUE);

    TCutG * mycut = (TCutG*)gPad->WaitPrimitive("CUTG");

    mycut->SetName(CutName.c_str());

    printf("%s cut created\n", CutName.c_str());
    printf("S - save the cut\nR - retry\nF - skip this cut\nD - delete a cut\nE - exit\n");
    char input;
    input=getchar();
    getchar();
    printf("you typed %c\n", input);
    if (input=='S' || input=='s') {
      fileout->WriteTObject(mycut, mycut->GetName());
      fileout->WriteTObject(hist, hist->GetName());
      printf ("%s cut saved to file\n", CutName.c_str());
    }
    else if (input == 'R' || input =='r') {
      i--;
    }
    else if (input == 'F' || input == 'f') {
    }
    else if (input == 'D' || input == 'd') {
      printf("Please type the cut to delete: \n");
      std::string CutToDelete;
      std::cin>>CutToDelete;
      CutToDelete.append(";*");
      fileout->Delete(CutToDelete.c_str());
      printf("deleted %s cut\n", CutToDelete.c_str());
      i--;
      std::cin.get();
    }
    else if (input == 'E' || input == 'e'){
      printf("Goodbye\n");
      fileout->Close();
      return;
    }
    else i--;
    }
  }

  fileout->Close();
  return;
}
