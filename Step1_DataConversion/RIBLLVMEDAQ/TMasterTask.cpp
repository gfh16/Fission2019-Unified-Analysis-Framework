/////////////////////////////////////////////////////////
// File name: TMasterTask.cpp                          //
// Brief introduction:                                 //
//       Master PC task: Control daq, get data form    //
//       UDP broadcast, save data to file ...          //
// Version: V1.0                                       //
// Author: Han Jianlong                                //
// Date: Aug. 2012                                     //
// For: RIBLL1                                         //
/////////////////////////////////////////////////////////

#include <iostream>
#include <string>
using namespace std;

#include "TDataFileBuilder.h"
#include "TControl.h"
#include "TGClient.h"
#include "TGLabel.h"
#include "TThread.h"
#include "TTimer.h"
#include "TTimer.h"
#include "TGTextEntry.h"
#include "TString.h"
#include "TGNumberEntry.h"
#include "TGButton.h"
#include "TSystem.h"
#include "TMasterTask.h"

#ifdef WIN32
#include <windows.h>
#define sleep Sleep
#define WaitSec 1000
#else
#include <unistd.h>
#define sleep usleep
#define WaitSec 1000000
#endif

ClassImp(TMasterTask);

TMasterTask *gMasterTask=0;
TTimer *TMasterTask::showevtimer=0;
TTimer *TMasterTask::showconnect=0;
TTimer *TMasterTask::checkdfsize=0;
bool    TMasterTask::TCPConnState = false;
bool    TMasterTask::TCPAcceptReturned = false;
const int msec_singleshot = 2;

TMasterTask::TMasterTask(TControl &netcon):TControlFrame(gClient->GetRoot(), 550, 450), mascontrol(netcon)
{
	if(gMasterTask)
	{
		cout <<"Only one instance of 'TMasterTask' permitted. "<< endl;
		return;
	}
	gMasterTask = this;
	fFileBuilder = new TDataFileBuilder();
	maxfilesize = fFileBuilder->GetMaxFileSizeMb();

	thispcaction = mascontrol.GetPCAction();

	if(thispcaction == CONTROL_PC)
	{
		InitButt();
	}
	else
	{
		SetOnlinePCButt();
	}

	Dfilestate = false;
	DaqRunning = false;

	//thshowevt    = 0;
	thtcpaccept  = 0;

	showevtimer = new TTimer(2000);
	showevtimer->Connect("Timeout()", Class_Name(), this, "ShowEventNumByTimer()");
	showconnect = new TTimer(500);
	showconnect->Connect("Timeout()", Class_Name(), this, "ProcessTCPConnectedTimer()");
	checkdfsize = new TTimer(20000);
	checkdfsize->Connect("Timeout()", Class_Name(), this, "RecrateDFileAuto()");


	if(thispcaction == CONTROL_PC)
	{
		mascontrol.Connect("MSignalEmit()",  Class_Name(), this, "ShowMessageSignal()");
		mascontrol.Connect("BMSignalEmit()", Class_Name(), this, "CoutBroadMessageSignal()");
	}
	else  
	{
		//DAQPC: Connect message to stdio, because signal from thread will cause error if connect to GUI fuctions
		mascontrol.Connect("MSignalEmit()",  Class_Name(), this, "CoutBroadMessageSignal()");
	}
	mascontrol.Connect("CommandSignal(int);", Class_Name(), this, "ProcessControlSignal(int)");
}

TMasterTask::~TMasterTask()
{
	delete showevtimer;
	delete fFileBuilder;
	//if(thshowevt != NULL)
	//{
	//	TThread::Delete(thshowevt);
	//	thshowevt = 0;
	//}
	if(thtcpaccept != NULL)
	{
		TThread::Delete(thtcpaccept);
		thtcpaccept = 0;
	}
}

void TMasterTask::ShowEventNumByTimer()
{
	//cout << "Timer ..." << endl;
	static int oldevtnum = 0; 
	int evtnum = TDataFileBuilder::eventcounter;
	if(evtnum != oldevtnum)
	{
		oldevtnum = evtnum;
		TThread::Lock();
		ShowEventNum(evtnum);
		TThread::UnLock();
	}
}

