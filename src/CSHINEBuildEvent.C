#include "../include/CSHINEBuildEvent.h"

Int_t max(Int_t a, Int_t b, Int_t c) {
  return (a>b ? a : (b>c ? b : c));
}


//______________________________________________________________________________
CSHINEBuildEvent::CSHINEBuildEvent()
{
  fSiEChcutl1s  = fCSHINESSDCalibratedData.GetSiEChCut ("L1S", 2*NUM_SIGMA);
  fSiEChcutl2f  = fCSHINESSDCalibratedData.GetSiEChCut ("L2F", NUM_SIGMA);
  fSiEChcutl2b  = fCSHINESSDCalibratedData.GetSiEChCut ("L2B", NUM_SIGMA);
  fCsIEChcutl3a = fCSHINESSDCalibratedData.GetCsIEChCut("L3A");

  fSlopel1s     = fCSHINESSDCalibratedData.GetSiCaliSlope    ("L1S");
  fSlopel2f     = fCSHINESSDCalibratedData.GetSiCaliSlope    ("L2F");
  fSlopel2b     = fCSHINESSDCalibratedData.GetSiCaliSlope    ("L2B");

  fInterceptl1s = fCSHINESSDCalibratedData.GetSiCaliIntercept("L1S");
  fInterceptl2f = fCSHINESSDCalibratedData.GetSiCaliIntercept("L2F");
  fInterceptl2b = fCSHINESSDCalibratedData.GetSiCaliIntercept("L2B");
}

//______________________________________________________________________________
CSHINEBuildEvent::~CSHINEBuildEvent()
{}

//______________________________________________________________________________
void CSHINEBuildEvent::BuildSSDEvent(CSHINESSDEvent& ssdevent, Int_t ssdindex,
  Int_t* echl1s, Int_t* echl2f, Int_t* echl2b, Int_t* echcsi)
{
   //_________________________________________________
   // 1.获取 multipicity, 并将点火的 channel index 存进 vector
   ssdevent.fMultiL1S = 0;
   ssdevent.fMultiL2F = 0;
   ssdevent.fMultiL2B = 0;
   ssdevent.fMultiCsI = 0;

   vector<Int_t> fChannels_l1s;
   vector<Int_t> fChannels_l2f;
   vector<Int_t> fChannels_l2b;
   vector<Int_t> fChannels_csi;

   for (Int_t chindex=0; chindex<NUM_STRIP; chindex++) {
     if (echl1s[chindex]>fSiEChcutl1s[ssdindex*NUM_STRIP+chindex]) {
       ssdevent.fMultiL1S ++;
       fChannels_l1s.push_back(chindex);
     }
     if (echl2f[chindex]>fSiEChcutl2f[ssdindex*NUM_STRIP+chindex]) {
       ssdevent.fMultiL2F ++;
       fChannels_l2f.push_back(chindex);
     }
     if (echl2b[chindex]>fSiEChcutl2b[ssdindex*NUM_STRIP+chindex]) {
       ssdevent.fMultiL2B ++;
       fChannels_l2b.push_back(chindex);
     }
   }
   for (Int_t chindex=0; chindex<NUM_CSI; chindex++) {
     if (echcsi[chindex]>fCsIEChcutl3a[ssdindex*NUM_CSI+chindex]) {
        ssdevent.fMultiCsI ++;
        fChannels_csi.push_back(chindex);
     }
   }
   ssdevent.fSSDMulti = max(ssdevent.fMultiL1S,ssdevent.fMultiL2F,ssdevent.fMultiL2B);

   //_____________________________________
   // 2.获取点火的 channel index, 并计算能量
   for (Int_t m=0; m<ssdevent.fMultiL1S; m++) {
      ssdevent.fNumStripL1S[m] = fChannels_l1s[m];
      ssdevent.fEMeVL1S    [m] = fSlopel1s[ssdindex*NUM_STRIP+fChannels_l1s[m]]*echl1s[fChannels_l1s[m]]+
                                 fInterceptl1s[ssdindex*NUM_STRIP+fChannels_l1s[m]];
   }
   for (Int_t m=0; m<ssdevent.fMultiL2F; m++) {
     ssdevent.fNumStripL2F[m]  = fChannels_l2f[m];
     ssdevent.fEMeVL2F    [m]  = fSlopel2f[ssdindex*NUM_STRIP+fChannels_l2f[m]]*echl2f[fChannels_l2f[m]]+
                                 fInterceptl2f[ssdindex*NUM_STRIP+fChannels_l2f[m]];
   }
   for (Int_t m=0; m<ssdevent.fMultiL2B; m++) {
     ssdevent.fNumStripL2B[m]  = fChannels_l2b[m];
     ssdevent.fEMeVL2B    [m]  = fSlopel2b[ssdindex*NUM_STRIP+fChannels_l2b[m]]*echl2b[fChannels_l2b[m]]+
                                 fInterceptl2b[ssdindex*NUM_STRIP+fChannels_l2b[m]];
   }
   for (Int_t m=0; m<ssdevent.fMultiCsI; m++) {
     ssdevent.fNumCsI[m] = fChannels_csi[m];
     ssdevent.fEChCsI[m] = echcsi[fChannels_csi[m]];
   }
}
