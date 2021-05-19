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

	std::string pathDEEFITPars_All(Form("%sDEEFITData/Fitparam_table_All.out",PATHROOTFILESFOLDER)); // 所有同位素拟合结果
	std::string pathDEEFITPars_ZOver2(Form("%sDEEFITData/Fitparam_table_ZOver2.out",PATHROOTFILESFOLDER)); // Z>1 同位素拟合结果
	std::string pathECsIRange(Form("%sdata_CsIEnergyCali/ECsIRange.dat",PATHDATAFOLDER));
	std::string pathDataOutput(Form("%sdata_CsIEnergyCali/CsIEnergyPoints.dat",PATHDATAFOLDER));

	ofstream FileOut(pathDataOutput.c_str());
	FileOut<<"* numtel = ssdindex * 9 + csiindex"<<endl;
	FileOut<<"* numtel"<<setw(7)<<"Z"<<setw(10)<<"A"<<setw(17)<<"ECsI_MeV"<<setw(15)<<"ECsI_Ch"<<endl;

	ifstream FileIn(pathECsIRange.c_str());

	// 读取 DEEFIT 拟合的 14 参数
 	Double_t** DEEFITPars_All = fDeefit->DEEFITLoadPars(pathDEEFITPars_All.c_str());
	Double_t** DEEFITPars_ZOver2 = fDeefit->DEEFITLoadPars(pathDEEFITPars_ZOver2.c_str());

	// 读取ECsIRange.dat 文件
	std::vector<Int_t> Charge[NUM_SSD*NUM_CSI];
	std::vector<Int_t> Mass[NUM_SSD*NUM_CSI];
	std::vector<Double_t> RangeMin[NUM_SSD*NUM_CSI];
	std::vector<Double_t> RangeMax[NUM_SSD*NUM_CSI];
	if (!FileIn.is_open()) {
		printf("Error: file %s not found\n",pathECsIRange.c_str());
    return;
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
	FileIn.close();

  // 对每一块 CsI、每一种粒子, 计算能量点 (ECsI, EMeV)
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		for (Int_t ip=0; ip<Charge[numtel].size(); ip++) {
			//if (Charge[numtel][ip]!=1) continue;
			Int_t ssdindex = numtel/NUM_CSI;
			Int_t csiindex = numtel%NUM_CSI;
			if (ssdindex==3 && (csiindex==4 || csiindex==5)) continue; // SSD4_CsI[4],CsI[5] 不能用
			Int_t ChStep = (Int_t) (RangeMax[numtel][ip]-RangeMin[numtel][ip])/NPoints;
      fDEEFITParticle.Z = Charge[numtel][ip];
			fDEEFITParticle.A = Mass[numtel][ip];

      for (Double_t ech=RangeMin[numtel][ip]; ech<RangeMax[numtel][ip]; ech+=ChStep) {
				fDEEFITParticle.E = ech;
				if (ssdindex<3 && Charge[numtel][ip]>1) ELoss = fDeefit->DEEFITFunc14(fDEEFITParticle, DEEFITPars_ZOver2[numtel]);
        else ELoss = fDeefit->DEEFITFunc14(fDEEFITParticle, DEEFITPars_All[numtel]);

        ECsI_Calc = EnergyDepositedInCsI(Charge[numtel][ip],Mass[numtel][ip],ELoss,SiThickness[ssdindex],CsIMylarThickness);

        FileOut<<setw(5)<<numtel<<setw(10)<<Charge[numtel][ip]<<setw(10)<<Mass[numtel][ip]<<setw(15)<<ECsI_Calc<<setw(15)<<ech<<endl;
				cout<<numtel<<setw(15)<<Charge[numtel][ip]<<setw(15)<<Mass[numtel][ip]<<setw(15)<<ELoss<<setw(15)<<ech<<setw(15)<<ECsI_Calc<<endl;
			}
		}
	}
	FileOut.close();
}


