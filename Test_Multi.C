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
            //if (ssdindex==3 && (LayerEvent_fL2FNumStrip[l2f]==0 || LayerEvent_fL2FNumStrip[l2f]==1)) continue;
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
//____________________________________
// 检查实验数据中: 如果硅条中有相邻的条点火, 则定义这样相邻条的数目为 ClusterSize
/* //这一段用于检查每种 clustersize 的情况
if (ClusterSize_L2F==2) {
  cout<<"SSD"<<ssdindex<<setw(10)<<LayerEvent_fSSDL2FMulti[ssdindex]<<setw(10)<<ClusterSize_L2F<<setw(10);
  for (Int_t i=0; i<LayerEvent_fL2FMulti; i++) {
    if (LayerEvent_fL2FSSDNum[i]==ssdindex) {
      cout<<setw(10)<<LayerEvent_fL2FNumStrip[i];
    }
  }
  cout<<endl;
} */
void Test_Multi::CheckClusterSize_Si()
{
  Int_t NBins = 2000;
  Int_t Count_ClusterSize_L1S;
  Int_t Count_ClusterSize_L2F;
  Int_t Count_ClusterSize_L2B;
  Int_t CountSum_ClusterSize_L1S;
  Int_t CountSum_ClusterSize_L2F;
  Int_t CountSum_ClusterSize_L2B;

  Double_t Ratio_ClusterSize_L1S;
  Double_t Ratio_ClusterSize_L2F;
  Double_t Ratio_ClusterSize_L2B;
  Double_t RatioSum_ClusterSize_L1S;
  Double_t RatioSum_ClusterSize_L2F;
  Double_t RatioSum_ClusterSize_L2B;

  Int_t Count_L1SMulti_ge1[4] = {0};
  Int_t Count_L2FMulti_ge1[4] = {0};
  Int_t Count_L2BMulti_ge1[4] = {0};

  Int_t ClusterSize_L1S = -99;
  Int_t ClusterSize_L2F = -99;
  Int_t ClusterSize_L2B = -99;

  Int_t count_L1S[4] = {0};
  Int_t count_L2F[4] = {0};
  Int_t count_L2B[4] = {0};

  std::vector<Int_t> NumStrip_L1S;
  std::vector<Int_t> NumStrip_L2F;
  std::vector<Int_t> NumStrip_L2B;

  std::string HistName_L1S[NUM_SSD];
  std::string HistName_L2F[NUM_SSD];
  std::string HistName_L2B[NUM_SSD];
  TH1I* hist_ClusterSize_L1S[NUM_SSD];
  TH1I* hist_ClusterSize_L2F[NUM_SSD];
  TH1I* hist_ClusterSize_L2B[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    HistName_L1S[ssdindex] = Form("SSD%d_L1S_ClusterSize",ssdindex+1);
    HistName_L2F[ssdindex] = Form("SSD%d_L2F_ClusterSize",ssdindex+1);
    HistName_L2B[ssdindex] = Form("SSD%d_L2B_ClusterSize",ssdindex+1);
    hist_ClusterSize_L1S[ssdindex] = new TH1I(HistName_L1S[ssdindex].c_str(), HistName_L1S[ssdindex].c_str(), NBins, 0, NBins);
    hist_ClusterSize_L2F[ssdindex] = new TH1I(HistName_L2F[ssdindex].c_str(), HistName_L2F[ssdindex].c_str(), NBins, 0, NBins);
    hist_ClusterSize_L2B[ssdindex] = new TH1I(HistName_L2B[ssdindex].c_str(), HistName_L2B[ssdindex].c_str(), NBins, 0, NBins);
  }

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries;ientry++) {

    fChain->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      if (LayerEvent_fSSDL1SMulti[ssdindex]>=1) Count_L1SMulti_ge1[ssdindex]++;
      if (LayerEvent_fSSDL2FMulti[ssdindex]>=1) Count_L2FMulti_ge1[ssdindex]++;
      if (LayerEvent_fSSDL2BMulti[ssdindex]>=1) Count_L2BMulti_ge1[ssdindex]++;

      // for L1S
      if (LayerEvent_fSSDL1SMulti[ssdindex]>=2 && LayerEvent_fSSDL1SMulti[ssdindex]<=MULTICUT_L1S) {
        for (Int_t l1smulti=0; l1smulti<LayerEvent_fL1SMulti; l1smulti++) {
          if (LayerEvent_fL1SSSDNum[l1smulti]==ssdindex) {
            NumStrip_L1S.push_back(LayerEvent_fL1SNumStrip[l1smulti]);
          }
        }
        CalcCluseterSize_SiLayer(LayerEvent_fSSDL1SMulti[ssdindex], NumStrip_L1S, ClusterSize_L1S);
        hist_ClusterSize_L1S[ssdindex]->Fill(ClusterSize_L1S);
      }

      // for L2F
      if (LayerEvent_fSSDL2FMulti[ssdindex]>=2 && LayerEvent_fSSDL2FMulti[ssdindex]<=MULTICUT_L2F) {
        for (Int_t l2fmulti=0; l2fmulti<LayerEvent_fL2FMulti; l2fmulti++) {
          if (LayerEvent_fL2FSSDNum[l2fmulti]==ssdindex) {
            NumStrip_L2F.push_back(LayerEvent_fL2FNumStrip[l2fmulti]);
          }
        }
        CalcCluseterSize_SiLayer(LayerEvent_fSSDL2FMulti[ssdindex], NumStrip_L2F, ClusterSize_L2F);
        hist_ClusterSize_L2F[ssdindex]->Fill(ClusterSize_L2F);
      }
      // for L2B
      if (LayerEvent_fSSDL2BMulti[ssdindex]>=2 && LayerEvent_fSSDL2BMulti[ssdindex]<=MULTICUT_L2B) {
        for (Int_t l2bmulti=0; l2bmulti<LayerEvent_fL2BMulti; l2bmulti++) {
          if (LayerEvent_fL2BSSDNum[l2bmulti]==ssdindex) {
            NumStrip_L2B.push_back(LayerEvent_fL2BNumStrip[l2bmulti]);
          }
        }
        CalcCluseterSize_SiLayer(LayerEvent_fSSDL2BMulti[ssdindex], NumStrip_L2B, ClusterSize_L2B);
        hist_ClusterSize_L2B[ssdindex]->Fill(ClusterSize_L2B);
      }
      NumStrip_L1S.clear();
      NumStrip_L2F.clear();
      NumStrip_L2B.clear();
    }
  }

  // 统计 ClusterSize 的取值与比例
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    CountSum_ClusterSize_L1S = 0;
    CountSum_ClusterSize_L2F = 0;
    CountSum_ClusterSize_L2B = 0;
    RatioSum_ClusterSize_L1S = 0.;
    RatioSum_ClusterSize_L2F = 0.;
    RatioSum_ClusterSize_L2B = 0.;
    // L1S
    cout<<endl;
    cout<<Form("SSD%d_L1S\n", ssdindex+1);
    for (Int_t nbin=1; nbin<NBins+1; nbin++) {
      Count_ClusterSize_L1S = hist_ClusterSize_L1S[ssdindex]->GetBinContent(nbin);
      if (Count_ClusterSize_L1S!=0) {
        Ratio_ClusterSize_L1S = (Double_t) Count_ClusterSize_L1S/Count_L1SMulti_ge1[ssdindex];
        cout<<left<<setw(30)<<Form("ClusterSize = %d", nbin-1)<<left<<setw(30)<<Form("count = %d",Count_ClusterSize_L1S);
        cout<<setprecision(4)<<left<<setw(10)<<"Ratio = "<<Ratio_ClusterSize_L1S<<endl;
        CountSum_ClusterSize_L1S += Count_ClusterSize_L1S;
        RatioSum_ClusterSize_L1S += Ratio_ClusterSize_L1S;
      }
    }
    cout<<"Count_L1SMulti_ge1 = "<<Count_L1SMulti_ge1[ssdindex]<<endl;
    cout<<"CountSum_L1S = "<<CountSum_ClusterSize_L1S<<endl;
    cout<<"RatioSum_L1S = "<<RatioSum_ClusterSize_L1S<<endl;

    // L2F
    cout<<endl;
    cout<<Form("SSD%d_L2F\n", ssdindex+1);
    for (Int_t nbin=1; nbin<NBins+1; nbin++) {
      Count_ClusterSize_L2F = hist_ClusterSize_L2F[ssdindex]->GetBinContent(nbin);
      if (Count_ClusterSize_L2F!=0) {
        Ratio_ClusterSize_L2F = (Double_t) Count_ClusterSize_L2F/Count_L2FMulti_ge1[ssdindex];
        cout<<left<<setw(30)<<Form("ClusterSize = %d", nbin-1)<<left<<setw(30)<<Form("count = %d",Count_ClusterSize_L2F);
        cout<<setprecision(4)<<left<<setw(10)<<"Ratio = "<<Ratio_ClusterSize_L2F<<endl;
        CountSum_ClusterSize_L2F += Count_ClusterSize_L2F;
        RatioSum_ClusterSize_L2F += Ratio_ClusterSize_L2F;
      }
    }
    cout<<"Count_L2FMulti_ge1 = "<<Count_L2FMulti_ge1[ssdindex]<<endl;
    cout<<"CountSum_L2F = "<<CountSum_ClusterSize_L2F<<endl;
    cout<<"RatioSum_L2F = "<<RatioSum_ClusterSize_L2F<<endl;
    // L2B
    cout<<endl;
    cout<<Form("SSD%d_L2B\n", ssdindex+1);
    for (Int_t nbin=1; nbin<NBins+1; nbin++) {
      Count_ClusterSize_L2B = hist_ClusterSize_L2B[ssdindex]->GetBinContent(nbin);
      if (Count_ClusterSize_L2B!=0) {
        Ratio_ClusterSize_L2B = (Double_t) Count_ClusterSize_L2B/Count_L2BMulti_ge1[ssdindex];
        cout<<left<<setw(30)<<Form("ClusterSize = %d", nbin-1)<<left<<setw(30)<<Form("count = %d",Count_ClusterSize_L2B);
        cout<<setprecision(4)<<left<<setw(10)<<"Ratio = "<<Ratio_ClusterSize_L2B<<endl;
        CountSum_ClusterSize_L2B += Count_ClusterSize_L2B;
        RatioSum_ClusterSize_L2B += Ratio_ClusterSize_L2B;
      }
    }
    cout<<"Count_L2BMulti_ge1 = "<<Count_L2BMulti_ge1[ssdindex]<<endl;
    cout<<"CountSum_L2B = "<<CountSum_ClusterSize_L2B<<endl;
    cout<<"RatioSum_L2B = "<<RatioSum_ClusterSize_L2B<<endl;
  }

  TCanvas* cans_L1S = new TCanvas("cans_L1S", "cans_L1S", 1000, 1000);
  TCanvas* cans_L2F = new TCanvas("cans_L2F", "cans_L2F", 1000, 1000);
  TCanvas* cans_L2B = new TCanvas("cans_L2B", "cans_L2B", 1000, 1000);
  cans_L1S->Divide(2,2);
  cans_L2F->Divide(2,2);
  cans_L2B->Divide(2,2);
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    cans_L1S->cd(ssdindex+1);
    gPad->SetLogy();
    hist_ClusterSize_L1S[ssdindex]->Draw();

    cans_L2F->cd(ssdindex+1);
    gPad->SetLogy();
    hist_ClusterSize_L2F[ssdindex]->Draw();

    cans_L2B->cd(ssdindex+1);
    gPad->SetLogy();
    hist_ClusterSize_L2B[ssdindex]->Draw();
  }
}

