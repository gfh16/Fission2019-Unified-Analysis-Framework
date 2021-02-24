//#define Test_Multi_cxx
#include "Test_Multi.h"
#include <iostream>

using namespace std;


//______________________________________________________________________________
Test_Multi::Test_Multi(Int_t firstrun, Int_t lastrun) : fChain(0)
{
  fFirstRun = firstrun;
  fLastRun = lastrun;

  std::string pathrootfilein(Form("/home/sea/Fission2019_Data/CSHINEEvent/EventTree_Run%04d-Run%04d.root", fFirstRun, fLastRun));

  TTree* tree = 0;
  TFile* f = (TFile*)gROOT->GetListOfFiles()->FindObject(pathrootfilein.c_str());
  if (!f || !f->IsOpen()) {
    f = new TFile(pathrootfilein.c_str());
  }
  f->GetObject("LayerEvent",tree);
  Init(tree);
}

Test_Multi::~Test_Multi()
{
  if (!fChain) return;
  // delete fChain->GetCurrentFile();
}


void Test_Multi::Init(TTree* tree)
{
  // Set branch addresses and branch pointers
  if (!tree) return;
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("LayerEvent.fL1SMulti",    &LayerEvent_fL1SMulti);
  fChain->SetBranchAddress("LayerEvent.fL1SSSDNum",   &LayerEvent_fL1SSSDNum);
  fChain->SetBranchAddress("LayerEvent.fL1SNumStrip", &LayerEvent_fL1SNumStrip);
  fChain->SetBranchAddress("LayerEvent.fL1SEMeV",     &LayerEvent_fL1SEMeV);
  fChain->SetBranchAddress("LayerEvent.fL2FMulti",    &LayerEvent_fL2FMulti);
  fChain->SetBranchAddress("LayerEvent.fL2FSSDNum",   &LayerEvent_fL2FSSDNum);
  fChain->SetBranchAddress("LayerEvent.fL2FNumStrip", &LayerEvent_fL2FNumStrip);
  fChain->SetBranchAddress("LayerEvent.fL2FEMeV",     &LayerEvent_fL2FEMeV);
  fChain->SetBranchAddress("LayerEvent.fL2BMulti",    &LayerEvent_fL2BMulti);
  fChain->SetBranchAddress("LayerEvent.fL2BSSDNum",   &LayerEvent_fL2BSSDNum);
  fChain->SetBranchAddress("LayerEvent.fL2BNumStrip", &LayerEvent_fL2BNumStrip);
  fChain->SetBranchAddress("LayerEvent.fL2BEMeV",     &LayerEvent_fL2BEMeV);
  fChain->SetBranchAddress("LayerEvent.fCsIMulti",    &LayerEvent_fCsIMulti);
  fChain->SetBranchAddress("LayerEvent.fCsISSDNum",   &LayerEvent_fCsISSDNum);
  fChain->SetBranchAddress("LayerEvent.fCsINum",      &LayerEvent_fCsINum);
  fChain->SetBranchAddress("LayerEvent.fCsIECh",      &LayerEvent_fCsIECh);
  fChain->SetBranchAddress("LayerEvent.fSSDL1SMulti", &LayerEvent_fSSDL1SMulti);
  fChain->SetBranchAddress("LayerEvent.fSSDL2FMulti", &LayerEvent_fSSDL2FMulti);
  fChain->SetBranchAddress("LayerEvent.fSSDL2BMulti", &LayerEvent_fSSDL2BMulti);
  fChain->SetBranchAddress("LayerEvent.fSSDCsIMulti", &LayerEvent_fSSDCsIMulti);
}

//______________________________________________________________________________


