/////////////////////////////////////////////////////////
// File name: TDAQPCTask.cpp                           //
// Brief introduction:                                 //
//       DAQ PC task: Read data form VME modules, send //
//       data to UDP broadcast, receive command from   //
//       master PC
// Version: V1.0                                       //
// Author: Han Jianlong                                //
// Date: Aug. 2012                                     //
// For: RIBLL1                                         //
/////////////////////////////////////////////////////////

#include <vector>
#include <string>
#include <exception>
#include <iostream>
#include "stdlib.h"
using namespace std;

#include "TDAQPCTask.h"
#include "TClientEvtBuilder.h"
#include "TControl.h"
#include "TThread.h"
#include "TConfig.h"
#include "TCrateCBLT.h"
#include "caenacq.h"

TDAQPCTask::TDAQPCTask(const char *ipaddr, TConfig &cod, std::vector<TCrateCBLT> &tcrate, unsigned int mastercrate)
{
	try
	{
		DAQPCControl = new TControl(ipaddr, DAQ_PC);
		//eventbuilder = new TClientEvtBuilder(ipaddr, "BROADCAST", cod, tcrate, mastercrate);
		//eventbuilder = new TClientEvtBuilder(MULTICAST_IP.c_str(), "MULTICAST", cod, tcrate, mastercrate);
		string path = getenv(gConfPath.c_str());
		path +=  IPLISTF;
		eventbuilder = new TClientEvtBuilder(path.c_str(), "IPLIST", cod, tcrate, mastercrate);
	}
	catch(bad_alloc &e)
	{
		cout << "TDAQPCTask>> " << e.what() << endl;
	}
	DAQPCControl->Connect("MSignalEmit()", "TClientEvtBuilder", eventbuilder, "ProcessStopSignal()");
}

TDAQPCTask::~TDAQPCTask()
{
	delete eventbuilder;
	delete DAQPCControl;
}

TThread *TDAQPCTask::StartDAQPCControlThread()
{
	DAQPCControl->StartDaqPCThread();
	return DAQPCControl->GetDaqPCThread();
}

TThread *TDAQPCTask::StartDAQThread()
{
	eventbuilder->StartDAQThread();
	return eventbuilder->GetDAQThread();
}
