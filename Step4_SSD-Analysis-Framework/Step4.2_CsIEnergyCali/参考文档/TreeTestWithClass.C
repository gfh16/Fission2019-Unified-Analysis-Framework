////////////////////////////////////////////////////////////////////////////////
//
//   使用 C++ 中的类来定义 Tree 中的 branch
//
////////////////////////////////////////////////////////////////////////////////

#include "CalibratedEvent.h"



//______________________________________________________________________________
void TreeTestWithClass()
{

  CalibratedEvent* event = new CalibratedEvent();

  TFile* rootfile = new TFile("TreeWithClass.root","RECREATE");
  TTree* CSHINE = new TTree("CSHINE2019","A Tree with Struct");
  CSHINE->Branch("Event1.","CalibratedEvent", &event, 32000,99);

  Int_t multi;
  for (Int_t loop=0; loop<100000; loop++)
  {
    multi  = (rand()%(3-1))  + 1;
    event->fMulti = multi;

    for (Int_t m=0; m<multi; m++)
    {
      event->fStripb[m] = (rand()%(15-0)) + 1;
      event->fStripf[m] = (rand()%(15-0)) + 1;
      event->fESib  [m] = TMath::Abs(gRandom->Gaus(100.,20));
      event->fESif  [m] = TMath::Abs(gRandom->Gaus(50.,10));

      cout<<loop<<setw(10)<<multi<<setw(10)<<m<<setw(10)<<event->fStripb[m]<<setw(10)
          <<event->fStripf[m]<<setw(10)<<event->fESib[m]<<setw(10)<<event->fESif[m]
          <<endl;
    }
    CSHINE->Fill(); // 注意 tree 的填充应该在这里进行!
  }
  CSHINE->Write();
  rootfile->Close();

}
