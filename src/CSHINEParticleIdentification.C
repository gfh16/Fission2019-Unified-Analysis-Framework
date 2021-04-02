# include "../include/CSHINEParticleIdentification.h"
using namespace std;


//______________________________________________________________________________
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
  fCurrent = -1;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("LayerEvent.fL1SMulti",    &LayerEvent_fL1SMulti);
  fChain->SetBranchAddress("LayerEvent.fL1SSSDNum",   &LayerEvent_fL1SSSDNum);
  fChain->SetBranchAddress("LayerEvent.fL1SNumStrip", &LayerEvent_fL1SNumStrip);
  fChain->SetBranchAddress("LayerEvent.fL1SEMeV",     &LayerEvent_fL1SEMeV);
  fChain->SetBranchAddress("LayerEvent.fL2FMulti",    &LayerEvent_fL2FMulti);
  fChain->SetBranchAddress("LayerEvent.fL2FSSDNum",   &LayerEvent_fL2FSSDNum);
  fChain->SetBranchAddress("LayerEvent.fL2FNumStrip", &LayerEvent_fL2FNumStrip);
  fChain->SetBranchAddress("LayerEvent.fL2FEMeV",     &LayerEvent_fL2FEMeV);
	fChain->SetBranchAddress("LayerEvent.fL2FTime",     &LayerEvent_fL2FTime);
  fChain->SetBranchAddress("LayerEvent.fL2BMulti",    &LayerEvent_fL2BMulti);
  fChain->SetBranchAddress("LayerEvent.fL2BSSDNum",   &LayerEvent_fL2BSSDNum);
  fChain->SetBranchAddress("LayerEvent.fL2BNumStrip", &LayerEvent_fL2BNumStrip);
  fChain->SetBranchAddress("LayerEvent.fL2BEMeV",     &LayerEvent_fL2BEMeV);
  fChain->SetBranchAddress("LayerEvent.fCsIMulti",    &LayerEvent_fCsIMulti);
  fChain->SetBranchAddress("LayerEvent.fCsISSDNum",   &LayerEvent_fCsISSDNum);
  fChain->SetBranchAddress("LayerEvent.fCsINum",      &LayerEvent_fCsINum);
  fChain->SetBranchAddress("LayerEvent.fCsIECh",      &LayerEvent_fCsIECh);
  fChain->SetBranchAddress("LayerEvent.fSSDL1SMulti", &LayerEvent_fSSDL1SMulti);
  fChain->SetBranchAddress("LayerEvent.fSSDL2FMulti", &LayerEvent_fSSDL2FMulti);
  fChain->SetBranchAddress("LayerEvent.fSSDL2BMulti", &LayerEvent_fSSDL2BMulti);
  fChain->SetBranchAddress("LayerEvent.fSSDCsIMulti", &LayerEvent_fSSDCsIMulti);
}
//______________________________________________________________________________