//______________________________________________________________________________
//               径迹重建前的一些测试与检查
//             ---------------------------
// L2F-L2B 能量关联
//------------------
void Test_Multi::CheckL2BL2FEnergyCorrelation(Int_t ssdindex)
{
  gStyle->SetOptStat(1);
  gStyle->SetPalette(1);

  std::string pathL2BL2FEneCorr_Cut(Form("figures/figure_TrackReconstruction/SSD%d_L2BL2F_EneCorrCut_Run120-Run130.png", ssdindex+1));
  std::string pathL2BL2FEneCorr_EnEratioMeV(Form("figures/figure_TrackReconstruction/SSD%d_L2BL2F_EneCorrEnEratioMeV_Run120-Run130.png", ssdindex+1));
  std::string pathL2BL2FEneCorr_EnEratioPer(Form("figures/figure_TrackReconstruction/SSD%d_L2BL2F_EneCorrEnEratioPer_Run120-Run130.png", ssdindex+1));

  Double_t L2FL2B_EnergyDiff_MeV;
  Double_t L2FL2B_EnergyDiff_Per;

  Double_t EnergyCut[4] = {0.05, 0.10, 0.15, 0.20};
  TH2D* hist_L2B_L2F[4];
  for (Int_t i=0; i<4; i++) {
    hist_L2B_L2F[i] = new TH2D(Form("hist_L2B_L2F_Cut%.2lf",EnergyCut[i]),Form("hist_L2B_L2F_Cut%.2lf",EnergyCut[i]),3000,0.,300.,3000,0.,300.);
  }

  TH2D* hist2_EnergyDifference_MeV = new TH2D("hist2_L2FL2B_EnEratio_MeV","",3000,0.,300.,1000,-50.,50.);
  TH2D* hist2_EnergyDifference_Per = new TH2D("hist2_L2FL2B_EnEratio_Per","",3000,0.,300.,600, -30., 30.);

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {

    fChain->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    if (LayerEvent_fSSDCsIMulti[ssdindex]==0) continue;
    if (LayerEvent_fSSDL2BMulti[ssdindex]==0) continue;
    if (LayerEvent_fSSDL2FMulti[ssdindex]==0) continue;
    if (LayerEvent_fSSDL1SMulti[ssdindex]==0) continue;

    for (Int_t i=0; i<4; i++) {
      for (Int_t l2b=0; l2b<LayerEvent_fL2BMulti; l2b++) {
        for (Int_t l2f=0; l2f<LayerEvent_fL2FMulti; l2f++) {
          if (fPattern.IsGeoConstraint_L3A_L2B(ssdindex, LayerEvent_fCsIMulti, LayerEvent_fCsINum.data(), LayerEvent_fCsISSDNum.data(),
                                               LayerEvent_fL2BNumStrip[l2b], LayerEvent_fL2BSSDNum[l2b]) &&
              fPattern.IsGeoConstraint_L3A_L2F(ssdindex, LayerEvent_fCsIMulti, LayerEvent_fCsINum.data(), LayerEvent_fCsISSDNum.data(),
                                               LayerEvent_fL2FNumStrip[l2f], LayerEvent_fL2FSSDNum[l2f]) &&
              fPattern.IsGeoConstraint_L2B_L1S(ssdindex, LayerEvent_fL2BNumStrip[l2b], LayerEvent_fL2BSSDNum[l2b],
                                               LayerEvent_fL1SMulti, LayerEvent_fL1SNumStrip.data(), LayerEvent_fL1SSSDNum.data()) &&
              fPattern.IsEneConstraint_L2B_L2F(LayerEvent_fL2BEMeV[l2b], LayerEvent_fL2FEMeV[l2f], EnergyCut[i]))
          {
            hist_L2B_L2F[i]->Fill(LayerEvent_fL2FEMeV[l2f], LayerEvent_fL2BEMeV[l2b]);

            L2FL2B_EnergyDiff_MeV = LayerEvent_fL2BEMeV[l2b] - LayerEvent_fL2FEMeV[l2f];
            hist2_EnergyDifference_MeV->Fill(LayerEvent_fL2FEMeV[l2f], L2FL2B_EnergyDiff_MeV);

            L2FL2B_EnergyDiff_Per = L2FL2B_EnergyDiff_MeV/LayerEvent_fL2FEMeV[l2f] * 100; // %
            hist2_EnergyDifference_Per->Fill(LayerEvent_fL2FEMeV[l2f], L2FL2B_EnergyDiff_Per);
          }
        }
      }
    }
  }

  TF1* func_L2B_eq_L2F = new TF1("func", "x", 0, 300);
  func_L2B_eq_L2F->SetLineColor(kRed);
  func_L2B_eq_L2F->SetLineStyle(7);
  func_L2B_eq_L2F->SetLineWidth(4);

  //________________________________________________________
  // 画出不同 EnergyCut 下的 L2B_L2F 能量关联
  TCanvas* cans = new TCanvas("cans", "cans", 1000, 1000);
  cans->Divide(2,2);
  for (Int_t i=0; i<4; i++) {
    cans->cd(i+1);
    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.12);
    gPad->SetTopMargin(0.12);
    hist_L2B_L2F[i]->GetXaxis()->SetTitle(Form("SSD%d_EL2F (MeV)", ssdindex+1));
    hist_L2B_L2F[i]->GetXaxis()->CenterTitle(1);
    hist_L2B_L2F[i]->GetXaxis()->SetTitleSize(0.05);
    hist_L2B_L2F[i]->GetXaxis()->SetLabelSize(0.05);

    hist_L2B_L2F[i]->GetYaxis()->SetTitle(Form("SSD%d_EL2B (MeV)", ssdindex+1));
    hist_L2B_L2F[i]->GetYaxis()->CenterTitle(1);
    hist_L2B_L2F[i]->GetYaxis()->SetTitleSize(0.05);
    hist_L2B_L2F[i]->GetYaxis()->SetLabelSize(0.05);

    hist_L2B_L2F[i]->Draw("COL");
    func_L2B_eq_L2F->Draw("SAME");
    gPad->Update();
    TPaveStats* st = (TPaveStats*)hist_L2B_L2F[i]->GetListOfFunctions()->FindObject("stats");
    st->SetX1NDC(0.55);
    st->SetX2NDC(0.85);
    st->SetY1NDC(0.15);
    st->SetY2NDC(0.45);
    gPad->Modified();
  }
  cans->Print(pathL2BL2FEneCorr_Cut.c_str());

  //_______________________________________________________________
  // 画出实验数据中的 EL2B-EL2F(MeV)的分布
  TCanvas* c_diff = new TCanvas("c_diff", "c_diff", 1200, 600);
  c_diff->Divide(2,1);
  c_diff->cd(1);
  gPad->SetLogz();
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.12);
  gPad->SetRightMargin(0.15);
  hist2_EnergyDifference_MeV->GetXaxis()->SetTitle(Form("SSD%d_EL2F (MeV)", ssdindex+1));
  hist2_EnergyDifference_MeV->GetXaxis()->CenterTitle(1);
  hist2_EnergyDifference_MeV->GetXaxis()->SetTitleSize(0.05);
  hist2_EnergyDifference_MeV->GetXaxis()->SetLabelSize(0.05);

  hist2_EnergyDifference_MeV->GetYaxis()->SetTitle("EL2B - EL2F (MeV)");
  hist2_EnergyDifference_MeV->GetYaxis()->CenterTitle(1);
  hist2_EnergyDifference_MeV->GetYaxis()->SetTitleSize(0.05);
  hist2_EnergyDifference_MeV->GetYaxis()->SetLabelSize(0.05);

  hist2_EnergyDifference_MeV->GetZaxis()->SetLabelSize(0.05);

  hist2_EnergyDifference_MeV->Draw("COLZ");
  gPad->Update();
  TPaveStats* st = (TPaveStats*)hist2_EnergyDifference_MeV->GetListOfFunctions()->FindObject("stats");
  st->SetX1NDC(0.16);
  st->SetX2NDC(0.37);
  st->SetY1NDC(0.65);
  st->SetY2NDC(0.88);
  st->Draw("SAME");
  gPad->Modified();

  TH2D* hist_clone = (TH2D*)hist2_EnergyDifference_MeV->Clone();
  TPad* subpad = new TPad("subpad", "", 0.15, 0.125, 0.42, 0.33);
  subpad->Draw();
  subpad->cd();
  gPad->SetFillStyle(0);
  gPad->SetLogz();
  gStyle->SetOptStat(0);
  hist_clone->GetXaxis()->SetTitle("");
  hist_clone->GetXaxis()->SetRangeUser(0., 50.);
  hist_clone->GetXaxis()->SetNdivisions(0);
  hist_clone->GetYaxis()->SetTitle("");
  hist_clone->GetYaxis()->SetRangeUser(-10, 10.);
  hist_clone->GetYaxis()->SetNdivisions(0);
  hist_clone->Draw("COL");

  // projectionY
  c_diff->cd(2);
  gPad->SetBottomMargin(0.12);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.15);
  TH1D* hist1_EnergyDifference_MeV = (TH1D*)hist2_EnergyDifference_MeV->ProjectionY();
  hist1_EnergyDifference_MeV->GetXaxis()->SetRangeUser(-20.,10);
  hist1_EnergyDifference_MeV->GetYaxis()->SetLabelSize(0.05);
  hist1_EnergyDifference_MeV->Draw();

  c_diff->Print(pathL2BL2FEneCorr_EnEratioMeV.c_str());


  //_______________________________________________________________
  // 画出实验数据中的 EL2B-EL2F(%)的分布
  TCanvas* c_per = new TCanvas("c_per", "c_per", 1200, 600);
  c_per->Divide(2,1);
  c_per->cd(1);
  gPad->SetLogz();
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.12);
  gPad->SetRightMargin(0.15);
  hist2_EnergyDifference_Per->GetXaxis()->SetTitle(Form("SSD%d_EL2F (MeV)", ssdindex+1));
  hist2_EnergyDifference_Per->GetXaxis()->CenterTitle(1);
  hist2_EnergyDifference_Per->GetXaxis()->SetTitleSize(0.05);
  hist2_EnergyDifference_Per->GetXaxis()->SetLabelSize(0.05);
  hist2_EnergyDifference_Per->GetXaxis()->SetNdivisions(508);
  hist2_EnergyDifference_Per->GetYaxis()->SetTitle("(EL2B - EL2F)/EL2F (%)");
  hist2_EnergyDifference_Per->GetYaxis()->CenterTitle(1);
  hist2_EnergyDifference_Per->GetYaxis()->SetTitleSize(0.05);
  hist2_EnergyDifference_Per->GetYaxis()->SetLabelSize(0.05);
  hist2_EnergyDifference_Per->GetZaxis()->SetLabelSize(0.05);
  hist2_EnergyDifference_Per->Draw("COLZ");

  TH2D* histper_clone = (TH2D*)hist2_EnergyDifference_Per->Clone();
  TPad* subpad_per = new TPad("subpad_per", "", 0.45, 0.45, 0.88, 0.93);
  subpad_per->Draw();
  subpad_per->cd();
  gPad->SetLogz();
  gPad->SetFillStyle(0);
  gStyle->SetOptStat(0);
  histper_clone->GetXaxis()->SetTitle("");
  histper_clone->GetXaxis()->SetRangeUser(0., 50.);
  histper_clone->GetXaxis()->SetNdivisions(0);
  histper_clone->GetYaxis()->SetTitle("");
  histper_clone->GetYaxis()->SetRangeUser(-20, 20.);
  histper_clone->GetYaxis()->SetNdivisions(0);
  histper_clone->Draw("COL");

  // projectionY
  TH1D* hist1_EnergyDifference_Per = (TH1D*)hist2_EnergyDifference_Per->ProjectionY();
  c_per->cd(2);
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.12);
  gPad->SetRightMargin(0.15);
  hist1_EnergyDifference_Per->GetYaxis()->SetLabelSize(0.05);
  hist1_EnergyDifference_Per->Draw();

  c_per->Print(pathL2BL2FEneCorr_EnEratioPer.c_str());
}

