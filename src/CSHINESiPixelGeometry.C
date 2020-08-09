#include "../include/CSHINESiPixelGeometry.h"
#include "../include/Vector3.h"

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
//       |  (从靶点看向硅条)   |                 |  (从靶点看向硅条)    |
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

//const Int_t SSDNum = 4;
//______________________________________________________________________________
CSHINESiPixelGeometry::CSHINESiPixelGeometry()
{
  fWidthPerStrip                    = 1.95;  // 条宽, mm
  fWidthPerGap                      = 0.05;  // 相邻条缝宽m, mm
  fDistOfAdjacentStrips             = 2.00;  // 条中心间距, mm

  fDistFromL1FToL2F                 = 7.50; // mm
  fDistFrom2ndMountingHoleToL2F     = 33.50; // mm

  fDistFrom2ndMountingHoleToTarget  = new Double_t [NUM_SSD];
  fL1Thickness                      = new Double_t [NUM_SSD];
  fL2Thickness                      = new Double_t [NUM_SSD];
  fPhiOfSSDFixedPosition            = new Double_t [NUM_SSD];
  fThetaOfSSDFixedPosition          = new Double_t [NUM_SSD];
  fDistOfL2FToTarget                = new Double_t [NUM_SSD];

  fDistFrom2ndMountingHoleToTarget[0] = 315.50;  // mm
  fDistFrom2ndMountingHoleToTarget[1] = 275.50;  // mm
  fDistFrom2ndMountingHoleToTarget[2] = 275.50;  // mm
  fDistFrom2ndMountingHoleToTarget[3] = 215.50;  // mm

  fL1Thickness[0] = 0.304;  // mm
  fL1Thickness[1] = 0.305;  // mm
  fL1Thickness[2] = 0.110;  // mm
  fL1Thickness[3] = 0.070;  // mm
  fL2Thickness[0] = 1.010;  // mm
  fL2Thickness[1] = 1.008;  // mm
  fL2Thickness[2] = 0.526;  // mm
  fL2Thickness[3] = 0.306;  // mm

  fPhiOfSSDFixedPosition[0] = +10.0; // degree
  fPhiOfSSDFixedPosition[1] = -20.0; // degree
  fPhiOfSSDFixedPosition[2] = -20.0; // degree
  fPhiOfSSDFixedPosition[3] = +10.0; // degree

  fThetaOfSSDFixedPosition[0] = +15.0; // degree
  fThetaOfSSDFixedPosition[1] = +15.0; // degree
  fThetaOfSSDFixedPosition[2] = -25.0; // degree
  fThetaOfSSDFixedPosition[3] = -50.0; // degree

  fDistOfL2FToTarget[0] = fDistFrom2ndMountingHoleToTarget[0]-fDistFrom2ndMountingHoleToL2F;
  fDistOfL2FToTarget[1] = fDistFrom2ndMountingHoleToTarget[1]-fDistFrom2ndMountingHoleToL2F;
  fDistOfL2FToTarget[2] = fDistFrom2ndMountingHoleToTarget[2]-fDistFrom2ndMountingHoleToL2F;
  fDistOfL2FToTarget[3] = fDistFrom2ndMountingHoleToTarget[3]-fDistFrom2ndMountingHoleToL2F;
}


//______________________________________________________________________________
CSHINESiPixelGeometry::~CSHINESiPixelGeometry()
{
  delete [] fDistFrom2ndMountingHoleToTarget;
  delete [] fL1Thickness;
  delete [] fL2Thickness;
  delete [] fPhiOfSSDFixedPosition;
  delete [] fThetaOfSSDFixedPosition;
  delete [] fDistOfL2FToTarget;

  cout<< "Exit Class CSHINESiPixelGeometry, Bye!"<< endl;
}


//______________________________________________________________________________
Double_t CSHINESiPixelGeometry::GetWidthPerStrip()
{
  return fWidthPerStrip;
}


//______________________________________________________________________________
Double_t CSHINESiPixelGeometry::Getd1(Int_t ssdnum)
{
  return fL1Thickness[ssdnum];
}


//______________________________________________________________________________
Double_t CSHINESiPixelGeometry::Getd2(Int_t ssdnum)
{
  return fL2Thickness[ssdnum];
}

//______________________________________________________________________________
Double_t CSHINESiPixelGeometry::GetDistOfL2FToTarget(Int_t ssdnum)
{
  return fDistOfL2FToTarget[ssdnum];
}

//______________________________________________________________________________
Double_t CSHINESiPixelGeometry::GetPhiOfSSDFixedPosition(Int_t ssdnum)
{
  return fPhiOfSSDFixedPosition[ssdnum];
}

//______________________________________________________________________________
Double_t CSHINESiPixelGeometry::GetThetaOfSSDFixedPosition(Int_t ssdnum)
{
  return fThetaOfSSDFixedPosition[ssdnum];
}