//______________________________________________________________________________
Double_t DoCalcdEMeV(Double_t* x, Double_t* pars)
{
	Double_t calc_dE = 0.;
	if (x[0] == 0.) {
		cout<<"Invalid Ex!"<<endl;
		return -99.;
	}
	for (Int_t i=0; i<8; i++) {
    calc_dE += pars[i]*pow(x[0], i-1); //
  }
	return calc_dE;
}
void CSHINECsIEnergyCali::GetStraighteningCsIEnergyPoints()
{
	Double_t Erange[4] = {320,280,150,100};
	Int_t NPoints = 30; // 每种粒子提取 NPoints 个点
	Double_t ELoss = -999.;
	Double_t ECsI_Calc;
	Int_t Xmin, Xmax;
	Int_t NPARS = 8;

	std::string pathDataOutput(Form("%sdata_CsIEnergyCali/StraighteningCsIEnergyPoints.dat",PATHDATAFOLDER));
	std::string pathStraighteningFitPars(Form("%sdata_PID/L2L3_StraighteningFitPars.dat",PATHDATAFOLDER));
	std::string pathDEECutsInput(Form("%sStraighteningData/L2L3DEECuts.root",PATHROOTFILESFOLDER));
	std::string pathECsIRange(Form("%sdata_CsIEnergyCali/ECsIRange.dat",PATHDATAFOLDER));

	ofstream FileOut(pathDataOutput.c_str());
	FileOut<<"* numtel = ssdindex * 9 + csiindex"<<endl;
	FileOut<<"* numtel"<<setw(7)<<"Z"<<setw(10)<<"A"<<setw(17)<<"ECsI_MeV"<<setw(15)<<"ECsI_Ch"<<endl;

	std::vector< vector<Int_t> > iCharge;
	std::vector< vector<Int_t> > iMass;
	std::vector< vector< vector<Double_t> > > FitPars;

	fStraightening.ReadStraighteningFitPars(NUM_SSD*NUM_CSI, pathStraighteningFitPars.c_str(), iCharge, iMass, FitPars);

	TFile* CutFile = new TFile(pathDEECutsInput.c_str(),"READONLY");
	TCutG* IsotopeCut[NUM_SSD*NUM_CSI][50];

	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		for (Int_t ip=0; ip<iCharge[numtel].size(); ip++) {
			IsotopeCut[numtel][ip] = (TCutG*) CutFile->Get(Form("h2_dE2_ECsI_Tel%02d_Z%02d_A%02d",numtel,iCharge[numtel][ip],iMass[numtel][ip]));
      if (IsotopeCut[numtel][ip]==0) continue;
			//cout<<IsotopeCut[ssdindex][ip]->GetName()<<endl;
		}
	}

  // 对每一块 CsI、每一种粒子, 计算能量点 (ECsI, EMeV)
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		for (Int_t ip=0; ip<iCharge[numtel].size(); ip++) {
			//if (Charge[numtel][ip]!=1) continue;
			Int_t ssdindex = numtel/NUM_CSI;
			Int_t csiindex = numtel%NUM_CSI;
			if (ssdindex==3 && (csiindex==4 || csiindex==5)) continue; // SSD4_CsI[4],CsI[5] 不能用
			Xmin = 200;
			Xmax = (Int_t) IsotopeCut[numtel][ip]->GetXaxis()->GetXmax()-100;
			Int_t ChStep = (Int_t) (Xmax-Xmin)/NPoints;

      for (Double_t ech=Xmin; ech<Xmax; ech+=ChStep) {
				ELoss = fStraightening.DoCalcdEMeV(ech, FitPars[numtel][ip].data(), NPARS);
        ECsI_Calc = EnergyDepositedInCsI(iCharge[numtel][ip],iMass[numtel][ip],ELoss,SiThickness[ssdindex],CsIMylarThickness);
        if (ECsI_Calc>0) {
					FileOut<<setw(5)<<numtel<<setw(10)<<iCharge[numtel][ip]<<setw(10)<<iMass[numtel][ip]<<setw(15)<<ECsI_Calc<<setw(15)<<ech<<endl;
					cout<<numtel<<setw(15)<<iCharge[numtel][ip]<<setw(15)<<iMass[numtel][ip]<<setw(15)<<ELoss<<setw(15)<<ech<<setw(15)<<ECsI_Calc<<endl;
				}
			}
		}
	}
	FileOut.close();
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
	Int_t MAXPOINTS = 50;
	Int_t charge, mass;

	std::string pathROOTInput(Form("%sDEEPlot/L2L3DEE.root",PATHROOTFILESFOLDER));
	std::string pathROOTCutOut(Form("%sDEEPlot/L2L3DEECuts.root",PATHROOTFILESFOLDER));
	std::string DEEHistName("DEEL2L3");

	TCanvas* cans = new TCanvas("cans","cans",800,600);

	TFile* FileIn  = new TFile(pathROOTInput.c_str(), "READONLY");
	TFile* FileOut = new TFile(pathROOTCutOut.c_str(), "RECREATE");

	TIter next(FileIn->GetListOfKeys());
	TKey* key;

	while ((key = (TKey*)next()))
	{
		TClass* cl = gROOT->GetClass(key->GetClassName());
		if (!cl->InheritsFrom("TH2D")) continue;
		cans->cd();
		TH2D *hist = (TH2D*)key->ReadObj();
		hist->Draw("colz");
		gPad->SetLogz(kTRUE);
		gPad->Modified();
		gPad->Update();
		std::string HistName(hist->GetName());
		if(HistName.find(DEEHistName.c_str())==std::string::npos) continue;

		for(int i=0; i<MAXPOINTS; i++) {
			std::string CutName;
			// 手动添加或删减粒子种类
			cout<<endl;
			cout<<"Please draw the isotope cut you want to do: "<<endl;
			cout<<"Z = "; cin>>charge;
			cout<<"A = "; cin>>mass;

			CutName.assign(HistName+Form("_Z%02d_A%02d",charge,mass));
			if(FileOut->Get(CutName.c_str()) || CutName.empty()) continue; //cut already existing

			// 这一行命令用于调用 “剪刀手” z作 cut
			TCutG* mycut = (TCutG*)gPad->WaitPrimitive("CUTG");
			mycut->SetName(CutName.c_str());

			printf("%s cut created\n", CutName.c_str());
			printf("S - save the cut\nR - retry\nF - skip this cut\nD - delete a cu\nN - next Tel\nE - exit\n");
			char input;
			cin>>input;
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
			else if (input == 'N' || input == 'n') { break; }// jump to next telescope}
			else if (input == 'E' || input == 'e'){
				printf("Goodbye\n");
				FileIn->Close();
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


//______________________________________________________________________________
// 对 Z=1 的同位素进行拟合
// 对提取的数据点进行拟合
void CSHINECsIEnergyCali::DEEFITDrawAndFit_Z1()
{
	gStyle->SetPalette(1);

  std::string pathECsIRange(Form("%sdata_PID/L2L3_StraighteningFitPars.dat",PATHDATAFOLDER));
	std::string pathDataInput(Form("%sdata_CsIEnergyCali/DEEFIT_CsIEnergyPoints.dat",PATHDATAFOLDER));

	std::string pathDataOut(Form("%sdata_CsIEnergyCali/DEEFITCsIEnergyFitPars_Z1.data", PATHDATAFOLDER));
	std::string pathPDFOut(Form("%sfigure_CsIEnergyCali/DEEFITCsIEnergyPointsAndFit_Z1.pdf",PATHFIGURESFOLDER));
	std::string pathPDFOutBegin(Form("%sfigure_CsIEnergyCali/DEEFITCsIEnergyPointsAndFit_Z1.pdf[",PATHFIGURESFOLDER));
	std::string pathPDFOutEnd(Form("%sfigure_CsIEnergyCali/DEEFITCsIEnergyPointsAndFit_Z1.pdf]",PATHFIGURESFOLDER));

	ofstream FileOut(pathDataOut.c_str());
	FileOut<<"* fit parameters for Z=1 isotopes"<<endl;
	FileOut<<"* fit_func = par[0]+par[1]*E^{(par[2]+A)/(par[3]+A)}"<<endl;
	FileOut<<"* numtel"<<setw(15)<<"ssdnum"<<setw(15)<<"csinum"<<setw(15)
	       <<"par[0]"<<setw(15)<<"par[1]"<<setw(15)<<"par[2]"<<setw(15)<<"par[3]"<<endl;

  ifstream FileInRange(pathECsIRange.c_str());
	ifstream FileInEnergyPoints(pathDataInput.c_str());

	Color_t colors [6] = {kRed-4, kMagenta-4, kViolet+6, kAzure+6, kSpring-5, kOrange-4};
	Int_t   markers[19] = {24,25,26,24,25,26,24,25,26,27,24,25,26,24,25,26,24,25,26};

	// 定义变量存储 ECsI_Ch, ECsI_MeV, ParticleID
	std::vector<Int_t> Charge[NUM_SSD*NUM_CSI];
	std::vector<Int_t> Mass[NUM_SSD*NUM_CSI];
	std::vector<Double_t> ECsI_Ch[NUM_SSD*NUM_CSI][10][20];   //[numtel][Z][A]
	std::vector<Double_t> ECsI_MeV[NUM_SSD*NUM_CSI][10][20];  //[numtel][Z][A]
	std::vector<Double_t> ParticleID[NUM_SSD*NUM_CSI][10][20];//[numtel][Z][A], ID=100*Z+A
	std::vector<Double_t> ECsI_Ch_Z1[NUM_SSD*NUM_CSI];
	std::vector<Double_t> ECsI_MeV_Z1[NUM_SSD*NUM_CSI];
	std::vector<Double_t> ParticleID_Z1[NUM_SSD*NUM_CSI];

	//__________________________
	// 提取电荷数 Z 与质量数 A
	if (!FileInRange.is_open()) {
		printf("Error: file %s not found\n",pathECsIRange.c_str());
    return;
	}
	while (FileInRange.good()) {
		// 按行读取数据
    std::string LineRead;
    std::getline(FileInRange, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

		Int_t numtel;
		Int_t charge, mass, particleno;
		LineStream>>numtel>>particleno>>charge>>mass;

	  Charge[numtel].push_back(charge);
		Mass[numtel].push_back(mass);

	}
	FileInRange.close();
  //____________________
	// 提取数据点
	if (!FileInEnergyPoints.is_open()) {
		printf("Error: file %s not found\n",pathDataInput.c_str());
    return;
	}
	while (FileInEnergyPoints.good()) {
		// 按行读取数据
    std::string LineRead;
    std::getline(FileInEnergyPoints, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

		Int_t numtel;
		Int_t charge, mass;
		Double_t ecsi_mev, ecsi_ch;
		LineStream>>numtel>>charge>>mass>>ecsi_mev>>ecsi_ch;

    Double_t particleID = (Double_t) 100*charge+mass;

		ECsI_Ch[numtel][charge][mass].push_back(ecsi_ch);
		ECsI_MeV[numtel][charge][mass].push_back(ecsi_mev);

    if (charge==1) {
			ECsI_MeV_Z1[numtel].push_back(ecsi_mev);
			ECsI_Ch_Z1[numtel].push_back(ecsi_ch);
			ParticleID_Z1[numtel].push_back(particleID);
		}
	}
	FileInEnergyPoints.close();

	// 定义 TMultiGraph && TGraph
	TMultiGraph* multi_graph_Z1[NUM_SSD*NUM_CSI];
	TGraph* graph[NUM_SSD*NUM_CSI][10][20];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		if (Charge[numtel].size()==0) continue;
		multi_graph_Z1[numtel] = new TMultiGraph();
		for (Int_t ip=0; ip<Charge[numtel].size(); ip++) {
			if (Charge[numtel][ip]==1) {
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]] = new TGraph(ECsI_MeV[numtel][Charge[numtel][ip]][Mass[numtel][ip]].size(),
				ECsI_MeV[numtel][Charge[numtel][ip]][Mass[numtel][ip]].data(),ECsI_Ch[numtel][Charge[numtel][ip]][Mass[numtel][ip]].data());
        multi_graph_Z1[numtel]->Add(graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetMarkerStyle(markers[ip]);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetMarkerColor(colors[Charge[numtel][ip]-1]);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetMarkerSize(1);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetLineColor(colors[Charge[numtel][ip]-1]);
			}
		}
	}

	// 定义 Canvas 与 graph 格式
	TCanvas* c_begin = new TCanvas("c_begin","c_begin",800,600);
	TCanvas* c_end = new TCanvas("c_end","c_end",800,600);
	c_begin->Close();
	c_end->Close();
	TCanvas* cans[NUM_SSD*NUM_CSI];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		Int_t ssdindex = numtel/NUM_CSI;
		Int_t csiindex = numtel%NUM_CSI;
		cans[numtel] = new TCanvas(Form("c_SSD%d_CsI%d",ssdindex+1,csiindex),Form("c_SSD%d_CsI%d",ssdindex+1,csiindex),1000,800);
	}
	c_begin->Print(pathPDFOutBegin.c_str());

	// 定义 TGraph2D, 用于拟合
	TGraph2D* gr2D_Z1[NUM_SSD*NUM_CSI];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {

		if (numtel==31 || numtel==32) continue; // SSD4_CsI[4] and SSD4_CsI[5]不能用
		Int_t ssdindex = numtel/NUM_CSI;
		Int_t csiindex = numtel%NUM_CSI;

    // for Z=1
		cans[numtel]->cd();
		gPad->SetLeftMargin(0.17);
		multi_graph_Z1[numtel]->Draw("AP");
		multi_graph_Z1[numtel]->SetTitle(Form("SSD%d_CsI%d_Z=1",ssdindex+1,csiindex));
		multi_graph_Z1[numtel]->GetXaxis()->SetTitle("EMeV");
		multi_graph_Z1[numtel]->GetXaxis()->CenterTitle(1);
		multi_graph_Z1[numtel]->GetXaxis()->SetTitleSize(0.05);
		multi_graph_Z1[numtel]->GetXaxis()->SetLabelSize(0.05);
		multi_graph_Z1[numtel]->GetXaxis()->SetRangeUser(0,150);
		multi_graph_Z1[numtel]->GetXaxis()->SetNdivisions(510);
		multi_graph_Z1[numtel]->GetYaxis()->SetTitle("ECh");
		multi_graph_Z1[numtel]->GetYaxis()->CenterTitle(1);
		multi_graph_Z1[numtel]->GetYaxis()->SetTitleSize(0.05);
		multi_graph_Z1[numtel]->GetYaxis()->SetLabelSize(0.05);
		multi_graph_Z1[numtel]->GetYaxis()->SetRangeUser(0,3000);
		multi_graph_Z1[numtel]->GetYaxis()->SetNdivisions(510);
		gPad->Modified();
		gPad->Update();

		gr2D_Z1[numtel] = new TGraph2D(ECsI_Ch_Z1[numtel].size(),ECsI_MeV_Z1[numtel].data(),ParticleID_Z1[numtel].data(),ECsI_Ch_Z1[numtel].data());
		TF2* fFitHydrogen = new TF2("fFitHydrogen",this,&CSHINECsIEnergyCali::FitFunc_Hydrogen,0,500,100,700,4);
		fFitHydrogen->SetParameters(1.,1.,1.,1.);
		gr2D_Z1[numtel]->Fit("fFitHydrogen");

		FileOut<<setw(5)<<numtel<<setw(15)<<ssdindex<<setw(15)<<csiindex<<setw(20)
		       <<fFitHydrogen->GetParameter(0)<<setw(15)<<fFitHydrogen->GetParameter(1)<<setw(15)
					 <<fFitHydrogen->GetParameter(2)<<setw(15)<<fFitHydrogen->GetParameter(3)<<endl;

		for (Int_t ip=0; ip<Charge[numtel].size(); ip++) {
			if (Charge[numtel][ip]==1) { // for Z==1
         TF1* fDrawHydrogen = new TF1("fDrawHydrogen",this,&CSHINECsIEnergyCali::DrawFunc_Hydrogen,0,500,6);
				 fDrawHydrogen->SetParameters(fFitHydrogen->GetParameters());
				 fDrawHydrogen->SetParameter(4,(Double_t)Charge[numtel][ip]);
				 fDrawHydrogen->SetParameter(5,(Double_t)Mass[numtel][ip]);
				 fDrawHydrogen->SetLineColor(colors[Charge[numtel][ip]-1]);
				 fDrawHydrogen->Draw("SAME");
			}
		}
    cans[numtel]->Print(pathPDFOut.c_str());
	}
  c_end->Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
//  对 Z=2 的同位素进行拟合
//  对提取的数据点进行拟合
void CSHINECsIEnergyCali::DEEFITDrawAndFit_ZOver2()
{
	gStyle->SetPalette(1);

  std::string pathECsIRange(Form("%sdata_PID/L2L3_StraighteningFitPars.dat",PATHDATAFOLDER));
	std::string pathDataInput(Form("%sdata_CsIEnergyCali/DEEFIT_CsIEnergyPoints.dat",PATHDATAFOLDER));

	std::string pathDataOut(Form("%sdata_CsIEnergyCali/DEEFITCsIEnergyFitPars_ZOver2.data", PATHDATAFOLDER));
	std::string pathPDFOut(Form("%sfigure_CsIEnergyCali/DEEFITCsIEnergyPointsAndFit_ZOver2.pdf",PATHFIGURESFOLDER));
	std::string pathPDFOutBegin(Form("%sfigure_CsIEnergyCali/DEEFITCsIEnergyPointsAndFit_ZOver2.pdf[",PATHFIGURESFOLDER));
	std::string pathPDFOutEnd(Form("%sfigure_CsIEnergyCali/DEEFITCsIEnergyPointsAndFit_ZOver2.pdf]",PATHFIGURESFOLDER));

	ofstream FileOut(pathDataOut.c_str());
	FileOut<<"* fit parameters for Z=2 isotopes"<<endl;
	FileOut<<"* fit_func = DEEFIT_Func14"<<endl;
	FileOut<<"* numtel"<<setw(15)<<"ssdnum"<<setw(15)<<"csinum"<<setw(15)
	       <<"par[0]"<<setw(15)<<"par[1]"<<setw(15)<<"par[2]"<<endl;

  ifstream FileInRange(pathECsIRange.c_str());
	ifstream FileInEnergyPoints(pathDataInput.c_str());

	Color_t colors [6] = {kRed-4, kMagenta-4, kViolet+6, kAzure+6, kSpring-5, kOrange-4};
	Int_t   markers[19] = {24,25,26,24,25,26,24,25,26,27,24,25,26,24,25,26,24,25,26};

	// 定义变量存储 ECsI_Ch, ECsI_MeV, ParticleID
	std::vector<Int_t> Charge[NUM_SSD*NUM_CSI];
	std::vector<Int_t> Mass[NUM_SSD*NUM_CSI];
	std::vector<Double_t> ECsI_Ch[NUM_SSD*NUM_CSI][10][20];   //[numtel][Z][A]
	std::vector<Double_t> ECsI_MeV[NUM_SSD*NUM_CSI][10][20];  //[numtel][Z][A]
	std::vector<Double_t> ParticleID[NUM_SSD*NUM_CSI][10][20];//[numtel][Z][A], ID=100*Z+A
	std::vector<Double_t> ECsI_Ch_ZOver2[NUM_SSD*NUM_CSI];
	std::vector<Double_t> ECsI_MeV_ZOver2[NUM_SSD*NUM_CSI];
	std::vector<Double_t> ParticleID_ZOver2[NUM_SSD*NUM_CSI];

	//__________________________
	// 提取电荷数 Z 与质量数 A
	if (!FileInRange.is_open()) {
		printf("Error: file %s not found\n",pathECsIRange.c_str());
    return;
	}
	while (FileInRange.good()) {
		// 按行读取数据
    std::string LineRead;
    std::getline(FileInRange, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

		Int_t numtel;
		Int_t charge, mass, particleno;
		LineStream>>numtel>>particleno>>charge>>mass;

	  Charge[numtel].push_back(charge);
		Mass[numtel].push_back(mass);

	}
	FileInRange.close();
  //____________________
	// 提取数据点
	if (!FileInEnergyPoints.is_open()) {
		printf("Error: file %s not found\n",pathDataInput.c_str());
    return;
	}
	while (FileInEnergyPoints.good()) {
		// 按行读取数据
    std::string LineRead;
    std::getline(FileInEnergyPoints, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

		Int_t numtel;
		Int_t charge, mass;
		Double_t ecsi_mev, ecsi_ch;
		LineStream>>numtel>>charge>>mass>>ecsi_mev>>ecsi_ch;

	  if (ecsi_mev==0) continue;

    Double_t particleID = (Double_t) 100*charge+mass;

		ECsI_Ch[numtel][charge][mass].push_back(ecsi_ch);
		ECsI_MeV[numtel][charge][mass].push_back(ecsi_mev);

		if (charge>=2) {
			ECsI_MeV_ZOver2[numtel].push_back(ecsi_mev);
			ECsI_Ch_ZOver2[numtel].push_back(ecsi_ch);
			ParticleID_ZOver2[numtel].push_back(particleID);
		}

	}
	FileInEnergyPoints.close();

	// 定义 TMultiGraph && TGraph
	TMultiGraph* multi_graph_ZOver2[NUM_SSD*NUM_CSI];
	TGraph* graph[NUM_SSD*NUM_CSI][10][20];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		if (Charge[numtel].size()==0) continue;
		multi_graph_ZOver2[numtel] = new TMultiGraph();
		for (Int_t ip=0; ip<Charge[numtel].size(); ip++) {
			if (Charge[numtel][ip]>1) {
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]] = new TGraph(ECsI_MeV[numtel][Charge[numtel][ip]][Mass[numtel][ip]].size(),
				ECsI_MeV[numtel][Charge[numtel][ip]][Mass[numtel][ip]].data(),ECsI_Ch[numtel][Charge[numtel][ip]][Mass[numtel][ip]].data());
        multi_graph_ZOver2[numtel]->Add(graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetMarkerStyle(markers[ip]);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetMarkerColor(colors[Charge[numtel][ip]-1]);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetMarkerSize(1);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetLineColor(colors[Charge[numtel][ip]-1]);
			}
		}
	}

	// 定义 Canvas 与 graph 格式
	TCanvas* c_begin = new TCanvas("c_begin","c_begin",800,600);
	TCanvas* c_end = new TCanvas("c_end","c_end",800,600);
	c_begin->Close();
	c_end->Close();
	TCanvas* cans[NUM_SSD*NUM_CSI];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		Int_t ssdindex = numtel/NUM_CSI;
		Int_t csiindex = numtel%NUM_CSI;
		cans[numtel] = new TCanvas(Form("c_SSD%d_CsI%d",ssdindex+1,csiindex),Form("c_SSD%d_CsI%d",ssdindex+1,csiindex),1000,800);
	}
	c_begin->Print(pathPDFOutBegin.c_str());

	// 定义 TGraph2D, 用于拟合
	TGraph2D* gr2D_ZOver2[NUM_SSD*NUM_CSI];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {

		if (numtel==31 || numtel==32) continue; // SSD4_CsI[4] and SSD4_CsI[5]不能用
		Int_t ssdindex = numtel/NUM_CSI;
		Int_t csiindex = numtel%NUM_CSI;

		// for Z>1
		cans[numtel]->cd();
		gPad->SetLeftMargin(0.17);
		multi_graph_ZOver2[numtel]->Draw("AP");
		multi_graph_ZOver2[numtel]->SetTitle(Form("SSD%d_CsI%d_Z#geq2",ssdindex+1,csiindex));
		multi_graph_ZOver2[numtel]->GetXaxis()->SetTitle("EMeV");
		multi_graph_ZOver2[numtel]->GetXaxis()->CenterTitle(1);
		multi_graph_ZOver2[numtel]->GetXaxis()->SetTitleSize(0.05);
		multi_graph_ZOver2[numtel]->GetXaxis()->SetLabelSize(0.05);
		multi_graph_ZOver2[numtel]->GetXaxis()->SetRangeUser(0,300);
		multi_graph_ZOver2[numtel]->GetXaxis()->SetNdivisions(510);
		multi_graph_ZOver2[numtel]->GetYaxis()->SetTitle("ECh");
		multi_graph_ZOver2[numtel]->GetYaxis()->CenterTitle(1);
		multi_graph_ZOver2[numtel]->GetYaxis()->SetTitleSize(0.05);
		multi_graph_ZOver2[numtel]->GetYaxis()->SetLabelSize(0.05);
		multi_graph_ZOver2[numtel]->GetYaxis()->SetRangeUser(0,4100);
		multi_graph_ZOver2[numtel]->GetYaxis()->SetNdivisions(510);
		gPad->Modified();
		gPad->Update();

    gr2D_ZOver2[numtel] = new TGraph2D(ECsI_MeV_ZOver2[numtel].size(),ECsI_MeV_ZOver2[numtel].data(),ParticleID_ZOver2[numtel].data(),ECsI_Ch_ZOver2[numtel].data());
		TF2* fFitHeavyIOn = new TF2("fFitHeavyIOn",this,&CSHINECsIEnergyCali::FitFunc_HeavyIon,0,500,100,700,3);
		fFitHeavyIOn->SetParameters(1.,100,1.);
		gr2D_ZOver2[numtel]->Fit("fFitHeavyIOn");

		FileOut<<setw(5)<<numtel<<setw(15)<<ssdindex<<setw(15)<<csiindex<<setw(20)
		       <<fFitHeavyIOn->GetParameter(0)<<setw(15)<<fFitHeavyIOn->GetParameter(1)<<setw(15)
					 <<fFitHeavyIOn->GetParameter(2)<<setw(15)<<endl;

		for (Int_t ip=0; ip<Charge[numtel].size(); ip++) {
			if (Charge[numtel][ip]>=2) { // for Z>=2
         TF1* fDrawHeavyIon = new TF1("fDrawHeavyIon",this,&CSHINECsIEnergyCali::DrawFunc_HeavyIon,0,500,5);
				 fDrawHeavyIon->SetParameters(fFitHeavyIOn->GetParameters());
				 fDrawHeavyIon->SetParameter(3,(Double_t)Charge[numtel][ip]);
				 fDrawHeavyIon->SetParameter(4,(Double_t)Mass[numtel][ip]);
				 fDrawHeavyIon->SetLineColor(colors[Charge[numtel][ip]-1]);
				 fDrawHeavyIon->Draw("SAME");
			}
		}
    cans[numtel]->Print(pathPDFOut.c_str());
	}
  c_end->Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void CSHINECsIEnergyCali::StraighteningDrawAndFit_Z1()
{
	gStyle->SetPalette(1);

  std::string pathFileInEnergyPoints(Form("%sdata_CsIEnergyCali/StraighteningCsIEnergyPoints.dat",PATHDATAFOLDER));
	std::string pathStraighteningFitPars(Form("%sdata_PID/L2L3_StraighteningFitPars.dat",PATHDATAFOLDER));

	std::string pathDataOut(Form("%sdata_CsIEnergyCali/StraighteningCsIEnergyFitPars_Z1.data", PATHDATAFOLDER));
	std::string pathPDFOut(Form("%sfigure_CsIEnergyCali/StraighteningCsIEnergyPointsAndFit_Z1.pdf",PATHFIGURESFOLDER));
	std::string pathPDFOutBegin(Form("%sfigure_CsIEnergyCali/StraighteningCsIEnergyPointsAndFit_Z1.pdf[",PATHFIGURESFOLDER));
	std::string pathPDFOutEnd(Form("%sfigure_CsIEnergyCali/StraighteningCsIEnergyPointsAndFit_Z1.pdf]",PATHFIGURESFOLDER));

	ofstream FileOut(pathDataOut.c_str());
	FileOut<<"* fit parameters for Z=1 isotopes"<<endl;
	FileOut<<"* fit_func = par[0]+par[1]*E^{(par[2]+A)/(par[3]+A)}"<<endl;
	FileOut<<"* numtel"<<setw(15)<<"ssdnum"<<setw(15)<<"csinum"<<setw(15)
	       <<"par[0]"<<setw(15)<<"par[1]"<<setw(15)<<"par[2]"<<setw(15)<<"par[3]"<<endl;

  ifstream FileInRange(pathStraighteningFitPars.c_str());
	ifstream FileInEnergyPoints(pathFileInEnergyPoints.c_str());

	Color_t colors [6] = {kRed-4, kMagenta-4, kViolet+6, kAzure+6, kSpring-5, kOrange-4};
	Int_t   markers[19] = {24,25,26,24,25,26,24,25,26,27,24,25,26,24,25,26,24,25,26};

	// 定义变量存储 ECsI_Ch, ECsI_MeV, ParticleID
	std::vector<Int_t> Charge[NUM_SSD*NUM_CSI];
	std::vector<Int_t> Mass[NUM_SSD*NUM_CSI];
	std::vector<Double_t> ECsI_Ch[NUM_SSD*NUM_CSI][10][20];   //[numtel][Z][A]
	std::vector<Double_t> ECsI_MeV[NUM_SSD*NUM_CSI][10][20];  //[numtel][Z][A]
	std::vector<Double_t> ParticleID[NUM_SSD*NUM_CSI][10][20];//[numtel][Z][A], ID=100*Z+A
	std::vector<Double_t> ECsI_Ch_Z1[NUM_SSD*NUM_CSI];
	std::vector<Double_t> ECsI_MeV_Z1[NUM_SSD*NUM_CSI];
	std::vector<Double_t> ParticleID_Z1[NUM_SSD*NUM_CSI];

	//__________________________
	// 提取电荷数 Z 与质量数 A
	if (!FileInRange.is_open()) {
		printf("Error: file %s not found\n",pathStraighteningFitPars.c_str());
    return;
	}
	while (FileInRange.good()) {
		// 按行读取数据
    std::string LineRead;
    std::getline(FileInRange, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

		Int_t numtel;
		Int_t charge, mass, particleno;
		LineStream>>numtel>>particleno>>charge>>mass;

	  Charge[numtel].push_back(charge);
		Mass[numtel].push_back(mass);

	}
	FileInRange.close();
  //____________________
	// 提取数据点
	if (!FileInEnergyPoints.is_open()) {
		printf("Error: file %s not found\n",pathFileInEnergyPoints.c_str());
    return;
	}
	while (FileInEnergyPoints.good()) {
		// 按行读取数据
    std::string LineRead;
    std::getline(FileInEnergyPoints, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

		Int_t numtel;
		Int_t charge, mass;
		Double_t ecsi_mev, ecsi_ch;
		LineStream>>numtel>>charge>>mass>>ecsi_mev>>ecsi_ch;

    Double_t particleID = (Double_t) 100*charge+mass;

		ECsI_Ch[numtel][charge][mass].push_back(ecsi_ch);
		ECsI_MeV[numtel][charge][mass].push_back(ecsi_mev);

    if (charge==1) {
			ECsI_MeV_Z1[numtel].push_back(ecsi_mev);
			ECsI_Ch_Z1[numtel].push_back(ecsi_ch);
			ParticleID_Z1[numtel].push_back(particleID);
		}
	}
	FileInEnergyPoints.close();

	// 定义 TMultiGraph && TGraph
	TMultiGraph* multi_graph_Z1[NUM_SSD*NUM_CSI];
	TGraph* graph[NUM_SSD*NUM_CSI][10][20];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		if (Charge[numtel].size()==0) continue;
		multi_graph_Z1[numtel] = new TMultiGraph();
		for (Int_t ip=0; ip<Charge[numtel].size(); ip++) {
			if (Charge[numtel][ip]==1) {
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]] = new TGraph(ECsI_MeV[numtel][Charge[numtel][ip]][Mass[numtel][ip]].size(),
				ECsI_MeV[numtel][Charge[numtel][ip]][Mass[numtel][ip]].data(),ECsI_Ch[numtel][Charge[numtel][ip]][Mass[numtel][ip]].data());
        multi_graph_Z1[numtel]->Add(graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetMarkerStyle(markers[ip]);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetMarkerColor(colors[Charge[numtel][ip]-1]);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetMarkerSize(1);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetLineColor(colors[Charge[numtel][ip]-1]);
			}
		}
	}

	// 定义 Canvas 与 graph 格式
	TCanvas* c_begin = new TCanvas("c_begin","c_begin",800,600);
	TCanvas* c_end = new TCanvas("c_end","c_end",800,600);
	c_begin->Close();
	c_end->Close();
	TCanvas* cans[NUM_SSD*NUM_CSI];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		Int_t ssdindex = numtel/NUM_CSI;
		Int_t csiindex = numtel%NUM_CSI;
		cans[numtel] = new TCanvas(Form("c_SSD%d_CsI%d",ssdindex+1,csiindex),Form("c_SSD%d_CsI%d",ssdindex+1,csiindex),1000,800);
	}
	c_begin->Print(pathPDFOutBegin.c_str());

	// 定义 TGraph2D, 用于拟合
	TGraph2D* gr2D_Z1[NUM_SSD*NUM_CSI];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {

		if (numtel==31 || numtel==32) continue; // SSD4_CsI[4] and SSD4_CsI[5]不能用
		Int_t ssdindex = numtel/NUM_CSI;
		Int_t csiindex = numtel%NUM_CSI;

    // for Z=1
		cans[numtel]->cd();
		gPad->SetLeftMargin(0.17);
		multi_graph_Z1[numtel]->Draw("AP");
		multi_graph_Z1[numtel]->SetTitle(Form("SSD%d_CsI%d_Z=1",ssdindex+1,csiindex));
		multi_graph_Z1[numtel]->GetXaxis()->SetTitle("EMeV");
		multi_graph_Z1[numtel]->GetXaxis()->CenterTitle(1);
		multi_graph_Z1[numtel]->GetXaxis()->SetTitleSize(0.05);
		multi_graph_Z1[numtel]->GetXaxis()->SetLabelSize(0.05);
		multi_graph_Z1[numtel]->GetXaxis()->SetRangeUser(0,150);
		multi_graph_Z1[numtel]->GetXaxis()->SetNdivisions(510);
		multi_graph_Z1[numtel]->GetYaxis()->SetTitle("ECh");
		multi_graph_Z1[numtel]->GetYaxis()->CenterTitle(1);
		multi_graph_Z1[numtel]->GetYaxis()->SetTitleSize(0.05);
		multi_graph_Z1[numtel]->GetYaxis()->SetLabelSize(0.05);
		multi_graph_Z1[numtel]->GetYaxis()->SetRangeUser(0,3000);
		multi_graph_Z1[numtel]->GetYaxis()->SetNdivisions(510);
		gPad->Modified();
		gPad->Update();

		gr2D_Z1[numtel] = new TGraph2D(ECsI_Ch_Z1[numtel].size(),ECsI_MeV_Z1[numtel].data(),ParticleID_Z1[numtel].data(),ECsI_Ch_Z1[numtel].data());
		TF2* fFitHydrogen = new TF2("fFitHydrogen",this,&CSHINECsIEnergyCali::FitFunc_Hydrogen,0,500,100,700,4);
		fFitHydrogen->SetParameters(1.,1.,1.,1.);
		gr2D_Z1[numtel]->Fit("fFitHydrogen");

		FileOut<<setw(5)<<numtel<<setw(15)<<ssdindex<<setw(15)<<csiindex<<setw(20)
		       <<fFitHydrogen->GetParameter(0)<<setw(15)<<fFitHydrogen->GetParameter(1)<<setw(15)
					 <<fFitHydrogen->GetParameter(2)<<setw(15)<<fFitHydrogen->GetParameter(3)<<endl;

		for (Int_t ip=0; ip<Charge[numtel].size(); ip++) {
			if (Charge[numtel][ip]==1) { // for Z==1
         TF1* fDrawHydrogen = new TF1("fDrawHydrogen",this,&CSHINECsIEnergyCali::DrawFunc_Hydrogen,0,500,6);
				 fDrawHydrogen->SetParameters(fFitHydrogen->GetParameters());
				 fDrawHydrogen->SetParameter(4,(Double_t)Charge[numtel][ip]);
				 fDrawHydrogen->SetParameter(5,(Double_t)Mass[numtel][ip]);
				 fDrawHydrogen->SetLineColor(colors[Charge[numtel][ip]-1]);
				 fDrawHydrogen->Draw("SAME");
			}
		}
    cans[numtel]->Print(pathPDFOut.c_str());
	}
  c_end->Print(pathPDFOutEnd.c_str());
}


//______________________________________________________________________________
void CSHINECsIEnergyCali::StraighteningDrawAndFit_ZOver2()
{
	gStyle->SetPalette(1);

	std::string pathFileInEnergyPoints(Form("%sdata_CsIEnergyCali/StraighteningCsIEnergyPoints.dat",PATHDATAFOLDER));
	std::string pathStraighteningFitPars(Form("%sdata_PID/L2L3_StraighteningFitPars.dat",PATHDATAFOLDER));

	std::string pathDataOut(Form("%sdata_CsIEnergyCali/StraighteningCsIEnergyFitPars_ZOver2.data", PATHDATAFOLDER));
	std::string pathPDFOut(Form("%sfigure_CsIEnergyCali/StraighteningCsIEnergyPointsAndFit_ZOver2.pdf",PATHFIGURESFOLDER));
	std::string pathPDFOutBegin(Form("%sfigure_CsIEnergyCali/StraighteningCsIEnergyPointsAndFit_ZOver2.pdf[",PATHFIGURESFOLDER));
	std::string pathPDFOutEnd(Form("%sfigure_CsIEnergyCali/StraighteningCsIEnergyPointsAndFit_ZOver2.pdf]",PATHFIGURESFOLDER));

	ofstream FileOut(pathDataOut.c_str());
	FileOut<<"* fit parameters for Z=2 isotopes"<<endl;
	FileOut<<"* fit_func = DEEFIT_Func14"<<endl;
	FileOut<<"* numtel"<<setw(15)<<"ssdnum"<<setw(15)<<"csinum"<<setw(15)
				 <<"par[0]"<<setw(15)<<"par[1]"<<setw(15)<<"par[2]"<<endl;

	ifstream FileInRange(pathStraighteningFitPars.c_str());
	ifstream FileInEnergyPoints(pathFileInEnergyPoints.c_str());

	Color_t colors [6] = {kRed-4, kMagenta-4, kViolet+6, kAzure+6, kSpring-5, kOrange-4};
	Int_t   markers[19] = {24,25,26,24,25,26,24,25,26,27,24,25,26,24,25,26,24,25,26};

	// 定义变量存储 ECsI_Ch, ECsI_MeV, ParticleID
	std::vector<Int_t> Charge[NUM_SSD*NUM_CSI];
	std::vector<Int_t> Mass[NUM_SSD*NUM_CSI];
	std::vector<Double_t> ECsI_Ch[NUM_SSD*NUM_CSI][10][20];   //[numtel][Z][A]
	std::vector<Double_t> ECsI_MeV[NUM_SSD*NUM_CSI][10][20];  //[numtel][Z][A]
	std::vector<Double_t> ParticleID[NUM_SSD*NUM_CSI][10][20];//[numtel][Z][A], ID=100*Z+A
	std::vector<Double_t> ECsI_Ch_ZOver2[NUM_SSD*NUM_CSI];
	std::vector<Double_t> ECsI_MeV_ZOver2[NUM_SSD*NUM_CSI];
	std::vector<Double_t> ParticleID_ZOver2[NUM_SSD*NUM_CSI];

	//__________________________
	// 提取电荷数 Z 与质量数 A
	if (!FileInRange.is_open()) {
		printf("Error: file %s not found\n",pathStraighteningFitPars.c_str());
		return;
	}
	while (FileInRange.good()) {
		// 按行读取数据
		std::string LineRead;
		std::getline(FileInRange, LineRead);
		LineRead.assign(LineRead.substr(0, LineRead.find('*')));
		if(LineRead.empty()) continue;
		if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
		std::istringstream LineStream(LineRead);

		Int_t numtel;
		Int_t charge, mass, particleno;
		LineStream>>numtel>>particleno>>charge>>mass;

		Charge[numtel].push_back(charge);
		Mass[numtel].push_back(mass);

	}
	FileInRange.close();
	//____________________
	// 提取数据点
	if (!FileInEnergyPoints.is_open()) {
		printf("Error: file %s not found\n",pathFileInEnergyPoints.c_str());
		return;
	}
	while (FileInEnergyPoints.good()) {
		// 按行读取数据
		std::string LineRead;
		std::getline(FileInEnergyPoints, LineRead);
		LineRead.assign(LineRead.substr(0, LineRead.find('*')));
		if(LineRead.empty()) continue;
		if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
		std::istringstream LineStream(LineRead);

		Int_t numtel;
		Int_t charge, mass;
		Double_t ecsi_mev, ecsi_ch;
		LineStream>>numtel>>charge>>mass>>ecsi_mev>>ecsi_ch;

		if (ecsi_mev==0) continue;

		Double_t particleID = (Double_t) 100*charge+mass;

		ECsI_Ch[numtel][charge][mass].push_back(ecsi_ch);
		ECsI_MeV[numtel][charge][mass].push_back(ecsi_mev);

		if (charge>=2) {
			ECsI_MeV_ZOver2[numtel].push_back(ecsi_mev);
			ECsI_Ch_ZOver2[numtel].push_back(ecsi_ch);
			ParticleID_ZOver2[numtel].push_back(particleID);
		}

	}
	FileInEnergyPoints.close();

	// 定义 TMultiGraph && TGraph
	TMultiGraph* multi_graph_ZOver2[NUM_SSD*NUM_CSI];
	TGraph* graph[NUM_SSD*NUM_CSI][10][20];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		if (Charge[numtel].size()==0) continue;
		multi_graph_ZOver2[numtel] = new TMultiGraph();
		for (Int_t ip=0; ip<Charge[numtel].size(); ip++) {
			if (Charge[numtel][ip]>1) {
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]] = new TGraph(ECsI_MeV[numtel][Charge[numtel][ip]][Mass[numtel][ip]].size(),
				ECsI_MeV[numtel][Charge[numtel][ip]][Mass[numtel][ip]].data(),ECsI_Ch[numtel][Charge[numtel][ip]][Mass[numtel][ip]].data());
				multi_graph_ZOver2[numtel]->Add(graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetMarkerStyle(markers[ip]);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetMarkerColor(colors[Charge[numtel][ip]-1]);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetMarkerSize(1);
				graph[numtel][Charge[numtel][ip]][Mass[numtel][ip]]->SetLineColor(colors[Charge[numtel][ip]-1]);
			}
		}
	}

	// 定义 Canvas 与 graph 格式
	TCanvas* c_begin = new TCanvas("c_begin","c_begin",800,600);
	TCanvas* c_end = new TCanvas("c_end","c_end",800,600);
	c_begin->Close();
	c_end->Close();
	TCanvas* cans[NUM_SSD*NUM_CSI];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		Int_t ssdindex = numtel/NUM_CSI;
		Int_t csiindex = numtel%NUM_CSI;
		cans[numtel] = new TCanvas(Form("c_SSD%d_CsI%d",ssdindex+1,csiindex),Form("c_SSD%d_CsI%d",ssdindex+1,csiindex),1000,800);
	}
	c_begin->Print(pathPDFOutBegin.c_str());

	// 定义 TGraph2D, 用于拟合
	TGraph2D* gr2D_ZOver2[NUM_SSD*NUM_CSI];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {

		if (numtel==31 || numtel==32) continue; // SSD4_CsI[4] and SSD4_CsI[5]不能用
		Int_t ssdindex = numtel/NUM_CSI;
		Int_t csiindex = numtel%NUM_CSI;

		// for Z>1
		cans[numtel]->cd();
		gPad->SetLeftMargin(0.17);
		multi_graph_ZOver2[numtel]->Draw("AP");
		multi_graph_ZOver2[numtel]->SetTitle(Form("SSD%d_CsI%d_Z#geq2",ssdindex+1,csiindex));
		multi_graph_ZOver2[numtel]->GetXaxis()->SetTitle("EMeV");
		multi_graph_ZOver2[numtel]->GetXaxis()->CenterTitle(1);
		multi_graph_ZOver2[numtel]->GetXaxis()->SetTitleSize(0.05);
		multi_graph_ZOver2[numtel]->GetXaxis()->SetLabelSize(0.05);
		multi_graph_ZOver2[numtel]->GetXaxis()->SetRangeUser(0,300);
		multi_graph_ZOver2[numtel]->GetXaxis()->SetNdivisions(510);
		multi_graph_ZOver2[numtel]->GetYaxis()->SetTitle("ECh");
		multi_graph_ZOver2[numtel]->GetYaxis()->CenterTitle(1);
		multi_graph_ZOver2[numtel]->GetYaxis()->SetTitleSize(0.05);
		multi_graph_ZOver2[numtel]->GetYaxis()->SetLabelSize(0.05);
		multi_graph_ZOver2[numtel]->GetYaxis()->SetRangeUser(0,4100);
		multi_graph_ZOver2[numtel]->GetYaxis()->SetNdivisions(510);
		gPad->Modified();
		gPad->Update();

		gr2D_ZOver2[numtel] = new TGraph2D(ECsI_MeV_ZOver2[numtel].size(),ECsI_MeV_ZOver2[numtel].data(),ParticleID_ZOver2[numtel].data(),ECsI_Ch_ZOver2[numtel].data());
		TF2* fFitHeavyIOn = new TF2("fFitHeavyIOn",this,&CSHINECsIEnergyCali::FitFunc_HeavyIon,0,500,100,700,3);
		fFitHeavyIOn->SetParameters(1.,100,1.);
		gr2D_ZOver2[numtel]->Fit("fFitHeavyIOn");

		FileOut<<setw(5)<<numtel<<setw(15)<<ssdindex<<setw(15)<<csiindex<<setw(20)
					 <<fFitHeavyIOn->GetParameter(0)<<setw(15)<<fFitHeavyIOn->GetParameter(1)<<setw(15)
					 <<fFitHeavyIOn->GetParameter(2)<<setw(15)<<endl;

		for (Int_t ip=0; ip<Charge[numtel].size(); ip++) {
			if (Charge[numtel][ip]>=2) { // for Z>=2
				 TF1* fDrawHeavyIon = new TF1("fDrawHeavyIon",this,&CSHINECsIEnergyCali::DrawFunc_HeavyIon,0,500,5);
				 fDrawHeavyIon->SetParameters(fFitHeavyIOn->GetParameters());
				 fDrawHeavyIon->SetParameter(3,(Double_t)Charge[numtel][ip]);
				 fDrawHeavyIon->SetParameter(4,(Double_t)Mass[numtel][ip]);
				 fDrawHeavyIon->SetLineColor(colors[Charge[numtel][ip]-1]);
				 fDrawHeavyIon->Draw("SAME");
			}
		}
		cans[numtel]->Print(pathPDFOut.c_str());
	}
	c_end->Print(pathPDFOutEnd.c_str());
}