void TMasterTask::ImB_SetFileName()
{
	TThread::Lock(); //lock the main mutex
	TString name(fTEfilename->GetText());
	TThread::UnLock(); //unlock the main mutex
	if(name.Length()>0)
	{
		if(name.IsWhitespace())
		{
			name.Clear();
			name = "RawData";
		}
		name.ReplaceAll(" ", "");
		TThread::Lock(); //lock the main mutex
		fFileBuilder->SetFileName(name.Data());
		TString text = "Set File Name: ";
		text += name;
		ShowText(text.Data());
		TThread::UnLock(); //unlock the main mutex
		fFileBuilder->SetRunNum(0); //reset the runnumber to 0
		TThread::Lock();   //lock the main mutex
		fTEfilerun->SetNumber(0);
		TThread::UnLock(); //unlock the main mutex
	}
}

void TMasterTask::ImB_SetFileRun()
{
	TThread::Lock();   //lock the main mutex
	unsigned int num = (unsigned int)fTEfilerun->GetNumber();
	TThread::UnLock(); //unlock the main mutex
	if(num>=0)
	{	
		TThread::Lock(); //lock the main mutex
		fFileBuilder->SetRunNum(num);
		TString text = "Set File RunNum: ";
		text += num;
		ShowText(text.Data());
		TThread::UnLock(); //unlock the main mutex
	}
}

void TMasterTask::ImB_SetFileHeader()
{
	TThread::Lock(); //lock the main mutex
	TString header(fTEfileheader->GetText());
	if(header.Length()>0)
	{
		fFileBuilder->SetFileHeader(header.Data());
		ShowText("Set File Heaer OK.");
	}
	TThread::UnLock(); //unlock the main mutex
}

bool TMasterTask::ProcessMessage(Long_t msg, Long_t param1, Long_t)
{
	switch (GET_MSG(msg))
	{
	case kC_COMMAND:
		switch(GET_SUBMSG(msg))
		{
		case kCM_BUTTON:
			switch (param1)
			{
			case kB_setfilename:
				ImB_SetFileName();
				break;
			case kB_setfilerun:
				ImB_SetFileRun();
				break;
			case kB_setfileheader:
				ImB_SetFileHeader();
				break;
			case kB_start:
				if(thispcaction == CONTROL_PC) ImB_StartDaq();
				if(thispcaction == ONLINE_PC ) ImB_StartOnlineDaq();
				break;
			case kB_stop:
				ImB_StopDaq();
				break;
			case kB_initdaq:
				ImB_InitDaq();
				break;
			case kB_openf:
				ImB_OpenFile();
				break;
			case kB_closef:
				ImB_CloseFile();
				break;
			case kB_exitcon:
				CloseWindow();
				break;
			case kB_connectodaq:
				ImB_Connect();
				break;
			case kB_testcomm:
				ImB_TestComm();
				break;
			case kB_exitdaq:
				ImB_ExitDaqpc();
				break;
			default:
				break;
			}
			break;
		}
		break;

	default:
		break;
	}

	return true;
}

void TMasterTask::ShowMessageSignal()
{
	TThread::Lock();
	string mess = mascontrol.GetAckMessages();
	//ShowText(mess.c_str());
	cout << mess.c_str() << endl;
	TThread::UnLock();
}

void TMasterTask::SetEnableAllButt(bool act)
{
	TThread::Lock(); //lock the main mutex
	//fBSetFileName->SetEnabled(act);
	//fBSetFileRun->SetEnabled(act);
	//fBSetFileHeader->SetEnabled(act);
	fBStart->SetEnabled(act);
	fBStop->SetEnabled(act);
	fBInitDAQ->SetEnabled(act);
	fBOpenF->SetEnabled(act);
	fBCloseF->SetEnabled(act);
	//fBConExit->SetEnabled(act);
	fBConnect->SetEnabled(act);
	fBTestComm->SetEnabled(act);
	fBDaqExit->SetEnabled(act);
	TThread::UnLock(); //unlock the main mutex
}

void TMasterTask::InitButt()
{
	TThread::Lock(); //lock the main mutex
	fBStart->SetEnabled(kFALSE);
	fBStop->SetEnabled(kFALSE);
	//fBInitDAQ->SetEnabled(kFALSE);
	fBOpenF->SetEnabled(kFALSE);
	fBCloseF->SetEnabled(kFALSE);
	fBTestComm->SetEnabled(kFALSE);
	fBDaqExit->SetEnabled(kFALSE);
	TThread::UnLock(); //unlock the main mutex
}

