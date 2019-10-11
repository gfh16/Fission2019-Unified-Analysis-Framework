///////////////////////////////////////////////////////////////
//  TDAQApplication class interface 
//  ver 0.2 : added "localhost" environment for singal PC
//            by Han jianlong (2013.06.16)
///////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <iostream>
#include <cstring>
using namespace std;

#include "TDAQApplication.h"
#include "TString.h"

TDAQApplication *gDAQApplication=0;
const char gversion[] = "0.1beta";

TDAQApplication::TDAQApplication(int argc, char **argv, bool master)
{
	if (gDAQApplication) {
		cout<<"TDAQApplication>> only one instance of TDAQApplication allowed";
		return;
	}
	fmaster = master;
	if(argc>0) Version();
	fmaddr= gMULTICAST;
	fcomm = "UNICAST";
	gDAQApplication = this;
	if(argc>0) {
		fargc = argc;
		fargv = (char **)new char*[fargc];
	} 
	else {
		fargc = 0;
		fargv = 0;
	}

	for (int i = 0; i<fargc; i++) {
		fargv[i] = new char[strlen(argv[i])+1];
		strcpy(fargv[i], argv[i]);
	}

	if(argc>0)
		GetOptions(argc,argv);

	// obtain th enviroment variable value for gConfPath 
	char *env = getenv(gConfPath.c_str());
	if(env==NULL) {
		fpathname = ".";
	}
	else {
		fpathname = env;
	}
	
	char *sip = getenv(gServerIP.c_str());
	if(sip == NULL)
	{
		fserverip = "localhost";
	}
	else
	{
		string csip(sip);
		if(csip.size()>0) fserverip = sip;
	}
}


void TDAQApplication::GetOptions(int argc, char **argv)
{
	char *cp;
	char *name = argv[0];

	argc--;
	argv++;

	while(argc>0 && **argv == '-') {
		cp=*argv + 1;
		switch(*cp) {
	case 'm':
		fcomm = "MULTICAST";
		break;

	case 'u':
		fcomm = "UNICAST";
		break;

	//case 'b':
	//	fcomm = "BROADCAST";
	//	break;

	case 'h':
		Usage(name);
		exit(0);

	default:
		cout<<"TDAQApplication>> Unknown option, default values used"<<endl;
		break;
		}

		argc--;
		argv++;
	}             
}

bool TDAQApplication::Get_ServerIP(std::string &sip)
{
	if(fserverip.size() == 0) return false;

	if(fserverip.compare("localhost") == 0)
	{
		sip = "localhost";
		return true;
	}

	TString ipcheck(fserverip);
	if(ipcheck.IsWhitespace()) return false;
	ipcheck.ReplaceAll(" ", "");
	if(ipcheck.Length()>15) return false;
	if(ipcheck.Contains(".") && ipcheck.CountChar('.')<3) return false;
	ipcheck.ReplaceAll(".", "");
	if(!ipcheck.IsDigit()) return false;
	sip = fserverip.c_str();
	return true;

}

TDAQApplication::~TDAQApplication()
{
	for (int i = 0; i<fargc; i++)
		if (fargv[i]) delete [] fargv[i];
	delete [] fargv;
}

void TDAQApplication::Usage(char *name)
{
	cout<<"Usage: "<<name<<" [-m|-b|-u] -h"<<endl;
	cout<<"\t -m: enable multicast"<<endl;
	cout<<"\t -b: enable broadcast"<<endl;
	cout<<"\t -u: unicast (point to point connection)"<<endl;
	cout<<"\t -h: this help"<<endl;
	cout<<endl;
}


void TDAQApplication::Version()
{
	cout<<"*********************************************"<<endl;
	cout<<" DAQ ver. "<<gversion<<" RIBLL ACQUISITION   "<<endl;
	cout<<" readout program for RIBLL "<<endl;
	cout<<" based upon ROOT v5.34 or later "<<endl;
	cout<<" DAQ for Caen V2718 PCI-VME optical bridge      "<<endl;
	cout<<" Han jianlong (2012)      "<<endl;
	cout<<"*********************************************\n\n";
}

