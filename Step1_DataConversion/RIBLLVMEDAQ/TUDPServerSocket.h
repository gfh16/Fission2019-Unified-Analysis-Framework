////////////////////////////////////////////////
//  TUDPServerSocket.h: 
//  receive broadcasted data UDPSocket
//	receive data from ethernet
//  Han Jianlong 07/2012 test version  v.01
////////////////////////////////////////////////

#ifndef TUDPServerSocket_H
#define TUDPServerSocket_H

#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#pragma comment(lib,"Ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h> /* close() */
#endif

class TUDPServerSocket
{
private:
	int isocket;
	unsigned int iport;
	struct sockaddr_in  cliAddr, servAddr;

public:
	TUDPServerSocket(){isocket = 0;}
	TUDPServerSocket(unsigned int port);
	virtual ~TUDPServerSocket();
	virtual unsigned int RecvRaw(char *buff, int max_len);
	bool IsValid(){return isocket < 0 ? false : true;}
	bool CloseCleanSocket();
	bool OpenInitSocket();
	bool SetRcvTimeOutValue(unsigned int msec);
	bool JoinMemberShip(const char *MC_IP);
};


#endif//#ifndef TUDPServerSocket_H