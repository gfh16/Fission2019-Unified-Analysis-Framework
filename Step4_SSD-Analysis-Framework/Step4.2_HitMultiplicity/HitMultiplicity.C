
////////////////////////////////////////////////////////////                                                                          //
//    Date: 2020-04-05
//    Author: gfh
////////////////////////////////////////////////////////////
#include "iostream"
#include "string.h"
#include "fstream"
#include "vector"

using namespace std;

Int_t FirstRun = 350;
Int_t LastRun = 351;

Int_t Nbins = 17;
Double_t Xmin = -1;
Double_t Xmax = 16;

Int_t CanvasWidth  = 1200;
Int_t CanvasHeight = 800;

Int_t SSDNum = 4;
Int_t CHNum = 16;
Int_t ParNum = 2;  // par0 = mean， pea1 = sigma
Int_t SigmaNum = 5;

//_______________________________________________________________________________
Double_t*** ReadData(const Char_t* datapath, Int_t& ssdnum, Int_t& chnum, Int_t& parnum);
void DeleteData(Double_t*** p, Int_t& ssdnum, Int_t& chnum, Int_t& parnum);
void HitMulti_Layer(const char* LayerTag);


//______________________________________________________________________________
void HitMultiplicity()
{
  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");

  HitMulti_Layer(L1STag.c_str());
  HitMulti_Layer(L2FTag.c_str());
  HitMulti_Layer(L2BTag.c_str());
}


//___________________________________________________
void HitMulti_Layer(const char* LayerTag)
{

  std::string pathFolderRootInput("/home/sea/Fission2019_Data/MapRoot/");
  std::string pathPedestalCut(Form("../Step4.1_EnergyCalibration/output/SSD_%s_PulserPedestals.dat",
                                   LayerTag));

  Int_t SSD_E_HitMultiplicity[SSDNum];
  Int_t SSD_E[SSDNum][CHNum];
  Double_t PedestalMean[SSDNum][CHNum];
  Double_t PedestalSigma[SSDNum][CHNum];

  std::string SSD_E_bname[SSDNum];
  std::string SSD_E_HitMulti_histname[SSDNum];

  Double_t*** PedestalCut = ReadData(pathPedestalCut.c_str(),SSDNum,CHNum,ParNum);

  auto* myChain = new TChain("KrPb");
  TH1D* hist_HitMultiplicity[SSDNum];
  TCanvas* cans = new TCanvas(Form("c_%s",LayerTag),Form("c_%s",LayerTag),CanvasWidth,CanvasHeight);
  cans->Divide(2,2);

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
    }
  }

  for (Int_t i=0; i<SSDNum; i++)
  {
    SSD_E_bname[i] = Form("SSD%d_%s_E", i+1, LayerTag);
    myChain->SetBranchStatus(SSD_E_bname[i].c_str(), true);
    myChain->SetBranchAddress(SSD_E_bname[i].c_str(), SSD_E[i]);

    SSD_E_HitMulti_histname[i] = Form("SSD%d_%s_HitMulti_Run%04d-%04d",i+1,LayerTag,FirstRun,LastRun);
    hist_HitMultiplicity[i] = new TH1D(SSD_E_HitMulti_histname[i].c_str(),SSD_E_HitMulti_histname[i].c_str(),
                                  Nbins,Xmin,Xmax);
  }

  unsigned long nentries = myChain->GetEntries();
  cout<< "Found " << nentries <<" entries\n";
  for (unsigned long entry=0; entry<nentries; entry++)
  {
    myChain->GetEntry(entry);
    for (Int_t i=0; i<SSDNum; i++)
    {
      SSD_E_HitMultiplicity[i] = 0;
      for (Int_t j=0; j<CHNum; j++)
      {
      //  cout<<SSD_E[i][j]<<"    "<<PedestalMean[i][j]+PedestalSigma[i][j]*SigmaNum<<endl;
        if (SSD_E[i][j]>(PedestalMean[i][j]+PedestalSigma[i][j]*SigmaNum))
        {
          SSD_E_HitMultiplicity[i]++;
        }
      }
      hist_HitMultiplicity[i]->Fill(SSD_E_HitMultiplicity[i], 1.0);
    }
  }

  // Draw
  for (Int_t i=0; i<SSDNum; i++)
  {
    cans->cd(i+1);
    hist_HitMultiplicity[i]->Draw();
  }

  DeleteData(PedestalCut,SSDNum, CHNum, ParNum);
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
