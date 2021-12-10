#include "include/CSHINEUnCaliDataToPhysicsEvent.h"
#include "include/shared.h"
#include "include/CSHINEEvent.h"
#include "include/CSHINETrackReconstructionSimplified.h"
#include "include/TimeAndPercentage.h"
#include "include/CSHINETrackReconstructionSimplified.h"


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void BuildEventFromFile(TFile* fileout, TChain* mychain);


//______________________________________________________________________________
void BuildEvent()
{
  const Int_t firstrun = 351;
  const Int_t lastrun  = 351;

  //
  for (Int_t irun=firstrun; irun<=lastrun; irun++) {
    std::string pathrootout = Form("%sCali_Run%04d.root",PATHROOTFILESFOLDER,irun);
    std::string pathrootin  = Form("%sMapRoot/MapFission2019_Kr_Pb%04d.root",PATHROOTFILESFOLDER,irun);

    // 输入 root 文件
    TFile*  filein = new TFile(pathrootin.c_str());
  	TChain* mychain = (TChain*) filein->Get("KrPb");

    // 输出 root 文件
    TFile* fileout = new TFile(pathrootout.c_str(), "RECREATE");

    //
		BuildEventFromFile(fileout, mychain);
    //
  	filein->Close();
	}
}


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void BuildEventFromFile(TFile* fileout, TChain* mychain)
{
  const Int_t NUMSSD = 4;
  // 定义粒子多重性
  Int_t gMulti;
  Int_t SSDMulti[NUMSSD];

  //
  CSHINEPPACEvent      *ppacevent   = new CSHINEPPACEvent();
	CSHINEAuSiTelEvent   *AuSievent   = new CSHINEAuSiTelEvent();
	CSHINERFSignal       *rfsignal    = new CSHINERFSignal();
	CSHINELayerEvent2    *layerevent  = new CSHINELayerEvent2();
	CSHINETrackEvent2    *trackevent  = new CSHINETrackEvent2();
	CSHINESSDEvent       *globalevent = new CSHINESSDEvent();
	TimeAndPercentage    *timer       = new TimeAndPercentage();
	L2L3_TrackDecoded    *l2l3track   = new L2L3_TrackDecoded();
  L1L2_TrackDecoded    *l1l2track   = new L1L2_TrackDecoded();
  //
	mychain->SetBranchStatus("*", false);
	CSHINEUnCaliDataToPhysicsEvent_SSD     *buildssdevent  = new CSHINEUnCaliDataToPhysicsEvent_SSD(mychain);
	CSHINEUnCaliDataToPhysicsEvent_PPAC    *buildppacevent = new CSHINEUnCaliDataToPhysicsEvent_PPAC(mychain);
	CSHINEUnCaliDataToPhysicsEvent_AuSiTel *buildAuSievent = new CSHINEUnCaliDataToPhysicsEvent_AuSiTel(mychain);
	CSHINEUnCaliDataToPhysicsEvent_RF      *buildrfsignal  = new CSHINEUnCaliDataToPhysicsEvent_RF(mychain);

  // ！！！注意: 存储 tree 时, 输出文件需要在输入文件之后打开, 否则会报错 ！！！
  fileout->cd();
  //
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

		//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
    //                           for SSD
    //                         -----------

    // 初始化
    gMulti = 0;
    for (Int_t ssdindex=0; ssdindex<NUMSSD; ssdindex++) SSDMulti[ssdindex] = 0;

    // build LayerEvent && TrackEvent
    buildssdevent->FillLayerEvent(layerevent);
    buildssdevent->FillTrackEvent(trackevent,layerevent);
    // track decoding
    for (Int_t ssdindex=0; ssdindex<NUMSSD; ssdindex++)
    {
      l2l3track->L2L3_AllModes_TrackDecoded(ssdindex,SSDMulti[ssdindex],globalevent,trackevent,kTRUE);
      l1l2track->L1L2_AllModes_TrackDecoded(ssdindex,SSDMulti[ssdindex],globalevent,trackevent,kTRUE);
      //
      globalevent->fSSDGlobalMulti.push_back(SSDMulti[ssdindex]);
    }
    globalevent->fGlobalMulti = SSDMulti[0]+SSDMulti[1]+SSDMulti[2]+SSDMulti[3];

    //oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


    //oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
    // for PPAC
		buildppacevent->FillPPACEvent(ppacevent);

    // for AuSi Tele
    buildAuSievent->FillAuSiTelEvent(AuSievent);

    // for RF
		buildrfsignal->FillRFSignal(rfsignal);

    //oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


    //__________________________________________________________________________
    // fill tree
    newtree->Fill();

    // 清空所有的branch, 等待下一次数据记录
    ppacevent->Swap();
		AuSievent->Swap();
		layerevent->Swap();
		trackevent->Swap();
	  globalevent->Swap();
	}
  newtree->Write();
  fileout->Close();

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
