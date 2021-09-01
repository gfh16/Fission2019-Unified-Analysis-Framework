////////////////////////////////////////////////////////////////////////////////
//  此程序用于 L2L3 中 globalmulti = 3 的径迹重建
//
// gfh, 2021-07-26
////////////////////////////////////////////////////////////////////////////////

#include "../include/DecodefGlobalMulti6.h"

#include <iostream>
#include <TTree.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TMath.h>


//______________________________________________________________________________
// 解码 fGlobalMulit = 3的事件
DecodefGlobalMulti6::DecodefGlobalMulti6()
{
	fFirstRun = 150;
  fLastRun  = 160;
}
//
DecodefGlobalMulti6::DecodefGlobalMulti6(Int_t firstrun, Int_t lastrun)
{
  fFirstRun = firstrun;
  fLastRun  = lastrun;

  trackreconstruct = new CSHINETrackReconstruction(fFirstRun, fLastRun);

  std::string pathBananaCut(Form("%sStraighteningData/L2L3DEECuts.root",PATHROOTFILESFOLDER));
  TFile* myfile = TFile::Open(pathBananaCut.c_str());
  // 读取 bananacut
  for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
    TIter next(myfile->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*) next())) {
      TCutG* cut = (TCutG*) key->ReadObj();
      std::string cutname(cut->GetName());

      if (strstr(cutname.c_str(), Form("_Tel%02d_",numtel)) != NULL) {
        bananacut[numtel].push_back(cut);
      //  cout<<cut->GetName()<<endl;
      }
    }
  }
}

DecodefGlobalMulti6::~DecodefGlobalMulti6()
{}
//______________________________________________________________________________


//______________________________________________________________________________
void DecodefGlobalMulti6::GlobalMulti6_Checks()
{
  gROOT->cd();

  Int_t gmulti = 6;
  const char* gmultitag = "fGlobalMulti6";
  trackreconstruct->L2L3_CheckParallelDraw(gmultitag, gmulti);
  trackreconstruct->L2L3_CheckL2L3DEEPlot (gmultitag, gmulti);
  trackreconstruct->L2L3_CheckL1L2DEEPlot (gmultitag, gmulti);
}
//______________________________________________________________________________



