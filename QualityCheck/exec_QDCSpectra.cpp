#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TChain.h"
#include "TNtuple.h"
#include <TH2.h>
#include <iostream>

void ReadTree(TChain * Chain, const char * file_name)
{
	TFile FileOut (file_name, "RECREATE");
	
	TH1I * hist_adc1[32];
	TH1I * hist_adc2[32];
	TH1I * hist_qdc1[32];
	TH1I * hist_qdc2[32];
	TH1I * hist_qdc3[32];
	TH1I * hist_qdc4[32];
	TH1I * hist_qdc5[32];
	TH1I * hist_qdc6[32];
	TH1I * hist_qdc7[32];
	TH1I * hist_qdc8[32];
	TH1I * hist_qdcfart[32];
	TH1I * hist_MBqdc1[32];
	TH1I * hist_MBqdc2[32];
	TH1I * hist_MBqdc3[32];
	TH1I * hist_MBqdc4[32];
	TH1I * hist_MBqdc5[32];
	TH1I * hist_MBqdc6[32];

	for(int i=0; i<32; i++) {
	  hist_adc1[i]= new TH1I (Form("adc1_ch%02d", i),"", 1000,0,999);
          hist_adc2[i]= new TH1I (Form("adc2_ch%02d", i),"", 1000,0,999);
          hist_qdc1[i]= new TH1I (Form("qdc1_ch%02d", i),"", 1000,0,999);
	  hist_qdc2[i]= new TH1I (Form("qdc2_ch%02d", i),"", 1000,0,999);
	  hist_qdc3[i]= new TH1I (Form("qdc3_ch%02d", i),"", 1000,0,999);
	  hist_qdc4[i]= new TH1I (Form("qdc4_ch%02d", i),"", 1000,0,999);
	  hist_qdc5[i]= new TH1I (Form("qdc5_ch%02d", i),"", 1000,0,999);
	  hist_qdc6[i]= new TH1I (Form("qdc6_ch%02d", i),"", 1000,0,999);
	  hist_qdc7[i]= new TH1I (Form("qdc7_ch%02d", i),"", 1000,0,999);
  	  hist_qdc8[i]= new TH1I (Form("qdc8_ch%02d", i),"", 1000,0,999);
	  hist_qdcfart[i]= new TH1I (Form("qdcfart_ch%02d", i),"", 1000,0,999);
	  hist_MBqdc1[i]= new TH1I (Form("MBqdc1_ch%02d", i),"", 1000,0,999);
	  hist_MBqdc2[i]= new TH1I (Form("MBqdc2_ch%02d", i),"", 1000,0,999);
	  hist_MBqdc3[i]= new TH1I (Form("MBqdc3_ch%02d", i),"", 1000,0,999);
	  hist_MBqdc4[i]= new TH1I (Form("MBqdc4_ch%02d", i),"", 1000,0,999);
	  hist_MBqdc5[i]= new TH1I (Form("MBqdc5_ch%02d", i),"", 1000,0,999);
	  hist_MBqdc6[i]= new TH1I (Form("MBqdc6_ch%02d", i),"", 1000,0,999);
	}

	TTreeReader myReader(Chain);
	TTreeReaderArray<Short_t> adc1(myReader,"adc1");
	TTreeReaderArray<Short_t> adc2(myReader,"adc2");
	TTreeReaderArray<Short_t> qdc1(myReader,"qdc1");
	TTreeReaderArray<Short_t> qdc2(myReader,"qdc2");
	TTreeReaderArray<Short_t> qdc3(myReader,"qdc3");
	TTreeReaderArray<Short_t> qdc4(myReader,"qdc4");
	TTreeReaderArray<Short_t> qdc5(myReader,"qdc5");
	TTreeReaderArray<Short_t> qdc6(myReader,"qdc6");
	TTreeReaderArray<Short_t> qdc7(myReader,"qdc7");
	TTreeReaderArray<Short_t> qdc8(myReader,"qdc8");
	TTreeReaderArray<Short_t> qdcfart(myReader,"qdcfart");
	TTreeReaderArray<Short_t> MBqdc1(myReader,"MBqdc1");
	TTreeReaderArray<Short_t> MBqdc2(myReader,"MBqdc2");
	TTreeReaderArray<Short_t> MBqdc3(myReader,"MBqdc3");
	TTreeReaderArray<Short_t> MBqdc4(myReader,"MBqdc4");
	TTreeReaderArray<Short_t> MBqdc5(myReader,"MBqdc5");
	TTreeReaderArray<Short_t> MBqdc6(myReader,"MBqdc6");

	while(myReader.Next())
	{
	  for(int i=0; i<32; i++)
	  {
            hist_adc1[i]->Fill(adc1[i]);
	    hist_adc2[i]->Fill(adc2[i]);
	    hist_qdc1[i]->Fill(qdc1[i]);
	    hist_qdc2[i]->Fill(qdc2[i]);
	    hist_qdc3[i]->Fill(qdc3[i]);
	    hist_qdc4[i]->Fill(qdc4[i]);
	    hist_qdc5[i]->Fill(qdc5[i]);
	    hist_qdc6[i]->Fill(qdc6[i]);
	    hist_qdc7[i]->Fill(qdc7[i]);
	    hist_qdc8[i]->Fill(qdc8[i]);
	    hist_qdcfart[i]->Fill(qdcfart[i]);
	    hist_MBqdc1[i]->Fill(MBqdc1[i]);
	    hist_MBqdc2[i]->Fill(MBqdc2[i]);
	    hist_MBqdc3[i]->Fill(MBqdc3[i]);
	    hist_MBqdc4[i]->Fill(MBqdc4[i]);
	    hist_MBqdc5[i]->Fill(MBqdc5[i]);
	    hist_MBqdc6[i]->Fill(MBqdc6[i]);
	  }
	}

	for(int i=0; i<32; i++) {
	  FileOut.WriteTObject(hist_adc1[i], hist_adc1[i]->GetName());
	  FileOut.WriteTObject(hist_adc2[i], hist_adc2[i]->GetName());
	  FileOut.WriteTObject(hist_qdc1[i], hist_qdc1[i]->GetName());
	  FileOut.WriteTObject(hist_qdc2[i], hist_qdc2[i]->GetName());
	  FileOut.WriteTObject(hist_qdc3[i], hist_qdc3[i]->GetName());
	  FileOut.WriteTObject(hist_qdc4[i], hist_qdc4[i]->GetName());
	  FileOut.WriteTObject(hist_qdc5[i], hist_qdc5[i]->GetName());
	  FileOut.WriteTObject(hist_qdc6[i], hist_qdc6[i]->GetName());
	  FileOut.WriteTObject(hist_qdc7[i], hist_qdc7[i]->GetName());
	  FileOut.WriteTObject(hist_qdc8[i], hist_qdc8[i]->GetName());
	  FileOut.WriteTObject(hist_qdcfart[i], hist_qdcfart[i]->GetName());
	  FileOut.WriteTObject(hist_MBqdc1[i], hist_MBqdc1[i]->GetName());
	  FileOut.WriteTObject(hist_MBqdc2[i], hist_MBqdc2[i]->GetName());
	  FileOut.WriteTObject(hist_MBqdc3[i], hist_MBqdc3[i]->GetName());
	  FileOut.WriteTObject(hist_MBqdc4[i], hist_MBqdc4[i]->GetName());
	  FileOut.WriteTObject(hist_MBqdc5[i], hist_MBqdc5[i]->GetName());
	  FileOut.WriteTObject(hist_MBqdc6[i], hist_MBqdc6[i]->GetName());
	}


	FileOut.Close();
	return;
}

int main (int argc, char ** argv)
{
  std::string data_path("/mnt/analysis/e15190/HiRAEVTUnpacker_rootfiles/");
  TChain *dataChain = (TChain*) new TChain("E15190");
  std::string first_run_name(argv[1]);
  int first_run = atoi(first_run_name.c_str());
  int last_run=first_run;
  Long64_t evt_amount=0;
  if(argc>2) {
    std::string last_run_name(argv[2]);
    last_run = atoi(last_run_name.c_str());
  }
  // Adding all the files to the TChain ================================
  for(int i=first_run; i<=last_run; i++)
  {
    char file_evt[100];
    sprintf (file_evt,"run-%04d-00.root",i);
    std::string file_evt_string(file_evt);
    std::string path_to_evt_file(data_path+file_evt_string);
    int n_files = dataChain->Add((data_path+file_evt).c_str());
    printf("%d Event files added to chain for run %d\n", n_files, i);
  }



  ReadTree(dataChain, Form("output/QDCSpectra_%04d_%04d.root", first_run, last_run));

  return 0;
}