//_________________________
// 对 cluster 进行分类
void Test_Multi::CalcCluseterSize_SiLayer(Int_t layermulti, vector<Int_t> numstrip, Int_t& clustersize)
{
  // layermulti = 2 的情况
  if (layermulti==2) {
    if (numstrip[0]-numstrip[1] == -1)        clustersize = 2;  // ---**------
    else                                      clustersize = 0;  // ---*--*----
  }
  // layermulti = 3 的情况:
  if (layermulti==3) {
    if (numstrip[0]-numstrip[2] == -2)        clustersize = 3; //----***------
    else if (numstrip[0]-numstrip[1] == -1)   clustersize = 2; //--**---*-----
    else if (numstrip[1]-numstrip[2] == -1)   clustersize = 2; //--*---**-----
    else                                      clustersize = 0; //---*--*----*--
  }
  // layermulti = 4 的情况
  if (layermulti==4) {
    if (numstrip[0]-numstrip[3] == -3)        clustersize = 4; //----****------
    else if (numstrip[0]-numstrip[2] == -2)   clustersize = 3; //---***--*-----
    else if (numstrip[1]-numstrip[3] == -2)   clustersize = 3; //---*---***----
    else if (numstrip[0]-numstrip[1] == -1) {
      if (numstrip[2]-numstrip[3] == -1)      clustersize = 22;//---**---**---
      else                                    clustersize = 2; //---**--*--*--
    }
    else if (numstrip[1]-numstrip[2] == -1)   clustersize = 2; //---*--**---*--
    else if (numstrip[2]-numstrip[3] == -1)   clustersize = 2; //---*--*--**---
    else                                      clustersize = 0; //--*--*--*--*--
  }
  // layermulti = 5 的情况
  if (layermulti==5) {
    if (numstrip[0]-numstrip[4] == -4)        clustersize = 5; //---*****-----
    else if (numstrip[0]-numstrip[3] == -3)   clustersize = 4; //---****--*---
    else if (numstrip[1]-numstrip[4] == -3)   clustersize = 4; //---*--****---
    else if (numstrip[0]-numstrip[2] == -2) {
      if (numstrip[3]-numstrip[4] == -1)      clustersize = 23;//---***--**---
      else                                    clustersize = 3; //--***--*--*--
    }
    else if (numstrip[1]-numstrip[3] == -2)   clustersize = 3; //--*--***-*---
    else if (numstrip[2]-numstrip[4] == -2) {
      if (numstrip[0]-numstrip[1] == -1)      clustersize = 23;//--**--***----
      else                                    clustersize = 3; //--*--*--***--
    }
    else if (numstrip[0]-numstrip[1] == -1) {
      if ((numstrip[2]-numstrip[3] == -1) || (numstrip[3]-numstrip[4] == -1))  clustersize = 122; //---**--**--*-- or ---**--*--**---
      else                                    clustersize = 2; //---**--*--*--*--
    }
    else if (numstrip[1]-numstrip[2] == -1) {
      if (numstrip[3]-numstrip[4] == -1)      clustersize = 122;  //--*--**---**--
      else                                    clustersize = 2; //--*--**--*--*--
    }
    else if (numstrip[2]-numstrip[3] == -1)   clustersize = 2; //--*--*--**--*--
    else if (numstrip[3]-numstrip[4] == -1)   clustersize = 2; //--*--*--*--**--
    else                                      clustersize = 0; //--*--*--*--*--*--
  }
  // layermulti = 6 的情况
  if (layermulti==6) {
    if (numstrip[0]-numstrip[5] == -5)        clustersize = 6; //---******----
    else if (numstrip[0]-numstrip[4] == -4)   clustersize = 5; //---*****--*--
    else if (numstrip[1]-numstrip[5] == -4)   clustersize = 5; //---*--*****--
    else if (numstrip[0]-numstrip[3] == -3) {
      if (numstrip[4]-numstrip[5] == -1)      clustersize = 24;//---****--**--
      else                                    clustersize = 4; //---****--*--*--
    }
    else if (numstrip[1]-numstrip[4] == -3)   clustersize = 4; //---*--****--*--
    else if (numstrip[2]-numstrip[5] == -3) {
      if (numstrip[0]-numstrip[1] == -1)      clustersize = 24;//--**--****--
      else                                    clustersize = 4; //--*--*--****--
    }
    else if (numstrip[0]-numstrip[2] == -2) {
      if (numstrip[3]-numstrip[5] == -2)      clustersize = 33; //---***---***--
      else if ((numstrip[3]-numstrip[4] == -1) || (numstrip[3]-numstrip[4] == -1)) clustersize = 123; //--***--**--*-- or --***--*--**--
      else                                    clustersize = 3; //--***--*--*--*--
    }
    else if (numstrip[1]-numstrip[3] == -2) {
      if (numstrip[4]-numstrip[5] == -1)      clustersize = 123; //--*--***--**--
      else                                    clustersize = 3; //---*--***--*--*--
    }
    else if (numstrip[2]-numstrip[4] == -2) {
      if (numstrip[0]-numstrip[1] == -1)      clustersize = 123; //--**---***--*--
      else                                    clustersize = 3; //--*--*--***--*--
    }
    else if (numstrip[3]-numstrip[5] == -2) {
      if ((numstrip[0]-numstrip[1] == -1) || (numstrip[1]-numstrip[2] == -1))  clustersize = 123; //--**--*--***-- or --*--**--***--
      else                                    clustersize = 3; //--*--*--*--***--
    }
    else if (numstrip[0]-numstrip[1] == -1) {
      if (numstrip[2]-numstrip[3] == -1) {
        if (numstrip[4]-numstrip[5] == -1)   clustersize = 222; //--**--**--**--
        else                                 clustersize = 1122;//--**--**--*--*--
      }
      else if (numstrip[3]-numstrip[4] == -1)  clustersize = 1122; //--**--*--**-*--
      else if (numstrip[4]-numstrip[5] == -1)  clustersize = 1122; //--**--*--*--**--
      else                                     clustersize = 2; //--**--*--*--*--*--
    }
    else if  (numstrip[1]-numstrip[2] == -1) {
      if (numstrip[3]-numstrip[4] == -1)       clustersize = 1122; //--*--**--**--*--
      else if (numstrip[4]-numstrip[5] == -1)  clustersize = 1122; //--*--**--*--**--
      else                                     clustersize = 2; //--*--**--*--*--*--
    }
    else if (numstrip[2]-numstrip[3] == -1) {
      if (numstrip[4]-numstrip[5] == -1)      clustersize = 1122; //--*--*--**--**--
      else                                    clustersize = 2; //--*--*--**--*--*--
    }
    else if (numstrip[3]-numstrip[4] == -1)   clustersize = 2; //--*--*--*--**--*--
    else if (numstrip[4]-numstrip[5] == -1)   clustersize = 2; //--*--*--*--*--**--
    else                                      clustersize = 0; //--*--*--*--*--*--*--
  }
}

