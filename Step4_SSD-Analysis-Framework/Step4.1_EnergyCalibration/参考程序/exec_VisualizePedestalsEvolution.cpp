void exec_VisualizePedestalsEvolution(int run_begin=0, int run_end=0)
{
  std::string ModuleName("qdcfart");
  
  TCanvas * c1 = new TCanvas("c1", "", 1500, 1000);
  c1->Divide(8,4);
  
  TFile FileIn(Form("report/QDCPedestals_%04d_%04d.root", run_begin >0 ? run_begin : 2000, run_end >0 ? run_end : 4663));
  
  if(!FileIn.IsOpen()) return;
        
  TGraph * graphs[32];
    
  for(int ch=0; ch<32; ch++) {
    c1->cd(ch+1);
    
    graphs[ch]=(TGraph*)FileIn.Get(Form("graph_%s_%02d",ModuleName.c_str(),ch));
    
    if(graphs[ch]==0) continue;
    
    graphs[ch]->SetTitle(Form("%02d", ch));
    graphs[ch]->SetMarkerColor(TColor::GetColorDark(ch));
    graphs[ch]->SetLineColor(TColor::GetColorDark(ch));
    graphs[ch]->SetMarkerStyle(6);
      
    graphs[ch]->Draw("APL");
    
    graphs[ch]->GetYaxis()->SetRangeUser(0,650);
      
  }
    
  gPad->Modified();
  gPad->Update();   
  getchar();

}
