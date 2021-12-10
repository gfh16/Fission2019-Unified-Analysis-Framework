#ifndef CSHINEUNCALIDATATOPHYSICSEVENT_H
#define CSHINEUNCALIDATATOPHYSICSEVENT_H

#include "CSHINEEvent.h"
#include "CSHINESSDCalibratedData.h"
#include "CSHINETrackReconstructionSimplified.h"
#include "CSHINEParticleIdentificationSimplified.h"
#include "CSHINEPPACCalibration.h"
#include "shared.h"

#include "TTree.h"
#include "TChain.h"
#include "TFile.h"



//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// CSHINEUnCaliDataToPhysicsEvent 用于从 Map 转换后的原始数据, 输出刻度好的物理事件
//
class CSHINEUnCaliDataToPhysicsEvent_SSD
{
public:
	CSHINEUnCaliDataToPhysicsEvent_SSD();
	CSHINEUnCaliDataToPhysicsEvent_SSD(TChain* InputChain);
	~CSHINEUnCaliDataToPhysicsEvent_SSD();

	virtual void  PrintUsage();

private:
	// 初始化
  TChain*         fChain_MapRoot;
	virtual void    Init_MapRootTree_SSD();
	virtual void    Init_SiCaliPars();

	CSHINETrackReconstructionSimplified    trackresconstruct;
	CSHINEParticleIdentificationSimplified particleidentification;

//____________________________
// 功能实现
public:
  virtual void    FillLayerEvent(CSHINELayerEvent2*  layerevent);
	virtual void    FillTrackEvent(CSHINETrackEvent2* trackevent, CSHINELayerEvent2* layerevent);
	virtual void    FillTrackEvent_L2L3(CSHINETrackEvent2* trackevent, CSHINELayerEvent2* layerevent);
	virtual void    FillTrackEvent_L1L2(CSHINETrackEvent2* trackevent, CSHINELayerEvent2* layerevent);


private:
	Int_t           fZone_index_L1L2;
  const Int_t     ZONE_LENGTH_L1L2 = 8;

//____________________________
// 构建事件
private:
  Int_t  TelNum_BananaCut;
	Int_t  SSDL1SMulti[NUM_SSD];
  Int_t  SSDL2FMulti[NUM_SSD];
  Int_t  SSDL2BMulti[NUM_SSD];
  Int_t  SSDCsIMulti[NUM_SSD];

//_____________________________
// 读取硅条参数
private:
	Double_t*     fSiEChcutl1s;   // [NUM_SSD*NUM_STRIP]
  Double_t*     fSiEChcutl2f;   // [NUM_SSD*NUM_STRIP]
  Double_t*     fSiEChcutl2b;   // [NUM_SSD*NUM_STRIP]
  Double_t*     fCsIEChcutl3a;  // [NUM_SSD*NUM_CSI]

  Double_t*     fSlopel1s;      // [NUM_SSD*NUM_STRIP]
  Double_t*     fSlopel2f;      // [NUM_SSD*NUM_STRIP]
  Double_t*     fSlopel2b;      // [NUM_SSD*NUM_STRIP]
  Double_t*     fInterceptl1s;  // [NUM_SSD*NUM_STRIP]
  Double_t*     fInterceptl2f;  // [NUM_SSD*NUM_STRIP]
  Double_t*     fInterceptl2b;  // [NUM_SSD*NUM_STRIP]

	const Int_t    NUMSIGMA = 5;  // cut = pedestal + 5*sigma
	CSHINESSDCalibratedData fCSHINESSDCalibratedData;


//_____________________________
// branch
private:
  const char*  L1STag  = "L1S";
  const char*  L2FTag  = "L2F";
  const char*  L2BTag  = "L2B";
  const char*  L3ATag  = "L3A";

	const char*  b_SSD1_L2F_T = "SSD1_L2F_T";
	const char*  b_SSD1_L1S_E = "SSD1_L1S_E";
  const char*  b_SSD1_L2F_E = "SSD1_L2F_E";
	const char*  b_SSD1_L2B_E = "SSD1_L2B_E";
	const char*  b_SSD1_L3A_E = "SSD1_L3A_E";

	const char*  b_SSD2_L2F_T = "SSD2_L2F_T";
	const char*  b_SSD2_L1S_E = "SSD2_L1S_E";
  const char*  b_SSD2_L2F_E = "SSD2_L2F_E";
	const char*  b_SSD2_L2B_E = "SSD2_L2B_E";
	const char*  b_SSD2_L3A_E = "SSD2_L3A_E";

	const char*  b_SSD3_L2F_T = "SSD3_L2F_T";
	const char*  b_SSD3_L1S_E = "SSD3_L1S_E";
  const char*  b_SSD3_L2F_E = "SSD3_L2F_E";
	const char*  b_SSD3_L2B_E = "SSD3_L2B_E";
	const char*  b_SSD3_L3A_E = "SSD3_L3A_E";

	const char*  b_SSD4_L2F_T = "SSD4_L2F_T";
	const char*  b_SSD4_L1S_E = "SSD4_L1S_E";
  const char*  b_SSD4_L2F_E = "SSD4_L2F_E";
	const char*  b_SSD4_L2B_E = "SSD4_L2B_E";
	const char*  b_SSD4_L3A_E = "SSD4_L3A_E";

