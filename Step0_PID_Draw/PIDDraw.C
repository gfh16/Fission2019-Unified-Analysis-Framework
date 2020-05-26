//_________________________
// definition of constants
Int_t FirstRun = 200;
Int_t LastRun  = 400;
Int_t SSDNum   = 4;
Int_t CHNum    = 16;
Int_t CsINum   = 9;
Int_t ParNum   = 2;
Int_t SigmaNum = 5;
Int_t NBinsX   = 4096;
Int_t NBinsY   = 4096;
Int_t HitMultiCut = 1;

Int_t L1S_CHIndex = 4;
Int_t L2F_CHIndex = 12;
Int_t L2B_CHIndex = 4;
Int_t L3A_CHIndex = 6;

Float_t small = 1e-5;
Double_t L3NoiseCut = 180.;

//______________________________________________________________________________
Double_t*** ReadData(const Char_t* datapath, Int_t& ssdnum, Int_t& chnum, Int_t& parnum);
void DeleteData(Double_t*** p, Int_t& ssdnum, Int_t& chnum, Int_t& parnum);
void AddChain(TChain *chain, const char *LayerTag, Int_t *dataarray, Int_t size, Int_t index);

//____________________________
void PIDDraw()
{
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);

  Int_t SSD_L1S_HitMulti[SSDNum];
  Int_t SSD_L2F_HitMulti[SSDNum];
  Int_t SSD_L2B_HitMulti[SSDNum];
  Int_t SSD_L1S_E[SSDNum][CHNum];
  Int_t SSD_L2F_E[SSDNum][CHNum];
  Int_t SSD_L2B_E[SSDNum][CHNum];
  Int_t SSD_L3A_E[SSDNum][CsINum];

  Int_t EnrtyNum_L2F_L3[SSDNum];
  Int_t EnrtyNum_L2B_L3[SSDNum];

  Double_t SSD_L1SCut[SSDNum][CHNum];
  Double_t SSD_L2FCut[SSDNum][CHNum];
  Double_t SSD_L2BCut[SSDNum][CHNum];


  TH2D *PID_L1_L2F[SSDNum];
  TH2D *PID_L1_L2B[SSDNum];
  TH2D *PID_L2F_L3[SSDNum];
  TH2D *PID_L2B_L3[SSDNum];

  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string L3ATag("L3A");

  ofstream FileOut_L2F_L3[SSDNum];
  ofstream FileOut_L2B_L3[SSDNum];
  std::string pathOut_L2F_L3[SSDNum];
  std::string pathOut_L2B_L3[SSDNum];
  for (Int_t i=0; i<SSDNum; i++)
  {
    EnrtyNum_L2F_L3[i] = 0;
    EnrtyNum_L2B_L3[i] = 0;

    pathOut_L2F_L3[i] = Form("output/SSD%d_L2FCH%02d_L3CH%02d.txt",i+1,L2F_CHIndex,L3A_CHIndex);
    pathOut_L2B_L3[i] = Form("output/SSD%d_L2BCH%02d_L3CH%02d.txt",i+1,L2B_CHIndex,L3A_CHIndex);
    FileOut_L2F_L3[i].open(pathOut_L2F_L3[i].c_str());
    FileOut_L2B_L3[i].open(pathOut_L2B_L3[i].c_str());

    FileOut_L2F_L3[i]<<"* EntryNumber"<<setw(25)<<"E_{CsI} (ADC Channel)"<<setw(25)<<"dE_{F} (ADC Channel))\n";
    FileOut_L2B_L3[i]<<"* EntryNumber"<<setw(25)<<"E_{CsI} (ADC Channel)"<<setw(25)<<"dE_{B} (ADC Channel))\n";
  }

  std::string pathOut_Cans_PID_L2F_L3(Form("output/SSD_L2FCH%02d_L3CH%02d.png",L2F_CHIndex,L3A_CHIndex));
  std::string pathOut_Cans_PID_L2B_L3(Form("output/SSD_L2BCH%02d_L3CH%02d.png",L2B_CHIndex,L3A_CHIndex));

  std::string pathFolder("/home/sea/Fission2019-Unified-Analysis-Framework"
                         "/Step4_SSD-Analysis-Framework/Step4.1_EnergyCalibration/output/");
  std::string pathSSD_L1SPedestal(Form("%sSSD_L1S_AlphaCaliPedestals_AlphaCali00_04.dat",pathFolder.c_str()));
  std::string pathSSD_L2FPedestal(Form("%sSSD_L2F_AlphaCaliPedestals_AlphaCali00_32.dat",pathFolder.c_str()));
  std::string pathSSD_L2BPedestal(Form("%sSSD_L2B_AlphaCaliPedestals_AlphaCali00_32.dat",pathFolder.c_str()));

  Double_t*** SSD_L1SPedestal = ReadData(pathSSD_L1SPedestal.c_str(),SSDNum,CHNum,ParNum);
  Double_t*** SSD_L2FPedestal = ReadData(pathSSD_L2FPedestal.c_str(),SSDNum,CHNum,ParNum);
  Double_t*** SSD_L2BPedestal = ReadData(pathSSD_L2BPedestal.c_str(),SSDNum,CHNum,ParNum);

  TChain *myData = new TChain("KrPb");

  for(int i=FirstRun; i<=LastRun; i++)
  {
    myData->Add(Form("/home/sea/Fission2019_Data/MapRoot/MapFission2019_Kr_Pb%04d.root", i));
  }
  myData->SetBranchStatus("*",false);


  for (Int_t i=0; i<SSDNum; i++)
  {
    PID_L1_L2F[i] = new TH2D(Form("SSD%d_L1_L2F",i+1),Form("PID_L1CH%02d_L2FCH%02d",
                    L1S_CHIndex,L2F_CHIndex),NBinsX,0,NBinsX,NBinsY,0,NBinsY);
    PID_L1_L2B[i] = new TH2D(Form("SSD%d_L1_L2B",i+1),Form("PID_L1CH%02d_L2BCH%02d",
                    L1S_CHIndex,L2B_CHIndex),NBinsX,0,NBinsX,NBinsY,0,NBinsY);
    PID_L2F_L3[i] = new TH2D(Form("SSD%d_L2F_L3",i+1),Form("PID_L2FCH%02d_L3CH%02d",
                    L2F_CHIndex,L3A_CHIndex),NBinsX,0,NBinsX,NBinsY,0,NBinsY);
    PID_L2B_L3[i] = new TH2D(Form("SSD%d_L2B_L3",i+1),Form("PID_L2BCH%02d_L3CH%02d",
                    L2B_CHIndex,L3A_CHIndex),NBinsX,0,NBinsX,NBinsY,0,NBinsY);

    AddChain(myData, L1STag.c_str(), SSD_L1S_E[i], CHNum,  i);
    AddChain(myData, L2FTag.c_str(), SSD_L2F_E[i], CHNum,  i);
    AddChain(myData, L2BTag.c_str(), SSD_L2B_E[i], CHNum,  i);
    AddChain(myData, L3ATag.c_str(), SSD_L3A_E[i], CsINum, i);

    for (Int_t j=0; j<CHNum; j++)
    {
      SSD_L1SCut[i][j] = SSD_L1SPedestal[i][j][0]+SigmaNum*SSD_L1SPedestal[i][j][1];
      SSD_L2FCut[i][j] = SSD_L2FPedestal[i][j][0]+SigmaNum*SSD_L2FPedestal[i][j][1];
      SSD_L2BCut[i][j] = SSD_L2BPedestal[i][j][0]+SigmaNum*SSD_L2BPedestal[i][j][1];
    }
  }

  unsigned long nentries = myData->GetEntries();
  cout<< "Found " << nentries <<" entries\n";
  for (unsigned long j=0; j<nentries; j++)
  {
    myData->GetEntry(j);

    for (Int_t ssdnum=0; ssdnum<SSDNum; ssdnum++)
    {
      SSD_L1S_HitMulti[ssdnum] = 0;
      SSD_L2F_HitMulti[ssdnum] = 0;
      SSD_L2B_HitMulti[ssdnum] = 0;

      for (Int_t chnum=0; chnum<CHNum; chnum++)
      {
        if (SSD_L1S_E[ssdnum][chnum]>SSD_L1SCut[ssdnum][chnum]) {
          SSD_L1S_HitMulti[ssdnum]++;
        }
        if (SSD_L2F_E[ssdnum][chnum]>SSD_L2FCut[ssdnum][chnum]) {
          SSD_L2F_HitMulti[ssdnum]++;
        }
        if (SSD_L2B_E[ssdnum][chnum]>SSD_L2BCut[ssdnum][chnum]) {
          SSD_L2B_HitMulti[ssdnum]++;
        }
      }
    }

    // Fill histograms
    for (Int_t ssdnum=0; ssdnum<SSDNum; ssdnum++)
    {
      // PID_L1_L2F
      if ((SSD_L1S_HitMulti[ssdnum]==HitMultiCut) &&
      (SSD_L2F_HitMulti[ssdnum]==HitMultiCut)) {
        if ((SSD_L1S_E[ssdnum][L1S_CHIndex]>SSD_L1SCut[ssdnum][L1S_CHIndex]) &&
        (SSD_L2F_E[ssdnum][L2F_CHIndex]>SSD_L2FCut[ssdnum][L2F_CHIndex])) {
          PID_L1_L2F[ssdnum]->Fill(SSD_L2F_E[ssdnum][L2F_CHIndex],SSD_L1S_E[ssdnum][L1S_CHIndex]);
        }
      }
      // PID_L1_L2B
      if ((SSD_L1S_HitMulti[ssdnum]==HitMultiCut) &&
      (SSD_L2B_HitMulti[ssdnum]==HitMultiCut)) {
        if ((SSD_L1S_E[ssdnum][L1S_CHIndex]>SSD_L1SCut[ssdnum][L1S_CHIndex]) &&
        (SSD_L2B_E[ssdnum][L2B_CHIndex]>SSD_L2BCut[ssdnum][L2B_CHIndex])) {
          PID_L1_L2B[ssdnum]->Fill(SSD_L2B_E[ssdnum][L2B_CHIndex],SSD_L1S_E[ssdnum][L1S_CHIndex]);
        }
      }
      // PID_L2F_L3
      if (SSD_L2F_HitMulti[ssdnum]==HitMultiCut) {
        if ((SSD_L2F_E[ssdnum][L2F_CHIndex]>SSD_L2FCut[ssdnum][L2F_CHIndex]) &&
        (SSD_L3A_E[ssdnum][L3A_CHIndex]>L3NoiseCut)) {
          PID_L2F_L3[ssdnum]->Fill(SSD_L3A_E[ssdnum][L3A_CHIndex],SSD_L2F_E[ssdnum][L2F_CHIndex]);

          EnrtyNum_L2F_L3[ssdnum]++;
          FileOut_L2F_L3[ssdnum]<<setw(5)<<std::right<<EnrtyNum_L2F_L3[ssdnum]
                                <<setw(20)<<std::right<<SSD_L3A_E[ssdnum][L3A_CHIndex]
                                <<setw(20)<<std::right<<SSD_L2F_E[ssdnum][L2F_CHIndex] <<endl;
        }
      }
      // PID_L2B_L3
      if (SSD_L2B_HitMulti[ssdnum]==HitMultiCut)  {
        if ((SSD_L2B_E[ssdnum][L2B_CHIndex]>SSD_L2BCut[ssdnum][L2B_CHIndex]) &&
        (SSD_L3A_E[ssdnum][L3A_CHIndex]>L3NoiseCut)) {
          PID_L2B_L3[ssdnum]->Fill(SSD_L3A_E[ssdnum][L3A_CHIndex],SSD_L2B_E[ssdnum][L2B_CHIndex]);

          EnrtyNum_L2B_L3[ssdnum]++;
          FileOut_L2B_L3[ssdnum]<<setw(5)<<std::right<<EnrtyNum_L2B_L3[ssdnum]
                                <<setw(20)<<std::right<<SSD_L3A_E[ssdnum][L3A_CHIndex]
                                <<setw(20)<<std::right<<SSD_L2B_E[ssdnum][L2B_CHIndex]<<endl;
        }
      }
    }
  }  // end the loop of entries

  //______________________________________
  TCanvas *Cans_PID_L2F_L3 = new TCanvas("PID_L2F_L3","PID_L2F_L3",1000,1000);
  TCanvas *Cans_PID_L2B_L3 = new TCanvas("PID_L2B_L3","PID_L2B_L3",1000,1000);
  Cans_PID_L2F_L3->Divide(2,2,small,small);
  Cans_PID_L2B_L3->Divide(2,2,small,small);

  TLatex *ssd1 = new TLatex(1700.,3700.,Form("SSD1_L2FCH%02d_L3CH%02d",L2F_CHIndex,L3A_CHIndex));
  TLatex *ssd2 = new TLatex(1700.,3700.,Form("SSD2_L2FCH%02d_L3CH%02d",L2F_CHIndex,L3A_CHIndex));
  TLatex *ssd3 = new TLatex(1700.,3700.,Form("SSD3_L2FCH%02d_L3CH%02d",L2F_CHIndex,L3A_CHIndex));
  TLatex *ssd4 = new TLatex(1700.,3700.,Form("SSD4_L2FCH%02d_L3CH%02d",L2F_CHIndex,L3A_CHIndex));
  TLatex *ssd11 = new TLatex(1700.,3700.,Form("SSD1_L2BCH%02d_L3CH%02d",L2F_CHIndex,L3A_CHIndex));
  TLatex *ssd22 = new TLatex(1700.,3700.,Form("SSD2_L2BCH%02d_L3CH%02d",L2F_CHIndex,L3A_CHIndex));
  TLatex *ssd33 = new TLatex(1700.,3700.,Form("SSD3_L2BCH%02d_L3CH%02d",L2F_CHIndex,L3A_CHIndex));
  TLatex *ssd44 = new TLatex(1700.,3700.,Form("SSD4_L2BCH%02d_L3CH%02d",L2F_CHIndex,L3A_CHIndex));
  ssd1->SetTextColor(kRed);
  ssd2->SetTextColor(kRed);
  ssd3->SetTextColor(kRed);
  ssd4->SetTextColor(kRed);
  ssd1->SetTextSize(0.04);
  ssd2->SetTextSize(0.04);
  ssd3->SetTextSize(0.04);
  ssd4->SetTextSize(0.04);
  ssd11->SetTextColor(kRed);
  ssd22->SetTextColor(kRed);
  ssd33->SetTextColor(kRed);
  ssd44->SetTextColor(kRed);
  ssd11->SetTextSize(0.04);
  ssd22->SetTextSize(0.04);
  ssd33->SetTextSize(0.04);
  ssd44->SetTextSize(0.04);

  Cans_PID_L2F_L3->cd(1);
  gPad->SetBottomMargin(small);
  gPad->SetRightMargin(small);
  gPad->SetLeftMargin(0.18);
  gPad->SetTopMargin(0.18);
  PID_L2F_L3[0]->SetTitle("");
  PID_L2F_L3[0]->GetXaxis()->SetRangeUser(1.,3999.);
  PID_L2F_L3[0]->GetXaxis()->SetNdivisions(1010);
  PID_L2F_L3[0]->GetXaxis()->SetLabelSize(0.05);
  PID_L2F_L3[0]->GetYaxis()->SetRangeUser(1.,4000.);
  PID_L2F_L3[0]->GetYaxis()->SetNdivisions(1008);
  PID_L2F_L3[0]->GetYaxis()->SetLabelSize(0.05);
  PID_L2F_L3[0]->GetYaxis()->SetTitle("dE_{F} (ADC CH)");
  PID_L2F_L3[0]->GetYaxis()->SetTitleSize(0.05);
  PID_L2F_L3[0]->GetYaxis()->CenterTitle(1);
  PID_L2F_L3[0]->GetYaxis()->SetTitleOffset(1.7);
  PID_L2F_L3[0]->Draw("COL");
  ssd1->Draw("SAME");

  Cans_PID_L2F_L3->cd(2);
  gPad->SetBottomMargin(small);
  gPad->SetLeftMargin(small);
  gPad->SetRightMargin(0.18);
  gPad->SetTopMargin(0.18);
  PID_L2F_L3[1]->SetTitle("");
  PID_L2F_L3[1]->GetXaxis()->SetRangeUser(1.,3999.);
  PID_L2F_L3[1]->GetXaxis()->SetNdivisions(1010);
  PID_L2F_L3[1]->GetXaxis()->SetLabelSize(0.05);
  PID_L2F_L3[1]->GetYaxis()->SetRangeUser(1.,4000.);
  PID_L2F_L3[1]->GetYaxis()->SetNdivisions(1008);
  PID_L2F_L3[1]->GetYaxis()->SetLabelSize(0.05);
  PID_L2F_L3[1]->Draw("COL");
  ssd2->Draw("SAME");

  Cans_PID_L2F_L3->cd(3);
  gPad->SetTopMargin(small);
  gPad->SetRightMargin(small);
  gPad->SetLeftMargin(0.18);
  gPad->SetBottomMargin(0.18);
  gPad->SetTickx();
  PID_L2F_L3[2]->SetTitle("");
  PID_L2F_L3[2]->GetXaxis()->SetRangeUser(1.,3999.);
  PID_L2F_L3[2]->GetXaxis()->SetNdivisions(1010);
  PID_L2F_L3[2]->GetXaxis()->SetLabelSize(0.05);
  PID_L2F_L3[2]->GetXaxis()->SetTitle("E_{CsI} (ADC CH)");
  PID_L2F_L3[2]->GetXaxis()->CenterTitle(1);
  PID_L2F_L3[2]->GetXaxis()->SetTitleSize(0.05);
  PID_L2F_L3[2]->GetXaxis()->SetTitleOffset(1.1);
  PID_L2F_L3[2]->GetYaxis()->SetRangeUser(1.,3999.);
  PID_L2F_L3[2]->GetYaxis()->SetNdivisions(1008);
  PID_L2F_L3[2]->GetYaxis()->SetLabelSize(0.05);
  PID_L2F_L3[2]->GetYaxis()->SetTitle("dE_{F} (ADC CH)");
  PID_L2F_L3[2]->GetYaxis()->CenterTitle(1);
  PID_L2F_L3[2]->GetYaxis()->SetTitleOffset(1.7);
  PID_L2F_L3[2]->GetYaxis()->SetTitleSize(0.05);
  PID_L2F_L3[2]->Draw("COL");
  ssd3->Draw("SAME");

  Cans_PID_L2F_L3->cd(4);
  gPad->SetTopMargin(small);
  gPad->SetLeftMargin(small);
  gPad->SetRightMargin(0.18);
  gPad->SetBottomMargin(0.18);
  gPad->SetTickx();
  PID_L2F_L3[3]->SetTitle("");
  PID_L2F_L3[3]->GetXaxis()->SetRangeUser(1.,3999.);
  PID_L2F_L3[3]->GetXaxis()->SetNdivisions(1010);
  PID_L2F_L3[3]->GetXaxis()->SetLabelSize(0.05);
  PID_L2F_L3[3]->GetXaxis()->SetTitle("E_{CsI} (ADC CH)");
  PID_L2F_L3[3]->GetXaxis()->SetTitleSize(0.05);
  PID_L2F_L3[3]->GetXaxis()->CenterTitle(1);
  PID_L2F_L3[3]->GetXaxis()->SetTitleOffset(1.1);
  PID_L2F_L3[3]->GetYaxis()->SetRangeUser(0.,4000.);
  PID_L2F_L3[3]->GetYaxis()->SetNdivisions(1008);
  PID_L2F_L3[3]->GetYaxis()->SetLabelSize(0.05);
  PID_L2F_L3[3]->Draw("COL");
  ssd4->Draw("SAME");


  Cans_PID_L2B_L3->cd(1);
  gPad->SetBottomMargin(small);
  gPad->SetRightMargin(small);
  gPad->SetLeftMargin(0.18);
  gPad->SetTopMargin(0.18);
  PID_L2B_L3[0]->SetTitle("");
  PID_L2B_L3[0]->GetXaxis()->SetRangeUser(1.,3999.);
  PID_L2B_L3[0]->GetXaxis()->SetNdivisions(1010);
  PID_L2B_L3[0]->GetXaxis()->SetLabelSize(0.05);
  PID_L2B_L3[0]->GetYaxis()->SetRangeUser(1.,4000.);
  PID_L2B_L3[0]->GetYaxis()->SetNdivisions(1008);
  PID_L2B_L3[0]->GetYaxis()->SetLabelSize(0.05);
  PID_L2B_L3[0]->GetYaxis()->SetTitle("dE_{B} (ADC CH)");
  PID_L2B_L3[0]->GetYaxis()->SetTitleSize(0.05);
  PID_L2B_L3[0]->GetYaxis()->CenterTitle(1);
  PID_L2B_L3[0]->GetYaxis()->SetTitleOffset(1.7);
  PID_L2B_L3[0]->Draw("COL");
  ssd11->Draw("SAME");

  Cans_PID_L2B_L3->cd(2);
  gPad->SetBottomMargin(small);
  gPad->SetLeftMargin(small);
  gPad->SetRightMargin(0.18);
  gPad->SetTopMargin(0.18);
  PID_L2B_L3[1]->SetTitle("");
  PID_L2B_L3[1]->GetXaxis()->SetRangeUser(1.,3999.);
  PID_L2B_L3[1]->GetXaxis()->SetNdivisions(1010);
  PID_L2B_L3[1]->GetXaxis()->SetLabelSize(0.05);
  PID_L2B_L3[1]->GetYaxis()->SetRangeUser(1.,4000.);
  PID_L2B_L3[1]->GetYaxis()->SetNdivisions(1008);
  PID_L2B_L3[1]->GetYaxis()->SetLabelSize(0.05);
  PID_L2B_L3[1]->Draw("COL");
  ssd22->Draw("SAME");

  Cans_PID_L2B_L3->cd(3);
  gPad->SetTopMargin(small);
  gPad->SetRightMargin(small);
  gPad->SetLeftMargin(0.18);
  gPad->SetBottomMargin(0.18);
  gPad->SetTickx();
  PID_L2B_L3[2]->SetTitle("");
  PID_L2B_L3[2]->GetXaxis()->SetRangeUser(1.,3999.);
  PID_L2B_L3[2]->GetXaxis()->SetNdivisions(1010);
  PID_L2B_L3[2]->GetXaxis()->SetLabelSize(0.05);
  PID_L2B_L3[2]->GetXaxis()->SetTitle("E_{CsI} (ADC CH)");
  PID_L2B_L3[2]->GetXaxis()->CenterTitle(1);
  PID_L2B_L3[2]->GetXaxis()->SetTitleSize(0.05);
  PID_L2B_L3[2]->GetXaxis()->SetTitleOffset(1.1);
  PID_L2B_L3[2]->GetYaxis()->SetRangeUser(1.,3999.);
  PID_L2B_L3[2]->GetYaxis()->SetNdivisions(1008);
  PID_L2B_L3[2]->GetYaxis()->SetLabelSize(0.05);
  PID_L2B_L3[2]->GetYaxis()->SetTitle("dE_{B} (ADC CH)");
  PID_L2B_L3[2]->GetYaxis()->CenterTitle(1);
  PID_L2B_L3[2]->GetYaxis()->SetTitleOffset(1.7);
  PID_L2B_L3[2]->GetYaxis()->SetTitleSize(0.05);
  PID_L2B_L3[2]->Draw("COL");
  ssd33->Draw("SAME");

  Cans_PID_L2B_L3->cd(4);
  gPad->SetTopMargin(small);
  gPad->SetLeftMargin(small);
  gPad->SetRightMargin(0.18);
  gPad->SetBottomMargin(0.18);
  gPad->SetTickx();
  PID_L2B_L3[3]->SetTitle("");
  PID_L2B_L3[3]->GetXaxis()->SetRangeUser(1.,3999.);
  PID_L2B_L3[3]->GetXaxis()->SetNdivisions(1010);
  PID_L2B_L3[3]->GetXaxis()->SetLabelSize(0.05);
  PID_L2B_L3[3]->GetXaxis()->SetTitle("E_{CsI} (ADC CH)");
  PID_L2B_L3[3]->GetXaxis()->SetTitleSize(0.05);
  PID_L2B_L3[3]->GetXaxis()->CenterTitle(1);
  PID_L2B_L3[3]->GetXaxis()->SetTitleOffset(1.1);
  PID_L2B_L3[3]->GetYaxis()->SetRangeUser(0.,4000.);
  PID_L2B_L3[3]->GetYaxis()->SetNdivisions(1008);
  PID_L2B_L3[3]->GetYaxis()->SetLabelSize(0.05);
  PID_L2B_L3[3]->Draw("COL");
  ssd44->Draw("SAME");

  Cans_PID_L2F_L3->Print(pathOut_Cans_PID_L2F_L3.c_str());
  Cans_PID_L2B_L3->Print(pathOut_Cans_PID_L2B_L3.c_str());

}