// 画出 L2B_L2F 能量关联的效果
void Test_Multi::DetermineL2BL2FEnergyErrRatio()
{
  gStyle->SetOptStat(1);
  gStyle->SetPalette(1);

  std::string pathL2BL2FEnergyCut("figures/figure_TrackReconstruction/SSD_L2BL2F_EnergyCut_Run120-Run130.png");

  TH2D* hist_L2B_L2F[4];
  for (Int_t i=0; i<4; i++) {
    hist_L2B_L2F[i] = new TH2D(Form("SSD%d_L2BL2F_EnergyCut",i+1),Form("SSD%d_L2BL2F_EnergyCut",i+1),3000,0.,300.,3000,0.,300.);
  }

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries;ientry++) {

    fChain->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      if (LayerEvent_fSSDCsIMulti[ssdindex]==0) continue;
      if (LayerEvent_fSSDL2BMulti[ssdindex]==0) continue;
      if (LayerEvent_fSSDL2FMulti[ssdindex]==0) continue;
      if (LayerEvent_fSSDL1SMulti[ssdindex]==0) continue;

      for (Int_t l2b=0; l2b<LayerEvent_fL2BMulti; l2b++) {
        for (Int_t l2f=0; l2f<LayerEvent_fL2FMulti; l2f++) {
          if (fPattern.IsGeoConstraint_L3A_L2B(ssdindex, LayerEvent_fCsIMulti, LayerEvent_fCsINum.data(), LayerEvent_fCsISSDNum.data(),
                                               LayerEvent_fL2BNumStrip[l2b], LayerEvent_fL2BSSDNum[l2b]) &&
              fPattern.IsGeoConstraint_L3A_L2F(ssdindex, LayerEvent_fCsIMulti, LayerEvent_fCsINum.data(), LayerEvent_fCsISSDNum.data(),
                                               LayerEvent_fL2FNumStrip[l2f], LayerEvent_fL2FSSDNum[l2f]) &&
              fPattern.IsGeoConstraint_L2B_L1S(ssdindex, LayerEvent_fL2BNumStrip[l2b], LayerEvent_fL2BSSDNum[l2b],
                                               LayerEvent_fL1SMulti, LayerEvent_fL1SNumStrip.data(), LayerEvent_fL1SSSDNum.data()) &&
              fPattern.IsEneConstraint_L2B_L2F(ssdindex, LayerEvent_fL2BEMeV[l2b], LayerEvent_fL2FEMeV[l2f]))
          {
            hist_L2B_L2F[ssdindex]->Fill(LayerEvent_fL2FEMeV[l2f], LayerEvent_fL2BEMeV[l2b]);
          }
        }
      }
    }
  }

  TF1* func_L2B_eq_L2F = new TF1("func", "x", 0, 300);
  func_L2B_eq_L2F->SetLineColor(kRed);
  func_L2B_eq_L2F->SetLineStyle(7);
  func_L2B_eq_L2F->SetLineWidth(4);

  //________________________________________________________
  // 画出不同 EnergyCut 下的 L2B_L2F 能量关联
  TCanvas* cans = new TCanvas("cans", "cans", 1000, 1000);
  cans->Divide(2,2);

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    cans->cd(ssdindex+1);
    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.12);
    gPad->SetTopMargin(0.12);
    hist_L2B_L2F[ssdindex]->GetXaxis()->SetTitle(Form("SSD%d_EL2F (MeV)", ssdindex+1));
    hist_L2B_L2F[ssdindex]->GetXaxis()->CenterTitle(1);
    hist_L2B_L2F[ssdindex]->GetXaxis()->SetTitleSize(0.05);
    hist_L2B_L2F[ssdindex]->GetXaxis()->SetLabelSize(0.05);

    hist_L2B_L2F[ssdindex]->GetYaxis()->SetTitle(Form("SSD%d_EL2B (MeV)", ssdindex+1));
    hist_L2B_L2F[ssdindex]->GetYaxis()->CenterTitle(1);
    hist_L2B_L2F[ssdindex]->GetYaxis()->SetTitleSize(0.05);
    hist_L2B_L2F[ssdindex]->GetYaxis()->SetLabelSize(0.05);

    hist_L2B_L2F[ssdindex]->Draw("COL");
    func_L2B_eq_L2F->Draw("SAME");
    gPad->Update();
    TPaveStats* st1 = (TPaveStats*)hist_L2B_L2F[ssdindex]->GetListOfFunctions()->FindObject("stats");
    st1->SetX1NDC(0.55);
    st1->SetX2NDC(0.85);
    st1->SetY1NDC(0.15);
    st1->SetY2NDC(0.45);
    gPad->Modified();
  }
  cans->Print(pathL2BL2FEnergyCut.c_str());
}


//________________________________
// L1S-L2F 能量关联
//------------------
// LISE++ 计算不同粒子在每套硅望远镜两层硅中的能损
void Test_Multi::CheckEnergyLossL1L2()
{
  EnergyLossModule eloss;
  Double_t Eloss_L1[16];
  Double_t Eloss_L2[16];
  Double_t Eres_L2 [16];

  for (Int_t issd=0; issd<4; issd++) {
    std::string pathDataOut(Form("data/data_Test_Multi/SSD%d_L1L2ELossTest.dat", issd+1));
    ofstream fileout(pathDataOut.c_str());
    fileout<<setw(5)<<"Z"<<setw(5)<<"A"<<setw(10)<<"Einc(MeV)"<<setw(12)<<"ELossL1"
           <<setw(12)<<"ELossL2"<<setw(12)<<"ERes"<<endl;

    for (Int_t ip=0; ip<16; ip++) {
      for (Double_t einc=0.; einc<400.; einc+=0.1) {
        Eloss_L1[ip] = eloss.GetEnergyLoss(Z_A_LCPs[ip][0], Z_A_LCPs[ip][1], einc, "Si", SIL1THICKNESS[issd]*1000, 1);
        Eloss_L2[ip] = eloss.GetEnergyLoss(Z_A_LCPs[ip][0], Z_A_LCPs[ip][1], einc-Eloss_L1[ip], "Si", SIL2THICKNESS[issd]*1000, 1);
        Eres_L2 [ip] = einc - Eloss_L1[ip] - Eloss_L2[ip];
        fileout<<setw(5)<<Z_A_LCPs[ip][0]<<setw(5)<<Z_A_LCPs[ip][1]<<setw(10)<<einc
               <<setw(12)<<Eloss_L1[ip]<<setw(12)<<Eloss_L2[ip]<<setw(12)<<Eres_L2[ip]<<endl;

        cout<<setw(5)<<Z_A_LCPs[ip][0]<<setw(5)<<Z_A_LCPs[ip][1]<<setw(10)<<einc
            <<setw(12)<<Eloss_L1[ip]<<setw(12)<<Eloss_L2[ip]<<setw(12)<<Eres_L2[ip]<<endl;
      }
    }
  }
}

