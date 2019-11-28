///////////////////////////////////////////////////////////////////////////
//  TConfiguration class.  E.d.F. (2007) 
//  main configuration class for hardware database 
//  and codifier initialization
//  Used to read configuration files of boards and write
//  (implement) the data to boards.
//	Modified by Han Jianlong 07/2012
//  Changes:
//		1. make the program use IRQ for DAQ
//		2. rewrite "Init_Caen_Boards(..)" to use "TClass" of ROOT
//		3. Add mark '*' for comment lines in configration file
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
//  TConfig class implementation  
///////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <exception>
using namespace std;
#include "TConfig.h"
#include "TVMELink.h"
#include "CAENVMEtypes.h"
#include "CAENVMElib.h"
#include "TDAQApplication.h"
#include "TROOT.h"
#include "TClass.h"
#include "TString.h"

// static members initialization
list <string> TConfig::fqnames;         //list of module configuration file names
list <TBoard *> TConfig::fboard;        //list of module configuration boards
vector <TTable> TConfig::ftable;        //vector of first board address in 
//fboard list for each crate (bookmark crate)

TConfig::TConfig(const char *confname)
{
	try
	{
		ffilename = new char[strlen(confname)+1];
		strcpy(ffilename,confname);
	}
	catch(exception &ex)
	{
		cout << "TConfig>> " << ex.what() << endl;
	}
};

void TConfig::EraseNamesList()
{
	if(!fqnames.empty()) {
		fqnames.clear();
	}
}

void TConfig::EraseBoardList()
{
	if(!fboard.empty()) {
		purge(fboard);
		fboard.clear();
	}
	ftable.clear();
}

TConfig::~TConfig()
{
	delete ffilename;
	if(!fboard.empty()) {
		purge(fboard);
		fboard.clear();
	}
	ftable.clear();
}

TConfig::TConfig(TConfig &source)
{
	unsigned int len = strnlen(source.ffilename, 200);
	char *fnames = new char[len];
	ffilename = fnames;
	fverror = source.fverror;
}

//read the main codifier name list for all crates 
//and put result in a container list
int TConfig:: GetConfigNames()
{
	const int SZ=300;
	char line[SZ];
	string names; 
	int kqdc = 0;

	ifstream fqdc(ffilename);
	if(!fqdc) {
		cout<<" TConfig>> File "<<ffilename<<" not found. "<<endl;
		return -1;
	}

	//remove all list elements 
	EraseNamesList();
	try{
		while(fqdc.getline(line,SZ))
		{
			TString checkline(line);
			while(checkline[0]==' '|| checkline[0]=='\t' ) //erase ' ' and '\t' that at the beginning of the line
			{
				checkline.Replace(0, 1, "");
			}
			if(checkline.Length()==0) continue;
			strcpy(line, checkline.Data());

			switch(line[0])
			{
			case '*':
				break;
			default:
				TString tsline = line;
				if(tsline.IsWhitespace()) break;
				names = gDAQApplication->Get_PathEnvdir() + "/" + line; 
				if(!names.empty()) {   
					fqnames.push_back(names);
					kqdc++;
				}
				break;    
			}
		}
	}
	catch(ios::failure &e)
	{
		cout << "Config file: " << ffilename << " read error!" << e.what()<< endl;
	}
	cout<<"Read "<<kqdc<<" names from file \""<<ffilename<<"\""<<endl;
	fqdc.close();
	return kqdc; 
}


