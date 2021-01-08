//#define Test_Multi_cxx
#include "Test_Multi.h"
#include <iostream>

using namespace std;


//______________________________________________________________________________
Test_Multi::Test_Multi(Int_t firstrun, Int_t lastrun) : fChain(0)
{
  fFirstRun = firstrun;
  fLastRun = lastrun;

  std::string pathrootfilein(Form("/home/sea/Fission2019_Data/CSHINEEvent/EventTree_Run%04d-Run%04d.root", fFirstRun, fLastRun));
  TTree* tree = 0;
  TFile* f = (TFile*)gROOT->GetListOfFiles()->FindObject(pathrootfilein.c_str());
  if (!f || !f->IsOpen()) {
    f = new TFile(pathrootfilein.c_str());
  }
  f->GetObject("LayerEvent",tree);
  Init(tree);
}

Test_Multi::~Test_Multi()
{
  if (!fChain) return;
  // delete fChain->GetCurrentFile();
}


void Test_Multi::Init(TTree* tree)
{
  // Set branch addresses and branch pointers
  if (!tree) return;
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("LayerEvent.fL1SMulti",            &LayerEvent_fL1SMulti);
  fChain->SetBranchAddress("LayerEvent.fL1SSSDNum",    &LayerEvent_fL1SSSDNum);
  fChain->SetBranchAddress("LayerEvent.fL1SNumStrip",  &LayerEvent_fL1SNumStrip);
  fChain->SetBranchAddress("LayerEvent.fL1SEMeV",           &LayerEvent_fL1SEMeV);
  fChain->SetBranchAddress("LayerEvent.fL2FMulti",            &LayerEvent_fL2FMulti);
  fChain->SetBranchAddress("LayerEvent.fL2FSSDNum",    &LayerEvent_fL2FSSDNum);
  fChain->SetBranchAddress("LayerEvent.fL2FNumStrip",   &LayerEvent_fL2FNumStrip);
  fChain->SetBranchAddress("LayerEvent.fL2FEMeV",            &LayerEvent_fL2FEMeV);
  fChain->SetBranchAddress("LayerEvent.fL2BMulti",            &LayerEvent_fL2BMulti);
  fChain->SetBranchAddress("LayerEvent.fL2BSSDNum",    &LayerEvent_fL2BSSDNum);
  fChain->SetBranchAddress("LayerEvent.fL2BNumStrip",  &LayerEvent_fL2BNumStrip);
  fChain->SetBranchAddress("LayerEvent.fL2BEMeV",           &LayerEvent_fL2BEMeV);
  fChain->SetBranchAddress("LayerEvent.fCsIMulti",             &LayerEvent_fCsIMulti);
  fChain->SetBranchAddress("LayerEvent.fCsISSDNum",     &LayerEvent_fCsISSDNum);
  fChain->SetBranchAddress("LayerEvent.fCsINum",             &LayerEvent_fCsINum);
  fChain->SetBranchAddress("LayerEvent.fCsIECh",               &LayerEvent_fCsIECh);
  fChain->SetBranchAddress("LayerEvent.fSSDL1SMulti",   &LayerEvent_fSSDL1SMulti);
  fChain->SetBranchAddress("LayerEvent.fSSDL2FMulti",   &LayerEvent_fSSDL2FMulti);
  fChain->SetBranchAddress("LayerEvent.fSSDL2BMulti",  &LayerEvent_fSSDL2BMulti);
  fChain->SetBranchAddress("LayerEvent.fSSDCsIMulti",    &LayerEvent_fSSDCsIMulti);
}

//______________________________________________________________________


