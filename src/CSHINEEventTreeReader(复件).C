////////////////////////////////////////////////////////////////////////////////
//
//  检查 dE-E 谱的质量, 确定采取怎样的方式填充 dE-E 并最终用于 DEFFIT 进行拟合
//  需要考虑以下几个问题:
//  1.Layer2 与 Layer3 的 dE-E
//    (1) L2F:L3A or L2B:L3A     // 选取 L2F, 还是 L2B 与 CsI 匹配？
//    (2) L2 : L3A[csi]          // 选取全部strip 与 CsI 匹配
//
//  2.Layer1 与 Layer2
//    (1) L1S:L2F or L1S:L2B    // 选取 L2F, 还是 L2B 与 CsI 匹配？
//    (2) L1S:L2[strip]         // 选取 L1 全部strip 与 L2 匹配
//
//  3. L1S 与 L2B 平行, L1S_CH00 <-> L2B_CH00
//
//
//  ** gfh, 2020-07-30
//  ** 添加 CSHINEEventTreeReader::DEE_L2F_L3
//  ** 添加 CSHINEEventTreeReader::DEE_L2B_L3
//  ** 添加 CSHINEEventTreeReader::DEE_L1_L2F
//  ** 添加 CSHINEEventTreeReader::DEE_L1_L2B
//
////////////////////////////////////////////////////////////////////////////////

#define CSHINEEventTreeReader_cxx
#include "../include/CSHINEEventTreeReader.h"
#include "../include/TimeAndPercentage.h"
#include "../include/shared.h"
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>


//______________________________________________________________________________
void CSHINEEventTreeReader::DEE_L1L2_L2L3(const char* pathOutput)
{
  if (fChain == 0 ) return;

  TimeAndPercentage time;
  time.GetBeginTime();





  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"*** Found  "<<nentries<<" entries. ***"<<endl;

  for (Long64_t jentry=0; jentry<nentries; jentry++)
  {
    time.PrintPercentageAndRemainingTime(jentry, nentries);

    fChain->GetEntry(jentry);
  }
}