//______________________________________________________________________________
Double_t CSHINESiPixelGeometry::GetEffThicknessOfPixel(Int_t ssdnum, Int_t stripb, Int_t stripf)
{
  Double_t d2 = Getd2(ssdnum);
  Double_t dist = GetDistOfL2FToTarget(ssdnum);
  Double_t pixelX = StripNumToPosInSSDFrame(ssdnum, stripb, stripf).X();
  Double_t pixelY = StripNumToPosInSSDFrame(ssdnum, stripb, stripf).Y();
  return d2 * TMath::Sqrt(pow(dist, 2) + pow(pixelX, 2) + pow(pixelY, 2)) / dist;
}

//______________________________________________________________________________
Double_t CSHINESiPixelGeometry::GetThetaDegOfPixel(Int_t ssdnum, Int_t stripb, Int_t stripf)
{
  Vector3 vec3 = StripNumToPosInLabFrame(ssdnum, stripb, stripf);
  return vec3.ThetaDeg();
}

//______________________________________________________________________________
Double_t CSHINESiPixelGeometry::GetPhiDegOfPixel(Int_t ssdnum, Int_t stripb, Int_t stripf)
{
  Vector3 vec3 = StripNumToPosInLabFrame(ssdnum, stripb, stripf);
  return vec3.PhiDeg();
}


//___________________________________________________________________________________________
Vector3 CSHINESiPixelGeometry::StripNumToPosInSSDFrame(Int_t ssdnum, Int_t stripb, Int_t stripf)
{
  Double_t pixelX, pixelY, pixelZ;
  switch (ssdnum)
  {
    case 0:
      pixelX = (stripb - 7.5) * 2 * fDistOfAdjacentStrips;
      pixelY = (7.5 - stripf) * 2 * fDistOfAdjacentStrips;
      pixelZ = 0.0;
      break;
    case 1:
      pixelX = (7.5 - stripb) * 2 * fDistOfAdjacentStrips;
      pixelY = (stripf - 7.5) * 2 * fDistOfAdjacentStrips;
      pixelZ = 0.0;
      break;
    case 2:
      pixelX = (7.5 - stripb) * 2 * fDistOfAdjacentStrips;
      pixelY = (stripf - 7.5) * 2 * fDistOfAdjacentStrips;
      pixelZ = 0.0;
      break;
    case 3:
      pixelX = (stripb - 7.5) * 2 * fDistOfAdjacentStrips;
      pixelY = (7.5 - stripf) * 2 * fDistOfAdjacentStrips;
      pixelZ = 0.0;
      break;
    default:
      cout<<"Invalid ssdnum for CSHINESiPixelGeometry::StripNumToPosInSSDFrame"<<endl;
  }
  Vector3 pixelvec3(pixelX, pixelY, pixelZ);
  return pixelvec3;
}


//______________________________________________________________________________
Vector3 CSHINESiPixelGeometry::StripNumToPosInLabFrame(Int_t ssdnum, Int_t stripb, Int_t stripf)
{
  Double_t pixelX = StripNumToPosInSSDFrame(ssdnum, stripb, stripf).X();
  Double_t pixelY = StripNumToPosInSSDFrame(ssdnum, stripb, stripf).Y();
  Double_t pixelZ = StripNumToPosInSSDFrame(ssdnum, stripb, stripf).Z();
  Double_t phi    = fPhiOfSSDFixedPosition[ssdnum];
  Double_t theta  = fThetaOfSSDFixedPosition[ssdnum];
  Double_t dist   = fDistOfL2FToTarget[ssdnum];

  Double_t labX, labY, labZ;
  labX = pixelX * TMath::Cos(phi*DegToRad) +
         pixelY * TMath::Sin(phi*DegToRad) * TMath::Sin(theta*DegToRad) +
         pixelZ * TMath::Sin(phi*DegToRad) * TMath::Cos(theta*DegToRad) +
         dist   * TMath::Sin(phi*DegToRad) * TMath::Cos(theta*DegToRad) ;

  labY = pixelY * TMath::Cos(theta*DegToRad) -
         pixelZ * TMath::Sin(theta*DegToRad) -
         dist   * TMath::Sin(theta*DegToRad) ;

  labZ = -pixelX * TMath::Sin(phi*DegToRad) +
          pixelY * TMath::Cos(phi*DegToRad) * TMath::Sin(theta*DegToRad) +
          pixelZ * TMath::Cos(phi*DegToRad) * TMath::Cos(theta*DegToRad) +
          dist   * TMath::Cos(phi*DegToRad) * TMath::Cos(theta*DegToRad) ;

  Vector3 pixelveclab(labX, labY, labZ);
  return pixelveclab;
}


