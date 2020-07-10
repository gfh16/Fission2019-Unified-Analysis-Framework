////////////////////////////////////////////////////////////////////////////////
//
//   计算硅条每个 pixel 中心点在实验室系下的 （R, Theta, Phi）
//   同时，计算每个 pixel 四个角落的 (R, Theta, Phi)
//   由此确定每个 pixel 的 Theta, Phi 范围
//
//
// SSD1 && SSD4 硅条本体坐标系定义:          // SSD2 && SSD3 硅条本体坐标系定义:
//                          EB=0               EB=0
//  EF=0 +-------------------+ EF=0            +-------------------+
//       |        Y'         |                 |        Y'          |
//       | 1 ___4 /|\        |                 |        /|\ 1  ___4 |
//       | 2|__|3  |         |                 |         |  2 |__|3 |
//       |         |         |                 |         |          |
//       | <-------O'        |                 | <-------O'         |
//       | X'                |                 | X'                 |
//       |  (从靶点看向硅条)   |                 |  (从靶点看向硅条)     |
//       |                   |                 |                    |
//       +-------------------+            EF=0 +--------------------+ EF=0
//                          EB=0             EB=0
//
////////////////////////////////////////////////////////////////////////////////

#include "../../include/CSHINESiPixelGeometry.h"
#include "../../include/Vector3.h"


const Int_t NumSSD = 4;
const Int_t NumCH  = 16;


//______________________________________________________________________________
void CheckSiPixellation()
{
  CSHINESiPixelGeometry pixel;

  Double_t DistToEdge = 2.0; // pixel 中心点到四条变距离相等, 都是 2mm (实验中两条并为一条)
  Double_t dist[NumSSD];
  Double_t theta[NumSSD];
  Double_t phi[NumSSD];

  Double_t X1, X2, X3, X4;
  Double_t Y1, Y2, Y3, Y4;
  Double_t Z1, Z2, Z3, Z4;

  for (Int_t ssdnum=0; ssdnum<NumSSD; ssdnum++)
  {
    dist[ssdnum]  = pixel.GetDistOfL2FToTarget(ssdnum);
    theta[ssdnum] = pixel.GetThetaOfSSDFixedPosition(ssdnum);
    phi[ssdnum]   = pixel.GetPhiOfSSDFixedPosition(ssdnum);

    for (Int_t nb=0; nb<NumCH; nb++)
    {
      for (Int_t nf=0; nf<NumCH; nf++)
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

}
