#include "../include/ReadFileModule.h"


//******************************************************************************
ReadFileModule::ReadFileModule()
{
  //cout<<"ReadFileModule Loaded!"<<endl;
}

//______________________________________________________________________________
ReadFileModule::~ReadFileModule()
{
  //cout<<"ReadFileModule Deleted!"<<endl;
}
//******************************************************************************


//******************************************************************************
Double_t*** ReadFileModule::ReadData(const Char_t* datapath, Int_t SSDNum,
            Int_t CHNum, Int_t ParNum)
{
  Double_t*** readpar = NULL;
  readpar = new Double_t** [SSDNum];
  for(Int_t i=0; i<SSDNum; i++)
  {
    readpar[i] = new Double_t* [CHNum];
    for(Int_t j=0; j<CHNum; j++) {
      readpar[i][j]= new Double_t[ParNum];
    }
  }
  ifstream in;
  in.open(datapath);
  if(!in.is_open()) {
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
    for(Int_t i=0; i<ParNum; i++) {
      LineStream >>readpar[ssdnum][chnum][i];
    }
  }
  in.close();
  return readpar;
}
//******************************************************************************


//******************************************************************************
void ReadFileModule::DeleteData(Double_t*** p, Int_t SSDNum, Int_t CHNum, Int_t ParNum)
{
  for(Int_t i=0; i<SSDNum; i++) {
    for(Int_t j=0; j<CHNum; j++) {
      delete [] p[i][j];
    }
  }
  for(Int_t i=0; i<SSDNum; i++) {
    delete [] p[i];
  }
  delete [] p;
}
//******************************************************************************


//******************************************************************************
void ReadFileModule::AddChain(TChain* chain, const char* LayerTag, Int_t* dataarray, Int_t size, Int_t index)
{
  for (Int_t j=0; j<size; j++) {
    dataarray[j] = 0;
  }
  std::string SSD_E_bname;
  SSD_E_bname = Form("SSD%d_%s_E", index+1, LayerTag);
  chain->SetBranchStatus(SSD_E_bname.c_str(), true);
  chain->SetBranchAddress(SSD_E_bname.c_str(), dataarray);
}
//******************************************************************************


//******************************************************************************
void ReadFileModule::GetFilesName(const char* pathfolder, const char* pathfileout)
{
	DIR* directory_pointer;
  struct dirent** namelist;

  ofstream fileout(pathfileout);
  fileout<<"* filename\n";

  if ((directory_pointer = opendir(pathfolder))==NULL) {
    printf("Error: folder %s not found!\n", pathfolder);
    return;
  } else {
    Int_t n = scandir(pathfolder, &namelist, 0, alphasort); //使用scandir函数对文件名进行排序
    if (n < 0) {
      cout<<"scandir return "<<n<<endl;
    } else {
      Int_t index = 0;
      while (index < n) {
        //printf("d_name: %s\n", namelist[index]->d_name);
        if (strstr(namelist[index]->d_name,"Fission") != NULL) {
          fileout<<namelist[index]->d_name<<endl;
        }
        free(namelist[index]);
        index++;
      }
      free(namelist);
    }
  }
  fileout.close();
}
//******************************************************************************