// 提取 LISE++ 计算的数据, 画出 DE-E Plot
// 目的: 为了确定粒子穿透 L2 到达 L3 时, EL1 与 EL2 的比值
void Test_Multi::CheckEnergyLossL1L2_Relationship(Bool_t punchthrough)
{
  gStyle->SetOptStat(0);

  std::string pathE1E2Ratio = "figures/figure_TrackReconstruction/SSD_E1E2Ratio.png";
  std::string pathE1E2Correlation;
  if (punchthrough) pathE1E2Correlation = "figures/figure_TrackReconstruction/SSD_E1E2Correlation_punchthrough.png";
  else pathE1E2Correlation = "figures/figure_TrackReconstruction/SSD_E1E2Correlation.png";

  std::vector<Double_t> dE1_SSD1;
  std::vector<Double_t> dE2_SSD1;
  std::vector<Double_t> dE1_SSD2;
  std::vector<Double_t> dE2_SSD2;
  std::vector<Double_t> dE1_SSD3;
  std::vector<Double_t> dE2_SSD3;
  std::vector<Double_t> dE1_SSD4;
  std::vector<Double_t> dE2_SSD4;

  std::vector<Double_t> EL1EL2Ratio_SSD1;
  std::vector<Double_t> EL1EL2Ratio_SSD2;
  std::vector<Double_t> EL1EL2Ratio_SSD3;
  std::vector<Double_t> EL1EL2Ratio_SSD4;

  std::string pathSSDELossInput[4];
  for (Int_t i=0; i<4; i++) {
    pathSSDELossInput[i] = Form("data/data_Test_Multi/SSD%d_L1L2ELossTest.dat", i+1);

    ifstream in;
    in.open(pathSSDELossInput[i].c_str());
    if(!in.is_open()) {
      printf("Error: file %s not found\n",pathSSDELossInput[i].c_str());
      return NULL;
    }
    std::vector<Double_t> dE1;
    std::vector<Double_t> dE2;
    std::vector<Double_t> EL1EL2Ratio;
    while(in.good()) {
      // 按行读取数据
      std::string LineRead;
      std::getline(in, LineRead);
      LineRead.assign(LineRead.substr(0, LineRead.find('*')));
      if(LineRead.empty()) continue;
      if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
      std::istringstream LineStream(LineRead);

      Int_t Z;
      Int_t A;
      Double_t Einc;
      Double_t dEL1;
      Double_t dEL2;
      Double_t Eres;
      LineStream>>Z>>A>>Einc>>dEL1>>dEL2>>Eres;
      if (punchthrough==kTRUE && Eres==0) continue;  // 只考虑能穿透第二层硅的情况
      dE1.push_back(dEL1);
      dE2.push_back(dEL2);
      EL1EL2Ratio.push_back(dEL1/dEL2 * 100);
    }
    if (i==0) { dE1_SSD1=dE1;  dE2_SSD1=dE2; EL1EL2Ratio_SSD1=EL1EL2Ratio; };
    if (i==1) { dE1_SSD2=dE1;  dE2_SSD2=dE2; EL1EL2Ratio_SSD2=EL1EL2Ratio; };
    if (i==2) { dE1_SSD3=dE1;  dE2_SSD3=dE2; EL1EL2Ratio_SSD3=EL1EL2Ratio; };
    if (i==3) { dE1_SSD4=dE1;  dE2_SSD4=dE2; EL1EL2Ratio_SSD4=EL1EL2Ratio; };
  }

  TH2D* hist2_SSD_dEE[NUM_SSD];
  TH1D* hist1_SSD_Eratio[NUM_SSD];
  for (Int_t ssd=0; ssd<NUM_SSD; ssd++) {
    hist2_SSD_dEE[ssd] = new TH2D(Form("hist2_SSD%d_DEE_LISE++", ssd+1), Form("hist2_SSD%d_DEE_LISE++", ssd+1),3000,0.,300.,1500,0.,150.);
    hist1_SSD_Eratio[ssd] = new TH1D(Form("hist1_SSD%d_Eratio_LISE++", ssd+1), Form("hist1_SSD%d_Eratio_LISE++", ssd+1), 500, 0., 50.);
  }

  for (Int_t i=0; i<dE1_SSD1.size(); i++) {
    hist2_SSD_dEE[0]->Fill(dE2_SSD1[i], dE1_SSD1[i]);
    hist2_SSD_dEE[1]->Fill(dE2_SSD2[i], dE1_SSD2[i]);
    hist2_SSD_dEE[2]->Fill(dE2_SSD3[i], dE1_SSD3[i]);
    hist2_SSD_dEE[3]->Fill(dE2_SSD4[i], dE1_SSD4[i]);
    cout<<EL1EL2Ratio_SSD1[i]<<endl;
    hist1_SSD_Eratio[0]->Fill(EL1EL2Ratio_SSD1[i]);
    hist1_SSD_Eratio[1]->Fill(EL1EL2Ratio_SSD2[i]);
    hist1_SSD_Eratio[2]->Fill(EL1EL2Ratio_SSD3[i]);
    hist1_SSD_Eratio[3]->Fill(EL1EL2Ratio_SSD4[i]);
  }

  TF1* func_L2B_eq_L2F = new TF1("func", "x", 0, 300);
  func_L2B_eq_L2F->SetLineColor(kRed);
  func_L2B_eq_L2F->SetLineStyle(7);
  func_L2B_eq_L2F->SetLineWidth(4);

  //________________________
  // 画出 DEE 曲线
  TCanvas* cans = new TCanvas("cans","cans",1000,1000);
  cans->Divide(2,2);
  //________________________
  // 画出 (EL2-EL1)/EL2 分布
  TCanvas* c_Eratio = new TCanvas("c_Eratio","c_Eratio",1000,1000);
  c_Eratio->Divide(2,2);

  for (Int_t index=0; index<4; index++) {
    cans->cd(index+1);
    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.12);
    hist2_SSD_dEE[index]->Draw("COL");
    hist2_SSD_dEE[index]->GetXaxis()->SetTitle("dE_{2} (MeV)");
    hist2_SSD_dEE[index]->GetXaxis()->SetTitleSize(0.05);
    hist2_SSD_dEE[index]->GetXaxis()->CenterTitle(1);
    hist2_SSD_dEE[index]->GetXaxis()->SetLabelSize(0.05);
    hist2_SSD_dEE[index]->GetXaxis()->SetNdivisions(508);
    hist2_SSD_dEE[index]->GetYaxis()->SetTitle("dE_{1} (MeV)");
    hist2_SSD_dEE[index]->GetYaxis()->SetTitleSize(0.05);
    hist2_SSD_dEE[index]->GetYaxis()->CenterTitle(1);
    hist2_SSD_dEE[index]->GetYaxis()->SetLabelSize(0.05);
    hist2_SSD_dEE[index]->GetYaxis()->SetNdivisions(508);
    func_L2B_eq_L2F->Draw("SAME");

    c_Eratio->cd(index+1);
    gPad->SetLeftMargin(0.17);
    gPad->SetBottomMargin(0.14);
    hist1_SSD_Eratio[index]->Draw();
    hist1_SSD_Eratio[index]->GetXaxis()->SetTitle("dE1/dE2 (%)");
    hist1_SSD_Eratio[index]->GetXaxis()->SetTitleSize(0.05);
    hist1_SSD_Eratio[index]->GetXaxis()->CenterTitle(1);
    hist1_SSD_Eratio[index]->GetXaxis()->SetLabelSize(0.05);
    hist1_SSD_Eratio[index]->GetXaxis()->SetNdivisions(505);
    hist1_SSD_Eratio[index]->GetYaxis()->SetTitle("Counts");
    hist1_SSD_Eratio[index]->GetYaxis()->SetTitleSize(0.05);
    hist1_SSD_Eratio[index]->GetYaxis()->CenterTitle(1);
    hist1_SSD_Eratio[index]->GetYaxis()->SetLabelSize(0.05);
    hist1_SSD_Eratio[index]->GetYaxis()->SetNdivisions(505);
  }
  c_Eratio->Print(pathE1E2Ratio.c_str());
  cans->Print(pathE1E2Correlation.c_str());
}


