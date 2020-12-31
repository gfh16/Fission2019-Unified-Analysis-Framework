#include "../include/CSHINECsIEnergyCali.h"
using namespace std;

//______________________________________________________________________________
CSHINECsIEnergyCali::CSHINECsIEnergyCali(Int_t firstrun, Int_t lastrun)
{
  fFirstRun = firstrun;
  fLastRun  = lastrun;
}

//_________________________________________
CSHINECsIEnergyCali::~CSHINECsIEnergyCali()
{}
//______________________________________________________________________________


//______________________________________________________________________________
// 在本次实验中, 将借助 DEEFIT 工具进行 CsI 晶体的能量刻度
// 由于 DEEFIT 对数据格式有要求, 因此首先需要将实验数据存成 DEEFIT 格式： 3个branch
// tree->SetBranch("numtel", &numtel, "numtel/s");
// tree->SetBranch("desipgf",&desipgf,"desipgf/f");
// ree->SetBranch("fastpg",  &fastpg, "fastpg/s");
void CSHINECsIEnergyCali::GenerateDEEFITData()
{
  std::string pathrootfilein(Form("/home/sea/Fission2019_Data/TrackReconstructionEvent_Run%04d-Run%04d.root", fFirstRun, fLastRun));
  std::string pathrootfileout(Form("/home/sea/Fission2019_Data/DEEFITData_Run%04d-%04d.root", fFirstRun, fLastRun));

 // 新建 root 文件
  TFile* newfile = new TFile(pathrootfileout.c_str(), "RECREATE");
  if (!newfile || !newfile->IsOpen()) {
    cout<<Form("File %s not found.\n", pathrootfileout.c_str());
    return;
  }
	// 创建 DEEFIT tree
  fdeefitdata = new DEEFITTreeData;
  TTree* newtree = new TTree("h1", "DEEFIT tree for Fission2019");
  newtree->Branch("numtel", &fdeefitdata->numtel, "numtel/S"); // number of CsI
  newtree->Branch("desipgf",&fdeefitdata->desipgf,"desipgf/F");// dESi
  newtree->Branch("fastpg", &fdeefitdata->fastpg, "fastpg/S"); // ECsI

  Int_t TrackEvent_fGlobalMulti;
  std::vector<Int_t> TrackEvent_fGSSDNum;
  std::vector<Double_t> TrackEvent_fGL2FEMeV;
  std::vector<Int_t> TrackEvent_fGCsINum;
  std::vector<Int_t> TrackEvent_fGCsIECh;
 // 读取 TrackEvent，用于生成 DEFFIT 数据
  TFile* readfile = new TFile(pathrootfilein.c_str(), "READONLY");
  TTree* readtree = (TTree*)readfile->Get("TrackEvent");
  readtree->SetMakeClass(1);
 // 只打开需要的 branch
  readtree->SetBranchStatus("*", 0);
  readtree->SetBranchStatus("TrackEvent.fGlobalMulti", 1);
  readtree->SetBranchStatus("TrackEvent.fGSSDNum", 1);
  readtree->SetBranchStatus("TrackEvent.fGL2FEMeV",1);
  readtree->SetBranchStatus("TrackEvent.fGCsINum", 1);
  readtree->SetBranchStatus("TrackEvent.fGCsIECh", 1);
 // SetBranchAddress
  readtree->SetBranchAddress("TrackEvent.fGlobalMulti", &TrackEvent_fGlobalMulti);
  readtree->SetBranchAddress("TrackEvent.fGSSDNum", &TrackEvent_fGSSDNum);
  readtree->SetBranchAddress("TrackEvent.fGL2FEMeV",&TrackEvent_fGL2FEMeV);
  readtree->SetBranchAddress("TrackEvent.fGCsINum", &TrackEvent_fGCsINum);
  readtree->SetBranchAddress("TrackEvent.fGCsIECh", &TrackEvent_fGCsIECh);

  Long64_t nentries = readtree->GetEntries();
  cout<<"nentries = "<<nentries<<endl;
  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    readtree->GetEntry(ientry);

    if (TrackEvent_fGlobalMulti==1) {
      fdeefitdata->numtel  = (UShort_t)TrackEvent_fGSSDNum[0]*9 + TrackEvent_fGCsINum[0];
      fdeefitdata->desipgf = (Float_t) TrackEvent_fGL2FEMeV[0];
      fdeefitdata->fastpg  = (UShort_t)TrackEvent_fGCsIECh[0];
    }
    newtree->Fill();
  }
  newfile->Write();
  newfile->Close();
  readfile->Close();
}


//______________________________________________________________________________
// DEEFIT 是意大利国家核物理研究院 INFN-LNS 为 CHIMERA 探测器开发的一套
// 用于硅望远镜粒子鉴别的工具
// DEEFIT 的源文件请参考 DEEFIT/ 文件夹下的：
// ** MDEEFrame.h && MDEEFrame.C ： DEEFIT 的框架与主体
// ** MTree.h && MTree.C : DEFFIT 输入数据的结构要求
void CSHINECsIEnergyCali::RunDEEFitCode()
{
  system("cd DEEFIT/ && ./deefit_S");
}


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
{

}


//______________________________________________________________________________
//              定义 DEEFIT 采用的拟合函数 func14
// Function for DE-E identification based on formulas from
//   *** N. Le Neindre et al. NIM A490 (2002) 251 ***
// modified following the formula of W. Gawlikowicz for Light-Energy relation in CsI(Tl)
//   *** W. Gawlikowicz, NIM A491 (2002) 181. ***
// Adapted by Amalia Pop following ref.:
//   *** J. Blicharska et al. LNS report 2005 ***
// This formula needs 14 fit parameters
Double_t CSHINECsIEnergyCali::DEEFITFunc14(DEEFITParticle& p, Double_t* par)
{
  Double_t X = p.E;
  Double_t Z = p.Z;
  Double_t A = p.A;
  Double_t xx, exp1, exp2, res=1.0;
  Double_t reslog, ene;

  if(Z==0 && A==0) return par[12];

  xx = X-par[13];
  bool foffset = false; //this is true for fitting procedure, false for the analysis
  //if xx<0 never reached from data analysis
  if(xx<=0) {
    if(foffset) xx = X;
    else return res;
  }

  exp1   = par[2] + par[3] + 1;
  exp2   = 1./exp1;
  reslog = log(1.+ par[9]*Z)*(1. - exp(-par[10]*pow(Z,par[11])))/Z;
  ene    = par[6]*reslog*xx ;
  ene    = ene + par[7]*Z*sqrt(A)*log(1. + par[8]*reslog*xx);
  res    = pow(ene,exp1)
         + pow((par[4]*pow(Z,par[0])*pow(A,par[1])),exp1)
         + par[5]*pow(Z,2)*pow(A,par[2])*pow(ene,par[3]);

  res    = pow(res,exp2) - ene + par[12];
  return res;
}
