#include "../include/CSHINECsIEnergyCali.h"
using namespace std;

//______________________________________________________________________________
CSHINECsIEnergyCali::CSHINECsIEnergyCali(Int_t firstrun, Int_t lastrun)
{
	fFirstRun = firstrun;
  fLastRun  = lastrun;
	fDeefit   = new CSHINEDEEFITPID(fFirstRun, fLastRun);
}

//_________________________________________
CSHINECsIEnergyCali::~CSHINECsIEnergyCali()
{}
//______________________________________________________________________________


//______________________________________________________________________________
//                CsI 能量刻度分为两部分: 取点 与 拟合
//         目前，拟合有两种可用: 1. DEEFIT 函数取点   2.投影取点
//       ---------------------------------------------------
//
//                      1. DEEFIT 函数取点
//                    ----------------------
// 根据 DEEFIT 拟合参数, 计算 CsI 中的能量点 (ECh, EMeV)
//   1.1 ECh: 手动选取.
//      a. 需要先手动选出每条 PID 带子ECh的范围，比如: 对于质子 p, 100< ECh < 600
//         这一步可以用 excel 完成, 然后将数据导出
//      b. 根据选定的范围, 对每个 ECh bin循环, 进行下一步计算
//
//               DEEFIT_Func14                 LISE++
//   1.2 EMeV: ECh ---------------> (ECh, dESi) ----------> (ECh, EMeV)
//
void CSHINECsIEnergyCali::GetDEEFITCsIEnergyPoints()
{
	Int_t NPoints = 30; // 每种粒子提取 NPoints 个点
	Double_t ELoss = -999.;
	Double_t ECsI_Calc;

	std::string pathDEEFITPars(Form("%sDEEFITData/Fitparam_table.out",PATHROOTFILESFOLDER));
	std::string pathECsIRange(Form("%sdata_CsIEnergyCali/ECsIRange.dat",PATHDATAFOLDER));

	ifstream FileIn(pathECsIRange.c_str());

	// 读取 DEEFIT 拟合的 14 参数
 	Double_t** DEEFITPars = fDeefit->DEEFITLoadPars(pathDEEFITPars.c_str());

	// 定义变量存储 ECsI_Ch, ECsI_MeV
	std::vector<Double_t> ECsI_Ch[NUM_SSD*NUM_CSI][10][20]; //[numtel][Z][A]
	std::vector<Double_t> ECsI_MeV[NUM_SSD*NUM_CSI][10][20]; //[numtel][Z][A]

	// 读取ECsIRange.dat 文件
	std::vector<Int_t> Charge[NUM_SSD*NUM_CSI];
	std::vector<Int_t> Mass[NUM_SSD*NUM_CSI];
	std::vector<Double_t> RangeMin[NUM_SSD*NUM_CSI];
	std::vector<Double_t> RangeMax[NUM_SSD*NUM_CSI];
	if (!FileIn.is_open()) {
		printf("Error: file %s not found\n",pathECsIRange.c_str());
    return NULL;
	}
	while (FileIn.good()) {
		// 按行读取数据
    std::string LineRead;
    std::getline(FileIn, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

		Int_t numtel;
		Int_t charge, mass;
		Double_t rangemin, rangemax;
		LineStream>>numtel>>charge>>mass>>rangemin>>rangemax;

	  Charge[numtel].push_back(charge);
		Mass[numtel].push_back(mass);
		RangeMin[numtel].push_back(rangemin);
		RangeMax[numtel].push_back(rangemax);
	}


  // 对每一块 CsI、每一种粒子, 计算能量点 (ECsI, EMeV)
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		for (Int_t ip=0; ip<Charge[numtel].size(); ip++) {
			if (Charge[numtel][ip]!=1 || Mass[numtel][ip]!=1) continue;
			Int_t ssdindex = numtel/NUM_CSI;
			Int_t csiindex = numtel%NUM_CSI;
			if (ssdindex==3 && (csiindex==4 || csiindex==5)) continue; // SSD4_CsI[4],CsI[5] 不能用
			Int_t ChStep = (Int_t) (RangeMax[numtel][ip]-RangeMin[numtel][ip])/NPoints;
      fDEEFITParticle.Z = Charge[numtel][ip];
			fDEEFITParticle.A = Mass[numtel][ip];

      for (Double_t ech=RangeMin[numtel][ip]; ech<RangeMax[numtel][ip]; ech+=ChStep) {
				fDEEFITParticle.E = ech;
        ELoss = fDeefit->DEEFITFunc14(fDEEFITParticle, DEEFITPars[numtel]);
        ECsI_Calc = EnergyDepositedInCsI(Charge[numtel][ip],Mass[numtel][ip],ELoss,SiThickness[ssdindex],CsIMylarThickness);

		    ECsI_Ch[numtel][Charge[numtel][ip]][Mass[numtel][ip]].push_back(ech);
				ECsI_MeV[numtel][Charge[numtel][ip]][Mass[numtel][ip]].push_back(ECsI_Calc);

				cout<<numtel<<setw(15)<<Charge[numtel][ip]<<setw(15)<<Mass[numtel][ip]<<setw(15)<<ELoss<<setw(20)<<ECsI_Calc<<endl;
			}
		}
	}

}
//___________________________________________________________
Double_t CSHINECsIEnergyCali::EnergyDepositedInCsI(Int_t Z, Int_t A, Double_t ELoss,
	Double_t SiThickness, Double_t CsIMylarThickness)
{
	Double_t EResidual = fLISEModule.GetResidualEnergy(Z,A,ELoss,"Si",SiThickness,1/*LiseModel*/);
	Double_t EResidualInCsI = EResidual - fLISEModule.GetEnergyLoss(Z,A,EResidual,"Mylar",CsIMylarThickness,1);
  return EResidualInCsI;
}

