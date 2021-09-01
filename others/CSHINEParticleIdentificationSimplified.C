#include "../include/CSHINEParticleIdentificationSimplified.h"


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//  实际数据分析中, 使用 "拉直法" 进行粒子鉴别
//  CSHINEParticleIdentificationSimplified 的所有成员函数都复制于
//  CSHINEParticleIdentification. 为了增强代码的可读性,这是简化版,仅仅包含了实际分析
//  中用到的功能
//
// P.S. 关于粒子鉴别的详细情况,请参考 CSHINEParticleIdentification
// gfh, 2021-08
//______________________________________________________________________________
CSHINEParticleIdentificationSimplified::CSHINEParticleIdentificationSimplified()
{
	Init_L2L3StraighteningFitPars();
	Init_L1L2StraighteningFitPars();
}

CSHINEParticleIdentificationSimplified::~CSHINEParticleIdentificationSimplified()
{}

void CSHINEParticleIdentificationSimplified::PrintUsage()
{
	// L2L3
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		if (L2L3_Charge[numtel].size()>0) {
			for (Int_t ip=0; ip<L2L3_Charge[numtel].size(); ip++) {
				cout<<numtel<<setw(10)<<L2L3_Charge[numtel][ip]<<setw(10)<<L2L3_Mass[numtel][ip];
				for (Int_t ipar=0; ipar<NFITPARS; ipar++) {
					cout<<setw(15)<<L2L3_DEEFitPars[numtel][ip][ipar];
				}
				cout<<endl;
			}
		}
	}
  // L1L2
	for (Int_t numtel=0; numtel<NUM_SSD; numtel++) {
		if (L1L2_Charge[numtel].size()>0) {
			for (Int_t ip=0; ip<L1L2_Charge[numtel].size(); ip++) {
				cout<<numtel<<setw(10)<<L1L2_Charge[numtel][ip]<<setw(10)<<L1L2_Mass[numtel][ip];
				for (Int_t ipar=0; ipar<NFITPARS; ipar++) {
					cout<<setw(15)<<L1L2_DEEFitPars[numtel][ip][ipar];
				}
				cout<<endl;
			}
		}
	}
}
//______________________________________________________________________________


//______________________________________________________________________________
// 确定了粒子所在的 BananaCut 之后,可以简化 PID 过程
// 即不需要从头开始找, 只需要从给定的 charge, mass 附近开始找
Double_t CSHINEParticleIdentificationSimplified::L2L3_GetPIDNumFromExpData(Int_t ssdindex,
	Int_t csiindex, Double_t EStoped, Double_t dE, Int_t icharge, Int_t imass)
{
	Double_t pidnum  = 0.;
	Double_t pidnum_low, pidnum_up;

  Double_t calc_dE = 0.;
	Double_t dist    = 0.;
	Double_t dEip;
	Double_t dErange;

  Int_t numtel = ssdindex*NUM_CSI + csiindex; // CsI 编号: 0-35
	Int_t numparticle = L2L3_Charge[numtel].size();

	Double_t calc_Eproton = DoCalcdEMeV(EStoped,L2L3_DEEFitPars[numtel][0],NFITPARS);
	Double_t calc_Elastparticle = DoCalcdEMeV(EStoped,L2L3_DEEFitPars[numtel][numparticle-1],NFITPARS);

	if (dE<=0)  { cout<<"Invalid dE: dE<0 !"<<endl; pidnum = 0.; }
	// 对于质子线以下的粒子
	else if ((dE>0) && (dE<calc_Eproton)) {
    pidnum = StdPIDNum(L2L3_Charge[numtel][0],L2L3_Mass[numtel][0]) * (dE/calc_Eproton);
	}
	// 超出最高线的粒子
	else if (dE>calc_Elastparticle) {
    pidnum = StdPIDNum(L2L3_Charge[numtel][numparticle-1],L2L3_Mass[numtel][numparticle-1]) * (dE/calc_Elastparticle);
	}
	// 介于质子线 与 最高粒子线之间的粒子, 可正常计算 pidnum
	else {
    for (Int_t ip=0; ip<numparticle; ip++) {
			if ((L2L3_Charge[numtel][ip]==icharge) && (L2L3_Mass[numtel][ip]==imass)) {
				dEip = DoCalcdEMeV(EStoped,L2L3_DEEFitPars[numtel][ip],NFITPARS);
				if (dE<=dEip) { // 粒子在 BananaCut 内且在中心线以下
          dist       = dE - DoCalcdEMeV(EStoped,L2L3_DEEFitPars[numtel][ip-1],NFITPARS);
					dErange    = dEip - DoCalcdEMeV(EStoped,L2L3_DEEFitPars[numtel][ip-1],NFITPARS);
					pidnum_low = StdPIDNum(L2L3_Charge[numtel][ip-1],L2L3_Mass[numtel][ip-1]);
					pidnum_up  = StdPIDNum(L2L3_Charge[numtel][ip],L2L3_Mass[numtel][ip]);
					pidnum     = pidnum_low + dist/dErange*(pidnum_up-pidnum_low);
				}
				else { // 粒子在 BananaCut 内且在中心线以上
					dist       = dE - dEip;
					dErange    = DoCalcdEMeV(EStoped,L2L3_DEEFitPars[numtel][ip+1],NFITPARS) - dEip;
					pidnum_low = StdPIDNum(L2L3_Charge[numtel][ip],L2L3_Mass[numtel][ip]);
					pidnum_up  = StdPIDNum(L2L3_Charge[numtel][ip+1],L2L3_Mass[numtel][ip+1]);
					pidnum     = pidnum_low + dist/dErange*(pidnum_up-pidnum_low);
				}
			}
		}
	}
	return pidnum;
}
//______________________________________________________________________________