//__________________________________________________
// 经过以上 CheckClusterSize_Si() 的分析, charge charing
// 只需考虑 clustersize=2 的情况即可 ！
Double_t CalcRatio(Double_t e1, Double_t e2)
{
  return (e1 > e2 ? e2/e1 : e1/e2);
}
void Test_Multi::CalcClusterSize_Equal2_ERatio()
{
  Int_t strip[2];
  Double_t efound[2];
  Int_t ClusterSize_L2F = -99;

  Double_t ERatio_L1S = -99.;
  Double_t ERatio_L2F = -99.;
  Double_t ERatio_L2B = -99.;
  std::vector<Int_t> NumStrip_L1S;
  std::vector<Int_t> NumStrip_L2F;
  std::vector<Int_t> NumStrip_L2B;
  std::vector<Double_t> EMeV_L1S;
  std::vector<Double_t> EMeV_L2F;
  std::vector<Double_t> EMeV_L2B;

  std::string L1S_ERatio[NUM_SSD];
  std::string L2F_ERatio[NUM_SSD];
  std::string L2B_ERatio[NUM_SSD];
  std::string L1S_ECorr[NUM_SSD];
  std::string L2F_ECorr[NUM_SSD];
  std::string L2B_ECorr[NUM_SSD];

  TH1D* hist_ERatio_L1S[NUM_SSD];
  TH1D* hist_ERatio_L2F[NUM_SSD];
  TH1D* hist_ERatio_L2B[NUM_SSD];
  TH2D* hist_ECorr_L1S[NUM_SSD];
  TH2D* hist_ECorr_L2F[NUM_SSD];
  TH2D* hist_ECorr_L2B[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    L1S_ERatio[ssdindex] = Form("SSD%d_L1S_ERatio",ssdindex+1);
    L2F_ERatio[ssdindex] = Form("SSD%d_L2F_ERatio",ssdindex+1);
    L2B_ERatio[ssdindex] = Form("SSD%d_L2B_ERatio",ssdindex+1);
    L1S_ECorr [ssdindex] = Form("SSD%d_L1S_ECorr",ssdindex+1);
    L2F_ECorr [ssdindex] = Form("SSD%d_L2F_ECorr",ssdindex+1);
    L2B_ECorr [ssdindex] = Form("SSD%d_L2B_ECorr",ssdindex+1);

    hist_ERatio_L1S[ssdindex] = new TH1D(L1S_ERatio[ssdindex].c_str(), L1S_ERatio[ssdindex].c_str(), 100, 0, 1);
    hist_ERatio_L2F[ssdindex] = new TH1D(L2F_ERatio[ssdindex].c_str(), L2F_ERatio[ssdindex].c_str(), 100, 0, 1);
    hist_ERatio_L2B[ssdindex] = new TH1D(L2B_ERatio[ssdindex].c_str(), L2B_ERatio[ssdindex].c_str(), 100, 0, 1);
    hist_ECorr_L1S [ssdindex] = new TH2D(L1S_ECorr[ssdindex].c_str(), L1S_ECorr[ssdindex].c_str(),3000,0,300,3000,0,300);
  }

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries;ientry++) {

    fChain->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {

      // for L1S
      if (LayerEvent_fSSDL1SMulti[ssdindex]>=2 && LayerEvent_fSSDL1SMulti[ssdindex]<=MULTICUT_L1S) {
        for (Int_t l1smulti=0; l1smulti<LayerEvent_fL1SMulti; l1smulti++) {
          if (LayerEvent_fL1SSSDNum[l1smulti]==ssdindex) {
            NumStrip_L1S.push_back(LayerEvent_fL1SNumStrip[l1smulti]);
            EMeV_L1S.push_back(LayerEvent_fL1SEMeV[l1smulti]);
          }
        }
        ClusterSize_Equal2_SiLayer(LayerEvent_fSSDL1SMulti[ssdindex], NumStrip_L1S, EMeV_L1S, strip, efound, ERatio_L1S);
        hist_ERatio_L1S[ssdindex]->Fill(ERatio_L1S);
        hist_ECorr_L1S [ssdindex]->Fill(efound[0], efound[1]);
      }
      // for L2F
      if (LayerEvent_fSSDL2FMulti[ssdindex]>=2 && LayerEvent_fSSDL2FMulti[ssdindex]<=MULTICUT_L2F) {
        for (Int_t l2fmulti=0; l2fmulti<LayerEvent_fL2FMulti; l2fmulti++) {
          NumStrip_L2F.push_back(LayerEvent_fL2FNumStrip[l2fmulti]);
          EMeV_L2F.push_back(LayerEvent_fL2FEMeV[l2fmulti]);
        }
        ClusterSize_Equal2_SiLayer(LayerEvent_fSSDL2FMulti[ssdindex], NumStrip_L2F, EMeV_L2F, strip, efound, ERatio_L2F);
        hist_ERatio_L2F[ssdindex]->Fill(ERatio_L2F);
      }
      // for L2B
      if (LayerEvent_fSSDL2BMulti[ssdindex]>=2 && LayerEvent_fSSDL2BMulti[ssdindex]<=MULTICUT_L2B) {
        for (Int_t l2bmulti=0; l2bmulti<LayerEvent_fL2BMulti; l2bmulti++) {
          if (LayerEvent_fL2BSSDNum[l2bmulti]==ssdindex) {
            NumStrip_L2B.push_back(LayerEvent_fL2BNumStrip[l2bmulti]);
            EMeV_L2B.push_back(LayerEvent_fL2BEMeV[l2bmulti]);
          }
        }
        ClusterSize_Equal2_SiLayer(LayerEvent_fSSDL2BMulti[ssdindex], NumStrip_L2B, EMeV_L2B, strip, efound, ERatio_L2B);
        hist_ERatio_L2B[ssdindex]->Fill(ERatio_L2B);
      }
      NumStrip_L1S.clear();
      NumStrip_L2F.clear();
      NumStrip_L2B.clear();
      EMeV_L1S.clear();
      EMeV_L2F.clear();
      EMeV_L2B.clear();
    }
  }

  TCanvas* cans_L1S = new TCanvas("cans_L1S", "cans_L1S", 1000, 1000);
  TCanvas* cans_L2F = new TCanvas("cans_L2F", "cans_L2F", 1000, 1000);
  TCanvas* cans_L2B = new TCanvas("cans_L2B", "cans_L2B", 1000, 1000);
  TCanvas* cans_L1S_ECorr = new TCanvas("cans_L1S_ECorr", "cans_L1S_ECorr", 1000, 1000);
  cans_L1S->Divide(2,2);
  cans_L2F->Divide(2,2);
  cans_L2B->Divide(2,2);
  cans_L1S_ECorr->Divide(2,2);
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    cans_L1S->cd(ssdindex+1);
    hist_ERatio_L1S[ssdindex]->Draw();

    cans_L2F->cd(ssdindex+1);
    hist_ERatio_L2F[ssdindex]->Draw();

    cans_L2B->cd(ssdindex+1);
    hist_ERatio_L2B[ssdindex]->Draw();

    cans_L1S_ECorr->cd(ssdindex+1);
    hist_ECorr_L1S[ssdindex]->Draw("COLZ");
  }
}


