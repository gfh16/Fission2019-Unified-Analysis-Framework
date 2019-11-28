////////////////////////////////////////////////
// TDataReceiver.h: Receive data form UDP
// broadcast socket, and write them to disk
// file.
// Han Jianlong (08/2012)
///////////////////////////////////////////////

#ifndef TDataReceiver_H
#define TDataReceiver_H

#include "caenacq.h"

class TUDPServerSocket;

class TDataReceiver
{
protected:
	static unsigned int fdatabuf[NETBUFFER/4];
	static TUDPServerSocket *UDPDataRec;

public:
	static int eventcounter;      //value of envent number from received data(the second int value)

public:
	TDataReceiver(unsigned int iport);
	virtual ~TDataReceiver();
	static int ReceiveData();
	static bool IsUDPValid();
	unsigned int *GetDataBuf(){return fdatabuf;}
	bool CloseRecDataSocket();
	bool OpenRecDataSocket();
};


#endif //#ifndef TDataReceiver_H