void figure_PlotEachModule()
{
  TFile FileIn("report/QDCPedestals_2000_4663.root");
  
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
  
  for(int ch=0; ch<32; ch++) {
    graph_adc1[ch]=(TGraph*)FileIn.Get(Form("graph_adc1_%02d",ch));
    graph_adc2[ch]=(TGraph*)FileIn.Get(Form("graph_adc2_%02d",ch));
    graph_qdc1[ch]=(TGraph*)FileIn.Get(Form("graph_qdc1_%02d",ch));
    graph_qdc2[ch]=(TGraph*)FileIn.Get(Form("graph_qdc2_%02d",ch));
    graph_qdc3[ch]=(TGraph*)FileIn.Get(Form("graph_qdc3_%02d",ch));
    graph_qdc4[ch]=(TGraph*)FileIn.Get(Form("graph_qdc4_%02d",ch));
    graph_qdc5[ch]=(TGraph*)FileIn.Get(Form("graph_qdc5_%02d",ch));
    graph_qdc6[ch]=(TGraph*)FileIn.Get(Form("graph_qdc6_%02d",ch));
    graph_qdc7[ch]=(TGraph*)FileIn.Get(Form("graph_qdc7_%02d",ch));
    graph_qdc8[ch]=(TGraph*)FileIn.Get(Form("graph_qdc8_%02d",ch));
    graph_qdcfart[ch]=(TGraph*)FileIn.Get(Form("graph_qdcfart_%02d",ch));
    graph_MBqdc1[ch]=(TGraph*)FileIn.Get(Form("graph_MBqdc1_%02d",ch));
    graph_MBqdc2[ch]=(TGraph*)FileIn.Get(Form("graph_MBqdc2_%02d",ch));
    graph_MBqdc3[ch]=(TGraph*)FileIn.Get(Form("graph_MBqdc3_%02d",ch));
    graph_MBqdc4[ch]=(TGraph*)FileIn.Get(Form("graph_MBqdc4_%02d",ch));
    graph_MBqdc5[ch]=(TGraph*)FileIn.Get(Form("graph_MBqdc5_%02d",ch));
    graph_MBqdc6[ch]=(TGraph*)FileIn.Get(Form("graph_MBqdc6_%02d",ch));
  }
  
  //TCanvas * adc1 = new TCanvas("adc1","",1024,768);
  TMultiGraph * multigraph_adc1 = new TMultiGraph();
  multigraph_adc1->SetTitle("adc1");
  for(int ch=0; ch<32; ch++) {
    if(graph_adc1[ch]==0) continue;
    graph_adc1[ch]->SetTitle(Form("%02d", ch));
    graph_adc1[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_adc1[ch]->SetMarkerStyle(6);
    multigraph_adc1->Add(graph_adc1[ch]);
  }
  //multigraph_adc1->Draw("APL");
  
  //TCanvas * adc2 = new TCanvas("adc2","",1024,768);
  TMultiGraph * multigraph_adc2 = new TMultiGraph();
  multigraph_adc2->SetTitle("adc2");
  for(int ch=0; ch<32; ch++) {
    if(graph_adc2[ch]==0) continue;
    graph_adc2[ch]->SetTitle(Form("%02d", ch));
    graph_adc2[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_adc2[ch]->SetMarkerStyle(6);
    multigraph_adc2->Add(graph_adc2[ch]);
  }
  //multigraph_adc2->Draw("APL");
  
  TCanvas * qdc1 = new TCanvas("qdc1","",1024,768);
  TMultiGraph * multigraph_qdc1 = new TMultiGraph();
  multigraph_qdc1->SetTitle("qdc1");
  for(int ch=0; ch<32; ch++) {
    if(graph_qdc1[ch]==0) continue;
    graph_qdc1[ch]->SetTitle(Form("%02d", ch));
    graph_qdc1[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_qdc1[ch]->SetLineColor(TColor::GetColorDark(ch));
    graph_qdc1[ch]->SetMarkerStyle(6);
    multigraph_qdc1->Add(graph_qdc1[ch]);
  }
  multigraph_qdc1->Draw("APL");
  
  TCanvas * qdc2 = new TCanvas("qdc2","",1024,768);
  TMultiGraph * multigraph_qdc2 = new TMultiGraph();
  multigraph_qdc2->SetTitle("qdc2");
  for(int ch=0; ch<32; ch++) {
    if(graph_qdc2[ch]==0) continue;
    graph_qdc2[ch]->SetTitle(Form("%02d", ch));
    graph_qdc2[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_qdc2[ch]->SetLineColor(TColor::GetColorDark(ch));
    graph_qdc2[ch]->SetMarkerStyle(6);
    multigraph_qdc2->Add(graph_qdc2[ch]);
  }
  multigraph_qdc2->Draw("APL");
  
  TCanvas * qdc3 = new TCanvas("qdc3","",1024,768);
  TMultiGraph * multigraph_qdc3 = new TMultiGraph();
  multigraph_qdc3->SetTitle("qdc3");
  for(int ch=0; ch<32; ch++) {
    if(graph_qdc3[ch]==0) continue;
    graph_qdc3[ch]->SetTitle(Form("%02d", ch));
    graph_qdc3[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_qdc3[ch]->SetLineColor(TColor::GetColorDark(ch));
    graph_qdc3[ch]->SetMarkerStyle(6);
    multigraph_qdc3->Add(graph_qdc3[ch]);
  }
  multigraph_qdc3->Draw("APL");
  
  //TCanvas * qdc4 = new TCanvas("qdc4","",1024,768);
  TMultiGraph * multigraph_qdc4 = new TMultiGraph();
  multigraph_qdc4->SetTitle("qdc4");
  for(int ch=0; ch<32; ch++) {
    if(graph_qdc4[ch]==0) continue;
    graph_qdc4[ch]->SetTitle(Form("%02d", ch));
    graph_qdc4[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_qdc4[ch]->SetMarkerStyle(6);
    multigraph_qdc4->Add(graph_qdc4[ch]);
  }
  //multigraph_qdc4->Draw("APL");
  
  //TCanvas * qdc5 = new TCanvas("qdc5","",1024,768);
  TMultiGraph * multigraph_qdc5 = new TMultiGraph();
  multigraph_qdc5->SetTitle("qdc5");
  for(int ch=0; ch<32; ch++) {
    if(graph_qdc5[ch]==0) continue;
    graph_qdc5[ch]->SetTitle(Form("%02d", ch));
    graph_qdc5[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_qdc5[ch]->SetMarkerStyle(6);
    multigraph_qdc5->Add(graph_qdc5[ch]);
  }
  //multigraph_qdc5->Draw("APL");  
  
  //TCanvas * qdc6 = new TCanvas("qdc6","",1024,768);
  TMultiGraph * multigraph_qdc6 = new TMultiGraph();
  multigraph_qdc6->SetTitle("qdc6");
  for(int ch=0; ch<32; ch++) {
    if(graph_qdc6[ch]==0) continue;
    graph_qdc6[ch]->SetTitle(Form("%02d", ch));
    graph_qdc6[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_qdc6[ch]->SetMarkerStyle(6);
    multigraph_qdc6->Add(graph_qdc6[ch]);
  }
  //multigraph_qdc6->Draw("APL");  
  
  TCanvas * qdc7 = new TCanvas("qdc7","",1024,768);
  TMultiGraph * multigraph_qdc7 = new TMultiGraph();
  multigraph_qdc7->SetTitle("qdc7");
  for(int ch=0; ch<32; ch++) {
    if(graph_qdc7[ch]==0) continue;
    graph_qdc7[ch]->SetTitle(Form("%02d", ch));
    graph_qdc7[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_qdc7[ch]->SetLineColor(TColor::GetColorDark(ch));
    graph_qdc7[ch]->SetMarkerStyle(6);
    multigraph_qdc7->Add(graph_qdc7[ch]);
  }
  multigraph_qdc7->Draw("APL");  
  
  TCanvas * qdc8 = new TCanvas("qdc8","",1024,768);
  TMultiGraph * multigraph_qdc8 = new TMultiGraph();
  multigraph_qdc8->SetTitle("qdc8");
  for(int ch=0; ch<32; ch++) {
    if(graph_qdc8[ch]==0) continue;
    graph_qdc8[ch]->SetTitle(Form("%02d", ch));
    graph_qdc8[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_qdc8[ch]->SetLineColor(TColor::GetColorDark(ch));
    graph_qdc8[ch]->SetMarkerStyle(6);
    multigraph_qdc8->Add(graph_qdc8[ch]);
  }
  multigraph_qdc8->Draw("APL");    
  
  TCanvas * qdcfart = new TCanvas("qdcfart","",1024,768);
  TMultiGraph * multigraph_qdcfart = new TMultiGraph();
  multigraph_qdcfart->SetTitle("qdcfart");
  for(int ch=0; ch<32; ch++) {
    if(graph_qdcfart[ch]==0) continue;
    graph_qdcfart[ch]->SetTitle(Form("%02d", ch));
    graph_qdcfart[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_qdcfart[ch]->SetLineColor(TColor::GetColorDark(ch));
    graph_qdcfart[ch]->SetMarkerStyle(6);
    multigraph_qdcfart->Add(graph_qdcfart[ch]);
  }
  multigraph_qdcfart->Draw("APL");   
      
  TCanvas * MBqdc1 = new TCanvas("MBqdc1","",1024,768);
  TMultiGraph * multigraph_MBqdc1 = new TMultiGraph();
  multigraph_MBqdc1->SetTitle("MBqdc1");
  for(int ch=0; ch<32; ch++) {
    if(graph_MBqdc1[ch]==0) continue;
    graph_MBqdc1[ch]->SetTitle(Form("%02d", ch));
    graph_MBqdc1[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_MBqdc1[ch]->SetLineColor(TColor::GetColorDark(ch));
    graph_MBqdc1[ch]->SetMarkerStyle(6);
    multigraph_MBqdc1->Add(graph_MBqdc1[ch]);
  }
  multigraph_MBqdc1->Draw("APL");
  
  TCanvas * MBqdc2 = new TCanvas("MBqdc2","",1024,768);
  TMultiGraph * multigraph_MBqdc2 = new TMultiGraph();
  multigraph_MBqdc2->SetTitle("MBqdc2");
  for(int ch=0; ch<32; ch++) {
    if(graph_MBqdc2[ch]==0) continue;
    graph_MBqdc2[ch]->SetTitle(Form("%02d", ch));
    graph_MBqdc2[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_MBqdc2[ch]->SetLineColor(TColor::GetColorDark(ch));
    graph_MBqdc2[ch]->SetMarkerStyle(6);
    multigraph_MBqdc2->Add(graph_MBqdc2[ch]);
  }
  multigraph_MBqdc2->Draw("APL");
  
  TCanvas * MBqdc3 = new TCanvas("MBqdc3","",1024,768);
  TMultiGraph * multigraph_MBqdc3 = new TMultiGraph();
  multigraph_MBqdc3->SetTitle("MBqdc3");
  for(int ch=0; ch<32; ch++) {
    if(graph_MBqdc3[ch]==0) continue;
    graph_MBqdc3[ch]->SetTitle(Form("%02d", ch));
    graph_MBqdc3[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_MBqdc3[ch]->SetLineColor(TColor::GetColorDark(ch));
    graph_MBqdc3[ch]->SetMarkerStyle(6);
    multigraph_MBqdc3->Add(graph_MBqdc3[ch]);
  }
  multigraph_MBqdc3->Draw("APL");
  
  TCanvas * MBqdc4 = new TCanvas("MBqdc4","",1024,768);
  TMultiGraph * multigraph_MBqdc4 = new TMultiGraph();
  multigraph_MBqdc4->SetTitle("MBqdc4");
  for(int ch=0; ch<32; ch++) {
    if(graph_MBqdc4[ch]==0) continue;
    graph_MBqdc4[ch]->SetTitle(Form("%02d", ch));
    graph_MBqdc4[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_MBqdc4[ch]->SetLineColor(TColor::GetColorDark(ch));
    graph_MBqdc4[ch]->SetMarkerStyle(6);
    multigraph_MBqdc4->Add(graph_MBqdc4[ch]);
  }
  multigraph_MBqdc4->Draw("APL");
  
  TCanvas * MBqdc5 = new TCanvas("MBqdc5","",1024,768);
  TMultiGraph * multigraph_MBqdc5 = new TMultiGraph();
  multigraph_MBqdc5->SetTitle("MBqdc5");
  for(int ch=0; ch<32; ch++) {
    if(graph_MBqdc5[ch]==0) continue;
    graph_MBqdc5[ch]->SetTitle(Form("%02d", ch));
    graph_MBqdc5[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_MBqdc5[ch]->SetLineColor(TColor::GetColorDark(ch));
    graph_MBqdc5[ch]->SetMarkerStyle(6);
    multigraph_MBqdc5->Add(graph_MBqdc5[ch]);
  }
  multigraph_MBqdc5->Draw("APL");
  
  TCanvas * MBqdc6 = new TCanvas("MBqdc6","",1024,768);
  TMultiGraph * multigraph_MBqdc6 = new TMultiGraph();
  multigraph_MBqdc6->SetTitle("MBqdc6");
  for(int ch=0; ch<32; ch++) {
    if(graph_MBqdc6[ch]==0) continue;
    graph_MBqdc6[ch]->SetTitle(Form("%02d", ch));
    graph_MBqdc6[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graph_MBqdc6[ch]->SetLineColor(TColor::GetColorDark(ch));
    graph_MBqdc6[ch]->SetMarkerStyle(6);
    multigraph_MBqdc6->Add(graph_MBqdc6[ch]);
  }
  multigraph_MBqdc6->Draw("APL");  
  
  
}
