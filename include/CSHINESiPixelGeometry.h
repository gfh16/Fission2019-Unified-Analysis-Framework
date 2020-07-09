#ifndef CSHINESIPIXELGEOMETRY_H
#define CSHINESIPIXELGEOMETRY_H

#include "Vector3.h"
#include <TMath.h>

static const Int_t SSDNum = 4;

class CSHINESiPixelGeometry
{
private:
  const Double_t RadToDeg = 180./TMath::Pi();
  const Double_t DegToRad = TMath::Pi()/180.;

  Double_t fWidthPerStrip;         // 条宽, mm
  Double_t fWidthPerGap;           // 相邻条缝宽, mm
  Double_t fDistOfAdjacentStrips;  // 条中心间距, mm

  Double_t fDistFromL1FToL2F;      // L1正面与L2F面之间的距离
  Double_t fDistFrom2ndMountingHoleToL2F;  // 硅条第二安装孔到L2F的距离
  Double_t fDistFrom2ndMountingHoleToTarget[SSDNum]; // 硅条第二安装孔到靶点的距离

  Double_t fL1Thickness[SSDNum]; // 单面硅条的厚度
  Double_t fL2Thickness[SSDNum]; // 双面硅条的厚度

  Double_t fDistOfL2FToTarget      [SSDNum]; // L2F 面到靶点的距离,单位：mm
  Double_t fPhiOfSSDFixedPosition  [SSDNum]; // 硅条安装的 phi 角,单位：degree
  Double_t fThetaOfSSDFixedPosition[SSDNum]; // 硅条安装的 theta 角,单位：degree

public:
  CSHINESiPixelGeometry();
  ~CSHINESiPixelGeometry();

  Double_t GetWidthPerStrip();
  Double_t GetDistFromL1FToL2F();
  Double_t Getd1(Int_t ssdnum);
  Double_t Getd2(Int_t ssdnum);
  Double_t GetDistOfL2FToTarget(Int_t ssdnum);
  Double_t GetPhiOfSSDFixedPosition(Int_t ssdnum);
  Double_t GetThetaOfSSDFixedPosition(Int_t ssdnum);
  Double_t GetEffThicknessOfPixel(Int_t ssdnum, Int_t stripb, Int_t stripf);

  Vector3 StripNumToPosInSSDFrame(Int_t ssdnum, Int_t stripb, Int_t stripf);

  Vector3 StripNumToPosInLabFrame(Int_t ssdnum, Int_t stripb, Int_t stripf);

  Vector3 PosInSSDFrameToLabFrame(Double_t pixelX, Double_t pixelY, Double_t pixelZ,
      Double_t dist, Double_t theta, Double_t phi);

  Vector3 PosInSSDFrameToLabFrame(Int_t ssdnum, Double_t pixelX, Double_t pixelY, Double_t pixelZ);

  Vector3 PosInLabFrameToSSDFrame(Double_t labX, Double_t labY, Double_t labZ,
      Double_t dist, Double_t theta, Double_t phi);

};

/*
// SSD1 && SSD4 硅条本体坐标系定义:          // SSD2 && SSD3 硅条本体坐标系定义:
//                          EB=0               EB=0
//  EF=0 +-------------------+ EF=0            +-------------------+
//       |        Y'         |                 |        Y'         |
//       |        /|\        |                 |        /|\        |
//       |         |         |                 |         |         |
//       |         |         |                 |         |         |
//       | <-------O'        |                 | <-------O'        |
//       | X'                |                 | X'                |
//       |  (从靶点看向硅条)  |                 |  (从靶点看向硅条)  |
//       |                   |                 |                   |
//       +-------------------+            EF=0 +-------------------+ EF=0
//                          EB=0             EB=0
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

#endif
