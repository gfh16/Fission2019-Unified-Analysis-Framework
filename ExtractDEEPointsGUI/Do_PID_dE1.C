#include <TGClient.h>
#include <TCanvas.h>
#include "TChain.h"
#include <TF1.h>
#include "TFile.h"
#include "TH1D.h"
#include "fstream"
#include <iomanip>
#include "TMarker.h"
#include "TH2I.h"
#include "Map_Fission.hh"

Double_t DoCalcdEMeV(Double_t Ex, Double_t* pars, Int_t ParsNum);
Double_t StdPIDNumber(Int_t Z_charge, Int_t A_mass);

//______________________________________________________________________________
void Do_PID_dE1()
{
  gStyle->SetOptStat(kFALSE);

  Int_t Num_of_Particle[100]    = {-99};      // [tel]
	Int_t Z_charge[100][100]      = {{-99}};    // [tel][partilce]
	Int_t A_mass[100][100]        = {{-99}};    // [tel][partilce]
	Double_t fitpars[100][100][8] = {{{-99}}}; // [tel][particle][pars], fit_func = [0]/x + pol6

  // 读取拟合参数
	ifstream in("fit_para_dE1.dat", ios::in);
	if (in) {
		while(!in.eof()) {
			std::string LineRead;
			std::getline(in, LineRead);
			LineRead.assign(LineRead.substr(0, LineRead.find('#')));
			if(LineRead.empty()) continue;
			if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
			std::istringstream LineStream(LineRead);

	    Int_t teleindex, particleindex;
			LineStream>>teleindex>>particleindex;
			LineStream>>Z_charge[teleindex][particleindex]>>A_mass[teleindex][particleindex];

      for (Int_t i=0; i<8; i++) {
				LineStream>>fitpars[teleindex][particleindex][i];
			}
			Num_of_Particle[teleindex]++;
		}
		in.close();
	}

  //______________________________________________
  // 读取 tree 数据
  Map_Fission mapper_exp;
  mapper_exp.ReadMap("../Config/Map_PPAC.data");
  mapper_exp.ReadMap("../Config/Map_Telescope.data");
  mapper_exp.ReadMap("../Config/Map_Scintillator.data");
  mapper_exp.ReadMap("../Config/Map_RF.data");
  mapper_exp.Produce_Global_Index();
  mapper_exp.PrintMap();

  TChain* t1_data  = new TChain("RawData");
  TH2D* h2_PID_dE2 = new TH2D("Tel2_PID_dE2","Tel2_PID_dE2",4000,0,4000,10000,0,10);
  TH2I* h2_dE1_dE2 = new TH2I("h2_dE1_dE2","h2_dE1_dE2",4000,0,4000,4000,0,4000);
  for(Int_t i=0; i<50; i++) {
    Char_t Name_Tem[200];
    sprintf(Name_Tem,"../Data_Root/ArAu0%d.root",400+i);
    t1_data->Add(Name_Tem);
  }
  Int_t ChNum_Total = 0;
  Int_t Ch_Value[500] = {0};
  t1_data->SetBranchAddress("ChNum_Total", &ChNum_Total);
  t1_data->SetBranchAddress("Data", Ch_Value);
  Int_t EvtNum = t1_data->GetEntries();
  cout<<"EvtNum: "<<EvtNum<<endl;

  Int_t Tele_Index = 2;
  Int_t NParticles = Num_of_Particle[Tele_Index];
  char Tele_Name[200];
  sprintf(Tele_Name,"telescope_%d", Tele_Index);
  Int_t dE1_Index  = 0;
  Int_t dE2_Index  = 0;
  Int_t E_Index    = 0;
  Int_t T_Index    = 0;
  mapper_exp.Get_Ch(&dE1_Index, Tele_Name, "Si1_E");
  mapper_exp.Get_Ch(&dE2_Index, Tele_Name, "Si2_E");
  mapper_exp.Get_Ch(&E_Index,   Tele_Name, "CsI_E");
  mapper_exp.Get_Ch(&T_Index,   Tele_Name, "Si2_T");

  //______________________________________________
  // 逐个 事件分析
  for(Int_t ientry=0; ientry<EvtNum; ientry++)
  {
    t1_data->GetEvent(ientry);

    Double_t PID_Num = 0.;
    Double_t Calc_dE1[NParticles];

    if (ientry%100000==0) { cout<<"Evt:"<<ientry<<endl; }
    if (Ch_Value[dE2_Index]<20 || Ch_Value[dE2_Index]>4000 || Ch_Value[dE1_Index]<40) { continue; }
    if (Ch_Value[T_Index] == 0) { continue; }
    if (Ch_Value[E_Index] < 10)
    {
      for (Int_t np=0; np<NParticles; np++) {
         Calc_dE1[np] = DoCalcdEMeV(Ch_Value[dE2_Index], fitpars[Tele_Index][np], 8);
      }

      // 在Ch_Value[dE2_Index]>900时, Z=7 的粒子超出量程了
      if(Ch_Value[dE2_Index]>900)
      {
        if (Ch_Value[dE1_Index]<=0) PID_Num = 0.;
        // 分析质子线以下的实验点
        if (0<Ch_Value[dE1_Index] && Ch_Value[dE1_Index]<=Calc_dE1[0]) {
          PID_Num = 0. + (StdPIDNumber(Z_charge[Tele_Index][0],A_mass[Tele_Index][0]) - 0.)*(Ch_Value[dE1_Index] - 0.)/(Calc_dE1[0] - 0.);
        }
        // 分析质子线以上，最大 PID 线以下的实验点
        if (Calc_dE1[0]<Ch_Value[dE1_Index] && Ch_Value[dE1_Index]<=Calc_dE1[NParticles-1]) {
          for (Int_t np=1; np<NParticles; np++) {
            if (Calc_dE1[np-1]<Ch_Value[dE1_Index] && Ch_Value[dE1_Index]<=Calc_dE1[np]) {
              PID_Num = StdPIDNumber(Z_charge[Tele_Index][np-1],A_mass[Tele_Index][np-1])
                      + (StdPIDNumber(Z_charge[Tele_Index][np],A_mass[Tele_Index][np]) - StdPIDNumber(Z_charge[Tele_Index][np-1],A_mass[Tele_Index][np-1]))
                      * (Ch_Value[dE1_Index] - Calc_dE1[np-1])/(Calc_dE1[np] - Calc_dE1[np-1]);
            }
          }
        }
        // 分析超出最高 PID 线的实验点
        if (Ch_Value[dE1_Index]>Calc_dE1[NParticles-1]) {
          PID_Num = StdPIDNumber(Z_charge[Tele_Index][NParticles-1],A_mass[Tele_Index][NParticles-1])
                  * Ch_Value[dE1_Index]/Calc_dE1[NParticles-1];
        }
      }
      // 在Ch_Value[dE2_Index]>900时, Z=7 的粒子超出量程了
      if (Ch_Value[dE2_Index]<900)
      {
        if (Ch_Value[dE1_Index]<=0) PID_Num = 0.;
        // 分析质子线以下的实验点
        if (0<Ch_Value[dE1_Index] && Ch_Value[dE1_Index]<=Calc_dE1[0]) {
          PID_Num = 0. + (StdPIDNumber(Z_charge[Tele_Index][0],A_mass[Tele_Index][0]) - 0.)*(Ch_Value[dE1_Index] - 0.)/(Calc_dE1[0] - 0.);
        }
        // 分析质子线以上，最大 PID 线以下的实验点
        if (Calc_dE1[0]<Ch_Value[dE1_Index] && Ch_Value[dE1_Index]<=Calc_dE1[NParticles-2]) {
          for (Int_t np=1; np<NParticles; np++) {
            if (Calc_dE1[np-1]<Ch_Value[dE1_Index] && Ch_Value[dE1_Index]<=Calc_dE1[np]) {
              PID_Num = StdPIDNumber(Z_charge[Tele_Index][np-1],A_mass[Tele_Index][np-1])
                      + (StdPIDNumber(Z_charge[Tele_Index][np],A_mass[Tele_Index][np]) - StdPIDNumber(Z_charge[Tele_Index][np-1],A_mass[Tele_Index][np-1]))
                      * (Ch_Value[dE1_Index] - Calc_dE1[np-1])/(Calc_dE1[np] - Calc_dE1[np-1]);
            }
          }
        }
        // 分析超出最高 PID 线的实验点
        if (Ch_Value[dE1_Index]>Calc_dE1[NParticles-2]) {
          PID_Num = StdPIDNumber(Z_charge[Tele_Index][NParticles-2],A_mass[Tele_Index][NParticles-2])
                  * Ch_Value[dE1_Index]/Calc_dE1[NParticles-2];
        }
      }
      //cout<<PID_Num<<endl;
      h2_PID_dE2->Fill(Ch_Value[dE2_Index], PID_Num);
      h2_dE1_dE2->Fill(Ch_Value[dE2_Index], Ch_Value[dE1_Index]);
    }
  }

  TCanvas* c1_Single_Spec = new TCanvas("c1_Single_Spec","c1_Single_Spec",1200,1200);
  c1_Single_Spec->Divide(2,2);

  c1_Single_Spec->cd(1);
  gPad->SetLeftMargin(0.16);
  h2_dE1_dE2->Draw("COL");
  h2_dE1_dE2->SetTitle(Form("Tel%d_dE1_vs_dE2", Tele_Index));
  h2_dE1_dE2->GetXaxis()->SetTitle("dE2 (ADC Ch.)");
  h2_dE1_dE2->GetXaxis()->CenterTitle(kTRUE);
  h2_dE1_dE2->GetXaxis()->SetTitleSize(0.05);
  h2_dE1_dE2->GetXaxis()->SetTitleOffset(1);
  h2_dE1_dE2->GetXaxis()->SetLabelSize(0.05);
  h2_dE1_dE2->GetXaxis()->SetNdivisions(1010);
  h2_dE1_dE2->GetYaxis()->SetTitle("dE1 (ADC Ch.)");
  h2_dE1_dE2->GetYaxis()->CenterTitle(kTRUE);
  h2_dE1_dE2->GetYaxis()->SetTitleSize(0.05);
  h2_dE1_dE2->GetYaxis()->SetTitleOffset(1.65);
  h2_dE1_dE2->GetYaxis()->SetLabelSize(0.05);
  h2_dE1_dE2->GetYaxis()->SetNdivisions(1006);

  c1_Single_Spec->cd(3);
  gPad->SetLeftMargin(0.16);
  h2_PID_dE2->Draw("COL");
  h2_PID_dE2->SetTitle(Form("Tel%d_PID_vs_dE2", Tele_Index));
  h2_PID_dE2->GetXaxis()->SetTitle("dE2 (ADC Ch.)");
  h2_PID_dE2->GetXaxis()->CenterTitle(kTRUE);
  h2_PID_dE2->GetXaxis()->SetTitleSize(0.05);
  h2_PID_dE2->GetXaxis()->SetTitleOffset(1);
  h2_PID_dE2->GetXaxis()->SetLabelSize(0.05);
  h2_PID_dE2->GetXaxis()->SetNdivisions(1010);
  h2_PID_dE2->GetYaxis()->SetTitle("PID");
  h2_PID_dE2->GetYaxis()->CenterTitle(kTRUE);
  h2_PID_dE2->GetYaxis()->SetTitleSize(0.05);
  h2_PID_dE2->GetYaxis()->SetTitleOffset(0.9);
  h2_PID_dE2->GetYaxis()->SetLabelSize(0.05);
  h2_PID_dE2->GetYaxis()->SetNdivisions(510);

  TLatex* latex = new TLatex(300.,8.5, "PID = Z + 0.2(A - 2Z)");
  latex->SetTextColor(kRed);
  latex->SetTextSize(0.06);
  latex->Draw("SAME");


  c1_Single_Spec->cd(4);
  gPad->SetLeftMargin(0.15);
  TH1D* hist_PID = (TH1D*) h2_PID_dE2->ProjectionY();
  hist_PID->Draw();
  hist_PID->SetTitle("");
  hist_PID->GetYaxis()->SetTitle("Counts");
  hist_PID->GetYaxis()->CenterTitle(kTRUE);
  hist_PID->GetYaxis()->SetTitleSize(0.05);
  hist_PID->GetYaxis()->SetLabelSize(0.05);
  hist_PID->GetYaxis()->SetTitleOffset(1.2);
  hist_PID->GetYaxis()->SetNdivisions(510);

  TPad* subpad = new TPad("subpad", "", 0.37, 0.2, 0.95, 0.95);
  subpad->Draw();
  subpad->cd();
  gPad->SetFillStyle(0);
  TH1D* hist_PID_Zone = (TH1D*) hist_PID->DrawClone();
  hist_PID_Zone->Draw();
  hist_PID_Zone->GetXaxis()->SetRangeUser(2.5, 8.5);
  hist_PID_Zone->GetXaxis()->SetTitle("");
  hist_PID_Zone->GetXaxis()->SetLabelSize(0.07);
  hist_PID_Zone->GetYaxis()->SetTitle("");
  hist_PID_Zone->GetYaxis()->SetLabelSize(0.07);

  c1_Single_Spec->Print("Tele2_dE1_E2_PID.png");
}
//______________________________________________________________________________

//______________________________________________________________________________
// fit_func = [0]/x + pol6
//          = [0]/x+[1]+[2]*x+[3]*x*x+[4]*x*x*x+[5]*x*x*x*x+[6]*x*x*x*x*x+[7]*x*x*x*x*x*x
Double_t DoCalcdEMeV(Double_t Ex, Double_t* pars, Int_t ParsNum)
{
	Double_t calc_dE = 0.;
	if (Ex == 0.) {
		cout<<"Invalid Ex!"<<endl;
		return -99.;
	}
	for (Int_t i=0; i<ParsNum; i++) {
    calc_dE += pars[i]*pow(Ex, i-1); //
  }
	return calc_dE;
}

//______________________________________________________________________________
// 定义标准 PID数： (Double_t) PID = Z + 0.2*(A-2*Z)
// 详见参考文献: L.Tassan-Got, Nucl.Instr.and Meth.B 194(2002)503.
Double_t StdPIDNumber(Int_t Z_charge, Int_t A_mass)
{
	return Z_charge + 0.2*(A_mass - 2*Z_charge);
}