void* TMasterTask::ConnecttoDAQPC(void *arg)
{
	TMasterTask *pthis = (TMasterTask *)arg;
	TControl &thismascontrol = pthis->GetControlMas();
	TCPAcceptReturned = false;
	TCPConnState = false;
	showconnect->TurnOn();
	TCPConnState = thismascontrol.AcceptReceiver();
	TCPAcceptReturned = true;
	//cout << "Accept thread: " << conn << endl;
	//EmitTCPConnectedSignal(arg);
	return 0;
}

bool TMasterTask::ImB_Connect()
{
	if(mascontrol.GetTCPSendSocketState())  //Control_PC and DAQ_PC is connected
	{
		TThread::Lock();
		ShowText("Connection between Control_PC and DAQ_PC is OK.");
		TThread::UnLock();
		return true;
	}

	SetEnableAllButt(kFALSE);
L100:
	if(!thtcpaccept)
	{
		thtcpaccept = new TThread("TCPACCEPT", (void*(*)(void*))(&TMasterTask::ConnecttoDAQPC), (void*)this);
		if(thtcpaccept)
		{
			thtcpaccept->Run();
			TThread::Lock();
			ShowText("Wait for connection from DAQ_PC...");
			TThread::UnLock();
			return true;
		}
		else
		{
			thtcpaccept = 0;
			return false;
		}
	}
	else
	{
		if(thtcpaccept->GetState() != TThread::kRunningState)  thtcpaccept->Run();
		TThread::Lock();
		ShowText("Wait for connection from DAQ_PC...");
		TThread::UnLock();
		if(thtcpaccept->GetState() == TThread::kInvalidState)
		{
			thtcpaccept = 0;
			goto L100;
		}
		return true;
	}

	//ConnecttoDAQPC(this);
	//return true;
}

void TMasterTask::ImB_TestComm()
{
	mascontrol.send_TCPcontrol_command(sCommands[0]);

}

void TMasterTask::ImB_InitDaq()
{
	mascontrol.send_TCPcontrol_command(sCommands[1]);
}

void TMasterTask::ImB_StartDaq()
{
	bool status = false;
	if(thispcaction==CONTROL_PC)
	{
		status = mascontrol.send_TCPcontrol_command(sCommands[2]);
		if(status)
		{
			TThread::Lock(); //lock the main mutex
			fBStart->SetEnabled(kFALSE);
			fBStop->SetEnabled(kTRUE);     //enable stop
			fBInitDAQ->SetEnabled(kFALSE);
			fBOpenF->SetEnabled(kFALSE);
			fBCloseF->SetEnabled(kFALSE);
			fBConExit->SetEnabled(kFALSE);
			fBConnect->SetEnabled(kFALSE);
			fBTestComm->SetEnabled(kFALSE);
			fBDaqExit->SetEnabled(kFALSE);
			//mascontrol.StartConPCRecErrThread();
			checkdfsize->TurnOn();
			TThread::UnLock(); //unlock the main mutex
		}
	}
}

void TMasterTask::ImB_StartOnlineDaq()
{
	TThread::Lock(); //lock the main mutex
	bool state = mascontrol.StartOnlinePCThread();
	if(state) 
	{
		fBStart->SetEnabled(kFALSE);
		ShowText("OnlinePCThread running...");
	}
	TThread::UnLock(); //unlock the main mutex
}

void TMasterTask::ImB_StopDaq()
{
	bool status = false;
	if(thispcaction==CONTROL_PC)
	{
		status = mascontrol.send_TCPcontrol_command(sCommands[3]);
		if(status)
		{
			TThread::Lock(); //lock the main mutex
			fBStart->SetEnabled(true);     //enable start
			fBStop->SetEnabled(kFALSE);     //disable stop
			fBInitDAQ->SetEnabled(true);
			if(!Dfilestate) fBOpenF->SetEnabled(true);
			if(Dfilestate)  fBCloseF->SetEnabled(true);
			if(!Dfilestate) fBConExit->SetEnabled(true);
			fBConnect->SetEnabled(true);
			fBTestComm->SetEnabled(true);
			if(!Dfilestate) fBDaqExit->SetEnabled(true);
			//mascontrol.StopConPCRecErrThread();
			checkdfsize->TurnOff();
			TThread::UnLock(); //unlock the main mutex
		}
	}
}

