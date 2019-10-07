//////////////////////////////////////////////////
// TDataFileBuilder.cpp: Data file manager, open
// close, write data file.
// Han Jianlong (08/2012)
/////////////////////////////////////////////////


#include "TDataFileBuilder.h"
#include "stdlib.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#ifdef WIN32
#include <io.h>
#include <windows.h>
#define sleep Sleep
#define WaitSec 500
#else
#include <unistd.h>
#define sleep usleep
#define WaitSec 600000
#endif

#ifndef WIN32
#define _access access
#endif

#include "caenacq.h"
#include "TThread.h"
#include "TString.h"
#include "TThread.h"
#include "TControl.h"

//unsigned int TDataFileBuilder::RunNum = 0;
ofstream* TDataFileBuilder::datafile = 0;
unsigned int TDataFileBuilder::MaxFileSize = 500; //Mb

TDataFileBuilder *gDataFileBuilder = 0;

TDataFileBuilder::TDataFileBuilder():TDataReceiver(UDPDataBroadPortCon)
{
	if(gDataFileBuilder)
	{
		cout << "Only one instance of 'TDataFileBuilder' permitted." << endl;
		return;
	}
	gDataFileBuilder=this;
	filename = "RawData";
	RunNum = 0;
	CurrRunNum = RunNum;
	thread_writedata = 0;
	fileDir =getenv( gDataPath.c_str() );
	void* status = gSystem->OpenDirectory(fileDir.Data());  //check if 'fileDir' exist or not, if not fileDir=pwd
	if(status == 0)
	{
		fileDir.Clear();
	}
	else
	{
		gSystem->FreeDirectory(status);
	}
	CurrFName  = filename;
	CurrFName += '.';
	CurrFName += RunNum;
}

//Form the fileheader used for UDP broadcast,
//this header begin with filename.runnum
void TDataFileBuilder::FormBroadFHeader()
{
	if( broadfileheader.Length()>0 ) broadfileheader.Clear();
	TString rnum = TString::Format("%04u", RunNum);  //format the runnum->0001-9999
	broadfileheader = filename;
	broadfileheader += '.';
	broadfileheader += rnum;
	broadfileheader += "#";         //separate the filenmae and file header
	broadfileheader += fileheader;
	cout <<"BroadFile Header: " << broadfileheader.Data() << endl;
}

//Extract the file name and run number form the braodcasted(received) fileheader
bool TDataFileBuilder::ExtractFNameNumFromBHeader()
{
	if( broadfileheader.Length()==0 ) return false;
	unsigned int pos = broadfileheader.First("#");
	TString tfile = broadfileheader(0, pos);
	pos = tfile.First('.');
	filename = tfile(0, pos);
	TString rnum = tfile(pos+1, 4);
	RunNum = rnum.Atoi();
	//cout << filename.Data() << "." << rnum.Data() << endl;
	return true;
}

bool TDataFileBuilder::OpenDataFile()
{
	if(datafile)
        {
            bool bclose = CloseDataFile();
            if(bclose) cout << "Current datafile: " << CurrFName << " closed." << endl;
        }
	if( filename.Length() == 0 ) return false;
	TString tfilename;
	TString rnum = TString::Format("%04u", RunNum);
	TString fname = fileDir;
	fname += '/';
	fname += filename;
	fname += '.';
	fname += rnum;
	CurrFName  = filename;
	CurrFName += '.';
	CurrFName += rnum;

	if( _access(fname.Data(), 0) != -1 ) //for file already exist
	{
		cout << " TDataFileBuilder>>File: " << fname << " already exist." << endl;
		return false;
	}
	try 
	{
		datafile = new ofstream(fname.Data(), ios_base::binary);
	}
	catch (bad_alloc &ex)
	{
		cout << "Open Data File Error: " << ex.what() << endl;
	}
	if( (!datafile) || (!datafile->good()) ) return false;
	CurrRunNum = RunNum;      //memory the current running file RunNum
	RunNum += 1;
	datafile->write(broadfileheader.Data(), FileHeaderLEN); //write the file header to datafile
	gSystem->Beep(10, 1000);
	return true;
}

bool TDataFileBuilder::CloseDataFile()
{
	bool state = false;
	TThread::Lock();            //Lock(), in case the thread "ReceiveDataToFile(void* arg)" is writing the file
	if(datafile)
	{
		int endv = Event_Header; //add the 'Event_Header' to the last 4byte of the file
		(*datafile)<<endv;
		datafile->close();
		delete datafile;
		datafile = 0;
		state = true;
		gSystem->Beep(10, 1000);
	}
	TThread::UnLock();

	return state;
}

bool TDataFileBuilder::CloseDataFileManual()
{
	//StopWriteDataThread();
	return CloseDataFile();
}

void* TDataFileBuilder::ReceiveDataToFile(void* arg)
{
	if( !IsUDPValid() ) return 0;
	TDataFileBuilder *thisp = (TDataFileBuilder*)arg;
	int nbyte = 0;
	do
	{
		nbyte = ReceiveData();
		eventcounter = fdatabuf[1];    //fdatabuf[1]:the first 'Event Count' of this block
		//cout << "nbyte: " << nbyte << endl;
		if( IsDataFileValid() && nbyte >0)
		{
			datafile->write( (char*)fdatabuf, nbyte );
		}
	}while(!CheckStop());

	cout << "Exit form receivedata socket. " << endl;
	return 0;
}

bool TDataFileBuilder::IsDataFileValid()
{
	bool valid = false;
	TThread::Lock();
	if(datafile) valid = datafile->good();
	TThread::UnLock();
	return valid;
}

bool TDataFileBuilder::CheckStop()
{
	TThread::Lock();
	bool check = (TControl::comm == kC_STOP);
	TThread::UnLock();
	if(check) {
		return true;
	}
	return false;
}

bool TDataFileBuilder::StartWriteDataThread()
{
	if(!thread_writedata)
	{
		//if( !IsUDPValid() ) OpenRecDataSocket();
		thread_writedata = new TThread("WriteData", (void*(*)(void*))(&TDataFileBuilder::ReceiveDataToFile), (void*)this);
		if(thread_writedata)
		{
			thread_writedata->Run();
			cout << thread_writedata->GetName() << " is running..." << endl;
			return (thread_writedata->GetState() == TThread::kRunningState);
		}
		else
		{
			thread_writedata = 0;
			return false;
		}
	}
	else
	{
		thread_writedata->Run();
		return true;
	}
}

bool TDataFileBuilder::StopWriteDataThread()
{
	sleep(WaitSec);      //in case 'start' 'stop' very fastly.
	if(thread_writedata)
	{
		string thname = thread_writedata->GetName();
		//if( IsUDPValid() ) cout << "Close socket: " << CloseRecDataSocket() << endl;  //close the socket
		TThread::Delete(thread_writedata);
		//delete thread_writedata; /// TThread::Delete will call destructor for thread_writedata
		thread_writedata = 0;
		//cout << thname << " stopped. " << endl;
		return true;
	}
	return true;
}

unsigned int TDataFileBuilder::CheckFileSize() //Mb
{
	if(IsDataFileValid())
	{
		TThread::Lock();
		long size_byte = datafile->tellp();
		TThread::UnLock();
		return (unsigned int)size_byte/10E5;
	}
	return 0;
}

bool TDataFileBuilder::ReCreateFileAuto()
{
	CloseDataFile();
	return OpenDataFile();
}
