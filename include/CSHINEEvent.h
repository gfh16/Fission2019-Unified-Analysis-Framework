#ifndef CSHINEEVENT_H
#define CSHINEEVENT_H

#include "shared.h"
#include "CSHINESSDCalibratedData.h"
#include "ReadFileModule.h"
#include "TimeAndPercentage.h"

#include <vector>
#include <string>
#include "TChain.h"
#include "TCanvas.h"
#include "TROOT.h"


//______________________________________________________________________________
// LayerEvent 是指: 对于每一个 entry, 确定各套硅条探测器各层的粒子多重性 LayerMulti，
// 并以此为变量, 记录每一层的事件信息
// 为了后续数据读取方便, 将四套硅条望远镜每一层的信息存到同一个branch中,
// 因此定义了四个变量以下四个变量, 用于记录每一个事件所属 SSD 的编号
// fL1SSSDNum, fL2FSSDNum, fL2BSSDNum, fCsISSDNum
class CSHINELayerEvent
{
public:
  CSHINELayerEvent();
  ~CSHINELayerEvent();
//ClassDef(CSHINELayerEvent, 1);


public:
  // for L1S
  Int_t                  fL1SMulti;
  std::vector<Int_t>     fL1SSSDNum;      //[fL1SMulti]
  std::vector<Int_t>     fL1SNumStrip;    //[fL1SMulti]
  std::vector<Double_t>  fL1SEMeV;        //[fL1SMulti]
  // for L2F
  Int_t                  fL2FMulti;
  std::vector<Int_t>     fL2FSSDNum;      //[fL2FMulti]
  std::vector<Int_t>     fL2FNumStrip;    //[fL2FMulti]
  std::vector<Double_t>  fL2FEMeV;        //[fL2FMulti]
  std::vector<Int_t>     fL2FTime;        //[fL2FMulti]
  // for L2B
  Int_t                  fL2BMulti;
  std::vector<Int_t>     fL2BSSDNum;     //[fL2BMulti]
  std::vector<Int_t>     fL2BNumStrip;   //[fL2BMulti]
  std::vector<Double_t>  fL2BEMeV;       //[fL2BMulti]
  // for CsI
  Int_t                  fCsIMulti;
  std::vector<Int_t>     fCsISSDNum;    //[fCsIMulti]
  std::vector<Int_t>     fCsINum;       //[fCsIMulti]
  std::vector<Int_t>     fCsIECh;       //[fCsIMulti]

  std::vector<Int_t>     fSSDL1SMulti;  //[NUM_SSD]
  std::vector<Int_t>     fSSDL2FMulti;  //[NUM_SSD]
  std::vector<Int_t>     fSSDL2BMulti;  //[NUM_SSD]
  std::vector<Int_t>     fSSDCsIMulti;  //[NUM_SSD]


  void Swap()
  {
    std::vector<Int_t>().swap(fL1SSSDNum);
    std::vector<Int_t>().swap(fL1SNumStrip);
    std::vector<Double_t>().swap(fL1SEMeV);
    std::vector<Int_t>().swap(fL2FSSDNum);
    std::vector<Int_t>().swap(fL2FNumStrip);
    std::vector<Double_t>().swap(fL2FEMeV);
    std::vector<Int_t>().swap(fL2FTime);
    std::vector<Int_t>().swap(fL2BSSDNum);
    std::vector<Int_t>().swap(fL2BNumStrip);
    std::vector<Double_t>().swap(fL2BEMeV);
    std::vector<Int_t>().swap(fCsISSDNum);
    std::vector<Int_t>().swap(fCsINum);
    std::vector<Int_t>().swap(fCsIECh);

    std::vector<Int_t>().swap(fSSDL1SMulti);
    std::vector<Int_t>().swap(fSSDL2FMulti);
    std::vector<Int_t>().swap(fSSDL2BMulti);
    std::vector<Int_t>().swap(fSSDCsIMulti);
  }