void TMasterTask::ImB_ExitDaqpc()
{
	bool status = mascontrol.send_TCPcontrol_command(sCommands[6]);
	if(status)
	{
		InitButt();
		mascontrol.ClosTCPimpSocket();
	}
}

void TMasterTask::ProcessTCPConnectedTimer()
{
	if(TCPAcceptReturned) //ConnecttoDAQPC() thread have returned
	{
		if(TCPConnState)
		{
			TThread::Lock(); //lock the main mutex
			SetEnableAllButt(true);
			DiableStopButt();
			DiableCloseButt();
			ShowText("OK! Control_PC and DAQ_PC connection established.");
			showconnect->TurnOff();
			TThread::UnLock(); //unlock the main mutex
		}
		else
		{
			TThread::Lock(); //lock the main mutex
			InitButt();
			ShowText("ERROR! Can NOT build connection between Control_PC and DAQ_PC.");
			showconnect->TurnOff();
			TThread::UnLock(); //unlock the main mutex
		}
	}
}

void TMasterTask::SetOnlinePCButt()
{
	TThread::Lock(); //lock the main mutex
	fBSetFileName->SetEnabled(kFALSE);
	fBSetFileRun->SetEnabled(kFALSE);
	fBSetFileHeader->SetEnabled(kFALSE);
	fBStart->SetEnabled(true);
	fBStop->SetEnabled(kFALSE);
	fBInitDAQ->SetEnabled(kFALSE);
	fBOpenF->SetEnabled(kFALSE);
	fBCloseF->SetEnabled(kFALSE);
	fBConExit->SetEnabled(true);
	fBConnect->SetEnabled(kFALSE);
	fBTestComm->SetEnabled(kFALSE);
	fBDaqExit->SetEnabled(kFALSE);
	TThread::UnLock(); //unlock the main mutex
}

void TMasterTask::CoutBroadMessageSignal()
{
	TThread::Lock();
	string mess = mascontrol.GetAckMessages();
	TThread::UnLock();
	cout << mess << endl;
}