//______________________________________________________________________________
// L1L2 的粒子鉴别方法与 L2L3 完全一样,不过需要读取的拟合参数不同
Double_t CSHINEParticleIdentificationSimplified::L1L2_GetPIDNumFromExpData(Int_t ssdindex,
	Double_t EStoped, Double_t dE, Int_t icharge, Int_t imass)
{
	Double_t pidnum  = 0.;
	Double_t pidnum_low, pidnum_up;

  Double_t calc_dE = 0.;
	Double_t dist    = 0.;
	Double_t dEip;
	Double_t dErange;

	Int_t numparticle = L1L2_Charge[ssdindex].size();

	Double_t calc_Eproton = DoCalcdEMeV(EStoped,L1L2_DEEFitPars[ssdindex][0],NFITPARS);
	Double_t calc_Elastparticle = DoCalcdEMeV(EStoped,L1L2_DEEFitPars[ssdindex][numparticle-1],NFITPARS);

	if (dE<=0)  { cout<<"Invalid dE: dE<0 !"<<endl; pidnum = 0.; }
	// 对于质子线以下的粒子
	else if ((dE>0) && (dE<calc_Eproton)) {
    pidnum = StdPIDNum(L1L2_Charge[ssdindex][0],L1L2_Mass[ssdindex][0]) * (dE/calc_Eproton);
	}
	// 超出最高线的粒子
	else if (dE>calc_Elastparticle) {
    pidnum = StdPIDNum(L1L2_Charge[ssdindex][numparticle-1],L1L2_Mass[ssdindex][numparticle-1]) * (dE/calc_Elastparticle);
	}
	// 介于质子线 与 最高粒子线之间的粒子, 可正常计算 pidnum
	else {
    for (Int_t ip=0; ip<numparticle; ip++) {
			if ((L1L2_Charge[ssdindex][ip]==icharge) && (L1L2_Mass[ssdindex][ip]==imass)) {
				dEip = DoCalcdEMeV(EStoped,L1L2_DEEFitPars[ssdindex][ip],NFITPARS);
				if (dE<=dEip) { // 粒子在 BananaCut 内且在中心线以下
          dist       = dE - DoCalcdEMeV(EStoped,L1L2_DEEFitPars[ssdindex][ip-1],NFITPARS);
					dErange    = dEip - DoCalcdEMeV(EStoped,L1L2_DEEFitPars[ssdindex][ip-1],NFITPARS);
					pidnum_low = StdPIDNum(L1L2_Charge[ssdindex][ip-1],L1L2_Mass[ssdindex][ip-1]);
					pidnum_up  = StdPIDNum(L1L2_Charge[ssdindex][ip],L1L2_Mass[ssdindex][ip]);
					pidnum     = pidnum_low + dist/dErange*(pidnum_up-pidnum_low);
				}
				else { // 粒子在 BananaCut 内且在中心线以上
					dist       = dE - dEip;
					dErange    = DoCalcdEMeV(EStoped,L1L2_DEEFitPars[ssdindex][ip+1],NFITPARS) - dEip;
					pidnum_low = StdPIDNum(L1L2_Charge[ssdindex][ip],L1L2_Mass[ssdindex][ip]);
					pidnum_up  = StdPIDNum(L1L2_Charge[ssdindex][ip+1],L1L2_Mass[ssdindex][ip+1]);
					pidnum     = pidnum_low + dist/dErange*(pidnum_up-pidnum_low);
				}
			}
		}
	}
	return pidnum;
}
//______________________________________________________________________________

