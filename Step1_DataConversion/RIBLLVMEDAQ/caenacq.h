/////////////////////////////////////////////////
// readout program chimera qdc/tdc
// CAEN V2718 Optical-LINK version 
// 07/2007   v 0.7 
// E. De Filippo  defilippo@ct.infn.it 
// Modified by Han jianlong for RIBLL1
////////////////////////////////////////////////
#ifndef CAENACQ_H
#define CAENACQ_H

#include <string>
using namespace std;

#ifdef WIN32
//#include "WTypes.h"
typedef short VARIANT_BOOL;
#endif

////Path environment
const string gConfPath = "DAQCONFPATH";           //environment, file PATH for vme config files
const string gDataPath = "DAQDataPath";           //environment, file PATH for data files
const string gServerIP = "DAQServerIP";              //environment, server ip

//VME Read functions definition
#define UserVMEFunctionBefore
#define UserVMEFunctionAfter

//// Data headers definition
#define Event_Header 0xFFFFFFFF
#define Crate_Header 0xFFFF0000

////Socket port definition
const unsigned int UDPDataBroadPortCon = 2221; //for control thread write data to disk
const unsigned int UDPDataBroadPortMon = 2223; //for mononlie
const unsigned int UDPCommBroadPort    = 2225;
const unsigned int TCPCommPort         = 2229;
const unsigned int UDPFHBroadPort      = 2235; // for data file header broadcast

////For UDP multicast, not used now in this program
const string MULTICAST_IP("239.0.0.99");       // multicast ip
const int RouteNum_TTL = 2;                    // Number of IP_MULTICAST_TTL
const bool MUDPLoopBack = false;               // IP_MULTICAST_LOOP

////For Data transfer, data only send to IPs in this file
#define IPLISTF  "/IPlist.dat"


////acquisition parameters
#define Wait_Data_Ready_IRQ
const unsigned int IRQ_Level = 0x7F;     // IRQ Levvel 0x7F
#define FIFO_Read_Mode

#define VME_Crate_BufLENGTH 65536        // VME crate acquisition buffer (bytes)
#define NETBUFFER VME_Crate_BufLENGTH*2  // MAX network buffer (128 kbytes) 
#define FileHeaderLEN 1024               // File header length char (bytes) 
#define MAX_NET_LEN 4096                 // Maximum network buffer for data

//#define UserVMEFunctionBefore
//#define UserVMEFunctionAfter


enum EBoardInit {kCheckOnly, kInitReg};
enum VMError {kSuccess, kBusError, kGenericError, kInvalidParam}; //VME error codes

////////////////////////////////////////////////////////////////
// generic garbage manager function for STL containers
// Delete pointers in an STL sequence container. 
////////////////////////////////////////////////////////////////
template<class Seq> void purge(Seq& c) {
  typename Seq::iterator i;
  for(i = c.begin(); i != c.end(); ++i) {
    delete *i;
    *i = 0;
  }
}
 
// Iterator version:
template<class InpIt> void purge(InpIt begin, InpIt end) {
  while(begin != end) {
    delete *begin;
    *begin = 0;
    ++begin;
  }

#define SafeDeleteArr(p) {if(p) {delete[] p; p = 0;}}
#define SafeDeleteP(p)   {if(p) {delete   p; p = 0;}}
}

#endif
