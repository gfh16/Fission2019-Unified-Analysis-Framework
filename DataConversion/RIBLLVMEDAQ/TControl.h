/////////////////////////////////////////
// TControl.h: Control the daq
// This class contents a socket used to
// receive 'control commands' from
// the cotrol side PC
// Han Jianlong 07/2012
/////////////////////////////////////////

#ifndef TControl_H
#define TControl_H

#include <string>
using namespace std;

#include "TString.h"
#include <RQ_OBJECT.h>
//#include "TQObject.h"

class TSocket;
class TServerSocket;
class TThread;
class TUDPClientSocket;
class TUDPServerSocket;

enum eAction
{
	NULL_PC,     // useless, only for initialization
	DAQ_PC,      // DAQ PC
	CONTROL_PC,  // the master control PC
	ONLINE_PC    // the other online PC for online monitoring and save data file
};
enum ECommands{kC_WAIT=0, kC_TEST, kC_INIT, kC_START, kC_STOP, kC_FOPEN, kC_FCLOSE, kC_EXIT};
const string sCommName[] = {"WAIT","TEST","INIT","START","STOP","OPEN","CLOSE","EXIT"};

const string sCommands[] = 
{
	"TEST CONNECTION",		/* CMD = 0 */
	"Initialize VME",       /*       1 */ 
	"Start Acquisition",    /*       2 */ 
	"Stop Acquisition",     /*       3 */ 
	"Open Tape File",       /*       4 */ 
	"Close Tape File",      /*       5 */  
	"Exit and Put offline"  /*       6 */ 
}; 

class TControl :public TQObject
{
	//RQ_OBJECT("TControl")

private:
	static TSocket *iRecvSocket;                 //! DAQ side PC socket, receive commands form control PC
	static TServerSocket *iSendSocket;           //! Control side PC socket, send commands to DAQ PC
	static TSocket *iSendSocket_imp;             //! implementation of the Send() with 'iSendSocket' & 'iRecvSocket'
	static TUDPClientSocket *iComBroadUDPSock;   //! Broadcast the commands to all PC
	static TUDPServerSocket *iComRevUDPSock;     //! Receive the bradcasted commands
	static TUDPClientSocket *iFHeBroadUDPSock;   //! Broadcast the data file header
	static TUDPServerSocket *iFHeRevUDPSock;     //! Receive the data file header

	TThread  *thread_DaqPC;                      //! thread used by DAQ_PC 
	TThread  *thread_OnlinePC;                   //! thread used by ONLINE_PC
	TThread  *thread_ConPCRevErr;                //! thread used by CONTROL_PC to receive some error messages from daqpc
	//TThread  *thread_DaqPC_WaitC;              //! waiting for connection from Control_PC

	
	static eAction PC_Action;                    //! eAction value, to assort the action of this 'TControl' object 
	static string sRecvComm;                     //!
	static bool TCPwait_command;                 //!
	static bool TCPRead_command;                 //!
	static bool UDPwait_command;                 //!

	static string ControlPCAddr;                 //! IP address of Control PC
	static string ack_mess;                      //! save the acknowlege message form the DAQ_PC
	//static string other_mess;                  //! for control_PC, save some no-command messages form Daq_PC

public:
	static ECommands comm;                        //!
	static string fheader_file;                   //! the data file header

public:
	TControl(const char* netaddr, eAction action);
	virtual ~TControl();
	bool        send_TCPcontrol_command(string scomm);
	static void *recv_TCPcontrol_command(void *arg);
	static void *recv_UDPBroadcontrol_command(void *arg);
	static void *recv_UDPBroadMessages(void *arg);  //for Control_PC use only
	static int  CommandForward(string message);
	static int  BroadMessage(string message);
	static void acknowledge();
	static void BroadFileHeader();
	bool        AcceptReceiver();
	static void ControlSignal(void *arg, int comm);
	void        CommandSignal(int comm);              //*SIGNAL*
	static void MessageSignal(void *arg);
	void        MSignalEmit();                        //*SIGNAL*
	static void BroadMessageSignal(void *arg);
	void        BMSignalEmit();                       //*SIGNAL*
	string      GetAckMessages() const {return ack_mess;};
	string      GetFileHeader()  const {return fheader_file;}
	TSocket*    GetTCPSendSocket(){return iSendSocket_imp;}
	bool        GetTCPSendSocketState();
	bool        ClosTCPimpSocket();                   //Close iSendSocket_imp
	void        SetFileHeader(const char* fh) {fheader_file = fh;}
	int         StartDaqPCThread();
	bool        StartOnlinePCThread();
	bool        StopOnlinePCThread();
	int         StartConPCRecErrThread();
	void        StopConPCRecErrThread();
	TThread*    GetDaqPCThread();
	TThread*    GetOnlinePCThread();
	string      GetNetAddr(){return ControlPCAddr;}
	eAction     GetPCAction(){return PC_Action;}
	void        printsignal();

protected:
	static bool DaqPCWaitConnection();

	ClassDef(TControl, 1);
};

extern TControl *onlyControl;
#endif //#ifndef TControl_H