//______________________________________________________________________________
void DecodefGlobalMulti6::GlobalMulti6_Decode_2222()
{
	const Int_t globalmulti = 6;
  const char* mode = "2222";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;
  Double_t EL2F_Sharing;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Int_t charge, mass;
  std::string cutname[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];
  Int_t candimulti_corrected[NUM_SSD];
  Int_t candimulti_sharing[NUM_SSD];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 6, -0.5, 5.5);
	  }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

		  h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	  }
  }

  // trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
	  trackreconstruct->fChainTrackTree->GetEntry(ientry);
	  timeper.PrintPercentageAndRemainingTime(ientry, nentries);

  	// fill data
	  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		  L2L3_SSDNum [ssdindex].clear();
		  L2L3_CsINum [ssdindex].clear();
		  L2L3_CSIECh [ssdindex].clear();
		  L2L3_L2BEMeV[ssdindex].clear();
		  L2L3_L2FEMeV[ssdindex].clear();
		  L2L3_L1SEMeV[ssdindex].clear();
		  L2L3_CsINum[ssdindex].clear();
		  L2L3_L2BStripNum[ssdindex].clear();
		  L2L3_L2FStripNum[ssdindex].clear();
		  L2L3_L1SStripNum[ssdindex].clear();
		  BananaCut_TelNum[ssdindex].clear();

	  	// 初始化
		  candimulti_sum[ssdindex] = 0;
		  candimulti_corrected[ssdindex] = 0;
		  candimulti_sharing[ssdindex] = 0;
		  for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

		  if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
			  for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
				  if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
				    L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
				  	L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
					  L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
					  L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
					  L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
					  L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
					  L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
					  L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
					  BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
				  }
			  }
	  	}
		  // 填充数据
	  	if (L2L3_CsINum[ssdindex].size() == globalmulti) {
		  	mode_exp = trackreconstruct->L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
				  	h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

				  	// 填充 mode statistic
				  	bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
				  	h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  //
					  if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])  &&
							  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
							  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]))
					  {
						  candimulti_sum[ssdindex]++;
						  candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
					  }
				  }
				  //__________________________________________________________________
			  	//        以下部分为当前 L2L3-globalomulti6-2222 模式的径迹重建策略！！！
			  	//       ---------------------------------------------------------
					// 自考虑  ---01---23---45----
					if ((L2L3_CsINum[ssdindex][0]==L2L3_CsINum[ssdindex][1]) && (L2L3_CsINum[ssdindex][2]==L2L3_CsINum[ssdindex][3]) && (L2L3_CsINum[ssdindex][4]==L2L3_CsINum[ssdindex][5]))
					{
						if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]) &&
							  trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][0]],cutname[0],L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]))
						{
	            candimulti_corrected[ssdindex]++;
							//
						  //	cout<<"ientry="<<ientry<<setw(15)<<"CsINum[0]="<<L2L3_CsINum [ssdindex][0]<<setw(15)<<"EL2B[0]="<<L2L3_L2BEMeV[ssdindex][0]<<setw(15)<<"EL2F[0]="<<L2L3_L2FEMeV[ssdindex][0]<<endl;
							// L2L3_DEE 直接填充
							h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_CSIECh[ssdindex][0],L2L3_L2FEMeV[ssdindex][0]);
							// 如果满足 EL1S_EL2F, 则直接填充
							if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][0], L2L3_L2FEMeV[ssdindex][0])) {
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L2L3_L1SEMeV[ssdindex][0]);
							}
							else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								trackreconstruct->GetZAFromBananaCutName(cutname[0],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][0]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][0],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][0]].Fill(L2L3_L2FEMeV[ssdindex][0],L1S_ELoss);
									}
								}
							}
						}
						else if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]) &&
							       trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][1]],cutname[1],L2L3_CSIECh[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]))
						{
	            candimulti_corrected[ssdindex]++;
							//
							// cout<<"ientry="<<ientry<<setw(15)<<"CsINum[1]="<<L2L3_CsINum [ssdindex][1]<<setw(15)<<"EL2B[1]="<<L2L3_L2BEMeV[ssdindex][1]<<setw(15)<<"EL2F[1]="<<L2L3_L2FEMeV[ssdindex][1]<<endl;
							// L2L3_DEE 直接填充
							h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_CSIECh[ssdindex][1],L2L3_L2FEMeV[ssdindex][1]);
							// 如果满足 EL1S_EL2F, 则直接填充
							if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][1], L2L3_L2FEMeV[ssdindex][1])) {
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_L2FEMeV[ssdindex][1],L2L3_L1SEMeV[ssdindex][1]);
							}
							else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								trackreconstruct->GetZAFromBananaCutName(cutname[1],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][1]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][1],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][1]].Fill(L2L3_L2FEMeV[ssdindex][1],L1S_ELoss);
									}
								}
							}
						}
						// 在 2,3 中挑选至多一条径迹
						if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][2],L2L3_L2FEMeV[ssdindex][2]) &&
							  trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][2]],cutname[2],L2L3_CSIECh[ssdindex][2],L2L3_L2FEMeV[ssdindex][2]))
						{
	            candimulti_corrected[ssdindex]++;
							//
							// cout<<"ientry="<<ientry<<setw(15)<<"CsINum[2]="<<L2L3_CsINum [ssdindex][2]<<setw(15)<<"EL2B[2]="<<L2L3_L2BEMeV[ssdindex][2]<<setw(15)<<"EL2F[2]="<<L2L3_L2FEMeV[ssdindex][2]<<endl;
							// L2L3_DEE 直接填充
							h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][2]].Fill(L2L3_CSIECh[ssdindex][2],L2L3_L2FEMeV[ssdindex][2]);
							// 如果满足 EL1S_EL2F, 则直接填充
							if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][2], L2L3_L2FEMeV[ssdindex][2])) {
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][2]].Fill(L2L3_L2FEMeV[ssdindex][2],L2L3_L1SEMeV[ssdindex][2]);
							}
							else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								trackreconstruct->GetZAFromBananaCutName(cutname[2],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][2]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][2],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][2]].Fill(L2L3_L2FEMeV[ssdindex][2],L1S_ELoss);
									}
								}
							}
						}
						else if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][3],L2L3_L2FEMeV[ssdindex][3]) &&
							       trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][3]],cutname[3],L2L3_CSIECh[ssdindex][3],L2L3_L2FEMeV[ssdindex][3]))
						{
	            candimulti_corrected[ssdindex]++;
							//
							// cout<<"ientry="<<ientry<<setw(15)<<"CsINum[3]="<<L2L3_CsINum [ssdindex][3]<<setw(15)<<"EL2B[3]="<<L2L3_L2BEMeV[ssdindex][3]<<setw(15)<<"EL2F[3]="<<L2L3_L2FEMeV[ssdindex][3]<<endl;
							// L2L3_DEE 直接填充
							h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][3]].Fill(L2L3_CSIECh[ssdindex][3],L2L3_L2FEMeV[ssdindex][3]);
							// 如果满足 EL1S_EL2F, 则直接填充
							if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][3], L2L3_L2FEMeV[ssdindex][3])) {
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][3]].Fill(L2L3_L2FEMeV[ssdindex][3],L2L3_L1SEMeV[ssdindex][3]);
							}
							else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								trackreconstruct->GetZAFromBananaCutName(cutname[3],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][3]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][3],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][3]].Fill(L2L3_L2FEMeV[ssdindex][3],L1S_ELoss);
									}
								}
							}
						}
						// 在 4,5 中挑选至多一条径迹
						if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][4],L2L3_L2FEMeV[ssdindex][4]) &&
							  trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][4]],cutname[4],L2L3_CSIECh[ssdindex][4],L2L3_L2FEMeV[ssdindex][4]))
						{
							candimulti_corrected[ssdindex]++;
							//
							// cout<<"ientry="<<ientry<<setw(15)<<"CsINum[4]="<<L2L3_CsINum [ssdindex][4]<<setw(15)<<"EL2B[4]="<<L2L3_L2BEMeV[ssdindex][4]<<setw(15)<<"EL2F[4]="<<L2L3_L2FEMeV[ssdindex][4]<<endl;
							// L2L3_DEE 直接填充
							h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][4]].Fill(L2L3_CSIECh[ssdindex][4],L2L3_L2FEMeV[ssdindex][4]);
							// 如果满足 EL1S_EL2F, 则直接填充
							if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][4], L2L3_L2FEMeV[ssdindex][4])) {
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][4]].Fill(L2L3_L2FEMeV[ssdindex][4],L2L3_L1SEMeV[ssdindex][4]);
							}
							else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								trackreconstruct->GetZAFromBananaCutName(cutname[4],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][4]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][4],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][4]].Fill(L2L3_L2FEMeV[ssdindex][4],L1S_ELoss);
									}
								}
							}
						}
						else if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][5],L2L3_L2FEMeV[ssdindex][5]) &&
							       trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][5]],cutname[5],L2L3_CSIECh[ssdindex][5],L2L3_L2FEMeV[ssdindex][5]))
						{
							candimulti_corrected[ssdindex]++;
							//
							// cout<<"ientry="<<ientry<<setw(15)<<"CsINum[5]="<<L2L3_CsINum [ssdindex][5]<<setw(15)<<"EL2B[5]="<<L2L3_L2BEMeV[ssdindex][5]<<setw(15)<<"EL2F[5]="<<L2L3_L2FEMeV[ssdindex][5]<<endl;
							// L2L3_DEE 直接填充
							h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][5]].Fill(L2L3_CSIECh[ssdindex][5],L2L3_L2FEMeV[ssdindex][5]);
							// 如果满足 EL1S_EL2F, 则直接填充
							if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][5], L2L3_L2FEMeV[ssdindex][5])) {
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][5]].Fill(L2L3_L2FEMeV[ssdindex][5],L2L3_L1SEMeV[ssdindex][5]);
							}
							else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								trackreconstruct->GetZAFromBananaCutName(cutname[5],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][5]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][5],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][5]].Fill(L2L3_L2FEMeV[ssdindex][5],L1S_ELoss);
									}
								}
							}
						}
					}
      		//____________________________________________________________________
				  // 填充每块 CsI 的 candidate multi
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
					  h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
				  }
			  	//
					if (candimulti_corrected[ssdindex] == 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						  h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
						  h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
					  }
				  }
					//
				  if (candimulti_corrected[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  //
				  if (candimulti_sharing[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  	h1_SubmodeSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  // 填充每一个 SSD 的 candidate multi
				  h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
			  }
		  }
	  }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
		  else {
			  entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
			  entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
		  }
	  }
	  lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  x1 = 0.4 * globalmulti;
	  y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
	  latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
	  latex_ratio[ssdindex]->SetTextSize(0.05);
	  latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  //
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeSharing[ssdindex][csiindex],"sharing","l");
		  //
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
	  }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  cans.cd(1);
		  h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
		  if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
			  for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
			  	bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
				  bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
			  }
		  }

		  cans.cd(2);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(3);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(4);
		  h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
		  h1_modestat[ssdindex][csiindex]->SetLineWidth(2);
		  h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
		  h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
		  h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  h1_SubmodeSharing[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineColor(kGreen);
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  legend_SubMode[ssdindex][csiindex]->Draw("SAME");

		  cans.cd(5);
		  h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
		  cans.cd(6);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(7);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(8);
		  // candimulti_sum
		  h1_candimulti_sum[ssdindex]->Draw();
		  h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
		  h1_candimulti_sum[ssdindex]->SetLineWidth(2);
		  h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(106);
		  h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
		  // candimulti_merged
		  h1_candimulti_merged[ssdindex]->Draw("SAME");
		  h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
		  h1_candimulti_merged[ssdindex]->SetLineWidth(2);
		  // candimulti
		  h1_candimulti[ssdindex][csiindex]->Draw("SAME");
		  h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
		  h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
		  // latex_ratio
		  latex_ratio[ssdindex]->Draw();
		  //
		  legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

		  gPad->Modified();
		  gPad->Update();

		  cans.Print(pathPDFOut.c_str());
	  }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void DecodefGlobalMulti6::GlobalMulti6_Decode_1211()
{
	const Int_t globalmulti = 6;
  const char* mode = "1211";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;
  Double_t EL2F_Sharing;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Int_t charge, mass;
  std::string cutname[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];
  Int_t candimulti_corrected[NUM_SSD];
  Int_t candimulti_sharing[NUM_SSD];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 6, -0.5, 5.5);
	  }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

		  h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	  }
  }

  trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
	  trackreconstruct->fChainTrackTree->GetEntry(ientry);
	  timeper.PrintPercentageAndRemainingTime(ientry, nentries);

  	// fill data
	  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		  L2L3_SSDNum [ssdindex].clear();
		  L2L3_CsINum [ssdindex].clear();
		  L2L3_CSIECh [ssdindex].clear();
		  L2L3_L2BEMeV[ssdindex].clear();
		  L2L3_L2FEMeV[ssdindex].clear();
		  L2L3_L1SEMeV[ssdindex].clear();
		  L2L3_CsINum[ssdindex].clear();
		  L2L3_L2BStripNum[ssdindex].clear();
		  L2L3_L2FStripNum[ssdindex].clear();
		  L2L3_L1SStripNum[ssdindex].clear();
		  BananaCut_TelNum[ssdindex].clear();

	  	// 初始化
		  candimulti_sum[ssdindex] = 0;
		  candimulti_corrected[ssdindex] = 0;
		  candimulti_sharing[ssdindex] = 0;
		  for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

		  if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
			  for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
				  if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
				    L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
				  	L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
					  L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
					  L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
					  L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
					  L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
					  L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
					  L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
					  BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
				  }
			  }
	  	}
		  // 填充数据
	  	if (L2L3_CsINum[ssdindex].size() == globalmulti) {
		  	mode_exp = trackreconstruct->L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
				  	h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

				  	// 填充 mode statistic
				  	bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
				  	h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  //
					  if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])  &&
							  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
							  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]))
					  {
						  candimulti_sum[ssdindex]++;
						  candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
					  }
				  }
				  //__________________________________________________________________
			  	//        以下部分为当前 L2L3-globalomulti6-1211 模式的径迹重建策略！！！
			  	//       ---------------------------------------------------------
					// ---0--12345---
					if (L2L3_CsINum[ssdindex][0]!=L2L3_CsINum[ssdindex][1])
					{
						//
						for (Int_t itrack=0; itrack<1; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=1; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
					// ---01--2345---
					else if (L2L3_CsINum[ssdindex][1]!=L2L3_CsINum[ssdindex][2])
					{
						//
						for (Int_t itrack=0; itrack<2; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							// cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---01--2345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=2; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---01--2345---"<<endl;
							  break;
							}
						}
					}
					/// ---012--345---
					else if (L2L3_CsINum[ssdindex][2]!=L2L3_CsINum[ssdindex][3])
				  {
						//
						for (Int_t itrack=0; itrack<3; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---012--345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=3; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---012--345---"<<endl;
							  break;
							}
						}
					}
					// ---0123--45---
					else if (L2L3_CsINum[ssdindex][3]!=L2L3_CsINum[ssdindex][4])
				  {
						//
						for (Int_t itrack=0; itrack<4; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0123--45---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=4; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0123--45---"<<endl;
							  break;
							}
						}
					}
					// ---01234--5---
					else if (L2L3_CsINum[ssdindex][4]!=L2L3_CsINum[ssdindex][5])
				  {
						//
						for (Int_t itrack=0; itrack<5; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---01234--5---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=5; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---01234--5---"<<endl;
							  break;
							}
						}
					}
      		//____________________________________________________________________
				  // 填充每块 CsI 的 candidate multi
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
					  h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
				  }
			  	//
				  if (candimulti_corrected[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  //
					if (candimulti_corrected[ssdindex]==0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
            }
					}
					//
				  if (candimulti_sharing[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  	h1_SubmodeSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  // 填充每一个 SSD 的 candidate multi
				  h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
			  }
		  }
	  }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
		  else {
			  entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
			  entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
		  }
	  }
	  lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  x1 = 0.4 * globalmulti;
	  y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
	  latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
	  latex_ratio[ssdindex]->SetTextSize(0.05);
	  latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  //
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeSharing[ssdindex][csiindex],"sharing","l");
		  //
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
	  }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  cans.cd(1);
		  h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
		  if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
			  for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
			  	bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
				  bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
			  }
		  }

		  cans.cd(2);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(3);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(4);
		  h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
		  h1_modestat[ssdindex][csiindex]->SetLineWidth(2);
		  h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
		  h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
		  h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  h1_SubmodeSharing[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineColor(kGreen);
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  legend_SubMode[ssdindex][csiindex]->Draw("SAME");

		  cans.cd(5);
		  h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
		  cans.cd(6);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(7);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(8);
		  // candimulti_sum
		  h1_candimulti_sum[ssdindex]->Draw();
		  h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
		  h1_candimulti_sum[ssdindex]->SetLineWidth(2);
		  h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(106);
		  h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
		  // candimulti_merged
		  h1_candimulti_merged[ssdindex]->Draw("SAME");
		  h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
		  h1_candimulti_merged[ssdindex]->SetLineWidth(2);
		  // candimulti
		  h1_candimulti[ssdindex][csiindex]->Draw("SAME");
		  h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
		  h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
		  // latex_ratio
		  latex_ratio[ssdindex]->Draw();
		  //
		  legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

		  gPad->Modified();
		  gPad->Update();

		  cans.Print(pathPDFOut.c_str());
	  }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void DecodefGlobalMulti6::GlobalMulti6_Decode_1121()
{
	const Int_t globalmulti = 6;
  const char* mode = "1121";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;
  Double_t EL2F_Sharing;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Int_t charge, mass;
  std::string cutname[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];
  Int_t candimulti_corrected[NUM_SSD];
  Int_t candimulti_sharing[NUM_SSD];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 6, -0.5, 5.5);
	  }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

		  h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	  }
  }

  trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
	  trackreconstruct->fChainTrackTree->GetEntry(ientry);
	  timeper.PrintPercentageAndRemainingTime(ientry, nentries);

  	// fill data
	  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		  L2L3_SSDNum [ssdindex].clear();
		  L2L3_CsINum [ssdindex].clear();
		  L2L3_CSIECh [ssdindex].clear();
		  L2L3_L2BEMeV[ssdindex].clear();
		  L2L3_L2FEMeV[ssdindex].clear();
		  L2L3_L1SEMeV[ssdindex].clear();
		  L2L3_CsINum[ssdindex].clear();
		  L2L3_L2BStripNum[ssdindex].clear();
		  L2L3_L2FStripNum[ssdindex].clear();
		  L2L3_L1SStripNum[ssdindex].clear();
		  BananaCut_TelNum[ssdindex].clear();

	  	// 初始化
		  candimulti_sum[ssdindex] = 0;
		  candimulti_corrected[ssdindex] = 0;
		  candimulti_sharing[ssdindex] = 0;
		  for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

		  if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
			  for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
				  if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
				    L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
				  	L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
					  L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
					  L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
					  L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
					  L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
					  L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
					  L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
					  BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
				  }
			  }
	  	}
		  // 填充数据
	  	if (L2L3_CsINum[ssdindex].size() == globalmulti) {
		  	mode_exp = trackreconstruct->L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
				  	h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

				  	// 填充 mode statistic
				  	bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
				  	h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  //
					  if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])  &&
							  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
							  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]))
					  {
						  candimulti_sum[ssdindex]++;
						  candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
					  }
				  }
					//__________________________________________________________________
			  	//        以下部分为当前 L2L3-globalomulti6-1121 模式的径迹重建策略！！！
			  	//       ---------------------------------------------------------
					// ---0--12345---
					if (L2L3_CsINum[ssdindex][0]!=L2L3_CsINum[ssdindex][1])
					{
						//
						for (Int_t itrack=0; itrack<1; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=1; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
					// ---01--2345---
					else if (L2L3_CsINum[ssdindex][1]!=L2L3_CsINum[ssdindex][2])
					{
						//
						for (Int_t itrack=0; itrack<2; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=2; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
					/// ---012--345---
					else if (L2L3_CsINum[ssdindex][2]!=L2L3_CsINum[ssdindex][3])
				  {
						//
						for (Int_t itrack=0; itrack<3; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=3; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
					// ---0123--45---
					else if (L2L3_CsINum[ssdindex][3]!=L2L3_CsINum[ssdindex][4])
				  {
						//
						for (Int_t itrack=0; itrack<4; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=4; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
					// ---01234--5---
					else if (L2L3_CsINum[ssdindex][4]!=L2L3_CsINum[ssdindex][5])
				  {
						//
						for (Int_t itrack=0; itrack<5; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=5; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
      		//____________________________________________________________________
				  // 填充每块 CsI 的 candidate multi
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
					  h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
				  }
			  	//
				  if (candimulti_corrected[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
					//
					if (candimulti_corrected[ssdindex]==0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
            }
					}
				  //
				  if (candimulti_sharing[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  	h1_SubmodeSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  // 填充每一个 SSD 的 candidate multi
				  h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
			  }
		  }
	  }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
		  else {
			  entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
			  entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
		  }
	  }
	  lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  x1 = 0.4 * globalmulti;
	  y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
	  latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
	  latex_ratio[ssdindex]->SetTextSize(0.05);
	  latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  //
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeSharing[ssdindex][csiindex],"sharing","l");
		  //
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
	  }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  cans.cd(1);
		  h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
		  if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
			  for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
			  	bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
				  bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
			  }
		  }

		  cans.cd(2);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(3);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(4);
		  h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
		  h1_modestat[ssdindex][csiindex]->SetLineWidth(2);
		  h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
		  h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
		  h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  h1_SubmodeSharing[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineColor(kGreen);
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  legend_SubMode[ssdindex][csiindex]->Draw("SAME");

		  cans.cd(5);
		  h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
		  cans.cd(6);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(7);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(8);
		  // candimulti_sum
		  h1_candimulti_sum[ssdindex]->Draw();
		  h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
		  h1_candimulti_sum[ssdindex]->SetLineWidth(2);
		  h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(106);
		  h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
		  // candimulti_merged
		  h1_candimulti_merged[ssdindex]->Draw("SAME");
		  h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
		  h1_candimulti_merged[ssdindex]->SetLineWidth(2);
		  // candimulti
		  h1_candimulti[ssdindex][csiindex]->Draw("SAME");
		  h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
		  h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
		  // latex_ratio
		  latex_ratio[ssdindex]->Draw();
		  //
		  legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

		  gPad->Modified();
		  gPad->Update();

		  cans.Print(pathPDFOut.c_str());
	  }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void DecodefGlobalMulti6::GlobalMulti6_Decode_1112()
{
	const Int_t globalmulti = 6;
  const char* mode = "1112";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;
  Double_t EL2F_Sharing;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Int_t charge, mass;
  std::string cutname[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];
  Int_t candimulti_corrected[NUM_SSD];
  Int_t candimulti_sharing[NUM_SSD];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 6, -0.5, 5.5);
	  }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

		  h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	  }
  }

  trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
	  trackreconstruct->fChainTrackTree->GetEntry(ientry);
	  timeper.PrintPercentageAndRemainingTime(ientry, nentries);

  	// fill data
	  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		  L2L3_SSDNum [ssdindex].clear();
		  L2L3_CsINum [ssdindex].clear();
		  L2L3_CSIECh [ssdindex].clear();
		  L2L3_L2BEMeV[ssdindex].clear();
		  L2L3_L2FEMeV[ssdindex].clear();
		  L2L3_L1SEMeV[ssdindex].clear();
		  L2L3_CsINum[ssdindex].clear();
		  L2L3_L2BStripNum[ssdindex].clear();
		  L2L3_L2FStripNum[ssdindex].clear();
		  L2L3_L1SStripNum[ssdindex].clear();
		  BananaCut_TelNum[ssdindex].clear();

	  	// 初始化
		  candimulti_sum[ssdindex] = 0;
		  candimulti_corrected[ssdindex] = 0;
		  candimulti_sharing[ssdindex] = 0;
		  for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

		  if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
			  for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
				  if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
				    L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
				  	L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
					  L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
					  L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
					  L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
					  L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
					  L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
					  L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
					  BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
				  }
			  }
	  	}
		  // 填充数据
	  	if (L2L3_CsINum[ssdindex].size() == globalmulti) {
		  	mode_exp = trackreconstruct->L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
				  	h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

				  	// 填充 mode statistic
				  	bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
				  	h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  //
					  if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])  &&
							  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
							  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]))
					  {
						  candimulti_sum[ssdindex]++;
						  candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
					  }
				  }
					//__________________________________________________________________
			  	//        以下部分为当前 L2L3-globalomulti6-1112 模式的径迹重建策略！！！
			  	//       ---------------------------------------------------------
					// ---0--12345---
					if (L2L3_CsINum[ssdindex][0]!=L2L3_CsINum[ssdindex][1])
					{
						//
						for (Int_t itrack=0; itrack<1; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=1; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
					// ---01--2345---
					else if (L2L3_CsINum[ssdindex][1]!=L2L3_CsINum[ssdindex][2])
					{
						//
						for (Int_t itrack=0; itrack<2; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=2; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
					/// ---012--345---
					else if (L2L3_CsINum[ssdindex][2]!=L2L3_CsINum[ssdindex][3])
				  {
						//
						for (Int_t itrack=0; itrack<3; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=3; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
					// ---0123--45---
					else if (L2L3_CsINum[ssdindex][3]!=L2L3_CsINum[ssdindex][4])
				  {
						//
						for (Int_t itrack=0; itrack<4; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=4; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
					// ---01234--5---
					else if (L2L3_CsINum[ssdindex][4]!=L2L3_CsINum[ssdindex][5])
				  {
						//
						for (Int_t itrack=0; itrack<5; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=5; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
      		//____________________________________________________________________
				  // 填充每块 CsI 的 candidate multi
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
					  h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
				  }
			  	//
				  if (candimulti_corrected[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
					//
					if (candimulti_corrected[ssdindex]==0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
            }
					}
				  //
				  if (candimulti_sharing[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  	h1_SubmodeSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  // 填充每一个 SSD 的 candidate multi
				  h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
			  }
		  }
	  }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
		  else {
			  entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
			  entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
		  }
	  }
	  lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  x1 = 0.4 * globalmulti;
	  y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
	  latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
	  latex_ratio[ssdindex]->SetTextSize(0.05);
	  latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  //
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeSharing[ssdindex][csiindex],"sharing","l");
		  //
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
	  }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  cans.cd(1);
		  h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
		  if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
			  for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
			  	bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
				  bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
			  }
		  }

		  cans.cd(2);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(3);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(4);
		  h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
		  h1_modestat[ssdindex][csiindex]->SetLineWidth(2);
		  h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
		  h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
		  h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  h1_SubmodeSharing[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineColor(kGreen);
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  legend_SubMode[ssdindex][csiindex]->Draw("SAME");

		  cans.cd(5);
		  h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
		  cans.cd(6);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(7);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(8);
		  // candimulti_sum
		  h1_candimulti_sum[ssdindex]->Draw();
		  h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
		  h1_candimulti_sum[ssdindex]->SetLineWidth(2);
		  h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(106);
		  h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
		  // candimulti_merged
		  h1_candimulti_merged[ssdindex]->Draw("SAME");
		  h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
		  h1_candimulti_merged[ssdindex]->SetLineWidth(2);
		  // candimulti
		  h1_candimulti[ssdindex][csiindex]->Draw("SAME");
		  h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
		  h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
		  // latex_ratio
		  latex_ratio[ssdindex]->Draw();
		  //
		  legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

		  gPad->Modified();
		  gPad->Update();

		  cans.Print(pathPDFOut.c_str());
	  }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void DecodefGlobalMulti6::GlobalMulti6_Decode_1111()
{
	const Int_t globalmulti = 6;
  const char* mode = "1111";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;
  Double_t EL2F_Sharing;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Int_t charge, mass;
  std::string cutname[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];
  Int_t candimulti_corrected[NUM_SSD];
  Int_t candimulti_sharing[NUM_SSD];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 6, -0.5, 5.5);
	  }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

		  h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	  }
  }

  trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
	  trackreconstruct->fChainTrackTree->GetEntry(ientry);
	  timeper.PrintPercentageAndRemainingTime(ientry, nentries);

  	// fill data
	  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		  L2L3_SSDNum [ssdindex].clear();
		  L2L3_CsINum [ssdindex].clear();
		  L2L3_CSIECh [ssdindex].clear();
		  L2L3_L2BEMeV[ssdindex].clear();
		  L2L3_L2FEMeV[ssdindex].clear();
		  L2L3_L1SEMeV[ssdindex].clear();
		  L2L3_CsINum[ssdindex].clear();
		  L2L3_L2BStripNum[ssdindex].clear();
		  L2L3_L2FStripNum[ssdindex].clear();
		  L2L3_L1SStripNum[ssdindex].clear();
		  BananaCut_TelNum[ssdindex].clear();

	  	// 初始化
		  candimulti_sum[ssdindex] = 0;
		  candimulti_corrected[ssdindex] = 0;
		  candimulti_sharing[ssdindex] = 0;
		  for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

		  if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
			  for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
				  if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
				    L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
				  	L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
					  L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
					  L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
					  L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
					  L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
					  L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
					  L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
					  BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
				  }
			  }
	  	}
		  // 填充数据
	  	if (L2L3_CsINum[ssdindex].size() == globalmulti) {
		  	mode_exp = trackreconstruct->L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
				  	h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

				  	// 填充 mode statistic
				  	bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
				  	h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  //
					  if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])  &&
							  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
							  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]))
					  {
						  candimulti_sum[ssdindex]++;
						  candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
					  }
				  }
					//__________________________________________________________________
			  	//        以下部分为当前 L2L3-globalomulti6-1111 模式的径迹重建策略！！！
			  	//       ---------------------------------------------------------
					// ---0--12345---
					if (L2L3_CsINum[ssdindex][0]!=L2L3_CsINum[ssdindex][1])
					{
						//
						for (Int_t itrack=0; itrack<1; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=1; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
					// ---01--2345---
					else if (L2L3_CsINum[ssdindex][1]!=L2L3_CsINum[ssdindex][2])
					{
						//
						for (Int_t itrack=0; itrack<2; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=2; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
					/// ---012--345---
					else if (L2L3_CsINum[ssdindex][2]!=L2L3_CsINum[ssdindex][3])
				  {
						//
						for (Int_t itrack=0; itrack<3; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=3; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
					// ---0123--45---
					else if (L2L3_CsINum[ssdindex][3]!=L2L3_CsINum[ssdindex][4])
				  {
						//
						for (Int_t itrack=0; itrack<4; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=4; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
					// ---01234--5---
					else if (L2L3_CsINum[ssdindex][4]!=L2L3_CsINum[ssdindex][5])
				  {
						//
						for (Int_t itrack=0; itrack<5; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=5; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
      		//____________________________________________________________________
				  // 填充每块 CsI 的 candidate multi
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
					  h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
				  }
			  	//
				  if (candimulti_corrected[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
					//
					if (candimulti_corrected[ssdindex]==0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
            }
					}
				  //
				  if (candimulti_sharing[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  	h1_SubmodeSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  // 填充每一个 SSD 的 candidate multi
				  h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
			  }
		  }
	  }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
		  else {
			  entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
			  entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
		  }
	  }
	  lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  x1 = 0.4 * globalmulti;
	  y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
	  latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
	  latex_ratio[ssdindex]->SetTextSize(0.05);
	  latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  //
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeSharing[ssdindex][csiindex],"sharing","l");
		  //
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
	  }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  cans.cd(1);
		  h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
		  if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
			  for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
			  	bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
				  bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
			  }
		  }

		  cans.cd(2);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(3);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(4);
		  h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
		  h1_modestat[ssdindex][csiindex]->SetLineWidth(2);
		  h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
		  h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
		  h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  h1_SubmodeSharing[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineColor(kGreen);
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  legend_SubMode[ssdindex][csiindex]->Draw("SAME");

		  cans.cd(5);
		  h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
		  cans.cd(6);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(7);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(8);
		  // candimulti_sum
		  h1_candimulti_sum[ssdindex]->Draw();
		  h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
		  h1_candimulti_sum[ssdindex]->SetLineWidth(2);
		  h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(106);
		  h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
		  // candimulti_merged
		  h1_candimulti_merged[ssdindex]->Draw("SAME");
		  h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
		  h1_candimulti_merged[ssdindex]->SetLineWidth(2);
		  // candimulti
		  h1_candimulti[ssdindex][csiindex]->Draw("SAME");
		  h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
		  h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
		  // latex_ratio
		  latex_ratio[ssdindex]->Draw();
		  //
		  legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

		  gPad->Modified();
		  gPad->Update();

		  cans.Print(pathPDFOut.c_str());
	  }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void DecodefGlobalMulti6::GlobalMulti6_Decode_1110()
{
	const Int_t globalmulti = 6;
  const char* mode = "1110";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;
  Double_t EL2F_Sharing;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Int_t charge, mass;
  std::string cutname[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];
  Int_t candimulti_corrected[NUM_SSD];
  Int_t candimulti_sharing[NUM_SSD];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 6, -0.5, 5.5);
	  }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

		  h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	  }
  }

  trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
	  trackreconstruct->fChainTrackTree->GetEntry(ientry);
	  timeper.PrintPercentageAndRemainingTime(ientry, nentries);

  	// fill data
	  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		  L2L3_SSDNum [ssdindex].clear();
		  L2L3_CsINum [ssdindex].clear();
		  L2L3_CSIECh [ssdindex].clear();
		  L2L3_L2BEMeV[ssdindex].clear();
		  L2L3_L2FEMeV[ssdindex].clear();
		  L2L3_L1SEMeV[ssdindex].clear();
		  L2L3_CsINum[ssdindex].clear();
		  L2L3_L2BStripNum[ssdindex].clear();
		  L2L3_L2FStripNum[ssdindex].clear();
		  L2L3_L1SStripNum[ssdindex].clear();
		  BananaCut_TelNum[ssdindex].clear();

	  	// 初始化
		  candimulti_sum[ssdindex] = 0;
		  candimulti_corrected[ssdindex] = 0;
		  candimulti_sharing[ssdindex] = 0;
		  for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

		  if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
			  for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
				  if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
				    L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
				  	L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
					  L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
					  L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
					  L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
					  L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
					  L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
					  L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
					  BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
				  }
			  }
	  	}
		  // 填充数据
	  	if (L2L3_CsINum[ssdindex].size() == globalmulti) {
		  	mode_exp = trackreconstruct->L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
				  	h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

				  	// 填充 mode statistic
				  	bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
				  	h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  //
					  if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])  &&
							  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
							  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]))
					  {
						  candimulti_sum[ssdindex]++;
						  candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
					  }
				  }
					//__________________________________________________________________
			  	//        以下部分为当前 L2L3-globalomulti6-1110 模式的径迹重建策略！！！
			  	//       ---------------------------------------------------------
					// ---0--12345---
					if (L2L3_CsINum[ssdindex][0]!=L2L3_CsINum[ssdindex][1])
					{
						//
						for (Int_t itrack=0; itrack<1; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								//
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
								  L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=1; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								//
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
					// ---01--2345---
					else if (L2L3_CsINum[ssdindex][1]!=L2L3_CsINum[ssdindex][2])
					{
						//
						for (Int_t itrack=0; itrack<2; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								//
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---01--2345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=2; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								//
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---01--2345---"<<endl;
							  break;
							}
						}
					}
					/// ---012--345---
					else if (L2L3_CsINum[ssdindex][2]!=L2L3_CsINum[ssdindex][3])
				  {
						//
						for (Int_t itrack=0; itrack<3; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								//
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---012----345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=3; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								//
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---012--345---"<<endl;
							  break;
							}
						}
					}
					// ---0123--45---
					else if (L2L3_CsINum[ssdindex][3]!=L2L3_CsINum[ssdindex][4])
				  {
						//
						for (Int_t itrack=0; itrack<4; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								//
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0123--45---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=4; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								//
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0123--45---"<<endl;
							  break;
							}
						}
					}
					// ---01234--5---
					else if (L2L3_CsINum[ssdindex][4]!=L2L3_CsINum[ssdindex][5])
				  {
						//
						for (Int_t itrack=0; itrack<5; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								//
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---01234--5---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=5; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								//
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
								if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---01234--5---"<<endl;
							  break;
							}
						}
					}
      		//____________________________________________________________________
				  // 填充每块 CsI 的 candidate multi
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
					  h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
				  }
			  	//
				  if (candimulti_corrected[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
					//
					if (candimulti_corrected[ssdindex]==0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
            }
					}
				  //
				  if (candimulti_sharing[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  	h1_SubmodeSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  // 填充每一个 SSD 的 candidate multi
				  h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
			  }
		  }
	  }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
		  else {
			  entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
			  entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
		  }
	  }
	  lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  x1 = 0.4 * globalmulti;
	  y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
	  latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
	  latex_ratio[ssdindex]->SetTextSize(0.05);
	  latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  //
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeSharing[ssdindex][csiindex],"sharing","l");
		  //
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
	  }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  cans.cd(1);
		  h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
		  if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
			  for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
			  	bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
				  bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
			  }
		  }

		  cans.cd(2);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(3);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(4);
		  h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
		  h1_modestat[ssdindex][csiindex]->SetLineWidth(2);
		  h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
		  h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
		  h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  h1_SubmodeSharing[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineColor(kGreen);
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  legend_SubMode[ssdindex][csiindex]->Draw("SAME");

		  cans.cd(5);
		  h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
		  cans.cd(6);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(7);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(8);
		  // candimulti_sum
		  h1_candimulti_sum[ssdindex]->Draw();
		  h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
		  h1_candimulti_sum[ssdindex]->SetLineWidth(2);
		  h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(106);
		  h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
		  // candimulti_merged
		  h1_candimulti_merged[ssdindex]->Draw("SAME");
		  h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
		  h1_candimulti_merged[ssdindex]->SetLineWidth(2);
		  // candimulti
		  h1_candimulti[ssdindex][csiindex]->Draw("SAME");
		  h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
		  h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
		  // latex_ratio
		  latex_ratio[ssdindex]->Draw();
		  //
		  legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

		  gPad->Modified();
		  gPad->Update();

		  cans.Print(pathPDFOut.c_str());
	  }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void DecodefGlobalMulti6::GlobalMulti6_Decode_1011()
{
	const Int_t globalmulti = 6;
  const char* mode = "1011";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Int_t charge, mass;
  std::string cutname[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];
  Int_t candimulti_corrected[NUM_SSD];
  Int_t candimulti_sharing[NUM_SSD];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_Submode2Hit[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_Submode2Hit[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_Submode2Hit_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_Submode2Hit_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 6, -0.5, 5.5);
	  }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

		  h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	  }
  }

  trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
	  trackreconstruct->fChainTrackTree->GetEntry(ientry);
	 // timeper.PrintPercentageAndRemainingTime(ientry, nentries);

  	// fill data
	  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		  L2L3_SSDNum [ssdindex].clear();
		  L2L3_CsINum [ssdindex].clear();
		  L2L3_CSIECh [ssdindex].clear();
		  L2L3_L2BEMeV[ssdindex].clear();
		  L2L3_L2FEMeV[ssdindex].clear();
		  L2L3_L1SEMeV[ssdindex].clear();
		  L2L3_CsINum[ssdindex].clear();
		  L2L3_L2BStripNum[ssdindex].clear();
		  L2L3_L2FStripNum[ssdindex].clear();
		  L2L3_L1SStripNum[ssdindex].clear();
		  BananaCut_TelNum[ssdindex].clear();

	  	// 初始化
		  candimulti_sum[ssdindex] = 0;
		  candimulti_corrected[ssdindex] = 0;
		  candimulti_sharing[ssdindex] = 0;
		  for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

		  if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
			  for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
				  if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
				    L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
				  	L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
					  L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
					  L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
					  L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
					  L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
					  L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
					  L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
					  BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
				  }
			  }
	  	}
		  // 填充数据
	  	if (L2L3_CsINum[ssdindex].size() == globalmulti) {
		  	mode_exp = trackreconstruct->L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
				  	h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

				  	// 填充 mode statistic
				  	bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
				  	h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  //
					  if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])  &&
							  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
							  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]))
					  {
						  candimulti_sum[ssdindex]++;
						  candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
					  }
				  }
					//__________________________________________________________________
			  	//        以下部分为当前 L2L3-globalomulti6-1011 模式的径迹重建策略！！！
			  	//       ---------------------------------------------------------
					// ---0--12345---
					// cout<<"ientry="<<ientry<<setw(15)<<( (L2L3_L2FEMeV[ssdindex][0]==L2L3_L2FEMeV[ssdindex][2]) ? 1 : 0)<<endl;
					if (L2L3_CsINum[ssdindex][0]!=L2L3_CsINum[ssdindex][1])
					{
						//
						for (Int_t itrack=0; itrack<1; itrack++) {
							if ((trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) ||
								   trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][0]+L2L3_L2FEMeV[ssdindex][2])) &&
						       trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							  // cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=1; itrack<globalmulti; itrack++) {
							if ((trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) ||
								   trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][0]+L2L3_L2FEMeV[ssdindex][2])) &&
						       trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							  // cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							  break;
							}
						}
					}
					// ---01--2345---
					else if (L2L3_CsINum[ssdindex][1]!=L2L3_CsINum[ssdindex][2])
					{
						//
						for (Int_t itrack=0; itrack<2; itrack++) {
							if ((trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) ||
								   trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][0]+L2L3_L2FEMeV[ssdindex][2]))  &&
						       trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							  // cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---01--2345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=2; itrack<globalmulti; itrack++) {
							if ((trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) ||
								   trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][0]+L2L3_L2FEMeV[ssdindex][2]))  &&
						       trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							  // cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---01--2345---"<<endl;
							  break;
							}
						}
					}
					/// ---012--345---
					else if (L2L3_CsINum[ssdindex][2]!=L2L3_CsINum[ssdindex][3])
				  {
						//
						for (Int_t itrack=0; itrack<3; itrack++) {
							if ((trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) ||
								   trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][0]+L2L3_L2FEMeV[ssdindex][2]))  &&
						       trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							  // cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---012--345---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=3; itrack<globalmulti; itrack++) {
							if ((trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) ||
								   trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][0]+L2L3_L2FEMeV[ssdindex][2]))  &&
						       trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							  // cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---012--345---"<<endl;
							  break;
							}
						}
					}
					// ---0123--45---
					else if (L2L3_CsINum[ssdindex][3]!=L2L3_CsINum[ssdindex][4])
				  {
						//
						for (Int_t itrack=0; itrack<4; itrack++) {
							if ((trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) ||
								   trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][0]+L2L3_L2FEMeV[ssdindex][2]))  &&
						       trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							  // cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0123--45---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=4; itrack<globalmulti; itrack++) {
							if ((trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) ||
								   trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][0]+L2L3_L2FEMeV[ssdindex][2]))  &&
						       trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							  // cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0123--45---"<<endl;
							  break;
							}
						}
					}
					// ---01234--5---
					else if (L2L3_CsINum[ssdindex][4]!=L2L3_CsINum[ssdindex][5])
				  {
						//
						for (Int_t itrack=0; itrack<5; itrack++) {
							if ((trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) ||
								   trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][0]+L2L3_L2FEMeV[ssdindex][2]))  &&
						       trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								  trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							  // cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---01234--5---"<<endl;
								break;
							}
						}
						//
						for (Int_t itrack=5; itrack<globalmulti; itrack++) {
							if ((trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) ||
								   trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][0]+L2L3_L2FEMeV[ssdindex][2]))  &&
						       trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
							  // L2L3_DEE 直接填充
								h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
								// 如果满足 EL1S_EL2F, 则直接填充
								if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
									h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
								}
								else { // 否则, 利用 LISE++ 反推 L1S_ELoss
									trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
									if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
										L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
										if (L2F_Einc > Condition_L2FEincCut) {
											L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
											h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
										}
									}
								}
							  // cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---01234--5---"<<endl;
							  break;
							}
						}
					}
      		//____________________________________________________________________
				  // 填充每块 CsI 的 candidate multi
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
					  h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
				  }
			  	//
				  if (candimulti_corrected[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
					//
					if (candimulti_corrected[ssdindex]==0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
            }
					}
				  //
				  if (candimulti_corrected[ssdindex]==2) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  	h1_Submode2Hit[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  // 填充每一个 SSD 的 candidate multi
				  h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
			  }
		  }
	  }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
		  else {
			  entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
			  entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
		  }
	  }
	  lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  x1 = 0.4 * globalmulti;
	  y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
	  latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
	  latex_ratio[ssdindex]->SetTextSize(0.05);
	  latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  //
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_Submode2Hit[ssdindex][csiindex],"sharing","l");
		  //
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
	  }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  cans.cd(1);
		  h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
		  if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
			  for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
			  	bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
				  bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
			  }
		  }

		  cans.cd(2);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(3);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(4);
		  h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
		  h1_modestat[ssdindex][csiindex]->SetLineWidth(2);
		  h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
		  h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
		  h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  h1_Submode2Hit[ssdindex][csiindex]->Draw("SAME");
		  h1_Submode2Hit[ssdindex][csiindex]->SetLineColor(kGreen);
		  h1_Submode2Hit[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  legend_SubMode[ssdindex][csiindex]->Draw("SAME");

		  cans.cd(5);
		  h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
		  cans.cd(6);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(7);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(8);
		  // candimulti_sum
		  h1_candimulti_sum[ssdindex]->Draw();
		  h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
		  h1_candimulti_sum[ssdindex]->SetLineWidth(2);
		  h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(106);
		  h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
		  // candimulti_merged
		  h1_candimulti_merged[ssdindex]->Draw("SAME");
		  h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
		  h1_candimulti_merged[ssdindex]->SetLineWidth(2);
		  // candimulti
		  h1_candimulti[ssdindex][csiindex]->Draw("SAME");
		  h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
		  h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
		  // latex_ratio
		  latex_ratio[ssdindex]->Draw();
		  //
		  legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

		  gPad->Modified();
		  gPad->Update();

		  cans.Print(pathPDFOut.c_str());
	  }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void DecodefGlobalMulti6::GlobalMulti6_Decode_0211()
{
	const Int_t globalmulti = 6;
  const char* mode = "0211";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;
  Double_t EL2F_Sharing;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Int_t charge, mass;
  std::string cutname[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];
  Int_t candimulti_corrected[NUM_SSD];
  Int_t candimulti_sharing[NUM_SSD];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 6, -0.5, 5.5);
	  }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

		  h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	  }
  }

  trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
	  trackreconstruct->fChainTrackTree->GetEntry(ientry);
	  timeper.PrintPercentageAndRemainingTime(ientry, nentries);

  	// fill data
	  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		  L2L3_SSDNum [ssdindex].clear();
		  L2L3_CsINum [ssdindex].clear();
		  L2L3_CSIECh [ssdindex].clear();
		  L2L3_L2BEMeV[ssdindex].clear();
		  L2L3_L2FEMeV[ssdindex].clear();
		  L2L3_L1SEMeV[ssdindex].clear();
		  L2L3_CsINum[ssdindex].clear();
		  L2L3_L2BStripNum[ssdindex].clear();
		  L2L3_L2FStripNum[ssdindex].clear();
		  L2L3_L1SStripNum[ssdindex].clear();
		  BananaCut_TelNum[ssdindex].clear();

	  	// 初始化
		  candimulti_sum[ssdindex] = 0;
		  candimulti_corrected[ssdindex] = 0;
		  candimulti_sharing[ssdindex] = 0;
		  for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

		  if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
			  for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
				  if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
				    L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
				  	L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
					  L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
					  L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
					  L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
					  L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
					  L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
					  L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
					  BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
				  }
			  }
	  	}
		  // 填充数据
	  	if (L2L3_CsINum[ssdindex].size() == globalmulti) {
		  	mode_exp = trackreconstruct->L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
				  	h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

				  	// 填充 mode statistic
				  	bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
				  	h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  //
					  if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])  &&
							  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
							  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]))
					  {
						  candimulti_sum[ssdindex]++;
						  candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
					  }
				  }
					//__________________________________________________________________
			  	//        以下部分为当前 L2L3-globalomulti6-0211 模式的径迹重建策略！！！
			  	//       ---------------------------------------------------------
					for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						  trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
						{
							candimulti_corrected[ssdindex]++;
							// L2L3_DEE 直接填充
							h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
							// 如果满足 EL1S_EL2F, 则直接填充
							if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
							}
							else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
							  if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							}
						//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							break;
						}
					}
      		//____________________________________________________________________
				  // 填充每块 CsI 的 candidate multi
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
					  h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
				  }
			  	//
				  if (candimulti_corrected[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
					//
					if (candimulti_corrected[ssdindex]==0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
            }
					}
				  //
				  if (candimulti_sharing[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  	h1_SubmodeSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  // 填充每一个 SSD 的 candidate multi
				  h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
			  }
		  }
	  }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
		  else {
			  entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
			  entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
		  }
	  }
	  lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  x1 = 0.4 * globalmulti;
	  y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
	  latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
	  latex_ratio[ssdindex]->SetTextSize(0.05);
	  latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  //
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeSharing[ssdindex][csiindex],"sharing","l");
		  //
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
	  }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  cans.cd(1);
		  h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
		  if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
			  for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
			  	bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
				  bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
			  }
		  }

		  cans.cd(2);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(3);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(4);
		  h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
		  h1_modestat[ssdindex][csiindex]->SetLineWidth(2);
		  h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
		  h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
		  h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  h1_SubmodeSharing[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineColor(kGreen);
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  legend_SubMode[ssdindex][csiindex]->Draw("SAME");

		  cans.cd(5);
		  h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
		  cans.cd(6);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(7);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(8);
		  // candimulti_sum
		  h1_candimulti_sum[ssdindex]->Draw();
		  h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
		  h1_candimulti_sum[ssdindex]->SetLineWidth(2);
		  h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(106);
		  h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
		  // candimulti_merged
		  h1_candimulti_merged[ssdindex]->Draw("SAME");
		  h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
		  h1_candimulti_merged[ssdindex]->SetLineWidth(2);
		  // candimulti
		  h1_candimulti[ssdindex][csiindex]->Draw("SAME");
		  h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
		  h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
		  // latex_ratio
		  latex_ratio[ssdindex]->Draw();
		  //
		  legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

		  gPad->Modified();
		  gPad->Update();

		  cans.Print(pathPDFOut.c_str());
	  }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void DecodefGlobalMulti6::GlobalMulti6_Decode_0121()
{
	const Int_t globalmulti = 6;
  const char* mode = "0121";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;
  Double_t EL2F_Sharing;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Int_t charge, mass;
  std::string cutname[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];
  Int_t candimulti_corrected[NUM_SSD];
  Int_t candimulti_sharing[NUM_SSD];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 6, -0.5, 5.5);
	  }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

		  h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	  }
  }

  trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
	  trackreconstruct->fChainTrackTree->GetEntry(ientry);
	  timeper.PrintPercentageAndRemainingTime(ientry, nentries);

  	// fill data
	  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		  L2L3_SSDNum [ssdindex].clear();
		  L2L3_CsINum [ssdindex].clear();
		  L2L3_CSIECh [ssdindex].clear();
		  L2L3_L2BEMeV[ssdindex].clear();
		  L2L3_L2FEMeV[ssdindex].clear();
		  L2L3_L1SEMeV[ssdindex].clear();
		  L2L3_CsINum[ssdindex].clear();
		  L2L3_L2BStripNum[ssdindex].clear();
		  L2L3_L2FStripNum[ssdindex].clear();
		  L2L3_L1SStripNum[ssdindex].clear();
		  BananaCut_TelNum[ssdindex].clear();

	  	// 初始化
		  candimulti_sum[ssdindex] = 0;
		  candimulti_corrected[ssdindex] = 0;
		  candimulti_sharing[ssdindex] = 0;
		  for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

		  if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
			  for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
				  if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
				    L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
				  	L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
					  L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
					  L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
					  L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
					  L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
					  L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
					  L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
					  BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
				  }
			  }
	  	}
		  // 填充数据
	  	if (L2L3_CsINum[ssdindex].size() == globalmulti) {
		  	mode_exp = trackreconstruct->L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
				  	h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

				  	// 填充 mode statistic
				  	bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
				  	h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  //
					  if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])  &&
							  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
							  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]))
					  {
						  candimulti_sum[ssdindex]++;
						  candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
					  }
				  }
					//__________________________________________________________________
			  	//        以下部分为当前 L2L3-globalomulti6-0121 模式的径迹重建策略！！！
			  	//       ---------------------------------------------------------
					for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						  trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
						{
							candimulti_corrected[ssdindex]++;
							// L2L3_DEE 直接填充
							h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
							// 如果满足 EL1S_EL2F, 则直接填充
							if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
							}
							else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
							  if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							}
						//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							break;
						}
					}
      		//____________________________________________________________________
				  // 填充每块 CsI 的 candidate multi
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
					  h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
				  }
			  	//
				  if (candimulti_corrected[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
					//
					if (candimulti_corrected[ssdindex]==0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
            }
					}
				  //
				  if (candimulti_sharing[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  	h1_SubmodeSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  // 填充每一个 SSD 的 candidate multi
				  h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
			  }
		  }
	  }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
		  else {
			  entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
			  entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
		  }
	  }
	  lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  x1 = 0.4 * globalmulti;
	  y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
	  latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
	  latex_ratio[ssdindex]->SetTextSize(0.05);
	  latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  //
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeSharing[ssdindex][csiindex],"sharing","l");
		  //
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
	  }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  cans.cd(1);
		  h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
		  if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
			  for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
			  	bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
				  bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
			  }
		  }

		  cans.cd(2);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(3);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(4);
		  h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
		  h1_modestat[ssdindex][csiindex]->SetLineWidth(2);
		  h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
		  h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
		  h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  h1_SubmodeSharing[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineColor(kGreen);
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  legend_SubMode[ssdindex][csiindex]->Draw("SAME");

		  cans.cd(5);
		  h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
		  cans.cd(6);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(7);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(8);
		  // candimulti_sum
		  h1_candimulti_sum[ssdindex]->Draw();
		  h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
		  h1_candimulti_sum[ssdindex]->SetLineWidth(2);
		  h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(106);
		  h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
		  // candimulti_merged
		  h1_candimulti_merged[ssdindex]->Draw("SAME");
		  h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
		  h1_candimulti_merged[ssdindex]->SetLineWidth(2);
		  // candimulti
		  h1_candimulti[ssdindex][csiindex]->Draw("SAME");
		  h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
		  h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
		  // latex_ratio
		  latex_ratio[ssdindex]->Draw();
		  //
		  legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

		  gPad->Modified();
		  gPad->Update();

		  cans.Print(pathPDFOut.c_str());
	  }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void DecodefGlobalMulti6::GlobalMulti6_Decode_0120()
{
	const Int_t globalmulti = 6;
  const char* mode = "0120";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;
  Double_t EL2F_Sharing;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Int_t charge, mass;
  std::string cutname[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];
  Int_t candimulti_corrected[NUM_SSD];
  Int_t candimulti_sharing[NUM_SSD];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 6, -0.5, 5.5);
	  }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

		  h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	  }
  }

  trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
	  trackreconstruct->fChainTrackTree->GetEntry(ientry);
	  timeper.PrintPercentageAndRemainingTime(ientry, nentries);

  	// fill data
	  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		  L2L3_SSDNum [ssdindex].clear();
		  L2L3_CsINum [ssdindex].clear();
		  L2L3_CSIECh [ssdindex].clear();
		  L2L3_L2BEMeV[ssdindex].clear();
		  L2L3_L2FEMeV[ssdindex].clear();
		  L2L3_L1SEMeV[ssdindex].clear();
		  L2L3_CsINum[ssdindex].clear();
		  L2L3_L2BStripNum[ssdindex].clear();
		  L2L3_L2FStripNum[ssdindex].clear();
		  L2L3_L1SStripNum[ssdindex].clear();
		  BananaCut_TelNum[ssdindex].clear();

	  	// 初始化
		  candimulti_sum[ssdindex] = 0;
		  candimulti_corrected[ssdindex] = 0;
		  candimulti_sharing[ssdindex] = 0;
		  for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

		  if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
			  for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
				  if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
				    L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
				  	L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
					  L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
					  L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
					  L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
					  L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
					  L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
					  L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
					  BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
				  }
			  }
	  	}
		  // 填充数据
	  	if (L2L3_CsINum[ssdindex].size() == globalmulti) {
		  	mode_exp = trackreconstruct->L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
				  	h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

				  	// 填充 mode statistic
				  	bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
				  	h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  //
					  if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])  &&
							  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
							  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]))
					  {
						  candimulti_sum[ssdindex]++;
						  candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
					  }
				  }
					//__________________________________________________________________
			  	//        以下部分为当前 L2L3-globalomulti6-0120 模式的径迹重建策略！！！
			  	//       ---------------------------------------------------------
					for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						  trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
						{
							candimulti_corrected[ssdindex]++;
							// L2L3_DEE 直接填充
							h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
							// 如果满足 EL1S_EL2F, 则直接填充
							if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
							}
							else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
							  if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							}
						//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							break;
						}
					}
      		//____________________________________________________________________
				  // 填充每块 CsI 的 candidate multi
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
					  h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
				  }
			  	//
				  if (candimulti_corrected[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
					//
					if (candimulti_corrected[ssdindex]==0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
            }
					}
				  //
				  if (candimulti_sharing[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  	h1_SubmodeSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  // 填充每一个 SSD 的 candidate multi
				  h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
			  }
		  }
	  }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
		  else {
			  entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
			  entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
		  }
	  }
	  lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  x1 = 0.4 * globalmulti;
	  y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
	  latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
	  latex_ratio[ssdindex]->SetTextSize(0.05);
	  latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  //
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeSharing[ssdindex][csiindex],"sharing","l");
		  //
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
	  }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  cans.cd(1);
		  h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
		  if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
			  for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
			  	bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
				  bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
			  }
		  }

		  cans.cd(2);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(3);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(4);
		  h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
		  h1_modestat[ssdindex][csiindex]->SetLineWidth(2);
		  h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
		  h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
		  h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  h1_SubmodeSharing[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineColor(kGreen);
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  legend_SubMode[ssdindex][csiindex]->Draw("SAME");

		  cans.cd(5);
		  h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
		  cans.cd(6);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(7);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(8);
		  // candimulti_sum
		  h1_candimulti_sum[ssdindex]->Draw();
		  h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
		  h1_candimulti_sum[ssdindex]->SetLineWidth(2);
		  h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(106);
		  h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
		  // candimulti_merged
		  h1_candimulti_merged[ssdindex]->Draw("SAME");
		  h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
		  h1_candimulti_merged[ssdindex]->SetLineWidth(2);
		  // candimulti
		  h1_candimulti[ssdindex][csiindex]->Draw("SAME");
		  h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
		  h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
		  // latex_ratio
		  latex_ratio[ssdindex]->Draw();
		  //
		  legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

		  gPad->Modified();
		  gPad->Update();

		  cans.Print(pathPDFOut.c_str());
	  }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void DecodefGlobalMulti6::GlobalMulti6_Decode_0112()
{
	const Int_t globalmulti = 6;
  const char* mode = "0112";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;
  Double_t EL2F_Sharing;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Int_t charge, mass;
  std::string cutname[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];
  Int_t candimulti_corrected[NUM_SSD];
  Int_t candimulti_sharing[NUM_SSD];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 6, -0.5, 5.5);
	  }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

		  h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	  }
  }

  trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
	  trackreconstruct->fChainTrackTree->GetEntry(ientry);
	  timeper.PrintPercentageAndRemainingTime(ientry, nentries);

  	// fill data
	  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		  L2L3_SSDNum [ssdindex].clear();
		  L2L3_CsINum [ssdindex].clear();
		  L2L3_CSIECh [ssdindex].clear();
		  L2L3_L2BEMeV[ssdindex].clear();
		  L2L3_L2FEMeV[ssdindex].clear();
		  L2L3_L1SEMeV[ssdindex].clear();
		  L2L3_CsINum[ssdindex].clear();
		  L2L3_L2BStripNum[ssdindex].clear();
		  L2L3_L2FStripNum[ssdindex].clear();
		  L2L3_L1SStripNum[ssdindex].clear();
		  BananaCut_TelNum[ssdindex].clear();

	  	// 初始化
		  candimulti_sum[ssdindex] = 0;
		  candimulti_corrected[ssdindex] = 0;
		  candimulti_sharing[ssdindex] = 0;
		  for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

		  if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
			  for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
				  if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
				    L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
				  	L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
					  L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
					  L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
					  L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
					  L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
					  L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
					  L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
					  BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
				  }
			  }
	  	}
		  // 填充数据
	  	if (L2L3_CsINum[ssdindex].size() == globalmulti) {
		  	mode_exp = trackreconstruct->L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
				  	h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

				  	// 填充 mode statistic
				  	bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
				  	h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  //
					  if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])  &&
							  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
							  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]))
					  {
						  candimulti_sum[ssdindex]++;
						  candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
					  }
				  }
					//__________________________________________________________________
			  	//        以下部分为当前 L2L3-globalomulti6-0112 模式的径迹重建策略！！！
			  	//       ---------------------------------------------------------
					for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						  trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
						{
							candimulti_corrected[ssdindex]++;
							// L2L3_DEE 直接填充
							h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
							// 如果满足 EL1S_EL2F, 则直接填充
							if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
							}
							else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
							  if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							}
						//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							break;
						}
					}
      		//____________________________________________________________________
				  // 填充每块 CsI 的 candidate multi
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
					  h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
				  }
			  	//
				  if (candimulti_corrected[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
					//
					if (candimulti_corrected[ssdindex]==0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
            }
					}
				  //
				  if (candimulti_sharing[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  	h1_SubmodeSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  // 填充每一个 SSD 的 candidate multi
				  h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
			  }
		  }
	  }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
		  else {
			  entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
			  entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
		  }
	  }
	  lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  x1 = 0.4 * globalmulti;
	  y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
	  latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
	  latex_ratio[ssdindex]->SetTextSize(0.05);
	  latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  //
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeSharing[ssdindex][csiindex],"sharing","l");
		  //
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
	  }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  cans.cd(1);
		  h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
		  if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
			  for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
			  	bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
				  bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
			  }
		  }

		  cans.cd(2);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(3);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(4);
		  h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
		  h1_modestat[ssdindex][csiindex]->SetLineWidth(2);
		  h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
		  h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
		  h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  h1_SubmodeSharing[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineColor(kGreen);
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  legend_SubMode[ssdindex][csiindex]->Draw("SAME");

		  cans.cd(5);
		  h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
		  cans.cd(6);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(7);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(8);
		  // candimulti_sum
		  h1_candimulti_sum[ssdindex]->Draw();
		  h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
		  h1_candimulti_sum[ssdindex]->SetLineWidth(2);
		  h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(106);
		  h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
		  // candimulti_merged
		  h1_candimulti_merged[ssdindex]->Draw("SAME");
		  h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
		  h1_candimulti_merged[ssdindex]->SetLineWidth(2);
		  // candimulti
		  h1_candimulti[ssdindex][csiindex]->Draw("SAME");
		  h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
		  h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
		  // latex_ratio
		  latex_ratio[ssdindex]->Draw();
		  //
		  legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

		  gPad->Modified();
		  gPad->Update();

		  cans.Print(pathPDFOut.c_str());
	  }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void DecodefGlobalMulti6::GlobalMulti6_Decode_0111()
{
	const Int_t globalmulti = 6;
  const char* mode = "0111";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;
  Double_t EL2F_Sharing;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Int_t charge, mass;
  std::string cutname[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];
  Int_t candimulti_corrected[NUM_SSD];
  Int_t candimulti_sharing[NUM_SSD];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 6, -0.5, 5.5);
	  }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

		  h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	  }
  }

  trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
	  trackreconstruct->fChainTrackTree->GetEntry(ientry);
	  timeper.PrintPercentageAndRemainingTime(ientry, nentries);

  	// fill data
	  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		  L2L3_SSDNum [ssdindex].clear();
		  L2L3_CsINum [ssdindex].clear();
		  L2L3_CSIECh [ssdindex].clear();
		  L2L3_L2BEMeV[ssdindex].clear();
		  L2L3_L2FEMeV[ssdindex].clear();
		  L2L3_L1SEMeV[ssdindex].clear();
		  L2L3_CsINum[ssdindex].clear();
		  L2L3_L2BStripNum[ssdindex].clear();
		  L2L3_L2FStripNum[ssdindex].clear();
		  L2L3_L1SStripNum[ssdindex].clear();
		  BananaCut_TelNum[ssdindex].clear();

	  	// 初始化
		  candimulti_sum[ssdindex] = 0;
		  candimulti_corrected[ssdindex] = 0;
		  candimulti_sharing[ssdindex] = 0;
		  for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

		  if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
			  for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
				  if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
				    L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
				  	L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
					  L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
					  L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
					  L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
					  L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
					  L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
					  L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
					  BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
				  }
			  }
	  	}
		  // 填充数据
	  	if (L2L3_CsINum[ssdindex].size() == globalmulti) {
		  	mode_exp = trackreconstruct->L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
				  	h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

				  	// 填充 mode statistic
				  	bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
				  	h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  //
					  if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])  &&
							  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
							  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]))
					  {
						  candimulti_sum[ssdindex]++;
						  candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
					  }
				  }
					//__________________________________________________________________
			  	//        以下部分为当前 L2L3-globalomulti6-0111 模式的径迹重建策略！！！
			  	//       ---------------------------------------------------------
					for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						  trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
						{
							candimulti_corrected[ssdindex]++;
							// L2L3_DEE 直接填充
							h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
							// 如果满足 EL1S_EL2F, 则直接填充
							if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
							}
							else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
							  if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							}
						//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							break;
						}
					}
      		//____________________________________________________________________
				  // 填充每块 CsI 的 candidate multi
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
					  h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
				  }
			  	//
				  if (candimulti_corrected[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
					//
					if (candimulti_corrected[ssdindex]==0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
            }
					}
				  //
				  if (candimulti_sharing[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  	h1_SubmodeSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  // 填充每一个 SSD 的 candidate multi
				  h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
			  }
		  }
	  }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
		  else {
			  entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
			  entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
		  }
	  }
	  lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  x1 = 0.4 * globalmulti;
	  y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
	  latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
	  latex_ratio[ssdindex]->SetTextSize(0.05);
	  latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  //
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeSharing[ssdindex][csiindex],"sharing","l");
		  //
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
	  }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  cans.cd(1);
		  h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
		  if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
			  for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
			  	bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
				  bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
			  }
		  }

		  cans.cd(2);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(3);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(4);
		  h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
		  h1_modestat[ssdindex][csiindex]->SetLineWidth(2);
		  h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
		  h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
		  h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  h1_SubmodeSharing[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineColor(kGreen);
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  legend_SubMode[ssdindex][csiindex]->Draw("SAME");

		  cans.cd(5);
		  h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
		  cans.cd(6);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(7);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(8);
		  // candimulti_sum
		  h1_candimulti_sum[ssdindex]->Draw();
		  h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
		  h1_candimulti_sum[ssdindex]->SetLineWidth(2);
		  h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(106);
		  h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
		  // candimulti_merged
		  h1_candimulti_merged[ssdindex]->Draw("SAME");
		  h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
		  h1_candimulti_merged[ssdindex]->SetLineWidth(2);
		  // candimulti
		  h1_candimulti[ssdindex][csiindex]->Draw("SAME");
		  h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
		  h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
		  // latex_ratio
		  latex_ratio[ssdindex]->Draw();
		  //
		  legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

		  gPad->Modified();
		  gPad->Update();

		  cans.Print(pathPDFOut.c_str());
	  }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void DecodefGlobalMulti6::GlobalMulti6_Decode_0021()
{
	const Int_t globalmulti = 6;
  const char* mode = "0021";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;
  Double_t EL2F_Sharing;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Int_t charge, mass;
  std::string cutname[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];
  Int_t candimulti_corrected[NUM_SSD];
  Int_t candimulti_sharing[NUM_SSD];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 6, -0.5, 5.5);
	  }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

		  h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	  }
  }

  trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
	  trackreconstruct->fChainTrackTree->GetEntry(ientry);
	  timeper.PrintPercentageAndRemainingTime(ientry, nentries);

  	// fill data
	  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		  L2L3_SSDNum [ssdindex].clear();
		  L2L3_CsINum [ssdindex].clear();
		  L2L3_CSIECh [ssdindex].clear();
		  L2L3_L2BEMeV[ssdindex].clear();
		  L2L3_L2FEMeV[ssdindex].clear();
		  L2L3_L1SEMeV[ssdindex].clear();
		  L2L3_CsINum[ssdindex].clear();
		  L2L3_L2BStripNum[ssdindex].clear();
		  L2L3_L2FStripNum[ssdindex].clear();
		  L2L3_L1SStripNum[ssdindex].clear();
		  BananaCut_TelNum[ssdindex].clear();

	  	// 初始化
		  candimulti_sum[ssdindex] = 0;
		  candimulti_corrected[ssdindex] = 0;
		  candimulti_sharing[ssdindex] = 0;
		  for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

		  if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
			  for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
				  if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
				    L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
				  	L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
					  L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
					  L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
					  L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
					  L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
					  L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
					  L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
					  BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
				  }
			  }
	  	}
		  // 填充数据
	  	if (L2L3_CsINum[ssdindex].size() == globalmulti) {
		  	mode_exp = trackreconstruct->L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
				  	h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

				  	// 填充 mode statistic
				  	bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
				  	h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  //
					  if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])  &&
							  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
							  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]))
					  {
						  candimulti_sum[ssdindex]++;
						  candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
					  }
				  }
					//__________________________________________________________________
			  	//        以下部分为当前 L2L3-globalomulti6-0021 模式的径迹重建策略！！！
			  	//       ---------------------------------------------------------
					for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						  trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
						{
							candimulti_corrected[ssdindex]++;
							// L2L3_DEE 直接填充
							h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
							// 如果满足 EL1S_EL2F, 则直接填充
							if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
							}
							else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
							  if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							}
						//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							break;
						}
					}
      		//____________________________________________________________________
				  // 填充每块 CsI 的 candidate multi
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
					  h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
				  }
			  	//
				  if (candimulti_corrected[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
					//
					if (candimulti_corrected[ssdindex]==0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
            }
					}
				  //
				  if (candimulti_sharing[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  	h1_SubmodeSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  // 填充每一个 SSD 的 candidate multi
				  h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
			  }
		  }
	  }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
		  else {
			  entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
			  entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
		  }
	  }
	  lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  x1 = 0.4 * globalmulti;
	  y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
	  latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
	  latex_ratio[ssdindex]->SetTextSize(0.05);
	  latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  //
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeSharing[ssdindex][csiindex],"sharing","l");
		  //
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
	  }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  cans.cd(1);
		  h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
		  if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
			  for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
			  	bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
				  bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
			  }
		  }

		  cans.cd(2);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(3);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(4);
		  h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
		  h1_modestat[ssdindex][csiindex]->SetLineWidth(2);
		  h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
		  h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
		  h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  h1_SubmodeSharing[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineColor(kGreen);
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  legend_SubMode[ssdindex][csiindex]->Draw("SAME");

		  cans.cd(5);
		  h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
		  cans.cd(6);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(7);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(8);
		  // candimulti_sum
		  h1_candimulti_sum[ssdindex]->Draw();
		  h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
		  h1_candimulti_sum[ssdindex]->SetLineWidth(2);
		  h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(106);
		  h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
		  // candimulti_merged
		  h1_candimulti_merged[ssdindex]->Draw("SAME");
		  h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
		  h1_candimulti_merged[ssdindex]->SetLineWidth(2);
		  // candimulti
		  h1_candimulti[ssdindex][csiindex]->Draw("SAME");
		  h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
		  h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
		  // latex_ratio
		  latex_ratio[ssdindex]->Draw();
		  //
		  legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

		  gPad->Modified();
		  gPad->Update();

		  cans.Print(pathPDFOut.c_str());
	  }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void DecodefGlobalMulti6::GlobalMulti6_Decode_0012()
{
	const Int_t globalmulti = 6;
  const char* mode = "0012";
  std::string mode_exp;

  Double_t Condition_L2FElossCut = 0.0; // MeV
  Double_t Condition_L2FEincCut  = 0.0; // MeV
  Double_t LISE_Presision = 0.5;

  Double_t L2F_Einc;
  Double_t L1S_ELoss;
  Double_t L1S_ECalc_EReal_ErrCut = 0.10;
  Double_t EL2F_Sharing;

  Int_t mode_index;
  Int_t bin_index[globalmulti];

  Int_t charge, mass;
  std::string cutname[globalmulti];

  Double_t lost_ratio[NUM_SSD] = {0};
  Int_t candmulti_csi[NUM_SSD][NUM_CSI];
  Int_t candimulti_sum[NUM_SSD];
  Int_t candimulti_merged[NUM_SSD];
  Int_t candimulti_corrected[NUM_SSD];
  Int_t candimulti_sharing[NUM_SSD];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L2L3_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeCorrected[NUM_SSD][NUM_CSI];
  TH1I* h1_SubmodeSharing[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti[NUM_SSD][NUM_CSI];
  TH1I* h1_candimulti_sum[NUM_SSD];
  TH1I* h1_candimulti_merged[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  h1_candimulti_sum[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  h1_candimulti_merged[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), Form("SSD%d_CandiMulti_Merged_%s",ssdindex+1,mode), 6, -0.5, 5.5);
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h1_modestat[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_ModeStat_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeCorrected[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeCorrected_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_SubmodeSharing[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_SubmodeSharing_%s",ssdindex+1,csiindex,mode), 8, -0.5, 7.5);
		  h1_candimulti[ssdindex][csiindex] = new TH1I(Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), Form("SSD%d_CsI%d_CandiMulti_%s",ssdindex+1,csiindex,mode), 6, -0.5, 5.5);
	  }
  }

  TH2F h2_L2L3_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L2L3_DEE_Discard[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[NUM_SSD][NUM_CSI];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD][NUM_CSI];

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  h2_L2L3_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Cuts_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L2L3_Discard_%s",ssdindex+1,csiindex,mode),4000,0,4000,NBinsL2[ssdindex],0,EL2Range[ssdindex]);

		  h2_L1L2_DEE[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Cuts_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex] = TH2F(Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),Form("SSD%d_CsI%d_L1L2_Discard_%s",ssdindex+1,csiindex,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	  }
  }

  trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;
  cout<<Form("Processing mode%s ......",mode)<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
	  trackreconstruct->fChainTrackTree->GetEntry(ientry);
	  timeper.PrintPercentageAndRemainingTime(ientry, nentries);

  	// fill data
	  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		  L2L3_SSDNum [ssdindex].clear();
		  L2L3_CsINum [ssdindex].clear();
		  L2L3_CSIECh [ssdindex].clear();
		  L2L3_L2BEMeV[ssdindex].clear();
		  L2L3_L2FEMeV[ssdindex].clear();
		  L2L3_L1SEMeV[ssdindex].clear();
		  L2L3_CsINum[ssdindex].clear();
		  L2L3_L2BStripNum[ssdindex].clear();
		  L2L3_L2FStripNum[ssdindex].clear();
		  L2L3_L1SStripNum[ssdindex].clear();
		  BananaCut_TelNum[ssdindex].clear();

	  	// 初始化
		  candimulti_sum[ssdindex] = 0;
		  candimulti_corrected[ssdindex] = 0;
		  candimulti_sharing[ssdindex] = 0;
		  for(Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) { candmulti_csi[ssdindex][csiindex] = 0; }

		  if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
			  for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
				  if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
				    L2L3_CSIECh [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsIECh[gmulti]);
				  	L2L3_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
					  L2L3_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
					  L2L3_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);
					  L2L3_CsINum [ssdindex].push_back(trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
					  L2L3_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
					  L2L3_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
					  L2L3_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
					  BananaCut_TelNum[ssdindex].push_back(ssdindex*NUM_CSI+trackreconstruct->fTrackEvent.fGCsINum[gmulti]);
				  }
			  }
	  	}
		  // 填充数据
	  	if (L2L3_CsINum[ssdindex].size() == globalmulti) {
		  	mode_exp = trackreconstruct->L2L3_CalcModeFromExpData(globalmulti,L2L3_CsINum[ssdindex],L2L3_L2BStripNum[ssdindex],L2L3_L2FStripNum[ssdindex],L2L3_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h2_L2L3_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
				  	h2_L1L2_DEE[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);

				  	// 填充 mode statistic
				  	bin_index[itrack] = trackreconstruct->L2L3_GetSubModeFromEneConstraints(trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]),
																																								  	trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex,L2L3_L1SEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]));
				  	h1_modestat[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  //
					  if (trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack])  &&
							  trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
							  trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]))
					  {
						  candimulti_sum[ssdindex]++;
						  candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]++;
					  }
				  }
					//__________________________________________________________________
			  	//        以下部分为当前 L2L3-globalomulti6-0012 模式的径迹重建策略！！！
			  	//       ---------------------------------------------------------
					for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L2L3_L2BEMeV[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]) &&
						  trackreconstruct->IsInsideABananaCut(bananacut[BananaCut_TelNum[ssdindex][itrack]],cutname[itrack],L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]))
						{
							candimulti_corrected[ssdindex]++;
							// L2L3_DEE 直接填充
							h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack],L2L3_L2FEMeV[ssdindex][itrack]);
							// 如果满足 EL1S_EL2F, 则直接填充
							if (trackreconstruct->IsEneConstraint_L1S_L2F(ssdindex, L2L3_L1SEMeV[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack])) {
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L2L3_L1SEMeV[ssdindex][itrack]);
							}
							else { // 否则, 利用 LISE++ 反推 L1S_ELoss
								trackreconstruct->GetZAFromBananaCutName(cutname[itrack],charge,mass);
							  if (L2L3_L2FEMeV[ssdindex][itrack]>Condition_L2FElossCut) {
									L2F_Einc = fEloss.GetIncidentEnergy(charge,mass,L2L3_L2FEMeV[ssdindex][itrack],"Si",SIL2THICKNESS[ssdindex]*1000,LISE_Presision,1);
									if (L2F_Einc > Condition_L2FEincCut) {
										L1S_ELoss = fEloss.GetEnergyLossFromResidualEnergy(charge,mass,L2F_Einc,"Si",SIL1THICKNESS[ssdindex]*1000,LISE_Presision,1);
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack],L1S_ELoss);
									}
								}
							}
						//  cout<<"ientry="<<ientry<<setw(15)<<"itrack="<<itrack<<setw(15)<<"---0--12345---"<<endl;
							break;
						}
					}
      		//____________________________________________________________________
				  // 填充每块 CsI 的 candidate multi
				  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  h1_candimulti[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
					  h1_candimulti_merged[ssdindex]->Fill(candmulti_csi[ssdindex][L2L3_CsINum[ssdindex][itrack]]);
				  }
			  	//
				  if (candimulti_corrected[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						  h1_SubmodeCorrected[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
					//
					if (candimulti_corrected[ssdindex]==0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
              h2_L2L3_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_CSIECh[ssdindex][itrack], L2L3_L2FEMeV[ssdindex][itrack]);
              h2_L1L2_DEE_Discard[ssdindex][L2L3_CsINum[ssdindex][itrack]].Fill(L2L3_L2FEMeV[ssdindex][itrack], L2L3_L1SEMeV[ssdindex][itrack]);
            }
					}
				  //
				  if (candimulti_sharing[ssdindex] > 0) {
					  for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  	h1_SubmodeSharing[ssdindex][L2L3_CsINum[ssdindex][itrack]]->Fill(bin_index[itrack]);
					  }
				  }
				  // 填充每一个 SSD 的 candidate multi
				  h1_candimulti_sum[ssdindex]->Fill(candimulti_sum[ssdindex]);
			  }
		  }
	  }
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  if ((ssdindex==3 && csiindex==4) || (ssdindex==3 && csiindex==5)) continue;
		  else {
			  entry_all[ssdindex] += h2_L2L3_DEE[ssdindex][csiindex].GetEntries();
			  entry_discard[ssdindex] += h2_L2L3_DEE_Discard[ssdindex][csiindex].GetEntries();
		  }
	  }
	  lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  x1 = 0.4 * globalmulti;
	  y1 = 0.9 * h1_candimulti_merged[ssdindex]->GetMaximum();
	  latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
	  latex_ratio[ssdindex]->SetTextSize(0.05);
	  latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD][NUM_CSI];
  TLegend* legend_CandiMulti[NUM_SSD][NUM_CSI];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  legend_SubMode[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  legend_CandiMulti[ssdindex][csiindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		  //
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_modestat[ssdindex][csiindex],"sub-modes","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeCorrected[ssdindex][csiindex],"corrected","l");
		  legend_SubMode[ssdindex][csiindex]->AddEntry(h1_SubmodeSharing[ssdindex][csiindex],"sharing","l");
		  //
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_sum[ssdindex],"candimulti","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti_merged[ssdindex],"merged","l");
		  legend_CandiMulti[ssdindex][csiindex]->AddEntry(h1_candimulti[ssdindex][csiindex],"csi","l");
	  }
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans","cans",1600,1000);
  cans.Divide(4,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
	  for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
		  cans.cd(1);
		  h2_L2L3_DEE[ssdindex][csiindex].Draw("COL");
		  if (bananacut[ssdindex*NUM_CSI+csiindex].size()>1) {
			  for (Int_t ip=0; ip<bananacut[ssdindex*NUM_CSI+csiindex].size(); ip++) {
			  	bananacut[ssdindex*NUM_CSI+csiindex][ip]->Draw("SAME");
				  bananacut[ssdindex*NUM_CSI+csiindex][ip]->SetLineColor(kRed);
			  }
		  }

		  cans.cd(2);
		  h2_L2L3_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(3);
		  h2_L2L3_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(4);
		  h1_modestat[ssdindex][csiindex]->Draw("HIST TEXT");
		  h1_modestat[ssdindex][csiindex]->SetLineWidth(2);
		  h1_modestat[ssdindex][csiindex]->GetXaxis()->SetNdivisions(108);
		  h1_modestat[ssdindex][csiindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex][csiindex]->GetMaximum());
		  h1_SubmodeCorrected[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineColor(kRed);
		  h1_SubmodeCorrected[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  h1_SubmodeSharing[ssdindex][csiindex]->Draw("SAME");
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineColor(kGreen);
		  h1_SubmodeSharing[ssdindex][csiindex]->SetLineWidth(2);
		  //
		  legend_SubMode[ssdindex][csiindex]->Draw("SAME");

		  cans.cd(5);
		  h2_L1L2_DEE[ssdindex][csiindex].Draw("COL");
		  cans.cd(6);
		  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F_PID[ssdindex][csiindex].Draw("COL");
		  cans.cd(7);
		  h2_L1L2_DEE_Discard[ssdindex][csiindex].Draw("COL");

		  cans.cd(8);
		  // candimulti_sum
		  h1_candimulti_sum[ssdindex]->Draw();
		  h1_candimulti_sum[ssdindex]->SetLineColor(kRed);
		  h1_candimulti_sum[ssdindex]->SetLineWidth(2);
		  h1_candimulti_sum[ssdindex]->GetXaxis()->SetNdivisions(106);
		  h1_candimulti_sum[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti_merged[ssdindex]->GetMaximum());
		  // candimulti_merged
		  h1_candimulti_merged[ssdindex]->Draw("SAME");
		  h1_candimulti_merged[ssdindex]->SetLineColor(kMagenta);
		  h1_candimulti_merged[ssdindex]->SetLineWidth(2);
		  // candimulti
		  h1_candimulti[ssdindex][csiindex]->Draw("SAME");
		  h1_candimulti[ssdindex][csiindex]->SetLineColor(kBlue);
		  h1_candimulti[ssdindex][csiindex]->SetLineWidth(2);
		  // latex_ratio
		  latex_ratio[ssdindex]->Draw();
		  //
		  legend_CandiMulti[ssdindex][csiindex]->Draw("SAME");

		  gPad->Modified();
		  gPad->Update();

		  cans.Print(pathPDFOut.c_str());
	  }
  }
  c_end.Print(pathPDFOutEnd.c_str());
}
