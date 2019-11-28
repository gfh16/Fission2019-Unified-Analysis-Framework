/////////////////////////////////////////////////////////
// File name: ControlFrame.cpp                          //
// Brief introduction:                                 //
//       This class create the main frame for          //
//       Online program of RIBLL1-DAQ                  //
//                                                     //
// Version: V1.0                                       //
// Author: Han Jianlong                                //
// Date: Nov. 2010                                     //
// For: RIBLL1                                         //
/////////////////////////////////////////////////////////

#include <iostream>
#include <ctime>
#include <string.h>
using namespace std;

#include "TApplication.h"
#include "TGClient.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "GuiTypes.h"
#include "TG3DLine.h"
#include "TGText.h"
#include "TGLabel.h"
#include "TGButton.h"
#include "TGTextView.h"
#include "TGTextEntry.h"
#include "TGNumberEntry.h"
#include "TGResourcePool.h"
#include "TGGC.h"
#include "TGFont.h"
#include "TString.h"
#include "TThread.h"
#include "TTimer.h"
#include "TControlFrame.h"


TControlFrame *gControlFrame =0;

ClassImp(TControlFrame);

TControlFrame::TControlFrame(const TGWindow *p, UInt_t w, UInt_t h):
TGMainFrame(p, w, h, kVerticalFrame)
{
	if(gControlFrame!=0)
	{
		cout<<"Only one instance of 'Online' permitted"<<endl;
		return;
	}
	gControlFrame = this;

	SetCleanup(kDeepCleanup);
	DontCallClose();

	//Start the up main Frame
	fFUpMain = new TGHorizontalFrame(this, 450, 250, kChildFrame|kVerticalFrame|kRaisedFrame);
	AddFrame(fFUpMain, new TGLayoutHints(kLineSolid, 1, 1, 1, 1));

	// Start of fFUpLeftMain---------------------------------------------------------
	fFUpLeftMain = new TGVerticalFrame(fFUpMain, 225, 300, kChildFrame|kVerticalFrame|kRaisedFrame);//|kFixedWidth);
	fFUpMain->AddFrame(fFUpLeftMain, new TGLayoutHints(kLineSolid, 1, 1, 1, 1));

	TGLayoutHints *flayoutButt  = new TGLayoutHints(kLHintsNormal|kLHintsCenterX|kLHintsCenterY, 5, 5, 7, 7);
	TGLayoutHints *flayoutfileb = new TGLayoutHints(kLHintsNormal|kLHintsLeft, 2, 2, 3, 3);

	TGGroupFrame *fFFileSettings = new TGGroupFrame(fFUpLeftMain, " File Settings ", kVerticalFrame);
	fFUpLeftMain->AddFrame(fFFileSettings, flayoutfileb);

	TGHorizontalFrame *fFsetfilename = new TGHorizontalFrame(fFFileSettings, 85, 10, kFitWidth);
	fFFileSettings->AddFrame(fFsetfilename, flayoutfileb);

	fTEfilename = new TGTextEntry(fFsetfilename, new TGTextBuffer(50));
	fFsetfilename->AddFrame(fTEfilename, flayoutfileb);
	fTEfilename->Resize(120, fTEfilename->GetDefaultHeight());
	fBSetFileName   =  new TGTextButton(fFsetfilename, "  SetFName  ", kB_setfilename);
	fFsetfilename->AddFrame(fBSetFileName, flayoutfileb);
	fBSetFileName->Resize(40, fBSetFileName->GetDefaultHeight());

	TGHorizontalFrame *fFsetfilerun = new TGHorizontalFrame(fFFileSettings, 85, 10, kFitWidth);
	fFFileSettings->AddFrame(fFsetfilerun, flayoutfileb);

	fTEfilerun = new TGNumberEntry(fFsetfilerun, 0, 4, 99, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELLimitMinMax, 0, 999);
	fFsetfilerun->AddFrame(fTEfilerun, flayoutfileb);
	fTEfilerun->Resize(120, fTEfilerun->GetDefaultHeight());
	fBSetFileRun   =  new TGTextButton(fFsetfilerun, " SetRunNum ", kB_setfilerun);
	fFsetfilerun->AddFrame(fBSetFileRun, flayoutfileb);
	fBSetFileRun->Resize(40, fBSetFileRun->GetDefaultWidth());

	TGHorizontalFrame *fFsetfileheader = new TGHorizontalFrame(fFFileSettings, 85, 10, kFitWidth);
	fFFileSettings->AddFrame(fFsetfileheader, flayoutfileb);
	fTEfileheader = new TGTextEntry(fFsetfileheader, new TGTextBuffer(750));
	fFsetfileheader->AddFrame(fTEfileheader, flayoutfileb);
	fTEfileheader->Resize(120, fTEfileheader->GetDefaultHeight());
	fBSetFileHeader =  new TGTextButton(fFsetfileheader, " SetFHeader ", kB_setfileheader);
	fFsetfileheader->AddFrame(fBSetFileHeader, flayoutfileb);
	fBSetFileRun->Resize(40, fBSetFileRun->GetDefaultHeight());

	fBSetFileName->Associate(this);
	fBSetFileRun->Associate(this);
	fBSetFileHeader->Associate(this);


	//File status of TGLabel
	TGGroupFrame *fFfilestatus = new TGGroupFrame(fFUpLeftMain, " File Status ", kHorizontalFrame|kChildFrame|kFixedWidth|kFixedHeight);
	//cout << fFUpLeftMain->GetDefaultWidth() << endl;
	fFfilestatus->Resize(fFUpLeftMain->GetDefaultWidth()-4, 45);
	fFUpLeftMain->AddFrame(fFfilestatus, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY, 1, 1, 1, 1));
	TGGC *fTextGC;
	const TGFont *font = gClient->GetFont("-*-times-bold-r-*-*-18-*-*-*-*-*-*-*");
	if (!font) font = gClient->GetResourcePool()->GetDefaultFont();
	FontStruct_t labelfont = font->GetFontStruct();
	GCValues_t   gval;
	gval.fMask = kGCBackground | kGCFont | kGCForeground;
	gval.fFont = font->GetFontHandle();
	gClient->GetColorByName("yellow", gval.fBackground);
	fTextGC = gClient->GetGC(&gval, kTRUE);   

	ULong_t bcolor, ycolor, rcolor;
	gClient->GetColorByName("yellow", ycolor);
	gClient->GetColorByName("blue", bcolor);
	gClient->GetColorByName("red", rcolor);

	flabFileStatus = new TGLabel(fFfilestatus, " Data File Closed ", fTextGC->GetGC(), labelfont, kChildFrame|kFixedWidth, bcolor);
	flabFileStatus->SetTextColor(ycolor);
	fFfilestatus->AddFrame(flabFileStatus, new TGLayoutHints(kLHintsNormal, 1, 1, 1, 1));

	flabFileStatus->Resize(fFUpLeftMain->GetDefaultWidth()-40, fFfilestatus->GetDefaultHeight());
	//end of fFUpLeftMain ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	TGVertical3DLine *sepupmain = new TGVertical3DLine(fFUpMain, 6, 200);
	fFUpMain->AddFrame(sepupmain, new TGLayoutHints(kLHintsNormal|kLHintsExpandY, 1, 1, 1, 1));

	//Start of fFUpRightMain--------------------------------------------------
	fFUpRightMain = new TGVerticalFrame(fFUpMain, 225, 300, kChildFrame|kVerticalFrame|kRaisedFrame);
	fFUpMain->AddFrame(fFUpRightMain, new TGLayoutHints(kLHintsNormal, 1, 1, 1, 1));

	TGGroupFrame *fFconbutton = new TGGroupFrame(fFUpRightMain, " Control Buttons: ", kVerticalFrame);
	fFUpRightMain->AddFrame(fFconbutton, new TGLayoutHints(kLHintsNormal, 1, 1, 1, 1));

	TGHorizontalFrame *butt1 = new TGHorizontalFrame(fFconbutton, 300, 60);
	fFconbutton->AddFrame(butt1, new TGLayoutHints(kLHintsNormal, 1, 1, 1, 1));
	fBStart    =  new TGTextButton(butt1, "  START ACQ  ", kB_start);
	fBStop     =  new TGTextButton(butt1, "  STOP  ACQ  ", kB_stop);
	fBInitDAQ  =  new TGTextButton(butt1, "  INIT ACQ   ", kB_initdaq);
	butt1->AddFrame(fBStart, flayoutButt);
	butt1->AddFrame(fBStop, flayoutButt);
	butt1->AddFrame(fBInitDAQ, flayoutButt);

	TGHorizontalFrame *butt2 = new TGHorizontalFrame(fFconbutton, 300, 40);
	fFconbutton->AddFrame(butt2, new TGLayoutHints(kLHintsNormal, 1, 1, 1, 1));
	fBOpenF   =  new TGTextButton(butt2, "  OPEN FILE   ", kB_openf);
	fBCloseF  =  new TGTextButton(butt2, "  CLOSE FILE ", kB_closef);
	fBConExit =  new TGTextButton(butt2, " EXIT CONTROL ", kB_exitcon);
	butt2->AddFrame(fBOpenF, flayoutButt);
	butt2->AddFrame(fBCloseF,  flayoutButt);
	butt2->AddFrame(fBConExit,  flayoutButt);

	TGHorizontalFrame *butt3 = new TGHorizontalFrame(fFconbutton, 300, 40);
	fFconbutton->AddFrame(butt3, new TGLayoutHints(kLHintsNormal, 1, 1, 1, 1));
	fBConnect   = new TGTextButton(butt3, "   CONNECT   ", kB_connectodaq);
	fBTestComm  = new TGTextButton(butt3, "  TEST COMM  ", kB_testcomm);
	fBDaqExit   = new TGTextButton(butt3, "  EXIT DAQ   ", kB_exitdaq);
	butt3->AddFrame(fBConnect, flayoutButt);
	butt3->AddFrame(fBTestComm, flayoutButt);
	butt3->AddFrame(fBDaqExit, flayoutButt);

	fBConExit->ChangeBackground(ycolor);
	fBDaqExit->ChangeBackground(rcolor);


	fBStart->Associate(this);
	fBStop->Associate(this);
	fBInitDAQ->Associate(this);
	fBOpenF->Associate(this);
	fBCloseF->Associate(this);
	fBConExit->Associate(this);
	fBConnect->Associate(this);
	fBTestComm->Associate(this);
	fBDaqExit->Associate(this);

	fBStart->SetToolTipText("Start DAQ");
	fBStop->SetToolTipText("Stop DAQ");
	fBInitDAQ->SetToolTipText("Initialization VMEREAD Process");
	fBOpenF->SetToolTipText("Open Data File");
	fBCloseF->SetToolTipText("Close Data File");
	fBConExit->SetToolTipText("Close This Control Frame");
	fBConnect->SetToolTipText("Connect to DAQ PC");
	fBTestComm->SetToolTipText("Test The Communication of This PC and DAQ PC");
	fBDaqExit->SetToolTipText("Make The VMEREAD Program on DAQPC Exit");


	//End of fFUpRightMain^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	TGHorizontal3DLine *sepupdownmain = new TGHorizontal3DLine(this, 450, 6, kChildFrame);
	AddFrame(sepupdownmain, new TGLayoutHints(kLHintsNormal|kLHintsCenterX|kLHintsExpandX, 1, 1, 1, 1));

	//Start the down main Frame
	fFDownMain = new TGHorizontalFrame(this, 550, 300, kChildFrame|kHorizontalFrame|kRaisedFrame);
	AddFrame(fFDownMain, new TGLayoutHints(kLHintsNormal, 1, 1, 1, 1));

	//Start of fFDownRightMain-------------------------------------------------
	TGGroupFrame *fFmessage = new TGGroupFrame(fFDownMain, " Messages: ", kVerticalFrame);
	fFDownMain->AddFrame(fFmessage, new TGLayoutHints(kLHintsNormal, 1, 1, 1, 1));

	fviewText = new TGTextView(fFmessage, 410, 220);
	fFmessage->AddFrame(fviewText, new TGLayoutHints(kLHintsExpandY|kLHintsExpandX, 2, 2, 2, 2));
	//End of fFDownRightMain^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	//Start of fFDownRightMain-------------------------------------------------
	TGGroupFrame *fFeventnum = new TGGroupFrame(fFDownMain, " Events: ", kVerticalFrame);
	fFDownMain->AddFrame(fFeventnum, new TGLayoutHints(kLHintsNormal, 1, 1, 1, 1));

	fvieweventnum = new TGTextView(fFeventnum, 120, 220);
	fFeventnum->AddFrame(fvieweventnum, new TGLayoutHints(kLHintsNormal, 1, 1, 1, 1));
	//End of fFDownRightMain^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


	SetWMSizeHints(GetDefaultWidth(), GetDefaultHeight(), GetDefaultWidth(), GetDefaultHeight(), 0, 0);

	SetWindowName("Control-RIBLL1-DAQ");
	MapSubwindows();
	MapWindow();

	Resize();
	gClient->FreeFont(font);
}