//______________________________________________________________________
// 自定义径迹查找算法1 : 循环-循环-判断-循环-判断-循环-判断
// 自定义径迹查找算法2 : 循环-循环-循环-循环-判断-判断-判断-判断
// 经验证, 算法2 与 算法1 等效 !
// 算法2 更加简洁明了，为方便起见，使用算法2进行后续分析
// 这一步生成 TrackEvent，便于后续的模式识别使用
void Test_Multi::TrackReconstructionAlgorithm()
{
  Int_t  globalmulti;
  Int_t  ssdglobalmulti;                                       // 每一个SSD的 globalmulti
  Int_t  count_satisfiedmulticut[4]  =  {0};   // 满足初步 multi cut 计数
  Int_t  count_effentries  [4]  =  {0};                 // globalmulti>0 的计数

  TimeAndPercentage timeper;

  TFile* myfile = new TFile(Form("/home/sea/Fission2019_Data/TrackReconstructionEvent_Run%04d-Run%04d.root", fFirstRun, fLastRun),"RECREATE");
  TTree* mytree = new TTree("TrackEvent","TrackEvent Tree");
  mytree->Branch("TrackEvent.", "CSHINETrackEvent", &fTrackEvent, 32000, 2);

  ofstream  FileOut[NUM_SSD];
  for (Int_t i=0; i<NUM_SSD; i++) {
    FileOut[i].open(Form("data/data_Test_Multi/SSD%d_Test_Multi_L1L2EnergyCut.dat",i+1));
  }

  //_____________________________________________________________________
  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  for (Long64_t jentry=0; jentry<nentries; jentry++) {

    fChain->GetEntry(jentry);

    timeper.PrintPercentageAndRemainingTime(jentry, nentries);
    globalmulti = 0 ;
    for (Int_t numtel=0; numtel<NUM_SSD; numtel++) {

      ssdglobalmulti = 0;

      if ((LayerEvent_fSSDCsIMulti[numtel]>0 && LayerEvent_fSSDCsIMulti[numtel]  <=MULTICUT_L3A) &&
       (LayerEvent_fSSDL2BMulti[numtel]>0 && LayerEvent_fSSDL2BMulti[numtel]<=MULTICUT_L2B) &&
       (LayerEvent_fSSDL2FMulti[numtel]>0 && LayerEvent_fSSDL2FMulti[numtel] <=MULTICUT_L2F)  &&
       (LayerEvent_fSSDL1SMulti[numtel]>0 && LayerEvent_fSSDL1SMulti[numtel]<=MULTICUT_L1S))   // 给定初步的 multi cut
      {
        count_satisfiedmulticut[numtel]++;

        for (Int_t csimulti=0; csimulti<LayerEvent_fCsIMulti; csimulti++) {
          for (Int_t l2bmulti=0; l2bmulti<LayerEvent_fL2BMulti; l2bmulti++) {
            for (Int_t l2fmulti=0; l2fmulti<LayerEvent_fL2FMulti; l2fmulti++) {
              for (Int_t l1smulti=0; l1smulti<LayerEvent_fL1SMulti; l1smulti++) {   //逐层循环,遍历所有可能的组合

                if (LayerEvent_fCsISSDNum[csimulti]==numtel && LayerEvent_fL2BSSDNum[l2bmulti]==numtel &&
                  LayerEvent_fL2FSSDNum[l2fmulti]==numtel && LayerEvent_fL1SSSDNum[l1smulti]==numtel)  // 对每个SSD分开讨论
                {
                  if (fPattern.GeoConstraint_L3A_L2B(LayerEvent_fCsINum[csimulti], LayerEvent_fL2BNumStrip[l2bmulti])      &&
                    fPattern.EneConstraint_L2B_L2F(numtel, LayerEvent_fL2BEMeV[l2bmulti], LayerEvent_fL2FEMeV[l2fmulti]) &&
                    fPattern.GeoConstraint_L3A_L2F(LayerEvent_fCsINum[csimulti], LayerEvent_fL2FNumStrip[l2fmulti])  &&
                    fPattern.GeoConstraint_L2B_L1S(LayerEvent_fL2BNumStrip[l2bmulti], LayerEvent_fL1SNumStrip[l1smulti])) // 应用几何约束条件与L2的能量约束条件
                  {
                    if (!fPattern.EneConstraint_L1_L2(numtel, LayerEvent_fL1SEMeV[l1smulti], LayerEvent_fL2FEMeV[l2fmulti])) continue; //利用L1-L2的能量约束条件
                    globalmulti++;
                    ssdglobalmulti++;

                    fTrackEvent.fGSSDNum.push_back(numtel);
                    fTrackEvent.fGCsINum.push_back(LayerEvent_fCsINum[csimulti]);
                    fTrackEvent.fGCsIECh.push_back(LayerEvent_fCsIECh[csimulti]);
                    fTrackEvent.fGL2BNumStrip.push_back(LayerEvent_fL2BNumStrip[l2bmulti]);
                    fTrackEvent.fGL2BEMeV.push_back(LayerEvent_fL2BEMeV[l2bmulti]);
                    fTrackEvent.fGL2FNumStrip.push_back(LayerEvent_fL2FNumStrip[l2fmulti]);
                    fTrackEvent.fGL2FEMeV.push_back(LayerEvent_fL2FEMeV[l2fmulti]);
                    fTrackEvent.fGL1SNumStrip.push_back(LayerEvent_fL1SNumStrip[l1smulti]);
                    fTrackEvent.fGL1SEMeV.push_back(LayerEvent_fL1SEMeV[l1smulti]);

                    FileOut[numtel]<<setw(10)<<"M_L3A = "<<setw(2)<<LayerEvent_fSSDCsIMulti[numtel]
                           <<setw(10)<<"M_L2B = "<<setw(2)<<LayerEvent_fSSDL2BMulti[numtel]
                           <<setw(10)<<"M_L2F = "<<setw(2)<<LayerEvent_fSSDL2FMulti[numtel]
                           <<setw(10)<<"M_L1S = "<<setw(2)<<LayerEvent_fSSDL1SMulti[numtel]
                           <<setw(15)<<"MultiGlobal = "<<ssdglobalmulti
                           <<setw(10)<<"CSI  = "<<setw(2)<<LayerEvent_fCsINum[csimulti]
                           <<setw(10)<<"L2B  = "<<setw(2)<<LayerEvent_fL2BNumStrip[l2bmulti]
                           <<setw(10)<<"L2F  = "<<setw(2)<<LayerEvent_fL2FNumStrip[l2fmulti]
                           <<setw(10)<<"L1S  = "<<setw(2)<<LayerEvent_fL1SNumStrip[l1smulti]
                           <<setw(10)<<"EL1S = "<<setw(7)<<LayerEvent_fL1SEMeV[l1smulti]
                           <<setw(10)<<"EL2F = "<<setw(7)<<LayerEvent_fL2FEMeV[l2fmulti]
                           <<setw(10)<<"EL2B = "<<setw(7)<<LayerEvent_fL2BEMeV[l2bmulti]
                           <<setw(10)<<"ECsI = "<<setw(4)<<LayerEvent_fCsIECh[csimulti]
                           <<endl;
                  }
                }
              }
            }
          }
        }
      }
      fTrackEvent.fSSDGlobalMulti.push_back(ssdglobalmulti);
      if (ssdglobalmulti>0) count_effentries[numtel]++;
    }
    fTrackEvent.fGlobalMulti = globalmulti;
    mytree->Fill();
    fTrackEvent.Swap();
  }
  mytree->Write();
  myfile->Close();

  for (Int_t i=0; i<NUM_SSD; i++) {
    cout<<Form("count_satisfiedmulticut[%d] = ",i+1)<<count_satisfiedmulticut[i]<<setw(30)
      <<Form("count_effentries[%d] = ",i+1)<<count_effentries[i]<<endl;
  }
}
//______________________________________________________________________


