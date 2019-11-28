///////////////////////////////////////////////////////
//  VME models class interface
//  This class define the interface for the 
//  boards implemented in the DAQ. 
//   All true type models must be derived form the
//   base class, and all the member functions of a 
//   true model must be declared as a pure virtual
//   function in this base class.
//   Version 0.01
//   Hanjianlong 07/2012
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// Do not modify this class, even the comment lines
///////////////////////////////////////////////////////

#ifndef TBoard_H
#define TBoard_H

#include "Rtypes.h"

class TBoard {
protected:
	unsigned int   fBaseAddr;       //! do not delete the '!', it is used by ROOT
	long           fHandle;         //! v2718 handle
	unsigned int   fGeo;            //  Geo address
	unsigned int   fCrateNum;       // which crate is this board blong to
	char           fname[20];	    //! name of this class
	bool           bTreeSwitch;     //! for onlinetree, if flase this module will not in Tree
	//TBoard*        pSelfObj;      //! point to this board object itself
	unsigned int   MaxChannel;      //for ROOT Variable Length Arry = fMaxChannel

public:
	TBoard() { bTreeSwitch = false; }                         // must have the default constructor
	virtual void   Initialization() = 0;
	virtual        ~TBoard() {}
	virtual int    Get_BoardNumber() {return 0;}              // get the board serial number, not used here
	unsigned int   GetGeo() const {return fGeo;}
	unsigned int   GetCrateNum() const {return fCrateNum;}
	virtual long   GetHandle() const {return fHandle;}
	virtual unsigned int    GetAcqReg() = 0;                            // the DRDY register
	virtual unsigned int    GetAcqRegMask() = 0;                        // the DRDY register mask
	virtual unsigned int    GetBaseAddr(){return fBaseAddr;}
	const char*    GetName(){return fname;}
	virtual int    Get_SerialNumber() {return 0;}              // get the board serial number, not used here
	virtual unsigned int    GetMaxChannel() = 0;                        // maximum number of valid signal input channels
	virtual unsigned int    GetMaxDataVal() = 0;                        // maximum value of the converted data(4096, 8192 ....)
	//virtual TBoard* GetSelfObj() const {return pSelfObj;}      //! Get the self object pointer
	virtual bool   GetTreeSwitch() {return bTreeSwitch;}
	virtual unsigned int    GetChannelData(int chnum) = 0;

	virtual void   SetGeo(unsigned int geo){fGeo = geo;}
	virtual void   SetBaseAddr(unsigned int Baddr) {fBaseAddr = Baddr;}
	virtual void   SetCrateNum(unsigned int cnum)  {fCrateNum = cnum;}
	virtual void   SetName(const char* name);
	virtual void   SetHandle(long handle){fHandle = handle;}
	virtual int    Set_BoardNumber(int b) {return 0;}           // not used
	//virtual void   SetSelfObj(TBoard *self) {pSelfObj = self;}  // a pointer to this object itself, used to build the root file
	virtual void   SetMaxChannel(unsigned int mchann) = 0;
	virtual void   SetMaxDataVal(unsigned int mdatav) = 0;
	virtual void   SetTreeSwitch(bool onoff) {bTreeSwitch = onoff;}

	virtual int    WriteGeoToBoard(unsigned int geo) = 0;
	virtual int    WriteCrateNumtoBoard(unsigned int cnum) =0;
	virtual int    Decode(unsigned int *&data_point) = 0;
	virtual void   Create1DHistos() = 0;
	virtual int    Init_Board() = 0;
	virtual void   CleanChData() = 0;
	virtual int    DataReset() = 0;
	virtual int    SoftReset() = 0;
	virtual int    SoftReset(unsigned int data, unsigned int addroffset, int datasize);
	virtual int    SingleWriteCycle(unsigned int data, unsigned int mask, unsigned int addroffset, int datasize);
	virtual int    SingleReadCycle(unsigned int& rdata, unsigned int addroffset, int datasize);
	//virtual const char *Class_Name() = 0;

	ClassDef(TBoard, 1);
};

#endif //#ifndef TBoard_H