
////////////////////////////////////////////////////////////                                                                          //
//    Date: 2020-04-05
//    Author: gfh
////////////////////////////////////////////////////////////
#include "iostream"
#include "string.h"
#include "fstream"

using namespace std;

Int_t FirstRun = 300;
Int_t LastRun  = 310;

Int_t Nbins = 17;
Double_t Xmin = -0.5;
Double_t Xmax = 16.5;

Int_t CanvasWidth  = 1200;
Int_t CanvasHeight = 800;

Int_t SSDNum = 4;
Int_t CHNum = 16;
Int_t ParNum = 2;  // par0 = mean， pea1 = sigma

Int_t SigmaNumNum = 6;  // analysing 6 different sigmas
Int_t SigmaNum[6] = {3,5,10,20,30,40};
Int_t color[6] = {kRed,kBlack,kBlue,kMagenta,kCyan,kYellow};


//_______________________________________________________________________________
Double_t*** ReadData(const Char_t* datapath, Int_t& ssdnum, Int_t& chnum, Int_t& parnum);
void DeleteData(Double_t*** p, Int_t& ssdnum, Int_t& chnum, Int_t& parnum);
void HitMulti_Layer(const char* LayerTag);
void HitMulti_CombineL2FL2B(const char* L2FTag, const char* L2BTag);



//______________________________________________________________________________
void HitMultiplicityChangingSigma()
{
  gStyle->SetOptStat(0);

  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");

  HitMulti_Layer(L1STag.c_str());
  HitMulti_Layer(L2FTag.c_str());
  HitMulti_Layer(L2BTag.c_str());
//  HitMulti_CombineL2FL2B(L2FTag.c_str(), L2BTag.c_str());
}