//______________________________________________________________________________
// 在本次实验中, 将借助 DEEFIT 工具进行 CsI 晶体的能量刻度
// 由于 DEEFIT 对数据格式有要求, 因此首先需要将实验数据存成 DEEFIT 格式： 3个branch
// tree->SetBranch("numtel", &numtel, "numtel/s");
// tree->SetBranch("desipgf",&desipgf,"desipgf/f");
// ree->SetBranch("fastpg",  &fastpg, "fastpg/s");
void CSHINEDEEFITPID::DEEFITGenerateData()
{
  std::string pathrootfileout(Form("/home/sea/Fission2019_Data/DEEFITData_Run%04d-%04d.root", fFirstRun, fLastRun));

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
			if (LayerEvent_fSSDCsIMulti[ssdindex]==1 && LayerEvent_fSSDL2BMulti[ssdindex]==1 &&
	        LayerEvent_fSSDL2FMulti[ssdindex]==1)
	    {
	      // 提取 CsI 的数据
	      for (Int_t csimulti=0; csimulti<LayerEvent_fCsIMulti; csimulti++) {
	        if (LayerEvent_fCsISSDNum[csimulti] == ssdindex) {
	          L2L3_NumCsI.push_back(LayerEvent_fCsINum[csimulti]);
	          L2L3_ECSI.push_back(LayerEvent_fCsIECh[csimulti]);
	        }
	      }
	      // 提取 L2B 的数据
	      for (Int_t l2bmulti=0; l2bmulti<LayerEvent_fL2BMulti; l2bmulti++) {
	        if (LayerEvent_fL2BSSDNum[l2bmulti] == ssdindex) {
	         L2L3_StripL2B.push_back(LayerEvent_fL2BNumStrip[l2bmulti]);
	         L2L3_EL2B.push_back(LayerEvent_fL2BEMeV[l2bmulti]);
	        }
	      }
	      // 提取 L2F 的数据
	      for (Int_t l2fmulti=0; l2fmulti<LayerEvent_fL2FMulti; l2fmulti++) {
	        if (LayerEvent_fL2FSSDNum[l2fmulti] == ssdindex) {
	          L2L3_StripL2F.push_back(LayerEvent_fL2FNumStrip[l2fmulti]);
	          L2L3_EL2F.push_back(LayerEvent_fL2FEMeV[l2fmulti]);
	        }
	      }
	      // 填充 hist_L2B_L3
	      if (fPattern.IsGeoConstraint_L3A_L2B(L2L3_NumCsI[0], L2L3_StripL2B[0]) &&
	          fPattern.IsGeoConstraint_L3A_L2F(L2L3_NumCsI[0], L2L3_StripL2F[0]) &&
	          fPattern.IsEneConstraint_L2B_L2F(ssdindex, L2L3_EL2B[0], L2L3_EL2F[0]))
	      {
					fdeefitdata->numtel  = (UShort_t)ssdindex*9 + L2L3_NumCsI[0];
		      fdeefitdata->desipgf = (Float_t)  L2L3_EL2F[0];
		      fdeefitdata->fastpg  = (UShort_t) L2L3_ECSI[0];
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
	return Z_charge + 0.2*(A_mass - 2*Z_charge);
}



//______________________________________________________________________________
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

	std::vector<Double_t> L2L3_EL2F;
  std::vector<Double_t> L2L3_EL2B;
  std::vector<Double_t> L2L3_ECSI;
  std::vector<Int_t>    L2L3_StripL2F;
  std::vector<Int_t>    L2L3_StripL2B;
  std::vector<Int_t>    L2L3_NumCsI;

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
			if (fDeefit->LayerEvent_fSSDCsIMulti[ssdindex]==1 && fDeefit->LayerEvent_fSSDL2BMulti[ssdindex]==1 &&
	        fDeefit->LayerEvent_fSSDL2FMulti[ssdindex]==1)
	    {
	      // 提取 CsI 的数据
	      for (Int_t csimulti=0; csimulti<fDeefit->LayerEvent_fCsIMulti; csimulti++) {
	        if (fDeefit->LayerEvent_fCsISSDNum[csimulti] == ssdindex) {
	          L2L3_NumCsI.push_back(fDeefit->LayerEvent_fCsINum[csimulti]);
	          L2L3_ECSI.push_back(fDeefit->LayerEvent_fCsIECh[csimulti]);
	        }
	      }
	      // 提取 L2B 的数据
	      for (Int_t l2bmulti=0; l2bmulti<fDeefit->LayerEvent_fL2BMulti; l2bmulti++) {
	        if (fDeefit->LayerEvent_fL2BSSDNum[l2bmulti] == ssdindex) {
	         L2L3_StripL2B.push_back(fDeefit->LayerEvent_fL2BNumStrip[l2bmulti]);
	         L2L3_EL2B.push_back(fDeefit->LayerEvent_fL2BEMeV[l2bmulti]);
	        }
	      }
	      // 提取 L2F 的数据
	      for (Int_t l2fmulti=0; l2fmulti<fDeefit->LayerEvent_fL2FMulti; l2fmulti++) {
	        if (fDeefit->LayerEvent_fL2FSSDNum[l2fmulti] == ssdindex) {
	          L2L3_StripL2F.push_back(fDeefit->LayerEvent_fL2FNumStrip[l2fmulti]);
	          L2L3_EL2F.push_back(fDeefit->LayerEvent_fL2FEMeV[l2fmulti]);
	        }
	      }
	      // 填充 hist_L2B_L3
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
// 产生 DEEFIT 数据前, 先检查 L1L2 的能量关联是否正常
void CSHINECheckDEEPlot::CheckL1L2DEE()
{
	Double_t EL1_Range[4] = {200.,200.,100.,70.};
	Double_t EL2_Range[4] = {250.,200.,140.,70.};
	Int_t EL1_NBins[4] = {2000,2000,1000,700};
	Int_t EL2_NBins[4] = {2500,2000,1400,700};

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

	std::vector<Double_t> L1L2_EL1S;
  std::vector<Double_t> L1L2_EL2F;
  std::vector<Double_t> L1L2_EL2B;
	std::vector<Int_t>    L1L2_StripL1S;
  std::vector<Int_t>    L1L2_StripL2F;
  std::vector<Int_t>    L1L2_StripL2B;
	std::vector<Int_t>    L2FTime;

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
			if (fDeefit->LayerEvent_fSSDCsIMulti[ssdindex]==0 && fDeefit->LayerEvent_fSSDL2BMulti[ssdindex]==1 &&
	        fDeefit->LayerEvent_fSSDL2FMulti[ssdindex]==1 && fDeefit->LayerEvent_fSSDL1SMulti[ssdindex]==1)
	    {
	      // 提取 L2B 的数据
	      for (Int_t l2bmulti=0; l2bmulti<fDeefit->LayerEvent_fL2BMulti; l2bmulti++) {
	        if (fDeefit->LayerEvent_fL2BSSDNum[l2bmulti] == ssdindex) {
	         L1L2_StripL2B.push_back(fDeefit->LayerEvent_fL2BNumStrip[l2bmulti]);
	         L1L2_EL2B.push_back(fDeefit->LayerEvent_fL2BEMeV[l2bmulti]);
	        }
	      }
	      // 提取 L2F 的数据
	      for (Int_t l2fmulti=0; l2fmulti<fDeefit->LayerEvent_fL2FMulti; l2fmulti++) {
	        if (fDeefit->LayerEvent_fL2FSSDNum[l2fmulti] == ssdindex) {
	          L1L2_StripL2F.push_back(fDeefit->LayerEvent_fL2FNumStrip[l2fmulti]);
	          L1L2_EL2F.push_back(fDeefit->LayerEvent_fL2FEMeV[l2fmulti]);
						L2FTime.push_back(fDeefit->LayerEvent_fL2FTime[l2fmulti]);
	        }
	      }
				// 提取 L1S 的数据
	      for (Int_t l1smulti=0; l1smulti<fDeefit->LayerEvent_fL1SMulti; l1smulti++) {
	        if (fDeefit->LayerEvent_fL1SSSDNum[l1smulti] == ssdindex) {
	          L1L2_StripL1S.push_back(fDeefit->LayerEvent_fL1SNumStrip[l1smulti]);
	          L1L2_EL1S.push_back(fDeefit->LayerEvent_fL1SEMeV[l1smulti]);
	        }
	      }
	      // 填充 hist_L1_L2F
	      if (fPattern.IsEneConstraint_L2B_L2F(ssdindex, L1L2_EL2B[0], L1L2_EL2F[0])) {
					for (Int_t l1smulti=0; l1smulti<fDeefit->LayerEvent_fSSDL1SMulti[ssdindex]; l1smulti++) {
						if (abs(L1L2_StripL1S[l1smulti]-L1L2_StripL2B[0])<=1) {
							if (L2FTime[0]>TimeCut_Low[ssdindex][L1L2_StripL2F[0]] && L2FTime[0]<TimeCut_Up[ssdindex][L1L2_StripL2F[0]]) {
								hist_L1L2_Merged[ssdindex]->Fill(L1L2_EL2B[0], L1L2_EL1S[l1smulti]);
				        hist_L1L2[ssdindex][L1L2_StripL2F[0]]->Fill(L1L2_EL2B[0], L1L2_EL1S[l1smulti]);
							}
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
	c_merged->Print("L1L2F_DEE_Merged.pdf");
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

	std::vector<Double_t> L1L2_EL1S;
  std::vector<Double_t> L1L2_EL2F;
  std::vector<Double_t> L1L2_EL2B;
	std::vector<Int_t>    L1L2_StripL1S;
  std::vector<Int_t>    L1L2_StripL2F;
  std::vector<Int_t>    L1L2_StripL2B;
	std::vector<Int_t>    L2FTime;

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
			if (fDeefit->LayerEvent_fSSDCsIMulti[ssdindex]==0 && fDeefit->LayerEvent_fSSDL2BMulti[ssdindex]==1 &&
	        fDeefit->LayerEvent_fSSDL2FMulti[ssdindex]==1 && fDeefit->LayerEvent_fSSDL1SMulti[ssdindex]==1)
	    {
	      // 提取 L2B 的数据
	      for (Int_t l2bmulti=0; l2bmulti<fDeefit->LayerEvent_fL2BMulti; l2bmulti++) {
	        if (fDeefit->LayerEvent_fL2BSSDNum[l2bmulti] == ssdindex) {
	         L1L2_StripL2B.push_back(fDeefit->LayerEvent_fL2BNumStrip[l2bmulti]);
	         L1L2_EL2B.push_back(fDeefit->LayerEvent_fL2BEMeV[l2bmulti]);
	        }
	      }
	      // 提取 L2F 的数据
	      for (Int_t l2fmulti=0; l2fmulti<fDeefit->LayerEvent_fL2FMulti; l2fmulti++) {
	        if (fDeefit->LayerEvent_fL2FSSDNum[l2fmulti] == ssdindex) {
	          L1L2_StripL2F.push_back(fDeefit->LayerEvent_fL2FNumStrip[l2fmulti]);
	          L1L2_EL2F.push_back(fDeefit->LayerEvent_fL2FEMeV[l2fmulti]);
						L2FTime.push_back(fDeefit->LayerEvent_fL2FTime[l2fmulti]);
	        }
	      }
				// 提取 L1S 的数据
	      for (Int_t l1smulti=0; l1smulti<fDeefit->LayerEvent_fL1SMulti; l1smulti++) {
	        if (fDeefit->LayerEvent_fL1SSSDNum[l1smulti] == ssdindex) {
	          L1L2_StripL1S.push_back(fDeefit->LayerEvent_fL1SNumStrip[l1smulti]);
	          L1L2_EL1S.push_back(fDeefit->LayerEvent_fL1SEMeV[l1smulti]);
	        }
	      }
	      // 填充 hist_L1_L2F
	      if (fPattern.IsEneConstraint_L2B_L2F(ssdindex, L1L2_EL2B[0], L1L2_EL2F[0])) {
					for (Int_t l1smulti=0; l1smulti<fDeefit->LayerEvent_fSSDL1SMulti[ssdindex]; l1smulti++) {
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

  std::string pathDEEFITData(Form("%sDEEFITData/DEEFITData_Run%04d-%04d.root",PATHROOTFILESFOLDER,fFirstRun,fLastRun));
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
		h_DEE_PIDCut[numtel]   = new TH2D(Form("PIDDEE_SSD%d_CsI%d",ssdnum+1,csinum),Form("PIDDEE_SSD%d_CsI%d",ssdnum+1,csinum), 4000,0,4000,EL2_NBins[ssdnum],0,EL2_Range[ssdnum]);
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
	Int_t ECh2 = 1100;
	Int_t ECh3 = 1900;
	Int_t ECh4 = 2700;

	Int_t  charge  = -99;  // 电荷数理论值
	Double_t mass  = -99.; // 质量数计算值

	Double_t zeta; // 电荷数计算值
	Int_t   imass; // 质量数理论值

	Short_t EX_gaus;
	Float_t EY_gaus;

  Int_t NPoints = 100; // 每个 X 点产生高斯弥散的数目
	Double_t sigmaELow = 0.01, sigmaEUp = 0.06, sigmaEStep = 0.002;
	Double_t sigmadELow = 0.005, sigmadEUp = 0.025, sigmadEStep = 0.002;
	Int_t Nbin_sigmaE  = Int_t ((sigmaEUp-sigmaELow)/sigmaEStep);
	Int_t Nbin_sigmadE = Int_t ((sigmadEUp-sigmadELow)/sigmadEStep);

	Double_t sigma_mass[NUM_SSD*NUM_CSI][3];
	Double_t sigma_mass_MC[NUM_SSD*NUM_CSI][Nbin_sigmaE*Nbin_sigmadE][3];

	Color_t contour_color[3] = {kRed-4, kAzure-3, kSpring-1};

  std::string pathDEEFITPars(Form("%sDEEFITData/Fitparam_table.out",PATHROOTFILESFOLDER));
	std::string pathDEEFITData(Form("%sDEEFITData/DEEFITData_Run%04d-%04d.root",PATHROOTFILESFOLDER,fFirstRun,fLastRun));
	std::string pathPDFMassDistr(Form("%sfigure_PID/AlphaMassDistr%04d-%04d.pdf",PATHFIGURESFOLDER,fFirstRun,fLastRun));
	std::string pathPDFMassDistrBegin(Form("%sfigure_PID/AlphaMassDistr%04d-%04d.pdf[",PATHFIGURESFOLDER,fFirstRun,fLastRun));
	std::string pathPDFMassDistrEnd(Form("%sfigure_PID/AlphaMassDistr%04d-%04d.pdf]",PATHFIGURESFOLDER,fFirstRun,fLastRun));
  std::string pathPDFCont(Form("%sfigure_PID/AlphaEnergyResolutionCont%04d-%04d.pdf",PATHFIGURESFOLDER,fFirstRun,fLastRun));
	std::string pathPDFContBegin(Form("%sfigure_PID/AlphaEnergyResolutionCont%04d-%04d.pdf[",PATHFIGURESFOLDER,fFirstRun,fLastRun));
	std::string pathPDFContEnd(Form("%sfigure_PID/AlphaEnergyResolutionCont%04d-%04d.pdf]",PATHFIGURESFOLDER,fFirstRun,fLastRun));
	std::string pathPIDAlphaMassResolution(Form("%sdata_PID/PID_Alpha_MassResolution_Range.dat",PATHDATAFOLDER));
  std::string pathROOTOutput("/home/sea/Fission2019_Data/CsIAlphaMassResolution.root");

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

				for (Int_t Ebin=ECh1; Ebin<ECh4; Ebin++) {
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
				for (Int_t iBindE=0; iBindE<Nbin_sigmadE; iBindE+=100) {
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