// 根据实验数据画出 L1-L2 的能量关联
void Test_Multi::CheckEnergyLossL1L2_Expdata()
{
  Double_t EnergyCut = 0.20;
  Double_t ratio_E1E2[4];

  std::string pathPNGOut("figures/figure_TrackReconstruction/SSD2_DEL1DEL2_Ratio_Expdata.png");

  TH1D* hist[4];
  for (Int_t i=0; i<4; i++) {
    hist[i] = new TH1D(Form("hist_SSD%d",i+1),"",100,0.,100.);
  }

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries;ientry++) {

    fChain->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      if (LayerEvent_fSSDCsIMulti[ssdindex]==0 || LayerEvent_fSSDCsIMulti[ssdindex]>MULTICUT_L3A) continue;
      if (LayerEvent_fSSDL2BMulti[ssdindex]==0 || LayerEvent_fSSDL2BMulti[ssdindex]>MULTICUT_L2B) continue;
      if (LayerEvent_fSSDL2FMulti[ssdindex]==0 || LayerEvent_fSSDL2FMulti[ssdindex]>MULTICUT_L2F) continue;
      if (LayerEvent_fSSDL1SMulti[ssdindex]==0 || LayerEvent_fSSDL1SMulti[ssdindex]>MULTICUT_L1S) continue;

      for (Int_t l2f=0; l2f<LayerEvent_fL2FMulti; l2f++) {
        for (Int_t l1s=0; l1s<LayerEvent_fL1SMulti; l1s++) {
          if (fPattern.IsGeoConstraint_L3A_L2B(ssdindex, LayerEvent_fCsIMulti, LayerEvent_fCsINum.data(), LayerEvent_fCsISSDNum.data(),
                                               LayerEvent_fL2BMulti, LayerEvent_fL2BNumStrip.data(), LayerEvent_fL2BSSDNum.data()) &&
              fPattern.IsGeoConstraint_L3A_L2F(ssdindex, LayerEvent_fCsIMulti, LayerEvent_fCsINum.data(), LayerEvent_fCsISSDNum.data(),
                                               LayerEvent_fL2FNumStrip[l2f], LayerEvent_fL2FSSDNum[l2f]) &&
              fPattern.IsEneConstraint_L2B_L2F(ssdindex, LayerEvent_fL2BMulti, LayerEvent_fL2BEMeV.data(), LayerEvent_fL2BSSDNum.data(),
                                               LayerEvent_fL2FNumStrip[l2f], LayerEvent_fL2BSSDNum[l2f], EnergyCut) &&
              fPattern.IsGeoConstraint_L2B_L1S(ssdindex, LayerEvent_fL2BMulti, LayerEvent_fL2BNumStrip.data(), LayerEvent_fL2BSSDNum.data(),
                                               LayerEvent_fL1SNumStrip[l1s], LayerEvent_fL1SSSDNum[l1s]))
          {
            if (ssdindex==3 && (LayerEvent_fL2FNumStrip[l2f]==0 || LayerEvent_fL2FNumStrip[l2f]==1)) continue;
            ratio_E1E2[ssdindex] = LayerEvent_fL1SEMeV[l1s]/LayerEvent_fL2FEMeV[l2f]*100;  // 单位: %
            hist[ssdindex]->Fill(ratio_E1E2[ssdindex]);
          }
        }
      }
    }
  }

  TCanvas* cans = new TCanvas("cans","cans",1200,1000);
  cans->Divide(2,2);

  for (Int_t index=0; index<4; index++) {
    cans->cd(index+1);
    gPad->SetLeftMargin(0.13);
    gPad->SetBottomMargin(0.12);
    hist[index]->Draw();
    hist[index]->GetXaxis()->SetTitle("EL1S/EL2F (%)");
    hist[index]->GetXaxis()->SetTitleSize(0.05);
    hist[index]->GetXaxis()->CenterTitle(kTRUE);
    hist[index]->GetXaxis()->SetLabelSize(0.05);
    hist[index]->GetXaxis()->SetNdivisions(505);
    hist[index]->GetYaxis()->SetTitle("Counts");
    hist[index]->GetYaxis()->SetTitleSize(0.05);
    hist[index]->GetYaxis()->CenterTitle(kTRUE);
    hist[index]->GetYaxis()->SetTitleOffset(1.2);
    hist[index]->GetYaxis()->SetLabelSize(0.05);
    hist[index]->GetYaxis()->SetNdivisions(1005);
  }
  cans->Print(pathPNGOut.c_str());
}


// 多重性测试
void PrintMulti(Int_t ssdindex, Int_t num_multi, string layertag, std::ofstream& fileout,
  Double_t* multiratio, Double_t sumratio)
{
  cout<<setw(5)<<Form("SSD%d_%s",ssdindex+1, layertag.c_str());
  fileout<<setw(5)<<Form("SSD%d_%s",ssdindex+1, layertag.c_str());
  for (Int_t multi=0; multi<num_multi; multi++) {
    cout<<setw(15)<<multiratio[multi]<<setw(15);
    fileout<<setw(15)<<multiratio[multi]<<setw(15);
  }
  cout<<sumratio<<endl;
  fileout<<sumratio<<endl;
}

void Test_Multi::CheckLayerMultiPercentage()
{
  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string L3ATag("L3A");

  Int_t Num_Multi = 7; // hit = 0, 1, 2, 3, 4, 5, 6

  std::string pathDataOut("data/data_TrackReconstruction/SSD_CheckLayerMultiPercentage.dat");

  ofstream fileout(pathDataOut.c_str());
  fileout<<setw(5)<<"SSD_Layer"<<setw(14)<<"Multi=0"<<setw(14)<<"Multi=1"
         <<setw(15)<<"Multi=2"<<setw(15)<<"Multi=3"<<setw(15)<<"Multi=4"
         <<setw(15)<<"Multi=5"<<setw(15)<<"Multi=6"<<setw(12)<<"Sum"<<endl;
  fileout<<""<<endl;

  Int_t Entries_L1S[NUM_SSD][Num_Multi];
  Int_t Entries_L2F[NUM_SSD][Num_Multi];
  Int_t Entries_L2B[NUM_SSD][Num_Multi];
  Int_t Entries_L3A[NUM_SSD][Num_Multi];
  Double_t MultiRatio_L1S[NUM_SSD][Num_Multi];
  Double_t MultiRatio_L2F[NUM_SSD][Num_Multi];
  Double_t MultiRatio_L2B[NUM_SSD][Num_Multi];
  Double_t MultiRatio_L3A[NUM_SSD][Num_Multi];
  Double_t Sum_L1S[NUM_SSD];
  Double_t Sum_L2F[NUM_SSD];
  Double_t Sum_L2B[NUM_SSD];
  Double_t Sum_L3A[NUM_SSD];

  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<Num_Multi; j++) {
      Entries_L1S[i][j] = 0;  MultiRatio_L1S[i][j] = 0.;
      Entries_L2F[i][j] = 0;  MultiRatio_L2F[i][j] = 0.;
      Entries_L2B[i][j] = 0;  MultiRatio_L2B[i][j] = 0.;
      Entries_L3A[i][j] = 0;  MultiRatio_L3A[i][j] = 0.;
    }
    Sum_L1S[i] = 0.;
    Sum_L2F[i] = 0.;
    Sum_L2B[i] = 0.;
    Sum_L3A[i] = 0.;
  }

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries;ientry++) {

    fChain->GetEntry(ientry);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      for (Int_t multi=0; multi<Num_Multi; multi++) {
        if (LayerEvent_fSSDL1SMulti[ssdindex]==multi)  Entries_L1S[ssdindex][multi]++;
        if (LayerEvent_fSSDL2FMulti[ssdindex]==multi)  Entries_L2F[ssdindex][multi]++;
        if (LayerEvent_fSSDL2BMulti[ssdindex]==multi)  Entries_L2B[ssdindex][multi]++;
        if (LayerEvent_fSSDCsIMulti[ssdindex]==multi)  Entries_L3A[ssdindex][multi]++;
      }
    }
  }
  // 计算各种情况的比例
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t multi=0; multi<Num_Multi; multi++) {
      MultiRatio_L1S[ssdindex][multi] = (Double_t) Entries_L1S[ssdindex][multi]/nentries;
      Sum_L1S[ssdindex] += MultiRatio_L1S[ssdindex][multi];

      MultiRatio_L2F[ssdindex][multi] = (Double_t) Entries_L2F[ssdindex][multi]/nentries;
      Sum_L2F[ssdindex] += MultiRatio_L2F[ssdindex][multi];

      MultiRatio_L2B[ssdindex][multi] = (Double_t) Entries_L2B[ssdindex][multi]/nentries;
      Sum_L2B[ssdindex] += MultiRatio_L2B[ssdindex][multi];

      MultiRatio_L3A[ssdindex][multi] = (Double_t) Entries_L3A[ssdindex][multi]/nentries;
      Sum_L3A[ssdindex] += MultiRatio_L3A[ssdindex][multi];
    }
  }
  // 输入结果
  cout<<setw(5)<<"SSD_Layer"<<setw(14)<<"Multi=0"<<setw(14)<<"Multi=1"
      <<setw(15)<<"Multi=2"<<setw(15)<<"Multi=3"<<setw(15)<<"Multi=4"
      <<setw(15)<<"Multi=5"<<setw(15)<<"Multi=6"<<setw(12)<<"Sum"<<endl;
  cout<<endl;

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    PrintMulti(ssdindex, Num_Multi, L1STag.c_str(), fileout, MultiRatio_L1S[ssdindex], Sum_L1S[ssdindex]);
    PrintMulti(ssdindex, Num_Multi, L2FTag.c_str(), fileout, MultiRatio_L2F[ssdindex], Sum_L2F[ssdindex]);
    PrintMulti(ssdindex, Num_Multi, L2BTag.c_str(), fileout, MultiRatio_L2B[ssdindex], Sum_L2B[ssdindex]);
    PrintMulti(ssdindex, Num_Multi, L3ATag.c_str(), fileout, MultiRatio_L3A[ssdindex], Sum_L3A[ssdindex]);
    cout<<endl;
    fileout<<endl;
  }
  fileout.close();
}



