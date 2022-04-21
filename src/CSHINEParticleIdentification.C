#include "../include/CSHINEParticleIdentification.h"
using namespace std;

//______________________________________________________________________________
CSHINEDEEFITPID::CSHINEDEEFITPID()
{
	fFirstRun = 100;
  fLastRun  = 110;
}

CSHINEDEEFITPID::CSHINEDEEFITPID(Int_t firstrun, Int_t lastrun) : fChain(0)
{
	fFirstRun = firstrun;
  fLastRun = lastrun;

  std::string pathrootfilein(Form("/home/sea/Fission2019_Data/CSHINEEvent/EventTree_Run%04d-Run%04d.root", fFirstRun, fLastRun));

  TTree* tree = 0;
  TFile* f = (TFile*)gROOT->GetListOfFiles()->FindObject(pathrootfilein.c_str());
  if (!f || !f->IsOpen()) {
    f = new TFile(pathrootfilein.c_str());
  }
  f->GetObject("LayerEvent",tree);
  Init(tree);
}

CSHINEDEEFITPID::~CSHINEDEEFITPID()
{
	if (!fChain) return;
}

void CSHINEDEEFITPID::Init(TTree* tree)
{
  // Set branch addresses and branch pointers
  if (!tree) return;
  fChain = tree;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("LayerEvent.fL1SMulti",    &fLayerEvent.fL1SMulti);
  fChain->SetBranchAddress("LayerEvent.fL1SSSDNum",   &fLayerEvent.fL1SSSDNum);
  fChain->SetBranchAddress("LayerEvent.fL1SNumStrip", &fLayerEvent.fL1SNumStrip);
  fChain->SetBranchAddress("LayerEvent.fL1SEMeV",     &fLayerEvent.fL1SEMeV);
  fChain->SetBranchAddress("LayerEvent.fL2FMulti",    &fLayerEvent.fL2FMulti);
  fChain->SetBranchAddress("LayerEvent.fL2FSSDNum",   &fLayerEvent.fL2FSSDNum);
  fChain->SetBranchAddress("LayerEvent.fL2FNumStrip", &fLayerEvent.fL2FNumStrip);
  fChain->SetBranchAddress("LayerEvent.fL2FEMeV",     &fLayerEvent.fL2FEMeV);
	fChain->SetBranchAddress("LayerEvent.fL2FTime",     &fLayerEvent.fL2FTime);
  fChain->SetBranchAddress("LayerEvent.fL2BMulti",    &fLayerEvent.fL2BMulti);
  fChain->SetBranchAddress("LayerEvent.fL2BSSDNum",   &fLayerEvent.fL2BSSDNum);
  fChain->SetBranchAddress("LayerEvent.fL2BNumStrip", &fLayerEvent.fL2BNumStrip);
  fChain->SetBranchAddress("LayerEvent.fL2BEMeV",     &fLayerEvent.fL2BEMeV);
  fChain->SetBranchAddress("LayerEvent.fCsIMulti",    &fLayerEvent.fCsIMulti);
  fChain->SetBranchAddress("LayerEvent.fCsISSDNum",   &fLayerEvent.fCsISSDNum);
  fChain->SetBranchAddress("LayerEvent.fCsINum",      &fLayerEvent.fCsINum);
  fChain->SetBranchAddress("LayerEvent.fCsIECh",      &fLayerEvent.fCsIECh);
  fChain->SetBranchAddress("LayerEvent.fSSDL1SMulti", &fLayerEvent.fSSDL1SMulti);
  fChain->SetBranchAddress("LayerEvent.fSSDL2FMulti", &fLayerEvent.fSSDL2FMulti);
  fChain->SetBranchAddress("LayerEvent.fSSDL2BMulti", &fLayerEvent.fSSDL2BMulti);
  fChain->SetBranchAddress("LayerEvent.fSSDCsIMulti", &fLayerEvent.fSSDCsIMulti);
}
//______________________________________________________________________________



//______________________________________________________________________________
// 在本次实验中, 将借助 DEEFIT 工具进行 CsI 晶体的能量刻度
// 由于 DEEFIT 对数据格式有要求, 因此首先需要将实验数据存成 DEEFIT 格式： 3个branch
// tree->SetBranch("numtel", &numtel, "numtel/s");
// tree->SetBranch("desipgf",&desipgf,"desipgf/f");
// ree->SetBranch("fastpg",  &fastpg, "fastpg/s");
void CSHINEDEEFITPID::DEEFITGenerateData_L2L3()
{
  std::string pathrootfileout(Form("/home/sea/Fission2019_Data/L2L3_DEEFITData_Run%04d-%04d.root", fFirstRun, fLastRun));
	std::string pathHistFileOut(Form("/home/sea/Fission2019_Data/L2L3_HistData_Run%04d-%04d.root", fFirstRun, fLastRun));

	TH2D* h2d[NUM_SSD*NUM_CSI];
	for (Int_t i=0; i<NUM_SSD*NUM_CSI; i++) {
		h2d[i] = new TH2D(Form("h2_dE2_ECsI_Tel%02d",i),Form("h2_dE2_ECsI_Tel%02d",i),4000,0,4000,4000,0,400);
	}

	std::vector<Double_t> L2L3_EL2F;
  std::vector<Double_t> L2L3_EL2B;
  std::vector<Double_t> L2L3_ECSI;
  std::vector<Int_t>    L2L3_StripL2F;
  std::vector<Int_t>    L2L3_StripL2B;
  std::vector<Int_t>    L2L3_NumCsI;

 // 新建 root 文件
  TFile* newfile = new TFile(pathrootfileout.c_str(), "RECREATE");
  if (!newfile || !newfile->IsOpen()) {
    cout<<Form("File %s not found.\n", pathrootfileout.c_str());
    return;
  }
 // 创建 DEEFIT tree
  fdeefitdata = new DEEFITTreeData;
  TTree* newtree = new TTree("h1", "DEEFIT tree for Fission2019");
  newtree->Branch("numtel", &fdeefitdata->numtel, "numtel/S"); // number of CsI
  newtree->Branch("desipgf",&fdeefitdata->desipgf,"desipgf/F");// dESi
  newtree->Branch("fastpg", &fdeefitdata->fastpg, "fastpg/S"); // ECsI

  Long64_t nentries = fChain->GetEntries();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    fChain->GetEntry(ientry);
		timeper.PrintPercentageAndRemainingTime(ientry, nentries);

		for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			if (fLayerEvent.fSSDCsIMulti[ssdindex]==1 && fLayerEvent.fSSDL2BMulti[ssdindex]==1 &&
	        fLayerEvent.fSSDL2FMulti[ssdindex]==1)
	    {
	      // 提取 CsI 的数据
	      for (Int_t csimulti=0; csimulti<fLayerEvent.fCsIMulti; csimulti++) {
	        if (fLayerEvent.fCsISSDNum[csimulti] == ssdindex) {
	          L2L3_NumCsI.push_back(fLayerEvent.fCsINum[csimulti]);
	          L2L3_ECSI.push_back(fLayerEvent.fCsIECh[csimulti]);
	        }
	      }
	      // 提取 L2B 的数据
	      for (Int_t l2bmulti=0; l2bmulti<fLayerEvent.fL2BMulti; l2bmulti++) {
	        if (fLayerEvent.fL2BSSDNum[l2bmulti] == ssdindex) {
	         L2L3_StripL2B.push_back(fLayerEvent.fL2BNumStrip[l2bmulti]);
	         L2L3_EL2B.push_back(fLayerEvent.fL2BEMeV[l2bmulti]);
	        }
	      }
	      // 提取 L2F 的数据
	      for (Int_t l2fmulti=0; l2fmulti<fLayerEvent.fL2FMulti; l2fmulti++) {
	        if (fLayerEvent.fL2FSSDNum[l2fmulti] == ssdindex) {
	          L2L3_StripL2F.push_back(fLayerEvent.fL2FNumStrip[l2fmulti]);
	          L2L3_EL2F.push_back(fLayerEvent.fL2FEMeV[l2fmulti]);
	        }
	      }
	      // 填充 hist_L2B_L3
	      if (fPattern.IsGeoConstraint_L3A_L2B(L2L3_NumCsI[0], L2L3_StripL2B[0]) &&
	          fPattern.IsGeoConstraint_L3A_L2F(L2L3_NumCsI[0], L2L3_StripL2F[0]) &&
	          fPattern.IsEneConstraint_L2B_L2F(ssdindex, L2L3_EL2B[0], L2L3_EL2F[0]))
	      {
					fdeefitdata->numtel  = (UShort_t) ssdindex*9 + L2L3_NumCsI[0];
		      fdeefitdata->desipgf = (Float_t)  L2L3_EL2F[0];
		      fdeefitdata->fastpg  = (UShort_t) L2L3_ECSI[0];

					h2d[fdeefitdata->numtel]->Fill(L2L3_ECSI[0], L2L3_EL2F[0]);
	      }
				newtree->Fill();

	      L2L3_EL2F.clear();
	      L2L3_EL2B.clear();
	      L2L3_ECSI.clear();
	      L2L3_StripL2F.clear();
	      L2L3_StripL2B.clear();
	      L2L3_NumCsI.clear();
	    }
		}
  }
  newfile->Write();
  newfile->Close();

	// 新建 root 文件
	 TFile* Histroot = new TFile(pathHistFileOut.c_str(), "RECREATE");
	 if (!Histroot || !Histroot->IsOpen()) {
		 cout<<Form("File %s not found.\n", pathHistFileOut.c_str());
		 return;
	 }
	 for (Int_t i=0; i<NUM_SSD*NUM_CSI; i++) {
		 Histroot->WriteTObject(h2d[i], h2d[i]->GetName());
	 }
	 Histroot->Close();
}



//______________________________________________________________________________
// 根据 StripB, StripF 对硅条表面分区域
Int_t CSHINEDEEFITPID::GetZone(Int_t stripb, Int_t stripf, Int_t ZoneLength)
{
	Int_t num_megded_chs = NUM_STRIP/ZoneLength;
	Int_t zone_x = std::abs(stripb/num_megded_chs - (ZoneLength-1));
	Int_t zone_y = std::abs(stripf/num_megded_chs - (ZoneLength-1));
	return (ZoneLength*zone_x+zone_y);
}


//______________________________________________________________________________
// 发现: 对于 L1-L2, DEEFIT 公式偏差较大！
void CSHINEDEEFITPID::DEEFITGenerateData_L1L2()
{
  std::string pathrootfileout(Form("/home/sea/Fission2019_Data/L1L2_DEEFITData_Run%04d-%04d.root", fFirstRun, fLastRun));
  std::string pathHistFileOut(Form("/home/sea/Fission2019_Data/L1L2_HistData_Run%04d-%04d.root", fFirstRun, fLastRun));

	Double_t EL1Range[4] = {500., 500., 300., 300.};
	Int_t    NBinsL1[4]  = {5000, 5000, 3000, 3000};
	Double_t EL2Range[4] = {300., 250., 150., 80.};
  Int_t    NBinsL2[4]  = {3000, 2500, 1500, 800};

  Int_t zone_Length = 8;
	Int_t zone_index;
	Double_t EL1S_Corrected;

	std::vector<Double_t> L1L2_EL1S;
  std::vector<Double_t> L1L2_EL2F;
	std::vector<Double_t> L1L2_EL2B;
  std::vector<Int_t>    L1L2_StripL1S;
  std::vector<Int_t>    L1L2_StripL2F;
	std::vector<Int_t>    L1L2_StripL2B;
	std::vector<Int_t>    L2FTime;

	TH2D* h2d[4];
	for (Int_t i=0; i<4; i++) {
		h2d[i] = new TH2D(Form("h2_dE1_dE2_Tel%02d",i),Form("h2_dE1_dE2_Tel%02d",i),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
	}

 // 新建 root 文件
  TFile* DEEFITroot = new TFile(pathrootfileout.c_str(), "RECREATE");
  if (!DEEFITroot || !DEEFITroot->IsOpen()) {
    cout<<Form("File %s not found.\n", pathrootfileout.c_str());
    return;
  }
 // 创建 DEEFIT tree
  fdeefitdata = new DEEFITTreeData;
  TTree* newtree = new TTree("h1", "DEEFIT tree for Fission2019");
  newtree->Branch("numtel", &fdeefitdata->numtel, "numtel/S"); // number of CsI
  newtree->Branch("desipgf",&fdeefitdata->desipgf,"desipgf/F");// dE1
  newtree->Branch("fastpg", &fdeefitdata->fastpg, "fastpg/F"); // EL2F

  Long64_t nentries = fChain->GetEntries();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    fChain->GetEntry(ientry);
	//	if (ientry>100) continue;
		timeper.PrintPercentageAndRemainingTime(ientry, nentries);

		for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			if (fLayerEvent.fSSDL1SMulti[ssdindex]==1 && fLayerEvent.fSSDL2FMulti[ssdindex]==1 &&
	        fLayerEvent.fSSDL2BMulti[ssdindex]==1 && fLayerEvent.fSSDCsIMulti[ssdindex]==0)
	    {
				L1L2_EL1S.clear();
	      L1L2_EL2F.clear();
	      L1L2_EL2B.clear();
				L1L2_StripL1S.clear();
	      L1L2_StripL2F.clear();
	      L1L2_StripL2B.clear();
				L2FTime.clear();

				// 提取 L1S 的数据
	      for (Int_t l1smulti=0; l1smulti<fLayerEvent.fL1SMulti; l1smulti++) {
	        if (fLayerEvent.fL1SSSDNum[l1smulti] == ssdindex) {
	          L1L2_StripL1S.push_back(fLayerEvent.fL1SNumStrip[l1smulti]);
	          L1L2_EL1S.push_back(fLayerEvent.fL1SEMeV[l1smulti]);
	        }
	      }
				// 提取 L2F 的数据
	      for (Int_t l2fmulti=0; l2fmulti<fLayerEvent.fL2FMulti; l2fmulti++) {
	        if (fLayerEvent.fL2FSSDNum[l2fmulti] == ssdindex) {
	          L1L2_StripL2F.push_back(fLayerEvent.fL2FNumStrip[l2fmulti]);
	          L1L2_EL2F.push_back(fLayerEvent.fL2FEMeV[l2fmulti]);
						L2FTime.push_back(fLayerEvent.fL2FTime[l2fmulti]);
	        }
	      }
	      // 提取 L2B 的数据
	      for (Int_t l2bmulti=0; l2bmulti<fLayerEvent.fL2BMulti; l2bmulti++) {
	        if (fLayerEvent.fL2BSSDNum[l2bmulti] == ssdindex) {
	         L1L2_StripL2B.push_back(fLayerEvent.fL2BNumStrip[l2bmulti]);
	         L1L2_EL2B.push_back(fLayerEvent.fL2BEMeV[l2bmulti]);
	        }
	      }

	      // 填充 hist_L1_L2F
	      if (fPattern.IsEneConstraint_L2B_L2F(ssdindex, L1L2_EL2B[0], L1L2_EL2F[0]) /*&& (L1L2_StripL1S[0]==L1L2_StripL2B[0])*/)
	      {
					if (L1L2_StripL1S[0]==L1L2_StripL2B[0]) {
					  if (L2FTime[0]>SiTimeCut_Low[ssdindex][L1L2_StripL2F[0]] && L2FTime[0]<SiTimeCut_Up[ssdindex][L1L2_StripL2F[0]]) {
							// 考虑 SSD3, ssd4 L1S 的厚度修正
							zone_index = GetZone(L1L2_StripL2B[0], L1L2_StripL2F[0], zone_Length);
							EL1S_Corrected = L1L2_EL1S[0]*(1+L1S_SITHICKNESSCORRECTION[ssdindex][zone_index]);
							//
              fdeefitdata->numtel  = (UShort_t) ssdindex;
							fdeefitdata->desipgf = (Float_t)  EL1S_Corrected;
					    fdeefitdata->fastpg  = (Float_t)  L1L2_EL2F[0];
						  // 用于 “拉直法” 的 DE-E plot
							h2d[ssdindex]->Fill(L1L2_EL2F[0], EL1S_Corrected);
            }
						newtree->Fill();
						/*
						cout<<"numtel = "<<fdeefitdata->numtel<<setw(20)<<"L1L2_StripL1S = "<<L1L2_StripL1S[0]
								<<setw(20)<<"L1L2_StripL2B = "<<L1L2_StripL2B[0]
								<<setw(20)<<"L2FTime = "<<L2FTime[0]<<setw(10)<<SiTimeCut_Low[ssdindex][L1L2_StripL2F[0]]
								<<setw(10)<<SiTimeCut_Up[ssdindex][L1L2_StripL2F[0]]
								<<endl; */
					}
	      }
	    }
		}
  }
  DEEFITroot->Write();
  DEEFITroot->Close();

	// 新建 root 文件
   TFile* Histroot = new TFile(pathHistFileOut.c_str(), "RECREATE");
   if (!Histroot || !Histroot->IsOpen()) {
     cout<<Form("File %s not found.\n", pathHistFileOut.c_str());
     return;
   }
	 for (Int_t i=0; i<4; i++) {
     Histroot->WriteTObject(h2d[i], h2d[i]->GetName());
	 }
	 Histroot->Close();
}

//______________________________________________________________________________
// DEEFIT 是意大利国家核物理研究院 INFN-LNS 为 CHIMERA 探测器开发的一套
// 用于硅望远镜粒子鉴别的工具
// DEEFIT 的源文件请参考 DEEFIT/ 文件夹下的：
// ** MDEEFrame.h && MDEEFrame.C ： DEEFIT 的框架与主体
// ** MTree.h && MTree.C : DEFFIT 输入数据的结构要求
void CSHINEDEEFITPID::DEEFITRunCode()
{
  system("cd DEEFIT/ && ./deefit_S");
}


//——————————————————————————————————————————————————————————————————————————————
//           以下函数都是从 DEFFIT 程序中直接移植过来的
//        ------------------------------------------
// Double_t   DEEFITFunc14(DEEFITParticle& p, Double_t* par)
// Double_t **LoadFitParam(const char* name);
// Int_t      Get_Charge(Int_t ntel, Double_t de, Double_t fast, Int_t* iter, Double_t* zeta, Double_t* par);
// Double_t   Get_Mass(Int_t ntel, Int_t charge, Double_t de, Double_t fast, Int_t* iter, Double_t* par);

//______________________________________________________________________________
//              定义 DEEFIT 采用的拟合函数 func14
// Function for DE-E identification based on formulas from
//   *** N. Le Neindre et al. NIM A490 (2002) 251 ***
// modified following the formula of W. Gawlikowicz for Light-Energy relation in CsI(Tl)
//   *** W. Gawlikowicz, NIM A491 (2002) 181. ***
// Adapted by Amalia Pop following ref.:
//   *** J. Blicharska et al. LNS report 2005 ***
// This formula needs 14 fit parameters
Double_t CSHINEDEEFITPID::DEEFITFunc14(DEEFITParticle& p, Double_t* par)
{
  Double_t X = p.E;
  Double_t Z = p.Z;
  Double_t A = p.A;
  Double_t xx, exp1, exp2, res=1.0;
  Double_t reslog, ene;

  if(Z==0 && A==0) return par[12];

  xx = X-par[13];
  bool foffset = false; //this is true for fitting procedure, false for the analysis
  //if xx<0 never reached from data analysis
  if(xx<=0) {
    if(foffset) xx = X;
    else return res;
  }

  exp1   = par[2] + par[3] + 1;
  exp2   = 1./exp1;
  reslog = log(1.+ par[9]*Z)*(1. - exp(-par[10]*pow(Z,par[11])))/Z;
  ene    = par[6]*reslog*xx ;
  ene    = ene + par[7]*Z*sqrt(A)*log(1. + par[8]*reslog*xx);
  res    = pow(ene,exp1)
         + pow((par[4]*pow(Z,par[0])*pow(A,par[1])),exp1)
         + par[5]*pow(Z,2)*pow(A,par[2])*pow(ene,par[3]);

  res    = pow(res,exp2) - ene + par[12];
  return res;
}


//______________________________________________________________________________
// 读取 DEFFIT 中 Func14 的拟合参数
// 拟合采用的是 14 参数的公式， 因此参数有 14 个
// LoadDEEFITPars() 由 DEEFIT 中函数 LoadFitParam（）变形而来
//     void MDEEFrame::LoadFitParam(const char *name)
Double_t** CSHINEDEEFITPID::DEEFITLoadPars(const char* pathParsFile)
{
  Int_t    ntel, np, lm;
  Double_t param[fMAXFP], chi2;
  Int_t    km;  // 源程序中, ecalcmass km; 没搞清楚 ecalcmass 是什么类型; 这里用 Int_t 代替

  Double_t** fvpar = NULL;
	fvpar = new Double_t* [NUM_SSD*NUM_CSI]; // four Si-Si-CsI telescopes, 36 CsI crystals in total
  for (Int_t i=0; i<NUM_SSD*NUM_CSI; i++) {
    fvpar[i] = new Double_t[fMAXFP];
	}

  ifstream in(pathParsFile, ios::in);
  if (in) {
    while(!in.eof()) {
			// 这里的按行读取数据的方式与 DEEFIT 元代码稍有不同！ 但不影响正常使用！
			std::string LineRead;
      std::getline(in, LineRead);
			LineRead.assign(LineRead.substr(0, LineRead.find('*')));
      if(LineRead.empty()) continue;
      if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
      std::istringstream LineStream(LineRead);

      LineStream>>ntel>>lm>>np;
      km = (Int_t) lm;
      if(!LineStream.fail()) {
        for(Int_t i=0; i<np; i++) {
          LineStream>>param[i];
          fvpar[ntel][i] = param[i];
        }
        LineStream>>chi2;
      }
    }
    cout<<"Parameter table value loaded from file: "<<pathParsFile<<endl;
    in.close();
  }
  return fvpar;
}


//______________________________________________________________________________
Int_t CSHINEDEEFITPID::DEEFITGetCharge(Double_t* par, Double_t de, Double_t fast, Double_t& zeta)
{
	const Int_t maxiter = 500;
	const Int_t zlim = 6; // 设定 Z 的寻找范围,可根据实验数据实际情况进行修改
  Int_t    cres  = 1, izeta;
  Double_t zmin  = 1, zmax = zlim, ztest, atest = 0, yy, dist;
  Double_t amass = 0, amassp = 0, amassm = 0;
  Bool_t   found = kFALSE;
  DEEFITParticle p;

  // 二分法迭代求解电荷数 Z
  Int_t iter = 0;
  zeta = 0.0;
  while(iter < maxiter) {
    iter++;
    ztest = (zmin + zmax)/2.0;  // 二分法查找最接近的电荷数 Z

	  atest = 2.0*ztest;          // GetCharge 时,假定原子核为 N=Z， A=2*Z
	  if(atest == 1) atest = 2.;  // 这里将源代码化简了,源代码中进行了分类讨论,使用
		                            // GetEMass((Int_t)(ztest+0.5)),GetMassCharity(Int_t(ztest+0.5));
    p.E = fast;
    p.Z = ztest;
    p.A = atest;
    yy = DEEFITFunc14(p, par);   // 给定 E(CsI),计算 yy==dE(Si)
    if(yy == 1.0) {
	    cres = 0;
	    return cres;
    }
    if((Int_t)(zmin + 0.5) == (Int_t)(zmax + 0.5)) { //如果 |zmin-zmax|<1,则表示找到了 Z
	    found = kTRUE;
	    break;
    }
    if((de - yy) >= 0) { // 若实验点>计算点(即实验点在计算点上方),则另 zmin = ztest，即在上半段继续寻找合适的 Z
	    zmin = ztest;
	    continue;
    }
		else { // 若实验点<计算点(即实验点在计算点下方),则另 zmax = ztest，即在下半段继续寻找合适的 Z
	    zmax = ztest;
	    continue;
    }
  } // 迭代结束

  if(!found) { // 没找到合适的 Z
    cout<<"Get_Charge>> Convergenze not reached"<<endl;
  }

  cres = (Int_t)(zmin + 0.5);
  izeta = cres;
  if(cres > zlim) {
    zeta = izeta;
    return cres; // 如果找到的 (Int_t)(zmin + 0.5) 超出预定范围 zlim, 则返回当前计算值
  }              // 否则,将进行下一步判断

  //___________________________________________
  // 根据前面得到的 cres(Z), 计算 Z 弥散 *zeta
  // dispersion around mean charge value
	amass  = 2.0*izeta;
	amassp = 2.0*(izeta + 1);
	amassm = 2.0*(izeta - 1);
  p.E = fast;
  p.Z = izeta;
  p.A = amass;
  yy = DEEFITFunc14(p, par);
  dist = de - yy; //计算实验值与Func14()计算值的偏差
  if(dist >= 0.0) {
	  p.Z = izeta + 1;
	  p.A = amassp;  //zeta+1
	  zeta = (Double_t) izeta + dist/(DEEFITFunc14(p, par) - yy); // 计算 izeta 对 Z 的偏离
  }
  else {
	  p.Z = izeta - 1;
	  p.A = amassm;  //zeta-1
	  zeta = (Double_t) izeta + dist/(yy- (izeta>1 ? DEEFITFunc14(p, par) : 0.));
  }
  if(zeta>0 && zeta<0.5) zeta = 0.51; // 对于无意义的情况,初始化 *zeta = 0.51

  return cres;
}


