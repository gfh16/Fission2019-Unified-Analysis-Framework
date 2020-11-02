#define Test_Multi_cxx
#include "Test_Multi.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include "include/TimeAndPercentage.h"

using namespace std;

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
  TimeAndPercentage time;
  TH2D* Correlation_L2B_L2F = new TH2D("hist_PID", "hist_PID", 200,0.,200.,200,0.,200.);
  TH2D* PID_L2B_L1S = new TH2D("PID_L2B_L1S", "PID_L2B_L1S", 200, 0., 200., 100, 0., 100.);
  ofstream  FileOut("Test_Multi.dat");

//______________________________________________________________________________
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {

     //if (jentry>10000000) continue;
     //time.PrintPercentageAndRemainingTime(jentry, nentries);

      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;

      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
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

      if (SSD1_fCsIMulti == 0) continue;
      if (SSD1_fL2BMulti > 5 || SSD1_fL2BMulti==0) continue;
      if (SSD1_fL2FMulti > 5 || SSD1_fL2FMulti==0) continue;
      if (SSD1_fL1SMulti > 5 || SSD1_fL1SMulti==0) continue;

      for (Int_t csich=0; csich<SSD1_fCsIMulti; csich++)
      {
        Int_t fGlobalMulti = 0;
        for (Int_t stripl2b=0; stripl2b<SSD1_fL2BMulti; stripl2b++)
        {
          if (fPattern.GeoConstraint_L3A_L2B(SSD1_fCsINum[csich], SSD1_fL2BNumStrip[stripl2b]))
          {
            for (Int_t stripl2f=0; stripl2f<SSD1_fL2FMulti; stripl2f++)
            {
              if (fPattern.EnergyConstraint_L2B_L2F(SSD1_fL2BEMeV[stripl2b], SSD1_fL2FEMeV[stripl2f], 0.05)
              && fPattern.GeoConstraint_L3A_L2F(SSD1_fCsINum[csich], SSD1_fL2FNumStrip[stripl2f]))
              {
                for (Int_t stripl1s=0; stripl1s<SSD1_fL1SMulti; stripl1s++)
                {
                  if (fPattern.GeoConstraint_L2B_L1S(SSD1_fL2BNumStrip[stripl2b], SSD1_fL1SNumStrip[stripl1s]))
                  {
                    fGlobalMulti++;

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

                    //Correlation_L2B_L2F->Fill(SSD1_fL2BEMeV[stripl2b], SSD1_fL2FEMeV[stripl2f]);
                    //PID_L2B_L1S->Fill(SSD1_fL2FEMeV[stripl2f], SSD1_fL1SEMeV[stripl1s]);
                  }
                }
              }
            }
          }
        }
      }
   }
   TCanvas* cans = new TCanvas("cans", "cans", 800, 600);
   cans->cd();
   PID_L2B_L1S->Draw("COL");
   cans->Print("PID.png");
}
