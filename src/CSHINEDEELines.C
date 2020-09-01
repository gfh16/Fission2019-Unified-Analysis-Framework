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
//  ** 添加 CSHINEDEELines::DEE_L2F_L3
//  ** 添加 CSHINEDEELines::DEE_L2B_L3
//  ** 添加 CSHINEDEELines::DEE_L1_L2F
//  ** 添加 CSHINEDEELines::DEE_L1_L2B
//
////////////////////////////////////////////////////////////////////////////////

#define  CSHINEDEELines_cxx
#include "../include/CSHINEDEELines.h"
#include "../include/TimeAndPercentage.h"
#include "../include/shared.h"
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>


//______________________________________________________________________________
void CSHINEDEELines::DEE_All(const char* pathOutput)
{
  if (fChain == 0 ) return;

  TimeAndPercentage time;
  time.GetBeginTime();

  std::string pathOut_L1L2F      (Form("%s_L1L2F.pdf" , pathOutput));
  std::string pathOut_L1L2F_Begin(Form("%s_L1L2F.pdf[", pathOutput));
  std::string pathOut_L1L2F_End  (Form("%s_L1L2F.pdf]", pathOutput));

  std::string pathOut_L1L2B      (Form("%s_L1L2B.pdf" , pathOutput));
  std::string pathOut_L1L2B_Begin(Form("%s_L1L2B.pdf[", pathOutput));
  std::string pathOut_L1L2B_End  (Form("%s_L1L2B.pdf]", pathOutput));

  std::string pathOut_L2FL3      (Form("%s_L2FL3.pdf",  pathOutput));
  std::string pathOut_L2FL3_Begin(Form("%s_L2FL3.pdf[", pathOutput));
  std::string pathOut_L2FL3_End  (Form("%s_L2FL3.pdf]", pathOutput));

  std::string pathOut_L2BL3      (Form("%s_L2BL3.pdf",  pathOutput));
  std::string pathOut_L2BL3_Begin(Form("%s_L2BL3.pdf[", pathOutput));
  std::string pathOut_L2BL3_End  (Form("%s_L2BL3.pdf]", pathOutput));

  Double_t RangeEMeVL1[4] = { 250.,  200.,  100.,  70. };
  Int_t    NBinsEMeVL1[4] = { 2500,  2000,  1000,  700 };

  Double_t RangeEMeVL2[4] = { 300.,  250.,  150.,  80. };
  Int_t    NBinsEMeVL2[4] = { 3000,  2500,  1500,  800 };

  TH2D* h_L2F_L3[NUM_SSD][NUM_CSI];
  TH2D* h_L2B_L3[NUM_SSD][NUM_CSI];
  TH2D* h_L1_L2F[NUM_SSD][NUM_STRIP];
  TH2D* h_L1_L2B[NUM_SSD][NUM_STRIP];
  //________________________________
  // 直方图定义
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_CSI; j++) {
      h_L2F_L3[i][j] = new TH2D(Form("h_L2F_L3_SSD%d_CsI%d",i+1,j), Form("h_L2F_L3_SSD%d_CsI%d",i+1,j),
                                4096, 0, 4096, NBinsEMeVL2[i], 0., RangeEMeVL2[i]);
      h_L2B_L3[i][j] = new TH2D(Form("h_L2B_L3_SSD%d_CsI%d",i+1,j), Form("h_L2B_L3_SSD%d_CsI%d",i+1,j),
                                4096, 0, 4096, NBinsEMeVL2[i], 0., RangeEMeVL2[i]);
    }

    for (Int_t k=0; k<NUM_STRIP; k++) {
      h_L1_L2F[i][k] = new TH2D(Form("h_L1_L2F_SSD%d_CH%d",i+1,k), Form("h_L1_L2F_SSD%d_CH%d",i+1,k),
                                NBinsEMeVL2[i], 0, RangeEMeVL2[i], NBinsEMeVL1[i], 0., RangeEMeVL1[i]);
      h_L1_L2B[i][k] = new TH2D(Form("h_L1_L2B_SSD%d_CH%d",i+1,k), Form("h_L1_L2B_SSD%d_CH%d",i+1,k),
                                NBinsEMeVL2[i], 0, RangeEMeVL2[i], NBinsEMeVL1[i], 0., RangeEMeVL1[i]);
    }
  }

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"*** Found  "<<nentries<<" entries. ***"<<endl;

  for (Long64_t jentry=0; jentry<nentries; jentry++)
  {
    time.PrintPercentageAndRemainingTime(jentry, nentries);

    fChain->GetEntry(jentry);

    //**************************************************************************
    //                             从这里开始进行分析

    //--------------------------------------------------------------------------
    //                              DEE_L2F_CsI
    //--------------------------------------------------------------------------
    //_______________________________________________
    //  DEE_L2F_CsI ------- SSD1
    if (SSD1_fL2FMulti == 1 && SSD1_fCsIMulti == 1) {  //挑选 one-hit 事件来刻度
      h_L2F_L3[0][SSD1_fCsINum[0]]->Fill(SSD1_fCsIECh[0], SSD1_fL2FEMeV[0]);
    }
    //_______________________________________________
    //  DEE_L2F_CsI ------- SSD2
    if (SSD2_fL2FMulti == 1 && SSD2_fCsIMulti == 1) { //挑选 one-hit 事件来刻度
      h_L2F_L3[1][SSD2_fCsINum[0]]->Fill(SSD2_fCsIECh[0], SSD2_fL2FEMeV[0]);
    }
    //_______________________________________________
    //  DEE_L2F_CsI ------- SSD3
    if (SSD3_fL2FMulti == 1 && SSD3_fCsIMulti == 1) { //挑选 one-hit 事件来刻度
      h_L2F_L3[2][SSD3_fCsINum[0]]->Fill(SSD3_fCsIECh[0], SSD3_fL2FEMeV[0]);
    }
    //_______________________________________________
    //  DEE_L2F_CsI ------- SSD4
    if (SSD4_fL2FMulti == 1 && SSD4_fCsIMulti == 1) { //挑选 one-hit 事件来刻度
      h_L2F_L3[3][SSD4_fCsINum[0]]->Fill(SSD4_fCsIECh[0], SSD4_fL2FEMeV[0]);
    }


    //--------------------------------------------------------------------------
    //                              DEE_L2B_CsI
    //--------------------------------------------------------------------------
    //_______________________________________________
    // DEE_L2B_CsI ------- SSD1
    if (SSD1_fL2BMulti == 1 && SSD1_fCsIMulti == 1) {  //挑选 one-hit 事件来刻度
      h_L2B_L3[0][SSD1_fCsINum[0]]->Fill(SSD1_fCsIECh[0], SSD1_fL2BEMeV[0]);
    }
    //______________________________________________
    // DEE_L2B_CsI ------- SSD2
    if (SSD2_fL2BMulti == 1 && SSD2_fCsIMulti == 1) { //挑选 one-hit 事件来刻度
      h_L2B_L3[1][SSD2_fCsINum[0]]->Fill(SSD2_fCsIECh[0], SSD2_fL2BEMeV[0]);
    }
    //_____________________________________________
    // DEE_L2B_CsI ------- SSD3
    if (SSD3_fL2BMulti == 1 && SSD3_fCsIMulti == 1) { //挑选 one-hit 事件来刻度
      h_L2B_L3[2][SSD3_fCsINum[0]]->Fill(SSD3_fCsIECh[0], SSD3_fL2BEMeV[0]);
    }
    //____________________________________________
    // DEE_L2B_CsI ------- SSD4
    if (SSD4_fL2BMulti == 1 && SSD4_fCsIMulti == 1) { //挑选 one-hit 事件来刻度
      h_L2B_L3[3][SSD4_fCsINum[0]]->Fill(SSD4_fCsIECh[0], SSD4_fL2BEMeV[0]);
    }


    //--------------------------------------------------------------------------
    //                             DEE_L1_L2F
    //--------------------------------------------------------------------------
    //________________________________________________
    // DEE_L1_L2F ------- SSD1
    if (SSD1_fL1SMulti == 1 && SSD1_fL2FMulti == 1) {  //挑选 one-hit 事件来刻度
      h_L1_L2F[0][SSD1_fL2FNumStrip[0]]->Fill(SSD1_fL2FEMeV[0], SSD1_fL1SEMeV[0]);
    }
    //_______________________________________________
    // DEE_L1_L2F ------- SSD2
    if (SSD2_fL1SMulti == 1 && SSD2_fL2FMulti == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2F[1][SSD2_fL2FNumStrip[0]]->Fill(SSD2_fL2FEMeV[0], SSD2_fL1SEMeV[0]);
    }
    //_______________________________________________
    // DEE_L1_L2F ------- SSD3
    if (SSD3_fL1SMulti == 1 && SSD3_fL2FMulti == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2F[2][SSD3_fL2FNumStrip[0]]->Fill(SSD3_fL2FEMeV[0], SSD3_fL1SEMeV[0]);
    }
    //_______________________________________________
    // DEE_L1_L2F ------- SSD4
    if (SSD4_fL1SMulti == 1 && SSD4_fL2FMulti == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2F[3][SSD4_fL2FNumStrip[0]]->Fill(SSD4_fL2FEMeV[0], SSD4_fL1SEMeV[0]);
    }


    //--------------------------------------------------------------------------
    //                             DEE_L1_L2B
    //--------------------------------------------------------------------------
    //______________________________________________
    // DEE_L1_L2B --------------- SSD1
    if (SSD1_fL1SMulti == 1 && SSD1_fL2BMulti == 1) {  //挑选 one-hit 事件来刻度
      h_L1_L2B[0][SSD1_fL2BNumStrip[0]]->Fill(SSD1_fL2BEMeV[0], SSD1_fL1SEMeV[0]);
    }
    //_____________________________________________
    /// DEE_L1_L2B -------- SSD2
    if (SSD2_fL1SMulti == 1 && SSD2_fL2BMulti == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2B[1][SSD2_fL2BNumStrip[0]]->Fill(SSD2_fL2BEMeV[0], SSD2_fL1SEMeV[0]);
    }
    //______________________________________________
    /// DEE_L1_L2B -------- SSD3
    if (SSD3_fL1SMulti == 1 && SSD3_fL2BMulti == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2B[2][SSD3_fL2BNumStrip[0]]->Fill(SSD3_fL2BEMeV[0], SSD3_fL1SEMeV[0]);
    }
    //______________________________________________
    /// DEE_L1_L2B -------- SSD4
    if (SSD4_fL1SMulti == 1 && SSD4_fL2BMulti == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2B[3][SSD4_fL2BNumStrip[0]]->Fill(SSD4_fL2BEMeV[0], SSD4_fL1SEMeV[0]);
    }
  }

  TCanvas* Cans_L2F_L3 = new TCanvas("Cans_L2F_L3", "Cans_L2F_L3", 1000, 800);
  TCanvas* Cans_L2B_L3 = new TCanvas("Cans_L2B_L3", "Cans_L2B_L3", 1000, 800);
  TCanvas* Cans_L1_L2F = new TCanvas("Cans_L1_L2F", "Cans_L1_L2F", 1000, 800);
  TCanvas* Cans_L1_L2B = new TCanvas("Cans_L1_L2B", "Cans_L1_L2B", 1000, 800);

  TCanvas* L1L2F_c_begin = new TCanvas();
  TCanvas* L1L2F_c_end   = new TCanvas();
  TCanvas* L1L2B_c_begin = new TCanvas();
  TCanvas* L1L2B_c_end   = new TCanvas();
  TCanvas* L2FL3_c_begin = new TCanvas();
  TCanvas* L2FL3_c_end   = new TCanvas();
  TCanvas* L2BL3_c_begin = new TCanvas();
  TCanvas* L2BL3_c_end   = new TCanvas();
  L1L2F_c_begin->Close();
  L1L2F_c_end  ->Close();
  L1L2B_c_begin->Close();
  L1L2B_c_end  ->Close();
  L2FL3_c_begin->Close();
  L2FL3_c_end  ->Close();
  L2BL3_c_begin->Close();
  L2BL3_c_end  ->Close();
  L1L2F_c_begin->Print(pathOut_L1L2F_Begin.c_str());
  L1L2B_c_begin->Print(pathOut_L1L2B_Begin.c_str());
  L2FL3_c_begin->Print(pathOut_L2FL3_Begin.c_str());
  L2BL3_c_begin->Print(pathOut_L2BL3_Begin.c_str());

  for (Int_t i=0; i<NUM_SSD; i++)
  {
    for (Int_t j=0; j<NUM_CSI; j++)
    {
      Cans_L2F_L3->cd();
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
      Cans_L2F_L3->Print(pathOut_L2FL3.c_str());
      gPad->Modified();
      gPad->Update();


      Cans_L2B_L3->cd();
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
      Cans_L2B_L3->Print(pathOut_L2BL3.c_str());
      gPad->Modified();
      gPad->Update();
    }

    for (Int_t k=0; k<NUM_STRIP; k++)
    {
      Cans_L1_L2F->cd();
      gPad->SetLeftMargin(0.12);
      gPad->SetBottomMargin(0.12);
      h_L1_L2F[i][k]->GetXaxis()->SetTitle("EMeV_L2F");
      h_L1_L2F[i][k]->GetXaxis()->SetTitleSize(0.05);
      h_L1_L2F[i][k]->GetXaxis()->CenterTitle(1);
      h_L1_L2F[i][k]->GetYaxis()->SetTitle("EMeV_L1S");
      h_L1_L2F[i][k]->GetYaxis()->SetTitleSize(0.05);
      h_L1_L2F[i][k]->GetYaxis()->CenterTitle(1);
      h_L1_L2F[i][k]->GetYaxis()->SetTitleOffset(0.9);
      h_L1_L2F[i][k]->Draw("COLZ");
      Cans_L1_L2F->Print(pathOut_L1L2F.c_str());
      gPad->Modified();
      gPad->Update();


      Cans_L1_L2B->cd();
      gPad->SetLeftMargin(0.12);
      gPad->SetBottomMargin(0.12);
      h_L1_L2B[i][k]->GetXaxis()->SetTitle("EMeV_L2B");
      h_L1_L2B[i][k]->GetXaxis()->SetTitleSize(0.05);
      h_L1_L2B[i][k]->GetXaxis()->CenterTitle(1);
      h_L1_L2B[i][k]->GetYaxis()->SetTitle("EMeV_L1S");
      h_L1_L2B[i][k]->GetYaxis()->SetTitleSize(0.05);
      h_L1_L2B[i][k]->GetYaxis()->CenterTitle(1);
      h_L1_L2B[i][k]->GetYaxis()->SetTitleOffset(0.9);
      h_L1_L2B[i][k]->Draw("COLZ");
      Cans_L1_L2B->Print(pathOut_L1L2B.c_str());
      gPad->Modified();
      gPad->Update();
    }
  }
  L1L2F_c_end->Print(pathOut_L1L2F_End.c_str());
  L1L2B_c_end->Print(pathOut_L1L2B_End.c_str());
  L2FL3_c_end->Print(pathOut_L2FL3_End.c_str());
  L2BL3_c_end->Print(pathOut_L2BL3_End.c_str());

  time.GetEndTime();
  time.GetRunTime();
}



