#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TChain.h"
#include "TNtuple.h"
#include <TH2.h>

#include "TSystem.h"
#include "TCanvas.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <map>

using namespace std;


//______________________________________________________________________________
void PrintUsage(Char_t *name);
void WriteHistforSSD(TFile* FileOut, TH1D* hist[],  const Int_t numtel);
void DrawSSD(const Char_t* pdfpath, TCanvas* canvas[], TH1D* hist[], const Int_t numtel);
void DrawSSDLogy(const Char_t* pdfpath, TCanvas* canvas[], TH1D* hist[], const Int_t numtel);
void ReadTree(TChain* myChain, const Char_t* rootpath, const Char_t* pdfpath);


//_________________________________________________________________________
//   主函数从这里开始
int main(int argc, char *argv[])
{
  string dfname;
  string rawlist, rawfile;
  bool inter = false;
  vector<string> rawdfname;
  fstream listf;

  string datapath = "../../Fission2019_Data/MapRoot"; // 设置读取的.root文件的路径
  //________________________________________________________________________________
  // 设置读取文件的方式：argc == 1 单个文件处理； argc == 2 按listfilename列举的文件批量处理
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
  //____________________________________________
  // 将listfilename文件中的文件名存储到 vector 容器中
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
  //________________________________
  //  逐个文件读取，作循环，进行数据处理
  for(int i=0; i<rawdfname.size(); i++)
  {
    string rdfname = rawdfname[i];
    TString pathrdfn = datapath;
    pathrdfn += "/";
    pathrdfn += rdfname;
    cout << pathrdfn << endl;
    //________________________________
    //      设置输出 .root 文件路径 与 命名
    TString outrootpath = "./QC_HistRoot";  //输出.root文件保存在 QC_HistRoot文件夹下
    outrootpath += "/QC_";
    outrootpath += rdfname;
    cout << outrootpath << endl;
    //______________________________
    //      设置输出 .pdf 文件路径 与 命名
    TString pdf_name(rdfname);
    pdf_name.ReplaceAll(".root", "");
    //___________________________
    TString outpdfpath = "QC_pdf"; //输出.pdf文件保存在 QC_pdf_ReadTree文件夹下
    outpdfpath += "/QC_";	                  //QC_标记是质检文件
    outpdfpath += pdf_name;
    outpdfpath += ".pdf";
    cout << outpdfpath << endl;

  //  TFile f(pathrdfn.Data());
  //  TTree *fChain = (TTree*)gFile->Get("KrPb");//fChain can be a TChain
    TChain* myChain = (TChain*) new TChain("KrPb");
    myChain->Add(pathrdfn);

    ReadTree(myChain,outrootpath,outpdfpath);
  }

  return 0;
} // int main() 函数到此结束


//______________________________________________________________________________
void PrintUsage(char *name)
{
	cout<<"Usage: "<<name<<"  "<<endl;
	cout<<"\t Interactive mode." << endl;
	cout<<"Usage: "<<name<<" listfilename "<<endl;
	cout<<"\t 'listfilename' is a text file contains the 'raw data file names'.";
	cout<<endl;
}