TControlFrame::~TControlFrame(){}

void TControlFrame::CloseWindow()
{
	DeleteWindow();
	gApplication->Terminate(0);
}

//bool TControlFrame::ProcessMessage(Long_t msg, Long_t param1, Long_t)
//{
//	switch(GET_MSG(msg))
//	{
//	case kC_COMMAND:
//		switch(GET_SUBMSG(msg))
//		{
//		case kCM_BUTTON:
//			switch(param1)
//			{
//			case kB_exitcon:
//				CloseWindow();
//				break;
//			}
//		}
//		break;
//
//	default:
//		break;
//	}
//	return kTRUE;
//}

void TControlFrame::ShowText(TGText *text)
{
	TThread::Lock(); //lock the main mutex
	ClearTextView();
	fviewText->AddText(text);
	fviewText->Update();
	fviewText->ShowBottom();
	TThread::UnLock(); //unlock the main mutex
}

void TControlFrame::ShowText(const char *text)
{
	TThread::Lock(); //lock the main mutex
	ClearTextView();
	fviewText->AddLineFast(text);
	fviewText->Update();
	fviewText->ShowBottom();
	//fviewText->AddLine(text);
	//TTimer::SingleShot(3, Class_Name(), this, "UpdateTextView()");
	TThread::UnLock(); //unlock the main mutex
}