bool TMasterTask::ProcessControlSignal(int comm)
{
	if(comm == kC_START)
	{
		TThread::Lock(); //lock the main mutex
		bool state  = fFileBuilder->StartWriteDataThread();
		TThread::UnLock(); //unlock the main mutex
		if(state)
		{
			showevtimer->TurnOn();
			DaqRunning = true;
			TThread::Lock();
			//ShowText("OK! Write Data to File 'Thread' Started.");
			SetTextLocal("OK! Write Data to File 'Thread' Started.");
			ShowTextLocal();
			//TTimer::SingleShot(msec_singleshot, Class_Name(), (void*)this, "ShowTextByTimer()");
			TThread::UnLock();
			return true;
		}
		else
		{
			TThread::Lock();
			//ShowText("ERROR! Start Write Data to File 'Thread' Fail.");
			SetTextLocal("ERROR! Start Write Data to File 'Thread' Fail.");
			ShowTextLocal();
			//TTimer::SingleShot(msec_singleshot, Class_Name(), (void*)this, "ShowTextByTimer()");
			TThread::UnLock();
			return false;
		}
	}
	if(comm == kC_STOP)
	{
		TThread::Lock(); //lock the main mutex
		bool state = fFileBuilder->StopWriteDataThread();
		TThread::UnLock(); //unlock the main mutex
		if(state)
		{
			showevtimer->TurnOff();
			DaqRunning = false;
			TThread::Lock();
			//ShowText("OK! Write Data to File 'Thread' Stopped.");
			SetTextLocal("OK! Write Data to File 'Thread' Stopped.");
			ShowTextLocal();
			//TTimer::SingleShot(msec_singleshot, Class_Name(), (void*)this, "ShowTextByTimer()");
			TThread::UnLock();
			return true;
		}
		return false;
	}
	if(comm == kC_FOPEN)
	{
		if(thispcaction == ONLINE_PC)
		{
			TThread::Lock(); //lock the main mutex
			//first: get the received File Header from TControl Online_PC
			TString getfheader = mascontrol.GetFileHeader();
			cout << "Broad file header: " << getfheader.Data() << endl;
			//Second: set the File Header to FileBuilder
			fFileBuilder->SetBroadFHeader(getfheader.Data());
			//Third: extract File Header
			fFileBuilder->ExtractFNameNumFromBHeader();
			//Open Data File
			Dfilestate = fFileBuilder->OpenDataFile();
			TThread::UnLock(); //unlock the main mutex
		}
		if(Dfilestate)
		{
			TString text;
			int rnum = fFileBuilder->GetCurrRunNum();
			text += rnum;
			text += "run File Opened.";
			TThread::Lock(); //lock the main mutex
			//ShowText(text.Data());
			SetTextLocal(text.Data());
			ShowTextLocal();
			//TTimer::SingleShot(msec_singleshot, Class_Name(), (void*)this, "ShowTextByTimer()");
			flabFileStatus->SetText(text.Data());
			TThread::UnLock(); //unlock the main mutex
			return true;
		}
		else
		{
			return false;
		}
	}
	if(comm == kC_FCLOSE)
	{
		bool state = fFileBuilder->CloseDataFile();
		if(state)
		{
			Dfilestate = false;
			TString text;
			int rnum = fFileBuilder->GetCurrRunNum();
			text += rnum;
			text += "run File Closed.";
			TThread::Lock(); //lock the main mutex
			//ShowText(text.Data());
			SetTextLocal(text.Data());
			ShowTextLocal();
			//TTimer::SingleShot(msec_singleshot, Class_Name(), (void*)this, "ShowTextByTimer()");
			flabFileStatus->SetText(text.Data());
			TThread::UnLock(); //unlock the main mutex
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

void TMasterTask::ImB_OpenFile()
{
	if(thispcaction==CONTROL_PC)
	{
		//first step build the FileHeader
		fFileBuilder->FormBroadFHeader();
		//Second step set the FileHeader to TControl master for UDP broadcast
		TString broadfheader = fFileBuilder->GetBroadFHeader();
		mascontrol.SetFileHeader(broadfheader.Data());
		//implement OPEN File Command
		//// first: open the local data file, if open error, the command will not send out
		Dfilestate = fFileBuilder->OpenDataFile();  //Open Data file after FormBroadFHeader()
		////send out the command
		bool status = false;
		if(Dfilestate)
		{
			status = mascontrol.send_TCPcontrol_command(sCommands[4]);
			if(status)
			{
				fBOpenF->SetEnabled(false);
				fBCloseF->SetEnabled(kTRUE);
				fBConExit->SetEnabled(false);               //Close the File Before Exit
				fBDaqExit->SetEnabled(false);
			}
		}
		else
		{
			TString text = "File ";
			text += fFileBuilder->GetCurrFName();
			text += " already exist.";
			TThread::Lock(); //lock the main mutex
			ShowText(text.Data());
			TThread::UnLock(); //unlock the main mutex
		}
	}
}

void TMasterTask::ImB_CloseFile()
{
	bool status = false;
	if(thispcaction==CONTROL_PC)
	{
		status = mascontrol.send_TCPcontrol_command(sCommands[5]);
		if(status)
		{
			TThread::Lock(); //lock the main mutex
			fBOpenF->SetEnabled(true);
			fBCloseF->SetEnabled(kFALSE);
			fBConExit->SetEnabled(true);
			fBDaqExit->SetEnabled(true);
			TThread::UnLock(); //unlock the main mutex
		}
	}
}

void TMasterTask::RecrateDFileAuto()
{
	unsigned int filesize = fFileBuilder->CheckFileSize();
	if(filesize>maxfilesize)
	{
		ImB_StopDaq();
		sleep(WaitSec);  //wait a little time
		ImB_CloseFile();
		sleep(WaitSec);  //wait a little time
		ImB_OpenFile();
		sleep(WaitSec);  //wait a little time
		ImB_StartDaq();
	}
}

void TMasterTask::ShowTextLocal()
{
	//ShowText(textmes.Data());
	cout << textmes.Data() << endl;
}

void TMasterTask::SetTextLocal(const char *tex)
{
	textmes.Clear();
	textmes = tex;
}
