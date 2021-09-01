#ifndef CSHINESIPIXELGEOMETRY_H
#define CSHINESIPIXELGEOMETRY_H

#include "Vector3.h"
#include "shared.h"

#include "TMath.h"
#include "TCanvas.h"
#include "TH2.h"
#include "TStyle.h"

#include <iostream>
#include <fstream>
#include <string.h>


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
class CSHINESiPixelGeometry
{
public:
  CSHINESiPixelGeometry();
  ~CSHINESiPixelGeometry();

  Double_t    GetWidthPerStrip() const;
  Double_t    GetDistFromL1FToL2F() const;
  Double_t    Getd1(Int_t ssdnum) const;
  Double_t    Getd2(Int_t ssdnum) const;
  Double_t    GetDistOfL2FToTarget(Int_t ssdnum) const;
  Double_t    GetPhiOfSSDFixedPosition(Int_t ssdnum) const;
  Double_t    GetThetaOfSSDFixedPosition(Int_t ssdnum) const;
  Double_t    GetEffThicknessOfPixel(Int_t ssdnum, Double_t stripb, Double_t stripf) const; // strip 使用 double 型,
  Double_t    GetThetaDegOfPixel(Int_t ssdnum, Double_t stripb, Double_t stripf) const;     // 是为了方便用电荷重心法表示sharing事件的位置
  Double_t    GetPhiDegOfPixel(Int_t ssdnum, Double_t stripb, Double_t stripf) const;

  Vector3     StripNumToPosInSSDFrame(Int_t ssdnum, Double_t stripb, Double_t stripf) const;
  Vector3     StripNumToPosInLabFrame(Int_t ssdnum, Double_t stripb, Double_t stripf) const;
  Vector3     PosInSSDFrameToLabFrame(Double_t pixelX, Double_t pixelY, Double_t pixelZ, Double_t dist, Double_t theta, Double_t phi) const;
  Vector3     PosInSSDFrameToLabFrame(Int_t ssdnum, Double_t pixelX, Double_t pixelY, Double_t pixelZ) const;
  Vector3     PosInLabFrameToSSDFrame(Double_t labX, Double_t labY, Double_t labZ, Double_t dist, Double_t theta, Double_t phi) const;

  void        CalculateEffectiveThickness();
  void        SiPixellation();
  void        CheckSiPixellation();

  Bool_t      IsPixelOnCsIGap(Int_t stripf, Int_t stripb);


private:
  Double_t     fWidthPerStrip;         // 条宽, mm
  Double_t     fWidthPerGap;           // 相邻条缝宽, mm
  Double_t     fDistOfAdjacentStrips;  // 条中心间距, mm

  Double_t     fDistFromL1FToL2F;                // L1正面与L2F面之间的距离
  Double_t     fDistFrom2ndMountingHoleToL2F;    // 硅条第二安装孔到L2F的距离
  Double_t    *fDistFrom2ndMountingHoleToTarget; // [NUM_SSD] 硅条第二安装孔到靶点的距离

  Double_t    *fDistOfL2FToTarget;       // [NUM_SSD] L2F 面到靶点的距离,单位：mm
  Double_t    *fPhiOfSSDFixedPosition;   // [NUM_SSD] 硅条安装的 phi 角,单位：degree
  Double_t    *fThetaOfSSDFixedPosition; // [NUM_SSD] 硅条安装的 theta 角,单位：degree
};

/*
//    SSD1 && SSD4 硅条本体坐标系定义:         SSD2 && SSD3 硅条本体坐标系定义:
//  EF=0 +-------------------+           EF=15 +-------------------+
//       |        Y'         |                 |        Y'         |
//       |        /|\        |                 |        /|\        |
//       |         |         |                 |         |         |
//       |         |         |                 |         |         |
//       | <-------O'        |                 | <-------O'        |
//       | X'                |                 | X'                |
//       |  (从靶点看向硅条)   |                 |  (从靶点看向硅条)    |
//       |                   |                 |                   |
// EF=15 +-------------------+            EF=0 +-------------------+
//       EB=15              EB=0               EB=0             EB=15
//
// 实验室系定义:
//     束流方向
//       /|\
//        |
//        |
// <----- O (靶点)
// X
//
*/
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo

#endif