//______________________________________________________________________
// 将 TrackReconstructionAlgorithm 生成的 TrackEvent
// 以 fridend 的形式添加到 LayerEvent 中
void Test_Multi::CheckTrackEvent()
{
  TFile*  myfile    = new TFile("/home/sea/Fission2019_Data/TrackReconstructionEvent.root");
  if (!myfile || !myfile->IsOpen()) {
    cout<<Form("File %s not founded.",  myfile->GetName());
    return;
  }

  TTree* mytree = (TTree*)myfile->Get("TrackEvent");
  mytree->SetMakeClass(1);
  //mytree->SetBranchAddress("LayerEvent.f");

  Long64_t nentries = mytree->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
  //  mytree->GetEntry(ientry);
  //  fChain->GetEntry(ientry);
  //  cout<<"LayerEvent_fL1SMulti = "<<LayerEvent_fSSDL1SMulti[1]<<endl;
  }

}




//______________________________________________________________________
// 测试约束条件对事件数的影响
// 计算每个条件对每一层的贡献
// 这里只是计算 L2FL2B_EnergyCut = 0.15 的情况
void Test_Multi::CheckCutEffOnMulti()
{}


//______________________________________________________________________
void Test_Multi::CheckL2BL2FEnergyCorrelation()
{}


//______________________________________________________________________
void Test_Multi::DetermineL2BL2FEnergyErrRatio()
{}


//______________________________________________________________________
void Test_Multi::CheckEnergyLossL1L2()
{}


//______________________________________________________________________
void Test_Multi::CheckEnergyLossL1L2_Relationship()
{}


//______________________________________________________________________
void Test_Multi::CheckEnergyLossL1L2_Expdata()
{}


//______________________________________________________________________
void Test_Multi::CheckLayerMultiPercentage()
{

}


