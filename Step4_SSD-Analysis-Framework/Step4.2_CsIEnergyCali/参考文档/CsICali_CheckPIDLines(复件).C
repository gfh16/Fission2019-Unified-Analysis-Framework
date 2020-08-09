////////////////////////////////////////////////////////////////////////////////
//
//  检查 dE-E 谱的质量, 确定采取怎样的方式填充 dE-E 并最终用于 DEFFIT 进行拟合
//  需要考虑以下几个问题:
//  1.Layer2 与 Layer3 的 dE-E
//    (1) L2F:L3A or L2B:L3A     // 选取 L2F, 还是 L2B 与 CsI 匹配？
//    (2) L2 : L3A[csi] or L2[ch] : L3A[csi] // 选取全部strip还是某一strip与 CsI 匹配
//
//  2.Layer1 与 Layer2
//    (1) L1S:L2F or L1S:L2B
//    (2) L1S:L2F or
//
//
//  gfh, 2020-07-30
////////////////////////////////////////////////////////////////////////////////
#include "../../include/CSHINEBuildEvent.h"
#include "../../include/CSHINEBuildEvent.h"
#include "../../include/CSHINEEvent.h"
#include "../../include/ReadFileModule.h"
#include "../../include/TimeAndPercentage.h"
#include "../../include/CSHINESSDCalibratedData.h"


Int_t FirstRun = 200;
Int_t LastRun  = 201;


//______________________________________________________________________________
void PID_L2F_L3(TTree* mytree);
void PID_L2B_L3(TTree* mytree);
void PID_L1_L2F(TTree* mytree);

//______________________________________________________________________________
void CsICali_CheckPIDLines()
{

  std::string pathRootEventTreeFolder("/home/sea/Fission2019_Data/CSHINEEvent/");
  std::string pathRootInput(Form("%sEventTree_Run%04d-Run%04d.root",pathRootEventTreeFolder.c_str(),
                                  FirstRun,LastRun));

  TFile* file = new TFile(pathRootInput.c_str());
  if (!file->IsOpen()) {
    cout<<Form("*** Error: Open file %s error! ***", pathRootInput.c_str())<<endl;
  }

  TTree* mytree = (TTree*)file->Get("CSHINEEvent");

  PID_L2F_L3(mytree);
  PID_L2B_L3(mytree);
//    PID_L1_L2F(mytree);

}


//______________________________________________________________________________
// 遇到问题: Error in <Setup>: Missing TClass object for
// 未解决, 不过对结果没有影响！
void PID_L2F_L3(TTree* mytree)
{

//   mytree->SetMakeClass(1);
//  mytree->SetBranchStatus("*", false);
//  mytree->SetBranchStatus("SSD1.fMultiL2F", true);
//  mytree->SetBranchStatus("SSD1.fEMeVL2F", true);


  TTreeReader myReader(mytree);
  TTreeReaderValue<Int_t>    SSD1_fMultiL1S(myReader, "SSD1.fMultiL1S");
  TTreeReaderArray<Double_t> SSD1_fEMeVL1S (myReader, "SSD1.fEMeVL1S");

  TH1D* hist = new TH1D("h1","h1", 1000,0.,1000.);

  unsigned long nentries = mytree->GetEntries();
  cout<< "Found " << nentries <<" entries"<<endl;
  unsigned long jentry = 0;

  while (myReader.Next())
  {
    if (*SSD1_fMultiL1S == 1) {
    //  hist->Fill(SSD1_fEMeVL1S[0]);
    }
  }
  TCanvas* cans1 = new TCanvas("cans1","cans1",800,600);
  cans1->cd();
  hist->Draw();

}


//______________________________________________________________________________
void PID_L2B_L3(TTree* mytree)
{
    Int_t    SSD1_fMultiL1S;
    Double_t SSD1_fEMeVL1S[16];

    mytree->SetMakeClass(1);   // 必须加上这一句
    mytree->SetBranchStatus("*", false);
    mytree->SetBranchStatus("SSD1.fMultiL1S", true);
    mytree->SetBranchStatus("SSD1.fEMeVL1S", true);

    mytree->SetBranchAddress("SSD1.fMultiL1S", &SSD1_fMultiL1S);
    mytree->SetBranchAddress("SSD1.fEMeVL1S",   SSD1_fEMeVL1S);

    TH1I* hist = new TH1I("h2","h2", 1000,0.,1000.);

    unsigned long nentries = mytree->GetEntries();
    cout<< "Found " << nentries <<" entries"<<endl;
    for (unsigned long jentry=0; jentry<nentries; jentry++)
    {
      mytree->GetEntry(jentry);

      if (SSD1_fMultiL1S == 1) {
        hist->Fill(SSD1_fEMeVL1S[0]);
      }

    }
    TCanvas* cans2 = new TCanvas("cans2","cans2",800,600);
    cans2->cd();
    hist->Draw();
}
