////////////////////////////////////////////////////////////////////////////////
//  此脚本用于寻找硅条望远镜径迹重建的算法, 对 globalmulti = 1, 2, 3, 4, 6 共
//  5 中情况进行解码
//
//  gfh, 2021-08-09
////////////////////////////////////////////////////////////////////////////////
#include "../include/L1L2TrackFinding.h"

//______________________________________________________________________________
// 开关: 是否剔除 CsI 的 gap 对应的条
Bool_t IsStripsOnCsIGap(Int_t stripb, Int_t stripf)
{
  Bool_t index = kFALSE;
	if ((stripb==0) || (stripb==4) || (stripb==5) || (stripb==10) || (stripb==11) || (stripb==15) ||
      (stripf==0) || (stripf==4) || (stripf==5) || (stripf==10) || (stripf==11) || (stripf==15))
	{
		index = kTRUE;
	}
	return index;
}


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//                        fGlobalMulti = 1
//                    ------------------------
L1L2_DecodefGlobalMulti1::L1L2_DecodefGlobalMulti1()
{
	fFirstRun = 150;
	fLastRun  = 160;
}

L1L2_DecodefGlobalMulti1::L1L2_DecodefGlobalMulti1(Int_t firstrun, Int_t lastrun)
{
  fFirstRun = firstrun;
  fLastRun  = lastrun;

  trackreconstruct = new CSHINETrackReconstruction(fFirstRun, fLastRun);

  std::string pathBananaCut(Form("%sStraighteningData/L1L2DEECuts.root",PATHROOTFILESFOLDER));
  TFile* myfile = TFile::Open(pathBananaCut.c_str());
  // 读取 bananacut
  for (Int_t numtel=0; numtel<NUM_SSD; numtel++) {
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

L1L2_DecodefGlobalMulti1::~L1L2_DecodefGlobalMulti1()
{}
//______________________________________________________________________________



//______________________________________________________________________________
void L1L2_DecodefGlobalMulti1::L1L2_GlobalMulti1_Decode()
{
	const Int_t globalmulti = 1;
	const char* mode = "111";

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];

	Int_t bin_index[globalmulti];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode.pdf",PATHFIGURESFOLDER,globalmulti));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode.pdf[",PATHFIGURESFOLDER,globalmulti));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode.pdf]",PATHFIGURESFOLDER,globalmulti));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat",ssdindex+1), Form("SSD%d_ModeStat",ssdindex+1), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected",ssdindex+1), Form("SSD%d_SubmodeCorrected",ssdindex+1), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti",ssdindex+1), Form("SSD%d_CandiMulti",ssdindex+1), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2",ssdindex+1),Form("SSD%d_L1L2",ssdindex+1),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts",ssdindex+1),Form("SSD%d_L1L2_Cuts",ssdindex+1),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard",ssdindex+1),Form("SSD%d_L1L2_Discard",ssdindex+1),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  //trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
        for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					{
						if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
						{
							h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

		          // 填充 mode statistic
		          bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
								                                                                      trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
						  h1_modestat[ssdindex]->Fill(bin_index[itrack]);

		          if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
		              trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
		          {
		            candimulti[ssdindex]++;
		          }
						}
					}
        }
        //____________________________________________________________________
        //           以下部分为当前 L1L2-globalomulti1 的径迹重建策略！！！
        //          -------------------------------------------------------
				if (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]])
				{
					if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
					{
						if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
					      trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0]))
						{
							candimulti_corrected[ssdindex]++;
							h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0]);
						}
		        else {
							h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0]);
						}
						//______________________________________________________________________
		        // 填充每一个 SSD 的 candidate multi
		        h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
						//
						if (candimulti_corrected[ssdindex] > 0) {
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.25 * EL2Range[ssdindex];
    y1 = 0.90 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);

		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo



//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//                        fGlobalMulti = 2
//                    ------------------------
L1L2_DecodefGlobalMulti2::L1L2_DecodefGlobalMulti2()
{
	fFirstRun = 150;
	fLastRun  = 160;
}

