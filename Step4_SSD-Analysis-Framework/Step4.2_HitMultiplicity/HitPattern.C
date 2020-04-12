
////////////////////////////////////////////////////////////                                                                          //
//    Date: 2020-04-05
//    Author: gfh
////////////////////////////////////////////////////////////
#include "iostream"
#include "string.h"
#include "fstream"
#include "vector"

using namespace std;

//_________________________________
Int_t FirstRun = 300;
Int_t LastRun  = 350;

Int_t Nbins   = 18;
Double_t Xmin = 0;
Double_t Xmax = 18;
Double_t Ymin = 0;
Double_t Ymax = 18;

Int_t CanvasWidth  = 1400;
Int_t CanvasHeight = 1000;

Int_t SSDNum = 4;
Int_t CHNum  = 16;
Int_t ParNum = 2;  // par0 = mean， pea1 = sigma
Int_t SigmaNum = 10;

Int_t HitMultiplicityCut = 2;

//______________________________________________________________________________
Double_t*** ReadData(const Char_t* datapath, Int_t& ssdnum, Int_t& chnum, Int_t& parnum);
void DeleteData(Double_t*** p, Int_t& ssdnum, Int_t& chnum, Int_t& parnum);
void AddChain(TChain* chain, const char* layertag, Int_t* dataarray, Int_t size, Int_t index);
void HitPattern_L1(TChain* chain, const char* L1STag);
void HitPattern_L2(TChain* chain, const char* L2FTag, const char* L2BTag);

//___________________________________________________
void HitPattern()
{
  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string pathFolderRootInput("/home/sea/Fission2019_Data/MapRoot/");

  auto* myChain = new TChain("KrPb");
  for (Int_t i=FirstRun; i<LastRun; i++)
  {
    myChain->Add(Form("%sMapFission2019_Kr_Pb%04d.root", pathFolderRootInput.c_str(),i));
    printf("MapFission2019_Kr_Pb%04d.root loaded!\n", i);
  }
  myChain->SetBranchStatus("*",false);

//  HitPattern_L1(myChain, L1STag.c_str());
  HitPattern_L2(myChain, L2FTag.c_str(), L2BTag.c_str());

}


//______________________________________________________________________________
void HitPattern_L1(TChain* chain, const char* L1STag)
{
  Int_t ECh_SSD_L1S[SSDNum][CHNum];
  Int_t HitMultiplicity_SSD_L1S[SSDNum];
  Double_t PedestalMean_L1S[SSDNum][CHNum];
  Double_t PedestalSigma_L1S[SSDNum][CHNum];

  TH1D* hist_HitPattern[SSDNum];
  TCanvas* Cans_L1S = new TCanvas("Cans_L1S","Cans_L1S",CanvasWidth,CanvasHeight);

  std::string histname_HitPattern[SSDNum];
  std::string pathPedestalCut(Form("../Step4.1_EnergyCalibration/output/SSD_%s_PulserPedestals.dat", L1STag));
  std::string pathFigureHitPattern(Form("figures/SSD_%s_HitPattern_Run%04d-%04d.png",L1STag,FirstRun,LastRun));

  Double_t*** PedestalCut = ReadData(pathPedestalCut.c_str(),SSDNum,CHNum,ParNum);

  for (Int_t i=0; i<SSDNum; i++)
  {
    AddChain(chain, L1STag, ECh_SSD_L1S[i], CHNum, i);
    histname_HitPattern[i]  = Form("SSD%d_%s_HitPattern_Run%04d-%04d", i+1, L1STag, FirstRun, LastRun);
    hist_HitPattern[i]      = new TH1D(histname_HitPattern[i].c_str(), histname_HitPattern[i].c_str(), Nbins, Xmin, Xmax);
    for (Int_t j=0; j<CHNum; j++)
    {
      PedestalMean_L1S[i][j]  = PedestalCut[i][j][0];
      PedestalSigma_L1S[i][j] = PedestalCut[i][j][1];
    }
  }

  unsigned long nentries = chain->GetEntries();
  cout<< "Found " << nentries <<" entries\n";
  for (unsigned long entry=0; entry<nentries; entry++)
  {
    chain->GetEntry(entry);
    for (Int_t i=0; i<SSDNum; i++)
    {
      HitMultiplicity_SSD_L1S[i] = 0;
      for (Int_t j=0; j<CHNum; j++)
      {
        if (ECh_SSD_L1S[i][j]>(PedestalMean_L1S[i][j]+PedestalSigma_L1S[i][j]*SigmaNum)) {
          HitMultiplicity_SSD_L1S[i]++;
        }
      }
    }
    for (Int_t i=0; i<SSDNum; i++)
    {
      if (HitMultiplicity_SSD_L1S[i]<HitMultiplicityCut) {
        for (Int_t j=0; j<CHNum; j++)
        {
          if (ECh_SSD_L1S[i][j]>(PedestalMean_L1S[i][j]+PedestalSigma_L1S[i][j]*SigmaNum)) {
            hist_HitPattern[i]->Fill(j+1, 1.0);
          }
        }
      }
    }
  }
  // Draw
  Cans_L1S->Divide(2,2);
  for (Int_t i=0; i<SSDNum; i++)
  {
    Cans_L1S->cd(i+1);
    gPad->SetGridx(1);
    hist_HitPattern[i]->GetXaxis()->SetNdivisions(118);
    hist_HitPattern[i]->GetXaxis()->SetTitle("ChIndex");
    hist_HitPattern[i]->SetLineColor(kRed);
    hist_HitPattern[i]->SetLineWidth(2);
    hist_HitPattern[i]->Draw();
  }
  Cans_L1S->Print(pathFigureHitPattern.c_str());

  DeleteData(PedestalCut,SSDNum,CHNum, ParNum);

  cout<<Form("HitPattern for L1S done!") <<endl;
}


