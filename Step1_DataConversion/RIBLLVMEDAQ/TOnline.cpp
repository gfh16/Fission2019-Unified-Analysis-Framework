/////////////////////////////////////////////////////////
// File name: Online.cpp                               //
// Brief introduction:                                 //
//       Online program of RIBLL1-DAQ                  //
//                                                     //
// Version: V1.0                                       //
// Author: Han Jianlong                                //
// Date: Nov. 2010                                     //
// For: RIBLL1                                         //
/////////////////////////////////////////////////////////
#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TGClient.h"
#include "TGWindow.h"
#include "TGButton.h"
#include "TGListView.h"
#include "TVirtualPadEditor.h"
#include "GuiTypes.h"
#include "TGFSContainer.h"
#include "WidgetMessageTypes.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TThread.h"
#include "TRint.h"
#include "TOnlineFrame.h"
#include "TOnline.h"

#include "TDataReceiver.h"
#include "TDataAnalyser.h"
#include "TBoard.h"

#ifndef WIN32
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/types.h>
#endif //#ifndef WIN32

#ifdef WIN32
#include <windows.h>
#define sleep Sleep
#define WaitSec 600
#else
#include <unistd.h>
#define sleep usleep
#define WaitSec 800000
#endif

#include <iostream>
#include <map>
#include <typeinfo>
using namespace std;

extern void OnlineUserFunction(TOnlineFrame *onl);
extern void UserTH_Init();
extern void UserTH_Fill();

TDataReceiver  *TOnline::datarec=0;
TDataAnalyser  *TOnline::anadata=0;
TTree          *TOnlineFrame::tree=0;
TOnline        *gOnline=0;
bool            TOnline::setonline=false;

TOnline::TOnline(const TGWindow *p, UInt_t w, UInt_t h, TDataAnalyser *dana):
TOnlineFrame(p, w, h)
{
	if(gOnline)
	{
		cout << "TOnline>> only one 'TOnline' object allowed." << endl;
		return;
	}
	gOnline = this;

	//open datafile first
	datafile = new TFile("Ronline.root", "RECREATE");
	if(!dana) return;
	anadata = dana;	
	datarec = new TDataReceiver(UDPDataBroadPortMon);
	UserTH_Init();    //create user defined histograms
	CreateTH1I();     //create all 1d histogram of each module
	CreateTree();     //create tree

	ObjList = datafile->GetList();
	objcurr = ObjList->First();
	fFileCont->AddFile(datafile->GetName());
	fFileCont->Resize();
	setonline = true;
	OnlineThread = 0;
	fBoffline->SetEnabled(false);

	MakeTH1MenuList();
	RemoveMenuEntry("Delete", THmlist);
	RemoveMenuEntry("Dump", THmlist);
	RemoveMenuEntry("SetName", THmlist);
	RemoveMenuEntry("SetMaximum", THmlist);
	RemoveMenuEntry("SetMinimum", THmlist);
	RemoveMenuEntry("ShowBackground", THmlist);

	MakeTH2MenuList();
	RemoveMenuEntry("Delete", TH2mlist);
	RemoveMenuEntry("Dump", TH2mlist);
	RemoveMenuEntry("SetName", TH2mlist);
	RemoveMenuEntry("SetMaximum", TH2mlist);
	RemoveMenuEntry("SetMinimum", TH2mlist);
	RemoveMenuEntry("ShowBackground", TH2mlist);
	OnlineUserFunction(gOnlineFrame);
}