  ClassDef(CSHINELayerEvent,1);
};

//______________________________________________________-
// 实际数据转换中, 使用以下 CSHINELayerEvent2 来存储数据
class CSHINELayerEvent2
{
public:
  CSHINELayerEvent2();
  ~CSHINELayerEvent2();
//ClassDef(CSHINELayerEvent, 1);


public:
  // for L1S
  Int_t                  fL1SMulti[NUM_SSD];
  std::vector<Int_t>     fL1SNumStrip[NUM_SSD]; //[fL1SMulti[NUM_SSD]]
  std::vector<Double_t>  fL1SEMeV[NUM_SSD];     //[fL1SMulti[NUM_SSD]]
  std::vector<Double_t>  fL1SECh[NUM_SSD];      //[fL1SMulti[NUM_SSD]]
  // for L2F
  Int_t                  fL2FMulti[NUM_SSD];
  std::vector<Int_t>     fL2FNumStrip[NUM_SSD]; //[fL2FMulti[NUM_SSD]]
  std::vector<Double_t>  fL2FEMeV[NUM_SSD];     //[fL2FMulti[NUM_SSD]]
  std::vector<Double_t>  fL2FECh[NUM_SSD];      //[fL2FMulti[NUM_SSD]]
  std::vector<Int_t>     fL2FTime[NUM_SSD];     //[fL2FMulti[NUM_SSD]]
  // for L2B
  Int_t                  fL2BMulti[NUM_SSD];
  std::vector<Int_t>     fL2BNumStrip[NUM_SSD]; //[fL2BMulti[NUM_SSD]]
  std::vector<Double_t>  fL2BEMeV[NUM_SSD];     //[fL2BMulti[NUM_SSD]]
  std::vector<Double_t>  fL2BECh[NUM_SSD];      //[fL2BMulti[NUM_SSD]]
  // for CsI
  Int_t                  fCsIMulti[NUM_SSD];
  std::vector<Int_t>     fCsINum[NUM_SSD];     //[fCsIMulti[NUM_SSD]]
  std::vector<Int_t>     fCsIECh[NUM_SSD];     //[fCsIMulti[NUM_SSD]]


  void Swap()
  {
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {
      std::vector<Int_t>().swap(fL1SNumStrip[ssdindex]);
      std::vector<Double_t>().swap(fL1SEMeV[ssdindex]);
      std::vector<Double_t>().swap(fL1SECh[ssdindex]);
      std::vector<Int_t>().swap(fL2FNumStrip[ssdindex]);
      std::vector<Double_t>().swap(fL2FEMeV[ssdindex]);
      std::vector<Double_t>().swap(fL2FECh[ssdindex]);
      std::vector<Int_t>().swap(fL2FTime[ssdindex]);
      std::vector<Int_t>().swap(fL2BNumStrip[ssdindex]);
      std::vector<Double_t>().swap(fL2BEMeV[ssdindex]);
      std::vector<Double_t>().swap(fL2BECh[ssdindex]);
      std::vector<Int_t>().swap(fCsINum[ssdindex]);
      std::vector<Int_t>().swap(fCsIECh[ssdindex]);
    }
  }

  ClassDef(CSHINELayerEvent2,1);
};
//______________________________________________________________________________


