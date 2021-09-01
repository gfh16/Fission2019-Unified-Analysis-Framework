#include "../include/ReadFileModule.h"
using namespace std;


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
ReadFileModule::ReadFileModule()
{
  //cout<<"ReadFileModule Loaded!"<<endl;
}

//______________________________________________________________________________
ReadFileModule::~ReadFileModule()
{
  //cout<<"ReadFileModule Deleted!"<<endl;
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
Double_t*** ReadFileModule::ReadData(const Char_t* datapath, Int_t SSDNum,
            Int_t CHNum, Int_t ParNum)
{
  ifstream in;
  in.open(datapath);
  if(!in.is_open()) {
    printf("Error: file %s not found\n",datapath);
    return NULL;
  }
  Double_t*** readpar = NULL;
  readpar = new Double_t** [SSDNum];
  for(Int_t i=0; i<SSDNum; i++) {
    readpar[i] = new Double_t* [CHNum];
    for(Int_t j=0; j<CHNum; j++) {
      readpar[i][j]= new Double_t[ParNum];
    }
  }
  while(in.good()) {
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
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
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
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void ReadFileModule::AddChain(TChain* chain, const char* LayerTag, Int_t* dataarray, Int_t size, Int_t index)
{
  for (Int_t j=0; j<size; j++) {
    dataarray[j] = 0;
  }
  std::string SSD_E_bname;
  SSD_E_bname = Form("SSD%d_%s", index+1, LayerTag); // e.g. SSD2_L2F_E, SSD2_L2F_T
  chain->SetBranchStatus(SSD_E_bname.c_str(), true);
  chain->SetBranchAddress(SSD_E_bname.c_str(), dataarray);
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void ReadFileModule::GetFileNamesFromFolder(const char* pathfolder, const char* pathfileout)
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
        if ((strstr(namelist[index]->d_name,"Fission") != NULL) || (strstr(namelist[index]->d_name,"Map") != NULL)) {
          fileout<<namelist[index]->d_name<<endl;
        }
        free(namelist[index]);
        index++;
      }
      free(namelist);
    }
  }
  fileout.close();
  printf("文件 %s 已生成!\n", pathfileout);
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void ReadFileModule::GetFileNamesFromFile(const char* pathfilein, vector<string>& filelist)
{
  std::string filename;

  ifstream filein(pathfilein);

  if (!filein.is_open()) {
    printf("Error: file %s not found\n", pathfilein);
    return;
  }
  //_______________________________________
  //      按行读取文件中的每一个文件名
  while (filein.good()) {
    std::string LineRead;
    std::getline(filein, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

    LineStream>>filename;
    filelist.push_back(filename);
  }
  printf("文件 %s 已读取完成!\n", pathfilein);
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
Bool_t ReadFileModule::IsFileExists(const std::string& filename)
{
  ifstream filein(filename.c_str());
  return filein.good();
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void ReadFileModule::CsICaliLoadPars(std::vector<Double_t> parlist[NUM_SSD*NUM_CSI], const char* pathFarsFile, Int_t npars)
{
  Int_t numtel;
  Int_t ssdnum, csinum;
  Double_t pars[npars];

  // 按行读取 CsI 能量拟合参数
	ifstream FileInPars(pathFarsFile);

	if (!FileInPars.is_open()) {
		printf("Error: file %s not found\n",pathFarsFile);
    return;
	}
	while (FileInPars.good()) {
		// 按行读取数据
    std::string LineRead;
    std::getline(FileInPars, LineRead);
    LineRead.assign(LineRead.substr(0, LineRead.find('*')));
    if(LineRead.empty()) continue;
    if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
    std::istringstream LineStream(LineRead);

		LineStream>>numtel>>ssdnum>>csinum;
    for (Int_t i=0; i<npars; i++)  {
      LineStream>>pars[i];
      parlist[numtel].push_back(pars[i]);
    }
	}
	FileInPars.close();
}


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
Double_t** ReadFileModule::CsICaliLoadPars2(const char* pathFarsFile, Int_t npars)
{
  Int_t numtel;
  Int_t ssdnum, csinum;
  Double_t pars[npars];

  Double_t** fvpar = NULL;
	fvpar = new Double_t* [NUM_SSD*NUM_CSI]; // four Si-Si-CsI telescopes, 36 CsI crystals in total
  for (Int_t i=0; i<NUM_SSD*NUM_CSI; i++) {
    fvpar[i] = new Double_t[npars];
	}

  ifstream filein(pathFarsFile, ios::in);

  if (!filein.is_open()) {
		printf("Error: file %s not found\n",pathFarsFile);
    return NULL;
	}

  if (filein) {
    while(!filein.eof()) {
			// 这里的按行读取数据的方式与 DEEFIT 元代码稍有不同！ 但不影响正常使用！
			std::string LineRead;
      std::getline(filein, LineRead);
			LineRead.assign(LineRead.substr(0, LineRead.find('*')));
      if(LineRead.empty()) continue;
      if(LineRead.find_first_not_of(' ')==std::string::npos) continue;
      std::istringstream LineStream(LineRead);

      LineStream>>numtel>>ssdnum>>csinum;

      if(!LineStream.fail()) {
        for(Int_t ipar=0; ipar<npars; ipar++) {
          LineStream>>pars[ipar];
          fvpar[numtel][ipar] = pars[ipar];
        }
      }
    }
    filein.close();
  }
  return fvpar;
}