//______________________________________________________________________________
Double_t CSHINEDEEFITPID::DEEFITGetMass(Double_t* par, Int_t charge, Double_t de, Double_t fast, Int_t& imass)
{
	const Int_t maxiter = 1000;
  Bool_t   found = kFALSE;
  Double_t amin, amax;
//  Double_t Amin[] = {0, 2,  4,  6,  7,  9,  11, 13, 14, 17, 19, 21, 22, 23};
//  Double_t Amax[] = {6, 10, 11, 12, 13, 15, 18, 26, 29, 32, 35, 36, 38, 41};
	Double_t Amin[] = {0.5, 2.5, 5.5, 6.5,  7.5,  11.5, 11, 13, 14, 17, 19, 21, 22, 23};
  Double_t Amax[] = {3.5, 6.5, 9.5, 10.5, 13.5, 14.5, 18, 26, 29, 32, 35, 36, 38, 41};
  Double_t mass = 0.0, fmass, yy, atest, dist;
	Double_t dmin = 0.01; // 设定 dE 能量误差范围
  DEEFITParticle p;

	imass = 0;

  Int_t iter = 0;
  amin = Amin[charge-1];
  amax = Amax[charge-1];

  p.E = fast;
  p.Z = charge;

	// 二分法迭代求解质量 (Double_t) A
  while(iter < maxiter) {
    iter++;
    atest = (amax + amin)/2.0;
    p.A = atest;
    yy = DEEFITFunc14(p, par);
    if(yy == 1.0) return 0.0;

    dist = de - yy;
    if(fabs(dist) <= dmin) { // 如果实验值与计算值小于设定误差,则表明找到了合适的 A
      found = kTRUE;
      break;
    }
    if((de - yy) >= 0) { // 在上半段继续寻找 A
      amin = atest;
      continue;
    }
    else { // 在下半段继续寻找 A
      amax = atest;
      continue;
    }
  }  // 迭代结束

  if(!found) { // 找不到合适的 A
    if(amin == amax) return 0.0;
    else {
      if(charge > 1) {
        cout<<"Get_Mass>> Warning: charge convergenze failed:  Z="<<charge<<endl;
      }
    }
  }
  imass = (Int_t)(atest + 0.5);
  mass = imass;

  p.A = mass;
  yy = DEEFITFunc14(p, par);
  if(yy == 1.0) return 0.0;
  if(mass == 0) return 0.0;

 //_________________________________________________
 // 根据带弥散的质量 A
 // calculate dispersion around mean mass value
  dist = de - yy;
  if(dist >= 0.0) {
    p.A = mass + 1.;  // A + 1
    fmass = mass + dist/(DEEFITFunc14(p, par) - yy);
  }
  else {
    p.A = mass - 1.;  // A - 1
    fmass = mass + dist/(yy - DEEFITFunc14(p, par));
  }
  if((Int_t)(fmass + 0.5) != (Int_t)(mass + 0.5)) {   // 如果 fmass 与 mass 不靠近同一个整数
    if(dist > 0) fmass = (Int_t)(mass + 0.5) + 0.501; // 实验点落在 A,A+1 的中点附近
    else fmass = (Int_t)(fmass + 0.5) + 0.499;        // 实验点落在 A-1，A的中点附近
  }
  if((fmass > 0) && (fmass < 0.5)) fmass = 0.51; // 对于无意义的情况,初始化 fmass = 0.51
  if(fmass < 0) fmass = 0.51; // 对于无意义的情况,初始化 fmass = 0.51

  return fmass;
}


//______________________________________________________________________________
CSHINEStraighteningPID::CSHINEStraighteningPID()
{}

CSHINEStraighteningPID::~CSHINEStraighteningPID()
{}


//______________________________________________________________________________
void CSHINEStraighteningPID::StraighteningBuildDEEPlot()
{}


//______________________________________________________________________________
// 对不同的同位素作 Cut
void CSHINEStraighteningPID::DoBananaCut()
{
	Int_t MAXPOINTS = 50;
	Int_t charge, mass;

	std::string pathROOTInput(Form("%sStraighteningData/L1L2_HistData_Run0150-0400.root",PATHROOTFILESFOLDER));
	std::string pathROOTCutOut(Form("%sStraighteningData/L1L2DEECuts.root",PATHROOTFILESFOLDER));
	std::string DEEHistName("h2_dE1_dE2");

	TCanvas* cans = new TCanvas("cans","cans",800,600);

	TFile* FileIn  = new TFile(pathROOTInput.c_str(), "READONLY");
	TFile* FileOut = new TFile(pathROOTCutOut.c_str(), "UPDATE");

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
// 用传统的方法进行粒子鉴别, 手动选点，拟合, 保存拟合参数, 以用于后续的粒子鉴别
// 需要根据实际情况修改 ExtractDEEPointsGUI/DoMarker.C
// 修改root文件路径: Cut_analyser->Initial_RootFile("dE1_dE2_Spec.root");
// 修改 Maker 输出文件路径/名称: Cut_analyser->Set_CutFile_Name("../data/data_PID/Cut_SingleCh.data", "h2_dE1_dE2");
// 修改输出拟合参数文件路径/名称: Cut_analyser->Set_FitParsFile_Name("../data/data_PID/FitPars.dat");
void CSHINEStraighteningPID::StraighteningExtractPointsAndFit()
{
	system("cd ExtractDEEPointsGUI/ && root -l DoMarker.C");
}


//______________________________________________________________________________
// 将 PID 带子"拉直"的方法策略如下:
//  1.根据 dE(Exp) 与 dE(Calc) 判断粒子所属带子范围, 其中每条带子的 Z, A 已知.
//  2.根据已知的 Z, A 标定 PID 数： (Double_t) PID = Z + 0.2*(A-2*Z)
//  3.根据实验点到标定刻度线的距离, 计算实验点的 PID 数
// **** 对应于对称核, PID = Z, 以此为标准，计算每个实验点的 PID 数
void CSHINEStraighteningPID::StraighteningGetExpPIDNumber(const char* pathRootFile)
{
	Int_t Num_of_Particle[100]    = {-99};      // [tel]
	Int_t Z_charge[100][100]      = {{-99}};    // [tel][partilce]
	Int_t A_mass[100][100]        = {{-99}};    // [tel][partilce]
	Double_t fitpars[100][100][8] = {{{-99.}}}; // [tel][particle][pars], fit_func = [0]/x + pol6

  Int_t TeleIndex = -99;
	Int_t NParticles = -99; // number of particles of a DEE plot
	Double_t Calc_dE1[100] = {-99.};
	Double_t PID_Num = -99.;

  // 读取拟合参数
	ifstream in(pathRootFile, ios::in);
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

  // 读取 tree 数据
	// 需要很具实际的 tree 数据结构来修改 !!!
	TFile* file_in = new TFile(pathRootFile, "READONLY");
	if (!file_in->IsOpen()) {
    cout<<"Open file failded."<<endl;
    return;
  }

	Int_t                   TrackEvent_fGlobalMulti;
  std::vector<Int_t>      TrackEvent_fSSDGlobalMulti;
  std::vector<Int_t>      TrackEvent_fGSSDNum;
  std::vector<Int_t>      TrackEvent_fGL1SNumStrip;
  std::vector<Double_t>   TrackEvent_fGL1SEMeV;
  std::vector<Int_t>      TrackEvent_fGL2FNumStrip;
  std::vector<Double_t>   TrackEvent_fGL2FEMeV;
  std::vector<Int_t>      TrackEvent_fGL2BNumStrip;
  std::vector<Double_t>   TrackEvent_fGL2BEMeV;
  std::vector<Int_t>      TrackEvent_fGCsINum;
  std::vector<Int_t>      TrackEvent_fGCsIECh;
  TTree* mytree = (TTree*) file_in->Get("TrackEvent");
  Long64_t nentries = mytree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  mytree->SetMakeClass(1);  // 如果 tree 的 branch 使用了自定义的类, 则这条语句不能省略！！！
  mytree->SetBranchAddress("TrackEvent.fGlobalMulti",    &TrackEvent_fGlobalMulti);
  mytree->SetBranchAddress("TrackEvent.fSSDGlobalMulti", &TrackEvent_fSSDGlobalMulti);
  mytree->SetBranchAddress("TrackEvent.fGSSDNum",        &TrackEvent_fGSSDNum);
  mytree->SetBranchAddress("TrackEvent.fGL1SNumStrip",   &TrackEvent_fGL1SNumStrip);
  mytree->SetBranchAddress("TrackEvent.fGL1SEMeV",       &TrackEvent_fGL1SEMeV);
  mytree->SetBranchAddress("TrackEvent.fGL2FNumStrip",   &TrackEvent_fGL2FNumStrip);
  mytree->SetBranchAddress("TrackEvent.fGL2FEMeV",       &TrackEvent_fGL2FEMeV);
  mytree->SetBranchAddress("TrackEvent.fGL2BNumStrip",   &TrackEvent_fGL2BNumStrip);
  mytree->SetBranchAddress("TrackEvent.fGL2BEMeV",       &TrackEvent_fGL2BEMeV);
  mytree->SetBranchAddress("TrackEvent.fGCsINum",        &TrackEvent_fGCsINum);
  mytree->SetBranchAddress("TrackEvent.fGCsIECh",        &TrackEvent_fGCsIECh);

	for (Long64_t ientry=0; ientry<nentries; ientry++) {
    mytree->GetEntry(ientry);

    // for L2F_CsI PID
    for (Int_t multi=0; multi<TrackEvent_fGlobalMulti; multi++)  // loop on all particles in one entry
		{
      TeleIndex = TrackEvent_fGSSDNum[multi]*9 + TrackEvent_fGCsINum[multi]; // the global number of csi
			NParticles = Num_of_Particle[TeleIndex];
			for (Int_t np=0; np<NParticles; np++) { // loop one all functions of the current DEE-Plot
        Calc_dE1[np] = DoCalcdEMeV(TrackEvent_fGCsIECh[multi], fitpars[TeleIndex][np], 8);
			}

			if (TrackEvent_fGL2FEMeV[multi]<=0) PID_Num = 0.;
      // 分析质子线以下的实验点
      if (0<TrackEvent_fGL2FEMeV[multi] && TrackEvent_fGL2FEMeV[multi]<=Calc_dE1[0]) {
        PID_Num = 0. + (StdPIDNumber(Z_charge[TeleIndex][0],A_mass[TeleIndex][0]) - 0.)*(TrackEvent_fGL2FEMeV[multi] - 0.)/(Calc_dE1[0] - 0.);
      }
      // 分析质子线以上，最大 PID 线以下的实验点
      if (Calc_dE1[0]<TrackEvent_fGL2FEMeV[multi] && TrackEvent_fGL2FEMeV[multi]<=Calc_dE1[NParticles-1]) {
        for (Int_t np=1; np<NParticles; np++) {
          if (Calc_dE1[np-1]<TrackEvent_fGL2FEMeV[multi] && TrackEvent_fGL2FEMeV[multi]<=Calc_dE1[np]) {
            PID_Num = StdPIDNumber(Z_charge[TeleIndex][np-1],A_mass[TeleIndex][np-1])
                    + (StdPIDNumber(Z_charge[TeleIndex][np],A_mass[TeleIndex][np]) - StdPIDNumber(Z_charge[TeleIndex][np-1],A_mass[TeleIndex][np-1]))
                    * (TrackEvent_fGL2FEMeV[multi] - Calc_dE1[np-1])/(Calc_dE1[np] - Calc_dE1[np-1]);
          }
        }
      }
      // 分析超出最高 PID 线的实验点
      if (TrackEvent_fGL2FEMeV[multi]>Calc_dE1[NParticles-1]) {
        PID_Num = StdPIDNumber(Z_charge[TeleIndex][NParticles-1],A_mass[TeleIndex][NParticles-1])
                * TrackEvent_fGL2FEMeV[multi]/Calc_dE1[NParticles-1];
      }
		}
	}
}


//______________________________________________________________________________
// "拉直法" 粒子鉴别
// StraighteningGetMass() 方法是 StraighteningGetExpPIDNumber() 方法的延伸
Double_t CSHINEStraighteningPID::StraighteningGetMass(Double_t e2, Double_t de1, TCutG* cut[50],
	vector< vector<Double_t> > pars, vector<Int_t> icharge, vector<Int_t> imass, Int_t& geticharge, Int_t& getimass, Double_t& pidnum)
{
	Int_t NPARS = pars[0].size();
	Double_t mass = -99.;
	Double_t calc_dE1 = 0.;
	Double_t dist = 0.;
	Double_t pidnum_range = 0., e2range = 0.;
	Double_t xrange_low = 0., xrange_up = 0.;

  Double_t calc_eproton = DoCalcdEMeV(e2, pars[0].data(), NPARS);
	Double_t calc_elastparticle = DoCalcdEMeV(e2, pars[icharge.size()-1].data(), NPARS);

	if (de1<=0) { cout<<"error: de1<=0!"<<endl; return (mass = 0.); }
	// 对于质子线以下的粒子
  else if (0<de1 && de1<=calc_eproton) {
		if (cut[0]->IsInside(e2, de1)) {
			geticharge = icharge[0];
			pidnum = StdPIDNumber(icharge[0],imass[0]) * (de1/calc_eproton);
			mass = StdPIDNumberToMass(pidnum, geticharge);
			getimass = Int_t (mass + 0.5);
		}
	}
	// 介于质子线 与 最高粒子线之间的粒子
  else if (calc_eproton<de1 && de1<=calc_elastparticle) {
    for (Int_t ip=0; ip<icharge.size(); ip++) {
      if (cut[ip]->IsInside(e2, de1)) {
				geticharge = icharge[ip];
				calc_dE1 = DoCalcdEMeV(e2, pars[ip].data(), NPARS);

        if (de1 > calc_dE1) {
					xrange_low = (cut[ip+1]->GetXaxis()->GetXmin());
					xrange_up  = (cut[ip+1]->GetXaxis()->GetXmax());
					if (xrange_low<=e2 && e2<=xrange_up) {
						dist = de1 - calc_dE1;
						e2range = DoCalcdEMeV(e2, pars[ip+1].data(), NPARS) - calc_dE1;
						pidnum_range = StdPIDNumber(icharge[ip+1],imass[ip+1]) - StdPIDNumber(icharge[ip],imass[ip]);
	          pidnum = StdPIDNumber(icharge[ip],imass[ip]) + dist/e2range * pidnum_range;
					}
					else { pidnum = StdPIDNumber(icharge[ip],imass[ip]) * (de1/calc_dE1); }
				}
				else {
					xrange_low = (cut[ip-1]->GetXaxis()->GetXmin());
          xrange_up  = (cut[ip-1]->GetXaxis()->GetXmax());
					if (xrange_low<=e2 && e2<=xrange_up) {
						dist = de1 - DoCalcdEMeV(e2, pars[ip-1].data(), NPARS);
						e2range = calc_dE1 - DoCalcdEMeV(e2, pars[ip-1].data(), NPARS);
						pidnum_range = StdPIDNumber(icharge[ip],imass[ip]) - StdPIDNumber(icharge[ip-1],imass[ip-1]);
	          pidnum = StdPIDNumber(icharge[ip-1],imass[ip-1]) + dist/e2range * pidnum_range;
					}
          else { pidnum = StdPIDNumber(icharge[ip],imass[ip]) * (de1/calc_dE1); }
				}
				mass = StdPIDNumberToMass(pidnum, geticharge);
				getimass = Int_t (mass + 0.5);
				break;
			}
		}
	}
	// 超出最高线的粒子
	else {
    if (cut[icharge.size()-1]->IsInside(e2, de1)) {
			geticharge = icharge[icharge.size()-1];
			pidnum = StdPIDNumber(icharge[icharge.size()-1],imass[icharge.size()-1]) * (de1/calc_elastparticle);
			mass = StdPIDNumberToMass(pidnum, geticharge);
			getimass = Int_t (mass + 0.5);
		}
	}
	return mass;
}



//______________________________________________________________________________
// fit_func = [0]/x + pol6
//          = [0]/x+[1]+[2]*x+[3]*x*x+[4]*x*x*x+[5]*x*x*x*x+[6]*x*x*x*x*x+[7]*x*x*x*x*x*x
Double_t CSHINEStraighteningPID::DoCalcdEMeV(Double_t Ex, Double_t* pars, Int_t ParsNum)
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
Double_t CSHINEStraighteningPID::StdPIDNumber(Int_t Z_charge, Int_t A_mass)
{
	return (Z_charge + 0.2*(A_mass - 2.0*Z_charge));
}

// 反解 StdPIDNumber(), 求出质量数 mass
Double_t CSHINEStraighteningPID::StdPIDNumberToMass(Double_t pidnumber, Int_t charge)
{
	return (5.0*pidnumber - 3.0*charge);
}

//______________________________________________________________________________
// 读取 “拉直法” 的拟合参数
void CSHINEStraighteningPID::ReadStraighteningFitPars(Int_t TelNum, const char* pathfile, vector< vector<Int_t> >& icharge,
	vector< vector<Int_t> >& imass, vector< vector< vector<Double_t> > >& pars)
{
	Int_t MAXPARTICLE = 50;
	Int_t NPARS = 8;
	Int_t particlenum[TelNum];

	// vector 初始化
	icharge.resize(TelNum);
	imass.resize(TelNum);
	pars.resize(TelNum);
	for (Int_t i=0; i<TelNum; i++) {
	  particlenum[i] = 0.;
    icharge[i].resize(MAXPARTICLE);
		imass[i].resize(MAXPARTICLE);
  	pars[i].resize(MAXPARTICLE);

		for (Int_t j=0; j<MAXPARTICLE; j++) {
			icharge[i][j] = 0;
			imass[i][j]   = 0;
			pars[i][j].resize(NPARS);
			for (Int_t k=0; k<NPARS; k++) pars[i][j][k] = 0.;
		}
	}

	ifstream FileIn(pathfile);
  // 判断文件是否正常打开
 	if (!FileIn.is_open()) {
		printf("Error: file %s not found\n", pathfile);
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

		Int_t ssdnum, particleindex, charge, mass;
		Double_t coeff[8];
		LineStream>>ssdnum>>particleindex>>charge>>mass;

		particlenum[ssdnum] = particleindex;
		icharge[ssdnum][particleindex] = charge;
		imass[ssdnum][particleindex] = mass;

		for (Int_t k=0; k<8; k++) {
			LineStream>>coeff[k];
			pars[ssdnum][particleindex][k] = coeff[k];
		}
	}
	FileIn.close();

	// resize
	for (Int_t i=0; i<TelNum; i++) {
		icharge[i].resize(particlenum[i]+1);
		imass[i].resize(particlenum[i]+1);
	  pars[i].resize(particlenum[i]+1);
	}
}


//______________________________________________________________________________
CSHINECheckDEEPlot::CSHINECheckDEEPlot()
{
	fFirstRun = 100;
	fLastRun  = 110;
}

CSHINECheckDEEPlot::CSHINECheckDEEPlot(Int_t firstrun, Int_t lastrun)
{
	fFirstRun = firstrun;
  fLastRun  = lastrun;
	fDeefit = new CSHINEDEEFITPID(fFirstRun, fLastRun);
}

CSHINECheckDEEPlot::~CSHINECheckDEEPlot()
{}
//_________________________________________________

//_________________________________________________
// 产生 DEEFIT 数据前, 先检查 L2L3 的能量关联是否正常
// 根据检查结果, 决定采用 EL2F-ECsI 的方式
void CSHINECheckDEEPlot::CheckL2L3DEE()
{
  Double_t EL2_Range[4] = {250.,250.,140.,70.};
	Int_t EL2_NBins[4] = {2500,2500,1400,700};

	std::string pathPDFOut("figures/figure_PID/L2FL3_DEE.pdf");
	std::string pathPDFOutBegin("figures/figure_PID/L2FL3_DEE.pdf[");
	std::string pathPDFOutEnd("figures/figure_PID/L2FL3_DEE.pdf]");
	std::string pathROOTOut(Form("%sL2L3DEE.root", PATHROOTFILESFOLDER));
	TFile* rootfile = new TFile(pathROOTOut.c_str(),"RECREATE");

	TH2D* hist_L2L3[NUM_SSD][NUM_CSI];
	for (Int_t i=0; i<NUM_SSD; i++) {
		for (Int_t j=0; j<NUM_CSI; j++) {
		  hist_L2L3[i][j] = new TH2D(Form("DEEL2L3_SSD%d_CsI%d", i+1, j),Form("DEEL2L3_SSD%d_CsI%d", i+1, j),4000,0,4000,EL2_NBins[i],0,EL2_Range[i]);
		}
	}

	Long64_t nentries = (fDeefit->fChain)->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    (fDeefit->fChain)->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			if (fDeefit->fLayerEvent.fSSDCsIMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDL2BMulti[ssdindex]==1 &&
	        fDeefit->fLayerEvent.fSSDL2FMulti[ssdindex]==1)
	    {
	      // 提取 CsI 的数据
	      for (Int_t csimulti=0; csimulti<fDeefit->fLayerEvent.fCsIMulti; csimulti++) {
	        if (fDeefit->fLayerEvent.fCsISSDNum[csimulti] == ssdindex) {
	          L2L3_NumCsI.push_back(fDeefit->fLayerEvent.fCsINum[csimulti]);
	          L2L3_ECSI.push_back(fDeefit->fLayerEvent.fCsIECh[csimulti]);
	        }
	      }
	      // 提取 L2B 的数据
	      for (Int_t l2bmulti=0; l2bmulti<fDeefit->fLayerEvent.fL2BMulti; l2bmulti++) {
	        if (fDeefit->fLayerEvent.fL2BSSDNum[l2bmulti] == ssdindex) {
	         L2L3_StripL2B.push_back(fDeefit->fLayerEvent.fL2BNumStrip[l2bmulti]);
	         L2L3_EL2B.push_back(fDeefit->fLayerEvent.fL2BEMeV[l2bmulti]);
	        }
	      }
	      // 提取 L2F 的数据
	      for (Int_t l2fmulti=0; l2fmulti<fDeefit->fLayerEvent.fL2FMulti; l2fmulti++) {
	        if (fDeefit->fLayerEvent.fL2FSSDNum[l2fmulti] == ssdindex) {
	          L2L3_StripL2F.push_back(fDeefit->fLayerEvent.fL2FNumStrip[l2fmulti]);
	          L2L3_EL2F.push_back(fDeefit->fLayerEvent.fL2FEMeV[l2fmulti]);
	        }
	      }
	      // 填充 hist_L2F_L3
	      if (fPattern.IsGeoConstraint_L3A_L2B(L2L3_NumCsI[0], L2L3_StripL2B[0]) &&
	          fPattern.IsGeoConstraint_L3A_L2F(L2L3_NumCsI[0], L2L3_StripL2F[0]) &&
	          fPattern.IsEneConstraint_L2B_L2F(ssdindex, L2L3_EL2B[0], L2L3_EL2F[0]))
	      {
					hist_L2L3[ssdindex][L2L3_NumCsI[0]]->Fill(L2L3_ECSI[0], L2L3_EL2F[0]);
	      }
	      L2L3_EL2F.clear();
	      L2L3_EL2B.clear();
	      L2L3_ECSI.clear();
	      L2L3_StripL2F.clear();
	      L2L3_StripL2B.clear();
	      L2L3_NumCsI.clear();
	    }
		}
	}

  TCanvas* cans_begin = new TCanvas("cans_begin", "cans_begin", 800, 600);
	TCanvas* cans_end   = new TCanvas("cans_end", "cans_end", 800, 600);
	cans_begin->Close();
	cans_end->Close();
	TCanvas* cans_L2FL3 = new TCanvas("cans_L2FL3", "cans_L2FL3", 800, 600);

  cans_begin->Print(pathPDFOutBegin.c_str());
	for (Int_t i=0; i<NUM_SSD; i++) {
		for (Int_t j=0; j<NUM_CSI; j++) {
			cans_L2FL3->cd();
			hist_L2L3[i][j]->Draw("COLZ");
			cans_L2FL3->Print(pathPDFOut.c_str());
			gPad->Modified();
			gPad->Update();

			// 写进 root file
			rootfile->WriteTObject(hist_L2L3[i][j], hist_L2L3[i][j]->GetName());
		}
	}
	cans_end->Print(pathPDFOutEnd.c_str());
	rootfile->Close();
}


