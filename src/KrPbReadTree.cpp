#include "../include/KrPbReadTree.h"

//________________________________________________
KrPbReadTree::KrPbReadTree()
{
  //cout<< "constructor is called"<<endl;
}

//________________________________________________
KrPbReadTree::~KrPbReadTree()
{
  //cout<<"disconstrctor is called"<<endl;
}

//___________________________________________________________________
void KrPbReadTree::WriteHist(TChain* myChain,  const char* rootpath)
{
  int BIN_NUM = 4096;
  int LOW_CH  = 0;
  int HIGH_CH = 4096;


  TFile FileOut(rootpath, "RECREATE");

  //===========================
  //       定义直方图
  //===========================
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
    Hist_SSD1_L2F_T[i] = new TH1D(Form("SSD1_L2F_T_CH%d", i), Form("SSD1_L2F_T_CH%d", i), BIN_NUM-100, LOW_CH+100, HIGH_CH);
    Hist_SSD1_L1S_E[i] = new TH1D(Form("SSD1_L1S_E_CH%d", i), Form("SSD1_L1S_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD1_L2F_E[i] = new TH1D(Form("SSD1_L2F_E_CH%d", i), Form("SSD1_L2F_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD1_L2B_E[i] = new TH1D(Form("SSD1_L2B_E_CH%d", i), Form("SSD1_L2B_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD2_L2F_T[i] = new TH1D(Form("SSD2_L2F_T_CH%d", i), Form("SSD2_L2F_T_CH%d", i), BIN_NUM-100, LOW_CH+100, HIGH_CH);
    Hist_SSD2_L1S_E[i] = new TH1D(Form("SSD2_L1S_E_CH%d", i), Form("SSD2_L1S_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD2_L2F_E[i] = new TH1D(Form("SSD2_L2F_E_CH%d", i), Form("SSD2_L2F_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD2_L2B_E[i] = new TH1D(Form("SSD2_L2B_E_CH%d", i), Form("SSD2_L2B_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD3_L2F_T[i] = new TH1D(Form("SSD3_L2F_T_CH%d", i), Form("SSD3_L2F_T_CH%d", i), BIN_NUM-100, LOW_CH+100, HIGH_CH);
    Hist_SSD3_L1S_E[i] = new TH1D(Form("SSD3_L1S_E_CH%d", i), Form("SSD3_L1S_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD3_L2F_E[i] = new TH1D(Form("SSD3_L2F_E_CH%d", i), Form("SSD3_L2F_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD3_L2B_E[i] = new TH1D(Form("SSD3_L2B_E_CH%d", i), Form("SSD3_L2B_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD4_L2F_T[i] = new TH1D(Form("SSD4_L2F_T_CH%d", i), Form("SSD4_L2F_T_CH%d", i), BIN_NUM-100, LOW_CH+100, HIGH_CH);
    Hist_SSD4_L1S_E[i] = new TH1D(Form("SSD4_L1S_E_CH%d", i), Form("SSD4_L1S_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD4_L2F_E[i] = new TH1D(Form("SSD4_L2F_E_CH%d", i), Form("SSD4_L2F_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD4_L2B_E[i] = new TH1D(Form("SSD4_L2B_E_CH%d", i), Form("SSD4_L2B_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
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
  }   // close for while(myReader.Next())

    //===========================================================
    //     将所有的直方图写入到 FileOut 文件中
    FileOut.WriteTObject(Hist_PPAC1_T,   Hist_PPAC1_T->GetName());
    FileOut.WriteTObject(Hist_PPAC1_X1,  Hist_PPAC1_X1->GetName());
    FileOut.WriteTObject(Hist_PPAC1_X2,  Hist_PPAC1_X2->GetName());
    FileOut.WriteTObject(Hist_PPAC1_Y1,  Hist_PPAC1_Y1->GetName());
    FileOut.WriteTObject(Hist_PPAC1_Y2,  Hist_PPAC1_Y2->GetName());
    FileOut.WriteTObject(Hist_PPAC1_T_E, Hist_PPAC1_T_E->GetName());
    FileOut.WriteTObject(Hist_PPAC2_T,   Hist_PPAC2_T->GetName());
    FileOut.WriteTObject(Hist_PPAC2_X1,  Hist_PPAC2_X1->GetName());
    FileOut.WriteTObject(Hist_PPAC2_X2,  Hist_PPAC2_X2->GetName());
    FileOut.WriteTObject(Hist_PPAC2_Y1,  Hist_PPAC2_Y1->GetName());
    FileOut.WriteTObject(Hist_PPAC2_Y2,  Hist_PPAC2_Y2->GetName());
    FileOut.WriteTObject(Hist_PPAC2_T_E, Hist_PPAC2_T_E->GetName());
    FileOut.WriteTObject(Hist_PPAC3_T,   Hist_PPAC3_T->GetName());
    FileOut.WriteTObject(Hist_PPAC3_X1,  Hist_PPAC3_X1->GetName());
    FileOut.WriteTObject(Hist_PPAC3_X2,  Hist_PPAC3_X2->GetName());
    FileOut.WriteTObject(Hist_PPAC3_Y1,  Hist_PPAC3_Y1->GetName());
    FileOut.WriteTObject(Hist_PPAC3_Y2,  Hist_PPAC3_Y2->GetName());
    FileOut.WriteTObject(Hist_PPAC3_T_E, Hist_PPAC3_T_E->GetName());

    FileOut.WriteTObject(Hist_AuSi1_L1T, Hist_AuSi1_L1T->GetName());
    FileOut.WriteTObject(Hist_AuSi1_L1E, Hist_AuSi1_L1E->GetName());
    FileOut.WriteTObject(Hist_AuSi1_L2E, Hist_AuSi1_L2E->GetName());
    FileOut.WriteTObject(Hist_AuSi2_L1T, Hist_AuSi2_L1T->GetName());
    FileOut.WriteTObject(Hist_AuSi2_L1E, Hist_AuSi2_L1E->GetName());
    FileOut.WriteTObject(Hist_AuSi2_L2E, Hist_AuSi2_L2E->GetName());

    FileOut.WriteTObject(Hist_RF1, Hist_RF1->GetName());
    FileOut.WriteTObject(Hist_RF2, Hist_RF2->GetName());

    //=====================
    //  for SSD1
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD1_L2F_T[i], Hist_SSD1_L2F_T[i]->GetName());
    }
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD1_L1S_E[i], Hist_SSD1_L1S_E[i]->GetName());
    }
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD1_L2F_E[i], Hist_SSD1_L2F_E[i]->GetName());
    }
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD1_L2B_E[i], Hist_SSD1_L2B_E[i]->GetName());
    }
    for(int i=0; i<9; i++)
    {
      FileOut.WriteTObject(Hist_SSD1_L3A_E[i], Hist_SSD1_L3A_E[i]->GetName());
    }

    //=====================
    //    for SSD2
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD2_L2F_T[i], Hist_SSD2_L2F_T[i]->GetName());
    }
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD2_L1S_E[i], Hist_SSD2_L1S_E[i]->GetName());
    }
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD2_L2F_E[i], Hist_SSD2_L2F_E[i]->GetName());
    }
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD2_L2B_E[i], Hist_SSD2_L2B_E[i]->GetName());
    }
    for(int i=0; i<9; i++)
    {
      FileOut.WriteTObject(Hist_SSD2_L3A_E[i], Hist_SSD2_L3A_E[i]->GetName());
    }

    //=====================
    //      for SSD3
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD3_L2F_T[i], Hist_SSD3_L2F_T[i]->GetName());
    }
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD3_L1S_E[i], Hist_SSD3_L1S_E[i]->GetName());
    }
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD3_L2F_E[i], Hist_SSD3_L2F_E[i]->GetName());
    }
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD3_L2B_E[i], Hist_SSD3_L2B_E[i]->GetName());
    }
    for(int i=0; i<9; i++)
    {
      FileOut.WriteTObject(Hist_SSD3_L3A_E[i], Hist_SSD3_L3A_E[i]->GetName());
    }

    //====================
    //      for SSD4
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD4_L2F_T[i], Hist_SSD4_L2F_T[i]->GetName());
    }
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD4_L1S_E[i], Hist_SSD4_L1S_E[i]->GetName());
    }
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD4_L2F_E[i], Hist_SSD4_L2F_E[i]->GetName());
    }
    for(int i=0; i<16; i++)
    {
      FileOut.WriteTObject(Hist_SSD4_L2B_E[i], Hist_SSD4_L2B_E[i]->GetName());
    }
    for(int i=0; i<9; i++)
    {
      FileOut.WriteTObject(Hist_SSD4_L3A_E[i], Hist_SSD4_L3A_E[i]->GetName());
    }

    FileOut.Close();
    return;
}


