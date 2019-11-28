////////////////////////////////////////////////
// VMEDAQPC.cpp: main() of DAQ. Using
// a thread to receive control commands
// form 'control pc', and then perform
// it.
// Han Jianlong (08/2012)
// Ver 0.2 : added "localhost" for singal PC
//     by Han jianlong (06/2013)
////////////////////////////////////////////////

#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <cstdlib>
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
#include "TDAQPCTask.h"

#ifdef WIN32
#include <windows.h>
#define sleep Sleep
#define WaitSec 1000
#else
#include <unistd.h>
#define WaitSec 0.3
#endif

int main(int argc, char* argv[])
{
	TApplication *app = new TApplication("Control_PC", &argc, argv, 0, 1);

	int status=-1;
	int MasterCrate_Virtual = 1;  //Master Crate Number
	bool okCBLT, okVME;
	vector <TVMELink> VMELink;

	// The Server DAQ Application defining general environment
	TDAQApplication ribll(argc, argv, false);
	//where the cblt setup files are 
	string pathchain = ribll.Get_PathEnvdir() + "/cblt_addr_crate";
	//where the filenameqdc and codifier setup file are
	string pathfilename = ribll.Get_PathEnvdir() + "/filenamemod.dat";

	// define bridge and crate parameters
	// Control_Board_type, pci_device_link, control_board_num, virtual_crate_num
	VMELink.push_back(TVMELink(cvV2718, 0, 0, 1));  //Crate 1, vs file 'cblt_addr_crate1.dat'
	//VMELink.push_back(TVMELink(cvV2718, 0, 1, 2));  //Crate 2, vs file 'cblt_addr_crate2.dat'
	//VMELink.push_back(TVMELink(cvV2718, 0, 2, 3));  //Crate 3 vs file 'cblt_addr_crate3.dat'
	MasterCrate_Virtual = 1;  //virtual_crate_num for the master crate;

	// Initialize bridges and IO ports
	okVME=true;
	for(unsigned int i=0; i<VMELink.size(); i++) {
		status = VMELink[i].Init();
		if(status!=0) {
			okVME=false;
			cout<<"Initialization failed for crate "<<VMELink[i].GetCrateNum()<<
				"  [Dev="<<VMELink[i].GetDevice()<<",Link="<<VMELink[i].GetLink()<<"]"<<endl;
		}
		else {
			//Set IO port for output signals on channel 0,1,2,3
			//channel 0: cpu busy
			//channel 1: trigger start/stop 
			//channel 2: pulse to end program without trigger			 
			VMELink[i].InitIOPort(cvOutput0, cvDirect, cvActiveHigh);
			VMELink[i].InitIOPort(cvOutput1, cvDirect, cvActiveHigh);
			VMELink[i].InitIOPort(cvOutput2, cvDirect, cvActiveHigh);
			VMELink[i].InitIOPort(cvOutput3, cvDirect, cvActiveHigh);
			VMELink[i].InitIOPort(cvOutput4, cvDirect, cvActiveHigh);
			VMELink[i].ClearIOPort(cvOut0Bit);
			VMELink[i].ClearIOPort(cvOut1Bit);
			VMELink[i].ClearIOPort(cvOut2Bit);
			VMELink[i].ClearIOPort(cvOut3Bit);
			VMELink[i].ClearIOPort(cvOut4Bit);
		}
	}

	okCBLT = true;
	vector <TCrateCBLT> tcrate;               //Crate CBLT implementation
	for(unsigned int i=0; i<VMELink.size(); i++) {
		int WaitForDReady = 0;
		if(VMELink[i].GetCrateNum() == MasterCrate_Virtual) WaitForDReady = 1; // this crate wait for the data ready--IRQ
		TCBLT tread(8192, WaitForDReady);        //cblt buffer in bytes and wait_for_ready boards
		TCrateCBLT tcr(VMELink[i], tread, i, pathchain.c_str());
		//status = tcr.Get_Readout().Get_CBLT_Config(pathchain.c_str(), tcr.GetCrateNum());
		//tcr.ReadoutProcsInit();               //do not delete this line
		status = tcr.GetCBLTConfigStatus();
		if(status!=0) {
			okCBLT=false;
			cout<<"CBLT Initialization failed for crate "<<VMELink[i].GetCrateNum()<<endl;
		} 
		else {
			tcrate.push_back(tcr);
			tcrate[i].Get_Readout().PrintCBLTInfo();
		}		
	}

	if( !(okVME && okCBLT) )
	{
		if(argc>1)
		{
			if(strcmp(argv[1], "checkconfig") != 0) exit(0); //only for checking the vme models config
		}
		else
		{
			exit(0);
		}
	}

	VMELink.clear();

	//Getting the main list of module configrations
	//Initialization all of the boards
	TConfig Codif(pathfilename.c_str());
	status = Codif.GetConfigNames();    //Read the file '/filenamemod.dat'
	if(status<0) {
		cout<<"Fatal error reading configuration file"<<endl;
		exit(1);
	}
	else
	{
		Codif.Init_Caen_Boards(kCheckOnly); //ReadConfigForAnalysis();
	}

	TThread *Daqthread =0, *DaqControlthread = 0;

	string serverip;
	bool bgetip = ribll.Get_ServerIP(serverip);
	if(!bgetip)
	{
		cout << "Get Server IP error!" << endl;
		exit(1);
	}
	cout << "VME Control PC IP: " << serverip.c_str() << endl;
	//control pc ip, config, crate vector, mastercrate
	TDAQPCTask daqpctask(serverip.c_str(), Codif, tcrate, MasterCrate_Virtual);//192.0.0.17

	//Join() the VMEReadThread
	DaqControlthread = daqpctask.StartDAQPCControlThread(); //start the rec_TCP_Command thread
	Daqthread = daqpctask.StartDAQThread();                 //start the VME readout thread
	if(Daqthread) 
	{
		Daqthread->Join();
	}
	else
	{
		cout << "DAQThread run error!" << endl;
	}
	//Join the control thread - DAQPCThread
	if(DaqControlthread) DaqControlthread->Join();

	cout << "VMEDAQPC finished, Exit..." << endl;
	exit(0);
	app->Run();
	return 0;
}