//______________________________________________________________________________
// 检查 L2L3 的 PID 结果
void CSHINECheckDEEPlot::CheckL2L3PIDResults()
{
	gStyle->SetPalette(1);

	Double_t EL2_Range[4] = {250.,250.,140.,70.};
	Int_t EL2_NBins[4] = {2500,2500,1400,700};

  Double_t mass_cut = 0.4;
	Int_t  charge  = -99;  // 电荷数理论值
	Double_t mass  = -99.; // 质量数计算值

	Double_t zeta; // 电荷数计算值
	Int_t   imass; // 质量数理论值

	Double_t sigma_sum = 0.;

  std::string pathDEEFITData(Form("%sDEEFITData/L2L3_EEFITData_Run%04d-%04d.root",PATHROOTFILESFOLDER,fFirstRun,fLastRun));
	std::string pathDEEFITPars(Form("%sDEEFITData/Fitparam_table.out",PATHROOTFILESFOLDER));
	std::string pathPIDResults(Form("%sfigure_PID/PIDResluts_Run%04d-%04d.pdf",PATHFIGURESFOLDER,fFirstRun,fLastRun));
	std::string pathPIDResultsBegin(Form("%sfigure_PID/PIDResluts_Run%04d-%04d.pdf[",PATHFIGURESFOLDER,fFirstRun,fLastRun));
	std::string pathPIDResultsEnd(Form("%sfigure_PID/PIDResluts_Run%04d-%04d.pdf]",PATHFIGURESFOLDER,fFirstRun,fLastRun));
	std::string pathPIDAlphaMassResolution(Form("%sdata_PID/PID_Alpha_MassResolution.dat",PATHDATAFOLDER));

	ofstream FileOut(pathPIDAlphaMassResolution.c_str());
	FileOut<<"* Mass resoltion of alpha particles in the experiment"<<endl;
	FileOut<<"* ssdnum"<<setw(15)<<"csinum"<<setw(15)<<"sigma"<<endl;

	TH1D* h_PID[NUM_SSD*NUM_CSI];
	TH1D* h_PID_alpha[NUM_SSD*NUM_CSI];
	TH2D* h_DEE[NUM_SSD*NUM_CSI];
	TH2D* h_DEE_PIDCut[NUM_SSD*NUM_CSI];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		Int_t ssdnum = numtel/NUM_CSI;
		Int_t csinum = numtel%NUM_CSI;
		h_PID[numtel]       = new TH1D(Form("PID_SSD%d_CsI%d",ssdnum+1,csinum),Form("PID_SSD%d_CsI%d",ssdnum+1,csinum),5000,0,50);
		h_PID_alpha[numtel] = new TH1D(Form("#{PID_{#alpha}}_SSD%d_CsI%d",ssdnum+1,csinum),Form("#{PID_{#alpha}}_SSD%d_CsI%d",ssdnum+1,csinum), 1000, 10, 20);
		h_DEE[numtel]       = new TH2D(Form("DEE_SSD%d_CsI%d",ssdnum+1,csinum),Form("DEE_SSD%d_CsI%d",ssdnum+1,csinum), 4000,0,4000,EL2_NBins[ssdnum],0,EL2_Range[ssdnum]);
		h_DEE_PIDCut[numtel]= new TH2D(Form("PIDDEE_SSD%d_CsI%d",ssdnum+1,csinum),Form("PIDDEE_SSD%d_CsI%d",ssdnum+1,csinum), 4000,0,4000,EL2_NBins[ssdnum],0,EL2_Range[ssdnum]);
	}

  // 读取 DEEFIT 拟合的 14 参数
  Double_t** DEEFITPars = fDeefit->DEEFITLoadPars(pathDEEFITPars.c_str());

	Short_t numtel;  // number of csi crystals
  Float_t  desipgf; // dE (MeV)
  Short_t fastpg;  // ECsI (ADC Chs)

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

		charge = fDeefit->DEEFITGetCharge(DEEFITPars[numtel], desipgf, fastpg, zeta); // Int_t charge
    mass   = fDeefit->DEEFITGetMass(DEEFITPars[numtel], charge, desipgf, fastpg, imass); // Double_t mass

		h_DEE[numtel]->Fill(fastpg, desipgf);

		if (mass != 0.0 && abs(mass-imass)<mass_cut) { // PID
			if ((charge==2 && imass==5) || (charge==4 && imass==8)) continue;
			h_DEE_PIDCut[numtel]->Fill(fastpg, desipgf);
			h_PID[numtel]->Fill(5*charge+mass);
			if (charge==2 && imass==4) h_PID_alpha[numtel]->Fill(5*charge+mass);
		}
	}

  //_________________________________________________
	TCanvas* c_begin = new TCanvas("c_begin","c_begin", 600, 600);
	TCanvas* c_end = new TCanvas("c_end","c_end", 600, 600);
	c_begin->Close();
	c_end->Close();
  TCanvas* cans[NUM_SSD*NUM_CSI];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		Int_t ssdnum = numtel/NUM_CSI;
		Int_t csinum = numtel%NUM_CSI;
		cans[numtel] = new TCanvas(Form("c_SSD%d_CsI%d",ssdnum+1,csinum),Form("c_SSD%d_CsI%d",ssdnum+1,csinum),1000,1000);
	}

  c_begin->Print(pathPIDResultsBegin.c_str());
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		Int_t ssdnum = numtel/NUM_CSI;
		Int_t csinum = numtel%NUM_CSI;

		cans[numtel]->Divide(1,2);
		// draw PID  plot
		cans[numtel]->cd(1);
		h_PID[numtel]->SetStats(0);
		h_PID[numtel]->Draw();
		// fit alpha to get mass resolution
		TF1* fit = new TF1("fit", "gaus", 13.5, 14.5);
		h_PID_alpha[numtel]->Fit("fit", "Q0");
		fit->Draw("SAME");
		Double_t sigma_mass = fit->GetParameter(2);
		sigma_sum += sigma_mass;
		TLatex* latex = new TLatex(10.,0.9*(h_PID[numtel]->GetMaximum()), Form("#sigma_{A(#alpha)}=%.3f",sigma_mass));
		latex->SetTextColor(kRed);
		latex->Draw("SAME");
		// sub-pad
		TPad* subpad = new TPad("subpad", "", 0.36, 0.15, 0.95, 0.95);
	  subpad->Draw();
	  subpad->cd();
	  gPad->SetFillStyle(0);
		TH1D* h_PID_Clone = (TH1D*) h_PID[numtel]->Clone();
		h_PID_Clone->SetTitle("");
		h_PID_Clone->GetXaxis()->SetRangeUser(18, 50);
		h_PID_Clone->Draw();

	  // draw DEE plots
		cans[numtel]->cd(2);
		gPad->Divide(2,1);
		gPad->cd(1);
		h_DEE[numtel]->Draw("COL");
		cans[numtel]->cd(2);
		gPad->cd(2);
		h_DEE_PIDCut[numtel]->Draw("COL");
		TLatex* l_masscut = new TLatex(1300, 0.92*EL2_Range[ssdnum], Form("|mass-imass|<%.1f",mass_cut));
		l_masscut->SetTextColor(kRed);
		l_masscut->Draw("SAME");

    FileOut<<ssdnum<<setw(20)<<csinum<<setw(20)<<sigma_mass<<endl;

		cans[numtel]->Print(pathPIDResults.c_str());
	}
	c_end->Print(pathPIDResultsEnd.c_str());

  FileOut<<endl;
  FileOut<<"* average mass resolution : "<<sigma_sum/34<<endl;
	FileOut.close();
}


//______________________________________________________________________________
// 检查实验中 CsI 对 alpha 粒子的能量分辨率
void CSHINECheckDEEPlot::CheckCsIAlphaEnergyResolution()
{
	srand((unsigned)time(NULL)); // 随机数种子采用系统时钟

	gStyle->SetPalette(1);

	Double_t EL2_Range[4] = {250.,250.,140.,70.};
	Int_t EL2_NBins[4] = {2500,2500,1400,700};

	Int_t ECh1 = 300;
	Int_t ECh2 = 1000;
	Int_t ECh3 = 1700;
	Int_t ECh4 = 2400;

	Int_t  charge  = -99;  // 电荷数理论值
	Double_t mass  = -99.; // 质量数计算值

	Double_t zeta; // 电荷数计算值
	Int_t   imass; // 质量数理论值

	Short_t EX_gaus;
	Float_t EY_gaus;

  Int_t NPoints = 100; // 每个 X 点产生高斯弥散的数目
	Double_t sigmaELow = 0.01, sigmaEUp = 0.06, sigmaEStep = 0.002;
	Double_t sigmadELow = 0.005, sigmadEUp = 0.025, sigmadEStep = 0.001;
	Int_t Nbin_sigmaE  = Int_t ((sigmaEUp-sigmaELow)/sigmaEStep);
	Int_t Nbin_sigmadE = Int_t ((sigmadEUp-sigmadELow)/sigmadEStep);

	Double_t sigma_mass[NUM_SSD*NUM_CSI][3];
	Double_t sigma_mass_MC[NUM_SSD*NUM_CSI][Nbin_sigmaE*Nbin_sigmadE][3];

	Color_t contour_color[3] = {kRed-4, kAzure-3, kSpring-1};

  std::string pathDEEFITPars(Form("%sDEEFITData/Fitparam_table_All.out",PATHROOTFILESFOLDER));
	std::string pathDEEFITData(Form("%sDEEFITData/L2L3_DEEFITData_Run%04d-%04d.root",PATHROOTFILESFOLDER,fFirstRun,fLastRun));
	std::string pathPDFMassDistr(Form("%sfigure_PID/AlphaMassDistr%04d-%04d.pdf",PATHFIGURESFOLDER,fFirstRun,fLastRun));
	std::string pathPDFMassDistrBegin(Form("%sfigure_PID/AlphaMassDistr%04d-%04d.pdf[",PATHFIGURESFOLDER,fFirstRun,fLastRun));
	std::string pathPDFMassDistrEnd(Form("%sfigure_PID/AlphaMassDistr%04d-%04d.pdf]",PATHFIGURESFOLDER,fFirstRun,fLastRun));
  std::string pathPDFCont(Form("%sfigure_PID/AlphaEnergyResolutionCont%04d-%04d.pdf",PATHFIGURESFOLDER,fFirstRun,fLastRun));
	std::string pathPDFContBegin(Form("%sfigure_PID/AlphaEnergyResolutionCont%04d-%04d.pdf[",PATHFIGURESFOLDER,fFirstRun,fLastRun));
	std::string pathPDFContEnd(Form("%sfigure_PID/AlphaEnergyResolutionCont%04d-%04d.pdf]",PATHFIGURESFOLDER,fFirstRun,fLastRun));
	std::string pathPIDAlphaMassResolution(Form("%sdata_PID/PID_Alpha_MassResolution_Range.dat",PATHDATAFOLDER));
  std::string pathROOTOutput("CsIAlphaMassResolution.root");

  TFile* rootfile = new TFile(pathROOTOutput.c_str(),"RECREATE");

	ofstream FileOut(pathPIDAlphaMassResolution.c_str());
	FileOut<<"* Mass resoltion of alpha particles in the experiment"<<endl;
	FileOut<<"* ssdnum"<<setw(15)<<"csinum"<<setw(20)<<"sigma_R1"<<setw(20)<<"sigma_R2"<<setw(20)<<"sigma_R3"<<endl;

	TH1D* h_PID_alpha[NUM_SSD*NUM_CSI][3];
	TH1D* h_PID_MC[NUM_SSD*NUM_CSI][3][Nbin_sigmaE*Nbin_sigmadE];
	TH2D* h_MC_sigma[NUM_SSD*NUM_CSI][3];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		Int_t ssdnum = numtel/NUM_CSI;
		Int_t csinum = numtel%NUM_CSI;
		h_PID_alpha[numtel][0] = new TH1D(Form("ExpSSD%d_CsI%d_Range1",ssdnum+1,csinum),Form("EXpSSD%d_CsI%d_Range1",ssdnum+1,csinum), 1000, 0, 10);
		h_PID_alpha[numtel][1] = new TH1D(Form("EXpSSD%d_CsI%d_Range2",ssdnum+1,csinum),Form("ExpSSD%d_CsI%d_Range2",ssdnum+1,csinum), 1000, 0, 10);
		h_PID_alpha[numtel][2] = new TH1D(Form("EXpSSD%d_CsI%d_Range3",ssdnum+1,csinum),Form("ExpSSD%d_CsI%d_Range3",ssdnum+1,csinum), 1000, 0, 10);

    for (Int_t irange=0; irange<3; irange++) {
			for (Int_t isigma=0; isigma<Nbin_sigmaE*Nbin_sigmadE; isigma++) {
				h_PID_MC[numtel][irange][isigma] = new TH1D(Form("MCSSD%d_CsI%d_sigma%d_Range%d",ssdnum+1,csinum,isigma,irange+1),Form("MCSSD%d_CsI%d_sigma%d_Range%d",ssdnum+1,csinum,isigma,irange+1), 1000, 0, 10);
			}
			h_MC_sigma[numtel][irange] = new TH2D(Form("h2_MCSSD%d_CsI%d_Range%d",ssdnum+1,csinum,irange+1),Form("h2_MCSSD%d_CsI%d_Range%d",ssdnum+1,csinum,irange+1),Nbin_sigmaE,sigmaELow,sigmaEUp,Nbin_sigmadE,sigmadELow,sigmadEUp);
		}
	}

  // 读取 DEEFIT 拟合的 14 参数
	Double_t** DEEFITPars = fDeefit->DEEFITLoadPars(pathDEEFITPars.c_str());

	fDeefitPartile.Z = 2;
	fDeefitPartile.A = 4;

  //__________________________________________________________
	//                    提取实验数据中 alpha 的能量分辨
	//                 ----------------------------------
	//
	Short_t numtel;  // number of csi crystals
  Float_t  desipgf; // dE (MeV)
  Short_t fastpg;  // ECsI (ADC Chs)
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

		charge = fDeefit->DEEFITGetCharge(DEEFITPars[numtel], desipgf, fastpg, zeta); // Int_t charge
    mass   = fDeefit->DEEFITGetMass(DEEFITPars[numtel], charge, desipgf, fastpg, imass); // Double_t mass

		if (charge==2 && imass==4) {
			if (fastpg>ECh1 && fastpg<ECh2) h_PID_alpha[numtel][0]->Fill(mass);
			if (fastpg>ECh2 && fastpg<ECh3) h_PID_alpha[numtel][1]->Fill(mass);
			if (fastpg>ECh3 && fastpg<ECh4) h_PID_alpha[numtel][2]->Fill(mass);
		}
	}
	// 画出每块晶体的 alpha 质量分布情况
	TCanvas* c_dist_begin = new TCanvas("c_dist_begin","c_dist_begin",800,600);
	TCanvas* c_dist_end = new TCanvas("c_dist_end","c_dist_end",800,600);
	c_dist_begin->Close();
	c_dist_end->Close();
	TCanvas* c_massdistr[NUM_SSD*NUM_CSI];
	for (Int_t i=0; i<NUM_SSD*NUM_CSI; i++) {
		Int_t numtel = i/NUM_CSI;
		Int_t numcsi = i%NUM_CSI;
		c_massdistr[i] = new TCanvas(Form("SSD%d_CsI%d",numtel+1,numcsi),Form("SSD%d_CsI%d",numtel+1,numcsi),1200,600);
	}

  c_dist_begin->Print(pathPDFMassDistrBegin.c_str());
  // 拟合实验数据中的 alpha 质量数分布, 得到质量数分布宽度
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		Int_t ssdnum = numtel/NUM_CSI;
		Int_t csinum = numtel%NUM_CSI;

	  TF1* fit_range1 = new TF1("fit_range1", "gaus", 3., 5.);
		TF1* fit_range2 = new TF1("fit_range2", "gaus", 3., 5.);
		TF1* fit_range3 = new TF1("fit_range3", "gaus", 3., 5.);
    h_PID_alpha[numtel][0]->Fit("fit_range1", "Q0");
    h_PID_alpha[numtel][1]->Fit("fit_range2", "Q0");
		h_PID_alpha[numtel][2]->Fit("fit_range3", "Q0");
		sigma_mass[numtel][0] = fit_range1->GetParameter(2);
		sigma_mass[numtel][1] = fit_range2->GetParameter(2);
		sigma_mass[numtel][2] = fit_range3->GetParameter(2);

		TString str1 = ((TString) h_PID_alpha[numtel][0]->GetTitle()).Append(Form("_%04d-%04d",ECh1,ECh2));
		TString str2 = ((TString) h_PID_alpha[numtel][1]->GetTitle()).Append(Form("_%04d-%04d",ECh2,ECh3));
		TString str3 = ((TString) h_PID_alpha[numtel][2]->GetTitle()).Append(Form("_%04d-%04d",ECh3,ECh4));
		h_PID_alpha[numtel][0]->SetTitle(str1.Data());
		h_PID_alpha[numtel][1]->SetTitle(str2.Data());
		h_PID_alpha[numtel][2]->SetTitle(str3.Data());

		c_massdistr[numtel]->Divide(3,1);
		c_massdistr[numtel]->cd(1);
		h_PID_alpha[numtel][0]->Draw();
		c_massdistr[numtel]->cd(2);
		h_PID_alpha[numtel][1]->Draw();
		c_massdistr[numtel]->cd(3);
		h_PID_alpha[numtel][2]->Draw();
		c_massdistr[numtel]->Print(pathPDFMassDistr.c_str());

		FileOut<<ssdnum<<setw(20)<<csinum<<setw(20)<<sigma_mass[numtel][0]<<setw(20)
		       <<sigma_mass[numtel][1]<<setw(20)<<sigma_mass[numtel][2]<<endl;
	}
	c_dist_end->Print(pathPDFMassDistrEnd.c_str());


  //------------------------------------------------------------------
	//                      Monte Carlo 模拟
	//                   ---------------------
	// 模拟 alpha 同位素带的宽度
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		for (Int_t iBinE=0; iBinE<Nbin_sigmaE; iBinE++) {
			for (Int_t iBindE=0; iBindE<Nbin_sigmadE; iBindE++) {
				Double_t sigmaE  = sigmaELow  + iBinE *sigmaEStep;
				Double_t sigmadE = sigmadELow + iBindE*sigmadEStep;

				for (Int_t Ebin=ECh1; Ebin<ECh4; Ebin+=10) {
					fDeefitPartile.E = Ebin;  // 给定 ECh_CsI
					Double_t dE = fDeefit->DEEFITFunc14(fDeefitPartile, DEEFITPars[numtel]); // 使用 DEEFIT 拟合结果计算 dE
					for (Int_t rbin=0; rbin<NPoints; rbin++) {    // 对于 ECh_CsI 的每个bin, 产生 100 个随机数
            EX_gaus = gRandom->Gaus(Ebin, Ebin*sigmaE); // 对 ECh 加入 gaus 弥散
						EY_gaus = gRandom->Gaus(dE, dE*sigmadE);    // 对 dE 加入 gaus 弥散
						charge = fDeefit->DEEFITGetCharge(DEEFITPars[numtel], EY_gaus, EX_gaus, zeta); // Int_t charge
				    mass   = fDeefit->DEEFITGetMass(DEEFITPars[numtel], charge, EY_gaus, EX_gaus, imass); // Double_t mass
						if (Ebin>ECh1 && Ebin<ECh2) h_PID_MC[numtel][0][iBinE*Nbin_sigmadE+iBindE]->Fill(mass);
						if (Ebin>ECh2 && Ebin<ECh3) h_PID_MC[numtel][1][iBinE*Nbin_sigmadE+iBindE]->Fill(mass);
						if (Ebin>ECh3 && Ebin<ECh4) h_PID_MC[numtel][2][iBinE*Nbin_sigmadE+iBindE]->Fill(mass);
					}
				}
			}
		}
	}

	// 计算模拟 alpha 质量数分布中的 sigma
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		for (Int_t irange=0; irange<3; irange++) {
			for (Int_t iBinE=0; iBinE<Nbin_sigmaE; iBinE++) {
				for (Int_t iBindE=0; iBindE<Nbin_sigmadE; iBindE++) {
					TF1* fit = new TF1("fit", "gaus", 0., 10.);
					 h_PID_MC[numtel][irange][iBinE*Nbin_sigmadE+iBindE]->Fit("fit", "Q0");
					 sigma_mass_MC[numtel][irange][iBinE*Nbin_sigmadE+iBindE] = fit->GetParameter(2);
					 h_MC_sigma[numtel][irange]->SetBinContent(iBinE+1,iBindE+1,sigma_mass_MC[numtel][irange][iBinE*Nbin_sigmadE+iBindE]);
				}
			}
			h_MC_sigma[numtel][irange]->GetXaxis()->SetNdivisions(505);
			h_MC_sigma[numtel][irange]->GetYaxis()->SetNdivisions(505);
			rootfile->WriteTObject(h_MC_sigma[numtel][irange],h_MC_sigma[numtel][irange]->GetName());
		}
	}

	// 模拟得到的二维质量数宽度分布的等高线
	TCanvas* c_cont_begin = new TCanvas("c_cont_begin","c_cont_begin",800,600);
	TCanvas* c_cont_end = new TCanvas("c_cont_end","c_cont_end",800,600);
	c_cont_begin->Close();
	c_cont_end->Close();
	TCanvas* c_cont[NUM_SSD*NUM_CSI];
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		for (Int_t irange=0; irange<3; irange++) {
			Double_t sigma_cont[1] = {sigma_mass[numtel][irange]};
			h_MC_sigma[numtel][irange]->SetContour(1, sigma_cont);
			h_MC_sigma[numtel][irange]->SetLineColor(contour_color[irange]);
		  h_MC_sigma[numtel][irange]->SetLineWidth(4);
			h_MC_sigma[numtel][irange]->SetLineStyle(1);
		}
		Int_t ssdnum = numtel/NUM_CSI;
		Int_t csinum = numtel%NUM_CSI;
		c_cont[numtel] = new TCanvas(Form("SSD%d_CsI%d",ssdnum+1,csinum),Form("SSD%d_CsI%d",ssdnum+1,csinum),800,600);
	}

	// 画图
  c_cont_begin->Print(pathPDFContBegin.c_str());
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		c_cont[numtel+1]->cd();
    h_MC_sigma[numtel][0]->Draw("CONT3");
		h_MC_sigma[numtel][1]->Draw("CONT3 SAME");
		h_MC_sigma[numtel][2]->Draw("CONT3 SAME");
		c_cont[numtel+1]->Print(pathPDFCont.c_str());
	}
	c_cont_end->Print(pathPDFContEnd.c_str());
}



