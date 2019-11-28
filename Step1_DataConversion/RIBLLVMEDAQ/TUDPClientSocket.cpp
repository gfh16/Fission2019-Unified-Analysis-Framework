/////////////////////////////////////////////////////////
//  TUDPClientSocket.cpp: Broadcast UDPSocket
//	Broadcast the data to ethernet
//  Han Jianlong 07/2012 test version  v.01
//  v.02 : added "localhost" environment for singal PC
//            by Han jianlong (06/2013)
/////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
#include "caenacq.h"
#include "TString.h"
#include "TUDPClientSocket.h"
#include "TSystem.h"
#include "TInetAddress.h"

TUDPClientSocket::TUDPClientSocket(const char* netaddress, unsigned int port)
{
	iport = port; 
	int opt=1;
	isocket = -1;
	TString netaddr = netaddress;
	netaddr.ReplaceAll(" ", "");

	struct sockaddr_in  addr_info;
	addr_info.sin_family=AF_INET;  
	addr_info.sin_addr.s_addr=inet_addr(netaddr.Data());  
	addr_info.sin_port=htons(port);

	v_addr_info.push_back(addr_info);

	addr_len=sizeof(struct sockaddr); 

#ifdef WIN32
	WSADATA wsd;   
	int err;   
	err = WSAStartup(MAKEWORD(2,2),&wsd); 
#endif //#ifdef WIN32

	isocket = socket(AF_INET,SOCK_DGRAM,0); //  IPPROTO_UDP
	if(isocket<0)  
	{  
		cout << "Data Broadcast Socket on ip: " << netaddress << " open error!" << endl;
		return;
	}
	
	//if the IP is "*.*.*.255", set its "SO_BROADCAST" property
	TString chaddr = netaddr;
	int pos = chaddr.Last('.');
	TString subaddr = chaddr(pos+1, 3);
	if( (subaddr.Length() == 3) && (subaddr.Contains("255")) )
	{
		setsockopt(isocket,SOL_SOCKET,SO_BROADCAST,(const char*)&opt,sizeof(opt));
	}
}

TUDPClientSocket::TUDPClientSocket(const char* netaddress, unsigned int port, bool multicast)
{
	iport = port; 
	int opt=1;
	isocket = -1;
	TString netaddr = netaddress;
	netaddr.ReplaceAll(" ", "");

	struct sockaddr_in  addr_info;
	addr_info.sin_family=AF_INET;  
	addr_info.sin_addr.s_addr=inet_addr(netaddr.Data());  
	addr_info.sin_port=htons(port);

	v_addr_info.push_back(addr_info);

	addr_len=sizeof(struct sockaddr); 

#ifdef WIN32
	WSADATA wsd;   
	int err;   
	err = WSAStartup(MAKEWORD(2,2),&wsd); 
#endif //#ifdef WIN32

	isocket = socket(AF_INET,SOCK_DGRAM,0); //  IPPROTO_UDP
	if(isocket<0)  
	{  
		cout << "Data Broadcast Socket on ip: " << netaddress << " open error!" << endl;
		return;
	}
	SetRouteNum(RouteNum_TTL);
	SetLoopBack(MUDPLoopBack);
}

TUDPClientSocket::TUDPClientSocket(const char *addrlistfile)
{
	iport = 0;
	int ipnum = ReadAddrListFileForDS(addrlistfile);
	int opt=1;
	isocket = -1;

	addr_len=sizeof(struct sockaddr); 

#ifdef WIN32
	WSADATA wsd;   
	int err;   
	err = WSAStartup(MAKEWORD(2,2),&wsd); 
#endif //#ifdef WIN32

	isocket = socket(AF_INET,SOCK_DGRAM,0); //  IPPROTO_UDP
	if(isocket<0)  
	{  
		cout << "Data Broadcast Socket on 'IPLIST' open error!" << endl;
		return;
	}
}

TUDPClientSocket::TUDPClientSocket(const char *addrlistfile, unsigned int port, char cf)
{
	iport = 0;
	int ipnum = ReadAddrListFileForCF(addrlistfile, port);
	int opt=1;
	isocket = -1;

	addr_len=sizeof(struct sockaddr); 

#ifdef WIN32
	WSADATA wsd;   
	int err;   
	err = WSAStartup(MAKEWORD(2,2),&wsd); 
#endif //#ifdef WIN32

	isocket = socket(AF_INET,SOCK_DGRAM,0); //  IPPROTO_UDP
	if(isocket<0)  
	{  
		if(cf == 'c')
			cout << "Command sending (broadcast) socket on 'IPLIST' open error!" << endl;
		if(cf == 'd')
			cout << "Data-file header sending  (broadcast) socket on 'IPLIST' open error!" << endl;
		return;
	}
}

int TUDPClientSocket::SendTo(const char *buff, int length)
{
	if(!IsValid()) return 0;
	int sendlen=0;
	unsigned int i=0;
	unsigned int sz = v_addr_info.size();
	for(i=0; i<sz; i++)
	sendlen = sendto(isocket, buff, length, 0, (struct sockaddr *)&v_addr_info[i], addr_len); 
	return sendlen;
}