//______________________________________________________________________________
//                电荷分配效应
//             -----------------
void Test_Multi::CheckChargeSharingEffect_L1(Int_t ssdindex)
{}



//______________________________________________________________________________
//               径迹重建 与 相关检测
//            ----------------------
// 径迹重建算法
//______________________________________________________________________
// 自定义径迹查找算法1 : 循环-循环-判断-循环-判断-循环-判断
// 自定义径迹查找算法2 : 循环-循环-循环-循环-判断-判断-判断-判断
// 经验证, 算法2 与 算法1 等效 !
// 算法2 更加简洁明了，为方便起见，使用算法2进行后续分析
// 这一步生成 TrackEvent，便于后续的模式识别使用
//
// 为了不引起混乱, 本函数仅用于生成 Track 事件, 对于径迹重建的检查
// 将由下一个函数 CheckTrackReconstructionResult() 来完成
void Test_Multi::TrackReconstructionAlgorithm()
{
  Int_t count_satisfiedmulticut[4] = {0}; // 满足初步 multi cut 计数
  Int_t count_effentries[4] = {0};        // globalmulti>0 的计数

  TimeAndPercentage timeper;

  std::string pathTrackEventRoot(Form("/home/sea/Fission2019_Data/TrackReconstructionEvent_Run%04d-Run%04d.root", fFirstRun, fLastRun));

  ofstream  FileOut[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    FileOut[i].open(Form("data/data_Test_Multi/SSD%d_Test_Multi_L1L2EnergyCut.dat",i+1));

    FileOut[i]<<setw(7)<<"EntryID"<<setw(15)<<"MultiGlobal"
              <<setw(11)<<"M_L3A"<<setw(11)<<"M_L2B"<<setw(11)<<"M_L2F"<<setw(11)<<"M_L1S"
              <<setw(11)<<"CSI"<<setw(11)<<"L2B"<<setw(11)<<"L2F"<<setw(11)<<"L1S"
              <<setw(13)<<"EL1S"<<setw(13)<<"EL2F"<<setw(13)<<"EL2B"<<setw(13)<<"ECsI"
              <<endl;
  }

  TFile* myfile = new TFile(pathTrackEventRoot.c_str(),"RECREATE");
  TTree* mytree = new TTree("TrackEvent","TrackEvent Tree");
  mytree->Branch("TrackEvent.", "CSHINETrackEvent", &fTrackEvent, 32000, 2);

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {

    fChain->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    Int_t globalmulti = 0 ;
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      Int_t ssdglobalmulti = 0;

      if ((LayerEvent_fSSDCsIMulti[ssdindex]>0 && LayerEvent_fSSDCsIMulti[ssdindex]<=MULTICUT_L3A) &&
          (LayerEvent_fSSDL2BMulti[ssdindex]>0 && LayerEvent_fSSDL2BMulti[ssdindex]<=MULTICUT_L2B) &&
          (LayerEvent_fSSDL2FMulti[ssdindex]>0 && LayerEvent_fSSDL2FMulti[ssdindex]<=MULTICUT_L2F) &&
          (LayerEvent_fSSDL1SMulti[ssdindex]>0 && LayerEvent_fSSDL1SMulti[ssdindex]<=MULTICUT_L1S)) // 给定初步的 multi cut
      {
        count_satisfiedmulticut[ssdindex]++;

        for (Int_t csimulti=0; csimulti<LayerEvent_fCsIMulti; csimulti++) {
          for (Int_t l2bmulti=0; l2bmulti<LayerEvent_fL2BMulti; l2bmulti++) {
            for (Int_t l2fmulti=0; l2fmulti<LayerEvent_fL2FMulti; l2fmulti++) {
              for (Int_t l1smulti=0; l1smulti<LayerEvent_fL1SMulti; l1smulti++) { // 逐层循环,遍历所有可能的组合

                if (LayerEvent_fCsISSDNum[csimulti]==ssdindex && LayerEvent_fL2BSSDNum[l2bmulti]==ssdindex &&
                    LayerEvent_fL2FSSDNum[l2fmulti]==ssdindex && LayerEvent_fL1SSSDNum[l1smulti]==ssdindex)  // 对每个SSD分开讨论
                {
                  if (fPattern.IsGeoConstraint_L3A_L2B(LayerEvent_fCsINum[csimulti], LayerEvent_fL2BNumStrip[l2bmulti]) &&
                      fPattern.IsGeoConstraint_L3A_L2F(LayerEvent_fCsINum[csimulti], LayerEvent_fL2FNumStrip[l2fmulti]) &&
                      fPattern.IsEneConstraint_L2B_L2F(ssdindex, LayerEvent_fL2BEMeV[l2bmulti], LayerEvent_fL2FEMeV[l2fmulti]) &&
                      fPattern.IsGeoConstraint_L2B_L1S(LayerEvent_fL2BNumStrip[l2bmulti], LayerEvent_fL1SNumStrip[l1smulti]) &&
                      fPattern.IsEneConstraint_L1S_L2F(ssdindex, LayerEvent_fL1SEMeV[l1smulti], LayerEvent_fL2FEMeV[l2fmulti]))
                  {
                    globalmulti++;
                    ssdglobalmulti++;

                    fTrackEvent.fGSSDNum.push_back(ssdindex);
                    fTrackEvent.fGCsINum.push_back(LayerEvent_fCsINum[csimulti]);
                    fTrackEvent.fGCsIECh.push_back(LayerEvent_fCsIECh[csimulti]);
                    fTrackEvent.fGL2BNumStrip.push_back(LayerEvent_fL2BNumStrip[l2bmulti]);
                    fTrackEvent.fGL2BEMeV.push_back(LayerEvent_fL2BEMeV[l2bmulti]);
                    fTrackEvent.fGL2FNumStrip.push_back(LayerEvent_fL2FNumStrip[l2fmulti]);
                    fTrackEvent.fGL2FEMeV.push_back(LayerEvent_fL2FEMeV[l2fmulti]);
                    fTrackEvent.fGL1SNumStrip.push_back(LayerEvent_fL1SNumStrip[l1smulti]);
                    fTrackEvent.fGL1SEMeV.push_back(LayerEvent_fL1SEMeV[l1smulti]);

                    FileOut[ssdindex]<<setw(7)<<ientry<<setw(10)<<ssdglobalmulti
                    <<setw(13)<<LayerEvent_fSSDCsIMulti[ssdindex]
                    <<setw(11)<<LayerEvent_fSSDL2BMulti[ssdindex]
                    <<setw(11)<<LayerEvent_fSSDL2FMulti[ssdindex]
                    <<setw(11)<<LayerEvent_fSSDL1SMulti[ssdindex]
                    <<setw(11)<<LayerEvent_fCsINum[csimulti]
                    <<setw(11)<<LayerEvent_fL2BNumStrip[l2bmulti]
                    <<setw(11)<<LayerEvent_fL2FNumStrip[l2fmulti]
                    <<setw(11)<<LayerEvent_fL1SNumStrip[l1smulti]
                    <<setw(14)<<LayerEvent_fL1SEMeV[l1smulti]
                    <<setw(14)<<LayerEvent_fL2FEMeV[l2fmulti]
                    <<setw(14)<<LayerEvent_fL2BEMeV[l2bmulti]
                    <<setw(14)<<LayerEvent_fCsIECh[csimulti]
                    <<endl;
                  }
                }
              }
            }
          }
        }
      }
      fTrackEvent.fSSDGlobalMulti.push_back(ssdglobalmulti);
      if (ssdglobalmulti>0) count_effentries[ssdindex]++;
    }
    fTrackEvent.fGlobalMulti = globalmulti;
    mytree->Fill();
    fTrackEvent.Swap();
  }
  mytree->Write();
  myfile->Close();

  cout<<endl;
  for (Int_t i=0; i<NUM_SSD; i++) {
    cout<<Form("count_satisfiedmulticut[%d] = ",i+1)<<count_satisfiedmulticut[i]<<setw(30)
        <<Form("count_effentries[%d] = ",i+1)<<count_effentries[i]<<setw(20)
        <<std::setprecision(4)<<(Double_t) count_effentries[i]/count_satisfiedmulticut[i]
        <<endl;
  }
}


