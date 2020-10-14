#include "../include/CSHINESiPixelGeometry.h"
using namespace std;


////////////////////////////////////////////////////////////////////////////////
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
//       /||
//        |
//        |
// <----- O (靶点)
// X
//
////////////////////////////////////////////////////////////////////////////////


//******************************************************************************
CSHINESiPixelGeometry::CSHINESiPixelGeometry()
{
  fWidthPerStrip                    = 1.95;  // 条宽, mm
  fWidthPerGap                      = 0.05;  // 相邻条缝宽m, mm
  fDistOfAdjacentStrips             = 2.00;  // 条中心间距, mm

  fDistFromL1FToL2F                 = 7.50; // mm
  fDistFrom2ndMountingHoleToL2F     = 33.50; // mm

  fDistFrom2ndMountingHoleToTarget  = new Double_t [NUM_SSD];
  fPhiOfSSDFixedPosition            = new Double_t [NUM_SSD];
  fThetaOfSSDFixedPosition          = new Double_t [NUM_SSD];
  fDistOfL2FToTarget                = new Double_t [NUM_SSD];

  fDistFrom2ndMountingHoleToTarget[0] = 315.50;  // mm
  fDistFrom2ndMountingHoleToTarget[1] = 275.50;  // mm
  fDistFrom2ndMountingHoleToTarget[2] = 275.50;  // mm
  fDistFrom2ndMountingHoleToTarget[3] = 215.50;  // mm

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
  delete [] fPhiOfSSDFixedPosition;
  delete [] fThetaOfSSDFixedPosition;
  delete [] fDistOfL2FToTarget;

  //cout<< "Exit Class CSHINESiPixelGeometry, Bye!"<< endl;
}
//******************************************************************************


//******************************************************************************
Double_t CSHINESiPixelGeometry::GetWidthPerStrip()
{
  return fWidthPerStrip;
}

//______________________________________________________________________________
Double_t CSHINESiPixelGeometry::Getd1(Int_t ssdnum)
{
  return SIL1THICKNESS[ssdnum];
}

