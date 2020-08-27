
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

using namespace std;

//const string gDataPath = "DAQDataPath";

const int BIN_NUM = 4096;
const int LOW_CH  = 0;
const int HIGH_CH = 4096;

void PrintUsage(char *name);
int main(int argc, char *argv[])
{

////////////////////////////////////////////////////////////////////////////////
  //read filenamelist
  //****************************************************************
  //defination
  //****************************************************************
  string dfname;
  string rawlist, rawfile;
  bool inter = false;
  vector<string> rawdfname;
  fstream listf;

  string datapath = "../../Fission2019_Data/MapRoot"; // to set the datapath
  //****************************************************************
  //judgement the way to solve the data by parameter
  //****************************************************************
  if(argc == 1)
  {
    cout <<"Input RawData File Name: ";
    cin >> dfname;
    inter = true;
  }
  else if(argc == 2)
  {
    rawlist = datapath+"/"+argv[1];
    cout <<"List file of raw data files: " << argv[1] << endl;
    inter = false;
  }
  else
  {
    PrintUsage(argv[0]);
    return 0;
  }
  //****************************************************************
  //Save filename in the vector
  //****************************************************************
  if(inter)
  {
    rawdfname.push_back(dfname);
  }
  else
  {
    cout << rawlist.c_str() << endl;
    listf.open( rawlist.c_str() );
    if( listf.fail() )
    {
      cout << "File: " << rawlist << " open error!" << endl;
      return 0;
    }

    string item;
    while( (listf.peek() != EOF) && (!listf.fail()) )
    {
      getline(listf, item);
      cout << item << endl;
      TString tsitem(item);
      if(tsitem.IsWhitespace())  continue;  //skip blank lines
      if(item.c_str()[0] == '*') continue;  //skip the comments
      istringstream s_item(item);
      if(item.size() > 0)
      {
        s_item >> dfname;
        rawdfname.push_back(dfname);
      }
      item.empty();
    }
    listf.close();
  }


////////////////////////////////////////////////////////////////////////////////

  //****************************************************************
  //    loop on each file
  //****************************************************************
  for(int i=0; i<rawdfname.size(); i++)
  {
    string rdfname = rawdfname[i];
    TString pathrdfn = datapath;
    pathrdfn += "/";
    pathrdfn += rdfname;
    cout << pathrdfn << endl;

/*
    //====================================
    //      设置输出 .root 文件路径 与 命名
    TString pathoutfile = "./QC_HistRoot";
    mkdir(pathoutfile,0777);
    pathoutfile += "/QC_";
    pathoutfile += rdfname;
    cout << pathoutfile << endl;

    TFile fout(pathoutfile.Data(),"recreate");
    fout.cd();
*/

    //====================================
    //      设置输出 .pdf 文件路径 与 命名
    TString Canvas_name(rdfname);
    Canvas_name.ReplaceAll(".root", "");
    TString pathoutpdf = "./QC_pdf";//------------------------------------------------------->outputpath of pdf
    mkdir(pathoutpdf,0777);
    pathoutpdf += "/QC_";	//--------"a_QA_" is the signature of qautify data
    pathoutpdf += Canvas_name;
    pathoutpdf += ".pdf";
    cout << pathoutpdf << endl;


    //read in the file
    TFile f(pathrdfn.Data());
    TTree *fChain = (TTree*)gFile->Get("KrPb");//fChain can be a TChain


////////////////////////////////////////////////////////////////////////////////
///        读取 .root文件中的 Tree

    //=============================================
    //                  For PPAC
    //=============================================
    int PPAC_T[3]  = {0};
    int PPAC_X1[3] = {0};
    int PPAC_X2[3] = {0};
    int PPAC_Y1[3] = {0};
    int PPAC_Y2[3] = {0};
    int PPAC_T_Energy[3] = {0};

    char T_name[3][200];
    char X1_name[3][200];
    char X2_name[3][200];
    char Y1_name[3][200];
    char Y2_name[3][200];
    char T_E_name[3][200];

    for(int i=0;i<3;i++)
    {
      sprintf(T_name[i], "PPAC%d_T",(i+1));
      sprintf(X1_name[i],"PPAC%d_X1",(i+1));
      sprintf(X2_name[i],"PPAC%d_X2",(i+1));
      sprintf(Y1_name[i],"PPAC%d_Y1",(i+1));
      sprintf(Y2_name[i],"PPAC%d_Y2",(i+1));
      sprintf(T_E_name[i],"PPAC%d_T_Energy",(i+1));
    }

    for(int i=0;i<3;i++)
    {
      fChain->SetBranchAddress(T_name[i], &PPAC_T[i]);
      fChain->SetBranchAddress(X1_name[i],&PPAC_X1[i]);
      fChain->SetBranchAddress(X2_name[i],&PPAC_X2[i]);
      fChain->SetBranchAddress(Y1_name[i],&PPAC_Y1[i]);
      fChain->SetBranchAddress(Y2_name[i],&PPAC_Y2[i]);
      fChain->SetBranchAddress(T_E_name[i],&PPAC_T_Energy[i]);
    }

    //========================================================
    //                       For Au-Si
    //========================================================
    int AuSi_L1E[2] = {0};
    int AuSi_L2E[2] = {0};
    int AuSi_L1T[2] = {0};

    char AuSi_L1E_name[2][200];
    char AuSi_L2E_name[2][200];
    char AuSi_L1T_name[2][200];

    for(int i=0;i<2;i++)
    {
      sprintf(AuSi_L1E_name[i],"AuSi%d_L1E",(i+1));
      sprintf(AuSi_L2E_name[i],"AuSi%d_L2E",(i+1));
      sprintf(AuSi_L1T_name[i],"AuSi%d_L1T",(i+1));
    }

    for(int i=0;i<2;i++)
    {
      fChain->SetBranchAddress(AuSi_L1E_name[i], &AuSi_L1E[i]);
      fChain->SetBranchAddress(AuSi_L2E_name[i], &AuSi_L2E[i]);
      fChain->SetBranchAddress(AuSi_L1T_name[i], &AuSi_L1T[i]);
    }


    //==============================================
    //               For RF
    ////////////////////////////////////////////////
    int RF1 = 0;
    int RF2 = 0;
    fChain->SetBranchAddress("RF1",&RF1);
    fChain->SetBranchAddress("RF2",&RF2);

    //========================================================
    //                    For SSD
    //========================================================
    int SSD_L2F_T[4][16] = {{0}};
    int SSD_L1S_E[4][16] = {{0}};
    int SSD_L2F_E[4][16] = {{0}};
    int SSD_L2B_E[4][16] = {{0}};
    int SSD_L3A_E[4][9]  = {{0}};

    char SSD_L2F_T_name[4][200];
    char SSD_L1S_E_name[4][200];
    char SSD_L2F_E_name[4][200];
    char SSD_L2B_E_name[4][200];
    char SSD_L3A_E_name[4][200];

    for(int i=0; i<4; i++)
    {
      sprintf(SSD_L2F_T_name[i],"SSD%d_L2F_T",i);
      sprintf(SSD_L1S_E_name[i],"SSD%d_L1S_E",i);
      sprintf(SSD_L2F_E_name[i],"SSD%d_L2F_E",i);
      sprintf(SSD_L2B_E_name[i],"SSD%d_L2B_E",i);
      sprintf(SSD_L3A_E_name[i],"SSD%d_L3A_E",i);
    }


    for(int i=0; i<4; i++)
    {
      fChain->SetBranchAddress(SSD_L2F_T_name[i], SSD_L2F_T[i]);
      fChain->SetBranchAddress(SSD_L1S_E_name[i], SSD_L1S_E[i]);
      fChain->SetBranchAddress(SSD_L2F_E_name[i], SSD_L2F_E[i]);
      fChain->SetBranchAddress(SSD_L2B_E_name[i], SSD_L2B_E[i]);
      fChain->SetBranchAddress(SSD_L3A_E_name[i], SSD_L3A_E[i]);
    }

///
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
///                     定义直方图

    //******************
    //   for PPAC
    //******************
    TH1D *Hist_PPAC_T[3];
    TH1D *Hist_PPAC_X1[3];
    TH1D *Hist_PPAC_X2[3];
    TH1D *Hist_PPAC_Y1[3];
    TH1D *Hist_PPAC_Y2[3];
    TH1D *Hist_PPAC_T_E[3];

    for(int i=0;i<3;i++)
    {
       Hist_PPAC_T[i]   = new TH1D(T_name[i],   T_name[i],  BIN_NUM, LOW_CH, HIGH_CH);
       Hist_PPAC_X1[i]  = new TH1D(X1_name[i], X1_name[i], BIN_NUM, LOW_CH, HIGH_CH);
       Hist_PPAC_X2[i]  = new TH1D(X2_name[i], X2_name[i], BIN_NUM, LOW_CH, HIGH_CH);
       Hist_PPAC_Y1[i]  = new TH1D(Y1_name[i], Y1_name[i], BIN_NUM, LOW_CH, HIGH_CH);
       Hist_PPAC_Y2[i]  = new TH1D(Y2_name[i], Y2_name[i], BIN_NUM, LOW_CH, HIGH_CH);
       Hist_PPAC_T_E[i] = new TH1D(T_E_name[i],T_E_name[i], BIN_NUM, LOW_CH, HIGH_CH);
    }

    //************************************
    //    for Au-Si
    //************************************
    TH1D * Hist_AuSi_L1T[2];
    TH1D * Hist_AuSi_L1E[2];
    TH1D * Hist_AuSi_L2E[2];

    for(int i=0; i<2; i++)
    {
      Hist_AuSi_L1T[i] = new TH1D(AuSi_L1T_name[i], AuSi_L1T_name[i], BIN_NUM, LOW_CH, HIGH_CH);
      Hist_AuSi_L1E[i] = new TH1D(AuSi_L1E_name[i], AuSi_L1E_name[i], BIN_NUM, LOW_CH, HIGH_CH);
      Hist_AuSi_L2E[i] = new TH1D(AuSi_L2E_name[i], AuSi_L2E_name[i], BIN_NUM, LOW_CH, HIGH_CH);
    }

    //***************
    //   for RF
    //***************
    TH1D * Hist_RF1 = new TH1D("RF1", "RF1", BIN_NUM, LOW_CH, HIGH_CH);
    TH1D * Hist_RF2 = new TH1D("RF2", "RF2", BIN_NUM, LOW_CH, HIGH_CH);


    //**************************************
    //    for SSD
    //**************************************
    TH1D * Hist_SSD_L2F_T[4][16];
    TH1D * Hist_SSD_L1S_E[4][16];
    TH1D * Hist_SSD_L2F_E[4][16];
    TH1D * Hist_SSD_L2B_E[4][16];
    TH1D * Hist_SSD_L3A_E[4][9];

    char SSD_L2F_T_nameCH[4][16][200];
    char SSD_L1S_E_nameCH[4][16][200];
    char SSD_L2F_E_nameCH[4][16][200];
    char SSD_L2B_E_nameCH[4][16][200];
    char SSD_L3A_E_nameCH[4][9][200];


    for(int i=0; i<4; i++)
    {
      for(int j=0; j<16; j++)
      {
        sprintf(SSD_L2F_T_nameCH[i][j], "SSD%d_L2F_T_CH%d",(i+1), j);
        sprintf(SSD_L1S_E_nameCH[i][j], "SSD%d_L1S_E_CH%d",(i+1), j);
        sprintf(SSD_L2F_E_nameCH[i][j], "SSD%d_L2F_E_CH%d",(i+1), j);
        sprintf(SSD_L2B_E_nameCH[i][j], "SSD%d_L2B_E_CH%d",(i+1), j);
      }
      for(int j=0; j<9; j++)
      {
        sprintf(SSD_L3A_E_nameCH[i][j], "SSD%d_L3A_E_CH%d",i, j);
      }
    }

    for(int i=0; i<4; i++)
    {
      for(int j=0; j<16; j++)
      {
         Hist_SSD_L2F_T[i][j] = new TH1D(SSD_L2F_T_nameCH[i][j], SSD_L2F_T_nameCH[i][j], BIN_NUM, LOW_CH, HIGH_CH);
         Hist_SSD_L1S_E[i][j] = new TH1D(SSD_L1S_E_nameCH[i][j], SSD_L1S_E_nameCH[i][j], BIN_NUM, LOW_CH, HIGH_CH);
         Hist_SSD_L2F_E[i][j] = new TH1D(SSD_L2F_E_nameCH[i][j], SSD_L2F_E_nameCH[i][j], BIN_NUM, LOW_CH, HIGH_CH);
         Hist_SSD_L2B_E[i][j] = new TH1D(SSD_L2B_E_nameCH[i][j], SSD_L2B_E_nameCH[i][j], BIN_NUM, LOW_CH, HIGH_CH);
      }
      for(int j=0; j<9; j++)
      {
         Hist_SSD_L3A_E[i][j] = new TH1D(SSD_L3A_E_nameCH[i][j], SSD_L3A_E_nameCH[i][j], BIN_NUM, LOW_CH, HIGH_CH);
      }
    }


///////////////////////////////////////////////////////////////////////////////
////    对 nentries 作循环，填充直方图
    //========================================================
    //     Get Entry number
    //========================================================
    Long64_t nentries = fChain->GetEntriesFast();
    cout << "nentries=" << nentries << endl;

    for (Long64_t ientry=0; ientry<nentries;ientry++)
    {
      Long64_t jentry = fChain->LoadTree(ientry);
      if (jentry < 0) break;
      fChain->GetEntry(ientry);

////////////////////////////////////////////////////////////////////////////////
////                  填充直方图

      //===============================
      //   for PPAC
      for(int i=0;i<3;i++)
      {
        Hist_PPAC_T[i]  ->Fill(PPAC_T[i],1.0);
        Hist_PPAC_X1[i] ->Fill(PPAC_X1[i],1.0);
        Hist_PPAC_X2[i] ->Fill(PPAC_X2[i],1.0);
        Hist_PPAC_Y1[i] ->Fill(PPAC_Y1[i],1.0);
        Hist_PPAC_Y2[i] ->Fill(PPAC_Y2[i],1.0);
        Hist_PPAC_T_E[i]->Fill(PPAC_T_Energy[i],1.0);
      }

      //==============================
      //        for Au-Si
      for(int i=0; i<2; i++)
      {
        Hist_AuSi_L1T[i] ->Fill(AuSi_L1T[i], 1.0);
        Hist_AuSi_L1E[i] ->Fill(AuSi_L1E[i], 1.0);
        Hist_AuSi_L2E[i] ->Fill(AuSi_L2E[i], 1.0);
      }

      //===============================
      //      for RF
      Hist_RF1 -> Fill(RF1, 1.0);
      Hist_RF2 -> Fill(RF2, 1.0);


      //===============================
      //    for SSD
      for(int i=0; i<4; i++)
      {
        for(int j=0; j<16; j++)
        {
          if(SSD_L2F_T[i][j]<10) continue;
          Hist_SSD_L2F_T[i][j] -> Fill(SSD_L2F_T[i][j], 1.0);
        }
        for(int j=0; j<16; j++)
        {
          Hist_SSD_L1S_E[i][j] -> Fill(SSD_L1S_E[i][j], 1.0);
          Hist_SSD_L2F_E[i][j] -> Fill(SSD_L2F_E[i][j], 1.0);
          Hist_SSD_L2B_E[i][j] -> Fill(SSD_L2B_E[i][j], 1.0);
        //  cout<< SSD_L1S_E[i][j] <<endl;
        }
        for(int j=0; j<9; j++)
        {
          Hist_SSD_L3A_E[i][j] -> Fill(SSD_L3A_E[i][j], 1.0);
        }
      }

    }


////////////////////////////////////////////////////////////////////////////////
///                   denifite canvas to draw histograms

    //Define the begin and end TCanvas
    TCanvas *Canvas_begin = new TCanvas("Canvas_begin","Canvas_begin");
    TCanvas *Canvas_end   = new TCanvas("Canvas_end","Canvas_end");

    //=========================
    //   for PPAC
    //=========================
    TCanvas *Canvas_PPAC_T[3];
    TCanvas *Canvas_PPAC_X1[3];
    TCanvas *Canvas_PPAC_X2[3];
    TCanvas *Canvas_PPAC_Y1[3];
    TCanvas *Canvas_PPAC_Y2[3];
    TCanvas *Canvas_PPAC_T_E[3];

    for(int i=0; i<3; i++)
    {
      Canvas_PPAC_T[i]   = new TCanvas(T_name[i], T_name[i]);
      Canvas_PPAC_X1[i]  = new TCanvas(X1_name[i],X1_name[i]);
      Canvas_PPAC_X2[i]  = new TCanvas(X2_name[i],X2_name[i]);
      Canvas_PPAC_Y1[i]  = new TCanvas(Y1_name[i],Y1_name[i]);
      Canvas_PPAC_Y2[i]  = new TCanvas(Y2_name[i],Y2_name[i]);
      Canvas_PPAC_T_E[i] = new TCanvas(T_E_name[i],T_E_name[i]);
    }

    //================================
    //       for Au-Si
    //================================
    TCanvas * Canvas_AuSi_L1T[2];
    TCanvas * Canvas_AuSi_L1E[2];
    TCanvas * Canvas_AuSi_L2E[2];

    for(int i=0; i<2; i++)
    {
      Canvas_AuSi_L1T[i] = new TCanvas(AuSi_L1T_name[i], AuSi_L1T_name[i]);
      Canvas_AuSi_L1E[i] = new TCanvas(AuSi_L1E_name[i], AuSi_L1E_name[i]);
      Canvas_AuSi_L2E[i] = new TCanvas(AuSi_L2E_name[i], AuSi_L2E_name[i]);
    }

    //===============================
    //      for RF
    TCanvas * Canvas_RF1 = new TCanvas("RF1", "RF1");
    TCanvas * Canvas_RF2 = new TCanvas("RF2", "RF2");


    //===============================
    //        for SSD
    //===============================
    TCanvas *Canvas_SSD_L2F_T[4][16];
    TCanvas *Canvas_SSD_L1S_E[4][16];
    TCanvas *Canvas_SSD_L2F_E[4][16];
    TCanvas *Canvas_SSD_L2B_E[4][16];
    TCanvas *Canvas_SSD_L3A_E[4][9];
    for(int i=0; i<4; i++)
    {
      for(int j=0; j<16; j++)
      {
        Canvas_SSD_L2F_T[i][j] = new TCanvas(SSD_L2F_T_nameCH[i][j], SSD_L2F_T_nameCH[i][j]);
        Canvas_SSD_L1S_E[i][j] = new TCanvas(SSD_L1S_E_nameCH[i][j], SSD_L1S_E_nameCH[i][j]);
        Canvas_SSD_L2F_E[i][j] = new TCanvas(SSD_L2F_E_nameCH[i][j], SSD_L2F_E_nameCH[i][j]);
        Canvas_SSD_L2B_E[i][j] = new TCanvas(SSD_L2B_E_nameCH[i][j], SSD_L2B_E_nameCH[i][j]);
      }
      for(int j=0; j<9; j++)
      {
        Canvas_SSD_L3A_E[i][j] = new TCanvas(SSD_L3A_E_nameCH[i][j], SSD_L3A_E_nameCH[i][j]);
      }
    }


////////////////////////////////////////////////////////////////////////////////
///                   Draw histograms

    //==================
    //   for PPAC
    for(int i=0; i<3; i++)
    {
      Canvas_PPAC_T[i]->cd();   /* gPad->SetLogy();  gPad->SetGrid(); */ Hist_PPAC_T[i]  ->Draw();
      Canvas_PPAC_X1[i]->cd();  /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_PPAC_X1[i] ->Draw();
      Canvas_PPAC_X2[i]->cd();  /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_PPAC_X2[i] ->Draw();
      Canvas_PPAC_Y1[i]->cd();  /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_PPAC_Y1[i] ->Draw();
      Canvas_PPAC_Y2[i]->cd();  /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_PPAC_Y2[i] ->Draw();
      Canvas_PPAC_T_E[i]->cd();  gPad->SetLogy();  gPad->SetGrid();   Hist_PPAC_T_E[i]->Draw();
    }

    ////////////////////
    //   for Au-Si
    for(int i=0; i<2; i++)
    {
      Canvas_AuSi_L1T[i]->cd();   /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_AuSi_L1T[i] -> Draw();
      Canvas_AuSi_L1E[i]->cd();   gPad->SetLogy();  gPad->SetGrid();  Hist_AuSi_L1E[i] -> Draw();
      Canvas_AuSi_L2E[i]->cd();   gPad->SetLogy();  gPad->SetGrid();  Hist_AuSi_L2E[i] -> Draw();
    }

    ///////////////////
    ///   for RF
    Canvas_RF1 -> cd();   /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_RF1 -> Draw();
    Canvas_RF2 -> cd();   /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_RF2 -> Draw();


    ////////////////////
    //   for SSD
    for(int i=0; i<4; i++)
    {
      for(int j=0; j<16; j++)
      {
        Canvas_SSD_L2F_T[i][j]->cd();   /* gPad->SetLogy();  gPad->SetGrid(); */  Hist_SSD_L2F_T[i][j] -> Draw();
        Canvas_SSD_L1S_E[i][j]->cd();   gPad->SetLogy();  gPad->SetGrid();  Hist_SSD_L1S_E[i][j] -> Draw();
        Canvas_SSD_L2F_E[i][j]->cd();   gPad->SetLogy();  gPad->SetGrid();  Hist_SSD_L2F_E[i][j] -> Draw();
        Canvas_SSD_L2B_E[i][j]->cd();   gPad->SetLogy();  gPad->SetGrid();  Hist_SSD_L2B_E[i][j] -> Draw();
      }

      for(int j=0; j<9; j++)
      {
        Canvas_SSD_L3A_E[i][j]->cd();   gPad->SetLogy();  gPad->SetGrid();  Hist_SSD_L3A_E[i][j] -> Draw();
      }
    }


//==============================================================================
//                         Save the TCanvas
//==============================================================================

    Canvas_begin->Print(pathoutpdf+'[');

    //====================
    //    for  PPAC
    for(int i=0; i<3; i++)
    {
      Canvas_PPAC_T[i]->Print(pathoutpdf);
      Canvas_PPAC_X1[i]->Print(pathoutpdf);
      Canvas_PPAC_X2[i]->Print(pathoutpdf);
      Canvas_PPAC_Y1[i]->Print(pathoutpdf);
      Canvas_PPAC_Y2[i]->Print(pathoutpdf);
      Canvas_PPAC_T_E[i]->Print(pathoutpdf);
    }

    //=====================
    //      for Au-Si
    for(int i=0; i<2; i++)
    {
      Canvas_AuSi_L1T[i]->Print(pathoutpdf);
      Canvas_AuSi_L1E[i]->Print(pathoutpdf);
      Canvas_AuSi_L2E[i]->Print(pathoutpdf);
    }

    //=====================
    //       for RF
    Canvas_RF1->Print(pathoutpdf);
    Canvas_RF2->Print(pathoutpdf);

    //=====================
    //      for SSD
    for(int i=0; i<4; i++)
    {
      for(int j=0; j<16; j++)
      {
        Canvas_SSD_L2F_T[i][j]->Print(pathoutpdf);
      }
      for(int j=0; j<16; j++)
      {
        Canvas_SSD_L1S_E[i][j]->Print(pathoutpdf);
      }
      for(int j=0; j<16; j++)
      {
        Canvas_SSD_L2F_E[i][j]->Print(pathoutpdf);
      }
      for(int j=0; j<16; j++)
      {
        Canvas_SSD_L2B_E[i][j]->Print(pathoutpdf);
      }

      for(int j=0; j<9; j++)
      {
        Canvas_SSD_L3A_E[i][j]->Print(pathoutpdf);
      }
    }

    Canvas_end->Print(pathoutpdf+']');

//=============================================================================
//    Save Histograms into rootfiles
/*
    fout.Write();
    fout.Close();
*/
  }

  return 0;
}


void PrintUsage(char *name){
	//cout<<"Usage: "<<name<<"  "<<endl;
	//cout<<"\t Interactive mode." << endl;
	//cout<<"Usage: "<<name<<" listfilename "<<endl;
	//cout<<"\t 'listfilename' is a text file contains the 'raw data file names'.";
	//cout<<endl;
}
