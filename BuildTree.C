#include "include/CSHINEUnCaliDataToPhysicsEvent.h"
#include "include/shared.h"
#include "include/CSHINEEvent.h"
#include "include/CSHINETrackReconstructionSimplified.h"
#include "include/TimeAndPercentage.h"
#include "include/CSHINETrackReconstructionSimplified.h"


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void BuildTreeFromFile(Int_t runnumber);

void BuildTree()
{
  for (Int_t irun=300; irun<=303; irun++) {
		BuildTreeFromFile(irun);
	}
}


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void BuildTreeFromFile(Int_t runnumber)
{
	std::string pathrootout = Form("%sCali_Run%04d.root",PATHROOTFILESFOLDER,runnumber);
  std::string pathrootin  = Form("%sMapRoot/MapFission2019_Kr_Pb%04d.root",PATHROOTFILESFOLDER,runnumber);

  CSHINEPPACEvent      *ppacevent   = new CSHINEPPACEvent();
	CSHINEAuSiTelEvent   *AuSievent   = new CSHINEAuSiTelEvent();
	CSHINERFSignal       *rfsignal    = new CSHINERFSignal();
	CSHINELayerEvent2    *layerevent  = new CSHINELayerEvent2();
	CSHINETrackEvent2    *trackevent  = new CSHINETrackEvent2();
	CSHINESSDEvent       *globalevent = new CSHINESSDEvent();
	TimeAndPercentage    *timer       = new TimeAndPercentage();
	L2L3_TrackDecoded    *l2l3track   = new L2L3_TrackDecoded();
  L1L2_TrackDecoded    *l1l2track   = new L1L2_TrackDecoded();

	TFile*  filein = new TFile(pathrootin.c_str());
	TChain* mychain = (TChain*) filein->Get("KrPb");
	mychain->SetBranchStatus("*", false);
	CSHINEUnCaliDataToPhysicsEvent_SSD     *buildssdevent  = new CSHINEUnCaliDataToPhysicsEvent_SSD(mychain);
	CSHINEUnCaliDataToPhysicsEvent_PPAC    *buildppacevent = new CSHINEUnCaliDataToPhysicsEvent_PPAC(mychain);
	CSHINEUnCaliDataToPhysicsEvent_AuSiTel *buildAuSievent = new CSHINEUnCaliDataToPhysicsEvent_AuSiTel(mychain);
	CSHINEUnCaliDataToPhysicsEvent_RF      *buildrfsignal  = new CSHINEUnCaliDataToPhysicsEvent_RF(mychain);

	TFile* fileout = new TFile(pathrootout.c_str(), "RECREATE");
	fileout->cd();

	TTree* newtree = new TTree("KrPbEvent", "Global event tree");
  newtree->Branch("SSD.",  "CSHINESSDEvent",     &globalevent, 32000, 2);
	newtree->Branch("PPAC.", "CSHINEPPACEvent",    &ppacevent,   32000, 2);
	newtree->Branch("AuSi.", "CSHINEAuSiTelEvent", &AuSievent,   32000, 2);
	newtree->Branch("RF.",   "CSHINERFSignal",     &rfsignal,    32000, 2);

  //mychain->SetEntries(10000);
	Long64_t nentries = mychain->GetEntries();
	cout<<Form("Building GlobalEvent File %s ......\n",fileout->GetName());
	cout<<"nentries = "<<nentries<<endl;

	for (Long64_t ientry=0; ientry<nentries; ientry++) {

    mychain->GetEntry(ientry);
    timer->PrintPercentageAndRemainingTime(ientry, nentries);

		//__________________________________________________________________
    //                           for SSD
    //                         -----------
    buildssdevent->FillLayerEvent(layerevent);

    // for L2-L3
	  buildssdevent->FillTrackEvent_L2L3(trackevent,layerevent);

    l2l3track->L2L3_g1_TrackDecoded(globalevent,trackevent,kTRUE);
		l2l3track->L2L3_g2_TrackDecoded(globalevent,trackevent,kTRUE);
		l2l3track->L2L3_g3_TrackDecoded(globalevent,trackevent,kTRUE);
    l2l3track->L2L3_g4_TrackDecoded(globalevent,trackevent,kTRUE);
    l2l3track->L2L3_g6_TrackDecoded(globalevent,trackevent,kTRUE);


    // for L1-L2
    buildssdevent->FillTrackEvent_L1L2(trackevent,layerevent);
    l1l2track->L1L2_g1_TrackDecoded(globalevent,trackevent,kTRUE);
    l1l2track->L1L2_g2_TrackDecoded(globalevent,trackevent,kTRUE);
    l1l2track->L1L2_g3_TrackDecoded(globalevent,trackevent,kTRUE);
    l1l2track->L1L2_g4_TrackDecoded(globalevent,trackevent,kTRUE);
    l1l2track->L1L2_g6_TrackDecoded(globalevent,trackevent,kTRUE);
   

		// for PPAC
		//buildppacevent->FillPPACEvent(ppacevent);

		// for AuSi Tele
    //buildAuSievent->FillAuSiTelEvent(AuSievent);

    // for RF
		//buildrfsignal->FillRFSignal(rfsignal);

    newtree->Fill();

    ppacevent->Swap();
		AuSievent->Swap();
		layerevent->Swap();
		trackevent->Swap();
	  globalevent->Swap();
	}
	cout<<endl;
	newtree->Write();
	fileout->Close();
	filein->Close();

  // 释放内存
	delete ppacevent;
	delete AuSievent;
	delete rfsignal;
	delete layerevent;
	delete trackevent;
	delete globalevent;
	delete timer;
	delete l2l3track;
  delete l1l2track;
	delete buildssdevent;
}
