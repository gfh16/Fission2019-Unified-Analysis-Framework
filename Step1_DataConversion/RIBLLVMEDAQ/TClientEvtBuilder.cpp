////////////////////////////////////////////////
// TEvtBuilder.cpp: Class to perform the VME 
// reading and build the 'event' structure.
// Use TCrateCBLT to loop reading procedure, 
// and then build the event in memory, then 
// send it out to ethernet by using UDP 
// socket.
// Han Jianlong 07/2012
////////////////////////////////////////////////

#include "TUDPClientSocket.h"
#include "TClientEvtBuilder.h"
#include "TControl.h"
#include "TCrateCBLT.h"
#include "caenacq.h"

#include <iostream>
#include <string>
using namespace std;
#include "TString.h"
#include "TThread.h"

#ifdef WIN32
#include <windows.h>
#define sleep Sleep
#define WaitSec 400
#else
#include <unistd.h>
#define sleep usleep
#define WaitSec 300000
#endif

TUDPClientSocket *TClientEvtBuilder::DataBroadUDPsock = 0;

extern int VMEReadBeforeCBLT(unsigned int* &buff, TConfig &gconfig);
extern int VMEReadAfterCBLT(unsigned int* &buff, TConfig &gconfig);

TClientEvtBuilder::TClientEvtBuilder(std::string addr, string udptype, TConfig &cod, std::vector<TCrateCBLT> &tcrate,
									 unsigned int mastercrate):TEvtBuilder(cod,tcrate, mastercrate)
{
	DAQThread = 0;
	if(udptype == "MULTICAST")
	{
		DataBroadUDPsock = new TUDPClientSocket(addr.c_str(), UDPDataBroadPortCon, true);
	}
	else if(udptype == "IPLIST")
	{
		DataBroadUDPsock = new TUDPClientSocket(addr.c_str()); // '10' is meaningless figure
	}
	else
	{
		//Do not used anymore

		//TString baddr = addr;
		//int pos = baddr.Last('.');
		//baddr.Replace(pos+1, 3, "255");
		//DataBroadUDPsock = new TUDPClientSocket(addr.c_str(), UDPDataBroadPortCon);
	}
	if(!DataBroadUDPsock->IsValid()) cout << " TClientEvtBuilder>> DataBroadUDPsocket error! " << endl;
	DataBroadUDPsock->SetSendTimeOutValue(200); //set sendto timeout
}

TClientEvtBuilder::~TClientEvtBuilder()
{
	SafeDeleteP(DataBroadUDPsock);
}

int TClientEvtBuilder::CheckErrors(std::string ferror, int merr)
{
	string errmsg;

	int num=fconfig.GetErrnum();
	if(num !=0) {
		for(int i=0; i<num; i++) {
			if(i>merr)break;
			errmsg = ferror + fconfig.GetErrInfo(i);
			cout << errmsg.c_str() << endl;
			//TControl::BroadMessage(errmsg);
		}
	}
	return num;
}

