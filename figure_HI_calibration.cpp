//______________________________________________________________________________
///////////////////////////////////////////////////////
///  definition of fitting function of 3He
double fit_He3(double *x, double *par)
{
  double consterm = par[2]*3*pow(2,2);
  double lineterm = x[0]+consterm;
  double logterm = consterm*log(lineterm/consterm);
  double light = par[0]+par[1]*(x[0]-logterm);
  return light;
}
//______________________________________________________________________________
//______________________________________________________________________________
///////////////////////////////////////////////////////
///  definition of fitting function of He4
double fit_He4(double *x, double *par)
{
  double consterm = par[2]*4*pow(2,2);
  double lineterm = x[0]+consterm;
  double logterm = consterm*log(lineterm/consterm);
  double light = par[0]+par[1]*(x[0]-logterm);
  return light;
}
//______________________________________________________________________________
//______________________________________________________________________________
///////////////////////////////////////////////////////
///  definition of fitting function of He6
double fit_He6(double *x, double *par)
{
  double consterm = par[2]*6*pow(2,2);
  double lineterm = x[0]+consterm;
  double logterm = consterm*log(lineterm/consterm);
  double light = par[0]+par[1]*(x[0]-logterm);
  return light;
}
//______________________________________________________________________________
//______________________________________________________________________________
///////////////////////////////////////////////////////
///  definition of fitting function of Li6
double fit_Li6(double *x, double *par)
{
  double consterm = par[2]*6*pow(3,2);
  double lineterm = x[0]+consterm;
  double logterm = consterm*log(lineterm/consterm);
  double light = par[0]+par[1]*(x[0]-logterm);
  return light;
}
//______________________________________________________________________________
//______________________________________________________________________________
///////////////////////////////////////////////////////
///  definition of fitting function of Li7
double fit_Li7(double *x, double *par)
{
  double consterm = par[2]*7*pow(3,2);
  double lineterm = x[0]+consterm;
  double logterm = consterm*log(lineterm/consterm);
  double light = par[0]+par[1]*(x[0]-logterm);
  return light;
}
//______________________________________________________________________________
//______________________________________________________________________________
///////////////////////////////////////////////////////
///  definition of fitting function of Li8
double fit_Li8(double *x, double *par)
{
  double consterm = par[2]*8*pow(3,2);
  double lineterm = x[0]+consterm;
  double logterm = consterm*log(lineterm/consterm);
  double light = par[0]+par[1]*(x[0]-logterm);
  return light;
}
//______________________________________________________________________________
//______________________________________________________________________________
///////////////////////////////////////////////////////
///  definition of fitting function of Be7
double fit_Be7(double *x, double *par)
{
  double consterm = par[2]*7*pow(4,2);
  double lineterm = x[0]+consterm;
  double logterm = consterm*log(lineterm/consterm);
  double light = par[0]+par[1]*(x[0]-logterm);
  return light;
}
//______________________________________________________________________________
//______________________________________________________________________________
///////////////////////////////////////////////////////
///  definition of fitting function of Be9
double fit_Be9(double *x, double *par)
{
  double consterm = par[2]*9*pow(4,2);
  double lineterm = x[0]+consterm;
  double logterm = consterm*log(lineterm/consterm);
  double light = par[0]+par[1]*(x[0]-logterm);
  return light;
}
//______________________________________________________________________________


//_______________________________________

int RetrieveCalibrationPoints(const char * file_name, int num_tel, int num_csi, double ** X, double ** Y, double ** errX, double ** errY)
{
    ifstream FileIn(file_name);
    if(!FileIn.is_open())
    {
      printf("Error: file%s not found\n", file_name);
      return 0;
    }
    
    std::vector <double> *CsIV = new std::vector<double>;
    std::vector <double> *errCsIV = new std::vector<double>;
    std::vector <double> *CsIE = new std::vector<double>;
    std::vector <double> *errCsIE = new std::vector<double>;
    
    const double EnergyCut = 150;
    
    int NRead=0;

    ////////////////////////////////////////////////////////////////////////////
    ///    Loop, to read each input data file
    while(!FileIn.eof())
    {
      std::string LineRead;
      std::getline(FileIn, LineRead);

      LineRead.assign(LineRead.substr(0, LineRead.find('*')));
      if(LineRead.empty()) continue;
      if(LineRead.find_first_not_of(' ')==std::string::npos) continue;

      std::istringstream LineStream(LineRead);

      int telnum;
      int csinum;
      double V;
      double errV;
      double E;
      double errE;

      LineStream >> telnum >> csinum >> V >> errV >> E >> errE; 
      
      if(E>EnergyCut) continue;
      
      if(telnum!=num_tel || num_csi!=csinum) continue;

      CsIV->push_back(V);
      errCsIV->push_back(errV);
      CsIE->push_back(E);
      errCsIE->push_back(errE);
      
      NRead++;
    }
    
    *X=CsIE->data();
    *Y=CsIV->data();
    *errX=errCsIE->data();
    *errY=errCsIV->data();
    
    return NRead;
}

