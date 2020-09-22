
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TSystem.h"
#include "TCanvas.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <map>

//add by wyj (2016-9-21)
#include<sys/stat.h>
#include<sys/types.h>

#include "../include/ReadFileModule.h"
#include "../include/TimeAndPercentage.h"

using namespace std;


void PrintUsage(char *name);
void ReadBranch(TChain* fChain, const char* pathrootout, const char* pathpdfout);


//______________________________________________________________________________
int main(int argc, char *argv[])
{
  TString pathlistfile("/home/sea/Fission2019_Data/MapRoot/alistfilename1");
  std::vector<string>  filenamelist;

  ReadFileModule readfile;
  readfile.GetFileNamesFromFile(pathlistfile, filenamelist);

  //__________________________________________
  //               逐个处理文件
  // !!! 重要 ！！！
  // 此处修改输入、输出文件路径
  for (Int_t i=0; i<filenamelist.size(); i++)
  {
    TString pathrootout = "/home/sea/Fission2019_Data/QualityCheck/";
    pathrootout += "QC_";
    pathrootout += filenamelist[i];

    TString pdf_name = filenamelist[i];
    pdf_name.ReplaceAll(".root", ".pdf");
    TString pathpdfout = "/home/sea/Fission2019_Data/QualityCheck_figures/";
    pathpdfout  += "QC_";
    pathpdfout  += pdf_name;

    TString pathrootin = "/home/sea/Fission2019_Data/MapRoot/";
    pathrootin += filenamelist[i];
    TFile* rootfile = new TFile(pathrootin);
    TChain* myChain = (TChain*)rootfile->Get("KrPb");
    cout<<"正在处理文件  : "<<pathrootin<<endl;

    ReadBranch(myChain, pathrootout, pathpdfout);
  }
  return 0;
}
//______________________________________________________________________________