//______________________________
// 计算 clustersize = 2 时两个能量的比值
void Test_Multi::ClusterSize_Equal2_SiLayer(Int_t layermulti, vector<Int_t> numstrip,
  vector<Double_t> ene, Int_t* strip, Double_t* efound, Double_t& eratio)
{
  if (layermulti==2 && (numstrip[0]-numstrip[1] == -1)) {
    eratio = CalcRatio(ene[0], ene[1]); //---**------
    strip[0] = numstrip[0]; strip[1] = numstrip[1];
    efound[0] = ene[0]; efound[1] = ene[1];
  }
  // multi = 3
  if (layermulti==3) {
    if ((numstrip[0]-numstrip[2]!=-2)&&(numstrip[0]-numstrip[1]==-1)) {
      eratio = CalcRatio(ene[0], ene[1]); //--**---*-----
      strip[0] = numstrip[0]; strip[1] = numstrip[1];
      efound[0] = ene[0]; efound[1] = ene[1];
    }
    if ((numstrip[0]-numstrip[2]!=-2)&&(numstrip[1]-numstrip[2]==-1)) {
      eratio = CalcRatio(ene[1], ene[2]);  //--*---**-----
      strip[0] = numstrip[1]; strip[1] = numstrip[2];
      efound[0] = ene[1]; efound[1] = ene[2];
    }
  }
  // multi = 4
  if (layermulti==4) {
    if ((numstrip[0]-numstrip[1]==-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]!=-1)) {
      eratio = CalcRatio(ene[0], ene[1]); //---**--*--*--
      strip[0] = numstrip[0]; strip[1] = numstrip[1];
      efound[0] = ene[0]; efound[1] = ene[1];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]==-1)&&(numstrip[2]-numstrip[3]!=-1)) {
      eratio = CalcRatio(ene[1], ene[2]); //---*--**---*--
      strip[0] = numstrip[1]; strip[1] = numstrip[2];
      efound[0] = ene[1]; efound[1] = ene[2];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]==-1)) {
      eratio = CalcRatio(ene[2], ene[3]); //---*--*--**---
      strip[0] = numstrip[2]; strip[1] = numstrip[3];
      efound[0] = ene[2]; efound[1] = ene[3];
    }
  }
  // multi = 5
  if (layermulti==5) {
    if ((numstrip[0]-numstrip[1]==-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]!=-1)&&(numstrip[3]-numstrip[4]!=-1)) {
      eratio = CalcRatio(ene[0], ene[1]); //---**--*--*--*--
      strip[0] = numstrip[0]; strip[1] = numstrip[1];
      efound[0] = ene[0]; efound[1] = ene[1];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]==-1)&&(numstrip[2]-numstrip[3]!=-1)&&(numstrip[3]-numstrip[4]!=-1)) {
      eratio = CalcRatio(ene[1], ene[2]); //--*--**--*--*--
      strip[0] = numstrip[1]; strip[1] = numstrip[2];
      efound[0] = ene[1]; efound[1] = ene[2];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]==-1)&&(numstrip[3]-numstrip[4]!=-1)) {
      eratio = CalcRatio(ene[2], ene[3]); //--*--*--**--*--
      strip[0] = numstrip[2]; strip[1] = numstrip[3];
      efound[0] = ene[2]; efound[1] = ene[3];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]!=-1)&&(numstrip[3]-numstrip[4]==-1)) {
      eratio = CalcRatio(ene[3], ene[4]); //--*--*--*--**--
      strip[0] = numstrip[3]; strip[1] = numstrip[4];
      efound[0] = ene[3]; efound[1] = ene[4];
    }
  }
  // mulit = 6
  if (layermulti==6) {
    if ((numstrip[0]-numstrip[1]==-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]!=-1)&&
        (numstrip[3]-numstrip[4]!=-1)&&(numstrip[4]-numstrip[5]!=-1)) {
      eratio = CalcRatio(ene[0], ene[1]); //--**--*--*--*--*--
      strip[0] = numstrip[0]; strip[1] = numstrip[1];
      efound[0] = ene[0]; efound[1] = ene[1];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]==-1)&&(numstrip[2]-numstrip[3]!=-1)&&
        (numstrip[3]-numstrip[4]!=-1)&&(numstrip[4]-numstrip[5]!=-1)) {
      eratio = CalcRatio(ene[1], ene[2]); //--*--**--*--*--*--
      strip[0] = numstrip[1]; strip[1] = numstrip[2];
      efound[0] = ene[1]; efound[1] = ene[2];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]==-1)&&
        (numstrip[3]-numstrip[4]!=-1)&&(numstrip[4]-numstrip[5]!=-1)) {
      eratio = CalcRatio(ene[2], ene[3]); //--*--*--**--*--*--
      strip[0] = numstrip[2]; strip[1] = numstrip[3];
      efound[0] = ene[2]; efound[1] = ene[3];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]!=-1)&&
        (numstrip[3]-numstrip[4]==-1)&&(numstrip[4]-numstrip[5]!=-1)) {
      eratio = CalcRatio(ene[3], ene[4]); //--*--*--*--**--*--
      strip[0] = numstrip[3]; strip[1] = numstrip[4];
      efound[0] = ene[3]; efound[1] = ene[4];
    }
    if ((numstrip[0]-numstrip[1]!=-1)&&(numstrip[1]-numstrip[2]!=-1)&&(numstrip[2]-numstrip[3]!=-1)&&
        (numstrip[3]-numstrip[4]!=-1)&&(numstrip[4]-numstrip[5]==-1)) {
      eratio = CalcRatio(ene[4], ene[5]); //--*--*--*--*--**--
      strip[0] = numstrip[4]; strip[1] = numstrip[5];
      efound[0] = ene[4]; efound[1] = ene[5];
    }
  }
}


