////////////////////////////////////////////////
// TEvtBuilder.cpp: Class to perform the VME 
// reading and build the 'event' structure.
// Use TCrateCBLT to loop reading procedure, 
// and then build the event in memory, then 
// send it out to ethernet by using UDP 
// socket.
// Han Jianlong 07/2012
////////////////////////////////////////////////

#include <iostream>
using namespace std;
//#include "TUDPClientSocket.h"
#include "TEvtBuilder.h"
#include "TControl.h"
#include "TCrateCBLT.h"

#include "TString.h"
#include "TThread.h"


int TEvtBuilder::fnumcrates = 0;
int TEvtBuilder::fevent_counter = 0;
unsigned int TEvtBuilder::fnetbuf[NETBUFFER/4];

TEvtBuilder* onlyTEvtBuilder = 0;

TEvtBuilder::TEvtBuilder(TConfig &cod, std::vector<TCrateCBLT> &tcrate, unsigned int mastercrate
						 ):fconfig(cod), fcrate(tcrate)
{
	if(onlyTEvtBuilder)
	{
		cout<< "TEvtBuilder>> only one instance of TEvtBuilder allowed. " << endl;
		return;
	}
	onlyTEvtBuilder = this;

	fnumcrates = tcrate.size();
	fokstop = true;
	fmastercrate = 0;
	// search the index of 'mastercrate' in vector 'fcrate'
	for(unsigned int i=0; i<fcrate.size(); i++)
	{
		//cout << "Crate number: " << fcrate[i].GetCrateNum() << endl;
		if(fcrate[i].GetCrateNum() == mastercrate) fmastercrate = i;
	}
}

bool TEvtBuilder::CheckStop()
{
	bool check = false;
	TThread::Lock();
	check = (TControl::comm == kC_STOP);
	TThread::UnLock();
	return check;
}