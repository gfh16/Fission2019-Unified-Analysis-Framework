#include "../include/CSHINEBuildEvent.h"

Int_t max(Int_t a, Int_t b, Int_t c) {
  return (a>b ? a : (b>c ? b : c));
}


//______________________________________________________________________________
CSHINEBuildEvent::CSHINEBuildEvent()
{
  fSiEChcutl1s  = fCSHINESSDCalibratedData.GetSiEChCut ("L1S", 2.0*NUM_SIGMA);
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
   ssdevent.fL1SMulti = 0;
   ssdevent.fL2FMulti = 0;
   ssdevent.fL2BMulti = 0;
   ssdevent.fCsIMulti = 0;

   vector<Int_t> fChannels_l1s;
   vector<Int_t> fChannels_l2f;
   vector<Int_t> fChannels_l2b;
   vector<Int_t> fChannels_csi;

   for (Int_t chindex=0; chindex<NUM_STRIP; chindex++) {
     if (echl1s[chindex]>fSiEChcutl1s[ssdindex*NUM_STRIP+chindex]) {
       ssdevent.fL1SMulti ++;
       fChannels_l1s.push_back(chindex);
     }
     if (echl2f[chindex]>fSiEChcutl2f[ssdindex*NUM_STRIP+chindex]) {
       ssdevent.fL2FMulti ++;
       fChannels_l2f.push_back(chindex);
     }
     if (echl2b[chindex]>fSiEChcutl2b[ssdindex*NUM_STRIP+chindex]) {
       ssdevent.fL2BMulti ++;
       fChannels_l2b.push_back(chindex);
     }
   }
   for (Int_t chindex=0; chindex<NUM_CSI; chindex++) {
     if (echcsi[chindex]>fCsIEChcutl3a[ssdindex*NUM_CSI+chindex]) {
        ssdevent.fCsIMulti ++;
        fChannels_csi.push_back(chindex);
     }
   }
   ssdevent.fSSDMulti = max(ssdevent.fL1SMulti,ssdevent.fL2FMulti,ssdevent.fL2BMulti);

   //_____________________________________
   // 2.获取点火的 channel index, 并计算能量
   for (Int_t m=0; m<ssdevent.fL1SMulti; m++) {
      ssdevent.fL1SNumStrip[m] = fChannels_l1s[m];
      ssdevent.fL1SEMeV    [m] = fSlopel1s[ssdindex*NUM_STRIP+fChannels_l1s[m]]*echl1s[fChannels_l1s[m]]+
                                 fInterceptl1s[ssdindex*NUM_STRIP+fChannels_l1s[m]];
   }
   for (Int_t m=0; m<ssdevent.fL2FMulti; m++) {
     ssdevent.fL2FNumStrip[m]  = fChannels_l2f[m];
     ssdevent.fL2FEMeV    [m]  = fSlopel2f[ssdindex*NUM_STRIP+fChannels_l2f[m]]*echl2f[fChannels_l2f[m]]+
                                 fInterceptl2f[ssdindex*NUM_STRIP+fChannels_l2f[m]];
   }
   for (Int_t m=0; m<ssdevent.fL2BMulti; m++) {
     ssdevent.fL2BNumStrip[m]  = fChannels_l2b[m];
     ssdevent.fL2BEMeV    [m]  = fSlopel2b[ssdindex*NUM_STRIP+fChannels_l2b[m]]*echl2b[fChannels_l2b[m]]+
                                 fInterceptl2b[ssdindex*NUM_STRIP+fChannels_l2b[m]];
   }
   for (Int_t m=0; m<ssdevent.fCsIMulti; m++) {
     ssdevent.fCsINum[m] = fChannels_csi[m];
     ssdevent.fCsIECh[m] = echcsi[fChannels_csi[m]];
   }
}