//_________________________________________
void Test_Multi::ClusterSize_Equal2_CsI()
{
  std::vector<Int_t> csinum;
  std::vector<Double_t>  csiech;

  Int_t count_eff[4] = {0};
  Int_t size_2[4]    = {0};
  Int_t size_1213[4] = {0};
  Int_t size_1223[4] = {0};
  Int_t size_1323[4] = {0};
  Int_t size_12[4]   = {0};
  Int_t size_13[4]   = {0};
  Int_t size_23[4]   = {0};

  Double_t ration_sum[4] = {0.};
  Double_t r_size_2[4]   = {0.};
  Double_t r_size_1213[4] = {0.};
  Double_t r_size_1223[4] = {0.};
  Double_t r_size_1323[4] = {0.};
  Double_t r_size_12[4]   = {0.};
  Double_t r_size_13[4]   = {0.};
  Double_t r_size_23[4]   = {0.};

  TH2D* hist_CsI_ECorr[NUM_SSD];
  TH2D* hist_CsI_Array[NUM_SSD][NUM_CSI];
  for (Int_t i=0; i<NUM_SSD; i++) {
    hist_CsI_ECorr[i] = new TH2D(Form("SSD%d_CsIECorr",i+1), Form("SSD%d_CsIECorr",i+1), 4000,0,4000,4000,0,4000);

    for (Int_t j=0; j<NUM_CSI; j++) {
      hist_CsI_Array[i][j] = new TH2D(Form("SSD%d_CsI%d", i+1, j), Form("SSD%d_CsI%d", i+1, j), 4000,0,4000,4000,0,4000);
    }
  }

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries;ientry++) {

    fChain->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {

      if (LayerEvent_fSSDCsIMulti[ssdindex] >= 1) count_eff[ssdindex]++;

      // for multi = 2
      if (LayerEvent_fSSDCsIMulti[ssdindex] == 2) {
        for (Int_t csimulti=0; csimulti<LayerEvent_fCsIMulti; csimulti++) {
          if (LayerEvent_fCsISSDNum[csimulti] == ssdindex) {
            csinum.push_back(LayerEvent_fCsINum[csimulti]);
            csiech.push_back(LayerEvent_fCsIECh[csimulti]);
          }
        }
        if (IsAdj_CsI(csinum[0],csinum[1])) {
          size_2[ssdindex]++;
          hist_CsI_ECorr[ssdindex]->Fill(csiech[0], csiech[1]);
          hist_CsI_Array[ssdindex][csinum[0]]->Fill(csiech[0], csiech[1]);
          // cout<<size_2[ssdindex]<<setw(20)<<csinum[0]<<setw(20)<<csinum[1]<<endl;
        }
      }
      // for multi = 3
      if (LayerEvent_fSSDCsIMulti[ssdindex] == 3) {
        for (Int_t csimulti=0; csimulti<LayerEvent_fCsIMulti; csimulti++) {
          if (LayerEvent_fCsISSDNum[csimulti] == ssdindex) {
            csinum.push_back(LayerEvent_fCsINum[csimulti]);
          }
        }
        if (IsAdj_CsI(csinum[0],csinum[1]) && IsAdj_CsI(csinum[0],csinum[2])) size_1213[ssdindex]++;
        if (IsAdj_CsI(csinum[0],csinum[1]) && IsAdj_CsI(csinum[1],csinum[2])) size_1223[ssdindex]++;
        if (IsAdj_CsI(csinum[0],csinum[2]) && IsAdj_CsI(csinum[1],csinum[2])) size_1323[ssdindex]++;
        if (IsAdj_CsI(csinum[0],csinum[1])  && !IsAdj_CsI(csinum[0],csinum[2]) && !IsAdj_CsI(csinum[1],csinum[2])) size_12[ssdindex]++;
        if (!IsAdj_CsI(csinum[0],csinum[1]) &&  IsAdj_CsI(csinum[0],csinum[2]) && !IsAdj_CsI(csinum[1],csinum[2])) size_13[ssdindex]++;
        if (!IsAdj_CsI(csinum[0],csinum[1]) && !IsAdj_CsI(csinum[0],csinum[2]) &&  IsAdj_CsI(csinum[1],csinum[2])) size_23[ssdindex]++;
      }
      csinum.clear();
      csiech.clear();
    }
  }

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    r_size_2[ssdindex]    = (Double_t) size_2[ssdindex]/count_eff[ssdindex];
    r_size_1213[ssdindex] = (Double_t) size_1213[ssdindex]/count_eff[ssdindex];
    r_size_1223[ssdindex] = (Double_t) size_1223[ssdindex]/count_eff[ssdindex];
    r_size_1323[ssdindex] = (Double_t) size_1323[ssdindex]/count_eff[ssdindex];
    r_size_12[ssdindex]   = (Double_t) size_12[ssdindex]/count_eff[ssdindex];
    r_size_13[ssdindex]   = (Double_t) size_13[ssdindex]/count_eff[ssdindex];
    r_size_23[ssdindex]   = (Double_t) size_23[ssdindex]/count_eff[ssdindex];

    ration_sum[ssdindex] = r_size_2[ssdindex]+r_size_1213[ssdindex]+r_size_1223[ssdindex]+
                           r_size_1323[ssdindex]+r_size_12[ssdindex]+r_size_13[ssdindex]+r_size_23[ssdindex];
  }

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    printf("SSD%d_CsI\n", ssdindex+1);
    cout<<"count_eff = "<<count_eff[ssdindex]<<endl;
    cout<<setprecision(4);
    cout<<setw(10)<<"size_2    = "<<size_2[ssdindex]<<setw(15)<<r_size_2[ssdindex]<<endl;
    cout<<setw(10)<<"size_1213 = "<<size_1213[ssdindex]<<setw(15)<<r_size_1213[ssdindex]<<endl;
    cout<<setw(10)<<"size_1223 = "<<size_1223[ssdindex]<<setw(15)<<r_size_1223[ssdindex]<<endl;
    cout<<setw(10)<<"size_1323 = "<<size_1323[ssdindex]<<setw(15)<<r_size_1323[ssdindex]<<endl;
    cout<<setw(10)<<"size_12   = "<<size_12[ssdindex]<<setw(15)<<r_size_12[ssdindex]<<endl;
    cout<<setw(10)<<"size_13   = "<<size_13[ssdindex]<<setw(15)<<r_size_13[ssdindex]<<endl;
    cout<<setw(10)<<"size_23   = "<<size_23[ssdindex]<<setw(15)<<r_size_23[ssdindex]<<endl;
    cout<<setw(10)<<"sum       = "<<ration_sum[ssdindex]<<endl;
    cout<<endl;
  }

  TCanvas* cans_CsI_ECorr = new TCanvas("cans_CsI_ECorr", "cans_CsI_ECorr", 1000, 1000);
  cans_CsI_ECorr->Divide(2,2);
  TCanvas* cans_CsI_Array[NUM_SSD];

  for (Int_t i=0; i<NUM_SSD; i++) {
    cans_CsI_ECorr->cd(i+1);
    hist_CsI_ECorr[i]->Draw("COLZ");

    cans_CsI_Array[i] = new TCanvas(Form("cans_SSD%d", i+1), Form("cans_SSD%d", i+1), 1200, 1200);
    cans_CsI_Array[i]->Divide(3,3);
    for (Int_t j=0; j<NUM_CSI; j++) {
      cans_CsI_Array[i]->cd(j+1);
      hist_CsI_Array[i][j]->Draw("COLZ");
    }
  }
}


