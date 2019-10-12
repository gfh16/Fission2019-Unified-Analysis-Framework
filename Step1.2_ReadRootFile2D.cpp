///////////////////////////////////////////////
// An Raw2ROOT.cpp: main() of Raw2ROOT, used
// for offline data analysis.
// Han Jianlong (08/2012)
///////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Use detector map to convert RawRoot data to MapRoot data
///////////////////////////////////////////////////////////

#include "TApplication.h"
#include "TDAQApplication.h"
#include "TConfig.h"
#include "TDataFileReader.h"
#include "TDataAnalyser.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TSystem.h"
#include "TCanvas.h"

#include "TBoard.h"
#include "TModV830AC.h"
#include "TModV785.h"
#include "TModV785N.h"
#include "TModV775.h"
#include "TModV775N.h"
#include "TModV792.h"

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

//const st ring gDataPath = "DAQDataPath";



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


  string datapath = "../../Fission2019_Data/RawRoot";

  //***************************************************
  //judgement the way to solve the data by parameter
  //***************************************************
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
    pathrdfn += '/';
    pathrdfn += rdfname;

	  TModV775N *vmod103 = new TModV775N();  // Timing for PPAC1 PPAC2 PPAC3
	  TModV785  *vmod104 = new TModV785();   // Energy for PPAC_T && Au-Si

	  TModV775  *vmod106 = new TModV775();  // Timing for Au-Si && 高频

  	TModV775  *vmod108 = new TModV775();  // TDC for SSD1_L2F_CH0-CH15 && SSD2_L2F_CH16-CH31
  	TModV775  *vmod109 = new TModV775();  // TDC for SSD3_L2F_CH0-CH15 && SSD4_L2F_CH16-CH31

	  TModV785  *vmod111 = new TModV785();  // ADC for SSD1_L1_S_CH0-CH15 && SSD1_L3_A_CH16-CH24
  	TModV785  *vmod112 = new TModV785();  // ADC for SSD1_L2_F_CH0-CH15 && SSD1_L2_B_CH16-CH31

	  TModV785  *vmod114 = new TModV785();  // ADC for SSD2_L1_S_CH0-CH15 && SSD2_L3_A_CH16-CH24
	  TModV785  *vmod115 = new TModV785();  // ADC for SSD2_L2_F_CH0-CH15 && SSD2_L2_B_CH16-CH31

	  TModV785  *vmod117 = new TModV785();  // ADC for SSD3_L1_S_CH0-CH15 && SSD3_L3_A_CH16-CH24
	  TModV785  *vmod118 = new TModV785();  // ADC for SSD3_L2_F_CH0-CH15 && SSD3_L2_B_CH16-CH31

	  TModV785  *vmod120 = new TModV785();  // ADC for SSD4_L1_S_CH0-CH15 && SSD4_L3_S_CH16-CH24
	  TModV785  *vmod121 = new TModV785();  // ADC for SSD4_L2_F_CH0-CH15 && SSD4_L2_B_CH16-CH31

    //read in the file
    TFile f(pathrdfn.Data());
    TTree *fChain = (TTree*)gFile->Get("RawData");//fChain can be a TChaisn
   	TBranch *bran103 = 0;
	  TBranch *bran104 = 0;
  	TBranch *bran106 = 0;
  	TBranch *bran108 = 0;
	  TBranch *bran109 = 0;
	  TBranch *bran111 = 0;
	  TBranch *bran112 = 0;
	  TBranch *bran114 = 0;
	  TBranch *bran115 = 0;
	  TBranch *bran117 = 0;
	  TBranch *bran118 = 0;
	  TBranch *bran120 = 0;
	  TBranch *bran121 = 0;
	  fChain->SetBranchAddress("Mod103_TModV775N", &vmod103, &bran103);
	  fChain->SetBranchAddress("Mod104_TModV785",  &vmod104, &bran104);
	  fChain->SetBranchAddress("Mod106_TModV775",  &vmod106, &bran106);
	  fChain->SetBranchAddress("Mod108_TModV775",  &vmod108, &bran108);
	  fChain->SetBranchAddress("Mod109_TModV775",  &vmod109, &bran109);
	  fChain->SetBranchAddress("Mod111_TModV785",  &vmod111, &bran111);
	  fChain->SetBranchAddress("Mod112_TModV785",  &vmod112, &bran112);
	  fChain->SetBranchAddress("Mod114_TModV785",  &vmod114, &bran114);
	  fChain->SetBranchAddress("Mod115_TModV785",  &vmod115, &bran115);
  	fChain->SetBranchAddress("Mod117_TModV785",  &vmod117, &bran117);
	  fChain->SetBranchAddress("Mod118_TModV785",  &vmod118, &bran118);
	  fChain->SetBranchAddress("Mod120_TModV785",  &vmod120, &bran120);
	  fChain->SetBranchAddress("Mod121_TModV785",  &vmod121, &bran121);

  	Long64_t nentries = fChain->GetEntriesFast();
  	cout << "nentries=" << nentries << endl;

    //output file
    TString pathoutfile = "../../Fission2019_Data/MapRoot";
    pathoutfile += "/aMap";	//"a" is the signature of first deal data
    pathoutfile += rdfname;

    TFile fout(pathoutfile.Data(),"recreate");

    TTree *tree = new TTree("KrPb","KrPb");


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
      sprintf(T_name, "PPAC%d_T", (i+1));  sprintf(T_nameI,"PPAC%d_T/I",(i+1));
      sprintf(X1_name,"PPAC%d_X1",(i+1));  sprintf(X1_nameI,"PPAC%d_X1/I",(i+1));
      sprintf(X2_name,"PPAC%d_X2",(i+1));  sprintf(X2_nameI,"PPAC%d_X2/I",(i+1));
      sprintf(Y1_name,"PPAC%d_Y1",(i+1));  sprintf(Y1_nameI,"PPAC%d_Y1/I",(i+1));
      sprintf(Y2_name,"PPAC%d_Y2",(i+1));  sprintf(Y2_nameI,"PPAC%d_Y2/I",(i+1));
      sprintf(T_E_name,"PPAC%d_T_Energy",(i+1));  sprintf(T_E_nameI,"PPAC%d_T_Energy/I",(i+1));

      tree->Branch(T_name,   &PPAC_T[i],  T_nameI);
      tree->Branch(X1_name,  &PPAC_X1[i], X1_nameI);
      tree->Branch(X2_name,  &PPAC_X2[i], X2_nameI);
      tree->Branch(Y1_name,  &PPAC_Y1[i], Y1_nameI);
      tree->Branch(Y2_name,  &PPAC_Y2[i], Y2_nameI);
      tree->Branch(T_E_name, &PPAC_T_Energy[i], T_E_nameI);
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
      sprintf(AuSi_L1E_name,"AuSi%d_L1E",(i+1));   sprintf(AuSi_L1E_nameI,"AuSi%d_L1E/I",(i+1));
      sprintf(AuSi_L2E_name,"AuSi%d_L2E",(i+1));   sprintf(AuSi_L2E_nameI,"AuSi%d_L2E/I",(i+1));
      sprintf(AuSi_L1T_name,"AuSi%d_L1T",(i+1));   sprintf(AuSi_L1T_nameI,"AuSi%d_L1T/I",(i+1));

      tree->Branch(AuSi_L1E_name, &AuSi_L1E[i], AuSi_L1E_nameI);
      tree->Branch(AuSi_L2E_name, &AuSi_L2E[i], AuSi_L2E_nameI);
      tree->Branch(AuSi_L1T_name, &AuSi_L1T[i], AuSi_L1T_nameI);
    }

    //==============================================
    //               For RF
    ////////////////////////////////////////////////
    int RF1 = 0;
    int RF2 = 0;

    tree->Branch("RF1",&RF1, "RF1/I");
    tree->Branch("RF2",&RF2, "RF2/I");


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

    ///////////////////////////////////////////////////////////////////////////////
    ///  对SSD数据填充方式1：CH0-CH15 填充到同一个branch

    for(int i=0; i<4; i++)
    {
      sprintf(SSD_L2F_T_name,"SSD%d_L2F_T",(i+1));    sprintf(SSD_L2F_T_nameI,"SSD%d_L2F_T[16]/I",(i+1));
      sprintf(SSD_L1S_E_name,"SSD%d_L1S_E",(i+1));    sprintf(SSD_L1S_E_nameI,"SSD%d_L1S_E[16]/I",(i+1));
      sprintf(SSD_L2F_E_name,"SSD%d_L2F_E",(i+1));    sprintf(SSD_L2F_E_nameI,"SSD%d_L2F_E[16]/I",(i+1));
      sprintf(SSD_L2B_E_name,"SSD%d_L2B_E",(i+1));    sprintf(SSD_L2B_E_nameI,"SSD%d_L2B_E[16]/I",(i+1));
      sprintf(SSD_L3A_E_name,"SSD%d_L3A_E",(i+1));    sprintf(SSD_L3A_E_nameI,"SSD%d_L3A_E[9]/I",(i+1));

      tree->Branch(SSD_L2F_T_name, SSD_L2F_T[i], SSD_L2F_T_nameI);
      tree->Branch(SSD_L1S_E_name, SSD_L1S_E[i], SSD_L1S_E_nameI);
      tree->Branch(SSD_L2F_E_name, SSD_L2F_E[i], SSD_L2F_E_nameI);
      tree->Branch(SSD_L2B_E_name, SSD_L2B_E[i], SSD_L2B_E_nameI);
      tree->Branch(SSD_L3A_E_name, SSD_L3A_E[i], SSD_L3A_E_nameI);
    }