//______________________________________________
// 检查径迹算法:
// 1.检查径迹识别效率: fglobalmulti>=1 的事件数/满足 multicut 的事件数
// 2.event-by-evnt 检查径迹重建的有效性
//    2.1 MultiCutData: 列举满足 multicut 的事件
//    2.2 TrackData: 列举满足 multicut && 满足所有约束条件的事件
void Test_Multi::CheckTrackReconstructionResult()
{
  Int_t count_multicut[4] = {0};
  Int_t count_track[4] = {0};
  Int_t count_index[4];
  Bool_t Geo_L3AL2B = kFALSE;
  Bool_t Geo_L3AL2F = kFALSE;
  Bool_t Geo_L2BL1S = kFALSE;
  Bool_t Ene_L2BL2F = kFALSE;
  Bool_t Ene_L1SL2F = kFALSE;

  std::string pathTrackEventRoot("/home/sea/Fission2019_Data/TrackReconstructionEvent_Run0120-Run0130.root");

  ofstream MultiCutDataOut[NUM_SSD];
  ofstream TrackDataOut[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    MultiCutDataOut[ssdindex].open(Form("data/data_Test_Multi/SSD%d_MultiCutData.dat", ssdindex+1));
    TrackDataOut[ssdindex].open(Form("data/data_Test_Multi/SSD%d_TrackData.dat", ssdindex+1));

    MultiCutDataOut[ssdindex]<<setw(7)<<"EntryID"<<setw(15)<<"MultiGlobal"
        <<setw(10)<<"M_L3A"<<setw(10)<<"M_L2B"<<setw(10)<<"M_L2F"<<setw(11)<<"M_L1S"
        <<setw(15)<<"Geo_L3AL2B"<<setw(15)<<"Geo_L3AL2F"<<setw(15)<<"Ene_L2BL2F"
        <<setw(15)<<"Geo_L2BL1S"<<setw(15)<<"Ene_L1SL2F"<<endl;
    TrackDataOut[ssdindex]<<setw(7)<<"EntryID"<<setw(15)<<"MultiGlobal"
        <<setw(10)<<"M_L3A"<<setw(10)<<"M_L2B"<<setw(10)<<"M_L2F"<<setw(11)<<"M_L1S"
        <<setw(15)<<"Geo_L3AL2B"<<setw(15)<<"Geo_L3AL2F"<<setw(15)<<"Ene_L2BL2F"
        <<setw(15)<<"Geo_L2BL1S"<<setw(15)<<"Ene_L1SL2F"<<endl;
  }

  TFile* myfile = new TFile(pathTrackEventRoot.c_str());
  if (!myfile || !myfile->IsOpen()) {
    cout<<Form("File %s not founded.",  myfile->GetName());
    return;
  }

  TTree* mytree = (TTree*)myfile->Get("TrackEvent");
  Long64_t nentries = mytree->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  std::vector<Int_t> TrackEvent_fSSDGlobalMulti;
  mytree->SetMakeClass(1);
  mytree->SetBranchStatus("*", kFALSE);
  mytree->SetBranchStatus("TrackEvent.fSSDGlobalMulti", kTRUE);
  mytree->SetBranchAddress("TrackEvent.fSSDGlobalMulti", &TrackEvent_fSSDGlobalMulti);

  for (Long64_t ientry=0; ientry<nentries; ientry++) {

    mytree->GetEntry(ientry);
    fChain->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      if ((LayerEvent_fSSDCsIMulti[ssdindex]>0 && LayerEvent_fSSDCsIMulti[ssdindex]<=MULTICUT_L3A) &&
          (LayerEvent_fSSDL2BMulti[ssdindex]>0 && LayerEvent_fSSDL2BMulti[ssdindex]<=MULTICUT_L2B) &&
          (LayerEvent_fSSDL2FMulti[ssdindex]>0 && LayerEvent_fSSDL2FMulti[ssdindex]<=MULTICUT_L2F) &&
          (LayerEvent_fSSDL1SMulti[ssdindex]>0 && LayerEvent_fSSDL1SMulti[ssdindex]<=MULTICUT_L1S))
      {
        count_multicut[ssdindex]++;
        count_index[ssdindex] = 0;

        for (Int_t csimulti=0; csimulti<LayerEvent_fCsIMulti; csimulti++) {
          for (Int_t l2bmulti=0; l2bmulti<LayerEvent_fL2BMulti; l2bmulti++) {
            for (Int_t l2fmulti=0; l2fmulti<LayerEvent_fL2FMulti; l2fmulti++) {
              for (Int_t l1smulti=0; l1smulti<LayerEvent_fL1SMulti; l1smulti++) { // 逐层循环,遍历所有可能的组合

                if (LayerEvent_fCsISSDNum[csimulti]==ssdindex && LayerEvent_fL2BSSDNum[l2bmulti]==ssdindex &&
                    LayerEvent_fL2FSSDNum[l2fmulti]==ssdindex && LayerEvent_fL1SSSDNum[l1smulti]==ssdindex)  // 对每个SSD分开讨论
                {
                  Geo_L3AL2B = fPattern.IsGeoConstraint_L3A_L2B(LayerEvent_fCsINum[csimulti], LayerEvent_fL2BNumStrip[l2bmulti]);
                  Geo_L3AL2F = fPattern.IsGeoConstraint_L3A_L2F(LayerEvent_fCsINum[csimulti], LayerEvent_fL2FNumStrip[l2fmulti]);
                  Geo_L2BL1S = fPattern.IsGeoConstraint_L2B_L1S(LayerEvent_fL2BNumStrip[l2bmulti], LayerEvent_fL1SNumStrip[l1smulti]);
                  Ene_L2BL2F = fPattern.IsEneConstraint_L2B_L2F(ssdindex, LayerEvent_fL2BEMeV[l2bmulti], LayerEvent_fL2FEMeV[l2fmulti]);
                  Ene_L1SL2F = fPattern.IsEneConstraint_L1S_L2F(ssdindex, LayerEvent_fL1SEMeV[l1smulti], LayerEvent_fL2FEMeV[l2fmulti]);
                  if (Geo_L3AL2B && Geo_L3AL2F && Geo_L2BL1S && Ene_L2BL2F && Ene_L1SL2F)
                  {
                    count_index[ssdindex]++;
                  }
                }
              }
            }
          }
        }
        if (count_index[ssdindex]>0) {
          count_track[ssdindex]++;
        }
        else {

        }
      }
    }
  }
  cout<<endl;
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    Double_t eff_ratio = (Double_t) count_track[ssdindex]/count_multicut[ssdindex];
    cout<<count_multicut[ssdindex]<<setw(20)<<count_track[ssdindex]<<setw(20)
        <<std::setprecision(4)<<eff_ratio<<endl;
  }
}


