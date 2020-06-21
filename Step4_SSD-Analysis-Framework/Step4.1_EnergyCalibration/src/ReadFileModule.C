# include "../include/ReadFileModule.h"

//_______________________________________________________
ReadFileModule::ReadFileModule()
{
  //cout<<"ReadFileModule Loaded!"<<endl;
}

//___________________________________________________________
ReadFileModule::~ReadFileModule()
{
  //cout<<"ReadFileModule Deleted!"<<endl;
}

//___________________________________________________________
Double_t*** ReadFileModule::ReadData(const Char_t* datapath, Int_t& SSDNum,
            Int_t& CHNum, Int_t& ParNum)
{
  Double_t*** readpar = NULL;
  readpar = new Double_t** [SSDNum];
  for(Int_t i=0; i<SSDNum; i++)
  {
    readpar[i] = new Double_t* [CHNum];
    for(Int_t j=0; j<CHNum; j++)
    {
      readpar[i][j]= new Double_t[ParNum];
    }
  }
  ifstream in;
  in.open(datapath);
  if(!in.is_open())
  {
    printf("Error: file %s not found\n",datapath);
    return NULL;
  }
  while(in.good())
  {
    // 按行读取数据
    std::string LineRead;
    std::getline(in, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

    Int_t ssdnum;
    Int_t chnum;

    LineStream >> ssdnum >> chnum;
    for(Int_t i=0; i<ParNum; i++)
    {
      LineStream >>readpar[ssdnum][chnum][i];
    }
  }
  in.close();
  return readpar;
}


//___________________________________________________________
void ReadFileModule::DeleteData(Double_t*** p, Int_t& SSDNum, Int_t& CHNum, Int_t& ParNum)
{
  for(Int_t i=0; i<SSDNum; i++)
  {
    for(Int_t j=0; j<CHNum; j++)
    {
      delete [] p[i][j];
    }
  }
  for(Int_t i=0; i<SSDNum; i++)
  {
    delete [] p[i];
  }
  delete [] p;
}


//______________________________________________________________________________
void ReadFileModule::AddChain(TChain* chain, const char* L1STag, Int_t* dataarray, Int_t size, Int_t index)
{
  for (Int_t j=0; j<size; j++)
  {
    dataarray[j] = 0;
  }
  std::string SSD_E_bname;
  SSD_E_bname = Form("SSD%d_%s_E", index+1, L1STag);
  chain->SetBranchStatus(SSD_E_bname.c_str(), true);
  chain->SetBranchAddress(SSD_E_bname.c_str(), dataarray);
}
