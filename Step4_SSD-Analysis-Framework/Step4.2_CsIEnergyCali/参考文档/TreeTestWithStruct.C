////////////////////////////////////////////////////////////////////////////////
//
//   使用 C++ 中的结构体来定义 Tree 中的 branch
//
////////////////////////////////////////////////////////////////////////////////

const Int_t kMaxMulti = 10;

//______________________________________________________________________________
struct CalibratedEvent
{
  Int_t    fMulti;
  Int_t    fStripb[kMaxMulti];
  Int_t    fStripf[kMaxMulti];
  Double_t fESib[kMaxMulti];
  Double_t fESif[kMaxMulti];
};


//______________________________________________________________________________
void TreeTestWithStruct()
{
  clock_t starttime, endtime;
  starttime = clock();

  CalibratedEvent event;

  TFile* rootfile = new TFile("TreeWithStruct.root","RECREATE");
  TTree* CSHINE = new TTree("CSHINE2019","A Tree with Struct");
  CSHINE->Branch("fMulti", &event.fMulti,  "fMulti/I");
  CSHINE->Branch("fStripb", event.fStripb, "fStripb[fMulti]/I");
  CSHINE->Branch("fStripf", event.fStripb, "fStripf[fMulti]/I");
  CSHINE->Branch("fESib",   event.fESib,   "fESib[fMulti]/D");
  CSHINE->Branch("fESif",   event.fESif,   "fESif[fMulti]/D");

  Int_t multi;
  for (Int_t loop=0; loop<100000; loop++)
  {
    multi  = (rand()%(3-1))  + 1;

    event.fMulti = multi;
    for (Int_t m=0; m<multi; m++)
    {
      event.fStripb[m] = (rand()%(15-0)) + 1;
      event.fStripf[m] = (rand()%(15-0)) + 1;
      event.fESib  [m] = TMath::Abs(gRandom->Gaus(100.,20));
      event.fESif  [m] = TMath::Abs(gRandom->Gaus(50.,10));

      cout<<loop<<setw(10)<<multi<<setw(10)<<m<<setw(10)<<event.fStripb[m]<<setw(10)
          <<event.fStripf[m]<<setw(10)<<event.fESib[m]<<setw(10)<<event.fESif[m]
          <<endl;
    }
    CSHINE->Fill(); // 注意 tree 的填充应该在这里进行!

  }
  CSHINE->Write();
  rootfile->Close();


  endtime = clock();
  cout<<"Total time = "<<(double)(endtime-starttime)/CLOCKS_PER_SEC<<"s"<<endl;
}
