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

  CSHINELayerEvent();
  ~CSHINELayerEvent();
//ClassDef(CSHINELayerEvent, 1);

  void Swap()
  {
    std::vector<Int_t>().swap(fL1SSSDNum);
    std::vector<Int_t>().swap(fL1SNumStrip);
    std::vector<Double_t>().swap(fL1SEMeV);
    std::vector<Int_t>().swap(fL2FSSDNum);
    std::vector<Int_t>().swap(fL2FNumStrip);
    std::vector<Double_t>().swap(fL2FEMeV);
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
};
//______________________________________________________________________________


//______________________________________________________________________________
// CSHINETrackEvent 是在硅条的径迹重建中生成的 .root 文件格式
// 该文件格式中的 tree 结构还不是最终的数据格式, 只是为了方便进一步的数据分析
// 为了与 CSHINELayerEvent 中的变量区分开, CSHINETrackEvent 的变量同意用 "fG" 开头
// 表示与 fGlobalMulti 有关的变量
class CSHINETrackEvent
{
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

  CSHINETrackEvent();
  ~CSHINETrackEvent();

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
  }
};


//______________________________________________________________________________
// 将四套硅条探测器合并起来讨论. 以总的粒子多重性为自变量记录事件信息.
class CSHINEGlobalEvent
{
public:
  Int_t                    fGlobalMulti;

  std::vector<Int_t>       fGSSDNum;
  std::vector<Int_t>       fGL1SNumStrip;
  std::vector<Double_t>    fGL1SEMeV;
  std::vector<Int_t>       fGL2FNumStrip;
  std::vector<Double_t>    fGL2FEMeV;
  std::vector<Int_t>       fGL2BNumStrip;
  std::vector<Double_t>    fGL2BEMeV;
  std::vector<Int_t>       fGCsINum;
  std::vector<Double_t>    fGCsIEMeV;

  std::vector<Int_t>       fGL2FTime;

  std::vector<Double_t>    fTheta;
  std::vector<Double_t>    fPhi;
  std::vector<Double_t>    fTotKinEnergy;
  std::vector<Double_t>    fCalKinEnergy;
  std::vector<Double_t>    fMomentum;
  std::vector<Double_t>    fBeta;
  std::vector<Int_t>       fZ;
  std::vector<Int_t>       fA;

  CSHINEGlobalEvent();
  ~CSHINEGlobalEvent();
  //ClassDef(CSHINEGlobalEvent, 1);

  void Swap()
  {
    std::vector<Int_t>().swap(fGSSDNum);
    std::vector<Int_t>().swap(fGL1SNumStrip);
    std::vector<Double_t>().swap(fGL1SEMeV);
    std::vector<Int_t>().swap(fGL2FNumStrip);
    std::vector<Double_t>().swap(fGL2FEMeV);
    std::vector<Int_t>().swap(fGL2BNumStrip);
    std::vector<Double_t>().swap(fGL2BEMeV);
    std::vector<Int_t>().swap(fGCsINum);
    std::vector<Double_t>().swap(fGCsIEMeV);

    std::vector<Int_t>().swap(fGL2FTime);

    std::vector<Double_t>().swap(fTheta);
    std::vector<Double_t>().swap(fPhi);
    std::vector<Double_t>().swap(fTotKinEnergy);
    std::vector<Double_t>().swap(fCalKinEnergy);
    std::vector<Double_t>().swap(fMomentum);
    std::vector<Double_t>().swap(fBeta);
    std::vector<Int_t>().swap(fZ);
    std::vector<Int_t>().swap(fA);
  }
};
//______________________________________________________________________________


//______________________________________________________________________________
class CSHINEBuildEvent
{
public:
  CSHINEBuildEvent();
  ~CSHINEBuildEvent();

  // 参数说明: CSHINESSDEvent class, ssdindex, ech_L1S[16], ech_L2F[16], ech_L2B[16], ech_L3A[9]
  void LayerEvent (CSHINELayerEvent&,  Int_t,  Int_t*, Int_t*, Int_t*, Int_t*);

  void BuildLayerEvent(CSHINELayerEvent& layerevent,
      Int_t* ssd1echl1s, Int_t* ssd1echl2f, Int_t* ssd1echl2b, Int_t* ssd1echcsi,
      Int_t* ssd2echl1s, Int_t* ssd2echl2f, Int_t* ssd2echl2b, Int_t* ssd2echcsi,
      Int_t* ssd3echl1s, Int_t* ssd3echl2f, Int_t* ssd3echl2b, Int_t* ssd3echcsi,
      Int_t* ssd4echl1s, Int_t* ssd4echl2f, Int_t* ssd4echl2b, Int_t* ssd4echcsi);

  void BuildGlobalEvent(CSHINEGlobalEvent&, Int_t*, Int_t*, Int_t*, Int_t*);

  void BuildSSDEventTree(Int_t fisrtrun, Int_t lastlun);
  void BuildLayerEventTree (Int_t fisrtrun, Int_t lastlun);
  void BuildGlobalEventTree(Int_t fisrtrun, Int_t lastlun);

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
