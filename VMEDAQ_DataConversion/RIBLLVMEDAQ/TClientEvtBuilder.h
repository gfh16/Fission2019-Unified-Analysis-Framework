////////////////////////////////////////////////
// TEvtBuilder.h: Class to perform the VME 
// reading and build the 'event' structure.
// Use TCrateCBLT to loop reading procedure, 
// and then build the event in memory, then 
// send it out to ethernet by using UDP 
// socket.
// Han Jianlong 07/2012
////////////////////////////////////////////////

#ifndef TClientEvtBuilder_H
#define TClientEvtBuilder_H

#include "TEvtBuilder.h" 
#include <RQ_OBJECT.h>

class TUDPClientSocket;
class TThread;


class TClientEvtBuilder: public TEvtBuilder
{

	RQ_OBJECT("TClientEvtBuilder")

protected:
	static TUDPClientSocket *DataBroadUDPsock;    //pointer to a UDP socket
	TThread * DAQThread;

public:
	TClientEvtBuilder(string addr, string udptype, TConfig &cod, vector <TCrateCBLT> &tcrate, unsigned int mastercrate);
	~TClientEvtBuilder();
	int CheckErrors(string ferror, int merr);
	static void* EventBuilderRun(void *arg);
	void StartDAQThread();
	TThread *GetDAQThread();
	void ProcessStopSignal();         //for 'stop signal'
};


#endif //#ifndef TClientEvtBuilder_H