//______________________________________________________________________________
//                          2. 投影取点
//                      -----------------
// 投影取点, 包括两步: 1.对所有的同位素带作 cut
void CSHINECsIEnergyCali::DoGraphicalCut()
{
	std::string pathROOTInput(Form("%sDEEPlot/L2L3DEE.root",PATHROOTFILESFOLDER));
	std::string pathROOTCutOut(Form("%sDEEPlot/L2L3DEECuts.root",PATHROOTFILESFOLDER));
	std::string DEEHistName("DEEL2L3");

	TFile* FileIn  = new TFile(pathROOTInput.c_str(), "READONLY");
	TFile* FileOut = new TFile(pathROOTCutOut.c_str(), "RECREATE");

	TIter next(FileIn->GetListOfKeys());
	TKey* key;

	while ((key = (TKey*)next()))
	{
		TClass* cl = gROOT->GetClass(key->GetClassName());
		if (!cl->InheritsFrom("TH2D")) continue;
		TH2D *hist = (TH2D*)key->ReadObj();

		std::string HistName(hist->GetName());
		if(HistName.find(DEEHistName.c_str())==std::string::npos) continue;

		for(int i=0; i<13; i++) {
		  std::string CutName;
		  // 手动添加或删减粒子种类
		  if(i==0 && FileOut->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),1,1))==0) {
			  printf("Please Draw cut on protons\n");
			  CutName.assign(HistName+Form("_Z%02d_A%02d",1,1));
		  }
		  if(i==1 && FileOut->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),1,2))==0) {
			  printf("Please Draw cut on deuterons\n");
			  CutName.assign(HistName+Form("_Z%02d_A%02d",1,2));
		  }
		  if(i==2 && FileOut->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),1,3))==0) {
			  printf("Please Draw cut on tritons\n");
			  CutName.assign(HistName+Form("_Z%02d_A%02d",1,3));
		  }
		  if(i==3 && FileOut->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),2,3))==0) {
			  printf("Please Draw cut on 3He\n");
			  CutName.assign(HistName+Form("_Z%02d_A%02d",2,3));
		  }
	  	if(i==4 && FileOut->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),2,4))==0) {
			  printf("Please Draw cut on 4He\n");
			  CutName.assign(HistName+Form("_Z%02d_A%02d",2,4));
		  }
			if(i==5 && FileOut->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),2,6))==0) {
			  printf("Please Draw cut on 6He\n");
			  CutName.assign(HistName+Form("_Z%02d_A%02d",2,6));
		  }
		  if(i==6 && FileOut->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),3,6))==0) {
			  continue;
			  printf("Please Draw cut on 6Li\n");
			  CutName.assign(HistName+Form("_Z%02d_A%02d",3,6));
		  }
		  if(i==7 && FileOut->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),3,7))==0) {
			  continue;
			  printf("Please Draw cut on 7Li\n");
			  CutName.assign(HistName+Form("_Z%02d_A%02d",3,7));
		  }
			if(i==8 && FileOut->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),3,8))==0) {
			  continue;
			  printf("Please Draw cut on 8Li\n");
			  CutName.assign(HistName+Form("_Z%02d_A%02d",3,8));
		  }
			if(i==9 && FileOut->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),3,9))==0) {
			  continue;
			  printf("Please Draw cut on 9Li\n");
			  CutName.assign(HistName+Form("_Z%02d_A%02d",3,9));
		  }
		  if(i==10 && FileOut->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),4,7))==0) {
			  continue;
			  printf("Please Draw cut on 7Be\n");
			  CutName.assign(HistName+Form("_Z%02d_A%02d",4,7));
		  }
			if(i==11 && FileOut->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),4,9))==0) {
			  continue;
			  printf("Please Draw cut on 9Be\n");
			  CutName.assign(HistName+Form("_Z%02d_A%02d",4,9));
		  }
			if(i==12 && FileOut->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),4,10))==0) {
			  continue;
			  printf("Please Draw cut on 10Be\n");
			  CutName.assign(HistName+Form("_Z%02d_A%02d",4,10));
		  }
			if(i==13 && FileOut->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),5,10))==0) {
			  continue;
			  printf("Please Draw cut on 10B\n");
			  CutName.assign(HistName+Form("_Z%02d_A%02d",5,10));
		  }
			if(i==14 && FileOut->Get(Form("%s_Z%02d_A%02d",HistName.c_str(),5,11))==0) {
			  continue;
			  printf("Please Draw cut on 11B\n");
			  CutName.assign(HistName+Form("_Z%02d_A%02d",5,11));
		  }

		  if(FileOut->Get(CutName.c_str()) || CutName.empty()) continue; //cut already existing
		  hist->Draw("colz");
		  gPad->SetLogz(kTRUE);

      // 这一行命令用于调用 “剪刀手” z作 cut
		  TCutG* mycut = (TCutG*)gPad->WaitPrimitive("CUTG");
		  mycut->SetName(CutName.c_str());

		  printf("%s cut created\n", CutName.c_str());
		  printf("S - save the cut\nR - retry\nF - skip this cut\nD - delete a cut\nE - exit\n");
		  char input;
		  input=getchar();
		  getchar();
		  printf("you typed %c\n", input);
		  if (input=='S' || input=='s') {
			  FileOut->WriteTObject(mycut, mycut->GetName()); // 保存 cut
		  	//FileOut->WriteTObject(hist, hist->GetName());
			  printf ("%s cut saved to file\n", CutName.c_str());
		  }
		  else if (input == 'R' || input =='r') { i--; }
		  else if (input == 'F' || input == 'f') {}
		  else if (input == 'D' || input == 'd') {
			  printf("Please type the cut to delete: \n");
			  std::string CutToDelete;
			  std::cin>>CutToDelete;
			  CutToDelete.append(";*");
		  	FileOut->Delete(CutToDelete.c_str());
			  printf("deleted %s cut\n", CutToDelete.c_str());
		  	i--;
			  std::cin.get();
		  }
		  else if (input == 'E' || input == 'e'){
			  printf("Goodbye\n");
			  FileOut->Close();
			  return;
		  }
		  else i--;
		}
	}
	FileOut->Close();
	return;
}