//______________________________________________________________________________
void CSHINEEventTreeReader::DEE_L2F_L3(const char* pathOutput)
{
  if (fChain == 0 ) return;

  TimeAndPercentage time;
  time.GetBeginTime();

  std::string pathOutBegin(Form("%s[", pathOutput));
  std::string pathOutEnd  (Form("%s]", pathOutput));

  Double_t RangeEMeVL2F[4] = {300., 250., 150., 80.};
  Int_t    NBinsEMeVL2F[4] = {3000, 2500, 1500, 800};

  TH2D* h_L2F_L3[NUM_SSD][NUM_CSI];
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_CSI; j++) {
      h_L2F_L3[i][j] = new TH2D(Form("h_L2F_L3_SSD%d_CsI%d",i+1,j), Form("h_L2F_L3_SSD%d_CsI%d",i+1,j),
                                4096, 0, 4096, NBinsEMeVL2F[i], 0., RangeEMeVL2F[i]);
    }
  }

  fChain->SetBranchStatus("*", false);
  fChain->SetBranchStatus("SSD1.fMultiL2F",    true);
  fChain->SetBranchStatus("SSD1.fNumStripL2F", true);
  fChain->SetBranchStatus("SSD1.fEMeVL2F",     true);
  fChain->SetBranchStatus("SSD1.fMultiCsI",    true);
  fChain->SetBranchStatus("SSD1.fNumCsI",      true);
  fChain->SetBranchStatus("SSD1.fEChCsI",      true);

  fChain->SetBranchStatus("SSD2.fMultiL2F",    true);
  fChain->SetBranchStatus("SSD2.fNumStripL2F", true);
  fChain->SetBranchStatus("SSD2.fEMeVL2F",     true);
  fChain->SetBranchStatus("SSD2.fMultiCsI",    true);
  fChain->SetBranchStatus("SSD2.fNumCsI",      true);
  fChain->SetBranchStatus("SSD2.fEChCsI",      true);

  fChain->SetBranchStatus("SSD3.fMultiL2F",    true);
  fChain->SetBranchStatus("SSD3.fNumStripL2F", true);
  fChain->SetBranchStatus("SSD3.fEMeVL2F",     true);
  fChain->SetBranchStatus("SSD3.fMultiCsI",    true);
  fChain->SetBranchStatus("SSD3.fNumCsI",      true);
  fChain->SetBranchStatus("SSD3.fEChCsI",      true);

  fChain->SetBranchStatus("SSD4.fMultiL2F",    true);
  fChain->SetBranchStatus("SSD4.fNumStripL2F", true);
  fChain->SetBranchStatus("SSD4.fEMeVL2F",     true);
  fChain->SetBranchStatus("SSD4.fMultiCsI",    true);
  fChain->SetBranchStatus("SSD4.fNumCsI",      true);
  fChain->SetBranchStatus("SSD4.fEChCsI",      true);

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"*** Found  "<<nentries<<" entries. ***"<<endl;

  for (Long64_t jentry=0; jentry<nentries; jentry++)
  {
    time.PrintPercentageAndRemainingTime(jentry, nentries);

    fChain->GetEntry(jentry);

    //**************************************************************************
    //                             从这里开始进行分析
    //__________________________________________________________________________
    //           for SSD1
    if (SSD1_fMultiL2F == 1 && SSD1_fMultiCsI == 1) {  //挑选 one-hit 事件来刻度
      h_L2F_L3[0][SSD1_fNumCsI[0]]->Fill(SSD1_fEChCsI[0], SSD1_fEMeVL2F[0]);
    }
    //__________________________________
    //          for SSD2
    if (SSD2_fMultiL2F == 1 && SSD2_fMultiCsI == 1) { //挑选 one-hit 事件来刻度
      h_L2F_L3[1][SSD2_fNumCsI[0]]->Fill(SSD2_fEChCsI[0], SSD2_fEMeVL2F[0]);
    }
    //__________________________________
    //          for SSD3
    if (SSD3_fMultiL2F == 1 && SSD3_fMultiCsI == 1) { //挑选 one-hit 事件来刻度
      h_L2F_L3[2][SSD3_fNumCsI[0]]->Fill(SSD3_fEChCsI[0], SSD3_fEMeVL2F[0]);
    }
    //__________________________________
    //          for SSD4
    if (SSD4_fMultiL2F == 1 && SSD4_fMultiCsI == 1) { //挑选 one-hit 事件来刻度
      h_L2F_L3[3][SSD4_fNumCsI[0]]->Fill(SSD4_fEChCsI[0], SSD4_fEMeVL2F[0]);
    }
  }

  TCanvas* cans    = new TCanvas("cans","cans",1000,800);

  TCanvas* c_begin = new TCanvas();
  c_begin->Close();
  TCanvas* c_end   = new TCanvas();
  c_end->Close();
  c_begin->Print(pathOutBegin.c_str());

  for (Int_t i=0; i<NUM_SSD; i++)
  {
    for (Int_t j=0; j<NUM_CSI; j++)
    {
      cans->cd();
      gPad->SetLeftMargin(0.12);
      gPad->SetBottomMargin(0.12);
      h_L2F_L3[i][j]->GetXaxis()->SetTitle("ECh_CsI");
      h_L2F_L3[i][j]->GetXaxis()->SetTitleSize(0.05);
      h_L2F_L3[i][j]->GetXaxis()->CenterTitle(1);
      h_L2F_L3[i][j]->GetYaxis()->SetTitle("EMeV_L2F");
      h_L2F_L3[i][j]->GetYaxis()->SetTitleSize(0.05);
      h_L2F_L3[i][j]->GetYaxis()->CenterTitle(1);
      h_L2F_L3[i][j]->GetYaxis()->SetTitleOffset(0.9);

      h_L2F_L3[i][j]->Draw("COLZ");
      gPad->Modified();
      gPad->Update();

      cans->Print(pathOutput);
    }
  }
  c_end->Print(pathOutEnd.c_str());

  time.GetEndTime();
  time.GetRunTime();
}


