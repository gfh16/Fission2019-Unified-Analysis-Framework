////////////////////////////////////////////////////////////////////////////////
/// This program is taken from Dell'Aquila, Daniele
///  modified by F.H.Guan
///  Oct 29, 2018 @MSU
////////////////////////////////////////////////////////////////////////////////
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TChain.h"
#include "TNtuple.h"
#include "TCutG.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TF1.h"
#include "TH2.h"
#include "TH1.h"
#include "TMarker.h"
#include "TObject.h"
#include "TPad.h"
#include "TGClient.h"
#include "TLine.h"
using namespace std;

int ica;
int np;
TMarker *m[2];
TLine *l[2];

///////////////////////////////////////////////////////////////////////////////
/// here to change the input data file && cut file
void TogetPunchThroughPoints(char const *histofilename="../../data/HiRA_DECal_CsIRaw_Pixelization.root", char const *cutfile="../../data/HiRA_DECal_CsIRaw_Cuts.root", int mode=0, int itel=0, int icsi=0)
{
  // open files and create new files
  TFile *f = new TFile (histofilename);
  TFile *g = new TFile (cutfile);
  TFile *outroot;
  FILE *results;

  TH2D *DEEHist[48];
  TCutG *cut[48][3];
  if(mode==0)
  {
    itel = 0; icsi=0;
    ////////////////////////////////////////////////////////////////////////////
    /// here to chnage the directory to save the output files
    outroot = new TFile ("data/HiRA_CsIPunchThrough_pdt_new.root","RECREATE");
    results= fopen("data/HiRA_CsIPunchThrough_pdt_new.dat","w");
  }
  else
  {
    outroot = new TFile (Form("data/HiRA_Punch_Through_pdt_tel%02d_csi%02d.root",itel,icsi),"RECREATE");
    results= fopen(Form("data/HiRA_Punch_Through_pdt_tel%02d_csi%02d.dat",itel,icsi),"w");
  }
  fprintf(results,"# itel   icsi   Z   A   Pt(ch)  ErrPt  KT  ErrKt  Chi2Rid  Xmin  Xmax \n");

  //Load Histograms
  for(int i=0, k=0; i<12; i++)
  {
    for(int j=0; j<4; j++, k++)
    {
      //////////////////////////////////////////////////////////////////////////
      ///// pay attention!!!!!
      ////  here to change the Object Name
      DEEHist[k]=(TH2D*)f->Get(Form("DECal_ERaw_%02d_%02d", i, j));
      //cout << DEEHist[k]->GetName() << endl;
      cut[k][0]=(TCutG*)g->Get(Form("DECal_ERaw_%02d_%02d_Z01_A01",i,j));
      //cout << cut[k][0]->GetName() << endl;
      cut[k][1]=(TCutG*)g->Get(Form("DECal_ERaw_%02d_%02d_Z01_A02",i,j));
      //cout << cut[k][1]->GetName() << endl;
      cut[k][2]=(TCutG*)g->Get(Form("DECal_ERaw_%02d_%02d_Z01_A03",i,j));
      //cout << cut[k][2]->GetName() << endl;
    }
  }

  printf("Histograms loaded\n");

  //////////////////////////////////////////////////////////////////////////////
  //// use Fermi function to fit
  TF1 *fermi[3];
  for(int i=0; i<3; i++) fermi[i]= new TF1(Form("fermi_z1a%d",i+1),"[3]+[0]*1/(1+TMath::Exp((x-[1])/[2]))",500,2000);

  TH1D *hpx[3];
  float limiti[3][2];
  for(int i=0; i<3; i++) 	for(int j=0; j<2; j++) limiti[i][j]=0;

  //////////////////////////////////////////////////////////////////////////////
  /// creating two canvases
  TCanvas *c[4];
  for(int i=0; i<2; i++)
  {
    c[i] = new TCanvas (Form("c%d",i),Form("c%d",i));
  }

  int nextel, nextelmax;
  int nextcrystal, nextcrystalmax;
  if(mode==0)
  {
    nextel=0;
    nextelmax=12;     // the number of silicon equal 12
    nextcrystal=0;
    nextcrystalmax=4; // the number of csi equal 4
  }
  else
  {
    nextel=itel;
    nextelmax=itel+1;
    nextcrystal=icsi;
    nextcrystalmax=icsi+1;
  }

  //////////////////////////////////////////////////////////////////////////////
  //// begin analysis here
  for(int i=nextel; i<nextelmax; i++)
  {
    printf("Analyzing telescope %02d\n",i);
    for(int j=nextcrystal, k=i*4+j; j<nextcrystalmax; j++, k++)
    {
      printf("Analyzing crystal %02d\n",j);
      int pippo=2;

      int z=1;
      c[0]->cd();
      gPad->SetLogz();

      if(DEEHist[k]==0)
      {
        printf("No data present for tel %d strip %d\n", i, j);
	      continue;
      }

      DEEHist[k]->GetXaxis()->SetRangeUser(0,2500);
      DEEHist[k]->GetYaxis()->SetRangeUser(0,6);
      DEEHist[k]->Draw("colz");
      outroot->WriteTObject(DEEHist[k], DEEHist[k]->GetName());
      outroot->Add(DEEHist[k]);
      for(int q=0; q<3; q++)
      {
        cut[k][q]->SetMarkerStyle(20);
        cut[k][q]->Draw("PLsame");
	      outroot->Add(cut[k][q]);
      }
      gPad->Modified();
      gPad->Update();

      int temp;
      for(int q=0; q<1; q++)
      {                                      //TEMPORARLY JUST PROTONS
        if(q==temp+1&& pippo==1) pippo=2;
        while (pippo==2)
        {
	        temp=q;
          c[1]->cd();
          gPad->Modified();
          gPad->Update();
          //c[q+1]->Draw();
          if(q==0)hpx[q] = DEEHist[k]->ProjectionX(Form("DECal_ERaw_%02d_%02d_proton",i,j),1,-1,Form("[%s]",  cut[k][q]->GetName()));
          if(q==1)hpx[q] = DEEHist[k]->ProjectionX(Form("DECal_ERaw_%02d_%02d_deuteron",i,j),1,-1,Form("[%s]",cut[k][q]->GetName()));
          if(q==2)hpx[q] = DEEHist[k]->ProjectionX(Form("DECal_ERaw_%02d_%02d_triton",i,j),1,-1,Form("[%s]",  cut[k][q]->GetName()));
          if(q==0)hpx[q]->GetXaxis()->SetRangeUser(500,1500);
          if(q==1)hpx[q]->GetXaxis()->SetRangeUser(1000,2000);
          if(q==2)hpx[q]->GetXaxis()->SetRangeUser(1300,2500);

         	hpx[q]->SetTitle(cut[k][q]->GetName());
          hpx[q]->SetMarkerStyle(20);
         	hpx[q]->SetMarkerSize(0.7);
	        hpx[q]->SetMarkerColor(kBlue);
	        hpx[q]->SetLineColor(kBlue);
          hpx[q]->Draw("PE");
	        hpx[q]->GetXaxis()->SetTitle("Raw CsI (ch)");
        	hpx[q]->GetXaxis()->CenterTitle(true);
	        hpx[q]->GetXaxis()->SetLabelSize(0.05);
	        hpx[q]->GetXaxis()->SetTitleSize(0.05);
	        hpx[q]->GetYaxis()->SetLabelSize(0.05);
	        hpx[q]->GetYaxis()->SetTitleSize(0.05);
	        gStyle->SetOptStat(0);

          ica=0;
          np=0;
          //////////////////////////////////////////////////////////////////////
          /// it is very important!!!! very convenient!!!
          /// here to set the range and fit on the GUI by hand
          c[1]->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",0,0,"puntix(Int_t,Int_t,Int_t,TObject*)");//non chiarissimo come funziona

          while(ica!=1)
          {
            usleep(100);
            gClient->HandleInput();
          }
          c[1]->Disconnect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)");

          if(ica==1)
          {
            for(int ip=0; ip<np; ip++)
            {
              limiti[q][ip]=m[ip]->GetX();
            }
          }
          printf("%f %f \n", limiti[q][0],limiti[q][1]);

          fermi[q]->SetParameters(hpx[q]->Integral(),hpx[q]->GetMean(),hpx[q]->GetStdDev(),50);
          hpx[q]->Fit(fermi[q],"","same",limiti[q][0],limiti[q][1]);
          if(q==0){fermi[q]->SetNameTitle(Form("DECal_ERaw_%02d_%02d_Z01_A01",i,j),Form("DECal_ERaw_%02d_%02d_proton",i,j));}
	        if(q==1){fermi[q]->SetNameTitle(Form("DECal_ERaw_%02d_%02d_Z01_A02",i,j),Form("DECal_ERaw_%02d_%02d_deuteron",i,j));}
	        if(q==2){fermi[q]->SetNameTitle(Form("DECal_ERaw_%02d_%02d_Z01_A03",i,j),Form("DECal_ERaw_%02d_%02d_triton",i,j));}
	        //outroot->Add(fermi[q]);

          gPad->Modified();
          gPad->Update();

	        cout << "Options? Save and next =1 , Retry the fit=2" << endl;
	        cin >> pippo;
          if(pippo==1)
          {
	          fprintf(results,"%02d %02d %d %d %15f %15f %15f %15f %15f %15f %15f\n",
	          i,j,z,q+1,fermi[q]->GetParameter(1),fermi[q]->GetParError(1),fermi[q]->GetParameter(2),
            fermi[q]->GetParError(2),fermi[q]->GetChisquare()/fermi[q]->GetNDF(),limiti[q][0],limiti[q][1]);

	          c[1]->Print(Form("pictures/Fit_CsIPunchThrough_%02d_%02d_Z%02d_A%02d.pdf",i,j,z,q+1));
	          outroot->WriteTObject(cut[k][q], cut[k][q]->GetName());
            outroot->WriteTObject(hpx[q], hpx[q]->GetName());
	        }
	      }//close while
      }
    }//close csi crystal
  }//close silicion tele

  fclose(results);
  cout << "File .txt closed" << endl;
  outroot->Close();
  cout << "File .root closed" << endl;
} // end void TogetPunchThroughPoints()



