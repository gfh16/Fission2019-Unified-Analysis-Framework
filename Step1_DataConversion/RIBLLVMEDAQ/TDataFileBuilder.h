//////////////////////////////////////////////////
// TDataFileBuilder.h: Data file manager, open
// close, write data file.
// Han Jianlong (08/2012)
/////////////////////////////////////////////////

#ifndef TDataFileBuilder_H
#define TDataFileBuilder_H

#include "TDataReceiver.h"
#include <TString.h>
using namespace std;

class TThread;

class TDataFileBuilder: public TDataReceiver
{
protected:
	TString filename;
	TString fileheader;
	TString broadfileheader;
	unsigned int RunNum;
	unsigned int CurrRunNum;
	TString  CurrFName;

	TString fileDir;
	static ofstream *datafile;
	TThread *thread_writedata;

public:
	static unsigned int MaxFileSize;

public:
	TDataFileBuilder();
	virtual ~TDataFileBuilder(){};

	void SetFileName(const char* name) {filename = name;}
	void SetRunNum(unsigned int rnum) {RunNum = rnum;}
	void SetFileHeader(const char* fheader) {fileheader = fheader;}
	void SetBroadFHeader(const char* bfheader){broadfileheader = bfheader;}

	void FormBroadFHeader();              //Form the fileheader used for UDP broadcast, this header begin with filename.runnum
	bool ExtractFNameNumFromBHeader();     //Extract the file name and run number form the braodcast fileheader
	TString GetBroadFHeader(){return broadfileheader;}
	bool OpenDataFile();
	bool CloseDataFile();
	bool CloseDataFileManual();
	void SetMaxFileSizeMb(unsigned int sizeMb){MaxFileSize = sizeMb;}
	unsigned int GetMaxFileSizeMb(){return MaxFileSize;}
	static unsigned int CheckFileSize();
	bool ReCreateFileAuto();
	static bool IsDataFileValid();
	static void* ReceiveDataToFile(void* arg);
	static bool CheckStop();
	unsigned int GetCurrRunNum(){return CurrRunNum;}
	const char * GetCurrFName(){return CurrFName.Data();}

	bool StartWriteDataThread();
	bool StopWriteDataThread();
	TThread* GetWriteDataThread(){return thread_writedata;}
};

extern TDataFileBuilder *gDataFileBuilder;
#endif //#ifndef TDataFileBuilder_H