//______________________________________________________________________________
void CSHINEEventTreeReader::DEE_L2B_L3(const char* pathOutput)
{
  if (fChain == 0 ) return;

  TimeAndPercentage time;
  time.GetBeginTime();

  std::string pathOutBegin(Form("%s[", pathOutput));
  std::string pathOutEnd  (Form("%s]", pathOutput));

  Double_t RangeEMeVL2B[4] = {300., 250., 150., 80.};
  Int_t    NBinsEMeVL2B[4] = {3000, 2500, 1500, 800};

  TH2D* h_L2B_L3[NUM_SSD][NUM_CSI];
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_CSI; j++) {
      h_L2B_L3[i][j] = new TH2D(Form("h_L2B_L3_SSD%d_CsI%d",i+1,j), Form("h_L2B_L3_SSD%d_CsI%d",i+1,j),
                                4096, 0, 4096, NBinsEMeVL2B[i], 0., RangeEMeVL2B[i]);
    }
  }

  fChain->SetBranchStatus("*", false);
  fChain->SetBranchStatus("SSD1.fMultiL2B",    true);
  fChain->SetBranchStatus("SSD1.fNumStripL2B", true);
  fChain->SetBranchStatus("SSD1.fEMeVL2B",     true);
  fChain->SetBranchStatus("SSD1.fMultiCsI",    true);
  fChain->SetBranchStatus("SSD1.fNumCsI",      true);
  fChain->SetBranchStatus("SSD1.fEChCsI",      true);

  fChain->SetBranchStatus("SSD2.fMultiL2B",    true);
  fChain->SetBranchStatus("SSD2.fNumStripL2B", true);
  fChain->SetBranchStatus("SSD2.fEMeVL2B",     true);
  fChain->SetBranchStatus("SSD2.fMultiCsI",    true);
  fChain->SetBranchStatus("SSD2.fNumCsI",      true);
  fChain->SetBranchStatus("SSD2.fEChCsI",      true);

  fChain->SetBranchStatus("SSD3.fMultiL2B",    true);
  fChain->SetBranchStatus("SSD3.fNumStripL2B", true);
  fChain->SetBranchStatus("SSD3.fEMeVL2B",     true);
  fChain->SetBranchStatus("SSD3.fMultiCsI",    true);
  fChain->SetBranchStatus("SSD3.fNumCsI",      true);
  fChain->SetBranchStatus("SSD3.fEChCsI",      true);

  fChain->SetBranchStatus("SSD4.fMultiL2B",    true);
  fChain->SetBranchStatus("SSD4.fNumStripL2B", true);
  fChain->SetBranchStatus("SSD4.fEMeVL2B",     true);
  fChain->SetBranchStatus("SSD4.fMultiCsI",    true);
  fChain->SetBranchStatus("SSD4.fNumCsI",      true);
  fChain->SetBranchStatus("SSD4.fEChCsI",      true);

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"*** Found  "<<nentries<<" entries. ***"<<endl;

  for (Long64_t jentry=0; jentry<nentries; jentry++)
  {
    time.PrintPercentageAndRemainingTime(jentry, nentries);

    fChain->GetEntry(jentry);

    //**************************************************************************
    //                             从这里开始进行分析
    //__________________________________________________________________________
    //           for SSD1
    if (SSD1_fMultiL2B == 1 && SSD1_fMultiCsI == 1) {  //挑选 one-hit 事件来刻度
      h_L2B_L3[0][SSD1_fNumCsI[0]]->Fill(SSD1_fEChCsI[0], SSD1_fEMeVL2B[0]);
    }
    //__________________________________
    //          for SSD2
    if (SSD2_fMultiL2B == 1 && SSD2_fMultiCsI == 1) { //挑选 one-hit 事件来刻度
      h_L2B_L3[1][SSD2_fNumCsI[0]]->Fill(SSD2_fEChCsI[0], SSD2_fEMeVL2B[0]);
    }
    //__________________________________
    //          for SSD3
    if (SSD3_fMultiL2B == 1 && SSD3_fMultiCsI == 1) { //挑选 one-hit 事件来刻度
      h_L2B_L3[2][SSD3_fNumCsI[0]]->Fill(SSD3_fEChCsI[0], SSD3_fEMeVL2B[0]);
    }
    //__________________________________
    //          for SSD4
    if (SSD4_fMultiL2B == 1 && SSD4_fMultiCsI == 1) { //挑选 one-hit 事件来刻度
      h_L2B_L3[3][SSD4_fNumCsI[0]]->Fill(SSD4_fEChCsI[0], SSD4_fEMeVL2B[0]);
    }
  }

  TCanvas* cans    = new TCanvas("cans","cans",1000,800);

  TCanvas* c_begin = new TCanvas();
  c_begin->Close();
  TCanvas* c_end   = new TCanvas();
  c_end->Close();
  c_begin->Print(pathOutBegin.c_str());

  for (Int_t i=0; i<NUM_SSD; i++)
  {
    for (Int_t j=0; j<NUM_CSI; j++)
    {
      cans->cd();
      gPad->SetLeftMargin(0.12);
      gPad->SetBottomMargin(0.12);
      h_L2B_L3[i][j]->GetXaxis()->SetTitle("ECh_CsI");
      h_L2B_L3[i][j]->GetXaxis()->SetTitleSize(0.05);
      h_L2B_L3[i][j]->GetXaxis()->CenterTitle(1);
      h_L2B_L3[i][j]->GetYaxis()->SetTitle("EMeV_L2B");
      h_L2B_L3[i][j]->GetYaxis()->SetTitleSize(0.05);
      h_L2B_L3[i][j]->GetYaxis()->CenterTitle(1);
      h_L2B_L3[i][j]->GetYaxis()->SetTitleOffset(0.9);

      h_L2B_L3[i][j]->Draw("COLZ");
      gPad->Modified();
      gPad->Update();

      cans->Print(pathOutput);
    }
  }
  c_end->Print(pathOutEnd.c_str());

  time.GetEndTime();
  time.GetRunTime();
}