//Init all codifiers registers or just check the configuration (default)
//Construct the board database list
int TConfig::Init_Caen_Boards(EBoardInit sflag=kCheckOnly)
{
	char line[300];
	char model[20];
	string filename;
	int nboard;
	int crate;
	unsigned int base=0, addr, addroff, data, dataw, mask, geo, treeswitch;
	bool infmod = false; //base information of this module is ok or not
	long handle =-1 ;
	TBoard *bp = 0;
	TBoardError err;

	nboard = 0; 
	//remove all list elements 
	EraseBoardList();
	//clear the error buffer
	if(!fverror.empty()) {
		fverror.clear();
	}
	list<string>::iterator it=fqnames.begin();
	try{
		while(it!=fqnames.end()) {
			filename = (*it);
			ifstream freg(filename.c_str());
			if(!freg) {
				cout<<"TConfig>> File "<<filename<<" can not be opened"<<endl;
				it++;
				continue;
			}

			int row = 0, lmark = 0;
			istringstream is;
			infmod = false;
			while(freg.getline(line, sizeof(line))) {
				row++;
				TString checkline(line);
				while(checkline[0]==' '|| checkline[0]=='\t' ) //erase ' ' and '\t' that at the beginning of the line
				{
					checkline.Replace(0, 1, "");
				}
				if(checkline.Length()==0)
				{
					//cout <<"The " << row << " line is empty." << endl; 
					continue;
				}
				strcpy(line, checkline.Data());
				switch(line[0]) 
				{
				case '*' :
					break;
				case '#' :
					//  obtain board information. check status registers
					//  fill the board database i
					if(infmod)
					{
						cout << " To much '#' lines in file: " << filename << endl;
						break;
					}
					is.str(line+1);
					is>>model>>crate>>hex>>base>>dec>>geo>>treeswitch;
					if(is.fail()) 
					{
						cout<<"File: "<< filename <<" iostream error at line #"<< row << endl;
						is.clear();
					}
					else 
					{
						infmod = true;
						handle = GetHandle(crate);
						if(handle>=0) 
						{
							TString modname = "TMod";
							modname += model;
							if((gROOT->GetClass(modname.Data())))
							{
								bp = 0;
								bp = (TBoard *)((gROOT->GetClass(modname.Data()))->New());					
								bp->Initialization();
								bp->SetHandle(handle);
								bp->SetBaseAddr(base);
								bp->SetGeo(geo);
								bp->SetCrateNum(crate);
								//bp->SetTreeSwitch(treeswitch);
								//bp->SetSelfObj(bp);
								bp->SetName(model);
								if( !(bp->WriteGeoToBoard(geo)) )
								{
									err.SetError(crate, base, geo);
									fverror.push_back(err);
									cout << "Write Geo address: "<< geo << " to board: 0x"<< hex<< base <<dec <<" error!" << endl;
								};
								if( !(bp->WriteCrateNumtoBoard(crate)) )
								{
									err.SetError(crate, base, crate);
									fverror.push_back(err);
									cout << "Write Crate Number: "<< crate << " to board: 0x"<< hex<< base <<dec <<" error!" << endl;
								}
								fboard.push_back(bp);
								printf("Board check: %10s %02d %04d  (0x%08x) [sn. %d]\n",model, crate, bp->GetGeo(), base, bp->Get_SerialNumber());
							}
							else
							{
								cout<<"File: "<<filename<< " Error at line #"<<row<< " Class of Module '" << model << "' do not exist!" << endl;
							}
						}
						else if(sflag==kCheckOnly)
						{
							printf("Board check: %10s %02d %04d  (0x%08x)\n",model, crate, geo, base);
						}
					}
					break;
				default:
					if(infmod)
					{
						is.clear();
						is.str(line);
						is>>dec>>lmark>>hex>>addroff>>data>>dec>>dataw>>hex>>mask>>dec;
						addr = base|(addroff & 0x0000ffff);
						if(is.fail()) {
							cout<< "Crate: " << crate << " Geo: " << geo <<" , error at line: "<< row << "| " << line << " |" <<" skipped"<<endl;
							is.clear();    
							break;
						}
						if(sflag!=kCheckOnly && handle>=0) {
							Init_Register(crate, addr, data, dataw, mask);
						}
						is.clear();
					}
					break;
				}

				memset(line, 0, sizeof(line));
			}
			nboard++;
			it++;
			freg.close();
		}
	}
	catch(ios::failure e)
	{
		cout <<"TConfig, read configration file: " << filename << " error!" << e.what() << endl;
	}
	BuildCrateBookmark();
	return nboard;
}