//___________________________________________________
void HitMulti_Layer(const char* LayerTag)
{

  std::string AlphaFileTagL1("AlphaCali00_04");
  std::string AlphaFileTagL2("AlphaCali00_32");
  std::string L1STag("L1S");

  std::string pathFolderRootInput("/home/sea/Fission2019_Data/MapRoot/");
  std::string pathPedestalCut;
  if (strcmp(L1STag.c_str(),LayerTag)==0) {
    pathPedestalCut = Form("../Step4.1_EnergyCalibration/output/SSD_%s_AlphaCaliPedestals_%s.dat",
                                     LayerTag, AlphaFileTagL1.c_str());
  } else {
    pathPedestalCut = Form("../Step4.1_EnergyCalibration/output/SSD_%s_AlphaCaliPedestals_%s.dat",
                          LayerTag, AlphaFileTagL2.c_str());
  }
  std::string pathFiguresMultiOutput(Form("figures/HitMultiplicity/SSD_%s_HitMulti_Run%04d-%04d_ChangingSigma.png",
                                         LayerTag,FirstRun,LastRun));

  Int_t SSD_E_HitMultiplicity[SSDNum][SigmaNumNum];
  Int_t SSD_E[SSDNum][CHNum];
  Double_t PedestalMean[SSDNum][CHNum];
  Double_t PedestalSigma[SSDNum][CHNum];

  std::string SSD_E_bname[SSDNum];
  std::string SSD_E_HitMulti_histname[SSDNum][SigmaNumNum];

  TH1D* hist_HitMultiplicity[SSDNum][SigmaNumNum];
  TLegend* legend[SSDNum];
  TCanvas* cans = new TCanvas(Form("c_%s",LayerTag),Form("c_%s",LayerTag),CanvasWidth,CanvasHeight);
  cans->Divide(2,2);

  Double_t*** PedestalCut = ReadData(pathPedestalCut.c_str(),SSDNum,CHNum,ParNum);

  auto* myChain = new TChain("KrPb");
  for (Int_t i=FirstRun; i<LastRun; i++)
  {
    myChain->Add(Form("%sMapFission2019_Kr_Pb%04d.root", pathFolderRootInput.c_str(),i));
    printf("MapFission2019_Kr_Pb%04d.root loaded!\n", i);
  }
  myChain->SetBranchStatus("*",false);

  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      PedestalMean[i][j]  = PedestalCut[i][j][0];
      PedestalSigma[i][j] = PedestalCut[i][j][1];
      SSD_E[i][j] = 0;
      legend[i] = new TLegend(0.77,0.6,0.9,0.9);
    }
  }

  for (Int_t i=0; i<SSDNum; i++)
  {
    SSD_E_bname[i] = Form("SSD%d_%s_E", i+1, LayerTag);
    myChain->SetBranchStatus(SSD_E_bname[i].c_str(), true);
    myChain->SetBranchAddress(SSD_E_bname[i].c_str(), SSD_E[i]);

    for (Int_t num=0; num<SigmaNumNum; num++)
    {
      SSD_E_HitMulti_histname[i][num] = Form("SSD%d_%s_HitMulti_Run%04d-%04d_Sigma%d",i+1,LayerTag,FirstRun,LastRun,num);
      hist_HitMultiplicity[i][num] = new TH1D(SSD_E_HitMulti_histname[i][num].c_str(),"",
                                    Nbins,Xmin,Xmax);
    }
  }

  unsigned long nentries = myChain->GetEntries();
  cout<< "Found " << nentries <<" entries\n";
  for (unsigned long entry=0; entry<nentries; entry++)
  {
    myChain->GetEntry(entry);

    for (Int_t num=0; num<SigmaNumNum; num++)
    {
      for (Int_t i=0; i<SSDNum; i++)
      {
        for (Int_t num=0; num<SigmaNumNum; num++)
        {
          SSD_E_HitMultiplicity[i][num] = 0;
          for (Int_t j=0; j<CHNum; j++)
          {
            if (SSD_E[i][j]>(PedestalMean[i][j]+PedestalSigma[i][j]*SigmaNum[num]))
            {
              SSD_E_HitMultiplicity[i][num]++;
            }
          }
          hist_HitMultiplicity[i][num]->Fill(SSD_E_HitMultiplicity[i][num], 1.0);
        }
      }
    }
  }

  // Draw
  for (Int_t i=0; i<SSDNum; i++)
  {
    cans->cd(i+1);
    gPad->SetGridx(1);

    hist_HitMultiplicity[i][0]->SetTitle(Form("SSD%d_%s_HitMulti_Run%04d-%04d",i+1,LayerTag,FirstRun,LastRun));
    hist_HitMultiplicity[i][0]->GetXaxis()->SetNdivisions(118);
    hist_HitMultiplicity[i][0]->GetXaxis()->SetTitle("Hit_Multiplicity");
    hist_HitMultiplicity[i][0]->GetYaxis()->SetRangeUser(0,(hist_HitMultiplicity[i][0]->GetMaximum())*1.5);
    hist_HitMultiplicity[i][0]->SetLineColor(color[0]);
    hist_HitMultiplicity[i][0]->SetLineWidth(2);
    hist_HitMultiplicity[i][0]->Draw();

    hist_HitMultiplicity[i][1]->SetLineColor(color[1]);
    hist_HitMultiplicity[i][2]->SetLineColor(color[2]);
    hist_HitMultiplicity[i][3]->SetLineColor(color[3]);
    hist_HitMultiplicity[i][4]->SetLineColor(color[4]);
    hist_HitMultiplicity[i][5]->SetLineColor(color[5]);

    hist_HitMultiplicity[i][1]->SetLineWidth(2);
    hist_HitMultiplicity[i][2]->SetLineWidth(2);
    hist_HitMultiplicity[i][3]->SetLineWidth(2);
    hist_HitMultiplicity[i][4]->SetLineWidth(2);
    hist_HitMultiplicity[i][5]->SetLineWidth(2);

    for (Int_t num=0; num<SigmaNumNum; num++)
    {
      legend[i]->AddEntry(hist_HitMultiplicity[i][num],Form("%d#sigma",SigmaNum[num]),"L");
    }

    hist_HitMultiplicity[i][1]->Draw("SAME");
    hist_HitMultiplicity[i][2]->Draw("SAME");
    hist_HitMultiplicity[i][3]->Draw("SAME");
    hist_HitMultiplicity[i][4]->Draw("SAME");
    hist_HitMultiplicity[i][5]->Draw("SAME");
    legend[i]->Draw("SAME");

  }
  cans->Print(pathFiguresMultiOutput.c_str());

  DeleteData(PedestalCut,SSDNum, CHNum, ParNum);
}