//______________________________________________________________________________
// “拉直法“ 对每一种粒子单独使用多项式拟合，这里需要检查每一种粒子的拟合结果
// 因为多项式拟合,只对选中的范围内拟合效果较好，范围之外的数据点可能差异巨大
Double_t DrawPoly(Double_t* x, Double_t* pars)
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

void CSHINECheckDEEPlot::CheckPolyFitResults()
{
	Double_t XRange[4] = {320,280,150,100};
	Double_t Erange[4] = {320,280,150,80};
	Double_t NBins[4] = {3200,2800,1500,800};
	Int_t NPoints = 30; // 每种粒子提取 NPoints 个点
	Double_t ELoss = -999.;
	Double_t ECsI_Calc;
	Int_t Xmin, Xmax;
	Int_t MaxParticle = 50;
	Int_t NPARS = 8;
	Int_t SSDNum;

	// for L1L2
  Int_t NUMTEL = 50;
	std::string DEETag("L1L2");
	std::string HistName("h2_dE1_dE2");

  // for L2L3
//	Int_t NUMTEL = NUM_SSD*NUM_CSI;
//	std::string DEETag("L2L3");
//	std::string HistName("h2_dE2_ECsI");

	std::string pathStraighteningFitPars(Form("%sdata_PID/%s_StraighteningFitPars.dat",PATHDATAFOLDER,DEETag.c_str()));
	std::string pathDEECutsInput(Form("%sStraighteningData/%sDEECuts.root",PATHROOTFILESFOLDER,DEETag.c_str()));
	std::string pathPDFCheckPoly(Form("%sfigure_PID/%sPolyDraw.pdf",PATHFIGURESFOLDER,DEETag.c_str()));
	std::string pathPDFCheckPolyBegin(Form("%sfigure_PID/%sPolyDraw.pdf[",PATHFIGURESFOLDER,DEETag.c_str()));
	std::string pathPDFCheckPolyEnd(Form("%sfigure_PID/%sPolyDraw.pdf]",PATHFIGURESFOLDER,DEETag.c_str()));

	std::vector< vector<Int_t> > iCharge;
	std::vector< vector<Int_t> > iMass;
	std::vector< vector< vector<Double_t> > > FitPars;

	fStraightening.ReadStraighteningFitPars(NUMTEL, pathStraighteningFitPars.c_str(), iCharge, iMass, FitPars);

	TFile* CutFile = new TFile(pathDEECutsInput.c_str(),"READONLY");
	TCutG* IsotopeCut[NUMTEL][MaxParticle];

	for (Int_t numtel=0; numtel<NUMTEL; numtel++) {
		for (Int_t ip=0; ip<iCharge[numtel].size(); ip++) {
			IsotopeCut[numtel][ip] = (TCutG*) CutFile->Get(Form("%s_Tel%02d_Z%02d_A%02d",HistName.c_str(),numtel,iCharge[numtel][ip],iMass[numtel][ip]));
      if (IsotopeCut[numtel][ip]==0) continue;
		}
	}

  TFile* fout = new TFile(Form("%s%sDEE_PolyDraw.root",PATHROOTFILESFOLDER,DEETag.c_str()), "RECREATE");
	fout->cd();

  TCanvas* c_begin = new TCanvas("c_begin","c_begin",600,600);
	TCanvas* c_end = new TCanvas("c_end","c_end",600,600);
	c_begin->Close();
	c_end->Close();
  TCanvas* cans[NUMTEL];
	TF1* func[MaxParticle];

	for (Int_t numtel=0; numtel<NUMTEL; numtel++) {
		if (iCharge[numtel].size() == 1) continue;
		if (numtel<5) SSDNum = numtel;
		if (numtel>5) SSDNum = numtel/10;

		cans[numtel] = new TCanvas(Form("Tel%d",numtel),Form("Tel%d",numtel),800,800);
		cans[numtel]->cd();
    // for L1L2
		if (strcmp(DEETag.c_str(),"L1L2")==0) {
			for (Int_t ip=0; ip<iCharge[numtel].size(); ip++) {
				if (IsotopeCut[numtel][ip]==0) continue;
				Xmin = (Int_t) IsotopeCut[numtel][ip]->GetXaxis()->GetXmin();
				Xmax = (Int_t) IsotopeCut[numtel][ip]->GetXaxis()->GetXmax();
				func[ip] = new TF1(Form("func%d",ip),DrawPoly,0,XRange[SSDNum],NPARS); // for L1L2
				func[ip]->SetRange(Xmin, Xmax);
				// cout<<"tel = "<<numtel<<setw(10)<<"ip = "<<ip<<setw(10)<<"xrange = "<<IsotopeCut[numtel][ip]->GetXaxis()->GetXmax()<<endl;
				for (Int_t i=0; i<NPARS; i++) {
					func[ip]->SetParameter(i,FitPars[numtel][ip][i]);
				}
			}
			TH1D* h1 = new TH1D(Form("h_tel%d",numtel),"",NBins[SSDNum],0,XRange[SSDNum]);  // for L1L2
			h1->SetTitle(Form("Tel%d",numtel));
			h1->Draw();
			h1->GetXaxis()->SetRangeUser(2,XRange[SSDNum]);  // for L1L2
			h1->GetYaxis()->SetRangeUser(0,Erange[SSDNum]);  // for L1L2
			for (Int_t i=0; i<iCharge[numtel].size(); i++) {
				func[i]->SetTitle(func[i]->GetName());
				func[i]->Draw("SAME");
				IsotopeCut[numtel][i]->Draw("SAME");
				IsotopeCut[numtel][i]->SetLineColor(kAzure+7);

				fout->WriteTObject(func[i], Form("SSD%d_Particle%d", numtel, i));
			}
		}
    // for L2L3
		if (strcmp(DEETag.c_str(),"L2L3")==0) {
			for (Int_t ip=0; ip<iCharge[numtel].size(); ip++) {
				if (IsotopeCut[numtel][ip]==0) continue;
				Xmin = (Int_t) IsotopeCut[numtel][ip]->GetXaxis()->GetXmin();
				Xmax = (Int_t) IsotopeCut[numtel][ip]->GetXaxis()->GetXmax();
	      func[ip] = new TF1(Form("func%d",ip),DrawPoly,0,4096,8);  // for L2L3
				func[ip]->SetRange(100, 4096);
				for (Int_t i=0; i<NPARS; i++) {
					func[ip]->SetParameter(i,FitPars[numtel][ip][i]);
				}
			}
			TH1D* h1 = new TH1D(Form("h_tel%d",numtel),"",4096,0,4096);  // for L2L3
			h1->SetTitle(Form("Tel%d",numtel));
			h1->Draw();
		 	h1->GetXaxis()->SetRangeUser(0,4096);       // for L2L3
			h1->GetYaxis()->SetRangeUser(0,Erange[numtel/NUM_CSI]);  // for L2L3
			for (Int_t i=0; i<iCharge[numtel].size(); i++) {
				func[i]->SetTitle(func[i]->GetName());
				func[i]->Draw("SAME");
				IsotopeCut[numtel][i]->Draw("SAME");
				IsotopeCut[numtel][i]->SetLineColor(kAzure+7);

				fout->WriteTObject(func[i], Form("SSD%d_Particle%d", numtel, i));
			}
		}
	}

	c_begin->Print(pathPDFCheckPolyBegin.c_str());
	for (Int_t numtel=0; numtel<NUMTEL; numtel++) {
		if (iCharge[numtel].size() == 1) continue;
		cans[numtel]->Print(pathPDFCheckPoly.c_str());
	}
	c_end->Print(pathPDFCheckPolyEnd.c_str());

	// close the file
	fout->Close();
}


//______________________________________________________________________________
void CSHINECheckDEEPlot::CheckL2L3StraighteningPIDResults()
{
	Int_t MAXPARTICLES = NUM_SSD*NUM_CSI;
  Int_t NumTel;
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2[4]  = {3000, 2500, 1500, 800};

	Double_t getmass, pidnum;
	Int_t geticharge, getimass;

  std::string pathDEECutsInput(Form("%sStraighteningData/L2L3DEECuts.root",PATHROOTFILESFOLDER));
  std::string pathStraighteningFitPars(Form("%sdata_PID/L2L3_StraighteningFitPars.dat",PATHDATAFOLDER));

	std::string pathL2L3PIDOutput(Form("%sfigure_PID/L2L3_StraighteningPID.pdf",PATHFIGURESFOLDER));
  std::string pathL2L3PIDOutputBegin(Form("%sfigure_PID/L2L3_StraighteningPID.pdf[",PATHFIGURESFOLDER));
	std::string pathL2L3PIDOutputEnd(Form("%sfigure_PID/L2L3_StraighteningPID.pdf]",PATHFIGURESFOLDER));

	TH2D* h2d_DEE[MAXPARTICLES];
	TH2D* h2d_DEE_Cut[MAXPARTICLES];
	TH1D* h1_PIDNum[MAXPARTICLES];
	for (Int_t i=0; i<MAXPARTICLES; i++) {
		Int_t ssdindex = (Int_t) i/NUM_CSI;
		Int_t csiindex = (Int_t) i%NUM_CSI;
		h2d_DEE[i] = new TH2D(Form("SSD%d_dE2_ECsI%d",ssdindex+1,csiindex),Form("SSD%d_dE2_ECsI%d",ssdindex+1,csiindex),4096,0,4096,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
    h2d_DEE_Cut[i] = new TH2D(Form("SSD%d_dE2_ECsI%d_Cut",ssdindex+1,csiindex),Form("SSD%d_dE2_ECsI%d_Cut",ssdindex+1,csiindex),4096,0,4096,NBinsL2[ssdindex],0,EL2Range[ssdindex]);
    h1_PIDNum[i] = new TH1D(Form("SSD%d_CsI%d_PID",ssdindex+1,csiindex),Form("SSD%d_CsI%d_PID",ssdindex+1,csiindex),600,0.,60.);
	}

  std::vector< vector<Int_t> > iCharge;
	std::vector< vector<Int_t> > iMass;
	std::vector< vector< vector<Double_t> > > FitPars;

  fStraightening.ReadStraighteningFitPars(MAXPARTICLES, pathStraighteningFitPars.c_str(), iCharge, iMass, FitPars);

	TFile* CutFile = new TFile(pathDEECutsInput.c_str(),"READONLY");
	TCutG* IsotopeCut[MAXPARTICLES][50];
	for (Int_t numtel=0; numtel<MAXPARTICLES; numtel++) {
		for (Int_t ip=0; ip<iCharge[numtel].size(); ip++) {
		 	IsotopeCut[numtel][ip] = (TCutG*) CutFile->Get(Form("h2_dE2_ECsI_Tel%02d_Z%02d_A%02d",numtel,iCharge[numtel][ip],iMass[numtel][ip]));
			if (IsotopeCut[numtel][ip]==0) continue;
		 //	cout<<ip<<setw(10)<<IsotopeCut[numtel][ip]->GetName()<<endl;
		}
		// cout<<numtel<<setw(10)<<iCharge[numtel].size()<<endl;
	}

	Long64_t nentries = (fDeefit->fChain)->GetEntries();
	cout<<"nentries = "<<nentries<<endl;

	for (Long64_t jentry=0; jentry<nentries; jentry++) {
		(fDeefit->fChain)->GetEntry(jentry);
	  timeper.PrintPercentageAndRemainingTime(jentry, nentries);

		for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			if (fDeefit->fLayerEvent.fSSDL1SMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDL2FMulti[ssdindex]==1 &&
				  fDeefit->fLayerEvent.fSSDL2BMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDCsIMulti[ssdindex]==1)
	    {
	      L2L3_EL2F.clear();
	      L2L3_EL2B.clear();
				L2L3_ECSI.clear();
	      L2L3_StripL2F.clear();
	      L2L3_StripL2B.clear();
				L2L3_NumCsI.clear();
				L2FTime.clear();

				// 提取 L2F 的数据
	      for (Int_t l2fmulti=0; l2fmulti<fDeefit->fLayerEvent.fL2FMulti; l2fmulti++) {
	        if (fDeefit->fLayerEvent.fL2FSSDNum[l2fmulti] == ssdindex) {
	          L2L3_StripL2F.push_back(fDeefit->fLayerEvent.fL2FNumStrip[l2fmulti]);
	          L2L3_EL2F.push_back(fDeefit->fLayerEvent.fL2FEMeV[l2fmulti]);
						L2FTime.push_back(fDeefit->fLayerEvent.fL2FTime[l2fmulti]);
	        }
	      }
	      // 提取 L2B 的数据
	      for (Int_t l2bmulti=0; l2bmulti<fDeefit->fLayerEvent.fL2BMulti; l2bmulti++) {
	        if (fDeefit->fLayerEvent.fL2BSSDNum[l2bmulti] == ssdindex) {
	         L2L3_StripL2B.push_back(fDeefit->fLayerEvent.fL2BNumStrip[l2bmulti]);
	         L2L3_EL2B.push_back(fDeefit->fLayerEvent.fL2BEMeV[l2bmulti]);
	        }
	      }
				// 提取 CsI 的数据
	      for (Int_t csimulti=0; csimulti<fDeefit->fLayerEvent.fCsIMulti; csimulti++) {
	        if (fDeefit->fLayerEvent.fCsISSDNum[csimulti] == ssdindex) {
	          L2L3_NumCsI.push_back(fDeefit->fLayerEvent.fCsINum[csimulti]);
	          L2L3_ECSI.push_back(fDeefit->fLayerEvent.fCsIECh[csimulti]);
	        }
	      }

				if (fPattern.IsGeoConstraint_L3A_L2B(L2L3_NumCsI[0], L2L3_StripL2B[0]) &&
	          fPattern.IsGeoConstraint_L3A_L2F(L2L3_NumCsI[0], L2L3_StripL2F[0]) &&
	          fPattern.IsEneConstraint_L2B_L2F(ssdindex, L2L3_EL2B[0], L2L3_EL2F[0]))
	      {
					NumTel = ssdindex*9 + L2L3_NumCsI[0];
					h2d_DEE[NumTel]->Fill(L2L3_ECSI[0], L2L3_EL2F[0]);
					if (iCharge[NumTel].size()<2) continue;
					for (Int_t ip=0; ip<iCharge[NumTel].size(); ip++) {
						if (IsotopeCut[NumTel][ip]->IsInside(L2L3_ECSI[0], L2L3_EL2F[0])) {
							h2d_DEE_Cut[NumTel]->Fill(L2L3_ECSI[0], L2L3_EL2F[0]);
							break;
						}
					}
					// PID
					getmass = fStraightening.StraighteningGetMass(L2L3_ECSI[0],L2L3_EL2F[0],IsotopeCut[NumTel],FitPars[NumTel],iCharge[NumTel], iMass[NumTel], geticharge, getimass, pidnum);
					if (getmass > 0) h1_PIDNum[NumTel]->Fill(5*geticharge+getmass);
				}
			}
		}

	}

	// 画图
	TCanvas* c_begin = new TCanvas("c_begin","c_begin",600,600);
	TCanvas* c_end = new TCanvas("c_end","c_end",600,600);
	c_begin->Print(pathL2L3PIDOutputBegin.c_str());
	for (Int_t ssdindex=0; ssdindex<MAXPARTICLES; ssdindex++) {
	  if (iCharge[ssdindex].size()<2) continue;
		DrawPID(h2d_DEE[ssdindex], h2d_DEE_Cut[ssdindex], h1_PIDNum[ssdindex], pathL2L3PIDOutput.c_str());
	}
	c_end->Print(pathL2L3PIDOutputEnd.c_str());
}


//______________________________________________________________________________
void CSHINECheckDEEPlot::L2L3DrawPIDECsIMap()
{
	Int_t MAXPARTICLES = NUM_SSD*NUM_CSI;
  Int_t NumTel;
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2[4]  = {3000, 2500, 1500, 800};

	Double_t getmass, pidnum;
	Int_t geticharge, getimass;

	std::string pathDEECutsInput(Form("%sStraighteningData/L2L3DEECuts.root",PATHROOTFILESFOLDER));
  std::string pathStraighteningFitPars(Form("%sdata_PID/L2L3_StraighteningFitPars.dat",PATHDATAFOLDER));

	TFile* fileout = new TFile("/home/sea/Fission2019_Data/StraighteningData/L2L3_PIDNum.root","RECREATE");

	TH2D* h2d_PID_vs_ECsI[MAXPARTICLES];
	for (Int_t i=0; i<MAXPARTICLES; i++) {
		Int_t ssdindex = (Int_t) i/NUM_CSI;
		Int_t csiindex = (Int_t) i%NUM_CSI;
		h2d_PID_vs_ECsI[i] = new TH2D(Form("SSD%d_CsI%02d",ssdindex+1,csiindex),Form("SSD%d_CsI%02d",ssdindex+1,csiindex),4096,0,4096,1000,0,10);
	}

  std::vector< vector<Int_t> > iCharge;
	std::vector< vector<Int_t> > iMass;
	std::vector< vector< vector<Double_t> > > FitPars;

  fStraightening.ReadStraighteningFitPars(MAXPARTICLES, pathStraighteningFitPars.c_str(), iCharge, iMass, FitPars);

	TFile* CutFile = new TFile(pathDEECutsInput.c_str(),"READONLY");
	TCutG* IsotopeCut[MAXPARTICLES][50];
	for (Int_t numtel=0; numtel<MAXPARTICLES; numtel++) {
		for (Int_t ip=0; ip<iCharge[numtel].size(); ip++) {
		 	IsotopeCut[numtel][ip] = (TCutG*) CutFile->Get(Form("h2_dE2_ECsI_Tel%02d_Z%02d_A%02d",numtel,iCharge[numtel][ip],iMass[numtel][ip]));
			if (IsotopeCut[numtel][ip]==0) continue;
		}
	}

	Long64_t nentries = (fDeefit->fChain)->GetEntries();
	cout<<"nentries = "<<nentries<<endl;

	for (Long64_t jentry=0; jentry<nentries; jentry++) {
		(fDeefit->fChain)->GetEntry(jentry);
	  timeper.PrintPercentageAndRemainingTime(jentry, nentries);

		for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			if (fDeefit->fLayerEvent.fSSDL1SMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDL2FMulti[ssdindex]==1 &&
				  fDeefit->fLayerEvent.fSSDL2BMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDCsIMulti[ssdindex]==1)
	    {
	      L2L3_EL2F.clear();
	      L2L3_EL2B.clear();
				L2L3_ECSI.clear();
	      L2L3_StripL2F.clear();
	      L2L3_StripL2B.clear();
				L2L3_NumCsI.clear();
				L2FTime.clear();

				// 提取 L2F 的数据
	      for (Int_t l2fmulti=0; l2fmulti<fDeefit->fLayerEvent.fL2FMulti; l2fmulti++) {
	        if (fDeefit->fLayerEvent.fL2FSSDNum[l2fmulti] == ssdindex) {
	          L2L3_StripL2F.push_back(fDeefit->fLayerEvent.fL2FNumStrip[l2fmulti]);
	          L2L3_EL2F.push_back(fDeefit->fLayerEvent.fL2FEMeV[l2fmulti]);
						L2FTime.push_back(fDeefit->fLayerEvent.fL2FTime[l2fmulti]);
	        }
	      }
	      // 提取 L2B 的数据
	      for (Int_t l2bmulti=0; l2bmulti<fDeefit->fLayerEvent.fL2BMulti; l2bmulti++) {
	        if (fDeefit->fLayerEvent.fL2BSSDNum[l2bmulti] == ssdindex) {
	         L2L3_StripL2B.push_back(fDeefit->fLayerEvent.fL2BNumStrip[l2bmulti]);
	         L2L3_EL2B.push_back(fDeefit->fLayerEvent.fL2BEMeV[l2bmulti]);
	        }
	      }
				// 提取 CsI 的数据
	      for (Int_t csimulti=0; csimulti<fDeefit->fLayerEvent.fCsIMulti; csimulti++) {
	        if (fDeefit->fLayerEvent.fCsISSDNum[csimulti] == ssdindex) {
	          L2L3_NumCsI.push_back(fDeefit->fLayerEvent.fCsINum[csimulti]);
	          L2L3_ECSI.push_back(fDeefit->fLayerEvent.fCsIECh[csimulti]);
	        }
	      }

				if (fPattern.IsGeoConstraint_L3A_L2B(L2L3_NumCsI[0], L2L3_StripL2B[0]) &&
	          fPattern.IsGeoConstraint_L3A_L2F(L2L3_NumCsI[0], L2L3_StripL2F[0]) &&
	          fPattern.IsEneConstraint_L2B_L2F(ssdindex, L2L3_EL2B[0], L2L3_EL2F[0]))
	      {
					NumTel = ssdindex*9 + L2L3_NumCsI[0];
					if (iCharge[NumTel].size()<2) continue;
					// PID
					getmass = fStraightening.StraighteningGetMass(L2L3_ECSI[0],L2L3_EL2F[0],IsotopeCut[NumTel],FitPars[NumTel],iCharge[NumTel], iMass[NumTel], geticharge, getimass, pidnum);
					if (getmass > 0) {
						h2d_PID_vs_ECsI[NumTel]->Fill(L2L3_ECSI[0], pidnum);
					}
				}
			}
		}
	}
	for (Int_t numtel=0; numtel<MAXPARTICLES; numtel++) {
    fileout->WriteTObject(h2d_PID_vs_ECsI[numtel]);
	}
  fileout->Close();
}