//______________________________________________________________________
// 分析不同 fGlobalMulti 下的 fGlobalMulti 的比例
// 这一步的分析依赖于 TrackReconstructionAlgorithm() 生成的 TrackEvent 文件
void Test_Multi::CheckGlobalMultiRatio(Int_t ssdindex)
{
  // 定义变量， 以用于计算不同 fGlobalMulti 的比例
  Int_t    nummulti = 7;  // 考察 globalmulti<nummulti 的情况
  Int_t    globalmulti[7]  = {0};
  Double_t multiratio_0[7] = {0.};
  Double_t sumratio_0      =  0.;
  Double_t multiratio_1[7] = {0.};
  Double_t sumratio_1      =  0.;
  Long64_t count = 0;

  TFile* myfile = new TFile("/home/sea/Fission2019_Data/TrackReconstructionEvent_Run0120-Run0130.root","READONLY");
  if (!myfile->IsOpen()) {
    cout<<"Open file failded."<<endl;
    return;
  }

  Int_t                   TrackEvent_fGlobalMulti;
  std::vector<Int_t>      TrackEvent_fSSDGlobalMulti;
  std::vector<Int_t>      TrackEvent_fGSSDNum;
  std::vector<Int_t>      TrackEvent_fGL1SNumStrip;
  std::vector<Double_t>   TrackEvent_fGL1SEMeV;
  std::vector<Int_t>      TrackEvent_fGL2FNumStrip;
  std::vector<Double_t>   TrackEvent_fGL2FEMeV;
  std::vector<Int_t>      TrackEvent_fGL2BNumStrip;
  std::vector<Double_t>   TrackEvent_fGL2BEMeV;
  std::vector<Int_t>      TrackEvent_fGCsINum;
  std::vector<Int_t>      TrackEvent_fGCsIECh;

  TTree* mytree = (TTree*)myfile->Get("TrackEvent");
  Long64_t nentries = mytree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  mytree->SetMakeClass(1);  // 如果 tree 的 branch 使用了自定义的类, 则这条语句不能省略！！！
  mytree->SetBranchAddress("TrackEvent.fGlobalMulti",    &TrackEvent_fGlobalMulti);
  mytree->SetBranchAddress("TrackEvent.fSSDGlobalMulti", &TrackEvent_fSSDGlobalMulti);
  mytree->SetBranchAddress("TrackEvent.fGSSDNum",        &TrackEvent_fGSSDNum);
  mytree->SetBranchAddress("TrackEvent.fGL1SNumStrip",   &TrackEvent_fGL1SNumStrip);
  mytree->SetBranchAddress("TrackEvent.fGL1SEMeV",       &TrackEvent_fGL1SEMeV);
  mytree->SetBranchAddress("TrackEvent.fGL2FNumStrip",   &TrackEvent_fGL2FNumStrip);
  mytree->SetBranchAddress("TrackEvent.fGL2FEMeV",       &TrackEvent_fGL2FEMeV);
  mytree->SetBranchAddress("TrackEvent.fGL2BNumStrip",   &TrackEvent_fGL2BNumStrip);
  mytree->SetBranchAddress("TrackEvent.fGL2BEMeV",       &TrackEvent_fGL2BEMeV);
  mytree->SetBranchAddress("TrackEvent.fGCsINum",        &TrackEvent_fGCsINum);
  mytree->SetBranchAddress("TrackEvent.fGCsIECh",        &TrackEvent_fGCsIECh);

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    mytree->GetEntry(ientry);

    //___________________________________________
    // 统计不同 fGlobalMult 的计数
    for (Int_t i=0; i<nummulti; i++) {
      if (TrackEvent_fSSDGlobalMulti[ssdindex]==i)   globalmulti[i]++;
    }
  }

  // 计算比例，从 globalmulti = 0 算起
  for (Int_t i=0; i<nummulti; i++) {
    multiratio_0[i] = (Double_t) globalmulti[i]/nentries;
    sumratio_0 += multiratio_0[i];
    cout<<Form("multi%d", i)<<" "<<multiratio_0[i]<<setw(12);
  }
  cout<<sumratio_0<<endl;

  // 计算比例，从 globalmulti = 1 算起
  for (Int_t i=1; i<nummulti; i++) {
    multiratio_1[i] = (Double_t) globalmulti[i]/(nentries-globalmulti[0]); // 剔除 fGlobalMulti=0 的情况
    sumratio_1 += multiratio_1[i];
    cout<<Form("multi%d", i)<<" "<<multiratio_1[i]<<setw(12);
    count += globalmulti[i];
  }
  cout<<sumratio_1<<endl;
  cout<<"count = "<<count<<endl;

  myfile->Close();
}

void Test_Multi::GlobalMulti_ExtractData(Int_t globalmulti)
{}

void Test_Multi::CheckParallelDraw(const char* fglobalmulti)
{}

void Test_Multi::CheckL2L3DEEPlot(const char* fglobalmulti)
{}

void Test_Multi::CheckL1L2DEEPlot(const char* fglobalmulti)
{}



//______________________________________________________________________________
//                  模式识别
//               --------------
//_________________________________________
// 定义 hit 的模式
// 用一个四位数字符串 index 来区分不同的模式
// 4个数从左往右,分别表示 L3A,L2B,L2F,L1S 的情况
// 每个数取值为 0 或 1
// index 的形式如: 1011, 0101
void Test_Multi::ChangeModeToNumber(std::string mode, Int_t& l3a, Int_t& l2b, Int_t& l2f, Int_t& l1s)
{
  // 将字符串 mode 转成成 int 数字
  Int_t index;
  std::stringstream ss;
  ss<<mode;
  ss>>index;

  l3a =  index/1000;
  l2b = (index%1000)/100;
  l2f = (index%100)/10;
  l1s =  index%10;
}

//___________________________________
// 判断 ch1 与 ch2 是否满足模式规则
Bool_t Test_Multi::IsModeMatched(Int_t mode_index, Int_t ch1, Int_t ch2)
{
  //return (((ch1==ch2) ? kTRUE : kFALSE)==mode_index) ? kTRUE : kFALSE;
  return ((ch1==ch2) == mode_index);
}
