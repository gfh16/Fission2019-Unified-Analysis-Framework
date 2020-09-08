#include "../include/CSHINEEvent.h"


//******************************************************************************
// 定义 CSHINELayerEvent
CSHINELayerEvent::CSHINELayerEvent() :
fL1SMulti(0),
fL2FMulti(0),
fL2BMulti(0),
fCsIMulti(0)
{
  fL1SNumStrip  =   new Int_t    [NUM_STRIP];
  fL1SEMeV      =   new Double_t [NUM_STRIP];
  fL2FNumStrip  =   new Int_t    [NUM_STRIP];
  fL2FEMeV      =   new Double_t [NUM_STRIP];
  fL2BNumStrip  =   new Int_t    [NUM_STRIP];
  fL2BEMeV      =   new Double_t [NUM_STRIP];
  fCsINum       =   new Int_t    [NUM_CSI];
  fCsIECh       =   new Int_t    [NUM_CSI];
}

//______________________________________________________________________________
CSHINELayerEvent::~CSHINELayerEvent()
{
  delete []  fL1SNumStrip;
  delete []  fL1SEMeV;
  delete []  fL2FNumStrip;
  delete []  fL2FEMeV;
  delete []  fL2BNumStrip;
  delete []  fL2BEMeV;
  delete []  fCsINum;
  delete []  fCsIECh;
}
//******************************************************************************


//******************************************************************************
// 定义 CSHINESSDEvent
CSHINESSDEvent::CSHINESSDEvent() : fSSDMulti(0)
{
  fL1SNumStrip  =   new Int_t    [NUM_STRIP];
  fL1SEMeV      =   new Double_t [NUM_STRIP];
  fL2FNumStrip  =   new Int_t    [NUM_STRIP];
  fL2FEMeV      =   new Double_t [NUM_STRIP];
  fL2BNumStrip  =   new Int_t    [NUM_STRIP];
  fL2BEMeV      =   new Double_t [NUM_STRIP];
  fCsINum       =   new Int_t    [NUM_CSI];
  fCsIECh       =   new Int_t    [NUM_CSI];

  fTheta        =   new Double_t [NUM_STRIP];
  fPhi          =   new Double_t [NUM_STRIP];
}

//______________________________________________________________________________
CSHINESSDEvent::~CSHINESSDEvent()
{
  delete []  fL1SNumStrip;
  delete []  fL1SEMeV;
  delete []  fL2FNumStrip;
  delete []  fL2FEMeV;
  delete []  fL2BNumStrip;
  delete []  fL2BEMeV;
  delete []  fCsINum;
  delete []  fCsIECh;

  delete []  fTheta;
  delete []  fPhi;
 }
//******************************************************************************


//******************************************************************************
// 定义 CSHINEGlobalEvent
CSHINEGlobalEvent::CSHINEGlobalEvent() : fGlobalMulti(0)
{
  fGNumOfSSD    =  new Int_t     [NUM_SSD*NUM_STRIP];
  fGL1SNumStrip =  new Int_t     [NUM_SSD*NUM_STRIP];
  fGL1SEMeV     =  new Double_t  [NUM_SSD*NUM_STRIP];
  fGL2FNumStrip =  new Int_t     [NUM_SSD*NUM_STRIP];
  fGL2FEMeV     =  new Double_t  [NUM_SSD*NUM_STRIP];
  fGL2BNumStrip =  new Int_t     [NUM_SSD*NUM_STRIP];
  fGL2BEMeV     =  new Double_t  [NUM_SSD*NUM_STRIP];
  fGCsINum      =  new Int_t     [NUM_SSD*NUM_CSI];
  fGCsIEMeV     =  new Double_t  [NUM_SSD*NUM_CSI];

  fGL2FTime     =  new Int_t     [NUM_SSD*NUM_STRIP];

  fTheta        =  new Double_t  [NUM_SSD*NUM_STRIP];
  fPhi          =  new Double_t  [NUM_SSD*NUM_STRIP];
  fTotKinEnergy =  new Double_t  [NUM_SSD*NUM_STRIP];
  fCalKinEnergy =  new Double_t  [NUM_SSD*NUM_STRIP];
  fMomentum     =  new Double_t  [NUM_SSD*NUM_STRIP];
  fBeta         =  new Double_t  [NUM_SSD*NUM_STRIP];
  fZ            =  new Int_t     [NUM_SSD*NUM_STRIP];
  fA            =  new Int_t     [NUM_SSD*NUM_STRIP];
}