////////////////////////////////////////////////////////////////////////////////
//// the macro to set the fit range by hand on GUI
///  Click the Central button of the mouse to get the range
void puntix(Int_t event,Int_t x,Int_t y ,TObject *selected)
{
  if(event==2)
  {
    //cout << "Dentro tasto centrale" << endl;
    float px= gPad->AbsPixeltoX(x);
    float py= gPad->AbsPixeltoX(y);
    py=gPad->PadtoY(py);
    float uymin=gPad->GetUymin();
    float uymax=gPad->GetUymax();
    if(px>=gPad->GetUxmin() && px<=gPad->GetUxmax())
    {
      //&& py>=gPad->PadtoY(uymin) && py<=gPad->PadtoY(uymax)){
      m[np]=new TMarker(px,py,3);
      l[np]=new TLine (px,uymin,px,uymax);
      l[np]->SetLineColor(2);
      l[np]->SetLineWidth(2);
      l[np]->Draw();
      for(int j=0; j<np; j++)
      {
        l[j]->Draw();
      }
      gPad->Modified();
      gPad->Update();
      printf("x=%f\n", px);
      np++;

      //////////////////////////////////////////////////////////////////////////
      ///  Modified by Daniele 03/12/2018
      // I get only 2 points and then I return
      if(np==2)
      {
        ica=1;
        return;
      }
      cout << "Click next point" << endl;
    }
  }

  if(event==24&&gPad->GetEventX()==99)
  {
    //c per uscire
    ica=1;
    return;
  }
  return;
}
