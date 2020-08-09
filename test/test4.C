#include "../include/CSHINEEvent.h"
#include "../include/TimeAndPercentage.h"


//______________________________________________________________________________
void test4()
{
  TimeAndPercentage time;
  time.GetBeginTime();

  TFile* file = new TFile("test4.root","RECREATE");
  TTree* t_event    = new TTree("CSHINEEvent", "SSDEvent tree");

  CSHINESSDEvent    *event1  = new CSHINESSDEvent();
  CSHINESSDEvent    *event2  = new CSHINESSDEvent();
  CSHINESSDEvent    *event3  = new CSHINESSDEvent();
  CSHINESSDEvent    *event4  = new CSHINESSDEvent();
  CSHINEGlobalEvent *event_g = new CSHINEGlobalEvent();

  t_event->Branch("SSD1.",   "CSHINESSDEvent",    &event1,  32000, 2);
  t_event->Branch("SSD2.",   "CSHINESSDEvent",    &event2,  32000, 2);
  t_event->Branch("SSD3.",   "CSHINESSDEvent",    &event3,  32000, 2);
  t_event->Branch("SSD4.",   "CSHINESSDEvent",    &event4,  32000, 2);
  t_event->Branch("Global.", "CSHINEGlobalEvent", &event_g, 32000, 2);

  //Int_t multi;
  Long64_t nentries = 5e4;
  for (Long64_t loop=0; loop<nentries; loop++)
  {
    time.PrintPercentageAndRemainingTime(loop, nentries);

    event1->fMultiL1S = Int_t (abs(gRandom->Gaus(0,1)));
    event1->fMultiL2F = event1->fMultiL1S;
    event1->fMultiL2B = event1->fMultiL1S;

    event2->fMultiL1S = Int_t (abs(gRandom->Gaus(0,1)));
    event2->fMultiL2F = event2->fMultiL1S;
    event2->fMultiL2B = event2->fMultiL1S;

    event3->fMultiL1S = Int_t (abs(gRandom->Gaus(0,1)));
    event3->fMultiL2F = event3->fMultiL1S;
    event3->fMultiL2B = event3->fMultiL1S;

    event4->fMultiL1S = Int_t (abs(gRandom->Gaus(0,1)));
    event4->fMultiL2F = event4->fMultiL1S;
    event4->fMultiL2B = event4->fMultiL1S;

    // SSD1
    for (Int_t m=0; m<event1->fMultiL1S; m++)
    {
      event1->fNumStripL1S[m] = (rand()%16) + 1;
      event1->fNumStripL2F[m] = (rand()%16) + 1;
      event1->fNumStripL2B[m] = (rand()%16) + 1;
      event1->fEMeVL1S    [m] = TMath::Abs(gRandom->Gaus(50,20));
      event1->fEMeVL2F    [m] = TMath::Abs(gRandom->Gaus(70,20));
      event1->fEMeVL2B    [m] = TMath::Abs(gRandom->Gaus(90,20));
    }
    // SSD2
    for (Int_t m=0; m<event2->fMultiL1S; m++)
    {
      event2->fNumStripL1S[m] = (rand()%16) + 1;
      event2->fNumStripL2F[m] = (rand()%16) + 1;
      event2->fNumStripL2B[m] = (rand()%16) + 1;
      event2->fEMeVL1S    [m] = TMath::Abs(gRandom->Gaus(110,20));
      event2->fEMeVL2F    [m] = TMath::Abs(gRandom->Gaus(130,20));
      event2->fEMeVL2B    [m] = TMath::Abs(gRandom->Gaus(150,20));
    }
    //SSD3
    for (Int_t m=0; m<event3->fMultiL1S; m++)
    {
      event3->fNumStripL1S[m] = (rand()%16) + 1;
      event3->fNumStripL2F[m] = (rand()%16) + 1;
      event3->fNumStripL2B[m] = (rand()%16) + 1;
      event3->fEMeVL1S    [m] = TMath::Abs(gRandom->Gaus(170,20));
      event3->fEMeVL2F    [m] = TMath::Abs(gRandom->Gaus(190,20));
      event3->fEMeVL2B    [m] = TMath::Abs(gRandom->Gaus(210,20));
    }
    //SSD4
    for (Int_t m=0; m<event4->fMultiL1S; m++)
    {
      event4->fNumStripL1S[m] = (rand()%16) + 1;
      event4->fNumStripL2F[m] = (rand()%16) + 1;
      event4->fNumStripL2B[m] = (rand()%16) + 1;
      event4->fEMeVL1S    [m] = TMath::Abs(gRandom->Gaus(230,20));
      event4->fEMeVL2F    [m] = TMath::Abs(gRandom->Gaus(250,20));
      event4->fEMeVL2B    [m] = TMath::Abs(gRandom->Gaus(270,20));
    }

    t_event->Fill();
  }
  t_event->Write();
  file->Close();

  time.GetEndTime();
  time.GetRunTime();
}
