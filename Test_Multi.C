//#define Test_Multi_cxx
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
Test_Multi::Test_Multi(TTree *tree) : fChain(0)
{
 // if parameter tree is not specified (or zero), connect the file
 // used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/home/sea/Fission2019_Data/CSHINEEvent/EventTree_Run0120-Run0130.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/home/sea/Fission2019_Data/CSHINEEvent/EventTree_Run0120-Run0130.root");
      }
      f->GetObject("CSHINEEvent",tree);

   }
   Init(tree);
}

Test_Multi::~Test_Multi()
{
   if (!fChain) return;
  // delete fChain->GetCurrentFile();
}

Int_t Test_Multi::GetEntry(Long64_t entry)
{
 // Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}

Long64_t Test_Multi::LoadTree(Long64_t entry)
{
 // Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Test_Multi::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("SSD1.fL1SMulti", &SSD1_fL1SMulti, &b_SSD1_fL1SMulti);
   fChain->SetBranchAddress("SSD1.fL1SNumStrip", SSD1_fL1SNumStrip, &b_SSD1_fL1SNumStrip);
   fChain->SetBranchAddress("SSD1.fL1SEMeV", SSD1_fL1SEMeV, &b_SSD1_fL1SEMeV);
   fChain->SetBranchAddress("SSD1.fL2FMulti", &SSD1_fL2FMulti, &b_SSD1_fL2FMulti);
   fChain->SetBranchAddress("SSD1.fL2FNumStrip", SSD1_fL2FNumStrip, &b_SSD1_fL2FNumStrip);
   fChain->SetBranchAddress("SSD1.fL2FEMeV", SSD1_fL2FEMeV, &b_SSD1_fL2FEMeV);
   fChain->SetBranchAddress("SSD1.fL2BMulti", &SSD1_fL2BMulti, &b_SSD1_fL2BMulti);
   fChain->SetBranchAddress("SSD1.fL2BNumStrip", SSD1_fL2BNumStrip, &b_SSD1_fL2BNumStrip);
   fChain->SetBranchAddress("SSD1.fL2BEMeV", SSD1_fL2BEMeV, &b_SSD1_fL2BEMeV);
   fChain->SetBranchAddress("SSD1.fCsIMulti", &SSD1_fCsIMulti, &b_SSD1_fCsIMulti);
   fChain->SetBranchAddress("SSD1.fCsINum", SSD1_fCsINum, &b_SSD1_fCsINum);
   fChain->SetBranchAddress("SSD1.fCsIECh", SSD1_fCsIECh, &b_SSD1_fCsIECh);
   fChain->SetBranchAddress("SSD2.fL1SMulti", &SSD2_fL1SMulti, &b_SSD2_fL1SMulti);
   fChain->SetBranchAddress("SSD2.fL1SNumStrip", SSD2_fL1SNumStrip, &b_SSD2_fL1SNumStrip);
   fChain->SetBranchAddress("SSD2.fL1SEMeV", SSD2_fL1SEMeV, &b_SSD2_fL1SEMeV);
   fChain->SetBranchAddress("SSD2.fL2FMulti", &SSD2_fL2FMulti, &b_SSD2_fL2FMulti);
   fChain->SetBranchAddress("SSD2.fL2FNumStrip", SSD2_fL2FNumStrip, &b_SSD2_fL2FNumStrip);
   fChain->SetBranchAddress("SSD2.fL2FEMeV", SSD2_fL2FEMeV, &b_SSD2_fL2FEMeV);
   fChain->SetBranchAddress("SSD2.fL2BMulti", &SSD2_fL2BMulti, &b_SSD2_fL2BMulti);
   fChain->SetBranchAddress("SSD2.fL2BNumStrip", SSD2_fL2BNumStrip, &b_SSD2_fL2BNumStrip);
   fChain->SetBranchAddress("SSD2.fL2BEMeV", SSD2_fL2BEMeV, &b_SSD2_fL2BEMeV);
   fChain->SetBranchAddress("SSD2.fCsIMulti", &SSD2_fCsIMulti, &b_SSD2_fCsIMulti);
   fChain->SetBranchAddress("SSD2.fCsINum", SSD2_fCsINum, &b_SSD2_fCsINum);
   fChain->SetBranchAddress("SSD2.fCsIECh", SSD2_fCsIECh, &b_SSD2_fCsIECh);
   fChain->SetBranchAddress("SSD3.fL1SMulti", &SSD3_fL1SMulti, &b_SSD3_fL1SMulti);
   fChain->SetBranchAddress("SSD3.fL1SNumStrip", SSD3_fL1SNumStrip, &b_SSD3_fL1SNumStrip);
   fChain->SetBranchAddress("SSD3.fL1SEMeV", SSD3_fL1SEMeV, &b_SSD3_fL1SEMeV);
   fChain->SetBranchAddress("SSD3.fL2FMulti", &SSD3_fL2FMulti, &b_SSD3_fL2FMulti);
   fChain->SetBranchAddress("SSD3.fL2FNumStrip", SSD3_fL2FNumStrip, &b_SSD3_fL2FNumStrip);
   fChain->SetBranchAddress("SSD3.fL2FEMeV", SSD3_fL2FEMeV, &b_SSD3_fL2FEMeV);
   fChain->SetBranchAddress("SSD3.fL2BMulti", &SSD3_fL2BMulti, &b_SSD3_fL2BMulti);
   fChain->SetBranchAddress("SSD3.fL2BNumStrip", SSD3_fL2BNumStrip, &b_SSD3_fL2BNumStrip);
   fChain->SetBranchAddress("SSD3.fL2BEMeV", SSD3_fL2BEMeV, &b_SSD3_fL2BEMeV);
   fChain->SetBranchAddress("SSD3.fCsIMulti", &SSD3_fCsIMulti, &b_SSD3_fCsIMulti);
   fChain->SetBranchAddress("SSD3.fCsINum", SSD3_fCsINum, &b_SSD3_fCsINum);
   fChain->SetBranchAddress("SSD3.fCsIECh", SSD3_fCsIECh, &b_SSD3_fCsIECh);
   fChain->SetBranchAddress("SSD4.fL1SMulti", &SSD4_fL1SMulti, &b_SSD4_fL1SMulti);
   fChain->SetBranchAddress("SSD4.fL1SNumStrip", SSD4_fL1SNumStrip, &b_SSD4_fL1SNumStrip);
   fChain->SetBranchAddress("SSD4.fL1SEMeV", SSD4_fL1SEMeV, &b_SSD4_fL1SEMeV);
   fChain->SetBranchAddress("SSD4.fL2FMulti", &SSD4_fL2FMulti, &b_SSD4_fL2FMulti);
   fChain->SetBranchAddress("SSD4.fL2FNumStrip", SSD4_fL2FNumStrip, &b_SSD4_fL2FNumStrip);
   fChain->SetBranchAddress("SSD4.fL2FEMeV", SSD4_fL2FEMeV, &b_SSD4_fL2FEMeV);
   fChain->SetBranchAddress("SSD4.fL2BMulti", &SSD4_fL2BMulti, &b_SSD4_fL2BMulti);
   fChain->SetBranchAddress("SSD4.fL2BNumStrip", SSD4_fL2BNumStrip, &b_SSD4_fL2BNumStrip);
   fChain->SetBranchAddress("SSD4.fL2BEMeV", SSD4_fL2BEMeV, &b_SSD4_fL2BEMeV);
   fChain->SetBranchAddress("SSD4.fCsIMulti", &SSD4_fCsIMulti, &b_SSD4_fCsIMulti);
   fChain->SetBranchAddress("SSD4.fCsINum", SSD4_fCsINum, &b_SSD4_fCsINum);
   fChain->SetBranchAddress("SSD4.fCsIECh", SSD4_fCsIECh, &b_SSD4_fCsIECh);
   fChain->SetBranchAddress("Global.fGlobalMulti", &Global_fGlobalMulti, &b_Global_fGlobalMulti);
   fChain->SetBranchAddress("Global.fGNumOfSSD", &Global_fGNumOfSSD, &b_Global_fGNumOfSSD);
   fChain->SetBranchAddress("Global.fGL1SNumStrip", &Global_fGL1SNumStrip, &b_Global_fGL1SNumStrip);
   fChain->SetBranchAddress("Global.fGL1SEMeV", &Global_fGL1SEMeV, &b_Global_fGL1SEMeV);
   fChain->SetBranchAddress("Global.fGL2FNumStrip", &Global_fGL2FNumStrip, &b_Global_fGL2FNumStrip);
   fChain->SetBranchAddress("Global.fGL2FEMeV", &Global_fGL2FEMeV, &b_Global_fGL2FEMeV);
   fChain->SetBranchAddress("Global.fGL2BNumStrip", &Global_fGL2BNumStrip, &b_Global_fGL2BNumStrip);
   fChain->SetBranchAddress("Global.fGL2BEMeV", &Global_fGL2BEMeV, &b_Global_fGL2BEMeV);
   fChain->SetBranchAddress("Global.fGCsINum", &Global_fGCsINum, &b_Global_fGCsINum);
   fChain->SetBranchAddress("Global.fGCsIEMeV", &Global_fGCsIEMeV, &b_Global_fGCsIEMeV);
   fChain->SetBranchAddress("Global.fGL2FTime", &Global_fGL2FTime, &b_Global_fGL2FTime);
   fChain->SetBranchAddress("Global.fTheta", &Global_fTheta, &b_Global_fTheta);
   fChain->SetBranchAddress("Global.fPhi", &Global_fPhi, &b_Global_fPhi);
   fChain->SetBranchAddress("Global.fTotKinEnergy", &Global_fTotKinEnergy, &b_Global_fTotKinEnergy);
   fChain->SetBranchAddress("Global.fCalKinEnergy", &Global_fCalKinEnergy, &b_Global_fCalKinEnergy);
   fChain->SetBranchAddress("Global.fMomentum", &Global_fMomentum, &b_Global_fMomentum);
   fChain->SetBranchAddress("Global.fBeta", &Global_fBeta, &b_Global_fBeta);
   fChain->SetBranchAddress("Global.fZ", &Global_fZ, &b_Global_fZ);
   fChain->SetBranchAddress("Global.fA", &Global_fA, &b_Global_fA);
   Notify();
}

Bool_t Test_Multi::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Test_Multi::Show(Long64_t entry)
{
  // Print contents of entry.
  // If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}

Int_t Test_Multi::Cut(Long64_t entry)
{
  // This function may be called from Loop.
  // returns  1 if entry is accepted.
  // returns -1 otherwise.
   return 1;
}
//______________________________________________________________________________


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


//______________________________________________________________________________
// 自定义径迹查找算法1 : 循环-循环-判断-循环-判断-循环-判断
// 经验证, 算法2 与 算法1 等效 !
// 算法2 更加简洁明了，为方便起见，使用算法2进行后续分析
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


//_______________________________________________________
// 自定义径迹查找算法2 : 循环-循环-循环-循环-判断-判断-判断-判断
// 经验证, 算法2 与 算法1 等效 !
// 算法2 更加简洁明了，为方便起见，使用算法2进行后续分析
class LayerEvent
{
  public:
    Int_t                  GlobalMulti;
    std::vector<Int_t>     L1SNumStrip;
    std::vector<Double_t>  L1SEMeV;
    std::vector<Int_t>     L2FNumStrip;
    std::vector<Double_t>  L2FEMeV;
    std::vector<Int_t>     L2BNumStrip;
    std::vector<Double_t>  L2BEMeV;
    std::vector<Int_t>     CsINum;
    std::vector<Int_t>     CsIECh;

    void Clear()
    {
      L1SNumStrip.clear();
      L1SEMeV.clear();
      L2FNumStrip.clear();
      L2FEMeV.clear();
      L2BNumStrip.clear();
      L2BEMeV.clear();
      CsINum.clear();
      CsIECh.clear();
    }
};

void Test_Multi::TrackFindingAlgorithm2()
{
  Int_t       fGlobalMulti;
  Long64_t    nlines;
  Long64_t    effentries = 0;
  Long64_t    ll = 0;

  TimeAndPercentage timeper;

  LayerEvent layerevent;
  TFile* myfile = new TFile("/home/sea/Fission2019_Data/TrackFindingAlgorithm2.root","RECREATE");
  TTree* mytree = new TTree("TrackEvent","TrackEvent Tree");
  mytree->Branch("SSD1.", "LayerEvent", &layerevent, 32000, 2);

  ofstream  FileOut("data/Test_Multi_L1L2EnergyCut.dat");
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

    timeper.PrintPercentageAndRemainingTime(jentry, nentries);

    if ((SSD1_fCsIMulti==0)||(SSD1_fCsIMulti>MULTICUT_L3A)) continue;
    if ((SSD1_fL2BMulti==0)||(SSD1_fL2BMulti>MULTICUT_L2B)) continue;
    if ((SSD1_fL2FMulti==0)||(SSD1_fL2FMulti>MULTICUT_L2F)) continue;
    if ((SSD1_fL1SMulti==0)||(SSD1_fL1SMulti>MULTICUT_L1S)) continue;

    effentries++;
    fGlobalMulti = 0;
    nlines       = 0;

    for (Int_t csich=0; csich<SSD1_fCsIMulti; csich++) {
      for (Int_t stripl2b=0; stripl2b<SSD1_fL2BMulti; stripl2b++) {
        for (Int_t stripl2f=0; stripl2f<SSD1_fL2FMulti; stripl2f++) {
          for (Int_t stripl1s=0; stripl1s<SSD1_fL1SMulti; stripl1s++) {
            if (fPattern.GeoConstraint_L3A_L2B(SSD1_fCsINum[csich], SSD1_fL2BNumStrip[stripl2b]) &&
                fPattern.EneConstraint_L2B_L2F(0, SSD1_fL2BEMeV[stripl2b], SSD1_fL2FEMeV[stripl2f]) &&
                fPattern.GeoConstraint_L3A_L2F(SSD1_fCsINum[csich], SSD1_fL2FNumStrip[stripl2f]) &&
                fPattern.GeoConstraint_L2B_L1S(SSD1_fL2BNumStrip[stripl2b], SSD1_fL1SNumStrip[stripl1s]))
            {
              if (!fPattern.EneConstraint_L1_L2(0, SSD1_fL1SEMeV[stripl1s], SSD1_fL2BEMeV[stripl2b])) continue;

              fGlobalMulti++;
              nlines++;

              layerevent.CsINum.push_back(SSD1_fCsINum[csich]);
              layerevent.CsIECh.push_back(SSD1_fCsIECh[csich]);

              layerevent.L2BNumStrip.push_back(SSD1_fL2BNumStrip[stripl2b]);
              layerevent.L2BEMeV.push_back(SSD1_fL2BEMeV[stripl2b]);

              layerevent.L2FNumStrip.push_back(SSD1_fL2FNumStrip[stripl2f]);
              layerevent.L2FEMeV.push_back(SSD1_fL2FEMeV[stripl2f]);

              layerevent.L1SNumStrip.push_back(SSD1_fL1SNumStrip[stripl1s]);
              layerevent.L1SEMeV.push_back(SSD1_fL1SEMeV[stripl1s]);


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
                     <<setw(10)<<"CSI  = "<<setw(2)<<SSD1_fCsINum[csich]
                     <<setw(10)<<"L2B  = "<<setw(2)<<SSD1_fL2BNumStrip[stripl2b]
                     <<setw(10)<<"L2F  = "<<setw(2)<<SSD1_fL2FNumStrip[stripl2f]
                     <<setw(10)<<"L1S  = "<<setw(2)<<SSD1_fL1SNumStrip[stripl1s]
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
    if (nlines>0) ll++;
    layerevent.GlobalMulti = fGlobalMulti;
    mytree->Fill();
    layerevent.Clear();
  }
  mytree->Write();
  myfile->Close();

  Double_t ratio1 = (Double_t) effentries/nentries;
  Double_t ratio2 = (Double_t) nlines/nentries;
  Double_t ratio3 = (Double_t) ll/effentries;

  cout<<endl;
  cout<<"effentries = "<<effentries<<setw(15)<<ratio1<<endl;
  cout<<"nlines = "<<nlines<<setw(15)<<ratio2<<endl;
  cout<<"ll = "<<ll<<setw(15)<<"ratio = "<<ratio3<<endl;

}
//______________________________________________________________________________


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


//______________________________________________________________________________
// 测试 L2B_L2F 能量关联
// 目的： 为了确定 EL2B 与 EL2F 之间的关系, 以便定量给出这个 EneCut 的大小
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


//_______________________________________________
// 目的: 根据上一步的测试结果, 已经确定了 EneCut 的大小
// 这一步是检查 EneCut 的最终效果
void Test_Multi::DetermineL2BL2FEnergyErrRatio()
{
  gStyle->SetOptStat(1);
  gStyle->SetPalette(1);

  std::string pathL2BL2FEnergyCut("figures/figure_HitPatternRecognition/SSD_L2BL2F_EnergyCut_Run120-Run130.pdf");

  TH2D* hist_L2B_L2F[4];
  for (Int_t i=0; i<4; i++) {
    hist_L2B_L2F[i] = new TH2D(Form("SSD%d_L2BL2F_EnergyCut",i+1),Form("SSD%d_L2BL2F_EnergyCut",i+1),3000,0.,300.,3000,0.,300.);
  }

  if (fChain == 0) return;

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    // for SSD1
    if (SSD1_fCsIMulti!=0 && SSD1_fL2BMulti!=0 && SSD1_fL2FMulti!=0) {
      if (fPattern.GeoConstraint_L3A_L2B(SSD1_fCsINum, SSD1_fL2BNumStrip, SSD1_fCsIMulti, SSD1_fL2BMulti)) {
        if (fPattern.GeoConstraint_L2B_L1S(SSD1_fL2BNumStrip, SSD1_fL1SNumStrip, SSD1_fL2BMulti, SSD1_fL1SMulti)) {
          if (fPattern.GeoConstraint_L3A_L2F(SSD1_fCsINum, SSD1_fL2FNumStrip, SSD1_fCsIMulti, SSD1_fL2FMulti)) {
            for (Int_t l2bch=0; l2bch<SSD1_fL2BMulti; l2bch++) {
              for (Int_t l2fch=0; l2fch<SSD1_fL2FMulti; l2fch++) {
                if (fPattern.EneConstraint_L2B_L2F(0, SSD1_fL2BEMeV[l2bch], SSD1_fL2FEMeV[l2fch])) {
                  hist_L2B_L2F[0]->Fill(SSD1_fL2BEMeV[l2bch], SSD1_fL2FEMeV[l2fch]);
                }
              }
            }
          }
        }
      }
    }
    // for SSD2
    if (SSD2_fCsIMulti!=0 && SSD2_fL2BMulti!=0 && SSD2_fL2FMulti!=0) {
      if (fPattern.GeoConstraint_L3A_L2B(SSD2_fCsINum, SSD2_fL2BNumStrip, SSD2_fCsIMulti, SSD2_fL2BMulti)) {
        if (fPattern.GeoConstraint_L2B_L1S(SSD2_fL2BNumStrip, SSD2_fL1SNumStrip, SSD2_fL2BMulti, SSD2_fL1SMulti)) {
          if (fPattern.GeoConstraint_L3A_L2F(SSD2_fCsINum, SSD2_fL2FNumStrip, SSD2_fCsIMulti, SSD2_fL2FMulti)) {
            for (Int_t l2bch=0; l2bch<SSD2_fL2BMulti; l2bch++) {
              for (Int_t l2fch=0; l2fch<SSD2_fL2FMulti; l2fch++) {
                if (fPattern.EneConstraint_L2B_L2F(1, SSD2_fL2BEMeV[l2bch], SSD2_fL2FEMeV[l2fch])) {
                  hist_L2B_L2F[1]->Fill(SSD2_fL2BEMeV[l2bch], SSD2_fL2FEMeV[l2fch]);
                }
              }
            }
          }
        }
      }
    }
    // for SSD3
    if (SSD3_fCsIMulti!=0 && SSD3_fL2BMulti!=0 && SSD3_fL2FMulti!=0) {
      if (fPattern.GeoConstraint_L3A_L2B(SSD3_fCsINum, SSD3_fL2BNumStrip, SSD3_fCsIMulti, SSD3_fL2BMulti)) {
        if (fPattern.GeoConstraint_L2B_L1S(SSD3_fL2BNumStrip, SSD3_fL1SNumStrip, SSD3_fL2BMulti, SSD3_fL1SMulti)) {
          if (fPattern.GeoConstraint_L3A_L2F(SSD3_fCsINum, SSD3_fL2FNumStrip, SSD3_fCsIMulti, SSD3_fL2FMulti)) {
            for (Int_t l2bch=0; l2bch<SSD3_fL2BMulti; l2bch++) {
              for (Int_t l2fch=0; l2fch<SSD3_fL2FMulti; l2fch++) {
                if (fPattern.EneConstraint_L2B_L2F(2, SSD3_fL2BEMeV[l2bch], SSD3_fL2FEMeV[l2fch])) {
                  hist_L2B_L2F[2]->Fill(SSD3_fL2BEMeV[l2bch], SSD3_fL2FEMeV[l2fch]);
                }
              }
            }
          }
        }
      }
    }
    // for SSD4
    if (SSD4_fCsIMulti!=0 && SSD4_fL2BMulti!=0 && SSD4_fL2FMulti!=0) {
      if (fPattern.GeoConstraint_L3A_L2B(SSD4_fCsINum, SSD4_fL2BNumStrip, SSD4_fCsIMulti, SSD4_fL2BMulti)) {
        if (fPattern.GeoConstraint_L2B_L1S(SSD4_fL2BNumStrip, SSD4_fL1SNumStrip, SSD4_fL2BMulti, SSD4_fL1SMulti)) {
          if (fPattern.GeoConstraint_L3A_L2F(SSD4_fCsINum, SSD4_fL2FNumStrip, SSD4_fCsIMulti, SSD4_fL2FMulti)) {
            for (Int_t l2bch=0; l2bch<SSD4_fL2BMulti; l2bch++) {
              for (Int_t l2fch=0; l2fch<SSD4_fL2FMulti; l2fch++) {
                if (fPattern.EneConstraint_L2B_L2F(3, SSD4_fL2BEMeV[l2bch], SSD4_fL2FEMeV[l2fch])) {
                  hist_L2B_L2F[3]->Fill(SSD4_fL2BEMeV[l2bch], SSD4_fL2FEMeV[l2fch]);
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

  // SSD1
  cans->cd(1);
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.12);
  gPad->SetTopMargin(0.12);
  hist_L2B_L2F[0]->GetXaxis()->SetTitle("SSD1_EL2B (MeV)");
  hist_L2B_L2F[0]->GetXaxis()->CenterTitle(1);
  hist_L2B_L2F[0]->GetXaxis()->SetTitleSize(0.05);
  hist_L2B_L2F[0]->GetXaxis()->SetLabelSize(0.05);

  hist_L2B_L2F[0]->GetYaxis()->SetTitle("SSD1_EL2F (MeV)");
  hist_L2B_L2F[0]->GetYaxis()->CenterTitle(1);
  hist_L2B_L2F[0]->GetYaxis()->SetTitleSize(0.05);
  hist_L2B_L2F[0]->GetYaxis()->SetLabelSize(0.05);

  hist_L2B_L2F[0]->Draw("COL");
  func_L2B_eq_L2F->Draw("SAME");
  gPad->Update();
  TPaveStats* st1 = (TPaveStats*)hist_L2B_L2F[0]->GetListOfFunctions()->FindObject("stats");
  st1->SetX1NDC(0.55);
  st1->SetX2NDC(0.85);
  st1->SetY1NDC(0.15);
  st1->SetY2NDC(0.45);
  gPad->Modified();

  // SSD2
  cans->cd(2);
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.12);
  gPad->SetTopMargin(0.12);
  hist_L2B_L2F[1]->GetXaxis()->SetTitle("SSD2_EL2B (MeV)");
  hist_L2B_L2F[1]->GetXaxis()->CenterTitle(1);
  hist_L2B_L2F[1]->GetXaxis()->SetTitleSize(0.05);
  hist_L2B_L2F[1]->GetXaxis()->SetLabelSize(0.05);

  hist_L2B_L2F[1]->GetYaxis()->SetTitle("SSD2_EL2F (MeV)");
  hist_L2B_L2F[1]->GetYaxis()->CenterTitle(1);
  hist_L2B_L2F[1]->GetYaxis()->SetTitleSize(0.05);
  hist_L2B_L2F[1]->GetYaxis()->SetLabelSize(0.05);

  hist_L2B_L2F[1]->Draw("COL");
  func_L2B_eq_L2F->Draw("SAME");
  gPad->Update();
  TPaveStats* st2 = (TPaveStats*)hist_L2B_L2F[1]->GetListOfFunctions()->FindObject("stats");
  st2->SetX1NDC(0.55);
  st2->SetX2NDC(0.85);
  st2->SetY1NDC(0.15);
  st2->SetY2NDC(0.45);
  gPad->Modified();

  // SSD3
  cans->cd(3);
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.12);
  gPad->SetTopMargin(0.12);
  hist_L2B_L2F[2]->GetXaxis()->SetTitle("SSD3_EL2B (MeV)");
  hist_L2B_L2F[2]->GetXaxis()->CenterTitle(1);
  hist_L2B_L2F[2]->GetXaxis()->SetTitleSize(0.05);
  hist_L2B_L2F[2]->GetXaxis()->SetLabelSize(0.05);

  hist_L2B_L2F[2]->GetYaxis()->SetTitle("SSD3_EL2F (MeV)");
  hist_L2B_L2F[2]->GetYaxis()->CenterTitle(1);
  hist_L2B_L2F[2]->GetYaxis()->SetTitleSize(0.05);
  hist_L2B_L2F[2]->GetYaxis()->SetLabelSize(0.05);

  hist_L2B_L2F[2]->Draw("COL");
  func_L2B_eq_L2F->Draw("SAME");
  gPad->Update();
  TPaveStats* st3 = (TPaveStats*)hist_L2B_L2F[2]->GetListOfFunctions()->FindObject("stats");
  st3->SetX1NDC(0.55);
  st3->SetX2NDC(0.85);
  st3->SetY1NDC(0.15);
  st3->SetY2NDC(0.45);
  gPad->Modified();

  // SSD2
  cans->cd(4);
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.12);
  gPad->SetTopMargin(0.12);
  hist_L2B_L2F[3]->GetXaxis()->SetTitle("SSD4_EL2B (MeV)");
  hist_L2B_L2F[3]->GetXaxis()->CenterTitle(1);
  hist_L2B_L2F[3]->GetXaxis()->SetTitleSize(0.05);
  hist_L2B_L2F[3]->GetXaxis()->SetLabelSize(0.05);

  hist_L2B_L2F[3]->GetYaxis()->SetTitle("SSD4_EL2F (MeV)");
  hist_L2B_L2F[3]->GetYaxis()->CenterTitle(1);
  hist_L2B_L2F[3]->GetYaxis()->SetTitleSize(0.05);
  hist_L2B_L2F[3]->GetYaxis()->SetLabelSize(0.05);

  hist_L2B_L2F[3]->Draw("COL");
  func_L2B_eq_L2F->Draw("SAME");
  gPad->Update();
  TPaveStats* st4 = (TPaveStats*)hist_L2B_L2F[3]->GetListOfFunctions()->FindObject("stats");
  st4->SetX1NDC(0.55);
  st4->SetX2NDC(0.85);
  st4->SetY1NDC(0.15);
  st4->SetY2NDC(0.45);
  gPad->Modified();
  cans->Print(pathL2BL2FEnergyCut.c_str());
}


//______________________________________________________________________________
// 计算不同粒子在每套硅望远镜两层硅中的能损
// 目的: 为了确定粒子穿透 L2 到达 L3 时, EL1 与 EL2 的比值
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

//________________________________________________
// 目的: 为了确定粒子穿透 L2 到达 L3 时, EL1 与 EL2 的比值
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
//______________________________________________________________________________


//______________________________________________________________________________
// 检查每一层 hit=1, 2, 3,... 所占的比例
//目的是在使用算法进行模式识别前，先对每一层的 multi 作出一个大致的 cut
void Test_Multi::CheckLayerMultiPercentage()
{
  std::string pathDataOut("data/data_HitPatternRecognition/CheckLayerMultiPercentage_Run120-130.dat");
  ofstream fileout(pathDataOut.c_str());
  fileout<<setw(5)<<"SSD_Layer"<<setw(14)<<"Multi=0"<<setw(14)<<"Multi=1"
         <<setw(15)<<"Multi=2"<<setw(15)<<"Multi=3"<<setw(15)<<"Multi=4"
         <<setw(15)<<"Multi=5"<<setw(15)<<"Multi=6"<<setw(12)<<"Sum"<<endl;
  fileout<<""<<endl;

  Int_t Num_SSD   = 4;
  Int_t Num_Multi = 7; // hit = 0, 1, 2, 3, 4, 5, 6

  Int_t Entries_L1S[Num_SSD][Num_Multi];
  Int_t Entries_L2F[Num_SSD][Num_Multi];
  Int_t Entries_L2B[Num_SSD][Num_Multi];
  Int_t Entries_L3A[Num_SSD][Num_Multi];
  Double_t MultiRatio_L1S[Num_SSD][Num_Multi];
  Double_t MultiRatio_L2F[Num_SSD][Num_Multi];
  Double_t MultiRatio_L2B[Num_SSD][Num_Multi];
  Double_t MultiRatio_L3A[Num_SSD][Num_Multi];
  Double_t Sum_L1S[Num_SSD];
  Double_t Sum_L2F[Num_SSD];
  Double_t Sum_L2B[Num_SSD];
  Double_t Sum_L3A[Num_SSD];

  for (Int_t i=0; i<Num_SSD; i++) {
    for (Int_t j=0; j<Num_Multi; j++) {
      Entries_L1S[i][j] = 0;
      Entries_L2F[i][j] = 0;
      Entries_L2B[i][j] = 0;
      Entries_L3A[i][j] = 0;

      MultiRatio_L1S[i][j] = 0.;
      MultiRatio_L2F[i][j] = 0.;
      MultiRatio_L2B[i][j] = 0.;
      MultiRatio_L3A[i][j] = 0.;
    }
    Sum_L1S[i] = 0.;
    Sum_L2F[i] = 0.;
    Sum_L2B[i] = 0.;
    Sum_L3A[i] = 0.;
  }

  if (fChain == 0) return;
  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    // if (Cut(ientry) < 0) continue;

    for (Int_t i=0; i<Num_Multi; i++) {
      if (SSD1_fL1SMulti==i)  Entries_L1S[0][i]++;
      if (SSD2_fL1SMulti==i)  Entries_L1S[1][i]++;
      if (SSD3_fL1SMulti==i)  Entries_L1S[2][i]++;
      if (SSD4_fL1SMulti==i)  Entries_L1S[3][i]++;

      if (SSD1_fL2FMulti==i)  Entries_L2F[0][i]++;
      if (SSD2_fL2FMulti==i)  Entries_L2F[1][i]++;
      if (SSD3_fL2FMulti==i)  Entries_L2F[2][i]++;
      if (SSD4_fL2FMulti==i)  Entries_L2F[3][i]++;

      if (SSD1_fL2BMulti==i)  Entries_L2B[0][i]++;
      if (SSD2_fL2BMulti==i)  Entries_L2B[1][i]++;
      if (SSD3_fL2BMulti==i)  Entries_L2B[2][i]++;
      if (SSD4_fL2BMulti==i)  Entries_L2B[3][i]++;

      if (SSD1_fCsIMulti==i)  Entries_L3A[0][i]++;
      if (SSD2_fCsIMulti==i)  Entries_L3A[1][i]++;
      if (SSD3_fCsIMulti==i)  Entries_L3A[2][i]++;
      if (SSD4_fCsIMulti==i)  Entries_L3A[3][i]++;
    }
  }
  // 计算各种情况的比例
  for (Int_t i=0; i<Num_SSD; i++) {
    for (Int_t j=0; j<Num_Multi; j++) {
      MultiRatio_L1S[i][j] = (Double_t) Entries_L1S[i][j]/nentries;
      Sum_L1S[i] += MultiRatio_L1S[i][j];

      MultiRatio_L2F[i][j] = (Double_t) Entries_L2F[i][j]/nentries;
      Sum_L2F[i] += MultiRatio_L2F[i][j];

      MultiRatio_L2B[i][j] = (Double_t) Entries_L2B[i][j]/nentries;
      Sum_L2B[i] += MultiRatio_L2B[i][j];

      MultiRatio_L3A[i][j] = (Double_t) Entries_L3A[i][j]/nentries;
      Sum_L3A[i] += MultiRatio_L3A[i][j];
    }
  }
  // 输入结果
  cout<<setw(5)<<"SSD_Layer"<<setw(14)<<"Multi=0"<<setw(14)<<"Multi=1"
      <<setw(15)<<"Multi=2"<<setw(15)<<"Multi=3"<<setw(15)<<"Multi=4"
      <<setw(15)<<"Multi=5"<<setw(15)<<"Multi=6"<<setw(12)<<"Sum"<<endl;
  cout<<endl;
  for (Int_t i=0; i<Num_SSD; i++) {
    // L1S
    cout<<setw(5)<<Form("SSD%d_L1S",i+1);
    fileout<<setw(5)<<Form("SSD%d_L1S",i+1);
    for (Int_t j=0; j<Num_Multi; j++) {
      cout<<setw(15)<<MultiRatio_L1S[i][j]<<setw(15);
      fileout<<setw(15)<<MultiRatio_L1S[i][j]<<setw(15);
    }
    cout<<Sum_L1S[i]<<endl;
    fileout<<Sum_L1S[i]<<endl;

    // L2F
    cout<<setw(5)<<Form("SSD%d_L2F",i+1);
    fileout<<setw(5)<<Form("SSD%d_L2F",i+1);
    for (Int_t j=0; j<Num_Multi; j++) {
      cout<<setw(15)<<MultiRatio_L2F[i][j]<<setw(15);
      fileout<<setw(15)<<MultiRatio_L2F[i][j]<<setw(15);
    }
    cout<<Sum_L2F[i]<<endl;
    fileout<<Sum_L2F[i]<<endl;

    // L2B
    cout<<setw(5)<<Form("SSD%d_L2B",i+1);
    fileout<<setw(5)<<Form("SSD%d_L2B",i+1);
    for (Int_t j=0; j<Num_Multi; j++) {
      cout<<setw(15)<<MultiRatio_L2B[i][j]<<setw(15);
      fileout<<setw(15)<<MultiRatio_L2B[i][j]<<setw(15);
    }
    cout<<Sum_L2B[i]<<endl;
    fileout<<Sum_L2B[i]<<endl;

    // L3A
    cout<<setw(5)<<Form("SSD%d_L3A",i+1);
    fileout<<setw(5)<<Form("SSD%d_L3A",i+1);
    for (Int_t j=0; j<Num_Multi; j++) {
      cout<<setw(15)<<MultiRatio_L3A[i][j]<<setw(15);
      fileout<<setw(15)<<MultiRatio_L3A[i][j]<<setw(15);
    }
    cout<<Sum_L3A[i]<<endl;
    cout<<endl;
    fileout<<Sum_L3A[i]<<endl;
    fileout<<""<<endl;
  }
  fileout.close();
}


//______________________________________________________________________________
// 分析不同 fGlobalMulti 下的 fGlobalMulti 的比例
// 这一步的分析依赖于 TrackFindingAlgorithm2() 生成的 .root 文件
void Test_Multi::CheckGlobalMultiRatio()
{
  // 定义变量， 以用于计算不同 fGlobalMulti 的比例
  Int_t    nummulti          = 7;
  Int_t    globalmulti[7]    = {0};
  Double_t multiratio_0[7]   = {0.};
  Double_t sumratio_0        = 0.;
  Double_t multiratio_1[7]   = {0.};
  Double_t sumratio_1        = 0.;
  Long64_t count = 0;

  TFile* myfile = new TFile("/home/sea/Fission2019_Data/TrackFindingAlgorithm2.root","READONLY");
  if (!myfile->IsOpen()) {
    cout<<"Open file failded."<<endl;
    return;
  }

  Int_t SSD1_GlobalMulti;
  std::vector<Int_t>    SSD1_L1SNumStrip;
  std::vector<Double_t> SSD1_L1SEMeV;
  std::vector<Int_t>    SSD1_L2FNumStrip;
  std::vector<Double_t> SSD1_L2FEMeV;
  std::vector<Int_t>    SSD1_L2BNumStrip;
  std::vector<Double_t> SSD1_L2BEMeV;
  std::vector<Int_t>    SSD1_CsINum;
  std::vector<Int_t>    SSD1_CsIECh;

  TTree* mytree = (TTree*)myfile->Get("TrackEvent");
  Long64_t nentries = mytree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  mytree->SetMakeClass(1);  // 如果 tree 的 branch 使用了自定义的类, 则这条语句不能省略！！！
  mytree->SetBranchAddress("SSD1.GlobalMulti",  &SSD1_GlobalMulti);
  mytree->SetBranchAddress("SSD1.L1SNumStrip",  &SSD1_L1SNumStrip);
  mytree->SetBranchAddress("SSD1.L1SEMeV",      &SSD1_L1SEMeV);
  mytree->SetBranchAddress("SSD1.L2FNumStrip",  &SSD1_L2FNumStrip);
  mytree->SetBranchAddress("SSD1.L2FEMeV",      &SSD1_L2FEMeV);
  mytree->SetBranchAddress("SSD1.L2BNumStrip",  &SSD1_L2BNumStrip);
  mytree->SetBranchAddress("SSD1.L2BEMeV",      &SSD1_L2BEMeV);
  mytree->SetBranchAddress("SSD1.CsINum",       &SSD1_CsINum);
  mytree->SetBranchAddress("SSD1.CsIECh",       &SSD1_CsIECh);

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    mytree->GetEntry(ientry);
    //___________________________________________
    // 统计不同 fGlobalMult 的计数
    for (Int_t i=0; i<nummulti; i++) {
      if (SSD1_GlobalMulti==i)   globalmulti[i]++;
    }
  }

  // 计算比例，从 globalmulti = 0 算起
  for (Int_t i=0; i<nummulti; i++) {
    multiratio_0[i] = (Double_t) globalmulti[i]/nentries;
    sumratio_0 += multiratio_0[i];
    cout<<multiratio_0[i]<<setw(12);
  }
  cout<<sumratio_0<<endl;

  // 计算比例，从 globalmulti = 1 算起
  for (Int_t i=1; i<nummulti; i++) {
    multiratio_1[i] = (Double_t) globalmulti[i]/(nentries-globalmulti[0]); // 剔除 fGlobalMulti=0 的情况
    sumratio_1 += multiratio_1[i];
    cout<<multiratio_1[i]<<setw(12);
    count += globalmulti[i];
  }
  cout<<sumratio_1<<endl;
  cout<<"count = "<<count<<endl;

  myfile->Close();
}


//______________________________________________________________________________
// 使用平行坐标显示每一层数据的关联
void Test_Multi::CheckParallelDraw(const char* fglobalmulti)
{
  gROOT->cd();

  TNtuple* nt = new TNtuple("nt", "NTUPLE", "fg:CsINum:L2BNum:L2FNum:L1SNum:EL1S:EL2F:EL2B:EL3A");
  nt->ReadFile(Form("data/Test_Multi_%s.dat", fglobalmulti));

  TCanvas* cans1 = new TCanvas("cans1","cans1",800,600);
  cans1->cd();
  nt->Draw("L1SNum:L2FNum:L2BNum:CsINum","","PARA");
  cans1->Print(Form("figures/figure_HitPatternRecognition/%s_SSD1_Para_L1S_L2F_L2B_L3A.png",fglobalmulti));

  TCanvas* cans2 = new TCanvas("cans2","cans2",800,600);
  cans2->cd();
  nt->Draw("L1SNum:L2BNum:CsINum","","PARA");
  cans2->Print(Form("figures/figure_HitPatternRecognition/%s_SSD1_Para_L1S_L2B_L3A.png",fglobalmulti));

  TCanvas* cans3 = new TCanvas("cans3","cans3",800,600);
  cans3->cd();
  nt->Draw("L1SNum:L2FNum:CsINum","","PARA");
  cans3->Print(Form("figures/figure_HitPatternRecognition/%s_SSD1_Para_L1S_L2F_L3A.png",fglobalmulti));

  TCanvas* cans4 = new TCanvas("cans4","cans4",800,600);
  cans4->cd();
  nt->Draw("EL1S:EL2F:EL2B:EL3A","","PARA");
  cans4->Print(Form("figures/figure_HitPatternRecognition/%s_SSD1_Para_EL1S_EL2F_EL2B_L3A.png",fglobalmulti));
}
//******************************************************************************




//******************************************************************************
// Test_Multi 的子类 DecodefGlobalMulti1
// 解码 fGlobalMulit = 1的事件, 即检查是否有异常事件

DecodefGlobalMulti1::DecodefGlobalMulti1()
{}

DecodefGlobalMulti1::~DecodefGlobalMulti1()
{}

//______________________________________________
void DecodefGlobalMulti1::Check_fGlobalMulti1()
{
  gStyle->SetOptStat(kFALSE);

  // 读取 .root 文件
  TFile* myfile = new TFile("/home/sea/Fission2019_Data/TrackFindingAlgorithm2.root","READONLY");
  if (!myfile->IsOpen()) {
    cout<<"Open file failded."<<endl;
    return;
  }

  ofstream FileOut("data/Test_Multi_fGlobalMulti1.dat");
  FileOut<<setw(10)<<"# MultiGlobal"<<setw(10)<<"CSI"<<setw(10)<<"L2B"
         <<setw(10)<<"L2F"<<setw(10)<<"L1S"<<setw(10)<<"EL1S"
         <<setw(10)<<"EL2F"<<setw(10)<<"EL2B"<<setw(10)<<"ECsI"
         <<endl;

  // 定义一维直方图, 画出每一层的能谱
  TH1D* hist_EL1S = new TH1D("hist_EL1S","hist_EL1S",150,0.,150.);
  TH1D* hist_EL2F = new TH1D("hist_EL2F","hist_EL2F",300,0.,300.);
  TH1D* hist_EL2B = new TH1D("hist_EL2B","hist_EL2B",300,0.,300);
  TH1D* hist_EL3A = new TH1D("hist_EL3A","hist_EL3A",400,0.,4000);

  Int_t SSD1_GlobalMulti;
  std::vector<Int_t>    SSD1_L1SNumStrip;
  std::vector<Double_t> SSD1_L1SEMeV;
  std::vector<Int_t>    SSD1_L2FNumStrip;
  std::vector<Double_t> SSD1_L2FEMeV;
  std::vector<Int_t>    SSD1_L2BNumStrip;
  std::vector<Double_t> SSD1_L2BEMeV;
  std::vector<Int_t>    SSD1_CsINum;
  std::vector<Int_t>    SSD1_CsIECh;

  TTree* mytree = (TTree*)myfile->Get("TrackEvent");
  Long64_t nentries = mytree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  mytree->SetMakeClass(1);  // 如果 tree 的 branch 使用了自定义的类, 则这条语句不能省略！！！
  mytree->SetBranchAddress("SSD1.GlobalMulti",  &SSD1_GlobalMulti);
  mytree->SetBranchAddress("SSD1.L1SNumStrip",  &SSD1_L1SNumStrip);
  mytree->SetBranchAddress("SSD1.L1SEMeV",      &SSD1_L1SEMeV);
  mytree->SetBranchAddress("SSD1.L2FNumStrip",  &SSD1_L2FNumStrip);
  mytree->SetBranchAddress("SSD1.L2FEMeV",      &SSD1_L2FEMeV);
  mytree->SetBranchAddress("SSD1.L2BNumStrip",  &SSD1_L2BNumStrip);
  mytree->SetBranchAddress("SSD1.L2BEMeV",      &SSD1_L2BEMeV);
  mytree->SetBranchAddress("SSD1.CsINum",       &SSD1_CsINum);
  mytree->SetBranchAddress("SSD1.CsIECh",       &SSD1_CsIECh);

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    mytree->GetEntry(ientry);
    //______________________________
    // 打印 fGlobleMulti = 1 的事件
    if (SSD1_GlobalMulti != 1) continue;
    for (Int_t i=0; i<SSD1_GlobalMulti; i++) {
      FileOut<<setw(7)<<i+1
             <<setw(12)<<SSD1_CsINum[i]
             <<setw(10)<<SSD1_L2BNumStrip[i]
             <<setw(10)<<SSD1_L2FNumStrip[i]
             <<setw(10)<<SSD1_L1SNumStrip[i]
             <<setw(13)<<SSD1_L1SEMeV[i]
             <<setw(10)<<SSD1_L2FEMeV[i]
             <<setw(10)<<SSD1_L2BEMeV[i]
             <<setw(10)<<SSD1_CsIECh[i]
             <<endl;

      hist_EL1S->Fill(SSD1_L1SEMeV[i]);
      hist_EL2F->Fill(SSD1_L2FEMeV[i]);
      hist_EL2B->Fill(SSD1_L2BEMeV[i]);
      hist_EL3A->Fill(SSD1_CsIECh[i]);
    }
  }

  // 画图
  TCanvas* cans = new TCanvas("cans","cans",1200,1000);
  cans->Divide(2,2);

  cans->cd(1);
  gPad->SetLeftMargin(0.12);
  gPad->SetBottomMargin(0.12);
  hist_EL1S->Draw();
  hist_EL1S->SetTitle("");
  hist_EL1S->SetLineColor(kRed);
  hist_EL1S->SetLineWidth(2);
  hist_EL1S->GetXaxis()->SetTitle("SSD1_EL1S");
  hist_EL1S->GetXaxis()->SetTitleSize(0.06);
  hist_EL1S->GetXaxis()->CenterTitle(kTRUE);
  hist_EL1S->GetXaxis()->SetLabelSize(0.06);
  hist_EL1S->GetXaxis()->SetNdivisions(505);
  hist_EL1S->GetYaxis()->SetLabelSize(0.06);
  hist_EL1S->GetYaxis()->SetNdivisions(505);

  cans->cd(2);
  gPad->SetLeftMargin(0.12);
  gPad->SetBottomMargin(0.12);
  hist_EL2F->Draw();
  hist_EL2F->SetTitle("");
  hist_EL2F->SetLineColor(kRed);
  hist_EL2F->SetLineWidth(2);
  hist_EL2F->GetXaxis()->SetTitle("SSD1_EL2F");
  hist_EL2F->GetXaxis()->SetTitleSize(0.06);
  hist_EL2F->GetXaxis()->CenterTitle(kTRUE);
  hist_EL2F->GetXaxis()->SetLabelSize(0.06);
  hist_EL2F->GetXaxis()->SetNdivisions(505);
  hist_EL2F->GetYaxis()->SetLabelSize(0.06);
  hist_EL2F->GetYaxis()->SetNdivisions(505);

  cans->cd(3);
  gPad->SetLeftMargin(0.12);
  gPad->SetBottomMargin(0.12);
  hist_EL2B->Draw();
  hist_EL2B->SetTitle("");
  hist_EL2B->SetLineColor(kRed);
  hist_EL2B->SetLineWidth(2);
  hist_EL2B->GetXaxis()->SetTitle("SSD1_EL2B");
  hist_EL2B->GetXaxis()->SetTitleSize(0.06);
  hist_EL2B->GetXaxis()->CenterTitle(kTRUE);
  hist_EL2B->GetXaxis()->SetLabelSize(0.06);
  hist_EL2B->GetXaxis()->SetNdivisions(505);
  hist_EL2B->GetYaxis()->SetLabelSize(0.06);
  hist_EL2B->GetYaxis()->SetNdivisions(505);

  cans->cd(4);
  gPad->SetLeftMargin(0.12);
  gPad->SetBottomMargin(0.12);
  hist_EL3A->Draw();
  hist_EL3A->SetTitle("");
  hist_EL3A->SetLineColor(kRed);
  hist_EL3A->SetLineWidth(2);
  hist_EL3A->GetXaxis()->SetTitle("SSD1_EL3A");
  hist_EL3A->GetXaxis()->SetTitleSize(0.06);
  hist_EL3A->GetXaxis()->CenterTitle(kTRUE);
  hist_EL3A->GetXaxis()->SetLabelSize(0.06);
  hist_EL3A->GetXaxis()->SetNdivisions(505);
  hist_EL3A->GetYaxis()->SetLabelSize(0.06);
  hist_EL3A->GetYaxis()->SetNdivisions(505);

  cans->Print("figures/figure_HitPatternRecognition/fGlobalMulti1_SSD1_EnergySpectrum.png");
}

//_______________________________________________
void DecodefGlobalMulti1::ParaDraw_fGlobalMulti1()
{
  CheckParallelDraw("fGlobalMulti1");
}

//******************************************************************************



//******************************************************************************
// Test_Multi 的子类 DecodefGlobalMulti2
// 解码 fGlobalMulit = 2的事件

DecodefGlobalMulti2::DecodefGlobalMulti2()
{}

DecodefGlobalMulti2::~DecodefGlobalMulti2()
{}


//______________________________________________
void DecodefGlobalMulti2::Check_fGlobalMulti2()
{
  gStyle->SetOptStat(kFALSE);

  // 读取 .root 文件
  TFile* myfile = new TFile("/home/sea/Fission2019_Data/TrackFindingAlgorithm2.root","READONLY");
  if (!myfile->IsOpen()) {
    cout<<"Open file failded."<<endl;
    return;
  }

  ofstream FileOut("data/Test_Multi_fGlobalMulti2.dat");
  FileOut<<setw(10)<<"# MultiGlobal"<<setw(10)<<"CSI"<<setw(10)<<"L2B"
         <<setw(10)<<"L2F"<<setw(10)<<"L1S"<<setw(10)<<"EL1S"
         <<setw(10)<<"EL2F"<<setw(10)<<"EL2B"<<setw(10)<<"ECsI"
         <<endl;

  Int_t SSD1_GlobalMulti;
  std::vector<Int_t>    SSD1_L1SNumStrip;
  std::vector<Double_t> SSD1_L1SEMeV;
  std::vector<Int_t>    SSD1_L2FNumStrip;
  std::vector<Double_t> SSD1_L2FEMeV;
  std::vector<Int_t>    SSD1_L2BNumStrip;
  std::vector<Double_t> SSD1_L2BEMeV;
  std::vector<Int_t>    SSD1_CsINum;
  std::vector<Int_t>    SSD1_CsIECh;

  TTree* mytree = (TTree*)myfile->Get("TrackEvent");
  Long64_t nentries = mytree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  mytree->SetMakeClass(1);  // 如果 tree 的 branch 使用了自定义的类, 则这条语句不能省略！！！
  mytree->SetBranchAddress("SSD1.GlobalMulti",  &SSD1_GlobalMulti);
  mytree->SetBranchAddress("SSD1.L1SNumStrip",  &SSD1_L1SNumStrip);
  mytree->SetBranchAddress("SSD1.L1SEMeV",      &SSD1_L1SEMeV);
  mytree->SetBranchAddress("SSD1.L2FNumStrip",  &SSD1_L2FNumStrip);
  mytree->SetBranchAddress("SSD1.L2FEMeV",      &SSD1_L2FEMeV);
  mytree->SetBranchAddress("SSD1.L2BNumStrip",  &SSD1_L2BNumStrip);
  mytree->SetBranchAddress("SSD1.L2BEMeV",      &SSD1_L2BEMeV);
  mytree->SetBranchAddress("SSD1.CsINum",       &SSD1_CsINum);
  mytree->SetBranchAddress("SSD1.CsIECh",       &SSD1_CsIECh);

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    mytree->GetEntry(ientry);
    //______________________________
    // 打印 fGlobleMulti = 2 的事件
    if (SSD1_GlobalMulti != 2) continue;
    for (Int_t i=0; i<SSD1_GlobalMulti; i++) {
      FileOut<<setw(7)<<i+1
             <<setw(12)<<SSD1_CsINum[i]
             <<setw(10)<<SSD1_L2BNumStrip[i]
             <<setw(10)<<SSD1_L2FNumStrip[i]
             <<setw(10)<<SSD1_L1SNumStrip[i]
             <<setw(13)<<SSD1_L1SEMeV[i]
             <<setw(10)<<SSD1_L2FEMeV[i]
             <<setw(10)<<SSD1_L2BEMeV[i]
             <<setw(10)<<SSD1_CsIECh[i]
             <<endl;
    }
  }
}


//_______________________________________________
void DecodefGlobalMulti2::ParaDraw_fGlobalMulti2()
{
  CheckParallelDraw("fGlobalMulti2");
}
