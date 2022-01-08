////////////////////////////////////////////////////////////////////////////////
//  CSHINEPPACCalibration 这个类用于将 PPAC 的信号从 PPAC 本体坐标系转换到实验室系
//  PPAC 信号在本体坐标系下的位置由 X1,X2,Y1,Y2 信号同时决定, 这一步刻度的结果由刁昕玥
//  提供. CSHINEPPACCalibration 根据刁昕玥提供的刻度参数进行坐标系转换
//
//  gfh, 2021年12月
////////////////////////////////////////////////////////////////////////////////

#include "../include/CSHINEPPACCalibration.h"


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
CSHINEPPACCalibration::CSHINEPPACCalibration()
{}

CSHINEPPACCalibration::~CSHINEPPACCalibration()
{}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// XY在这一步存数据时做顺序交换(PPAC自身坐标系定义与实验室系定义XY不同）
// X正方向需要做反向处理，同时需要加上原点平移的7.5
//
// ！！！刻度数据由刁昕玥提供！！！
//
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
TVector3 CSHINEPPACCalibration::XYSignalToCoordinateInPPACFrame(Int_t numppac,
	Int_t X1, Int_t X2, Int_t Y1, Int_t Y2)
{
  if (numppac == 0) { // PPAC #1
    fVectorInPPACFrame[0].SetX( -(0.103135*(Y1-Y2)-5.70571) - 7.5 );
		fVectorInPPACFrame[0].SetY( 0.10269*(X1-X2) + 1.82741 );
		fVectorInPPACFrame[0].SetZ( 0. );
	}
	if (numppac == 1) { // PPAC #2
		fVectorInPPACFrame[1].SetX( -(0.102065*(Y1-Y2)+2.96969) - 7.5 );
		fVectorInPPACFrame[1].SetY( 0.101775*(X1-X2) + 0.691147 );
		fVectorInPPACFrame[1].SetZ( 0. );
	}
	if (numppac == 2) { // PPAC #3
		fVectorInPPACFrame[2].SetX( -(0.10199*(Y1-Y2)-1.08413) - 7.5 );
		fVectorInPPACFrame[2].SetY( 0.103011*(X1-X2) - 0.17619 );
		fVectorInPPACFrame[2].SetZ( 0. );
	}

	return fVectorInPPACFrame[numppac];
}


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
TVector3 CSHINEPPACCalibration::CoordinatePPACFrameToLabFrame(Int_t numppac, TVector3 vecppacframe)
{
  Double_t X_Lab =  vecppacframe.X()*TMath::Cos(fPhiOfPPACFixedPosition[numppac]*TMath::DegToRad()) + fDistFromTtoTarget[numppac]*TMath::Sin(fPhiOfPPACFixedPosition[numppac]*TMath::DegToRad());
  Double_t Y_Lab =  vecppacframe.Y();
	Double_t Z_Lab = -vecppacframe.X()*TMath::Sin(fPhiOfPPACFixedPosition[numppac]*TMath::DegToRad()) + fDistFromTtoTarget[numppac]*TMath::Cos(fPhiOfPPACFixedPosition[numppac]*TMath::DegToRad());

	fVectorInLabFrame[numppac].SetXYZ(X_Lab, Y_Lab, Z_Lab);
	return fVectorInLabFrame[numppac];
}


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void CSHINEPPACCalibration::GetAndCorrectOriginalSignals(Int_t numppac, Int_t& T,
	Int_t& X1, Int_t& X2, Int_t& Y1, Int_t& Y2)
{
	// 只有当 X面, Y面都至少有一个信号时,该时间为有效事件
	PPAC_X1_GOOD = X1 > PPAC_TDC_CUT ? true : false;
	PPAC_X2_GOOD = X2 > PPAC_TDC_CUT ? true : false;
	PPAC_Y1_GOOD = Y1 > PPAC_TDC_CUT ? true : false;
	PPAC_Y2_GOOD = Y2 > PPAC_TDC_CUT ? true : false;

	T = T;

	if (PPAC_X1_GOOD && PPAC_X2_GOOD)    { X1 = X1; X2 = X2; }
	if (PPAC_X1_GOOD && (!PPAC_X2_GOOD)) { X1 = X1; X2 = Int_t (fPPAC_X1_Plus_X2_Minus_2T[numppac] + 2*T - X1 + 0.5); } // 取最接近的整数
	if ((!PPAC_X1_GOOD) && PPAC_X2_GOOD) { X1 = Int_t (fPPAC_X1_Plus_X2_Minus_2T[numppac] + 2*T - X2 + 0.5); X2 = X2; } // 取最接近的整数

	if (PPAC_Y1_GOOD && PPAC_Y2_GOOD)    { Y1 = Y1; Y2 = Y2; }
	if (PPAC_Y1_GOOD && (!PPAC_Y2_GOOD)) { Y1 = Y1; Y2 = Int_t (fPPAC_Y1_Plus_Y2_Minus_2T[numppac] + 2*T - Y1 + 0.5); } // 取最接近的整数
	if ((!PPAC_Y1_GOOD) && PPAC_Y2_GOOD) { Y1 = Int_t (fPPAC_Y1_Plus_Y2_Minus_2T[numppac] + 2*T - Y2 + 0.5); Y2 = Y2; } // 取最接近的整数
}


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// 将 RF1 的信号平移到同一个周期
Int_t CSHINEPPACCalibration::ShiftRF1IntoOnePeriod(Int_t RF1)
{
	if (RF1>0 && RF1<=185) {
	   RF1 = RF1 + fRF1Period*3;
  }
  else if (RF1>185 && RF1<=940) {
	   RF1 = RF1 + fRF1Period*2;
  }
	else if (RF1>940 && RF1<=1695) {
	  RF1 = RF1 + fRF1Period*1;
  }
  else if (RF1>1695 && RF1<=2450) {
	  RF1 = RF1 + fRF1Period*0;
  }
  else if (RF1>2450 && RF1<=3205) {
	  RF1 = RF1 + fRF1Period*(-1);
  }
  else {
	  RF1 = RF1 + fRF1Period*(-2);;
  }

	return RF1;
}


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// 单位: ns  [ 利用高频信号 RF1, 计算裂变碎片的飞行时间 ]
Double_t CSHINEPPACCalibration::CalcTOFWithRF1Calibrated(Int_t PPAC_T, Int_t RF1)
{
  return ( (PPAC_T-ShiftRF1IntoOnePeriod(RF1))*fADCChannelTons - fCalibratedTimeConst );
}
