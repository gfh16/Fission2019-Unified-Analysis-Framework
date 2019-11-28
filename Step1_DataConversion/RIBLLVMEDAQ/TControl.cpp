/////////////////////////////////////////
// TControl.cpp: Control the daq
// This class contents a socket used to
// receive 'control commands' from
// the cotrol side PC
// Han Jianlong 07/2012
/////////////////////////////////////////

#include <iostream>
#include <string>
#include "stdlib.h"
#include "TUDPClientSocket.h"
#include "TUDPServerSocket.h"
#ifdef WIN32
#include <windows.h>
#define sleep Sleep
#define WaitSec 600
#else
#include <unistd.h>
#define sleep usleep
#define WaitSec 300000
#endif
using namespace std;

#include "caenacq.h"
#include "TControl.h"
#include "Rtypes.h"
#include "TServerSocket.h"
#include "TSocket.h"
#include "TString.h"
#include "TThread.h"

TSocket * TControl::iRecvSocket=0;              
TServerSocket *TControl::iSendSocket=0;        
TSocket *TControl::iSendSocket_imp=0;          
TUDPClientSocket *TControl::iComBroadUDPSock=0;
TUDPServerSocket *TControl::iComRevUDPSock=0;  
TUDPClientSocket *TControl::iFHeBroadUDPSock=0;
TUDPServerSocket *TControl::iFHeRevUDPSock=0;  

eAction TControl::PC_Action;   
string TControl::sRecvComm;
bool TControl::TCPwait_command;
bool TControl::TCPRead_command;
bool TControl::UDPwait_command;

string TControl::ControlPCAddr;
string TControl::fheader_file;
string TControl::ack_mess;
//string TControl::other_mess;

ECommands TControl::comm;



static string Comm_Acknowlege = " COMMAND ACKNOWLEDGED";
static string Comm_Not_Acknow =  " Do not receive acknlwlege form DAQ_PC." ;
const string tname = "TControl>> ";

TControl *onlyControl = 0;

ClassImp(TControl);

TControl::TControl(const char *netaddr, eAction action)
{
	if(onlyControl)
	{
		cout<< "TControl>> only one instance of TControl allowed. " << endl;
		return;
	}
	onlyControl = this;

	iRecvSocket          = 0;
	iSendSocket          = 0;
	iSendSocket_imp      = 0;
	iComBroadUDPSock     = 0;
	iComRevUDPSock       = 0;
	iFHeBroadUDPSock     = 0;
	iFHeRevUDPSock       = 0; 
	thread_DaqPC         = 0;
	thread_OnlinePC      = 0;
	thread_ConPCRevErr   = 0;

	PC_Action = NULL_PC;
	TCPwait_command = false;
	TCPRead_command = false;
	UDPwait_command = false;
	comm = kC_WAIT;

	if(action == DAQ_PC)
	{
		PC_Action = DAQ_PC;
		//iRecvSocket = new TSocket(netaddr, TCPCommPort);
		ControlPCAddr = netaddr;
		//TString staddr = netaddr;
		//int pos = staddr.Last('.');
		//staddr.Replace(pos+1, 3, "255"); 
		string path = getenv(gConfPath.c_str());
		path +=  IPLISTF;
		iComBroadUDPSock  = new TUDPClientSocket(path.c_str(), UDPCommBroadPort, 'c');
		if(!iComBroadUDPSock->IsValid()) cout << tname << " ComBroadUDPSock error. "<< endl;

		iFHeBroadUDPSock  = new TUDPClientSocket(path.c_str(), UDPFHBroadPort, 'f');
		if(!iFHeBroadUDPSock->IsValid()) cout << tname << " FHeBroadUDPSock error. "<< endl;
		//DaqPCWaitConnection();
		TCPwait_command = true;
		TCPRead_command = true;
	}
	else if(action == CONTROL_PC)
	{
		PC_Action = CONTROL_PC;
		iSendSocket = new TServerSocket(TCPCommPort, kTRUE);
		iComRevUDPSock = new TUDPServerSocket(UDPCommBroadPort); // used to receive some broadcast messages here.
		if(!iComRevUDPSock->IsValid()) cout << tname << " ComRevUDPSock error. "<< endl;
		UDPwait_command = true;
	}
	else if(action == ONLINE_PC)
	{
		PC_Action = ONLINE_PC;
		iComRevUDPSock = new TUDPServerSocket(UDPCommBroadPort);
		if(!iComRevUDPSock->IsValid()) cout << tname << " ComRevUDPSock error. "<< endl;
		iFHeRevUDPSock = new TUDPServerSocket(UDPFHBroadPort);
		if(!iFHeRevUDPSock->IsValid()) cout << tname << " FHeRevUDPSock error. "<< endl;
		UDPwait_command = true;
	}
}

