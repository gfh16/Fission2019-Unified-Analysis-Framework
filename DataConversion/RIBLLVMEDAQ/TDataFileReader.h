//////////////////////////////////////////////////
// TDataFileReader.h: Read the data file for 
// offline analysis. Save the 'File header' to 
// a log file and loop reading to the end of the
// file.
// Han jianlong (08/2012)
//////////////////////////////////////////////////


#ifndef TDataFileReader_H
#define TDataFileReader_H

#include <fstream>
using namespace std;


class TDataFileReader
{
public:
	unsigned int *fevtbuf;
	unsigned int evtlen;
	ifstream infile;
	ofstream logfile;

public:
	TDataFileReader();
	virtual ~TDataFileReader();
	bool    OpenDataFile(const char* fname);
	void    CloseDataFile();
	bool    EvtReadingLoop();
	unsigned int*& GetEvtBuf(){return fevtbuf;}
	unsigned int  GetEventLength(){return evtlen;}
	bool    SkipFileHeader();
};

#endif //#ifndef TDataFileReader_H