////////////////////////////////////////////////////////////////////////////////
//  CSHINEPPACCalibration 这个类用于将 PPAC 的信号从 PPAC 本体坐标系转换到实验室系
//  PPAC 信号在本体坐标系下的位置由 X1,X2,Y1,Y2 信号同时决定, 这一步刻度的结果由刁昕玥
//  提供. CSHINEPPACCalibration 根据刁昕玥提供的刻度参数进行坐标系转换
//
//  gfh, 2021年12月
////////////////////////////////////////////////////////////////////////////////

#ifndef CSHINEPPACCALIBRATION_H
#define CSHINEPPACCALIBRATION_H

#include "TVector3.h"
#include "shared.h"


//______________________________________________________________________________
class CSHINEPPACCalibration
{
public:
  CSHINEPPACCalibration();
  ~CSHINEPPACCalibration();


public:
  TVector3         XYSignalToCoordinateInPPACFrame(Int_t numppac, Int_t X1, Int_t X2, Int_t Y1, Int_t Y2); // 由XY信号转换到 PPAC本体系的坐标, 单位: mm
  TVector3         CoordinatePPACFrameToLabFrame(Int_t numppac, TVector3 vecppacframe);  // 坐标变换：由PPAC本体系转换到实验室系, 单位：mm


public:
  void             GetAndCorrectOriginalSignals(Int_t numppac, Int_t& T, Int_t& X1, Int_t& X2, Int_t& Y1, Int_t& Y2);


private:
  Bool_t           PPAC_X1_GOOD;
  Bool_t           PPAC_X2_GOOD;
  Bool_t           PPAC_Y1_GOOD;
  Bool_t           PPAC_Y2_GOOD;


// 以下这种方法计算 TOF 仅适用于 PPAC 两体符合事件，实际数据生成中没用上
public:
  Int_t            ShiftRF1IntoOnePeriod(Int_t RF1);  // 将 RF1 的信号平移到同一个周期
  Double_t         CalcTOFWithRF1Calibrated(Int_t PPAC_T, Int_t RF1); // 单位: ns  [ 利用高频信号 RF1, 计算裂变碎片的飞行时间 ]


private:
	const Double_t   fDistFromTtoTarget[NUM_PPAC]        = { 427.5, 427.5, 427.5 }; // PPAC 阴极膜到靶点的距离, 单位：mm
  const Double_t   fPhiOfPPACFixedPosition[NUM_PPAC]   = { 50.,  -55.,  -100. };  // PPAC 安装的 phi 角,单位：degree
  const Double_t   fThetaOfPPACFixedPosition[NUM_PPAC] = { 0.,    0.,    0. };    // PPAC 安装的 theta 角,单位：degree; PPAC中心在束流高度, theta=0

private:
  TVector3         fVectorInPPACFrame[NUM_PPAC];
  TVector3         fVectorInLabFrame[NUM_PPAC];


private:
  const Double_t   fADCChannelTons = 0.1;         // ns  P.S.: fADCChannelTons = 8.9/(5*16+9);  // TDC 量程设置 0x00000059
  const Double_t   fCalibratedTimeConst = -117.6; // ns  P.S.: 刻度得到的时间常数
  const Int_t      fRF1Period = 755; // ADC channel, 高频 RF1 的周期


private:
  Double_t   fPPAC_X1_Plus_X2_Minus_2T[NUM_PPAC] = {1355.64, 1393.50, 1289.52}; // X 方向的时间常数： X1+X2-2T = const
  Double_t   fPPAC_Y1_Plus_Y2_Minus_2T[NUM_PPAC] = {1033.71, 1108.57, 1201.30}; // Y 方向的时间常数: Y1+Y2-2T = const


//______________________________________________________________________________
// 探测器固定坐标系:
// 对于探测器固定的角度，以靶盘平面为 X''-Y'' 平面
//
//              Z''         X'' (beam)
//              /|\         /
//               | phi>0  / phi<0
//               |      /
//               |    /
//               |  /
//  Y'' <— — — —- /
//
//______________________________________________________________________________
// PPAC 本体坐标系
// P.S.: X1,X2 对应 Y 方向的坐标; Y1,Y2 对应 X 方向的坐标
//
//            Y'
//      Y1   /|\   Y2
//      ______|______
//   X2 |     |     |
//      |     |     |
// X'<--|-----------|---
//      |     |     |
//   X1 |_____|_____|
//            |
//______________________________________________________________________________

};

#endif