//______________________________________________________________________________
void HitMulti_CombineL2FL2B(const char* L2FTag, const char* L2BTag)
{
  gStyle->SetOptStat(0);

  std::string AlphaFileTag("AlphaCali00_32");

  std::string pathFolderRootInput("/home/sea/Fission2019_Data/MapRoot/");
  std::string pathPedestalCut_L2F(Form("../Step4.1_EnergyCalibration/output/SSD_%s_AlphaCaliPedestals_%s.dat",
                                  L2FTag, AlphaFileTag.c_str()));
  std::string pathPedestalCut_L2B(Form("../Step4.1_EnergyCalibration/output/SSD_%s_AlphaCaliPedestals_%s.dat",
                                  L2BTag, AlphaFileTag.c_str()));

  std::string pathFiguresHitMulti_L2(Form("figures/HitMultiplicity/SSD_L2_HitMulti_Run%04d-%04d.png",FirstRun,LastRun));

  Int_t SSD_L2F_HitMultiplicity[SSDNum];
  Int_t SSD_L2B_HitMultiplicity[SSDNum];
  Int_t SSD_L2F[SSDNum][CHNum];
  Int_t SSD_L2B[SSDNum][CHNum];
  Double_t PedestalMean_L2F[SSDNum][CHNum];
  Double_t PedestalMean_L2B[SSDNum][CHNum];
  Double_t PedestalSigma_L2F[SSDNum][CHNum];
  Double_t PedestalSigma_L2B[SSDNum][CHNum];

  std::string SSD_L2F_bname[SSDNum];
  std::string SSD_L2B_bname[SSDNum];
  std::string SSD_L2F_HitMulti_histname[SSDNum];
  std::string SSD_L2B_HitMulti_histname[SSDNum];

  TH1D* hist_HitMultiplicity_L2F[SSDNum];
  TH1D* hist_HitMultiplicity_L2B[SSDNum];
  TLegend* legend[SSDNum];
  TCanvas* cans = new TCanvas("c_L2FL2B","c_L2FL2B",CanvasWidth,CanvasHeight);
  cans->Divide(2,2);

  Double_t*** PedestalCut_L2F = ReadData(pathPedestalCut_L2F.c_str(),SSDNum,CHNum,ParNum);
  Double_t*** PedestalCut_L2B = ReadData(pathPedestalCut_L2B.c_str(),SSDNum,CHNum,ParNum);

  auto* myChain = new TChain("KrPb");
  for (Int_t i=FirstRun; i<LastRun; i++)
  {
    myChain->Add(Form("%sMapFission2019_Kr_Pb%04d.root", pathFolderRootInput.c_str(),i));
    printf("MapFission2019_Kr_Pb%04d.root loaded!\n", i);
  }
  myChain->SetBranchStatus("*",false);

  for (Int_t i=0; i<SSDNum; i++)
  {
    for (Int_t j=0; j<CHNum; j++)
    {
      PedestalMean_L2F[i][j]  = PedestalCut_L2F[i][j][0];
      PedestalMean_L2B[i][j]  = PedestalCut_L2B[i][j][0];
      PedestalSigma_L2F[i][j] = PedestalCut_L2F[i][j][1];
      PedestalSigma_L2B[i][j] = PedestalCut_L2B[i][j][1];
      SSD_L2F[i][j] = 0;  // 初始化
      SSD_L2B[i][j] = 0;  // 初始化
      legend[i] = new TLegend(0.6,0.7,0.9,0.9);
    }
  }

  for (Int_t i=0; i<SSDNum; i++)
  {
    SSD_L2F_bname[i] = Form("SSD%d_%s_E", i+1, L2FTag);
    SSD_L2B_bname[i] = Form("SSD%d_%s_E", i+1, L2BTag);
    myChain->SetBranchStatus(SSD_L2F_bname[i].c_str(), true);
    myChain->SetBranchStatus(SSD_L2B_bname[i].c_str(), true);
    myChain->SetBranchAddress(SSD_L2F_bname[i].c_str(), SSD_L2F[i]);
    myChain->SetBranchAddress(SSD_L2B_bname[i].c_str(), SSD_L2B[i]);

    SSD_L2F_HitMulti_histname[i] = Form("C_SSD%d_%s_HitMulti_Run%04d-%04d",i+1,L2FTag,FirstRun,LastRun);
    SSD_L2B_HitMulti_histname[i] = Form("C_SSD%d_%s_HitMulti_Run%04d-%04d",i+1,L2BTag,FirstRun,LastRun);
    hist_HitMultiplicity_L2F[i] = new TH1D(SSD_L2F_HitMulti_histname[i].c_str(),SSD_L2F_HitMulti_histname[i].c_str(),
                                  Nbins,Xmin,Xmax);
    hist_HitMultiplicity_L2B[i] = new TH1D(SSD_L2B_HitMulti_histname[i].c_str(),SSD_L2B_HitMulti_histname[i].c_str(),
                                  Nbins,Xmin,Xmax);
  }

  unsigned long nentries = myChain->GetEntries();
  cout<< "Found " << nentries <<" entries\n";
  for (unsigned long entry=0; entry<nentries; entry++)
  {
    myChain->GetEntry(entry);
    for (Int_t i=0; i<SSDNum; i++)
    {
      SSD_L2F_HitMultiplicity[i] = 0;
      SSD_L2B_HitMultiplicity[i] = 0;
      for (Int_t j=0; j<CHNum; j++)
      {
        if (SSD_L2F[i][j]>(PedestalMean_L2F[i][j]+PedestalSigma_L2F[i][j]*SigmaNum[1]))
        {
          SSD_L2F_HitMultiplicity[i]++;
        }
        if (SSD_L2B[i][j]>(PedestalMean_L2B[i][j]+PedestalSigma_L2B[i][j]*SigmaNum[1]))
        {
          SSD_L2B_HitMultiplicity[i]++;
        }
      }
      hist_HitMultiplicity_L2F[i]->Fill(SSD_L2F_HitMultiplicity[i], 1.0);
      hist_HitMultiplicity_L2B[i]->Fill(SSD_L2B_HitMultiplicity[i], 1.0);
    }
  }

  // Draw
  for (Int_t i=0; i<SSDNum; i++)
  {
    cans->cd(i+1);
    gPad->SetGridx(1);
    hist_HitMultiplicity_L2F[i]->GetXaxis()->SetNdivisions(118);
    hist_HitMultiplicity_L2F[i]->GetXaxis()->SetTitle("Hit_Multiplicity");
    hist_HitMultiplicity_L2F[i]->SetLineColor(kGreen);
    hist_HitMultiplicity_L2F[i]->SetLineWidth(2);
    Double_t Yrange = (hist_HitMultiplicity_L2F[i]->GetMaximum())*1.1;
    hist_HitMultiplicity_L2F[i]->GetYaxis()->SetRangeUser(0,Yrange);

    hist_HitMultiplicity_L2B[i]->SetLineColor(kRed);
    hist_HitMultiplicity_L2B[i]->SetLineWidth(2);

    legend[i]->AddEntry(hist_HitMultiplicity_L2F[i],SSD_L2F_bname[i].c_str(),"l");
    legend[i]->AddEntry(hist_HitMultiplicity_L2B[i],SSD_L2B_bname[i].c_str(),"l");

    hist_HitMultiplicity_L2F[i]->Draw();
    hist_HitMultiplicity_L2B[i]->Draw("SAME");
    legend[i]->Draw("SAME");
  }
  cans->Print(pathFiguresHitMulti_L2.c_str());

  DeleteData(PedestalCut_L2F,SSDNum, CHNum, ParNum);
  DeleteData(PedestalCut_L2B,SSDNum, CHNum, ParNum);
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
