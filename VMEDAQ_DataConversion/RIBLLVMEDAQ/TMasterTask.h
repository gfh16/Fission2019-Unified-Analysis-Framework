/////////////////////////////////////////////////////////
// File name: TMasterTask.h                            //
// Brief introduction:                                 //
//       Master PC task: Control daq, get data form    //
//       UDP broadcast, save data to file ...          //
// Version: V1.0                                       //
// Author: Han Jianlong                                //
// Date: Aug. 2012                                     //
// For: RIBLL1                                         //
/////////////////////////////////////////////////////////

#ifndef TMasterTask_H
#define TMasterTask_H

#include "TControlFrame.h"
#ifndef TControl_H
#include "TControl.h"
#endif

#include "Rtypes.h"
#include <RQ_OBJECT.h>
#include "TQObject.h"
#include "TString.h"

class TDataFileBuilder;
class TThread;
class TTimer;
class TControl;

class TMasterTask: public TControlFrame
{
	//RQ_OBJECT("TMasterTask");

protected:
	TDataFileBuilder *fFileBuilder;   //!
	TControl         &mascontrol;     //!
	eAction          thispcaction;    //!

	static TTimer    *showevtimer;    //!
	static TTimer    *showconnect;    //!
	static TTimer    *checkdfsize;    //!used to check the data file size, and recreate data file automaticly
	//TThread          *thshowevt;      //!
	TThread          *thtcpaccept;    //!

	static bool TCPConnState;         //!
	static bool TCPAcceptReturned;    //!
	bool        Dfilestate;           //!data file opened or not
	bool        DaqRunning;           //!daq is running or not
	unsigned int maxfilesize;         //!max file size form fFileBuilder
	TString      textmes;             //!message text to be showed on TGTextView

public:
	TMasterTask(TControl &netcon);
	virtual ~TMasterTask();
	virtual bool ProcessMessage(Long_t msg, Long_t param1, Long_t);
	TControl&    GetControlMas(){return mascontrol;}
	void         ShowMessageSignal();
	void         CoutBroadMessageSignal();
	void         ShowEventNumByTimer();
	static void* ConnecttoDAQPC(void *arg);

	void ImB_SetFileName();
	void ImB_SetFileRun();
	void ImB_SetFileHeader();
	void ImB_StartDaq();
	void ImB_StartOnlineDaq();
	void ImB_StopDaq();
	void ImB_InitDaq();
	void ImB_OpenFile();
	void ImB_CloseFile();
	bool ImB_Connect();
	void ImB_TestComm();
	void ImB_ExitDaqpc();

	void ProcessTCPConnectedTimer();
	void InitButt();
	void SetEnableAllButt(bool act);
	void DiableStopButt() {DisableButt(fBStop);}
	void DiableCloseButt(){DisableButt(fBCloseF);}
	void SetOnlinePCButt();
	bool ProcessControlSignal(int comm);
	void RecrateDFileAuto();
	void ShowTextLocal(); //Timer::SingleShot() slot to show text
	void SetTextLocal(const char* tex);

protected:
	//TMasterTask(TMasterTask const &source){};

	ClassDef(TMasterTask, 1);
};

extern TMasterTask *gMasterTask;
#endif //#ifndef TMasterTask_H