  Int_t SSD_L2F_T[NUM_SSD][NUM_STRIP];
  Int_t SSD_L1S_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2F_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2B_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L3A_E[NUM_SSD][NUM_CSI];
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// PPAC 信号刻度
class CSHINEUnCaliDataToPhysicsEvent_PPAC
{
public:
	CSHINEUnCaliDataToPhysicsEvent_PPAC();
	CSHINEUnCaliDataToPhysicsEvent_PPAC(TChain* InputChain);
	~CSHINEUnCaliDataToPhysicsEvent_PPAC();

public:
	virtual void   FillPPACEvent(CSHINEPPACEvent* ppacevent);


private:
	CSHINEPPACCalibration fPPACCali;

private:
	TVector3              fVec3InPPACFrame;
	TVector3              fVec3InLabFrame;
	Double_t              fTOFWithRF1;
	Double_t              fVelocity_cm_per_ns;

private:
	// 初始化
  TChain*         fChain_MapRoot;
	virtual void    Init_MapRootTree_PPAC();

private:
	const char* b_PPAC1_T         = "PPAC1_T";
	const char* b_PPAC1_X1        = "PPAC1_X1";
	const char* b_PPAC1_X2        = "PPAC1_X2";
	const char* b_PPAC1_Y1        = "PPAC1_Y1";
	const char* b_PPAC1_Y2        = "PPAC1_Y2";
	const char* b_PPAC1_T_Energy  = "PPAC1_T_Energy";
	//
	const char* b_PPAC2_T         = "PPAC2_T";
	const char* b_PPAC2_X1        = "PPAC2_X1";
	const char* b_PPAC2_X2        = "PPAC2_X2";
	const char* b_PPAC2_Y1        = "PPAC2_Y1";
	const char* b_PPAC2_Y2        = "PPAC2_Y2";
	const char* b_PPAC2_T_Energy  = "PPAC2_T_Energy";
	//
	const char* b_PPAC3_T         = "PPAC3_T";
	const char* b_PPAC3_X1        = "PPAC3_X1";
	const char* b_PPAC3_X2        = "PPAC3_X2";
	const char* b_PPAC3_Y1        = "PPAC3_Y1";
	const char* b_PPAC3_Y2        = "PPAC3_Y2";
	const char* b_PPAC3_T_Energy  = "PPAC3_T_Energy";
	//
	const char* b_RF1             = "RF1";

private:
	Int_t PPAC_T [NUM_PPAC];
	Int_t PPAC_X1[NUM_PPAC];
	Int_t PPAC_X2[NUM_PPAC];
	Int_t PPAC_Y1[NUM_PPAC];
	Int_t PPAC_Y2[NUM_PPAC];
	Int_t PPAC_TE[NUM_PPAC];
	Int_t RF1;

private:
	Int_t   gMulti;
	Bool_t  PPAC_X1_GOOG[NUM_PPAC];
	Bool_t  PPAC_X2_GOOG[NUM_PPAC];
	Bool_t  PPAC_Y1_GOOG[NUM_PPAC];
	Bool_t  PPAC_Y2_GOOG[NUM_PPAC];
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// Au-Si 面垒望远镜事件
class CSHINEUnCaliDataToPhysicsEvent_AuSiTel
{
public:
	CSHINEUnCaliDataToPhysicsEvent_AuSiTel();
	CSHINEUnCaliDataToPhysicsEvent_AuSiTel(TChain* InputChain);
	~CSHINEUnCaliDataToPhysicsEvent_AuSiTel();

public:
	virtual void   FillAuSiTelEvent(CSHINEAuSiTelEvent* AuSievent);

private:
	// 初始化
  TChain*         fChain_MapRoot;
	virtual void    Init_MapRootTree_AuSiTel();

private:
	const char* b_AuSi1_L1T = "AuSi1_L1T";
	const char* b_AuSi1_L1E = "AuSi1_L1E";
	const char* b_AuSi1_L2E = "AuSi1_L2E";
	const char* b_AuSi2_L1T = "AuSi2_L1T";
	const char* b_AuSi2_L1E = "AuSi2_L1E";
	const char* b_AuSi2_L2E = "AuSi2_L2E";

private:
	static const Int_t NUM_AUSI = 2;
  Int_t  AuSi_L1T[NUM_AUSI];
	Int_t  AuSi_L1E[NUM_AUSI];
	Int_t  AuSi_L2E[NUM_AUSI];

private:
	Int_t  gMulti;
	const Int_t AUSI_TDC_CUT = 500;
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo

//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// 高频信号
class CSHINEUnCaliDataToPhysicsEvent_RF
{
public:
	CSHINEUnCaliDataToPhysicsEvent_RF();
	CSHINEUnCaliDataToPhysicsEvent_RF(TChain* InputChain);
	~CSHINEUnCaliDataToPhysicsEvent_RF();

public:
	virtual void   FillRFSignal(CSHINERFSignal* rfsignal);

private:
	CSHINEPPACCalibration fPPACCali;

private:
	// 初始化
  TChain*         fChain_MapRoot;
	virtual void    Init_MapRootTree_RF();

private:
	const char* b_RF1 = "RF1";
	const char* b_RF2 = "RF2";

	Int_t RF1;
	Int_t RF2;
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo

#endif
