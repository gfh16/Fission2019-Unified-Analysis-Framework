////////////////////////////////////////////////
//  TUDPServerSocket.cpp: 
//  receive broadcasted data UDPSocket
//	receive data from ethernet
//  Han Jianlong 07/2012 test version  v.01
////////////////////////////////////////////////
#include <iostream>
#include <cstring>
using namespace std;

#include "TUDPServerSocket.h"
#include <stdio.h>

TUDPServerSocket::TUDPServerSocket(unsigned int port)
{
	iport = port;
	OpenInitSocket();
	//printf("TUDPServerSocket:: waiting for data on port UDP %u\n", iport);
}

unsigned int TUDPServerSocket::RecvRaw(char *buff, int max_len)
{
	if(!IsValid()) return 0;
	int flags  =0;
#ifdef WIN32
	int cliLen = sizeof(cliAddr);
#else
	socklen_t cliLen = sizeof(cliAddr);
#endif //#ifdef WIN32

	int len = recvfrom(isocket, buff, max_len, flags, (struct sockaddr *)&cliAddr, &cliLen);
	if(len<0) len = 0;
	return len;
}

TUDPServerSocket::~TUDPServerSocket()
{
	CloseCleanSocket();
}

bool TUDPServerSocket::CloseCleanSocket()
{
	int sta = 0;
	if(isocket)
	{
#ifdef WIN32
		sta = closesocket(isocket);
		WSACleanup();
		isocket = 0;
#else
		sta = close(isocket);
		isocket = 0;
#endif //#ifdef WIN32
		if(sta == 0)
			return true;
		else
			return false;
	}
}

bool TUDPServerSocket::OpenInitSocket()
{
	//iport = port;
	int  rc = 0, sset =0;
	int  opt = 1;
	isocket = -1;

#ifdef _WIN32
	WSADATA wsaData = {0};
	int iResult = 0;
	/* Initialize Winsock */
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		wprintf(L"WSAStartup failed: %d\n", iResult);
		return 0;
	}
	/* socket creation */
	isocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#else
	/* socket creation */
	isocket = socket(AF_INET, SOCK_DGRAM, 0);
#endif
	if (isocket<0) {
		printf("TUDPServerSocket: cannot open socket \n");
		isocket = 0;
	}

	/* bind local server port */
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(iport);
	if(isocket)
	{
		sset = setsockopt(isocket,SOL_SOCKET,SO_REUSEADDR,(const char*)&opt,sizeof(opt));
		if(sset != 0) 
		{
			printf("TUDPServerSocket: Set socket option 'SO_REUSEADDR' error on port %d \n", iport);
		}
		rc = bind (isocket, (struct sockaddr *) &servAddr, sizeof(servAddr));
		if (rc<0) 
		{
			printf("TUDPServerSocket: cannot bind port number %d \n", iport);
		}
	}
	else
	{
		isocket =0;
	}

	if(isocket) return true;
	return false;
}

bool TUDPServerSocket::SetRcvTimeOutValue(unsigned int millisec)
{
	if( !IsValid() ) return false;
	int sta = -1;
	int msec = millisec;
	unsigned int sec  = (unsigned int)millisec/1000;
	unsigned int usec = (millisec%1000) * 1000;

	struct timeval tv_out;
	tv_out.tv_sec  = sec;
	tv_out.tv_usec = usec;

#ifdef _WIN32
	sta = setsockopt(isocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&msec, sizeof(msec));
#else
	sta = setsockopt(isocket,SOL_SOCKET,SO_RCVTIMEO,&tv_out, sizeof(tv_out));
#endif
	if(sta == 0 ) return true;
	return false;
}

bool TUDPServerSocket::JoinMemberShip(const char *MC_IP)
{
	if( !IsValid() ) return false;
	ip_mreq mreq;
	memset(&mreq, 0, sizeof(mreq));

#ifdef WIN32
	mreq.imr_interface.S_un.S_addr = INADDR_ANY;
	mreq.imr_multiaddr.S_un.S_addr = inet_addr(MC_IP);
#else
	mreq.imr_interface.s_addr  = INADDR_ANY;
	mreq.imr_multiaddr.s_addr  = inet_addr(MC_IP);
#endif //#ifdef WIN32

	int ret = setsockopt(isocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	if( ret != 0 )
	{
		cout<<"TUDPCLientSocket>> Error in setsockopt(IP_ADD_MEMBERSHIP). "<<endl;
		return false;
	}
	return true;
}
