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

#include "../../include/CSHINESiPixelGeometry.h"
#include "../../include/Vector3.h"


const Int_t NumSSD = 4;
const Int_t NumCH  = 16;

//______________________________________________________________________________
Double_t average(Double_t a[], Int_t size);


//______________________________________________________________________________
void SiPixellation()
{
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);

  string pathPNGOutput("figures/CSHINE2019_ SSDArrayCoverage.png");
  string pathDataOutput("data/CSHINE2019_SiPixelAngles.dat");

  ofstream SiPixelAngles(pathDataOutput.c_str());
  SiPixelAngles<<"* ssdnum"<<setw(7)<<"stripb"<<setw(7)<<"stripf"
               <<setw(8)<<"theta"     <<setw(11)<<"phi"
               <<setw(15)<<"theta_min"<<setw(12)<<"theta_max"
               <<setw(12)<<"phi_min"  <<setw(12)<<"phi_max"
               <<setw(12)<<"theta_err"<<setw(12)<<"phi_err"
               <<endl;

  CSHINESiPixelGeometry pixel;

  Double_t DistToEdge = 2.0; // pixel 中心点到四条变距离相等, 都是 2mm (实验中两条并为一条)
  Double_t dist[NumSSD];
  Double_t theta[NumSSD];
  Double_t phi[NumSSD];

  Double_t X1, X2, X3, X4;
  Double_t Y1, Y2, Y3, Y4;
  Double_t Z1, Z2, Z3, Z4;

  TH2D* h2d_phi_theta = new TH2D("h2d","h2d_phi_over_theta",70,0.0,70.,360,0.0,360.);

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

        SiPixelAngles<<setw(5)<<ssdnum<<setw(7)<<nb<<setw(7)<<nf
                     <<setw(12)<<PosInLabFrame.ThetaDeg()<<setw(12)<<PosInLabFrame.PhiDeg()
                     <<setw(12)<<theta_min<<setw(12)<<theta_max
                     <<setw(12)<<phi_min<<setw(12)<<phi_max
                     <<setw(12)<<theta_ave_err<<setw(12)<<phi_ave_err
                     <<endl;
      }
    }
  }

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

  cans->Print(pathPNGOutput.c_str());
  SiPixelAngles.close();


}


//______________________________________________________________________________
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