void* TClientEvtBuilder::EventBuilderRun(void *arg)
{
	// arg is a value of 'this' pointer, used to make this
	// 'static' function to visit the 'unstatic' variables
	TEvtBuilder *pthis = (TEvtBuilder *)arg;
	vector <TCrateCBLT> & fcrate = pthis->GetCrateCBLT();
	TConfig &fconfig = pthis->GetConfig();
	unsigned int fmastercrate = pthis->fmastercrate;

	enum kmsg {INIT, START, STOP, CLOSE, EXIT};
	string ferror="File_Error: Init  ";
	//string werror="20 W_Error";
	unsigned int *curr_ptr;
	string curr_header, errmsg;
	int const maxmsg = 2;
	int absize = 0;
	int status = 0;

	const char *msg[ ] =
	{
		"Initialization ",
		"Start Acquisition ",
		"Stop of Acquisition ",
		"End Of File ",
		"End Of Program ",
		"  "
	}; 

	ECommands comm; 

	for (int i=0; i<fnumcrates; i++) {
		fcrate[i].ReadoutProcsInit();
	}

	while(true) { //control loop, wait for commands
		{ // wait for command, ie the change of TControl::comm
			TControl::comm = kC_WAIT;
			comm = TControl::comm;
			while(comm == kC_WAIT)
			{
				sleep(WaitSec);
				comm = TControl::comm;
			}
			if(comm == kC_EXIT) break;
		} 

		// INITIALIZATION 
		if(comm == kC_INIT) {
			//master IO-port initialization 
			fcrate[fmastercrate].ClearIOPort(cvOut0Bit); //1st,
			fcrate[fmastercrate].SetIOPort(cvOut1Bit);   //2nd, stop trigger(veto)
			fcrate[fmastercrate].ClearIOPort(cvOut2Bit); //3rd, clear 'stop program trig'
			fcrate[fmastercrate].PulseOutput(cvOut0Bit); //4th, clear VME reading busy

			fconfig.GetConfigNames();
			fconfig.Init_Caen_Boards(kInitReg);
			int errn = pthis->CheckErrors(ferror, maxmsg);  //local readout errors
			if(errn == 0) {
				//TControl::BroadMessage(msg[INIT]);
				cout<<"Initialization OK!"<<endl;
			}
			else {
				//TControl::BroadMessage("Initialization Terminated with Errors.");
				cout<<"Initialization Terminated with Errors"<<endl;
			}
		}

		// start acquisition
		if(comm == kC_START) {
			//master IO-port initialization 
			fcrate[fmastercrate].ClearIOPort(cvOut0Bit);  //1st,
			fcrate[fmastercrate].SetIOPort(cvOut1Bit);    //2nd, stop trigger
			fcrate[fmastercrate].ClearIOPort(cvOut2Bit);  //3rd, clear 'stop program trig'
			fcrate[fmastercrate].PulseOutput(cvOut0Bit);  //4th, clear VME reading busy

			fconfig.GetConfigNames();
			fconfig.Init_Caen_Boards(kInitReg);
			int errn = pthis->CheckErrors(ferror, maxmsg);
			if((errn) == 0) {
				//TControl::BroadMessage(msg[START]);
				cout<<"Startup OK. Running now..."<<endl;
				do{
					status = fcrate[fmastercrate].ClearIOPort(cvOut1Bit);  //start trigger
				}while(status != 0);
			}
			else {
				cout<<"Startup Failed: "<<errn<<" error(s)"<<endl;
				continue;
			}
			//fcrate[fmastercrate].SetIOPort(cvOut0Bit);  //test only
			sleep(WaitSec);
			pthis->fokstop=false;
			fevent_counter = 0;

			curr_ptr = fnetbuf; //init curr_ptr
			int buflen = 0;
			while(true)//waiting for readout tasks or stop command
			{
				// reset dead time: End of VME Reading Busy
				do{
					status = fcrate[fmastercrate].PulseOutput(cvOut0Bit);
				}while(status != 0);

				if(CheckStop()) 
				{
					comm = kC_STOP;
					break;
				}

				int tot = 0;
				int res = 0;
				while(tot!=fnumcrates) 
				{ 
					tot = 0;
					res = 0;
					for(int i=0; i<fnumcrates; i++) {
						res = fcrate[i].WaitForReady();
						tot += res;
					}
					//need to catch stop during data-ready
					if(CheckStop()) 
					{
						comm = kC_STOP;
						break;
					}
				}

				if(tot==fnumcrates) {
					
					// local buffer header construction 
					fevent_counter++;
					if(fevent_counter == 0x10000000) fevent_counter=0;

					// Write the event separator and eventnumber;
					*(curr_ptr) = Event_Header;
					curr_ptr++;
					buflen += sizeof(int); //buflen in byte, same as buflen += 4;

					*(curr_ptr) = fevent_counter;
					curr_ptr++;
					buflen += sizeof(int); //buflen in byte, same as buflen += 4;

					//Before main CBLT read
#ifdef UserVMEFunctionBefore //define in caenacq.h
					absize = VMEReadBeforeCBLT(curr_ptr, fconfig);
					buflen += absize;
#endif

					//main readout loop (asyncronous readout)
					for(int i=0; i<fnumcrates; i++) {  
						if(fcrate[i].GetReadoutEnable()) {
							fcrate[i].Readout();
						}
					}

					//After main CBLT read
#ifdef UserVMEFunctionAfter //define in caenacq.h
					absize = VMEReadAfterCBLT(curr_ptr, fconfig);
					buflen += absize;
#endif

#ifdef Wait_Data_Ready_IRQ
					//enable IRQ Lines of master crate V2718 after the all of VME read process
					//int eirqtot = 0;
					//int eirqres = 0;
					//while(eirqtot!=fnumcrates) 
					//{ 
					//	eirqtot = 0;
					//	eirqres = 0;
					//	for(int i=0; i<fnumcrates; i++) {
					//		eirqres = fcrate[i].EnableIRQ();
					//		eirqtot += eirqres;
					//	}
					//}
					//fcrate[fmastercrate].EnableIRQ();
#endif //#ifdef Wait_Data_Ready_IRQ

					
					// local buffer reconstruction 
					int size;
					for (int i=0; i<fnumcrates; i++) { 
						if(fcrate[i].GetReadoutEnable()) {
							size = (fcrate[i].Get_Readout()).GetUsedSizeByte();
							memcpy(curr_ptr, (fcrate[i].Get_Readout()).GetBLTBuff(), size);
							curr_ptr += size>>2;
							buflen+=size;
						}
					}
					
					//broadcast data to Ethernet
					if(buflen >= MAX_NET_LEN) {
						curr_ptr = fnetbuf; //reset curr_ptr
						int out_len = DataBroadUDPsock->SendTo((const char *)curr_ptr, buflen);
						//if (out_len < buflen) TControl::BroadMessage(" Data send out error. Out<source ! ");
						//cout << "Send out length: " << out_len << " Real length: " << buflen << endl;
						buflen = 0; //reset buflen
					}
				}
			} //start loop  
		}  //if start 

		if(comm==kC_FOPEN) {
			continue;
		}

		if(comm==kC_FCLOSE) {
			//TControl::BroadMessage(msg[CLOSE]);
			continue;
		}

		//stop acquisition
		if(comm == kC_STOP) {
			fcrate[fmastercrate].ClearIOPort(cvOut0Bit);  //1st,
			fcrate[fmastercrate].SetIOPort(cvOut1Bit);    //2nd, stop trigger(veto trigger)
			fcrate[fmastercrate].ClearIOPort(cvOut2Bit);  //3rd, sclear 'stop program trig'
			fcrate[fmastercrate].PulseOutput(cvOut0Bit);  //4th, clear VME reading busy
			pthis->fokstop=true;
			cout<<" Stop acquisition. "<<endl;
			continue;
		}

		//exit acquisition    
		if(comm == kC_EXIT) {
			for(int i=0; i<fnumcrates; i++) {
				fcrate[i].EndVMEHandle();
			}
			break;
		}
	}   //event builder infinite loop
	cout << "EventBuilderRun() Stopped." << endl;
	return 0;
}