//______________________________________________________________________________
Vector3 CSHINESiPixelGeometry::PosInSSDFrameToLabFrame(Int_t ssdnum, Double_t pixelX,
   Double_t pixelY, Double_t pixelZ)
{
  Double_t phi    = fPhiOfSSDFixedPosition[ssdnum];
  Double_t theta  = fThetaOfSSDFixedPosition[ssdnum];
  Double_t dist   = fDistOfL2FToTarget[ssdnum];

  Double_t labX, labY, labZ;
  labX = pixelX * TMath::Cos(phi*DegToRad) +
         pixelY * TMath::Sin(phi*DegToRad) * TMath::Sin(theta*DegToRad) +
         pixelZ * TMath::Sin(phi*DegToRad) * TMath::Cos(theta*DegToRad) +
         dist   * TMath::Sin(phi*DegToRad) * TMath::Cos(theta*DegToRad) ;

  labY = pixelY * TMath::Cos(theta*DegToRad) -
         pixelZ * TMath::Sin(theta*DegToRad) -
         dist   * TMath::Sin(theta*DegToRad);

  labZ = -pixelX * TMath::Sin(phi*DegToRad) +
          pixelY * TMath::Cos(phi*DegToRad) * TMath::Sin(theta*DegToRad) +
          pixelZ * TMath::Cos(phi*DegToRad) * TMath::Cos(theta*DegToRad) +
          dist   * TMath::Cos(phi*DegToRad) * TMath::Cos(theta*DegToRad) ;

  Vector3 pixelveclab(labX, labY, labZ);
  return pixelveclab;
}


//______________________________________________________________________________
Vector3 CSHINESiPixelGeometry::PosInSSDFrameToLabFrame(Double_t pixelX, Double_t pixelY, Double_t pixelZ,
  Double_t dist, Double_t theta, Double_t phi)
{
  Double_t labX, labY, labZ;
  labX = pixelX * TMath::Cos(phi*DegToRad) +
         pixelY * TMath::Sin(phi*DegToRad) * TMath::Sin(theta*DegToRad) +
         pixelZ * TMath::Sin(phi*DegToRad) * TMath::Cos(theta*DegToRad) +
         dist   * TMath::Sin(phi*DegToRad) * TMath::Cos(theta*DegToRad) ;

  labY = pixelY * TMath::Cos(theta*DegToRad) -
         pixelZ * TMath::Sin(theta*DegToRad) -
         dist   * TMath::Sin(theta*DegToRad);

  labZ = -pixelX * TMath::Sin(phi*DegToRad) +
          pixelY * TMath::Cos(phi*DegToRad) * TMath::Sin(theta*DegToRad) +
          pixelZ * TMath::Cos(phi*DegToRad) * TMath::Cos(theta*DegToRad) +
          dist   * TMath::Cos(phi*DegToRad) * TMath::Cos(theta*DegToRad) ;

  Vector3 pixelveclab(labX, labY, labZ);
  return pixelveclab;
}


//______________________________________________________________________________
Vector3 CSHINESiPixelGeometry::PosInLabFrameToSSDFrame(Double_t labX, Double_t labY, Double_t labZ,
  Double_t dist, Double_t theta, Double_t phi)
{
  Double_t pixelX, pixelY, pixelZ;
  pixelX = (labX - dist*TMath::Cos(theta*DegToRad) *TMath::Sin(phi*DegToRad)) * TMath::Cos(phi*DegToRad) -
           (labZ - dist*TMath::Cos(theta*DegToRad) *TMath::Cos(phi*DegToRad)) * TMath::Sin(phi*DegToRad) ;

  pixelY = (labX - dist*TMath::Cos(theta*DegToRad) *TMath::Sin(phi*DegToRad)) * TMath::Sin(theta*DegToRad) * TMath::Sin(phi*DegToRad) +
           (labY + dist*TMath::Sin(theta*DegToRad))*TMath::Cos(theta*DegToRad)  +
           (labZ - dist*TMath::Cos(theta*DegToRad) *TMath::Cos(phi*DegToRad)) * TMath::Sin(theta*DegToRad) * TMath::Cos(phi*DegToRad) ;

  pixelZ =-(labX - dist*TMath::Cos(theta*DegToRad) *TMath::Sin(phi*DegToRad)) * TMath::Cos(theta*DegToRad) * TMath::Sin(phi*DegToRad) -
           (labY + dist*TMath::Sin(theta*DegToRad))*TMath::Sin(theta*DegToRad)  +
           (labZ - dist*TMath::Cos(theta*DegToRad) *TMath::Cos(phi*DegToRad)) * TMath::Cos(theta*DegToRad) * TMath::Cos(phi*DegToRad) ;

  return Vector3(pixelX, pixelY, pixelZ);
}
