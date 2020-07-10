////////////////////////////////////////////////////////////////////////////////
//
//   粒子以某个角度入射到双面硅中时, 粒子实际穿过硅的厚度比硅的标准厚度略大,
//   因此需要对每个 pixel 处的硅的厚度作修正, 称为硅的 “等效厚度”
//
////////////////////////////////////////////////////////////////////////////////

#include "../../include/CSHINESiPixelGeometry.h"
#include "../../include/Vector3.h"


const Int_t NumSSD = 4;
const Int_t NumCH  = 16;


//______________________________________________________________________________
void CalculateEffectiveThickness()
{
  string pathDataOutput("data/CSHINE2019_EffThicknessOfL2.dat");

  ofstream EffThickness(pathDataOutput.c_str());
  EffThickness<<"* ssdnum"<<setw(10)<<"stripb"<<setw(10)<<"stripf"
              <<setw(20)<<"th_standard (mm)"<<setw(20)<<"th_effective (mm)"<<endl;

  CSHINESiPixelGeometry pixel;

  Double_t d2[NumSSD]; // thickness of L2


  for (Int_t ssdnum=0; ssdnum<NumSSD; ssdnum++)
  {
    d2[ssdnum] = pixel.Getd2(ssdnum);

    for (Int_t nb=0; nb<NumCH; nb++)
    {
      for (Int_t nf=0; nf<NumCH; nf++)
      {
        EffThickness<<setw(5)<<ssdnum<<setw(10)<<nb<<setw(10)<<nf
                    <<setw(15)<<d2[ssdnum]<<setw(20)<<pixel.GetEffThicknessOfPixel(ssdnum, nb, nf)
                    <<endl;
      }
    }
  }
}
