#define Test_Multi_cxx
#include "Test_Multi.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include "include/TimeAndPercentage.h"
#include "include/EnergyLossModule.h"
#include "include/shared.h"

using namespace std;

//______________________________________________________________________________
void Test_Multi::Loop()
{
//______________________________________________________________________________
//   In a ROOT session, you can do:
//      root> .L Test_Multi.C
//      root> Test_Multi t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
//______________________________________________________________________________

  if (fChain == 0) return;

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}



//______________________________________________________________________________
// 自定义径迹查找算法1 : 循环-循环-判断-循环-判断-循环-判断
// 经验证, 算法2 与 算法1 等效 !
void Test_Multi::TrackFindingAlgorithm1()
{
  gStyle->SetPalette(1);
  Int_t  nlines = 0;
  Double_t L2FL2B_EnergyCut = 0.15;

  TH2D* Correlation_L2B_L2F = new TH2D("hist_PID", "hist_PID", 2500, 0.,250., 2500, 0., 250.);
  TH2D* PID_L2B_L1S = new TH2D("PID_L2B_L1S", "PID_L2B_L1S", 2500, 0., 250., 1500, 0., 150.);
  TH2D* PID_L3_L2B[9];
  for (Int_t i=0; i<9; i++) {
    PID_L3_L2B[i] = new TH2D(Form("PID_L3%d_L2B",i+1),Form("PID_L3%d_L2B",i+1),4000,0.,4000.,2500,0.,250.);
  }

  ofstream  FileOut("Test_Multi1.dat");
  //____________________________________________________________________________
  if (fChain == 0) return;
  fChain->SetBranchStatus("*", 0);
  fChain->SetBranchStatus("SSD1.fL1SMulti",    1);
  fChain->SetBranchStatus("SSD1.fL1SNumStrip", 1);
  fChain->SetBranchStatus("SSD1.fL1SEMeV",     1);
  fChain->SetBranchStatus("SSD1.fL2FMulti",    1);
  fChain->SetBranchStatus("SSD1.fL2FNumStrip", 1);
  fChain->SetBranchStatus("SSD1.fL2FEMeV",     1);
  fChain->SetBranchStatus("SSD1.fL2BMulti",    1);
  fChain->SetBranchStatus("SSD1.fL2BNumStrip", 1);
  fChain->SetBranchStatus("SSD1.fL2BEMeV",     1);
  fChain->SetBranchStatus("SSD1.fCsIMulti",    1);
  fChain->SetBranchStatus("SSD1.fCsINum",      1);
  fChain->SetBranchStatus("SSD1.fCsIECh",      1);

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {

      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      if (SSD1_fCsIMulti==0) continue;
      if (SSD1_fL2BMulti==0) continue;
      if (SSD1_fL2FMulti==0) continue;
      if (SSD1_fL1SMulti==0) continue;

      Int_t fGlobalMulti = 0;
      for (Int_t csich=0; csich<SSD1_fCsIMulti; csich++)
      {
        for (Int_t stripl2b=0; stripl2b<SSD1_fL2BMulti; stripl2b++)
        {
          if (fPattern.GeoConstraint_L3A_L2B(SSD1_fCsINum[csich], SSD1_fL2BNumStrip[stripl2b]))
          {
            for (Int_t stripl2f=0; stripl2f<SSD1_fL2FMulti; stripl2f++)
            {
              if (fPattern.EneConstraint_L2B_L2F(SSD1_fL2BEMeV[stripl2b], SSD1_fL2FEMeV[stripl2f], L2FL2B_EnergyCut)
              && fPattern.GeoConstraint_L3A_L2F(SSD1_fCsINum[csich], SSD1_fL2FNumStrip[stripl2f]))
              {
                for (Int_t stripl1s=0; stripl1s<SSD1_fL1SMulti; stripl1s++)
                {
                  if (fPattern.GeoConstraint_L2B_L1S(SSD1_fL2BNumStrip[stripl2b], SSD1_fL1SNumStrip[stripl1s]))
                  {
                    //if (SSD1_fL1SEMeV[stripl1s]/SSD1_fL2FEMeV[stripl2f]>0.5) continue;
                    fGlobalMulti++;
                    nlines++;

                    cout<<setw(10)<<"M_L3A = "<<setw(2)<<SSD1_fCsIMulti
                        <<setw(10)<<"M_L2B = "<<setw(2)<<SSD1_fL2BMulti
                        <<setw(10)<<"M_L2F = "<<setw(2)<<SSD1_fL2FMulti
                        <<setw(10)<<"M_L1S = "<<setw(2)<<SSD1_fL1SMulti
                        <<setw(15)<<"MultiGlobal = "<<fGlobalMulti
                        <<setw(10)<<"CSI = "<<setw(2)<<SSD1_fCsINum[csich]
                        <<setw(10)<<"L2B = "<<setw(2)<<SSD1_fL2BNumStrip[stripl2b]
                        <<setw(10)<<"L2F = "<<setw(2)<<SSD1_fL2FNumStrip[stripl2f]
                        <<setw(10)<<"L1S = "<<setw(2)<<SSD1_fL1SNumStrip[stripl1s]
                        <<setw(10)<<"EL1S = "<<setw(7)<<SSD1_fL1SEMeV[stripl1s]
                        <<setw(10)<<"EL2F = "<<setw(7)<<SSD1_fL2FEMeV[stripl2f]
                        <<setw(10)<<"EL2B = "<<setw(7)<<SSD1_fL2BEMeV[stripl2b]
                        <<setw(10)<<"ECsI = "<<setw(4)<<SSD1_fCsIECh[csich]
                        <<endl;

                    FileOut<<setw(10)<<"M_L3A = "<<setw(2)<<SSD1_fCsIMulti
                           <<setw(10)<<"M_L2B = "<<setw(2)<<SSD1_fL2BMulti
                           <<setw(10)<<"M_L2F = "<<setw(2)<<SSD1_fL2FMulti
                           <<setw(10)<<"M_L1S = "<<setw(2)<<SSD1_fL1SMulti
                           <<setw(15)<<"MultiGlobal = "<<fGlobalMulti
                           <<setw(10)<<"CSI = "<<setw(2)<<SSD1_fCsINum[csich]
                           <<setw(10)<<"L2B = "<<setw(2)<<SSD1_fL2BNumStrip[stripl2b]
                           <<setw(10)<<"L2F = "<<setw(2)<<SSD1_fL2FNumStrip[stripl2f]
                           <<setw(10)<<"L1S = "<<setw(2)<<SSD1_fL1SNumStrip[stripl1s]
                           <<setw(10)<<"EL1S = "<<setw(7)<<SSD1_fL1SEMeV[stripl1s]
                           <<setw(10)<<"EL2F = "<<setw(7)<<SSD1_fL2FEMeV[stripl2f]
                           <<setw(10)<<"EL2B = "<<setw(7)<<SSD1_fL2BEMeV[stripl2b]
                           <<setw(10)<<"ECsI = "<<setw(4)<<SSD1_fCsIECh[csich]
                           <<endl;
                    PID_L3_L2B[SSD1_fCsINum[csich]]->Fill(SSD1_fCsIECh[csich], SSD1_fL2BEMeV[stripl2b]);
                  }
                }
              }
            }
          }
        }
      }
   }
   cout<<"nlines = "<<nlines<<endl;

   TCanvas* cans = new TCanvas("cans", "cans", 800, 600);
   cans->cd();
   PID_L3_L2B[4]->Draw("COL");
}