//______________________________________________________________________________
// 产生 DEEFIT 数据前, 先检查 L1L2 的能量关联是否正常
void CSHINECheckDEEPlot::CheckL1L2DEE()
{
	Double_t EL1_Range[4] = {200.,200.,100.,70.};
	Double_t EL2_Range[4] = {250.,200.,140.,70.};
	Int_t EL1_NBins[4] = {2000,2000,1000,700};
	Int_t EL2_NBins[4] = {2500,2000,1400,700};

	std::string pathPDFOut("figures/figure_PID/L1L2F_DEE.pdf");
	std::string pathPDFOutBegin("figures/figure_PID/L1L2F_DEE.pdf[");
	std::string pathPDFOutEnd("figures/figure_PID/L1L2F_DEE.pdf]");

  TH2D* hist_L1L2_Merged[NUM_SSD];
	TH2D* hist_L1L2[NUM_SSD][NUM_STRIP];
	for (Int_t i=0; i<NUM_SSD; i++) {
		hist_L1L2_Merged[i] = new TH2D(Form("SSD%d_Merged", i+1),Form("SSD%d_Merged",i+1),EL2_NBins[i],0,EL2_Range[i],EL1_NBins[i],0,EL1_Range[i]);

		for (Int_t j=0; j<NUM_STRIP; j++) {
			hist_L1L2[i][j] = new TH2D(Form("SSD%d_Strip%d", i+1, j+1),Form("SSD%d_Strip%d", i+1, j+1),
			                        EL2_NBins[i],0,EL2_Range[i],EL1_NBins[i],0,EL1_Range[i]);
		}
	}

	Long64_t nentries = (fDeefit->fChain)->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    (fDeefit->fChain)->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			if (fDeefit->fLayerEvent.fSSDCsIMulti[ssdindex]==0 && fDeefit->fLayerEvent.fSSDL2BMulti[ssdindex]==1 &&
	        fDeefit->fLayerEvent.fSSDL2FMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDL1SMulti[ssdindex]==1)
	    {
	      // 提取 L2B 的数据
	      for (Int_t l2bmulti=0; l2bmulti<fDeefit->fLayerEvent.fL2BMulti; l2bmulti++) {
	        if (fDeefit->fLayerEvent.fL2BSSDNum[l2bmulti] == ssdindex) {
	         L1L2_StripL2B.push_back(fDeefit->fLayerEvent.fL2BNumStrip[l2bmulti]);
	         L1L2_EL2B.push_back(fDeefit->fLayerEvent.fL2BEMeV[l2bmulti]);
	        }
	      }
	      // 提取 L2F 的数据
	      for (Int_t l2fmulti=0; l2fmulti<fDeefit->fLayerEvent.fL2FMulti; l2fmulti++) {
	        if (fDeefit->fLayerEvent.fL2FSSDNum[l2fmulti] == ssdindex) {
	          L1L2_StripL2F.push_back(fDeefit->fLayerEvent.fL2FNumStrip[l2fmulti]);
	          L1L2_EL2F.push_back(fDeefit->fLayerEvent.fL2FEMeV[l2fmulti]);
						L2FTime.push_back(fDeefit->fLayerEvent.fL2FTime[l2fmulti]);
	        }
	      }
				// 提取 L1S 的数据
	      for (Int_t l1smulti=0; l1smulti<fDeefit->fLayerEvent.fL1SMulti; l1smulti++) {
	        if (fDeefit->fLayerEvent.fL1SSSDNum[l1smulti] == ssdindex) {
	          L1L2_StripL1S.push_back(fDeefit->fLayerEvent.fL1SNumStrip[l1smulti]);
	          L1L2_EL1S.push_back(fDeefit->fLayerEvent.fL1SEMeV[l1smulti]);
	        }
	      }
	      // 填充 hist_L1_L2F
	      if (fPattern.IsEneConstraint_L2B_L2F(ssdindex, L1L2_EL2B[0], L1L2_EL2F[0])) {
					if (abs(L1L2_StripL1S[0]-L1L2_StripL2B[0])==0) {
						if (L2FTime[0]>SiTimeCut_Low[ssdindex][L1L2_StripL2F[0]] && L2FTime[0]<SiTimeCut_Up[ssdindex][L1L2_StripL2F[0]]) {
              if (ssdindex==0 || ssdindex==1)          hist_L1L2_Merged[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
							if (ssdindex==2 && (L1L2_StripL2F[0]<4)) hist_L1L2_Merged[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
              if (ssdindex==3 && (L1L2_StripL2F[0]<4)) hist_L1L2_Merged[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
							hist_L1L2[ssdindex][L1L2_StripL2F[0]]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
						}
					}
	      }
				L1L2_EL1S.clear();
	      L1L2_EL2F.clear();
	      L1L2_EL2B.clear();
				L1L2_StripL1S.clear();
	      L1L2_StripL2F.clear();
	      L1L2_StripL2B.clear();
				L2FTime.clear();
	    }
		}
	}

  TCanvas* c_begin = new TCanvas("c_begin","c_begin",800,600);
	TCanvas* c_end   = new TCanvas("c_end","c_end",800,600);
	c_begin->Close();
	c_end->Close();
	TCanvas* cans_L1L2[NUM_SSD][NUM_STRIP];
	for (Int_t i=0; i<NUM_SSD; i++) {
		for (Int_t j=0; j<NUM_STRIP; j++) {
			cans_L1L2[i][j] = new TCanvas(Form("cans_SSD%d_Strip%d",i+1,j+1),Form("cans_SSD%d_Strip%d",i+1,j+1), 800, 600);
		}
	}

	c_begin->Print(pathPDFOutBegin.c_str());
	for (Int_t i=0; i<NUM_SSD; i++) {
		for (Int_t j=0; j<NUM_STRIP; j++) {
			cans_L1L2[i][j]->cd();
			hist_L1L2[i][j]->Draw("COLZ");
			cans_L1L2[i][j]->Print(pathPDFOut.c_str());
		}
	}
	c_end->Print(pathPDFOutEnd.c_str());

	TCanvas* c_merged = new TCanvas("c_merged","c_merged",1000,1000);
	c_merged->Divide(2,2);

	for (Int_t i=0; i<NUM_SSD; i++) {
		c_merged->cd(i+1);
		hist_L1L2_Merged[i]->Draw("COL");
	}
	c_merged->Print("figures/figure_PID/L1L2F_DEE_Merged.pdf");
}


//______________________________________________________________________________
// SSD3, SSD4 前两层硅的 DEE 分辨很差, 现在使用刻度前的数据
void CSHINECheckDEEPlot::CheckL1L2DEE_Uncalibrated()
{
	Double_t EL1_Range[4] = {4000.,4000.,4000.,4000.};
	Double_t EL2_Range[4] = {4000.,4000.,4000.,4000.};
	Int_t EL1_NBins[4] = {4000,4000,4000,4000};
	Int_t EL2_NBins[4] = {4000,4000,4000,4000};

	Int_t TimeCut_Low[4][16] = { {2100,2100,2100,2100,2140,2100,2050,2140,2100,2140,2100,2100,2100,2050,2140,2050},
	                             {2050,2050,2050,2140,2050,2150,2100,2140,2100,2150,2100,2100,2100,2140,2050,2050},
															 {2150,2150,2150,2150,2150,2150,2220,2140,2150,2140,2150,2100,2140,2140,2140,2100},
															 {2120,2100,2100,2160,2140,2140,2180,2180,2140,2140,2140,2140,2100,2100,2100,2180}
                             };


	Int_t TimeCut_Up[4][16]  = { {2200,2200,2150,2200,2220,2200,2150,2200,2200,2200,2200,2200,2150,2100,2200,2100},
	                             {2150,2150,2150,2200,2150,2250,2150,2200,2150,2200,2200,2150,2200,2200,2150,2150},
															 {2250,2250,2250,2200,2250,2250,2260,2200,2250,2200,2200,2150,2200,2200,2200,2160},
															 {2160,2200,2200,2230,2240,2180,2230,2220,2200,2200,2190,2190,2150,2150,2150,2230}
                             };

  Int_t SSD_L1S_E[NUM_SSD][NUM_STRIP];
	Int_t SSD_L2F_E[NUM_SSD][NUM_STRIP];
	Int_t SSD_L2B_E[NUM_SSD][NUM_STRIP];

	std::string L1S_E("L1S_E");
	std::string L2F_E("L2F_E");
	std::string L2B_E("L2B_E");

	std::string pathPDFOut("figures/figure_PID/L1L2F_DEE_Uncalibrated.pdf");
	std::string pathPDFOutBegin("figures/figure_PID/L1L2F_DEE_Uncalibrated.pdf[");
	std::string pathPDFOutEnd("figures/figure_PID/L1L2F_DEE_Uncalibrated.pdf]");

  TH2D* hist_L1L2_Merged[NUM_SSD];
	TH2D* hist_L1L2[NUM_SSD][NUM_STRIP];
	for (Int_t i=0; i<NUM_SSD; i++) {
		hist_L1L2_Merged[i] = new TH2D(Form("SSD%d_Merged", i+1),Form("SSD%d_Merged",i+1),EL2_NBins[i],0,EL2_Range[i],EL1_NBins[i],0,EL1_Range[i]);

		for (Int_t j=0; j<NUM_STRIP; j++) {
			hist_L1L2[i][j] = new TH2D(Form("SSD%d_Strip%d", i+1, j+1),Form("SSD%d_Strip%d", i+1, j+1),
			                        EL2_NBins[i],0,EL2_Range[i],EL1_NBins[i],0,EL1_Range[i]);
		}
	}

	TChain* mychain = new TChain("KrPb");
	for (Int_t ifile=fFirstRun; ifile<(fLastRun+1); ifile++) {
		mychain->Add(Form("%sMapRoot/MapFission2019_Kr_Pb%04d.root",PATHROOTFILESFOLDER,ifile));
	}
	Long64_t nentries = mychain->GetEntries();
	cout<<"nentries = "<<nentries<<endl;
  // 使用 friend tree 的方式访问 Maproot 数据与  LayerEvent 数据
  (fDeefit->fChain)->AddFriend(mychain);

	// 读取 Tree
	mychain->SetBranchStatus("*", 0);
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
    readfile.AddChain(mychain, L1S_E.c_str(), SSD_L1S_E[ssdindex], NUM_STRIP, ssdindex);
		readfile.AddChain(mychain, L2F_E.c_str(), SSD_L2F_E[ssdindex], NUM_STRIP, ssdindex);
		readfile.AddChain(mychain, L2B_E.c_str(), SSD_L2B_E[ssdindex], NUM_STRIP, ssdindex);
	}

	for (Long64_t ientry=0; ientry<nentries; ientry++) {
    (fDeefit->fChain)->GetEntry(ientry);
		mychain->GetEntry(ientry);
    timeper.PrintPercentageAndRemainingTime(ientry, nentries);

    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			if (fDeefit->fLayerEvent.fSSDCsIMulti[ssdindex]==0 && fDeefit->fLayerEvent.fSSDL2BMulti[ssdindex]==1 &&
	        fDeefit->fLayerEvent.fSSDL2FMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDL1SMulti[ssdindex]==1)
	    {
	      // 提取 L2B 的数据
	      for (Int_t l2bmulti=0; l2bmulti<fDeefit->fLayerEvent.fL2BMulti; l2bmulti++) {
	        if (fDeefit->fLayerEvent.fL2BSSDNum[l2bmulti] == ssdindex) {
	         L1L2_StripL2B.push_back(fDeefit->fLayerEvent.fL2BNumStrip[l2bmulti]);
	         L1L2_EL2B.push_back(fDeefit->fLayerEvent.fL2BEMeV[l2bmulti]);
	        }
	      }
	      // 提取 L2F 的数据
	      for (Int_t l2fmulti=0; l2fmulti<fDeefit->fLayerEvent.fL2FMulti; l2fmulti++) {
	        if (fDeefit->fLayerEvent.fL2FSSDNum[l2fmulti] == ssdindex) {
	          L1L2_StripL2F.push_back(fDeefit->fLayerEvent.fL2FNumStrip[l2fmulti]);
	          L1L2_EL2F.push_back(fDeefit->fLayerEvent.fL2FEMeV[l2fmulti]);
						L2FTime.push_back(fDeefit->fLayerEvent.fL2FTime[l2fmulti]);
	        }
	      }
				// 提取 L1S 的数据
	      for (Int_t l1smulti=0; l1smulti<fDeefit->fLayerEvent.fL1SMulti; l1smulti++) {
	        if (fDeefit->fLayerEvent.fL1SSSDNum[l1smulti] == ssdindex) {
	          L1L2_StripL1S.push_back(fDeefit->fLayerEvent.fL1SNumStrip[l1smulti]);
	          L1L2_EL1S.push_back(fDeefit->fLayerEvent.fL1SEMeV[l1smulti]);
	        }
	      }
	      // 填充 hist_L1_L2F
	      if (fPattern.IsEneConstraint_L2B_L2F(ssdindex, L1L2_EL2B[0], L1L2_EL2F[0])) {
					for (Int_t l1smulti=0; l1smulti<fDeefit->fLayerEvent.fSSDL1SMulti[ssdindex]; l1smulti++) {
						if (abs(L1L2_StripL1S[l1smulti]-L1L2_StripL2B[0])<=1) {
							if (L2FTime[0]>TimeCut_Low[ssdindex][L1L2_StripL2F[0]] && L2FTime[0]<TimeCut_Up[ssdindex][L1L2_StripL2F[0]]) {
								//hist_L1L2_Merged[ssdindex]->Fill(SSD_L2F_E[ssdindex][L1L2_StripL2F[0]], SSD_L1S_E[ssdindex][L1L2_StripL1S[l1smulti]]);
				        //hist_L1L2[ssdindex][L1L2_StripL2F[0]]->Fill(SSD_L2F_E[ssdindex][L1L2_StripL2F[0]], SSD_L1S_E[ssdindex][L1L2_StripL1S[l1smulti]]);
							}
						}
					}
	      }
				hist_L1L2_Merged[ssdindex]->Fill(SSD_L2F_E[ssdindex][L1L2_StripL2F[0]], SSD_L1S_E[ssdindex][L1L2_StripL1S[0]]);
				L1L2_EL1S.clear();
	      L1L2_EL2F.clear();
	      L1L2_EL2B.clear();
				L1L2_StripL1S.clear();
	      L1L2_StripL2F.clear();
	      L1L2_StripL2B.clear();
				L2FTime.clear();
	    }
		}
	}
/*
  TCanvas* c_begin = new TCanvas("c_begin","c_begin",800,600);
	TCanvas* c_end   = new TCanvas("c_end","c_end",800,600);
	c_begin->Close();
	c_end->Close();
	TCanvas* cans_L1L2[NUM_SSD][NUM_STRIP];
	for (Int_t i=0; i<NUM_SSD; i++) {
		for (Int_t j=0; j<NUM_STRIP; j++) {
			cans_L1L2[i][j] = new TCanvas(Form("cans_SSD%d_Strip%d",i+1,j+1),Form("cans_SSD%d_Strip%d",i+1,j+1), 800, 600);
		}
	}

	c_begin->Print(pathPDFOutBegin.c_str());
	for (Int_t i=0; i<NUM_SSD; i++) {
		for (Int_t j=0; j<NUM_STRIP; j++) {
			cans_L1L2[i][j]->cd();
			hist_L1L2[i][j]->Draw("COLZ");
			cans_L1L2[i][j]->Print(pathPDFOut.c_str());
		}
	}
	c_end->Print(pathPDFOutEnd.c_str());
*/
	TCanvas* c_merged = new TCanvas("c_merged","c_merged",1000,1000);
	c_merged->Divide(2,2);

	for (Int_t i=0; i<NUM_SSD; i++) {
		c_merged->cd(i+1);
		hist_L1L2_Merged[i]->Draw("COL");
	}
	c_merged->Print("L1L2F_DEE_Merged.pdf");
}


