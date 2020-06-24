///////////////////////////////////////////////
//   MEPAX2 class implementation 
///////////////////////////////////////////////
#include "MEpax.h"


//constructor: for details see Phys. Rev. C61 034607 (2000) 
// zp, ap = Z projectile, A projectile
// zt, at = Z target, A target
MEpax::MEpax(Double_t zp, Double_t ap, Double_t zt, Double_t at) : 
          fzp(zp),fap(ap),fzt(zt), fat(at) 
{
 fdelt1 = -1.09;
 fdelt2 = 3.05e-2;
 fdelt3 = 2.14e-4;
 fdelt4 = 71.35;
 fpm1 = -10.25;
 fpm2 = 10.10;
 fnm1 = 0.400;
 fnm2 = 0.600; 
 fd1 = -25.0;
 fd2 = 0.80;
 
 const Int_t amin[]={0,1,3,6,7,8,9,12,13,17,17,20,20,22,22,26,27,31,31,35,35,
               40,39,43,42,46,45,50,48,55,55,60,61,64,65,70,69,74,73,78,80,
    82,84,86,87,89,91,94,97,98,100,103,106,108,110,112,114,120,123,124,127};

 
 const Int_t amax[]=
   {0,3,8,11,14,19,22,23,24,29,32,35,36,39,42,46,48,51,51,54,53,
    55,58,61,64,66,69,75,78,80,81,84,86,89,91,94,96,102,102,105,107,
    110,113,115,118,121,123,127,128,129,130,
    131,132,133,134,135,136,137,138,139,140};
   
  for(Int_t i=0; i<kepmax; i++) {
   famin[i] = amin[i];
   famax[i] = amax[i];
   fAprob[i]=0.0;
  }
 
 SetAZArray();
}

// Return A corresponding to the value of Zprob nearest to the input charge Z
// Use epax2 corrections for nuclei far for from beta stability line 
// Z input charge
Double_t MEpax::GetMassEpax(Double_t Z)
{
 Double_t zbeta, zbetap, A, delta, zprob, deltam;

 Double_t amin=famin[(int)Z];
 Double_t amax=famax[(int)Z];
 Double_t ares=0.,diff,diff1;
 
 if(Z>fzp)return 0.0;

 A = amin;
 diff1 = 100.0;
 while(A<=amax) {
  zbeta = A / (1.98 + 0.0155* pow(A,(2./3.)));
  zbetap = fap / (1.98 + 0.0155* pow(fap,(2./3.)));
  
  if(A < fdelt4) { 
   delta = fdelt3 * A*A;
  }
  else {
   delta = fdelt2*A + fdelt1;
  }
  
// delta correction if close to projectile
  if(A/fap > fd2) {
   Double_t d = A/fap - fd2; 
   Double_t deltac = 1.0 + fd1*d*d;
   delta = delta * deltac;
  }
 
  zprob = zbeta + delta;

// correction for neutron-rich or neutron poor projectiles
  if((fzp - zbetap) > 0) { 
  // neutron poor
   deltam = exp(fpm1 + fpm2*(A/fap));
  }
  else { 
  // neutron rich (use of fap instead of fat; geant4 compatibility )
  // Also because we use epax2 for the most forward rings 
   deltam = fnm1*pow((A/fap),2) + fnm2*pow((A/fap),4);
  }
  deltam = deltam * (fzp - zbetap);
  zprob = zprob + deltam;
// small correction
// zprob = zprob + 0.0020*A; 

  diff = fabs(Z-zprob);
  if(diff<=diff1) {
   ares = A;
   diff1 = diff;
  }
  A = A + 1;
 }
 return ares;
}

// fill a table with (A,Z) values from epax2
void MEpax::SetAZArray()
{
 memset(fAprob,0,sizeof(fAprob));
 for(Int_t z=1; z<=fzp; z++) { 
  if(z<=4) 
   fAprob[z] = gLight[z-1];
  else
   fAprob[z] = GetMassEpax((Double_t)z);
 }
 //epax formula is not valid for Z>Zproj. We extrapolate few values
 //beyond projectile value in order to use during identification 
 //procedure where ZMAX = ZPROJ + 2 
  fAprob[(Int_t)fzp+1] = fap + 1;
  fAprob[(Int_t)fzp+2] = fap + 3;
}


// Set projectile-target combination
void MEpax::SetReaction(Double_t zp, Double_t ap, Double_t zt, Double_t at)
{
 fzp = zp;
 fap = ap;
 fzt = zt;
 fat = at;
}