//______________________________________________________________________________
CSHINEGlobalEvent::~CSHINEGlobalEvent()
{
  delete []  fGNumOfSSD;
  delete []  fGL1SNumStrip;
  delete []  fGL1SEMeV;
  delete []  fGL2FNumStrip;
  delete []  fGL2FEMeV;
  delete []  fGL2BNumStrip;
  delete []  fGL2BEMeV;
  delete []  fGCsINum;
  delete []  fGCsIEMeV;

  delete []  fGL2FTime;

  delete []  fTheta;
  delete []  fPhi;
  delete []  fTotKinEnergy;
  delete []  fCalKinEnergy;
  delete []  fMomentum;
  delete []  fBeta;
  delete []  fZ;
  delete []  fA;
}
//******************************************************************************


//******************************************************************************
// 创建、填充 Event
CSHINEBuildEvent::CSHINEBuildEvent()
{
  fSiEChcutl1s  = fCSHINESSDCalibratedData.GetSiEChCut ("L1S");
  fSiEChcutl2f  = fCSHINESSDCalibratedData.GetSiEChCut ("L2F");
  fSiEChcutl2b  = fCSHINESSDCalibratedData.GetSiEChCut ("L2B");
  fCsIEChcutl3a = fCSHINESSDCalibratedData.GetCsIEChCut("L3A");

  fSlopel1s     = fCSHINESSDCalibratedData.GetSiCaliSlope("L1S");
  fSlopel2f     = fCSHINESSDCalibratedData.GetSiCaliSlope("L2F");
  fSlopel2b     = fCSHINESSDCalibratedData.GetSiCaliSlope("L2B");

  fInterceptl1s = fCSHINESSDCalibratedData.GetSiCaliIntercept("L1S");
  fInterceptl2f = fCSHINESSDCalibratedData.GetSiCaliIntercept("L2F");
  fInterceptl2b = fCSHINESSDCalibratedData.GetSiCaliIntercept("L2B");
}

//______________________________________________________________________________
CSHINEBuildEvent::~CSHINEBuildEvent()
{}
//******************************************************************************


//******************************************************************************
void CSHINEBuildEvent::BuildLayerEvent(CSHINELayerEvent& layerevent, Int_t ssdindex,
    Int_t* echl1s, Int_t* echl2f, Int_t* echl2b, Int_t* echcsi)
{
  //_________________________________________________
  // 1.获取 multipicity, 并将点火的 channel index 存进 vector
  layerevent.fL1SMulti = 0;
  layerevent.fL2FMulti = 0;
  layerevent.fL2BMulti = 0;
  layerevent.fCsIMulti = 0;

  vector<Int_t> fChannels_l1s;
  vector<Int_t> fChannels_l2f;
  vector<Int_t> fChannels_l2b;
  vector<Int_t> fChannels_csi;

  for (Int_t chindex=0; chindex<NUM_STRIP; chindex++) {
    if (echl1s[chindex]>fSiEChcutl1s[ssdindex*NUM_STRIP+chindex]) {
      layerevent.fL1SMulti ++;
      fChannels_l1s.push_back(chindex);
    }
    if (echl2f[chindex]>fSiEChcutl2f[ssdindex*NUM_STRIP+chindex]) {
      layerevent.fL2FMulti ++;
      fChannels_l2f.push_back(chindex);
    }
    if (echl2b[chindex]>fSiEChcutl2b[ssdindex*NUM_STRIP+chindex]) {
      layerevent.fL2BMulti ++;
      fChannels_l2b.push_back(chindex);
    }
  }
  for (Int_t chindex=0; chindex<NUM_CSI; chindex++) {
    if (echcsi[chindex]>fCsIEChcutl3a[ssdindex*NUM_CSI+chindex]) {
      layerevent.fCsIMulti ++;
      fChannels_csi.push_back(chindex);
    }
  }

  //_____________________________________
  // 2.获取点火的 channel index, 并计算能量
  for (Int_t m=0; m<layerevent.fL1SMulti; m++) {
    layerevent.fL1SNumStrip[m] = fChannels_l1s[m];
    layerevent.fL1SEMeV    [m] = fSlopel1s[ssdindex*NUM_STRIP+fChannels_l1s[m]]*echl1s[fChannels_l1s[m]]+
                               fInterceptl1s[ssdindex*NUM_STRIP+fChannels_l1s[m]];
  }
  for (Int_t m=0; m<layerevent.fL2FMulti; m++) {
    layerevent.fL2FNumStrip[m]  = fChannels_l2f[m];
    layerevent.fL2FEMeV    [m]  = fSlopel2f[ssdindex*NUM_STRIP+fChannels_l2f[m]]*echl2f[fChannels_l2f[m]]+
                                 fInterceptl2f[ssdindex*NUM_STRIP+fChannels_l2f[m]];
  }
  for (Int_t m=0; m<layerevent.fL2BMulti; m++) {
    layerevent.fL2BNumStrip[m]  = fChannels_l2b[m];
    layerevent.fL2BEMeV    [m]  = fSlopel2b[ssdindex*NUM_STRIP+fChannels_l2b[m]]*echl2b[fChannels_l2b[m]]+
                                 fInterceptl2b[ssdindex*NUM_STRIP+fChannels_l2b[m]];
  }
  for (Int_t m=0; m<layerevent.fCsIMulti; m++) {
    layerevent.fCsINum[m] = fChannels_csi[m];
    layerevent.fCsIECh[m] = echcsi[fChannels_csi[m]];
  }
}
//******************************************************************************