//______________________________________________________________________________
// 自定义径迹查找算法2 : 循环-循环-循环-循环-判断-判断-判断-判断
// 经验证, 算法2 与 算法1 等效 !
// 为方便起见, 使用算法2进行后续分析
void Test_Multi::TrackFindingAlgorithm2()
{
  Double_t L2FL2B_EnergyCut = 0.15;
  Int_t  nlines = 0;

  ofstream  FileOut("Test_Multi2.dat");
  //____________________________________________________________________________
  if (fChain == 0) return;
  fChain->SetBranchStatus("*", 0);
  fChain->SetBranchStatus("SSD1.fL1SMulti",    1);
  fChain->SetBranchStatus("SSD1.fL1SNumStrip", 1);
  fChain->SetBranchStatus("SSD1.fL1SEMeV",     1);
  fChain->SetBranchStatus("SSD1.fL2FMulti",    1);
  fChain->SetBranchStatus("SSD1.fL2FNumStrip", 1);
  fChain->SetBranchStatus("SSD1.fL2FEMeV",     1);
  fChain->SetBranchStatus("SSD1.fL2BMulti",    1);
  fChain->SetBranchStatus("SSD1.fL2BNumStrip", 1);
  fChain->SetBranchStatus("SSD1.fL2BEMeV",     1);
  fChain->SetBranchStatus("SSD1.fCsIMulti",    1);
  fChain->SetBranchStatus("SSD1.fCsINum",      1);
  fChain->SetBranchStatus("SSD1.fCsIECh",      1);

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {

    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    // if (Cut(ientry) < 0) continue;

    Int_t fGlobalMulti = 0;

    if (SSD1_fCsIMulti==0) continue;
    if (SSD1_fL2BMulti==0) continue;
    if (SSD1_fL2FMulti==0) continue;
    if (SSD1_fL1SMulti==0) continue;


    for (Int_t csich=0; csich<SSD1_fCsIMulti; csich++) {
      for (Int_t stripl2b=0; stripl2b<SSD1_fL2BMulti; stripl2b++) {
        for (Int_t stripl2f=0; stripl2f<SSD1_fL2FMulti; stripl2f++) {
          for (Int_t stripl1s=0; stripl1s<SSD1_fL1SMulti; stripl1s++) {
            if (fPattern.GeoConstraint_L3A_L2B(SSD1_fCsINum[csich], SSD1_fL2BNumStrip[stripl2b]) &&
                fPattern.EneConstraint_L2B_L2F(SSD1_fL2BEMeV[stripl2b], SSD1_fL2FEMeV[stripl2f], L2FL2B_EnergyCut) &&
                fPattern.GeoConstraint_L3A_L2F(SSD1_fCsINum[csich], SSD1_fL2FNumStrip[stripl2f]) &&
                fPattern.GeoConstraint_L2B_L1S(SSD1_fL2BNumStrip[stripl2b], SSD1_fL1SNumStrip[stripl1s]))
            {
              fGlobalMulti++;
              nlines++;

              cout<<setw(10)<<"M_L3A = "<<setw(2)<<SSD1_fCsIMulti
                  <<setw(10)<<"M_L2B = "<<setw(2)<<SSD1_fL2BMulti
                  <<setw(10)<<"M_L2F = "<<setw(2)<<SSD1_fL2FMulti
                  <<setw(10)<<"M_L1S = "<<setw(2)<<SSD1_fL1SMulti
                  <<setw(15)<<"MultiGlobal = "<<fGlobalMulti
                  <<setw(10)<<"CSI = "<<setw(2)<<SSD1_fCsINum[csich]
                  <<setw(10)<<"L2B = "<<setw(2)<<SSD1_fL2BNumStrip[stripl2b]
                  <<setw(10)<<"L2F = "<<setw(2)<<SSD1_fL2FNumStrip[stripl2f]
                  <<setw(10)<<"L1S = "<<setw(2)<<SSD1_fL1SNumStrip[stripl1s]
                  <<setw(10)<<"EL1S = "<<setw(7)<<SSD1_fL1SEMeV[stripl1s]
                  <<setw(10)<<"EL2F = "<<setw(7)<<SSD1_fL2FEMeV[stripl2f]
                  <<setw(10)<<"EL2B = "<<setw(7)<<SSD1_fL2BEMeV[stripl2b]
                  <<setw(10)<<"ECsI = "<<setw(4)<<SSD1_fCsIECh[csich]
                  <<endl;

              FileOut<<setw(10)<<"M_L3A = "<<setw(2)<<SSD1_fCsIMulti
                     <<setw(10)<<"M_L2B = "<<setw(2)<<SSD1_fL2BMulti
                     <<setw(10)<<"M_L2F = "<<setw(2)<<SSD1_fL2FMulti
                     <<setw(10)<<"M_L1S = "<<setw(2)<<SSD1_fL1SMulti
                     <<setw(15)<<"MultiGlobal = "<<fGlobalMulti
                     <<setw(10)<<"CSI = "<<setw(2)<<SSD1_fCsINum[csich]
                     <<setw(10)<<"L2B = "<<setw(2)<<SSD1_fL2BNumStrip[stripl2b]
                     <<setw(10)<<"L2F = "<<setw(2)<<SSD1_fL2FNumStrip[stripl2f]
                     <<setw(10)<<"L1S = "<<setw(2)<<SSD1_fL1SNumStrip[stripl1s]
                     <<setw(10)<<"EL1S = "<<setw(7)<<SSD1_fL1SEMeV[stripl1s]
                     <<setw(10)<<"EL2F = "<<setw(7)<<SSD1_fL2FEMeV[stripl2f]
                     <<setw(10)<<"EL2B = "<<setw(7)<<SSD1_fL2BEMeV[stripl2b]
                     <<setw(10)<<"ECsI = "<<setw(4)<<SSD1_fCsIECh[csich]
                     <<endl;
            }
          }
        }
      }
    }
  }
  cout<<"nlines = "<<nlines<<endl;
}