//______________________________________________________________________________
// 自定义函数, 用于拟合 p,d,t
// NIMA 929,162(2019), 使用公式如下：
//  Ch = par[0]+par[1]*E^{(par[2]+A)/(par[3]+A)}
Double_t CSHINECsIEnergyCali::FitFunc_Hydrogen(Double_t* x, Double_t* par)
{
	// if (par[2]>=par[3]) return -1;
	// x[0]=E,x[1]=100*Z+A
	if (x[1]>100) {
		Int_t Z = (Int_t) x[1]/100;
		Int_t A = (Int_t) x[1]%100;
    Double_t exponent = (A+par[2])/(A+par[3]);
    Double_t light = par[0]+par[1]*pow(x[0],exponent);
    return light;
	}
	return -99.;
}

Double_t CSHINECsIEnergyCali::DrawFunc_Hydrogen(Double_t* x, Double_t* par)
{
	// x[0]=E,x[1]=100*Z+A
	Int_t Z = (Int_t) par[4];
	Int_t A = (Int_t) par[5];
  Double_t exponent = (A+par[2])/(A+par[3]);
  Double_t light = par[0]+par[1]*pow(x[0],exponent);
  return light;
}

Double_t CSHINECsIEnergyCali::Func4_Hydrogen(DEEFITParticle& p, Double_t* par)
{
	Int_t Z = (Int_t) p.Z;
	Int_t A = (Int_t) p.A;
	Double_t EMeV = p.E;
	Double_t exponent = (A+par[2])/(A+par[3]);
  Double_t light = par[0]+par[1]*pow(EMeV,exponent);
  return light;
}

