#include "../include/CSHINECsIEnergyCali.h"
using namespace std;

//______________________________________________________________________
CSHINECsIEnergyCali::CSHINECsIEnergyCali()
{}

//_____________________________________________
CSHINECsIEnergyCali::~CSHINECsIEnergyCali()
{}


//______________________________________________________________________
// 在本次实验中, 将借助 DEEFIT 工具进行 CsI 晶体的能量刻度
// 由于 DEEFIT 对数据格式有要求, 因此首先需要将实验数据存成 DEEFIT 格式： 3个branch
// tree->SetBranch("numtel",  &numtel, "numtel/s");
// tree->SetBranch("desipgf", &desipgf, "desipgf/f");
// ree->SetBranch("fastpg",    &fastpg,   "fastpg/s");
void CSHINECsIEnergyCali::GenerateDEEFITData(Int_t firstrun, Int_t lastrun)
{
  std::string pathrootfileout(Form("/home/sea/Fission2019_Data/DEEFITData_Run%04d-%04d.root", firstrun, lastrun));
  TFile* newfile = new TFile(pathrootfileout.c_str(), "RECREATE");
  if (!newfile || !newfile->IsOpen()) {
    cout<<Form("File %s not found.\n", pathrootfileout.c_str());
    return;
  }

  fdeefitdata = new DEEFITData;
  TTree* newtree = new TTree("DEEFITtree", "DEEFIT tree for Fission2019");
  newtree->Branch("numtel", &fdeefitdata->numtel, "numtel/S"); // number of CsI
  newtree->Branch("desipgf", &fdeefitdata->desipgf, "desipgf/F"); // dESi
  newtree->Branch("fastpg",   &fdeefitdata->fastpg,    "fastpg/S");  // ECsI



  newfile->Close();
}