//______________________________________________________________________
// 分析不同 fGlobalMulti 下的 fGlobalMulti 的比例
// 这一步的分析依赖于 TrackReconstructionAlgorithm() 生成的 TrackEvent 文件
void Test_Multi::CheckGlobalMultiRatio(Int_t ssdindex)
{
  // 定义变量， 以用于计算不同 fGlobalMulti 的比例
  Int_t  nummulti = 7;     // 考察 globalmulti<nummulti 的情况
  Int_t  globalmulti[7] = {0};
  Double_t multiratio_0[7]   = {0.};
  Double_t sumratio_0        =  0.;
  Double_t multiratio_1[7]   = {0.};
  Double_t sumratio_1        =  0.;
  Long64_t count = 0;

  TFile* myfile = new TFile("/home/sea/Fission2019_Data/TrackReconstructionEvent.root","READONLY");
  if (!myfile->IsOpen()) {
    cout<<"Open file failded."<<endl;
    return;
  }

  Int_t                   TrackEvent_fGlobalMulti;
  std::vector<Int_t>      TrackEvent_fSSDGlobalMulti;
  std::vector<Int_t>      TrackEvent_fGSSDNum;
  std::vector<Int_t>      TrackEvent_fGL1SNumStrip;
  std::vector<Double_t>   TrackEvent_fGL1SEMeV;
  std::vector<Int_t>      TrackEvent_fGL2FNumStrip;
  std::vector<Double_t>   TrackEvent_fGL2FEMeV;
  std::vector<Int_t>      TrackEvent_fGL2BNumStrip;
  std::vector<Double_t>   TrackEvent_fGL2BEMeV;
  std::vector<Int_t>      TrackEvent_fGCsINum;
  std::vector<Int_t>      TrackEvent_fGCsIECh;

  TTree* mytree = (TTree*)myfile->Get("TrackEvent");
  Long64_t nentries = mytree->GetEntries();
  cout<<"Found nentries = "<<nentries<<endl;

  mytree->SetMakeClass(1);  // 如果 tree 的 branch 使用了自定义的类, 则这条语句不能省略！！！
  mytree->SetBranchAddress("TrackEvent.fGlobalMulti",    &TrackEvent_fGlobalMulti);
  mytree->SetBranchAddress("TrackEvent.fSSDGlobalMulti", &TrackEvent_fSSDGlobalMulti);
  mytree->SetBranchAddress("TrackEvent.fGSSDNum",        &TrackEvent_fGSSDNum);
  mytree->SetBranchAddress("TrackEvent.fGL1SNumStrip",   &TrackEvent_fGL1SNumStrip);
  mytree->SetBranchAddress("TrackEvent.fGL1SEMeV",       &TrackEvent_fGL1SEMeV);
  mytree->SetBranchAddress("TrackEvent.fGL2FNumStrip",   &TrackEvent_fGL2FNumStrip);
  mytree->SetBranchAddress("TrackEvent.fGL2FEMeV",       &TrackEvent_fGL2FEMeV);
  mytree->SetBranchAddress("TrackEvent.fGL2BNumStrip",   &TrackEvent_fGL2BNumStrip);
  mytree->SetBranchAddress("TrackEvent.fGL2BEMeV",       &TrackEvent_fGL2BEMeV);
  mytree->SetBranchAddress("TrackEvent.fGCsINum",        &TrackEvent_fGCsINum);
  mytree->SetBranchAddress("TrackEvent.fGCsIECh",        &TrackEvent_fGCsIECh);

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    mytree->GetEntry(ientry);
    //___________________________________________
    // 统计不同 fGlobalMult 的计数
    for (Int_t i=0; i<nummulti; i++) {
      if (TrackEvent_fSSDGlobalMulti[ssdindex]==i)   globalmulti[i]++;
    }
  }

  // 计算比例，从 globalmulti = 0 算起
  for (Int_t i=0; i<nummulti; i++) {
    multiratio_0[i] = (Double_t) globalmulti[i]/nentries;
    sumratio_0 += multiratio_0[i];
    cout<<multiratio_0[i]<<setw(12);
  }
  cout<<sumratio_0<<endl;

  // 计算比例，从 globalmulti = 1 算起
  for (Int_t i=1; i<nummulti; i++) {
    multiratio_1[i] = (Double_t) globalmulti[i]/(nentries-globalmulti[0]); // 剔除 fGlobalMulti=0 的情况
    sumratio_1 += multiratio_1[i];
    cout<<multiratio_1[i]<<setw(12);
    count += globalmulti[i];
  }
  cout<<sumratio_1<<endl;
  cout<<"count = "<<count<<endl;

  myfile->Close();
}


//______________________________________________________________________
void Test_Multi::GlobalMulti_ExtractData(Int_t globalmulti)
{}


//______________________________________________________________________
void Test_Multi::CheckParallelDraw(const char* fglobalmulti)
{}


//______________________________________________________________________
void Test_Multi::CheckL2L3DEEPlot(const char* fglobalmulti)
{}


//______________________________________________________________________
void Test_Multi::CheckL1L2DEEPlot(const char* fglobalmulti)
{}


//______________________________________________________________________
void Test_Multi::CheckChargeSharingEffect_L1(Int_t ssdindex)
{}


//______________________________________________________________________
void Test_Multi::ChangeModeToNumber(std::string mode, Int_t& l3a, Int_t& l2b, Int_t& l2f, Int_t& l1s)
{}


//______________________________________________________________________
Bool_t Test_Multi::IsModeMatched(Int_t mode_index, Int_t ch1, Int_t ch2)
{
  return true;
}