//______________________________________________________________________________
Double_t CSHINESiPixelGeometry::Getd2(Int_t ssdnum)
{
  return SIL2THICKNESS[ssdnum];
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
  Double_t d2     = Getd2(ssdnum);
  Double_t dist   = GetDistOfL2FToTarget(ssdnum);
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
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//
//   粒子以某个角度入射到双面硅中时, 粒子实际穿过硅的厚度比硅的标准厚度略大,
//   因此需要对每个 pixel 处的硅的厚度作修正, 称为硅的 “等效厚度”
//
////////////////////////////////////////////////////////////////////////////////
void CSHINESiPixelGeometry::CalculateEffectiveThickness()
{
  std::string pathDataOutput(Form("%sdata_Pixellation/CSHINE2019_EffThicknessOfL2.dat", PATHDATAFOLDER));

  ofstream EffThickness(pathDataOutput.c_str());
  EffThickness<<"* ssdnum"<<setw(10)<<"stripb"<<setw(10)<<"stripf"
              <<setw(20)<<"th_standard (mm)"<<setw(20)<<"th_effective (mm)"<<endl;

  CSHINESiPixelGeometry pixel;

  Double_t d2[NUM_SSD]; // thickness of L2


  for (Int_t ssdnum=0; ssdnum<NUM_SSD; ssdnum++)
  {
    d2[ssdnum] = pixel.Getd2(ssdnum);

    for (Int_t nb=0; nb<NUM_STRIP; nb++)
    {
      for (Int_t nf=0; nf<NUM_STRIP; nf++)
      {
        EffThickness<<setw(5) <<ssdnum    <<setw(10)<<nb<<setw(10)<<nf
                    <<setw(15)<<d2[ssdnum]<<setw(20)<<pixel.GetEffThicknessOfPixel(ssdnum, nb, nf)
                    <<endl;
      }
    }
  }
  cout<<"*******************************************"<<endl;
  cout<<"**   CalculateEffectiveThickness done!   **"<<endl;
  cout<<"*******************************************"<<endl;
}
//******************************************************************************


////////////////////////////////////////////////////////////////////////////////
//
//   计算硅条每个 pixel 中心点在实验室系下的 （R, Theta, Phi）
//   同时，计算每个 pixel 四个角落的 (R, Theta, Phi)
//   由此确定每个 pixel 的 Theta, Phi 范围
//
//   对于 pixel 角度的误差, 采取两种方式：
//   1.取四个端点角度的最小值、最大值, 作为 pixel 角度的取值范围
//   2.计算四个端点与中心点的角度误差, 取平均值, 且认为 pixel 的角度误差是对称的
//
//
// SSD1 && SSD4 硅条本体坐标系定义:          // SSD2 && SSD3 硅条本体坐标系定义:
//                          EB=0               EB=0
//  EF=0 +-------------------+ EF=0            +-------------------+
//       |        Y'         |                 |        Y'          |
//       | 1 ___2 /|\        |                 |        /|\  1 ___2 |
//       | 4|__|3  |         |                 |         |   4|__|3 |
//       |         |         |                 |         |          |
//       | <-------O'        |                 | <-------O'         |
//       | X'                |                 | X'                 |
//       |  (从靶点看向硅条)   |                 |  (从靶点看向硅条)     |
//       |                   |                 |                    |
//       +-------------------+            EF=0 +--------------------+ EF=0
//                          EB=0             EB=0
//
////////////////////////////////////////////////////////////////////////////////
Double_t average(Double_t a[], Int_t size)
{
  Double_t sum = 0.0;
  Double_t ave = 0.0;
  Double_t sumerr = 0.0;
  Double_t aveerr = 0.0;

  for (Int_t i=0; i<size; i++) {
    sum += a[i];
  }
  ave = sum / size;

  for (Int_t n=0; n<size; n++) {
    sumerr += TMath::Abs(a[n] - ave);
  }
  aveerr = sumerr / size;
  return aveerr;
}
//______________________________________________________________________________
void CSHINESiPixelGeometry::SiPixellation()
{
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);

  std::string pathPNGCoverage(Form("%sfigure_Pixellation/CSHINE2019_ SSDArrayCoverage.png", PATHFIGURESFOLDER));
  std::string pathPNGCheckDirection(Form("%sfigure_Pixellation/CSHINE2019_CheckStripDirection.png", PATHFIGURESFOLDER));
  std::string pathDataOutput(Form("%sdata_Pixellation/CSHINE2019_SiPixelAngles.dat", PATHDATAFOLDER));

  ofstream SiPixelAngles(pathDataOutput.c_str());
  SiPixelAngles<<"* ssdnum"<<setw(7)<<"stripb"<<setw(7)<<"stripf"
               <<setw(8)<<"theta"     <<setw(11)<<"phi"
               <<setw(15)<<"theta_min"<<setw(12)<<"theta_max"
               <<setw(12)<<"phi_min"  <<setw(12)<<"phi_max"
               <<setw(12)<<"theta_err"<<setw(12)<<"phi_err"
               <<endl;

  CSHINESiPixelGeometry pixel;

  Double_t DistToEdge = 2.0; // pixel 中心点到四条边距离相等, 都是 2mm (实验中两条并为一条)
//  Double_t dist [NUM_SSD];
//  Double_t theta[NUM_SSD];
//  Double_t phi  [NUM_SSD];

  Double_t X1, X2, X3, X4;
  Double_t Y1, Y2, Y3, Y4;
  Double_t Z1, Z2, Z3, Z4;

  TH2D* h2d_phi_theta =  new TH2D("h2d",      "h2d_phi_over_theta",     70,0.0,70.,360,0.0,360.);
  TH2D* h2d_nb0       =  new TH2D("h2d_nb0",  "check stripb direction", 70,0.0,70.,360,0.0,360.);
//  TH2D* h2d_nb5       =  new TH2D("h2d_nb5",  "check stripb direction", 70,0.0,70.,360,0.0,360.);
  TH2D* h2d_nb10      =  new TH2D("h2d_nb10", "check stripb direction", 70,0.0,70.,360,0.0,360.);
  TH2D* h2d_nb15      =  new TH2D("h2d_nb15", "check stripb direction", 70,0.0,70.,360,0.0,360.);
  TH2D* h2d_nf0       =  new TH2D("h2d_nf0",  "check stripf direction", 70,0.0,70.,360,0.0,360.);
//  TH2D* h2d_nf5       =  new TH2D("h2d_nf5",  "check stripf direction", 70,0.0,70.,360,0.0,360.);
  TH2D* h2d_nf10      =  new TH2D("h2d_nf10", "check stripf direction", 70,0.0,70.,360,0.0,360.);
  TH2D* h2d_nf15      =  new TH2D("h2d_nf15", "check stripf direction", 70,0.0,70.,360,0.0,360.);

  for (Int_t ssdnum=0; ssdnum<NUM_SSD; ssdnum++)
  {
//    dist[ssdnum]  = pixel.GetDistOfL2FToTarget(ssdnum);
//    theta[ssdnum] = pixel.GetThetaOfSSDFixedPosition(ssdnum);
//    phi[ssdnum]   = pixel.GetPhiOfSSDFixedPosition(ssdnum);

    for (Int_t nb=0; nb<NUM_STRIP; nb++)
    {
      for (Int_t nf=0; nf<NUM_STRIP; nf++)
      {
        Vector3 PosInSSDFrame = pixel.StripNumToPosInSSDFrame(ssdnum, nb, nf);
        Vector3 PosInLabFrame = pixel.StripNumToPosInLabFrame(ssdnum, nb, nf);

        X1 = PosInSSDFrame.X()+DistToEdge;
        X2 = PosInSSDFrame.X()-DistToEdge;
        X3 = PosInSSDFrame.X()-DistToEdge;
        X4 = PosInSSDFrame.X()+DistToEdge;

        Y1 = PosInSSDFrame.Y()+DistToEdge;
        Y2 = PosInSSDFrame.Y()+DistToEdge;
        Y3 = PosInSSDFrame.Y()-DistToEdge;
        Y4 = PosInSSDFrame.Y()-DistToEdge;

        Z1 = PosInSSDFrame.Z();
        Z2 = PosInSSDFrame.Z();
        Z3 = PosInSSDFrame.Z();
        Z4 = PosInSSDFrame.Z();

        Vector3 PosInLabFrame_Corner1 = pixel.PosInSSDFrameToLabFrame(ssdnum, X1, Y1, Z1);
        Vector3 PosInLabFrame_Corner2 = pixel.PosInSSDFrameToLabFrame(ssdnum, X2, Y2, Z2);
        Vector3 PosInLabFrame_Corner3 = pixel.PosInSSDFrameToLabFrame(ssdnum, X3, Y3, Z3);
        Vector3 PosInLabFrame_Corner4 = pixel.PosInSSDFrameToLabFrame(ssdnum, X4, Y4, Z4);

        Double_t theta_corner[4] = {PosInLabFrame_Corner1.ThetaDeg(), PosInLabFrame_Corner2.ThetaDeg(),
                                    PosInLabFrame_Corner3.ThetaDeg(), PosInLabFrame_Corner4.ThetaDeg()};
        Double_t phi_corner  [4] = {PosInLabFrame_Corner1.PhiDeg(), PosInLabFrame_Corner2.PhiDeg(),
                                    PosInLabFrame_Corner3.PhiDeg(), PosInLabFrame_Corner4.PhiDeg()};

        Double_t theta_ave_err = average(theta_corner, 4);
        Double_t phi_ave_err   = average(phi_corner  , 4);

        Double_t theta_min = *min_element(theta_corner, theta_corner + 4);
        Double_t theta_max = *max_element(theta_corner, theta_corner + 4);
        Double_t phi_min   = *min_element(phi_corner  , phi_corner   + 4);
        Double_t phi_max   = *max_element(phi_corner  , phi_corner   + 4);

        h2d_phi_theta->Fill(PosInLabFrame.ThetaDeg(), PosInLabFrame.PhiDeg());

        if (nb==0)  h2d_nb0 ->Fill(PosInLabFrame.ThetaDeg(), PosInLabFrame.PhiDeg());
      //if (nb==5)  h2d_nb5 ->Fill(PosInLabFrame.ThetaDeg(), PosInLabFrame.PhiDeg());
        if (nb==10) h2d_nb10->Fill(PosInLabFrame.ThetaDeg(), PosInLabFrame.PhiDeg());
        if (nb==15) h2d_nb15->Fill(PosInLabFrame.ThetaDeg(), PosInLabFrame.PhiDeg());

        if (nf==0)  h2d_nf0 ->Fill(PosInLabFrame.ThetaDeg(), PosInLabFrame.PhiDeg());
      //if (nf==5)  h2d_nf5 ->Fill(PosInLabFrame.ThetaDeg(), PosInLabFrame.PhiDeg());
        if (nf==10) h2d_nf10->Fill(PosInLabFrame.ThetaDeg(), PosInLabFrame.PhiDeg());
        if (nf==15) h2d_nf15->Fill(PosInLabFrame.ThetaDeg(), PosInLabFrame.PhiDeg());

        SiPixelAngles<<setw(5)<<ssdnum<<setw(7)<<nb<<setw(7)<<nf
                     <<setw(12)<<PosInLabFrame.ThetaDeg()<<setw(12)<<PosInLabFrame.PhiDeg()
                     <<setw(12)<<theta_min<<setw(12)<<theta_max
                     <<setw(12)<<phi_min<<setw(12)<<phi_max
                     <<setw(12)<<theta_ave_err<<setw(12)<<phi_ave_err
                     <<endl;
      }
    }
  }

  TCanvas* cans_check = new TCanvas("cans_check","cans_check", 1200, 800);
  cans_check->Divide(2,1);
  TCanvas* cans = new TCanvas("cans","cans_h2d", 1200, 1200);
  cans->Divide(2,2);

  // phi vs theta Distributions
  cans->cd(2);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetLeftMargin(0.12);
  h2d_phi_theta->SetTitle("#phi vs #theta  - Distributions");
  h2d_phi_theta->GetXaxis()->SetTitle("#theta (#circ)");
  h2d_phi_theta->GetXaxis()->SetTitleSize(0.05);
  h2d_phi_theta->GetXaxis()->CenterTitle(1);
  h2d_phi_theta->GetYaxis()->SetTitle("#phi (#circ)");
  h2d_phi_theta->GetYaxis()->SetTitleSize(0.05);
  h2d_phi_theta->GetYaxis()->CenterTitle(1);
  h2d_phi_theta->Draw("COL");

  // phi distrbution
  cans->cd(1);
  auto projectionY = h2d_phi_theta->ProjectionY();
  projectionY->SetTitle("#phi - Distributions");
  projectionY->Draw();

  // theta distribution
  cans->cd(4);
  auto projectionX = h2d_phi_theta->ProjectionX();
  projectionX->SetTitle("#theta - Distributions");
  projectionX->Draw();

  // 关闭 L2F/L2B 的某个条，检查strip 的方向
  cans_check->cd(1);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetLeftMargin(0.15);
  h2d_nb0->GetXaxis()->SetTitle("#theta (#circ)");
  h2d_nb0->GetXaxis()->SetTitleSize(0.05);
  h2d_nb0->GetXaxis()->CenterTitle(1);
  h2d_nb0->GetYaxis()->SetTitle("#phi (#circ)");
  h2d_nb0->GetYaxis()->SetTitleSize(0.05);
  h2d_nb0->GetYaxis()->CenterTitle(1);
  h2d_nb0 ->Draw("COL");
  h2d_nb10->Draw("COL SAME");
  h2d_nb15->Draw("COL SAME");

  cans_check->cd(2);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetLeftMargin(0.15);
  h2d_nf0->GetXaxis()->SetTitle("#theta (#circ)");
  h2d_nf0->GetXaxis()->SetTitleSize(0.05);
  h2d_nf0->GetXaxis()->CenterTitle(1);
  h2d_nf0->GetYaxis()->SetTitle("#phi (#circ)");
  h2d_nf0->GetYaxis()->SetTitleSize(0.05);
  h2d_nf0->GetYaxis()->CenterTitle(1);
  h2d_nf0 ->Draw("COL");
  h2d_nf10->Draw("COL SAME");
  h2d_nf15->Draw("COL SAME");

  cans->Print(pathPNGCoverage.c_str());
  cans_check->Print(pathPNGCheckDirection.c_str());
  SiPixelAngles.close();

  cout<<"*******************************"<<endl;
  cout<<"**    SiPixellation done!    **"<<endl;
  cout<<"*******************************"<<endl;
}
//******************************************************************************


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
//   计算硅条每个 pixel 中心点在实验室系下的 （R, Theta, Phi）
//   同时，计算每个 pixel 四个角落的 (R, Theta, Phi)
//   由此确定每个 pixel 的 Theta, Phi 范围
//   **** 此函数用于检查 pixel 是否正确 ****
////////////////////////////////////////////////////////////////////////////////
void CSHINESiPixelGeometry::CheckSiPixellation()
{
  CSHINESiPixelGeometry pixel;

  Double_t DistToEdge = 2.0; // pixel 中心点到四条变距离相等, 都是 2mm (实验中两条并为一条)
//  Double_t dist [NUM_SSD];
//  Double_t theta[NUM_SSD];
//  Double_t phi  [NUM_SSD];

  Double_t X1, X2, X3, X4;
  Double_t Y1, Y2, Y3, Y4;
  Double_t Z1, Z2, Z3, Z4;

  for (Int_t ssdnum=0; ssdnum<NUM_SSD; ssdnum++)
  {
//    dist[ssdnum]  = pixel.GetDistOfL2FToTarget(ssdnum);
//    theta[ssdnum] = pixel.GetThetaOfSSDFixedPosition(ssdnum);
//    phi[ssdnum]   = pixel.GetPhiOfSSDFixedPosition(ssdnum);

    for (Int_t nb=0; nb<NUM_STRIP; nb++)
    {
      for (Int_t nf=0; nf<NUM_STRIP; nf++)
      {
        Vector3 PosInSSDFrame = pixel.StripNumToPosInSSDFrame(ssdnum, nb, nf);
        Vector3 PosInLabFrame = pixel.StripNumToPosInLabFrame(ssdnum, nb, nf);

        X1 = PosInSSDFrame.X()+DistToEdge;
        X2 = PosInSSDFrame.X()-DistToEdge;
        X3 = PosInSSDFrame.X()-DistToEdge;
        X4 = PosInSSDFrame.X()+DistToEdge;

        Y1 = PosInSSDFrame.Y()+DistToEdge;
        Y2 = PosInSSDFrame.Y()+DistToEdge;
        Y3 = PosInSSDFrame.Y()-DistToEdge;
        Y4 = PosInSSDFrame.Y()-DistToEdge;

        Z1 = PosInSSDFrame.Z();
        Z2 = PosInSSDFrame.Z();
        Z3 = PosInSSDFrame.Z();
        Z4 = PosInSSDFrame.Z();

        Vector3 PosInLabFrame_Corner1 = pixel.PosInSSDFrameToLabFrame(ssdnum, X1, Y1, Z1);
        Vector3 PosInLabFrame_Corner2 = pixel.PosInSSDFrameToLabFrame(ssdnum, X2, Y2, Z2);
        Vector3 PosInLabFrame_Corner3 = pixel.PosInSSDFrameToLabFrame(ssdnum, X3, Y3, Z3);
        Vector3 PosInLabFrame_Corner4 = pixel.PosInSSDFrameToLabFrame(ssdnum, X4, Y4, Z4);

      /*
        Vector3 PosInLabFrame_Corner1 = pixel.PosInSSDFrameToLabFrame(X1, Y1, Z1, dist[ssdnum], theta[ssdnum], phi[ssdnum]);
        Vector3 PosInLabFrame_Corner2 = pixel.PosInSSDFrameToLabFrame(X2, Y2, Z2, dist[ssdnum], theta[ssdnum], phi[ssdnum]);
        Vector3 PosInLabFrame_Corner3 = pixel.PosInSSDFrameToLabFrame(X3, Y3, Z3, dist[ssdnum], theta[ssdnum], phi[ssdnum]);
        Vector3 PosInLabFrame_Corner4 = pixel.PosInSSDFrameToLabFrame(X4, Y4, Z4, dist[ssdnum], theta[ssdnum], phi[ssdnum]);
      */
      /*
        Vector3 PosInLabFrame_Corner1 = pixel.PosInSSDFrameToLabFrame(ssdnum, PosInSSDFrame.X()+DistToEdge,
                                          PosInSSDFrame.Y()+DistToEdge, PosInSSDFrame.Z());
        Vector3 PosInLabFrame_Corner2 = pixel.PosInSSDFrameToLabFrame(ssdnum, PosInSSDFrame.X()+DistToEdge,
                                          PosInSSDFrame.Y()-DistToEdge, PosInSSDFrame.Z());
        Vector3 PosInLabFrame_Corner3 = pixel.PosInSSDFrameToLabFrame(ssdnum, PosInSSDFrame.X()-DistToEdge,
                                          PosInSSDFrame.Y()-DistToEdge, PosInSSDFrame.Z());
        Vector3 PosInLabFrame_Corner4 = pixel.PosInSSDFrameToLabFrame(ssdnum, PosInSSDFrame.X()-DistToEdge,
                                          PosInSSDFrame.Y()+DistToEdge, PosInSSDFrame.Z());
     */


      /*
        cout<<setw(5) <<ssdnum<<setw(7)<<nb<<setw(7)<<nf
            <<setw(10)<<PosInSSDFrame.X()
            <<setw(10)<<PosInSSDFrame.X()+DistToEdge
            <<setw(10)<<PosInSSDFrame.X()-DistToEdge
            <<setw(10)<<PosInSSDFrame.Y()
            <<setw(10)<<PosInSSDFrame.Y()+DistToEdge
            <<setw(10)<<PosInSSDFrame.Y()-DistToEdge
            <<setw(10)<<PosInSSDFrame.Z()
            <<endl;
       */

       cout<<setw(5)<<ssdnum<<setw(7)<<nb<<setw(7)<<nf
           <<setw(15)<<PosInLabFrame.ThetaDeg()<<setw(10)<<PosInLabFrame.PhiDeg()
           <<setw(10)<<PosInLabFrame.DistToZAxis()
           <<setw(10)<<PosInLabFrame_Corner1.DistToZAxis()<<setw(10)<<PosInLabFrame_Corner2.DistToZAxis()
           <<setw(10)<<PosInLabFrame_Corner3.DistToZAxis()<<setw(10)<<PosInLabFrame_Corner4.DistToZAxis()
           <<setw(10)<<PosInLabFrame.R()
           <<setw(10)<<PosInLabFrame_Corner1.R()<<setw(10)<<PosInLabFrame_Corner2.R()
           <<setw(10)<<PosInLabFrame_Corner3.R()<<setw(10)<<PosInLabFrame_Corner4.R()
           <<endl;

       /*
       cout<<setw(5)<<ssdnum<<setw(7)<<nb<<setw(7)<<nf
           <<setw(15)<<PosInLabFrame.ThetaDeg()<<setw(10)<<PosInLabFrame.PhiDeg()
           <<setw(10)<<PosInLabFrame_Corner1.ThetaDeg()<<setw(10)<<PosInLabFrame_Corner2.ThetaDeg()
           <<setw(10)<<PosInLabFrame_Corner3.ThetaDeg()<<setw(10)<<PosInLabFrame_Corner4.ThetaDeg()
           <<setw(10)<<PosInLabFrame_Corner1.PhiDeg()<<setw(10)<<PosInLabFrame_Corner2.PhiDeg()
           <<setw(10)<<PosInLabFrame_Corner3.PhiDeg()<<setw(10)<<PosInLabFrame_Corner4.PhiDeg()
           <<endl;
      */
      /*
      cout<<setw(5)<<ssdnum<<setw(5)<<nb<<setw(5)<<nf
          <<setw(12)<<PosInLabFrame.ThetaDeg()<<setw(10)<<PosInLabFrame.PhiDeg()
          <<setw(10)<<PosInLabFrame.X()
          <<setw(10)<<PosInLabFrame_Corner1.X()<<setw(10)<<PosInLabFrame_Corner2.X()
          <<setw(10)<<PosInLabFrame_Corner3.X()<<setw(10)<<PosInLabFrame_Corner4.X()
          <<setw(10)<<PosInLabFrame.Y()
          <<setw(10)<<PosInLabFrame_Corner1.Y()<<setw(10)<<PosInLabFrame_Corner2.Y()
          <<setw(10)<<PosInLabFrame_Corner3.Y()<<setw(10)<<PosInLabFrame_Corner4.Y()
          <<setw(10)<<PosInLabFrame.Z()
          <<setw(10)<<PosInLabFrame_Corner1.Z()<<setw(10)<<PosInLabFrame_Corner2.Z()
          <<setw(10)<<PosInLabFrame_Corner3.Z()<<setw(10)<<PosInLabFrame_Corner4.Z()
          <<endl;
      */
      }
    }
  }
  cout<<"************************************"<<endl;
  cout<<"**    CheckSiPixellation done!    **"<<endl;
  cout<<"************************************"<<endl;
}
//******************************************************************************