L1L2_DecodefGlobalMulti2::L1L2_DecodefGlobalMulti2(Int_t firstrun, Int_t lastrun)
{
  fFirstRun = firstrun;
  fLastRun  = lastrun;

  trackreconstruct = new CSHINETrackReconstruction(fFirstRun, fLastRun);

  std::string pathBananaCut(Form("%sStraighteningData/L1L2DEECuts.root",PATHROOTFILESFOLDER));
  TFile* myfile = TFile::Open(pathBananaCut.c_str());
  // 读取 bananacut
  for (Int_t numtel=0; numtel<NUM_SSD; numtel++) {
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

L1L2_DecodefGlobalMulti2::~L1L2_DecodefGlobalMulti2()
{}
//______________________________________________________________________________


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti2::L1L2_GlobalMulti2_Decode_001()
{
	const Int_t globalmulti = 2;
	const char* mode = "001";
  std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL1S_Sum01;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  printf("Processing mode g%d-%s ...\n", globalmulti, mode);
  //trackreconstruct->fChainTrackTree->SetEntries(1000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

		            // 填充 mode statistic
		            bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
								                                                                        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
						    h1_modestat[ssdindex]->Fill(bin_index[itrack]);

		            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
		                trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
		            {
		              candimulti[ssdindex]++;
		            }
							}
					  }
          }
          //____________________________________________________________________
          //           以下部分为当前 L1L2-globalomulti2-001 的径迹重建策略！！！
          //          -------------------------------------------------------
					EL1S_Sum01 = L1L2_L1SEMeV[ssdindex][0] + L1L2_L1SEMeV[ssdindex][1];
					//
					if ( (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]) &&
					     (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0])) &&
				       (TMath::Abs(L1L2_L1SStripNum[ssdindex][0]-L1L2_L1SStripNum[ssdindex][1])==1) &&
							 (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][0],EL1S_Sum01)) )
					{
						if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
						{
							candimulti_corrected[ssdindex]++;
							candimulti_sharing[ssdindex]++;
							h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][0],EL1S_Sum01);
						}
					}

					for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
						  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
					        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
								{
									candimulti_corrected[ssdindex]++;
								  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									break;
								}
						  }
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
								{
								  h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					//
					if (candimulti_sharing[ssdindex] > 0) {
						Sharing_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex]    = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
		MHits_Ratio[ssdindex]   = (Double_t) MHits_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
    Sharing_Ratio[ssdindex] = (Double_t) Sharing_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
  }

	// 定义 latex
  Double_t x1,y1,x2,y2;
  TLatex* latex_ratio[NUM_SSD];
  TLatex* latex_decoded[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * EL2Range[ssdindex];
    y1 = 0.8 * EL1Range[ssdindex];
    x2 = 0.2 * EL2Range[ssdindex];
    y2 = 0.5 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.2f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
    latex_decoded[ssdindex] = new TLatex(x2, y2, Form("r_{sh}=%.4f%%, r_{mh}=%.4f%%",Sharing_Ratio[ssdindex],MHits_Ratio[ssdindex]));
    latex_decoded[ssdindex]->SetTextSize(0.05);
    latex_decoded[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
    legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");
		latex_decoded[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
    h1_SubmodeSharing[ssdindex]->SetLineColor(kGreen);
    h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");

		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti2::L1L2_GlobalMulti2_Decode_010()
{
	const Int_t globalmulti = 2;
	const char* mode = "010";
  std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2F_Sum;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  printf("Processing mode g%d-%s ...\n", globalmulti, mode);
  //trackreconstruct->fChainTrackTree->SetEntries(100000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

		            // 填充 mode statistic
		            bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
								                                                                        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
						    h1_modestat[ssdindex]->Fill(bin_index[itrack]);

		            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
		                trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
		            {
		              candimulti[ssdindex]++;
		            }
							}
					  }
          }
          //____________________________________________________________________
          //           以下部分为当前 L1L2-globalomulti2-010 的径迹重建策略！！！
          //          -------------------------------------------------------
          EL2F_Sum = L1L2_L2FEMeV[ssdindex][0]+L1L2_L2FEMeV[ssdindex][1];
					//
					if ( ((L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]) ||
					      (L1L2_L2FTime[ssdindex][1]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][1]] && L1L2_L2FTime[ssdindex][1]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][1]])) &&
								(TMath::Abs(L1L2_L2FStripNum[ssdindex][0]-L1L2_L2FStripNum[ssdindex][1])==1) &&
								(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],EL2F_Sum)) &&
								(trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],EL2F_Sum,L1L2_L1SEMeV_Corrected[ssdindex][0])) )
          {
						if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
						{
							candimulti_corrected[ssdindex]++;
							candimulti_sharing[ssdindex]++;
							h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(EL2F_Sum,L1L2_L1SEMeV_Corrected[ssdindex][0]);
						}
					}
					else {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
									{
										candimulti_corrected[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
							  }
							}
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
								{
								  h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						Sharing_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex]    = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
		MHits_Ratio[ssdindex]   = (Double_t) MHits_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
    Sharing_Ratio[ssdindex] = (Double_t) Sharing_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
  }

	// 定义 latex
  Double_t x1,y1,x2,y2;
  TLatex* latex_ratio[NUM_SSD];
  TLatex* latex_decoded[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * EL2Range[ssdindex];
    y1 = 0.8 * EL1Range[ssdindex];
    x2 = 0.2 * EL2Range[ssdindex];
    y2 = 0.5 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.2f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
    latex_decoded[ssdindex] = new TLatex(x2, y2, Form("r_{sh}=%.4f%%, r_{mh}=%.4f%%",Sharing_Ratio[ssdindex],MHits_Ratio[ssdindex]));
    latex_decoded[ssdindex]->SetTextSize(0.05);
    latex_decoded[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
    legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");
		latex_decoded[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kGreen);
    h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");

		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti2::L1L2_GlobalMulti2_Decode_100()
{
	const Int_t globalmulti = 2;
	const char* mode = "100";
  std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2B_Sum;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  printf("Processing mode g%d-%s ...\n", globalmulti, mode);
  //trackreconstruct->fChainTrackTree->SetEntries(100000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

							 // 填充 mode statistic
							 bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
																																											 trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
							 h1_modestat[ssdindex]->Fill(bin_index[itrack]);

							 if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
									 trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							 {
								 candimulti[ssdindex]++;
							 }
						 }
					  }
          }
          //____________________________________________________________________
          //           以下部分为当前 L1L2-globalomulti2-100 的径迹重建策略！！！
          //          -------------------------------------------------------
          EL2B_Sum = L1L2_L2BEMeV[ssdindex][0]+L1L2_L2BEMeV[ssdindex][1];
					//
					if ( (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]) &&
							 (TMath::Abs(L1L2_L2BStripNum[ssdindex][0]-L1L2_L2BStripNum[ssdindex][1])==1) &&
							 (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum,L1L2_L2FEMeV[ssdindex][0])) &&
							 (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0])) )
          {
						if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
						{
							candimulti_corrected[ssdindex]++;
							candimulti_sharing[ssdindex]++;
							h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0]);
						}
					}
					else {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
									{
										candimulti_corrected[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
							  }
							}
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
								{
								  h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						Sharing_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex]    = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
		MHits_Ratio[ssdindex]   = (Double_t) MHits_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
    Sharing_Ratio[ssdindex] = (Double_t) Sharing_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
  }

	// 定义 latex
  Double_t x1,y1,x2,y2;
  TLatex* latex_ratio[NUM_SSD];
  TLatex* latex_decoded[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * EL2Range[ssdindex];
    y1 = 0.8 * EL1Range[ssdindex];
    x2 = 0.2 * EL2Range[ssdindex];
    y2 = 0.5 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.2f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
    latex_decoded[ssdindex] = new TLatex(x2, y2, Form("r_{sh}=%.4f%%, r_{mh}=%.4f%%",Sharing_Ratio[ssdindex],MHits_Ratio[ssdindex]));
    latex_decoded[ssdindex]->SetTextSize(0.05);
    latex_decoded[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
    legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");
		latex_decoded[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kGreen);
    h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");

		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti2::L1L2_GlobalMulti2_Decode_101()
{
	const Int_t globalmulti = 2;
	const char* mode = "101";
  std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_2hit[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2B_Sum01;
	Double_t EL1S_Sum01;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_Submode2Hit[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode2Hit[ssdindex] = new TH1I(Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  printf("Processing mode g%d-%s ...\n", globalmulti, mode);
  //trackreconstruct->fChainTrackTree->SetEntries(100000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;
			candimulti_2hit[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

		            // 填充 mode statistic
		            bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
								                                                                        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
						    h1_modestat[ssdindex]->Fill(bin_index[itrack]);

		            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
		                trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
		            {
		              candimulti[ssdindex]++;
		            }
							}
					  }
          }
          //____________________________________________________________________
          //           以下部分为当前 L1L2-globalomulti2-101 的径迹重建策略！！！
          //          -------------------------------------------------------
          EL2B_Sum01 = L1L2_L2BEMeV[ssdindex][0]+L1L2_L2BEMeV[ssdindex][1];
					EL1S_Sum01 = L1L2_L1SEMeV[ssdindex][0]+L1L2_L1SEMeV[ssdindex][1];
					//
					// 考虑 L2F 双击
					if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum01,L1L2_L2FEMeV[ssdindex][0])) &&
				       (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][0]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
					     (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][1]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][1])) &&
				       (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]) &&
						   (L1L2_L2FTime[ssdindex][1]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][1]] && L1L2_L2FTime[ssdindex][1]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][1]]))
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++)
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_2hit[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
							}
						}
					}
					// 考虑 L2B sharing
					else if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][0]-L1L2_L2BStripNum[ssdindex][1])==1) &&
				            (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum01,L1L2_L2FEMeV[ssdindex][0])) &&
						        (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]) )
					{
            if ( (TMath::Abs(L1L2_L1SStripNum[ssdindex][0]-L1L2_L1SStripNum[ssdindex][1])==1) &&
							   (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][0],EL1S_Sum01)) )
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][0],EL1S_Sum01);
							}
						}
						else
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++)
							{
								if (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
									{
										candimulti_corrected[ssdindex]++;
										candimulti_sharing[ssdindex]++;
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
								}
							}
						}
					}
					else {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
									{
										candimulti_corrected[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
							  }
							}
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
									h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					if (candimulti_2hit[ssdindex] > 0) {
						MHits_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_Submode2Hit[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						Sharing_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex]    = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
		MHits_Ratio[ssdindex]   = (Double_t) MHits_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
    Sharing_Ratio[ssdindex] = (Double_t) Sharing_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
  }

	// 定义 latex
  Double_t x1,y1,x2,y2;
  TLatex* latex_ratio[NUM_SSD];
  TLatex* latex_decoded[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * EL2Range[ssdindex];
    y1 = 0.8 * EL1Range[ssdindex];
    x2 = 0.2 * EL2Range[ssdindex];
    y2 = 0.5 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.2f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
    latex_decoded[ssdindex] = new TLatex(x2, y2, Form("r_{sh}=%.4f%%, r_{mh}=%.4f%%",Sharing_Ratio[ssdindex],MHits_Ratio[ssdindex]));
    latex_decoded[ssdindex]->SetTextSize(0.05);
    latex_decoded[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
    legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode2Hit[ssdindex],"2-hit","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");
		latex_decoded[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_Submode2Hit[ssdindex]->Draw("SAME");
		h1_Submode2Hit[ssdindex]->SetLineColor(kGreen);
    h1_Submode2Hit[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kMagenta);
    h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");

		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo



//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//                        fGlobalMulti = 3
//                    ------------------------
L1L2_DecodefGlobalMulti3::L1L2_DecodefGlobalMulti3()
{
	fFirstRun = 150;
	fLastRun  = 160;
}

L1L2_DecodefGlobalMulti3::L1L2_DecodefGlobalMulti3(Int_t firstrun, Int_t lastrun)
{
  fFirstRun = firstrun;
  fLastRun  = lastrun;

  trackreconstruct = new CSHINETrackReconstruction(fFirstRun, fLastRun);

  std::string pathBananaCut(Form("%sStraighteningData/L1L2DEECuts.root",PATHROOTFILESFOLDER));
  TFile* myfile = TFile::Open(pathBananaCut.c_str());
  // 读取 bananacut
  for (Int_t numtel=0; numtel<NUM_SSD; numtel++) {
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

L1L2_DecodefGlobalMulti3::~L1L2_DecodefGlobalMulti3()
{}
//______________________________________________________________________________


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti3::L1L2_GlobalMulti3_Decode_002()
{
	const Int_t globalmulti = 3;
	const char* mode = "002";
  std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_2hit[NUM_SSD];

	Int_t bin_index[globalmulti];

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_Submode2Hit[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode2Hit[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  printf("Processing mode g%d-%s ...\n", globalmulti, mode);
  //trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;
			candimulti_2hit[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

		            // 填充 mode statistic
		            bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
								                                                                        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
						    h1_modestat[ssdindex]->Fill(bin_index[itrack]);

		            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
		                trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
		            {
		              candimulti[ssdindex]++;
		            }
							}
					  }
          }
          //____________________________________________________________________
          //           以下部分为当前 L1L2-globalomulti3-002 的径迹重建策略！！！
          //          -------------------------------------------------------
					for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
						  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
					        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
									candimulti_corrected[ssdindex]++;
								  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									break;
								}
						  }
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
								  h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					if (candimulti_2hit[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_Submode2Hit[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

	// 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
    legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode2Hit[ssdindex],"2-hit","l");
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.25 * EL2Range[ssdindex];
    y1 = 0.90 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_Submode2Hit[ssdindex]->Draw("SAME");
		h1_Submode2Hit[ssdindex]->SetLineColor(kGreen);
    h1_Submode2Hit[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");

		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti3::L1L2_GlobalMulti3_Decode_020()
{
	const Int_t globalmulti = 3;
	const char* mode = "020";
  std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2F_Sum01;
	Double_t EL2F_Sum12;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  printf("Processing mode g%d-%s ...\n", globalmulti, mode);
  //trackreconstruct->fChainTrackTree->SetEntries(1000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

		            // 填充 mode statistic
		            bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
								                                                                        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
						    h1_modestat[ssdindex]->Fill(bin_index[itrack]);

		            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
		                trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
		            {
		              candimulti[ssdindex]++;
		            }
							}
					  }
          }
          //____________________________________________________________________
          //           以下部分为当前 L1L2-globalomulti3-020 的径迹重建策略！！！
          //          -------------------------------------------------------
					EL2F_Sum01 = L1L2_L2FEMeV[ssdindex][0]+L1L2_L2FEMeV[ssdindex][1];
					EL2F_Sum12 = L1L2_L2FEMeV[ssdindex][1]+L1L2_L2FEMeV[ssdindex][2];
					//
					if ( ((L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]) ||
					      (L1L2_L2FTime[ssdindex][1]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][1]] && L1L2_L2FTime[ssdindex][1]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][1]])) &&
							 (TMath::Abs(L1L2_L2FStripNum[ssdindex][0]-L1L2_L2FStripNum[ssdindex][1])==1) &&
							 (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],EL2F_Sum01)) &&
							 (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],EL2F_Sum01,L1L2_L1SEMeV_Corrected[ssdindex][0])) )
          {
						if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
						{
							candimulti_corrected[ssdindex]++;
							candimulti_sharing[ssdindex]++;
							h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(EL2F_Sum01,L1L2_L1SEMeV_Corrected[ssdindex][0]);
						}
					}
					else if ( ((L1L2_L2FTime[ssdindex][1]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][1]] && L1L2_L2FTime[ssdindex][1]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][1]]) ||
					           (L1L2_L2FTime[ssdindex][2]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][2]] && L1L2_L2FTime[ssdindex][2]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][2]])) &&
							    	(TMath::Abs(L1L2_L2FStripNum[ssdindex][1]-L1L2_L2FStripNum[ssdindex][2])==1) &&
							    	(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][1],EL2F_Sum12)) &&
								    (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],EL2F_Sum12,L1L2_L1SEMeV_Corrected[ssdindex][1])) )
          {
						if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][1], L1L2_L2FStripNum[ssdindex][1]))
						{
							candimulti_corrected[ssdindex]++;
							candimulti_sharing[ssdindex]++;
							h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(EL2F_Sum12,L1L2_L1SEMeV_Corrected[ssdindex][1]);
						}
					}
					else {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
							  }
							}
						}
					}

					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
									h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						Sharing_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex]    = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
		MHits_Ratio[ssdindex]   = (Double_t) MHits_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
    Sharing_Ratio[ssdindex] = (Double_t) Sharing_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
  }

	// 定义 latex
  Double_t x1,y1,x2,y2;
  TLatex* latex_ratio[NUM_SSD];
  TLatex* latex_decoded[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * EL2Range[ssdindex];
    y1 = 0.8 * EL1Range[ssdindex];
    x2 = 0.2 * EL2Range[ssdindex];
    y2 = 0.5 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.2f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
    latex_decoded[ssdindex] = new TLatex(x2, y2, Form("r_{sh}=%.4f%%, r_{mh}=%.4f%%",Sharing_Ratio[ssdindex],MHits_Ratio[ssdindex]));
    latex_decoded[ssdindex]->SetTextSize(0.05);
    latex_decoded[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
    legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");
		latex_decoded[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kGreen);
    h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");

		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti3::L1L2_GlobalMulti3_Decode_101()
{
	const Int_t globalmulti = 3;
	const char* mode = "101";
  std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_2hit[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2B_Sum01;
	Double_t EL2B_Sum02;
	Double_t EL2B_Sum12;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_Submode2Hit[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode2Hit[ssdindex] = new TH1I(Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  printf("Processing mode g%d-%s ...\n", globalmulti, mode);
  //trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    // timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;
			candimulti_2hit[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

		            // 填充 mode statistic
		            bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
								                                                                        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
						    h1_modestat[ssdindex]->Fill(bin_index[itrack]);

		            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
		                trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
		            {
		              candimulti[ssdindex]++;
		            }
							}
					  }
          }
          //____________________________________________________________________
          //           以下部分为当前 L1L2-globalomulti3-101 的径迹重建策略！！！
          //          -------------------------------------------------------
					EL2B_Sum01 = L1L2_L2BEMeV[ssdindex][0]+L1L2_L2BEMeV[ssdindex][1];
					EL2B_Sum02 = L1L2_L2BEMeV[ssdindex][0]+L1L2_L2BEMeV[ssdindex][2];
					EL2B_Sum12 = L1L2_L2BEMeV[ssdindex][1]+L1L2_L2BEMeV[ssdindex][2];
					// 考虑 L2F 双击
					if ( ((L1L2_L2BEMeV[ssdindex][0]!=L1L2_L2BEMeV[ssdindex][2]) && (L1L2_L1SEMeV[ssdindex][0]!=L1L2_L1SEMeV[ssdindex][2])) &&
				       (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum02,L1L2_L2FEMeV[ssdindex][0])) &&
							 (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][0]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
		 					 (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][2]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][2])) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (itrack!=1) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
							 		if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
						    	{
										candimulti_corrected[ssdindex]++;
										candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
						}
					}
					// 考虑 L2B sharing
					else if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][0]-L1L2_L2BStripNum[ssdindex][2])==1) &&
					          (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum02,L1L2_L2FEMeV[ssdindex][0])))
	        {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (itrack==0 || itrack==2) {
								if (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_sharing[ssdindex]++;
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
								}
							}
						}
					}
					// 考虑 3 条候选径迹无关联
					else {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
							  }
							}
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
								  h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					if (candimulti_2hit[ssdindex] > 0) {
						MHits_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_Submode2Hit[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						Sharing_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex]    = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
		MHits_Ratio[ssdindex]   = (Double_t) MHits_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
    Sharing_Ratio[ssdindex] = (Double_t) Sharing_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
  }

	// 定义 latex
  Double_t x1,y1,x2,y2;
  TLatex* latex_ratio[NUM_SSD];
  TLatex* latex_decoded[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * EL2Range[ssdindex];
    y1 = 0.8 * EL1Range[ssdindex];
    x2 = 0.2 * EL2Range[ssdindex];
    y2 = 0.5 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.2f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
    latex_decoded[ssdindex] = new TLatex(x2, y2, Form("r_{sh}=%.4f%%, r_{mh}=%.4f%%",Sharing_Ratio[ssdindex],MHits_Ratio[ssdindex]));
    latex_decoded[ssdindex]->SetTextSize(0.05);
    latex_decoded[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
    legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode2Hit[ssdindex],"2-hit","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");
		latex_decoded[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_Submode2Hit[ssdindex]->Draw("SAME");
		h1_Submode2Hit[ssdindex]->SetLineColor(kGreen);
    h1_Submode2Hit[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kMagenta);
    h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");

		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti3::L1L2_GlobalMulti3_Decode_102()
{
	const Int_t globalmulti = 3;
	const char* mode = "102";
  std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_2hit[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2B_Sum01;
	Double_t EL2B_Sum02;
	Double_t EL2B_Sum12;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_Submode2Hit[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode2Hit[ssdindex] = new TH1I(Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  printf("Processing mode g%d-%s ...\n", globalmulti, mode);
  //trackreconstruct->fChainTrackTree->SetEntries(1000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;
			candimulti_2hit[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

		            // 填充 mode statistic
		            bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
								                                                                        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
						    h1_modestat[ssdindex]->Fill(bin_index[itrack]);

		            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
		                trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
		            {
		              candimulti[ssdindex]++;
		            }
							}
					  }
          }
          //____________________________________________________________________
          //           以下部分为当前 L1L2-globalomulti3-102 的径迹重建策略！！！
          //          -------------------------------------------------------
					EL2B_Sum01 = L1L2_L2BEMeV[ssdindex][0]+L1L2_L2BEMeV[ssdindex][1];
					EL2B_Sum02 = L1L2_L2BEMeV[ssdindex][0]+L1L2_L2BEMeV[ssdindex][2];
					EL2B_Sum12 = L1L2_L2BEMeV[ssdindex][1]+L1L2_L2BEMeV[ssdindex][2];
					// 考虑 L2F 双击
					if ( ((L1L2_L2BEMeV[ssdindex][0]!=L1L2_L2BEMeV[ssdindex][1])) &&
				       (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum01,L1L2_L2FEMeV[ssdindex][0])) &&
						   (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][0]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
						   (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][1]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][1])) )
					{
            for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (itrack!=2) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
						}
					}
					// 考虑 L2F 双击
					else if ( ((L1L2_L2BEMeV[ssdindex][0]!=L1L2_L2BEMeV[ssdindex][2])) &&
				            (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum02,L1L2_L2FEMeV[ssdindex][0])) &&
										(trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][0]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
		 						    (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][2]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][2])) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (itrack!=1) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
						}
					}
					// 考虑 L2F 双击
					else if ( ((L1L2_L2BEMeV[ssdindex][1]!=L1L2_L2BEMeV[ssdindex][2])) &&
				            (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum12,L1L2_L2FEMeV[ssdindex][1])) &&
										(trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][1]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][1])) &&
		 						    (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][2]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][2])) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (itrack!=0) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_2hit[ssdindex]++;
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
						}
					}
					// 考虑 L2B sharing
					else if ((TMath::Abs(L1L2_L2BStripNum[ssdindex][0]-L1L2_L2BStripNum[ssdindex][1])==1) && (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum01,L1L2_L2FEMeV[ssdindex][0])))
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ((itrack!=2) && (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_sharing[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
								}
							}
						}
					}
					// 考虑 L2B sharing
					else if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][1]-L1L2_L2BStripNum[ssdindex][2])==1) && (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum12,L1L2_L2FEMeV[ssdindex][1])) &&
					          (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][1],L1L2_L1SEMeV_Corrected[ssdindex][1])) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ((itrack!=0) && (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][1], L1L2_L2FStripNum[ssdindex][1]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_sharing[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][1],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
								}
							}
						}
					}
					// 考虑 3 条候选径迹无关联
					else {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
							  }
							}
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
								  h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					if (candimulti_2hit[ssdindex] > 0) {
						MHits_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_Submode2Hit[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						Sharing_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex]    = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
		MHits_Ratio[ssdindex]   = (Double_t) MHits_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
    Sharing_Ratio[ssdindex] = (Double_t) Sharing_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
  }

	// 定义 latex
  Double_t x1,y1,x2,y2;
  TLatex* latex_ratio[NUM_SSD];
  TLatex* latex_decoded[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * EL2Range[ssdindex];
    y1 = 0.8 * EL1Range[ssdindex];
    x2 = 0.2 * EL2Range[ssdindex];
    y2 = 0.5 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.2f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
    latex_decoded[ssdindex] = new TLatex(x2, y2, Form("r_{sh}=%.4f%%, r_{mh}=%.4f%%",Sharing_Ratio[ssdindex],MHits_Ratio[ssdindex]));
    latex_decoded[ssdindex]->SetTextSize(0.05);
    latex_decoded[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
    legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode2Hit[ssdindex],"2-hit","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");
		latex_decoded[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_Submode2Hit[ssdindex]->Draw("SAME");
		h1_Submode2Hit[ssdindex]->SetLineColor(kGreen);
    h1_Submode2Hit[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kMagenta);
    h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");

		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti3::L1L2_GlobalMulti3_Decode_200()
{
	const Int_t globalmulti = 3;
	const char* mode = "200";
  std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2B_Sum01;
	Double_t EL2B_Sum02;
	Double_t EL2B_Sum12;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  printf("Processing mode g%d-%s ...\n", globalmulti, mode);
  //trackreconstruct->fChainTrackTree->SetEntries(1000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

		            // 填充 mode statistic
		            bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
								                                                                        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
						    h1_modestat[ssdindex]->Fill(bin_index[itrack]);

		            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
		                trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
		            {
		              candimulti[ssdindex]++;
		            }
							}
					  }
          }
          //____________________________________________________________________
          //           以下部分为当前 L1L2-globalomulti3-200 的径迹重建策略！！！
          //          -------------------------------------------------------
					EL2B_Sum01 = L1L2_L2BEMeV[ssdindex][0]+L1L2_L2BEMeV[ssdindex][1];
					EL2B_Sum02 = L1L2_L2BEMeV[ssdindex][0]+L1L2_L2BEMeV[ssdindex][2];
					EL2B_Sum12 = L1L2_L2BEMeV[ssdindex][1]+L1L2_L2BEMeV[ssdindex][2];
					//
					if ( (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]) &&
						   (TMath::Abs(L1L2_L2BStripNum[ssdindex][0]-L1L2_L2BStripNum[ssdindex][1])==1) &&
				       (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum01,L1L2_L2FEMeV[ssdindex][0])) &&
						   (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0])) )
					{
						if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
						{
							candimulti_corrected[ssdindex]++;
							candimulti_sharing[ssdindex]++;
							h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0]);
						}
					}
					else if ( (L1L2_L2FTime[ssdindex][1]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][1]] && L1L2_L2FTime[ssdindex][1]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][1]]) &&
						        (TMath::Abs(L1L2_L2BStripNum[ssdindex][1]-L1L2_L2BStripNum[ssdindex][2])==1) &&
				            (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum12,L1L2_L2FEMeV[ssdindex][1])) &&
						        (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][1],L1L2_L1SEMeV_Corrected[ssdindex][1])) )
					{
						if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][1], L1L2_L2FStripNum[ssdindex][1]))
						{
							candimulti_corrected[ssdindex]++;
							candimulti_sharing[ssdindex]++;
							h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][1],L1L2_L1SEMeV_Corrected[ssdindex][1]);
						}
					}
					else {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
							  }
							}
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
									h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						Sharing_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex]    = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
		MHits_Ratio[ssdindex]   = (Double_t) MHits_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
    Sharing_Ratio[ssdindex] = (Double_t) Sharing_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
  }

	// 定义 latex
  Double_t x1,y1,x2,y2;
  TLatex* latex_ratio[NUM_SSD];
  TLatex* latex_decoded[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * EL2Range[ssdindex];
    y1 = 0.8 * EL1Range[ssdindex];
    x2 = 0.2 * EL2Range[ssdindex];
    y2 = 0.5 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.2f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
    latex_decoded[ssdindex] = new TLatex(x2, y2, Form("r_{sh}=%.4f%%, r_{mh}=%.4f%%",Sharing_Ratio[ssdindex],MHits_Ratio[ssdindex]));
    latex_decoded[ssdindex]->SetTextSize(0.05);
    latex_decoded[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
    legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");
		latex_decoded[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kGreen);
    h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");

		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti3::L1L2_GlobalMulti3_Decode_201()
{
	const Int_t globalmulti = 3;
	const char* mode = "201";
  std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_2hit[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2B_Sum01;
	Double_t EL2B_Sum02;
	Double_t EL2B_Sum12;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_Submode2Hit[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode2Hit[ssdindex] = new TH1I(Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  printf("Processing mode g%d-%s ...\n", globalmulti, mode);
  //trackreconstruct->fChainTrackTree->SetEntries(1000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;
			candimulti_2hit[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

		            // 填充 mode statistic
		            bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
								                                                                        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
						    h1_modestat[ssdindex]->Fill(bin_index[itrack]);

		            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
		                trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
		            {
		              candimulti[ssdindex]++;
		            }
							}
					  }
          }
          //____________________________________________________________________
          //           以下部分为当前 L1L2-globalomulti3-201 的径迹重建策略！！！
          //          -------------------------------------------------------
					EL2B_Sum01 = L1L2_L2BEMeV[ssdindex][0]+L1L2_L2BEMeV[ssdindex][1];
					EL2B_Sum02 = L1L2_L2BEMeV[ssdindex][0]+L1L2_L2BEMeV[ssdindex][2];
					EL2B_Sum12 = L1L2_L2BEMeV[ssdindex][1]+L1L2_L2BEMeV[ssdindex][2];
					// 考虑 L2F 双击
					if ( ((L1L2_L1SEMeV[ssdindex][0]!=L1L2_L1SEMeV[ssdindex][1])) &&
				       (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum01,L1L2_L2FEMeV[ssdindex][0])) &&
						   (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][0]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
						   (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][1]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][1])) )
					{
            for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (itrack!=2) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
						}
					}
					// 考虑 L2F 双击
					else if ( ((L1L2_L1SEMeV[ssdindex][0]!=L1L2_L1SEMeV[ssdindex][2])) &&
				            (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum02,L1L2_L2FEMeV[ssdindex][0])) &&
										(trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][0]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
		 						    (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][2]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][2])) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (itrack!=1) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
						}
					}
					// 考虑 L2F 双击
					else if ( ((L1L2_L1SEMeV[ssdindex][1]!=L1L2_L1SEMeV[ssdindex][2])) &&
				            (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum12,L1L2_L2FEMeV[ssdindex][1])) &&
										(trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][1]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][1])) &&
		 						    (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][2]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][2])) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (itrack!=0) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
						}
					}
					// 考虑 L2B sharing
					else if ((TMath::Abs(L1L2_L2BStripNum[ssdindex][0]-L1L2_L2BStripNum[ssdindex][1])==1) && (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum01,L1L2_L2FEMeV[ssdindex][0])))
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ((itrack!=2) && (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_sharing[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
								}
							}
						}
					}
					// 考虑 L2B sharing
					else if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][1]-L1L2_L2BStripNum[ssdindex][2])==1) && (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum12,L1L2_L2FEMeV[ssdindex][1])) &&
					          (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][1],L1L2_L1SEMeV_Corrected[ssdindex][1])) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ((itrack!=0) && (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_sharing[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
								}
							}
						}
					}
					// 考虑 3 条候选径迹无关联
					else {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
							  }
							}
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
									h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					if (candimulti_2hit[ssdindex] > 0) {
						MHits_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_Submode2Hit[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						Sharing_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex]    = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
		MHits_Ratio[ssdindex]   = (Double_t) MHits_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
    Sharing_Ratio[ssdindex] = (Double_t) Sharing_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
  }

	// 定义 latex
  Double_t x1,y1,x2,y2;
  TLatex* latex_ratio[NUM_SSD];
  TLatex* latex_decoded[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * EL2Range[ssdindex];
    y1 = 0.8 * EL1Range[ssdindex];
    x2 = 0.2 * EL2Range[ssdindex];
    y2 = 0.5 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.2f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
    latex_decoded[ssdindex] = new TLatex(x2, y2, Form("r_{sh}=%.4f%%, r_{mh}=%.4f%%",Sharing_Ratio[ssdindex],MHits_Ratio[ssdindex]));
    latex_decoded[ssdindex]->SetTextSize(0.05);
    latex_decoded[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
    legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode2Hit[ssdindex],"2-hit","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");
		latex_decoded[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_Submode2Hit[ssdindex]->Draw("SAME");
		h1_Submode2Hit[ssdindex]->SetLineColor(kGreen);
    h1_Submode2Hit[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kMagenta);
    h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");

		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti3::L1L2_GlobalMulti3_Decode_202()
{
	const Int_t globalmulti = 3;
	const char* mode = "202";
  std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_3hit[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2B_Sum012;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_Submode3Hit[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode3Hit[ssdindex] = new TH1I(Form("SSD%d_Submode3Hit_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_Submode3Hit_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  printf("Processing mode g%d-%s ...\n", globalmulti, mode);
  //trackreconstruct->fChainTrackTree->SetEntries(1000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;
			candimulti_3hit[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

		            // 填充 mode statistic
		            bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
								                                                                        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
						    h1_modestat[ssdindex]->Fill(bin_index[itrack]);

		            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
		                trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
		            {
		              candimulti[ssdindex]++;
		            }
							}
					  }
          }
          //____________________________________________________________________
          //           以下部分为当前 L1L2-globalomulti3-202 的径迹重建策略！！！
          //          -------------------------------------------------------
					EL2B_Sum012 = L1L2_L2BEMeV[ssdindex][0]+L1L2_L2BEMeV[ssdindex][1]+L1L2_L2BEMeV[ssdindex][2];
					// 考虑 L2F 三击事件
					if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum012,L1L2_L2FEMeV[ssdindex][0]))
			    {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][itrack])) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_3hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
						}
					}
					// 考虑 3 条候选径迹无关联
					else {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
							  }
							}
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
								  h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					if (candimulti_3hit[ssdindex] > 0) {
					  MHits_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_Submode3Hit[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						Sharing_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex]    = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
		MHits_Ratio[ssdindex]   = (Double_t) MHits_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
    Sharing_Ratio[ssdindex] = (Double_t) Sharing_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
  }

	// 定义 latex
  Double_t x1,y1,x2,y2;
  TLatex* latex_ratio[NUM_SSD];
  TLatex* latex_decoded[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * EL2Range[ssdindex];
    y1 = 0.8 * EL1Range[ssdindex];
    x2 = 0.2 * EL2Range[ssdindex];
    y2 = 0.5 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.2f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
    latex_decoded[ssdindex] = new TLatex(x2, y2, Form("r_{sh}=%.4f%%, r_{mh}=%.4f%%",Sharing_Ratio[ssdindex],MHits_Ratio[ssdindex]));
    latex_decoded[ssdindex]->SetTextSize(0.05);
    latex_decoded[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
    legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode3Hit[ssdindex],"2-hit","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");
		latex_decoded[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_Submode3Hit[ssdindex]->Draw("SAME");
		h1_Submode3Hit[ssdindex]->SetLineColor(kGreen);
    h1_Submode3Hit[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kMagenta);
    h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");

		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo



//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//                        fGlobalMulti = 4
//                    ------------------------
L1L2_DecodefGlobalMulti4::L1L2_DecodefGlobalMulti4()
{
	fFirstRun = 150;
	fLastRun  = 160;
}

L1L2_DecodefGlobalMulti4::L1L2_DecodefGlobalMulti4(Int_t firstrun, Int_t lastrun)
{
  fFirstRun = firstrun;
  fLastRun  = lastrun;

  trackreconstruct = new CSHINETrackReconstruction(fFirstRun, fLastRun);

  std::string pathBananaCut(Form("%sStraighteningData/L1L2DEECuts.root",PATHROOTFILESFOLDER));
  TFile* myfile = TFile::Open(pathBananaCut.c_str());
  // 读取 bananacut
  for (Int_t numtel=0; numtel<NUM_SSD; numtel++) {
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

L1L2_DecodefGlobalMulti4::~L1L2_DecodefGlobalMulti4()
{}
//______________________________________________________________________________


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti4::L1L2_GlobalMulti4_Decode_011()
{
	const Int_t globalmulti = 4;
	const char* mode = "011";
  std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_2hit[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2F_Sum03;
	Double_t EL2F_Sum12;
	Double_t EL1S_Sum01;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_Submode2Hit[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode2Hit[ssdindex] = new TH1I(Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  printf("Processing mode g%d-%s ...\n", globalmulti, mode);
  //trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;
			candimulti_2hit[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

		            // 填充 mode statistic
		            bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
								                                                                        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
						    h1_modestat[ssdindex]->Fill(bin_index[itrack]);

		            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
		                trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
		            {
		              candimulti[ssdindex]++;
		            }
							}
					  }
          }
          //____________________________________________________________________
          //           以下部分为当前 L1L2-globalomulti4-011 的径迹重建策略！！！
          //          -------------------------------------------------------
					//  0)  000
					//  1)  001
					//  2)  010
					//  3)  011
					EL2F_Sum03 = L1L2_L2FEMeV[ssdindex][0] + L1L2_L2FEMeV[ssdindex][3];
					EL2F_Sum12 = L1L2_L2FEMeV[ssdindex][1] + L1L2_L2FEMeV[ssdindex][2];
					EL1S_Sum01 = L1L2_L1SEMeV[ssdindex][0] + L1L2_L1SEMeV[ssdindex][1];

					// 考虑 L2B 双击
					if ( trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],EL2F_Sum03) &&
				       trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0]) &&
						   trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][3],L1L2_L1SEMeV_Corrected[ssdindex][3]) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ((itrack!=1) && (itrack!=2)) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
						}
					}
					// 考虑 L2B 双击
					else if ( trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][1],EL2F_Sum12) &&
				            trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][1],L1L2_L1SEMeV_Corrected[ssdindex][1]) &&
						        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][2],L1L2_L1SEMeV_Corrected[ssdindex][2]) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ((itrack!=0) && (itrack!=3)) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
                  /*
									cout<<"ientry="<<ientry<<setw(12)<<"itrack="<<itrack<<setw(12)<<"StripL2B="<<L1L2_L2BStripNum[ssdindex][itrack]<<setw(12)<<"StripL2F="<<L1L2_L2FStripNum[ssdindex][itrack]<<setw(12)
									    <<"StripL1S="<<L1L2_L1SStripNum[ssdindex][itrack]<<setw(12)<<"EL2B="<<L1L2_L2BEMeV[ssdindex][itrack]<<setw(12)<<"EL2F="<<L1L2_L2FEMeV[ssdindex][itrack]<<setw(12)
											<<"EL1S="<<L1L2_L1SEMeV[ssdindex][itrack]<<setw(12)<<(trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]) ? "Y" : "N")
											<<endl;
								  */
								}
							}
						}
					}
					// 考虑 L2F sharing
					else if ( (TMath::Abs(L1L2_L2FStripNum[ssdindex][0]-L1L2_L2FStripNum[ssdindex][3])==1) &&
				            trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],EL2F_Sum03) &&
									  trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],EL2F_Sum03,L1L2_L1SEMeV_Corrected[ssdindex][0]) )
					{
						if ( (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]) ||
					       (L1L2_L2FTime[ssdindex][3]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][3]] && L1L2_L2FTime[ssdindex][3]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][3]]) )
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(EL2F_Sum03,L1L2_L1SEMeV_Corrected[ssdindex][0]);
							}
						}
					}
					// 考虑 L2F sharing
					else if ( (TMath::Abs(L1L2_L2FStripNum[ssdindex][0]-L1L2_L2FStripNum[ssdindex][3])==1) &&
				            trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][3],EL2F_Sum03) &&
									  trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],EL2F_Sum03,L1L2_L1SEMeV_Corrected[ssdindex][3]) )
					{
						if ( (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]) ||
					       (L1L2_L2FTime[ssdindex][3]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][3]] && L1L2_L2FTime[ssdindex][3]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][3]]) )
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][3], L1L2_L2FStripNum[ssdindex][3]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(EL2F_Sum03,L1L2_L1SEMeV_Corrected[ssdindex][3]);
							}
						}
					}
					// 考虑 L1S sharing
					else if ( trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
						        (TMath::Abs(L1L2_L1SStripNum[ssdindex][0]-L1L2_L1SStripNum[ssdindex][1])==1) &&
				            trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][0],EL1S_Sum01) )
					{
						if (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]])
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][0],EL1S_Sum01);
							}
						}
					}
          else if ( trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][3],L1L2_L2FEMeV[ssdindex][3]) &&
						        (TMath::Abs(L1L2_L1SStripNum[ssdindex][0]-L1L2_L1SStripNum[ssdindex][1])==1) &&
				            trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][3],EL1S_Sum01) )
					{
						if (L1L2_L2FTime[ssdindex][3]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][3]] && L1L2_L2FTime[ssdindex][3]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][3]])
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][3], L1L2_L2FStripNum[ssdindex][3]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][3],EL1S_Sum01);
							}
						}
					}
					// 考虑只有一个粒子的情况
          else {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
							  }
							}
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
									h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
                /*
								cout<<"ientry="<<ientry<<setw(12)<<"itrack="<<itrack<<setw(12)<<"StripL2B="<<L1L2_L2BStripNum[ssdindex][itrack]<<setw(12)<<"StripL2F="<<L1L2_L2FStripNum[ssdindex][itrack]<<setw(12)
										<<"StripL1S="<<L1L2_L1SStripNum[ssdindex][itrack]<<setw(12)<<"EL2B="<<L1L2_L2BEMeV[ssdindex][itrack]<<setw(12)<<"EL2F="<<L1L2_L2FEMeV[ssdindex][itrack]<<setw(12)
										<<"EL1S="<<L1L2_L1SEMeV[ssdindex][itrack]<<setw(12)<<(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) ? "Y" : "N")<<setw(12)
										<<(trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]) ? "Y" : "N")<<setw(12)
										<<( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
										     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack])) ? 1 : 0 )
										<<endl;
                */
							}
						}
					}
					//
					if (candimulti_2hit[ssdindex] > 0) {
						MHits_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_Submode2Hit[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						Sharing_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex]    = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
		MHits_Ratio[ssdindex]   = (Double_t) MHits_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
    Sharing_Ratio[ssdindex] = (Double_t) Sharing_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
  }

	// 定义 latex
  Double_t x1,y1,x2,y2;
  TLatex* latex_ratio[NUM_SSD];
  TLatex* latex_decoded[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * EL2Range[ssdindex];
    y1 = 0.8 * EL1Range[ssdindex];
    x2 = 0.2 * EL2Range[ssdindex];
    y2 = 0.5 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.2f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
    latex_decoded[ssdindex] = new TLatex(x2, y2, Form("r_{sh}=%.4f%%, r_{mh}=%.4f%%",Sharing_Ratio[ssdindex],MHits_Ratio[ssdindex]));
    latex_decoded[ssdindex]->SetTextSize(0.05);
    latex_decoded[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
    legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode2Hit[ssdindex],"2-hit","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");
		latex_decoded[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_Submode2Hit[ssdindex]->Draw("SAME");
		h1_Submode2Hit[ssdindex]->SetLineColor(kGreen);
    h1_Submode2Hit[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kMagenta);
    h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");


		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti4::L1L2_GlobalMulti4_Decode_101()
{
	const Int_t globalmulti = 4;
	const char* mode = "101";
  std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_2hit[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2B_Sum03;
	Double_t EL2B_Sum12;
	Double_t EL1S_Sum01;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_Submode2Hit[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode2Hit[ssdindex] = new TH1I(Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  printf("Processing mode g%d-%s ...\n", globalmulti, mode);
  //trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;
			candimulti_2hit[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

		            // 填充 mode statistic
		            bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
								                                                                        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
						    h1_modestat[ssdindex]->Fill(bin_index[itrack]);

		            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
		                trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
		            {
		              candimulti[ssdindex]++;
		            }
							}
					  }
          }
          //____________________________________________________________________
          //           以下部分为当前 L1L2-globalomulti4-101 的径迹重建策略！！！
          //          -------------------------------------------------------
					//  0)  000
					//  1)  001
					//  2)  100
					//  3)  101
					EL2B_Sum03 = L1L2_L2BEMeV[ssdindex][0] + L1L2_L2BEMeV[ssdindex][3];
					EL2B_Sum12 = L1L2_L2BEMeV[ssdindex][1] + L1L2_L2BEMeV[ssdindex][2];
					EL1S_Sum01 = L1L2_L1SEMeV[ssdindex][0] + L1L2_L1SEMeV[ssdindex][1];

          // 考虑 L2F 双击
					if ( trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum03,L1L2_L2FEMeV[ssdindex][0]) &&
				       trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][0]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][0]) &&
						   trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][3]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][3]) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ((itrack!=1) && (itrack!=2)) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
						}
					}
					// 考虑 L2F 双击
					if ( trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum12,L1L2_L2FEMeV[ssdindex][1]) &&
				       trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][1]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][1]) &&
						   trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2BEMeV[ssdindex][2]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][2]) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ((itrack!=0) && (itrack!=3)) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2BEMeV[ssdindex][itrack]*EL2B_TO_EL2F_SCALE[ssdindex],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
						}
					}
					// 考虑 L2B sharing
					else if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][0]-L1L2_L2BStripNum[ssdindex][3])==1) &&
				            trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum03,L1L2_L2FEMeV[ssdindex][0]) &&
									  trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0]) )
					{
						if ((L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]))
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0]);
							}
						}
					}
					// 考虑 L2B sharing
					else if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][0]-L1L2_L2BStripNum[ssdindex][3])==1) &&
				            trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum03,L1L2_L2FEMeV[ssdindex][3]) &&
									  trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][3],L1L2_L1SEMeV_Corrected[ssdindex][3]) )
					{
						if ((L1L2_L2FTime[ssdindex][3]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][3]] && L1L2_L2FTime[ssdindex][3]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][3]]))
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][3], L1L2_L2FStripNum[ssdindex][3]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][3],L1L2_L1SEMeV_Corrected[ssdindex][3]);
							}
						}
					}
					// 考虑 L1S sharing
					else if ( trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
						        (TMath::Abs(L1L2_L1SStripNum[ssdindex][0]-L1L2_L1SStripNum[ssdindex][1])==1) &&
				            trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][0],EL1S_Sum01) )
					{
						if ((L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]))
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][0],EL1S_Sum01);
							}
						}
					}
					// 考虑 L1S sharing
          else if ( trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][3],L1L2_L2FEMeV[ssdindex][3]) &&
						        (TMath::Abs(L1L2_L1SStripNum[ssdindex][0]-L1L2_L1SStripNum[ssdindex][1])==1) &&
				            trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][3],EL1S_Sum01) )
					{
						if ((L1L2_L2FTime[ssdindex][3]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][3]] && L1L2_L2FTime[ssdindex][3]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][3]]))
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][3], L1L2_L2FStripNum[ssdindex][3]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][3],EL1S_Sum01);
							}
						}
					}
					// 考虑只有一个粒子的情况
          else {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
							  }
							}
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
									h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
                /*
								cout<<"ientry="<<ientry<<setw(12)<<"itrack="<<itrack<<setw(12)<<"StripL2B="<<L1L2_L2BStripNum[ssdindex][itrack]<<setw(12)<<"StripL2F="<<L1L2_L2FStripNum[ssdindex][itrack]<<setw(12)
										<<"StripL1S="<<L1L2_L1SStripNum[ssdindex][itrack]<<setw(12)<<"EL2B="<<L1L2_L2BEMeV[ssdindex][itrack]<<setw(12)<<"EL2F="<<L1L2_L2FEMeV[ssdindex][itrack]<<setw(12)
										<<"EL1S="<<L1L2_L1SEMeV[ssdindex][itrack]<<setw(12)<<(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) ? "Y" : "N")<<setw(12)
										<<(trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]) ? "Y" : "N")<<setw(12)
										<<( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
										     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack])) ? 1 : 0 )
										<<endl;
                */
							}
						}
					}
					//
					if (candimulti_2hit[ssdindex] > 0) {
						MHits_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_Submode2Hit[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						Sharing_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex]    = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
		MHits_Ratio[ssdindex]   = (Double_t) MHits_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
    Sharing_Ratio[ssdindex] = (Double_t) Sharing_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
  }

	// 定义 latex
  Double_t x1,y1,x2,y2;
  TLatex* latex_ratio[NUM_SSD];
  TLatex* latex_decoded[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * EL2Range[ssdindex];
    y1 = 0.8 * EL1Range[ssdindex];
    x2 = 0.2 * EL2Range[ssdindex];
    y2 = 0.5 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.2f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
    latex_decoded[ssdindex] = new TLatex(x2, y2, Form("r_{sh}=%.4f%%, r_{mh}=%.4f%%",Sharing_Ratio[ssdindex],MHits_Ratio[ssdindex]));
    latex_decoded[ssdindex]->SetTextSize(0.05);
    latex_decoded[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
    legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode2Hit[ssdindex],"2-hit","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");
		latex_decoded[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_Submode2Hit[ssdindex]->Draw("SAME");
		h1_Submode2Hit[ssdindex]->SetLineColor(kGreen);
    h1_Submode2Hit[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kMagenta);
    h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");


		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti4::L1L2_GlobalMulti4_Decode_110()
{
	const Int_t globalmulti = 4;
	const char* mode = "110";
  std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_2hit[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2B_Sum03;
	Double_t EL2F_Sum03;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_Submode2Hit[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode2Hit[ssdindex] = new TH1I(Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  printf("Processing mode g%d-%s ...\n", globalmulti, mode);
  //trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;
			candimulti_2hit[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

		            // 填充 mode statistic
		            bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
								                                                                        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
						    h1_modestat[ssdindex]->Fill(bin_index[itrack]);

		            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
		                trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
		            {
		              candimulti[ssdindex]++;
		            }
							}
					  }
          }
          //____________________________________________________________________
          //           以下部分为当前 L1L2-globalomulti4-110 的径迹重建策略！！！
          //          -------------------------------------------------------
					//  0)  000
					//  1)  010
					//  2)  100
					//  3)  110
					EL2B_Sum03 = L1L2_L2BEMeV[ssdindex][0] + L1L2_L2BEMeV[ssdindex][3];
					EL2F_Sum03 = L1L2_L2FEMeV[ssdindex][0] + L1L2_L2FEMeV[ssdindex][3];

					// 考虑 L2B sharing
          if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][0]-L1L2_L2BStripNum[ssdindex][3])==1) &&
					     trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum03,L1L2_L2FEMeV[ssdindex][0]) &&
							 trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0]) )
					{
						if (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]])
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0]);
							}
						}
					}
					// 考虑 L2B sharing
					else if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][0]-L1L2_L2BStripNum[ssdindex][3])==1) &&
					          trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum03,L1L2_L2FEMeV[ssdindex][3]) &&
							      trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][3],L1L2_L1SEMeV_Corrected[ssdindex][3]) )
					{
						if (L1L2_L2FTime[ssdindex][3]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][3]] && L1L2_L2FTime[ssdindex][3]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][3]])
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][3], L1L2_L2FStripNum[ssdindex][3]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][3],L1L2_L1SEMeV_Corrected[ssdindex][3]);
							}
						}
					}
					// 考虑 L2F sharing
					else if ( (TMath::Abs(L1L2_L2FStripNum[ssdindex][0]-L1L2_L2FStripNum[ssdindex][3])==1) &&
					          trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],EL2F_Sum03) &&
							      trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],EL2F_Sum03,L1L2_L1SEMeV_Corrected[ssdindex][0]) )
					{
						if ( (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]) ||
					       (L1L2_L2FTime[ssdindex][3]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][3]] && L1L2_L2FTime[ssdindex][3]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][3]]) )
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(EL2F_Sum03,L1L2_L1SEMeV_Corrected[ssdindex][0]);
							}
						}
					}
					// 考虑 L2F sharing
					else if ( (TMath::Abs(L1L2_L2FStripNum[ssdindex][0]-L1L2_L2FStripNum[ssdindex][3])==1) &&
					          trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][3],EL2F_Sum03) &&
							      trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],EL2F_Sum03,L1L2_L1SEMeV_Corrected[ssdindex][3]) )
					{
						if ( (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]) ||
					       (L1L2_L2FTime[ssdindex][3]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][3]] && L1L2_L2FTime[ssdindex][3]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][3]]) )
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][3], L1L2_L2FStripNum[ssdindex][3]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(EL2F_Sum03,L1L2_L1SEMeV_Corrected[ssdindex][3]);
							}
						}
					}
					// 考虑L2B, L2F 同时 sharing
					else if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][0]-L1L2_L2BStripNum[ssdindex][3])==1) && (TMath::Abs(L1L2_L2FStripNum[ssdindex][0]-L1L2_L2FStripNum[ssdindex][3])==1) &&
					          trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum03,EL2F_Sum03) &&
							      trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],EL2F_Sum03,L1L2_L1SEMeV_Corrected[ssdindex][3]) )
					{
						if ( (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]) ||
					       (L1L2_L2FTime[ssdindex][3]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][3]] && L1L2_L2FTime[ssdindex][3]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][3]]) )
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][3], L1L2_L2FStripNum[ssdindex][3]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(EL2F_Sum03,L1L2_L1SEMeV_Corrected[ssdindex][3]);
							}
						}
					}

					// 考虑只有一个粒子的情况
          else {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
							  }
							}
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
								  h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					if (candimulti_2hit[ssdindex] > 0) {
						MHits_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_Submode2Hit[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						Sharing_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex]    = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
		MHits_Ratio[ssdindex]   = (Double_t) MHits_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
    Sharing_Ratio[ssdindex] = (Double_t) Sharing_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
  }

	// 定义 latex
  Double_t x1,y1,x2,y2;
  TLatex* latex_ratio[NUM_SSD];
  TLatex* latex_decoded[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * EL2Range[ssdindex];
    y1 = 0.8 * EL1Range[ssdindex];
    x2 = 0.2 * EL2Range[ssdindex];
    y2 = 0.5 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.2f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
    latex_decoded[ssdindex] = new TLatex(x2, y2, Form("r_{sh}=%.4f%%, r_{mh}=%.4f%%",Sharing_Ratio[ssdindex],MHits_Ratio[ssdindex]));
    latex_decoded[ssdindex]->SetTextSize(0.05);
    latex_decoded[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
    legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode2Hit[ssdindex],"2-hit","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");
		latex_decoded[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_Submode2Hit[ssdindex]->Draw("SAME");
		h1_Submode2Hit[ssdindex]->SetLineColor(kGreen);
    h1_Submode2Hit[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kMagenta);
    h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");


		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti4::L1L2_GlobalMulti4_Decode_111()
{
	const Int_t globalmulti = 4;
	const char* mode = "111";
  std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_2hit[NUM_SSD];
	Int_t candimulti_4hit[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

	Double_t EL2B_Sum03;
	Double_t EL2F_Sum03;
  std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
  std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

  TH1I* h1_modestat[NUM_SSD];
  TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_Submode2Hit[NUM_SSD];
	TH1I* h1_Submode4Hit[NUM_SSD];
  TH1I* h1_candimulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode2Hit[ssdindex] = new TH1I(Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode4Hit[ssdindex] = new TH1I(Form("SSD%d_Submode4Hit_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_Submode4Hit_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
    h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
  }

  TH2F h2_L1L2_DEE[NUM_SSD];
  TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
  TH2F h2_L1L2_DEE_Discard[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
  }

  printf("Processing mode g%d-%s ...\n", globalmulti, mode);
  //trackreconstruct->fChainTrackTree->SetEntries(10000000);
  Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    trackreconstruct->fChainTrackTree->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      L1L2_L2BEMeV[ssdindex].clear();
      L1L2_L2FEMeV[ssdindex].clear();
      L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
		  L1L2_L2FStripNum[ssdindex].clear();
		  L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
      candimulti[ssdindex] = 0;
      candimulti_corrected[ssdindex] = 0;
			candimulti_2hit[ssdindex] = 0;
			candimulti_4hit[ssdindex] = 0;

      if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
        for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
          if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
            L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
            L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
            L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
            L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
            L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
          }
        }
      }

      // 填充数据
      if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
			  if (strcmp(mode_exp.c_str(),mode) == 0) {
          for (Int_t itrack=0; itrack<globalmulti; itrack++) {
					  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
					  {
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

		            // 填充 mode statistic
		            bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
								                                                                        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
						    h1_modestat[ssdindex]->Fill(bin_index[itrack]);

		            if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
		                trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
		            {
		              candimulti[ssdindex]++;
		            }
							}
					  }
          }
          //____________________________________________________________________
          //           以下部分为当前 L1L2-globalomulti4-111 的径迹重建策略！！！
          //          -------------------------------------------------------
					//  0)  000
					//  1)  010
					//  2)  101
					//  3)  111
					//
					// 检查 4-hit 事件所占比例
					if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
								trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
							 (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][1],L1L2_L2FEMeV[ssdindex][1]) &&
 								trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][1],L1L2_L1SEMeV_Corrected[ssdindex][1])) &&
							 (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][2],L1L2_L2FEMeV[ssdindex][2]) &&
 								trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][2],L1L2_L1SEMeV_Corrected[ssdindex][2])) &&
							 (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][3],L1L2_L2FEMeV[ssdindex][3]) &&
								trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][3],L1L2_L1SEMeV_Corrected[ssdindex][3])) )
					{
            candimulti_4hit[ssdindex]++;
					}
					//
					if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
							 (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][3],L1L2_L2FEMeV[ssdindex][3]) &&
								trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][3],L1L2_L1SEMeV_Corrected[ssdindex][3])) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ((itrack!=1) && (itrack!=2)) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
						}
					}
					else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][1],L1L2_L2FEMeV[ssdindex][1]) &&
						         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][1],L1L2_L1SEMeV_Corrected[ssdindex][1])) &&
							      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][2],L1L2_L2FEMeV[ssdindex][2]) &&
								     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][2],L1L2_L1SEMeV_Corrected[ssdindex][2])) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ((itrack!=0) && (itrack!=3)) {
								if ((L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
						}
					}
					// 考虑只有一个粒子的情况
          else {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						        trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
							  }
							}
						}
					}

					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
								  h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					if (candimulti_2hit[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_Submode2Hit[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
					if (candimulti_4hit[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  h1_Submode4Hit[ssdindex]->Fill(bin_index[itrack]);
						  }
						}
					}
					//
				}
      }
    }
  }

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

  // 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
  }

  // 定义 latex
  Double_t x1,y1;
  TLatex* latex_ratio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.25 * EL2Range[ssdindex];
    y1 = 0.90 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
  Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
  TLegend* legend_SubMode[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
    legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode2Hit[ssdindex],"2-hit","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode4Hit[ssdindex],"4-hit","l");
  }

  // 画图
  TCanvas c_begin("c_begin","c_begin",600,600);
  TCanvas c_end("c_end","c_end",600,600);
  TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

  c_begin.Print(pathPDFOutBegin.c_str());
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
    //
	  cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
    //
    cans.cd(3);
    h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");

    //
    cans.cd(4);
    // candimulti_sum
    h1_candimulti[ssdindex]->Draw();
    h1_candimulti[ssdindex]->SetLineColor(kBlue);
    h1_candimulti[ssdindex]->SetLineWidth(2);
    h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
    h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
    cans.cd(5);
    h1_modestat[ssdindex]->Draw();
    h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
    h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
    h1_SubmodeCorrected[ssdindex]->Draw("SAME");
    h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
    h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_Submode2Hit[ssdindex]->Draw("SAME");
		h1_Submode2Hit[ssdindex]->SetLineColor(kGreen);
    h1_Submode2Hit[ssdindex]->SetLineWidth(2);
		h1_Submode4Hit[ssdindex]->Draw("SAME");
		h1_Submode4Hit[ssdindex]->SetLineColor(kMagenta);
    h1_Submode4Hit[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");


		gPad->Modified();
    gPad->Update();

		cans.Print(pathPDFOut.c_str());
  }
  c_end.Print(pathPDFOutEnd.c_str());
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo



