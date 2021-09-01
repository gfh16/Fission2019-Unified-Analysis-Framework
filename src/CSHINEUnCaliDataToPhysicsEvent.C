#include "../include/CSHINEUnCaliDataToPhysicsEvent.h"


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
CSHINEUnCaliDataToPhysicsEvent_SSD::CSHINEUnCaliDataToPhysicsEvent_SSD()
{}

CSHINEUnCaliDataToPhysicsEvent_SSD::CSHINEUnCaliDataToPhysicsEvent_SSD(TChain* InputChain) : fChain_MapRoot(0)
{
	fChain_MapRoot = InputChain;
	if (!fChain_MapRoot) {
		cout<<"Error: MapRoot Input not exist!"<<endl;
		return;
	}
	// 初始化
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
	// firstly, close all the branches
	// fChain_MapRoot->SetBranchStatus("*", false);
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
	fSiEChcutl1s   = fCSHINESSDCalibratedData.GetSiEChCut (L1STag, NUMSIGMA);
  fSiEChcutl2f   = fCSHINESSDCalibratedData.GetSiEChCut (L2FTag, NUMSIGMA);
  fSiEChcutl2b   = fCSHINESSDCalibratedData.GetSiEChCut (L2BTag, NUMSIGMA);
  fCsIEChcutl3a  = fCSHINESSDCalibratedData.GetCsIEChCut(L3ATag, NUMSIGMA);

  fSlopel1s      = fCSHINESSDCalibratedData.GetSiCaliSlope(L1STag);
  fSlopel2f      = fCSHINESSDCalibratedData.GetSiCaliSlope(L2FTag);
  fSlopel2b      = fCSHINESSDCalibratedData.GetSiCaliSlope(L2BTag);

  fInterceptl1s = fCSHINESSDCalibratedData.GetSiCaliIntercept(L1STag);
  fInterceptl2f = fCSHINESSDCalibratedData.GetSiCaliIntercept(L2FTag);
  fInterceptl2b = fCSHINESSDCalibratedData.GetSiCaliIntercept(L2BTag);
}


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEUnCaliDataToPhysicsEvent_SSD::FillLayerEvent(CSHINELayerEvent2* layerevent)
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
	      layerevent->fL1SNumStrip[ssdindex].push_back(strip);
	      layerevent->fL1SEMeV[ssdindex].push_back(fSlopel1s[ssdindex*NUM_STRIP+strip]*SSD_L1S_E[ssdindex][strip]+fInterceptl1s[ssdindex*NUM_STRIP+strip]);
	    }
	    // for L2F
	    if (SSD_L2F_E[ssdindex][strip]>fSiEChcutl2f[ssdindex*NUM_STRIP+strip]) {
	      if (ssdindex==0 || ssdindex==2) {
	        SSDL2FMulti[ssdindex]++;
	        layerevent->fL2FNumStrip[ssdindex].push_back(strip);
	        layerevent->fL2FEMeV[ssdindex].push_back(fSlopel2f[ssdindex*NUM_STRIP+strip]*SSD_L2F_E[ssdindex][strip]+fInterceptl2f[ssdindex*NUM_STRIP+strip]);
	        layerevent->fL2FTime[ssdindex].push_back(SSD_L2F_T[ssdindex][strip]);
	      }
	      if (ssdindex==1) {
	        if (strip!=0 && strip!=1) { // 对于 SSD2_L2F, 扔掉 CH[0] 与 CH[1]
	          SSDL2FMulti[ssdindex]++;
	          layerevent->fL2FNumStrip[ssdindex].push_back(strip);
	          layerevent->fL2FEMeV[ssdindex].push_back(fSlopel2f[ssdindex*NUM_STRIP+strip]*SSD_L2F_E[ssdindex][strip]+fInterceptl2f[ssdindex*NUM_STRIP+strip]);
	          layerevent->fL2FTime[ssdindex].push_back(SSD_L2F_T[ssdindex][strip]);
	        }
	      }
	      if (ssdindex==3) {
	        if (strip!=13 && strip!=14) { // 对于 SSD4_L2F, 扔掉 CH[13] 与 CH[14]
	          SSDL2FMulti[ssdindex]++;
	          layerevent->fL2FNumStrip[ssdindex].push_back(strip);
	          layerevent->fL2FEMeV[ssdindex].push_back(fSlopel2f[ssdindex*NUM_STRIP+strip]*SSD_L2F_E[ssdindex][strip]+fInterceptl2f[ssdindex*NUM_STRIP+strip]);
	          layerevent->fL2FTime[ssdindex].push_back(SSD_L2F_T[ssdindex][strip]);
	        }
	      }
	    }
	    // for L2B
	    if (SSD_L2B_E[ssdindex][strip]>fSiEChcutl2b[ssdindex*NUM_STRIP+strip]) {
	      SSDL2BMulti[ssdindex]++;
	      layerevent->fL2BNumStrip[ssdindex].push_back(strip);
	      layerevent->fL2BEMeV[ssdindex].push_back(fSlopel2b[ssdindex*NUM_STRIP+strip]*SSD_L2B_E[ssdindex][strip]+fInterceptl2b[ssdindex*NUM_STRIP+strip]);
	    }
	  }
	  // for L3A
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
	    if (SSD_L3A_E[ssdindex][csiindex]>fCsIEChcutl3a[ssdindex*NUM_CSI+csiindex]) {
				if ((ssdindex!=3) || (csiindex!=4 && csiindex!=5)) { // SSD4_CsI[4], SSD4_CsI[5] 数据异常, 扔掉
					SSDCsIMulti[ssdindex]++;
		      layerevent->fCsINum[ssdindex].push_back(csiindex);
		      layerevent->fCsIECh[ssdindex].push_back(SSD_L3A_E[ssdindex][csiindex]);
				}
	    }
	  }
	  layerevent->fL1SMulti[ssdindex] = SSDL1SMulti[ssdindex];
	  layerevent->fL2FMulti[ssdindex] = SSDL2FMulti[ssdindex];
	  layerevent->fL2BMulti[ssdindex] = SSDL2BMulti[ssdindex];
	  layerevent->fCsIMulti[ssdindex] = SSDCsIMulti[ssdindex];
	}
}


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEUnCaliDataToPhysicsEvent_SSD::FillTrackEvent_L2L3(CSHINETrackEvent2* trackevent, CSHINELayerEvent2* layerevent)
{
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
	{
    if ((layerevent->fCsIMulti[ssdindex]>0 && layerevent->fCsIMulti[ssdindex]<=MULTICUT_L3A) &&
        (layerevent->fL2BMulti[ssdindex]>0 && layerevent->fL2BMulti[ssdindex]<=MULTICUT_L2B) &&
        (layerevent->fL2FMulti[ssdindex]>0 && layerevent->fL2FMulti[ssdindex]<=MULTICUT_L2F) &&
        (layerevent->fL1SMulti[ssdindex]>0 && layerevent->fL1SMulti[ssdindex]<=MULTICUT_L1S)) // 给定初步的 multi cut
    {
      for (Int_t csimulti=0; csimulti<layerevent->fCsIMulti[ssdindex]; csimulti++) {
        for (Int_t l2bmulti=0; l2bmulti<layerevent->fL2BMulti[ssdindex]; l2bmulti++) {
          for (Int_t l2fmulti=0; l2fmulti<layerevent->fL2FMulti[ssdindex]; l2fmulti++) {
            for (Int_t l1smulti=0; l1smulti<layerevent->fL1SMulti[ssdindex]; l1smulti++) { // 逐层循环,遍历所有可能的组合

              if (trackresconstruct.IsGeoConstraint_L3A_L2B(layerevent->fCsINum[ssdindex][csimulti], layerevent->fL2BNumStrip[ssdindex][l2bmulti]) &&
                  trackresconstruct.IsGeoConstraint_L3A_L2F(layerevent->fCsINum[ssdindex][csimulti], layerevent->fL2FNumStrip[ssdindex][l2fmulti]) &&
                  trackresconstruct.IsGeoConstraint_L2B_L1S(layerevent->fL2BNumStrip[ssdindex][l2bmulti], layerevent->fL1SNumStrip[ssdindex][l1smulti]))
              {
								TelNum_BananaCut = ssdindex*NUM_CSI + layerevent->fCsINum[ssdindex][csimulti];

                trackevent->fCsINum[ssdindex].push_back(layerevent->fCsINum[ssdindex][csimulti]);
                trackevent->fCsIECh[ssdindex].push_back(layerevent->fCsIECh[ssdindex][csimulti]);
                trackevent->fL2BNumStrip[ssdindex].push_back(layerevent->fL2BNumStrip[ssdindex][l2bmulti]);
                trackevent->fL2BEMeV[ssdindex].push_back(layerevent->fL2BEMeV[ssdindex][l2bmulti]);
                trackevent->fL2FNumStrip[ssdindex].push_back(layerevent->fL2FNumStrip[ssdindex][l2fmulti]);
                trackevent->fL2FEMeV[ssdindex].push_back(layerevent->fL2FEMeV[ssdindex][l2fmulti]);
                trackevent->fL1SNumStrip[ssdindex].push_back(layerevent->fL1SNumStrip[ssdindex][l1smulti]);
                trackevent->fL1SEMeV[ssdindex].push_back(layerevent->fL1SEMeV[ssdindex][l1smulti]);
                trackevent->fL2FTime[ssdindex].push_back(layerevent->fL2FTime[ssdindex][l2fmulti]);
								trackevent->fCutTelNum[ssdindex].push_back(TelNum_BananaCut);
              }
            }
          }
        }
      }
    }
  }
}


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEUnCaliDataToPhysicsEvent_SSD::FillTrackEvent_L1L2(CSHINETrackEvent2* trackevent, CSHINELayerEvent2* layerevent)
{
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
	{
    if ((layerevent->fCsIMulti[ssdindex]==0) &&
        (layerevent->fL2BMulti[ssdindex]>0 && layerevent->fL2BMulti[ssdindex]<=MULTICUT_L2B) &&
        (layerevent->fL2FMulti[ssdindex]>0 && layerevent->fL2FMulti[ssdindex]<=MULTICUT_L2F) &&
        (layerevent->fL1SMulti[ssdindex]>0 && layerevent->fL1SMulti[ssdindex]<=MULTICUT_L1S)) // 给定初步的 multi cut
    {
      for (Int_t l2bmulti=0; l2bmulti<layerevent->fL2BMulti[ssdindex]; l2bmulti++) {
        for (Int_t l2fmulti=0; l2fmulti<layerevent->fL2FMulti[ssdindex]; l2fmulti++) {
          for (Int_t l1smulti=0; l1smulti<layerevent->fL1SMulti[ssdindex]; l1smulti++) { // 逐层循环,遍历所有可能的组合

            if (trackresconstruct.IsGeoConstraint_L2B_L1S(layerevent->fL2BNumStrip[ssdindex][l2bmulti], layerevent->fL1SNumStrip[ssdindex][l1smulti]))
            {
              trackevent->fL2BNumStrip[ssdindex].push_back(layerevent->fL2BNumStrip[ssdindex][l2bmulti]);
              trackevent->fL2BEMeV[ssdindex].push_back(layerevent->fL2BEMeV[ssdindex][l2bmulti]);
              trackevent->fL2FNumStrip[ssdindex].push_back(layerevent->fL2FNumStrip[ssdindex][l2fmulti]);
              trackevent->fL2FEMeV[ssdindex].push_back(layerevent->fL2FEMeV[ssdindex][l2fmulti]);
              trackevent->fL1SNumStrip[ssdindex].push_back(layerevent->fL1SNumStrip[ssdindex][l1smulti]);
              trackevent->fL1SEMeV[ssdindex].push_back(layerevent->fL1SEMeV[ssdindex][l1smulti]);

              trackevent->fL2FTime[ssdindex].push_back(layerevent->fL2FTime[ssdindex][l2fmulti]);

							fZone_index_L1L2 = particleidentification.GetZoneOfPixel(layerevent->fL2BNumStrip[ssdindex][l2bmulti], layerevent->fL2FNumStrip[ssdindex][l2fmulti], ZONE_LENGTH_L1L2);
							trackevent->fL1SEMeV_Corrected[ssdindex].push_back(layerevent->fL1SEMeV[ssdindex][l1smulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index_L1L2]));
            }
          }
        }
      }
    }
  }
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// PPAC 信号刻度
CSHINEUnCaliDataToPhysicsEvent_PPAC::CSHINEUnCaliDataToPhysicsEvent_PPAC()
{}

