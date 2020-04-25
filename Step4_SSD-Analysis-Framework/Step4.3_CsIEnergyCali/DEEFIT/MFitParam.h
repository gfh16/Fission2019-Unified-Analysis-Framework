#ifndef MFITPARAM_H
#define MFITPARAM_H

#include "deefit.h"

using namespace std;

// The MFitParam class manage the parameter fit
// saving and loading from a file
class MFitParam {
private:
 Int_t fntel;              //telescope number
 Int_t fnum;               //number of fit parameters for fntel
 ecalcmass fmfor;          //mass formula used
 Double_t fparam[MAXFP];   //vector of parameters
 Double_t fchi2;           //chisquare of the fit
public:
 TH1F *fhz[MAXMAS];        //histograms
 MFitParam();              //Constructor
 MFitParam(Int_t, Int_t, ecalcmass, Double_t *, Double_t);  //Constructor
 virtual ~MFitParam() {}                                    //Destructor
 Int_t GetNpar() {return fnum;}                             //Get number of parameters
 Double_t *GetParam() {return fparam;}                      //Return the parameter vector
 friend ostream &operator<<(ostream &, const MFitParam &);  //Print results
 ClassDef(MFitParam,0)
};

#endif