//******************************************************************************
// 创建 LayerEvent 的树结构
void CSHINEBuildEvent::BuildLayerEventTree(Int_t firstrun, Int_t lastrun)
{
  TimeAndPercentage time;
  time.GetBeginTime();

  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string L3ATag("L3A");

  std::string pathRootInputFolder(Form("%sMapRoot/", PATHROOTFILESFOLDER));
  std::string pathEventTreeOutput(Form("%sCSHINEEvent/", PATHROOTFILESFOLDER));

  Int_t SSD_L1S_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2F_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2B_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L3A_E[NUM_SSD][NUM_CSI];

  cout<<Form("Build Event for Run%04d-Run%04d", firstrun, lastrun)<<endl;
  TFile* file   = new TFile(Form("%sEventTree_Run%04d-Run%04d.root",pathEventTreeOutput.c_str(),
                                  firstrun, lastrun), "RECREATE");
  TTree* mytree = new TTree("CSHINEEvent", "CSHINEEvent Tree");

  CSHINEBuildEvent    buildevent;

  CSHINELayerEvent    layerevent1;
  CSHINELayerEvent    layerevent2;
  CSHINELayerEvent    layerevent3;
  CSHINELayerEvent    layerevent4;
  CSHINEGlobalEvent   event_g;

  mytree->Branch("SSD1.",   "CSHINELayerEvent",  &layerevent1,  32000, 2);
  mytree->Branch("SSD2.",   "CSHINELayerEvent",  &layerevent2,  32000, 2);
  mytree->Branch("SSD3.",   "CSHINELayerEvent",  &layerevent3,  32000, 2);
  mytree->Branch("SSD4.",   "CSHINELayerEvent",  &layerevent4,  32000, 2);
  mytree->Branch("Global.", "CSHINEGlobalEvent", &event_g,      32000, 2);

  TChain* mychain = new TChain("KrPb");
  for (Int_t i=firstrun; i<(lastrun+1); i++) {
    mychain->Add(Form("%sMapFission2019_Kr_Pb%04d.root",pathRootInputFolder.c_str(),i));
  }
  mychain->SetBranchStatus("*",false);

  ReadFileModule readfile;
  for (Int_t i=0; i<NUM_SSD; i++)
  {
    readfile.AddChain(mychain,L1STag.c_str(),SSD_L1S_E[i],NUM_STRIP,i);
    readfile.AddChain(mychain,L2FTag.c_str(),SSD_L2F_E[i],NUM_STRIP,i);
    readfile.AddChain(mychain,L2BTag.c_str(),SSD_L2B_E[i],NUM_STRIP,i);
    readfile.AddChain(mychain,L3ATag.c_str(),SSD_L3A_E[i],NUM_CSI,  i);
  }

  //____________________________________________________________________________
  // 逐一读取事件进行分析
  unsigned long nentries = mychain->GetEntries();
  cout<< "Found " << nentries <<" entries"<<endl;
  for (unsigned long jentry=0; jentry<nentries; jentry++)
  {
    mychain->GetEntry(jentry);

    buildevent.BuildLayerEvent(layerevent1,0,SSD_L1S_E[0],SSD_L2F_E[0],SSD_L2B_E[0],SSD_L3A_E[0]);
    buildevent.BuildLayerEvent(layerevent2,1,SSD_L1S_E[1],SSD_L2F_E[1],SSD_L2B_E[1],SSD_L3A_E[1]);
    buildevent.BuildLayerEvent(layerevent3,2,SSD_L1S_E[2],SSD_L2F_E[2],SSD_L2B_E[2],SSD_L3A_E[2]);
    buildevent.BuildLayerEvent(layerevent4,3,SSD_L1S_E[3],SSD_L2F_E[3],SSD_L2B_E[3],SSD_L3A_E[3]);

    mytree->Fill();

    time.PrintPercentageAndRemainingTime(jentry, nentries);
  }
  mytree->Write();
  file->Close();

  time.GetEndTime();
  time.GetRunTime();
}
//******************************************************************************
