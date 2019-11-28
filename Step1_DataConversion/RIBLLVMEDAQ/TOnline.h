/////////////////////////////////////////////////////////
// File name: Online.h                                 //
// Brief introduction:                                 //
//       Online program of RIBLL1-DAQ                  //
//                                                     //
// Version: V1.0                                       //
// Author: Han Jianlong                                //
// Date: Nov. 2010                                     //
// For: RIBLL1                                         //
/////////////////////////////////////////////////////////

#ifndef Online_ROOT_H
#define Online_ROOT_H

#ifndef OnlineFrame_H
#include "TOnlineFrame.h"
#endif //#ifndef OnlineFrame_H

class TGWindow;
class TGLVEntry;
class TFile;
class TCanvas;
class TThread;
class TGTextButton;
class TDataReceiver;
class TDataAnalyser;
class TTree;

class TOnline :public TOnlineFrame
{
protected:
	TThread *OnlineThread;
	static bool setonline;
	static TDataReceiver *datarec;
	static TDataAnalyser *anadata;

public:
	TOnline(const TGWindow *p, UInt_t w, UInt_t h, TDataAnalyser *dana);
	virtual ~TOnline();
	virtual bool ProcessMessage(Long_t msg, Long_t param1, Long_t);
	void  SetOffline();
	static void SetOnline(void *arg);
	int StartOnline();
	TGTextButton *GetBOnline(){return fBonline;}
	static bool& Getsetonline(){return setonline;}
	void CreateTH1I();
	void CreateTree();
	TDataAnalyser *GetDataAnalyser(){return anadata;}
	friend int GetRawData(int Crate, int Geo, int channel);

protected:
	TOnline(const TOnline &onl);
	TOnline &operator=(const TOnline &onl);

};

extern TOnline *gOnline;
#endif //#ifndef Online_ROOT_H
