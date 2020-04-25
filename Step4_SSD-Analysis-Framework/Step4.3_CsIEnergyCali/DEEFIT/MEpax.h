#ifndef MEPAX_H
#define MEPAX_H

#include "deefit.h"
#include <cmath>


const Int_t kepmax = 61;              //max charge for epax formula 
const Double_t gLight[] = {1,4,7,9};  //default masses for light charges Z=1..Z=4

// EPAX2 Mass parametrization class 
class MEpax {
private:
 Double_t fdelt1, fdelt2, fdelt3, fdelt4; //Zprob shift parameters
 Double_t fpm1, fpm2;                     //proton rich memory effect
 Double_t fnm1, fnm2;                     //neutron rich memory effect
 Double_t fd1, fd2;                       //correction factors
 int famin[kepmax], famax[kepmax];        //isotopes range for a given Z up to Z=60
 Double_t fzp,fap,fzt,fat;                //target-projectile combination
 Double_t fAprob[kepmax];                 //a vector containing Z[i] = A 
public:
 MEpax(Double_t, Double_t, Double_t, Double_t);   //Constructor
 ~MEpax() {}                                      //Destructor
 Double_t GetMassEpax(Double_t );                 //Get Epax Mass for a given input Z charge
 Double_t GetZP() {return fzp;}                   //Get Z projectile
 Double_t GetAP() {return fap;}                   //Get Mass projectile
 Double_t GetZT() {return fzt;}                   //Get Z Target
 Double_t GetAT() {return fat;}                   //Get Mass target
 Double_t GetEMass(Int_t Z) {return fAprob[Z];}   //Get most probable Epax mass
 void SetReaction(Double_t, Double_t, Double_t, Double_t); //Set reaction parameters
 void SetAZArray();                                        //Store Epax data
};

#endif