//_________________________________________
//        检查 CsI 之间的串扰
// 选取 L2F, L2B 都只有一个粒子的事件, 然后对 CsI 中的情况进行分类:
// 如果点火的 CsI 数目大于1, 且至少有那块 CsI 相邻，则认为该事件
// “可能”存在串扰. 以此通过实验数据给出串扰的概率上限
//
void Test_Multi::CheckCrossTalk_CsI()
{
  gStyle->SetPalette(1);
//  gStyle->SetOptStat(0);

  TimeAndPercentage time;

  Int_t firstrun  = 120;
  Int_t lastrun   = 200;

  Double_t eneratio = 0.15;
  Int_t csiech_cut = 60;

  Int_t count_csimulti_ge1[4] = {0};
  Int_t count_csimulti_ge2[4] = {0};
  Int_t count_multi2_crosstalk[4] = {0};
  Int_t count_multi3_crosstalk[4] = {0};

  std::vector<Int_t> csinum;
  std::vector<Int_t> csiene;

  std::string pathFolder("/home/sea/Fission2019_Data/CSHINEEvent/");
  std::string pathROOT(Form("%sEventTree_Run%04d-Run%04d.root", pathFolder.c_str(),firstrun,lastrun));
  std::string pathPNGout[4];

  TH1D* hist_CsI[4];
  TH2D* h2_crosstalk[4];
  for (Int_t i=0; i<4; i++) {
    pathPNGout[i] = Form("figures/figure_TrackReconstruction/SSD%d_CsI_CrossTalk.png", i+1);
    hist_CsI[i] = new TH1D(Form("SSD%d_CsIMulti",i+1),Form("SSD%d_CsIMulti",i+1),10,0,10);
    h2_crosstalk[i] = new TH2D(Form("SSD%d_crosstalk",i+1),Form("SSD%d_crosstalk",i+1),4000,0,4000,4000,0,4000);
  }

  Long64_t nentries = fChain->GetEntries();
  cout<<"Found "<<nentries<<" entries."<<endl;

  Int_t ientry = 0;
  for (Long64_t ientry=0; ientry<nentries; ientry++)
  {
    fChain->GetEntry(ientry);
    time.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<4; ssdindex++) {
      //_______________________
      // for L2L3
      if (LayerEvent_fSSDL1SMulti[ssdindex]>0 && LayerEvent_fSSDL2BMulti[ssdindex]==1 &&
          LayerEvent_fSSDL2FMulti[ssdindex]==1 && LayerEvent_fSSDCsIMulti[ssdindex]>0)
      {
        hist_CsI[ssdindex]->Fill(LayerEvent_fSSDCsIMulti[ssdindex]);

        if (LayerEvent_fSSDCsIMulti[ssdindex]>=1) count_csimulti_ge1[ssdindex]++;
        if (LayerEvent_fSSDCsIMulti[ssdindex]>=2) count_csimulti_ge2[ssdindex]++;

        // for multi = 2
        if (LayerEvent_fSSDCsIMulti[ssdindex] == 2) {
          for (Int_t csimulti=0; csimulti<LayerEvent_fCsIMulti; csimulti++) {
            if (LayerEvent_fCsISSDNum[csimulti] == ssdindex) {
              csinum.push_back(LayerEvent_fCsINum[csimulti]);
              csiene.push_back(LayerEvent_fCsIECh[csimulti]);
            }
          }
          if (csinum.size()>0) {
            if (IsAdj_CsI(csinum[0],csinum[1])) {
              if (csiene[0]>csiech_cut && csiene[1]>csiech_cut) {
                count_multi2_crosstalk[ssdindex]++;

                // SSD4_CsI[4] 与 CsI[5] 信号异常, 需要扔掉
                if (ssdindex==3 && ((csinum[0]==4 || csinum[0]==5) || (csinum[1]==4 || csinum[1]==5))) continue;
                h2_crosstalk[ssdindex]->Fill(csiene[0], csiene[1]);
                //  cout<<"csi no. = "<<csinum[0]<<setw(15)<<"csi no. = "<<csinum[1]<<setw(15)
                //      <<"csi ene = "<<csiene[0]<<setw(15)<<"csi ene = "<<csiene[1]<<endl;
              }
            }
          }
        }

        // for multi = 3
        if (LayerEvent_fSSDCsIMulti[ssdindex] == 3) {
          for (Int_t csimulti=0; csimulti<LayerEvent_fCsIMulti; csimulti++) {
            if (LayerEvent_fCsISSDNum[csimulti] == ssdindex) {
              csinum.push_back(LayerEvent_fCsINum[csimulti]);
            }
          }
          if ( (IsAdj_CsI(csinum[0],csinum[1]) && IsAdj_CsI(csinum[0],csinum[2])) ||
               (IsAdj_CsI(csinum[0],csinum[1]) && IsAdj_CsI(csinum[1],csinum[2])) ||
               (IsAdj_CsI(csinum[0],csinum[2]) && IsAdj_CsI(csinum[1],csinum[2])) ||
               (IsAdj_CsI(csinum[0],csinum[1]) && IsAdj_CsI(csinum[0],csinum[2]) && IsAdj_CsI(csinum[1],csinum[2])) ||
               (IsAdj_CsI(csinum[0],csinum[1]) && IsAdj_CsI(csinum[0],csinum[2]) && IsAdj_CsI(csinum[1],csinum[2])) ||
               (IsAdj_CsI(csinum[0],csinum[1] )&& IsAdj_CsI(csinum[0],csinum[2]) && IsAdj_CsI(csinum[1],csinum[2])) )
          {
            count_multi3_crosstalk[ssdindex]++;
          }
        }
        csinum.clear();
        csiene.clear();
      }
    }
  }
  for (Int_t i=0; i<4; i++) {
    cout<<endl;
    cout<<"for SSD"<<i<<endl;
    cout<<"count_csimulti_ge1 = "<<count_csimulti_ge1[i]<<endl;
    cout<<"count_csimulti_ge2 = "<<count_csimulti_ge2[i]<<endl;
    cout<<"count_multi2_crosstalk = "<<count_multi2_crosstalk[i]<<endl;
    cout<<"count_multi3_crosstalk = "<<count_multi3_crosstalk[i]<<endl;

    Double_t ratio = (Double_t) count_csimulti_ge2[i]/count_csimulti_ge1[i];
    Double_t ratio_crosstalk = (Double_t) (count_multi2_crosstalk[i]+count_multi3_crosstalk[i])/count_csimulti_ge1[i];
    cout<<setprecision(6)<<"multi>=2 / multi>=1 = "<<ratio<<endl;
    cout<<setprecision(6)<<"ratio_crosstalk = "<<ratio_crosstalk<<endl;
  }


  TCanvas* cans[4];
  for (Int_t i=0; i<4; i++) {
    cans[i] = new TCanvas(Form("c_SSD%d",i+1), Form("c_SSD%d",i+1), 1200, 500);
    cans[i]->Divide(2,1);
    cans[i]->cd(1);
    hist_CsI[i]->Draw();
    cans[i]->cd(2);
    h2_crosstalk[i]->Draw("COL");
    cans[i]->Print(pathPNGout[i].c_str());
  }
}



