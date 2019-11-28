/////////////////////////////////////////////////////////
// File name: TOnlineFrame.h                            //
// Brief introduction:                                 //
//       This class create the main frame for          //
//       Online program of RIBLL1-DAQ                  //
//                                                     //
// Version: V1.0                                       //
// Author: Han Jianlong                                //
// Date: Nov. 2010                                     //
// For: RIBLL1                                         //
/////////////////////////////////////////////////////////

#ifndef OnlineFrame_H
#define OnlineFrame_H

#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif //#ifndef ROOT_TGFrame

#ifndef ROOT_TString
#include "TString.h"
#endif //#ifndef ROOT_TString

class TGMainFrame;
class TGVerticalFrame;
class TGWindow;
class TGFileContainer;
class TGLabel;
class TGListView;
class TGLVEntry;
class TGTextView;
class TGComboBox;
class TGTextButton;
class TGText;
class TCanvas;
class TString;
class TList;
class TFile;
class TIter;
class TTree;
class TObject;

enum CMDIdentifiers
{
	kB_online,
	kB_offline,
	kB_exit,
	kB_resetcurr,
	kB_resetall,
	kB_privious,
	kB_next,
	kB_update,
	kB_integral
};

class TOnlineFrame : public TGMainFrame
{
protected:
	TGVerticalFrame *fFFileList, *fFButton;
	TGHorizontalFrame *fFTextView;

	TGLabel *flabDrawOpt,  *flabDivPad, *flabText;
	TGComboBox *fcomDrawOpt, *fcomAxisOpt, *fcomDivPad;
	TGListView  *flvFile;
	TGFileContainer  *fFileCont;
	TGTextButton *fBonline, *fBoffline, *fBresetcurr, *fBresetall, *fBprevious, *fBnext, *fBupdate, *fBintegral, *fBexit;
	TGTextView *fviewText;
	TCanvas *candaq;
	TObject *objcurr;
	TList *ObjList;
	TFile *datafile;
	static TTree *tree;
	TList *TCmlist;   //TCavas Contextmenu list
	TList *THmlist;   //TH1F Contextmenu list
	TList *TH2mlist;  //TH2F Contextmenu list
	

public:
	TOnlineFrame(const TGWindow *p, UInt_t w, UInt_t h);
	virtual ~TOnlineFrame();
	virtual void CloseWindow();
	virtual bool ProcessMessage(Long_t msg, Long_t param1, Long_t);
	const char* GetDrawOpt();
	void ShowText(TGText *text);
	void ShowText(const char *text);
	virtual void CreateCanvas();
	void ClearTextView();
	virtual void OnDoubleClick(TGLVEntry* f, Int_t btn);
	virtual void DisplayFile(const TString &fname);
	virtual void DisplayObject(const TString& fname,const TString& name);
	virtual void DisplayDirectory(const TString &fname);
	virtual void ImB_update();
	virtual void ImB_next();
	virtual void ImB_previous();
	virtual void ImB_integral();
	virtual bool ObjListOK();
	virtual void DrawObj(TObject *obj);
	virtual void DrawObj(CMDIdentifiers id);
	virtual void ResetTH(TObject *obj);
	virtual void ImB_ResetAllTH();
	virtual void ImB_ResetCurrTH();
	virtual void GetDivPad(int &nx, int &ny);
	virtual void RemoveMenuEntry(const char *menuTitle, TList *mlist);
	virtual void MakeTcMenuList();
	virtual void MakeTH1MenuList();
	virtual void MakeTH2MenuList();
	virtual void GetAxisOpt(unsigned int& lx, unsigned int& ly);
	friend void OnlineUserFunction(TOnlineFrame *onl);
	friend void HBOOK1(int id, const char *title, int nxbin, float xlow, float xup, float vmx);
	friend void HBOOK2(int id, const char *title, int nxbin, float xlow, float xup, int nybin, float ylow, float yup, float vmx);
	friend void HF1(int id, float value, float weight);
	friend void HF2(int id, float x, float y, float weight);
	friend bool HEXIST(int ihist);


protected:
	TOnlineFrame(const TOnlineFrame &onf);
	TOnlineFrame& operator=(const TOnlineFrame &onf);
};

extern TOnlineFrame *gOnlineFrame;

#endif //#ifndef OnlineFrame_H