//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//                        fGlobalMulti = 6
//                    ------------------------
L1L2_DecodefGlobalMulti6::L1L2_DecodefGlobalMulti6()
{
	fFirstRun = 150;
	fLastRun  = 160;
}

L1L2_DecodefGlobalMulti6::L1L2_DecodefGlobalMulti6(Int_t firstrun, Int_t lastrun)
{
  fFirstRun = firstrun;
  fLastRun  = lastrun;

  trackreconstruct = new CSHINETrackReconstruction(fFirstRun, fLastRun);

  std::string pathBananaCut(Form("%sStraighteningData/L1L2DEECuts.root",PATHROOTFILESFOLDER));
  TFile* myfile = TFile::Open(pathBananaCut.c_str());
  // 读取 bananacut
  for (Int_t numtel=0; numtel<NUM_SSD; numtel++) {
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

L1L2_DecodefGlobalMulti6::~L1L2_DecodefGlobalMulti6()
{}
//______________________________________________________________________________



//______________________________________________________________________________
void L1L2_DecodefGlobalMulti6::L1L2_GlobalMulti6_Decode_111()
{
	const Int_t globalmulti = 6;
	const char* mode = "111";
	std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_2hit[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2F_Sum03;
	Double_t EL2F_Sum12;
	Double_t EL1S_Sum01;

	std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
	std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
	std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

	TH1I* h1_modestat[NUM_SSD];
	TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_Submode2Hit[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
	TH1I* h1_candimulti[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode2Hit[ssdindex] = new TH1I(Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
	}

	TH2F h2_L1L2_DEE[NUM_SSD];
	TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
	TH2F h2_L1L2_DEE_Discard[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	}

	printf("Processing mode g%d-%s ...\n", globalmulti, mode);
	//trackreconstruct->fChainTrackTree->SetEntries(10000000);
	Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
	cout<<"Found nentries = "<<nentries<<endl;

	for (Long64_t ientry=0; ientry<nentries; ientry++) {
		trackreconstruct->fChainTrackTree->GetEntry(ientry);
	  timeper.PrintPercentageAndRemainingTime(ientry, nentries);

		for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			L1L2_L2BEMeV[ssdindex].clear();
			L1L2_L2FEMeV[ssdindex].clear();
			L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
			L1L2_L2FStripNum[ssdindex].clear();
			L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
			candimulti[ssdindex] = 0;
			candimulti_corrected[ssdindex] = 0;
			candimulti_2hit[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

			if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
				for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
					if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
						L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
						L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
						L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
						L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
						L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
					}
				}
			}

			// 填充数据
			if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
				if (strcmp(mode_exp.c_str(),mode) == 0) {
					for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

								// 填充 mode statistic
								bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
																																												trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
								h1_modestat[ssdindex]->Fill(bin_index[itrack]);

								if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
										trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
								{
									candimulti[ssdindex]++;
								}
							}
						}
					}
					//____________________________________________________________________
					//           以下部分为当前 L1L2-globalomulti6-111 的径迹重建策略！！！
					//          -------------------------------------------------------
					//  0) 000            0) 000
					//  1）010            1) 001
					//  2）100            2) 010
					//  3）101            3) 011
					//  4）110            4) 101
					//  5）111            5) 111
					//
					// L2B: 01-2345
					if (L1L2_L2BStripNum[ssdindex][1]!=L1L2_L2BStripNum[ssdindex][2])
					{
						if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
							    trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][0], L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
								 (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][5],L1L2_L2FEMeV[ssdindex][5]) &&
 		 							trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][5], L1L2_L1SEMeV_Corrected[ssdindex][5])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==0 || itrack==5) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
							  }
							}
						}
						else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][1],L1L2_L2FEMeV[ssdindex][1]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][1], L1L2_L1SEMeV_Corrected[ssdindex][1])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][3],L1L2_L2FEMeV[ssdindex][3]) &&
 		 							     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][3], L1L2_L1SEMeV_Corrected[ssdindex][3])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==1 || itrack==3) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
							  }
							}
						}
						else {
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										  break;
										}
									}
							  }
							}
						}
					}
					// L2B: 0123-45
					else if ( L1L2_L2BStripNum[ssdindex][3]!=L1L2_L2BStripNum[ssdindex][4])
          {
						if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
							    trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][0], L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
								 (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][5],L1L2_L2FEMeV[ssdindex][5]) &&
 		 							trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][5], L1L2_L1SEMeV_Corrected[ssdindex][5])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==0 || itrack==5) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
							  }
							}
						}
						else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][2],L1L2_L2FEMeV[ssdindex][2]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][2], L1L2_L1SEMeV_Corrected[ssdindex][2])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][4],L1L2_L2FEMeV[ssdindex][4]) &&
 		 							     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][4], L1L2_L1SEMeV_Corrected[ssdindex][4])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==2 || itrack==4) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
							  }
							}
						}
						else {
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										  break;
										}
									}
							  }
							}
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
									h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}

								/*
								cout<<"ientry="<<ientry<<setw(12)<<"itrack="<<itrack<<setw(12)<<"StripL2B="<<L1L2_L2BStripNum[ssdindex][itrack]<<setw(12)<<"StripL2F="<<L1L2_L2FStripNum[ssdindex][itrack]<<setw(12)
										<<"StripL1S="<<L1L2_L1SStripNum[ssdindex][itrack]<<setw(12)<<"EL2B="<<L1L2_L2BEMeV[ssdindex][itrack]<<setw(12)<<"EL2F="<<L1L2_L2FEMeV[ssdindex][itrack]<<setw(12)
										<<"EL1S="<<L1L2_L1SEMeV[ssdindex][itrack]<<setw(12)<<(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) ? "Y" : "N")<<setw(12)
										<<(trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]) ? "Y" : "N")<<setw(12)
										<<( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
												 trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack])) ? 1 : 0 )
										<<endl;
								*/
							}
						}
					}
					//
					if (candimulti_2hit[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								h1_Submode2Hit[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
					//
				}
			}
		}
	}

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
	Int_t entry_all[NUM_SSD] = {0};
	Int_t entry_discard[NUM_SSD] = {0};
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
		entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

		lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
	}

	// 定义 latex
	Double_t x1,y1;
	TLatex* latex_ratio[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		x1 = 0.25 * EL2Range[ssdindex];
		y1 = 0.90 * EL1Range[ssdindex];
		latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
		latex_ratio[ssdindex]->SetTextSize(0.05);
		latex_ratio[ssdindex]->SetTextColor(kOrange);
	}

	// 定义 Legend
	Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
	TLegend* legend_SubMode[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode2Hit[ssdindex],"2-hit","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
	}

	// 画图
	TCanvas c_begin("c_begin","c_begin",600,600);
	TCanvas c_end("c_end","c_end",600,600);
	TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

	c_begin.Print(pathPDFOutBegin.c_str());
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
		//
		cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
		//
		cans.cd(3);
		h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");

		//
		cans.cd(4);
		// candimulti_sum
		h1_candimulti[ssdindex]->Draw();
		h1_candimulti[ssdindex]->SetLineColor(kBlue);
		h1_candimulti[ssdindex]->SetLineWidth(2);
		h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
		h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
		cans.cd(5);
		h1_modestat[ssdindex]->Draw();
		h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
		h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
		h1_SubmodeCorrected[ssdindex]->Draw("SAME");
		h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
		h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_Submode2Hit[ssdindex]->Draw("SAME");
		h1_Submode2Hit[ssdindex]->SetLineColor(kGreen);
		h1_Submode2Hit[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kMagenta);
		h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");


		gPad->Modified();
		gPad->Update();

		cans.Print(pathPDFOut.c_str());
	}
	c_end.Print(pathPDFOutEnd.c_str());
}