//if there is no trigger the 'Stop' command may not work,
//because the program is blocked to 'WaitForIRQ'. 
//this function receive the 'MessageSignal(void *arg)' 
//and then use output-2 of V2718 to gernerate a trigger,
//in this way the program can breakout form the 'loop' and
//then 'Stop'

void TClientEvtBuilder::ProcessStopSignal()
{
	//	cout << " Process Signal... " << endl;
	if(CheckStop())
	{
		fcrate[fmastercrate].PulseOutput(cvOut2Bit);    //trigger to 'stop' the program
	}
}

void TClientEvtBuilder::StartDAQThread()
{
	if(!DAQThread)
	{
		DAQThread = new TThread("VMEReadThread", (void*(*)(void*))&TClientEvtBuilder::EventBuilderRun, (void*)this);
		if(DAQThread)
		{
			DAQThread->Run();
			cout << DAQThread->GetName() << " runing... " << endl;
			return;
		}
		else
		{
			DAQThread = 0;
			return;
		}
	}
	else
	{
		DAQThread->Run();
		return;
	}
}

TThread* TClientEvtBuilder::GetDAQThread()
{
	if(DAQThread)
	{
		TThread::EState stat = DAQThread->GetState();
		if(stat == TThread::kRunningState || stat == TThread::kNewState)
			return DAQThread;
	}
	return 0;
}