//______________________________________________________________________________
void ReadBranch(TChain* fChain, const char* pathrootout, const char* pathpdfout)
{
    const Int_t BIN_NUM = 4096;
    const Int_t LOW_CH  = 0;
    const Int_t HIGH_CH = 4096;

    TimeAndPercentage time;

    //_________________________
    //         For PPAC
    Int_t  PPAC_T[3]  = {0};
    Int_t  PPAC_X1[3] = {0};
    Int_t  PPAC_X2[3] = {0};
    Int_t  PPAC_Y1[3] = {0};
    Int_t  PPAC_Y2[3] = {0};
    Int_t  PPAC_T_Energy[3] = {0};
    Char_t T_name  [3][200];
    Char_t X1_name [3][200];
    Char_t X2_name [3][200];
    Char_t Y1_name [3][200];
    Char_t Y2_name [3][200];
    Char_t T_E_name[3][200];
    for(Int_t i=0; i<3; i++) {
      sprintf(T_name[i], "PPAC%d_T", (i+1));
      sprintf(X1_name[i],"PPAC%d_X1",(i+1));
      sprintf(X2_name[i],"PPAC%d_X2",(i+1));
      sprintf(Y1_name[i],"PPAC%d_Y1",(i+1));
      sprintf(Y2_name[i],"PPAC%d_Y2",(i+1));
      sprintf(T_E_name[i],"PPAC%d_T_Energy",(i+1));
    }
    for(Int_t i=0; i<3; i++) {
      fChain->SetBranchAddress(T_name[i],  &PPAC_T[i]);
      fChain->SetBranchAddress(X1_name[i], &PPAC_X1[i]);
      fChain->SetBranchAddress(X2_name[i], &PPAC_X2[i]);
      fChain->SetBranchAddress(Y1_name[i], &PPAC_Y1[i]);
      fChain->SetBranchAddress(Y2_name[i], &PPAC_Y2[i]);
      fChain->SetBranchAddress(T_E_name[i],&PPAC_T_Energy[i]);
    }

    //__________________________________________________________________________
    //                       For Au-Si
    Int_t  AuSi_L1E[2] = {0};
    Int_t  AuSi_L2E[2] = {0};
    Int_t  AuSi_L1T[2] = {0};
    Char_t AuSi_L1E_name[2][200];
    Char_t AuSi_L2E_name[2][200];
    Char_t AuSi_L1T_name[2][200];
    for(Int_t i=0; i<2; i++) {
      sprintf(AuSi_L1E_name[i], "AuSi%d_L1E", (i+1));
      sprintf(AuSi_L2E_name[i], "AuSi%d_L2E", (i+1));
      sprintf(AuSi_L1T_name[i], "AuSi%d_L1T", (i+1));
    }
    for(Int_t i=0; i<2; i++) {
      fChain->SetBranchAddress(AuSi_L1E_name[i], &AuSi_L1E[i]);
      fChain->SetBranchAddress(AuSi_L2E_name[i], &AuSi_L2E[i]);
      fChain->SetBranchAddress(AuSi_L1T_name[i], &AuSi_L1T[i]);
    }

    //__________________________________________________________________________
    //                           For RF
    Int_t  RF1 = 0;
    Int_t  RF2 = 0;
    fChain->SetBranchAddress("RF1", &RF1);
    fChain->SetBranchAddress("RF2", &RF2);

    //__________________________________________________________________________
    //                          For SSD
    Int_t  SSD_L2F_T[4][16] = {{0}};
    Int_t  SSD_L1S_E[4][16] = {{0}};
    Int_t  SSD_L2F_E[4][16] = {{0}};
    Int_t  SSD_L2B_E[4][16] = {{0}};
    Int_t  SSD_L3A_E[4][9]  = {{0}};
    Char_t SSD_L2F_T_name[4][200];
    Char_t SSD_L1S_E_name[4][200];
    Char_t SSD_L2F_E_name[4][200];
    Char_t SSD_L2B_E_name[4][200];
    Char_t SSD_L3A_E_name[4][200];
    for(Int_t i=0; i<4; i++) {
      sprintf(SSD_L2F_T_name[i], "SSD%d_L2F_T", i+1);
      sprintf(SSD_L1S_E_name[i], "SSD%d_L1S_E", i+1);
      sprintf(SSD_L2F_E_name[i], "SSD%d_L2F_E", i+1);
      sprintf(SSD_L2B_E_name[i], "SSD%d_L2B_E", i+1);
      sprintf(SSD_L3A_E_name[i], "SSD%d_L3A_E", i+1);
    }
    for(Int_t i=0; i<4; i++) {
      fChain->SetBranchAddress(SSD_L2F_T_name[i], SSD_L2F_T[i]);
      fChain->SetBranchAddress(SSD_L1S_E_name[i], SSD_L1S_E[i]);
      fChain->SetBranchAddress(SSD_L2F_E_name[i], SSD_L2F_E[i]);
      fChain->SetBranchAddress(SSD_L2B_E_name[i], SSD_L2B_E[i]);
      fChain->SetBranchAddress(SSD_L3A_E_name[i], SSD_L3A_E[i]);
    }

    //__________________________________________________________________________
    //                定义直方图
    //  for  PPAC
    TH1D* Hist_PPAC_T[3];
    TH1D* Hist_PPAC_X1[3];
    TH1D* Hist_PPAC_X2[3];
    TH1D* Hist_PPAC_Y1[3];
    TH1D* Hist_PPAC_Y2[3];
    TH1D* Hist_PPAC_T_E[3];
    for(Int_t i=0;i<3;i++) {
       Hist_PPAC_T  [i]  =  new TH1D(T_name[i],   T_name[i],  BIN_NUM, LOW_CH, HIGH_CH);
       Hist_PPAC_X1 [i]  =  new TH1D(X1_name[i], X1_name[i],  BIN_NUM, LOW_CH, HIGH_CH);
       Hist_PPAC_X2 [i]  =  new TH1D(X2_name[i], X2_name[i],  BIN_NUM, LOW_CH, HIGH_CH);
       Hist_PPAC_Y1 [i]  =  new TH1D(Y1_name[i], Y1_name[i],  BIN_NUM, LOW_CH, HIGH_CH);
       Hist_PPAC_Y2 [i]  =  new TH1D(Y2_name[i], Y2_name[i],  BIN_NUM, LOW_CH, HIGH_CH);
       Hist_PPAC_T_E[i]  =  new TH1D(T_E_name[i],T_E_name[i], BIN_NUM, LOW_CH, HIGH_CH);
    }
    //______________________________
    //          for Au-Si
    TH1D* Hist_AuSi_L1T[2];
    TH1D* Hist_AuSi_L1E[2];
    TH1D* Hist_AuSi_L2E[2];
    for(Int_t i=0; i<2; i++){
      Hist_AuSi_L1T[i] = new TH1D(AuSi_L1T_name[i], AuSi_L1T_name[i], BIN_NUM, LOW_CH, HIGH_CH);
      Hist_AuSi_L1E[i] = new TH1D(AuSi_L1E_name[i], AuSi_L1E_name[i], BIN_NUM, LOW_CH, HIGH_CH);
      Hist_AuSi_L2E[i] = new TH1D(AuSi_L2E_name[i], AuSi_L2E_name[i], BIN_NUM, LOW_CH, HIGH_CH);
    }
    //______________________________
    //           for RF
    TH1D* Hist_RF1 = new TH1D("RF1", "RF1", BIN_NUM, LOW_CH, HIGH_CH);
    TH1D* Hist_RF2 = new TH1D("RF2", "RF2", BIN_NUM, LOW_CH, HIGH_CH);

    //_______________________________
    //           for SSD
    TH1D* Hist_SSD_L2F_T[4][16];
    TH1D* Hist_SSD_L1S_E[4][16];
    TH1D* Hist_SSD_L2F_E[4][16];
    TH1D* Hist_SSD_L2B_E[4][16];
    TH1D* Hist_SSD_L3A_E[4][9];
    Char_t SSD_L2F_T_nameCH[4][16][200];
    Char_t SSD_L1S_E_nameCH[4][16][200];
    Char_t SSD_L2F_E_nameCH[4][16][200];
    Char_t SSD_L2B_E_nameCH[4][16][200];
    Char_t SSD_L3A_E_nameCH[4][9][200];
    for(Int_t i=0; i<4; i++) {
      for(Int_t j=0; j<16; j++) {
        sprintf(SSD_L2F_T_nameCH[i][j], "SSD%d_L2F_T_CH%02d", (i+1), j);
        sprintf(SSD_L1S_E_nameCH[i][j], "SSD%d_L1S_E_CH%02d", (i+1), j);
        sprintf(SSD_L2F_E_nameCH[i][j], "SSD%d_L2F_E_CH%02d", (i+1), j);
        sprintf(SSD_L2B_E_nameCH[i][j], "SSD%d_L2B_E_CH%02d", (i+1), j);
      }
      for(Int_t j=0; j<9; j++) {
        sprintf(SSD_L3A_E_nameCH[i][j], "SSD%d_L3A_E_CH%02d",i, j);
      }
    }
    for(Int_t i=0; i<4; i++) {
      for(Int_t j=0; j<16; j++) {
         Hist_SSD_L2F_T[i][j] = new TH1D(SSD_L2F_T_nameCH[i][j], SSD_L2F_T_nameCH[i][j], BIN_NUM, LOW_CH, HIGH_CH);
         Hist_SSD_L1S_E[i][j] = new TH1D(SSD_L1S_E_nameCH[i][j], SSD_L1S_E_nameCH[i][j], BIN_NUM, LOW_CH, HIGH_CH);
         Hist_SSD_L2F_E[i][j] = new TH1D(SSD_L2F_E_nameCH[i][j], SSD_L2F_E_nameCH[i][j], BIN_NUM, LOW_CH, HIGH_CH);
         Hist_SSD_L2B_E[i][j] = new TH1D(SSD_L2B_E_nameCH[i][j], SSD_L2B_E_nameCH[i][j], BIN_NUM, LOW_CH, HIGH_CH);
      }
      for(Int_t j=0; j<9; j++) {
         Hist_SSD_L3A_E[i][j] = new TH1D(SSD_L3A_E_nameCH[i][j], SSD_L3A_E_nameCH[i][j], BIN_NUM, LOW_CH, HIGH_CH);
      }
    }

    //__________________________________________________________________________
    //                   对 nentries 作循环，填充直方图
    Long64_t nentries = fChain->GetEntriesFast();
    cout << "nentries = " << nentries << endl;
    for (Long64_t ientry=0; ientry<nentries;ientry++)
    {
      time.PrintPercentageAndRemainingTime(ientry, nentries);

      fChain->GetEntry(ientry);
      //_________________________________________
      //              for PPAC
      for(Int_t i=0; i<3; i++) {
        Hist_PPAC_T[i]  ->Fill(PPAC_T[i],  1.0);
        Hist_PPAC_X1[i] ->Fill(PPAC_X1[i], 1.0);
        Hist_PPAC_X2[i] ->Fill(PPAC_X2[i], 1.0);
        Hist_PPAC_Y1[i] ->Fill(PPAC_Y1[i], 1.0);
        Hist_PPAC_Y2[i] ->Fill(PPAC_Y2[i], 1.0);
        Hist_PPAC_T_E[i]->Fill(PPAC_T_Energy[i], 1.0);
      }
      //________________________________________
      //             for Au-Si
      for(Int_t i=0; i<2; i++) {
        Hist_AuSi_L1T[i]->Fill(AuSi_L1T[i], 1.0);
        Hist_AuSi_L1E[i]->Fill(AuSi_L1E[i], 1.0);
        Hist_AuSi_L2E[i]->Fill(AuSi_L2E[i], 1.0);
      }
      //_______________________________________
      //              for RF
      Hist_RF1 -> Fill(RF1, 1.0);
      Hist_RF2 -> Fill(RF2, 1.0);
      //_______________________________________
      //             for SSD
      for(Int_t i=0; i<4; i++) {
        for(Int_t j=0; j<16; j++) {
          Hist_SSD_L1S_E[i][j]->Fill(SSD_L1S_E[i][j], 1.0);
          Hist_SSD_L2F_E[i][j]->Fill(SSD_L2F_E[i][j], 1.0);
          Hist_SSD_L2B_E[i][j]->Fill(SSD_L2B_E[i][j], 1.0);
          if (SSD_L2F_T[i][j]<10) continue;
          Hist_SSD_L2F_T[i][j]->Fill(SSD_L2F_T[i][j], 1.0);
        }
        for(Int_t j=0; j<9; j++) {
          Hist_SSD_L3A_E[i][j] -> Fill(SSD_L3A_E[i][j], 1.0);
        }
      }
    }

    //__________________________________________________________________________
    //                   定义 Canvas, 画图
    TCanvas* Canvas_begin = new TCanvas("Canvas_begin","Canvas_begin");
    TCanvas* Canvas_end   = new TCanvas("Canvas_end",  "Canvas_end");

    //—————————————————————————
    //       for PPAC
    TCanvas* Canvas_PPAC_T[3];
    TCanvas* Canvas_PPAC_X1[3];
    TCanvas* Canvas_PPAC_X2[3];
    TCanvas* Canvas_PPAC_Y1[3];
    TCanvas* Canvas_PPAC_Y2[3];
    TCanvas* Canvas_PPAC_T_E[3];
    for(Int_t i=0; i<3; i++) {
      Canvas_PPAC_T[i]   = new TCanvas(T_name[i],  T_name[i]);
      Canvas_PPAC_X1[i]  = new TCanvas(X1_name[i], X1_name[i]);
      Canvas_PPAC_X2[i]  = new TCanvas(X2_name[i], X2_name[i]);
      Canvas_PPAC_Y1[i]  = new TCanvas(Y1_name[i], Y1_name[i]);
      Canvas_PPAC_Y2[i]  = new TCanvas(Y2_name[i], Y2_name[i]);
      Canvas_PPAC_T_E[i] = new TCanvas(T_E_name[i],T_E_name[i]);
    }
    //___________________________
    //       for Au-Si
    TCanvas* Canvas_AuSi_L1T[2];
    TCanvas* Canvas_AuSi_L1E[2];
    TCanvas* Canvas_AuSi_L2E[2];
    for(Int_t i=0; i<2; i++) {
      Canvas_AuSi_L1T[i] = new TCanvas(AuSi_L1T_name[i], AuSi_L1T_name[i]);
      Canvas_AuSi_L1E[i] = new TCanvas(AuSi_L1E_name[i], AuSi_L1E_name[i]);
      Canvas_AuSi_L2E[i] = new TCanvas(AuSi_L2E_name[i], AuSi_L2E_name[i]);
    }
    //_________________________________
    //            for RF
    TCanvas* Canvas_RF1 = new TCanvas("RF1", "RF1");
    TCanvas* Canvas_RF2 = new TCanvas("RF2", "RF2");

    //_________________________________
    //            for SSD
    TCanvas* Canvas_SSD_L2F_T[4][16];
    TCanvas* Canvas_SSD_L1S_E[4][16];
    TCanvas* Canvas_SSD_L2F_E[4][16];
    TCanvas* Canvas_SSD_L2B_E[4][16];
    TCanvas* Canvas_SSD_L3A_E[4][9];
    for(Int_t i=0; i<4; i++) {
      for(Int_t j=0; j<16; j++) {
        Canvas_SSD_L2F_T[i][j] = new TCanvas(SSD_L2F_T_nameCH[i][j], SSD_L2F_T_nameCH[i][j]);
        Canvas_SSD_L1S_E[i][j] = new TCanvas(SSD_L1S_E_nameCH[i][j], SSD_L1S_E_nameCH[i][j]);
        Canvas_SSD_L2F_E[i][j] = new TCanvas(SSD_L2F_E_nameCH[i][j], SSD_L2F_E_nameCH[i][j]);
        Canvas_SSD_L2B_E[i][j] = new TCanvas(SSD_L2B_E_nameCH[i][j], SSD_L2B_E_nameCH[i][j]);
      }
      for(Int_t j=0; j<9; j++) {
        Canvas_SSD_L3A_E[i][j] = new TCanvas(SSD_L3A_E_nameCH[i][j], SSD_L3A_E_nameCH[i][j]);
      }
    }

    //__________________________________________________________________________
    //                   Draw histograms
    //       for PPAC
    for(Int_t i=0; i<3; i++) {
      Canvas_PPAC_T[i] ->cd();  /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_PPAC_T[i] ->Draw();
      Canvas_PPAC_X1[i]->cd();  /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_PPAC_X1[i]->Draw();
      Canvas_PPAC_X2[i]->cd();  /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_PPAC_X2[i]->Draw();
      Canvas_PPAC_Y1[i]->cd();  /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_PPAC_Y1[i]->Draw();
      Canvas_PPAC_Y2[i]->cd();  /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_PPAC_Y2[i]->Draw();
      Canvas_PPAC_T_E[i]->cd();  gPad->SetLogy();  gPad->SetGrid();   Hist_PPAC_T_E[i]->Draw();
    }
    //_____________________________
    //        for Au-Si
    for(Int_t i=0; i<2; i++) {
      Canvas_AuSi_L1T[i]->cd();   /* gPad->SetLogy();  gPad->SetGrid(); */ Hist_AuSi_L1T[i]->Draw();
      Canvas_AuSi_L1E[i]->cd();   gPad->SetLogy();  gPad->SetGrid();  Hist_AuSi_L1E[i]->Draw();
      Canvas_AuSi_L2E[i]->cd();   gPad->SetLogy();  gPad->SetGrid();  Hist_AuSi_L2E[i]->Draw();
    }
    //_____________________________
    //         for RF
    Canvas_RF1->cd();   /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_RF1->Draw();
    Canvas_RF2->cd();   /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_RF2->Draw();
    //____________________________
    //        for SSD
    for(Int_t i=0; i<4; i++) {
      for(Int_t j=0; j<16; j++) {
        Canvas_SSD_L2F_T[i][j]->cd();   /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_SSD_L2F_T[i][j]->Draw();
        Canvas_SSD_L1S_E[i][j]->cd();   gPad->SetLogy();  gPad->SetGrid();  Hist_SSD_L1S_E[i][j]->Draw();
        Canvas_SSD_L2F_E[i][j]->cd();   gPad->SetLogy();  gPad->SetGrid();  Hist_SSD_L2F_E[i][j]->Draw();
        Canvas_SSD_L2B_E[i][j]->cd();   gPad->SetLogy();  gPad->SetGrid();  Hist_SSD_L2B_E[i][j]->Draw();
      }
      for(Int_t j=0; j<9; j++) {
        Canvas_SSD_L3A_E[i][j]->cd();   gPad->SetLogy();  gPad->SetGrid();  Hist_SSD_L3A_E[i][j]->Draw();
      }
    }

    //__________________________________________________________________________
    //                         Save the TCanvas

    Canvas_begin->Print(pathpdfout+'[');
    //_____________________________________
    //            for  PPAC
    for(Int_t i=0; i<3; i++) {
      Canvas_PPAC_T[i]  ->Print(pathpdfout);
      Canvas_PPAC_X1[i] ->Print(pathpdfout);
      Canvas_PPAC_X2[i] ->Print(pathpdfout);
      Canvas_PPAC_Y1[i] ->Print(pathpdfout);
      Canvas_PPAC_Y2[i] ->Print(pathpdfout);
      Canvas_PPAC_T_E[i]->Print(pathpdfout);
    }
    //_____________________________________
    //            for Au-Si
    for(Int_t i=0; i<2; i++) {
      Canvas_AuSi_L1T[i]->Print(pathpdfout);
      Canvas_AuSi_L1E[i]->Print(pathpdfout);
      Canvas_AuSi_L2E[i]->Print(pathpdfout);
    }
    //_____________________________________
    //            for RF
    Canvas_RF1->Print(pathpdfout);
    Canvas_RF2->Print(pathpdfout);
    //____________________________________
    //      for SSD
    for(Int_t i=0; i<4; i++) {
      for(Int_t j=0; j<16; j++) {
        Canvas_SSD_L2F_T[i][j]->Print(pathpdfout);
      }
      for(Int_t j=0; j<16; j++) {
        Canvas_SSD_L1S_E[i][j]->Print(pathpdfout);
      }
      for(Int_t j=0; j<16; j++) {
        Canvas_SSD_L2F_E[i][j]->Print(pathpdfout);
      }
      for(Int_t j=0; j<16; j++) {
        Canvas_SSD_L2B_E[i][j]->Print(pathpdfout);
      }
      for(Int_t j=0; j<9; j++) {
        Canvas_SSD_L3A_E[i][j]->Print(pathpdfout);
      }
    }
    Canvas_end->Print(pathpdfout+']');
}


//______________________________________________________________________________
void PrintUsage(char *name){
	//cout<<"Usage: "<<name<<"  "<<endl;
	//cout<<"\t Interactive mode." << endl;
	//cout<<"Usage: "<<name<<" listfilename "<<endl;
	//cout<<"\t 'listfilename' is a text file contains the 'raw data file names'.";
	//cout<<endl;
}