//__________________________________________________________________
//      从这里开始编辑 ReadTree() 函数
void ReadTree( TChain* myChain, const char* rootpath, const char* pdfpath)
{
  int BIN_NUM = 4096;
  int LOW_CH  = 0;
  int HIGH_CH = 4096;

  TFile* FileOut = new TFile(rootpath, "RECREATE");

  TH1D *Hist_PPAC1_T   = new TH1D("PPAC1_T",  "PPAC1_T",  BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_PPAC1_X1  = new TH1D("PPAC1_X1", "PPAC1_X1", BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_PPAC1_X2  = new TH1D("PPAC1_X2", "PPAC1_X2", BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_PPAC1_Y1  = new TH1D("PPAC1_Y1", "PPAC1_Y1", BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_PPAC1_Y2  = new TH1D("PPAC1_Y2", "PPAC1_Y2", BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_PPAC1_T_E = new TH1D("PPAC1_T_E","PPAC1_T_E",BIN_NUM, LOW_CH, HIGH_CH);
  TH1D *Hist_PPAC2_T   = new TH1D("PPAC2_T",  "PPAC2_T",  BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_PPAC2_X1  = new TH1D("PPAC2_X1", "PPAC2_X1", BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_PPAC2_X2  = new TH1D("PPAC2_X2", "PPAC2_X2", BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_PPAC2_Y1  = new TH1D("PPAC2_Y1", "PPAC2_Y1", BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_PPAC2_Y2  = new TH1D("PPAC2_Y2", "PPAC2_Y2", BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_PPAC2_T_E = new TH1D("PPAC2_T_E","PPAC2_T_E",BIN_NUM, LOW_CH, HIGH_CH);
  TH1D *Hist_PPAC3_T   = new TH1D("PPAC3_T",  "PPAC3_T",  BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_PPAC3_X1  = new TH1D("PPAC3_X1", "PPAC3_X1", BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_PPAC3_X2  = new TH1D("PPAC3_X2", "PPAC3_X2", BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_PPAC3_Y1  = new TH1D("PPAC3_Y1", "PPAC3_Y1", BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_PPAC3_Y2  = new TH1D("PPAC3_Y2", "PPAC3_Y2", BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_PPAC3_T_E = new TH1D("PPAC3_T_E","PPAC3_T_E",BIN_NUM, LOW_CH, HIGH_CH);

  TH1D *Hist_AuSi1_L1T  = new TH1D("AuSi1_L1T", "AuSi1_L1T", BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_AuSi1_L1E  = new TH1D("AuSi1_L1E", "AuSi1_L1E", BIN_NUM, LOW_CH, HIGH_CH);
  TH1D *Hist_AuSi1_L2E  = new TH1D("AuSi1_L2E", "AuSi1_L2E", BIN_NUM, LOW_CH, HIGH_CH);
  TH1D *Hist_AuSi2_L1T  = new TH1D("AuSi2_L1T", "AuSi2_L1T", BIN_NUM-100, LOW_CH+100, HIGH_CH);
  TH1D *Hist_AuSi2_L1E  = new TH1D("AuSi2_L1E", "AuSi2_L1E", BIN_NUM, LOW_CH, HIGH_CH);
  TH1D *Hist_AuSi2_L2E  = new TH1D("AuSi2_L2E", "AuSi2_L2E", BIN_NUM, LOW_CH, HIGH_CH);

  TH1D * Hist_RF1 = new TH1D("RF1", "RF1", BIN_NUM, LOW_CH, HIGH_CH);
  TH1D * Hist_RF2 = new TH1D("RF2", "RF2", BIN_NUM, LOW_CH, HIGH_CH);

  TH1D * Hist_SSD1_L2F_T[16];
  TH1D * Hist_SSD1_L1S_E[16];
  TH1D * Hist_SSD1_L2F_E[16];
  TH1D * Hist_SSD1_L2B_E[16];
  TH1D * Hist_SSD1_L3A_E[9];
  TH1D * Hist_SSD2_L2F_T[16];
  TH1D * Hist_SSD2_L1S_E[16];
  TH1D * Hist_SSD2_L2F_E[16];
  TH1D * Hist_SSD2_L2B_E[16];
  TH1D * Hist_SSD2_L3A_E[9];
  TH1D * Hist_SSD3_L2F_T[16];
  TH1D * Hist_SSD3_L1S_E[16];
  TH1D * Hist_SSD3_L2F_E[16];
  TH1D * Hist_SSD3_L2B_E[16];
  TH1D * Hist_SSD3_L3A_E[9];
  TH1D * Hist_SSD4_L2F_T[16];
  TH1D * Hist_SSD4_L1S_E[16];
  TH1D * Hist_SSD4_L2F_E[16];
  TH1D * Hist_SSD4_L2B_E[16];
  TH1D * Hist_SSD4_L3A_E[9];
  for(int i=0; i<16; i++)
  {
    Hist_SSD1_L2F_T[i] = new TH1D(Form("SSD1_L2F_T_CH%02d", i), Form("SSD1_L2F_T_CH%02d", i), BIN_NUM-100, LOW_CH+100, HIGH_CH);
    Hist_SSD1_L1S_E[i] = new TH1D(Form("SSD1_L1S_E_CH%02d", i), Form("SSD1_L1S_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD1_L2F_E[i] = new TH1D(Form("SSD1_L2F_E_CH%02d", i), Form("SSD1_L2F_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD1_L2B_E[i] = new TH1D(Form("SSD1_L2B_E_CH%02d", i), Form("SSD1_L2B_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD2_L2F_T[i] = new TH1D(Form("SSD2_L2F_T_CH%02d", i), Form("SSD2_L2F_T_CH%02d", i), BIN_NUM-100, LOW_CH+100, HIGH_CH);
    Hist_SSD2_L1S_E[i] = new TH1D(Form("SSD2_L1S_E_CH%02d", i), Form("SSD2_L1S_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD2_L2F_E[i] = new TH1D(Form("SSD2_L2F_E_CH%02d", i), Form("SSD2_L2F_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD2_L2B_E[i] = new TH1D(Form("SSD2_L2B_E_CH%02d", i), Form("SSD2_L2B_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD3_L2F_T[i] = new TH1D(Form("SSD3_L2F_T_CH%02d", i), Form("SSD3_L2F_T_CH%02d", i), BIN_NUM-100, LOW_CH+100, HIGH_CH);
    Hist_SSD3_L1S_E[i] = new TH1D(Form("SSD3_L1S_E_CH%02d", i), Form("SSD3_L1S_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD3_L2F_E[i] = new TH1D(Form("SSD3_L2F_E_CH%02d", i), Form("SSD3_L2F_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD3_L2B_E[i] = new TH1D(Form("SSD3_L2B_E_CH%02d", i), Form("SSD3_L2B_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD4_L2F_T[i] = new TH1D(Form("SSD4_L2F_T_CH%02d", i), Form("SSD4_L2F_T_CH%02d", i), BIN_NUM-100, LOW_CH+100, HIGH_CH);
    Hist_SSD4_L1S_E[i] = new TH1D(Form("SSD4_L1S_E_CH%02d", i), Form("SSD4_L1S_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD4_L2F_E[i] = new TH1D(Form("SSD4_L2F_E_CH%02d", i), Form("SSD4_L2F_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD4_L2B_E[i] = new TH1D(Form("SSD4_L2B_E_CH%02d", i), Form("SSD4_L2B_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
  }
  for(int i=0; i<9; i++)
  {
    Hist_SSD1_L3A_E[i] = new TH1D(Form("SSD1_L3A_E_CH%d", i), Form("SSD1_L3A_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD2_L3A_E[i] = new TH1D(Form("SSD2_L3A_E_CH%d", i), Form("SSD2_L3A_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD3_L3A_E[i] = new TH1D(Form("SSD3_L3A_E_CH%d", i), Form("SSD3_L3A_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD4_L3A_E[i] = new TH1D(Form("SSD4_L3A_E_CH%d", i), Form("SSD4_L3A_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
  }

  //===========================================
  //  TTreeReader 方法遍历读取 Tree 的所有branch
  TTreeReader myReader(myChain);
  TTreeReaderValue<Int_t> PPAC1_T(myReader,"PPAC1_T");
  TTreeReaderValue<Int_t> PPAC1_X1(myReader,"PPAC1_X1");
  TTreeReaderValue<Int_t> PPAC1_X2(myReader,"PPAC1_X2");
  TTreeReaderValue<Int_t> PPAC1_Y1(myReader,"PPAC1_Y1");
  TTreeReaderValue<Int_t> PPAC1_Y2(myReader,"PPAC1_Y2");
  TTreeReaderValue<Int_t> PPAC1_T_Energy(myReader,"PPAC1_T_Energy");
  TTreeReaderValue<Int_t> PPAC2_T(myReader,"PPAC2_T");
  TTreeReaderValue<Int_t> PPAC2_X1(myReader,"PPAC2_X1");
  TTreeReaderValue<Int_t> PPAC2_X2(myReader,"PPAC2_X2");
  TTreeReaderValue<Int_t> PPAC2_Y1(myReader,"PPAC2_Y1");
  TTreeReaderValue<Int_t> PPAC2_Y2(myReader,"PPAC2_Y2");
  TTreeReaderValue<Int_t> PPAC2_T_Energy(myReader,"PPAC2_T_Energy");
  TTreeReaderValue<Int_t> PPAC3_T(myReader,"PPAC3_T");
  TTreeReaderValue<Int_t> PPAC3_X1(myReader,"PPAC3_X1");
  TTreeReaderValue<Int_t> PPAC3_X2(myReader,"PPAC3_X2");
  TTreeReaderValue<Int_t> PPAC3_Y1(myReader,"PPAC3_Y1");
  TTreeReaderValue<Int_t> PPAC3_Y2(myReader,"PPAC3_Y2");
  TTreeReaderValue<Int_t> PPAC3_T_Energy(myReader,"PPAC3_T_Energy");

  TTreeReaderValue<Int_t> AuSi1_L1E(myReader,"AuSi1_L1E");
  TTreeReaderValue<Int_t> AuSi1_L2E(myReader,"AuSi1_L2E");
  TTreeReaderValue<Int_t> AuSi1_L1T(myReader,"AuSi1_L1T");
  TTreeReaderValue<Int_t> AuSi2_L1E(myReader,"AuSi2_L1E");
  TTreeReaderValue<Int_t> AuSi2_L2E(myReader,"AuSi2_L2E");
  TTreeReaderValue<Int_t> AuSi2_L1T(myReader,"AuSi2_L1T");

  TTreeReaderValue<Int_t> RF1(myReader,"RF1");
  TTreeReaderValue<Int_t> RF2(myReader,"RF2");

  TTreeReaderArray<Int_t> SSD1_L2F_T(myReader,"SSD1_L2F_T");
  TTreeReaderArray<Int_t> SSD1_L1S_E(myReader,"SSD1_L1S_E");
  TTreeReaderArray<Int_t> SSD1_L2F_E(myReader,"SSD1_L2F_E");
  TTreeReaderArray<Int_t> SSD1_L2B_E(myReader,"SSD1_L2B_E");
  TTreeReaderArray<Int_t> SSD1_L3A_E(myReader,"SSD1_L3A_E");
  TTreeReaderArray<Int_t> SSD2_L2F_T(myReader,"SSD2_L2F_T");
  TTreeReaderArray<Int_t> SSD2_L1S_E(myReader,"SSD2_L1S_E");
  TTreeReaderArray<Int_t> SSD2_L2F_E(myReader,"SSD2_L2F_E");
  TTreeReaderArray<Int_t> SSD2_L2B_E(myReader,"SSD2_L2B_E");
  TTreeReaderArray<Int_t> SSD2_L3A_E(myReader,"SSD2_L3A_E");
  TTreeReaderArray<Int_t> SSD3_L2F_T(myReader,"SSD3_L2F_T");
  TTreeReaderArray<Int_t> SSD3_L1S_E(myReader,"SSD3_L1S_E");
  TTreeReaderArray<Int_t> SSD3_L2F_E(myReader,"SSD3_L2F_E");
  TTreeReaderArray<Int_t> SSD3_L2B_E(myReader,"SSD3_L2B_E");
  TTreeReaderArray<Int_t> SSD3_L3A_E(myReader,"SSD3_L3A_E");
  TTreeReaderArray<Int_t> SSD4_L2F_T(myReader,"SSD4_L2F_T");
  TTreeReaderArray<Int_t> SSD4_L1S_E(myReader,"SSD4_L1S_E");
  TTreeReaderArray<Int_t> SSD4_L2F_E(myReader,"SSD4_L2F_E");
  TTreeReaderArray<Int_t> SSD4_L2B_E(myReader,"SSD4_L2B_E");
  TTreeReaderArray<Int_t> SSD4_L3A_E(myReader,"SSD4_L3A_E");

  //=======================
  //   填充直方图
  Int_t nentries = 0;
  while(myReader.Next())
  {
    Hist_PPAC1_T   ->Fill(*PPAC1_T);
    Hist_PPAC1_X1  ->Fill(*PPAC1_X1);
    Hist_PPAC1_X2  ->Fill(*PPAC1_X2);
    Hist_PPAC1_Y1  ->Fill(*PPAC1_Y1);
    Hist_PPAC1_Y2  ->Fill(*PPAC1_Y2);
    Hist_PPAC1_T_E ->Fill(*PPAC1_T_Energy);
    Hist_PPAC2_T   ->Fill(*PPAC2_T);
    Hist_PPAC2_X1  ->Fill(*PPAC2_X1);
    Hist_PPAC2_X2  ->Fill(*PPAC2_X2 );
    Hist_PPAC2_Y1  ->Fill(*PPAC2_Y1);
    Hist_PPAC2_Y2  ->Fill(*PPAC2_Y2);
    Hist_PPAC2_T_E ->Fill(*PPAC2_T_Energy);
    Hist_PPAC3_T   ->Fill(*PPAC3_T);
    Hist_PPAC3_X1  ->Fill(*PPAC3_X1);
    Hist_PPAC3_X2  ->Fill(*PPAC3_X2);
    Hist_PPAC3_Y1  ->Fill(*PPAC3_Y1);
    Hist_PPAC3_Y2  ->Fill(*PPAC3_Y2);
    Hist_PPAC3_T_E ->Fill(*PPAC3_T_Energy);

    Hist_AuSi1_L1T  ->Fill(*AuSi1_L1T);
    Hist_AuSi1_L1E  ->Fill(*AuSi1_L1E);
    Hist_AuSi1_L2E  ->Fill(*AuSi1_L2E);
    Hist_AuSi2_L1T  ->Fill(*AuSi2_L1T);
    Hist_AuSi2_L1E  ->Fill(*AuSi2_L1E);
    Hist_AuSi2_L2E  ->Fill(*AuSi2_L2E);

    Hist_RF1 ->Fill(*RF1);
    Hist_RF2 ->Fill(*RF2);

    for(int i=0; i<16; i++)
    {
      Hist_SSD1_L2F_T[i] ->Fill(SSD1_L2F_T[i]);
      Hist_SSD1_L1S_E[i] ->Fill(SSD1_L1S_E[i]);
      Hist_SSD1_L2F_E[i] ->Fill(SSD1_L2F_E[i]);
      Hist_SSD1_L2B_E[i] ->Fill(SSD1_L2B_E[i]);
      Hist_SSD2_L2F_T[i] ->Fill(SSD2_L2F_T[i]);
      Hist_SSD2_L1S_E[i] ->Fill(SSD2_L1S_E[i]);
      Hist_SSD2_L2F_E[i] ->Fill(SSD2_L2F_E[i]);
      Hist_SSD2_L2B_E[i] ->Fill(SSD2_L2B_E[i]);
      Hist_SSD3_L2F_T[i] ->Fill(SSD3_L2F_T[i]);
      Hist_SSD3_L1S_E[i] ->Fill(SSD3_L1S_E[i]);
      Hist_SSD3_L2F_E[i] ->Fill(SSD3_L2F_E[i]);
      Hist_SSD3_L2B_E[i] ->Fill(SSD3_L2B_E[i]);
      Hist_SSD4_L2F_T[i] ->Fill(SSD4_L2F_T[i]);
      Hist_SSD4_L1S_E[i] ->Fill(SSD4_L1S_E[i]);
      Hist_SSD4_L2F_E[i] ->Fill(SSD4_L2F_E[i]);
      Hist_SSD4_L2B_E[i] ->Fill(SSD4_L2B_E[i]);
    }
    for(int i=0; i<9; i++)
    {
      Hist_SSD1_L3A_E[i] ->Fill(SSD1_L3A_E[i]);
      Hist_SSD2_L3A_E[i] ->Fill(SSD2_L3A_E[i]);
      Hist_SSD3_L3A_E[i] ->Fill(SSD3_L3A_E[i]);
      Hist_SSD4_L3A_E[i] ->Fill(SSD4_L3A_E[i]);
    }
    nentries++;
  }   // close for while(myReader.Next())
  cout<< nentries <<"  "<<"entries analyzed" <<endl;
    //===========================================================
    //     将所有的直方图写入到 FileOut 文件中
    FileOut->WriteTObject(Hist_PPAC1_T,   Hist_PPAC1_T->GetName());
    FileOut->WriteTObject(Hist_PPAC1_X1,  Hist_PPAC1_X1->GetName());
    FileOut->WriteTObject(Hist_PPAC1_X2,  Hist_PPAC1_X2->GetName());
    FileOut->WriteTObject(Hist_PPAC1_Y1,  Hist_PPAC1_Y1->GetName());
    FileOut->WriteTObject(Hist_PPAC1_Y2,  Hist_PPAC1_Y2->GetName());
    FileOut->WriteTObject(Hist_PPAC1_T_E, Hist_PPAC1_T_E->GetName());
    FileOut->WriteTObject(Hist_PPAC2_T,   Hist_PPAC2_T->GetName());
    FileOut->WriteTObject(Hist_PPAC2_X1,  Hist_PPAC2_X1->GetName());
    FileOut->WriteTObject(Hist_PPAC2_X2,  Hist_PPAC2_X2->GetName());
    FileOut->WriteTObject(Hist_PPAC2_Y1,  Hist_PPAC2_Y1->GetName());
    FileOut->WriteTObject(Hist_PPAC2_Y2,  Hist_PPAC2_Y2->GetName());
    FileOut->WriteTObject(Hist_PPAC2_T_E, Hist_PPAC2_T_E->GetName());
    FileOut->WriteTObject(Hist_PPAC3_T,   Hist_PPAC3_T->GetName());
    FileOut->WriteTObject(Hist_PPAC3_X1,  Hist_PPAC3_X1->GetName());
    FileOut->WriteTObject(Hist_PPAC3_X2,  Hist_PPAC3_X2->GetName());
    FileOut->WriteTObject(Hist_PPAC3_Y1,  Hist_PPAC3_Y1->GetName());
    FileOut->WriteTObject(Hist_PPAC3_Y2,  Hist_PPAC3_Y2->GetName());
    FileOut->WriteTObject(Hist_PPAC3_T_E, Hist_PPAC3_T_E->GetName());

    FileOut->WriteTObject(Hist_AuSi1_L1T, Hist_AuSi1_L1T->GetName());
    FileOut->WriteTObject(Hist_AuSi1_L1E, Hist_AuSi1_L1E->GetName());
    FileOut->WriteTObject(Hist_AuSi1_L2E, Hist_AuSi1_L2E->GetName());
    FileOut->WriteTObject(Hist_AuSi2_L1T, Hist_AuSi2_L1T->GetName());
    FileOut->WriteTObject(Hist_AuSi2_L1E, Hist_AuSi2_L1E->GetName());
    FileOut->WriteTObject(Hist_AuSi2_L2E, Hist_AuSi2_L2E->GetName());

    FileOut->WriteTObject(Hist_RF1, Hist_RF1->GetName());
    FileOut->WriteTObject(Hist_RF2, Hist_RF2->GetName());

    //=====================
    //  for SSD1
    WriteHistforSSD(FileOut, Hist_SSD1_L1S_E, 16);
    WriteHistforSSD(FileOut, Hist_SSD1_L1S_E, 16);
    WriteHistforSSD(FileOut, Hist_SSD1_L2F_E, 16);
    WriteHistforSSD(FileOut, Hist_SSD1_L2B_E, 16);
    WriteHistforSSD(FileOut, Hist_SSD1_L3A_E, 9);
    WriteHistforSSD(FileOut, Hist_SSD2_L2F_T, 16);
    WriteHistforSSD(FileOut, Hist_SSD2_L1S_E, 16);
    WriteHistforSSD(FileOut, Hist_SSD2_L2F_E, 16);
    WriteHistforSSD(FileOut, Hist_SSD2_L2B_E, 16);
    WriteHistforSSD(FileOut, Hist_SSD2_L3A_E, 9);
    WriteHistforSSD(FileOut, Hist_SSD3_L2F_T, 16);
    WriteHistforSSD(FileOut, Hist_SSD3_L1S_E, 16);
    WriteHistforSSD(FileOut, Hist_SSD3_L2F_E, 16);
    WriteHistforSSD(FileOut, Hist_SSD3_L2B_E, 16);
    WriteHistforSSD(FileOut, Hist_SSD3_L3A_E, 9);
    WriteHistforSSD(FileOut, Hist_SSD4_L2F_T, 16);
    WriteHistforSSD(FileOut, Hist_SSD4_L1S_E, 16);
    WriteHistforSSD(FileOut, Hist_SSD4_L2F_E, 16);
    WriteHistforSSD(FileOut, Hist_SSD4_L2B_E, 16);
    WriteHistforSSD(FileOut, Hist_SSD4_L3A_E, 9);

    //=========================================================================
    //     定义Canvas
    TCanvas *Canvas_begin = new TCanvas("Canvas_begin","Canvas_begin");
    TCanvas *Canvas_end   = new TCanvas("Canvas_end","Canvas_end");

    TCanvas *Canvas_PPAC1_T   = new TCanvas("PPAC1_T", "PPAC1_T");
    TCanvas *Canvas_PPAC1_X1  = new TCanvas("PPAC1_X1", "PPAC1_X1");
    TCanvas *Canvas_PPAC1_X2  = new TCanvas("PPAC1_X2", "PPAC1_X2");
    TCanvas *Canvas_PPAC1_Y1  = new TCanvas("PPAC1_Y1", "PPAC1_Y1");
    TCanvas *Canvas_PPAC1_Y2  = new TCanvas("PPAC1_Y2", "PPAC1_Y2");
    TCanvas *Canvas_PPAC1_T_E = new TCanvas("PPAC1_T_E","PPAC1_T_E");
    TCanvas *Canvas_PPAC2_T   = new TCanvas("PPAC2_T", "PPAC2_T");
    TCanvas *Canvas_PPAC2_X1  = new TCanvas("PPAC2_X1", "PPAC2_X1");
    TCanvas *Canvas_PPAC2_X2  = new TCanvas("PPAC2_X2", "PPAC2_X2");
    TCanvas *Canvas_PPAC2_Y1  = new TCanvas("PPAC2_Y1", "PPAC2_Y1");
    TCanvas *Canvas_PPAC2_Y2  = new TCanvas("PPAC2_Y2", "PPAC2_Y2");
    TCanvas *Canvas_PPAC2_T_E = new TCanvas("PPAC2_T_E","PPAC2_T_E");
    TCanvas *Canvas_PPAC3_T   = new TCanvas("PPAC3_T", "PPAC3_T");
    TCanvas *Canvas_PPAC3_X1  = new TCanvas("PPAC3_X1", "PPAC3_X1");
    TCanvas *Canvas_PPAC3_X2  = new TCanvas("PPAC3_X2", "PPAC3_X2");
    TCanvas *Canvas_PPAC3_Y1  = new TCanvas("PPAC3_Y1", "PPAC3_Y1");
    TCanvas *Canvas_PPAC3_Y2  = new TCanvas("PPAC3_Y2", "PPAC3_Y2");
    TCanvas *Canvas_PPAC3_T_E = new TCanvas("PPAC3_T_E","PPAC3_T_E");

    TCanvas *Canvas_AuSi1_L1T = new TCanvas("AuSi1_L1T","AuSi1_L1T");
    TCanvas *Canvas_AuSi1_L1E = new TCanvas("AuSi1_L1E","AuSi1_L1E");
    TCanvas *Canvas_AuSi1_L2E = new TCanvas("AuSi1_L2E","AuSi1_L2E");
    TCanvas *Canvas_AuSi2_L1T = new TCanvas("AuSi2_L1T","AuSi2_L1T");
    TCanvas *Canvas_AuSi2_L1E = new TCanvas("AuSi2_L1E","AuSi2_L1E");
    TCanvas *Canvas_AuSi2_L2E = new TCanvas("AuSi2_L2E","AuSi2_L2E");

    TCanvas * Canvas_RF1 = new TCanvas("RF1", "RF1");
    TCanvas * Canvas_RF2 = new TCanvas("RF2", "RF2");

    TCanvas *Canvas_SSD1_L2F_T[16];
    TCanvas *Canvas_SSD1_L1S_E[16];
    TCanvas *Canvas_SSD1_L2F_E[16];
    TCanvas *Canvas_SSD1_L2B_E[16];
    TCanvas *Canvas_SSD1_L3A_E[9];
    TCanvas *Canvas_SSD2_L2F_T[16];
    TCanvas *Canvas_SSD2_L1S_E[16];
    TCanvas *Canvas_SSD2_L2F_E[16];
    TCanvas *Canvas_SSD2_L2B_E[16];
    TCanvas *Canvas_SSD2_L3A_E[9];
    TCanvas *Canvas_SSD3_L2F_T[16];
    TCanvas *Canvas_SSD3_L1S_E[16];
    TCanvas *Canvas_SSD3_L2F_E[16];
    TCanvas *Canvas_SSD3_L2B_E[16];
    TCanvas *Canvas_SSD3_L3A_E[9];
    TCanvas *Canvas_SSD4_L2F_T[16];
    TCanvas *Canvas_SSD4_L1S_E[16];
    TCanvas *Canvas_SSD4_L2F_E[16];
    TCanvas *Canvas_SSD4_L2B_E[16];
    TCanvas *Canvas_SSD4_L3A_E[9];
    for(int i=0; i<16; i++)
    {
      Canvas_SSD1_L2F_T[i] = new TCanvas(Form("SSD1_L2F_T_CH%d",i), Form("SSD1_L2F_T_CH%d",i));
      Canvas_SSD1_L1S_E[i] = new TCanvas(Form("SSD1_L1S_E_CH%d",i), Form("SSD1_L1S_E_CH%d",i));
      Canvas_SSD1_L2F_E[i] = new TCanvas(Form("SSD1_L2F_E_CH%d",i), Form("SSD1_L2F_E_CH%d",i));
      Canvas_SSD1_L2B_E[i] = new TCanvas(Form("SSD1_L2B_E_CH%d",i), Form("SSD1_L2B_E_CH%d",i));
      Canvas_SSD2_L2F_T[i] = new TCanvas(Form("SSD2_L2F_T_CH%d",i), Form("SSD2_L2F_T_CH%d",i));
      Canvas_SSD2_L1S_E[i] = new TCanvas(Form("SSD2_L1S_E_CH%d",i), Form("SSD2_L1S_E_CH%d",i));
      Canvas_SSD2_L2F_E[i] = new TCanvas(Form("SSD2_L2F_E_CH%d",i), Form("SSD2_L2F_E_CH%d",i));
      Canvas_SSD2_L2B_E[i] = new TCanvas(Form("SSD2_L2B_E_CH%d",i), Form("SSD2_L2B_E_CH%d",i));
      Canvas_SSD3_L2F_T[i] = new TCanvas(Form("SSD3_L2F_T_CH%d",i), Form("SSD3_L2F_T_CH%d",i));
      Canvas_SSD3_L1S_E[i] = new TCanvas(Form("SSD3_L1S_E_CH%d",i), Form("SSD3_L1S_E_CH%d",i));
      Canvas_SSD3_L2F_E[i] = new TCanvas(Form("SSD3_L2F_E_CH%d",i), Form("SSD3_L2F_E_CH%d",i));
      Canvas_SSD3_L2B_E[i] = new TCanvas(Form("SSD3_L2B_E_CH%d",i), Form("SSD3_L2B_E_CH%d",i));
      Canvas_SSD4_L2F_T[i] = new TCanvas(Form("SSD4_L2F_T_CH%d",i), Form("SSD4_L2F_T_CH%d",i));
      Canvas_SSD4_L1S_E[i] = new TCanvas(Form("SSD4_L1S_E_CH%d",i), Form("SSD4_L1S_E_CH%d",i));
      Canvas_SSD4_L2F_E[i] = new TCanvas(Form("SSD4_L2F_E_CH%d",i), Form("SSD4_L2F_E_CH%d",i));
      Canvas_SSD4_L2B_E[i] = new TCanvas(Form("SSD4_L2B_E_CH%d",i), Form("SSD4_L2B_E_CH%d",i));
    }
    for(int i=0; i<9; i++)
    {
      Canvas_SSD1_L3A_E[i] = new TCanvas(Form("SSD1_L3A_E_CH%d",i), Form("SSD1_L3A_E_CH%d",i));
      Canvas_SSD2_L3A_E[i] = new TCanvas(Form("SSD2_L3A_E_CH%d",i), Form("SSD2_L3A_E_CH%d",i));
      Canvas_SSD3_L3A_E[i] = new TCanvas(Form("SSD3_L3A_E_CH%d",i), Form("SSD3_L3A_E_CH%d",i));
      Canvas_SSD4_L3A_E[i] = new TCanvas(Form("SSD4_L3A_E_CH%d",i), Form("SSD4_L3A_E_CH%d",i));
    }

    //=====================================================
    //  Draw and Save the Canvas
    //  注意：在void ReadTree()中，pdfpath是 const char* 类型
    //        Canvas_begin->Print(pdfpath+'[')会报错
    TString string_pfgpath_begin = pdfpath;
    string_pfgpath_begin +='[';
    Canvas_begin->Print(string_pfgpath_begin);

    Canvas_PPAC1_T->cd(); Hist_PPAC1_T->Draw(); Canvas_PPAC1_T->Print(pdfpath);

    Canvas_PPAC1_X1  ->cd(); Hist_PPAC1_X1 ->Draw(); Canvas_PPAC1_X1 ->Print(pdfpath);
    Canvas_PPAC1_X2  ->cd(); Hist_PPAC1_X2 ->Draw(); Canvas_PPAC1_X2 ->Print(pdfpath);
    Canvas_PPAC1_Y1  ->cd(); Hist_PPAC1_Y1 ->Draw(); Canvas_PPAC1_Y1 ->Print(pdfpath);
    Canvas_PPAC1_Y2  ->cd(); Hist_PPAC1_Y2 ->Draw(); Canvas_PPAC1_Y2 ->Print(pdfpath);
    Canvas_PPAC1_T_E ->cd(); gPad->SetLogy(); Hist_PPAC1_T_E->Draw(); Canvas_PPAC1_T_E->Print(pdfpath);

    Canvas_PPAC2_T   ->cd(); Hist_PPAC2_T  ->Draw(); Canvas_PPAC2_T  ->Print(pdfpath);
    Canvas_PPAC2_X1  ->cd(); Hist_PPAC2_X1 ->Draw(); Canvas_PPAC2_X1 ->Print(pdfpath);
    Canvas_PPAC2_X2  ->cd(); Hist_PPAC2_X2 ->Draw(); Canvas_PPAC2_X2 ->Print(pdfpath);
    Canvas_PPAC2_Y1  ->cd(); Hist_PPAC2_Y1 ->Draw(); Canvas_PPAC2_Y1 ->Print(pdfpath);
    Canvas_PPAC2_Y2  ->cd(); Hist_PPAC2_Y2 ->Draw(); Canvas_PPAC2_Y2 ->Print(pdfpath);
    Canvas_PPAC2_T_E ->cd(); gPad->SetLogy(); Hist_PPAC2_T_E->Draw(); Canvas_PPAC2_T_E->Print(pdfpath);

    Canvas_PPAC3_T   ->cd(); Hist_PPAC3_T  ->Draw(); Canvas_PPAC3_T  ->Print(pdfpath);
    Canvas_PPAC3_X1  ->cd(); Hist_PPAC3_X1 ->Draw(); Canvas_PPAC3_X1 ->Print(pdfpath);
    Canvas_PPAC3_X2  ->cd(); Hist_PPAC3_X2 ->Draw(); Canvas_PPAC3_X2 ->Print(pdfpath);
    Canvas_PPAC3_Y1  ->cd(); Hist_PPAC3_Y1 ->Draw(); Canvas_PPAC3_Y1 ->Print(pdfpath);
    Canvas_PPAC3_Y2  ->cd(); Hist_PPAC3_Y2 ->Draw(); Canvas_PPAC3_Y2 ->Print(pdfpath);
    Canvas_PPAC3_T_E ->cd(); gPad->SetLogy(); Hist_PPAC3_T_E->Draw(); Canvas_PPAC3_T_E->Print(pdfpath);

    Canvas_AuSi1_L1T ->cd(); Hist_AuSi1_L1T ->Draw(); Canvas_AuSi1_L1T ->Print(pdfpath);
    Canvas_AuSi1_L1E ->cd(); gPad->SetLogy(); Hist_AuSi1_L1E ->Draw(); Canvas_AuSi1_L1E ->Print(pdfpath);
    Canvas_AuSi1_L2E ->cd(); gPad->SetLogy(); Hist_AuSi1_L2E ->Draw(); Canvas_AuSi1_L2E ->Print(pdfpath);
    Canvas_AuSi2_L1T ->cd(); Hist_AuSi2_L1T ->Draw(); Canvas_AuSi2_L1T ->Print(pdfpath);
    Canvas_AuSi2_L1E ->cd(); gPad->SetLogy(); Hist_AuSi2_L1E ->Draw(); Canvas_AuSi2_L1E ->Print(pdfpath);
    Canvas_AuSi2_L2E ->cd(); gPad->SetLogy(); Hist_AuSi2_L2E ->Draw(); Canvas_AuSi2_L2E ->Print(pdfpath);

    Canvas_RF1 ->cd(); Hist_RF1 ->Draw(); Canvas_RF1 ->Print(pdfpath);
    Canvas_RF2 ->cd(); Hist_RF2 ->Draw(); Canvas_RF2 ->Print(pdfpath);

    DrawSSD(pdfpath,     Canvas_SSD1_L2F_T, Hist_SSD1_L2F_T, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD1_L1S_E, Hist_SSD1_L1S_E, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD1_L2F_E, Hist_SSD1_L2F_E, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD1_L2B_E, Hist_SSD1_L2B_E, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD1_L3A_E, Hist_SSD1_L3A_E, 9);
    DrawSSD(pdfpath,     Canvas_SSD2_L2F_T, Hist_SSD2_L2F_T, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD2_L1S_E, Hist_SSD2_L1S_E, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD2_L2F_E, Hist_SSD2_L2F_E, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD2_L2B_E, Hist_SSD2_L2B_E, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD2_L3A_E, Hist_SSD2_L3A_E, 9);
    DrawSSD(pdfpath,     Canvas_SSD3_L2F_T, Hist_SSD3_L2F_T, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD3_L1S_E, Hist_SSD3_L1S_E, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD3_L2F_E, Hist_SSD3_L2F_E, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD3_L2B_E, Hist_SSD3_L2B_E, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD3_L3A_E, Hist_SSD3_L3A_E, 9);
    DrawSSD(pdfpath,     Canvas_SSD4_L2F_T, Hist_SSD4_L2F_T, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD4_L1S_E, Hist_SSD4_L1S_E, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD4_L2F_E, Hist_SSD4_L2F_E, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD4_L2B_E, Hist_SSD4_L2B_E, 16);
    DrawSSDLogy(pdfpath, Canvas_SSD4_L3A_E, Hist_SSD4_L3A_E, 9);

    TString string_pfgpath_end = pdfpath;
    string_pfgpath_end +=']';
    Canvas_end->Print(string_pfgpath_end);

    FileOut->Close();
    return;
}

//______________________________________________________________________________
void WriteHistforSSD(TFile* FileOut, TH1D* hist[], const Int_t numtel)
{
  for(Int_t i=0; i<numtel; i++){
    FileOut->WriteTObject(hist[i], hist[i]->GetName());
  }
}

//______________________________________________________________________________
void DrawSSD(const Char_t* pdfpath, TCanvas* canvas[], TH1D* hist[], const Int_t numtel)
{
  for(Int_t i=0; i<numtel; i++){
    canvas[i]->cd(); hist[i] ->Draw(); canvas[i] ->Print(pdfpath);
  }
}

//______________________________________________________________________________
void DrawSSDLogy(const Char_t* pdfpath, TCanvas* canvas[], TH1D* hist[], const Int_t numtel)
{
  for(Int_t i=0; i<numtel; i++){
    canvas[i]->cd(); gPad->SetLogy(); hist[i] ->Draw(); canvas[i] ->Print(pdfpath);
  }
}