//______________________________________________________________________________
void CSHINEEventTreeReader::DEE_L1_L2F(const char* pathOutput)
{
  if (fChain == 0 ) return;

  TimeAndPercentage time;
  time.GetBeginTime();

  std::string pathOutBegin(Form("%s[", pathOutput));
  std::string pathOutEnd  (Form("%s]", pathOutput));

  Double_t RangeEMeVL1S[4] = {250., 200., 100., 70.};
  Int_t    NBinsEMeVL1S[4] = {2500, 2000, 1000, 700};  // Y Axis
  Double_t RangeEMeVL2F[4] = {300., 250., 150., 80.};
  Int_t    NBinsEMeVL2F[4] = {3000, 2500, 1500, 800};  // X Axis

  TH2D* h_L1_L2F[NUM_SSD][NUM_STRIP];
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
      h_L1_L2F[i][j] = new TH2D(Form("h_L1_L2F_SSD%d_CH%d",i+1,j), Form("h_L1_L2F_SSD%d_CH%d",i+1,j),
                                NBinsEMeVL2F[i], 0, RangeEMeVL2F[i], NBinsEMeVL1S[i], 0., RangeEMeVL1S[i]);
    }
  }

  fChain->SetBranchStatus("*", false);
  fChain->SetBranchStatus("SSD1.fMultiL1S",    true);
  fChain->SetBranchStatus("SSD1.fNumStripL1S", true);
  fChain->SetBranchStatus("SSD1.fEMeVL1S",     true);
  fChain->SetBranchStatus("SSD1.fMultiL2F",    true);
  fChain->SetBranchStatus("SSD1.fNumStripL2F", true);
  fChain->SetBranchStatus("SSD1.fEMeVL2F",     true);

  fChain->SetBranchStatus("SSD2.fMultiL1S",    true);
  fChain->SetBranchStatus("SSD2.fNumStripL1S", true);
  fChain->SetBranchStatus("SSD2.fEMeVL1S",     true);
  fChain->SetBranchStatus("SSD2.fMultiL2F",    true);
  fChain->SetBranchStatus("SSD2.fNumStripL2F", true);
  fChain->SetBranchStatus("SSD2.fEMeVL2F",     true);

  fChain->SetBranchStatus("SSD3.fMultiL1S",    true);
  fChain->SetBranchStatus("SSD3.fNumStripL1S", true);
  fChain->SetBranchStatus("SSD3.fEMeVL1S",     true);
  fChain->SetBranchStatus("SSD3.fMultiL2F",    true);
  fChain->SetBranchStatus("SSD3.fNumStripL2F", true);
  fChain->SetBranchStatus("SSD3.fEMeVL2F",     true);

  fChain->SetBranchStatus("SSD4.fMultiL1S",    true);
  fChain->SetBranchStatus("SSD4.fNumStripL1S", true);
  fChain->SetBranchStatus("SSD4.fEMeVL1S",     true);
  fChain->SetBranchStatus("SSD4.fMultiL2F",    true);
  fChain->SetBranchStatus("SSD4.fNumStripL2F", true);
  fChain->SetBranchStatus("SSD4.fEMeVL2F",     true);

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"*** Found  "<<nentries<<" entries. ***"<<endl;

  for (Long64_t jentry=0; jentry<nentries; jentry++)
  {
    time.PrintPercentageAndRemainingTime(jentry, nentries);

    fChain->GetEntry(jentry);

    //**************************************************************************
    //                             从这里开始进行分析
    //__________________________________________________________________________
    //           for SSD1
    if (SSD1_fMultiL1S == 1 && SSD1_fMultiL2F == 1) {  //挑选 one-hit 事件来刻度
      h_L1_L2F[0][SSD1_fNumStripL2F[0]]->Fill(SSD1_fEMeVL2F[0], SSD1_fEMeVL1S[0]);
    }
    //__________________________________
    //          for SSD2
    if (SSD2_fMultiL1S == 1 && SSD2_fMultiL2F == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2F[1][SSD2_fNumStripL2F[0]]->Fill(SSD2_fEMeVL2F[0], SSD2_fEMeVL1S[0]);
    }
    //__________________________________
    //          for SSD3
    if (SSD3_fMultiL1S == 1 && SSD3_fMultiL2F == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2F[2][SSD3_fNumStripL2F[0]]->Fill(SSD3_fEMeVL2F[0], SSD3_fEMeVL1S[0]);
    }
    //__________________________________
    //          for SSD4
    if (SSD4_fMultiL1S == 1 && SSD4_fMultiL2F == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2F[3][SSD4_fNumStripL2F[0]]->Fill(SSD4_fEMeVL2F[0], SSD4_fEMeVL1S[0]);
    }
  }

  TCanvas* cans    = new TCanvas("cans","cans",1000,800);

  TCanvas* c_begin = new TCanvas();
  c_begin->Close();
  TCanvas* c_end   = new TCanvas();
  c_end->Close();
  c_begin->Print(pathOutBegin.c_str());

  for (Int_t i=0; i<NUM_SSD; i++)
  {
    for (Int_t j=0; j<NUM_STRIP; j++)
    {
      cans->cd();
      gPad->SetLeftMargin(0.12);
      gPad->SetBottomMargin(0.12);
      h_L1_L2F[i][j]->GetXaxis()->SetTitle("EMeV_L2F");
      h_L1_L2F[i][j]->GetXaxis()->SetTitleSize(0.05);
      h_L1_L2F[i][j]->GetXaxis()->CenterTitle(1);
      h_L1_L2F[i][j]->GetYaxis()->SetTitle("EMeV_L1S");
      h_L1_L2F[i][j]->GetYaxis()->SetTitleSize(0.05);
      h_L1_L2F[i][j]->GetYaxis()->CenterTitle(1);
      h_L1_L2F[i][j]->GetYaxis()->SetTitleOffset(0.9);

      h_L1_L2F[i][j]->Draw("COLZ");
      gPad->Modified();
      gPad->Update();

      cans->Print(pathOutput);
    }
  }
  c_end->Print(pathOutEnd.c_str());

  time.GetEndTime();
  time.GetRunTime();
}

