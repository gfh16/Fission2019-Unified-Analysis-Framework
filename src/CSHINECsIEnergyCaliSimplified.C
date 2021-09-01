#include "../include/CSHINECsIEnergyCaliSimplified.h"


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//  这个类是为了对 CsI 能量刻度参数的调用而写,CSHINECsIEnergyCaliSimplified 所有的成员
//  函数都来自于 CSHINECsIEnergyCali 这个类, 这里仅仅摘取了必要的功能，以便阅读
//  CsI 能量刻度的方法请参考 CSHINECsIEnergyCali
//
// gfh, 2021-08
//
CSHINECsIEnergyCaliSimplified::CSHINECsIEnergyCaliSimplified()
{
	Init_ZA_InCsI();
	Init_fFuncCsIFitting();
	Init_fECsISpline();
}


//______________________________________________________________________________
CSHINECsIEnergyCaliSimplified::~CSHINECsIEnergyCaliSimplified()
{}

//______________________________________________________________________________
void CSHINECsIEnergyCaliSimplified::PrintUsage()
{
	// check ZA
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		if (Z_InCsI[numtel].size()>0) {
			for (Int_t ip=0; ip<Z_InCsI[numtel].size(); ip++) {
				cout<<numtel<<setw(10)<<Z_InCsI[numtel][ip]<<setw(10)<<A_InCsI[numtel][ip];

				if (Z_InCsI[numtel][ip]<=1) {
					for (Int_t ipar=0; ipar<CsIEnergyFitPars_Z1[numtel].size(); ipar++) {
					  cout<<setw(15)<<CsIEnergyFitPars_Z1[numtel][ipar];
					}
				}
				else {
					for (Int_t ipar=0; ipar<CsIEnergyFitPars_ZOver2[numtel].size(); ipar++) {
					  cout<<setw(15)<<CsIEnergyFitPars_ZOver2[numtel][ipar];
					}
				}
				cout<<endl;
			}
		}
	}
}
//______________________________________________________________________________


//______________________________________________________________________________
// 自定义函数, 用于拟合 p,d,t
// NIMA 929,162(2019), 使用公式如下：
//  Ch = par[0]+par[1]*E^{(par[2]+A)/(par[3]+A)}
Double_t CSHINECsIEnergyCaliSimplified::DrawFunc_Hydrogen(Double_t* x, Double_t* par)
{
	// x[0]=E,x[1]=100*Z+A
	Double_t Z =  par[4];
	Double_t A = par[5];
  Double_t exponent = (A+par[2])/(A+par[3]);
  Double_t light = par[0]+par[1]*pow(x[0],exponent);
  return light;
}
//______________________________________________________________________________

//______________________________________________________________________________
// 对重离子(Z>=2), 采用 Horn-fit
// D.Horn et al, NIM A320(1992) 273-276
Double_t CSHINECsIEnergyCaliSimplified::DrawFunc_HeavyIon(Double_t* x, Double_t* par)
{
	// x[0]=E, x[1]=10*Z+A
	Double_t Z = par[3];
  Double_t A = par[4];
  Double_t consterm = par[2]*A*pow(Z,2);
  Double_t lineterm = x[0]+consterm;
  Double_t logterm = consterm*log(abs(lineterm/consterm));
  Double_t light = par[0]+par[1]*(x[0]-logterm);
  return light;
}
//______________________________________________________________________________

//______________________________________________________________________________
Double_t CSHINECsIEnergyCaliSimplified::CsICaliChToMeV(Int_t ssdindex, Int_t csiindex, Int_t charge, Int_t mass, Double_t ECh) const
{
  Int_t numtel = ssdindex*NUM_CSI + csiindex;
	Double_t emev = fECsISpline[numtel][charge][mass]->Eval(ECh);
	return (emev > 0 ? emev : 0.);
}
//______________________________________________________________________________

