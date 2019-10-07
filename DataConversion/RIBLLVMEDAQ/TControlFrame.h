/////////////////////////////////////////////////////////
// File name: TControlFrame.h                            //
// Brief introduction:                                 //
//       This class create the main frame for          //
//       Online program of RIBLL1-DAQ                  //
//                                                     //
// Version: V1.0                                       //
// Author: Han Jianlong                                //
// Date: Nov. 2010                                     //
// For: RIBLL1                                         //
/////////////////////////////////////////////////////////

#ifndef ControlFrame_H
#define ControlFrame_H

#include "TGFrame.h"
#include "Rtypes.h"

class TGMainFrame;
class TGVerticalFrame;
class TGHorizontalFrame;
class TGWindow;
class TGLabel;
class TGTextView;
class TGTextEntry;
class TGTextButton;
class TGText;
class TGNumberEntry;

enum CMDBIdentifiers
{
	kB_setfilename,
	kB_setfilerun,
	kB_setfileheader,
	kB_start,
	kB_stop,
	kB_initdaq,
	kB_openf,
	kB_closef,
	kB_exitcon,
	kB_testcomm,
	kB_connectodaq,
	kB_exitdaq,
};

class TControlFrame : public TGMainFrame
{
protected:
	TGHorizontalFrame *fFUpMain, *fFDownMain;          //!
	TGVerticalFrame   *fFUpLeftMain, *fFUpRightMain;   //!

	TGTextEntry *fTEfilename, *fTEfileheader;          //!
	TGNumberEntry *fTEfilerun;                         //!
	TGLabel *flabFileStatus;                           //!

	TGTextButton *fBSetFileName, *fBSetFileRun, *fBSetFileHeader;  //!
	TGTextButton *fBStart, *fBStop, *fBInitDAQ, *fBOpenF, *fBCloseF, *fBConExit, *fBTestComm, *fBConnect, *fBDaqExit; //!

	TGTextView *fviewText, *fvieweventnum;	           //!

public:
	TControlFrame(const TGWindow *p, UInt_t w, UInt_t h);
	virtual ~TControlFrame();
	virtual void CloseWindow();
	virtual bool ProcessMessage(Long_t msg, Long_t param1, Long_t){return true;};
	virtual void ShowText(TGText *text);
	virtual void ShowText(const char *text);
	virtual void ClearTextView();
	virtual void ShowEventNum(int num);
	virtual void ClearEventNum();
	virtual void DisableButt(TGTextButton *butt);
	virtual void EnableButt(TGTextButton *butt);
	virtual void UpdateTextView();

protected:
	TControlFrame(const TControlFrame &onf);
	TControlFrame& operator=(const TControlFrame &onf);

	ClassDef(TControlFrame, 1);
};

extern TControlFrame *gControlFrame;

#endif //#ifndef ControlFrame_H