//______________________________________________________________________________
// 对重离子(Z>=2), 采用 Horn-fit
// D.Horn et al, NIM A320(1992) 273-276
Double_t CSHINECsIEnergyCali::FitFunc_HeavyIon(Double_t* x, Double_t* par)
{
	// x[0]=E, x[1]=10*Z+A
	if(x[1]>200) {
	  Int_t Z = (Int_t) x[1]/100;
    Int_t A = (Int_t) x[1]%100;
    Double_t consterm = par[2]*A*pow(Z,2);
    Double_t lineterm = x[0]+consterm;
    Double_t logterm = consterm*log(abs(lineterm/consterm));
    Double_t light = par[0]+par[1]*(x[0]-logterm);
    return light;
	}
	else { return -99.; }
}

Double_t CSHINECsIEnergyCali::DrawFunc_HeavyIon(Double_t* x, Double_t* par)
{
	// x[0]=E, x[1]=10*Z+A
	Int_t Z = (Int_t) par[3];
  Int_t A = (Int_t) par[4];
  Double_t consterm = par[2]*A*pow(Z,2);
  Double_t lineterm = x[0]+consterm;
  Double_t logterm = consterm*log(abs(lineterm/consterm));
  Double_t light = par[0]+par[1]*(x[0]-logterm);
  return light;
}