TControl::~TControl()
{
	SafeDeleteP(iRecvSocket);
	SafeDeleteP(iSendSocket);
	SafeDeleteP(iSendSocket_imp);
	SafeDeleteP(iComBroadUDPSock);
	SafeDeleteP(iComRevUDPSock);
	SafeDeleteP(iFHeBroadUDPSock);
	SafeDeleteP(iFHeRevUDPSock); 
}

/////////////////////////////////////////////////////////////////
// Block the Process ( use while(true) ) to wait for
// conneciton form Control_PC
// The 'while(true)' infenite loop will block the process 
// until a success connection to Control PC is established
/////////////////////////////////////////////////////////////////
bool TControl::DaqPCWaitConnection()
{
	while(true)  //this 'while' will block the process until a success connection to Control PC is established
	{
		cout << "Waiting for connection to Control_PC......  " << endl;
		sleep(WaitSec);
L150:
		if(iRecvSocket)
		{
			if(iRecvSocket->IsValid())
			{
				return true;  // break out thi
			}
			else
			{
				delete iRecvSocket;
				iRecvSocket = 0;
				goto L150;
			}
		}
		else
		{
			iRecvSocket = new TSocket(ControlPCAddr.c_str(), TCPCommPort);
			if(iRecvSocket->IsValid()) return true;
		}
	}
}

//used for DAQ_PC to receive the control command
void* TControl::recv_TCPcontrol_command(void *arg)
{
	DaqPCWaitConnection();
	if(!iRecvSocket)
	{
		cout << "TControl: Communiction socket for receiving 'command' invalid." << endl; 
		return 0;
	}

	TCPwait_command = true;
	TCPRead_command = true;

	char commbuf[200];
	int bufsize = sizeof(commbuf);
	char fhbuf[FileHeaderLEN];
	int inmess_len = 0, outmess_len = 0;

	while(TCPwait_command)
	{
		sleep(WaitSec);
		memset(commbuf, 0x0, bufsize);
		int i=0;
		while(TCPRead_command)
		{
			sleep(WaitSec);
			if(iRecvSocket->IsValid()) break;  // normal break, used while the connection working well 
			if(DaqPCWaitConnection())  break;  // if reconnect the Control_PC, if the connection is not valid.
		}
		//cout << " Waiting for command ...... " << endl;
		inmess_len = iRecvSocket->Recv(commbuf, bufsize);
		sRecvComm = commbuf;
		//cout << " Got 'command': " << sRecvComm << endl;

		// test connection
		if(sRecvComm == sCommands[0]) {
			comm = kC_TEST;
			if(outmess_len=iRecvSocket->Send("Server is Alive and Ready") < 0) {
				cout<<tname<<"Error sending reply message to control master PC."<<endl;
			}
			CommandForward(sRecvComm);
			MessageSignal(arg);
			continue;
		}

		// Initialization
		if(sRecvComm == sCommands[1]) {
			comm = kC_INIT;
			CommandForward(sRecvComm);
			acknowledge();
			MessageSignal(arg);
			continue;
		}

		// Start
		if(sRecvComm == sCommands[2]) { 
			comm = kC_START;
			CommandForward(sRecvComm);
			acknowledge();
			MessageSignal(arg);
			continue;
		}

		// Stop
		if(sRecvComm == sCommands[3]) { 
			comm = kC_STOP;
			//the master forwards STOP command to all servers
			CommandForward(sRecvComm);
			acknowledge();
			MessageSignal(arg);
			continue;
		}

		// Open file
		if(sRecvComm == sCommands[4]) {
			if(iRecvSocket->Select(TSocket::kRead, 800))  // waiting the control PC to send the data file header
			{
				if( (inmess_len = iRecvSocket->Recv(fhbuf, FileHeaderLEN)) <0 ) {
					cout<<tname<<"Error receiving data file header from control master PC."<<endl;
					continue;
				}
			}
			else
			{
				cout << tname << " Time Out! Wait for the data file header." << endl;
				continue;
			}
			fheader_file = fhbuf;
			cout<<tname<< " Got File Header: " <<fheader_file<<endl;
			comm = kC_FOPEN;
			CommandForward(sRecvComm);
			sleep(WaitSec);      // Wait for other online monitering PC to accept the 'file header' fheader_file
			BroadFileHeader();   // Broadcast the file header to all online monitering PC
			acknowledge();
			MessageSignal(arg);
			continue;
		}

		// Close File
		if(sRecvComm == sCommands[5]) { 
			comm = kC_FCLOSE;
			CommandForward(sRecvComm);
			acknowledge();
			MessageSignal(arg);
			continue;
		}

		//exit and put offline
		if(sRecvComm == sCommands[6]) { 
			comm = kC_EXIT;
			CommandForward(sRecvComm);
			acknowledge();
			//comm = kC_WAIT;
			TCPwait_command = false;
			MessageSignal(arg);
			sleep(WaitSec);
			continue;
		}

		// for not a valide 'command'
		CommandForward(sRecvComm);
		acknowledge();
	}
	return 0;
}