//______________________________________________________________________________
// SSD3, SSD4 前两层硅的 DE-E 关联, 粒子鉴别能力较差, 分不清同位素
// 这里通过 LISE++, 给 Si 手动加上能量分辨, 给硅的厚度均匀性加上范围,
// 考察 DE-E 的结果
void CSHINECheckDEEPlot::CheckDEEL1L2_SiResolution()
{
	srand((unsigned)time(NULL)); // 随机数种子采用系统时钟

  Double_t SiEnergyResolution = 0.01;
//	Double_t SiThicknessUniformity = 0.05;
	Double_t SiThicknessUniformity = 10.0;
	//Double_t SiThicknessUniformity_L1[4] = {0.03, 0.03, 0.05, 0.05};
	//Double_t SiThicknessUniformity_L2[4] = {0.01, 0.01, 0.02, 0.03};

	EnergyLossModule eloss;
  Double_t Eloss_L1[16];
  Double_t Eloss_L2[16];
  Double_t Eres_L2 [16];

	Double_t EL1Range[4] = {150., 150., 100., 100.};
	Int_t    NBinsL1[4]  = {1500, 1500, 1000, 1000};
	Double_t EL2Range[4] = {300., 300., 250., 200.};
  Int_t    NBinsL2[4]  = {3000, 3000, 2500, 2000};
  // 定义直方图
	TH2D* h2d[4];
	for (Int_t i=0; i<4; i++) {
		h2d[i] = new TH2D(Form("SSD%d_DEE_L1L2",i+1),Form("SSD%d_DEE_L1L2",i+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
	}
  // 产生数据
  for (Int_t issd=0; issd<4; issd++) {
    for (Int_t ip=0; ip<16; ip++) {
      for (Double_t einc=0.; einc<400.; einc+=0.1) {
			//	Double_t Thickness_L1 = gRandom->Uniform(SIL1THICKNESS[issd]*1000*(1-SiThicknessUniformity_L1[issd]), SIL1THICKNESS[issd]*1000*(1+SiThicknessUniformity_L1[issd]));
			  Double_t Thickness_L1 = gRandom->Uniform(SIL1THICKNESS[issd]*1000-SiThicknessUniformity, SIL1THICKNESS[issd]*1000+SiThicknessUniformity);
        Eloss_L1[ip] = eloss.GetEnergyLoss(Z_A_LCPs[ip][0], Z_A_LCPs[ip][1], einc, "Si", Thickness_L1, 1);

			//	Double_t Thickness_L2 = gRandom->Uniform(SIL2THICKNESS[issd]*1000*(1-SiThicknessUniformity_L2[i]), SIL2THICKNESS[issd]*1000*(1+SiThicknessUniformity_L2[i]));
			  Double_t Thickness_L2 = gRandom->Uniform(SIL2THICKNESS[issd]*1000-SiThicknessUniformity, SIL2THICKNESS[issd]*1000+SiThicknessUniformity);
				Double_t ELoss_L1_Gaus = gRandom->Gaus(Eloss_L1[ip], Eloss_L1[ip]*SiEnergyResolution);
        Eloss_L2[ip] = eloss.GetEnergyLoss(Z_A_LCPs[ip][0], Z_A_LCPs[ip][1], einc-ELoss_L1_Gaus, "Si", Thickness_L2, 1);
        Double_t ELoss_L2_Gaus = gRandom->Gaus(Eloss_L2[ip], Eloss_L2[ip]*SiEnergyResolution);
				// h2d[issd]->Fill(Eloss_L2[ip], Eloss_L1[ip]);
				h2d[issd]->Fill(ELoss_L2_Gaus, ELoss_L1_Gaus);
			}
    }
  }
  // 画图
	TCanvas* cans = new TCanvas("cans","cans",1000,1000);
	cans->Divide(2,2);
	for (Int_t i=0; i<4; i++) {
		cans->cd(i+1);
		h2d[i]->Draw("COL");
	}
	cans->Print("CheckDEE_L1L2.png");
}


//________________________________________________________________
void CSHINECheckDEEPlot::CheckL1L2StraighteningPIDResults()
{
	Int_t MAXPARTICLES = 40;
  Int_t NumSSD;
	Int_t csinum;
	Double_t EL1Range[4] = {400., 320., 100., 100.};
	Int_t    NBinsL1[4]  = {4000, 3200, 1000, 1000};
	Double_t EL2Range[4] = {300., 250., 150., 100.};
	Int_t    NBinsL2[4]  = {3000, 2500, 1500, 1000};

	Double_t getmass, pidnum;
	Int_t geticharge, getimass;

  std::string pathDEECutsInput(Form("%sStraighteningData/L1L2DEECuts.root",PATHROOTFILESFOLDER));
  std::string pathStraighteningFitPars(Form("%sdata_PID/L1L2_StraighteningFitPars.dat",PATHDATAFOLDER));

	std::string pathL1L2PIDOutput(Form("%sfigure_PID/L1L2_StraighteningPID.pdf",PATHFIGURESFOLDER));
  std::string pathL1L2PIDOutputBegin(Form("%sfigure_PID/L1L2_StraighteningPID.pdf[",PATHFIGURESFOLDER));
	std::string pathL1L2PIDOutputEnd(Form("%sfigure_PID/L1L2_StraighteningPID.pdf]",PATHFIGURESFOLDER));

	TH2D* h2d_DEE[MAXPARTICLES];
	TH2D* h2d_DEE_Cut[MAXPARTICLES];
	TH1D* h1_PIDNum[MAXPARTICLES];
	for (Int_t i=0; i<MAXPARTICLES; i++) {
		if (i<4) NumSSD = i;
		if (i>4) NumSSD = i/10;
		h2d_DEE[i] = new TH2D(Form("Tel%d_dE1_E2",i),Form("Tel%d_dE1_E2",i),NBinsL2[NumSSD],0,EL2Range[NumSSD],NBinsL1[NumSSD],0,EL1Range[NumSSD]);
    h2d_DEE_Cut[i] = new TH2D(Form("Tel%d_dE1_E2_Cut",i),Form("Tel%d_dE1_E2_Cut",i),NBinsL2[NumSSD],0,EL2Range[NumSSD],NBinsL1[NumSSD],0,EL1Range[NumSSD]);
    h1_PIDNum[i] = new TH1D(Form("Tel%d_L1L2_PID",i),Form("Tel%d_L1L2_PID",i),1600,0.,80.);
	}

  std::vector< vector<Int_t> > iCharge;
	std::vector< vector<Int_t> > iMass;
	std::vector< vector< vector<Double_t> > > FitPars;

  fStraightening.ReadStraighteningFitPars(MAXPARTICLES, pathStraighteningFitPars.c_str(), iCharge, iMass, FitPars);

	TFile* CutFile = new TFile(pathDEECutsInput.c_str(),"READONLY");
	TCutG* IsotopeCut[MAXPARTICLES][50];
	for (Int_t ssdindex=0; ssdindex<MAXPARTICLES; ssdindex++) {
		for (Int_t ip=0; ip<iCharge[ssdindex].size(); ip++) {
		 	IsotopeCut[ssdindex][ip] = (TCutG*) CutFile->Get(Form("h2_dE1_dE2_Tel%02d_Z%02d_A%02d",ssdindex,iCharge[ssdindex][ip],iMass[ssdindex][ip]));
			if (IsotopeCut[ssdindex][ip]==0) continue;
		}
	}

	Long64_t nentries = (fDeefit->fChain)->GetEntries();
	cout<<"nentries = "<<nentries<<endl;

	for (Long64_t jentry=0; jentry<nentries; jentry++) {
		(fDeefit->fChain)->GetEntry(jentry);
	  timeper.PrintPercentageAndRemainingTime(jentry, nentries);

		for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			if (fDeefit->fLayerEvent.fSSDL1SMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDL2FMulti[ssdindex]==1 &&
				  fDeefit->fLayerEvent.fSSDL2BMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDCsIMulti[ssdindex]==0)
	    {
				L1L2_EL1S.clear();
	      L1L2_EL2F.clear();
	      L1L2_EL2B.clear();
				L1L2_StripL1S.clear();
	      L1L2_StripL2F.clear();
	      L1L2_StripL2B.clear();
				L2FTime.clear();

				// 提取 L1S 的数据
	      for (Int_t l1smulti=0; l1smulti<fDeefit->fLayerEvent.fL1SMulti; l1smulti++) {
	        if (fDeefit->fLayerEvent.fL1SSSDNum[l1smulti] == ssdindex) {
	          L1L2_StripL1S.push_back(fDeefit->fLayerEvent.fL1SNumStrip[l1smulti]);
	          L1L2_EL1S.push_back(fDeefit->fLayerEvent.fL1SEMeV[l1smulti]);
	        }
	      }
				// 提取 L2F 的数据
	      for (Int_t l2fmulti=0; l2fmulti<fDeefit->fLayerEvent.fL2FMulti; l2fmulti++) {
	        if (fDeefit->fLayerEvent.fL2FSSDNum[l2fmulti] == ssdindex) {
	          L1L2_StripL2F.push_back(fDeefit->fLayerEvent.fL2FNumStrip[l2fmulti]);
	          L1L2_EL2F.push_back(fDeefit->fLayerEvent.fL2FEMeV[l2fmulti]);
						L2FTime.push_back(fDeefit->fLayerEvent.fL2FTime[l2fmulti]);
	        }
	      }
	      // 提取 L2B 的数据
	      for (Int_t l2bmulti=0; l2bmulti<fDeefit->fLayerEvent.fL2BMulti; l2bmulti++) {
	        if (fDeefit->fLayerEvent.fL2BSSDNum[l2bmulti] == ssdindex) {
	         L1L2_StripL2B.push_back(fDeefit->fLayerEvent.fL2BNumStrip[l2bmulti]);
	         L1L2_EL2B.push_back(fDeefit->fLayerEvent.fL2BEMeV[l2bmulti]);
	        }
	      }

				csinum = fPattern.GetCsINumFromPixel(L1L2_StripL2F[0],L1L2_StripL2B[0]);
				if (IsPixelOnCsIGap(L1L2_StripL2F[0],L1L2_StripL2B[0])) continue;
	      if (fPattern.IsEneConstraint_L2B_L2F(ssdindex, L1L2_EL2B[0], L1L2_EL2F[0]))
	      {
					if (L1L2_StripL1S[0]==L1L2_StripL2B[0]) {
					  if (L2FTime[0]>SiTimeCut_Low[ssdindex][L1L2_StripL2F[0]] && L2FTime[0]<SiTimeCut_Up[ssdindex][L1L2_StripL2F[0]]) {
              // h2d_DEE
							h2d_DEE[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
						  if (ssdindex==2 || ssdindex==3) h2d_DEE[ssdindex*10+csinum]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
              // h2d_DEE_Cut
							for (Int_t ip=0; ip<iCharge[ssdindex].size(); ip++) {
								if (IsotopeCut[ssdindex][ip]->IsInside(L1L2_EL2F[0], L1L2_EL1S[0])) {
									h2d_DEE_Cut[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
									if (ssdindex==2 || ssdindex==3) h2d_DEE_Cut[ssdindex*10+csinum]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
									break;
								}
							}
              // PID
							getmass = fStraightening.StraighteningGetMass(L1L2_EL2F[0],L1L2_EL1S[0],IsotopeCut[ssdindex],FitPars[ssdindex],iCharge[ssdindex], iMass[ssdindex], geticharge, getimass, pidnum);
              if (getmass > 0) h1_PIDNum[ssdindex]->Fill(5*geticharge+getmass);
              if (ssdindex==2 || ssdindex==3) {
								getmass = fStraightening.StraighteningGetMass(L1L2_EL2F[0],L1L2_EL1S[0],IsotopeCut[ssdindex*10+csinum],FitPars[ssdindex*10+csinum],iCharge[ssdindex*10+csinum], iMass[ssdindex*10+csinum], geticharge, getimass, pidnum);
                if (getmass > 0) h1_PIDNum[ssdindex*10+csinum]->Fill(5*geticharge+getmass);
							}
            }
					}
	      }
			}
		}
	}

	// 画图
	TCanvas* c_begin = new TCanvas("c_begin","c_begin",600,600);
	TCanvas* c_end = new TCanvas("c_end","c_end",600,600);
	c_begin->Print(pathL1L2PIDOutputBegin.c_str());
	for (Int_t ssdindex=0; ssdindex<MAXPARTICLES; ssdindex++) {
		if (iCharge[ssdindex].size()<5) continue;
		DrawPID(h2d_DEE[ssdindex], h2d_DEE_Cut[ssdindex], h1_PIDNum[ssdindex], pathL1L2PIDOutput.c_str());
	}
	c_end->Print(pathL1L2PIDOutputEnd.c_str());
}


//______________________________________________________________________________
void CSHINECheckDEEPlot::DrawPID(TH2D* h2d, TH2D* h2d_cut, TH1D* h_pid, std::string path)
{
	TCanvas* cans = new TCanvas("cans","cans",1000,1000);

	cans->cd();
	TPad* pad1 = new TPad("pad1", "", 0.05, 0.5, 0.5, 0.95);
	pad1->Draw();
	pad1->cd();
	gPad->SetFillStyle(0);
	h2d->Draw("COL");
	gPad->Modified();
	gPad->Update();

	cans->cd();
	TPad* pad2 = new TPad("pad2", "", 0.5, 0.5, 0.95, 0.95);
	pad2->Draw();
	pad2->cd();
	gPad->SetFillStyle(0);
	h2d_cut->Draw("COL");
	gPad->Modified();
	gPad->Update();

	cans->cd();
	TPad* pad3 = new TPad("pad3", "", 0.05, 0.05, 0.95, 0.5);
	pad3->Draw();
	pad3->cd();
	gPad->SetFillStyle(0);
	h_pid->Draw();
	gPad->Modified();
	gPad->Update();

	Double_t pad_xmin = (Double_t) 20./(h_pid->GetXaxis()->GetXmax());
	TPad* subpad = new TPad("subpad","",pad_xmin,0.15,0.9,0.9);
	subpad->Draw();
	subpad->cd();
	gPad->SetFillStyle(0);
	TH1D* hpid_clone = (TH1D*) h_pid->Clone();
	hpid_clone->GetXaxis()->SetRangeUser(18,80);
	hpid_clone->Draw();
	gPad->Modified();
	gPad->Update();

	cans->Print(path.c_str());
}

//______________________________________________________________________________
Bool_t CSHINECheckDEEPlot::IsPixelOnCsIGap(Int_t stripf, Int_t stripb)
{
	Bool_t index = kFALSE;

	std::vector<Int_t> StripsOnCsIGap{0,4,5,10,11,15}; // 判断给定的条是否包含在一个数组内
	if ((std::find(StripsOnCsIGap.begin(),StripsOnCsIGap.end(),stripf) != StripsOnCsIGap.end()) ||
      (std::find(StripsOnCsIGap.begin(),StripsOnCsIGap.end(),stripb) != StripsOnCsIGap.end()))
	{
		index = kTRUE;
	}
	return index;
}


//______________________________________________________________________________
void CSHINECheckDEEPlot::CheckGoodPIDOfSSD3AndSSD4()
{
	Double_t EL1Range[4] = {400., 320., 100., 100.};
	Int_t    NBinsL1[4]  = {4000, 3200, 1000, 1000};
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2[4]  = {3000, 2500, 1500, 800};

	Double_t getmass = 0., pidnum = 0.;
	Int_t geticharge = 0, getimass = 0;
	Int_t csinum = -99;

	std::vector<Int_t> IsSSD3BadStripF{7,8,9,10,11,12};
	std::vector<Int_t> IsSSD4BadStripF{0,5,6,7,9};

  std::string pathDEECutsInput(Form("%sStraighteningData/L1L2DEECuts.root",PATHROOTFILESFOLDER));
  std::string pathStraighteningFitPars(Form("%sdata_PID/L1L2_StraighteningFitPars.dat",PATHDATAFOLDER));
	std::string pathLayerEventRootInput(Form("%sCSHINEEvent/EventTree_Run%04d-Run%04d.root",PATHROOTFILESFOLDER,fFirstRun,fLastRun));

  //	std::string pathHistDataOutput(Form("%sStraighteningData/L1L2_SSD3SSD4_HistData_Run%04d-%04d.root",PATHROOTFILESFOLDER,fFirstRun,fLastRun));
  //  TFile* L1L2_SSD3SSD4_HistData = new TFile(pathHistDataOutput.c_str(),"RECREATE");

	std::string pathSSD3SSD4PIDCheck(Form("%sfigure_PID/SSD3_SSD4_PID_CheckStrip.pdf",PATHFIGURESFOLDER));
	std::string pathSSD3SSD4PIDCheckBegin(Form("%sfigure_PID/SSD3_SSD4_PID_CheckStrip.pdf[",PATHFIGURESFOLDER));
	std::string pathSSD3SSD4PIDCheckEnd(Form("%sfigure_PID/SSD3_SSD4_PID_CheckStrip.pdf]",PATHFIGURESFOLDER));

	std::string pathSSD3SSD4PIDCheckOneCsI(Form("%sfigure_PID/SSD3_SSD4_PID_CheckCsI.pdf",PATHFIGURESFOLDER));
	std::string pathSSD3SSD4PIDCheckOneCsIBegin(Form("%sfigure_PID/SSD3_SSD4_PID_CheckCsI.pdf[",PATHFIGURESFOLDER));
	std::string pathSSD3SSD4PIDCheckOneCsIEnd(Form("%sfigure_PID/SSD3_SSD4_PID_CheckCsI.pdf]",PATHFIGURESFOLDER));

	TH2D* h2d_DEE[NUM_SSD];
	TH2D* h2d_DEE_Cut[NUM_SSD];
	TH1D* h1_PIDNum[NUM_SSD];

	TH2D* h2d_DEE_OneStrip[NUM_SSD][NUM_STRIP];
	TH2D* h2d_DEE_Cut_OneStrip[NUM_SSD][NUM_STRIP];
	TH1D* h1_PIDNum_OneStrip[NUM_SSD][NUM_STRIP];

	TH2D* h2d_DEE_OneCsI[NUM_SSD][NUM_CSI];
	TH2D* h2d_DEE_Cut_OneCsI[NUM_SSD][NUM_CSI];
	TH1D* h1_PIDNum_OneCsI[NUM_SSD][NUM_CSI];

	TH2D* h2d_DEE_AllStrip[NUM_SSD];
	TH2D* h2d_DEE_Cut_AllStrip[NUM_SSD];
	TH1D* h1_PIDNum_AllStrip[NUM_SSD];

	TH2D* h2d_DEE_GoodStrip[NUM_SSD];
	TH2D* h2d_DEE_Cut_GoodStrip[NUM_SSD];
	TH1D* h1_PIDNum_GoodStrip[NUM_SSD];

	TH2D* h2d_DEE_AllCsI[NUM_SSD];
	TH2D* h2d_DEE_Cut_AllCsI[NUM_SSD];
	TH1D* h1_PIDNum_AllCsI[NUM_SSD];

	TH2D* h2d_DEE_GoodCsI[NUM_SSD];
	TH2D* h2d_DEE_Cut_GoodCsI[NUM_SSD];
	TH1D* h1_PIDNum_GoodCsI[NUM_SSD];

	for (Int_t i=0; i<NUM_SSD; i++) {
		h2d_DEE[i] = new TH2D(Form("SSD%d_dE1_E2",i+1),Form("SSD%d_dE1_E2",i+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
    h2d_DEE_Cut[i] = new TH2D(Form("SSD%d_dE1_E2_Cut",i+1),Form("SSD%d_dE1_E2_Cut",i+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
    h1_PIDNum[i] = new TH1D(Form("SSD%d_L1L2_PID",i+1),Form("SSD%d_L1L2_PID",i+1),800,0.,80.);

		for (Int_t j=0; j<NUM_STRIP; j++) {
			h2d_DEE_OneStrip[i][j] = new TH2D(Form("SSD%d_Strip%d_dE1_E2",i+1,j+1),Form("SSD%d_Strip%d_dE1_E2",i+1,j+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
      h2d_DEE_Cut_OneStrip[i][j] = new TH2D(Form("SSD%d_Strip%d_dE1_E2_Cut",i+1,j+1),Form("SSD%d_Strip%d_dE1_E2_Cut",i+1,j+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
			h1_PIDNum_OneStrip[i][j] = new TH1D(Form("SSD%d_Strip%d_L1L2_PID",i+1,j+1),Form("SSD%d_Strip%d_L1L2_PID",i+1,j+1),800,0.,80.);
		}

		for (Int_t k=0; k<NUM_CSI; k++) {
			h2d_DEE_OneCsI[i][k]     = new TH2D(Form("SSD%d_CsI%d_dE1_E2",i+1,k+1),Form("SSD%d_CsI%d_dE1_E2",i+1,k+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
      h2d_DEE_Cut_OneCsI[i][k] = new TH2D(Form("SSD%d_CsI%d_dE1_E2_Cut",i+1,k+1),Form("SSD%d_CsI%d_dE1_E2_Cut",i+1,k+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
			h1_PIDNum_OneCsI[i][k]   = new TH1D(Form("SSD%d_CsI%d_L1L2_PID",i+1,k+1),Form("SSD%d_CsI%d_L1L2_PID",i+1,k+1),800,0.,80.);
		}

		h2d_DEE_AllStrip[i]     = new TH2D(Form("SSD%d_dE1_E2_AllStrip",i+1),Form("SSD%d_dE1_E2_AllStrip",i+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
		h2d_DEE_Cut_AllStrip[i] = new TH2D(Form("SSD%d_dE1_E2_Cut_AllStrip",i+1),Form("SSD%d_dE1_E2_Cut_AllStrip",i+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
		h1_PIDNum_AllStrip[i]   = new TH1D(Form("SSD%d_L1L2_AllStrip_PID",i+1),Form("SSD%d_L1L2_AllStrip_PID",i+1),800,0.,80.);

		h2d_DEE_GoodStrip[i]     = new TH2D(Form("SSD%d_dE1_E2_GoodStrip",i+1),Form("SSD%d_dE1_E2_GoodStrip",i+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
		h2d_DEE_Cut_GoodStrip[i] = new TH2D(Form("SSD%d_dE1_E2_Cut_GoodStrip",i+1),Form("SSD%d_dE1_E2_Cut_GoodStrip",i+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
		h1_PIDNum_GoodStrip[i]   = new TH1D(Form("SSD%d_L1L2_GoodStrip_PID",i+1),Form("SSD%d_L1L2_GoodStrip_PID",i+1),800,0.,80.);

		h2d_DEE_AllCsI[i]     = new TH2D(Form("SSD%d_dE1_E2_AllCsI",i+1),Form("SSD%d_dE1_E2_AllCsI",i+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
		h2d_DEE_Cut_AllCsI[i] = new TH2D(Form("SSD%d_dE1_E2_Cut_AllCsI",i+1),Form("SSD%d_dE1_E2_Cut_AllCsI",i+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
		h1_PIDNum_AllCsI[i]   = new TH1D(Form("SSD%d_L1L2_AllCsI_PID",i+1),Form("SSD%d_L1L2_AllCsI_PID",i+1),800,0.,80.);

		h2d_DEE_GoodCsI[i]     = new TH2D(Form("SSD%d_dE1_E2_GoodCsI",i+1),Form("SSD%d_dE1_E2_GoodCsI",i+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
		h2d_DEE_Cut_GoodCsI[i] = new TH2D(Form("SSD%d_dE1_E2_Cut_GoodCsI",i+1),Form("SSD%d_dE1_E2_Cut_GoodCsI",i+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
		h1_PIDNum_GoodCsI[i]   = new TH1D(Form("SSD%d_L1L2_GoodCsI_PID",i+1),Form("SSD%d_L1L2_GoodCsI_PID",i+1),800,0.,80.);
	}

  std::vector< vector<Int_t> > iCharge;
	std::vector< vector<Int_t> > iMass;
	std::vector< vector< vector<Double_t> > > FitPars;

  fStraightening.ReadStraighteningFitPars(NUM_SSD, pathStraighteningFitPars.c_str(), iCharge, iMass, FitPars);

	TFile* CutFile = new TFile(pathDEECutsInput.c_str(),"READONLY");
	TCutG* IsotopeCut[NUM_SSD][50];
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		for (Int_t ip=0; ip<iCharge[ssdindex].size(); ip++) {
			IsotopeCut[ssdindex][ip] = (TCutG*) CutFile->Get(Form("h2_dE1_dE2_Tel%02d_Z%02d_A%02d",ssdindex,iCharge[ssdindex][ip],iMass[ssdindex][ip]));
      // cout<<IsotopeCut[ssdindex][ip]->GetName()<<endl;
		}
	}

  Long64_t nentries = (fDeefit->fChain)->GetEntries();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    (fDeefit->fChain)->GetEntry(ientry);
		// if (ientry>100) continue;
		timeper.PrintPercentageAndRemainingTime(ientry, nentries);

		for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			if (fDeefit->fLayerEvent.fSSDL1SMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDL2FMulti[ssdindex]==1 &&
	        fDeefit->fLayerEvent.fSSDL2BMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDCsIMulti[ssdindex]==0)
	    {
				L1L2_EL1S.clear();
	      L1L2_EL2F.clear();
	      L1L2_EL2B.clear();
				L1L2_StripL1S.clear();
	      L1L2_StripL2F.clear();
	      L1L2_StripL2B.clear();
				L2FTime.clear();

				// 提取 L1S 的数据
	      for (Int_t l1smulti=0; l1smulti<fDeefit->fLayerEvent.fL1SMulti; l1smulti++) {
	        if (fDeefit->fLayerEvent.fL1SSSDNum[l1smulti] == ssdindex) {
	          L1L2_StripL1S.push_back(fDeefit->fLayerEvent.fL1SNumStrip[l1smulti]);
	          L1L2_EL1S.push_back(fDeefit->fLayerEvent.fL1SEMeV[l1smulti]);
	        }
	      }
				// 提取 L2F 的数据
	      for (Int_t l2fmulti=0; l2fmulti<fDeefit->fLayerEvent.fL2FMulti; l2fmulti++) {
	        if (fDeefit->fLayerEvent.fL2FSSDNum[l2fmulti] == ssdindex) {
	          L1L2_StripL2F.push_back(fDeefit->fLayerEvent.fL2FNumStrip[l2fmulti]);
	          L1L2_EL2F.push_back(fDeefit->fLayerEvent.fL2FEMeV[l2fmulti]);
						L2FTime.push_back(fDeefit->fLayerEvent.fL2FTime[l2fmulti]);
	        }
	      }
	      // 提取 L2B 的数据
	      for (Int_t l2bmulti=0; l2bmulti<fDeefit->fLayerEvent.fL2BMulti; l2bmulti++) {
	        if (fDeefit->fLayerEvent.fL2BSSDNum[l2bmulti] == ssdindex) {
	         L1L2_StripL2B.push_back(fDeefit->fLayerEvent.fL2BNumStrip[l2bmulti]);
	         L1L2_EL2B.push_back(fDeefit->fLayerEvent.fL2BEMeV[l2bmulti]);
	        }
	      }

	      // 填充 hist_L1_L2F
	      if (fPattern.IsEneConstraint_L2B_L2F(ssdindex, L1L2_EL2B[0], L1L2_EL2F[0]))
	      {
					if (L1L2_StripL1S[0]==L1L2_StripL2B[0]) {
					  if (L2FTime[0]>SiTimeCut_Low[ssdindex][L1L2_StripL2F[0]] && L2FTime[0]<SiTimeCut_Up[ssdindex][L1L2_StripL2F[0]]) {
						  // 用于 “拉直法” 的 DE-E plot
							if (ssdindex==2) {
								h2d_DEE_AllStrip[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
								if (std::find(IsSSD3BadStripF.begin(),IsSSD3BadStripF.end(),L1L2_StripL2F[0]) == IsSSD3BadStripF.end()) h2d_DEE_GoodStrip[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
								if (L1L2_StripL2F[0]<16) {
									h2d_DEE_OneStrip[ssdindex][L1L2_StripL2F[0]]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
									for (Int_t ip=0; ip<iCharge[ssdindex].size(); ip++) {
										if (IsotopeCut[ssdindex][ip]->IsInside(L1L2_EL2F[0], L1L2_EL1S[0])) {
											h2d_DEE_Cut_AllStrip[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
											h2d_DEE_Cut_OneStrip[ssdindex][L1L2_StripL2F[0]]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
											if (std::find(IsSSD3BadStripF.begin(),IsSSD3BadStripF.end(),L1L2_StripL2F[0]) == IsSSD3BadStripF.end()) h2d_DEE_Cut_GoodStrip[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
											getmass = fStraightening.StraighteningGetMass(L1L2_EL2F[0],L1L2_EL1S[0],IsotopeCut[ssdindex],FitPars[ssdindex],iCharge[ssdindex], iMass[ssdindex], geticharge, getimass, pidnum);
											if (getmass > 0.) {
												h1_PIDNum_OneStrip[ssdindex][L1L2_StripL2F[0]]->Fill(5.*geticharge+getmass);
												h1_PIDNum_AllStrip[ssdindex]->Fill(5.*geticharge+getmass);
												if (std::find(IsSSD3BadStripF.begin(),IsSSD3BadStripF.end(),L1L2_StripL2F[0]) == IsSSD3BadStripF.end()) h1_PIDNum_GoodStrip[ssdindex]->Fill(5.*geticharge+getmass);
											//	cout<<"ssd"<<ssdindex<<setw(15)<<"icharge = "<<geticharge<<setw(10)<<"mass = "<<getmass<<setw(10)<<"pid = "<<5.*geticharge+getmass<<endl;
											}
											break;
										}
									}
								}
								// 对应不同 CsI 区域画出 PID
								csinum = fPattern.GetCsINumFromPixel(L1L2_StripL2F[0],L1L2_StripL2B[0]);
								h2d_DEE_OneCsI[ssdindex][csinum]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
								h2d_DEE_AllCsI[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
								if (csinum!=1 && csinum!=4) h2d_DEE_GoodCsI[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
								for (Int_t ip=0; ip<iCharge[ssdindex].size(); ip++) {
									if (IsotopeCut[ssdindex][ip]->IsInside(L1L2_EL2F[0], L1L2_EL1S[0])) {
										h2d_DEE_Cut_OneCsI[ssdindex][csinum]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
										h2d_DEE_Cut_AllCsI[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
										if (csinum!=1 && csinum!=4) h2d_DEE_Cut_GoodCsI[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
										getmass = fStraightening.StraighteningGetMass(L1L2_EL2F[0],L1L2_EL1S[0],IsotopeCut[ssdindex],FitPars[ssdindex],iCharge[ssdindex], iMass[ssdindex], geticharge, getimass, pidnum);
										if (getmass > 0.) {
											h1_PIDNum_OneCsI[ssdindex][csinum]->Fill(5.*geticharge+getmass);
											h1_PIDNum_AllCsI[ssdindex]->Fill(5.*geticharge+getmass);
											if (csinum!=1 && csinum!=4) h1_PIDNum_GoodCsI[ssdindex]->Fill(5.*geticharge+getmass);
										//	cout<<"ssd"<<ssdindex<<setw(15)<<"icharge = "<<geticharge<<setw(10)<<"mass = "<<getmass<<setw(10)<<"pid = "<<5.*geticharge+getmass<<endl;
										}
										break;
									}
								}
							}

							if (ssdindex==3) {
								h2d_DEE_AllStrip[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
								if (std::find(IsSSD4BadStripF.begin(),IsSSD4BadStripF.end(),L1L2_StripL2F[0]) == IsSSD4BadStripF.end()) h2d_DEE_GoodStrip[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
								if (L1L2_StripL2F[0]<16) {
									h2d_DEE_OneStrip[ssdindex][L1L2_StripL2F[0]]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
									for (Int_t ip=0; ip<iCharge[ssdindex].size(); ip++) {
										if (IsotopeCut[ssdindex][ip]->IsInside(L1L2_EL2F[0], L1L2_EL1S[0])) {
											h2d_DEE_Cut_AllStrip[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
											h2d_DEE_Cut_OneStrip[ssdindex][L1L2_StripL2F[0]]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
											if (std::find(IsSSD4BadStripF.begin(),IsSSD4BadStripF.end(),L1L2_StripL2F[0]) == IsSSD4BadStripF.end()) h2d_DEE_Cut_GoodStrip[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
											getmass = fStraightening.StraighteningGetMass(L1L2_EL2F[0],L1L2_EL1S[0],IsotopeCut[ssdindex],FitPars[ssdindex],iCharge[ssdindex], iMass[ssdindex], geticharge, getimass, pidnum);
											if (getmass > 0.) {
												h1_PIDNum_OneStrip[ssdindex][L1L2_StripL2F[0]]->Fill(5.*geticharge+getmass);
												h1_PIDNum_AllStrip[ssdindex]->Fill(5.*geticharge+getmass);
												if (std::find(IsSSD4BadStripF.begin(),IsSSD4BadStripF.end(),L1L2_StripL2F[0]) == IsSSD4BadStripF.end()) h1_PIDNum_GoodStrip[ssdindex]->Fill(5.*geticharge+getmass);
											//	cout<<"ssd"<<ssdindex<<setw(15)<<"icharge = "<<geticharge<<setw(10)<<"mass = "<<getmass<<setw(10)<<"pid = "<<5.*geticharge+getmass<<endl;
											}
											break;
										}
									}
								}
								// 对应不同 CsI 区域画出 PID
								csinum = fPattern.GetCsINumFromPixel(L1L2_StripL2F[0],L1L2_StripL2B[0]);
								h2d_DEE_OneCsI[ssdindex][csinum]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
								h2d_DEE_AllCsI[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
								if (csinum!=0 && csinum!=3 && csinum!=4 && csinum!=6) h2d_DEE_GoodCsI[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
								for (Int_t ip=0; ip<iCharge[ssdindex].size(); ip++) {
									if (IsotopeCut[ssdindex][ip]->IsInside(L1L2_EL2F[0], L1L2_EL1S[0])) {
										h2d_DEE_Cut_OneCsI[ssdindex][csinum]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
										h2d_DEE_Cut_AllCsI[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
										if (csinum!=0 && csinum!=3 && csinum!=4 && csinum!=6) h2d_DEE_Cut_GoodCsI[ssdindex]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
										getmass = fStraightening.StraighteningGetMass(L1L2_EL2F[0],L1L2_EL1S[0],IsotopeCut[ssdindex],FitPars[ssdindex],iCharge[ssdindex], iMass[ssdindex], geticharge, getimass, pidnum);
										if (getmass > 0.) {
											h1_PIDNum_OneCsI[ssdindex][csinum]->Fill(5.*geticharge+getmass);
											h1_PIDNum_AllCsI[ssdindex]->Fill(5.*geticharge+getmass);
											if (csinum!=0 && csinum!=3 && csinum!=4 && csinum!=6) h1_PIDNum_GoodCsI[ssdindex]->Fill(5.*geticharge+getmass);
										//	cout<<"ssd"<<ssdindex<<setw(15)<<"icharge = "<<geticharge<<setw(10)<<"mass = "<<getmass<<setw(10)<<"pid = "<<5.*geticharge+getmass<<endl;
										}
										break;
									}
								}
							}
            }
					}
	      }
	    }
		}
  }

	// 画图
	TCanvas* c_begin = new TCanvas("c_begin","c_begin",600,600);
	TCanvas* c_end = new TCanvas("c_end","c_end",600,600);
	TCanvas* c_csi_begin = new TCanvas("c_csi_begin","c_csi_begin",600,600);
	TCanvas* c_csi_end = new TCanvas("c_csi_end","c_csi_end",600,600);
  //	c_begin->Close();
  //	c_end->Close();
  //	c_csi_begin->Close();
  //	c_csi_end->Close();

  c_begin->Print(pathSSD3SSD4PIDCheckBegin.c_str());
	for (Int_t ssdindex=2; ssdindex<NUM_SSD; ssdindex++) {
		DrawPID(h2d_DEE_AllStrip[ssdindex], h2d_DEE_Cut_AllStrip[ssdindex], h1_PIDNum_AllStrip[ssdindex], pathSSD3SSD4PIDCheck);
		DrawPID(h2d_DEE_GoodStrip[ssdindex],h2d_DEE_Cut_GoodStrip[ssdindex],h1_PIDNum_GoodStrip[ssdindex],pathSSD3SSD4PIDCheck);

		for (Int_t strip=0; strip<NUM_STRIP; strip++) {
			DrawPID(h2d_DEE_OneStrip[ssdindex][strip],h2d_DEE_Cut_OneStrip[ssdindex][strip],h1_PIDNum_OneStrip[ssdindex][strip],pathSSD3SSD4PIDCheck);
		}
	}
	c_end->Print(pathSSD3SSD4PIDCheckEnd.c_str());

  c_csi_begin->Print(pathSSD3SSD4PIDCheckOneCsIBegin.c_str());
	for (Int_t ssdindex=2; ssdindex<NUM_SSD; ssdindex++) {
		DrawPID(h2d_DEE_AllCsI[ssdindex], h2d_DEE_Cut_AllCsI[ssdindex], h1_PIDNum_AllCsI[ssdindex], pathSSD3SSD4PIDCheckOneCsI);
		DrawPID(h2d_DEE_GoodCsI[ssdindex],h2d_DEE_Cut_GoodCsI[ssdindex],h1_PIDNum_GoodCsI[ssdindex],pathSSD3SSD4PIDCheckOneCsI);

		for (Int_t csinum=0; csinum<NUM_CSI; csinum++) {
      // 将每块 CsI 对应的 L1L2_DEE 保存成 .root 文件, 用于SSD3、SDD4的 PID
      //  L1L2_SSD3SSD4_HistData->WriteTObject(h2d_DEE_OneCsI[ssdindex][csinum],Form("h2_dE1_dE2_Tel%d",10*ssdindex+csinum));

	    DrawPID(h2d_DEE_OneCsI[ssdindex][csinum],h2d_DEE_Cut_OneCsI[ssdindex][csinum],h1_PIDNum_OneCsI[ssdindex][csinum],pathSSD3SSD4PIDCheckOneCsI);
		}
	}
	c_csi_end->Print(pathSSD3SSD4PIDCheckOneCsIEnd.c_str());

	//L1L2_SSD3SSD4_HistData->Close();
}


//______________________________________________________________________________
// 检查 L1L2 DEE 图中的 “拐点” 出现的原因
// 发现: 即使 CsI 中的多重性为 0, 任然不能完全消除 "拐点", 现在要做的是检查 “拐点” 现象
//      的来源, 以及如何才能消除 “拐点”
void CSHINECheckDEEPlot::CheckL1L2DEEPunchThrough()
{
	Double_t EL1Range[4] = {400., 320., 100., 100.};
	Int_t    NBinsL1[4]  = {4000, 3200, 1000, 1000};
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2[4]  = {3000, 2500, 1500, 800};

	Int_t csinum = -99;

  std::string pathL1L2PunchThroughCuts(Form("%sStraighteningData/L1L2PunchThroughCuts.root",PATHROOTFILESFOLDER));
	TFile* PunchThroughCuts = new TFile(pathL1L2PunchThroughCuts.c_str(),"READONLY");

	std::string pathCheckL1L2PunchThrough(Form("%sfigure_PID/CheckL1L2DEEPunchThrough.pdf",PATHFIGURESFOLDER));
	std::string pathCheckL1L2PunchThroughBegin(Form("%sfigure_PID/CheckL1L2DEEPunchThrough.pdf[",PATHFIGURESFOLDER));
	std::string pathCheckL1L2PunchThroughEnd(Form("%sfigure_PID/CheckL1L2DEEPunchThrough.pdf]",PATHFIGURESFOLDER));

  TCutG* cuts[NUM_SSD];
	TH2D* h2d_PunchThrough_SiPixelFired[NUM_SSD];
	TH2D* h2d_PunchThrough_CsIPixelFired[NUM_SSD];
	TH2D* h2d_DEE_OneCsI_MCsI0[NUM_SSD][NUM_CSI];
	TH2D* h2d_DEE_OneCsI[NUM_SSD][NUM_CSI];
	TH2D* h2d_DEE_OneCsI_MCsI0_NotGap[NUM_SSD][NUM_CSI];
	for (Int_t i=0; i<NUM_SSD; i++) {
		cuts[i] = (TCutG*) PunchThroughCuts->Get(Form("h2_dE1_dE2_Tel%02d_Z02_A04",i));
		h2d_PunchThrough_SiPixelFired[i]  = new TH2D(Form("SSD%d_PunchThrough_SiPixelFired",i+1),Form("SSD%d_PunchThrough_SiPixelFired",i+1),16,-0.5,15.5,16,-0.5,15.5);
    h2d_PunchThrough_CsIPixelFired[i] = new TH2D(Form("SSD%d_PunchThrough_CsIPixelFired",i+1),Form("SSD%d_PunchThrough_CsIPixelFired",i+1),3,-0.5,2.5,3,-0.5,2.5);

		for (Int_t k=0; k<NUM_CSI; k++) {
			h2d_DEE_OneCsI[i][k] = new TH2D(Form("SSD%d_CsI%d_dE1_E2",i+1,k+1),Form("SSD%d_CsI%d_dE1_E2",i+1,k+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
      h2d_DEE_OneCsI_MCsI0[i][k] = new TH2D(Form("SSD%d_CsI%d_MCsI0",i+1,k+1),Form("SSD%d_CsI%d_MCsI0",i+1,k+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
      h2d_DEE_OneCsI_MCsI0_NotGap[i][k] = new TH2D(Form("SSD%d_CsI%d_MCsI0_NotGap",i+1,k+1),Form("SSD%d_CsI%d_MCsI0_NotGap",i+1,k+1),NBinsL2[i],0,EL2Range[i],NBinsL1[i],0,EL1Range[i]);
		}
	}

  Long64_t nentries = (fDeefit->fChain)->GetEntries();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    (fDeefit->fChain)->GetEntry(ientry);
	//	if (ientry>100) continue;
		timeper.PrintPercentageAndRemainingTime(ientry, nentries);

		for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			if (fDeefit->fLayerEvent.fSSDL1SMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDL2FMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDL2BMulti[ssdindex]==1)
	    {
				L1L2_EL1S.clear();
	      L1L2_EL2F.clear();
	      L1L2_EL2B.clear();
				L1L2_StripL1S.clear();
	      L1L2_StripL2F.clear();
	      L1L2_StripL2B.clear();
				L2FTime.clear();

				// 提取 L1S 的数据
	      for (Int_t l1smulti=0; l1smulti<fDeefit->fLayerEvent.fL1SMulti; l1smulti++) {
	        if (fDeefit->fLayerEvent.fL1SSSDNum[l1smulti] == ssdindex) {
	          L1L2_StripL1S.push_back(fDeefit->fLayerEvent.fL1SNumStrip[l1smulti]);
	          L1L2_EL1S.push_back(fDeefit->fLayerEvent.fL1SEMeV[l1smulti]);
	        }
	      }
				// 提取 L2F 的数据
	      for (Int_t l2fmulti=0; l2fmulti<fDeefit->fLayerEvent.fL2FMulti; l2fmulti++) {
	        if (fDeefit->fLayerEvent.fL2FSSDNum[l2fmulti] == ssdindex) {
	          L1L2_StripL2F.push_back(fDeefit->fLayerEvent.fL2FNumStrip[l2fmulti]);
	          L1L2_EL2F.push_back(fDeefit->fLayerEvent.fL2FEMeV[l2fmulti]);
						L2FTime.push_back(fDeefit->fLayerEvent.fL2FTime[l2fmulti]);
	        }
	      }
	      // 提取 L2B 的数据
	      for (Int_t l2bmulti=0; l2bmulti<fDeefit->fLayerEvent.fL2BMulti; l2bmulti++) {
	        if (fDeefit->fLayerEvent.fL2BSSDNum[l2bmulti] == ssdindex) {
	         L1L2_StripL2B.push_back(fDeefit->fLayerEvent.fL2BNumStrip[l2bmulti]);
	         L1L2_EL2B.push_back(fDeefit->fLayerEvent.fL2BEMeV[l2bmulti]);
	        }
	      }

	      // 填充 hist_L1_L2F
				csinum = fPattern.GetCsINumFromPixel(L1L2_StripL2F[0],L1L2_StripL2B[0]);
	      if (fPattern.IsEneConstraint_L2B_L2F(ssdindex, L1L2_EL2B[0], L1L2_EL2F[0]))
	      {
					if (L1L2_StripL1S[0]==L1L2_StripL2B[0]) {
					  if (L2FTime[0]>SiTimeCut_Low[ssdindex][L1L2_StripL2F[0]] && L2FTime[0]<SiTimeCut_Up[ssdindex][L1L2_StripL2F[0]]) {
							h2d_DEE_OneCsI[ssdindex][csinum]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
							if (fDeefit->fLayerEvent.fSSDCsIMulti[ssdindex]==0) {
								h2d_DEE_OneCsI_MCsI0[ssdindex][csinum]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
								if (cuts[ssdindex]->IsInside(L1L2_EL2F[0], L1L2_EL1S[0])) {
									h2d_PunchThrough_SiPixelFired[ssdindex]->Fill(L1L2_StripL2B[0],L1L2_StripL2F[0]);
									h2d_PunchThrough_CsIPixelFired[ssdindex]->Fill(csinum/3, csinum%3);
								}
								if (!IsPixelOnCsIGap(L1L2_StripL2F[0],L1L2_StripL2B[0])) h2d_DEE_OneCsI_MCsI0_NotGap[ssdindex][csinum]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
							}

            }
					}
	      }
	    }
		}
  }

  TCanvas* c_begin = new TCanvas("c_begin","c_begin",600,600);
	TCanvas* c_end = new TCanvas("c_end","c_end",600,600);
	c_begin->Close();
	c_end->Close();

	TCanvas* c_draw = new TCanvas("c_draw","c_draw",1500,600);
	c_draw->Divide(3,1);

	TCanvas* c_punch = new TCanvas("c_punch","c_punch",1200,600);
	c_punch->Divide(2,1);

  c_begin->Print(pathCheckL1L2PunchThroughBegin.c_str());
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		gStyle->SetOptStat(0);
		c_punch->cd(1);
    h2d_PunchThrough_SiPixelFired[ssdindex]->Draw("COLZ");
    h2d_PunchThrough_SiPixelFired[ssdindex]->GetXaxis()->SetTitle("StripB");
		h2d_PunchThrough_SiPixelFired[ssdindex]->GetXaxis()->CenterTitle(1);
		h2d_PunchThrough_SiPixelFired[ssdindex]->GetXaxis()->SetNdivisions(116);
		h2d_PunchThrough_SiPixelFired[ssdindex]->GetYaxis()->SetTitle("StripB");
		h2d_PunchThrough_SiPixelFired[ssdindex]->GetYaxis()->CenterTitle(1);
		h2d_PunchThrough_SiPixelFired[ssdindex]->GetYaxis()->SetNdivisions(116);

		c_punch->cd(2);
		h2d_PunchThrough_CsIPixelFired[ssdindex]->Draw("COLZ");
    h2d_PunchThrough_CsIPixelFired[ssdindex]->GetXaxis()->SetTitle("CsiNum/3");
		h2d_PunchThrough_CsIPixelFired[ssdindex]->GetXaxis()->CenterTitle(1);
		h2d_PunchThrough_CsIPixelFired[ssdindex]->GetXaxis()->SetNdivisions(103);
		h2d_PunchThrough_CsIPixelFired[ssdindex]->GetYaxis()->SetTitle("CsINum%3");
		h2d_PunchThrough_CsIPixelFired[ssdindex]->GetYaxis()->CenterTitle(1);
		h2d_PunchThrough_CsIPixelFired[ssdindex]->GetYaxis()->SetNdivisions(103);

		gPad->Modified();
		gPad->Update();

		c_punch->Print(pathCheckL1L2PunchThrough.c_str());
	}
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
			c_draw->cd(1);
			h2d_DEE_OneCsI[ssdindex][csiindex]->Draw("COL");
			c_draw->cd(2);
			h2d_DEE_OneCsI_MCsI0[ssdindex][csiindex]->Draw("COL");
			c_draw->cd(3);
			h2d_DEE_OneCsI_MCsI0_NotGap[ssdindex][csiindex]->Draw("COL");

			gPad->Modified();
			gPad->Update();

			c_draw->Print(pathCheckL1L2PunchThrough.c_str());
		}
	}
	c_end->Print(pathCheckL1L2PunchThroughEnd.c_str());
}


//______________________________________________________________________________
// 经过前面的考查: 对于 L1L2 的 DEE,可能需要剔除哪些落在相邻 CsI 缝隙中的条
// 而一旦去掉这些条, 会导致统计量大大减少
// 现在考查这个 gap 效应对 L2L3 的 DEE 的影响, 如果影响不大, 则在 L2L3 不用考虑 gap 效应
void CSHINECheckDEEPlot::CheckL2L3DEECsIGapEffect()
{
	Double_t EL2Range[4] = {300., 250., 150., 80.};
	Int_t    NBinsL2[4]  = {3000, 2500, 1500, 800};

	std::string pathL1L2PunchThroughCuts(Form("%sStraighteningData/L1L2PunchThroughCuts.root",PATHROOTFILESFOLDER));
	TFile* PunchThroughCuts = new TFile(pathL1L2PunchThroughCuts.c_str(),"READONLY");

	std::string pathCheckL2L3GapEffect(Form("%sfigure_PID/CheckL2L3GapEffect.pdf",PATHFIGURESFOLDER));
	std::string pathCheckL2L3GapEffectBegin(Form("%sfigure_PID/CheckL2L3GapEffect.pdf[",PATHFIGURESFOLDER));
	std::string pathCheckL2L3GapEffectEnd(Form("%sfigure_PID/CheckL2L3GapEffect.pdf]",PATHFIGURESFOLDER));

	TH2D* h2d_L2L3DEE[NUM_SSD][NUM_CSI];
	TH2D* h2d_L2L3DEE_CsIGap[NUM_SSD][NUM_CSI];
	for (Int_t i=0; i<NUM_SSD; i++) {
		for (Int_t j=0; j<NUM_CSI; j++) {
			h2d_L2L3DEE[i][j] = new TH2D(Form("SSD%d_CsI%d",i+1,j),Form("SSD%d_CsI%d",i+1,j),4000,0,4000,NBinsL2[i],0,EL2Range[i]);
			h2d_L2L3DEE_CsIGap[i][j] = new TH2D(Form("SSD%d_CsI%d_GapEffect",i+1,j),Form("SSD%d_CsI%d_GapEffect",i+1,j),4000,0,4000,NBinsL2[i],0,EL2Range[i]);
		}
	}

	Long64_t nentries = (fDeefit->fChain)->GetEntries();
	cout<<"nentries = "<<nentries<<endl;
	for (Long64_t ientry=0; ientry<nentries; ientry++) {
		(fDeefit->fChain)->GetEntry(ientry);
	//	if (ientry>100) continue;
		timeper.PrintPercentageAndRemainingTime(ientry, nentries);

		for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
			if (fDeefit->fLayerEvent.fSSDL2FMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDL2BMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDCsIMulti[ssdindex]==1)
			{
				L2L3_EL2F.clear();
			  L2L3_EL2B.clear();
			  L2L3_ECSI.clear();
			  L2L3_StripL2F.clear();
			  L2L3_StripL2B.clear();
			  L2L3_NumCsI.clear();

				// 提取 L2F 的数据
				for (Int_t l2fmulti=0; l2fmulti<fDeefit->fLayerEvent.fL2FMulti; l2fmulti++) {
					if (fDeefit->fLayerEvent.fL2FSSDNum[l2fmulti] == ssdindex) {
						L2L3_StripL2F.push_back(fDeefit->fLayerEvent.fL2FNumStrip[l2fmulti]);
						L2L3_EL2F.push_back(fDeefit->fLayerEvent.fL2FEMeV[l2fmulti]);
					}
				}
				// 提取 L2B 的数据
				for (Int_t l2bmulti=0; l2bmulti<fDeefit->fLayerEvent.fL2BMulti; l2bmulti++) {
					if (fDeefit->fLayerEvent.fL2BSSDNum[l2bmulti] == ssdindex) {
					 L2L3_StripL2B.push_back(fDeefit->fLayerEvent.fL2BNumStrip[l2bmulti]);
					 L2L3_EL2B.push_back(fDeefit->fLayerEvent.fL2BEMeV[l2bmulti]);
					}
				}

				// 提取 CsI 的数据
				for (Int_t csimulti=0; csimulti<fDeefit->fLayerEvent.fCsIMulti; csimulti++) {
					if (fDeefit->fLayerEvent.fCsISSDNum[csimulti] == ssdindex) {
						L2L3_NumCsI.push_back(fDeefit->fLayerEvent.fCsINum[csimulti]);
						L2L3_ECSI.push_back(fDeefit->fLayerEvent.fCsIECh[csimulti]);
					}
				}
        // 填充 hist
				if (fPattern.IsGeoConstraint_L3A_L2B(L2L3_NumCsI[0], L2L3_StripL2B[0]) &&
	          fPattern.IsGeoConstraint_L3A_L2F(L2L3_NumCsI[0], L2L3_StripL2F[0]) &&
	          fPattern.IsEneConstraint_L2B_L2F(ssdindex, L2L3_EL2B[0], L2L3_EL2F[0]))
	      {
					if (fDeefit->fLayerEvent.fSSDL1SMulti[ssdindex]==1) {
						h2d_L2L3DEE[ssdindex][L2L3_NumCsI[0]]->Fill(L2L3_ECSI[0], L2L3_EL2F[0]);
						if (!IsPixelOnCsIGap(L2L3_StripL2F[0],L2L3_StripL2B[0])) h2d_L2L3DEE_CsIGap[ssdindex][L2L3_NumCsI[0]]->Fill(L2L3_ECSI[0], L2L3_EL2F[0]);
					}
	      }
			}
		}
	}

	// TCanvas
	TCanvas* c_begin = new TCanvas("c_begin","c_begin",600,600);
	TCanvas* c_end = new TCanvas("c_end","c_end",600,600);
	TCanvas* cans = new TCanvas("cans","cans",1200,600);
	cans->Divide(2,1);

  c_begin->Print(pathCheckL2L3GapEffectBegin.c_str());
	for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
		for (Int_t csiindex=0; csiindex<NUM_CSI; csiindex++) {
			cans->cd(1);
			h2d_L2L3DEE[ssdindex][csiindex]->Draw("COL");

			cans->cd(2);
			h2d_L2L3DEE_CsIGap[ssdindex][csiindex]->Draw("COL");

			gPad->Modified();
			gPad->Update();

			cans->Print(pathCheckL2L3GapEffect.c_str());
		}
	}
	c_end->Print(pathCheckL2L3GapEffectEnd.c_str());
}
//______________________________________________________________________________



//______________________________________________________________________________
// 修正薄硅条的厚度
// 手动调节厚度, 找到最佳的 DE-E
void CSHINECheckDEEPlot::L1S_FindThicknessUniformity()
{
	Int_t ssdindex = 2; // SSD3
	Int_t ZoneLength = 8;
	Int_t zone_index;
	Int_t num_zones = ZoneLength*ZoneLength;

	Double_t EL2FEL2B_Cut = 0.15;

	Int_t num_factor = 21;
	Double_t factor_min =  -0.05;
	Double_t factor_step = 0.005;

	Int_t    NBinsL1, NBinsL2;
	Double_t EL1Range, EL2Range;
	if (ssdindex == 2) { NBinsL1 = 1000; EL1Range = 100.; NBinsL2 = 1500;  EL2Range = 150.; }
	if (ssdindex == 3) { NBinsL1 = 1000; EL1Range = 100.; NBinsL2 = 1000;  EL2Range = 100.; }

	Double_t thickness_correction[num_factor];
  for (Int_t ifactor=0; ifactor<num_factor; ifactor++) {
		thickness_correction[ifactor] = factor_min + ifactor*factor_step;
	}

	std::string pathPDFOut(Form("%sfigure_PID/SSD%d_L1S_ThicknessUniformity.pdf",PATHFIGURESFOLDER,ssdindex+1));
	std::string pathPDFOutBegin(Form("%sfigure_PID/SSD%d_L1S_ThicknessUniformity.pdf[",PATHFIGURESFOLDER,ssdindex+1));
	std::string pathPDFOutEnd(Form("%sfigure_PID/SSD%d_L1S_ThicknessUniformity.pdf]",PATHFIGURESFOLDER,ssdindex+1));

	TH2D* h2_SSD3_L1L2_DEE[num_zones];
	for (Int_t izone=0; izone<num_zones; izone++) {
		h2_SSD3_L1L2_DEE[izone] = new TH2D(Form("SSD%d_L1L2_DEE_Zone%d",ssdindex+1,izone),Form("SSD%d_L1L2_DEE_Zone%d",ssdindex+1,izone),NBinsL2,0,EL2Range,NBinsL1,0,EL1Range);
	}

	TH2D* h2_SSD3_L1L2_Correction[num_zones-1][num_factor];
	for (Int_t izone=0; izone<(num_zones-1); izone++) {
		for (Int_t ifactor=0; ifactor<num_factor; ifactor++) {
			h2_SSD3_L1L2_Correction[izone][ifactor] = new TH2D(Form("h2_Zone0_Zone%d_%02f",izone+1, thickness_correction[ifactor]),Form("h2_Zone0_Zone%d_%02f",izone+1,thickness_correction[ifactor]),NBinsL2,0,EL2Range,NBinsL1,0,EL1Range);
		}
	}

	TH2D* h2_SSD3_L1L2_DEE_Merged = new TH2D(Form("SSD%d_L1L2_DEE_Merged",ssdindex+1),Form("SSD%d_L1L2_DEE_Merged",ssdindex+1),NBinsL2,0,EL2Range,NBinsL1,0,EL1Range);
	TH2D* h2_SSD3_L1L2_DEE_Corrected = new TH2D(Form("SSD%d_L1L2_DEE_Corrected",ssdindex+1),Form("SSD%d_L1L2_DEE_Corrected",ssdindex+1),NBinsL2,0,EL2Range,NBinsL1,0,EL1Range);

  (fDeefit->fChain)->SetEntries(1000000);
	Long64_t nentries = (fDeefit->fChain)->GetEntries();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    (fDeefit->fChain)->GetEntry(ientry);
		// if (ientry>100) continue;
		timeper.PrintPercentageAndRemainingTime(ientry, nentries);

		if (fDeefit->fLayerEvent.fSSDL1SMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDL2FMulti[ssdindex]==1 &&
	      fDeefit->fLayerEvent.fSSDL2BMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDCsIMulti[ssdindex]==0)
	  {
			L1L2_EL1S.clear();
	    L1L2_EL2F.clear();
	    L1L2_EL2B.clear();
			L1L2_StripL1S.clear();
	    L1L2_StripL2F.clear();
	    L1L2_StripL2B.clear();

			// 提取 L1S 的数据
	    for (Int_t l1smulti=0; l1smulti<fDeefit->fLayerEvent.fL1SMulti; l1smulti++) {
	      if (fDeefit->fLayerEvent.fL1SSSDNum[l1smulti] == ssdindex) {
	        L1L2_StripL1S.push_back(fDeefit->fLayerEvent.fL1SNumStrip[l1smulti]);
	        L1L2_EL1S.push_back(fDeefit->fLayerEvent.fL1SEMeV[l1smulti]);
	      }
	    }
			// 提取 L2F 的数据
	    for (Int_t l2fmulti=0; l2fmulti<fDeefit->fLayerEvent.fL2FMulti; l2fmulti++) {
	      if (fDeefit->fLayerEvent.fL2FSSDNum[l2fmulti] == ssdindex) {
	        L1L2_StripL2F.push_back(fDeefit->fLayerEvent.fL2FNumStrip[l2fmulti]);
		  		L1L2_EL2F.push_back(fDeefit->fLayerEvent.fL2FEMeV[l2fmulti]);
	      }
	    }
	    // 提取 L2B 的数据
	    for (Int_t l2bmulti=0; l2bmulti<fDeefit->fLayerEvent.fL2BMulti; l2bmulti++) {
	      if (fDeefit->fLayerEvent.fL2BSSDNum[l2bmulti] == ssdindex) {
	        L1L2_StripL2B.push_back(fDeefit->fLayerEvent.fL2BNumStrip[l2bmulti]);
	        L1L2_EL2B.push_back(fDeefit->fLayerEvent.fL2BEMeV[l2bmulti]);
	      }
	    }
			if (std::abs(L1L2_EL2B[0]-L1L2_EL2F[0])/L1L2_EL2F[0]<EL2FEL2B_Cut) {
				zone_index = fDeefit->GetZone(L1L2_StripL2B[0], L1L2_StripL2F[0], ZoneLength);
				h2_SSD3_L1L2_DEE[zone_index]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
				h2_SSD3_L1L2_DEE_Merged->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
				for (Int_t ifactor=0; ifactor<num_factor; ifactor++) {
					if (zone_index==0)  {
						for (Int_t izone=0; izone<(num_zones-1); izone++) {
							h2_SSD3_L1L2_Correction[izone][ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
						}
					}
					if (zone_index>0) {
						h2_SSD3_L1L2_Correction[zone_index-1][ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+thickness_correction[ifactor]));
					}
				}
			}
		}
	}
	// 画图
	TCanvas* cans = new TCanvas("cans","cans",1000,1000);
	TCanvas* c_begin = new TCanvas("c_begin","c_bein",600,600);
	TCanvas* c_end = new TCanvas("c_end","c_end",600,600);
  // draw zone
	c_begin->Print(pathPDFOutBegin.c_str());
	for (Int_t izone=0; izone<num_zones; izone++) {
		cans->cd();
		h2_SSD3_L1L2_DEE[izone]->Draw("COL");

		cans->Print(pathPDFOut.c_str());

		gPad->Modified();
		gPad->Update();
	}
  // draw factor
	for (Int_t izone=0; izone<(num_zones-1); izone++) {
		for (Int_t ifactor=0; ifactor<num_factor; ifactor++) {
			cans->cd();
			h2_SSD3_L1L2_Correction[izone][ifactor]->Draw("COL");

			cans->Print(pathPDFOut.c_str());

			gPad->Modified();
			gPad->Update();
		}
	}

	// draw merged
	cans->cd();
	h2_SSD3_L1L2_DEE_Merged->Draw("COL");
	cans->Print(pathPDFOut.c_str());

	c_end->Print(pathPDFOutEnd.c_str());
}
//______________________________________________________________________________


void CSHINECheckDEEPlot::L1S_DetermineThicknessUniformity()
{
	Int_t ssdindex = 2; // 2-SSD3 ,3-SSD4
	Int_t ZoneLength = 8;
	Int_t zone_index;
	Int_t num_zones = ZoneLength*ZoneLength;
	Double_t EL2FEL2B_Cut = 0.15;

	Int_t    NBinsL1, NBinsL2;
	Double_t EL1Range, EL2Range;
	if (ssdindex == 2) { NBinsL1 = 1200; EL1Range = 120.; NBinsL2 = 1200; EL2Range = 120.; }
	if (ssdindex == 3) { NBinsL1 = 900;  EL1Range = 90.;  NBinsL2 = 700;  EL2Range = 70.; }

  Double_t SSD3_L1S_corrfatcor[64] = {  0.000, 0.000, 0.005, 0.015, 0.040, 0.045, 0.050, 0.055,
		                                    0.000, 0.000, 0.000, 0.000, 0.020, 0.040, 0.050, 0.055,
																      	0.000, 0.000, 0.000, 0.000, 0.010, 0.030, 0.040, 0.055,
														  	       -0.010, 0.000, 0.000,-0.005,-0.010, 0.030, 0.040, 0.050,
																	      0.000, 0.000, 0.005, 0.020, 0.030, 0.040, 0.050, 0.050,
																	      0.000, 0.000, 0.020, 0.020, 0.030, 0.040, 0.045, 0.055,
																	      0.000, 0.000, 0.020, 0.020, 0.035, 0.040, 0.050, 0.055,
																        0.000, 0.015, 0.030, 0.030, 0.050, 0.050, 0.050, 0.060
																     };

	Double_t SSD4_L1S_corrfatcor[64] = {  0.000, 0.020, 0.015,-0.020,-0.020,-0.020,-0.150,-0.050,
	                                      0.000, 0.000, 0.000, 0.000,-0.020,-0.020,-0.030,-0.040,
																			  0.015,-0.010,-0.020, 0.000,-0.010,-0.020,-0.015,-0.030,
																			  0.030, 0.000, 0.000, 0.010, 0.020, 0.000, 0.000,-0.020,
																			  0.020, 0.000, 0.000, 0.010, 0.010, 0.000,-0.020,-0.025,
																			  0.030, 0.020, 0.010,-0.010, 0.005, 0.000,-0.020,-0.035,
																			  0.030, 0.020, 0.010,-0.010, 0.005,-0.010,-0.020,-0.040,
																			  0.025, 0.000, 0.000,-0.010, 0.000,-0.010,-0.020,-0.040
																		 };

  Double_t L1S_corrfatcor[num_zones];
	for (Int_t izone=0; izone<num_zones; izone++) {
		if (ssdindex == 2) L1S_corrfatcor[izone] = SSD3_L1S_corrfatcor[izone];
		if (ssdindex == 3) L1S_corrfatcor[izone] = SSD4_L1S_corrfatcor[izone];
	}

	std::string pathPDFOut(Form("%sfigure_PID/SSD%d_L1S_ThicknessUniformity_Corrected.png",PATHFIGURESFOLDER,ssdindex+1));
	std::string pathROOTout(Form("%sSSD3_SSD4_L1L2_DEE_ThicknessCorrection.root",PATHROOTFILESFOLDER));
	TFile* myfile = new TFile(pathROOTout.c_str(),"RECREATE");

	TH2D* h2_SSD3_L1L2_DEE_Merged = new TH2D(Form("SSD%d_L1L2_DEE_Merged",ssdindex+1),Form("SSD%d_L1L2_DEE_Merged",ssdindex+1),NBinsL2,0,EL2Range,NBinsL1,0,EL1Range);
	TH2D* h2_SSD3_L1L2_DEE_Corrected = new TH2D(Form("SSD%d_L1L2_DEE_Corrected",ssdindex+1),Form("SSD%d_L1L2_DEE_Corrected",ssdindex+1),NBinsL2,0,EL2Range,NBinsL1,0,EL1Range);

	TH2D* h2_ThicknessCorrection = new TH2D(Form("SSD%d_ThicknessVariation",ssdindex+1),Form("SSD%d_ThicknessVariation",ssdindex+1),ZoneLength,0,ZoneLength,ZoneLength,0,ZoneLength);
	for (Int_t izonex=0; izonex<ZoneLength; izonex++) {
		for (Int_t izoney=0; izoney<ZoneLength; izoney++) {
			h2_ThicknessCorrection->SetBinContent(izonex,izoney,L1S_corrfatcor[izonex*ZoneLength+izoney]*100);
		}
	}

  (fDeefit->fChain)->SetEntries(20000000);
	Long64_t nentries = (fDeefit->fChain)->GetEntries();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    (fDeefit->fChain)->GetEntry(ientry);
		// if (ientry>100) continue;
		timeper.PrintPercentageAndRemainingTime(ientry, nentries);

		if (fDeefit->fLayerEvent.fSSDL1SMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDL2FMulti[ssdindex]==1 &&
	      fDeefit->fLayerEvent.fSSDL2BMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDCsIMulti[ssdindex]==0)
	  {
			L1L2_EL1S.clear();
	    L1L2_EL2F.clear();
	    L1L2_EL2B.clear();
			L1L2_StripL1S.clear();
	    L1L2_StripL2F.clear();
	    L1L2_StripL2B.clear();
			L2FTime.clear();

			// 提取 L1S 的数据
	    for (Int_t l1smulti=0; l1smulti<fDeefit->fLayerEvent.fL1SMulti; l1smulti++) {
	      if (fDeefit->fLayerEvent.fL1SSSDNum[l1smulti] == ssdindex) {
	        L1L2_StripL1S.push_back(fDeefit->fLayerEvent.fL1SNumStrip[l1smulti]);
	        L1L2_EL1S.push_back(fDeefit->fLayerEvent.fL1SEMeV[l1smulti]);
	      }
	    }
			// 提取 L2F 的数据
	    for (Int_t l2fmulti=0; l2fmulti<fDeefit->fLayerEvent.fL2FMulti; l2fmulti++) {
	      if (fDeefit->fLayerEvent.fL2FSSDNum[l2fmulti] == ssdindex) {
	        L1L2_StripL2F.push_back(fDeefit->fLayerEvent.fL2FNumStrip[l2fmulti]);
		  		L1L2_EL2F.push_back(fDeefit->fLayerEvent.fL2FEMeV[l2fmulti]);
					L2FTime.push_back(fDeefit->fLayerEvent.fL2FTime[l2fmulti]);
	      }
	    }
	    // 提取 L2B 的数据
	    for (Int_t l2bmulti=0; l2bmulti<fDeefit->fLayerEvent.fL2BMulti; l2bmulti++) {
	      if (fDeefit->fLayerEvent.fL2BSSDNum[l2bmulti] == ssdindex) {
	        L1L2_StripL2B.push_back(fDeefit->fLayerEvent.fL2BNumStrip[l2bmulti]);
	        L1L2_EL2B.push_back(fDeefit->fLayerEvent.fL2BEMeV[l2bmulti]);
	      }
	    }
			if (std::abs(L1L2_EL2B[0]-L1L2_EL2F[0])/L1L2_EL2F[0]<EL2FEL2B_Cut) {
				zone_index = fDeefit->GetZone(L1L2_StripL2B[0], L1L2_StripL2F[0], ZoneLength);
				if (L2FTime[0]>SiTimeCut_Low[ssdindex][L1L2_StripL2F[0]] && L2FTime[0]<SiTimeCut_Up[ssdindex][L1L2_StripL2F[0]]) {
					if ((L1L2_StripL2B[0]!=0)&&(L1L2_StripL2B[0]!=4)&&(L1L2_StripL2B[0]!=5)&&(L1L2_StripL2B[0]!=10)&&(L1L2_StripL2B[0]!=11)&&(L1L2_StripL2B[0]!=15) &&
				      (L1L2_StripL2F[0]!=0)&&(L1L2_StripL2F[0]!=4)&&(L1L2_StripL2F[0]!=5)&&(L1L2_StripL2F[0]!=10)&&(L1L2_StripL2F[0]!=11)&&(L1L2_StripL2F[0]!=15))
					{
						h2_SSD3_L1L2_DEE_Merged->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
						h2_SSD3_L1L2_DEE_Corrected->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+L1S_corrfatcor[zone_index]));
					}
				}
			}
		}
	}
	// 画图
	TCanvas* cans = new TCanvas("cans","cans",1500,600);
	cans->Divide(3,1);

	// draw merged
	cans->cd(1);
	gPad->SetLeftMargin(0.12);
	h2_SSD3_L1L2_DEE_Merged->Draw("COL");
	h2_SSD3_L1L2_DEE_Merged->GetXaxis()->SetTitle("EL2F (MeV)");
	h2_SSD3_L1L2_DEE_Merged->GetXaxis()->CenterTitle();
	h2_SSD3_L1L2_DEE_Merged->GetYaxis()->SetTitle("EL1S (MeV)");
	h2_SSD3_L1L2_DEE_Merged->GetYaxis()->CenterTitle();

	cans->cd(2);
	gPad->SetLeftMargin(0.12);
	h2_SSD3_L1L2_DEE_Corrected->Draw("COL");
	h2_SSD3_L1L2_DEE_Corrected->GetXaxis()->SetTitle("EL2F (MeV)");
	h2_SSD3_L1L2_DEE_Corrected->GetXaxis()->CenterTitle();
	h2_SSD3_L1L2_DEE_Corrected->GetYaxis()->SetTitle("EL1S (MeV)");
	h2_SSD3_L1L2_DEE_Corrected->GetYaxis()->CenterTitle();

  cans->cd(3);
	h2_ThicknessCorrection->Draw("LEGO");
	h2_ThicknessCorrection->GetXaxis()->SetTitle("StripB_Zone");
	h2_ThicknessCorrection->GetXaxis()->CenterTitle();
	h2_ThicknessCorrection->GetXaxis()->SetTitleOffset(1.5);
	h2_ThicknessCorrection->GetYaxis()->SetTitle("StripF_Zone");
	h2_ThicknessCorrection->GetYaxis()->CenterTitle();
	h2_ThicknessCorrection->GetYaxis()->SetTitleOffset(1.5);
	h2_ThicknessCorrection->GetZaxis()->SetTitle("Thickness Variation (%)");
	h2_ThicknessCorrection->GetZaxis()->CenterTitle();

	cans->Print(pathPDFOut.c_str());

	// 写入 root file
	myfile->WriteTObject(h2_SSD3_L1L2_DEE_Merged,    h2_SSD3_L1L2_DEE_Merged->GetName());
	myfile->WriteTObject(h2_SSD3_L1L2_DEE_Corrected, h2_SSD3_L1L2_DEE_Corrected->GetName());
	myfile->WriteTObject(h2_ThicknessCorrection,     h2_ThicknessCorrection->GetName());
 //	myfile->Close();
}


/*
//______________________________________________________________________________
void CSHINECheckDEEPlot::SSD4L1S_ThicknessUniformity()
{

  Int_t    NBinsL1 = 1000;
	Double_t EL1Range = 100.;
	Int_t    NBinsL2 = 1500;
	Double_t EL2Range = 150.;

	Int_t zone_index;

  Int_t ssdindex = 3; // SSD3
  Double_t EL2FEL2B_Cut = 0.15;

	Int_t num_factor = 21;
	Double_t thickness_correction[21] = {-0.05,-0.045,-0.04,-0.035,-0.03,-0.025,-0.02,-0.015,-0.01,-0.005,
		                                    0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05};

	std::string pathPDFOut(Form("%sfigure_PID/SSD4L1S_ThicknessUniformity.pdf",PATHFIGURESFOLDER));
	std::string pathPDFOutBegin(Form("%sfigure_PID/SSD4L1S_ThicknessUniformity.pdf[",PATHFIGURESFOLDER));
	std::string pathPDFOutEnd(Form("%sfigure_PID/SSD4L1S_ThicknessUniformity.pdf]",PATHFIGURESFOLDER));

	TH2D* h2_SSD4_L1L2_DEE[16];
	for (Int_t izone=0; izone<16; izone++) {
		h2_SSD4_L1L2_DEE[izone] = new TH2D(Form("SSD4_L1L2_DEE_Zone%d",izone),Form("SSD4_L1L2_DEE_Zone%d",izone),NBinsL2,0,EL2Range,NBinsL1,0,EL1Range);
	}

	TH2D* h2_SSD4_L1L2_Correction[num_factor];
	for (Int_t ifactor=0; ifactor<num_factor; ifactor++) {
		h2_SSD4_L1L2_Correction[ifactor] = new TH2D(Form("h2_Correction%02f",thickness_correction[ifactor]),Form("h2_Correction%02f",thickness_correction[ifactor]),NBinsL2,0,EL2Range,NBinsL1,0,EL1Range);
	}

	TH2D* h2_SSD4_L1L2_DEE_Merged = new TH2D("SSD4_L1L2_DEE_Merged","SSD4_L1L2_DEE_Merged",NBinsL2,0,EL2Range,NBinsL1,0,EL1Range);

  (fDeefit->fChain)->SetEntries(10000000);
	Long64_t nentries = (fDeefit->fChain)->GetEntries();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    (fDeefit->fChain)->GetEntry(ientry);
		// if (ientry>100) continue;
		timeper.PrintPercentageAndRemainingTime(ientry, nentries);

		if (fDeefit->fLayerEvent.fSSDL1SMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDL2FMulti[ssdindex]==1 &&
	      fDeefit->fLayerEvent.fSSDL2BMulti[ssdindex]==1 && fDeefit->fLayerEvent.fSSDCsIMulti[ssdindex]==0)
	  {
			L1L2_EL1S.clear();
	    L1L2_EL2F.clear();
	    L1L2_EL2B.clear();
			L1L2_StripL1S.clear();
	    L1L2_StripL2F.clear();
	    L1L2_StripL2B.clear();

			// 提取 L1S 的数据
	    for (Int_t l1smulti=0; l1smulti<fDeefit->fLayerEvent.fL1SMulti; l1smulti++) {
	      if (fDeefit->fLayerEvent.fL1SSSDNum[l1smulti] == ssdindex) {
	        L1L2_StripL1S.push_back(fDeefit->fLayerEvent.fL1SNumStrip[l1smulti]);
	        L1L2_EL1S.push_back(fDeefit->fLayerEvent.fL1SEMeV[l1smulti]);
	      }
	    }
			// 提取 L2F 的数据
	    for (Int_t l2fmulti=0; l2fmulti<fDeefit->fLayerEvent.fL2FMulti; l2fmulti++) {
	      if (fDeefit->fLayerEvent.fL2FSSDNum[l2fmulti] == ssdindex) {
	        L1L2_StripL2F.push_back(fDeefit->fLayerEvent.fL2FNumStrip[l2fmulti]);
		  		L1L2_EL2F.push_back(fDeefit->fLayerEvent.fL2FEMeV[l2fmulti]);
	      }
	    }
	    // 提取 L2B 的数据
	    for (Int_t l2bmulti=0; l2bmulti<fDeefit->fLayerEvent.fL2BMulti; l2bmulti++) {
	      if (fDeefit->fLayerEvent.fL2BSSDNum[l2bmulti] == ssdindex) {
	        L1L2_StripL2B.push_back(fDeefit->fLayerEvent.fL2BNumStrip[l2bmulti]);
	        L1L2_EL2B.push_back(fDeefit->fLayerEvent.fL2BEMeV[l2bmulti]);
	      }
	    }
			if (std::abs(L1L2_EL2B[0]-L1L2_EL2F[0])/L1L2_EL2F[0]<EL2FEL2B_Cut) {
				zone_index = fDeefit->GetZone(L1L2_StripL2B[0], L1L2_StripL2F[0]);
				h2_SSD4_L1L2_DEE[zone_index]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
				h2_SSD4_L1L2_DEE_Merged->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);

				for (Int_t ifactor=0; ifactor<num_factor; ifactor++) {
					if (zone_index==0) h2_SSD4_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
					if (zone_index==1) h2_SSD4_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
					if (zone_index==2) h2_SSD4_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+0.040));
					if (zone_index==3) h2_SSD4_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+0.050));
					if (zone_index==4) h2_SSD4_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1-0.005));
					if (zone_index==5) h2_SSD4_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
					if (zone_index==6) h2_SSD4_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+0.03));
					if (zone_index==7) h2_SSD4_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+0.035));

				//	if (zone_index==8) h2_SSD3_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+thickness_correction[ifactor]));
				//	if (zone_index==9) h2_SSD3_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+thickness_correction[ifactor]));
				//	if (zone_index==10) h2_SSD3_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+thickness_correction[ifactor]));
				//	if (zone_index==11) h2_SSD3_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+thickness_correction[ifactor]));
				//	if (zone_index==12) h2_SSD3_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+thickness_correction[ifactor]));
				//	if (zone_index==13) h2_SSD3_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+thickness_correction[ifactor]));
				//	if (zone_index==14) h2_SSD3_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+thickness_correction[ifactor]));
				//	if (zone_index==15) h2_SSD3_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+thickness_correction[ifactor]));
				}
			}
		}
	}
	// 画图
	TCanvas* cans = new TCanvas("cans","cans",1000,1000);
	TCanvas* c_begin = new TCanvas("c_begin","c_bein",600,600);
	TCanvas* c_end = new TCanvas("c_end","c_end",600,600);

	c_begin->Print(pathPDFOutBegin.c_str());
	for (Int_t izone=0; izone<16; izone++) {
		cans->cd();
		h2_SSD4_L1L2_DEE[izone]->Draw("COL");

		cans->Print(pathPDFOut.c_str());

		gPad->Modified();
		gPad->Update();
	}
	for (Int_t ifactor=0; ifactor<num_factor; ifactor++) {
		cans->cd();
		h2_SSD4_L1L2_Correction[ifactor]->Draw("COL");

		cans->Print(pathPDFOut.c_str());

		gPad->Modified();
		gPad->Update();
	}

	cans->cd();
	h2_SSD4_L1L2_DEE_Merged->Draw("COL");
	cans->Print(pathPDFOut.c_str());

	c_end->Print(pathPDFOutEnd.c_str());
}
*/



/*
if (zone_index==0)  h2_SSD3_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
if (zone_index==1)  h2_SSD3_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+0.013));
//	if (zone_index==2)  h2_SSD3_L1L2_Correction[ifactor]-->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+thickness_correction[ifactor])); !!
//	if (zone_index==3)  h2_SSD3_L1L2_Correction[ifactor]-->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+0.05)); !!

//	if (zone_index==4)  h2_SSD3_L1L2_Correction[ifactor]-->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1-0.005)); !!
//	if (zone_index==5)  h2_SSD3_L1L2_Correction[ifactor]-->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);           !!
//	if (zone_index==6)  h2_SSD3_L1L2_Correction[ifactor]-->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+0.03));  !!
if (zone_index==7)  h2_SSD3_L1L2_Correction[ifactor]->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+thickness_correction[ifactor]));
//	if (zone_index==8)  h2_SSD3_L1L2_Correction[ifactor]-->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
//	if (zone_index==9)  h2_SSD3_L1L2_Correction[ifactor]-->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+0.02));
//	if (zone_index==10) h2_SSD3_L1L2_Correction[ifactor]-->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+0.035));
//	if (zone_index==11) h2_SSD3_L1L2_Correction[ifactor]-->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+0.045));
//	if (zone_index==12) h2_SSD3_L1L2_Correction[ifactor]-->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+0.015));
//	if (zone_index==13) h2_SSD3_L1L2_Correction[ifactor]-->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+0.03));
//	if (zone_ex==14) h2_SSD3_L1L2_Correction[ifactor]-->Fill(L1L2_EL2F[0], L1L2_EL1S[0]*(1+0.04));
//	if (zone_index==15) h2_SSD3_L1L2_Correction[ifactor]-->Fill(L1L2_EL2F[0], L1L2_EL1S[0]);
*/
