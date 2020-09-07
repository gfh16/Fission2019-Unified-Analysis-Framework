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
Int_t LastRun  = 202;


//_____________________________________________________________________________
void PID_L2B_L3(TTree* mytree);


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

  PID_L2B_L3(mytree);

}


//______________________________________________________________________________
void PID_L2B_L3(TTree* mytree)
{
    Int_t      SSD1_fMultiL2B;
    Int_t      SSD1_fNumStripL2B[16];
    Double_t   SSD1_fEMeVL2B    [16];

    Int_t      SSD1_fMultiCsI;
    Int_t      SSD1_fNumCsI[9];
    Int_t      SSD1_fEChCsI[9];


    mytree->SetBranchStatus("*", false);
    mytree->SetMakeClass(1);   // 必须加上这一句
    mytree->SetBranchStatus("SSD1.fMultiL2B",    true);
    mytree->SetBranchStatus("SSD1.fNumStripL2B", true);
    mytree->SetBranchStatus("SSD1.fEMeVL2B",     true);
    mytree->SetBranchStatus("SSD1.fMultiCsI",    true);
    mytree->SetBranchStatus("SSD1.fNumCsI",      true);
    mytree->SetBranchStatus("SSD1.fEChCsI",      true);

    mytree->SetBranchAddress("SSD1.fMultiL2B",    &SSD1_fMultiL2B    );
    mytree->SetBranchAddress("SSD1.fNumStripL2B",  SSD1_fNumStripL2B );
    mytree->SetBranchAddress("SSD1.fEMeVL2B",      SSD1_fEMeVL2B     );
    mytree->SetBranchAddress("SSD1.fMultiCsI",    &SSD1_fMultiCsI    );
    mytree->SetBranchAddress("SSD1.fNumCsI",       SSD1_fNumCsI      );
    mytree->SetBranchAddress("SSD1.fEChCsI",       SSD1_fEChCsI      );


    TH2D* hist = new TH2D("h2d","h2d", 4096,0,4096,300,0.,300.);

    unsigned long nentries = mytree->GetEntries();
    cout<< "Found " << nentries <<" entries"<<endl;

    for (unsigned long jentry=0; jentry<nentries; jentry++)
    {
      mytree->GetEntry(jentry);

    //  for (Int_t i=0; i)
      if (SSD1_fMultiL2B == 1 && SSD1_fMultiCsI ==1) {
        if (SSD1_fNumCsI[0] == 1) {
          hist->Fill(SSD1_fEChCsI[0],SSD1_fEMeVL2B[0]);
        }
      }

    }
    TCanvas* cans2 = new TCanvas("cans2","cans2",800,600);
    cans2->cd();
    hist->Draw("COLZ");
}