//______________________________________________________________________________
void L1L2_DecodefGlobalMulti6::L1L2_GlobalMulti6_Decode_112()
{
	const Int_t globalmulti = 6;
	const char* mode = "112";
	std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_2hit[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2F_Sum03;
	Double_t EL2F_Sum12;
	Double_t EL1S_Sum01;

	std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
	std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
	std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

	TH1I* h1_modestat[NUM_SSD];
	TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_Submode2Hit[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
	TH1I* h1_candimulti[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode2Hit[ssdindex] = new TH1I(Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
	}

	TH2F h2_L1L2_DEE[NUM_SSD];
	TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
	TH2F h2_L1L2_DEE_Discard[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	}

	printf("Processing mode g%d-%s ...\n", globalmulti, mode);
	//trackreconstruct->fChainTrackTree->SetEntries(10000000);
	Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
	cout<<"Found nentries = "<<nentries<<endl;

	for (Long64_t ientry=0; ientry<nentries; ientry++) {
		trackreconstruct->fChainTrackTree->GetEntry(ientry);
		timeper.PrintPercentageAndRemainingTime(ientry, nentries);

		for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			L1L2_L2BEMeV[ssdindex].clear();
			L1L2_L2FEMeV[ssdindex].clear();
			L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
			L1L2_L2FStripNum[ssdindex].clear();
			L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
			candimulti[ssdindex] = 0;
			candimulti_corrected[ssdindex] = 0;
			candimulti_2hit[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

			if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
				for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
					if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
						L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
						L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
						L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
						L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
						L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
					}
				}
			}

			// 填充数据
			if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
				if (strcmp(mode_exp.c_str(),mode) == 0) {
					for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

								// 填充 mode statistic
								bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
																																												trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
								h1_modestat[ssdindex]->Fill(bin_index[itrack]);

								if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
										trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
								{
									candimulti[ssdindex]++;
								}
							}
						}
					}
					//____________________________________________________________________
					//           以下部分为当前 L1L2-globalomulti6-112 的径迹重建策略！！！
					//          -------------------------------------------------------
					//  0) 000            0) 000
					//  1）010            1) 001
					//  2）101            2) 010
					//  3）102            3) 011
					//  4）111            4) 102
					//  5）112            5) 112
					//
					// L2B: 01-2345
					if (L1L2_L2BStripNum[ssdindex][1]!=L1L2_L2BStripNum[ssdindex][2])
					{
						if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
							    trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][0], L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
								 (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][4],L1L2_L2FEMeV[ssdindex][4]) &&
 		 							trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][4], L1L2_L1SEMeV_Corrected[ssdindex][4])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==0 || itrack==4) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						//
						else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][0], L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][5],L1L2_L2FEMeV[ssdindex][5]) &&
 		 							     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][5], L1L2_L1SEMeV_Corrected[ssdindex][5])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==0 || itrack==5) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						//
						else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][1],L1L2_L2FEMeV[ssdindex][1]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][1], L1L2_L1SEMeV_Corrected[ssdindex][1])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][2],L1L2_L2FEMeV[ssdindex][2]) &&
 		 							     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][2], L1L2_L1SEMeV_Corrected[ssdindex][2])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==1 || itrack==2) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						//
						//
						else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][1],L1L2_L2FEMeV[ssdindex][1]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][1], L1L2_L1SEMeV_Corrected[ssdindex][1])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][3],L1L2_L2FEMeV[ssdindex][3]) &&
 		 							     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][3], L1L2_L1SEMeV_Corrected[ssdindex][3])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==1 || itrack==3) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						else {
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										  break;
										}
									}
							  }
							}
						}
					}
					// L2B: 0123-45
					else if ( L1L2_L2BStripNum[ssdindex][3]!=L1L2_L2BStripNum[ssdindex][4])
          {
						if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
							    trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][0], L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
								 (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][5],L1L2_L2FEMeV[ssdindex][5]) &&
 		 							trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][5], L1L2_L1SEMeV_Corrected[ssdindex][5])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==0 || itrack==5) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][1],L1L2_L2FEMeV[ssdindex][1]) &&
							    trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][1], L1L2_L1SEMeV_Corrected[ssdindex][1])) &&
								 (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][5],L1L2_L2FEMeV[ssdindex][5]) &&
 		 							trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][5], L1L2_L1SEMeV_Corrected[ssdindex][5])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==1 || itrack==5) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][2],L1L2_L2FEMeV[ssdindex][2]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][2], L1L2_L1SEMeV_Corrected[ssdindex][2])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][4],L1L2_L2FEMeV[ssdindex][4]) &&
 		 							     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][4], L1L2_L1SEMeV_Corrected[ssdindex][4])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==2 || itrack==4) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][3],L1L2_L2FEMeV[ssdindex][3]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][3], L1L2_L1SEMeV_Corrected[ssdindex][3])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][4],L1L2_L2FEMeV[ssdindex][4]) &&
 		 							     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][4], L1L2_L1SEMeV_Corrected[ssdindex][4])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==3 || itrack==4) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						else {
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										  break;
										}
									}
							  }
							}
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
									h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}

								/*
								cout<<"ientry="<<ientry<<setw(12)<<"itrack="<<itrack<<setw(12)<<"StripL2B="<<L1L2_L2BStripNum[ssdindex][itrack]<<setw(12)<<"StripL2F="<<L1L2_L2FStripNum[ssdindex][itrack]<<setw(12)
										<<"StripL1S="<<L1L2_L1SStripNum[ssdindex][itrack]<<setw(12)<<"EL2B="<<L1L2_L2BEMeV[ssdindex][itrack]<<setw(12)<<"EL2F="<<L1L2_L2FEMeV[ssdindex][itrack]<<setw(12)
										<<"EL1S="<<L1L2_L1SEMeV[ssdindex][itrack]<<setw(12)<<(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) ? "Y" : "N")<<setw(12)
										<<(trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]) ? "Y" : "N")<<setw(12)
										<<( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
												 trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack])) ? 1 : 0 )
										<<endl;
								*/
							}
						}
					}
					//
					if (candimulti_2hit[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								h1_Submode2Hit[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
					//
				}
			}
		}
	}

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
	Int_t entry_all[NUM_SSD] = {0};
	Int_t entry_discard[NUM_SSD] = {0};
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
		entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

		lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
	}

	// 定义 latex
	Double_t x1,y1;
	TLatex* latex_ratio[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		x1 = 0.25 * EL2Range[ssdindex];
		y1 = 0.90 * EL1Range[ssdindex];
		latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
		latex_ratio[ssdindex]->SetTextSize(0.05);
		latex_ratio[ssdindex]->SetTextColor(kOrange);
	}

	// 定义 Legend
	Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
	TLegend* legend_SubMode[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode2Hit[ssdindex],"2-hit","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
	}

	// 画图
	TCanvas c_begin("c_begin","c_begin",600,600);
	TCanvas c_end("c_end","c_end",600,600);
	TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

	c_begin.Print(pathPDFOutBegin.c_str());
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
		//
		cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
		//
		cans.cd(3);
		h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");

		//
		cans.cd(4);
		// candimulti_sum
		h1_candimulti[ssdindex]->Draw();
		h1_candimulti[ssdindex]->SetLineColor(kBlue);
		h1_candimulti[ssdindex]->SetLineWidth(2);
		h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
		h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
		cans.cd(5);
		h1_modestat[ssdindex]->Draw();
		h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
		h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
		h1_SubmodeCorrected[ssdindex]->Draw("SAME");
		h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
		h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_Submode2Hit[ssdindex]->Draw("SAME");
		h1_Submode2Hit[ssdindex]->SetLineColor(kGreen);
		h1_Submode2Hit[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kMagenta);
		h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");


		gPad->Modified();
		gPad->Update();

		cans.Print(pathPDFOut.c_str());
	}
	c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti6::L1L2_GlobalMulti6_Decode_120()
{
	const Int_t globalmulti = 6;
	const char* mode = "120";
	std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_2hit[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2F_Sum01;
	Double_t EL2F_Sum12;
	Double_t EL2F_Sum04;
	Double_t EL2F_Sum15;
	Double_t EL2B_Sum03;
	Double_t EL2B_Sum04;
	Double_t EL2B_Sum15;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

	std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
	std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
	std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

	TH1I* h1_modestat[NUM_SSD];
	TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_Submode2Hit[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
	TH1I* h1_candimulti[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode2Hit[ssdindex] = new TH1I(Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
	}

	TH2F h2_L1L2_DEE[NUM_SSD];
	TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
	TH2F h2_L1L2_DEE_Discard[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	}

	printf("Processing mode g%d-%s ...\n", globalmulti, mode);
	//trackreconstruct->fChainTrackTree->SetEntries(10000000);
	Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
	cout<<"Found nentries = "<<nentries<<endl;

	for (Long64_t ientry=0; ientry<nentries; ientry++) {
		trackreconstruct->fChainTrackTree->GetEntry(ientry);
		timeper.PrintPercentageAndRemainingTime(ientry, nentries);

		for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			L1L2_L2BEMeV[ssdindex].clear();
			L1L2_L2FEMeV[ssdindex].clear();
			L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
			L1L2_L2FStripNum[ssdindex].clear();
			L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
			candimulti[ssdindex] = 0;
			candimulti_corrected[ssdindex] = 0;
			candimulti_2hit[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

			if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
				for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
					if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
						L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
						L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
						L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
						L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
						L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
					}
				}
			}

			// 填充数据
			if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
				if (strcmp(mode_exp.c_str(),mode) == 0) {
					for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
              {
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

								// 填充 mode statistic
								bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
																																												trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
								h1_modestat[ssdindex]->Fill(bin_index[itrack]);

								if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
										trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
								{
									candimulti[ssdindex]++;
								}
							}
						}
					}
					//____________________________________________________________________
					//           以下部分为当前 L1L2-globalomulti6-120 的径迹重建策略！！！
					//          -------------------------------------------------------
					//  0)  000
					//  1)  010
					//  2)  020
					//  3)  100
					//  4)  110
					//  5)  120
					//
					// L2B: 012-345
					EL2B_Sum03 = L1L2_L2BEMeV[ssdindex][0] + L1L2_L2BEMeV[ssdindex][3];
					EL2B_Sum04 = L1L2_L2BEMeV[ssdindex][0] + L1L2_L2BEMeV[ssdindex][4];
					EL2B_Sum15 = L1L2_L2BEMeV[ssdindex][1] + L1L2_L2BEMeV[ssdindex][5];

					EL2F_Sum12 = L1L2_L2FEMeV[ssdindex][1] + L1L2_L2FEMeV[ssdindex][2];
					EL2F_Sum04 = L1L2_L2FEMeV[ssdindex][0] + L1L2_L2FEMeV[ssdindex][4];
					EL2F_Sum15 = L1L2_L2FEMeV[ssdindex][1] + L1L2_L2FEMeV[ssdindex][5];
					//
					// 考虑 L2B sharing
					if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][0]-L1L2_L2BStripNum[ssdindex][3])==1) &&
				       ((trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum03,L1L2_L2FEMeV[ssdindex][0])) ||
						    (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum03,L1L2_L2FEMeV[ssdindex][1])) ||
						    (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum03,L1L2_L2FEMeV[ssdindex][2]))) )
					{
						for (Int_t itrack=0; itrack<3; itrack++) {
							  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						    {
							    if (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_sharing[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										  break;
										}
									}
							  }
							}
					}
					// 考虑 L2F sharing
					else if ( (TMath::Abs(L1L2_L2FStripNum[ssdindex][0]-L1L2_L2FStripNum[ssdindex][4])==1) &&
				            (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BStripNum[ssdindex][0],EL2F_Sum04)) &&
									  (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],EL2F_Sum04,L1L2_L1SEMeV_Corrected[ssdindex][0])) )
					{
						if ( (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]) ||
					       (L1L2_L2FTime[ssdindex][4]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][4]] && L1L2_L2FTime[ssdindex][4]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][4]]) )
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(EL2F_Sum04,L1L2_L1SEMeV_Corrected[ssdindex][0]);
							}
						}
					}
					// 考虑 L2F sharing
					else if ( (TMath::Abs(L1L2_L2FStripNum[ssdindex][0]-L1L2_L2FStripNum[ssdindex][4])==1) &&
				            (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BStripNum[ssdindex][4],EL2F_Sum04)) &&
									  (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],EL2F_Sum04,L1L2_L1SEMeV_Corrected[ssdindex][4])) )
					{
						if ( (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]) ||
					       (L1L2_L2FTime[ssdindex][4]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][4]] && L1L2_L2FTime[ssdindex][4]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][4]]) )
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][4], L1L2_L2FStripNum[ssdindex][4]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(EL2F_Sum04,L1L2_L1SEMeV_Corrected[ssdindex][4]);
							}
						}
					}
					// 考虑 L2F sharing
					else if ( (TMath::Abs(L1L2_L2FStripNum[ssdindex][1]-L1L2_L2FStripNum[ssdindex][5])==1) &&
				            (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BStripNum[ssdindex][1],EL2F_Sum15)) &&
									  (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],EL2F_Sum15,L1L2_L1SEMeV_Corrected[ssdindex][1])) )
					{
						if ( (L1L2_L2FTime[ssdindex][1]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][1]] && L1L2_L2FTime[ssdindex][1]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][1]]) ||
					       (L1L2_L2FTime[ssdindex][5]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][5]] && L1L2_L2FTime[ssdindex][5]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][5]]) )
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][1], L1L2_L2FStripNum[ssdindex][1]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(EL2F_Sum15,L1L2_L1SEMeV_Corrected[ssdindex][1]);
							}
						}
					}
					// 考虑 L2F sharing
					else if ( (TMath::Abs(L1L2_L2FStripNum[ssdindex][1]-L1L2_L2FStripNum[ssdindex][5])==1) &&
				            (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BStripNum[ssdindex][5],EL2F_Sum15)) &&
									  (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],EL2F_Sum15,L1L2_L1SEMeV_Corrected[ssdindex][5])) )
					{
						if ( (L1L2_L2FTime[ssdindex][1]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][1]] && L1L2_L2FTime[ssdindex][1]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][1]]) ||
					       (L1L2_L2FTime[ssdindex][2]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][5]] && L1L2_L2FTime[ssdindex][5]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][5]]) )
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][5], L1L2_L2FStripNum[ssdindex][5]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(EL2F_Sum12,L1L2_L1SEMeV_Corrected[ssdindex][5]);
							}
						}
					}
					// 考虑 L2B, L2F 同时 sharing
					else if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][0]-L1L2_L2BStripNum[ssdindex][4])==1) &&
					          (TMath::Abs(L1L2_L2FStripNum[ssdindex][0]-L1L2_L2FStripNum[ssdindex][4])==1) &&
				            (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum04,EL2F_Sum04)) &&
									  (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],EL2F_Sum04,L1L2_L1SEMeV_Corrected[ssdindex][0])) )
					{
						if ( (L1L2_L2FTime[ssdindex][0]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][0]] && L1L2_L2FTime[ssdindex][0]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][0]]) ||
					       (L1L2_L2FTime[ssdindex][4]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][4]] && L1L2_L2FTime[ssdindex][4]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][4]]) )
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][0], L1L2_L2FStripNum[ssdindex][0]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(EL2F_Sum04,L1L2_L1SEMeV_Corrected[ssdindex][0]);
							}
						}
					}
					// 考虑 L2B, L2F 同时 sharing
					else if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][1]-L1L2_L2BStripNum[ssdindex][5])==1) &&
					          (TMath::Abs(L1L2_L2FStripNum[ssdindex][1]-L1L2_L2FStripNum[ssdindex][5])==1) &&
				            (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum15,EL2F_Sum15)) &&
									  (trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],EL2F_Sum15,L1L2_L1SEMeV_Corrected[ssdindex][1])) )
					{
						if ( (L1L2_L2FTime[ssdindex][1]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][1]] && L1L2_L2FTime[ssdindex][1]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][1]]) ||
					       (L1L2_L2FTime[ssdindex][5]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][5]] && L1L2_L2FTime[ssdindex][5]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][5]]) )
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][1], L1L2_L2FStripNum[ssdindex][1]))
							{
								candimulti_corrected[ssdindex]++;
								candimulti_sharing[ssdindex]++;
								h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(EL2F_Sum15,L1L2_L1SEMeV_Corrected[ssdindex][1]);
							}
						}
					}
					else {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
							      trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
								{
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
										h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
								}
							}
					  }
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
									h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					if (candimulti_2hit[ssdindex] > 0) {
						MHits_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								h1_Submode2Hit[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						Sharing_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
					//
				}
			}
		}
	}

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex]    = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
		MHits_Ratio[ssdindex]   = (Double_t) MHits_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
    Sharing_Ratio[ssdindex] = (Double_t) Sharing_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
  }

	// 定义 latex
  Double_t x1,y1,x2,y2;
  TLatex* latex_ratio[NUM_SSD];
  TLatex* latex_decoded[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * EL2Range[ssdindex];
    y1 = 0.8 * EL1Range[ssdindex];
    x2 = 0.2 * EL2Range[ssdindex];
    y2 = 0.5 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.2f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
    latex_decoded[ssdindex] = new TLatex(x2, y2, Form("r_{sh}=%.4f%%, r_{mh}=%.4f%%",Sharing_Ratio[ssdindex],MHits_Ratio[ssdindex]));
    latex_decoded[ssdindex]->SetTextSize(0.05);
    latex_decoded[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
	Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
	TLegend* legend_SubMode[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode2Hit[ssdindex],"2-hit","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
	}

	// 画图
	TCanvas c_begin("c_begin","c_begin",600,600);
	TCanvas c_end("c_end","c_end",600,600);
	TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

	c_begin.Print(pathPDFOutBegin.c_str());
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
		//
		cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
		//
		cans.cd(3);
		h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");
		latex_decoded[ssdindex]->Draw("SAME");

		//
		cans.cd(4);
		// candimulti_sum
		h1_candimulti[ssdindex]->Draw();
		h1_candimulti[ssdindex]->SetLineColor(kBlue);
		h1_candimulti[ssdindex]->SetLineWidth(2);
		h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
		h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
		cans.cd(5);
		h1_modestat[ssdindex]->Draw();
		h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
		h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
		h1_SubmodeCorrected[ssdindex]->Draw("SAME");
		h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
		h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_Submode2Hit[ssdindex]->Draw("SAME");
		h1_Submode2Hit[ssdindex]->SetLineColor(kGreen);
		h1_Submode2Hit[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kMagenta);
		h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");


		gPad->Modified();
		gPad->Update();

		cans.Print(pathPDFOut.c_str());
	}
	c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti6::L1L2_GlobalMulti6_Decode_121()
{
	const Int_t globalmulti = 6;
	const char* mode = "121";
	std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_2hit[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2F_Sum01;
	Double_t EL2F_Sum12;
	Double_t EL2B_Sum03;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

	std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
	std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
	std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

	TH1I* h1_modestat[NUM_SSD];
	TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_Submode2Hit[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
	TH1I* h1_candimulti[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode2Hit[ssdindex] = new TH1I(Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
	}

	TH2F h2_L1L2_DEE[NUM_SSD];
	TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
	TH2F h2_L1L2_DEE_Discard[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	}

	printf("Processing mode g%d-%s ...\n", globalmulti, mode);
	//trackreconstruct->fChainTrackTree->SetEntries(10000000);
	Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
	cout<<"Found nentries = "<<nentries<<endl;

	for (Long64_t ientry=0; ientry<nentries; ientry++) {
		trackreconstruct->fChainTrackTree->GetEntry(ientry);
		timeper.PrintPercentageAndRemainingTime(ientry, nentries);

		for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			L1L2_L2BEMeV[ssdindex].clear();
			L1L2_L2FEMeV[ssdindex].clear();
			L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
			L1L2_L2FStripNum[ssdindex].clear();
			L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
			candimulti[ssdindex] = 0;
			candimulti_corrected[ssdindex] = 0;
			candimulti_2hit[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

			if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
				for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
					if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
						L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
						L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
						L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
						L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
						L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
					}
				}
			}

			// 填充数据
			if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
				if (strcmp(mode_exp.c_str(),mode) == 0) {
					for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

								// 填充 mode statistic
								bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
																																												trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
								h1_modestat[ssdindex]->Fill(bin_index[itrack]);

								if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
										trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
								{
									candimulti[ssdindex]++;
								}
							}
						}
					}
					//____________________________________________________________________
					//           以下部分为当前 L1L2-globalomulti6-121 的径迹重建策略！！！
					//          -------------------------------------------------------
					//  0)  000
					//  1)  010
					//  2)  020
					//  3)  101
					//  4)  111
					//  5)  121
					//
				  if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
						    trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
							 (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][4],L1L2_L2FEMeV[ssdindex][4]) &&
 		 						trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][4],L1L2_L1SEMeV_Corrected[ssdindex][4])) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ( (itrack==0 || itrack==4) &&
							     (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						      trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
					  }
					}
					else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
						         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][0],L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
							      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][5],L1L2_L2FEMeV[ssdindex][5]) &&
 		 						     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][5],L1L2_L1SEMeV_Corrected[ssdindex][5])) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ( (itrack==0 || itrack==5) &&
							     (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						      trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
					  }
					}
					else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][1],L1L2_L2FEMeV[ssdindex][1]) &&
						         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][1],L1L2_L1SEMeV_Corrected[ssdindex][1])) &&
							      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][3],L1L2_L2FEMeV[ssdindex][3]) &&
 		 						     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][3],L1L2_L1SEMeV_Corrected[ssdindex][3])) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ( (itrack==1 || itrack==3) &&
							     (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						      trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
					  }
					}
					else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][1],L1L2_L2FEMeV[ssdindex][1]) &&
						         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][1],L1L2_L1SEMeV_Corrected[ssdindex][1])) &&
							      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][5],L1L2_L2FEMeV[ssdindex][5]) &&
 		 						     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][5],L1L2_L1SEMeV_Corrected[ssdindex][5])) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ( (itrack==1 || itrack==5) &&
							     (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						      trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
					  }
					}
					else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][2],L1L2_L2FEMeV[ssdindex][2]) &&
						         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][2],L1L2_L1SEMeV_Corrected[ssdindex][2])) &&
							      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][3],L1L2_L2FEMeV[ssdindex][3]) &&
 		 						     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][3],L1L2_L1SEMeV_Corrected[ssdindex][3])) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ( (itrack==2 || itrack==3) &&
							     (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						      trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
					  }
					}
					else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][2],L1L2_L2FEMeV[ssdindex][2]) &&
						         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][2],L1L2_L1SEMeV_Corrected[ssdindex][2])) &&
							      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][4],L1L2_L2FEMeV[ssdindex][4]) &&
 		 						     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][4],L1L2_L1SEMeV_Corrected[ssdindex][4])) )
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if ( (itrack==2 || itrack==4) &&
							     (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						      trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  candimulti_2hit[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
									}
								}
							}
					  }
					}
					else
					{
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						  {
							  if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						      trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							  {
									if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							    {
										candimulti_corrected[ssdindex]++;
									  h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										break;
									}
							  }
							}
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
									h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					if (candimulti_2hit[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								h1_Submode2Hit[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
					//
				}
			}
		}
	}

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
	Int_t entry_all[NUM_SSD] = {0};
	Int_t entry_discard[NUM_SSD] = {0};
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
		entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

		lost_ratio[ssdindex] = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
	}

	// 定义 latex
	Double_t x1,y1;
	TLatex* latex_ratio[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		x1 = 0.25 * EL2Range[ssdindex];
		y1 = 0.90 * EL1Range[ssdindex];
		latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.4f%%",lost_ratio[ssdindex]));
		latex_ratio[ssdindex]->SetTextSize(0.05);
		latex_ratio[ssdindex]->SetTextColor(kOrange);
	}

	// 定义 Legend
	Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
	TLegend* legend_SubMode[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode2Hit[ssdindex],"2-hit","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
	}

	// 画图
	TCanvas c_begin("c_begin","c_begin",600,600);
	TCanvas c_end("c_end","c_end",600,600);
	TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

	c_begin.Print(pathPDFOutBegin.c_str());
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
		//
		cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
		//
		cans.cd(3);
		h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");

		//
		cans.cd(4);
		// candimulti_sum
		h1_candimulti[ssdindex]->Draw();
		h1_candimulti[ssdindex]->SetLineColor(kBlue);
		h1_candimulti[ssdindex]->SetLineWidth(2);
		h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
		h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
		cans.cd(5);
		h1_modestat[ssdindex]->Draw();
		h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
		h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
		h1_SubmodeCorrected[ssdindex]->Draw("SAME");
		h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
		h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_Submode2Hit[ssdindex]->Draw("SAME");
		h1_Submode2Hit[ssdindex]->SetLineColor(kGreen);
		h1_Submode2Hit[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kMagenta);
		h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");


		gPad->Modified();
		gPad->Update();

		cans.Print(pathPDFOut.c_str());
	}
	c_end.Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void L1L2_DecodefGlobalMulti6::L1L2_GlobalMulti6_Decode_211()
{
	const Int_t globalmulti = 6;
	const char* mode = "211";
	std::string mode_exp;

	Double_t lost_ratio[NUM_SSD] = {0};
	Int_t candimulti[NUM_SSD];
	Int_t candimulti_corrected[NUM_SSD];
	Int_t candimulti_2hit[NUM_SSD];
	Int_t candimulti_sharing[NUM_SSD];

	Int_t bin_index[globalmulti];

	Double_t EL2B_Sum24;
	Double_t EL2B_Sum35;
	Double_t EL2B_Sum02;
	Double_t EL2B_Sum13;

	Double_t MHits_Ratio[NUM_SSD]   = {0};
  Double_t Sharing_Ratio[NUM_SSD] = {0};
  Int_t MHits_Entries[NUM_SSD]    = {0};
  Int_t Sharing_Entries[NUM_SSD]  = {0};

	std::string pathPDFOut(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf",PATHFIGURESFOLDER,globalmulti,mode));
	std::string pathPDFOutBegin(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf[",PATHFIGURESFOLDER,globalmulti,mode));
	std::string pathPDFOutEnd(Form("%sfigure_TrackReconstruction/L1L2_GlobalMulti%d_Decode_%s.pdf]",PATHFIGURESFOLDER,globalmulti,mode));

	TH1I* h1_modestat[NUM_SSD];
	TH1I* h1_SubmodeCorrected[NUM_SSD];
	TH1I* h1_Submode2Hit[NUM_SSD];
	TH1I* h1_SubmodeSharing[NUM_SSD];
	TH1I* h1_candimulti[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		h1_modestat[ssdindex] = new TH1I(Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_ModeStat_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeCorrected[ssdindex] = new TH1I(Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeCorrected_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_Submode2Hit[ssdindex] = new TH1I(Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_Submode2Hit_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_SubmodeSharing[ssdindex] = new TH1I(Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_SubmodeSharing_g%d-%s",ssdindex+1,globalmulti,mode), 4, -0.5, 3.5);
		h1_candimulti[ssdindex] = new TH1I(Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), Form("SSD%d_CandiMulti_g%d-%s",ssdindex+1,globalmulti,mode), globalmulti+1, -0.5, globalmulti+0.5);
	}

	TH2F h2_L1L2_DEE[NUM_SSD];
	TH2F h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[NUM_SSD];
	TH2F h2_L1L2_DEE_Discard[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		h2_L1L2_DEE[ssdindex]= TH2F(Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex]= TH2F(Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Cuts_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
		h2_L1L2_DEE_Discard[ssdindex]= TH2F(Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),Form("SSD%d_L1L2_Discard_g%d-%s",ssdindex+1,globalmulti,mode),NBinsL2[ssdindex],0,EL2Range[ssdindex],NBinsL1[ssdindex],0,EL1Range[ssdindex]);
	}

	printf("Processing mode g%d-%s ...\n", globalmulti, mode);
	//trackreconstruct->fChainTrackTree->SetEntries(10000000);
	Long64_t nentries = trackreconstruct->fChainTrackTree->GetEntries();
	cout<<"Found nentries = "<<nentries<<endl;

	for (Long64_t ientry=0; ientry<nentries; ientry++) {
		trackreconstruct->fChainTrackTree->GetEntry(ientry);
	//	timeper.PrintPercentageAndRemainingTime(ientry, nentries);

		for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			L1L2_L2BEMeV[ssdindex].clear();
			L1L2_L2FEMeV[ssdindex].clear();
			L1L2_L1SEMeV[ssdindex].clear();
			L1L2_L1SEMeV_Corrected[ssdindex].clear();
			L1L2_L2BStripNum[ssdindex].clear();
			L1L2_L2FStripNum[ssdindex].clear();
			L1L2_L1SStripNum[ssdindex].clear();
			L1L2_L2FTime[ssdindex].clear();

			// 初始化
			candimulti[ssdindex] = 0;
			candimulti_corrected[ssdindex] = 0;
			candimulti_2hit[ssdindex] = 0;
			candimulti_sharing[ssdindex] = 0;

			if (trackreconstruct->fTrackEvent.fSSDGlobalMulti[ssdindex] == globalmulti) {
				for (Int_t gmulti=0; gmulti<trackreconstruct->fTrackEvent.fGlobalMulti; gmulti++) {
					if (trackreconstruct->fTrackEvent.fGSSDNum[gmulti] == ssdindex) {
						L1L2_L2BStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti]);
						L1L2_L2FStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti]);
						L1L2_L1SStripNum[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SNumStrip[gmulti]);
						L1L2_L2FTime[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FTime[gmulti]);
						L1L2_L2BEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2BEMeV[gmulti]);
						L1L2_L2FEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL2FEMeV[gmulti]);
						L1L2_L1SEMeV[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]);

						fZone_index = fPID.GetZone(trackreconstruct->fTrackEvent.fGL2BNumStrip[gmulti], trackreconstruct->fTrackEvent.fGL2FNumStrip[gmulti], fZone_length);
						L1L2_L1SEMeV_Corrected[ssdindex].push_back(trackreconstruct->fTrackEvent.fGL1SEMeV[gmulti]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][fZone_index]));
					}
				}
			}

			// 填充数据
			if (L1L2_L2FEMeV[ssdindex].size() == globalmulti) {
				mode_exp = trackreconstruct->L1L2_CalcModeFromExpData(globalmulti,L1L2_L2BStripNum[ssdindex],L1L2_L2FStripNum[ssdindex],L1L2_L1SStripNum[ssdindex]);
				if (strcmp(mode_exp.c_str(),mode) == 0) {
					for (Int_t itrack=0; itrack<globalmulti; itrack++) {
						if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						{
							if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							{
								h2_L1L2_DEE[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);

								// 填充 mode statistic
								bin_index[itrack] = trackreconstruct->L1L2_GetSubModeFromEneConstraints(trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]),
																																												trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]));
								h1_modestat[ssdindex]->Fill(bin_index[itrack]);

								if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
										trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
								{
									candimulti[ssdindex]++;
								}
							}
						}
					}
					//____________________________________________________________________
					//           以下部分为当前 L1L2-globalomulti6-211 的径迹重建策略！！！
					//          -------------------------------------------------------
					//  0)  000         0) 000
					//  1)  010         1) 010
					//  2)  101         2) 100
					//  3)  111         3) 110
					//  4)  201         4) 201
					//  5)  211         5) 211
					//
					// L1S: 01-2345
					EL2B_Sum24 = L1L2_L2BEMeV[ssdindex][2] + L1L2_L2BEMeV[ssdindex][4];
					EL2B_Sum35 = L1L2_L2BEMeV[ssdindex][3] + L1L2_L2BEMeV[ssdindex][5];
					EL2B_Sum02 = L1L2_L2BEMeV[ssdindex][0] + L1L2_L2BEMeV[ssdindex][2];
					EL2B_Sum13 = L1L2_L2BEMeV[ssdindex][1] + L1L2_L2BEMeV[ssdindex][3];
					//
					if (L1L2_L1SStripNum[ssdindex][1]!=L1L2_L1SStripNum[ssdindex][2])
					{
						if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][2]-L1L2_L2BStripNum[ssdindex][4])==1) &&
					       (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum24,L1L2_L2FEMeV[ssdindex][2]) &&
							    trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][2], L1L2_L1SEMeV_Corrected[ssdindex][2])) &&
								 (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][1],L1L2_L2FEMeV[ssdindex][1]) &&
 							    trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][1], L1L2_L1SEMeV_Corrected[ssdindex][1])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==1 || itrack==2) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_sharing[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						else if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][3]-L1L2_L2BStripNum[ssdindex][5])==1) &&
					            (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum35,L1L2_L2FEMeV[ssdindex][3]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][3], L1L2_L1SEMeV_Corrected[ssdindex][3])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
 							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][0], L1L2_L1SEMeV_Corrected[ssdindex][0])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==0 || itrack==3) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_sharing[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
            //
						else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][0], L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][3],L1L2_L2FEMeV[ssdindex][3]) &&
 		 							     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][3], L1L2_L1SEMeV_Corrected[ssdindex][3])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==0 || itrack==3) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						//
						else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][0], L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][5],L1L2_L2FEMeV[ssdindex][5]) &&
 		 							     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][5], L1L2_L1SEMeV_Corrected[ssdindex][5])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==0 || itrack==5) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						//
						else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][1],L1L2_L2FEMeV[ssdindex][1]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][1], L1L2_L1SEMeV_Corrected[ssdindex][1])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][2],L1L2_L2FEMeV[ssdindex][2]) &&
 		 						      	trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][2], L1L2_L1SEMeV_Corrected[ssdindex][2])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==1 || itrack==2) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						//
					  else 	if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][1],L1L2_L2FEMeV[ssdindex][1]) &&
							          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][1], L1L2_L1SEMeV_Corrected[ssdindex][1])) &&
								       (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][4],L1L2_L2FEMeV[ssdindex][4]) &&
 		 							      trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][4], L1L2_L1SEMeV_Corrected[ssdindex][4])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==1 || itrack==4) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						else {
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										  break;
										}
									}
							  }
							}
						}
					}
					// L1S: 0123-45
					else if (L1L2_L1SStripNum[ssdindex][3]!=L1L2_L1SStripNum[ssdindex][4])
					{
						if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][0]-L1L2_L2BStripNum[ssdindex][2])==1) &&
					       (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum02,L1L2_L2FEMeV[ssdindex][0]) &&
							    trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][0], L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
								 (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][5],L1L2_L2FEMeV[ssdindex][5]) &&
 							    trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][5], L1L2_L1SEMeV_Corrected[ssdindex][5])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==0 || itrack==5) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_sharing[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						else if ( (TMath::Abs(L1L2_L2BStripNum[ssdindex][1]-L1L2_L2BStripNum[ssdindex][3])==1) &&
					            (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,EL2B_Sum13,L1L2_L2FEMeV[ssdindex][1]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][1], L1L2_L1SEMeV_Corrected[ssdindex][1])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][4],L1L2_L2FEMeV[ssdindex][4]) &&
 							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][4], L1L2_L1SEMeV_Corrected[ssdindex][4])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==1 || itrack==4) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_sharing[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
            //
						else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][0],L1L2_L2FEMeV[ssdindex][0]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][0], L1L2_L1SEMeV_Corrected[ssdindex][0])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][5],L1L2_L2FEMeV[ssdindex][5]) &&
 		 							     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][5], L1L2_L1SEMeV_Corrected[ssdindex][5])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==0 || itrack==5) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						//
						else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][1],L1L2_L2FEMeV[ssdindex][1]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][1], L1L2_L1SEMeV_Corrected[ssdindex][1])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][4],L1L2_L2FEMeV[ssdindex][4]) &&
 		 							     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][4], L1L2_L1SEMeV_Corrected[ssdindex][4])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==1 || itrack==4) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						//
						else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][2],L1L2_L2FEMeV[ssdindex][2]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][2], L1L2_L1SEMeV_Corrected[ssdindex][2])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][5],L1L2_L2FEMeV[ssdindex][5]) &&
 		 							     trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][5], L1L2_L1SEMeV_Corrected[ssdindex][5])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==2 || itrack==5) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						//
						else if ( (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][3],L1L2_L2FEMeV[ssdindex][3]) &&
							         trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][3], L1L2_L1SEMeV_Corrected[ssdindex][3])) &&
								      (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][4],L1L2_L2FEMeV[ssdindex][4]) &&
 		 						       trackreconstruct->IsInsideABananaCut(bananacut[ssdindex], L1L2_L2FEMeV[ssdindex][4], L1L2_L1SEMeV_Corrected[ssdindex][4])) )
						{
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
								if ( (itrack==3 || itrack==4) &&
							       (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]]) )
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
											candimulti_2hit[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										}
									}
							  }
							}
						}
						//
						else {
							for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							  if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
						    {
							    if (trackreconstruct->IsEneConstraint_L2B_L2F(ssdindex,L1L2_L2BEMeV[ssdindex][itrack],L1L2_L2FEMeV[ssdindex][itrack]) &&
						          trackreconstruct->IsInsideABananaCut(bananacut[ssdindex],L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]))
							    {
										if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							      {
											candimulti_corrected[ssdindex]++;
									    h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
										  break;
										}
									}
							  }
							}
						}
					}
					//______________________________________________________________________
					// 填充每一个 SSD 的 candidate multi
					h1_candimulti[ssdindex]->Fill(candimulti[ssdindex]);
					//
					if (candimulti_corrected[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								h1_SubmodeCorrected[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
					//
					if (candimulti_corrected[ssdindex] == 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								if (!IsStripsOnCsIGap(L1L2_L2BStripNum[ssdindex][itrack], L1L2_L2FStripNum[ssdindex][itrack]))
							  {
                  h2_L1L2_DEE_Discard[ssdindex].Fill(L1L2_L2FEMeV[ssdindex][itrack],L1L2_L1SEMeV_Corrected[ssdindex][itrack]);
								}
							}
						}
					}
					//
					if (candimulti_2hit[ssdindex] > 0) {
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								h1_Submode2Hit[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
					//
					if (candimulti_sharing[ssdindex] > 0) {
						Sharing_Entries[ssdindex]++;
						for (Int_t itrack=0; itrack<globalmulti; itrack++) {
							if (L1L2_L2FTime[ssdindex][itrack]>SiTimeCut_Low[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]] && L1L2_L2FTime[ssdindex][itrack]<SiTimeCut_Up[ssdindex][L1L2_L2FStripNum[ssdindex][itrack]])
							{
								h1_SubmodeSharing[ssdindex]->Fill(bin_index[itrack]);
							}
						}
					}
					//
				}
			}
		}
	}

	//
  TFile* fileout = new TFile(Form("/home/sea/Fission2019_Data/TrackDecoded/L1L2_g%d_0%s.root",globalmulti,mode),"RECREATE");
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE[ssdindex],Form("h2_DEE_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
  //
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex],Form("h2_GOOD_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }
	//
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    fileout->WriteTObject(&h2_L1L2_DEE_Discard[ssdindex],Form("h2_LOST_SSD%d_g%d_0%s",ssdindex+1,globalmulti,mode));
  }

	// 计算比例
  Int_t entry_all[NUM_SSD] = {0};
  Int_t entry_discard[NUM_SSD] = {0};
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    entry_all[ssdindex] = h2_L1L2_DEE[ssdindex].GetEntries();
    entry_discard[ssdindex] = h2_L1L2_DEE_Discard[ssdindex].GetEntries();

    lost_ratio[ssdindex]    = (Double_t) entry_discard[ssdindex]/entry_all[ssdindex] * 100;
		MHits_Ratio[ssdindex]   = (Double_t) MHits_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
    Sharing_Ratio[ssdindex] = (Double_t) Sharing_Entries[ssdindex] * globalmulti/entry_all[ssdindex] * 100;
  }

	// 定义 latex
  Double_t x1,y1,x2,y2;
  TLatex* latex_ratio[NUM_SSD];
  TLatex* latex_decoded[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    x1 = 0.4 * EL2Range[ssdindex];
    y1 = 0.8 * EL1Range[ssdindex];
    x2 = 0.2 * EL2Range[ssdindex];
    y2 = 0.5 * EL1Range[ssdindex];
    latex_ratio[ssdindex] = new TLatex(x1, y1, Form("r_{lost} = %.2f%%",lost_ratio[ssdindex]));
    latex_ratio[ssdindex]->SetTextSize(0.05);
    latex_ratio[ssdindex]->SetTextColor(kOrange);
    latex_decoded[ssdindex] = new TLatex(x2, y2, Form("r_{sh}=%.4f%%, r_{mh}=%.4f%%",Sharing_Ratio[ssdindex],MHits_Ratio[ssdindex]));
    latex_decoded[ssdindex]->SetTextSize(0.05);
    latex_decoded[ssdindex]->SetTextColor(kOrange);
  }

	// 定义 Legend
	Double_t x1_legend = 0.12, y1_legend = 0.7, x2_legend = 0.35, y2_legend = 0.89;
	TLegend* legend_SubMode[NUM_SSD];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		legend_SubMode[ssdindex] = new TLegend(x1_legend,y1_legend,x2_legend,y2_legend);
		legend_SubMode[ssdindex]->AddEntry(h1_modestat[ssdindex],"sub-modes","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeCorrected[ssdindex],"corrected","l");
		legend_SubMode[ssdindex]->AddEntry(h1_Submode2Hit[ssdindex],"2-hit","l");
		legend_SubMode[ssdindex]->AddEntry(h1_SubmodeSharing[ssdindex],"sharing","l");
	}

	// 画图
	TCanvas c_begin("c_begin","c_begin",600,600);
	TCanvas c_end("c_end","c_end",600,600);
	TCanvas cans("cans", "cans", 1500, 1000);
	cans.Divide(3,2);

	c_begin.Print(pathPDFOutBegin.c_str());
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		//
		cans.cd(1);
		h2_L1L2_DEE[ssdindex].Draw("COL");
		if (bananacut[ssdindex].size()>1) {
			for (Int_t ip=0; ip<bananacut[ssdindex].size(); ip++) {
				bananacut[ssdindex][ip]->Draw("SAME");
				bananacut[ssdindex][ip]->SetLineColor(kRed);
			}
		}
		//
		cans.cd(2);
		h2_L1L2_DEE_EneL2BL2F_EneL1SL2F[ssdindex].Draw("COL");
		//
		cans.cd(3);
		h2_L1L2_DEE_Discard[ssdindex].Draw("COL");
		latex_ratio[ssdindex]->Draw("SAME");
		latex_decoded[ssdindex]->Draw("SAME");

		//
		cans.cd(4);
		// candimulti_sum
		h1_candimulti[ssdindex]->Draw();
		h1_candimulti[ssdindex]->SetLineColor(kBlue);
		h1_candimulti[ssdindex]->SetLineWidth(2);
		h1_candimulti[ssdindex]->GetXaxis()->SetNdivisions(103);
		h1_candimulti[ssdindex]->GetYaxis()->SetRangeUser(0, 1.1*h1_candimulti[ssdindex]->GetMaximum());

		//
		cans.cd(5);
		h1_modestat[ssdindex]->Draw();
		h1_modestat[ssdindex]->GetXaxis()->SetNdivisions(104);
		h1_modestat[ssdindex]->GetYaxis()->SetRangeUser(0, 1.3*h1_modestat[ssdindex]->GetMaximum());
		h1_SubmodeCorrected[ssdindex]->Draw("SAME");
		h1_SubmodeCorrected[ssdindex]->SetLineColor(kRed);
		h1_SubmodeCorrected[ssdindex]->SetLineWidth(2);
		h1_Submode2Hit[ssdindex]->Draw("SAME");
		h1_Submode2Hit[ssdindex]->SetLineColor(kGreen);
		h1_Submode2Hit[ssdindex]->SetLineWidth(2);
		h1_SubmodeSharing[ssdindex]->Draw("SAME");
		h1_SubmodeSharing[ssdindex]->SetLineColor(kMagenta);
		h1_SubmodeSharing[ssdindex]->SetLineWidth(2);
		legend_SubMode[ssdindex]->Draw("SAME");


		gPad->Modified();
		gPad->Update();

		cans.Print(pathPDFOut.c_str());
	}
	c_end.Print(pathPDFOutEnd.c_str());
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
