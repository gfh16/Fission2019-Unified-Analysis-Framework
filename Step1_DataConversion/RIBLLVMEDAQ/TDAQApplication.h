////////////////////////////////////////////////////////
//  This class implements a 
//  server readout Application environment
//  to manage command line options, directory files, etc.
//  This class must be instantiated exactly once in one 
//  daq server application. Credits: root.cern.ch
//  E.d.F (c) 3/2008  v. 012 rev. 4/2008
//  Chimera Acquisition 
//  ver 0.2 : added "localhost" environment for singal PC
//            by Han jianlong (2013.06.16)
/////////////////////////////////////////////////////////
#ifndef TDAQAPPLICATION_H
#define TDAQAPPLICATION_H

#include <string>
using namespace std;
#include "caenacq.h"

const string gMULTICAST = "225.0.0.1";        //multicast UDP address (def.) 
//const string gConfPath = "DAQCONFPATH";     //environment file PATH for setup files

class TDAQApplication {
private:
	int  fargc;       //Number of command line arguments
	char **fargv;     //Command line arguments 
	bool fmaster;     //This application is the DAQ master (default=true) 
	string fcomm;     //Communication mode (unicast,multicast,broadcast)
	string fmaddr;    //Multicast address (default=gMULTICAST=255.0.0.1)
	string fpathname; //The environment variable ACQConfPATH (if defined)
	string fserverip; //server ip, or "localhost"
public:
	TDAQApplication(int argc, char **argv, bool ismaster=true);
	~TDAQApplication();
	void GetOptions(int, char **);
	void Usage(char *);
	int Argc() {return fargc;}
	char **Argv() {return fargv;}
	char *Argv(int i) {return fargv ? fargv[i] : 0; }
	string GetComm() {return fcomm;}
	void SetMulticastAddress(string addr=gMULTICAST) {fmaddr=addr;}
	string GetMulticastAddress() {return fmaddr;}
	void Version();
	string Get_PathEnvdir() {return fpathname;}
	bool Get_ServerIP(string &sip);
	bool IsMaster() {return fmaster;}
};

extern TDAQApplication *gDAQApplication;
#endif