// 判断两块 CsI 晶体是否相邻. 以下是 Fission2019 实验中 CsI 阵列的排布:
//  -----------
//   2   5   8
//   1   4   7
//   0   3   6
//  -----------
Bool_t Test_Multi::IsAdj_CsI(Int_t n1, Int_t n2)
{
  //（ 列相邻 || 行相邻 ）
  return (((n1/3 == n2/3)&&(n1-n2 == -1)) || ((n1%3 == n2%3)&&(n1-n2 == -3)));
}




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
void Test_Multi::CheckTrackReconstructionResult()
{
  Int_t count_multicut[4] = {0};
  Int_t count_track[4] = {0};
  Int_t count_global[4];
  Int_t count_globalmulti0[4] = {0};
  Int_t track_cand_total[4];
  Int_t track_cand_No[4];

  Bool_t Is_Geo_L3AL2B = kFALSE;
  Bool_t Is_Geo_L3AL2F = kFALSE;
  Bool_t Is_Geo_L2BL1S = kFALSE;
  Bool_t Is_Ene_L2BL2F = kFALSE;
  Bool_t Is_Ene_L1SL2F = kFALSE;

  std::string pathTrackEventRoot("/home/sea/Fission2019_Data/TrackReconstructionEvent_Run0120-Run0130.root");

  ofstream TrackDataOut[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    TrackDataOut[ssdindex].open(Form("data/data_Test_Multi/SSD%d_TrackData.dat", ssdindex+1));
    TrackDataOut[ssdindex]<<setw(7)<<"EntryNo."<<setw(10)<<"EntryID"<<setw(15)<<"MultiGlobal"
        <<setw(17)<<"Track_Cand._Tot"<<setw(17)<<"Track_Cand._No."<<setw(10)<<"Track ?"
        <<setw(10)<<"M_L3A"<<setw(10)<<"M_L2B"<<setw(10)<<"M_L2F"<<setw(11)<<"M_L1S"
        <<setw(15)<<"Is_Geo_L3AL2B"<<setw(15)<<"Is_Geo_L3AL2F"<<setw(15)<<"Is_Ene_L2BL2F"
        <<setw(15)<<"Is_Geo_L2BL1S"<<setw(15)<<"Is_Ene_L1SL2F"<<endl;
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

  cout<<setw(7)<<"EventNo."<<setw(10)<<"EntryID"<<setw(15)<<"MultiGlobal"
      <<setw(17)<<"Track_Cand._Tot"<<setw(17)<<"Track_Cand._No."<<setw(10)<<"Track ?"
      <<setw(10)<<"M_L3A"<<setw(10)<<"M_L2B"<<setw(10)<<"M_L2F"<<setw(11)<<"M_L1S"
      <<setw(15)<<"Is_Geo_L3AL2B"<<setw(15)<<"Is_Geo_L3AL2F"<<setw(15)<<"Is_Ene_L2BL2F"
      <<setw(15)<<"Is_Geo_L2BL1S"<<setw(15)<<"Is_Ene_L1SL2F"<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {

    mytree->GetEntry(ientry);  // TrackEvent
    fChain->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {

      if ((LayerEvent_fSSDCsIMulti[ssdindex]>0 && LayerEvent_fSSDCsIMulti[ssdindex]<=MULTICUT_L3A) &&
          (LayerEvent_fSSDL2BMulti[ssdindex]>0 && LayerEvent_fSSDL2BMulti[ssdindex]<=MULTICUT_L2B) &&
          (LayerEvent_fSSDL2FMulti[ssdindex]>0 && LayerEvent_fSSDL2FMulti[ssdindex]<=MULTICUT_L2F) &&
          (LayerEvent_fSSDL1SMulti[ssdindex]>0 && LayerEvent_fSSDL1SMulti[ssdindex]<=MULTICUT_L1S))
      {
        count_multicut[ssdindex]++;
        count_global[ssdindex] = 0;
        track_cand_total[ssdindex] = 0;
        track_cand_No[ssdindex] = 0;

        if (TrackEvent_fSSDGlobalMulti[ssdindex] == 0) count_globalmulti0[ssdindex]++;

        for (Int_t csimulti=0; csimulti<LayerEvent_fCsIMulti; csimulti++) {
          for (Int_t l2bmulti=0; l2bmulti<LayerEvent_fL2BMulti; l2bmulti++) {
            for (Int_t l2fmulti=0; l2fmulti<LayerEvent_fL2FMulti; l2fmulti++) {
              for (Int_t l1smulti=0; l1smulti<LayerEvent_fL1SMulti; l1smulti++) { // 逐层循环,遍历所有可能的组合

                if (LayerEvent_fCsISSDNum[csimulti]==ssdindex && LayerEvent_fL2BSSDNum[l2bmulti]==ssdindex &&
                    LayerEvent_fL2FSSDNum[l2fmulti]==ssdindex && LayerEvent_fL1SSSDNum[l1smulti]==ssdindex)  // 对每个SSD分开讨论
                {
                  Is_Geo_L3AL2B = fPattern.IsGeoConstraint_L3A_L2B(LayerEvent_fCsINum[csimulti], LayerEvent_fL2BNumStrip[l2bmulti]);
                  Is_Geo_L3AL2F = fPattern.IsGeoConstraint_L3A_L2F(LayerEvent_fCsINum[csimulti], LayerEvent_fL2FNumStrip[l2fmulti]);
                  Is_Geo_L2BL1S = fPattern.IsGeoConstraint_L2B_L1S(LayerEvent_fL2BNumStrip[l2bmulti], LayerEvent_fL1SNumStrip[l1smulti]);
                  Is_Ene_L2BL2F = fPattern.IsEneConstraint_L2B_L2F(ssdindex, LayerEvent_fL2BEMeV[l2bmulti], LayerEvent_fL2FEMeV[l2fmulti]);
                  Is_Ene_L1SL2F = fPattern.IsEneConstraint_L1S_L2F(ssdindex, LayerEvent_fL1SEMeV[l1smulti], LayerEvent_fL2FEMeV[l2fmulti]);
                  if (Is_Geo_L3AL2B && Is_Geo_L3AL2F && Is_Geo_L2BL1S && Is_Ene_L2BL2F && Is_Ene_L1SL2F)
                  {
                    count_global[ssdindex]++;
                  }
                  track_cand_total[ssdindex] = LayerEvent_fSSDCsIMulti[ssdindex] * LayerEvent_fSSDL2BMulti[ssdindex]
                                             * LayerEvent_fSSDL2FMulti[ssdindex] * LayerEvent_fSSDL1SMulti[ssdindex];

                  // 只关心 fglobalmulti=0 的事件, 检查有没有误扔掉的事件
                  if (TrackEvent_fSSDGlobalMulti[ssdindex] == 0) {
                    cout<<setw(3)<<count_globalmulti0[ssdindex]<<setw(10)<<ientry<<setw(13)<<TrackEvent_fSSDGlobalMulti[ssdindex]
                        <<setw(15)<<track_cand_total[ssdindex]<<setw(15)<<track_cand_No[ssdindex]
                        <<setw(15)<<((Is_Geo_L3AL2B*Is_Geo_L3AL2F*Is_Geo_L2BL1S*Is_Ene_L2BL2F*Is_Ene_L1SL2F>0) ? "Y" : "N")
                        <<setw(12)<<LayerEvent_fSSDCsIMulti[ssdindex]<<setw(10)<<LayerEvent_fSSDL2BMulti[ssdindex]
                        <<setw(10)<<LayerEvent_fSSDL2FMulti[ssdindex]<<setw(10)<<LayerEvent_fSSDL1SMulti[ssdindex]
                        <<setw(15)<<Is_Geo_L3AL2B<<setw(15)<<Is_Geo_L3AL2F
                        <<setw(15)<<Is_Geo_L2BL1S<<setw(15)<<Is_Ene_L2BL2F
                        <<setw(15)<<Is_Ene_L1SL2F<<endl;

                    TrackDataOut[ssdindex]<<setw(3)<<count_globalmulti0[ssdindex]<<setw(10)<<ientry
                        <<setw(13)<<TrackEvent_fSSDGlobalMulti[ssdindex]
                        <<setw(15)<<track_cand_total[ssdindex]<<setw(15)<<track_cand_No[ssdindex]
                        <<setw(15)<<((Is_Geo_L3AL2B*Is_Geo_L3AL2F*Is_Geo_L2BL1S*Is_Ene_L2BL2F*Is_Ene_L1SL2F>0) ? "Y" : "N")
                        <<setw(12)<<LayerEvent_fSSDCsIMulti[ssdindex]<<setw(10)<<LayerEvent_fSSDL2BMulti[ssdindex]
                        <<setw(10)<<LayerEvent_fSSDL2FMulti[ssdindex]<<setw(10)<<LayerEvent_fSSDL1SMulti[ssdindex]
                        <<setw(15)<<Is_Geo_L3AL2B<<setw(15)<<Is_Geo_L3AL2F
                        <<setw(15)<<Is_Geo_L2BL1S<<setw(15)<<Is_Ene_L2BL2F
                        <<setw(15)<<Is_Ene_L1SL2F<<endl;

                    track_cand_No[ssdindex]++;
                  }
                }
              }
            }
          }
        }
        if (count_global[ssdindex]>0) {
          count_track[ssdindex]++;
        }
      }
    }
  }

  // 输出径迹查找效率: N_track/N_multi
  cout<<endl;
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    Double_t eff_ratio = (Double_t) count_track[ssdindex]/count_multicut[ssdindex];
    cout<<count_multicut[ssdindex]<<setw(20)<<count_track[ssdindex]<<setw(20)
        <<std::setprecision(4)<<eff_ratio<<endl;
  }
}