//______________________________________________________________________________
void CSHINEDEELines::DEE_L2F_L3(const char* pathOutput)
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
  fChain->SetBranchStatus("SSD1.fL2FMulti",    true);
  fChain->SetBranchStatus("SSD1.fL2FNumStrip", true);
  fChain->SetBranchStatus("SSD1.fL2FEMeV",     true);
  fChain->SetBranchStatus("SSD1.fCsIMulti",    true);
  fChain->SetBranchStatus("SSD1.fCsINum",      true);
  fChain->SetBranchStatus("SSD1.fCsIECh",      true);

  fChain->SetBranchStatus("SSD2.fL2FMulti",    true);
  fChain->SetBranchStatus("SSD2.fL2FNumStrip", true);
  fChain->SetBranchStatus("SSD2.fL2FEMeV",     true);
  fChain->SetBranchStatus("SSD2.fCsIMulti",    true);
  fChain->SetBranchStatus("SSD2.fCsINum",      true);
  fChain->SetBranchStatus("SSD2.fCsIECh",      true);

  fChain->SetBranchStatus("SSD3.fL2FMulti",    true);
  fChain->SetBranchStatus("SSD3.fL2FNumStrip", true);
  fChain->SetBranchStatus("SSD3.fL2FEMeV",     true);
  fChain->SetBranchStatus("SSD3.fCsIMulti",    true);
  fChain->SetBranchStatus("SSD3.fCsINum",      true);
  fChain->SetBranchStatus("SSD3.fCsIECh",      true);

  fChain->SetBranchStatus("SSD4.fL2FMulti",    true);
  fChain->SetBranchStatus("SSD4.fL2FNumStrip", true);
  fChain->SetBranchStatus("SSD4.fL2FEMeV",     true);
  fChain->SetBranchStatus("SSD4.fCsIMulti",    true);
  fChain->SetBranchStatus("SSD4.fCsINum",      true);
  fChain->SetBranchStatus("SSD4.fCsIECh",      true);

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
    if (SSD1_fL2FMulti == 1 && SSD1_fCsIMulti == 1) {  //挑选 one-hit 事件来刻度
      h_L2F_L3[0][SSD1_fCsINum[0]]->Fill(SSD1_fCsIECh[0], SSD1_fL2FEMeV[0]);
    }
    //__________________________________
    //          for SSD2
    if (SSD2_fL2FMulti == 1 && SSD2_fCsIMulti == 1) { //挑选 one-hit 事件来刻度
      h_L2F_L3[1][SSD2_fCsINum[0]]->Fill(SSD2_fCsIECh[0], SSD2_fL2FEMeV[0]);
    }
    //__________________________________
    //          for SSD3
    if (SSD3_fL2FMulti == 1 && SSD3_fCsIMulti == 1) { //挑选 one-hit 事件来刻度
      h_L2F_L3[2][SSD3_fCsINum[0]]->Fill(SSD3_fCsIECh[0], SSD3_fL2FEMeV[0]);
    }
    //__________________________________
    //          for SSD4
    if (SSD4_fL2FMulti == 1 && SSD4_fCsIMulti == 1) { //挑选 one-hit 事件来刻度
      h_L2F_L3[3][SSD4_fCsINum[0]]->Fill(SSD4_fCsIECh[0], SSD4_fL2FEMeV[0]);
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
void CSHINEDEELines::DEE_L2B_L3(const char* pathOutput)
{
  if (fChain == 0 ) return;

  TimeAndPercentage time;
  time.GetBeginTime();

  std::string pathOutBegin(Form("%s[", pathOutput));
  std::string pathOutEnd  (Form("%s]", pathOutput));

  Double_t RangeEMeVL2B[4] = {250., 250., 150., 100.};
  Int_t    NBinsEMeVL2B[4] = {2500, 2500, 1500, 1000};

  TH2D* h_L2B_L3[NUM_SSD][NUM_CSI];
  for (Int_t i=0; i<NUM_SSD; i++) {
    for (Int_t j=0; j<NUM_CSI; j++) {
      h_L2B_L3[i][j] = new TH2D(Form("h_L2B_L3_SSD%d_CsI%d",i+1,j), Form("h_L2B_L3_SSD%d_CsI%d",i+1,j),
                                4096, 0, 4096, NBinsEMeVL2B[i], 0., RangeEMeVL2B[i]);
    }
  }

  fChain->SetBranchStatus("*", false);
  fChain->SetBranchStatus("SSD1.fL2BMulti",    true);
  fChain->SetBranchStatus("SSD1.fL2BNumStrip", true);
  fChain->SetBranchStatus("SSD1.fL2BEMeV",     true);
  fChain->SetBranchStatus("SSD1.fCsIMulti",    true);
  fChain->SetBranchStatus("SSD1.fCsINum",      true);
  fChain->SetBranchStatus("SSD1.fCsIECh",      true);

  fChain->SetBranchStatus("SSD2.fL2BMulti",    true);
  fChain->SetBranchStatus("SSD2.fL2BNumStrip", true);
  fChain->SetBranchStatus("SSD2.fL2BEMeV",     true);
  fChain->SetBranchStatus("SSD2.fCsIMulti",    true);
  fChain->SetBranchStatus("SSD2.fCsINum",      true);
  fChain->SetBranchStatus("SSD2.fCsIECh",      true);

  fChain->SetBranchStatus("SSD3.fL2BMulti",    true);
  fChain->SetBranchStatus("SSD3.fL2BNumStrip", true);
  fChain->SetBranchStatus("SSD3.fL2BEMeV",     true);
  fChain->SetBranchStatus("SSD3.fCsIMulti",    true);
  fChain->SetBranchStatus("SSD3.fCsINum",      true);
  fChain->SetBranchStatus("SSD3.fCsIECh",      true);

  fChain->SetBranchStatus("SSD4.fL2BMulti",    true);
  fChain->SetBranchStatus("SSD4.fL2BNumStrip", true);
  fChain->SetBranchStatus("SSD4.fL2BEMeV",     true);
  fChain->SetBranchStatus("SSD4.fCsIMulti",    true);
  fChain->SetBranchStatus("SSD4.fCsINum",      true);
  fChain->SetBranchStatus("SSD4.fCsIECh",      true);

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
    if (SSD1_fL2BMulti == 1 && SSD1_fCsIMulti == 1) {  //挑选 one-hit 事件来刻度
      h_L2B_L3[0][SSD1_fCsINum[0]]->Fill(SSD1_fCsIECh[0], SSD1_fL2BEMeV[0]);
    }
    //__________________________________
    //          for SSD2
    if (SSD2_fL2BMulti == 1 && SSD2_fCsIMulti == 1) { //挑选 one-hit 事件来刻度
      h_L2B_L3[1][SSD2_fCsINum[0]]->Fill(SSD2_fCsIECh[0], SSD2_fL2BEMeV[0]);
    }
    //__________________________________
    //          for SSD3
    if (SSD3_fL2BMulti == 1 && SSD3_fCsIMulti == 1) { //挑选 one-hit 事件来刻度
      h_L2B_L3[2][SSD3_fCsINum[0]]->Fill(SSD3_fCsIECh[0], SSD3_fL2BEMeV[0]);
    }
    //__________________________________
    //          for SSD4
    if (SSD4_fL2BMulti == 1 && SSD4_fCsIMulti == 1) { //挑选 one-hit 事件来刻度
      h_L2B_L3[3][SSD4_fCsINum[0]]->Fill(SSD4_fCsIECh[0], SSD4_fL2BEMeV[0]);
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
void CSHINEDEELines::DEE_L1_L2F(const char* pathOutput)
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
  fChain->SetBranchStatus("SSD1.fL1SMulti",    true);
  fChain->SetBranchStatus("SSD1.fL1SNumStrip", true);
  fChain->SetBranchStatus("SSD1.fL1SEMeV",     true);
  fChain->SetBranchStatus("SSD1.fL2FMulti",    true);
  fChain->SetBranchStatus("SSD1.fL2FNumStrip", true);
  fChain->SetBranchStatus("SSD1.fL2FEMeV",     true);

  fChain->SetBranchStatus("SSD2.fL1SMulti",    true);
  fChain->SetBranchStatus("SSD2.fL1SNumStrip", true);
  fChain->SetBranchStatus("SSD2.fL1SEMeV",     true);
  fChain->SetBranchStatus("SSD2.fL2FMulti",    true);
  fChain->SetBranchStatus("SSD2.fL2FNumStrip", true);
  fChain->SetBranchStatus("SSD2.fL2FEMeV",     true);

  fChain->SetBranchStatus("SSD3.fL1SMulti",    true);
  fChain->SetBranchStatus("SSD3.fL1SNumStrip", true);
  fChain->SetBranchStatus("SSD3.fL1SEMeV",     true);
  fChain->SetBranchStatus("SSD3.fL2FMulti",    true);
  fChain->SetBranchStatus("SSD3.fL2FNumStrip", true);
  fChain->SetBranchStatus("SSD3.fL2FEMeV",     true);

  fChain->SetBranchStatus("SSD4.fL1SMulti",    true);
  fChain->SetBranchStatus("SSD4.fL1SNumStrip", true);
  fChain->SetBranchStatus("SSD4.fL1SEMeV",     true);
  fChain->SetBranchStatus("SSD4.fL2FMulti",    true);
  fChain->SetBranchStatus("SSD4.fL2FNumStrip", true);
  fChain->SetBranchStatus("SSD4.fL2FEMeV",     true);

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
    if (SSD1_fL1SMulti == 1 && SSD1_fL2FMulti == 1) {  //挑选 one-hit 事件来刻度
      h_L1_L2F[0][SSD1_fL2FNumStrip[0]]->Fill(SSD1_fL2FEMeV[0], SSD1_fL1SEMeV[0]);
    }
    //__________________________________
    //          for SSD2
    if (SSD2_fL1SMulti == 1 && SSD2_fL2FMulti == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2F[1][SSD2_fL2FNumStrip[0]]->Fill(SSD2_fL2FEMeV[0], SSD2_fL1SEMeV[0]);
    }
    //__________________________________
    //          for SSD3
    if (SSD3_fL1SMulti == 1 && SSD3_fL2FMulti == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2F[2][SSD3_fL2FNumStrip[0]]->Fill(SSD3_fL2FEMeV[0], SSD3_fL1SEMeV[0]);
    }
    //__________________________________
    //          for SSD4
    if (SSD4_fL1SMulti == 1 && SSD4_fL2FMulti == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2F[3][SSD4_fL2FNumStrip[0]]->Fill(SSD4_fL2FEMeV[0], SSD4_fL1SEMeV[0]);
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
void CSHINEDEELines::DEE_L1_L2B(const char* pathOutput)
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
  fChain->SetBranchStatus("SSD1.fL1SMulti",    true);
  fChain->SetBranchStatus("SSD1.fL1SNumStrip", true);
  fChain->SetBranchStatus("SSD1.fL1SEMeV",     true);
  fChain->SetBranchStatus("SSD1.fL2BMulti",    true);
  fChain->SetBranchStatus("SSD1.fL2BNumStrip", true);
  fChain->SetBranchStatus("SSD1.fL2BEMeV",     true);

  fChain->SetBranchStatus("SSD2.fL1SMulti",    true);
  fChain->SetBranchStatus("SSD2.fL1SNumStrip", true);
  fChain->SetBranchStatus("SSD2.fL1SEMeV",     true);
  fChain->SetBranchStatus("SSD2.fL2BMulti",    true);
  fChain->SetBranchStatus("SSD2.fL2BNumStrip", true);
  fChain->SetBranchStatus("SSD2.fL2BEMeV",     true);

  fChain->SetBranchStatus("SSD3.fL1SMulti",    true);
  fChain->SetBranchStatus("SSD3.fL1SNumStrip", true);
  fChain->SetBranchStatus("SSD3.fL1SEMeV",     true);
  fChain->SetBranchStatus("SSD3.fL2BMulti",    true);
  fChain->SetBranchStatus("SSD3.fL2BNumStrip", true);
  fChain->SetBranchStatus("SSD3.fL2BEMeV",     true);

  fChain->SetBranchStatus("SSD4.fL1SMulti",    true);
  fChain->SetBranchStatus("SSD4.fL1SNumStrip", true);
  fChain->SetBranchStatus("SSD4.fL1SEMeV",     true);
  fChain->SetBranchStatus("SSD4.fL2BMulti",    true);
  fChain->SetBranchStatus("SSD4.fL2BNumStrip", true);
  fChain->SetBranchStatus("SSD4.fL2BEMeV",     true);

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
    if (SSD1_fL1SMulti == 1 && SSD1_fL2BMulti == 1) {  //挑选 one-hit 事件来刻度
      h_L1_L2B[0][SSD1_fL2BNumStrip[0]]->Fill(SSD1_fL2BEMeV[0], SSD1_fL1SEMeV[0]);
    }
    //__________________________________
    //          for SSD2
    if (SSD2_fL1SMulti == 1 && SSD2_fL2BMulti == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2B[1][SSD2_fL2BNumStrip[0]]->Fill(SSD2_fL2BEMeV[0], SSD2_fL1SEMeV[0]);
    }
    //__________________________________
    //          for SSD3
    if (SSD3_fL1SMulti == 1 && SSD3_fL2BMulti == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2B[2][SSD3_fL2BNumStrip[0]]->Fill(SSD3_fL2BEMeV[0], SSD3_fL1SEMeV[0]);
    }
    //__________________________________
    //          for SSD4
    if (SSD4_fL1SMulti == 1 && SSD4_fL2BMulti == 1) { //挑选 one-hit 事件来刻度
      h_L1_L2B[3][SSD4_fL2BNumStrip[0]]->Fill(SSD4_fL2BEMeV[0], SSD4_fL1SEMeV[0]);
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


//______________________________________________________________________________
void CSHINEDEELines::Check_EMeVL2F_EMeVL2B_Correlation(const char* pathOutput)
{
  if (fChain == 0 ) return;

  TimeAndPercentage time;
  time.GetBeginTime();

  std::string pathOutBegin(Form("%s[", pathOutput));
  std::string pathOutEnd  (Form("%s]", pathOutput));

  Double_t RangeEMeV[4] = {250., 250., 150., 100.};
  Int_t    NBinsEMeV[4] = {2500, 2500, 1500, 1000};
  Char_t   histname[4][20];

  TH2D* hist2_L2F_L2B[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    sprintf(histname[i], "hist_SSD%d_L2F_L2B", i+1);
    hist2_L2F_L2B[i] = new TH2D(histname[i], histname[i], NBinsEMeV[i], 0, RangeEMeV[i], NBinsEMeV[i], 0, RangeEMeV[i]);
  }

  fChain->SetBranchStatus("*", false);
  fChain->SetBranchStatus("SSD1.fL2FMulti",  true);
  fChain->SetBranchStatus("SSD1.fL2FEMeV" ,  true);
  fChain->SetBranchStatus("SSD1.fL2BMulti",  true);
  fChain->SetBranchStatus("SSD1.fL2BEMeV" ,  true);

  fChain->SetBranchStatus("SSD2.fL2FMulti",  true);
  fChain->SetBranchStatus("SSD2.fL2FEMeV" ,  true);
  fChain->SetBranchStatus("SSD2.fL2BMulti",  true);
  fChain->SetBranchStatus("SSD2.fL2BEMeV" ,  true);

  fChain->SetBranchStatus("SSD3.fL2FMulti",  true);
  fChain->SetBranchStatus("SSD3.fL2FEMeV" ,  true);
  fChain->SetBranchStatus("SSD3.fL2BMulti",  true);
  fChain->SetBranchStatus("SSD3.fL2BEMeV" ,  true);

  fChain->SetBranchStatus("SSD4.fL2FMulti",  true);
  fChain->SetBranchStatus("SSD4.fL2FEMeV" ,  true);
  fChain->SetBranchStatus("SSD4.fL2BMulti",  true);
  fChain->SetBranchStatus("SSD4.fL2BEMeV" ,  true);

  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"*** Found  "<<nentries<<" entries. ***"<<endl;

  for (Long64_t jentry=0; jentry<nentries; jentry++)
  {

    time.PrintPercentageAndRemainingTime(jentry, nentries);

    fChain->GetEntry(jentry);

    //___________________________________________________
    if (SSD1_fL2FMulti == 1 && SSD1_fL2BMulti == 1) {
      hist2_L2F_L2B[0]->Fill(SSD1_fL2FEMeV[0], SSD1_fL2BEMeV[0]);
    }

    //___________________________________________________
    if (SSD2_fL2FMulti == 1 && SSD2_fL2BMulti == 1) {
      hist2_L2F_L2B[1]->Fill(SSD2_fL2FEMeV[0], SSD2_fL2BEMeV[0]);
    }

    //___________________________________________________
    if (SSD3_fL2FMulti == 1 && SSD3_fL2BMulti == 1) {
      hist2_L2F_L2B[2]->Fill(SSD3_fL2FEMeV[0], SSD3_fL2BEMeV[0]);
    }

    //__________________________________________________
    if (SSD4_fL2FMulti == 1 && SSD4_fL2BMulti == 1) {
      hist2_L2F_L2B[3]->Fill(SSD4_fL2FEMeV[0], SSD4_fL2BEMeV[0]);
    }
  }

  TCanvas* cans = new TCanvas("L2FL2B_ECorrelation", "L2FL2B_ECorrelation", 1000, 1000);
  TCanvas* c_begin = new TCanvas();
  c_begin->Close();
  TCanvas* c_end   = new TCanvas();
  c_end->Close();
  c_begin->Print(pathOutBegin.c_str());

  for (Int_t i=0; i<NUM_SSD; i++)
  {
    cans->cd();
    hist2_L2F_L2B[i]->Draw("COL");

    gPad->Modified();
    gPad->Update();

    cans->Print(pathOutput);
  }
  c_end->Print(pathOutEnd.c_str());

  time.GetEndTime();
  time.GetRunTime();
}
