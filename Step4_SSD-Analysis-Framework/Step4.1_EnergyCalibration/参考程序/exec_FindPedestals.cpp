#include "TFile.h"
#include "TString.h"
#include <TH1.h>
#include <iostream>
#include <fstream>
#include <TGraph.h>

int main (int argc, char ** argv)
{
  std::string first_run_name(argv[1]);
  int first_run = atoi(first_run_name.c_str());
  int last_run=first_run;
  Long64_t evt_amount=0;
  if(argc>2) {
    std::string last_run_name(argv[2]);
    last_run = atoi(last_run_name.c_str());
  }
  
  TFile * FileOutRoot= new TFile(Form("report/QDCPedestals_%04d_%04d.root",first_run, last_run), "RECREATE");
  
  std::ofstream FileOut(Form("report/QDCPedestals_%04d_%04d.csv",first_run, last_run));
  FileOut << "Run" << " , ";
  for(int i=0; i<32; i++) {
    FileOut << Form("adc1_%02d", i)  << " , " ;
  }
  for(int i=0; i<32; i++) {
    FileOut << Form("adc2_%02d", i)  << " , " ;
  } 
  for(int i=0; i<32; i++) {
    FileOut << Form("qdc1_%02d", i)  << " , " ;
  }
  for(int i=0; i<32; i++) {
    FileOut << Form("qdc2_%02d", i)  << " , " ;
  }
  for(int i=0; i<32; i++) {
    FileOut << Form("qdc3_%02d", i)  << " , " ;
  }
  for(int i=0; i<32; i++) {
    FileOut << Form("qdc4_%02d", i)  << " , " ;
  } 
  for(int i=0; i<32; i++) {
    FileOut << Form("qdc5_%02d", i)  << " , " ;
  }
  for(int i=0; i<32; i++) {
    FileOut << Form("qdc6_%02d", i)  << " , " ;
  } 
  for(int i=0; i<32; i++) {
    FileOut << Form("qdc7_%02d", i)  << " , " ;
  }
  for(int i=0; i<32; i++) {
    FileOut << Form("qdc8_%02d", i)  << " , " ;
  } 
  for(int i=0; i<32; i++) {
    FileOut << Form("qdcfart_%02d", i)  << " , " ;
  }   
  for(int i=0; i<32; i++) {
    FileOut << Form("MBqdc1_%02d", i)  << " , " ;
  }
  for(int i=0; i<32; i++) {
    FileOut << Form("MBqdc2_%02d", i)  << " , " ;
  } 
  for(int i=0; i<32; i++) {
    FileOut << Form("MBqdc3_%02d", i)  << " , " ;
  }
  for(int i=0; i<32; i++) {
    FileOut << Form("MBqdc4_%02d", i)  << " , " ;
  } 
  for(int i=0; i<32; i++) {
    FileOut << Form("MBqdc5_%02d", i)  << " , " ;
  }
  for(int i=0; i<32; i++) {
    FileOut << Form("MBqdc6_%02d", i)  << (i!=31 ? " , " : "\n");
  } 
  
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
  
  TGraph * graph_adc1[32];
  TGraph * graph_adc2[32];
  TGraph * graph_qdc1[32];
  TGraph * graph_qdc2[32];
  TGraph * graph_qdc3[32];
  TGraph * graph_qdc4[32];
  TGraph * graph_qdc5[32];
  TGraph * graph_qdc6[32];
  TGraph * graph_qdc7[32];
  TGraph * graph_qdc8[32];
  TGraph * graph_qdcfart[32];
  TGraph * graph_MBqdc1[32];
  TGraph * graph_MBqdc2[32];
  TGraph * graph_MBqdc3[32];
  TGraph * graph_MBqdc4[32];
  TGraph * graph_MBqdc5[32];
  TGraph * graph_MBqdc6[32];  
    
  int Nruns_adc1[32]={};
  int Nruns_adc2[32]={};
  int Nruns_qdc1[32]={};
  int Nruns_qdc2[32]={};
  int Nruns_qdc3[32]={};
  int Nruns_qdc4[32]={};
  int Nruns_qdc5[32]={};
  int Nruns_qdc6[32]={};
  int Nruns_qdc7[32]={};
  int Nruns_qdc8[32]={};
  int Nruns_qdcfart[32]={};
  int Nruns_MBqdc1[32]={};
  int Nruns_MBqdc2[32]={};
  int Nruns_MBqdc3[32]={};
  int Nruns_MBqdc4[32]={};
  int Nruns_MBqdc5[32]={};
  int Nruns_MBqdc6[32]={};
  double * Run_adc1[32];
  double * Run_adc2[32];
  double * Run_qdc1[32];
  double * Run_qdc2[32];
  double * Run_qdc3[32];
  double * Run_qdc4[32];
  double * Run_qdc5[32];
  double * Run_qdc6[32];
  double * Run_qdc7[32];
  double * Run_qdc8[32];
  double * Run_qdcfart[32];
  double * Run_MBqdc1[32];
  double * Run_MBqdc2[32];
  double * Run_MBqdc3[32];
  double * Run_MBqdc4[32];
  double * Run_MBqdc5[32];
  double * Run_MBqdc6[32];
  double * Ped_adc1[32];
  double * Ped_adc2[32];
  double * Ped_qdc1[32];
  double * Ped_qdc2[32];
  double * Ped_qdc3[32];
  double * Ped_qdc4[32];
  double * Ped_qdc5[32];
  double * Ped_qdc6[32];
  double * Ped_qdc7[32];
  double * Ped_qdc8[32];
  double * Ped_qdcfart[32];
  double * Ped_MBqdc1[32];
  double * Ped_MBqdc2[32];
  double * Ped_MBqdc3[32];
  double * Ped_MBqdc4[32];
  double * Ped_MBqdc5[32];
  double * Ped_MBqdc6[32];
  for(int i=0; i<32; i++) {
    Run_adc1[i]=new double[4000];
    Run_adc2[i]=new double[4000];
    Run_qdc1[i]=new double[4000];
    Run_qdc2[i]=new double[4000];
    Run_qdc3[i]=new double[4000];
    Run_qdc4[i]=new double[4000];
    Run_qdc5[i]=new double[4000];
    Run_qdc6[i]=new double[4000];
    Run_qdc7[i]=new double[4000];
    Run_qdc8[i]=new double[4000];
    Run_qdcfart[i]=new double[4000];
    Run_MBqdc1[i]=new double[4000];
    Run_MBqdc2[i]=new double[4000];
    Run_MBqdc3[i]=new double[4000];
    Run_MBqdc4[i]=new double[4000];
    Run_MBqdc5[i]=new double[4000];
    Run_MBqdc6[i]=new double[4000];
    Ped_adc1[i]=new double[4000];
    Ped_adc2[i]=new double[4000];
    Ped_qdc1[i]=new double[4000];
    Ped_qdc2[i]=new double[4000];
    Ped_qdc3[i]=new double[4000];
    Ped_qdc4[i]=new double[4000];
    Ped_qdc5[i]=new double[4000];
    Ped_qdc6[i]=new double[4000];
    Ped_qdc7[i]=new double[4000];
    Ped_qdc8[i]=new double[4000];
    Ped_qdcfart[i]=new double[4000];
    Ped_MBqdc1[i]=new double[4000];
    Ped_MBqdc2[i]=new double[4000];
    Ped_MBqdc3[i]=new double[4000];
    Ped_MBqdc4[i]=new double[4000];
    Ped_MBqdc5[i]=new double[4000];
    Ped_MBqdc6[i]=new double[4000];
  } 
  
  double PedAmplitude=0;
    
  // Looping all over the files ================================
  for(int i=first_run; i<=last_run; i++)
  {
    TFile FileIn(Form("output/QDCSpectra_%04d_%04d.root",i,i));
    
    if(!FileIn.IsOpen()) continue;
    
    FileOut << i << " , ";
    
      for(int ch=0; ch<32; ch++) {
        hist_adc1[ch]=(TH1I*)FileIn.Get(Form("adc1_ch%02d", ch));
    	Ped_adc1[ch][Nruns_adc1[ch]] = (hist_adc1[ch]->Integral()>6000 ? hist_adc1[ch]->GetBinCenter(hist_adc1[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_adc1[ch]->GetBinContent(hist_adc1[ch]->GetMaximumBin());
	FileOut << Ped_adc1[ch][Nruns_adc1[ch]] << " , " ;
	if(Ped_adc1[ch][Nruns_adc1[ch]]>1 && Ped_adc1[ch][Nruns_adc1[ch]]<150) {Run_adc1[ch][Nruns_adc1[ch]]=i; Nruns_adc1[ch]++;}
      }
      for(int ch=0; ch<32; ch++) {
        hist_adc2[ch]=(TH1I*)FileIn.Get(Form("adc2_ch%02d", ch));
    	Ped_adc2[ch][Nruns_adc2[ch]] = (hist_adc2[ch]->Integral()>6000 ? hist_adc2[ch]->GetBinCenter(hist_adc2[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_adc2[ch]->GetBinContent(hist_adc2[ch]->GetMaximumBin());
	FileOut << Ped_adc2[ch][Nruns_adc2[ch]] << " , " ;
	if(Ped_adc2[ch][Nruns_adc2[ch]]>1) {Run_adc2[ch][Nruns_adc2[ch]]=i; Nruns_adc2[ch]++;}
      }
      for(int ch=0; ch<32; ch++) {
        hist_qdc1[ch]=(TH1I*)FileIn.Get(Form("qdc1_ch%02d", ch));
    	Ped_qdc1[ch][Nruns_qdc1[ch]] = (hist_qdc1[ch]->Integral()>6000 ? hist_qdc1[ch]->GetBinCenter(hist_qdc1[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_qdc1[ch]->GetBinContent(hist_qdc1[ch]->GetMaximumBin());
	FileOut << Ped_qdc1[ch][Nruns_qdc1[ch]] << " , " ;
	if(Ped_qdc1[ch][Nruns_qdc1[ch]]>1) {Run_qdc1[ch][Nruns_qdc1[ch]]=i; Nruns_qdc1[ch]++;}
      }
      for(int ch=0; ch<32; ch++) {
        hist_qdc2[ch]=(TH1I*)FileIn.Get(Form("qdc2_ch%02d", ch));
    	Ped_qdc2[ch][Nruns_qdc2[ch]] = (hist_qdc2[ch]->Integral()>6000 ? hist_qdc2[ch]->GetBinCenter(hist_qdc2[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_qdc2[ch]->GetBinContent(hist_qdc2[ch]->GetMaximumBin());
	FileOut << Ped_qdc2[ch][Nruns_qdc2[ch]] << " , " ;
	if(Ped_qdc2[ch][Nruns_qdc2[ch]]>1) {Run_qdc2[ch][Nruns_qdc2[ch]]=i; Nruns_qdc2[ch]++;}
      }
      for(int ch=0; ch<32; ch++) {
        hist_qdc3[ch]=(TH1I*)FileIn.Get(Form("qdc3_ch%02d", ch));
    	Ped_qdc3[ch][Nruns_qdc3[ch]] = (hist_qdc3[ch]->Integral()>6000 ? hist_qdc3[ch]->GetBinCenter(hist_qdc3[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_qdc3[ch]->GetBinContent(hist_qdc3[ch]->GetMaximumBin());
	FileOut << Ped_qdc3[ch][Nruns_qdc3[ch]] << " , " ;
	if(Ped_qdc3[ch][Nruns_qdc3[ch]]>1) {Run_qdc3[ch][Nruns_qdc3[ch]]=i; Nruns_qdc3[ch]++;}
      }
      for(int ch=0; ch<32; ch++) {
        hist_qdc4[ch]=(TH1I*)FileIn.Get(Form("qdc4_ch%02d", ch));
    	Ped_qdc4[ch][Nruns_qdc4[ch]] = (hist_qdc4[ch]->Integral()>6000 ? hist_qdc4[ch]->GetBinCenter(hist_qdc4[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_qdc4[ch]->GetBinContent(hist_qdc4[ch]->GetMaximumBin());
	FileOut << Ped_qdc4[ch][Nruns_qdc4[ch]] << " , " ;
	if(Ped_qdc4[ch][Nruns_qdc4[ch]]>1) {Run_qdc4[ch][Nruns_qdc4[ch]]=i; Nruns_qdc4[ch]++;}
      }
      for(int ch=0; ch<32; ch++) {
        hist_qdc5[ch]=(TH1I*)FileIn.Get(Form("qdc5_ch%02d", ch));
    	Ped_qdc5[ch][Nruns_qdc5[ch]] = (hist_qdc5[ch]->Integral()>6000 ? hist_qdc5[ch]->GetBinCenter(hist_qdc5[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_qdc5[ch]->GetBinContent(hist_qdc5[ch]->GetMaximumBin());
	FileOut << Ped_qdc5[ch][Nruns_qdc5[ch]] << " , " ;
	if(Ped_qdc5[ch][Nruns_qdc5[ch]]>1) {Run_qdc5[ch][Nruns_qdc5[ch]]=i; Nruns_qdc5[ch]++;}
      }
      for(int ch=0; ch<32; ch++) {
        hist_qdc6[ch]=(TH1I*)FileIn.Get(Form("qdc6_ch%02d", ch));
    	Ped_qdc6[ch][Nruns_qdc6[ch]] = (hist_qdc6[ch]->Integral()>6000 ? hist_qdc6[ch]->GetBinCenter(hist_qdc6[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_qdc6[ch]->GetBinContent(hist_qdc6[ch]->GetMaximumBin());
	FileOut << Ped_qdc6[ch][Nruns_qdc6[ch]] << " , " ;
	if(Ped_qdc6[ch][Nruns_qdc6[ch]]>1) {Run_qdc6[ch][Nruns_qdc6[ch]]=i; Nruns_qdc6[ch]++;}
      }
      for(int ch=0; ch<32; ch++) {
        hist_qdc7[ch]=(TH1I*)FileIn.Get(Form("qdc7_ch%02d", ch));
    	Ped_qdc7[ch][Nruns_qdc7[ch]] = (hist_qdc7[ch]->Integral()>6000 ? hist_qdc7[ch]->GetBinCenter(hist_qdc7[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_qdc7[ch]->GetBinContent(hist_qdc7[ch]->GetMaximumBin());
	FileOut << Ped_qdc7[ch][Nruns_qdc7[ch]] << " , " ;
	if(Ped_qdc7[ch][Nruns_qdc7[ch]]>1) {Run_qdc7[ch][Nruns_qdc7[ch]]=i; Nruns_qdc7[ch]++;}
      }
      for(int ch=0; ch<32; ch++) {
        hist_qdc8[ch]=(TH1I*)FileIn.Get(Form("qdc8_ch%02d", ch));
    	Ped_qdc8[ch][Nruns_qdc8[ch]] = (hist_qdc8[ch]->Integral()>6000 ? hist_qdc8[ch]->GetBinCenter(hist_qdc8[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_qdc8[ch]->GetBinContent(hist_qdc8[ch]->GetMaximumBin());
	FileOut << Ped_qdc8[ch][Nruns_qdc8[ch]] << " , " ;
	if(Ped_qdc8[ch][Nruns_qdc8[ch]]>1) {Run_qdc8[ch][Nruns_qdc8[ch]]=i; Nruns_qdc8[ch]++;}
      }
      for(int ch=0; ch<32; ch++) {
        hist_qdcfart[ch]=(TH1I*)FileIn.Get(Form("qdcfart_ch%02d", ch));
    	Ped_qdcfart[ch][Nruns_qdcfart[ch]] = (hist_qdcfart[ch]->Integral()>6000 ? hist_qdcfart[ch]->GetBinCenter(hist_qdcfart[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_qdcfart[ch]->GetBinContent(hist_qdcfart[ch]->GetMaximumBin());
	FileOut << Ped_qdcfart[ch][Nruns_qdcfart[ch]] << " , " ;
	if(Ped_qdcfart[ch][Nruns_qdcfart[ch]]>1) {Run_qdcfart[ch][Nruns_qdcfart[ch]]=i; Nruns_qdcfart[ch]++;}
      }      
      for(int ch=0; ch<32; ch++) {
        hist_MBqdc1[ch]=(TH1I*)FileIn.Get(Form("MBqdc1_ch%02d", ch));
    	Ped_MBqdc1[ch][Nruns_MBqdc1[ch]] = (hist_MBqdc1[ch]->Integral()>6000 ? hist_MBqdc1[ch]->GetBinCenter(hist_MBqdc1[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_MBqdc1[ch]->GetBinContent(hist_MBqdc1[ch]->GetMaximumBin());
	FileOut << Ped_MBqdc1[ch][Nruns_MBqdc1[ch]] << " , " ;
	if(Ped_MBqdc1[ch][Nruns_MBqdc1[ch]]>1) {Run_MBqdc1[ch][Nruns_MBqdc1[ch]]=i; Nruns_MBqdc1[ch]++;}
      }
      for(int ch=0; ch<32; ch++) {
        hist_MBqdc2[ch]=(TH1I*)FileIn.Get(Form("MBqdc2_ch%02d", ch));
    	Ped_MBqdc2[ch][Nruns_MBqdc2[ch]] = (hist_MBqdc2[ch]->Integral()>6000 ? hist_MBqdc2[ch]->GetBinCenter(hist_MBqdc2[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_MBqdc2[ch]->GetBinContent(hist_MBqdc2[ch]->GetMaximumBin());
	FileOut << Ped_MBqdc2[ch][Nruns_MBqdc2[ch]] << " , " ;
	if(Ped_MBqdc2[ch][Nruns_MBqdc2[ch]]>1) {Run_MBqdc2[ch][Nruns_MBqdc2[ch]]=i; Nruns_MBqdc2[ch]++;}
      }
      for(int ch=0; ch<32; ch++) {
        hist_MBqdc3[ch]=(TH1I*)FileIn.Get(Form("MBqdc3_ch%02d", ch));
    	Ped_MBqdc3[ch][Nruns_MBqdc3[ch]] = (hist_MBqdc3[ch]->Integral()>6000 ? hist_MBqdc3[ch]->GetBinCenter(hist_MBqdc3[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_MBqdc3[ch]->GetBinContent(hist_MBqdc3[ch]->GetMaximumBin());
	FileOut << Ped_MBqdc3[ch][Nruns_MBqdc3[ch]] << " , " ;
	if(Ped_MBqdc3[ch][Nruns_MBqdc3[ch]]>1) {Run_MBqdc3[ch][Nruns_MBqdc3[ch]]=i; Nruns_MBqdc3[ch]++;}
      }
      for(int ch=0; ch<32; ch++) {
        hist_MBqdc4[ch]=(TH1I*)FileIn.Get(Form("MBqdc4_ch%02d", ch));
    	Ped_MBqdc4[ch][Nruns_MBqdc4[ch]] = (hist_MBqdc4[ch]->Integral()>6000 ? hist_MBqdc4[ch]->GetBinCenter(hist_MBqdc4[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_MBqdc4[ch]->GetBinContent(hist_MBqdc4[ch]->GetMaximumBin());
	FileOut << Ped_MBqdc4[ch][Nruns_MBqdc4[ch]] << " , " ;
	if(Ped_MBqdc4[ch][Nruns_MBqdc4[ch]]>1) {Run_MBqdc4[ch][Nruns_MBqdc4[ch]]=i; Nruns_MBqdc4[ch]++;}
      }
      for(int ch=0; ch<32; ch++) {
        hist_MBqdc5[ch]=(TH1I*)FileIn.Get(Form("MBqdc5_ch%02d", ch));
    	Ped_MBqdc5[ch][Nruns_MBqdc5[ch]] = (hist_MBqdc5[ch]->Integral()>6000 ? hist_MBqdc5[ch]->GetBinCenter(hist_MBqdc5[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_MBqdc5[ch]->GetBinContent(hist_MBqdc5[ch]->GetMaximumBin());
	FileOut << Ped_MBqdc5[ch][Nruns_MBqdc5[ch]] << " , " ;
	if(Ped_MBqdc5[ch][Nruns_MBqdc5[ch]]>1) {Run_MBqdc5[ch][Nruns_MBqdc5[ch]]=i; Nruns_MBqdc5[ch]++;}
      }
      for(int ch=0; ch<32; ch++) {
        hist_MBqdc6[ch]=(TH1I*)FileIn.Get(Form("MBqdc6_ch%02d", ch));
    	Ped_MBqdc6[ch][Nruns_MBqdc6[ch]] = (hist_MBqdc6[ch]->Integral()>6000 ? hist_MBqdc6[ch]->GetBinCenter(hist_MBqdc6[ch]->GetMaximumBin()) : 0);
	PedAmplitude=hist_MBqdc6[ch]->GetBinContent(hist_MBqdc6[ch]->GetMaximumBin());
	FileOut << Ped_MBqdc6[ch][Nruns_MBqdc6[ch]] << " , " ;
	if(Ped_MBqdc6[ch][Nruns_MBqdc6[ch]]>1) {Run_MBqdc6[ch][Nruns_MBqdc6[ch]]=i; Nruns_MBqdc6[ch]++;}
      }
      
      FileOut<<std::endl; 
    
  }
  
  for(int ch=0; ch<32; ch++) {
    graph_adc1[ch]=new TGraph(Nruns_adc1[ch],Run_adc1[ch], Ped_adc1[ch]);
    graph_adc2[ch]=new TGraph(Nruns_adc2[ch],Run_adc2[ch], Ped_adc2[ch]);
    graph_qdc1[ch]=new TGraph(Nruns_qdc1[ch],Run_qdc1[ch], Ped_qdc1[ch]);
    graph_qdc2[ch]=new TGraph(Nruns_qdc2[ch],Run_qdc2[ch], Ped_qdc2[ch]);
    graph_qdc3[ch]=new TGraph(Nruns_qdc3[ch],Run_qdc3[ch], Ped_qdc3[ch]);
    graph_qdc4[ch]=new TGraph(Nruns_qdc4[ch],Run_qdc4[ch], Ped_qdc4[ch]);
    graph_qdc5[ch]=new TGraph(Nruns_qdc5[ch],Run_qdc5[ch], Ped_qdc5[ch]);
    graph_qdc6[ch]=new TGraph(Nruns_qdc6[ch],Run_qdc6[ch], Ped_qdc6[ch]);
    graph_qdc7[ch]=new TGraph(Nruns_qdc7[ch],Run_qdc7[ch], Ped_qdc7[ch]);
    graph_qdc8[ch]=new TGraph(Nruns_qdc8[ch],Run_qdc8[ch], Ped_qdc8[ch]);
    graph_qdcfart[ch]=new TGraph(Nruns_qdcfart[ch],Run_qdcfart[ch], Ped_qdcfart[ch]);
    graph_MBqdc1[ch]=new TGraph(Nruns_MBqdc1[ch],Run_MBqdc1[ch], Ped_MBqdc1[ch]);
    graph_MBqdc2[ch]=new TGraph(Nruns_MBqdc2[ch],Run_MBqdc2[ch], Ped_MBqdc2[ch]);
    graph_MBqdc3[ch]=new TGraph(Nruns_MBqdc3[ch],Run_MBqdc3[ch], Ped_MBqdc3[ch]);
    graph_MBqdc4[ch]=new TGraph(Nruns_MBqdc4[ch],Run_MBqdc4[ch], Ped_MBqdc4[ch]);
    graph_MBqdc5[ch]=new TGraph(Nruns_MBqdc5[ch],Run_MBqdc5[ch], Ped_MBqdc5[ch]);
    graph_MBqdc6[ch]=new TGraph(Nruns_MBqdc6[ch],Run_MBqdc6[ch], Ped_MBqdc6[ch]);
    FileOutRoot->WriteTObject(graph_adc1[ch], Form("graph_adc1_%02d",ch));
    FileOutRoot->WriteTObject(graph_adc2[ch], Form("graph_adc2_%02d",ch));
    FileOutRoot->WriteTObject(graph_qdc1[ch], Form("graph_qdc1_%02d",ch));
    FileOutRoot->WriteTObject(graph_qdc2[ch], Form("graph_qdc2_%02d",ch));
    FileOutRoot->WriteTObject(graph_qdc3[ch], Form("graph_qdc3_%02d",ch));
    FileOutRoot->WriteTObject(graph_qdc4[ch], Form("graph_qdc4_%02d",ch));
    FileOutRoot->WriteTObject(graph_qdc5[ch], Form("graph_qdc5_%02d",ch));
    FileOutRoot->WriteTObject(graph_qdc6[ch], Form("graph_qdc6_%02d",ch));
    FileOutRoot->WriteTObject(graph_qdc7[ch], Form("graph_qdc7_%02d",ch));
    FileOutRoot->WriteTObject(graph_qdc8[ch], Form("graph_qdc8_%02d",ch));
    FileOutRoot->WriteTObject(graph_qdcfart[ch], Form("graph_qdcfart_%02d",ch));
    FileOutRoot->WriteTObject(graph_MBqdc1[ch], Form("graph_MBqdc1_%02d",ch));
    FileOutRoot->WriteTObject(graph_MBqdc2[ch], Form("graph_MBqdc2_%02d",ch));
    FileOutRoot->WriteTObject(graph_MBqdc3[ch], Form("graph_MBqdc3_%02d",ch));
    FileOutRoot->WriteTObject(graph_MBqdc4[ch], Form("graph_MBqdc4_%02d",ch));
    FileOutRoot->WriteTObject(graph_MBqdc5[ch], Form("graph_MBqdc5_%02d",ch));
    FileOutRoot->WriteTObject(graph_MBqdc6[ch], Form("graph_MBqdc6_%02d",ch));
  }
  
  FileOut.close();
  FileOutRoot->Close();
  
  return 0;
}