//______________________________________________________________________________
// CSHINETrackEvent 是在硅条的径迹重建中生成的 .root 文件格式
// 该文件格式中的 tree 结构还不是最终的数据格式, 只是为了方便进一步的数据分析
// 为了与 CSHINELayerEvent 中的变量区分开, CSHINETrackEvent 的变量同意用 "fG" 开头
// 表示与 fGlobalMulti 有关的变量
//
// P.S. TrackEvent 是指, 只使用几何约束得到的候选径迹事件
//
class CSHINETrackEvent
{
public:
  CSHINETrackEvent();
  ~CSHINETrackEvent();


public:
  Int_t                  fGlobalMulti;
  std::vector<Int_t>     fSSDGlobalMulti;   //[NUM_SSD]
  std::vector<Int_t>     fGSSDNum;          //[fGlobalMulti]
  std::vector<Int_t>     fGL1SNumStrip;     //[fGlobalMulti]
  std::vector<Double_t>  fGL1SEMeV;         //[fGlobalMulti]
  std::vector<Int_t>     fGL2FNumStrip;     //[fGlobalMulti]
  std::vector<Double_t>  fGL2FEMeV;         //[fGlobalMulti]
  std::vector<Int_t>     fGL2BNumStrip;     //[fGlobalMulti]
  std::vector<Double_t>  fGL2BEMeV;         //[fGlobalMulti]
  std::vector<Int_t>     fGCsINum;          //[fGlobalMulti]
  std::vector<Int_t>     fGCsIECh;          //[fGlobalMulti]

  std::vector<Int_t>     fGL2FTime;         //[fGlobalMulti]

  void Swap()
  {
    std::vector<Int_t>().swap(fGSSDNum);
    std::vector<Int_t>().swap(fSSDGlobalMulti);
    std::vector<Int_t>().swap(fGL1SNumStrip);
    std::vector<Double_t>().swap(fGL1SEMeV);
    std::vector<Int_t>().swap(fGL2FNumStrip);
    std::vector<Double_t>().swap(fGL2FEMeV);
    std::vector<Int_t>().swap(fGL2BNumStrip);
    std::vector<Double_t>().swap(fGL2BEMeV);
    std::vector<Int_t>().swap(fGCsINum);
    std::vector<Int_t>().swap(fGCsIECh);
    std::vector<Int_t>().swap(fGL2FTime);
  }

  ClassDef(CSHINETrackEvent,1);
};



//______________________________________________________________________________
// GeoTrackEvent 是将四套硅条望远镜数据分别存在长度为 4 的 vector 数组里面
// 这样方便在作径迹重建时数据读取
//
// P.S. TrackEvent 是指, 只使用几何约束得到的候选径迹事件
//
// 实际数据处理中, 使用以下 CSHINETrackEvent2 存储数据
class CSHINETrackEvent2
{
public:
  CSHINETrackEvent2();
  ~CSHINETrackEvent2();

public:

  std::vector<Double_t>  fL1SNumStrip[NUM_SSD];
  std::vector<Double_t>  fL1SEMeV[NUM_SSD];
  std::vector<Double_t>  fL1SECh[NUM_SSD];
  std::vector<Double_t>  fL1SEMeV_Corrected[NUM_SSD]; // for L1L2 track resconstruction
  std::vector<Double_t>  fL2FNumStrip[NUM_SSD];
  std::vector<Double_t>  fL2FEMeV[NUM_SSD];
  std::vector<Double_t>  fL2FECh[NUM_SSD];
  std::vector<Double_t>  fL2BNumStrip[NUM_SSD];
  std::vector<Double_t>  fL2BEMeV[NUM_SSD];
  std::vector<Double_t>  fL2BECh[NUM_SSD];
  std::vector<Double_t>  fCsINum[NUM_SSD];
  std::vector<Double_t>  fCsIECh[NUM_SSD];

  std::vector<Int_t>     fL2FTime[NUM_SSD];

  std::vector<Int_t>     fCutTelNum[NUM_SSD];        // for L2L3 track resconstruction

