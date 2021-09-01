#include "../include/CSHINEQualityCheck.h"
using namespace std;


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
CSHINEQualityCheck::CSHINEQualityCheck()
{}

//______________________________________________________________________________
CSHINEQualityCheck::~CSHINEQualityCheck()
{}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEQualityCheck::QC_TreeReader(const char* pathlistfiles)
{
  std::vector<string>  filenamelist;

  ReadFileModule readfile;
  readfile.GetFileNamesFromFile(pathlistfiles, filenamelist);

  //__________________________________________
  //               逐个处理文件
  // !!! 重要 ！！！
  // 此处修改输入、输出文件路径
  for (Int_t i=0; i<Int_t(filenamelist.size()); i++)
  {
    TString pathrootout = Form("%sQualityCheck/", PATHROOTFILESFOLDER);
    pathrootout += "QC_";
    pathrootout += filenamelist[i];

    TString pdf_name = filenamelist[i];
    pdf_name.ReplaceAll(".root", ".pdf");
    TString pathpdfout = Form("%sQualityCheck_figures/", PATHROOTFILESFOLDER);
    pathpdfout  += "QC_";
    pathpdfout  += pdf_name;

    TString pathrootin = Form("%sMapRoot/", PATHROOTFILESFOLDER);
    pathrootin += filenamelist[i];

    TFile* rootfile = new TFile(pathrootin);
    TChain* myChain = (TChain*)rootfile->Get("KrPb");
    cout<<"正在处理文件  : "<<pathrootin<<endl;

    TreeReaderMethod(myChain, pathrootout, pathpdfout);
  }
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEQualityCheck::QC_BranchAdress(const char* pathlistfiles)
{
  std::vector<string>  filenamelist;

  ReadFileModule readfile;
  readfile.GetFileNamesFromFile(pathlistfiles, filenamelist);

  //__________________________________________
  //               逐个处理文件
  // !!! 重要 ！！！
  // 此处修改输入、输出文件路径
  for (Int_t i=0; i<Int_t(filenamelist.size()); i++)
  {
    TString pathrootout = Form("%sQualityCheck/", PATHROOTFILESFOLDER);
    pathrootout += "QC_";
    pathrootout += filenamelist[i];

    TString pdf_name = filenamelist[i];
    pdf_name.ReplaceAll(".root", ".pdf");
    TString pathpdfout = Form("%sQualityCheck_figures/", PATHROOTFILESFOLDER);
    pathpdfout  += "QC_";
    pathpdfout  += pdf_name;

    TString pathrootin = Form("%s/MapRoot/", PATHROOTFILESFOLDER);
    pathrootin += filenamelist[i];

    TFile* rootfile = new TFile(pathrootin);
    TChain* myChain = (TChain*)rootfile->Get("KrPb");
    cout<<"正在处理文件  : "<<pathrootin<<endl;

    ReadBranchMethod(myChain, pathrootout, pathpdfout);
  }
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEQualityCheck::QC_ReadHistToDraw(Int_t runnumber, const char* layertag)
{
  std::string pathfilein(Form("%sMapRoot/MapFission2019_Kr_Pb%04d.root",PATHROOTFILESFOLDER,runnumber));

  TCanvas* cans = new TCanvas("cans", "cans", 800, 600);
  TFile* filein = new TFile(pathfilein.c_str());
  TIter next(filein->GetListOfKeys());
  TKey* key;
  while ((key = (TKey*)next())) {
    TH1D* hist = (TH1D*)key->ReadObj();
    std::string histname(hist->GetName());
    if(histname.find("SSD")==std::string::npos) continue;
    if(histname.find(layertag)==std::string::npos) continue;
    printf("histname = %s\n", histname.c_str());

    cans->cd();
    hist->GetXaxis()->SetRangeUser(150, 400);
    hist->GetYaxis()->SetRangeUser(0, (hist->GetMaximum())*1.1);
    hist->Draw();

    cans->Modified();
    cans->Update();
  }
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEQualityCheck::TreeReaderMethod(TChain* mychain, const char* pathrootout, const char* pathpdfout)
{
  Int_t  BIN_NUM   =  4096;
  Int_t  LOW_CH    =  0;
  Int_t  HIGH_CH   =  4096;
  Int_t  BIN_SHIFT =  100;

  TimeAndPercentage time;

  Long64_t nentries = mychain->GetEntriesFast();
  cout << "nentries = " << nentries << endl;

  TFile* FileOut = new TFile(pathrootout, "RECREATE");

  TH1D* Hist_PPAC1_T    =  new TH1D("PPAC1_T",  "PPAC1_T",  BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_PPAC1_X1   =  new TH1D("PPAC1_X1", "PPAC1_X1", BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_PPAC1_X2   =  new TH1D("PPAC1_X2", "PPAC1_X2", BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_PPAC1_Y1   =  new TH1D("PPAC1_Y1", "PPAC1_Y1", BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_PPAC1_Y2   =  new TH1D("PPAC1_Y2", "PPAC1_Y2", BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_PPAC1_T_E  =  new TH1D("PPAC1_T_E","PPAC1_T_E",BIN_NUM,           LOW_CH,           HIGH_CH);
  TH1D* Hist_PPAC2_T    =  new TH1D("PPAC2_T",  "PPAC2_T",  BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_PPAC2_X1   =  new TH1D("PPAC2_X1", "PPAC2_X1", BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_PPAC2_X2   =  new TH1D("PPAC2_X2", "PPAC2_X2", BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_PPAC2_Y1   =  new TH1D("PPAC2_Y1", "PPAC2_Y1", BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_PPAC2_Y2   =  new TH1D("PPAC2_Y2", "PPAC2_Y2", BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_PPAC2_T_E  =  new TH1D("PPAC2_T_E","PPAC2_T_E",BIN_NUM,           LOW_CH,           HIGH_CH);
  TH1D* Hist_PPAC3_T    =  new TH1D("PPAC3_T",  "PPAC3_T",  BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_PPAC3_X1   =  new TH1D("PPAC3_X1", "PPAC3_X1", BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_PPAC3_X2   =  new TH1D("PPAC3_X2", "PPAC3_X2", BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_PPAC3_Y1   =  new TH1D("PPAC3_Y1", "PPAC3_Y1", BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_PPAC3_Y2   =  new TH1D("PPAC3_Y2", "PPAC3_Y2", BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_PPAC3_T_E  =  new TH1D("PPAC3_T_E","PPAC3_T_E",BIN_NUM,           LOW_CH,           HIGH_CH);

  TH1D* Hist_AuSi1_L1T  =  new TH1D("AuSi1_L1T", "AuSi1_L1T", BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_AuSi1_L1E  =  new TH1D("AuSi1_L1E", "AuSi1_L1E", BIN_NUM,           LOW_CH,           HIGH_CH);
  TH1D* Hist_AuSi1_L2E  =  new TH1D("AuSi1_L2E", "AuSi1_L2E", BIN_NUM,           LOW_CH,           HIGH_CH);
  TH1D* Hist_AuSi2_L1T  =  new TH1D("AuSi2_L1T", "AuSi2_L1T", BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
  TH1D* Hist_AuSi2_L1E  =  new TH1D("AuSi2_L1E", "AuSi2_L1E", BIN_NUM,           LOW_CH,           HIGH_CH);
  TH1D* Hist_AuSi2_L2E  =  new TH1D("AuSi2_L2E", "AuSi2_L2E", BIN_NUM,           LOW_CH,           HIGH_CH);

  TH1D* Hist_RF1 = new TH1D("RF1", "RF1", BIN_NUM, LOW_CH, HIGH_CH);
  TH1D* Hist_RF2 = new TH1D("RF2", "RF2", BIN_NUM, LOW_CH, HIGH_CH);

  TH1D* Hist_SSD1_L2F_T[16];
  TH1D* Hist_SSD1_L1S_E[16];
  TH1D* Hist_SSD1_L2F_E[16];
  TH1D* Hist_SSD1_L2B_E[16];
  TH1D* Hist_SSD1_L3A_E[9];
  TH1D* Hist_SSD2_L2F_T[16];
  TH1D* Hist_SSD2_L1S_E[16];
  TH1D* Hist_SSD2_L2F_E[16];
  TH1D* Hist_SSD2_L2B_E[16];
  TH1D* Hist_SSD2_L3A_E[9];
  TH1D* Hist_SSD3_L2F_T[16];
  TH1D* Hist_SSD3_L1S_E[16];
  TH1D* Hist_SSD3_L2F_E[16];
  TH1D* Hist_SSD3_L2B_E[16];
  TH1D* Hist_SSD3_L3A_E[9];
  TH1D* Hist_SSD4_L2F_T[16];
  TH1D* Hist_SSD4_L1S_E[16];
  TH1D* Hist_SSD4_L2F_E[16];
  TH1D* Hist_SSD4_L2B_E[16];
  TH1D* Hist_SSD4_L3A_E[9];
  for(int i=0; i<16; i++)
  {
    Hist_SSD1_L2F_T[i] = new TH1D(Form("SSD1_L2F_T_CH%02d", i), Form("SSD1_L2F_T_CH%02d", i), BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
    Hist_SSD1_L1S_E[i] = new TH1D(Form("SSD1_L1S_E_CH%02d", i), Form("SSD1_L1S_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD1_L2F_E[i] = new TH1D(Form("SSD1_L2F_E_CH%02d", i), Form("SSD1_L2F_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD1_L2B_E[i] = new TH1D(Form("SSD1_L2B_E_CH%02d", i), Form("SSD1_L2B_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD2_L2F_T[i] = new TH1D(Form("SSD2_L2F_T_CH%02d", i), Form("SSD2_L2F_T_CH%02d", i), BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
    Hist_SSD2_L1S_E[i] = new TH1D(Form("SSD2_L1S_E_CH%02d", i), Form("SSD2_L1S_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD2_L2F_E[i] = new TH1D(Form("SSD2_L2F_E_CH%02d", i), Form("SSD2_L2F_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD2_L2B_E[i] = new TH1D(Form("SSD2_L2B_E_CH%02d", i), Form("SSD2_L2B_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD3_L2F_T[i] = new TH1D(Form("SSD3_L2F_T_CH%02d", i), Form("SSD3_L2F_T_CH%02d", i), BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
    Hist_SSD3_L1S_E[i] = new TH1D(Form("SSD3_L1S_E_CH%02d", i), Form("SSD3_L1S_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD3_L2F_E[i] = new TH1D(Form("SSD3_L2F_E_CH%02d", i), Form("SSD3_L2F_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD3_L2B_E[i] = new TH1D(Form("SSD3_L2B_E_CH%02d", i), Form("SSD3_L2B_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD4_L2F_T[i] = new TH1D(Form("SSD4_L2F_T_CH%02d", i), Form("SSD4_L2F_T_CH%02d", i), BIN_NUM-BIN_SHIFT, LOW_CH+BIN_SHIFT, HIGH_CH);
    Hist_SSD4_L1S_E[i] = new TH1D(Form("SSD4_L1S_E_CH%02d", i), Form("SSD4_L1S_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD4_L2F_E[i] = new TH1D(Form("SSD4_L2F_E_CH%02d", i), Form("SSD4_L2F_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD4_L2B_E[i] = new TH1D(Form("SSD4_L2B_E_CH%02d", i), Form("SSD4_L2B_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
  }
  for(int i=0; i<9; i++)
  {
    Hist_SSD1_L3A_E[i] = new TH1D(Form("SSD1_L3A_E_CH%02d", i), Form("SSD1_L3A_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD2_L3A_E[i] = new TH1D(Form("SSD2_L3A_E_CH%02d", i), Form("SSD2_L3A_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD3_L3A_E[i] = new TH1D(Form("SSD3_L3A_E_CH%02d", i), Form("SSD3_L3A_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD4_L3A_E[i] = new TH1D(Form("SSD4_L3A_E_CH%02d", i), Form("SSD4_L3A_E_CH%02d", i), BIN_NUM, LOW_CH, HIGH_CH);
  }

  //____________________________________________________________________________
  //    TTreeReader 方法遍历读取 Tree 的所有branch
  TTreeReader myReader(mychain);
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

  //______________________________________
  //      填充直方图
  Int_t ientry = 0;
  while(myReader.Next())
  {
    ientry++;
    time.PrintPercentageAndRemainingTime(ientry, nentries);

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
  }   // close for while(myReader.Next())

    //__________________________________________________________________________
    //     将所有的直方图写入到 FileOut 文件中
    FileOut->WriteTObject(Hist_PPAC1_T,   Hist_PPAC1_T  ->GetName());
    FileOut->WriteTObject(Hist_PPAC1_X1,  Hist_PPAC1_X1 ->GetName());
    FileOut->WriteTObject(Hist_PPAC1_X2,  Hist_PPAC1_X2 ->GetName());
    FileOut->WriteTObject(Hist_PPAC1_Y1,  Hist_PPAC1_Y1 ->GetName());
    FileOut->WriteTObject(Hist_PPAC1_Y2,  Hist_PPAC1_Y2 ->GetName());
    FileOut->WriteTObject(Hist_PPAC1_T_E, Hist_PPAC1_T_E->GetName());
    FileOut->WriteTObject(Hist_PPAC2_T,   Hist_PPAC2_T  ->GetName());
    FileOut->WriteTObject(Hist_PPAC2_X1,  Hist_PPAC2_X1 ->GetName());
    FileOut->WriteTObject(Hist_PPAC2_X2,  Hist_PPAC2_X2 ->GetName());
    FileOut->WriteTObject(Hist_PPAC2_Y1,  Hist_PPAC2_Y1 ->GetName());
    FileOut->WriteTObject(Hist_PPAC2_Y2,  Hist_PPAC2_Y2 ->GetName());
    FileOut->WriteTObject(Hist_PPAC2_T_E, Hist_PPAC2_T_E->GetName());
    FileOut->WriteTObject(Hist_PPAC3_T,   Hist_PPAC3_T  ->GetName());
    FileOut->WriteTObject(Hist_PPAC3_X1,  Hist_PPAC3_X1 ->GetName());
    FileOut->WriteTObject(Hist_PPAC3_X2,  Hist_PPAC3_X2 ->GetName());
    FileOut->WriteTObject(Hist_PPAC3_Y1,  Hist_PPAC3_Y1 ->GetName());
    FileOut->WriteTObject(Hist_PPAC3_Y2,  Hist_PPAC3_Y2 ->GetName());
    FileOut->WriteTObject(Hist_PPAC3_T_E, Hist_PPAC3_T_E->GetName());

    FileOut->WriteTObject(Hist_AuSi1_L1T, Hist_AuSi1_L1T->GetName());
    FileOut->WriteTObject(Hist_AuSi1_L1E, Hist_AuSi1_L1E->GetName());
    FileOut->WriteTObject(Hist_AuSi1_L2E, Hist_AuSi1_L2E->GetName());
    FileOut->WriteTObject(Hist_AuSi2_L1T, Hist_AuSi2_L1T->GetName());
    FileOut->WriteTObject(Hist_AuSi2_L1E, Hist_AuSi2_L1E->GetName());
    FileOut->WriteTObject(Hist_AuSi2_L2E, Hist_AuSi2_L2E->GetName());

    FileOut->WriteTObject(Hist_RF1, Hist_RF1->GetName());
    FileOut->WriteTObject(Hist_RF2, Hist_RF2->GetName());

    //____________________________________________
    //                for SSD
    WriteHistforSSD(FileOut, Hist_SSD1_L2F_T, 16);
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
    TCanvas* Canvas_begin = new TCanvas("Canvas_begin","Canvas_begin");
    TCanvas* Canvas_end   = new TCanvas("Canvas_end"  ,"Canvas_end");

    TCanvas* Canvas_PPAC1_T   = new TCanvas("PPAC1_T",  "PPAC1_T");
    TCanvas* Canvas_PPAC1_X1  = new TCanvas("PPAC1_X1", "PPAC1_X1");
    TCanvas* Canvas_PPAC1_X2  = new TCanvas("PPAC1_X2", "PPAC1_X2");
    TCanvas* Canvas_PPAC1_Y1  = new TCanvas("PPAC1_Y1", "PPAC1_Y1");
    TCanvas* Canvas_PPAC1_Y2  = new TCanvas("PPAC1_Y2", "PPAC1_Y2");
    TCanvas* Canvas_PPAC1_T_E = new TCanvas("PPAC1_T_E","PPAC1_T_E");
    TCanvas* Canvas_PPAC2_T   = new TCanvas("PPAC2_T",  "PPAC2_T");
    TCanvas* Canvas_PPAC2_X1  = new TCanvas("PPAC2_X1", "PPAC2_X1");
    TCanvas* Canvas_PPAC2_X2  = new TCanvas("PPAC2_X2", "PPAC2_X2");
    TCanvas* Canvas_PPAC2_Y1  = new TCanvas("PPAC2_Y1", "PPAC2_Y1");
    TCanvas* Canvas_PPAC2_Y2  = new TCanvas("PPAC2_Y2", "PPAC2_Y2");
    TCanvas* Canvas_PPAC2_T_E = new TCanvas("PPAC2_T_E","PPAC2_T_E");
    TCanvas* Canvas_PPAC3_T   = new TCanvas("PPAC3_T",  "PPAC3_T");
    TCanvas* Canvas_PPAC3_X1  = new TCanvas("PPAC3_X1", "PPAC3_X1");
    TCanvas* Canvas_PPAC3_X2  = new TCanvas("PPAC3_X2", "PPAC3_X2");
    TCanvas* Canvas_PPAC3_Y1  = new TCanvas("PPAC3_Y1", "PPAC3_Y1");
    TCanvas* Canvas_PPAC3_Y2  = new TCanvas("PPAC3_Y2", "PPAC3_Y2");
    TCanvas* Canvas_PPAC3_T_E = new TCanvas("PPAC3_T_E","PPAC3_T_E");

    TCanvas* Canvas_AuSi1_L1T = new TCanvas("AuSi1_L1T","AuSi1_L1T");
    TCanvas* Canvas_AuSi1_L1E = new TCanvas("AuSi1_L1E","AuSi1_L1E");
    TCanvas* Canvas_AuSi1_L2E = new TCanvas("AuSi1_L2E","AuSi1_L2E");
    TCanvas* Canvas_AuSi2_L1T = new TCanvas("AuSi2_L1T","AuSi2_L1T");
    TCanvas* Canvas_AuSi2_L1E = new TCanvas("AuSi2_L1E","AuSi2_L1E");
    TCanvas* Canvas_AuSi2_L2E = new TCanvas("AuSi2_L2E","AuSi2_L2E");

    TCanvas* Canvas_RF1 = new TCanvas("RF1", "RF1");
    TCanvas* Canvas_RF2 = new TCanvas("RF2", "RF2");

    TCanvas* Canvas_SSD1_L2F_T[16];
    TCanvas* Canvas_SSD1_L1S_E[16];
    TCanvas* Canvas_SSD1_L2F_E[16];
    TCanvas* Canvas_SSD1_L2B_E[16];
    TCanvas* Canvas_SSD1_L3A_E[9];
    TCanvas* Canvas_SSD2_L2F_T[16];
    TCanvas* Canvas_SSD2_L1S_E[16];
    TCanvas* Canvas_SSD2_L2F_E[16];
    TCanvas* Canvas_SSD2_L2B_E[16];
    TCanvas* Canvas_SSD2_L3A_E[9];
    TCanvas* Canvas_SSD3_L2F_T[16];
    TCanvas* Canvas_SSD3_L1S_E[16];
    TCanvas* Canvas_SSD3_L2F_E[16];
    TCanvas* Canvas_SSD3_L2B_E[16];
    TCanvas* Canvas_SSD3_L3A_E[9];
    TCanvas* Canvas_SSD4_L2F_T[16];
    TCanvas* Canvas_SSD4_L1S_E[16];
    TCanvas* Canvas_SSD4_L2F_E[16];
    TCanvas* Canvas_SSD4_L2B_E[16];
    TCanvas* Canvas_SSD4_L3A_E[9];
    for(int i=0; i<16; i++)
    {
      Canvas_SSD1_L2F_T[i] = new TCanvas(Form("SSD1_L2F_T_CH%02d",i), Form("SSD1_L2F_T_CH%02d",i));
      Canvas_SSD1_L1S_E[i] = new TCanvas(Form("SSD1_L1S_E_CH%02d",i), Form("SSD1_L1S_E_CH%02d",i));
      Canvas_SSD1_L2F_E[i] = new TCanvas(Form("SSD1_L2F_E_CH%02d",i), Form("SSD1_L2F_E_CH%02d",i));
      Canvas_SSD1_L2B_E[i] = new TCanvas(Form("SSD1_L2B_E_CH%02d",i), Form("SSD1_L2B_E_CH%02d",i));
      Canvas_SSD2_L2F_T[i] = new TCanvas(Form("SSD2_L2F_T_CH%02d",i), Form("SSD2_L2F_T_CH%02d",i));
      Canvas_SSD2_L1S_E[i] = new TCanvas(Form("SSD2_L1S_E_CH%02d",i), Form("SSD2_L1S_E_CH%02d",i));
      Canvas_SSD2_L2F_E[i] = new TCanvas(Form("SSD2_L2F_E_CH%02d",i), Form("SSD2_L2F_E_CH%02d",i));
      Canvas_SSD2_L2B_E[i] = new TCanvas(Form("SSD2_L2B_E_CH%02d",i), Form("SSD2_L2B_E_CH%02d",i));
      Canvas_SSD3_L2F_T[i] = new TCanvas(Form("SSD3_L2F_T_CH%02d",i), Form("SSD3_L2F_T_CH%02d",i));
      Canvas_SSD3_L1S_E[i] = new TCanvas(Form("SSD3_L1S_E_CH%02d",i), Form("SSD3_L1S_E_CH%02d",i));
      Canvas_SSD3_L2F_E[i] = new TCanvas(Form("SSD3_L2F_E_CH%02d",i), Form("SSD3_L2F_E_CH%02d",i));
      Canvas_SSD3_L2B_E[i] = new TCanvas(Form("SSD3_L2B_E_CH%02d",i), Form("SSD3_L2B_E_CH%02d",i));
      Canvas_SSD4_L2F_T[i] = new TCanvas(Form("SSD4_L2F_T_CH%02d",i), Form("SSD4_L2F_T_CH%02d",i));
      Canvas_SSD4_L1S_E[i] = new TCanvas(Form("SSD4_L1S_E_CH%02d",i), Form("SSD4_L1S_E_CH%02d",i));
      Canvas_SSD4_L2F_E[i] = new TCanvas(Form("SSD4_L2F_E_CH%02d",i), Form("SSD4_L2F_E_CH%02d",i));
      Canvas_SSD4_L2B_E[i] = new TCanvas(Form("SSD4_L2B_E_CH%02d",i), Form("SSD4_L2B_E_CH%02d",i));
    }
    for(int i=0; i<9; i++)
    {
      Canvas_SSD1_L3A_E[i] = new TCanvas(Form("SSD1_L3A_E_CH%02d",i), Form("SSD1_L3A_E_CH%02d",i));
      Canvas_SSD2_L3A_E[i] = new TCanvas(Form("SSD2_L3A_E_CH%02d",i), Form("SSD2_L3A_E_CH%02d",i));
      Canvas_SSD3_L3A_E[i] = new TCanvas(Form("SSD3_L3A_E_CH%02d",i), Form("SSD3_L3A_E_CH%02d",i));
      Canvas_SSD4_L3A_E[i] = new TCanvas(Form("SSD4_L3A_E_CH%02d",i), Form("SSD4_L3A_E_CH%02d",i));
    }

    //=====================================================
    //  Draw and Save the Canvas
    //  注意：在void ReadTree()中，pdfpath是 const char* 类型
    //        Canvas_begin->Print(pdfpath+'[')会报错
    TString string_pfgpath_begin = pathpdfout;
    string_pfgpath_begin +='[';
    Canvas_begin->Print(string_pfgpath_begin);

    Canvas_PPAC1_T->cd(); Hist_PPAC1_T->Draw(); Canvas_PPAC1_T->Print(pathpdfout);

    Canvas_PPAC1_X1  ->cd(); Hist_PPAC1_X1 ->Draw(); Canvas_PPAC1_X1 ->Print(pathpdfout);
    Canvas_PPAC1_X2  ->cd(); Hist_PPAC1_X2 ->Draw(); Canvas_PPAC1_X2 ->Print(pathpdfout);
    Canvas_PPAC1_Y1  ->cd(); Hist_PPAC1_Y1 ->Draw(); Canvas_PPAC1_Y1 ->Print(pathpdfout);
    Canvas_PPAC1_Y2  ->cd(); Hist_PPAC1_Y2 ->Draw(); Canvas_PPAC1_Y2 ->Print(pathpdfout);
    Canvas_PPAC1_T_E ->cd(); gPad->SetLogy(); Hist_PPAC1_T_E->Draw(); Canvas_PPAC1_T_E->Print(pathpdfout);

    Canvas_PPAC2_T   ->cd(); Hist_PPAC2_T  ->Draw(); Canvas_PPAC2_T  ->Print(pathpdfout);
    Canvas_PPAC2_X1  ->cd(); Hist_PPAC2_X1 ->Draw(); Canvas_PPAC2_X1 ->Print(pathpdfout);
    Canvas_PPAC2_X2  ->cd(); Hist_PPAC2_X2 ->Draw(); Canvas_PPAC2_X2 ->Print(pathpdfout);
    Canvas_PPAC2_Y1  ->cd(); Hist_PPAC2_Y1 ->Draw(); Canvas_PPAC2_Y1 ->Print(pathpdfout);
    Canvas_PPAC2_Y2  ->cd(); Hist_PPAC2_Y2 ->Draw(); Canvas_PPAC2_Y2 ->Print(pathpdfout);
    Canvas_PPAC2_T_E ->cd(); gPad->SetLogy(); Hist_PPAC2_T_E->Draw(); Canvas_PPAC2_T_E->Print(pathpdfout);

    Canvas_PPAC3_T   ->cd(); Hist_PPAC3_T  ->Draw(); Canvas_PPAC3_T  ->Print(pathpdfout);
    Canvas_PPAC3_X1  ->cd(); Hist_PPAC3_X1 ->Draw(); Canvas_PPAC3_X1 ->Print(pathpdfout);
    Canvas_PPAC3_X2  ->cd(); Hist_PPAC3_X2 ->Draw(); Canvas_PPAC3_X2 ->Print(pathpdfout);
    Canvas_PPAC3_Y1  ->cd(); Hist_PPAC3_Y1 ->Draw(); Canvas_PPAC3_Y1 ->Print(pathpdfout);
    Canvas_PPAC3_Y2  ->cd(); Hist_PPAC3_Y2 ->Draw(); Canvas_PPAC3_Y2 ->Print(pathpdfout);
    Canvas_PPAC3_T_E ->cd(); gPad->SetLogy(); Hist_PPAC3_T_E->Draw(); Canvas_PPAC3_T_E->Print(pathpdfout);

    Canvas_AuSi1_L1T ->cd(); Hist_AuSi1_L1T ->Draw(); Canvas_AuSi1_L1T ->Print(pathpdfout);
    Canvas_AuSi1_L1E ->cd(); gPad->SetLogy(); Hist_AuSi1_L1E ->Draw(); Canvas_AuSi1_L1E ->Print(pathpdfout);
    Canvas_AuSi1_L2E ->cd(); gPad->SetLogy(); Hist_AuSi1_L2E ->Draw(); Canvas_AuSi1_L2E ->Print(pathpdfout);
    Canvas_AuSi2_L1T ->cd(); Hist_AuSi2_L1T ->Draw(); Canvas_AuSi2_L1T ->Print(pathpdfout);
    Canvas_AuSi2_L1E ->cd(); gPad->SetLogy(); Hist_AuSi2_L1E ->Draw(); Canvas_AuSi2_L1E ->Print(pathpdfout);
    Canvas_AuSi2_L2E ->cd(); gPad->SetLogy(); Hist_AuSi2_L2E ->Draw(); Canvas_AuSi2_L2E ->Print(pathpdfout);

    Canvas_RF1 ->cd(); Hist_RF1 ->Draw(); Canvas_RF1 ->Print(pathpdfout);
    Canvas_RF2 ->cd(); Hist_RF2 ->Draw(); Canvas_RF2 ->Print(pathpdfout);

    DrawSSD    (pathpdfout, Canvas_SSD1_L2F_T, Hist_SSD1_L2F_T, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD1_L1S_E, Hist_SSD1_L1S_E, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD1_L2F_E, Hist_SSD1_L2F_E, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD1_L2B_E, Hist_SSD1_L2B_E, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD1_L3A_E, Hist_SSD1_L3A_E, 9);
    DrawSSD    (pathpdfout, Canvas_SSD2_L2F_T, Hist_SSD2_L2F_T, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD2_L1S_E, Hist_SSD2_L1S_E, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD2_L2F_E, Hist_SSD2_L2F_E, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD2_L2B_E, Hist_SSD2_L2B_E, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD2_L3A_E, Hist_SSD2_L3A_E, 9);
    DrawSSD    (pathpdfout, Canvas_SSD3_L2F_T, Hist_SSD3_L2F_T, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD3_L1S_E, Hist_SSD3_L1S_E, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD3_L2F_E, Hist_SSD3_L2F_E, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD3_L2B_E, Hist_SSD3_L2B_E, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD3_L3A_E, Hist_SSD3_L3A_E, 9);
    DrawSSD    (pathpdfout, Canvas_SSD4_L2F_T, Hist_SSD4_L2F_T, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD4_L1S_E, Hist_SSD4_L1S_E, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD4_L2F_E, Hist_SSD4_L2F_E, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD4_L2B_E, Hist_SSD4_L2B_E, 16);
    DrawSSDLogy(pathpdfout, Canvas_SSD4_L3A_E, Hist_SSD4_L3A_E, 9);

    TString string_pfgpath_end = pathpdfout;
    string_pfgpath_end +=']';
    Canvas_end->Print(string_pfgpath_end);

    FileOut->Close();
    return;
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEQualityCheck::ReadBranchMethod(TChain* fChain, const char* pathrootout, const char* pathpdfout)
{
    Int_t   BIN_NUM  =  4096;
    Int_t   LOW_CH   =  0;
    Int_t   HIGH_CH  =  4096;
    //__________________________________________________________________________
    //                                For PPAC
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
      sprintf(SSD_L2F_T_name[i], "SSD%d_L2F_T", i);
      sprintf(SSD_L1S_E_name[i], "SSD%d_L1S_E", i);
      sprintf(SSD_L2F_E_name[i], "SSD%d_L2F_E", i);
      sprintf(SSD_L2B_E_name[i], "SSD%d_L2B_E", i);
      sprintf(SSD_L3A_E_name[i], "SSD%d_L3A_E", i);
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
    cout << "nentries=" << nentries << endl;
    for (Long64_t ientry=0; ientry<nentries;ientry++)
    {
      Long64_t jentry = fChain->LoadTree(ientry);
      if (jentry < 0) continue;
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
    TString pathpdfout_begin = pathpdfout;
    pathpdfout_begin        += '[';
    Canvas_begin->Print(pathpdfout_begin);
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
    for(Int_t i=0; i<4; i++)
    {
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
    TString pathpdfout_end = pathpdfout;
    pathpdfout_end        += "]";
    Canvas_end->Print(pathpdfout_end);
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEQualityCheck::WriteHistforSSD(TFile* FileOut, TH1D* hist[],  const Int_t numtel)
{
  for(Int_t i=0; i<numtel; i++){
    FileOut->WriteTObject(hist[i], hist[i]->GetName());
  }
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEQualityCheck::DrawSSD(const Char_t* pdfpath, TCanvas* canvas[],
  TH1D* hist[], const Int_t numtel)
{
  for(Int_t i=0; i<numtel; i++){
    canvas[i]->cd(); hist[i] ->Draw(); canvas[i] ->Print(pdfpath);
  }
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEQualityCheck::DrawSSDLogy(const Char_t* pdfpath, TCanvas* canvas[],
  TH1D* hist[], const Int_t numtel)
{
  for(Int_t i=0; i<numtel; i++){
    canvas[i]->cd(); gPad->SetLogy(); hist[i] ->Draw(); canvas[i] ->Print(pdfpath);
  }
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
