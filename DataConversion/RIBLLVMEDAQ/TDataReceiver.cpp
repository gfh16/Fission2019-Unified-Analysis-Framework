////////////////////////////////////////////////
// TDataReceiver.cpp: Receive data form UDP
// broadcast socket, and write them to disk
// file.
// Han Jianlong (08/2012)
///////////////////////////////////////////////

#include "TUDPServerSocket.h"
#include "TDataReceiver.h"
#include "TControl.h"
#include "caenacq.h"
#include <cstring>

unsigned int TDataReceiver::fdatabuf[NETBUFFER/4];
TUDPServerSocket *TDataReceiver::UDPDataRec;
int TDataReceiver::eventcounter = 0;

TDataReceiver::TDataReceiver(unsigned int iport)
{
	UDPDataRec = new TUDPServerSocket(iport);
	UDPDataRec->SetRcvTimeOutValue(700); //time out 700 mSec
	//UDPDataRec->JoinMemberShip(MULTICAST_IP.c_str());
}

TDataReceiver::~TDataReceiver()
{
	delete UDPDataRec;
}

//return the received data length in byte
int TDataReceiver::ReceiveData()
{
	memset(fdatabuf, 0x0, NETBUFFER);
	return UDPDataRec->RecvRaw( (char*)fdatabuf, sizeof(fdatabuf) );
}

bool TDataReceiver::IsUDPValid()
{
	return UDPDataRec->IsValid();
}

bool TDataReceiver::CloseRecDataSocket()
{
	return UDPDataRec->CloseCleanSocket();
}

bool TDataReceiver::OpenRecDataSocket()
{
	return UDPDataRec->OpenInitSocket();
}