  void Swap()
  {
    for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++) {

      std::vector<Double_t>().swap(fL1SNumStrip[ssdindex]);
      std::vector<Double_t>().swap(fL2FNumStrip[ssdindex]);
      std::vector<Double_t>().swap(fL2BNumStrip[ssdindex]);
      std::vector<Double_t>().swap(fCsINum[ssdindex]);

      std::vector<Double_t>().swap(fL1SEMeV[ssdindex]);
      std::vector<Double_t>().swap(fL1SECh[ssdindex]);
      std::vector<Double_t>().swap(fL1SEMeV_Corrected[ssdindex]);
      std::vector<Double_t>().swap(fL2FEMeV[ssdindex]);
      std::vector<Double_t>().swap(fL2FECh[ssdindex]);
      std::vector<Double_t>().swap(fL2BEMeV[ssdindex]);
      std::vector<Double_t>().swap(fL2BECh[ssdindex]);
      std::vector<Double_t>().swap(fCsIECh[ssdindex]);

      std::vector<Int_t>().swap(fL2FTime[ssdindex]);

      std::vector<Int_t>().swap(fCutTelNum[ssdindex]);
    }
  }

  ClassDef(CSHINETrackEvent2,1);
};



//______________________________________________________________________________
// 将四套硅条探测器合并起来讨论. 以总的粒子多重性为自变量记录事件信息.
class CSHINESSDEvent
{
public:
  CSHINESSDEvent();
  ~CSHINESSDEvent();
  //ClassDef(CSHINESSDEvent, 1);

public:
  Int_t                    fGlobalMulti;       // 四套SSD总事件数

  std::vector<Int_t>       fSSDGlobalMulti;    //[NUM_SSD],      每套SSD的事件数
  std::vector<Int_t>       fGSSDNum;           //[fGlobalMulti], 每个事件点火的SSD编号
  std::vector<Double_t>    fGL1SNumStrip;      //[fGlobalMulti], L1S 点火条的编号
  std::vector<Double_t>    fGL1SEMeV;          //[fGlobalMulti], L1S 能量, MeV
  std::vector<Double_t>    fGL1SEMeVCorrected; //[fGlobalMulti], L1S 能量,考虑L1的厚度修正
  std::vector<Double_t>    fGL1SECh;           //[fGlobalMulti], L1S 能量, Ch
  std::vector<Double_t>    fGL2FNumStrip;      //[fGlobalMulti], L2F 点火条的编号
  std::vector<Double_t>    fGL2FEMeV;          //[fGlobalMulti], L2F 能量, MeV
  std::vector<Double_t>    fGL2FECh;           //[fGlobalMulti], L2F 能量, Ch
  std::vector<Double_t>    fGL2BNumStrip;      //[fGlobalMulti], L2B 点火条的编号
  std::vector<Double_t>    fGL2BEMeV;          //[fGlobalMulti], L2B 能量, MeV
  std::vector<Double_t>    fGL2BECh;           //[fGlobalMulti], L2B 能量, Ch
  std::vector<Double_t>    fGCsINum;           //[fGlobalMulti], CsI 点火编号
  std::vector<Double_t>    fGCsIEMeV;          //[fGlobalMulti], CsI 能量, MeV
  std::vector<Double_t>    fGCsIECh;           //[fGlobalMulti], CsI 能量, Ch

  std::vector<Int_t>       fGL2FTime;          //[fGlobalMulti], L2F 时间信号

  std::vector<Double_t>    fDist;              //[fGlobalMulti], 粒子击中位置到靶点的距离
  std::vector<Double_t>    fTheta;             //[fGlobalMulti], 极角(实验室系)
  std::vector<Double_t>    fPhi;               //[fGlobalMulti], 方位角(实验室系)
  std::vector<Double_t>    fTotEkinc;          //[fGlobalMulti], 入射粒子总动能 (MeV)
  std::vector<Double_t>    fP;                 //[fGlobalMulti], 粒子动量大小 (MeV/c)
  std::vector<Double_t>    fBeta;              //[fGlobalMulti], 粒子速度大小 (v/c)
  std::vector<Int_t>       fZ;                 //[fGlobalMulti], 电荷数
  std::vector<Int_t>       fA;                 //[fGlobalMulti], 质量数 (Int_t)
  std::vector<Double_t>    fAd;                //[fGlobalMulti], 质量数 (Double_t)
  std::vector<Double_t>    fPIDNum;            //[fGlobalMulti], PID 数

  std::vector<Int_t>       fTrackModeID;     //[fGlobalMulti], 径迹模式 ID


