////////////////////////////////////////////////////////////////////////////////
//
//   计算硅条每个 pixel 中心点在实验室系下的 （R, Theta, Phi）
//   同时，计算每个 pixel 四个角落的 (R, Theta, Phi)
//   由此确定每个 pixel 的 Theta, Phi 范围
//
//   Phi 角的范围比较好评估 : 将每个正方形的 pixel 投影到 X-Y 平面, 则
//   平面内一个四边形的方位角, 其最值一定在端点取得.
//
//   现在考虑 Theta 角的误差范围: 每个 Theta 角都在一个特定的圆锥曲面上, 将某一点
//   投影到 X-Y 平面, 该点将落在一个圆心在原点的圆上. 记该圆的半径为 r, 该点的 Z
//   坐标为 z，该点所在极角为 theta， 有：
//         tan(theta) = r/z   or  theta = arctan(r/z)
//   tan() 是单调递增函数, 现在以 r/z 为半径画圆, 则 r/z 的大小可以表征 theta 角
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

#include "../../include/CSHINESiPixelGeometry.h"
#include "../../include/Vector3.h"


const Int_t NumSSD = 4;
const Int_t NumCH  = 16;
const Double_t DistToEdge = 2.0; // pixel 中心点到四条变距离相等, 都是 2mm (实验中两条并为一条)

//______________________________________________________________________________
Double_t average(Double_t a[], Int_t size);


//______________________________________________________________________________
void DrawSiPixellationErrorCircles()
{
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);

  Int_t ssdindex = 5;
  Int_t chindex  = 5;
  Double_t norm  = 3.5;
  Double_t ratio[NumSSD] = {7.0, 4.0, 3.0, 1.5};

  string pathPNGOutput("figures/CSHINE2019_ SSDArrayCoverage.png");

  CSHINESiPixelGeometry pixel;

  Double_t X1, X2, X3, X4;
  Double_t Y1, Y2, Y3, Y4;
  Double_t Z1, Z2, Z3, Z4;

  Double_t DistToZAxisOverZ_Cneter [NumSSD][NumCH][NumCH];
  Double_t DistToZAxisOverZ_Corner1[NumSSD][NumCH][NumCH];
  Double_t DistToZAxisOverZ_Corner2[NumSSD][NumCH][NumCH];
  Double_t DistToZAxisOverZ_Corner3[NumSSD][NumCH][NumCH];
  Double_t DistToZAxisOverZ_Corner4[NumSSD][NumCH][NumCH];

  for (Int_t ssdnum=0; ssdnum<NumSSD; ssdnum++)
  {
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

        DistToZAxisOverZ_Cneter [ssdnum][nb][nf] = 1./norm * PosInLabFrame.DistToZAxis() / PosInLabFrame.Z();
        DistToZAxisOverZ_Corner1[ssdnum][nb][nf] = 1./norm * PosInLabFrame_Corner1.DistToZAxis() / PosInLabFrame_Corner1.Z();
        DistToZAxisOverZ_Corner2[ssdnum][nb][nf] = 1./norm * PosInLabFrame_Corner2.DistToZAxis() / PosInLabFrame_Corner2.Z();
        DistToZAxisOverZ_Corner3[ssdnum][nb][nf] = 1./norm * PosInLabFrame_Corner3.DistToZAxis() / PosInLabFrame_Corner3.Z();
        DistToZAxisOverZ_Corner4[ssdnum][nb][nf] = 1./norm * PosInLabFrame_Corner4.DistToZAxis() / PosInLabFrame_Corner4.Z();

        cout<<setw(5)<<ssdnum<<setw(7)<<nb<<setw(7)<<nf
            <<setw(12)<<DistToZAxisOverZ_Cneter [ssdnum][nb][nf]
            <<setw(12)<<DistToZAxisOverZ_Corner1[ssdnum][nb][nf]
            <<setw(12)<<DistToZAxisOverZ_Corner2[ssdnum][nb][nf]
            <<setw(12)<<DistToZAxisOverZ_Corner3[ssdnum][nb][nf]
            <<setw(12)<<DistToZAxisOverZ_Corner4[ssdnum][nb][nf]
            <<endl;
      }
    }
  }

  TCanvas* cans = new TCanvas("cans","cans",1200,1200);
  //cans->Range(0,0,5,5);
  //cans->Divide(2,2);

  TEllipse* ciricle[NumSSD][5];
  Double_t r0[NumSSD];
  Double_t r1[NumSSD];
  Double_t r2[NumSSD];
  Double_t r3[NumSSD];
  Double_t r4[NumSSD];

  for (Int_t i=0; i<1; i++)
  {
    r0[i] = DistToZAxisOverZ_Cneter [i][ssdindex][chindex];
    r1[i] = DistToZAxisOverZ_Corner1[i][ssdindex][chindex];
    r2[i] = DistToZAxisOverZ_Corner2[i][ssdindex][chindex];
    r3[i] = DistToZAxisOverZ_Corner3[i][ssdindex][chindex];
    r4[i] = DistToZAxisOverZ_Corner4[i][ssdindex][chindex];

    ciricle[i][0] = new TEllipse(ratio[i]*r0[i], ratio[i]*r0[i], 5*r0[i], 5*r0[i]);
    ciricle[i][1] = new TEllipse(ratio[i]*r0[i], ratio[i]*r0[i], 5*r1[i], 5*r1[i]);
    ciricle[i][2] = new TEllipse(ratio[i]*r0[i], ratio[i]*r0[i], 5*r2[i], 5*r2[i]);
    ciricle[i][3] = new TEllipse(ratio[i]*r0[i], ratio[i]*r0[i], 5*r3[i], 5*r3[i]);
    ciricle[i][4] = new TEllipse(ratio[i]*r0[i], ratio[i]*r0[i], 5*r4[i], 5*r4[i]);


    cout<<setw(12)<<r0[i]<<setw(12)<<r1[i]<<setw(12)<<r2[i]<<setw(12)<<r3[i]
        <<setw(12)<<r4[i]<<endl;


    cans->cd();
    ciricle[i][0]->SetLineColor(kRed);
    ciricle[i][1]->SetLineColor(kMagenta+3);
    ciricle[i][2]->SetLineColor(kGreen+1);
    ciricle[i][3]->SetLineColor(kAzure+1);
    ciricle[i][4]->SetLineColor(kOrange);
    ciricle[i][0]->Draw();
    ciricle[i][1]->Draw("SAME");
    ciricle[i][2]->Draw("SAME");
    ciricle[i][3]->Draw("SAME");
    ciricle[i][4]->Draw("SAME");

    gPad->Modified();
    gPad->Update();


  }







}
