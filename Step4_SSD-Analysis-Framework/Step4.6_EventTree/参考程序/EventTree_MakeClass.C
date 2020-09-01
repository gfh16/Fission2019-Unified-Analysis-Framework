////////////////////////////////////////////////////////////////////////////////
//
//  使用 TTree::MakeClass 方法, 创建关于 Event 的类,
//  运行此 macro 后，将生成 .h, .C 两个文件
//  进行数据分析, 修改 .C 文件即可
//
////////////////////////////////////////////////////////////////////////////////


Int_t FirstRun = 200;
Int_t LastRun  = 400;


//______________________________________________________________________________
void PID_L2_L3(TTree* mytree);
void PID_L1_L2(TTree* mytree);


//______________________________________________________________________________
void EventTree_MakeClass()
{

  std::string pathRootEventTreeFolder("/home/sea/Fission2019_Data/CSHINEEvent/");
  std::string pathRootInput(Form("%sEventTree_Run%04d-Run%04d.root",pathRootEventTreeFolder.c_str(),
                                  FirstRun,LastRun));

  TFile* file = new TFile(pathRootInput.c_str());
  if (!file->IsOpen()) {
    cout<<Form("*** Error: Open file %s error! ***", pathRootInput.c_str())<<endl;
  }

  TTree* mytree = (TTree*)file->Get("CSHINEEvent");

  mytree->MakeClass("CsICali_EventClassAnalysis");
}