//______________________________________________________________________________
// 使用 "投影法" 取点 (ECh, EMeV)
void CSHINECsIEnergyCali::GetProjectionCsIEnergPoints()
{
	TFile* FileInRoot = new TFile(Form("%sDEEPlot/L2L3DEE.root",PATHROOTFILESFOLDER));
  TFile* FileInCuts = new TFile(Form("%sDEEPlot/L2L3DEECuts.root",PATHROOTFILESFOLDER));

	std::string pathDataOutput(Form("%sdata_CsIEnergyCali/CsIEnergyPoints.dat",PATHDATAFOLDER));
	ofstream FileOut(pathDataOutput.c_str());
	FileOut<<"*SSDNum"<<setw(10)<<"CsINum"<<setw(10)<<setw(15)<<"Z"<<setw(15)<<"A"<<setw(15)
	       <<"CsICh"<<setw(15)<<"CsICh_Err"<<setw(15)<<"ESiMeV"<<setw(15)<<"ESiMeV_Err"<<endl;

  Int_t BinStep = 5;
	Int_t ProjBinWidth = 2;

  Double_t EMeVErrorCut = 1.;
	Double_t CsICh_Err = 0.5; // 手动给 CsICh 添加 0.5 道的误差

	std::vector<Double_t> CsICh;
	std::vector<Double_t> ESi_MeV;

  TCanvas* cans = new TCanvas("cans","cans",800,600);
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
			for (Int_t np=0; np<fNParticles; np++) {
				TH2D* DEEL2L3Hist = (TH2D*) FileInRoot->Get(Form("DEEL2L3_SSD%d_CsI%d",ssdindex+1,csiindex));
				if (DEEL2L3Hist == 0) continue;
				const char* CutName = Form("DEEL2L3_SSD%d_CsI%d_Z%02d_A%02d",ssdindex+1,csiindex,fParticle[np][0],fParticle[np][1]);
				TCutG* ParticleCut=(TCutG*)FileInCuts->Get(CutName);
	      if(ParticleCut==0) continue;

	      Int_t MinBin = DEEL2L3Hist->GetXaxis()->FindBin(ParticleCut->GetXaxis()->GetXmin());
				Int_t MaxBin = DEEL2L3Hist->GetXaxis()->FindBin(ParticleCut->GetXaxis()->GetXmax());

				// 对 DEE plot 向 y 轴作投影
				for (Int_t ibin=MinBin; ibin<MaxBin; ibin+=BinStep) {
          TH1D* ProjectionHist = new TH1D(Form("ProjectionHist%d",ibin), "", 1000,0,100);
					const char* ProjectionCut = Form("[DEEL2L3_SSD%d_CsI%d_Z%02d_A%02d]",ssdindex+1,csiindex,fParticle[np][0],fParticle[np][1]);
					// !!! 重点 !!!
					DEEL2L3Hist->ProjectionY(Form("ProjectionHist%d",ibin),ibin,ibin+ProjBinWidth,ProjectionCut);
					// 跳过统计较低的 bin
					Double_t Ey = ProjectionHist->GetBinCenter(ProjectionHist->GetMaximumBin());
					Double_t Ey_err = ProjectionHist->GetRMS();
					if (Ey_err == 0) continue;
					if (ProjectionHist->Integral(ProjectionHist->FindBin(Ey-Ey_err),ProjectionHist->FindBin(Ey+Ey_err))<50) continue;
					// gaus 拟合
					ProjectionHist->Fit("gaus","0Q", "", Ey-Ey_err, Ey+Ey_err);
      	  TF1* FitFunc = ProjectionHist->GetFunction("gaus");
					Double_t ESi = FitFunc->GetParameter(1);
					Double_t ESi_RMS = FitFunc->GetParError(1);
					if (ESi_RMS > EMeVErrorCut) continue;
					// 提取数据点
					Double_t csi_ech = DEEL2L3Hist->GetXaxis()->GetBinCenter(ibin+ProjBinWidth/2);
					CsICh.push_back(csi_ech);
					ESi_MeV.push_back(Ey);

          Double_t EResidual = fLISEModule.GetResidualEnergy(fParticle[np][0],fParticle[np][1],ESi,"Si",SiThickness[ssdindex],1/*LiseModel*/);
          Double_t EResidualInCsI = EResidual - fLISEModule.GetEnergyLoss(fParticle[np][0],fParticle[np][1],EResidual,"Mylar",CsIMylarThickness,1);
					Double_t CsIE = EResidualInCsI;
          Double_t EResidualMax = fLISEModule.GetResidualEnergy(fParticle[np][0],fParticle[np][1],ESi+ESi_RMS,"Si",SiThickness[ssdindex],1);
          Double_t EResidualInCsIMax = EResidualMax - fLISEModule.GetEnergyLoss(fParticle[np][0],fParticle[np][1],EResidualMax,"Mylar",CsIMylarThickness,1);
          Double_t CsIE_Err = TMath::Abs(EResidualInCsIMax - EResidualInCsI);

					printf("Tel %02d CsI %02d: ESi=%.3f -> ECsI=%.3f\n", ssdindex+1, csiindex, ESi, CsIE);

					FileOut<<ssdindex<<setw(15)<<csiindex<<setw(15)<<fParticle[np][0]<<setw(15)<<fParticle[np][1]<<setw(15)
								 <<csi_ech<<setw(15)<<CsICh_Err<<setw(15)<<ESi<<setw(15)<<ESi_RMS<<endl;
				}

				// 将投影得到的数据点画出来, 检查是否在 cut 内
				DEEL2L3Hist->Draw("COLZ");
        DEEL2L3Hist->GetXaxis()->SetRangeUser(ParticleCut->GetXaxis()->GetXmin(),ParticleCut->GetXaxis()->GetXmax());
		    DEEL2L3Hist->GetYaxis()->SetRangeUser(ParticleCut->GetYaxis()->GetXmin(),ParticleCut->GetYaxis()->GetXmax());
				ParticleCut->SetLineColor(kRed);
				ParticleCut->SetLineWidth(2);
        ParticleCut->Draw("*L SAME");
        TGraph* CheckGraph = new TGraph(CsICh.size(),CsICh.data(),ESi_MeV.data());
        CheckGraph->SetMarkerStyle(20);
				CheckGraph->SetMarkerColor(kBlack);
        CheckGraph->Draw("PE SAME");
        gPad->SetLogz(1);
        gPad->Modified();
        gPad->Update();
      //  getchar();
			}
		}
	}
}