//______________________________________________________________________________
// fit_func =   [0]/x + pol6
//          =   [0]*pow(x,-1)+[1]*pow(x,0)+[2]*pow(x,1)+[3]*pow(x,2)+[4]*pow(x,3)
//            + [5]*pow(x, 4)+[6]*pow(x,5)+[7]*pow(x,6);
Double_t CSHINEParticleIdentificationSimplified::DoCalcdEMeV(Double_t ex, Double_t* parlist, Int_t fitparsum)
{
  Double_t calc_dE = 0.;
	if (ex <= 0.) {
		cout<<"Invalid Ex!"<<endl;
		return -99.;
	}
	for (Int_t ipar=0; ipar<fitparsum; ipar++) {
    calc_dE += parlist[ipar]*pow(ex, ipar-1); //
  }
	return calc_dE;
}
//______________________________________________________________________________


//______________________________________________________________________________
// 定义标准 PID数： (Double_t) PID = Z + 0.2*(A-2*Z)
// 详见参考文献: L.Tassan-Got, Nucl.Instr.and Meth.B 194(2002)503.
Double_t CSHINEParticleIdentificationSimplified::StdPIDNum(Int_t Z_charge, Int_t A_mass)
{
	Double_t pidnum = Z_charge + 0.2*(A_mass - 2.0*Z_charge);
  return pidnum;
}

//________________________________________
// 反解 StdPIDNumber(), 求出质量数 mass
Double_t CSHINEParticleIdentificationSimplified::GetMassFromPIDNum(Double_t pidnum, Int_t charge)
{
  Double_t dmass = 5.0*pidnum - 3.0*charge;
	return dmass;
}
//______________________________________________________________________________


//______________________________________________________________________________
void CSHINEParticleIdentificationSimplified::Init_L2L3StraighteningFitPars()
{
  ifstream FileIn_GetL2L3StraighteningFitPars(pathL2L3StraighteningFitPars.c_str());

	// 提取拟合数据
	if (!FileIn_GetL2L3StraighteningFitPars.is_open()) {
		printf("Error: file %s not found\n",pathL2L3StraighteningFitPars.c_str());
    return;
	}
	while (FileIn_GetL2L3StraighteningFitPars.good()) {
		// 按行读取数据
    std::string LineRead;
    std::getline(FileIn_GetL2L3StraighteningFitPars, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

		Int_t numtel, partilenum;
		Int_t charge, mass;
		LineStream>>numtel>>partilenum>>charge>>mass;

    for (Int_t ipar=0; ipar<NFITPARS; ipar++) {
			LineStream>>L2L3_DEEFitPars[numtel][partilenum][ipar];
		}

	  L2L3_Charge[numtel].push_back(charge);
		L2L3_Mass[numtel].push_back(mass);
	}
	FileIn_GetL2L3StraighteningFitPars.close();
}

//______________________________________________________________________________
void CSHINEParticleIdentificationSimplified::Init_L1L2StraighteningFitPars()
{
  ifstream FileIn_GetL1L2StraighteningFitPars(pathL1L2StraighteningFitPars.c_str());

	// 提取拟合数据
	if (!FileIn_GetL1L2StraighteningFitPars.is_open()) {
		printf("Error: file %s not found\n",pathL1L2StraighteningFitPars.c_str());
    return;
	}
	while (FileIn_GetL1L2StraighteningFitPars.good()) {
		// 按行读取数据
    std::string LineRead;
    std::getline(FileIn_GetL1L2StraighteningFitPars, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

		Int_t numtel, partilenum;
		Int_t charge, mass;
		LineStream>>numtel>>partilenum>>charge>>mass;

    for (Int_t ipar=0; ipar<NFITPARS; ipar++) {
			LineStream>>L1L2_DEEFitPars[numtel][partilenum][ipar];
		}
	  L1L2_Charge[numtel].push_back(charge);
		L1L2_Mass[numtel].push_back(mass);
	}
	FileIn_GetL1L2StraighteningFitPars.close();
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
