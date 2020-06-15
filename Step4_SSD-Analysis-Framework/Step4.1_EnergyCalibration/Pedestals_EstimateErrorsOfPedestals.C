///////////////////////////////////////////////////////////////////////////////
//
//   考察不同的实验条件下，硅条的 Pedestals 的差异
//
//   gfh , 2020-06-04
////////////////////////////////////////////////////////////////////////////////

#include "../../include/ReadFileModule.h"


//______________________________________________________________________________
void Pedestals_EstimateErrorsOfPedestals()
{
   std::string LayerTag("L2");
   std::string LayerWithLabelTag("L2B");
   std::string CaliTag1("PulserCaliPedestals");   // "PulserCaliPedestals" "AlphaCaliPedestals"
   std::string CaliTag2("AlphaCaliPedestals");
   std::string FileTag1("Pedestal0000");        // "Pedestal0000"  "AlphaCali33_48"
   std::string FileTag2("AlphaCali33_48");

   std::string pathPedestal1(Form("output/SSD_%s_%s_%s.dat",LayerWithLabelTag.c_str(),
                               CaliTag1.c_str(),FileTag1.c_str()));
   std::string pathPedestal2(Form("output/SSD_%s_%s_%s.dat",LayerWithLabelTag.c_str(),
                               CaliTag2.c_str(),FileTag2.c_str()));
   std::string pathPNGOut(Form("figures/SSD_%s_PedestalError_%s_%s.png",LayerWithLabelTag.c_str(),
                          FileTag1.c_str(),FileTag2.c_str()));

   Int_t SSDNum = 4;
   Int_t CHNum  = 16;
   Int_t numpar = 2;    // mean, sigma
   Float_t small = 1e-5;
   Double_t XLow = 0;
   Double_t XUp  = 17;

   ReadFileModule readfile;
   Double_t*** Pedestal1 = readfile.ReadData(pathPedestal1.c_str(),SSDNum,CHNum,numpar);
   Double_t*** Pedestal2 = readfile.ReadData(pathPedestal2.c_str(),SSDNum,CHNum,numpar);

   Double_t SSDCHNum[SSDNum][CHNum];
   Double_t err_pedestal[SSDNum][CHNum];

   for (Int_t i=0; i<SSDNum; i++)
   {
     for (Int_t j=0; j<CHNum; j++)
     {
       SSDCHNum[i][j] = j+1;
       err_pedestal[i][j] = abs(Pedestal1[i][j][0]-Pedestal2[i][j][0]);
       //err_pedestal[i][j] = 100*abs(Pedestal1[i][j][0]-Pedestal2[i][j][0])/Pedestal1[i][j][0];
     }
   }

   // build graphs
   TGraph* graph[SSDNum];
   for (Int_t i=0; i<SSDNum; i++)
   {
     graph[i] = new TGraph(CHNum,SSDCHNum[i],err_pedestal[i]);
     graph[i]->SetTitle(Form("SSD%d_%s_PedestalError_%s_%s",i+1,LayerWithLabelTag.c_str(),
                        FileTag1.c_str(),FileTag2.c_str()));
   }

   // draw
   TCanvas* cans = new TCanvas("cans","Error of Pedestals",1200,1000);
   cans->Divide(2,2,small,small);

   cans->cd(1);
   gPad->SetGridx();
   gPad->SetLeftMargin(0.13);
   graph[0]->SetMarkerStyle(20);
   graph[0]->SetMarkerColor(kRed);
   graph[0]->GetXaxis()->SetRangeUser(XLow,XUp);
   graph[0]->GetXaxis()->SetNdivisions(117);
   graph[0]->GetXaxis()->SetTitle("Strip_Number");
   graph[0]->GetXaxis()->CenterTitle(1);
   graph[0]->GetXaxis()->SetTitleSize(0.04);
   graph[0]->GetYaxis()->SetTitle("PedestalError (Ch)");
   graph[0]->GetYaxis()->CenterTitle(1);
   graph[0]->GetYaxis()->SetTitleSize(0.04);
   graph[0]->Draw("APL");

   cans->cd(2);
   gPad->SetGridx();
   gPad->SetLeftMargin(0.13);
   graph[1]->SetMarkerStyle(20);
   graph[1]->SetMarkerColor(kRed);
   graph[1]->GetXaxis()->SetRangeUser(XLow,XUp);
   graph[1]->GetXaxis()->SetNdivisions(117);
   graph[1]->GetXaxis()->SetTitle("Strip_Number");
   graph[1]->GetXaxis()->CenterTitle(1);
   graph[1]->GetXaxis()->SetTitleSize(0.04);
   graph[1]->GetYaxis()->SetTitle("PedestalError (Ch)");
   graph[1]->GetYaxis()->CenterTitle(1);
   graph[1]->GetYaxis()->SetTitleSize(0.04);
   graph[1]->Draw("APL");

   cans->cd(3);
   gPad->SetGridx();
   gPad->SetLeftMargin(0.13);
   graph[2]->SetMarkerStyle(20);
   graph[2]->SetMarkerColor(kRed);
   graph[2]->GetXaxis()->SetRangeUser(XLow,XUp);
   graph[2]->GetXaxis()->SetNdivisions(117);
   graph[2]->GetXaxis()->SetTitle("Strip_Number");
   graph[2]->GetXaxis()->CenterTitle(1);
   graph[2]->GetXaxis()->SetTitleSize(0.04);
   graph[2]->GetYaxis()->SetTitle("PedestalError (Ch)");
   graph[2]->GetYaxis()->CenterTitle(1);
   graph[2]->GetYaxis()->SetTitleSize(0.04);
   graph[2]->Draw("APL");

   cans->cd(4);
   gPad->SetGridx();
   gPad->SetLeftMargin(0.13);
   graph[3]->SetMarkerStyle(20);
   graph[3]->SetMarkerColor(kRed);
   graph[3]->GetXaxis()->SetRangeUser(XLow,XUp);
   graph[3]->GetXaxis()->SetNdivisions(117);
   graph[3]->GetXaxis()->SetTitle("Strip_Number");
   graph[3]->GetXaxis()->CenterTitle(1);
   graph[3]->GetXaxis()->SetTitleSize(0.04);
   graph[3]->GetYaxis()->SetTitle("PedestalError (Ch)");
   graph[3]->GetYaxis()->CenterTitle(1);
   graph[3]->GetYaxis()->SetTitleSize(0.04);
   graph[3]->Draw("APL");

   cans->Print(pathPNGOut.c_str());
}