int TControl::CommandForward(string message)
{
	int len = 0;
	if(iComBroadUDPSock && (PC_Action == DAQ_PC) )
	{
		len = iComBroadUDPSock->SendTo(message.c_str(), message.size());
		if(len<=0) cout << tname << " Error broadcast commands." << endl;
	}
	return len;
}

void TControl::acknowledge()
{	
	if( iRecvSocket && (PC_Action == DAQ_PC) )
	{
		TString ackmess = sRecvComm;
		ackmess += Comm_Acknowlege;
		iRecvSocket->Select(TSocket::kWrite, 800);
		int len = iRecvSocket->Send(ackmess.Data());
		if(len<0) cout << tname << "Error sending Acknowledge."<<endl;
	}
}

void TControl::BroadFileHeader()
{
	if(iFHeBroadUDPSock && (PC_Action == DAQ_PC) )
	{
		int len = iFHeBroadUDPSock->SendTo(fheader_file.c_str(), fheader_file.size());
		if(len<=0) cout << tname << " Error broadcast data file header." << endl;
	}
}

TThread * TControl::GetDaqPCThread()
{
	if(thread_DaqPC)
	{
		TThread::EState stat = thread_DaqPC->GetState();
		if(stat == TThread::kRunningState || stat == TThread::kNewState)
		return thread_DaqPC;
	}
	return 0;
}

TThread * TControl::GetOnlinePCThread()
{
	return thread_OnlinePC;
}