int RetrieveFitParameters(const char * file_name, int num_tel, int num_csi, int Z, int A, double ** Parameters)
{
    ifstream FileIn(file_name);
    if(!FileIn.is_open())
    {
      printf("Error: file%s not found\n", file_name);
      return 0;
    }
        
    std::vector <double> *TheParamaterVector = new std::vector<double>;
    
    int NRead=0;

    ////////////////////////////////////////////////////////////////////////////
    ///    Loop, to read each input data file
    while(!FileIn.eof())
    {
      std::string LineRead;
      std::getline(FileIn, LineRead);
      
      LineRead.assign(LineRead.substr(0, LineRead.find('*')));
      if(LineRead.empty()) continue;
      if(LineRead.find_first_not_of(' ')==std::string::npos) continue;

      std::istringstream LineStream(LineRead);
      
      int telnum;
      int csinum;
      int Zread;
      int Aread;
      std::string Formula;
      int num_par;
      
      LineStream >> telnum >> csinum >> Zread >> Aread >> Formula >> num_par; 
                  
      if(telnum!=num_tel || num_csi!=csinum || Z!=Zread || A!=Aread) continue;

      for(int i=0; i< num_par; i++) {
        double AParameter;
	LineStream>>AParameter;
        TheParamaterVector->push_back(AParameter);
		
	NRead++;
      }  
    }
    
    *Parameters=TheParamaterVector->data();
    
    return NRead;
}

//_______________________________________