//______________________________________________________________________________
void HitPattern_L2(TChain* chain, const char* L2FTag, const char* L2BTag)
{
  Int_t ECh_SSD_L2F[SSDNum][CHNum];
  Int_t ECh_SSD_L2B[SSDNum][CHNum];
  Int_t HitMultiplicity_SSD_L2F[SSDNum];
  Int_t HitMultiplicity_SSD_L2B[SSDNum];
  Double_t PedestalMean_L2F[SSDNum][CHNum];
  Double_t PedestalMean_L2B[SSDNum][CHNum];
  Double_t PedestalSigma_L2F[SSDNum][CHNum];
  Double_t PedestalSigma_L2B[SSDNum][CHNum];
  Double_t PedestalCut_L2F[SSDNum][CHNum];
  Double_t PedestalCut_L2B[SSDNum][CHNum];

  TH2D* hist2D_HitPattern[SSDNum];
  TH1D* hist1D_HitPattern_L2F[SSDNum];
  TH1D* hist1D_HitPattern_L2B[SSDNum];
  TCanvas* Cans_2D = new TCanvas("Cans_2D","Cans_2D",CanvasWidth,CanvasHeight);
  TCanvas* Cans_1D[SSDNum];

  std::string histname2D[SSDNum];
  std::string histname1D_L2F[SSDNum];
  std::string histname1D_L2B[SSDNum];
  std::string pathPedestal_L2F(Form("../Step4.1_EnergyCalibration/output/SSD_%s_PulserPedestals.dat", L2FTag));
  std::string pathPedestal_L2B(Form("../Step4.1_EnergyCalibration/output/SSD_%s_PulserPedestals.dat", L2BTag));

  std::string pathFigureHitPattern(Form("L2_HitPattern_Run%04d-%04d.png",FirstRun,LastRun));

  Double_t*** Pedestal_L2F = ReadData(pathPedestal_L2F.c_str(),SSDNum,CHNum,ParNum);
  Double_t*** Pedestal_L2B = ReadData(pathPedestal_L2B.c_str(),SSDNum,CHNum,ParNum);

  for (Int_t i=0; i<SSDNum; i++)
  {
    AddChain(chain, L2FTag, ECh_SSD_L2F[i], CHNum, i);
    AddChain(chain, L2BTag, ECh_SSD_L2B[i], CHNum, i);

    histname1D_L2F[i] = Form("SSD%d_%s_HitPattern_Run%04d-%04d", i+1, L2FTag, FirstRun, LastRun);
    histname1D_L2B[i] = Form("SSD%d_%s_HitPattern_Run%04d-%04d", i+1, L2BTag, FirstRun, LastRun);
    histname2D[i] = Form("SSD%d_L2_HitPattern_Run%04d-%04d", i+1, FirstRun, LastRun);
    hist1D_HitPattern_L2F[i] = new TH1D(histname1D_L2F[i].c_str(), histname1D_L2F[i].c_str(), Nbins, Xmin, Xmax);
    hist1D_HitPattern_L2B[i] = new TH1D(histname1D_L2B[i].c_str(), histname1D_L2B[i].c_str(), Nbins, Xmin, Xmax);
    hist2D_HitPattern[i] = new TH2D(histname2D[i].c_str(),histname2D[i].c_str(), Nbins, Xmin, Xmax, Nbins, Ymin, Ymax);

    Cans_1D[i] = new TCanvas(Form("SSD%d_Cans_1D",i+1),Form("SSD%d_Cans_1D",i+1),CanvasWidth,CanvasHeight);

    for (Int_t j=0; j<CHNum; j++)
    {
      PedestalMean_L2F[i][j]  = Pedestal_L2F[i][j][0];
      PedestalSigma_L2F[i][j] = Pedestal_L2F[i][j][1];
      PedestalMean_L2B[i][j]  = Pedestal_L2B[i][j][0];
      PedestalSigma_L2B[i][j] = Pedestal_L2B[i][j][1];
      PedestalCut_L2F[i][j] = PedestalMean_L2F[i][j] + PedestalSigma_L2F[i][j] * SigmaNum;
      PedestalCut_L2B[i][j] = PedestalMean_L2B[i][j] + PedestalSigma_L2B[i][j] * SigmaNum;
    }
  }

  unsigned long nentries = chain->GetEntries();
  cout<< "Found " << nentries <<" entries\n";
  for (unsigned long entry=0; entry<nentries; entry++)
  {
    chain->GetEntry(entry);
    for (Int_t i=0; i<SSDNum; i++)
    {
      HitMultiplicity_SSD_L2F[i] = 0;
      HitMultiplicity_SSD_L2B[i] = 0;

      //  Get hit multiplicity
      for (Int_t j=0; j<CHNum; j++)
      {
        if (ECh_SSD_L2F[i][j]>PedestalCut_L2F[i][j]) {
          HitMultiplicity_SSD_L2F[i]++;
        }
        if (ECh_SSD_L2B[i][j]>PedestalCut_L2B[i][j]) {
          HitMultiplicity_SSD_L2B[i]++;
        }
      }
      // Fill the hit pattern with hit multiplicity
      // 1D hit pattern for SSD_L2B
      for (Int_t i=0; i<SSDNum; i++)
      {
        if (HitMultiplicity_SSD_L2F[i]<HitMultiplicityCut) {
          for (Int_t j=0; j<CHNum; j++)
          {
            if (ECh_SSD_L2F[i][j]>PedestalCut_L2F[i][j]) {
              hist1D_HitPattern_L2F[i]->Fill(j+1, 1.0);
            }
          }
        }  // 1D hit pattern for SSD_L2B
        if (HitMultiplicity_SSD_L2B[i]<HitMultiplicityCut) {
          for (Int_t j=0; j<CHNum; j++)
          {
            if (ECh_SSD_L2B[i][j]>PedestalCut_L2B[i][j]) {
              hist1D_HitPattern_L2B[i]->Fill(j+1, 1.0);
            }
          }
        }  // 2D hit pattern for SSD_L2
        if ((HitMultiplicity_SSD_L2F[i]<HitMultiplicityCut)||(HitMultiplicity_SSD_L2B[i]<HitMultiplicityCut)) {
          for (Int_t j=0; j<CHNum; j++)
          {
            if ((ECh_SSD_L2F[i][j]>PedestalCut_L2F[i][j])) {
              for (Int_t k=0; k<CHNum; k++)
              {
                if ((ECh_SSD_L2B[i][k]>PedestalCut_L2B[i][k])) {
                  hist2D_HitPattern[i]->Fill(j+1, k+1, 1.0);
                }
              }
            }
          }
        }
      }
    }
  }

  // Draw
  Cans_2D->Divide(2,2);
  for (Int_t i=0; i<SSDNum; i++)
  {
    Cans_2D->cd(i+1);
    hist2D_HitPattern[i]->GetXaxis()->SetNdivisions(118);
    hist2D_HitPattern[i]->GetXaxis()->SetTitle("FrontStrip");
    hist2D_HitPattern[i]->GetYaxis()->SetNdivisions(118);
    hist2D_HitPattern[i]->GetYaxis()->SetTitle("BackStrip");
    hist2D_HitPattern[i]->Draw("colz");
    gPad->Modified();
    gPad->Update();

    hist1D_HitPattern_L2F[i]->GetXaxis()->SetNdivisions(118);
    hist1D_HitPattern_L2F[i]->GetXaxis()->SetTitle("FrontStrip");
    hist1D_HitPattern_L2F[i]->SetLineColor(kGreen);
    hist1D_HitPattern_L2F[i]->SetLineWidth(2);
    hist1D_HitPattern_L2B[i]->GetXaxis()->SetNdivisions(118);
    hist1D_HitPattern_L2B[i]->GetXaxis()->SetTitle("BackStrip");
    hist1D_HitPattern_L2B[i]->SetLineColor(kRed);
    hist1D_HitPattern_L2B[i]->SetLineWidth(2);

    Cans_1D[i]->Divide(2,2);
    Cans_1D[i]->cd(1);
    hist2D_HitPattern[i]->Draw("colz");
    Cans_1D[i]->cd(2);
    gPad->SetGridx(1);
    hist1D_HitPattern_L2B[i]->Draw();
    Cans_1D[i]->cd(3);
    gPad->SetGridx(1);
    hist1D_HitPattern_L2F[i]->Draw();

    Cans_1D[i]->Print(Form("figures/SSD%d_%s",i+1,pathFigureHitPattern.c_str()));
  }
  Cans_2D->Print(Form("figures/SSD_%s",pathFigureHitPattern.c_str()));


  DeleteData(Pedestal_L2F,SSDNum,CHNum, ParNum);
  DeleteData(Pedestal_L2B,SSDNum,CHNum, ParNum);

  cout<<Form("HitPattern for L2 done!") <<endl;
}


//______________________________________________________________________________
void AddChain(TChain* chain, const char* L1STag, Int_t* dataarray, Int_t size, Int_t index)
{
  for (Int_t j=0; j<CHNum; j++)
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
