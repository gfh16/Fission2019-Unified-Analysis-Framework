void exec_VisualizePedestals(int run_fixed=0)
{
  std::string ModuleName("qdcfart");
  
  TCanvas * c1 = new TCanvas("c1", "", 1500, 1000);
  c1->Divide(8,4);
  
  for(int run=2000; run<=4663; run++) 
  {
    run=(run_fixed>0 ? run_fixed : run);
  
    TFile FileIn(Form("output/QDCSpectra_%04d_%04d.root", run, run));
    
    if(!FileIn.IsOpen()) continue;
    
    c1->SetTitle(Form("run%04d", run));
        
    TH1I * histograms[32];
    
    for(int ch=0; ch<32; ch++) {
      c1->cd(ch+1);
    
      histograms[ch]=(TH1I*)FileIn.Get(Form("%s_ch%02d", ModuleName.c_str(), ch));
      
      if(histograms[ch]==0) continue;
      
      histograms[ch]->Draw();
      
      gPad->SetLogy(true);

    }
    
    gPad->Modified();
    gPad->Update(); 
    //getchar();
      
    if(run_fixed>0) break;    
  }
  
  getchar();

}