TUDPClientSocket::~TUDPClientSocket()
{
	if(isocket)
	{
#ifdef WIN32
		closesocket(isocket);
		WSACleanup();
#else
		close(isocket);
#endif //#ifdef WIN32
	}
}

bool TUDPClientSocket::SetSendTimeOutValue(unsigned int usec)
{
	if( !IsValid() ) return false;
	int sta = -1;
	int msec;
	msec = usec/1000;
	if(msec < 1) msec = 1;
	unsigned int seco  = (unsigned int)usec/1000000;
	unsigned int useco = usec%1000000;

	struct timeval tv_out;
	tv_out.tv_sec  = seco;
	tv_out.tv_usec = useco;

#ifdef _WIN32
	sta = setsockopt(isocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&msec, sizeof(msec));
#else
	sta = setsockopt(isocket, SOL_SOCKET, SO_SNDTIMEO, &tv_out, sizeof(tv_out));
#endif
	if(sta == 0 ) return true;
	return false;
}

bool TUDPClientSocket::SetRouteNum(unsigned int rnum)
{
	if( !IsValid() ) return false;
	int routenum = rnum;
	int ret = setsockopt(isocket, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&routenum,sizeof(routenum));
	if( ret != 0 )
	{
		cout<<"TUDPCLientSocket>> Error in setsockopt(IP_MULTICAST_TTL)."<<endl;
		return false;
	}
	return true;
}

bool TUDPClientSocket::SetLoopBack(bool lback)
{
	if( !IsValid() ) return false;
	unsigned int loopback = (unsigned int)lback;
	int ret = setsockopt(isocket,IPPROTO_IP,IP_MULTICAST_LOOP, (char*)&loopback,sizeof(loopback));
	if( ret != 0 )
	{
		cout<<"TUDPCLientSocket>> Error in setsockopt(IP_MULTICAST_LOOP): "<<endl;
		return false;
	}
	return true;
}

bool TUDPClientSocket::JoinMemberShip(const char *MC_IP)
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

int TUDPClientSocket::ReadAddrListFileForDS(const char *listfile)
{
	ifstream  addlf(listfile);
	if( !addlf.good() )
	{
		cout << endl << endl;
		cout << "Open IP list file: " << listfile << " ERROR." << endl;
		cout << endl << endl;
		return 0;
	}

	struct sockaddr_in  addr_info;
	addr_info.sin_family=AF_INET;  

	char ipline[300];
	TString ipcheck, ip;
	cout << "Data Will Be Send to: " << endl;
	while(addlf.getline( ipline, sizeof(ipline) ))
	{
		ipcheck.Clear();
		ip.Clear();
		ipcheck = ipline;
		if(ipcheck.IsWhitespace()) continue;
		ipcheck.ReplaceAll(" ", "");
		if(ipcheck.Length()>15) continue;
		TInetAddress addip = gSystem->GetHostByName(ipcheck.Data());
		ipcheck = addip.GetHostAddress();
		ip = ipcheck;
		if(ipcheck.Contains("."))
		{
			ipcheck.ReplaceAll(".", "");
			if(ipcheck.Atoi() == 0) continue;
		}
		
		addr_info.sin_addr.s_addr=inet_addr(ip.Data());

		addr_info.sin_port=htons(UDPDataBroadPortCon);
		v_addr_info.push_back(addr_info);               //UDPDataBroadPortCon  port

		addr_info.sin_port=htons(UDPDataBroadPortMon);
		v_addr_info.push_back(addr_info);               //UDPDataBroadPortMon  port

		cout << "\t" << ip.Data() << endl;
	}
	return v_addr_info.size();
}

int TUDPClientSocket::ReadAddrListFileForCF(const char *listfile, unsigned int port)
{
	ifstream  addlf(listfile);
	if( !addlf.good() )
	{
		cout << endl << endl;
		cout << "Open IP list file: " << listfile << " ERROR." << endl;
		cout << endl << endl;
		return 0;
	}

	struct sockaddr_in  addr_info;
	addr_info.sin_family=AF_INET;  

	char ipline[300];
	TString ipcheck, ip;
	while(addlf.getline( ipline, sizeof(ipline) ))
	{
		ipcheck.Clear();
		ip.Clear();
		ipcheck = ipline;
		if(ipcheck.IsWhitespace()) continue;
		ipcheck.ReplaceAll(" ", "");
		if(ipcheck.Length()>15) continue;
		TInetAddress addip = gSystem->GetHostByName(ipcheck.Data());
		ipcheck = addip.GetHostAddress();
		ip = ipcheck;
		if(ipcheck.Contains("."))
		{
			ipcheck.ReplaceAll(".", "");
			if(ipcheck.Atoi() == 0) continue;
		}
		
		addr_info.sin_addr.s_addr=inet_addr(ip.Data());

		addr_info.sin_port=htons(port);
		v_addr_info.push_back(addr_info);               //for UDPCommBroadPort/UDPFHBroadPort  port
	}
	return v_addr_info.size();
}