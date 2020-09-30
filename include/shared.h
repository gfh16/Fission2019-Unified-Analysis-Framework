#ifndef SHARED_H
#define SHARED_H

#include "TMath.h"

//______________________________________________________________________________
extern const double RadToDeg;
extern const double DegToRad;

//______________________________________________________________________________
extern const int NUM_SSD;
extern const int NUM_STRIP;
extern const int NUM_CSI;
extern const int NUM_MAX_PARTICLES_PER_TEL;

//______________________________________________________________________________
extern const double NUM_SIGMA_L1S[4];
extern const double NUM_SIGMA_L2F[4];
extern const double NUM_SIGMA_L2B[4];
extern const double NUM_SIGMA_L3A[4];

//______________________________________________________________________________
// Setting the Cut for CsI
extern const double ECSICHCUT;
extern const double CSIPEDESTALSIGMA;

//______________________________________________________________________________
extern const char* PATHDATAFOLDER;
extern const char* PATHFIGURESFOLDER;
extern const char* PATHROOTFILESFOLDER;



#endif