void* TControl::recv_UDPBroadcontrol_command(void *arg)
{
	if(!iComRevUDPSock)
	{
		cout << "TControl: Communiction UDP socket for receiving UDP broadcasted 'command' invalid." << endl; 
		ack_mess = "TControl: Communiction UDP socket for receiving UDP broadcasted 'command' invalid.";
		MessageSignal(arg);
		return 0;
	}

	UDPwait_command = true;

	char commbuf[200];
	int bufsize = sizeof(commbuf);
	char fhbuf[FileHeaderLEN];
	int inmess_len = 0, outmess_len = 0;
	ack_mess = "Not a valid command.";

	while(UDPwait_command)
	{
		memset(commbuf, 0x0, sizeof(commbuf));
		memset(fhbuf, 0x0, sizeof(fhbuf));

		//cout << " Waiting for command ...... " << endl;
		inmess_len =  iComRevUDPSock->RecvRaw(commbuf, bufsize);
		if(inmess_len<=0)
		{
			MessageSignal(arg);
			continue;
		}

		sRecvComm = commbuf;
		//cout << " Got command: " << sRecvComm << endl;

		// test connection
		if(sRecvComm == sCommands[0]) {
			comm = kC_TEST;
			ack_mess = sCommands[0];
			ack_mess += Comm_Acknowlege;
			MessageSignal(arg);
			continue;
		}

		// Initialization
		if(sRecvComm == sCommands[1]) {
			comm = kC_INIT;
			ack_mess = sCommands[1];
			ack_mess += Comm_Acknowlege;
			MessageSignal(arg);
			continue;
		}

		// Start
		if(sRecvComm == sCommands[2]) { 
			comm = kC_START;
			ack_mess = sCommands[2];
			ack_mess += Comm_Acknowlege;
			MessageSignal(arg);
			ControlSignal(arg, kC_START);
			continue;
		}

		// Stop
		if(sRecvComm == sCommands[3]) { 
			comm = kC_STOP;
			ack_mess = sCommands[3];
			ack_mess += Comm_Acknowlege;
			MessageSignal(arg);
			ControlSignal(arg, kC_STOP);
			continue;
		}

		// Open file
		if(sRecvComm == sCommands[4]) { 
			if( ( inmess_len = iFHeRevUDPSock->RecvRaw(fhbuf, FileHeaderLEN) ) <0 ) {
				cout<<tname<<"Error receiving data file header from DAQ_PC's UDP broadcasting."<<endl;
				continue;
			}
			fheader_file = fhbuf;
			//cout<<tname<<fheader_file<<endl;
			comm = kC_FOPEN;
			ack_mess = sCommands[4];
			ack_mess += Comm_Acknowlege;
			MessageSignal(arg);
			ControlSignal(arg, kC_FOPEN);
			continue;
		}

		// Close File
		if(sRecvComm == sCommands[5]) { 
			comm = kC_FCLOSE;
			ack_mess = sCommands[5];
			ack_mess += Comm_Acknowlege;
			MessageSignal(arg);
			ControlSignal(arg, kC_FCLOSE);
			continue;
		}

		//exit and put offline (do not used for OnlinePC)
		if(sRecvComm == sCommands[6]) { 
			comm = kC_EXIT;
			ack_mess = sCommands[6];
			ack_mess += Comm_Acknowlege;
			comm = kC_WAIT;
			MessageSignal(arg);
			continue;
		}

		ack_mess = sRecvComm;
		MessageSignal(arg);
		//ack_mess += " Is not a valide command!";
	}
	
	return 0;
}

// for control_PC only, used to receive some no-command messages,
// such as some errors, maybe useless
void* TControl::recv_UDPBroadMessages(void *arg)
{
	if(!iComRevUDPSock)
	{
		cout << "TControl: Communiction UDP socket for receiving UDP broadcasted 'command' invalid." << endl; 
		ack_mess = "TControl: Communiction UDP socket for receiving UDP broadcasted 'command' invalid.";
		//MessageSignal(arg);
		BroadMessageSignal(arg);
		return 0;
	}

	UDPwait_command = true;

	char commbuf[200];
	int bufsize = sizeof(commbuf);
	char fhbuf[FileHeaderLEN];
	int inmess_len = 0, outmess_len = 0;

	while(UDPwait_command)
	{
		memset(commbuf, 0x0, sizeof(commbuf));
		memset(fhbuf, 0x0, sizeof(fhbuf));

		//cout << " Waiting for command ...... " << endl;
		inmess_len =  iComRevUDPSock->RecvRaw(commbuf, bufsize);
		if(inmess_len<=0) continue;

		sRecvComm = commbuf;
		//cout << " Got command: " << sRecvComm << endl;

		// test connection
		if(sRecvComm == sCommands[0]) {
			continue;
		}

		// Initialization
		if(sRecvComm == sCommands[1]) {
			continue;
		}

		// Start
		if(sRecvComm == sCommands[2]) { 
			continue;
		}

		// Stop
		if(sRecvComm == sCommands[3]) { 
			continue;
		}

		// Open file
		if(sRecvComm == sCommands[4]) { 
			continue;
		}

		// Close File
		if(sRecvComm == sCommands[5]) { 
		}

		// exit and put offline (do not used by OnlinePC)
		if(sRecvComm == sCommands[6]) {
			continue;
		}

		// for some other messages
		ack_mess = sRecvComm;
		BroadMessageSignal(arg);
		//ack_mess += " Is not a valide command!";
	}

	return 0;
}