//______________________________________________________________________________
void AddChain(TChain* chain, const char* L1STag, Int_t* dataarray, Int_t size, Int_t index)
{
  for (Int_t j=0; j<size; j++)
  {
    dataarray[j] = 0;
  }
  std::string SSD_E_bname;
  SSD_E_bname = Form("SSD%d_%s_E", index+1, L1STag);
  chain->SetBranchStatus(SSD_E_bname.c_str(), true);
  chain->SetBranchAddress(SSD_E_bname.c_str(), dataarray);
}


//_______________________________________________
Double_t*** ReadData(const Char_t* datapath, Int_t& ssdnum, Int_t& chnum, Int_t& parnum)
{
///////////////////////////////////////////////////////
//     构建一个函数，返回类型是 三维数组(三级指针)          //
//     注意多级指针的使用！！！                          //
///////////////////////////////////////////////////////
   Double_t*** readpar = NULL;
   readpar = new Double_t** [ssdnum];
   for(Int_t i=0; i<ssdnum; i++)
   {
     readpar[i] = new Double_t* [chnum];
     for(Int_t j=0; j<chnum; j++)
     {
       readpar[i][j]= new Double_t[parnum];
     }
   }
   ifstream in;
   in.open(datapath);
   if(!in.is_open())
   {
     printf("Error: file %s not found\n",datapath);
     return NULL;
   }
   while(in.good())
   {
     // 按行读取数据
     std::string LineRead;
     std::getline(in, LineRead);
     LineRead.assign(LineRead.substr(0, LineRead.find('*')));
     if(LineRead.empty()) continue;
     if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
     std::istringstream LineStream(LineRead);

     Int_t ssd_id;
     Int_t ch_id;

     LineStream >> ssd_id >> ch_id;
     for(Int_t i=0; i<parnum; i++)
     {
       LineStream >>readpar[ssd_id][ch_id][i];
     }
   }
   in.close();
   return readpar;
}

//______________________________________________________________________________
void DeleteData(Double_t*** p, Int_t& ssdnum, Int_t& chnum, Int_t& parnum)
{
/////////////////////////////////////////////////////////////
//    释放前面用 new方法给 Double_t*** p 动态分配的内存         //
//    多级指针需要逐层释放内存！！！                            //
/////////////////////////////////////////////////////////////
  for(Int_t i=0; i<ssdnum; i++)
  {
    for(Int_t j=0; j<chnum; j++)
    {
      delete [] p[i][j];
    }
  }
  for(Int_t i=0; i<ssdnum; i++)
  {
    delete [] p[i];
  }
  delete [] p;
}