void TControlFrame::ClearTextView()
{
	TThread::Lock(); //lock the main mutex
	if(fviewText->ReturnLineCount()>=50)
	{
		fviewText->GetText()->DelLine(1);
	}
	TThread::UnLock(); //unlock the main mutex
}

void TControlFrame::ShowEventNum(int num)
{
	TString chnum = TString::Format("%09d", num);
	TThread::Lock(); //lock the main mutex
	ClearEventNum();
	fvieweventnum->AddLineFast(chnum.Data());
	fvieweventnum->Update();
	//fvieweventnum->AddLine(chnum.Data());
	fvieweventnum->ShowBottom();
	TThread::UnLock(); //unlock the main mutex
}

void TControlFrame::ClearEventNum()
{
	TThread::Lock(); //lock the main mutex
	if(fvieweventnum->ReturnLineCount()>=50)
	{
		fvieweventnum->GetText()->DelLine(1);
	}
	TThread::UnLock(); //unlock the main mutex
}

void TControlFrame::DisableButt(TGTextButton *butt)
{
	TThread::Lock(); //lock the main mutex
	butt->SetEnabled(kFALSE);
	TThread::UnLock(); //unlock the main mutex
}

void TControlFrame::EnableButt(TGTextButton *butt)
{
	TThread::Lock(); //lock the main mutex
	butt->SetEnabled(kTRUE);
	TThread::UnLock(); //unlock the main mutex
}

void TControlFrame::UpdateTextView()
{
   fviewText->Update();
   fviewText->ShowBottom();
}