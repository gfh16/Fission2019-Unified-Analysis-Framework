////////////////////////////////////////////////////////////////////////////////
//  这个类用于定义数据分析中可能用到的物理计算
//
//  gfh, 2021-08
////////////////////////////////////////////////////////////////////////////////
#include "../include/PhysicalCalculator.h"
using namespace std;


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
PhysicalCalculator::PhysicalCalculator()
{}

PhysicalCalculator::~PhysicalCalculator()
{}
//______________________________________________________________________________


//______________________________________________________________________________
Double_t PhysicalCalculator::GetMomentumValue(Double_t Ekinc, Int_t Z_Charge, Int_t A_Mass) const
{
  Double_t mass_mev = fNuclMass.GetMass_Z_A_MeV(Z_Charge, A_Mass);       // MeV/c^2
	Double_t p_relativistic = TMath::Sqrt(2*mass_mev*Ekinc + Ekinc*Ekinc); // 相对论情况普遍适用,且形式也不是很复杂
	return p_relativistic;
}

//______________________________________________________________________________
Double_t PhysicalCalculator::GetBeta(Double_t Ekinc, Int_t Z_Charge, Int_t A_Mass) const
{
  Double_t mass_mev = fNuclMass.GetMass_Z_A_MeV(Z_Charge, A_Mass);                // MeV/c^2
	Double_t beta = TMath::Sqrt(2*mass_mev*Ekinc + Ekinc*Ekinc)/(mass_mev + Ekinc); // beta = v/c
	return beta;
}


//______________________________________________________________________________
Double_t PhysicalCalculator::GetBeta(Double_t Ekinc, Int_t Z_Charge, Int_t A_Mass, Double_t Momentum) const
{
  Double_t mass_mev = fNuclMass.GetMass_Z_A_MeV(Z_Charge, A_Mass);       // MeV/c^2
	Double_t beta = Momentum / (mass_mev + Ekinc);
	return beta;
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