  void Swap()
  {
    std::vector<Int_t>().swap(fSSDGlobalMulti);
    std::vector<Int_t>().swap(fGSSDNum);
    std::vector<Double_t>().swap(fGL1SNumStrip);
    std::vector<Double_t>().swap(fGL1SEMeV);
    std::vector<Double_t>().swap(fGL1SEMeVCorrected);
    std::vector<Double_t>().swap(fGL1SECh);
    std::vector<Double_t>().swap(fGL2FNumStrip);
    std::vector<Double_t>().swap(fGL2FEMeV);
    std::vector<Double_t>().swap(fGL2FECh);
    std::vector<Double_t>().swap(fGL2BNumStrip);
    std::vector<Double_t>().swap(fGL2BEMeV);
    std::vector<Double_t>().swap(fGL2BECh);
    std::vector<Double_t>().swap(fGCsINum);
    std::vector<Double_t>().swap(fGCsIEMeV);
    std::vector<Double_t>().swap(fGCsIECh);

    std::vector<Int_t>().swap(fGL2FTime);

    std::vector<Double_t>().swap(fDist);
    std::vector<Double_t>().swap(fTheta);
    std::vector<Double_t>().swap(fPhi);
    std::vector<Double_t>().swap(fTotEkinc);
    std::vector<Double_t>().swap(fP);
    std::vector<Double_t>().swap(fBeta);
    std::vector<Int_t>().swap(fZ);
    std::vector<Int_t>().swap(fA);
    std::vector<Double_t>().swap(fAd);
    std::vector<Double_t>().swap(fPIDNum);

    std::vector<Int_t>().swap(fTrackModeID);
  }


  ClassDef(CSHINESSDEvent,1);
};
//______________________________________________________________________________


//______________________________________________________________________________
class CSHINEPPACEvent
{
public:
  CSHINEPPACEvent();
  ~CSHINEPPACEvent();

public:
  Int_t                    fFFMulti;        // PPAC阵列中探测到的碎片总数
  std::vector<Int_t>       fPPACFiredNum;   // [fFFMulti], 点火的PPAC编号
  std::vector<Int_t>       fT;              // [fFFMulti], PPAC原始T信号
  std::vector<Int_t>       fX1;             // [fFFMulti], PPAC原始X1信号
  std::vector<Int_t>       fX2;             // [fFFMulti], PPAC原始X2信号
  std::vector<Int_t>       fY1;             // [fFFMulti], PPAC原始Y1信号
  std::vector<Int_t>       fY2;             // [fFFMulti], PPAC原始Y2信号
  std::vector<Int_t>       fTE;             // [fFFMulti], PPAC阴极膜的能量信号

  std::vector<Double_t>    fDist;           // [fFFMulti], 击中点到靶点的距离 [实验室系]
  std::vector<Double_t>    fTheta;          // [fFFMulti], 击中点的 theta 角 [实验室系]
  std::vector<Double_t>    fPhi;            // [fFFMulti], 击中点的 phi 角   [实验室系]


  void Swap()
  {
    std::vector<Int_t>().swap(fPPACFiredNum);
    std::vector<Int_t>().swap(fT);
    std::vector<Int_t>().swap(fX1);
    std::vector<Int_t>().swap(fX2);
    std::vector<Int_t>().swap(fY1);
    std::vector<Int_t>().swap(fY2);
    std::vector<Int_t>().swap(fTE);

    std::vector<Double_t>().swap(fDist);
    std::vector<Double_t>().swap(fTheta);
    std::vector<Double_t>().swap(fPhi);
  }
};
//______________________________________________________________________________


