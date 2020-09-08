#ifndef SHARED_H
#define SHARED_H

#include <cmath>

//______________________________________________________________________________
extern const Double_t RadToDeg;
extern const Double_t DegToRad;

//______________________________________________________________________________
extern const Int_t NUM_SSD;
extern const Int_t NUM_STRIP;
extern const Int_t NUM_CSI;
extern const Int_t NUM_MAX_PARTICLES_PER_TEL;

//______________________________________________________________________________
extern const Double_t NUM_SIGMA_L1S[4];
extern const Double_t NUM_SIGMA_L2F[4];
extern const Double_t NUM_SIGMA_L2B[4];
extern const Double_t NUM_SIGMA_L3A[4];

//______________________________________________________________________________
// Setting the Cut for CsI
extern const Double_t ECSICHCUT;
extern const Double_t CSIPEDESTALSIGMA;

//______________________________________________________________________________
extern const char* PATHDATAFOLDER;
extern const char* PATHFIGURESFOLDER;
extern const char* PATHROOTFILESFOLDER;



#endif
