//////////////////////////////////////////////////
// TDataFileReader.cpp: Read the data file for 
// offline analysis. Save the 'File header' to 
// a log file and loop reading to the end of the
// file.
// Han jianlong (08/2012)
//////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <exception>
using namespace std;

#include "TSystem.h"
#include "TString.h"
#include "TDataFileReader.h"
#include "caenacq.h"

//int *TDataFileReader::fevtbuf = 0;
//int  TDataFileReader::evtlen = 0;
//ifstream TDataFileReader::infile;
//ofstream TDataFileReader::logfile;

TDataFileReader::TDataFileReader()
{
	try
	{
		fevtbuf = new unsigned int[VME_Crate_BufLENGTH/4];
	}
	catch(bad_alloc &e)
	{
		cout<<"TDataFileReader>> " << e.what() << endl;
	}
	TString path_name = gSystem->Getenv(gDataPath.c_str());
	path_name += "/DataFile_log.txt";
	logfile.open(path_name.Data());
}

TDataFileReader::~TDataFileReader()
{
	SafeDeleteArr(fevtbuf);
}

bool TDataFileReader::OpenDataFile(const char* fname)
{
	infile.open(fname, ios_base::binary);
	if(infile.fail()) cout<<"Open data file: " << fname << " ERROR!" << endl; 
	return ( infile.good() );
}

void TDataFileReader::CloseDataFile()
{
	infile.close();
}

bool TDataFileReader::EvtReadingLoop()
{
	if(!infile.good()) return false;
	//if(infile.eof())   return false;
	//if(infile.fail())  return false;
	//if(infile.bad())   return false;
	unsigned int  endmark = Event_Header;
	unsigned int* read_pointer = fevtbuf;

	unsigned int end = 0;
	evtlen = 0;
	while((end != endmark) && (evtlen<(int)(VME_Crate_BufLENGTH/4)))  // one event read while 'Event_Header' met.
	{
		if(infile.eof())   return false;
		if(infile.fail())  return false;
		infile.read((char*)read_pointer, sizeof(unsigned int));  //read 4-byte, an int value
		end = *read_pointer;
		//cout << "this value: " << end << " evtlen: " << evtlen << endl;
		read_pointer++;
		evtlen++;                        //this event length
	}
	return true;
}

bool TDataFileReader::SkipFileHeader()
{
	if(!infile.good()) return false;
	char *fheader = new char[FileHeaderLEN];
	infile.read(fheader, FileHeaderLEN);
	logfile<<fheader;
	delete[] fheader;
	int firstheader = 0;
	infile.read((char*)&firstheader, sizeof(int));
	if (firstheader == Event_Header) return true;
	return false;
}