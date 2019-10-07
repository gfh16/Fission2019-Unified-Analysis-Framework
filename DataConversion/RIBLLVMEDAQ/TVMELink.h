//////////////////////////////////////////////////////////
//  TVMELink.h include file   interfaces 
//  TVME class interface: V2718 manager
//  These classes define the DAQ hardware configuration
//  E.d.F (08/2007) test version  v.07
//  (Modified by Han jianlong 07/2012)
//  caen lib wrapper not yet implemented
/////////////////////////////////////////////////////////

// The Caen bridge V2718 class 

#ifndef TVMELink_H
#define TVMELink_H

#include <map>
#include "caenacq.h"
#include "CAENVMEtypes.h"
using namespace std;

class TVMELink {
protected:
	int  fHandle;                      // V2718 handle
	char fHwrel[32];                   // V2718 hardware release number 
	char fSwrel[32];                   // V2718 software release number 
	CVErrorCodes fStatus;              // V2718 error codes 
	CVBoardTypes fControlBoardType;    // V2718 board type 
	short fPCILinkNum;                 // V2718 PCI device  
	short fVMEControlBoardNum;         // V2718 VME-PCI link 
	int   fCrate;                      // virtual crate number associated with V2718 
public:
	static map<int, int> flookup_ind;      //map crate-handle corrispondence

public:
	TVMELink(CVBoardTypes ControlBoardType=cvV2718, short PCILinkNum=0, short VMEControlBoardNum=0, int crate=1);
	TVMELink(TVMELink const &source);
	virtual ~TVMELink(); 
	void SetCrateNum(int crate) {fCrate=crate;}
	long GetHandle() {return fHandle;}
	int GetCrateNum() {return fCrate;}
	short GetDevice() {return fPCILinkNum;}
	short GetLink() {return fVMEControlBoardNum;}
	char *GetHWRel() {return fHwrel;}
	char *GetSWRel() {return fSwrel;}  
	int Init();
	CVErrorCodes EndVMEHandle();
	void copy(TVMELink const &source);
	CVErrorCodes InitIOPort(CVOutputSelect, CVIOPolarity, CVLEDPolarity);
	CVErrorCodes SetIOPort(CVOutputRegisterBits cvbit);       //level up
	CVErrorCodes ClearIOPort(CVOutputRegisterBits cvbit);     //level down  
	CVErrorCodes PulseOutput(CVOutputRegisterBits cvbit);     //generate a pulse
};

#endif //#ifndef TVMELink_H