//____________________________________________________________________
void KrPbReadTree::SaveCanvas(TChain* myChain, const char* pdfpath)
{
  int BIN_NUM = 4096;
  int LOW_CH  = 0;
  int HIGH_CH = 4096;


  //===========================
  //       定义直方图
  //===========================
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
    Hist_SSD1_L2F_T[i] = new TH1D(Form("SSD1_L2F_T_CH%d", i), Form("SSD1_L2F_T_CH%d", i), BIN_NUM-100, LOW_CH+100, HIGH_CH);
    Hist_SSD1_L1S_E[i] = new TH1D(Form("SSD1_L1S_E_CH%d", i), Form("SSD1_L1S_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD1_L2F_E[i] = new TH1D(Form("SSD1_L2F_E_CH%d", i), Form("SSD1_L2F_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD1_L2B_E[i] = new TH1D(Form("SSD1_L2B_E_CH%d", i), Form("SSD1_L2B_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD2_L2F_T[i] = new TH1D(Form("SSD2_L2F_T_CH%d", i), Form("SSD2_L2F_T_CH%d", i), BIN_NUM-100, LOW_CH+100, HIGH_CH);
    Hist_SSD2_L1S_E[i] = new TH1D(Form("SSD2_L1S_E_CH%d", i), Form("SSD2_L1S_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD2_L2F_E[i] = new TH1D(Form("SSD2_L2F_E_CH%d", i), Form("SSD2_L2F_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD2_L2B_E[i] = new TH1D(Form("SSD2_L2B_E_CH%d", i), Form("SSD2_L2B_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD3_L2F_T[i] = new TH1D(Form("SSD3_L2F_T_CH%d", i), Form("SSD3_L2F_T_CH%d", i), BIN_NUM-100, LOW_CH+100, HIGH_CH);
    Hist_SSD3_L1S_E[i] = new TH1D(Form("SSD3_L1S_E_CH%d", i), Form("SSD3_L1S_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD3_L2F_E[i] = new TH1D(Form("SSD3_L2F_E_CH%d", i), Form("SSD3_L2F_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD3_L2B_E[i] = new TH1D(Form("SSD3_L2B_E_CH%d", i), Form("SSD3_L2B_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD4_L2F_T[i] = new TH1D(Form("SSD4_L2F_T_CH%d", i), Form("SSD4_L2F_T_CH%d", i), BIN_NUM-100, LOW_CH+100, HIGH_CH);
    Hist_SSD4_L1S_E[i] = new TH1D(Form("SSD4_L1S_E_CH%d", i), Form("SSD4_L1S_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD4_L2F_E[i] = new TH1D(Form("SSD4_L2F_E_CH%d", i), Form("SSD4_L2F_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
    Hist_SSD4_L2B_E[i] = new TH1D(Form("SSD4_L2B_E_CH%d", i), Form("SSD4_L2B_E_CH%d", i), BIN_NUM, LOW_CH, HIGH_CH);
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
  }   // close for while(myReader.Next())


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


    //======================================================
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

    //====================
    //    for SSD1
    for(int i=0; i<16; i++)
    {
      Canvas_SSD1_L2F_T[i] ->cd(); Hist_SSD1_L2F_T[i] ->Draw(); Canvas_SSD1_L2F_T[i] ->Print(pdfpath);
    }
    for(int i=0; i<16; i++)
    {
      Canvas_SSD1_L1S_E[i] ->cd(); gPad->SetLogy(); Hist_SSD1_L1S_E[i] ->Draw(); Canvas_SSD1_L1S_E[i] ->Print(pdfpath);
    }
    for(int i=0; i<16; i++)
    {
      Canvas_SSD1_L2F_E[i] ->cd(); gPad->SetLogy(); Hist_SSD1_L2F_E[i] ->Draw(); Canvas_SSD1_L2F_E[i] ->Print(pdfpath);
    }
    for(int i=0; i<16; i++)
    {
      Canvas_SSD1_L2B_E[i] ->cd(); gPad->SetLogy(); Hist_SSD1_L2B_E[i] ->Draw(); Canvas_SSD1_L2B_E[i] ->Print(pdfpath);
    }
    for(int i=0; i<9; i++)
    {
      Canvas_SSD1_L3A_E[i] ->cd(); gPad->SetLogy(); Hist_SSD1_L3A_E[i] ->Draw(); Canvas_SSD1_L3A_E[i] ->Print(pdfpath);
    }

    //====================
    //    for SSD2
    for(int i=0; i<16; i++)
    {
      Canvas_SSD2_L2F_T[i] ->cd(); Hist_SSD2_L2F_T[i] ->Draw(); Canvas_SSD2_L2F_T[i] ->Print(pdfpath);
    }
    for(int i=0; i<16; i++)
    {
      Canvas_SSD2_L1S_E[i] ->cd(); gPad->SetLogy(); Hist_SSD2_L1S_E[i] ->Draw(); Canvas_SSD2_L1S_E[i] ->Print(pdfpath);
    }
    for(int i=0; i<16; i++)
    {
      Canvas_SSD2_L2F_E[i] ->cd(); gPad->SetLogy(); Hist_SSD2_L2F_E[i] ->Draw(); Canvas_SSD2_L2F_E[i] ->Print(pdfpath);
    }
    for(int i=0; i<16; i++)
    {
      Canvas_SSD2_L2B_E[i] ->cd(); gPad->SetLogy(); Hist_SSD2_L2B_E[i] ->Draw(); Canvas_SSD2_L2B_E[i] ->Print(pdfpath);
    }
    for(int i=0; i<9; i++)
    {
      Canvas_SSD2_L3A_E[i] ->cd(); gPad->SetLogy(); Hist_SSD2_L3A_E[i] ->Draw(); Canvas_SSD2_L3A_E[i] ->Print(pdfpath);
    }

    //====================
    //    for SSD3
    for(int i=0; i<16; i++)
    {
      Canvas_SSD3_L2F_T[i] ->cd(); Hist_SSD3_L2F_T[i] ->Draw(); Canvas_SSD3_L2F_T[i] ->Print(pdfpath);
    }
    for(int i=0; i<16; i++)
    {
      Canvas_SSD3_L1S_E[i] ->cd(); gPad->SetLogy(); Hist_SSD3_L1S_E[i] ->Draw(); Canvas_SSD3_L1S_E[i] ->Print(pdfpath);
    }
    for(int i=0; i<16; i++)
    {
      Canvas_SSD3_L2F_E[i] ->cd(); gPad->SetLogy(); Hist_SSD3_L2F_E[i] ->Draw(); Canvas_SSD3_L2F_E[i] ->Print(pdfpath);
    }
    for(int i=0; i<16; i++)
    {
      Canvas_SSD3_L2B_E[i] ->cd(); gPad->SetLogy(); Hist_SSD3_L2B_E[i] ->Draw(); Canvas_SSD3_L2B_E[i] ->Print(pdfpath);
    }
    for(int i=0; i<9; i++)
    {
      Canvas_SSD3_L3A_E[i] ->cd(); gPad->SetLogy(); Hist_SSD3_L3A_E[i] ->Draw(); Canvas_SSD3_L3A_E[i] ->Print(pdfpath);
    }

    //====================
    //    for SSD4
    for(int i=0; i<16; i++)
    {
      Canvas_SSD4_L2F_T[i] ->cd(); Hist_SSD4_L2F_T[i] ->Draw(); Canvas_SSD4_L2F_T[i] ->Print(pdfpath);
    }
    for(int i=0; i<16; i++)
    {
      Canvas_SSD4_L1S_E[i] ->cd(); gPad->SetLogy(); Hist_SSD4_L1S_E[i] ->Draw(); Canvas_SSD4_L1S_E[i] ->Print(pdfpath);
    }
    for(int i=0; i<16; i++)
    {
      Canvas_SSD4_L2F_E[i] ->cd(); gPad->SetLogy(); Hist_SSD4_L2F_E[i] ->Draw(); Canvas_SSD4_L2F_E[i] ->Print(pdfpath);
    }
    for(int i=0; i<16; i++)
    {
      Canvas_SSD4_L2B_E[i] ->cd(); gPad->SetLogy(); Hist_SSD4_L2B_E[i] ->Draw(); Canvas_SSD4_L2B_E[i] ->Print(pdfpath);
    }
    for(int i=0; i<9; i++)
    {
      Canvas_SSD4_L3A_E[i] ->cd(); gPad->SetLogy(); Hist_SSD4_L3A_E[i] ->Draw(); Canvas_SSD4_L3A_E[i] ->Print(pdfpath);
    }

    TString string_pfgpath_end = pdfpath;
    string_pfgpath_end +=']';
    Canvas_end->Print(string_pfgpath_end);

    return;
}