//Waite for a TCP connection form DAQ_PC, even if the CONTROL_PC is power
//on later than DAQ_PC.
bool TControl::AcceptReceiver()
{
	cout << " Waiting for DAQ_PC connection...... " << endl;
	if( !iSendSocket || (PC_Action != CONTROL_PC) )
	{
		cout << "TControl: Communiction TCP socket for master control PC invalid." << endl;
		ack_mess = "TControl: Communiction TCP socket for master control PC invalid.";
		//MessageSignal(this);
		return false;
	}

	if(!iSendSocket_imp)
	{
		ack_mess = "Waiting for TCP connection from DAQPC...";
		//MessageSignal(this);
		iSendSocket_imp =  iSendSocket->Accept();
		cout << " Communiction TCP socket established between Control_PC and DAQ_PC. " << endl;
		ack_mess = "Communiction TCP socket established between Control_PC and DAQ_PC. ";
		//MessageSignal(this);
		return true;
	}

	if(iSendSocket_imp) // for iSendSocket_imp exist, but not valid
	{
		if( iSendSocket_imp->IsValid() )
		{
			ack_mess = "Communiction TCP socket established between Control_PC and DAQ_PC. ";
			//MessageSignal((void*)this);
			return true;
		}
		else
		{
			delete iSendSocket_imp;
			iSendSocket_imp =0;
			ack_mess = "Waiting for TCP connection from DAQPC...";
			//MessageSignal((void*)this);
			iSendSocket_imp =  iSendSocket->Accept();
			cout << " Communiction TCP socket established between Control_PC and DAQ_PC. " << endl;
			ack_mess = " Communiction TCP socket established between Control_PC and DAQ_PC. ";
			//MessageSignal((void*)this);
			return true;
		}
	}
	return false;
}

