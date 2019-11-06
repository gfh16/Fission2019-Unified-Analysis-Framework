// This macro is used to find the pedestals for Energy Channel of SSD



int Index = 0;
int NPoints;
TMarker *m[2];
TLine *l[2];

void FindPedestals()
{

   std::string FileOutTag1("Pedestals");  // here to change "Pedestals" or "Pulser" or "Alpha"
   std::string FileOutTag2("L1S_E");      // here to change "L1S_E" or "L2F_E" or "L2B_E" or "L3A_E"
   std::string path_to_file("data/QC_MapSSD_PulserCali_Pedestal0000.root");

   TFile * FileIn = new TFile(path_to_file.c_str());
   if(!FileIn)
   {
     cout<<"Open file "<< path_to_file.c_str() << "failed"<<endl;
   }

   ofstream FileOut(Form("output/SSD_%s_%s.dat", FileOutTag1.c_str(), FileOutTag2.c_str()));
   FileOut << "*SSDNum" << setw(7) << "CHNum" << setw(12) << "Mean"<< setw(12)<< "sigma"<< setw(17)
           <<"Mean+5sigma"<< setw(12)<<"Chi2" << setw(12) <<"Xmin"<< setw(12)<<"Xmax" << endl;

   TH1D * PedestalHist[4][16];
   for(int i=0; i<4; i++)
   {
     for(int j=0; j<16; j++)
     {
       PedestalHist[i][j]=(TH1D*)FileIn->Get(Form("SSD%d_%s_CH%d",(i+1),FileOutTag2.c_str(),j));
       //cout<< PedestalHist[i][j]->GetName()<<endl;
     }
   }
   printf("Histograms loaded\n");

   float limit[2] = {0};
   TCanvas *c_begin = new TCanvas("c2","");
   TCanvas *c_end   = new TCanvas("c3","");
   TCanvas *c1 = new TCanvas("c1","c1",1200,800);


   c_begin->Print(Form("figures/SSD_%s_%s.pdf[", FileOutTag1.c_str(), FileOutTag2.c_str()));
   //========================================================
   //   behgen analysis here
   for(int i=0; i<4; i++)
   {
     for(int j=0; j<16; j++)
     {
       printf("Analysis SSD%d_%s_CH%d\n",(i+1),FileOutTag2.c_str(),j);

       int Option = 2;

       c1->cd();
       gPad->Modified();
       gPad->Update();

       if(PedestalHist[i][j]==0)
       {
         printf("No data present for SSD%d_%s_CH%d\n",(i+1),FileOutTag2.c_str(),j);
         continue;
       }

       PedestalHist[i][j]->GetXaxis()->SetRangeUser(50,150);
       PedestalHist[i][j]->Draw();

       if(Option==1) Option=2;
       while(Option==2)
       {
         Index = 0;
         NPoints = 0;

         //===========================================================
         //   it is very important!!! very convenient!!!
         //   here to set the range and fit on the GUI by hand
         c1->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",0,0,"SetPoints(Int_t,Int_t,Int_t,TObject*)");
         while(Index!=1)
         {
           usleep(100);
           gClient->HandleInput();
         }
         c1->Disconnect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)");

         if(Index==1)
         {
           for(int ip=0; ip<NPoints; ip++)
           {
             limit[ip] = m[ip]->GetX();
           }
         }
         //cout<<"The Fit range is (" << limit[0] << "," << limit[1] <<")" <<endl;

         PedestalHist[i][j]->Fit("gaus","","same",limit[0],limit[1]);

      //   gPad->Modified();
      //   gPad->Update();

         cout<< "Options? 1. Save and next, 2.Retry the fit"<< endl;
         cin >> Option;
         if(Option==1)
         {
           TF1 *fit = PedestalHist[i][j]->GetFunction("gaus");
           double Mean  = fit->GetParameter(1);
           double Sigma = fit->GetParameter(2);
           double Chi2  = fit->GetChisquare();

           FileOut<<i+1<< setw(10) <<j<< setw(15)<<Mean<<setw(12)<<Sigma<< setw(17) <<Mean+5*Sigma
                  <<setw(12)<<Chi2<<setw(12)<<limit[0]<<setw(12)<<limit[1]<<endl;

           c1->Print(Form("figures/SSD_%s_%s.pdf", FileOutTag1.c_str(), FileOutTag2.c_str()));

         }
       } // close while(Option==2)
     }   // close silicon strip  (j)
   }     // close silicon number (i)
   c_end->Print(Form("figures/SSD_%s_%s.pdf]", FileOutTag1.c_str(), FileOutTag2.c_str()));

   delete c_begin;
   delete c1;
   delete c_end;

   FileOut.close();
   cout<<"File .dat closed"<<endl;
   return;
} // end void FindPedestals()

//==============================================================================


////////////////////////////////////////////////////////////////////////////////
//// the macro to set the fit range by hand on GUI
///  Click the Central button of the mouse to get the range
void SetPoints(Int_t event, Int_t x, Int_t y, TObject *selected)
{
  if(event == 2)
  {
    float px = gPad->AbsPixeltoX(x);
    float py = gPad->AbsPixeltoX(y);
    py = gPad->PadtoY(py);
    float Uymin = gPad->GetUymin(); //Returns the minimum y-coordinate value visible on the pad
    float Uymax = gPad->GetUymax();

    //=================================================
    //  save the clicks as a marker
    if(px>=gPad->GetUxmin() && px<=gPad->GetUxmax())
    {
      m[NPoints] = new TMarker(px,py,3);          // marker style = 3, means “*”
      l[NPoints] = new TLine(px,Uymin,px,Uymax);  //画出 X=px 这条线段
      l[NPoints] -> SetLineColor(2);
      l[NPoints] -> SetLineWidth(2);
      l[NPoints] -> Draw();
      for(int i=0; i<NPoints; i++)
      {
        l[i] -> Draw();
      }
      gPad->Modified();    // Tell the canvas that an object it is displaying have changed
      gPad->Update();      //  Force the canvas to refresh
      printf("x=%f\n", px);
      NPoints++;

      //==============================================
      //  *** I get only 2 points and then I return
      if(NPoints==2)
      {
        Index = 1;   // Index
        return;
      }
      cout<< "Click next point"<< endl;
    }
  }

  return;

}
