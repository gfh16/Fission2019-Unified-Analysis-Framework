////////////////////////////////////////////
// TModV775N.h: Implementation of CAEN
// module V775NAC
// All module class must be inherited form
// 'TBoard'
// Han Jianlong 07/2012
////////////////////////////////////////////

#ifndef V775N_H
#define V775N_H

#include "TBoard.h"
#include "Rtypes.h"

class TH1;

class TModV775N: public TBoard
{
protected:
	enum MaxCh{fMaxChannel = 16};          //!Max channel of this module
protected:
	static unsigned int facqreg[2];        //! acquisition register and valid data mask
	static unsigned int fMaxDataVal;       //! Maximum valid data value(4095, 8191 ...)

public:
	TH1 **his1d;                          //![fMaxChannel] pointer to the histograms of this module
	UShort_t chdata[fMaxChannel];         //channel data

public:
	TModV775N(){ MaxChannel = (unsigned int)fMaxChannel;}  //default constructor, ROOT TClass need this
	virtual void   Initialization();         //initialization the object before use it
	virtual        ~TModV775N();
	virtual unsigned int    GetAcqReg(){return (fBaseAddr | facqreg[0]);}
	virtual unsigned int    GetAcqRegMask(){return facqreg[1];}
	virtual unsigned int    GetMaxChannel(){return fMaxChannel;}
	virtual unsigned int    GetMaxDataVal(){return fMaxDataVal;}
	virtual unsigned int    GetChannelData(int chnum);

	virtual void   SetMaxChannel(unsigned int mchann) {MaxChannel = mchann;}
	virtual void   SetMaxDataVal(unsigned int mdatav) {fMaxDataVal = mdatav;}

	virtual int    WriteGeoToBoard(unsigned int geo);
	virtual int    WriteCrateNumtoBoard(unsigned int cnum);
	virtual int    Decode(unsigned int *&data_point);
	virtual int    DataReset();                  // 'Data Reset' command of this module
	virtual int    SoftReset();                  // need by WriteGeoToBoard(unsigned int geo)
	virtual int    Init_Board(){return 0;}
	virtual void   CleanChData(){for(int i=0; i<fMaxChannel; i++) chdata[i] =0;}
	virtual void   Create1DHistos();
	

	ClassDef(TModV775N, 1);
};


#endif //#ifndef V775N_H