CSHINEUnCaliDataToPhysicsEvent_PPAC::CSHINEUnCaliDataToPhysicsEvent_PPAC(TChain* InputChain)
{
	fChain_MapRoot = InputChain;
	if (!fChain_MapRoot) {
		cout<<"Error: MapRoot Input not exist!"<<endl;
		return;
	}
	// 初始化
	Init_MapRootTree_PPAC();
}

CSHINEUnCaliDataToPhysicsEvent_PPAC::~CSHINEUnCaliDataToPhysicsEvent_PPAC()
{}
//______________________________________________________________________________

//______________________________________________________________________________
void CSHINEUnCaliDataToPhysicsEvent_PPAC::Init_MapRootTree_PPAC()
{
	// firstly, close all the branches
	//fChain_MapRoot->SetBranchStatus("*", false);
	// then, open selected branches
	fChain_MapRoot->SetBranchStatus(b_PPAC1_T,  true);
	fChain_MapRoot->SetBranchStatus(b_PPAC1_X1, true);
	fChain_MapRoot->SetBranchStatus(b_PPAC1_X2, true);
	fChain_MapRoot->SetBranchStatus(b_PPAC1_Y1, true);
	fChain_MapRoot->SetBranchStatus(b_PPAC1_Y2, true);
	fChain_MapRoot->SetBranchStatus(b_PPAC1_T_Energy, true);
	fChain_MapRoot->SetBranchStatus(b_PPAC2_T,  true);
	fChain_MapRoot->SetBranchStatus(b_PPAC2_X1, true);
	fChain_MapRoot->SetBranchStatus(b_PPAC2_X2, true);
	fChain_MapRoot->SetBranchStatus(b_PPAC2_Y1, true);
	fChain_MapRoot->SetBranchStatus(b_PPAC2_Y2, true);
	fChain_MapRoot->SetBranchStatus(b_PPAC2_T_Energy, true);
	fChain_MapRoot->SetBranchStatus(b_PPAC3_T,  true);
	fChain_MapRoot->SetBranchStatus(b_PPAC3_X1, true);
	fChain_MapRoot->SetBranchStatus(b_PPAC3_X2, true);
	fChain_MapRoot->SetBranchStatus(b_PPAC3_Y1, true);
	fChain_MapRoot->SetBranchStatus(b_PPAC3_Y2, true);
	fChain_MapRoot->SetBranchStatus(b_PPAC3_T_Energy, true);
	// finally, set branch address
	fChain_MapRoot->SetBranchAddress(b_PPAC1_T,  &PPAC_T [0]);
	fChain_MapRoot->SetBranchAddress(b_PPAC1_X1, &PPAC_X1[0]);
	fChain_MapRoot->SetBranchAddress(b_PPAC1_X2, &PPAC_X2[0]);
	fChain_MapRoot->SetBranchAddress(b_PPAC1_Y1, &PPAC_Y1[0]);
	fChain_MapRoot->SetBranchAddress(b_PPAC1_Y2, &PPAC_Y2[0]);
	fChain_MapRoot->SetBranchAddress(b_PPAC1_T_Energy, &PPAC_TE[0]);
	fChain_MapRoot->SetBranchAddress(b_PPAC2_T,  &PPAC_T [1]);
	fChain_MapRoot->SetBranchAddress(b_PPAC2_X1, &PPAC_X1[1]);
	fChain_MapRoot->SetBranchAddress(b_PPAC2_X2, &PPAC_X2[1]);
	fChain_MapRoot->SetBranchAddress(b_PPAC2_Y1, &PPAC_Y1[1]);
	fChain_MapRoot->SetBranchAddress(b_PPAC2_Y2, &PPAC_Y2[1]);
	fChain_MapRoot->SetBranchAddress(b_PPAC2_T_Energy, &PPAC_TE[1]);
	fChain_MapRoot->SetBranchAddress(b_PPAC3_T,  &PPAC_T [2]);
	fChain_MapRoot->SetBranchAddress(b_PPAC3_X1, &PPAC_X1[2]);
	fChain_MapRoot->SetBranchAddress(b_PPAC3_X2, &PPAC_X2[2]);
	fChain_MapRoot->SetBranchAddress(b_PPAC3_Y1, &PPAC_Y1[2]);
	fChain_MapRoot->SetBranchAddress(b_PPAC3_Y2, &PPAC_Y2[2]);
	fChain_MapRoot->SetBranchAddress(b_PPAC3_T_Energy, &PPAC_TE[2]);
}

