////////////////////////////////////////////////////////////////
//  TREADOUT and TCBLT class source implementation 
//  Modified by Han Jianlong 07/2012
//  Changes:
//    1. Add comment lines in cblt setting files, skip using'*'
////////////////////////////////////////////////////////////////
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <exception>
using namespace std;

#include "caenacq.h"
#include "TCBLT.h"
#include "TString.h"

///////////////////////////////////////////
//  The TREADOUT class implementation    //
///////////////////////////////////////////

// swap bytes on 32 bit longword from VME format to 
// Linux ones 
//void TReadout::swap32(unsigned int *buffer, int max)
//{
//	union dwor {
//		long dat;
//		unsigned char wrd[4];
//	};
//
//	dwor comp;
//	unsigned char temp;
//	unsigned int *p1=buffer, *p2=&buffer[max-1];
//
//	for(; p1<=p2; p1++) {
//		comp.dat = *p1;
//		temp = comp.wrd[0];
//		comp.wrd[0] = comp.wrd[3];
//		comp.wrd[3] = temp;
//		temp = comp.wrd[2];
//		comp.wrd[2] = comp.wrd[1];
//		comp.wrd[1] = temp;
//		*p1 = comp.dat;
//	}
//}

TReadout::TReadout(int dim)
{
	if(dim >= 8192)
	{
		fbufdim = dim;
	}
	else
	{
		fbufdim = VME_Crate_BufLENGTH;
	}
	try
	{
		fiobuf = new char[fbufdim];
	}
	catch(bad_alloc& ba)
	{
		cout << "TReadout>> do not have "<< dim << " bytes memory for allocation." << ba.what() << endl;
	}
	if(fiobuf) memset(fiobuf, 0x0, fbufdim);
}

TReadout::~TReadout()
{
	SafeDeleteArr(fiobuf);
}

///////////////////////////////////////////
//  The TCBLT class implementation       //
///////////////////////////////////////////
//copy constructor 
TCBLT::TCBLT(TCBLT const &other)
{
	try
	{
		fbase  = new int[other.fchains];
		fdummy = new int[other.fchains];
		fnum   = new int[other.fchains];
	}
	catch(exception &ex)
	{
		cout<<"TCBLT >> " << ex.what() << endl;
	}
	for(int i=0; i<other.fchains; i++) {
		fbase[i] = other.fbase[i];
		fdummy[i] = other.fdummy[i];
		fnum[i] = other.fnum[i];
	}
	fchains = other.fchains;
	fwait_for_ready = other.fwait_for_ready;
	foffset = other.foffset;
}

//overloading the assignment operator
TCBLT &TCBLT::operator=(TCBLT const &other)
{
	if(this != &other) {
		SafeDeleteArr(fbase);
		SafeDeleteArr(fdummy);
		SafeDeleteArr(fnum);
		try
		{
			fbase  = new int[other.fchains];
			fdummy = new int[other.fchains];
			fnum   = new int[other.fchains];
		}
		catch(exception &ex)
		{
			cout<<"TCBLT >> " << ex.what() << endl;
		}
		for(int i=0; i<other.fchains; i++) {
			fbase[i] = other.fbase[i];
			fdummy[i] = other.fdummy[i];
			fnum[i] = other.fnum[i];
		}
		fchains = other.fchains;
		fwait_for_ready = other.fwait_for_ready;
		foffset = other.foffset;
	}
	return *this;
}

TCBLT::~TCBLT()
{
	SafeDeleteArr(fnum); 
	SafeDeleteArr(fdummy);
	SafeDeleteArr(fbase);
}

//Get CBLT configuration
int TCBLT::Get_CBLT_Config(string name, int crate)
{
	int num = 0;
	unsigned int addr, nboard, mask;
	const char *fname; 
	char line[300];
	TString checkline;

	ostringstream filen(name,ios::ate);
	filen<<crate;
	filen<<".dat";
	name = filen.str();
	fname = name.c_str();
	ifstream in(fname);
	try{
		if(in) {
			while(in.getline(line, sizeof(line)))  // skip the comment lines by using '*'
			{					
				checkline = line;
				checkline.ReplaceAll(" ", "");
				checkline.ReplaceAll("\t", "");
				if(checkline.Length() > 0 && checkline[0] != '*') break;
			};

			if(checkline.IsDigit())	num = checkline.Atoi();
			if(num>0) 
			{
				fchains = num;
				fbase  = new int[num];
				fdummy = new int[num];
				fnum   = new int[num];

				for(int i=0; i<num; i++) 
				{
					in>>hex>>addr;
					in>>dec>>nboard;
					in>>hex>>mask>>dec;
					if(in.fail())
					{
						cout << "Read CBLT setting file: " << name.c_str() << " error!" <<endl;
					}
					if(nboard==0)break;
					fbase[i]  = addr;
					fdummy[i] = addr | mask;
					fnum[i]   = nboard;
				} 
			}
		}		
		else {
			cout << " TCBLT>>Config file: " << fname << " open error. "<< endl;
			return -1;
		}
	}
	catch(ifstream::failure e)
	{
		cout <<" TCBLT>>CBLT file: "<< fname << " read error!" << e.what() << endl;
	}
	in.close();
	cout<<"TCBLT>>Read CBLT config.: "<<fname<<" for crate "<<crate<<endl;
	return 0;
}


int TCBLT :: GetChainBoard(int chain)
{
	if(chain>fchains)
		return 0;
	else 
		return fnum[chain-1];
}

void TCBLT::PrintCBLTInfo() 
{
	cout<<"CBLT>>: chains = "<<fchains<<endl;
	for(int i=0; i<fchains; i++) {
		cout<<"CBLT>>: Nboards = "<<setw(2)<<fnum[i]<<"  Dummy address = 0x"<<hex<<fdummy[i]<<
			"  Base address = 0x"<<hex<<fbase[i]<<dec<<endl;
		cout<<dec;
	}
}