///////////////////////////////////////////////////////////////////////////////
///  对SSD数据填充方式2：CH0-CH15 单独作为 branch

/*
for(int i=0; i<4; i++)
{
  for(int j=0; j<16; j++)
  {
    sprintf(SSD_L2F_T_name,"SSD%d_L2F_T_CH%d",(i+1),j);    sprintf(SSD_L2F_T_nameI,"SSD%d_L2F_T_CH%d/I",(i+1),j);
    tree->Branch(SSD_L2F_T_name, &SSD_L2F_T[i][j], SSD_L2F_T_nameI);
  }

  for(int j=0; j<16; j++)
  {
    sprintf(SSD_L1S_E_name,"SSD%d_L1S_E_CH%d",(i+1),j);    sprintf(SSD_L1S_E_nameI,"SSD%d_L1S_E_CH%d/I",(i+1),j);
    tree->Branch(SSD_L1S_E_name, &SSD_L1S_E[i][j], SSD_L1S_E_nameI);
  }

  for(int j=0; j<16; j++)
  {
    sprintf(SSD_L2F_E_name,"SSD%d_L2F_E_CH%d",(i+1),j);    sprintf(SSD_L2F_E_nameI,"SSD%d_L2_FE_CH%d/I",(i+1),j);
    tree->Branch(SSD_L2F_E_name, &SSD_L2F_E[i][j], SSD_L2F_E_nameI);
  }

  for(int j=0; j<16; j++)
  {
    sprintf(SSD_L2B_E_name,"SSD%d_L2B_E_CH%d",(i+1),j);    sprintf(SSD_L2B_E_nameI,"SSD%d_L2B_E_CH%d/I",(i+1),j);
    tree->Branch(SSD_L2B_E_name, &SSD_L2B_E[i][j], SSD_L2B_E_nameI);
  }

  for(int j=0;j<9;j++)
  {
   sprintf(SSD_L3A_E_name,"SSD%d_L3A_E_CH%d",(i+1),j);    sprintf(SSD_L3A_E_nameI,"SSD%d_L3A_E_CH%d/I",(i+1),j);
   tree->Branch(SSD_L3A_E_name, &SSD_L3A_E[i][j], SSD_L3A_E_nameI);
  }
}
*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


    //****************************************************************
    //loop on each Events in the rootfile
    //****************************************************************
    for (Long64_t ientry=0; ientry<nentries;ientry++)
    {
      Long64_t jentry = fChain->LoadTree(ientry);
      if (jentry < 0) break;
      fChain->GetEntry(ientry);

      ///////////////////////////////////////////
      //The map of the Channel to Detector
      //////////////////////////////////////////

      //////////////////
      // for PPAC
      //////////////////
      for(int i=0; i<3; i++)
      {
        PPAC_T[i]  = vmod103->chdata[5*i+0];
        PPAC_X1[i] = vmod103->chdata[5*i+1];
        PPAC_X2[i] = vmod103->chdata[5*i+2];
        PPAC_Y1[i] = vmod103->chdata[5*i+3];
        PPAC_Y2[i] = vmod103->chdata[5*i+4];

        PPAC_T_Energy[i] = vmod104->chdata[28+i];
      }

      ////////////////////////
      // for Au-Si
      ////////////////////////
      // 这里需要修改：run121-run432 T104020-T104023
      //            run89-run120  T104016-T104019
      for(int i=0; i<2; i++)
      {
        AuSi_L1E[i] = vmod104->chdata[20+2*i];
        AuSi_L2E[i] = vmod104->chdata[20+2*i+1];
        AuSi_L1T[i] = vmod106->chdata[i];
      }

      ///////////////////////
      //    for RF
      ///////////////////////
      RF1 = vmod106->chdata[2];
      RF2 = vmod106->chdata[3];

      ///////////////////////
      // for SSD L1 && L2
      //////////////////////
      for(int i=0; i<16; i++)
      {
        SSD_L2F_T[0][i] = vmod108->chdata[i];
        SSD_L1S_E[0][i] = vmod111->chdata[i];
    		SSD_L2F_E[0][i] = vmod112->chdata[i];
    		SSD_L2B_E[0][i] = vmod112->chdata[i+16];

        SSD_L2F_T[1][i] = vmod108->chdata[i+16];
        SSD_L1S_E[1][i] = vmod114->chdata[i];
    		SSD_L2F_E[1][i] = vmod115->chdata[i];
    		SSD_L2B_E[1][i] = vmod115->chdata[i+16];

        SSD_L2F_T[2][i] = vmod109->chdata[i];
        SSD_L1S_E[2][i] = vmod117->chdata[i];
    		SSD_L2F_E[2][i] = vmod118->chdata[i];
    		SSD_L2B_E[2][i] = vmod118->chdata[i+16];

        SSD_L2F_T[3][i] = vmod109->chdata[i+16];
        SSD_L1S_E[3][i] = vmod120->chdata[i];
    		SSD_L2F_E[3][i] = vmod121->chdata[i];
    		SSD_L2B_E[3][i] = vmod121->chdata[i+16];
      }

      //========================================================
      //SSD_Telescope_L3
      //========================================================
      for (int i=0; i<9; i++)
      {
        SSD_L3A_E[0][i] = vmod111->chdata[16+i];
        SSD_L3A_E[1][i] = vmod114->chdata[16+i];
        SSD_L3A_E[2][i] = vmod117->chdata[16+i];
        SSD_L3A_E[3][i] = vmod120->chdata[16+i];
      }

      tree->Fill();
    }

    f.Close();
    fout.cd();
    fout.Write();
    fout.Close();
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
