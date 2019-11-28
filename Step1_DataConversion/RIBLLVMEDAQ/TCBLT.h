/////////////////////////////////////////////////////////////
//  TReadout interface  
//  Readout modes for the VME bridge 
//  The TReadout defines the three basic modes 
//  for data readout with a VME bridge interface:
//  # the BLT (block transfer readout)
//  # the CBLT (chained block transfer readout)
//  # Hibrid (CBLT + BLT) readout 
//  # facilities and wrappers for single channel access. 
//  The class defines
//  the readout basic implementation independently
//  from the VME configuration that is managed by other 
//  classes. 
//  E.d.F.  (08/2007)  prototype test version (CBLT only) 
//  (Modified by Hanjianlong 07/2012)
////////////////////////////////////////////////////////////
#ifndef TCBLT_H
#define TCBLT_H

#include "caenacq.h"
#include <new>

class TReadout {
protected: 
	char *fiobuf;   //buffer for blt or cblt readout
	unsigned int fusedsize;  //used size in byte
	unsigned int fbufdim;    //dimension of 'fiobuf'
public:
	TReadout(int dim=VME_Crate_BufLENGTH);
	virtual ~TReadout();
	char *GetBLTBuff() {return fiobuf;}
	unsigned int GetUsedSizeByte() const {return fusedsize;}
	unsigned int GetBufDimension() const {return fbufdim;}
	void SetUsedSizeByte(unsigned int nbyte) {fusedsize = nbyte;}
	void swap32(unsigned int *buf, int max){};
};


//The CBLT class manages the chained block transfer 
//readout for one or more codifiers chains. The class 
//mandatory defines a copy constructor and assignment 
//operator=() definition 
class TCBLT : public TReadout {
private:
	int fchains;   //CBLT chains number
	int *fnum;     //pointer to board number for each chain
	int *fbase;    //pointer to base address for each chain
	int *fdummy;   //pointer to dummy address for each chain
	int foffset;         //offset respect to the first codifier for polling (use for not first board DRDY)
	int fwait_for_ready; //number of boards to lookup before readout
public:
	TCBLT(int bufsize=VME_Crate_BufLENGTH, int wait=1):
	  TReadout(bufsize), fchains(0), fnum(0), fbase(0), fdummy(0), foffset(0),
		  fwait_for_ready(wait) {}
	  TCBLT(TCBLT const &);                 //copy constructor
	  TCBLT &operator=(TCBLT const &);      //assignement operator 
	  virtual ~TCBLT();
	  int Get_CBLT_Config(string name, int crate);
	  int GetChains() {return fchains;}
	  int *GetNum() {return fnum;}
	  int *GetDummyAddr() {return fdummy;}
	  int GetWait() {return fwait_for_ready;}
	  int GetChainBoard(int chain);
	  void SetOffsetPolling(int off) {foffset=off;}
	  int GetOffsetPolling() {return foffset;}
	  void PrintCBLTInfo();
};

#endif