//Print codifiers list
void TConfig::ShowList()
{
	for (list<string>::iterator it=fqnames.begin(); it!=fqnames.end(); ++it)
	{ 
		cout << *it << " ";
		cout << endl;
	}
}


void TConfig::Init_Register(int crate, int addr, int data, int datawidth, int mask) {
	// Set QDC/TDC registers 

	bool RWok;
	CVErrorCodes status;
	int rdata, loop=20;
	CVDataWidth data_size=cvD16;
	int wdata=0;
	if(datawidth == 16)
	{
		data_size = cvD16;
		wdata = data & 0x0000ffff;
	}
	else if(datawidth == 32)
	{
		data_size = cvD32;
		wdata = data & 0xffffffff;
	}
	else
	{
		wdata = data & 0x0000ffff;
	}

	TBoardError err;

	long BHandle = GetHandle(crate);
	if (BHandle<0)
	{
		cout << "Device handle of crate: " << crate << " error." << endl;
		err.SetError(crate, 0, 0);
		fverror.push_back(err);
		return;
	}

	status = CAENVME_WriteCycle(BHandle, addr, &wdata, cvA32_U_DATA, data_size);
	if(status == cvSuccess)
	{
		if(mask != 0x0000) {
			RWok = false;
			while(loop--) {
				status = CAENVME_ReadCycle(BHandle, addr, &rdata, cvA32_U_DATA, data_size);
				if((rdata & mask) == data) {
					RWok = true;
					break;
				}  
				else {
					status = CAENVME_WriteCycle(BHandle, addr, &wdata, cvA32_U_DATA, data_size);
				}
			}
			if(RWok==false) {
				printf("INIT>> Warning: Failed in writing %04x to register %08x\n", wdata, addr); 
				printf("INIT>> Handle=%d  ADDR = 0x%08x DATUM_READ = 0x%08x\n\n",BHandle,addr,rdata);
				err.SetError(crate,addr,wdata);
				fverror.push_back(err);
			}
		}
	}
	else
	{
		cout << "TConfig>> Initialize module in Crate: " << crate << " ,address: 0x" << hex << addr << dec << " error." << endl;
	}
}



long TConfig::GetHandle(int crate)
{
	map<int, int> ::iterator it1; 
	int handle;

	it1 = TVMELink::flookup_ind.find(crate);
	it1 == TVMELink::flookup_ind.end() ? handle=-1 : handle = (*it1).second;
	return handle;
}

//Print codifiers list
void TConfig::ShowBoardList()
{
	list<TBoard *>::const_iterator it;

	cout<<"\n------ Boards database list ------"<<endl;
	cout<<"There are #"<<fboard.size()<<" boards in the list"<<endl;
	for (it=fboard.begin(); it!=fboard.end(); ++it)
	{ 
		cout <<"Base addr: "<<showbase<<hex<<(*it)->GetBaseAddr()<< "   BHandle: "<<dec<<
			(*it)->GetHandle()<<"   Type: "<<(*it)->GetName()<<endl;
	}
}

// Find the first board of each crate
void TConfig::BuildCrateBookmark()
{
	int h, oldh;
	int num = 0;
	TTable t;

	list<TBoard *>::const_iterator it;

	if(fboard.size()==0)return;
	it=fboard.begin();
	h = (*it)->GetHandle();
	oldh = h;
	t.ffirst = it;
	t.fhandle = h;
	num++;
	it++;
	for (;it!=fboard.end(); it++) {
		h = (*it)->GetHandle();
		if(oldh != h) {
			t.fnum = num;
			ftable.push_back(t);
			oldh = h;
			t.ffirst = it;
			t.fhandle = h;
			num=0;
		}
		num++;
	}
	//assign the last node found
	t.fnum = num;
	ftable.push_back(t);
}