//______________________________________________________________________________
// 测试约束条件对事件数的影响
void Test_Multi::CheckCutEffOnMulti()
{
  Double_t L2FL2B_EnergyCut = 0.15;

  Int_t Entries_L3A_MultiCut = 0;
  Int_t Entries_L2B_MultiCut = 0;
  Int_t Entries_L2F_MultiCut = 0;
  Int_t Entries_L1S_MultiCut = 0;

  Int_t Entries_L3A_L3AL2B   = 0;
  Int_t Entries_L3A_L3AL2F   = 0;

  Int_t Entries_L2B_L3AL2B   = 0;
  Int_t Entries_L2B_L2BL2F   = 0;
  Int_t Entries_L2B_L2BL1S   = 0;

  Int_t Entries_L2F_L3AL2F   = 0;
  Int_t Entries_L2F_L2BL2F   = 0;

  Int_t Entries_L1S_L2BL1S   = 0;

  if (fChain == 0) return;
  fChain->SetBranchStatus("*", 0);
  fChain->SetBranchStatus("SSD1.fL1SMulti",    1);
  fChain->SetBranchStatus("SSD1.fL1SNumStrip", 1);
  fChain->SetBranchStatus("SSD1.fL1SEMeV",     1);
  fChain->SetBranchStatus("SSD1.fL2FMulti",    1);
  fChain->SetBranchStatus("SSD1.fL2FNumStrip", 1);
  fChain->SetBranchStatus("SSD1.fL2FEMeV",     1);
  fChain->SetBranchStatus("SSD1.fL2BMulti",    1);
  fChain->SetBranchStatus("SSD1.fL2BNumStrip", 1);
  fChain->SetBranchStatus("SSD1.fL2BEMeV",     1);
  fChain->SetBranchStatus("SSD1.fCsIMulti",    1);
  fChain->SetBranchStatus("SSD1.fCsINum",      1);
  fChain->SetBranchStatus("SSD1.fCsIECh",      1);

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      //___________________________________________
      // 考查每一层的 Multi>0 时探测到的粒子数
      // L3A
      if (SSD1_fCsIMulti!=0) {
        for (Int_t csich=0; csich<SSD1_fCsIMulti; csich++) {
          Entries_L3A_MultiCut++;
          if (fPattern.GeoConstraint_L3A_L2B(csich, SSD1_fL2BNumStrip, SSD1_fL2BMulti)) {
            Entries_L3A_L3AL2B++;
            if (fPattern.GeoConstraint_L3A_L2F(csich, SSD1_fL2FNumStrip, SSD1_fL2FMulti)) {
              Entries_L3A_L3AL2F++;
            }
          }
        }
      }
      // L2B
      if (SSD1_fL2BMulti!=0) {
        for (Int_t l2bch=0; l2bch<SSD1_fL2BMulti; l2bch++) {
          Entries_L2B_MultiCut++;
          if (fPattern.GeoConstraint_L3A_L2B(SSD1_fCsINum, SSD1_fL2BNumStrip[l2bch], SSD1_fCsIMulti)) {
            Entries_L2B_L3AL2B++;
            if (fPattern.GeoConstraint_L3A_L2F(SSD1_fCsINum, SSD1_fL2FNumStrip, SSD1_fCsIMulti, SSD1_fL2FMulti)) {
              if (fPattern.EneConstraint_L2B_L2F(SSD1_fL2BEMeV[l2bch], SSD1_fL2FEMeV, SSD1_fL2FMulti, L2FL2B_EnergyCut)) {
                Entries_L2B_L2BL2F++;
                if (fPattern.GeoConstraint_L2B_L1S(SSD1_fL2BNumStrip[l2bch], SSD1_fL1SNumStrip, SSD1_fL1SMulti)) {
                  Entries_L2B_L2BL1S++;
                }
              }
            }
          }
        }
      }
      // L2F
      if (SSD1_fL2FMulti!=0) {
        for (Int_t l2fch=0; l2fch<SSD1_fL2FMulti; l2fch++) {
          Entries_L2F_MultiCut++;
          if (fPattern.GeoConstraint_L3A_L2F(SSD1_fCsINum, SSD1_fL2FNumStrip[l2fch], SSD1_fCsIMulti)) {
            Entries_L2F_L3AL2F++;
            if (fPattern.GeoConstraint_L3A_L2B(SSD1_fCsINum, SSD1_fL2BNumStrip, SSD1_fCsIMulti, SSD1_fL2BMulti)) {
              if (fPattern.GeoConstraint_L2B_L1S(SSD1_fL2BNumStrip, SSD1_fL1SNumStrip, SSD1_fL2BMulti, SSD1_fL1SMulti)) {
                if (fPattern.EneConstraint_L2B_L2F(SSD1_fL2BEMeV, SSD1_fL2FEMeV[l2fch], SSD1_fL2BMulti, L2FL2B_EnergyCut)) {
                  Entries_L2F_L2BL2F++;
                }
              }
            }
          }
        }
      }
      // L1S
      if (SSD1_fL1SMulti!=0) {
        for (Int_t l1sch=0; l1sch<SSD1_fL1SMulti; l1sch++) {
          Entries_L1S_MultiCut++;
          if (fPattern.GeoConstraint_L3A_L2B(SSD1_fCsINum, SSD1_fL2BNumStrip, SSD1_fCsIMulti, SSD1_fL2BMulti)) {
            if (fPattern.GeoConstraint_L3A_L2F(SSD1_fCsINum, SSD1_fL2FNumStrip, SSD1_fCsIMulti, SSD1_fL2FMulti)) {
              if (fPattern.EneConstraint_L2B_L2F(SSD1_fL2BEMeV, SSD1_fL2FEMeV, SSD1_fL2BMulti, SSD1_fL2FMulti, L2FL2B_EnergyCut)) {
                if (fPattern.GeoConstraint_L2B_L1S(SSD1_fL2BNumStrip, SSD1_fL1SNumStrip[l1sch], SSD1_fL2BMulti)) {
                  Entries_L1S_L2BL1S++;
                }
              }
            }
          }
        }
      }
   }
   cout<<"Entries_L3A_MultiCut = "<<Entries_L3A_MultiCut<<endl;
   cout<<"Entries_L3A_L3AL2B   = "<<Entries_L3A_L3AL2B<<endl;
   cout<<"Entries_L3A_L3AL2F   = "<<Entries_L3A_L3AL2F<<endl;
   cout<<endl;
   cout<<"Entries_L2B_MultiCut = "<<Entries_L2B_MultiCut<<endl;
   cout<<"Entries_L2B_L3AL2B   = "<<Entries_L2B_L3AL2B<<endl;
   cout<<"Entries_L2B_L2BL2F   = "<<Entries_L2B_L2BL2F<<endl;
   cout<<"Entries_L2B_L2BL1S   = "<<Entries_L2B_L2BL1S<<endl;
   cout<<endl;
   cout<<"Entries_L2F_MultiCut = "<<Entries_L2F_MultiCut<<endl;
   cout<<"Entries_L2F_L3AL2F   = "<<Entries_L2F_L3AL2F<<endl;
   cout<<"Entries_L2F_L2BL2F   = "<<Entries_L2F_L2BL2F<<endl;
   cout<<endl;
   cout<<"Entries_L1S_MultiCut = "<<Entries_L1S_MultiCut<<endl;
   cout<<"Entries_L1S_L2BL1S   = "<<Entries_L1S_L2BL1S<<endl;

}


