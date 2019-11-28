//Test main

#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
using namespace std;

#include "TUDPClientSocket.h"  //must be the first 'include'
#include "TDAQApplication.h"
#include "TControl.h"
#include "TVMELink.h"
#include "TConfig.h"
#include "TCBLT.h"
#include "TCrateCBLT.h"
#include "TUDPServerSocket.h"
#include "TBoard.h"
#include "TClientEvtBuilder.h"
#include "TDataFileBuilder.h"
#include "TThread.h"
#include "TSystem.h"
#include "TApplication.h"

#ifdef WIN32
#include <windows.h>
#define sleep Sleep
#define WaitSec 1000
#else
#include <unistd.h>
#define WaitSec 0.3
#endif

// show sCommands here
//string sCommands[] = 
//{
//	"TEST CONNECTION",		/* CMD = 0 */
//	"Initialize VME",       /*       1 */ 
//	"Start Acquisition",    /*       2 */ 
//	"Stop Acquisition",     /*       3 */ 
//	"Open Tape File",       /*       4 */ 
//	"Close Tape File",      /*       5 */  
//	"Exit and Put offline"  /*       6 */ 
//};


int main(int argc, char*argv[])
{
	TApplication *app = new TApplication("Control_PC", &argc, argv, 0, 1);

	int status;
	bool okCBLT;
	vector <TVMELink> VMELink;
	bool IsMaster = true;  //this is the acquisition master 

	// The Server DAQ Application defining general environment
	TDAQApplication ribll(argc, argv, IsMaster);
	//where the cblt setup files are 
	string pathchain = ribll.Get_PathEnvdir() + "/cblt_addr_c";
	//where the filenameqdc and codifier setup file are
	string pathfilename = ribll.Get_PathEnvdir() + "/filenamemod.dat";

	// define bridge and crate parameters
	// bridge type, link, node, virtual crate
	VMELink.push_back(TVMELink(cvV2718, 0, 0, 1));  //Crate 1 
	VMELink.push_back(TVMELink(cvV2718, 0, 1, 2));  //Crate 2
	//VMELink.push_back(TVMELink(cvV2718, 0, 2, 3));  //Crate 2

	okCBLT = true;
	vector <TCrateCBLT> tcrate;
	for(unsigned int i=0; i<VMELink.size(); i++) {
		TCBLT tread(8192, 1);   //cblt buffer in bytes and wait_for_ready boards
		TCrateCBLT tcr(VMELink[i], tread, i, pathchain.c_str());
		status = tcr.Get_Readout().Get_CBLT_Config(pathchain.c_str(), tcr.GetCrateNum());
		tcr.ReadoutProcsInit();  //do not delete this line
		if(status!=0) {
			okCBLT=false;
			cout<<"CBLT Initialization failed for crate "<<VMELink[i].GetCrateNum()<<endl;
		} 
		else {
			tcrate.push_back(tcr);
			tcrate[i].Get_Readout().PrintCBLTInfo();
		}		
	} 
	//	if(okCBLT)gCheck.SetCBLTStatus(true);
	VMELink.clear();

	////Getting the main list of module configrations  
	TConfig Codif(pathfilename.c_str());
	status = Codif.GetConfigNames();
	if(status<0) {
		cout<<"Fatal error reading configuration file"<<endl;
		exit(1);
	}
	else
	{
		Codif.ReadConfigForAnalysis();
	}

	char buf[100] = {'a','s','d','f'};
	string comm = "stringcomm";
	string comm1;

	//TClientEvtBuilder EvtB("90.0.6.103", Codif, tcrate, 1);

	//TUDPClientSocket databroad("192.168.1.255", UDPDataBroadPort);
	//int i = 0;
	//while(i<5)//(fgets(buf, sizeof(buf), stdin) != NULL)
	//{
	//	i++;
	//	int len = strnlen(buf, sizeof(buf));
	//	databroad.SendTo(comm.c_str(), comm.size());
	//  if(i==4) databroad.SendTo("stop", 4);
	//}
	//TUDPServerSocket revdata(UDPDataBroadPort);
	//while (strncmp(buf, "stop", 4) !=0)
	//{
	//	int n = revdata.RecvRaw(buf, 100);
	//	//buf[n-1] = '\0';
	//	comm1 = buf;
	//	cout <<"TUDPServerSocket: " << comm1 << "  " << n << " bytes." << endl;
	//}

	TControl controlmaster("90.0.6.103", CONTROL_PC);
	controlmaster.AcceptReceiver();
	controlmaster.SetFileHeader("..Test File Header..");
	for(int i=0; i<6; i++)
	{
		cout << "Sending command: " << sCommands[i] << endl;
		controlmaster.send_TCPcontrol_command(sCommands[i]);
		cout << "Reply: " << controlmaster.GetAckMessages() << endl;
	}

	//TThread *DaqPC_revComm = 0;
	//TControl controldaq("90.0.6.103", DAQ_PC);
	//DaqPC_revComm = new TThread("DaqPC_WaitComm", (void*(*)(void *))(&TControl::recv_TCPcontrol_command), 0);
	//DaqPC_revComm->Run();
	//DaqPC_revComm->Join();
	//controldaq.StartDaqPCThread();
	//(controldaq.GetDaqPCThread())->Join();

	//TControl controlonline("90.0.6.103", ONLINE_PC);
	//controlonline.Connect("MessageSignal(void *)", "TClientEvtBuilder", &EvtB, "ProcessStopSignal()");
	//controlonline.recv_UDPBroadcontrol_command(0);
	//if( controlonline.StartOnlinePCThread())//{cout << " Thread runing..." << endl;} //;
		//(controlonline.GetOnlinePCThread())->Join();//Ps();
	//else cout << " Online thread fail. " << endl;
	//cout << " I am here... " << endl; 
	//cout << "UDP Reply: " << controlonline.GetAckMessages() << endl;

	//TDataFileBuilder file;
	//file.SetFileHeader("File header.");
	//file.SetFileName("test");
	//file.SetRunNum(2);
	//file.FormBroadFHeader();
	//file.ExtractFNameNumFromHeader();
	//file.OpenDataFile();

	//file.StartWriteDataThread();
	////sleep(1000);
	//file.StopWriteDataThread();
	////sleep(1000);
	//file.StartWriteDataThread();
	////sleep(1000);
	//file.StopWriteDataThread();
	//file.CloseDataFile();
	//cout << " I am here.. " << endl;
	//for(int i=0; i<5; i++)	file.OpenDataFile();


	app->Run();
	return 0;
}