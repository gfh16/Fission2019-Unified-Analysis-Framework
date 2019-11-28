///////////////////////////////////////////////////////////
//  TConfiguration class interface 
//  TConfiguration class.  E.d.F. (2007) 
//  main configuration class for hardware database 
//  and codifier initialization.
//  Used to read configuration files of boards and write
//  (implement) the data to boards.
//  Modified by Han Jianlong 07/2012
///////////////////////////////////////////////////////////

#ifndef TCONFIG_H
#define TCONFIG_H

#include <list>
#include <vector>
using namespace std;

#include "caenacq.h"
#include "TBoard.h"

class TTable;
class TBoardError;

class TConfig {

private:
	static list <string> fqnames;        //list of modules configuration file names
	static list <TBoard *> fboard;       //list of modules definitions
	static vector<TTable> ftable;        //crate sublist handler the first board of each handle(crate)
	char *ffilename;                     //the main codifier declaration list
	vector<TBoardError> fverror;

public:
	TConfig(const char *confname);
	TConfig(TConfig &);
	~TConfig();
	int GetConfigNames();
	int Init_Caen_Boards(EBoardInit sflag);
	void Init_Register(int crate, int addr, int data, int datawidth, int mask);
	long GetHandle(int );
	void ShowList();
	void ShowBoardList();
	void BuildCrateBookmark();
	static list<TBoard*>::const_iterator &LookupTable(int handle, int &num);
	static list<TBoard*>& GetBoardList();
	int GetErrnum() {return fverror.size();}
	string GetErrInfo(int );
	int ReadConfigForAnalysis();

private:
	void EraseNamesList();
	void EraseBoardList();
};


//This helper class maintain bookmarks to the first board and number of boards 
//belonging to a single crate. It is supposed that the main list is 
//ordered as a function of the handle number by construction.
//the table is cleaned and constructed again when Init_Caen_Boards member function
//is called. TConfig has full privileges over this class (friend declaration)
class TTable {
	friend class TConfig;
private:
	long fhandle;
	list <TBoard *>::const_iterator ffirst;
	int fnum;
public:
	TTable () : fnum(0), fhandle(-1) {}
	~TTable () {}
};

// This class contains a list of initialization errors
class TBoardError {
private:
	int fcrate;
	int faddr;
	int fdata;
public: 
	TBoardError() : fcrate(0), faddr(0), fdata(0) {}
	~TBoardError() {}
	void SetError(int crate, int addr, int data) {fcrate=crate; faddr=addr, fdata=data;}
	int GetECrate() {return fcrate;}
	int GetEAddr() {return faddr;}
	int GetEData() {return fdata;}
};
#endif