//______________________________________________________________________________
// Fission2019实验中, Au-Si面垒望远镜用来测量弹性散射 (监测束流)
// 只使用两叠层: Si + CsI
class CSHINEAuSiTelEvent
{
public:
  CSHINEAuSiTelEvent();
  ~CSHINEAuSiTelEvent();

public:
  Int_t                  fGlobalMulti; // 总粒子多重性
  std::vector<Int_t>     fTelNum;      // [fGlobalMulti], 点火探测器编号
  std::vector<Int_t>     fL1Time;      // [fGlobalMulti], L1 (Si) 的时间信号
  std::vector<Int_t>     fL1ESi;       // [fGlobalMulti], L1 (Si) 的能量信号
  std::vector<Int_t>     fL2ECsI;      // [fGlobalMulti], L2 (CsI) 的能量信号

  void Swap()
  {
    std::vector<Int_t>().swap(fTelNum);
    std::vector<Int_t>().swap(fL1Time);
    std::vector<Int_t>().swap(fL1ESi);
    std::vector<Int_t>().swap(fL2ECsI);
  }


  ClassDef(CSHINEAuSiTelEvent,1);
};
//______________________________________________________________________________


//______________________________________________________________________________
class CSHINERFSignal
{
public:
  CSHINERFSignal();
  ~CSHINERFSignal();

public:
  Int_t     fRF1Shifted; // 将第1路高频信号平移到同一个周期内, 作为实际的高频信号
  Int_t     fRF1;        // 第1路高频信号
  Int_t     fRF2;        // 第2路高频信号


  ClassDef(CSHINERFSignal,1);
};
//______________________________________________________________________________


//______________________________________________________________________________
class CSHINEBuildEvent
{
public:
  CSHINEBuildEvent();
  ~CSHINEBuildEvent();

  // 参数说明: CSHINESSDEvent class, ssdindex, ech_L1S[16], ech_L2F[16], ech_L2B[16], ech_L3A[9], time_L2F[16]
  void LayerEvent (CSHINELayerEvent&,  Int_t,  Int_t*, Int_t*, Int_t*, Int_t*, Int_t*);

  void BuildLayerEvent(CSHINELayerEvent& layerevent,
      Int_t* ssd1echl1s, Int_t* ssd1echl2f, Int_t* ssd1echl2b, Int_t* ssd1echcsi, Int_t* ssd1timel2f,
      Int_t* ssd2echl1s, Int_t* ssd2echl2f, Int_t* ssd2echl2b, Int_t* ssd2echcsi, Int_t* ssd2timel2f,
      Int_t* ssd3echl1s, Int_t* ssd3echl2f, Int_t* ssd3echl2b, Int_t* ssd3echcsi, Int_t* ssd3timel2f,
      Int_t* ssd4echl1s, Int_t* ssd4echl2f, Int_t* ssd4echl2b, Int_t* ssd4echcsi, Int_t* ssd4timel2f);


  void BuildLayerEventTree (Int_t fisrtrun, Int_t lastlun);

private:
  Double_t     *fSiEChcutl1s;   // [NUM_SSD*NUM_STRIP]
  Double_t     *fSiEChcutl2f;   // [NUM_SSD*NUM_STRIP]
  Double_t     *fSiEChcutl2b;   // [NUM_SSD*NUM_STRIP]
  Double_t     *fCsIEChcutl3a;  // [NUM_SSD*NUM_CSI]

  Double_t     *fSlopel1s;      // [NUM_SSD*NUM_STRIP]
  Double_t     *fSlopel2f;      // [NUM_SSD*NUM_STRIP]
  Double_t     *fSlopel2b;      // [NUM_SSD*NUM_STRIP]
  Double_t     *fInterceptl1s;  // [NUM_SSD*NUM_STRIP]
  Double_t     *fInterceptl2f;  // [NUM_SSD*NUM_STRIP]
  Double_t     *fInterceptl2b;  // [NUM_SSD*NUM_STRIP]

  Int_t         SSDL1SMulti;
  Int_t         SSDL2FMulti;
  Int_t         SSDL2BMulti;
  Int_t         SSDCsIMulti;

  CSHINESSDCalibratedData fCSHINESSDCalibratedData;
};
//______________________________________________________________________________

#endif