bool TControl::send_TCPcontrol_command(string scomm)
{
	if(iSendSocket_imp)
	{
		if(!iSendSocket_imp->IsValid() )
		{		
			cout << tname << "TControl: Communiction TCP socket for master control PC cannot Accept() to DAQ_PC." << endl;
			ack_mess = "TControl: Communiction TCP socket for master control PC cannot Accept() to DAQ_PC.";
			MessageSignal((void*)this);
			return false;	
		}
	}
	else
	{
		ack_mess = "ERROR! TCP Socket between ControlPC and DAQ_PC not valied.";
		MessageSignal((void*)this);
		return false;
	}

	char commbuf[200];
	int bufsize = sizeof(commbuf);
	memset(commbuf, 0x0, bufsize);
	int inmess_len = 0, outmess_len = 0;
	ack_mess = "Command Send Out Error.";
	bool sstatus = false;

	//	cout << "Receiver waiting for connection..... " << endl; 
	iSendSocket_imp->Select(TSocket::kWrite);

	// test connection
	if(scomm == sCommands[0]){
		outmess_len = iSendSocket_imp->Send(sCommands[0].c_str());
		if( outmess_len <0 )
		{
			MessageSignal((void*)this);
			return false;
		}
		inmess_len = iSendSocket_imp->Recv(commbuf, bufsize);
		if(inmess_len)
		{
			ack_mess = commbuf;
			comm = kC_TEST;
			sstatus = true;
		}
		else
		{
			ack_mess = sCommands[0];
			ack_mess += Comm_Not_Acknow;
			sstatus = false;
		}
		MessageSignal((void*)this);
		return sstatus;
	}

	// Initialization
	if(scomm == sCommands[1]){
		outmess_len = iSendSocket_imp->Send(sCommands[1].c_str());
		if( outmess_len <0 )
		{
			MessageSignal((void*)this);
			return false;
		}
		inmess_len = iSendSocket_imp->Recv(commbuf, bufsize);
		if(inmess_len)
		{
			ack_mess = commbuf;
			comm = kC_INIT;
			sstatus = true;
		}
		else
		{
			ack_mess = sCommands[1];
			ack_mess += Comm_Not_Acknow;
			sstatus = false;
		}
		MessageSignal((void*)this);
		return sstatus;		
	}

	// Start
	if(scomm == sCommands[2]){
		outmess_len = iSendSocket_imp->Send(sCommands[2].c_str());
		if( outmess_len <0 )
		{
			MessageSignal((void*)this);
			return false;
		}
		inmess_len = iSendSocket_imp->Recv(commbuf, bufsize);
		if(inmess_len)
		{
			ack_mess = commbuf;
			comm = kC_START;
			sstatus = true;
			ControlSignal((void*)this, kC_START);
		}
		else
		{
			ack_mess = sCommands[2];
			ack_mess += Comm_Not_Acknow;
			sstatus = false;
		}
		MessageSignal((void*)this);
		return sstatus;
	}

	// Stop
	if(scomm == sCommands[3]){
		outmess_len = iSendSocket_imp->Send(sCommands[3].c_str());
		if( outmess_len <0 )
		{
			MessageSignal((void*)this);
			return false;
		}
		inmess_len = iSendSocket_imp->Recv(commbuf, bufsize);
		if(inmess_len)
		{
			ack_mess = commbuf;
			comm = kC_STOP;
			sstatus = true;
			ControlSignal((void*)this, kC_STOP);
		}
		else
		{
			ack_mess = sCommands[3];
			ack_mess += Comm_Not_Acknow;
			sstatus = false;
		}
		MessageSignal((void*)this);
		return sstatus;
	}

	// Open file
	if(scomm == sCommands[4]){
		outmess_len = iSendSocket_imp->Send(sCommands[4].c_str());
		if( outmess_len <0 )
		{
			MessageSignal((void*)this);
			return false;
		}
		if(iSendSocket_imp->Select(TSocket::kWrite, 800))
		{
			outmess_len = iSendSocket_imp->Send(fheader_file.c_str());  // Send the data file header to DAQ_PC
			if( outmess_len <0 )
			{
				ack_mess = "TControl: 'File Header' send out ERROR.";
				MessageSignal((void*)this);
				return false;
			}
		}
		inmess_len = iSendSocket_imp->Recv(commbuf, bufsize);
		//cout <<" this message: "<< inmess_len << endl;
		if(inmess_len)
		{
			ack_mess = commbuf;
			comm = kC_FOPEN;
			sstatus = true;
			ControlSignal((void*)this, kC_FOPEN);
		}
		else
		{
			ack_mess = sCommands[4];
			ack_mess += Comm_Not_Acknow;
			sstatus = false;
		}
		MessageSignal((void*)this);
		return sstatus;
	}

	// Close File
	if(scomm == sCommands[5]){
		outmess_len = iSendSocket_imp->Send(sCommands[5].c_str());
		if( outmess_len <0 )
		{
			MessageSignal((void*)this);
			return false;
		}
		inmess_len = iSendSocket_imp->Recv(commbuf, bufsize);
		if(inmess_len)
		{
			ack_mess = commbuf;
			comm = kC_FCLOSE;
			sstatus = true;
			ControlSignal((void*)this, kC_FCLOSE);
		}
		else
		{
			ack_mess = sCommands[5];
			ack_mess += Comm_Not_Acknow;
			sstatus = false;
		}
		MessageSignal((void*)this);
		return sstatus;
	}

	//exit and put offline
	if(scomm == sCommands[6]){
		outmess_len = iSendSocket_imp->Send(sCommands[6].c_str());
		if( outmess_len <0 )
		{
			MessageSignal((void*)this);
			return false;
		}
		inmess_len = iSendSocket_imp->Recv(commbuf, bufsize);
		if(inmess_len)
		{
			ack_mess = commbuf;
			comm = kC_EXIT;
			sstatus = true;
		}
		else
		{
			ack_mess = sCommands[6];
			ack_mess += Comm_Not_Acknow;
			sstatus = false;
		}
		MessageSignal((void*)this);
		return sstatus;
	}

	return false;
}

