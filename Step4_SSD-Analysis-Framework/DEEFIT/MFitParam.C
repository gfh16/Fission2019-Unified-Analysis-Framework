//MFitParam class implementation 

#include "MFitParam.h"
#include "MDEEFrame.h"

//MFitParam constructor 
MFitParam::MFitParam()
{
 Int_t np = gfit->GetNPar();
 for(int i=0; i<np; i++) {
  fparam[i] = 0.;
 }
 for(int i=0; i<MAXMAS; i++) {
  fhz[i] = 0;
 }
 fchi2=0;
 fntel=0;
 fnum=0;
 fmfor = gfit->GetMassFormula();
}



//MFitParam constructor
MFitParam::MFitParam(Int_t ntel, Int_t np, ecalcmass mf, Double_t *p, Double_t chi2)
{
 fntel = ntel;
// Int_t np = gfit->GetNPar();
 for(int i=0; i<np; i++) {
  fparam[i] = p[i];
 }
 fchi2 = chi2;
 fnum = np;
 fmfor = mf;
 for(int i=0; i<MAXMAS; i++) {
  fhz[i] = 0;
 }
}


// Make the class MFitParam writable to a stream 
ostream &operator<<(ostream &out, const MFitParam &p)
{
 Int_t kf = (Int_t)p.fmfor;

 ios_base::fmtflags flags = cout.flags();  // save flags
 Int_t np = gfit->GetNPar();
 out<<setw(4)<<setprecision(4)<<p.fntel<<" "<<kf<<" "<<p.fnum<<" ";
 out<<fixed<<setprecision(5);
 for(int i=0; i<np; i++) {
//  cout<<p.fparam[i]<<endl;
  out<<p.fparam[i]<<" ";
 }
 out<<p.fchi2<<endl;

 cout.flags(flags);
 return(out);
}