//______________________________________________________________________________
void CSHINEEventTreeReader::DEE_L1_L2B(const char* pathOutput)
{
  if (fChain == 0 ) return;

  TimeAndPercentage time;
  time.GetBeginTime();

  std::string pathOutBegin(Form("%s[", pathOutput));
  std::string pathOutEnd  (Form("%s]", pathOutput));

  Double_t RangeEMeVL1S[4] = {250., 200., 100., 70.};
  Int_t    NBinsEMeVL1S[4] = {2500, 2000, 1000, 700};  // Y Axis
  Double_t RangeEMeVL2B[4] = {300., 250., 150., 80.};
  Int_t    NBinsEMeVL2B[4] = {3000, 2500, 1500, 800};  // X Axis

  TH2D* h_L1_L2B[NUM_SSD][NUM_STRIP];
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_STRIP; j++) {
      h_L1_L2B[i][j] = new TH2D(Form("h_L1_L2B_SSD%d_CH%d",i+1,j), Form("h_L1_L2B_SSD%d_CH%d",i+1,j),
                                NBinsEMeVL2B[i], 0, RangeEMeVL2B[i], NBinsEMeVL1S[i], 0., RangeEMeVL1S[i]);
    }
  }

  fChain->SetBranchStatus("*", false);
  fChain->SetBranchStatus("SSD1.fMultiL1S",    true);
  fChain->SetBranchStatus("SSD1.fNumStripL1S", true);
  fChain->SetBranchStatus("SSD1.fEMeVL1S",     true);
  fChain->SetBranchStatus("SSD1.fMultiL2B",    true);
  fChain->SetBranchStatus("SSD1.fNumStripL2B", true);
  fChain->SetBranchStatus("SSD1.fEMeVL2B",     true);

  fChain->SetBranchStatus("SSD2.fMultiL1S",    true);
  fChain->SetBranchStatus("SSD2.fNumStripL1S", true);
  fChain->SetBranchStatus("SSD2.fEMeVL1S",     true);
  fChain->SetBranchStatus("SSD2.fMultiL2B",    true);
  fChain->SetBranchStatus("SSD2.fNumStripL2B", true);
  fChain->SetBranchStatus("SSD2.fEMeVL2B",     true);

  fChain->SetBranchStatus("SSD3.fMultiL1S",    true);
  fChain->SetBranchStatus("SSD3.fNumStripL1S", true);
  fChain->SetBranchStatus("SSD3.fEMeVL1S",     true);
  fChain->SetBranchStatus("SSD3.fMultiL2B",    true);
  fChain->SetBranchStatus("SSD3.fNumStripL2B", true);
  fChain->SetBranchStatus("SSD3.fEMeVL2B",     true);

  fChain->SetBranchStatus("SSD4.fMultiL1S",    true);
  fChain->SetBranchStatus("SSD4.fNumStripL1S", true);
  fChain->SetBranchStatus("SSD4.fEMeVL1S",     true);
  fChain->SetBranchStatus("SSD4.fMultiL2B",    true);
  fChain->SetBranchStatus("SSD4.fNumStripL2B", true);
  fChain->SetBranchStatus("SSD4.fEMeVL2B",     true);

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"*** Found  "<<nentries<<" entries. ***"<<endl;

  for (Long64_t jentry=0; jentry<nentries; jentry++)
  {
    time.PrintPercentageAndRemainingTime(jentry, nentries);

    fChain->GetEntry(jentry);

    //**************************************************************************
    //                             从这里开始进行分析
    //__________________________________________________________________________
    //           for SSD1
    if (SSD1_fMultiL1S == 1 && SSD1_fMultiL2B == 1) {  //挑选 one-hit 事件来刻度
      h_L1_L2B[0][SSD1_fNumStripL2B[0]]->Fill(SSD1_fEMeVL2B[0], SSD1_fEMeVL1S[0]);
    }
    //__________________________________
    //          for SSD2
    if (SSD2_fMultiL1S == 1 && SSD2_fMultiL2B == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2B[1][SSD2_fNumStripL2B[0]]->Fill(SSD2_fEMeVL2B[0], SSD2_fEMeVL1S[0]);
    }
    //__________________________________
    //          for SSD3
    if (SSD3_fMultiL1S == 1 && SSD3_fMultiL2B == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2B[2][SSD3_fNumStripL2B[0]]->Fill(SSD3_fEMeVL2B[0], SSD3_fEMeVL1S[0]);
    }
    //__________________________________
    //          for SSD4
    if (SSD4_fMultiL1S == 1 && SSD4_fMultiL2B == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2B[3][SSD4_fNumStripL2B[0]]->Fill(SSD4_fEMeVL2B[0], SSD4_fEMeVL1S[0]);
    }
  }

  TCanvas* cans    = new TCanvas("cans","cans",1000,800);

  TCanvas* c_begin = new TCanvas();
  c_begin->Close();
  TCanvas* c_end   = new TCanvas();
  c_end->Close();
  c_begin->Print(pathOutBegin.c_str());

  for (Int_t i=0; i<NUM_SSD; i++)
  {
    for (Int_t j=0; j<NUM_STRIP; j++)
    {
      cans->cd();
      gPad->SetLeftMargin(0.12);
      gPad->SetBottomMargin(0.12);
      h_L1_L2B[i][j]->GetXaxis()->SetTitle("EMeV_L2B");
      h_L1_L2B[i][j]->GetXaxis()->SetTitleSize(0.05);
      h_L1_L2B[i][j]->GetXaxis()->CenterTitle(1);
      h_L1_L2B[i][j]->GetYaxis()->SetTitle("EMeV_L1S");
      h_L1_L2B[i][j]->GetYaxis()->SetTitleSize(0.05);
      h_L1_L2B[i][j]->GetYaxis()->CenterTitle(1);
      h_L1_L2B[i][j]->GetYaxis()->SetTitleOffset(0.9);

      h_L1_L2B[i][j]->Draw("COLZ");
      gPad->Modified();
      gPad->Update();

      cans->Print(pathOutput);
    }
  }
  c_end->Print(pathOutEnd.c_str());

  time.GetEndTime();
  time.GetRunTime();
}