//______________________________________________________________________________
void CSHINECsIEnergyCaliSimplified::Init_ZA_InCsI()
{
	ifstream FileIn_GetZA_InCsI(pathGetZAInCsI.c_str());

	// 提取电荷数 Z 与质量数 A
	if (!FileIn_GetZA_InCsI.is_open()) {
		printf("Error: file %s not found\n",pathGetZAInCsI.c_str());
    return;
	}
	while (FileIn_GetZA_InCsI.good()) {
		// 按行读取数据
    std::string LineRead;
    std::getline(FileIn_GetZA_InCsI, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

		Int_t numtel, partilenum;
		Int_t charge, mass;
		LineStream>>numtel>>partilenum>>charge>>mass;

	  Z_InCsI[numtel].push_back(charge);
		A_InCsI[numtel].push_back(mass);
	}
	FileIn_GetZA_InCsI.close();
}
//______________________________________________________________________________

//______________________________________________________________________________
void CSHINECsIEnergyCaliSimplified::Init_fFuncCsIFitting()
{
	readfile.CsICaliLoadPars(CsIEnergyFitPars_Z1,pathCsIFitPars_Z1.c_str(),NFitPars_Z1);
  readfile.CsICaliLoadPars(CsIEnergyFitPars_ZOver2,pathCsIFitPars_ZOver2.c_str(),NFitPars_ZOver2);

	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		if (Z_InCsI[numtel].size()>0) {
			for (Int_t ip=0; ip<Z_InCsI[numtel].size(); ip++) {
				Int_t icharge = Z_InCsI[numtel][ip];
				Int_t imass   = A_InCsI[numtel][ip];
				Double_t dcharge = (Double_t) Z_InCsI[numtel][ip];
				Double_t dmass   = (Double_t) A_InCsI[numtel][ip];
	      // TSpline(TF1* f1) 差值计算
				if (icharge == 1) {
					fFuncCsIFitting[numtel][icharge][imass] = new TF1(Form("func_%d_%d_%d",numtel,icharge,imass),this,&CSHINECsIEnergyCaliSimplified::DrawFunc_Hydrogen,EnergyLowLimitsForInterpolation,EnergyUpLimitsForInterpolation,6);
	        fFuncCsIFitting[numtel][icharge][imass]->SetParameters(CsIEnergyFitPars_Z1[numtel].data());
					fFuncCsIFitting[numtel][icharge][imass]->SetParameter(4,dcharge);
					fFuncCsIFitting[numtel][icharge][imass]->SetParameter(5,dmass);
				}
				if (icharge > 1) {
					fFuncCsIFitting[numtel][icharge][imass] = new TF1(Form("func_%d_%d_%d",numtel,icharge,imass),this,&CSHINECsIEnergyCaliSimplified::DrawFunc_HeavyIon,EnergyLowLimitsForInterpolation,EnergyUpLimitsForInterpolation,5);
	        fFuncCsIFitting[numtel][icharge][imass]->SetParameters(CsIEnergyFitPars_ZOver2[numtel].data());
					fFuncCsIFitting[numtel][icharge][imass]->SetParameter(3,dcharge);
					fFuncCsIFitting[numtel][icharge][imass]->SetParameter(4,dmass);
				}
			}
		}
	}
}
//______________________________________________________________________________

//______________________________________________________________________________
void CSHINECsIEnergyCaliSimplified::Init_fECsISpline()
{
	for (Int_t numtel=0; numtel<NUM_SSD*NUM_CSI; numtel++) {
		if (Z_InCsI[numtel].size()>0) {
			for (Int_t ip=0; ip<Z_InCsI[numtel].size(); ip++) {
				// 构造 TSpline3
				Double_t ECh[NPoints_ForInterpolation];
				Double_t EMeV[NPoints_ForInterpolation];
				Int_t icharge = Z_InCsI[numtel][ip];
				Int_t imass   = A_InCsI[numtel][ip];
				//
				for (Int_t jp=0; jp<NPoints_ForInterpolation; jp++) {
			    Double_t emev = EnergyLowLimitsForInterpolation + jp*(EnergyUpLimitsForInterpolation-EnergyLowLimitsForInterpolation)/NPoints_ForInterpolation;
					Double_t ech = fFuncCsIFitting[numtel][icharge][imass]->Eval(emev);
					ECh [jp] = ech;
					EMeV[jp] = emev;
				}
				TGraph* gr = new TGraph(NPoints_ForInterpolation, ECh, EMeV);
				fECsISpline[numtel][icharge][imass] = new TSpline3(Form("spline_%d_%d_%d",numtel,icharge,imass), gr);
			}
		}
	}
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