bool TOnline::ProcessMessage(Long_t msg, Long_t param1, Long_t)
{
	switch (GET_MSG(msg))
	{
	case kC_COMMAND:
		switch(GET_SUBMSG(msg))
		{
		case kCM_BUTTON:
			switch (param1)
			{
			case kB_online:
				StartOnline();//SetOnline();
				break;
			case kB_offline:
				SetOffline();
				break;
			case kB_exit:
				CloseWindow();
				break;
			case kB_resetall:
				ImB_ResetAllTH();
				OnlineUserFunction(gOnlineFrame);
				break;
			case kB_resetcurr:
				ImB_ResetCurrTH();
				OnlineUserFunction(gOnlineFrame);
				break;
			case kB_integral:
				ImB_integral();
				break;
			case kB_privious:
				ImB_previous();
				break;
			case kB_next:
				ImB_next();
				break;
			case kB_update:
				ImB_update();
				break;
			default:
				break;
			}
			break;
		}
		break;

	case kC_CONTAINER:
		switch(GET_SUBMSG(msg))
		{
		case kCT_ITEMDBLCLICK:
			if (param1==kButton1) OnDoubleClick((TGLVEntry *)fFileCont->GetLastActive(), param1);
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}

	return true;
}

TOnline::~TOnline()
{
	//setonline = false;
	datafile->Close();
	SafeDeleteP(datafile);
	if (TVirtualPadEditor::GetPadEditor(kFALSE) != 0)
		TVirtualPadEditor::Terminate();

}

void TOnline::SetOffline()
{
	setonline = false;
	sleep(WaitSec);
	if(OnlineThread)
	{
		TThread::Delete(OnlineThread);
		delete OnlineThread;
		OnlineThread = 0;
		TThread::Lock(); //lock the main mutex
		ShowText("Set Offline Success.");
		fBonline->SetEnabled(true);
		fBoffline->SetEnabled(false);
		TThread::UnLock(); //unlock the main mutex
	}
}

int TOnline::StartOnline()
{
	if(!OnlineThread)
	{
		setonline = true;
		OnlineThread = new TThread("MonOnlineThread",(void(*)(void *))(&TOnline::SetOnline), (void*)this);
		OnlineThread->Run();
		TThread::Lock(); //lock the main mutex
		ShowText("Online thread started.");
		fBonline->SetEnabled(false);
		fBoffline->SetEnabled(true);
		TThread::UnLock(); //unlock the main mutex
		return 1;
	}
	return 0;
}

void TOnline::SetOnline(void *arg)
{
	unsigned int recbyte = 0;
	unsigned int *sentinel = datarec->GetDataBuf();
	while(Getsetonline())
	{
		recbyte = datarec->ReceiveData();
		unsigned int *datap = datarec->GetDataBuf();
		do
		{
			datap++; //skip the EVENT_HEADER
			int evtc = anadata->GlobalDecoder(datap, recbyte/sizeof(int));
			if(evtc>0)
			{
				UserTH_Fill();
				tree->Fill();
			}
		}while(datap<(sentinel+recbyte));
	}
}

void TOnline::CreateTH1I()
{
	map<int, TBoard*> *CrateGeoMap = anadata->GetCrateGeoMap();
	map<int, TBoard*>::const_iterator it=CrateGeoMap->begin();
	
	for(it; it!=CrateGeoMap->end(); it++)
	{
		(*it).second->Create1DHistos();
	}
}

void TOnline::CreateTree()
{
	tree = new TTree("RawData", "ModuleData");
	tree->SetCircular(10000);
	map<int, TBoard*> *CrateGeoMap = anadata->GetCrateGeoMap();
	map<int, TBoard*>::const_iterator it=CrateGeoMap->begin();
	for(it; it!=CrateGeoMap->end(); it++)
	{
		if( (*it).second->GetTreeSwitch() )
		{
			TString tname = (*it).second->Class_Name();//typeid(*((*it).second)).name();
			tname += (*it).first;
			tree->Branch(tname.Data(), (*it).second->Class_Name(), (*it).second);
		}
	}
}

int GetRawData(int Crate, int Geo, int channel)
{
	if(!gOnline) return 0;
	int CGid = Crate*100 + Geo;
	int value = 0;
	TThread::Lock(); //lock the main mutex
	TDataAnalyser *dataana = gOnline->GetDataAnalyser();
	TBoard *mod = dataana->GetTBoardPointer(Crate, Geo);
	if(mod)
	{
		value = mod->GetChannelData(channel);
	}
	TThread::UnLock(); //unlock the main mutex
	return value;
}

#if !defined(__CINT__) && !defined(__MAKECINT__)

//int main(int argc, char *argv[])
//{
//	TRint apponline("Online", &argc, argv);
//	// The Server DAQ Application defining general environment
//	TDAQApplication ribll(argc, argv, IsMaster);
//	//where the cblt setup files are 
//	string pathchain = ribll.Get_PathEnvdir() + "/cblt_addr_c";
//	//where the filenameqdc and codifier setup file are
//	string pathfilename = ribll.Get_PathEnvdir() + "/filenamemod.dat";
//	
//	TConfig conf = TConfig(pathfilename.c_str());
//	TDataAnalyser anadata = TDataAnalyser(&conf);
//
//	TOnline *onl = new TOnline(gClient->GetRoot(), 100, 200, &anadata);
//
//
//	apponline.Run();
//	return 0;
//
////	//TApplication apponline("Online", &argc, argv);
////	TRint apponline("Online", &argc, argv);
////
////	TOnline *onl = new TOnline(gClient->GetRoot(), 100, 200);
////
////	apponline.Run();
////
////	return 0;
//}

#endif// #if !defined(__CINT__) && !defined(__MAKECINT__)
