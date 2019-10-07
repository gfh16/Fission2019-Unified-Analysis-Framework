///////////////////////////////////////////////
// An a_QA.cc: main() of qautify the data,
//
// yj-wang 2018-2-27
///////////////////////////////////////////////

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

void PrintUsage(char *name);
int main(int argc, char *argv[])
{
  //read filenamelist

  //****************************************************************
  //defination
  //****************************************************************
  string dfname;
  string rawlist, rawfile;
  bool inter = false;
  vector<string> rawdfname;
  fstream listf;


  string datapath = string datapath = "../../Fission2019_Data/MapRoot"; // to set the datapath
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

  //****************************************************************
  //loop on each file
  //****************************************************************
  for(int i=0; i<rawdfname.size(); i++)
  {
    string rdfname = rawdfname[i];
    TString pathrdfn = datapath;
    pathrdfn += "/";
    pathrdfn += rdfname;
    cout << pathrdfn << endl;

    //read in the file
    TFile f(pathrdfn.Data());
    TTree *fChain = (TTree*)gFile->Get("PbAu");//fChain can be a TChain

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

    char T_name[200];   char T_nameI[200];
    char X1_name[200];  char X1_nameI[200];
    char X2_name[200];  char X2_nameI[200];
    char Y1_name[200];  char Y1_nameI[200];
    char Y2_name[200];  char Y2_nameI[200];
    char T_E_name[200]; char T_E_nameI[200];

    for(int i=0;i<3;i++)
    {
      sprintf(T_name,"PPAC%d_T",(i+1));
      sprintf(X1_name,"PPAC%d_X1",(i+1));
      sprintf(X2_name,"PPAC%d_X2",(i+1));
      sprintf(Y1_name,"PPAC%d_Y1",(i+1));
      sprintf(Y2_name,"PPAC%d_Y2",(i+1));
      sprintf(T_Ename,"PPAC%d_T_Energy",(i+1));

      fChain->SetBranchAddress(T_name,&T[i]);
      fChain->SetBranchAddress(X1_name,&X1[i]);
      fChain->SetBranchAddress(X2_name,&X2[i]);
      fChain->SetBranchAddress(Y1_name,&Y1[i]);
      fChain->SetBranchAddress(Y2_name,&Y2[i]);
      fChain->SetBranchAddress(T_Ename,&PPAC_T_Energy[i]);
    }

    //========================================================
    //                       For Au-Si
    //========================================================
    int AuSi_L1E[2] = {0};
    int AuSi_L2E[2] = {0};
    int AuSi_L1T[2] = {0};

    char AuSi_L1E_name[200];    char AuSi_L1E_nameI[200];
    char AuSi_L2E_name[200];    char AuSi_L2E_nameI[200];
    char AuSi_L1T_name[200];    char AuSi_L1T_nameI[200];

    for(int i=0;i<2;i++)
    {
      sprintf(AuSi_L1E_name,"AuSi%d_L1E",(i+1));
      sprintf(AuSi_L2E_name,"AuSi%d_L2E",(i+1));
      sprintf(AuSi_L1T_name,"AuSi%d_L1T",(i+1));

      fChain->SetBranchAddress(AuSi_L1E_name, &AuSi_L1E[i]);
      fChain->SetBranchAddress(AuSi_L2E_name, &AuSi_L2E[i]);
      fChain->SetBranchAddress(AuSi_L1T_name, &AuSi_L1T[i]);
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
    int SSD_L2F_T[4][16];
    int SSD_L1S_E[4][16];
    int SSD_L2F_E[4][16];
    int SSD_L2B_E[4][16];
    int SSD_L3A_E[4][9];
    for(int i=0; i<4; i++)
    {
      for(int j=0; j<16; j++)
      {
         SSD_L2F_T[i][j] = 0;
         SSD_L1S_E[i][j] = 0;
         SSD_L2F_E[i][j] = 0;
         SSD_L2B_E[i][j] = 0;
      }

      for(int j=0; j<9; j++)
      {
         SSD_L3A_E[i][j]  = 0;
      }
    }

    char SSD_L2F_T_name[200];  char SSD_L2F_T_nameI[200];
    char SSD_L1S_E_name[200];  char SSD_L1S_E_nameI[200];
    char SSD_L2F_E_name[200];  char SSD_L2F_E_nameI[200];
    char SSD_L2B_E_name[200];  char SSD_L2B_E_nameI[200];
    char SSD_L3A_E_name[200];  char SSD_L3A_E_nameI[200];

    for(int i=0; i<4; i++)
    {
      sprintf(SSD_L2F_T_name,"SSD%d_L2F_T",(i+1));
      sprintf(SSD_L1S_E_name,"SSD%d_L1S_E",(i+1));
      sprintf(SSD_L2F_E_name,"SSD%d_L2F_E",(i+1));    
      sprintf(SSD_L2B_E_name,"SSD%d_L2B_E",(i+1));
      sprintf(SSD_L3A_E_name,"SSD%d_L3A_E",(i+1));

      fChain->SetBranchAddress(SSD_L2F_T_name, SSD_L2F_T[i]);
      fChain->SetBranchAddress(SSD_L1S_E_name, SSD_L1S_E[i]);
      fChain->SetBranchAddress(SSD_L2F_E_name, SSD_L2F_E[i]);
      fChain->SetBranchAddress(SSD_L2B_E_name, SSD_L2B_E[i]);
      fChain->SetBranchAddress(SSD_L3A_E_name, SSD_L3A_E[i]);
    }



    for(int j=0;j<2;j++)
    {
      for(int i=0;i<32;i++)
      {
        sprintf(SSD_L1_E_name,"SSD%d_L1_E_CH%d",(j+1),i);
        sprintf(SSD_L2_F_E_name,"SSD%d_L2_F_E_CH%d",(j+1),i);
        sprintf(SSD_L2_B_E_name,"SSD%d_L2_B_E_CH%d",(j+1),i);
        sprintf(SSD_L2_F_T_name,"SSD%d_L2_F_T_CH%d",(j+1),i);

        fChain->SetBranchAddress(SSD_L1_E_name,&SSD_L1_Energy[j][i]);
        fChain->SetBranchAddress(SSD_L2_F_E_name,&SSD_L2_F_Energy[j][i]);
        fChain->SetBranchAddress(SSD_L2_B_E_name,&SSD_L2_B_Energy[j][i]);
        fChain->SetBranchAddress(SSD_L2_F_T_name,&SSD_L2_F_T[j][i]);
      }
      for(int i=0;i<9;i++)
      {
       sprintf(SSD_L3_E_name,"SSD%d_L3_E_CH%d",(j+1),i);
       fChain->SetBranchAddress(SSD_L3_E_name,&SSD_L3_Energy[j][i]);
      }
    }



    //========================================================
    //GetEntry number
    //========================================================
	Long64_t nentries = fChain->GetEntriesFast();
	cout << "nentries=" << nentries << endl;
    //========================================================
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    //****************************************************************
    //output file
    //****************************************************************
    TString pathoutfile = "./a_QA_rootdata";//------------------------------------------------------->outputpath
    mkdir(pathoutfile,0777);
    pathoutfile += "/a_QA_";	//---------------------->"a_Cali_" is the signature of first Calibrate data
    pathoutfile += rdfname;
    cout << pathoutfile << endl;

    //****************************************************************
    //define the TH1F
    //****************************************************************
	int Bin_Num = 4096;
	int Low_CH  = 0;
	int High_CH = 4096;
    //****************************************************************
    //PPAC Histogram Crate
    //****************************************************************
    TH1D *h_PPAC_T[3];
    TH1D *h_PPAC_X1[3];
    TH1D *h_PPAC_X2[3];
    TH1D *h_PPAC_Y1[3];
    TH1D *h_PPAC_Y2[3];
    TH1D *h_PPAC_T_E[3];

    for(int i=0;i<3;i++)
    {
      sprintf(T_name,"PPAC%d_T",(i+1));
      sprintf(X1_name,"PPAC%d_X1",(i+1));
      sprintf(X2_name,"PPAC%d_X2",(i+1));
      sprintf(Y1_name,"PPAC%d_Y1",(i+1));
      sprintf(Y2_name,"PPAC%d_Y2",(i+1));
      sprintf(T_Ename,"PPAC%d_T_Energy",(i+1));

      h_PPAC_T[i] = new TH1D(T_name,T_name,Bin_Num,Low_CH,High_CH);
      h_PPAC_X1[i] = new TH1D(X1_name,X1_name,Bin_Num,Low_CH,High_CH);
      h_PPAC_X2[i] = new TH1D(X2_name,X2_name,Bin_Num,Low_CH,High_CH);
      h_PPAC_Y1[i] = new TH1D(Y1_name,Y1_name,Bin_Num,Low_CH,High_CH);
      h_PPAC_Y2[i] = new TH1D(Y2_name,Y2_name,Bin_Num,Low_CH,High_CH);
      h_PPAC_T_E[i] = new TH1D(T_Ename,T_Ename,Bin_Num,Low_CH,High_CH);
    }
    //****************************************************************
    //SSD_Tele Histogram Crate
    //****************************************************************
    TH1D * h_SSD_L1_Energy[2][32]   = {{0}};
    TH1D * h_SSD_L2_F_Energy[2][32] = {{0}};
    TH1D * h_SSD_L2_B_Energy[2][32] = {{0}};
    TH1D * h_SSD_L3_Energy[2][9] = {{0}};
    TH1D * h_SSD_L2_F_T[2][32] = {{0}};

    for(int j=0;j<2;j++)
    {
      for(int i=0;i<32;i++)
      {
        sprintf(SSD_L1_E_name,"SSD%d_L1_E_CH%d",(j+1),i);
        sprintf(SSD_L2_F_E_name,"SSD%d_L2_F_E_CH%d",(j+1),i);
        sprintf(SSD_L2_B_E_name,"SSD%d_L2_B_E_CH%d",(j+1),i);
        sprintf(SSD_L2_F_T_name,"SSD%d_L2_F_T_CH%d",(j+1),i);

        h_SSD_L1_Energy[j][i] = new TH1D(SSD_L1_E_name,SSD_L1_E_name,Bin_Num,Low_CH,High_CH);
        h_SSD_L2_F_Energy[j][i] = new TH1D(SSD_L2_F_E_name,SSD_L2_F_E_name,Bin_Num,Low_CH,High_CH);
        h_SSD_L2_B_Energy[j][i] = new TH1D(SSD_L2_B_E_name,SSD_L2_B_E_name,Bin_Num,Low_CH,High_CH);
        h_SSD_L2_F_T[j][i] = new TH1D(SSD_L2_F_T_name,SSD_L2_F_T_name,Bin_Num,Low_CH,High_CH);
      }
      for(int i=0;i<9;i++)
      {
       sprintf(SSD_L3_E_name,"SSD%d_L3_E_CH%d",(j+1),i);
       h_SSD_L3_Energy[j][i] = new TH1D(SSD_L3_E_name,SSD_L3_E_name,Bin_Num,Low_CH,High_CH);
      }
    }
    //****************************************************************
    //Small_Si_Telescope Histogram Crate
    //****************************************************************
    TH1D * h_Small_Si_L1_Energy[4] = {0};
    TH1D * h_Small_Si_L2_Energy[4] = {0};
    TH1D * h_Small_Si_L3_Energy[4] = {0};
    TH1D * h_Small_Si_L2_T[4] = {0};

    for(int i=0;i<4;i++)
    {
      sprintf(Si_L1_E_name,"Si%d_L1_E",(i+1));
      sprintf(Si_L2_E_name,"Si%d_L2_E",(i+1));
      sprintf(Si_L3_E_name,"Si%d_L3_E",(i+1));
      sprintf(Si_L2_T_name,"Si%d_L2_T",(i+1));

      h_Small_Si_L1_Energy[i] = new TH1D(Si_L1_E_name,Si_L1_E_name,Bin_Num,Low_CH,High_CH);
      h_Small_Si_L2_Energy[i] = new TH1D(Si_L2_E_name,Si_L2_E_name,Bin_Num,Low_CH,High_CH);
      h_Small_Si_L3_Energy[i] = new TH1D(Si_L3_E_name,Si_L3_E_name,Bin_Num,Low_CH,High_CH);
      h_Small_Si_L2_T[i] = new TH1D(Si_L2_T_name,Si_L2_T_name,Bin_Num,Low_CH,High_CH);
    }
    //****************************************************************
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    //****************************************************************
    //loop on each Events in the rootfile
    //****************************************************************
    for (Long64_t ientry=0; ientry<nentries;ientry++)
    {
      Long64_t jentry = fChain->LoadTree(ientry);
      if (jentry < 0) break;
      fChain->GetEntry(ientry);
      //========================================================
      //Start your analysing ......
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      //author:yj-wang15    date: 2018-3-3
      //========================================================
      //PPAC histogram fill
      //========================================================
      for(int i=0;i<3;i++)
      {
        h_PPAC_T[i] ->Fill(T[i],1.0);
        h_PPAC_X1[i] ->Fill(X1[i],1.0);
        h_PPAC_X2[i] ->Fill(X2[i],1.0);
        h_PPAC_Y1[i] ->Fill(Y1[i],1.0);
        h_PPAC_Y2[i] ->Fill(Y2[i],1.0);
        h_PPAC_T_E[i] ->Fill(PPAC_T_Energy[i],1.0);
      }
      //========================================================
      //SSD_Tele histogram fill
      //========================================================
      for(int j=0;j<2;j++)
      {
      for(int i=0;i<32;i++)
        {
        h_SSD_L1_Energy[j][i] ->Fill(SSD_L1_Energy[j][i],1.0);
        h_SSD_L2_F_Energy[j][i] ->Fill(SSD_L2_F_Energy[j][i],1.0);
        h_SSD_L2_B_Energy[j][i] ->Fill(SSD_L2_B_Energy[j][i],1.0);
        h_SSD_L2_F_T[j][i] ->Fill(SSD_L2_F_T[j][i],1.0);
        }
      for(int i=0;i<9;i++)
        {
       h_SSD_L3_Energy[j][i] ->Fill(SSD_L3_Energy[j][i],1.0);
        }
      }
      //========================================================
      //Small_Si_Telescope histogram fill
      //========================================================
      for(int i=0;i<4;i++)
      {
      h_Small_Si_L1_Energy[i] ->Fill(Small_Si_L1_Energy[i],1.0);
      h_Small_Si_L2_Energy[i] ->Fill(Small_Si_L2_Energy[i],1.0);
      h_Small_Si_L3_Energy[i] ->Fill(Small_Si_L3_Energy[i],1.0);
      h_Small_Si_L2_T[i] ->Fill(Small_Si_L2_T[i],1.0);
      }
      //========================================================
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    }
    //f.Close();
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //author:yj-wang15    date: 2018-3-3
    //========================================================
    //Draw the histogram
    //========================================================
    //Define the begin and end TCanvas
    TCanvas *Canvas_begin = new TCanvas("Canvas_begin","Canvas_begin");
    TCanvas *Canvas_end   = new TCanvas("Canvas_end","Canvas_end");
    //========================================================
    //Define the PPAC TCanvas
    //========================================================
    TCanvas *Canvas_PPAC_T[3];
    TCanvas *Canvas_PPAC_X1[3];
    TCanvas *Canvas_PPAC_X2[3];
    TCanvas *Canvas_PPAC_Y1[3];
    TCanvas *Canvas_PPAC_Y2[3];
    TCanvas *Canvas_PPAC_T_E[3];

    for(int i=0;i<3;i++)
    {
      sprintf(T_name,"PPAC%d_T",(i+1));
      sprintf(X1_name,"PPAC%d_X1",(i+1));
      sprintf(X2_name,"PPAC%d_X2",(i+1));
      sprintf(Y1_name,"PPAC%d_Y1",(i+1));
      sprintf(Y2_name,"PPAC%d_Y2",(i+1));
      sprintf(T_Ename,"PPAC%d_T_Energy",(i+1));

      Canvas_PPAC_T[i]  = new TCanvas(T_name,T_name);
      Canvas_PPAC_X1[i] = new TCanvas(X1_name,X1_name);
      Canvas_PPAC_X2[i] = new TCanvas(X2_name,X2_name);
      Canvas_PPAC_Y1[i] = new TCanvas(Y1_name,Y1_name);
      Canvas_PPAC_Y2[i] = new TCanvas(Y2_name,Y2_name);
      Canvas_PPAC_T_E[i] = new TCanvas(T_Ename,T_Ename);
    }
    //========================================================
    //Define the SSD_Tele TCanvas
    //========================================================
    TCanvas *Canvas_SSD_L1_Energy[2][32]   = {{0}};
    TCanvas *Canvas_SSD_L2_F_Energy[2][32] = {{0}};
    TCanvas *Canvas_SSD_L2_B_Energy[2][32] = {{0}};
    TCanvas *Canvas_SSD_L3_Energy[2][9] = {{0}};
    TCanvas *Canvas_SSD_L2_F_T[2][32] = {{0}};

    for(int j=0;j<2;j++)
    {
      for(int i=0;i<32;i++)
      {
        sprintf(SSD_L1_E_name,"SSD%d_L1_E_CH%d",(j+1),i);
        sprintf(SSD_L2_F_E_name,"SSD%d_L2_F_E_CH%d",(j+1),i);
        sprintf(SSD_L2_B_E_name,"SSD%d_L2_B_E_CH%d",(j+1),i);
        sprintf(SSD_L2_F_T_name,"SSD%d_L2_F_T_CH%d",(j+1),i);

        Canvas_SSD_L1_Energy[j][i] = new TCanvas(SSD_L1_E_name,SSD_L1_E_name);
        Canvas_SSD_L2_F_Energy[j][i] = new TCanvas(SSD_L2_F_E_name,SSD_L2_F_E_name);
        Canvas_SSD_L2_B_Energy[j][i] = new TCanvas(SSD_L2_B_E_name,SSD_L2_B_E_name);
        Canvas_SSD_L2_F_T[j][i] = new TCanvas(SSD_L2_F_T_name,SSD_L2_F_T_name);
      }
      for(int i=0;i<9;i++)
      {
       sprintf(SSD_L3_E_name,"SSD%d_L3_E_CH%d",(j+1),i);
       Canvas_SSD_L3_Energy[j][i] = new TCanvas(SSD_L3_E_name,SSD_L3_E_name);
      }
    }
    //========================================================
    //Define the Small_Si_Tele TCanvas
    //========================================================
    TCanvas * Canvas_Small_Si_L1_Energy[4] = {0};
    TCanvas * Canvas_Small_Si_L2_Energy[4] = {0};
    TCanvas * Canvas_Small_Si_L3_Energy[4] = {0};
    TCanvas * Canvas_Small_Si_L2_T[4] = {0};

    for(int i=0;i<4;i++)
    {
      sprintf(Si_L1_E_name,"Si%d_L1_E",(i+1));
      sprintf(Si_L2_E_name,"Si%d_L2_E",(i+1));
      sprintf(Si_L3_E_name,"Si%d_L3_E",(i+1));
      sprintf(Si_L2_T_name,"Si%d_L2_T",(i+1));

      Canvas_Small_Si_L1_Energy[i] = new TCanvas(Si_L1_E_name,Si_L1_E_name);
      Canvas_Small_Si_L2_Energy[i] = new TCanvas(Si_L2_E_name,Si_L2_E_name);
      Canvas_Small_Si_L3_Energy[i] = new TCanvas(Si_L3_E_name,Si_L3_E_name);
      Canvas_Small_Si_L2_T[i] = new TCanvas(Si_L2_T_name,Si_L2_T_name);
    }
    //****************************************************************
    //****************************************************************
    //========================================================
      //PPAC histogram Draw in every Canvas
    //========================================================
    //PPAC
    for(int i=0;i<3;i++)
    {
      Canvas_PPAC_T[i]->cd();   gPad->SetLogy();  gPad->SetGrid();  h_PPAC_T[i]  ->Draw();
      Canvas_PPAC_X1[i]->cd();  gPad->SetLogy();  gPad->SetGrid();  h_PPAC_X1[i] ->Draw();
      Canvas_PPAC_X2[i]->cd();  gPad->SetLogy();  gPad->SetGrid();  h_PPAC_X2[i] ->Draw();
      Canvas_PPAC_Y1[i]->cd();  gPad->SetLogy();  gPad->SetGrid();  h_PPAC_Y1[i] ->Draw();
      Canvas_PPAC_Y2[i]->cd();  gPad->SetLogy();  gPad->SetGrid();  h_PPAC_Y2[i] ->Draw();
      Canvas_PPAC_T_E[i]->cd(); gPad->SetLogy();  gPad->SetGrid();  h_PPAC_T_E[i] ->Draw();
    }
    //SSD_Tele
    for(int j=0;j<2;j++)
    {
      for(int i=0;i<32;i++)
      {
        Canvas_SSD_L1_Energy[j][i]->cd();     gPad->SetLogy();  gPad->SetGrid();  h_SSD_L1_Energy[j][i]    ->Draw();
        Canvas_SSD_L2_F_Energy[j][i]->cd();   gPad->SetLogy();  gPad->SetGrid();  h_SSD_L2_F_Energy[j][i]  ->Draw();
        Canvas_SSD_L2_B_Energy[j][i]->cd();   gPad->SetLogy();  gPad->SetGrid();  h_SSD_L2_B_Energy[j][i]  ->Draw();
        Canvas_SSD_L2_F_T[j][i]->cd();        gPad->SetLogy();  gPad->SetGrid();  h_SSD_L2_F_T[j][i]       ->Draw();
      }
      for(int i=0;i<9;i++)
      {
       Canvas_SSD_L3_Energy[j][i]->cd();     gPad->SetLogy();  gPad->SetGrid();  h_SSD_L3_Energy[j][i]    ->Draw();
      }
    }
    //Small_Si_Tele
    for(int i=0;i<4;i++)
    {
      Canvas_Small_Si_L1_Energy[i]->cd();   gPad->SetLogy();  gPad->SetGrid();  h_Small_Si_L1_Energy[i]  ->Draw();
      Canvas_Small_Si_L2_Energy[i]->cd();   gPad->SetLogy();  gPad->SetGrid();  h_Small_Si_L2_Energy[i]  ->Draw();
      Canvas_Small_Si_L3_Energy[i]->cd();   gPad->SetLogy();  gPad->SetGrid();  h_Small_Si_L3_Energy[i]  ->Draw();
      Canvas_Small_Si_L2_T[i]->cd();        gPad->SetLogy();  gPad->SetGrid();  h_Small_Si_L2_T[i]       ->Draw();
    }


    //========================================================
    //Save the TCanvas
    //========================================================
    TString Canvas_name(rdfname);
    Canvas_name.ReplaceAll(".root", "");
    TString pathoutpdf = "./a_QA_pdf";//------------------------------------------------------->outputpath of pdf
    mkdir(pathoutpdf,0777);
    pathoutpdf += "/a_QA_";	//---------------------->"a_QA_" is the signature of qautify data
    pathoutpdf += Canvas_name;
    pathoutpdf += ".pdf";
    cout << pathoutpdf << endl;

    Canvas_begin->Print(pathoutpdf+'[');
    //PPAC
    for(int i=0;i<3;i++)
    {
      Canvas_PPAC_T[i]->Print(pathoutpdf);
      Canvas_PPAC_X1[i]->Print(pathoutpdf);
      Canvas_PPAC_X2[i]->Print(pathoutpdf);
      Canvas_PPAC_Y1[i]->Print(pathoutpdf);
      Canvas_PPAC_Y2[i]->Print(pathoutpdf);
      Canvas_PPAC_T_E[i]->Print(pathoutpdf);
    }
    //SSD_Tele
    for(int j=0;j<2;j++)
    {
      for(int i=0;i<32;i++)
      {
        Canvas_SSD_L1_Energy[j][i]->Print(pathoutpdf);
        Canvas_SSD_L2_F_Energy[j][i]->Print(pathoutpdf);
        Canvas_SSD_L2_B_Energy[j][i]->Print(pathoutpdf);
        Canvas_SSD_L2_F_T[j][i]->Print(pathoutpdf);
      }
      for(int i=0;i<9;i++)
      {
       Canvas_SSD_L3_Energy[j][i]->Print(pathoutpdf);
      }
    }
    //Small_Si_Tele
    for(int i=0;i<4;i++)
    {
      Canvas_Small_Si_L1_Energy[i]->Print(pathoutpdf);
      Canvas_Small_Si_L2_Energy[i]->Print(pathoutpdf);
      Canvas_Small_Si_L3_Energy[i]->Print(pathoutpdf);
      Canvas_Small_Si_L2_T[i]->Print(pathoutpdf);
    }
    Canvas_end->Print(pathoutpdf+']');
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
