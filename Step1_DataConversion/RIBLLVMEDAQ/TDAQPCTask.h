/////////////////////////////////////////////////////////
// File name: TDAQPCTask.h                             //
// Brief introduction:                                 //
//       DAQ PC task: Read data form VME modules, send //
//       data to UDP broadcast, receive command from   //
//       master PC
// Version: V1.0                                       //
// Author: Han Jianlong                                //
// Date: Aug. 2012                                     //
// For: RIBLL1                                         //
/////////////////////////////////////////////////////////

#ifndef DAQPCTask_H
#define DAQPCTask_H

#include <vector>
using namespace std;

class TClientEvtBuilder;
class TControl;
class TThread;
class TConfig;
class TCrateCBLT;

class TDAQPCTask
{
protected:
	TClientEvtBuilder *eventbuilder;
	TControl          *DAQPCControl;

public:
	TDAQPCTask(const char* ipaddr, TConfig &cod, std::vector<TCrateCBLT> &tcrate, unsigned int mastercrate);
	virtual ~TDAQPCTask();
	TThread * StartDAQThread();
	TThread * StartDAQPCControlThread();

protected:
	TDAQPCTask(TDAQPCTask const &source){}
};


#endif //#ifndef DAQPCTask_H