///////////////////////////////////////////////
// MonOnline.cpp: main() of MonOnline, used
// for online Monitoring, show histograms and
// trees(if defined)
// Han Jianlong (08/2012)
///////////////////////////////////////////////

#include "TApplication.h"
#include "TRint.h"

#include "TDAQApplication.h"
#include "TOnline.h"
#include "TConfig.h"
#include "TDataAnalyser.h"


int main(int argc, char *argv[])
{
	TRint apponline("Online", &argc, argv);
	
	// The Server DAQ Application defining general environment
	TDAQApplication ribll(argc, argv, false);
	//where the cblt setup files are
	string pathchain = ribll.Get_PathEnvdir() + "/cblt_addr_crate";
	//where the filenameqdc and codifier setup file are
	string pathfilename = ribll.Get_PathEnvdir() + "/filenamemod.dat";
	
	TConfig conf(pathfilename.c_str());
	TDataAnalyser anadata = TDataAnalyser(&conf);

	TOnline *onl = new TOnline(gClient->GetRoot(), 100, 200, &anadata);


	apponline.Run();
	return 0;
}