//______________________________________________________________________________
// 测试 L2B_L2F 能量关联
void Test_Multi::CheckL2BL2FEnergyCorrelation()
{
  gStyle->SetOptStat(1);
  gStyle->SetPalette(1);

  std::string pathL2BL2FEneCorr_Cut("figures/figure_HitPatternRecognition/SSD1_L2BL2F_EneCorrCut_Run120-Run130.pdf");
  std::string pathL2BL2FEneCorr_EnEratioMeV("figures/figure_HitPatternRecognition/SSD1_L2BL2F_EneCorrEnEratioMeV_Run120-Run130.pdf");
  std::string pathL2BL2FEneCorr_EnEratioPer("figures/figure_HitPatternRecognition/SSD1_L2BL2F_EneCorrEnEratioPer_Run120-Run130.pdf");

  Double_t L2FL2B_EnergyDiff_MeV;
  Double_t L2FL2B_EnergyDiff_Per;

  Double_t EnergyCut[4] = {0.05, 0.10, 0.15, 0.20};
  TH2D* hist_L2B_L2F[4];
  for (Int_t i=0; i<4; i++) {
    hist_L2B_L2F[i] = new TH2D(Form("hist_L2B_L2F_Cut%.2lf",EnergyCut[i]),Form("hist_L2B_L2F_Cut%.2lf",EnergyCut[i]),3000,0.,300.,3000,0.,300.);
  }

  TH2D* hist2_EnergyDifference_MeV = new TH2D("hist2_L2FL2B_EnEratio_MeV","",3000,0.,300.,1000,-50.,50.);
  TH2D* hist2_EnergyDifference_Per = new TH2D("hist2_L2FL2B_EnEratio_Per","",3000,0.,300.,600, -30., 30.);

  if (fChain == 0) return;
  fChain->SetBranchStatus("*", 0);
  fChain->SetBranchStatus("SSD1.fL1SMulti",    1);
  fChain->SetBranchStatus("SSD1.fL1SNumStrip", 1);
  fChain->SetBranchStatus("SSD1.fL1SEMeV",     1);
  fChain->SetBranchStatus("SSD1.fL2FMulti",    1);
  fChain->SetBranchStatus("SSD1.fL2FNumStrip", 1);
  fChain->SetBranchStatus("SSD1.fL2FEMeV",     1);
  fChain->SetBranchStatus("SSD1.fL2BMulti",    1);
  fChain->SetBranchStatus("SSD1.fL2BNumStrip", 1);
  fChain->SetBranchStatus("SSD1.fL2BEMeV",     1);
  fChain->SetBranchStatus("SSD1.fCsIMulti",    1);
  fChain->SetBranchStatus("SSD1.fCsINum",      1);
  fChain->SetBranchStatus("SSD1.fCsIECh",      1);

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    for (Int_t i=0; i<4; i++) {
      if (SSD1_fCsIMulti!=0 && SSD1_fL2BMulti!=0 && SSD1_fL2FMulti!=0) {
        if (fPattern.GeoConstraint_L3A_L2B(SSD1_fCsINum, SSD1_fL2BNumStrip, SSD1_fCsIMulti, SSD1_fL2BMulti)) {
          if (fPattern.GeoConstraint_L2B_L1S(SSD1_fL2BNumStrip, SSD1_fL1SNumStrip, SSD1_fL2BMulti, SSD1_fL1SMulti)) {
            if (fPattern.GeoConstraint_L3A_L2F(SSD1_fCsINum, SSD1_fL2FNumStrip, SSD1_fCsIMulti, SSD1_fL2FMulti)) {
              for (Int_t l2bch=0; l2bch<SSD1_fL2BMulti; l2bch++) {
                for (Int_t l2fch=0; l2fch<SSD1_fL2FMulti; l2fch++) {
                  if (fPattern.EneConstraint_L2B_L2F(SSD1_fL2BEMeV[l2bch], SSD1_fL2FEMeV[l2fch], EnergyCut[i])) {
                    hist_L2B_L2F[i]->Fill(SSD1_fL2BEMeV[l2bch], SSD1_fL2FEMeV[l2fch]);

                    L2FL2B_EnergyDiff_MeV = SSD1_fL2BEMeV[l2bch]-SSD1_fL2FEMeV[l2fch];
                    hist2_EnergyDifference_MeV->Fill(SSD1_fL2BEMeV[l2bch], L2FL2B_EnergyDiff_MeV);

                    L2FL2B_EnergyDiff_Per = L2FL2B_EnergyDiff_MeV/SSD1_fL2BEMeV[l2bch] * 100; // %
                    hist2_EnergyDifference_Per->Fill(SSD1_fL2BEMeV[l2bch], L2FL2B_EnergyDiff_Per);
                  }
                }
              }
            }
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
    hist_L2B_L2F[i]->GetXaxis()->SetTitle("SSD1_EL2B (MeV)");
    hist_L2B_L2F[i]->GetXaxis()->CenterTitle(1);
    hist_L2B_L2F[i]->GetXaxis()->SetTitleSize(0.05);
    hist_L2B_L2F[i]->GetXaxis()->SetLabelSize(0.05);

    hist_L2B_L2F[i]->GetYaxis()->SetTitle("SSD1_EL2F (MeV)");
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
  hist2_EnergyDifference_MeV->GetXaxis()->SetTitle("SSD1_EL2B (MeV)");
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
  hist2_EnergyDifference_Per->GetXaxis()->SetTitle("SSD1_EL2B (MeV)");
  hist2_EnergyDifference_Per->GetXaxis()->CenterTitle(1);
  hist2_EnergyDifference_Per->GetXaxis()->SetTitleSize(0.05);
  hist2_EnergyDifference_Per->GetXaxis()->SetLabelSize(0.05);
  hist2_EnergyDifference_Per->GetXaxis()->SetNdivisions(508);
  hist2_EnergyDifference_Per->GetYaxis()->SetTitle("(EL2B - EL2F)/EL2B (%)");
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


//______________________________________________________________________________
// 计算不同粒子在每套硅望远镜两层硅中的能损
void Test_Multi::CheckEnergyLossL1L2()
{
  EnergyLossModule eloss;
  Double_t Eloss_L1[16];
  Double_t Eloss_L2[16];
  Double_t Eres_L2 [16];

  for (Int_t issd=0; issd<4; issd++) {
    std::string pathDataOut(Form("SSD%d_L1L2ELossTest.dat", issd+1));
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


//______________________________________________________________________________
void Test_Multi::CheckEnergyLossL1L2_Relationship()
{
  gStyle->SetOptStat(0);

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
    pathSSDELossInput[i] = Form("data/SSD%d_L1L2ELossTest.dat", i+1);

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
      if (Eres==0) continue;  // 只考虑能穿透第二层硅的情况
      dE1.push_back(dEL1);
      dE2.push_back(dEL2);
      EL1EL2Ratio.push_back(dEL1/dEL2 * 100);
    }
    if (i==0) { dE1_SSD1=dE1;  dE2_SSD1=dE2; EL1EL2Ratio_SSD1=EL1EL2Ratio; };
    if (i==1) { dE1_SSD2=dE1;  dE2_SSD2=dE2; EL1EL2Ratio_SSD2=EL1EL2Ratio; };
    if (i==2) { dE1_SSD3=dE1;  dE2_SSD3=dE2; EL1EL2Ratio_SSD3=EL1EL2Ratio; };
    if (i==3) { dE1_SSD4=dE1;  dE2_SSD4=dE2; EL1EL2Ratio_SSD4=EL1EL2Ratio; };
  }

  // SSD1_DEE
  TH2D* hist2_SSD1_dEE = new TH2D("hist2_SSD1_DEE_LISE++","hist2_SSD1_DEE_LISE++",3000,0.,300.,1500,0.,150.);
  TH1D* hist1_SSD1_Eratio = new TH1D("hist1_SSD1_Eratio_LISE++", "hist1_SSD1_Eratio_LISE++", 500, 0., 50.);
  for (Int_t i=0; i<dE1_SSD1.size(); i++) {
    hist2_SSD1_dEE->Fill(dE2_SSD1[i], dE1_SSD1[i]);
    hist1_SSD1_Eratio->Fill(EL1EL2Ratio_SSD1[i]);
  }
  // SSD2_DEE
  TH2D* hist2_SSD2_dEE = new TH2D("hist2_SSD2_DEE_LISE++","hist2_SSD2_DEE_LISE++",3000,0.,300.,1500,0.,150.);
  TH1D* hist1_SSD2_Eratio = new TH1D("hist1_SSD2_Eratio_LISE++", "hist1_SSD2_Eratio_LISE++", 500, 0., 50.);
  for (Int_t i=0; i<dE1_SSD2.size(); i++) {
    hist2_SSD2_dEE->Fill(dE2_SSD2[i], dE1_SSD2[i]);
    hist1_SSD2_Eratio->Fill(EL1EL2Ratio_SSD2[i]);
  }
  // SSD3_DEE
  TH2D* hist2_SSD3_dEE = new TH2D("hist2_SSD3_DEE_LISE++","hist2_SSD3_DEE_LISE++",2000,0.,200.,800,0.,80.);
  TH1D* hist1_SSD3_Eratio = new TH1D("hist1_SSD3_Eratio_LISE++", "hist1_SSD3_Eratio_LISE++", 500, 0., 50.);
  for (Int_t i=0; i<dE1_SSD3.size(); i++) {
    hist2_SSD3_dEE->Fill(dE2_SSD3[i], dE1_SSD3[i]);
    hist1_SSD3_Eratio->Fill(EL1EL2Ratio_SSD3[i]);
  }
  // SSD4_DEE
  TH2D* hist2_SSD4_dEE = new TH2D("hist2_SSD4_DEE_LISE++","hist2_SSD4_DEE_LISE++",1500,0.,150.,600,0.,60.);
  TH1D* hist1_SSD4_Eratio = new TH1D("hist1_SSD4_Eratio_LISE++", "hist1_SSD4_Eratio_LISE++", 500, 0., 50.);
  for (Int_t i=0; i<dE1_SSD4.size(); i++) {
    hist2_SSD4_dEE->Fill(dE2_SSD4[i], dE1_SSD4[i]);
    hist1_SSD4_Eratio->Fill(EL1EL2Ratio_SSD4[i]);
  }

  TF1* func_L2B_eq_L2F = new TF1("func", "x", 0, 300);
  func_L2B_eq_L2F->SetLineColor(kRed);
  func_L2B_eq_L2F->SetLineStyle(7);
  func_L2B_eq_L2F->SetLineWidth(4);

  //____________________________________________________________________________
  // 画出 DEE 曲线
  TCanvas* cans = new TCanvas("cans","cans",1000,1000);
  cans->Divide(2,2);

  cans->cd(1);
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.12);
  hist2_SSD1_dEE->Draw("COL");
  func_L2B_eq_L2F->Draw("SAME");
  hist2_SSD1_dEE->GetXaxis()->SetTitle("dE_{2} (MeV)");
  hist2_SSD1_dEE->GetXaxis()->SetTitleSize(0.05);
  hist2_SSD1_dEE->GetXaxis()->CenterTitle(1);
  hist2_SSD1_dEE->GetXaxis()->SetLabelSize(0.05);
  hist2_SSD1_dEE->GetXaxis()->SetNdivisions(508);
  hist2_SSD1_dEE->GetYaxis()->SetTitle("dE_{1} (MeV)");
  hist2_SSD1_dEE->GetYaxis()->SetTitleSize(0.05);
  hist2_SSD1_dEE->GetYaxis()->CenterTitle(1);
  hist2_SSD1_dEE->GetYaxis()->SetLabelSize(0.05);
  hist2_SSD1_dEE->GetYaxis()->SetNdivisions(508);

  cans->cd(2);
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.12);
  hist2_SSD2_dEE->Draw("COL");
  func_L2B_eq_L2F->Draw("SAME");
  hist2_SSD2_dEE->GetXaxis()->SetTitle("dE_{2} (MeV)");
  hist2_SSD2_dEE->GetXaxis()->SetTitleSize(0.05);
  hist2_SSD2_dEE->GetXaxis()->CenterTitle(1);
  hist2_SSD2_dEE->GetXaxis()->SetLabelSize(0.05);
  hist2_SSD2_dEE->GetXaxis()->SetNdivisions(508);
  hist2_SSD2_dEE->GetYaxis()->SetTitle("dE_{1} (MeV)");
  hist2_SSD2_dEE->GetYaxis()->SetTitleSize(0.05);
  hist2_SSD2_dEE->GetYaxis()->CenterTitle(1);
  hist2_SSD2_dEE->GetYaxis()->SetLabelSize(0.05);
  hist2_SSD2_dEE->GetYaxis()->SetNdivisions(508);

  cans->cd(3);
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.12);
  hist2_SSD3_dEE->Draw("COL");
  func_L2B_eq_L2F->Draw("SAME");
  hist2_SSD3_dEE->GetXaxis()->SetTitle("dE_{2} (MeV)");
  hist2_SSD3_dEE->GetXaxis()->SetTitleSize(0.05);
  hist2_SSD3_dEE->GetXaxis()->CenterTitle(1);
  hist2_SSD3_dEE->GetXaxis()->SetLabelSize(0.05);
  hist2_SSD3_dEE->GetXaxis()->SetNdivisions(508);
  hist2_SSD3_dEE->GetYaxis()->SetTitle("dE_{1} (MeV)");
  hist2_SSD3_dEE->GetYaxis()->SetTitleSize(0.05);
  hist2_SSD3_dEE->GetYaxis()->CenterTitle(1);
  hist2_SSD3_dEE->GetYaxis()->SetLabelSize(0.05);
  hist2_SSD3_dEE->GetYaxis()->SetNdivisions(508);

  cans->cd(4);
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.12);
  hist2_SSD4_dEE->Draw("COL");
  func_L2B_eq_L2F->Draw("SAME");
  hist2_SSD4_dEE->GetXaxis()->SetTitle("dE_{2} (MeV)");
  hist2_SSD4_dEE->GetXaxis()->SetTitleSize(0.05);
  hist2_SSD4_dEE->GetXaxis()->CenterTitle(1);
  hist2_SSD4_dEE->GetXaxis()->SetLabelSize(0.05);
  hist2_SSD4_dEE->GetXaxis()->SetNdivisions(508);
  hist2_SSD4_dEE->GetYaxis()->SetTitle("dE_{1} (MeV)");
  hist2_SSD4_dEE->GetYaxis()->SetTitleSize(0.05);
  hist2_SSD4_dEE->GetYaxis()->CenterTitle(1);
  hist2_SSD4_dEE->GetYaxis()->SetLabelSize(0.05);
  hist2_SSD4_dEE->GetYaxis()->SetNdivisions(508);


  //________________________________________________________
  // 画出 (EL2-EL1)/EL2 分布
  TCanvas* c_Eratio = new TCanvas("c_Eratio","c_Eratio",1000,1000);
  c_Eratio->Divide(2,2);

  c_Eratio->cd(1);
  gPad->SetLeftMargin(0.17);
  gPad->SetBottomMargin(0.14);
  hist1_SSD1_Eratio->Draw();
  hist1_SSD1_Eratio->GetXaxis()->SetTitle("dE1/dE2 (%)");
  hist1_SSD1_Eratio->GetXaxis()->SetTitleSize(0.05);
  hist1_SSD1_Eratio->GetXaxis()->CenterTitle(1);
  hist1_SSD1_Eratio->GetXaxis()->SetLabelSize(0.05);
  hist1_SSD1_Eratio->GetXaxis()->SetNdivisions(505);
  hist1_SSD1_Eratio->GetYaxis()->SetTitle("Counts");
  hist1_SSD1_Eratio->GetYaxis()->SetTitleSize(0.05);
  hist1_SSD1_Eratio->GetYaxis()->CenterTitle(1);
  hist1_SSD1_Eratio->GetYaxis()->SetLabelSize(0.05);
  hist1_SSD1_Eratio->GetYaxis()->SetNdivisions(505);

  c_Eratio->cd(2);
  gPad->SetLeftMargin(0.17);
  gPad->SetBottomMargin(0.14);
  hist1_SSD2_Eratio->Draw();
  hist1_SSD2_Eratio->GetXaxis()->SetTitle("dE1/dE2 (%)");
  hist1_SSD2_Eratio->GetXaxis()->SetTitleSize(0.05);
  hist1_SSD2_Eratio->GetXaxis()->CenterTitle(1);
  hist1_SSD2_Eratio->GetXaxis()->SetLabelSize(0.05);
  hist1_SSD2_Eratio->GetXaxis()->SetNdivisions(505);
  hist1_SSD2_Eratio->GetYaxis()->SetTitle("Counts");
  hist1_SSD2_Eratio->GetYaxis()->SetTitleSize(0.05);
  hist1_SSD2_Eratio->GetYaxis()->CenterTitle(1);
  hist1_SSD2_Eratio->GetYaxis()->SetLabelSize(0.05);
  hist1_SSD2_Eratio->GetYaxis()->SetNdivisions(505);

  c_Eratio->cd(3);
  gPad->SetLeftMargin(0.17);
  gPad->SetBottomMargin(0.14);
  hist1_SSD3_Eratio->Draw();
  hist1_SSD3_Eratio->GetXaxis()->SetTitle("dE1/dE2 (%)");
  hist1_SSD3_Eratio->GetXaxis()->SetTitleSize(0.05);
  hist1_SSD3_Eratio->GetXaxis()->CenterTitle(1);
  hist1_SSD3_Eratio->GetXaxis()->SetLabelSize(0.05);
  hist1_SSD3_Eratio->GetXaxis()->SetNdivisions(505);
  hist1_SSD3_Eratio->GetYaxis()->SetTitle("Counts");
  hist1_SSD3_Eratio->GetYaxis()->SetTitleSize(0.05);
  hist1_SSD3_Eratio->GetYaxis()->CenterTitle(1);
  hist1_SSD3_Eratio->GetYaxis()->SetLabelSize(0.05);
  hist1_SSD3_Eratio->GetYaxis()->SetNdivisions(505);

  c_Eratio->cd(4);
  gPad->SetLeftMargin(0.17);
  gPad->SetBottomMargin(0.14);
  hist1_SSD4_Eratio->Draw();
  hist1_SSD4_Eratio->GetXaxis()->SetTitle("dE1/dE2 (%)");
  hist1_SSD4_Eratio->GetXaxis()->SetTitleSize(0.05);
  hist1_SSD4_Eratio->GetXaxis()->CenterTitle(1);
  hist1_SSD4_Eratio->GetXaxis()->SetLabelSize(0.05);
  hist1_SSD4_Eratio->GetXaxis()->SetNdivisions(505);
  hist1_SSD4_Eratio->GetYaxis()->SetTitle("Counts");
  hist1_SSD4_Eratio->GetYaxis()->SetTitleSize(0.05);
  hist1_SSD4_Eratio->GetYaxis()->CenterTitle(1);
  hist1_SSD4_Eratio->GetYaxis()->SetLabelSize(0.05);
  hist1_SSD4_Eratio->GetYaxis()->SetNdivisions(505);
}