//______________________________________________________________________
// 分析不同 fGlobalMulti 所占的比例
void Test_Multi::CheckGlobalMultiRatio()
{
  // 定义变量， 以用于计算不同 fGlobalMulti 的比例
  Int_t    nummulti = 7;
  Int_t    globalmulti[NUM_SSD][7]  = {{0}};
  Double_t multiratio_from0[NUM_SSD][7] = {{0.}};
  Double_t multiratio_from1[NUM_SSD][7] = {{0.}};
  Double_t ratiosum_from0[NUM_SSD]      = {0.};
  Double_t ratiosum_from1[NUM_SSD]      = {0.};

  std::string pathTrackEventRoot = "/home/sea/Fission2019_Data/TrackReconstructionEvent_Run0120-Run0130.root";
  TFile* myfile = new TFile(pathTrackEventRoot.c_str(), "READONLY");
  if (!myfile || !myfile->IsOpen()) {
    cout<<Form("File %s not founded.",  myfile->GetName());
    return;
  }
  TTree* mytree = (TTree*)myfile->Get("TrackEvent");
  Long64_t nentries = mytree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  std::vector<Int_t>      TrackEvent_fSSDGlobalMulti;
  mytree->SetMakeClass(1);  // 如果 tree 的 branch 使用了自定义的类, 则这条语句不能省略！！！
  mytree->SetBranchAddress("TrackEvent.fSSDGlobalMulti", &TrackEvent_fSSDGlobalMulti);

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    mytree->GetEntry(ientry);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      for (Int_t i=0; i<nummulti; i++) {
        if (TrackEvent_fSSDGlobalMulti[ssdindex]==i)   globalmulti[ssdindex][i]++;
      }
    }
  }

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    cout<<"------------------"<<endl;
    cout<<Form("For SSD%d :\n",ssdindex+1)<<endl;

    // 计算比例，从 globalmulti = 0 算起
    for (Int_t i=0; i<nummulti; i++) {
      multiratio_from0[ssdindex][i] = (Double_t) globalmulti[ssdindex][i]/nentries;
      ratiosum_from0[ssdindex] += multiratio_from0[ssdindex][i];
      cout<<Form("multi%d = ", i)<<multiratio_from0[ssdindex][i]<<setw(12);
    }
    cout<<ratiosum_from0[ssdindex]<<endl;

    // 计算比例，从 globalmulti = 1 算起
    for (Int_t i=1; i<nummulti; i++) {
      multiratio_from1[ssdindex][i] = (Double_t) globalmulti[ssdindex][i]/(nentries-globalmulti[ssdindex][0]); // 剔除 fGlobalMulti=0 的情况
      ratiosum_from1[ssdindex] += multiratio_from1[ssdindex][i];
      cout<<Form("multi%d = ", i)<<multiratio_from1[ssdindex][i]<<setw(12);
    }
    cout<<ratiosum_from1[ssdindex]<<endl;
  }

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
