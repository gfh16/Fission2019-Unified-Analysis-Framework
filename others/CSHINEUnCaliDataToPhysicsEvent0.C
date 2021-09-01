#include "../include/CSHINEUnCaliDataToPhysicsEvent.h"


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
CSHINEUnCaliDataToPhysicsEvent::CSHINEUnCaliDataToPhysicsEvent()
{}

CSHINEUnCaliDataToPhysicsEvent::CSHINEUnCaliDataToPhysicsEvent(const char* pathrootin) : fChain_MapRoot(0)
{
	pathToROOTINPUT = pathrootin;
}

CSHINEUnCaliDataToPhysicsEvent::~CSHINEUnCaliDataToPhysicsEvent()
{}
//______________________________________________________________________________

//______________________________________________________________________________
void CSHINEUnCaliDataToPhysicsEvent::FillAndBuildGlobalEvent(const char* pathrootout)
{
  TFile fileout(pathrootout,"RECREATE");

  TTree newtree("GlobalEvent", "Global event tree");
	newtree.Branch("SSDEvent.","GlobalEvent of SSD Array",&globalevent,32000,2);

  cout<<Form("Building GlobalEvent File %s ......\n",fileout.GetName());
  //fChain_MapRoot->SetEntries(10000);
	Long64_t nentries = fChain_MapRoot->GetEntriesFast();
	cout<<"nentries = "<<nentries<<endl;

	for (Long64_t ientry=0; ientry<nentries; ientry++) {

    fChain_MapRoot->GetEntry(ientry);
    timer.PrintPercentageAndRemainingTime(ientry, nentries);

	//	CSHINEUnCaliDataToPhysicsEvent_SSD::FillLayerEvent();
	//	CSHINEUnCaliDataToPhysicsEvent_SSD::FillTrackEvent_L2L3();
	//	l2l3track.L2L3_g1_TrackDecoded(globalevent,trackevent,kTRUE);
		//l2l3track.L2L3_g2_TrackDecoded(globalevent,trackevent,kTRUE);
		//l2l3track.L2L3_g3_TrackDecoded(globalevent,trackevent,kTRUE);

    newtree.Fill();
		trackevent.Swap();
	  globalevent.Swap();
	}
	newtree.Write();
	fileout.Close();
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
CSHINEUnCaliDataToPhysicsEvent_SSD::CSHINEUnCaliDataToPhysicsEvent_SSD()
{}

CSHINEUnCaliDataToPhysicsEvent_SSD::CSHINEUnCaliDataToPhysicsEvent_SSD()
{
	fPathRootInput = pathToROOTINPUT;
	Init_MapRootTree_SSD();
	Init_SiCaliPars();
}

CSHINEUnCaliDataToPhysicsEvent_SSD::~CSHINEUnCaliDataToPhysicsEvent_SSD()
{
	if (!fChain_MapRoot) {
		cout<<"Error: MapRoot Input not exist!"<<endl;
		return;
	}
}

void CSHINEUnCaliDataToPhysicsEvent_SSD::PrintUsage()
{}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEUnCaliDataToPhysicsEvent_SSD::Init_MapRootTree_SSD()
{
	if (!readfile.IsFileExists(fPathRootInput)) {
		printf("Error: file %s not exist!\n",fPathRootInput.c_str());
		return;
	}

	fFileRootIn = new TFile(fPathRootInput.c_str(),"READONLY");
	fChain_MapRoot = (TChain*)fFileRootIn->Get("KrPb");

	// firstly, close all the branches
	fChain_MapRoot->SetBranchStatus("*", false);
	// then, open selected branches
	fChain_MapRoot->SetBranchStatus(b_SSD1_L2F_T, true);
	fChain_MapRoot->SetBranchStatus(b_SSD1_L1S_E, true);
	fChain_MapRoot->SetBranchStatus(b_SSD1_L2F_E, true);
	fChain_MapRoot->SetBranchStatus(b_SSD1_L2B_E, true);
	fChain_MapRoot->SetBranchStatus(b_SSD1_L3A_E, true);
	fChain_MapRoot->SetBranchStatus(b_SSD2_L2F_T, true);
	fChain_MapRoot->SetBranchStatus(b_SSD2_L1S_E, true);
	fChain_MapRoot->SetBranchStatus(b_SSD2_L2F_E, true);
	fChain_MapRoot->SetBranchStatus(b_SSD2_L2B_E, true);
	fChain_MapRoot->SetBranchStatus(b_SSD2_L3A_E, true);
	fChain_MapRoot->SetBranchStatus(b_SSD3_L2F_T, true);
	fChain_MapRoot->SetBranchStatus(b_SSD3_L1S_E, true);
	fChain_MapRoot->SetBranchStatus(b_SSD3_L2F_E, true);
	fChain_MapRoot->SetBranchStatus(b_SSD3_L2B_E, true);
	fChain_MapRoot->SetBranchStatus(b_SSD3_L3A_E, true);
	fChain_MapRoot->SetBranchStatus(b_SSD4_L2F_T, true);
	fChain_MapRoot->SetBranchStatus(b_SSD4_L1S_E, true);
	fChain_MapRoot->SetBranchStatus(b_SSD4_L2F_E, true);
	fChain_MapRoot->SetBranchStatus(b_SSD4_L2B_E, true);
	fChain_MapRoot->SetBranchStatus(b_SSD4_L3A_E, true);
  // finally, set branch address
	fChain_MapRoot->SetBranchAddress(b_SSD1_L2F_T, SSD_L2F_T[0]);
	fChain_MapRoot->SetBranchAddress(b_SSD1_L1S_E, SSD_L1S_E[0]);
	fChain_MapRoot->SetBranchAddress(b_SSD1_L2F_E, SSD_L2F_E[0]);
	fChain_MapRoot->SetBranchAddress(b_SSD1_L2B_E, SSD_L2B_E[0]);
	fChain_MapRoot->SetBranchAddress(b_SSD1_L3A_E, SSD_L3A_E[0]);
	fChain_MapRoot->SetBranchAddress(b_SSD2_L2F_T, SSD_L2F_T[1]);
	fChain_MapRoot->SetBranchAddress(b_SSD2_L1S_E, SSD_L1S_E[1]);
	fChain_MapRoot->SetBranchAddress(b_SSD2_L2F_E, SSD_L2F_E[1]);
	fChain_MapRoot->SetBranchAddress(b_SSD2_L2B_E, SSD_L2B_E[1]);
	fChain_MapRoot->SetBranchAddress(b_SSD2_L3A_E, SSD_L3A_E[1]);
	fChain_MapRoot->SetBranchAddress(b_SSD3_L2F_T, SSD_L2F_T[2]);
	fChain_MapRoot->SetBranchAddress(b_SSD3_L1S_E, SSD_L1S_E[2]);
	fChain_MapRoot->SetBranchAddress(b_SSD3_L2F_E, SSD_L2F_E[2]);
	fChain_MapRoot->SetBranchAddress(b_SSD3_L2B_E, SSD_L2B_E[2]);
	fChain_MapRoot->SetBranchAddress(b_SSD3_L3A_E, SSD_L3A_E[2]);
	fChain_MapRoot->SetBranchAddress(b_SSD4_L2F_T, SSD_L2F_T[3]);
	fChain_MapRoot->SetBranchAddress(b_SSD4_L1S_E, SSD_L1S_E[3]);
	fChain_MapRoot->SetBranchAddress(b_SSD4_L2F_E, SSD_L2F_E[3]);
	fChain_MapRoot->SetBranchAddress(b_SSD4_L2B_E, SSD_L2B_E[3]);
	fChain_MapRoot->SetBranchAddress(b_SSD4_L3A_E, SSD_L3A_E[3]);
}


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEUnCaliDataToPhysicsEvent_SSD::Init_SiCaliPars()
{
	fSiEChcutl1s   = fCSHINESSDCalibratedData.GetSiEChCut (L1STag, NumSigma);
  fSiEChcutl2f   = fCSHINESSDCalibratedData.GetSiEChCut (L2FTag, NumSigma);
  fSiEChcutl2b   = fCSHINESSDCalibratedData.GetSiEChCut (L2BTag, NumSigma);
  fCsIEChcutl3a  = fCSHINESSDCalibratedData.GetCsIEChCut(L3ATag, NumSigma);

  fSlopel1s      = fCSHINESSDCalibratedData.GetSiCaliSlope(L1STag);
  fSlopel2f      = fCSHINESSDCalibratedData.GetSiCaliSlope(L2FTag);
  fSlopel2b      = fCSHINESSDCalibratedData.GetSiCaliSlope(L2BTag);

  fInterceptl1s = fCSHINESSDCalibratedData.GetSiCaliIntercept(L1STag);
  fInterceptl2f = fCSHINESSDCalibratedData.GetSiCaliIntercept(L2FTag);
  fInterceptl2b = fCSHINESSDCalibratedData.GetSiCaliIntercept(L2BTag);
}


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEUnCaliDataToPhysicsEvent_SSD::FillLayerEvent()
{
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
	{
		SSDL1SMulti[ssdindex]=0;
		SSDL2FMulti[ssdindex]=0;
		SSDL2BMulti[ssdindex]=0;
		SSDCsIMulti[ssdindex]=0;

	  for (Int_t strip=0; strip<NUM_STRIP; strip++) {
	    // for L1S
	    if (SSD_L1S_E[ssdindex][strip]>fSiEChcutl1s[ssdindex*NUM_STRIP+strip]) {
	      SSDL1SMulti[ssdindex]++;
	      layerevent.fL1SNumStrip[ssdindex].push_back(strip);
	      layerevent.fL1SEMeV[ssdindex].push_back(fSlopel1s[ssdindex*NUM_STRIP+strip]*SSD_L1S_E[ssdindex][strip]+fInterceptl1s[ssdindex*NUM_STRIP+strip]);
	    }
	    // for L2F
	    if (SSD_L2F_E[ssdindex][strip]>fSiEChcutl2f[ssdindex*NUM_STRIP+strip]) {
	      if (ssdindex==0 || ssdindex==2) {
	        SSDL2FMulti[ssdindex]++;
	        layerevent.fL2FNumStrip[ssdindex].push_back(strip);
	        layerevent.fL2FEMeV[ssdindex].push_back(fSlopel2f[ssdindex*NUM_STRIP+strip]*SSD_L2F_E[ssdindex][strip]+fInterceptl2f[ssdindex*NUM_STRIP+strip]);
	        layerevent.fL2FTime[ssdindex].push_back(SSD_L2F_T[ssdindex][strip]);
	      }
	      if (ssdindex==1) {
	        if (strip!=0 && strip!=1) { // 对于 SSD2_L2F, 扔掉 CH[0] 与 CH[1]
	          SSDL2FMulti[ssdindex]++;
	          layerevent.fL2FNumStrip[ssdindex].push_back(strip);
	          layerevent.fL2FEMeV[ssdindex].push_back(fSlopel2f[ssdindex*NUM_STRIP+strip]*SSD_L2F_E[ssdindex][strip]+fInterceptl2f[ssdindex*NUM_STRIP+strip]);
	          layerevent.fL2FTime[ssdindex].push_back(SSD_L2F_T[ssdindex][strip]);
	        }
	      }
	      if (ssdindex==3) {
	        if (strip!=13 && strip!=14) { // 对于 SSD4_L2F, 扔掉 CH[13] 与 CH[14]
	          SSDL2FMulti[ssdindex]++;
	          layerevent.fL2FNumStrip[ssdindex].push_back(strip);
	          layerevent.fL2FEMeV[ssdindex].push_back(fSlopel2f[ssdindex*NUM_STRIP+strip]*SSD_L2F_E[ssdindex][strip]+fInterceptl2f[ssdindex*NUM_STRIP+strip]);
	          layerevent.fL2FTime[ssdindex].push_back(SSD_L2F_T[ssdindex][strip]);
	        }
	      }
	    }
	    // for L2B
	    if (SSD_L2B_E[ssdindex][strip]>fSiEChcutl2b[ssdindex*NUM_STRIP+strip]) {
	      SSDL2BMulti[ssdindex]++;
	      layerevent.fL2BNumStrip[ssdindex].push_back(strip);
	      layerevent.fL2BEMeV[ssdindex].push_back(fSlopel2b[ssdindex*NUM_STRIP+strip]*SSD_L2B_E[ssdindex][strip]+fInterceptl2b[ssdindex*NUM_STRIP+strip]);
	    }
	  }
	  // for L3A
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
	    if (SSD_L3A_E[ssdindex][csiindex]>fCsIEChcutl3a[ssdindex*NUM_CSI+csiindex]) {
				if ((ssdindex!=3) || (csiindex!=4 && csiindex!=5)) { // SSD4_CsI[4], SSD4_CsI[5] 数据异常, 扔掉
					SSDCsIMulti[ssdindex]++;
		      layerevent.fCsINum[ssdindex].push_back(csiindex);
		      layerevent.fCsIECh[ssdindex].push_back(SSD_L3A_E[ssdindex][csiindex]);
				}
	    }
	  }
	  layerevent.fL1SMulti[ssdindex] = SSDL1SMulti[ssdindex];
	  layerevent.fL2FMulti[ssdindex] = SSDL2FMulti[ssdindex];
	  layerevent.fL2BMulti[ssdindex] = SSDL2BMulti[ssdindex];
	  layerevent.fCsIMulti[ssdindex] = SSDCsIMulti[ssdindex];
	}
}


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEUnCaliDataToPhysicsEvent_SSD::FillTrackEvent_L2L3()
{
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
	{
    if ((layerevent.fCsIMulti[ssdindex]>0 && layerevent.fCsIMulti[ssdindex]<=MULTICUT_L3A) &&
        (layerevent.fL2BMulti[ssdindex]>0 && layerevent.fL2BMulti[ssdindex]<=MULTICUT_L2B) &&
        (layerevent.fL2FMulti[ssdindex]>0 && layerevent.fL2FMulti[ssdindex]<=MULTICUT_L2F) &&
        (layerevent.fL1SMulti[ssdindex]>0 && layerevent.fL1SMulti[ssdindex]<=MULTICUT_L1S)) // 给定初步的 multi cut
    {
      for (Int_t csimulti=0; csimulti<layerevent.fCsIMulti[ssdindex]; csimulti++) {
        for (Int_t l2bmulti=0; l2bmulti<layerevent.fL2BMulti[ssdindex]; l2bmulti++) {
          for (Int_t l2fmulti=0; l2fmulti<layerevent.fL2FMulti[ssdindex]; l2fmulti++) {
            for (Int_t l1smulti=0; l1smulti<layerevent.fL1SMulti[ssdindex]; l1smulti++) { // 逐层循环,遍历所有可能的组合

              if (trackresconstruct.IsGeoConstraint_L3A_L2B(layerevent.fCsINum[ssdindex][csimulti], layerevent.fL2BNumStrip[ssdindex][l2bmulti]) &&
                  trackresconstruct.IsGeoConstraint_L3A_L2F(layerevent.fCsINum[ssdindex][csimulti], layerevent.fL2FNumStrip[ssdindex][l2fmulti]) &&
                  trackresconstruct.IsGeoConstraint_L2B_L1S(layerevent.fL2BNumStrip[ssdindex][l2bmulti], layerevent.fL1SNumStrip[ssdindex][l1smulti]))
              {
								TelNum_BananaCut = ssdindex*NUM_CSI + layerevent.fCsINum[ssdindex][csimulti];

                trackevent.fCsINum[ssdindex].push_back(layerevent.fCsINum[ssdindex][csimulti]);
                trackevent.fCsIECh[ssdindex].push_back(layerevent.fCsIECh[ssdindex][csimulti]);
                trackevent.fL2BNumStrip[ssdindex].push_back(layerevent.fL2BNumStrip[ssdindex][l2bmulti]);
                trackevent.fL2BEMeV[ssdindex].push_back(layerevent.fL2BEMeV[ssdindex][l2bmulti]);
                trackevent.fL2FNumStrip[ssdindex].push_back(layerevent.fL2FNumStrip[ssdindex][l2fmulti]);
                trackevent.fL2FEMeV[ssdindex].push_back(layerevent.fL2FEMeV[ssdindex][l2fmulti]);
                trackevent.fL1SNumStrip[ssdindex].push_back(layerevent.fL1SNumStrip[ssdindex][l1smulti]);
                trackevent.fL1SEMeV[ssdindex].push_back(layerevent.fL1SEMeV[ssdindex][l1smulti]);
                trackevent.fL2FTime[ssdindex].push_back(layerevent.fL2FTime[ssdindex][l2fmulti]);
								trackevent.fCutTelNum[ssdindex].push_back(TelNum_BananaCut);
              }
            }
          }
        }
      }
    }
  }
	layerevent.Swap();
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// PPAC 信号刻度
CSHINEUnCaliDataToPhysicsEvent_PPAC::CSHINEUnCaliDataToPhysicsEvent_PPAC()
{}

CSHINEUnCaliDataToPhysicsEvent_PPAC::CSHINEUnCaliDataToPhysicsEvent_PPAC(const char* pathrootin)
{}

CSHINEUnCaliDataToPhysicsEvent_PPAC::~CSHINEUnCaliDataToPhysicsEvent_PPAC()
{}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// Au-Si 面垒望远镜事件
CSHINEUnCaliDataToPhysicsEvent_AuSiTel::CSHINEUnCaliDataToPhysicsEvent_AuSiTel()
{}

CSHINEUnCaliDataToPhysicsEvent_AuSiTel::CSHINEUnCaliDataToPhysicsEvent_AuSiTel(const char* pathrootin)
{}

CSHINEUnCaliDataToPhysicsEvent_AuSiTel::~CSHINEUnCaliDataToPhysicsEvent_AuSiTel()
{}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// 高频信号
CSHINEUnCaliDataToPhysicsEvent_RF::CSHINEUnCaliDataToPhysicsEvent_RF()
{}

CSHINEUnCaliDataToPhysicsEvent_RF::CSHINEUnCaliDataToPhysicsEvent_RF(const char* pathrootin)
{}

CSHINEUnCaliDataToPhysicsEvent_RF::~CSHINEUnCaliDataToPhysicsEvent_RF()
{}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
