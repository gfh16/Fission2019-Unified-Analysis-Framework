////////////////////////////////////////////////
// TEvtBuilder.h: Class to perform the VME 
// reading and build the 'event' structure.
// Use TCrateCBLT to loop reading procedure, 
// and then build the event in memory, then 
// send it out to ethernet by using UDP 
// socket.
// Han Jianlong 07/2012
////////////////////////////////////////////////

#ifndef TEvtBuilder_H
#define TEvtBuilder_H
#include <vector>
using namespace std;

#include <RQ_OBJECT.h>

#include "caenacq.h"
#include "TConfig.h"
#include "TCrateCBLT.h"

class TEvtBuilder {

protected:
	TConfig &fconfig;                      //reference to TConfig class
	vector <TCrateCBLT> &fcrate;
	static unsigned int fnetbuf[NETBUFFER/4];

public:
	static int fnumcrates;
	static int fevent_counter;
	unsigned int fmastercrate;             // the 'fcrate' vector index (0 is the first element of tcrate vector)
	bool fokstop;                          // stop the acquisition ok or not

public:
	TEvtBuilder(TConfig &cod, vector <TCrateCBLT> &tcrate, unsigned int mastercrate);
	virtual ~TEvtBuilder() {}
	static bool CheckStop();
	virtual int CheckErrors(string, int ) = 0;
	vector <TCrateCBLT> & GetCrateCBLT() {return fcrate;}
	TConfig& GetConfig(){return fconfig;}

protected:
	int GetMask();
};

extern TEvtBuilder* onlyTEvtBuilder;

#endif //#ifndef TEvtBuilder_H