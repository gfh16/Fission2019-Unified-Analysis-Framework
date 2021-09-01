#ifndef PHYSICALCALCULATOR_H
#define PHYSICALCALCULATOR_H

#include "Nuclear_masses.h"


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
class PhysicalCalculator
{
public:
	PhysicalCalculator();
	~PhysicalCalculator();


public:
	Double_t  GetMomentumValue(Double_t Ekinc, Int_t Z_Charge, Int_t A_Mass) const; // 粒子的动能大小

	Double_t  GetBeta(Double_t Ekinc, Int_t Z_Charge, Int_t A_Mass) const;  // 粒子的速度 beta = v/c
	Double_t  GetBeta(Double_t Ekinc, Int_t Z_Charge, Int_t A_Mass, Double_t Momentum) const;

private:
	Nuclear_masses  fNuclMass;
};

#endif
