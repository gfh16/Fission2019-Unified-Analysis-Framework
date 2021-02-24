#include "../include/CSHINECsIEnergyCali.h"
using namespace std;

//______________________________________________________________________________
CSHINECsIEnergyCali::CSHINECsIEnergyCali(Int_t firstrun, Int_t lastrun)
{;}

//_________________________________________
CSHINECsIEnergyCali::~CSHINECsIEnergyCali()
{;}
//______________________________________________________________________________


//______________________________________________________________________________
// 根据 DEEFIT 拟合参数, 计算 CsI 中的能量点 (ECh, EMeV)
// 1. ECh: 手动选取.
//    1.1 需要先手动选出每条 PID 带子ECh的范围，比如: 对于质子 p, 100< ECh < 600
//        这一步可以用 excel 完成, 然后将数据导出
//    1.2 根据选定的范围, 对每个 ECh bin循环, 进行下一步计算
//
//               DEEFIT_Func14                 LISE++
// 2. EMeV: ECh ---------------> (ECh, dESi) ----------> (ECh, EMeV)
//
void CSHINECsIEnergyCali::GetDEEFITCsIEnergyPoints()
{}


//______________________________________________________________________________
// load DEEFITFunc14() 拟合参数
Double_t** CSHINECsIEnergyCali::DEEFITLoadPars(const char* pathParsFile)
{
	return NULL;
}


//______________________________________________________________________________
// 使用 TCutG 对所有的同位素作 cut
void CSHINECsIEnergyCali::DoGraphicalCut()
{}

//______________________________________________________________________________
// 使用 "投影法" 取点 (ECh, EMeV)
void CSHINECsIEnergyCali::GetProjectionCsIEnergPoints()
{}
