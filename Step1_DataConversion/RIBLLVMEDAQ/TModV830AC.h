////////////////////////////////////////////
// TModV830AC.h: Implementation of CAEN
// module V830AC
// All module class must be inherited form
// 'TBoard'
// Han Jianlong 07/2012
////////////////////////////////////////////

#ifndef V830AC_H
#define V830AC_H

#include "Rtypes.h"
#include "TBoard.h"

class TH1;

class TModV830AC: public TBoard
{
public:
	enum MaxCh{fMaxChannel = 32};          //!Max channel of this module
protected:
	static unsigned int facqreg[2];        //! acquisition register and valid data mask
	static unsigned int fMaxDataVal;       // Maximum valid data value(4095, 8191 ...)

public:
	TH1 **his1d;                          //![fMaxChannel] pointer to the histograms of this module
	UInt_t chdata[fMaxChannel];           //channel data

public:
	TModV830AC(){ MaxChannel = (unsigned int)fMaxChannel;} //default constructor, ROOT TClass need this
	virtual void   Initialization();       //initialization the object before use it
	virtual        ~TModV830AC();
	virtual unsigned int    GetAcqReg(){return (fBaseAddr | facqreg[0]);}
	virtual unsigned int    GetAcqRegMask(){return facqreg[1];}
	virtual unsigned int    GetMaxChannel(){return fMaxChannel;}
	virtual unsigned int    GetMaxDataVal(){return fMaxDataVal;}
	virtual unsigned int    GetChannelData(int chnum);

	virtual void   SetMaxChannel(unsigned int mchann) {MaxChannel = mchann;}
	virtual void   SetMaxDataVal(unsigned int mdatav) {fMaxDataVal = mdatav;}

	virtual int    WriteGeoToBoard(unsigned int geo);
	virtual int    WriteCrateNumtoBoard(unsigned int cnum){return 1;};
	virtual int    Decode(unsigned int *&data_point);
	virtual int    DataReset(){return 1;}     // 'Data Reset' command of this module(empty func for V830AC)
	virtual int    SoftReset(){return 1;}     // need by WriteGeoToBoard(unsigned int geo)
	virtual int    Init_Board(){return 0;}
	virtual void   CleanChData(){for(int i=0; i<fMaxChannel; i++) chdata[i] =0;}
	virtual void   Create1DHistos();
	ClassDef(TModV830AC, 1)
};

#endif //#ifndef V830AC_H