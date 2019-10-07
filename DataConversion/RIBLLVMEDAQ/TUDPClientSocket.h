//////////////////////////////////////////////////////
//  TUDPClientSocket.h: Broadcast UDPSocket
//	Broadcast the data to ethernet
//  Han Jianlong 07/2012 test version  v.01
//  v.02 : added "localhost" environment for singal PC
//            by Han jianlong (06/2013)
//////////////////////////////////////////////////////

#ifndef TUDPClientSocket_H
#define TUDPClientSocket_H

#ifdef WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
//#include <windows.h>
#else
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#endif //#ifdef WIN32

#ifdef WIN32
#pragma comment(lib,"Ws2_32.lib")
#endif //#ifdef WIN32

#include <vector>
using namespace std;

class TUDPClientSocket
{
private:
	int isocket;
	unsigned int iport;
	vector<struct sockaddr_in> v_addr_info;
	int addr_len; 

public:
	TUDPClientSocket(){isocket = 0;}
	TUDPClientSocket(const char* netaddress, unsigned int port);
	TUDPClientSocket(const char* netaddress, unsigned int port, bool multicast); //for multicast udp socket
	TUDPClientSocket(const char* addrlistfile, unsigned int port, char cf); //for command and data file header udp socket
	TUDPClientSocket(const char* addrlistfile);
	virtual ~TUDPClientSocket();
	virtual int SendTo(const char* buff, int length);
	bool IsValid() {return isocket<0 ? false: true;}
	bool SetSendTimeOutValue(unsigned int usec);
	bool SetRouteNum(unsigned int rnum);
	bool SetLoopBack(bool lback);
	bool JoinMemberShip(const char* MC_IP);
	int  ReadAddrListFileForDS(const char *listfile); //read ip list for data sending
	int  ReadAddrListFileForCF(const char *listfile, unsigned int port); //read ip list for sending command and file header

};


#endif //#ifndef TUDPBroadSocket_H
