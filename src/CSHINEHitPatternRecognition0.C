#include "../include/CSHINEHitPatternRecognition.h"
using namespace std;

////////////////////////////////////////////////////////////////////////////////
//
//  此程序用于硅条望远镜的模式识别. 大致可以分为以下几部分:
//   1.SSD 各层的多重性
//   2.确定粒子入射位置的约束条件
//   3.L2F 与 L2B 能量关联
//   4.确定每个 SSD 的多重性
//   5.确定 SSD 阵列的多重性
//
//   a)添加函数 EstimateLayerMulti(Int_t fisrtrun, Int_t lastrun);
//
//
//  gfh, 2020-09-06
////////////////////////////////////////////////////////////////////////////////


//******************************************************************************
CSHINEHitPatternRecognition::CSHINEHitPatternRecognition()
{
  //cout<<"Enter class CSHINEHitPatternRecognition!"<<endl;
}

//______________________________________________________________________________
CSHINEHitPatternRecognition::~CSHINEHitPatternRecognition()
{
  //cout<<"Exit class CSHINEHitPatternRecognition!"<<endl;
}
//******************************************************************************



//******************************************************************************
//                            1. SSD 各层的多重性
//
// 计算每一层的多重数
Int_t CSHINEHitPatternRecognition::LayerMultiplicity(Int_t ssdindex, const char* layertag,
  Int_t* ech, Double_t* echcut)
{
  Int_t multi = 0;
  Int_t NUM_CH;

  if (strcmp(layertag, "L3A") == 0) {
    NUM_CH = NUM_CSI;     // for CsI
  } else {
    NUM_CH = NUM_STRIP;   // for Si
  }

  for (Int_t i=0; i<NUM_CH; i++) {
    if (ech[i]>echcut[ssdindex*NUM_CH + i]) {
      multi++;
    }
  }
  return multi;
}
//******************************************************************************



//******************************************************************************
//                          2. 确定粒子入射位置的约束条件
//
// 几何判据一: GeoConstraint_L3A_L2B
Bool_t CSHINEHitPatternRecognition::GeoConstraint_L3A_L2B(Int_t csiindex, Int_t stripl2b)
{
  if ((csiindex/3) == 0) {      // csiindex = 0, 1, 2
    return (stripl2b >= 10 && stripl2b <= 15);
  }
  else if ((csiindex/3) == 1) { // csiindex = 3, 4, 5
    return (stripl2b >= 5 && stripl2b <= 10);
  }
  else {                       // csiindex = 6, 7, 8
    return (stripl2b >= 0 && stripl2b <= 5);
  }
}

//______________________________________________________________________________
// 几何判据二: GeoConstraint_L3A_L2F
Bool_t CSHINEHitPatternRecognition::GeoConstraint_L3A_L2F(Int_t csiindex, Int_t stripl2f)
{
  if ((csiindex%3) == 0) {      // csiindex = 0, 3, 6
    return (stripl2f >= 10 && stripl2f <= 15);
  }
  else if ((csiindex%3) == 1) { // csiindex = 1, 4, 7
    return (stripl2f >= 5 && stripl2f <= 10);
  }
  else {                       // csiindex = 2, 5, 8
    return (stripl2f >= 0 && stripl2f <= 5);
  }
}

//______________________________________________________________________________
// 几何判据三: GeoConstraint_L2B_L1S
Bool_t CSHINEHitPatternRecognition::GeoConstraint_L2B_L1S(Int_t stripl2b, Int_t stripl1s)
{
  return (TMath::Abs(stripl2b - stripl1s) <= 1);
}

// 几何判据三: 考虑 L2B 与 L1S 的道址可能相差 0 或 1
Bool_t CSHINEHitPatternRecognition::GeoConstraint_L2B_L1S(Int_t stripl2b, Int_t stripl1s, Int_t deltastrip)
{
  return (TMath::Abs(stripl2b - stripl1s) == deltastrip);
}

//______________________________________________________________________________
// 能量判据四: 考虑 L2B 与 L2F 的能量关联
Bool_t CSHINEHitPatternRecognition::EnergyConstraint_L2B_L2F(Double_t El2b, Double_t El2f, Double_t deltaE)
{
  return (TMath::Abs(El2b - El2f) < deltaE);
}
//******************************************************************************



