////////////////////////////////////////////////////////
//  TCrateCBLT.h include file   interfaces 
//  These classes defined(combination) the CBLTs
//  in a VME crate
//  E.d.F (08/2007) test version  v.07 
//  caen lib wrapper not yet implemented
//  Modified by Hanjianlong 07/2012
///////////////////////////////////////////////////////

#ifndef TCrateCBLT_H
#define TCrateCBLT_H

#include <string>
using namespace std;

#include "TVMELink.h"
#include "TCBLT.h"
#include "TString.h"

class TCrateCBLT : public TVMELink {
private:
	TCBLT freadout;
	int findex;
	bool fenabled;        //enabled for readout (true) or disabled (false)
	int fwait_for_ready;  //number of crates to be wait for readout
	int fchains;          //number of chains
	int *fcbltnum;        //board number for each chain 
	int *fcblt_addr;      //dummy address for each chain 
	int *fcbltloop;       //max number of cblt loop  
	unsigned int *faddr;
	unsigned int *fmask;  //address and mask cblt
	TString vmeerror;     // Vme error during read data

	int status;           //freadout.Get_CBLT_Config(..) status

public:
	static int fstatus;
	static bool fready;
	static bool fokstop;

public:
	TCrateCBLT(TVMELink &link, TCBLT &readout, int index, string configfile);
	//TCrateCBLT(CVBoardTypes ControlBoardType=cvV2718, short PCILinkNum=0, short VMEControlBoardNum=0, 
	//	int fCrate=1, TCBLT readout=0) : 
	//TVMELink(ControlBoardType, PCILinkNum, VMEControlBoardNum, fCrate), freadout(readout) {} // {freadout=readout;}
	TCrateCBLT(TCrateCBLT const &source);
	~TCrateCBLT();
	TCBLT &Get_Readout() {return freadout;}
	void Set_Readout(TCBLT &readout) {freadout=readout;}
	void SetReadoutEnable(bool status) {fenabled=status;}
	bool GetReadoutEnable() {return fenabled;}
	void ReadoutProcsInit(bool print=true);
	int EnableIRQ();    //make this V2718 answer the IRQ lines
	int WaitForReady();
	void Readout();
	void copy(TCrateCBLT const &source);
	int GetCBLTConfigStatus(){return status;}
};

#endif //#ifndef TCrateCBLT_H