Double_t CSHINECsIEnergyCali::Func3_HeavyIon(DEEFITParticle& p, Double_t* par)
{
	// x[0]=E, x[1]=10*Z+A
	Int_t Z = (Int_t) p.Z;
  Int_t A = (Int_t) p.A;
	Double_t EMeV = p.E;
  Double_t consterm = par[2]*A*pow(Z,2);
  Double_t lineterm = EMeV+consterm;
  Double_t logterm = consterm*log(abs(lineterm/consterm));
  Double_t light = par[0]+par[1]*(EMeV-logterm);
  return light;
}



//______________________________________________________________________________
Bool_t IsInsideArray(Int_t charge, Int_t mass, vector<Int_t> arr_charge, vector<Int_t> arr_mass)
{
	Bool_t isinside = kFALSE;
	for (Int_t ip=0; ip<arr_charge.size(); ip++) {
		if ((charge==arr_charge[ip]) && (mass==arr_mass[ip])) {
			isinside = kTRUE;
			return isinside;
		}
	}
	return isinside;
}

void CSHINECsIEnergyCali::CheckCsIEnergyCaliResults()
{
	Int_t NPars_Z1 = 4;
	Int_t NPars_ZOver2 = 3;

	Int_t NPoints = 100;
	Double_t xmin = 0.;
	Double_t xmax = 500.;

  Double_t ECh_Cut = 150.;
	Double_t ECsI_MeV;
	Double_t ECsI_MeV_1;
	Double_t ECsI_Ch;

	Double_t epsilon = 1.E-10;

	Double_t EL3_Range[4] = {300.,250.,250.,250.};
	Int_t EL3_NBins   [4] = {3000,2500,2500,2500};
	Double_t EL2_Range[4] = {250.,250.,140.,70.};
	Int_t EL2_NBins   [4] = {2500,2500,1400,700};

	std::vector<Int_t> Charge[NUM_SSD*NUM_CSI];
	std::vector<Int_t> Mass[NUM_SSD*NUM_CSI];

	Double_t mass_cut = 0.4;
	Int_t  iCharge  = -99;  // 电荷数理论值
	Double_t dMass  = -99.; // 质量数计算值

	Double_t dCharge; // 电荷数计算值
	Int_t    iMass; // 质量数理论值

	Double_t CsIEnergyCaliPars_Z1[NUM_SSD*NUM_CSI][NPars_Z1];
	Double_t CsIEnergyCaliPars_ZOver2[NUM_SSD*NUM_CSI][NPars_ZOver2];

  std::string pathECsIRange(Form("%sdata_CsIEnergyCali/ECsIRange.dat",PATHDATAFOLDER));
  std::string pathDEEFITPars(Form("%sDEEFITData/Fitparam_table_All.out",PATHROOTFILESFOLDER));
  std::string pathDEEFITData(Form("%sDEEFITData/DEEFITData_Run%04d-%04d.root",PATHROOTFILESFOLDER,fFirstRun,fLastRun));
	std::string pathCsIFitPars_Z1(Form("%sdata_CsIEnergyCali/CsIEnergyFit_Z1.data",PATHDATAFOLDER));
	std::string pathCsIFitPars_ZOver2(Form("%sdata_CsIEnergyCali/CsIEnergyFit_ZOver2.data",PATHDATAFOLDER));
	std::string pathDEECali(Form("%sfigure_CsIEnergyCali/DEECali_Run%04d-%04d.pdf",PATHFIGURESFOLDER,fFirstRun,fLastRun));
	std::string pathDEECaliBegin(Form("%sfigure_CsIEnergyCali/DEECali_Run%04d-%04d.pdf[",PATHFIGURESFOLDER,fFirstRun,fLastRun));
	std::string pathDEECaliEnd(Form("%sfigure_CsIEnergyCali/DEECali_Run%04d-%04d.pdf]",PATHFIGURESFOLDER,fFirstRun,fLastRun));

	TH2D* h_DEE[NUM_SSD*NUM_CSI];
	TH2D* h_DEE_Cali[NUM_SSD*NUM_CSI];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		Int_t ssdnum = numtel/NUM_CSI;
		Int_t csinum = numtel%NUM_CSI;
		h_DEE[numtel]      = new TH2D(Form("DEE_SSD%d_CsI%d",ssdnum+1,csinum),Form("DEE_SSD%d_CsI%d",ssdnum+1,csinum), 4000,0,4000,EL2_NBins[ssdnum],0,EL2_Range[ssdnum]);
		h_DEE_Cali[numtel] = new TH2D(Form("CaliDEE_SSD%d_CsI%d",ssdnum+1,csinum),Form("CaliDEE_SSD%d_CsI%d",ssdnum+1,csinum), EL3_NBins[ssdnum],0,EL3_Range[ssdnum],EL2_NBins[ssdnum],0,EL2_Range[ssdnum]);
	}

  // 按行读取 CsI 能量拟合参数
	ifstream FileInRange(pathECsIRange.c_str());
	ifstream FileInPars_Z1(pathCsIFitPars_Z1.c_str());
  ifstream FileInPars_ZOver2(pathCsIFitPars_ZOver2.c_str());

	//___________________________________________________________
	// 提取电荷数 Z 与质量数 A
	if (!FileInRange.is_open()) {
		printf("Error: file %s not found\n",pathECsIRange.c_str());
    return;
	}
	while (FileInRange.good()) {
		// 按行读取数据
    std::string LineRead;
    std::getline(FileInRange, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

		Int_t numtel;
		Int_t charge, mass;
		LineStream>>numtel>>charge>>mass;

	  Charge[numtel].push_back(charge);
		Mass[numtel].push_back(mass);

	}
	FileInRange.close();

	// 读取拟合参数: for Z = 1
	if (!FileInPars_Z1.is_open()) {
		printf("Error: file %s not found\n",pathCsIFitPars_Z1.c_str());
    return;
	}
	while (FileInPars_Z1.good()) {
		// 按行读取数据
    std::string LineRead;
    std::getline(FileInPars_Z1, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

		Int_t numtel;
		Int_t ssdnum, csinum;
		LineStream>>numtel>>ssdnum>>csinum;
		for (Int_t i=0; i<NPars_Z1; i++)  { LineStream>>CsIEnergyCaliPars_Z1[numtel][i]; }
	}
	FileInPars_Z1.close();

	// 读取拟合参数: for Z >= 2
	if (!FileInPars_ZOver2.is_open()) {
		printf("Error: file %s not found\n",pathCsIFitPars_ZOver2.c_str());
    return;
	}
	while (FileInPars_ZOver2.good()) {
		// 按行读取数据
    std::string LineRead;
    std::getline(FileInPars_ZOver2, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

		Int_t numtel;
		Int_t ssdnum, csinum;
		LineStream>>numtel>>ssdnum>>csinum;
		for (Int_t i=0; i<NPars_ZOver2; i++) { LineStream>>CsIEnergyCaliPars_ZOver2[numtel][i]; }
	}
	FileInPars_ZOver2.close();
	//_______________________________________________________

  //__________________________________________________________
	// 定义拟合函数
	TSpline3* spline[NUM_SSD*NUM_CSI][6][15];
  TF1* func[NUM_SSD*NUM_CSI][6][15];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		for (Int_t ip=0; ip<Charge[numtel].size(); ip++) {
			Int_t icharge = Charge[numtel][ip];
			Int_t imass   = Mass[numtel][ip];
			fDEEFITParticle.Z = (Double_t) Charge[numtel][ip];
			fDEEFITParticle.A = (Double_t) Mass[numtel][ip];
      // TSpline(TF1* f1) 差值计算
			if (icharge == 1) {
				func[numtel][icharge][imass] = new TF1(Form("func_%d_%d_%d",numtel,icharge,imass),this,&CSHINECsIEnergyCali::DrawFunc_Hydrogen,0,500,6);
        func[numtel][icharge][imass]->SetParameters(CsIEnergyCaliPars_Z1[numtel]);
				func[numtel][icharge][imass]->SetParameter(4,fDEEFITParticle.Z);
				func[numtel][icharge][imass]->SetParameter(5,fDEEFITParticle.A);
			}
			if (icharge > 1) {
				func[numtel][icharge][imass] = new TF1(Form("func_%d_%d_%d",numtel,icharge,imass),this,&CSHINECsIEnergyCali::DrawFunc_HeavyIon,0,500,5);
        func[numtel][icharge][imass]->SetParameters(CsIEnergyCaliPars_ZOver2[numtel]);
				func[numtel][icharge][imass]->SetParameter(3,fDEEFITParticle.Z);
				func[numtel][icharge][imass]->SetParameter(4,fDEEFITParticle.A);
			}
      // 构造 TSpline3
			Double_t ech;
			Double_t emev;
			Double_t ECh[NPoints];
			Double_t EMeV[NPoints];
			for (Int_t i=0; i<NPoints; i++) {
		    emev = xmin + i*(xmax-xmin)/NPoints;
				ech  = func[numtel][icharge][imass]->Eval(emev);
				ECh[i]  = ech;
				EMeV[i] = emev;
			}
			TGraph* gr = new TGraph(NPoints, ECh, EMeV);
			spline[numtel][icharge][imass] = new TSpline3(Form("spline_%d_%d_%d",numtel,icharge,imass), gr);
		}
	}
	//______________________________________________________________

	// 读取 DEEFIT 拟合的 14 参数
  Double_t** DEEFITPars = fDeefit->DEEFITLoadPars(pathDEEFITPars.c_str());

	Short_t  numtel;  // number of csi crystals
  Float_t  desipgf; // dE (MeV)
  Short_t  fastpg;  // ECsI (ADC Chs)

  TFile* myfile = TFile::Open(pathDEEFITData.c_str());
  TTree* mytree = (TTree*) myfile->Get("h1");
	mytree->SetBranchAddress("numtel", &numtel);   // number of CsI
	mytree->SetBranchAddress("desipgf",&desipgf);  // dESi
	mytree->SetBranchAddress("fastpg", &fastpg);   // ECsI

	Long64_t nentries = mytree->GetEntries();
	cout<<"nentries = "<<nentries<<endl;

	for (Long64_t jentry=0; jentry<nentries; jentry++) {
		//if (jentry>10000) continue;
		mytree->GetEntry(jentry);
		timeper.PrintPercentageAndRemainingTime(jentry, nentries);

		iCharge = fDeefit->DEEFITGetCharge(DEEFITPars[numtel], desipgf, fastpg, dCharge); // Int_t charge
	  dMass   = fDeefit->DEEFITGetMass(DEEFITPars[numtel], iCharge, desipgf, fastpg, iMass); // Double_t mass

		ECsI_Ch = (Double_t) fastpg;
		if (ECsI_Ch > ECh_Cut) {
			if (IsInsideArray(iCharge,iMass,Charge[numtel],Mass[numtel])) {
				// ECsI_MeV = func[numtel][iCharge][iMass]->GetX(ECsI_Ch,0,500.,epsilon); // 这种方法简单,但是输出有 bug, 虽然对最后的结果没有影响！
				ECsI_MeV = spline[numtel][iCharge][iMass]->Eval(ECsI_Ch);
				if (ECsI_MeV < 0) continue;
				h_DEE[numtel]->Fill(fastpg, desipgf);
				h_DEE_Cali[numtel]->Fill(ECsI_MeV, desipgf);
			}
		}
	}
  //__________________________________________________

  // 画图
	TCanvas* c_begin = new TCanvas("c_begin","c_begin", 600, 600);
	TCanvas* c_end = new TCanvas("c_end","c_end", 600, 600);
	c_begin->Close();
	c_end->Close();
  TCanvas* cans[NUM_SSD*NUM_CSI];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		Int_t ssdnum = numtel/NUM_CSI;
		Int_t csinum = numtel%NUM_CSI;
		cans[numtel] = new TCanvas(Form("c_SSD%d_CsI%d",ssdnum+1,csinum),Form("c_SSD%d_CsI%d",ssdnum+1,csinum),1200,600);
	}

	c_begin->Print(pathDEECaliBegin.c_str());
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
	  cans[numtel]->Divide(2,1);
		cans[numtel]->cd(1);
		h_DEE[numtel]->Draw("COLZ");
		cans[numtel]->cd(2);
		h_DEE_Cali[numtel]->Draw("COLZ");

		cans[numtel]->Print(pathDEECali.c_str());
	}
  c_end->Print(pathDEECaliEnd.c_str());
}