void figure_HI_calibration()
{
  TCanvas * c1 = new TCanvas ("c1","",800,600);
  
  const int NumTel=5;
  const int NumCsI=0;
     
  const Int_t NFiles_Z02_A04=2;
  std::string * FileIn_name_Z02_A04[NFiles_Z02_A04];
  FileIn_name_Z02_A04[0] = new std::string("/projects/hira/guan/HiRACsILightResponse/data/DEEPointsInVoltage_Z02_A04.dat"); //DEE points
  FileIn_name_Z02_A04[1] = new std::string("/projects/hira/guan/HiRACsILightResponse/data/WMUdataAddOnePecenterrEnergy_Z02_A04.dat");
  int NumPoints_Z02_A04[NFiles_Z02_A04];
  
  const Int_t NFiles_Z02_A06=1;
  std::string * FileIn_name_Z02_A06[NFiles_Z02_A06];
  FileIn_name_Z02_A06[0] = new std::string("/projects/hira/guan/HiRACsILightResponse/data/DEEPointsInVoltage_Z02_A06.dat"); //DEE points
  int NumPoints_Z02_A06[NFiles_Z02_A06];
  
  const Int_t NFiles_Z02_A03=1;
  std::string * FileIn_name_Z02_A03[NFiles_Z02_A03];
  FileIn_name_Z02_A03[0] = new std::string("/projects/hira/guan/HiRACsILightResponse/data/DEEPointsInVoltage_Z02_A03.dat"); //DEE points
  int NumPoints_Z02_A03[NFiles_Z02_A03];
  
  const Int_t NFiles_Z03_A06=1;
  std::string * FileIn_name_Z03_A06[NFiles_Z03_A06];
  FileIn_name_Z03_A06[0] = new std::string("/projects/hira/guan/HiRACsILightResponse/data/DEEPointsInVoltage_Z03_A06.dat"); //DEE points
  int NumPoints_Z03_A06[NFiles_Z03_A06];
  
  const Int_t NFiles_Z03_A07=1;
  std::string * FileIn_name_Z03_A07[NFiles_Z03_A07];
  FileIn_name_Z03_A07[0] = new std::string("/projects/hira/guan/HiRACsILightResponse/data/DEEPointsInVoltage_Z03_A07.dat"); //DEE points
  int NumPoints_Z03_A07[NFiles_Z03_A07];
  
  const Int_t NFiles_Z03_A08=1;
  std::string * FileIn_name_Z03_A08[NFiles_Z03_A08];
  FileIn_name_Z03_A08[0] = new std::string("/projects/hira/guan/HiRACsILightResponse/data/DEEPointsInVoltage_Z03_A08.dat"); //DEE points
  int NumPoints_Z03_A08[NFiles_Z03_A08];
  
  const Int_t NFiles_Z04_A07=1;
  std::string * FileIn_name_Z04_A07[NFiles_Z04_A07];
  FileIn_name_Z04_A07[0] = new std::string("/projects/hira/guan/HiRACsILightResponse/data/DEEPointsInVoltage_Z04_A07.dat"); //DEE points
  int NumPoints_Z04_A07[NFiles_Z04_A07];
  
  const Int_t NFiles_Z04_A09=1;
  std::string * FileIn_name_Z04_A09[NFiles_Z04_A09];
  FileIn_name_Z04_A09[0] = new std::string("/projects/hira/guan/HiRACsILightResponse/data/DEEPointsInVoltage_Z04_A09.dat"); //DEE points
  int NumPoints_Z04_A09[NFiles_Z04_A09];
  
  double * FitParameters_Z02_A03;
  double * FitParameters_Z02_A04;
  double * FitParameters_Z02_A06;
  double * FitParameters_Z03_A06;
  double * FitParameters_Z03_A07;
  double * FitParameters_Z03_A08;
  double * FitParameters_Z04_A07;
  double * FitParameters_Z04_A09;
  
  const int NumParameters_Z02_A03=RetrieveFitParameters("/projects/hira/guan/HiRACsILightResponse/calibrations/SimultaneouslyFitParHeavyIons.dat", NumTel, NumCsI, 2, 3, &FitParameters_Z02_A03);
  const int NumParameters_Z02_A04=RetrieveFitParameters("/projects/hira/guan/HiRACsILightResponse/calibrations/SimultaneouslyFitParHeavyIons.dat", NumTel, NumCsI, 2, 4, &FitParameters_Z02_A04);
  const int NumParameters_Z02_A06=RetrieveFitParameters("/projects/hira/guan/HiRACsILightResponse/calibrations/SimultaneouslyFitParHeavyIons.dat", NumTel, NumCsI, 2, 6, &FitParameters_Z02_A06);
  const int NumParameters_Z03_A06=RetrieveFitParameters("/projects/hira/guan/HiRACsILightResponse/calibrations/SimultaneouslyFitParHeavyIons.dat", NumTel, NumCsI, 3, 6, &FitParameters_Z03_A06);
  const int NumParameters_Z03_A07=RetrieveFitParameters("/projects/hira/guan/HiRACsILightResponse/calibrations/SimultaneouslyFitParHeavyIons.dat", NumTel, NumCsI, 3, 7, &FitParameters_Z03_A07);
  const int NumParameters_Z03_A08=RetrieveFitParameters("/projects/hira/guan/HiRACsILightResponse/calibrations/SimultaneouslyFitParHeavyIons.dat", NumTel, NumCsI, 3, 8, &FitParameters_Z03_A08);
  const int NumParameters_Z04_A07=RetrieveFitParameters("/projects/hira/guan/HiRACsILightResponse/calibrations/SimultaneouslyFitParHeavyIons.dat", NumTel, NumCsI, 4, 7, &FitParameters_Z04_A07);
  const int NumParameters_Z04_A09=RetrieveFitParameters("/projects/hira/guan/HiRACsILightResponse/calibrations/SimultaneouslyFitParHeavyIons.dat", NumTel, NumCsI, 4, 9, &FitParameters_Z04_A09);
  
  TF1 * Fit_Z02_A03 = new TF1 ("Fit_Z02_A03", fit_He3, 0, 2000, NumParameters_Z02_A03);
  TF1 * Fit_Z02_A04 = new TF1 ("Fit_Z02_A04", fit_He4, 0, 2000, NumParameters_Z02_A04);
  TF1 * Fit_Z02_A06 = new TF1 ("Fit_Z02_A06", fit_He6, 0, 2000, NumParameters_Z02_A06);
  TF1 * Fit_Z03_A06 = new TF1 ("Fit_Z03_A06", fit_Li6, 0, 2000, NumParameters_Z03_A06);
  TF1 * Fit_Z03_A07 = new TF1 ("Fit_Z03_A07", fit_Li7, 0, 2000, NumParameters_Z03_A07);
  TF1 * Fit_Z03_A08 = new TF1 ("Fit_Z03_A08", fit_Li8, 0, 2000, NumParameters_Z03_A08);
  TF1 * Fit_Z04_A07 = new TF1 ("Fit_Z04_A07", fit_Be7, 0, 2000, NumParameters_Z04_A07);
  TF1 * Fit_Z04_A09 = new TF1 ("Fit_Z04_A09", fit_Be9, 0, 2000, NumParameters_Z04_A09);
  Fit_Z02_A03->SetParameters(FitParameters_Z02_A03);
  Fit_Z02_A04->SetParameters(FitParameters_Z02_A04);
  Fit_Z02_A06->SetParameters(FitParameters_Z02_A06);
  Fit_Z03_A06->SetParameters(FitParameters_Z03_A06);
  Fit_Z03_A07->SetParameters(FitParameters_Z03_A07);
  Fit_Z03_A08->SetParameters(FitParameters_Z03_A08);
  Fit_Z04_A07->SetParameters(FitParameters_Z04_A07);
  Fit_Z04_A09->SetParameters(FitParameters_Z04_A09);
  
  double * CsIV_Z02_A03[NFiles_Z02_A03];
  double * CsIV_Z02_A04[NFiles_Z02_A04];
  double * CsIV_Z02_A06[NFiles_Z02_A06];
  double * CsIV_Z03_A06[NFiles_Z03_A06];
  double * CsIV_Z03_A07[NFiles_Z03_A07];
  double * CsIV_Z03_A08[NFiles_Z03_A08];
  double * CsIV_Z04_A07[NFiles_Z04_A07];
  double * CsIV_Z04_A09[NFiles_Z04_A09];
  
  double * CsIE_Z02_A03[NFiles_Z02_A03];
  double * CsIE_Z02_A04[NFiles_Z02_A04];
  double * CsIE_Z02_A06[NFiles_Z02_A06];
  double * CsIE_Z03_A06[NFiles_Z03_A06];
  double * CsIE_Z03_A07[NFiles_Z03_A07];
  double * CsIE_Z03_A08[NFiles_Z03_A08];
  double * CsIE_Z04_A07[NFiles_Z04_A07];
  double * CsIE_Z04_A09[NFiles_Z04_A09];
  
  double * errCsIV_Z02_A03[NFiles_Z02_A03];
  double * errCsIV_Z02_A04[NFiles_Z02_A04];
  double * errCsIV_Z02_A06[NFiles_Z02_A06];
  double * errCsIV_Z03_A06[NFiles_Z03_A06];
  double * errCsIV_Z03_A07[NFiles_Z03_A07];
  double * errCsIV_Z03_A08[NFiles_Z03_A08];
  double * errCsIV_Z04_A07[NFiles_Z04_A07];
  double * errCsIV_Z04_A09[NFiles_Z04_A09];
  
  double * errCsIE_Z02_A03[NFiles_Z02_A03];
  double * errCsIE_Z02_A04[NFiles_Z02_A04];
  double * errCsIE_Z02_A06[NFiles_Z02_A06];
  double * errCsIE_Z03_A06[NFiles_Z03_A06];
  double * errCsIE_Z03_A07[NFiles_Z03_A07];
  double * errCsIE_Z03_A08[NFiles_Z03_A08];
  double * errCsIE_Z04_A07[NFiles_Z04_A07];
  double * errCsIE_Z04_A09[NFiles_Z04_A09];
  
  
  TGraphErrors * Data_Z02_A03[NFiles_Z02_A03];
  TGraphErrors * Data_Z02_A04[NFiles_Z02_A04];
  TGraphErrors * Data_Z02_A06[NFiles_Z02_A06];
  TGraphErrors * Data_Z03_A06[NFiles_Z03_A06];
  TGraphErrors * Data_Z03_A07[NFiles_Z03_A07];
  TGraphErrors * Data_Z03_A08[NFiles_Z03_A08];
  TGraphErrors * Data_Z04_A07[NFiles_Z04_A07];
  TGraphErrors * Data_Z04_A09[NFiles_Z04_A09];
  
  TGraphErrors * Data_Z02_A03Insert[NFiles_Z02_A03];
  TGraphErrors * Data_Z02_A04Insert[NFiles_Z02_A04];
  TGraphErrors * Data_Z02_A06Insert[NFiles_Z02_A06];
  TGraphErrors * Data_Z03_A06Insert[NFiles_Z03_A06];
  TGraphErrors * Data_Z03_A07Insert[NFiles_Z03_A07];
  TGraphErrors * Data_Z03_A08Insert[NFiles_Z03_A08];
  TGraphErrors * Data_Z04_A07Insert[NFiles_Z04_A07];
  TGraphErrors * Data_Z04_A09Insert[NFiles_Z04_A09];
    
  TMultiGraph * Graph = new TMultiGraph();
  TMultiGraph * TheInsertGraph = new TMultiGraph ();
    
  //////////////////////////////////////////////////////////////////////////////
  ///  definition of the number of data points for each input file
  for(int FileNum=0; FileNum<NFiles_Z02_A03; FileNum++)
  {
    NumPoints_Z02_A03[FileNum] = RetrieveCalibrationPoints(FileIn_name_Z02_A03[FileNum]->c_str(), NumTel, NumCsI, &CsIE_Z02_A03[FileNum], &CsIV_Z02_A03[FileNum], &errCsIV_Z02_A03[FileNum], &errCsIV_Z02_A03[FileNum]);
    if(NumPoints_Z02_A03[FileNum]==0) continue;
    Data_Z02_A03[FileNum] = new TGraphErrors(NumPoints_Z02_A03[FileNum], CsIE_Z02_A03[FileNum], CsIV_Z02_A03[FileNum], errCsIV_Z02_A03[FileNum], errCsIV_Z02_A03[FileNum]);
    Data_Z02_A03[FileNum]->SetMarkerColor(1);
    Data_Z02_A03[FileNum]->SetLineColor(1);
    Data_Z02_A03[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z02_A03[FileNum]->SetMarkerSize(1.3);
    Data_Z02_A03Insert[FileNum] = new TGraphErrors(NumPoints_Z02_A03[FileNum], CsIE_Z02_A03[FileNum], CsIV_Z02_A03[FileNum], errCsIV_Z02_A03[FileNum], errCsIV_Z02_A03[FileNum]);
    Data_Z02_A03Insert[FileNum]->SetMarkerColor(1);
    Data_Z02_A03Insert[FileNum]->SetLineColor(1);
    Data_Z02_A03Insert[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z02_A03Insert[FileNum]->SetMarkerSize(1.);
    Graph->Add(Data_Z02_A03[FileNum]);
    TheInsertGraph->Add(Data_Z02_A03Insert[FileNum]);
  } 
  for(int FileNum=0; FileNum<NFiles_Z02_A04; FileNum++)
  {
    NumPoints_Z02_A04[FileNum] = RetrieveCalibrationPoints(FileIn_name_Z02_A04[FileNum]->c_str(), NumTel, NumCsI, &CsIE_Z02_A04[FileNum], &CsIV_Z02_A04[FileNum], &errCsIV_Z02_A04[FileNum], &errCsIV_Z02_A04[FileNum]);
    if(NumPoints_Z02_A04[FileNum]==0) continue;
    Data_Z02_A04[FileNum] = new TGraphErrors(NumPoints_Z02_A04[FileNum], CsIE_Z02_A04[FileNum], CsIV_Z02_A04[FileNum], errCsIV_Z02_A04[FileNum], errCsIV_Z02_A04[FileNum]);
    Data_Z02_A04[FileNum]->SetMarkerColor(2);
    Data_Z02_A04[FileNum]->SetLineColor(2);
    Data_Z02_A04[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z02_A04[FileNum]->SetMarkerSize(1.3);
    Data_Z02_A04Insert[FileNum] = new TGraphErrors(NumPoints_Z02_A04[FileNum], CsIE_Z02_A04[FileNum], CsIV_Z02_A04[FileNum], errCsIV_Z02_A04[FileNum], errCsIV_Z02_A04[FileNum]);
    Data_Z02_A04Insert[FileNum]->SetMarkerColor(2);
    Data_Z02_A04Insert[FileNum]->SetLineColor(2);
    Data_Z02_A04Insert[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z02_A04Insert[FileNum]->SetMarkerSize(1.);
    Graph->Add(Data_Z02_A04[FileNum]);
    TheInsertGraph->Add(Data_Z02_A04Insert[FileNum]);
  } 
  for(int FileNum=0; FileNum<NFiles_Z02_A06; FileNum++)
  {
    NumPoints_Z02_A06[FileNum] = RetrieveCalibrationPoints(FileIn_name_Z02_A06[FileNum]->c_str(), NumTel, NumCsI, &CsIE_Z02_A06[FileNum], &CsIV_Z02_A06[FileNum], &errCsIV_Z02_A06[FileNum], &errCsIV_Z02_A06[FileNum]);
    if(NumPoints_Z02_A06[FileNum]==0) continue;
    Data_Z02_A06[FileNum] = new TGraphErrors(NumPoints_Z02_A06[FileNum], CsIE_Z02_A06[FileNum], CsIV_Z02_A06[FileNum], errCsIV_Z02_A06[FileNum], errCsIV_Z02_A06[FileNum]);
    Data_Z02_A06[FileNum]->SetMarkerColor(3);
    Data_Z02_A06[FileNum]->SetLineColor(3);
    Data_Z02_A06[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z02_A06[FileNum]->SetMarkerSize(1.3);
    Data_Z02_A06Insert[FileNum] = new TGraphErrors(NumPoints_Z02_A06[FileNum], CsIE_Z02_A06[FileNum], CsIV_Z02_A06[FileNum], errCsIV_Z02_A06[FileNum], errCsIV_Z02_A06[FileNum]);
    Data_Z02_A06Insert[FileNum]->SetMarkerColor(3);
    Data_Z02_A06Insert[FileNum]->SetLineColor(3);
    Data_Z02_A06Insert[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z02_A06Insert[FileNum]->SetMarkerSize(1.);
    Graph->Add(Data_Z02_A06[FileNum]);
    TheInsertGraph->Add(Data_Z02_A06Insert[FileNum]);
  } 
  for(int FileNum=0; FileNum<NFiles_Z03_A06; FileNum++)
  {
    NumPoints_Z03_A06[FileNum] = RetrieveCalibrationPoints(FileIn_name_Z03_A06[FileNum]->c_str(), NumTel, NumCsI, &CsIE_Z03_A06[FileNum], &CsIV_Z03_A06[FileNum], &errCsIV_Z03_A06[FileNum], &errCsIV_Z03_A06[FileNum]);
    if(NumPoints_Z03_A06[FileNum]==0) continue;
    Data_Z03_A06[FileNum] = new TGraphErrors(NumPoints_Z03_A06[FileNum], CsIE_Z03_A06[FileNum], CsIV_Z03_A06[FileNum], errCsIV_Z03_A06[FileNum], errCsIV_Z03_A06[FileNum]);
    Data_Z03_A06[FileNum]->SetMarkerColor(4);
    Data_Z03_A06[FileNum]->SetLineColor(4);
    Data_Z03_A06[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z03_A06[FileNum]->SetMarkerSize(1.3);
    Data_Z03_A06Insert[FileNum] = new TGraphErrors(NumPoints_Z03_A06[FileNum], CsIE_Z03_A06[FileNum], CsIV_Z03_A06[FileNum], errCsIV_Z03_A06[FileNum], errCsIV_Z03_A06[FileNum]);
    Data_Z03_A06Insert[FileNum]->SetMarkerColor(4);
    Data_Z03_A06Insert[FileNum]->SetLineColor(4);
    Data_Z03_A06Insert[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z03_A06Insert[FileNum]->SetMarkerSize(1.);
    Graph->Add(Data_Z03_A06[FileNum]);
    TheInsertGraph->Add(Data_Z03_A06Insert[FileNum]);
  } 
  for(int FileNum=0; FileNum<NFiles_Z03_A07; FileNum++)
  {
    NumPoints_Z03_A07[FileNum] = RetrieveCalibrationPoints(FileIn_name_Z03_A07[FileNum]->c_str(), NumTel, NumCsI, &CsIE_Z03_A07[FileNum], &CsIV_Z03_A07[FileNum], &errCsIV_Z03_A07[FileNum], &errCsIV_Z03_A07[FileNum]);
    if(NumPoints_Z03_A07[FileNum]==0) continue;
    Data_Z03_A07[FileNum] = new TGraphErrors(NumPoints_Z03_A07[FileNum], CsIE_Z03_A07[FileNum], CsIV_Z03_A07[FileNum], errCsIV_Z03_A07[FileNum], errCsIV_Z03_A07[FileNum]);
    Data_Z03_A07[FileNum]->SetMarkerColor(6);
    Data_Z03_A07[FileNum]->SetLineColor(6);
    Data_Z03_A07[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z03_A07[FileNum]->SetMarkerSize(1.3);
    Data_Z03_A07Insert[FileNum] = new TGraphErrors(NumPoints_Z03_A07[FileNum], CsIE_Z03_A07[FileNum], CsIV_Z03_A07[FileNum], errCsIV_Z03_A07[FileNum], errCsIV_Z03_A07[FileNum]);
    Data_Z03_A07Insert[FileNum]->SetMarkerColor(6);
    Data_Z03_A07Insert[FileNum]->SetLineColor(6);
    Data_Z03_A07Insert[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z03_A07Insert[FileNum]->SetMarkerSize(1.);
    Graph->Add(Data_Z03_A07[FileNum]);
    TheInsertGraph->Add(Data_Z03_A07Insert[FileNum]);
  } 
  for(int FileNum=0; FileNum<NFiles_Z03_A08; FileNum++)
  {
    NumPoints_Z03_A08[FileNum] = RetrieveCalibrationPoints(FileIn_name_Z03_A08[FileNum]->c_str(), NumTel, NumCsI, &CsIE_Z03_A08[FileNum], &CsIV_Z03_A08[FileNum], &errCsIV_Z03_A08[FileNum], &errCsIV_Z03_A08[FileNum]);
    if(NumPoints_Z03_A08[FileNum]==0) continue;
    Data_Z03_A08[FileNum] = new TGraphErrors(NumPoints_Z03_A08[FileNum], CsIE_Z03_A08[FileNum], CsIV_Z03_A08[FileNum], errCsIV_Z03_A08[FileNum], errCsIV_Z03_A08[FileNum]);
    Data_Z03_A08[FileNum]->SetMarkerColor(7);
    Data_Z03_A08[FileNum]->SetLineColor(7);
    Data_Z03_A08[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z03_A08[FileNum]->SetMarkerSize(1.3);
    Data_Z03_A08Insert[FileNum] = new TGraphErrors(NumPoints_Z03_A08[FileNum], CsIE_Z03_A08[FileNum], CsIV_Z03_A08[FileNum], errCsIV_Z03_A08[FileNum], errCsIV_Z03_A08[FileNum]);
    Data_Z03_A08Insert[FileNum]->SetMarkerColor(7);
    Data_Z03_A08Insert[FileNum]->SetLineColor(7);
    Data_Z03_A08Insert[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z03_A08Insert[FileNum]->SetMarkerSize(1.);
    Graph->Add(Data_Z03_A08[FileNum]);
    TheInsertGraph->Add(Data_Z03_A08Insert[FileNum]);
  } 
  for(int FileNum=0; FileNum<NFiles_Z04_A07; FileNum++)
  {
    NumPoints_Z04_A07[FileNum] = RetrieveCalibrationPoints(FileIn_name_Z04_A07[FileNum]->c_str(), NumTel, NumCsI, &CsIE_Z04_A07[FileNum], &CsIV_Z04_A07[FileNum], &errCsIV_Z04_A07[FileNum], &errCsIV_Z04_A07[FileNum]);
    if(NumPoints_Z04_A07[FileNum]==0) continue;
    Data_Z04_A07[FileNum] = new TGraphErrors(NumPoints_Z04_A07[FileNum], CsIE_Z04_A07[FileNum], CsIV_Z04_A07[FileNum], errCsIV_Z04_A07[FileNum], errCsIV_Z04_A07[FileNum]);
    Data_Z04_A07[FileNum]->SetMarkerColor(9);
    Data_Z04_A07[FileNum]->SetLineColor(9);
    Data_Z04_A07[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z04_A07[FileNum]->SetMarkerSize(1.3);
    Data_Z04_A07Insert[FileNum] = new TGraphErrors(NumPoints_Z04_A07[FileNum], CsIE_Z04_A07[FileNum], CsIV_Z04_A07[FileNum], errCsIV_Z04_A07[FileNum], errCsIV_Z04_A07[FileNum]);
    Data_Z04_A07Insert[FileNum]->SetMarkerColor(9);
    Data_Z04_A07Insert[FileNum]->SetLineColor(9);
    Data_Z04_A07Insert[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z04_A07Insert[FileNum]->SetMarkerSize(1.);
    Graph->Add(Data_Z04_A07[FileNum]);
    TheInsertGraph->Add(Data_Z04_A07Insert[FileNum]);
  } 
  for(int FileNum=0; FileNum<NFiles_Z04_A09; FileNum++)
  {
    NumPoints_Z04_A09[FileNum] = RetrieveCalibrationPoints(FileIn_name_Z04_A09[FileNum]->c_str(), NumTel, NumCsI, &CsIE_Z04_A09[FileNum], &CsIV_Z04_A09[FileNum], &errCsIV_Z04_A09[FileNum], &errCsIV_Z04_A09[FileNum]);
    if(NumPoints_Z04_A09[FileNum]==0) continue;
    Data_Z04_A09[FileNum] = new TGraphErrors(NumPoints_Z04_A09[FileNum], CsIE_Z04_A09[FileNum], CsIV_Z04_A09[FileNum], errCsIV_Z04_A09[FileNum], errCsIV_Z04_A09[FileNum]);
    Data_Z04_A09[FileNum]->SetMarkerColor(11);
    Data_Z04_A09[FileNum]->SetLineColor(11);
    Data_Z04_A09[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z04_A09[FileNum]->SetMarkerSize(1.3);
    Data_Z04_A09Insert[FileNum] = new TGraphErrors(NumPoints_Z04_A09[FileNum], CsIE_Z04_A09[FileNum], CsIV_Z04_A09[FileNum], errCsIV_Z04_A09[FileNum], errCsIV_Z04_A09[FileNum]);
    Data_Z04_A09Insert[FileNum]->SetMarkerColor(11);
    Data_Z04_A09Insert[FileNum]->SetLineColor(11);
    Data_Z04_A09Insert[FileNum]->SetMarkerStyle(25+FileNum);
    Data_Z04_A09Insert[FileNum]->SetMarkerSize(1.);
    Graph->Add(Data_Z04_A09[FileNum]);
    TheInsertGraph->Add(Data_Z04_A09Insert[FileNum]);
  } 
              
  //Drawing
  Graph->Draw("APE");
  TAxis * XAxis = Graph->GetXaxis();
  TAxis * YAxis = Graph->GetYaxis();
  XAxis->SetLabelSize(0.05);
  XAxis->SetTitleSize(0.05);
  XAxis->CenterTitle(true);
  YAxis->SetLabelSize(0.05);
  YAxis->SetTitleSize(0.05);
  YAxis->CenterTitle(true); 
  XAxis->SetTitle("E (MeV)");
  YAxis->SetTitle("Light Output (V)");
  YAxis->SetNdivisions(6,5,0);
  XAxis->SetLimits(0,190);
  
  Fit_Z02_A03->SetLineColor(1);
  Fit_Z02_A04->SetLineColor(2);
  Fit_Z02_A06->SetLineColor(3);
  Fit_Z03_A06->SetLineColor(4);
  Fit_Z03_A07->SetLineColor(6);
  Fit_Z03_A08->SetLineColor(7);
  Fit_Z04_A07->SetLineColor(9);
  Fit_Z04_A09->SetLineColor(11);
  Fit_Z02_A03->SetLineWidth(2);
  Fit_Z02_A04->SetLineWidth(2);
  Fit_Z02_A06->SetLineWidth(2);
  Fit_Z03_A06->SetLineWidth(2);
  Fit_Z03_A07->SetLineWidth(2);
  Fit_Z03_A08->SetLineWidth(2);
  Fit_Z04_A07->SetLineWidth(2);
  Fit_Z04_A09->SetLineWidth(2);          
  
  Fit_Z02_A03->Draw("SAME");
  Fit_Z02_A04->Draw("SAME");
  Fit_Z02_A06->Draw("SAME");
  Fit_Z03_A06->Draw("SAME");
  Fit_Z03_A07->Draw("SAME");
  Fit_Z03_A08->Draw("SAME");
  Fit_Z04_A07->Draw("SAME");
  Fit_Z04_A09->Draw("SAME");
  
  TLegend * legendario = new TLegend(0.12,0.50,0.4,0.85);
  legendario->SetBorderSize(0);
  if(NumPoints_Z02_A03[0]) legendario->AddEntry(Data_Z02_A03[0],"^{3}He ELoss Table","P");
  if(NumPoints_Z02_A04[0]) legendario->AddEntry(Data_Z02_A04[0],"^{4}He WMU Exp","P");
  if(NumPoints_Z02_A04[1]) legendario->AddEntry(Data_Z02_A04[1],"^{4}He ELoss Table","P");
  if(NumPoints_Z02_A06[0]) legendario->AddEntry(Data_Z02_A06[0],"^{6}He ELoss Table","P");
  if(NumPoints_Z03_A06[0]) legendario->AddEntry(Data_Z03_A06[0],"^{6}Li ELoss Table","P");
  if(NumPoints_Z03_A07[0]) legendario->AddEntry(Data_Z03_A07[0],"^{7}Li ELoss Table","P");
  if(NumPoints_Z03_A08[0]) legendario->AddEntry(Data_Z03_A08[0],"^{8}Li ELoss Table","P");
  if(NumPoints_Z04_A07[0]) legendario->AddEntry(Data_Z04_A07[0],"^{7}Be ELoss Table","P");
  if(NumPoints_Z04_A09[0]) legendario->AddEntry(Data_Z04_A09[0],"^{9}Be ELoss Table","P");

  legendario->Draw("SAME");
  
  gPad->RedrawAxis();
  
  TPad * TheInsert = new TPad("TheInsert", "", 0.60,0.11,0.89,0.49);
  TheInsert->Draw("SAME");
  TheInsert->cd();
  TheInsertGraph->Draw("APE");
  TAxis * XAxisInsert = TheInsertGraph->GetXaxis();
  TAxis * YAxisInsert = TheInsertGraph->GetYaxis();
  XAxisInsert->SetLabelSize(0.08);
  XAxisInsert->SetTitleSize(0.08);
  XAxisInsert->CenterTitle(true);
  YAxisInsert->SetLabelSize(0.08);
  YAxisInsert->SetTitleSize(0.08);
  YAxisInsert->CenterTitle(true); 
  YAxisInsert->SetNdivisions(6,1,0);
  XAxisInsert->SetNdivisions(6,1,0);
  XAxisInsert->SetLimits(0,25);
  YAxisInsert->SetRangeUser(0,0.13);
  
  TLine * discr_line = new TLine (15,0,15,0.13);
  discr_line->SetLineStyle(2);
  discr_line->Draw("SAME");
  
  TLatex * wmu_label = new TLatex (2,0.113,"WMU data");
  wmu_label->SetTextColor(kOrange+2);
  wmu_label->SetTextSize(0.08);
  wmu_label->SetTextFont(42);
  wmu_label->Draw("SAME");
  
  TPave * shadowed_region = new TPave(0,0,15,0.13);
  shadowed_region->SetBorderSize(0);
  shadowed_region->SetFillColorAlpha(kYellow-9,0.30);
  shadowed_region->Draw("SAME");
  
  Fit_Z02_A03->Draw("SAME");
  Fit_Z02_A04->Draw("SAME");
  Fit_Z02_A06->Draw("SAME");
  Fit_Z03_A06->Draw("SAME");
  Fit_Z03_A07->Draw("SAME");
  Fit_Z03_A08->Draw("SAME");
  Fit_Z04_A07->Draw("SAME");
  Fit_Z04_A09->Draw("SAME");
  
  gPad->RedrawAxis();
  
  c1->Print("figure_HI_calibration.png");
  c1->Print("figure_HI_calibration.pdf");
  
}