//______________________________________________________________________________
void CSHINEUnCaliDataToPhysicsEvent_PPAC::FillPPACEvent(CSHINEPPACEvent* ppacevent)
{
  gMulti = 0;
	//
	for (Int_t ippac=0; ippac<NUM_PPAC; ippac++)
	{
		// 只有当 X面, Y面都至少有一个信号时,该时间为有效事件
		PPAC_X1_GOOG[ippac] = PPAC_X1[ippac]>PPAC_TDC_CUT ? true : false;
    PPAC_X2_GOOG[ippac] = PPAC_X2[ippac]>PPAC_TDC_CUT ? true : false;
		PPAC_Y1_GOOG[ippac] = PPAC_Y1[ippac]>PPAC_TDC_CUT ? true : false;
		PPAC_Y2_GOOG[ippac] = PPAC_Y2[ippac]>PPAC_TDC_CUT ? true : false;

		if ((PPAC_X1_GOOG[ippac]+PPAC_X2_GOOG[ippac]) && (PPAC_Y1_GOOG[ippac]+PPAC_Y2_GOOG[ippac]))
		{
			gMulti++;
			ppacevent->fPPACFiredNum.push_back(ippac);
			ppacevent->fT.push_back(PPAC_T[ippac]);
			ppacevent->fX1.push_back(PPAC_X1[ippac]);
			ppacevent->fX2.push_back(PPAC_X2[ippac]);
			ppacevent->fY1.push_back(PPAC_Y1[ippac]);
			ppacevent->fY2.push_back(PPAC_Y2[ippac]);
			ppacevent->fTE.push_back(PPAC_TE[ippac]);

			PPACModeID[ippac] = (ippac+1)*10000 + PPAC_X1_GOOG[ippac]*1000 + PPAC_X2_GOOG[ippac]*100 + PPAC_Y1_GOOG[ippac]*10 + PPAC_Y2_GOOG[ippac];
			ppacevent->fModeID.push_back(PPACModeID[ippac]);
		}
	}
	ppacevent->fFFMulti = gMulti;
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// Au-Si 面垒望远镜事件
CSHINEUnCaliDataToPhysicsEvent_AuSiTel::CSHINEUnCaliDataToPhysicsEvent_AuSiTel()
{}

CSHINEUnCaliDataToPhysicsEvent_AuSiTel::CSHINEUnCaliDataToPhysicsEvent_AuSiTel(TChain* InputChain)
{
	fChain_MapRoot = InputChain;
	if (!fChain_MapRoot) {
		cout<<"Error: MapRoot Input not exist!"<<endl;
		return;
	}
	// 初始化
	Init_MapRootTree_AuSiTel();
}

CSHINEUnCaliDataToPhysicsEvent_AuSiTel::~CSHINEUnCaliDataToPhysicsEvent_AuSiTel()
{}
//______________________________________________________________________________


//______________________________________________________________________________
void CSHINEUnCaliDataToPhysicsEvent_AuSiTel::Init_MapRootTree_AuSiTel()
{
	// firstly, close all the branches
	// fChain_MapRoot->SetBranchStatus("*", false);
	// then, open selected branches
	fChain_MapRoot->SetBranchStatus(b_AuSi1_L1T, true);
	fChain_MapRoot->SetBranchStatus(b_AuSi1_L1E, true);
	fChain_MapRoot->SetBranchStatus(b_AuSi1_L2E, true);
	fChain_MapRoot->SetBranchStatus(b_AuSi2_L1T, true);
	fChain_MapRoot->SetBranchStatus(b_AuSi2_L1E, true);
	fChain_MapRoot->SetBranchStatus(b_AuSi2_L2E, true);
	// finally, set branch address
	fChain_MapRoot->SetBranchAddress(b_AuSi1_L1T, &AuSi_L1T[0]);
  fChain_MapRoot->SetBranchAddress(b_AuSi1_L1E, &AuSi_L1E[0]);
	fChain_MapRoot->SetBranchAddress(b_AuSi1_L2E, &AuSi_L2E[0]);
	fChain_MapRoot->SetBranchAddress(b_AuSi2_L1T, &AuSi_L1T[1]);
  fChain_MapRoot->SetBranchAddress(b_AuSi2_L1E, &AuSi_L1E[1]);
	fChain_MapRoot->SetBranchAddress(b_AuSi2_L2E, &AuSi_L2E[1]);
}
//______________________________________________________________________________

//______________________________________________________________________________
void CSHINEUnCaliDataToPhysicsEvent_AuSiTel::FillAuSiTelEvent(CSHINEAuSiTelEvent* AuSievent)
{
	gMulti = 0;
	//
	for (Int_t itel=0; itel<NUM_AUSI; itel++)
	{
    if (AuSi_L1T[itel]>AUSI_TDC_CUT)
		{
			gMulti++;
			AuSievent->fTelNum.push_back(itel);
			AuSievent->fL1Time.push_back(AuSi_L1T[itel]);
			AuSievent->fL1ESi.push_back(AuSi_L1E[itel]);
			AuSievent->fL2ECsI.push_back(AuSi_L2E[itel]);
		}
	}
  AuSievent->fGlobalMulti = gMulti;
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// 高频信号
CSHINEUnCaliDataToPhysicsEvent_RF::CSHINEUnCaliDataToPhysicsEvent_RF()
{}

CSHINEUnCaliDataToPhysicsEvent_RF::CSHINEUnCaliDataToPhysicsEvent_RF(TChain* InputChain)
{
	fChain_MapRoot = InputChain;
	if (!fChain_MapRoot) {
		cout<<"Error: MapRoot Input not exist!"<<endl;
		return;
	}
	// 初始化
	Init_MapRootTree_RF();
}

CSHINEUnCaliDataToPhysicsEvent_RF::~CSHINEUnCaliDataToPhysicsEvent_RF()
{}
//______________________________________________________________________________

//______________________________________________________________________________
void CSHINEUnCaliDataToPhysicsEvent_RF::Init_MapRootTree_RF()
{
	// firstly, close all the branches
	// fChain_MapRoot->SetBranchStatus("*", false);
	// then, open selected branches
	fChain_MapRoot->SetBranchStatus(b_RF1, true);
	fChain_MapRoot->SetBranchStatus(b_RF2, true);
	// finally, set branch address
	fChain_MapRoot->SetBranchAddress(b_RF1, &RF1);
  fChain_MapRoot->SetBranchAddress(b_RF2, &RF2);
}
//______________________________________________________________________________

//______________________________________________________________________________
void CSHINEUnCaliDataToPhysicsEvent_RF::FillRFSignal(CSHINERFSignal* rfsignal)
{
  rfsignal->fRF1 = RF1;
	rfsignal->fRF2 = RF2;
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
