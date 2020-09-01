#include "../include/TimeAndPercentage.h"
#include "../include/CSHINEHitPatternRecognition.h"
#include "../include/CSHINESSDCalibratedData.h"
#include "../include/shared.h"

//______________________________________________________________________________
Int_t       FirstRun  = 200;
Int_t       LastRun   = 201;
const Int_t NPoints   = 10;
Int_t       MultiCut  = 2;
Int_t       Color[4]  = {kRed-4, kMagenta, kCyan-4, kOrange-3};
Int_t       Maker[4]  = {20, 21, 22, 33};
Double_t    MakerSize = 1.5;
Double_t    SigmaIndex[NPoints] =  {1.,3.,5.,7.,9.,12.,15.,20.,25.,30.};

//______________________________________________________________________________
Double_t EventRatioWithMultiCut(Int_t ssdindex, const char* layertag, Int_t effentries, Int_t entriswithmulticut);
void     AddNumOfCount(Int_t multi, Int_t& emptyentries, Int_t& effentries, Int_t& entrieswithmulticut);

//______________________________________________________________________________
void test10()
{
  gStyle->SetPalette(1);

  TimeAndPercentage time;
  time.GetBeginTime();

  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string L3ATag("L3A");

  std::string pathRootInputFolder("/home/sea/Fission2019_Data/MapRoot/");

  TChain* mychain = new TChain("KrPb");
  for (Int_t i=FirstRun; i<(LastRun+1); i++) {
    mychain->Add(Form("%sMapFission2019_Kr_Pb%04d.root",pathRootInputFolder.c_str(),i));
  }
  mychain->SetBranchStatus("*",false);

  Int_t L1SMulti[NUM_SSD][NPoints]  =  {{0}};
  Int_t L2FMulti[NUM_SSD][NPoints]  =  {{0}};
  Int_t L2BMulti[NUM_SSD][NPoints]  =  {{0}};
  Int_t L3AMulti[NUM_SSD][NPoints]  =  {{0}};

  Int_t L1SNumOfEmptyEntries[NUM_SSD][NPoints]         =  {{0}};
  Int_t L2FNumOfEmptyEntries[NUM_SSD][NPoints]         =  {{0}};
  Int_t L2BNumOfEmptyEntries[NUM_SSD][NPoints]         =  {{0}};
  Int_t L3ANumOfEmptyEntries[NUM_SSD][NPoints]         =  {{0}};

  Int_t L1SNumOfEffEntries[NUM_SSD][NPoints]           =  {{0}};
  Int_t L2FNumOfEffEntries[NUM_SSD][NPoints]           =  {{0}};
  Int_t L2BNumOfEffEntries[NUM_SSD][NPoints]           =  {{0}};
  Int_t L3ANumOfEffEntries[NUM_SSD][NPoints]           =  {{0}};

  Int_t L1SNumOfEntriesWithMultiCut[NUM_SSD][NPoints]  =  {{0}};
  Int_t L2FNumOfEntriesWithMultiCut[NUM_SSD][NPoints]  =  {{0}};
  Int_t L2BNumOfEntriesWithMultiCut[NUM_SSD][NPoints]  =  {{0}};
  Int_t L3ANumOfEntriesWithMultiCut[NUM_SSD][NPoints]  =  {{0}};

  Double_t L1SMultiRatio[NUM_SSD][NPoints]             =  {{0}};
  Double_t L2FMultiRatio[NUM_SSD][NPoints]             =  {{0}};
  Double_t L2BMultiRatio[NUM_SSD][NPoints]             =  {{0}};
  Double_t L3AMultiRatio[NUM_SSD][NPoints]             =  {{0}};

  Int_t SSD_L1S_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2F_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2B_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L3A_E[NUM_SSD][NUM_CSI];

  Double_t* L1SEChCut[NPoints];
  Double_t* L2FEChCut[NPoints];
  Double_t* L2BEChCut[NPoints];
  Double_t* L3AEChCut[NPoints];

  CSHINESSDCalibratedData     calidata[NPoints];
  CSHINEHitPatternRecognition hitpattern;

  // 计算每一层的 EChCut
  for (Int_t np=0; np<NPoints; np++)
  {
    L1SEChCut[np] = calidata[np].GetSiEChCut (L1STag.c_str(), SigmaIndex[np]);
    L2FEChCut[np] = calidata[np].GetSiEChCut (L2FTag.c_str(), SigmaIndex[np]);
    L2BEChCut[np] = calidata[np].GetSiEChCut (L2BTag.c_str(), SigmaIndex[np]);
    L3AEChCut[np] = calidata[np].GetCsIEChCut(L3ATag.c_str(), SigmaIndex[np]);
  }

  // set branchadress
  ReadFileModule readfile;
  for (Int_t i=0; i<NUM_SSD; i++)
  {
    readfile.AddChain(mychain, L1STag.c_str(), SSD_L1S_E[i], NUM_STRIP, i);
    readfile.AddChain(mychain, L2FTag.c_str(), SSD_L2F_E[i], NUM_STRIP, i);
    readfile.AddChain(mychain, L2BTag.c_str(), SSD_L2B_E[i], NUM_STRIP, i);
    readfile.AddChain(mychain, L3ATag.c_str(), SSD_L3A_E[i], NUM_CSI,   i);
  }

  //____________________________________________________________________________
  //                       逐一读取事件进行分析
  unsigned long nentries = mychain->GetEntries();
  cout<< "Found " << nentries <<" entries"<<endl;
  for (unsigned long jentry=0; jentry<nentries; jentry++)
  {
    mychain->GetEntry(jentry);

   for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
   {
     for (Int_t np=0; np<NPoints; np++)
     {
       L1SMulti[ssdindex][np] = hitpattern.LayerMultiplicity(ssdindex, L1STag.c_str(), SSD_L1S_E[ssdindex], L1SEChCut[np]);
       L2FMulti[ssdindex][np] = hitpattern.LayerMultiplicity(ssdindex, L2FTag.c_str(), SSD_L2F_E[ssdindex], L2FEChCut[np]);
       L2BMulti[ssdindex][np] = hitpattern.LayerMultiplicity(ssdindex, L2BTag.c_str(), SSD_L2B_E[ssdindex], L2BEChCut[np]);
       L3AMulti[ssdindex][np] = hitpattern.LayerMultiplicity(ssdindex, L3ATag.c_str(), SSD_L3A_E[ssdindex], L3AEChCut[np]);

       AddNumOfCount(L1SMulti[ssdindex][np], L1SNumOfEmptyEntries[ssdindex][np], L1SNumOfEffEntries[ssdindex][np], L1SNumOfEntriesWithMultiCut[ssdindex][np]);
       AddNumOfCount(L2FMulti[ssdindex][np], L2FNumOfEmptyEntries[ssdindex][np], L2FNumOfEffEntries[ssdindex][np], L2FNumOfEntriesWithMultiCut[ssdindex][np]);
       AddNumOfCount(L2BMulti[ssdindex][np], L2BNumOfEmptyEntries[ssdindex][np], L2BNumOfEffEntries[ssdindex][np], L2BNumOfEntriesWithMultiCut[ssdindex][np]);
       AddNumOfCount(L3AMulti[ssdindex][np], L3ANumOfEmptyEntries[ssdindex][np], L3ANumOfEffEntries[ssdindex][np], L3ANumOfEntriesWithMultiCut[ssdindex][np]);
     }
   }
   time.PrintPercentageAndRemainingTime(jentry, nentries);
  }

  //____________________________________________________________________________
  // 计算 MultiRatio
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    for (Int_t np=0; np<NPoints; np++)
    {
      L1SMultiRatio[ssdindex][np] = EventRatioWithMultiCut(ssdindex,L1STag.c_str(),L1SNumOfEffEntries[ssdindex][np],
                                      L1SNumOfEntriesWithMultiCut[ssdindex][np]);
      L2FMultiRatio[ssdindex][np] = EventRatioWithMultiCut(ssdindex,L2FTag.c_str(),L2FNumOfEffEntries[ssdindex][np],
                                      L2FNumOfEntriesWithMultiCut[ssdindex][np]);
      L2BMultiRatio[ssdindex][np] = EventRatioWithMultiCut(ssdindex,L2BTag.c_str(),L2BNumOfEffEntries[ssdindex][np],
                                      L2BNumOfEntriesWithMultiCut[ssdindex][np]);
      L3AMultiRatio[ssdindex][np] = EventRatioWithMultiCut(ssdindex,L3ATag.c_str(),L3ANumOfEffEntries[ssdindex][np],
                                      L3ANumOfEntriesWithMultiCut[ssdindex][np]);
    }
  }

  //____________________________________________________________________________
  // TGraph 画图
  TLegend* legend_L1S = new TLegend(0.6, 0.1, 0.9, 0.3);
  TLegend* legend_L2F = new TLegend(0.6, 0.1, 0.9, 0.3);
  TLegend* legend_L2B = new TLegend(0.6, 0.1, 0.9, 0.3);
  TLegend* legend_L3A = new TLegend(0.6, 0.1, 0.9, 0.3);

  TMultiGraph* mg_L1S = new TMultiGraph();
  TMultiGraph* mg_L2F = new TMultiGraph();
  TMultiGraph* mg_L2B = new TMultiGraph();
  TMultiGraph* mg_L3A = new TMultiGraph();

  TGraph* gr_L1SMultiRatio[NUM_SSD];
  TGraph* gr_L2FMultiRatio[NUM_SSD];
  TGraph* gr_L2BMultiRatio[NUM_SSD];
  TGraph* gr_L3AMultiRatio[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    gr_L1SMultiRatio[ssdindex] = new TGraph(NPoints, SigmaIndex, L1SMultiRatio[ssdindex]);
    gr_L2FMultiRatio[ssdindex] = new TGraph(NPoints, SigmaIndex, L2FMultiRatio[ssdindex]);
    gr_L2BMultiRatio[ssdindex] = new TGraph(NPoints, SigmaIndex, L2BMultiRatio[ssdindex]);
    gr_L3AMultiRatio[ssdindex] = new TGraph(NPoints, SigmaIndex, L3AMultiRatio[ssdindex]);

    gr_L1SMultiRatio[ssdindex]->SetMarkerStyle(Maker[ssdindex]);
    gr_L2FMultiRatio[ssdindex]->SetMarkerStyle(Maker[ssdindex]);
    gr_L2BMultiRatio[ssdindex]->SetMarkerStyle(Maker[ssdindex]);
    gr_L3AMultiRatio[ssdindex]->SetMarkerStyle(Maker[ssdindex]);

    gr_L1SMultiRatio[ssdindex]->SetMarkerSize(MakerSize);
    gr_L2FMultiRatio[ssdindex]->SetMarkerSize(MakerSize);
    gr_L2BMultiRatio[ssdindex]->SetMarkerSize(MakerSize);
    gr_L3AMultiRatio[ssdindex]->SetMarkerSize(MakerSize);

    gr_L1SMultiRatio[ssdindex]->SetMarkerColor(Color[ssdindex]);
    gr_L2FMultiRatio[ssdindex]->SetMarkerColor(Color[ssdindex]);
    gr_L2BMultiRatio[ssdindex]->SetMarkerColor(Color[ssdindex]);
    gr_L3AMultiRatio[ssdindex]->SetMarkerColor(Color[ssdindex]);

    mg_L1S->Add(gr_L1SMultiRatio[ssdindex]);
    mg_L2F->Add(gr_L2FMultiRatio[ssdindex]);
    mg_L2B->Add(gr_L2BMultiRatio[ssdindex]);
    mg_L3A->Add(gr_L3AMultiRatio[ssdindex]);

    legend_L1S->AddEntry(gr_L1SMultiRatio[ssdindex],Form("L1S_SSD%d", ssdindex+1));
    legend_L2F->AddEntry(gr_L2FMultiRatio[ssdindex],Form("L2F_SSD%d", ssdindex+1));
    legend_L2B->AddEntry(gr_L2BMultiRatio[ssdindex],Form("L2B_SSD%d", ssdindex+1));
    legend_L3A->AddEntry(gr_L3AMultiRatio[ssdindex],Form("L3A_SSD%d", ssdindex+1));
  }

  // 画图
  TCanvas* cans_multiratio = new TCanvas("cans_multi", "cans_multi", 1200, 1200);
  cans_multiratio->Divide(2,2);

  cans_multiratio->cd(1);
  gPad->SetGridx();
  mg_L1S->SetTitle(Form("L1S_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_L1S->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_L1S->GetXaxis()->SetTitleSize(0.04);
  mg_L1S->GetXaxis()->CenterTitle(1);
  mg_L1S->GetXaxis()->SetRangeUser(0,30.);
  mg_L1S->GetXaxis()->SetNdivisions(510);
  mg_L1S->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_L1S->GetYaxis()->SetTitleSize(0.04);
  mg_L1S->GetYaxis()->CenterTitle(1);
  mg_L1S->GetYaxis()->SetRangeUser(0,1.1);
  mg_L1S->GetYaxis()->SetNdivisions(1010);
  mg_L1S->Draw("APL");
  legend_L1S->Draw("SAME");

  cans_multiratio->cd(2);
  gPad->SetGridx();
  mg_L2F->SetTitle(Form("L2F_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_L2F->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_L2F->GetXaxis()->SetTitleSize(0.04);
  mg_L2F->GetXaxis()->CenterTitle(1);
  mg_L2F->GetXaxis()->SetRangeUser(0,30.);
  mg_L2F->GetXaxis()->SetNdivisions(510);
  mg_L2F->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_L2F->GetYaxis()->SetTitleSize(0.04);
  mg_L2F->GetYaxis()->CenterTitle(1);
  mg_L2F->GetYaxis()->SetRangeUser(0,1.1);
  mg_L2F->GetYaxis()->SetNdivisions(1010);
  mg_L2F->Draw("APL");
  legend_L2F->Draw("SAME");

  cans_multiratio->cd(3);
  gPad->SetGridx();
  mg_L2B->SetTitle(Form("L2B_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_L2B->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_L2B->GetXaxis()->SetTitleSize(0.04);
  mg_L2B->GetXaxis()->CenterTitle(1);
  mg_L2B->GetXaxis()->SetRangeUser(0,30.);
  mg_L2B->GetXaxis()->SetNdivisions(510);
  mg_L2B->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_L2B->GetYaxis()->SetTitleSize(0.04);
  mg_L2B->GetYaxis()->CenterTitle(1);
  mg_L2B->GetYaxis()->SetRangeUser(0,1.1);
  mg_L2B->GetYaxis()->SetNdivisions(1010);
  mg_L2B->Draw("APL");
  legend_L2B->Draw("SAME");

  cans_multiratio->cd(4);
  gPad->SetGridx();
  mg_L3A->SetTitle(Form("L3A_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_L3A->GetXaxis()->SetTitle(Form("%.1f + N #times %.1f", ECSICHCUT, CSIPEDESTALSIGMA));
  mg_L3A->GetXaxis()->SetTitleSize(0.04);
  mg_L3A->GetXaxis()->CenterTitle(1);
  mg_L3A->GetXaxis()->SetRangeUser(0,30.);
  mg_L3A->GetXaxis()->SetNdivisions(510);
  mg_L3A->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_L3A->GetYaxis()->SetTitleSize(0.04);
  mg_L3A->GetYaxis()->CenterTitle(1);
  mg_L3A->GetYaxis()->SetRangeUser(0,1.1);
  mg_L3A->GetYaxis()->SetNdivisions(1010);
  mg_L3A->Draw("APL");
  legend_L3A->Draw("SAME");

  time.GetEndTime();
  time.GetRunTime();
}


//______________________________________________________________________________
void AddNumOfCount(Int_t multi, Int_t& emptyentries, Int_t& effentries, Int_t& entrieswithmulticut)
{
  if (multi == 0) {          // 空的事件数
    emptyentries++;
  } else {
    effentries++;            // 有效事件数 (multi >= 1)
    if (multi <= MultiCut) {
      entrieswithmulticut++; // 1 <= 多重数 <= multi 的事件数
    }
  }
}


//______________________________________________________________________________
Double_t EventRatioWithMultiCut(Int_t ssdindex, const char* layertag, Int_t effentries,
  Int_t entriswithmulticut)
{
  Double_t ratio;
  if (effentries == 0) {
    cout<<Form("Error: Multiplicity of SSD%d_%s is 0!",ssdindex+1, layertag)<<endl;
  } else {
    ratio = (Double_t) entriswithmulticut/effentries;
  }
  return ratio;
}