//******************************************************************************
// 创建函数 EstimateLayerMulti(Int_t fisrtrun, Int_t lastrun)
void AddNumOfCount(Int_t multi, Int_t multicut, Int_t& emptyentries, Int_t& effentries, Int_t& entrieswithmulticut)
{
  if (multi == 0) {          // 空的事件数
    emptyentries++;
  } else {
    effentries++;            // 有效事件数 (multi >= 1)
    if (multi <= multicut) {
      entrieswithmulticut++; // 1 <= 多重数 <= multi 的事件数
    }
  }
}
//______________________________________________________________________________
Double_t EventRatio(Int_t ssdindex, const char* layertag, Int_t entries_large, Int_t entries_small)
{
  Double_t ratio;
  if (entries_large == 0) {
    cout<<Form("Error: Multiplicity of SSD%d_%s is 0!",ssdindex+1, layertag)<<endl;
  } else {
    ratio = (Double_t) entries_small/entries_large;
  }
  return ratio;
}
//______________________________________________________________________________
////////////////////////////////////////////////////////////////////////////////
//
//  考察、确定每一层的粒子多重性,
//  实质上是，确定每一层的 cut 取值，思路如下:
//  1. 对于每一层，考虑 cut = pedestal + N * sigma
//  2. 画出不同 N下, 1 <= multi <= MultiCut 的事件数占有效 multi >= 1 事件数的比率
//  3. 根据比率的分布，确定 N 的取值
//
//  gfh, 2020-09-02
////////////////////////////////////////////////////////////////////////////////
void CSHINEHitPatternRecognition::EstimateLayerMulti(Int_t firstrun, Int_t lastrun)
{
  gStyle->SetPalette(1);

  TimeAndPercentage time;
  time.GetBeginTime();

  const Int_t NPoints    =  15;
  Int_t       MultiCut   =  2;
  Int_t       Color[4]   =  {kRed-4, kMagenta, kViolet+7, kOrange-3};
  Int_t       Marker[4]  =  {20, 21, 22, 33};
  Double_t    MarkerSize =  1.2;
  Double_t    SigmaIndex[NPoints] =  {1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.};

  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string L3ATag("L3A");

  std::string pathRootInputFolder(Form("%sMapRoot/", PATHROOTFILESFOLDER));
  std::string pathLayerMultiPNDOut(Form("%sfigure_HitPatternRecognition/Layer_MultiRatio_Run%03d-Run%03d.png",PATHFIGURESFOLDER,firstrun,lastrun));
  std::string pathSSDMultiPNDOut  (Form("%sfigure_HitPatternRecognition/SSD_MultiRatio_Run%03d-Run%03d.png",  PATHFIGURESFOLDER,firstrun,lastrun));
  std::string pathMultiCheckOut[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    pathMultiCheckOut[i] = Form("%sfigure_HitPatternRecognition/SSD%d_MultiCheck_Run%03d-Run%03d.png",PATHFIGURESFOLDER,i+1,firstrun,lastrun);
  }

  TChain* mychain = new TChain("KrPb");
  for (Int_t i=firstrun; i<(lastrun+1); i++) {
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

  Double_t L1SEffRatio[NUM_SSD][NPoints]               =  {{0}};
  Double_t L2FEffRatio[NUM_SSD][NPoints]               =  {{0}};
  Double_t L2BEffRatio[NUM_SSD][NPoints]               =  {{0}};
  Double_t L3AEffRatio[NUM_SSD][NPoints]               =  {{0}};

  Double_t L1SEmptyRatio[NUM_SSD][NPoints]             =  {{0}};
  Double_t L2FEmptyRatio[NUM_SSD][NPoints]             =  {{0}};
  Double_t L2BEmptyRatio[NUM_SSD][NPoints]             =  {{0}};
  Double_t L3AEmptyRatio[NUM_SSD][NPoints]             =  {{0}};

  Double_t L1SSumRatio[NUM_SSD][NPoints]               =  {{0}};
  Double_t L2FSumRatio[NUM_SSD][NPoints]               =  {{0}};
  Double_t L2BSumRatio[NUM_SSD][NPoints]               =  {{0}};
  Double_t L3ASumRatio[NUM_SSD][NPoints]               =  {{0}};

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
  for (Int_t np=0; np<NPoints; np++) {
    L1SEChCut[np] = calidata[np].GetSiEChCut (L1STag.c_str(), SigmaIndex[np]);
    L2FEChCut[np] = calidata[np].GetSiEChCut (L2FTag.c_str(), SigmaIndex[np]);
    L2BEChCut[np] = calidata[np].GetSiEChCut (L2BTag.c_str(), SigmaIndex[np]);
    L3AEChCut[np] = calidata[np].GetCsIEChCut(L3ATag.c_str(), SigmaIndex[np]);
  }

  // set branchadress
  ReadFileModule readfile;
  for (Int_t i=0; i<NUM_SSD; i++) {
    readfile.AddChain(mychain, L1STag.c_str(), SSD_L1S_E[i], NUM_STRIP, i);
    readfile.AddChain(mychain, L2FTag.c_str(), SSD_L2F_E[i], NUM_STRIP, i);
    readfile.AddChain(mychain, L2BTag.c_str(), SSD_L2B_E[i], NUM_STRIP, i);
    readfile.AddChain(mychain, L3ATag.c_str(), SSD_L3A_E[i], NUM_CSI,   i);
  }

  //____________________________________________________________________________
  //                       逐一读取事件进行分析
  unsigned long nentries = mychain->GetEntries();
  cout<< "Found " << nentries <<" entries"<<endl;
  for (unsigned long jentry=0; jentry<nentries; jentry++) {

    mychain->GetEntry(jentry);

   for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
     for (Int_t np=0; np<NPoints; np++) {
       L1SMulti[ssdindex][np] = hitpattern.LayerMultiplicity(ssdindex, L1STag.c_str(), SSD_L1S_E[ssdindex], L1SEChCut[np]);
       L2FMulti[ssdindex][np] = hitpattern.LayerMultiplicity(ssdindex, L2FTag.c_str(), SSD_L2F_E[ssdindex], L2FEChCut[np]);
       L2BMulti[ssdindex][np] = hitpattern.LayerMultiplicity(ssdindex, L2BTag.c_str(), SSD_L2B_E[ssdindex], L2BEChCut[np]);
       L3AMulti[ssdindex][np] = hitpattern.LayerMultiplicity(ssdindex, L3ATag.c_str(), SSD_L3A_E[ssdindex], L3AEChCut[np]);

       AddNumOfCount(L1SMulti[ssdindex][np], MultiCut, L1SNumOfEmptyEntries[ssdindex][np], L1SNumOfEffEntries[ssdindex][np], L1SNumOfEntriesWithMultiCut[ssdindex][np]);
       AddNumOfCount(L2FMulti[ssdindex][np], MultiCut, L2FNumOfEmptyEntries[ssdindex][np], L2FNumOfEffEntries[ssdindex][np], L2FNumOfEntriesWithMultiCut[ssdindex][np]);
       AddNumOfCount(L2BMulti[ssdindex][np], MultiCut, L2BNumOfEmptyEntries[ssdindex][np], L2BNumOfEffEntries[ssdindex][np], L2BNumOfEntriesWithMultiCut[ssdindex][np]);
       AddNumOfCount(L3AMulti[ssdindex][np], MultiCut, L3ANumOfEmptyEntries[ssdindex][np], L3ANumOfEffEntries[ssdindex][np], L3ANumOfEntriesWithMultiCut[ssdindex][np]);
     }
   }
   time.PrintPercentageAndRemainingTime(jentry, nentries);
  }

  //____________________________________________________________________________
  // 计算 MultiRatio
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    for (Int_t np=0; np<NPoints; np++) {
      // multi ratio
      L1SMultiRatio[ssdindex][np] = EventRatio(ssdindex,L1STag.c_str(),L1SNumOfEffEntries[ssdindex][np],
                                      L1SNumOfEntriesWithMultiCut[ssdindex][np]);
      L2FMultiRatio[ssdindex][np] = EventRatio(ssdindex,L2FTag.c_str(),L2FNumOfEffEntries[ssdindex][np],
                                      L2FNumOfEntriesWithMultiCut[ssdindex][np]);
      L2BMultiRatio[ssdindex][np] = EventRatio(ssdindex,L2BTag.c_str(),L2BNumOfEffEntries[ssdindex][np],
                                      L2BNumOfEntriesWithMultiCut[ssdindex][np]);
      L3AMultiRatio[ssdindex][np] = EventRatio(ssdindex,L3ATag.c_str(),L3ANumOfEffEntries[ssdindex][np],
                                      L3ANumOfEntriesWithMultiCut[ssdindex][np]);
      // effective event ratio
      L1SEffRatio[ssdindex][np]   = EventRatio(ssdindex,L1STag.c_str(),nentries,L1SNumOfEffEntries[ssdindex][np]);
      L2FEffRatio[ssdindex][np]   = EventRatio(ssdindex,L2FTag.c_str(),nentries,L2FNumOfEffEntries[ssdindex][np]);
      L2BEffRatio[ssdindex][np]   = EventRatio(ssdindex,L2BTag.c_str(),nentries,L2BNumOfEffEntries[ssdindex][np]);
      L3AEffRatio[ssdindex][np]   = EventRatio(ssdindex,L3ATag.c_str(),nentries,L3ANumOfEffEntries[ssdindex][np]);

      // empty event ratio
      L1SEmptyRatio[ssdindex][np] = EventRatio(ssdindex,L1STag.c_str(),nentries,L1SNumOfEmptyEntries[ssdindex][np]);
      L2FEmptyRatio[ssdindex][np] = EventRatio(ssdindex,L2FTag.c_str(),nentries,L2FNumOfEmptyEntries[ssdindex][np]);
      L2BEmptyRatio[ssdindex][np] = EventRatio(ssdindex,L2BTag.c_str(),nentries,L2BNumOfEmptyEntries[ssdindex][np]);
      L3AEmptyRatio[ssdindex][np] = EventRatio(ssdindex,L3ATag.c_str(),nentries,L3ANumOfEmptyEntries[ssdindex][np]);

      // sum ratio
      L1SSumRatio[ssdindex][np]   = L1SEffRatio[ssdindex][np] + L1SEmptyRatio[ssdindex][np];
      L2FSumRatio[ssdindex][np]   = L2FEffRatio[ssdindex][np] + L2FEmptyRatio[ssdindex][np];
      L2BSumRatio[ssdindex][np]   = L2BEffRatio[ssdindex][np] + L2BEmptyRatio[ssdindex][np];
      L3ASumRatio[ssdindex][np]   = L3AEffRatio[ssdindex][np] + L3AEmptyRatio[ssdindex][np];
    }
  }

  //____________________________________________________________________________
  // 画图
  TCanvas* cans_mutliratio_layer = new TCanvas("cans_multi_layer", "cans_multi_layer", 1200, 1200);
  TCanvas* cans_mutliratio_ssd   = new TCanvas("cans_multi_ssd",   "cans_multi_ssd",   1200, 1200);
  TCanvas* cans_multiratio_check[NUM_SSD];

  // 画出 ratio = 1 作为基准线
  TLine* line_one = new TLine(1, 1.0, 15., 1.0);
  line_one->SetLineColor(kTeal+2);
  line_one->SetLineWidth(2);
  line_one->SetLineStyle(2);

  TLegend* legend_L1S  =  new TLegend(0.6, 0.12, 0.88, 0.32);
  TLegend* legend_L2F  =  new TLegend(0.6, 0.12, 0.88, 0.32);
  TLegend* legend_L2B  =  new TLegend(0.6, 0.12, 0.88, 0.32);
  TLegend* legend_L3A  =  new TLegend(0.6, 0.12, 0.88, 0.32);
  TLegend* legend_SSD[NUM_SSD];
  TLegend* legend_SSD_MultiCheck[NUM_SSD][NUM_SSD]; // [layer][ssdindex]

  TGraph* gr_L1SMultiRatio[NUM_SSD];
  TGraph* gr_L2FMultiRatio[NUM_SSD];
  TGraph* gr_L2BMultiRatio[NUM_SSD];
  TGraph* gr_L3AMultiRatio[NUM_SSD];

  TGraph* gr_L1SMultiRatio_Clone[NUM_SSD];
  TGraph* gr_L2FMultiRatio_Clone[NUM_SSD];
  TGraph* gr_L2BMultiRatio_Clone[NUM_SSD];
  TGraph* gr_L3AMultiRatio_Clone[NUM_SSD];

  TGraph* gr_L1SMultiRatio_Clone2[NUM_SSD];
  TGraph* gr_L2FMultiRatio_Clone2[NUM_SSD];
  TGraph* gr_L2BMultiRatio_Clone2[NUM_SSD];
  TGraph* gr_L3AMultiRatio_Clone2[NUM_SSD];

  TGraph* gr_L1SEffRatio[NUM_SSD];
  TGraph* gr_L2FEffRatio[NUM_SSD];
  TGraph* gr_L2BEffRatio[NUM_SSD];
  TGraph* gr_L3AEffRatio[NUM_SSD];

  TGraph* gr_L1SEmptyRatio[NUM_SSD];
  TGraph* gr_L2FEmptyRatio[NUM_SSD];
  TGraph* gr_L2BEmptyRatio[NUM_SSD];
  TGraph* gr_L3AEmptyRatio[NUM_SSD];

  TGraph* gr_L1SSumRatio[NUM_SSD];  // SumRatio = EffRatio + EmptyRatio
  TGraph* gr_L2FSumRatio[NUM_SSD];  // SumRatio = EffRatio + EmptyRatio
  TGraph* gr_L2BSumRatio[NUM_SSD];  // SumRatio = EffRatio + EmptyRatio
  TGraph* gr_L3ASumRatio[NUM_SSD];  // SumRatio = EffRatio + EmptyRatio

  TMultiGraph* mg_L1S  =  new TMultiGraph();
  TMultiGraph* mg_L2F  =  new TMultiGraph();
  TMultiGraph* mg_L2B  =  new TMultiGraph();
  TMultiGraph* mg_L3A  =  new TMultiGraph();
  TMultiGraph* mg_SSD[NUM_SSD];
  TMultiGraph* mg_SSD_MultiCheck[NUM_SSD][NUM_SSD];  // [layer][ssdindex]

  for (Int_t i=0; i<NUM_SSD; i++) {
    legend_SSD[i] = new TLegend(0.6, 0.12, 0.88, 0.32);
    mg_SSD    [i] = new TMultiGraph();

    cans_multiratio_check[i] = new TCanvas(Form("SSD%d_multiratio_check", i+1), Form("SSD%d_multiratio_check", i+1), 1200, 1200);

    for (Int_t j=0; j<NUM_SSD; j++){
      mg_SSD_MultiCheck    [i][j] = new TMultiGraph();
      legend_SSD_MultiCheck[i][j] = new TLegend(0.56, 0.10, 0.88, 0.32);
    }
  }

  //____________________________________________________________________________
  //                     将不同 SSD 的同一层画在一起
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    gr_L1SMultiRatio[ssdindex] = new TGraph(NPoints, SigmaIndex, L1SMultiRatio[ssdindex]);
    gr_L2FMultiRatio[ssdindex] = new TGraph(NPoints, SigmaIndex, L2FMultiRatio[ssdindex]);
    gr_L2BMultiRatio[ssdindex] = new TGraph(NPoints, SigmaIndex, L2BMultiRatio[ssdindex]);
    gr_L3AMultiRatio[ssdindex] = new TGraph(NPoints, SigmaIndex, L3AMultiRatio[ssdindex]);

    gr_L1SMultiRatio[ssdindex]->SetMarkerStyle(Marker[ssdindex]);
    gr_L2FMultiRatio[ssdindex]->SetMarkerStyle(Marker[ssdindex]);
    gr_L2BMultiRatio[ssdindex]->SetMarkerStyle(Marker[ssdindex]);
    gr_L3AMultiRatio[ssdindex]->SetMarkerStyle(Marker[ssdindex]);

    gr_L1SMultiRatio[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2FMultiRatio[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2BMultiRatio[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L3AMultiRatio[ssdindex]->SetMarkerSize(MarkerSize);

    gr_L1SMultiRatio[ssdindex]->SetMarkerColor(Color[ssdindex]);
    gr_L2FMultiRatio[ssdindex]->SetMarkerColor(Color[ssdindex]);
    gr_L2BMultiRatio[ssdindex]->SetMarkerColor(Color[ssdindex]);
    gr_L3AMultiRatio[ssdindex]->SetMarkerColor(Color[ssdindex]);

    mg_L1S->Add(gr_L1SMultiRatio[ssdindex]);
    mg_L2F->Add(gr_L2FMultiRatio[ssdindex]);
    mg_L2B->Add(gr_L2BMultiRatio[ssdindex]);
    mg_L3A->Add(gr_L3AMultiRatio[ssdindex]);

    legend_L1S->AddEntry(gr_L1SMultiRatio[ssdindex],Form("SSD%d_L1S", ssdindex+1));
    legend_L2F->AddEntry(gr_L2FMultiRatio[ssdindex],Form("SSD%d_L2F", ssdindex+1));
    legend_L2B->AddEntry(gr_L2BMultiRatio[ssdindex],Form("SSD%d_L2B", ssdindex+1));
    legend_L3A->AddEntry(gr_L3AMultiRatio[ssdindex],Form("SSD%d_L3A", ssdindex+1));
  }

  cans_mutliratio_layer->Divide(2,2);
  cans_mutliratio_layer->cd(1);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_L1S->SetTitle(Form("L1S_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_L1S->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_L1S->GetXaxis()->SetTitleSize(0.04);
  mg_L1S->GetXaxis()->CenterTitle(1);
  mg_L1S->GetXaxis()->SetRangeUser(0,15.);
  mg_L1S->GetXaxis()->SetNdivisions(115);
  mg_L1S->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_L1S->GetYaxis()->SetTitleSize(0.04);
  mg_L1S->GetYaxis()->CenterTitle(1);
  mg_L1S->GetYaxis()->SetRangeUser(0,1.1);
  mg_L1S->GetYaxis()->SetNdivisions(511);
  mg_L1S->Draw("APL");
  legend_L1S->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_layer->cd(2);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_L2F->SetTitle(Form("L2F_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_L2F->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_L2F->GetXaxis()->SetTitleSize(0.04);
  mg_L2F->GetXaxis()->CenterTitle(1);
  mg_L2F->GetXaxis()->SetRangeUser(0,15.);
  mg_L2F->GetXaxis()->SetNdivisions(115);
  mg_L2F->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_L2F->GetYaxis()->SetTitleSize(0.04);
  mg_L2F->GetYaxis()->CenterTitle(1);
  mg_L2F->GetYaxis()->SetRangeUser(0,1.1);
  mg_L2F->GetYaxis()->SetNdivisions(511);
  mg_L2F->Draw("APL");
  legend_L2F->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_layer->cd(3);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_L2B->SetTitle(Form("L2B_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_L2B->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_L2B->GetXaxis()->SetTitleSize(0.04);
  mg_L2B->GetXaxis()->CenterTitle(1);
  mg_L2B->GetXaxis()->SetRangeUser(0,15.);
  mg_L2B->GetXaxis()->SetNdivisions(115);
  mg_L2B->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_L2B->GetYaxis()->SetTitleSize(0.04);
  mg_L2B->GetYaxis()->CenterTitle(1);
  mg_L2B->GetYaxis()->SetRangeUser(0,1.1);
  mg_L2B->GetYaxis()->SetNdivisions(511);
  mg_L2B->Draw("APL");
  legend_L2B->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_layer->cd(4);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_L3A->SetTitle(Form("L3A_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_L3A->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_L3A->GetXaxis()->SetTitleSize(0.04);
  mg_L3A->GetXaxis()->CenterTitle(1);
  mg_L3A->GetXaxis()->SetRangeUser(0,15.);
  mg_L3A->GetXaxis()->SetNdivisions(115);
  mg_L3A->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_L3A->GetYaxis()->SetTitleSize(0.04);
  mg_L3A->GetYaxis()->CenterTitle(1);
  mg_L3A->GetYaxis()->SetRangeUser(0,1.1);
  mg_L3A->GetYaxis()->SetNdivisions(511);
  mg_L3A->Draw("APL");
  legend_L3A->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_layer->Print(pathLayerMultiPNDOut.c_str());

  //____________________________________________________________________________
  //                     将同一个 SSD 的不同层画在一起
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {

    gr_L1SMultiRatio_Clone[ssdindex] = (TGraph*) gr_L1SMultiRatio[ssdindex]->Clone();
    gr_L2FMultiRatio_Clone[ssdindex] = (TGraph*) gr_L2FMultiRatio[ssdindex]->Clone();
    gr_L2BMultiRatio_Clone[ssdindex] = (TGraph*) gr_L2BMultiRatio[ssdindex]->Clone();
    gr_L3AMultiRatio_Clone[ssdindex] = (TGraph*) gr_L3AMultiRatio[ssdindex]->Clone();

    gr_L1SMultiRatio_Clone[ssdindex]->SetMarkerStyle(Marker[0]);
    gr_L2FMultiRatio_Clone[ssdindex]->SetMarkerStyle(Marker[1]);
    gr_L2BMultiRatio_Clone[ssdindex]->SetMarkerStyle(Marker[2]);
    gr_L3AMultiRatio_Clone[ssdindex]->SetMarkerStyle(Marker[3]);

    gr_L1SMultiRatio_Clone[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2FMultiRatio_Clone[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2BMultiRatio_Clone[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L3AMultiRatio_Clone[ssdindex]->SetMarkerSize(MarkerSize);

    gr_L1SMultiRatio_Clone[ssdindex]->SetMarkerColor(Color[0]);
    gr_L2FMultiRatio_Clone[ssdindex]->SetMarkerColor(Color[1]);
    gr_L2BMultiRatio_Clone[ssdindex]->SetMarkerColor(Color[2]);
    gr_L3AMultiRatio_Clone[ssdindex]->SetMarkerColor(Color[3]);

    mg_SSD[ssdindex]->Add(gr_L1SMultiRatio_Clone[ssdindex]);
    mg_SSD[ssdindex]->Add(gr_L2FMultiRatio_Clone[ssdindex]);
    mg_SSD[ssdindex]->Add(gr_L2BMultiRatio_Clone[ssdindex]);
    mg_SSD[ssdindex]->Add(gr_L3AMultiRatio_Clone[ssdindex]);

    legend_SSD[ssdindex]->AddEntry(gr_L1SMultiRatio_Clone[ssdindex],Form("SSD%d_L1S", ssdindex+1));
    legend_SSD[ssdindex]->AddEntry(gr_L2FMultiRatio_Clone[ssdindex],Form("SSD%d_L2F", ssdindex+1));
    legend_SSD[ssdindex]->AddEntry(gr_L2BMultiRatio_Clone[ssdindex],Form("SSD%d_L2B", ssdindex+1));
    legend_SSD[ssdindex]->AddEntry(gr_L3AMultiRatio_Clone[ssdindex],Form("SSD%d_L3A", ssdindex+1));
  }

  cans_mutliratio_ssd->Divide(2,2);
  cans_mutliratio_ssd->cd(1);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_SSD[0]->SetTitle(Form("SSD1_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_SSD[0]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_SSD[0]->GetXaxis()->SetTitleSize(0.04);
  mg_SSD[0]->GetXaxis()->CenterTitle(1);
  mg_SSD[0]->GetXaxis()->SetRangeUser(0,15.);
  mg_SSD[0]->GetXaxis()->SetNdivisions(115);
  mg_SSD[0]->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_SSD[0]->GetYaxis()->SetTitleSize(0.04);
  mg_SSD[0]->GetYaxis()->CenterTitle(1);
  mg_SSD[0]->GetYaxis()->SetRangeUser(0,1.1);
  mg_SSD[0]->GetYaxis()->SetNdivisions(511);
  mg_SSD[0]->Draw("APL");
  legend_SSD[0]->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_ssd->cd(2);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_SSD[1]->SetTitle(Form("SSD2_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_SSD[1]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_SSD[1]->GetXaxis()->SetTitleSize(0.04);
  mg_SSD[1]->GetXaxis()->CenterTitle(1);
  mg_SSD[1]->GetXaxis()->SetRangeUser(0,15.);
  mg_SSD[1]->GetXaxis()->SetNdivisions(115);
  mg_SSD[1]->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_SSD[1]->GetYaxis()->SetTitleSize(0.04);
  mg_SSD[1]->GetYaxis()->CenterTitle(1);
  mg_SSD[1]->GetYaxis()->SetRangeUser(0,1.1);
  mg_SSD[1]->GetYaxis()->SetNdivisions(511);
  mg_SSD[1]->Draw("APL");
  legend_SSD[1]->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_ssd->cd(3);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_SSD[2]->SetTitle(Form("SSD3_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_SSD[2]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_SSD[2]->GetXaxis()->SetTitleSize(0.04);
  mg_SSD[2]->GetXaxis()->CenterTitle(1);
  mg_SSD[2]->GetXaxis()->SetRangeUser(0,15.);
  mg_SSD[2]->GetXaxis()->SetNdivisions(115);
  mg_SSD[2]->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_SSD[2]->GetYaxis()->SetTitleSize(0.04);
  mg_SSD[2]->GetYaxis()->CenterTitle(1);
  mg_SSD[2]->GetYaxis()->SetRangeUser(0,1.1);
  mg_SSD[2]->GetYaxis()->SetNdivisions(511);
  mg_SSD[2]->Draw("APL");
  legend_SSD[2]->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_ssd->cd(4);
  gPad->SetGridx();
  gPad->SetGridy();
  mg_SSD[3]->SetTitle(Form("SSD4_MultiRatio_1 #leq multi #leq %d",MultiCut));
  mg_SSD[3]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
  mg_SSD[3]->GetXaxis()->SetTitleSize(0.04);
  mg_SSD[3]->GetXaxis()->CenterTitle(1);
  mg_SSD[3]->GetXaxis()->SetRangeUser(0,15);
  mg_SSD[3]->GetXaxis()->SetNdivisions(115);
  mg_SSD[3]->GetYaxis()->SetTitle(Form("(1 #leq multi #leq %d)/(multi #geq 1)", MultiCut));
  mg_SSD[3]->GetYaxis()->SetTitleSize(0.04);
  mg_SSD[3]->GetYaxis()->CenterTitle(1);
  mg_SSD[3]->GetYaxis()->SetRangeUser(0,1.1);
  mg_SSD[3]->GetYaxis()->SetNdivisions(511);
  mg_SSD[3]->Draw("APL");
  legend_SSD[3]->Draw("SAME");
  line_one->Draw("SAME");

  cans_mutliratio_ssd->Print(pathSSDMultiPNDOut.c_str());


  //____________________________________________________________________________
  //            将 EffRatio, EmptyRatio 画出来, 检查多重性计算是否准确
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    gr_L1SMultiRatio_Clone2[ssdindex]  =  (TGraph*) gr_L1SMultiRatio[ssdindex]->Clone();
    gr_L2FMultiRatio_Clone2[ssdindex]  =  (TGraph*) gr_L2FMultiRatio[ssdindex]->Clone();
    gr_L2BMultiRatio_Clone2[ssdindex]  =  (TGraph*) gr_L2BMultiRatio[ssdindex]->Clone();
    gr_L3AMultiRatio_Clone2[ssdindex]  =  (TGraph*) gr_L3AMultiRatio[ssdindex]->Clone();

    gr_L1SEffRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L1SEffRatio[ssdindex]);
    gr_L2FEffRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L2FEffRatio[ssdindex]);
    gr_L2BEffRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L2BEffRatio[ssdindex]);
    gr_L3AEffRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L3AEffRatio[ssdindex]);

    gr_L1SEmptyRatio       [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L1SEmptyRatio[ssdindex]);
    gr_L2FEmptyRatio       [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L2FEmptyRatio[ssdindex]);
    gr_L2BEmptyRatio       [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L2BEmptyRatio[ssdindex]);
    gr_L3AEmptyRatio       [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L3AEmptyRatio[ssdindex]);

    gr_L1SSumRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L1SSumRatio[ssdindex]);
    gr_L2FSumRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L2FSumRatio[ssdindex]);
    gr_L2BSumRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L2BSumRatio[ssdindex]);
    gr_L3ASumRatio         [ssdindex]  =  new TGraph(NPoints, SigmaIndex, L3ASumRatio[ssdindex]);

    gr_L1SMultiRatio_Clone2[ssdindex]->SetMarkerStyle(Marker[0]);
    gr_L2FMultiRatio_Clone2[ssdindex]->SetMarkerStyle(Marker[0]);
    gr_L2BMultiRatio_Clone2[ssdindex]->SetMarkerStyle(Marker[0]);
    gr_L3AMultiRatio_Clone2[ssdindex]->SetMarkerStyle(Marker[0]);
    gr_L1SEffRatio         [ssdindex]->SetMarkerStyle(Marker[1]);
    gr_L2FEffRatio         [ssdindex]->SetMarkerStyle(Marker[1]);
    gr_L2BEffRatio         [ssdindex]->SetMarkerStyle(Marker[1]);
    gr_L3AEffRatio         [ssdindex]->SetMarkerStyle(Marker[1]);
    gr_L1SEmptyRatio       [ssdindex]->SetMarkerStyle(Marker[2]);
    gr_L2FEmptyRatio       [ssdindex]->SetMarkerStyle(Marker[2]);
    gr_L2BEmptyRatio       [ssdindex]->SetMarkerStyle(Marker[2]);
    gr_L3AEmptyRatio       [ssdindex]->SetMarkerStyle(Marker[2]);
    gr_L1SSumRatio         [ssdindex]->SetMarkerStyle(Marker[3]);
    gr_L2FSumRatio         [ssdindex]->SetMarkerStyle(Marker[3]);
    gr_L2BSumRatio         [ssdindex]->SetMarkerStyle(Marker[3]);
    gr_L3ASumRatio         [ssdindex]->SetMarkerStyle(Marker[3]);

    gr_L1SMultiRatio_Clone2[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2FMultiRatio_Clone2[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2BMultiRatio_Clone2[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L3AMultiRatio_Clone2[ssdindex]->SetMarkerSize(MarkerSize);
    gr_L1SEffRatio         [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2FEffRatio         [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2BEffRatio         [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L3AEffRatio         [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L1SEmptyRatio       [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2FEmptyRatio       [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2BEmptyRatio       [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L3AEmptyRatio       [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L1SSumRatio         [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2FSumRatio         [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L2BSumRatio         [ssdindex]->SetMarkerSize(MarkerSize);
    gr_L3ASumRatio         [ssdindex]->SetMarkerSize(MarkerSize);

    gr_L1SMultiRatio_Clone2[ssdindex]->SetMarkerColor(Color[0]);
    gr_L2FMultiRatio_Clone2[ssdindex]->SetMarkerColor(Color[0]);
    gr_L2BMultiRatio_Clone2[ssdindex]->SetMarkerColor(Color[0]);
    gr_L3AMultiRatio_Clone2[ssdindex]->SetMarkerColor(Color[0]);
    gr_L1SEffRatio         [ssdindex]->SetMarkerColor(Color[1]);
    gr_L2FEffRatio         [ssdindex]->SetMarkerColor(Color[1]);
    gr_L2BEffRatio         [ssdindex]->SetMarkerColor(Color[1]);
    gr_L3AEffRatio         [ssdindex]->SetMarkerColor(Color[1]);
    gr_L1SEmptyRatio       [ssdindex]->SetMarkerColor(Color[2]);
    gr_L2FEmptyRatio       [ssdindex]->SetMarkerColor(Color[2]);
    gr_L2BEmptyRatio       [ssdindex]->SetMarkerColor(Color[2]);
    gr_L3AEmptyRatio       [ssdindex]->SetMarkerColor(Color[2]);
    gr_L1SSumRatio         [ssdindex]->SetMarkerColor(Color[3]);
    gr_L2FSumRatio         [ssdindex]->SetMarkerColor(Color[3]);
    gr_L2BSumRatio         [ssdindex]->SetMarkerColor(Color[3]);
    gr_L3ASumRatio         [ssdindex]->SetMarkerColor(Color[3]);

    mg_SSD_MultiCheck[0][ssdindex]->Add(gr_L1SMultiRatio_Clone2[ssdindex]);
    mg_SSD_MultiCheck[0][ssdindex]->Add(gr_L1SEffRatio[ssdindex]);
    mg_SSD_MultiCheck[0][ssdindex]->Add(gr_L1SEmptyRatio[ssdindex]);
    mg_SSD_MultiCheck[0][ssdindex]->Add(gr_L1SSumRatio[ssdindex]);
    mg_SSD_MultiCheck[1][ssdindex]->Add(gr_L2FMultiRatio_Clone2[ssdindex]);
    mg_SSD_MultiCheck[1][ssdindex]->Add(gr_L2FEffRatio[ssdindex]);
    mg_SSD_MultiCheck[1][ssdindex]->Add(gr_L2FEmptyRatio[ssdindex]);
    mg_SSD_MultiCheck[1][ssdindex]->Add(gr_L2FSumRatio[ssdindex]);
    mg_SSD_MultiCheck[2][ssdindex]->Add(gr_L2BMultiRatio_Clone2[ssdindex]);
    mg_SSD_MultiCheck[2][ssdindex]->Add(gr_L2BEffRatio[ssdindex]);
    mg_SSD_MultiCheck[2][ssdindex]->Add(gr_L2BEmptyRatio[ssdindex]);
    mg_SSD_MultiCheck[2][ssdindex]->Add(gr_L2BSumRatio[ssdindex]);
    mg_SSD_MultiCheck[3][ssdindex]->Add(gr_L3AMultiRatio_Clone2[ssdindex]);
    mg_SSD_MultiCheck[3][ssdindex]->Add(gr_L3AEffRatio[ssdindex]);
    mg_SSD_MultiCheck[3][ssdindex]->Add(gr_L3AEmptyRatio[ssdindex]);
    mg_SSD_MultiCheck[3][ssdindex]->Add(gr_L3ASumRatio[ssdindex]);

    legend_SSD_MultiCheck[0][ssdindex]->AddEntry(gr_L1SMultiRatio_Clone2[ssdindex], Form("SSD%d_L1S_MultiRatio", ssdindex+1));
    legend_SSD_MultiCheck[0][ssdindex]->AddEntry(gr_L1SEffRatio[ssdindex],          Form("SSD%d_L1S_EffRatio"  , ssdindex+1));
    legend_SSD_MultiCheck[0][ssdindex]->AddEntry(gr_L1SEmptyRatio[ssdindex],        Form("SSD%d_L1S_EmptyRatio", ssdindex+1));
    legend_SSD_MultiCheck[0][ssdindex]->AddEntry(gr_L1SSumRatio[ssdindex],          Form("SSD%d_L1S_SumRatio"  , ssdindex+1));
    legend_SSD_MultiCheck[1][ssdindex]->AddEntry(gr_L2FMultiRatio_Clone2[ssdindex], Form("SSD%d_L2F_MultiRatio", ssdindex+1));
    legend_SSD_MultiCheck[1][ssdindex]->AddEntry(gr_L2FEffRatio[ssdindex],          Form("SSD%d_L2F_EffRatio"  , ssdindex+1));
    legend_SSD_MultiCheck[1][ssdindex]->AddEntry(gr_L2FEmptyRatio[ssdindex],        Form("SSD%d_L2F_EmptyRatio", ssdindex+1));
    legend_SSD_MultiCheck[1][ssdindex]->AddEntry(gr_L2FSumRatio[ssdindex],          Form("SSD%d_L2F_SumRatio"  , ssdindex+1));
    legend_SSD_MultiCheck[2][ssdindex]->AddEntry(gr_L2BMultiRatio_Clone2[ssdindex], Form("SSD%d_L2B_MultiRatio", ssdindex+1));
    legend_SSD_MultiCheck[2][ssdindex]->AddEntry(gr_L2BEffRatio[ssdindex],          Form("SSD%d_L2B_EffRatio"  , ssdindex+1));
    legend_SSD_MultiCheck[2][ssdindex]->AddEntry(gr_L2BEmptyRatio[ssdindex],        Form("SSD%d_L2B_EmptyRatio", ssdindex+1));
    legend_SSD_MultiCheck[2][ssdindex]->AddEntry(gr_L2BSumRatio[ssdindex],          Form("SSD%d_L2B_SumRatio"  , ssdindex+1));
    legend_SSD_MultiCheck[3][ssdindex]->AddEntry(gr_L3AMultiRatio_Clone2[ssdindex], Form("SSD%d_L3A_MultiRatio", ssdindex+1));
    legend_SSD_MultiCheck[3][ssdindex]->AddEntry(gr_L3AEffRatio[ssdindex],          Form("SSD%d_L3A_EffRatio"  , ssdindex+1));
    legend_SSD_MultiCheck[3][ssdindex]->AddEntry(gr_L3AEmptyRatio[ssdindex],        Form("SSD%d_L3A_EmptyRatio", ssdindex+1));
    legend_SSD_MultiCheck[3][ssdindex]->AddEntry(gr_L3ASumRatio[ssdindex],          Form("SSD%d_L3A_SumRatio"  , ssdindex+1));
  }

  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    cans_multiratio_check[ssdindex]->Divide(2,2);

    cans_multiratio_check[ssdindex]->cd(1);
    gPad->SetGridx();
    gPad->SetGridy();
    mg_SSD_MultiCheck[0][ssdindex]->SetTitle(Form("SSD%d_L1S_MultiRatio", ssdindex+1));
    mg_SSD_MultiCheck[0][ssdindex]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
    mg_SSD_MultiCheck[0][ssdindex]->GetXaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[0][ssdindex]->GetXaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[0][ssdindex]->GetXaxis()->SetRangeUser(0,15.);
    mg_SSD_MultiCheck[0][ssdindex]->GetXaxis()->SetNdivisions(115);
    mg_SSD_MultiCheck[0][ssdindex]->GetYaxis()->SetTitle("Ratio");
    mg_SSD_MultiCheck[0][ssdindex]->GetYaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[0][ssdindex]->GetYaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[0][ssdindex]->GetYaxis()->SetRangeUser(0,1.1);
    mg_SSD_MultiCheck[0][ssdindex]->GetYaxis()->SetNdivisions(511);
    mg_SSD_MultiCheck[0][ssdindex]->Draw("APL");
    legend_SSD_MultiCheck[0][ssdindex]->Draw("SAME");
    line_one->Draw("SAME");

    cans_multiratio_check[ssdindex]->cd(2);
    gPad->SetGridx();
    gPad->SetGridy();
    mg_SSD_MultiCheck[1][ssdindex]->SetTitle(Form("SSD%d_L2F_MultiRatio", ssdindex+1));
    mg_SSD_MultiCheck[1][ssdindex]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
    mg_SSD_MultiCheck[1][ssdindex]->GetXaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[1][ssdindex]->GetXaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[1][ssdindex]->GetXaxis()->SetRangeUser(0,15.);
    mg_SSD_MultiCheck[1][ssdindex]->GetXaxis()->SetNdivisions(115);
    mg_SSD_MultiCheck[1][ssdindex]->GetYaxis()->SetTitle("Ratio");
    mg_SSD_MultiCheck[1][ssdindex]->GetYaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[1][ssdindex]->GetYaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[1][ssdindex]->GetYaxis()->SetRangeUser(0,1.1);
    mg_SSD_MultiCheck[1][ssdindex]->GetYaxis()->SetNdivisions(511);
    mg_SSD_MultiCheck[1][ssdindex]->Draw("APL");
    legend_SSD_MultiCheck[1][ssdindex]->Draw("SAME");
    line_one->Draw("SAME");

    cans_multiratio_check[ssdindex]->cd(3);
    gPad->SetGridx();
    gPad->SetGridy();
    mg_SSD_MultiCheck[2][ssdindex]->SetTitle(Form("SSD%d_L2B_MultiRatio", ssdindex+1));
    mg_SSD_MultiCheck[2][ssdindex]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
    mg_SSD_MultiCheck[2][ssdindex]->GetXaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[2][ssdindex]->GetXaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[2][ssdindex]->GetXaxis()->SetRangeUser(0,15.);
    mg_SSD_MultiCheck[2][ssdindex]->GetXaxis()->SetNdivisions(115);
    mg_SSD_MultiCheck[2][ssdindex]->GetYaxis()->SetTitle("Ratio");
    mg_SSD_MultiCheck[2][ssdindex]->GetYaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[2][ssdindex]->GetYaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[2][ssdindex]->GetYaxis()->SetRangeUser(0,1.1);
    mg_SSD_MultiCheck[2][ssdindex]->GetYaxis()->SetNdivisions(511);
    mg_SSD_MultiCheck[2][ssdindex]->Draw("APL");
    legend_SSD_MultiCheck[2][ssdindex]->Draw("SAME");
    line_one->Draw("SAME");

    cans_multiratio_check[ssdindex]->cd(4);
    gPad->SetGridx();
    gPad->SetGridy();
    mg_SSD_MultiCheck[3][ssdindex]->SetTitle(Form("SSD%d_L3A_MultiRatio", ssdindex+1));
    mg_SSD_MultiCheck[3][ssdindex]->GetXaxis()->SetTitle(Form("Pedestal + N #times #sigma"));
    mg_SSD_MultiCheck[3][ssdindex]->GetXaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[3][ssdindex]->GetXaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[3][ssdindex]->GetXaxis()->SetRangeUser(0,15.);
    mg_SSD_MultiCheck[3][ssdindex]->GetXaxis()->SetNdivisions(510);
    mg_SSD_MultiCheck[3][ssdindex]->GetYaxis()->SetTitle("Ratio");
    mg_SSD_MultiCheck[3][ssdindex]->GetYaxis()->SetTitleSize(0.04);
    mg_SSD_MultiCheck[3][ssdindex]->GetYaxis()->CenterTitle(1);
    mg_SSD_MultiCheck[3][ssdindex]->GetYaxis()->SetRangeUser(0,1.1);
    mg_SSD_MultiCheck[3][ssdindex]->GetYaxis()->SetNdivisions(511);
    mg_SSD_MultiCheck[3][ssdindex]->Draw("APL");
    legend_SSD_MultiCheck[3][ssdindex]->Draw("SAME");
    line_one->Draw("SAME");

    cans_multiratio_check[ssdindex]->Print(pathMultiCheckOut[ssdindex].c_str());
  }
  time.GetEndTime();
  time.GetRunTime();
}
//******************************************************************************


//******************************************************************************
void CSHINEHitPatternRecognition::GetPunchThroughEnergiesOfLCPs()
{
  Int_t NUM_LCPs = 16;
  Int_t Z_A_LCPs[16][2] = { {1,1}, {1,2}, {1,3},   // H1,  H2,  H3
                            {2,3}, {2,4}, {2,6},   // He3, He4, He6
                            {3,6}, {3,7}, {3,8},   // Li6, Li7, Li8
                            {4,7}, {4,9}, {4,10},  // Be7, Be9, Be10
                            {5,8}, {5,10},{5,11},  // B8,  B10, B11
                            {6,12}                 // C12
                          };
  std::string SiTag("Si");
  std::string CsITag("CsI");
  const Int_t LiseModel = 1;
  //LiseModel=1 is good for low energy， J.F.Ziegler et al, Pergamon Press, NY (low energy)

  std::string pathPunchThroughOut(Form("%sdata_HitPatternRecognition/PunchThroughEnergies_of_LCPs.dat", PATHDATAFOLDER));
  ofstream FileOut(pathPunchThroughOut.c_str());
  FileOut<<setw(10)<<"*SSDNum"<<setw(20)<<"material"<<setw(15)<<"thickness/um"<<setw(15)<<"Charge(Z)"
         <<setw(15)<<"Mass(A)"<<setw(25)<<"PunchThroughEnergy/MeV"<<endl;

  for (Int_t ssdnum=0; ssdnum<NUM_SSD; ssdnum++) {
    for (Int_t lcp=0; lcp<NUM_LCPs; lcp++) {
      //Double_t PunchThroughL1 = fLISEModule.GetEnergyFromRange(Z_A_LCPs[lcp][0],Z_A_LCPs[lcp][1],SIL1THICKNESS[ssdnum]*1000,"Si",LiseModel);
      Double_t PunchThroughL1 = fLISEModule.GetEnergyFromRange(1,1,100.,"Si",1);

      FileOut<<setw(10)<<ssdnum<<setw(15)<<SiTag.c_str()<<setw(15)<<SIL1THICKNESS[ssdnum]*1000<<setw(15)
             <<Z_A_LCPs[lcp][0]<<setw(15)<<Z_A_LCPs[lcp][1]<<setw(15)<<PunchThroughL1<<endl;
    }
    for (Int_t lcp=0; lcp<NUM_LCPs; lcp++) {
      Double_t PunchThroughL2 = fLISEModule.GetEnergyFromRange(Z_A_LCPs[lcp][0],Z_A_LCPs[lcp][1],SIL2THICKNESS[ssdnum]*1000,SiTag.c_str(),LiseModel);
      FileOut<<setw(10)<<ssdnum<<setw(15)<<SiTag.c_str()<<setw(15)<<SIL2THICKNESS[ssdnum]*1000<<setw(15)
             <<Z_A_LCPs[lcp][0]<<setw(15)<<Z_A_LCPs[lcp][1]<<setw(15)<<PunchThroughL2<<endl;
    }
    for (Int_t lcp=0; lcp<NUM_LCPs; lcp++) {
      Double_t PunchThroughL3 = fLISEModule.GetEnergyFromRange(Z_A_LCPs[lcp][0],Z_A_LCPs[lcp][1],CSITHICKNESS*1000,CsITag.c_str(),LiseModel);
      FileOut<<setw(10)<<ssdnum<<setw(15)<<CsITag.c_str()<<setw(15)<<CSITHICKNESS*1000<<setw(15)
             <<Z_A_LCPs[lcp][0]<<setw(15)<<Z_A_LCPs[lcp][1]<<setw(15)<<PunchThroughL3<<endl;
    }
  }
  FileOut.close();
}

//******************************************************************************