list<TBoard*>::const_iterator &TConfig::LookupTable(int handle, int &num) 
{
	static list<TBoard *>::const_iterator it;
	num = 0;

	for(unsigned int i=0; i<ftable.size(); i++) {
		if(ftable[i].fhandle==handle) {
			num = ftable[i].fnum;
			return ftable[i].ffirst;
		}
	}
	return it;  //return default iterator for an empty crate 
}

string TConfig::GetErrInfo(int i)
{
	stringstream is;
	if(i<0) return " ";
	if(fverror.size()!=0 && i<fverror.size()) {
		is<<"Error #"<<i<<" of #"<<fverror.size()<<"  Crate: "<<fverror[i].GetECrate()<<"  Addr: 0x"<<hex<<fverror[i].GetEAddr()<<dec<<
			"  Datum: 0x"<<hex<<fverror[i].GetEData();
		return is.str();
	}
	return " ";
}

// Read the 'config files' for data analysis
int TConfig::ReadConfigForAnalysis()
{
	char line[300];
	char model[20];
	string filename;
	int nboard = 0;
	int row=0, crate=0;
	unsigned int base=0, geo =0;
	bool treeswitch = 0;
	//long handle;
	TBoard *bp = 0;

	nboard = 0; 
	//remove all list elements 
	EraseBoardList();
	//clear the error buffer
	if(!fverror.empty()) {
		fverror.clear();
	}
	list<string>::iterator it=fqnames.begin();
	try{
		while(it!=fqnames.end()) {
			filename = (*it);
			ifstream freg(filename.c_str());
			if(!freg) {
				cout<<"TConfig>> File "<<filename<<" can not be opened"<<endl;
				it++;
				continue;
			}

			row = 0;
			istringstream is;
			while(freg.getline(line, sizeof(line))) {
				row++;
				TString checkline(line);
				while(checkline[0]==' '|| checkline[0]=='\t' ) //erase ' ' and '\t' that at the beginning of the line
				{
					checkline.Replace(0, 1, "");
				}
				if(checkline.Length()==0) continue;
				strcpy(line, checkline.Data());

				switch(line[0]) 
				{
				case '*' :
					break;
				case '#' :
					//  obtain board information. check status registers
					//  fill the board database i
					is.str(line+1);
					is>>model>>crate>>hex>>base>>dec>>geo>>treeswitch;
					if(is.fail()) {
						cout<<"File: "<< filename <<" iostream error at line #"<< row << endl;
						is.clear();
					}
					else {
						//handle = GetHandle(crate);
						//if(handle>=0)
						{
							TString modname = "TMod";
							modname += model;
							if((gROOT->GetClass(modname.Data())))
							{
								bp = 0;
								bp = (TBoard *)((gROOT->GetClass(modname.Data()))->New());
								bp->Initialization();					
								//bp->SetHandle(handle);
								bp->SetBaseAddr(base);					
								bp->SetGeo(geo);					
								bp->SetCrateNum(crate);					
								//bp->SetSelfObj(bp);
								bp->SetName(model);				
								//#ifdef ONLINETREE
								//cout << treeswitch << endl;
								bp->SetTreeSwitch(treeswitch);
								//#endif //#ifdef ONLINETREE
								//bp->WriteGeoToBoard(geo);
								//bp->WriteCrateNumtoBoard(crate);
								fboard.push_back(bp);					
								printf("Board check: %12s Crate %02d Geo %04d  (0x%08x) \n",bp->GetName(), crate, bp->GetGeo(), base);
							}
							else
							{
								cout<<"File: "<<filename<< " Error at line #"<<row<< " Module name '" << model << "' do not exist!" << endl;
							}
						}
					}
					break;
				default:
					break;
				}
			}
			nboard++;
			it++;
			freg.close();
		}
	}
	catch(ios::failure &e)
	{
		cout <<"TConfig, read configration file: " << filename << " error!" << e.what() << endl;
		return -1;
	}
	return nboard;
}


list<TBoard*>& TConfig::GetBoardList()
{
	return fboard;
}