int TControl::StartDaqPCThread()
{
	if(!thread_DaqPC)
	{
		thread_DaqPC = new TThread("DAQPCThread", (void*(*)(void *))(&TControl::recv_TCPcontrol_command), (void*)this);
		if(thread_DaqPC)
		{
			thread_DaqPC->Run();
			cout << thread_DaqPC->GetName() << " is runing..." << endl;
			return 1;
		}
		else
		{
			thread_DaqPC = 0;
			return 0;
		}
	}
	else
	{
		thread_DaqPC->Run();
	}
	return 1;
}

bool TControl::StartOnlinePCThread()
{
	if(!thread_OnlinePC)
	{
		thread_OnlinePC = new TThread("OnlinePCThread", (void*(*)(void *))(&TControl::recv_UDPBroadcontrol_command), (void*)this);
		if(thread_OnlinePC)
		{
			thread_OnlinePC->Run();
			cout << thread_OnlinePC->GetName() << " is running..." << endl;
			return (thread_OnlinePC->GetState() == TThread::kRunningState);
		}
		else
		{
			thread_OnlinePC = 0;
			return 0;
		}
	}
	else
	{
		return 0;
	}
}


bool TControl::StopOnlinePCThread()
{
	UDPwait_command = false;
	sleep(WaitSec);
	if(thread_OnlinePC)
	{
		string thname = thread_OnlinePC->GetName();
		int state = TThread::Delete(thread_OnlinePC);
		delete thread_OnlinePC;
		thread_OnlinePC = 0;
		cout << thname << " stopped." << endl;
		return true;
	}
	return true;
}


int TControl::StartConPCRecErrThread()
{
	if(!thread_ConPCRevErr)
	{
		thread_ConPCRevErr = new TThread("ControlPCRevErr", (void*(*)(void*))(&TControl::recv_UDPBroadMessages), (void*)this);
		if(thread_ConPCRevErr)
		{
			thread_ConPCRevErr->Run();
			cout<< thread_ConPCRevErr->GetName() << " is running..." << endl;
			return 1;
		}
		else
		{
			thread_ConPCRevErr = 0;
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

void TControl::StopConPCRecErrThread()
{
	sleep(WaitSec);
	if(thread_ConPCRevErr)
	{
		string thname = thread_ConPCRevErr->GetName();
		TThread::Delete(thread_ConPCRevErr);
		delete thread_ConPCRevErr;
		thread_ConPCRevErr =0;
		cout << thname << " stopped." << endl;
	}
}

int TControl::BroadMessage(std::string message)
{
	return CommandForward(message);
}

// Signal function used to Emit a 'ack_mess' changed signal
void TControl::MessageSignal(void *arg)
{
	TControl *pthis = (TControl *)arg;
	//if(pthis) cout << "pthis value ok "<< endl;
	//pthis->Emit("MessageSignal(void *)");
	pthis->Emit("MSignalEmit()");
	//cout << "here..."<<endl;
}

void TControl::BroadMessageSignal(void *arg)
{
	TControl *pthis = (TControl *)arg;
	pthis->Emit("BMSignalEmit()");
}

void TControl::MSignalEmit(){}

void TControl::BMSignalEmit(){}

bool TControl::GetTCPSendSocketState()
{
	if(iSendSocket_imp) return iSendSocket_imp->IsValid();
	return false;
}

bool TControl::ClosTCPimpSocket()
{
	if(iSendSocket_imp)
	{
		delete iSendSocket_imp;
		iSendSocket_imp = 0;
	}
	return true;
}

void TControl::ControlSignal(void *arg, int comm)
{
	TControl *pthis = (TControl *)arg;
	pthis->Emit("CommandSignal(int)", comm);
}

void TControl::CommandSignal(int comm){};

void TControl::printsignal()
{
	cout << ".....Got Signal....." << endl;
}
