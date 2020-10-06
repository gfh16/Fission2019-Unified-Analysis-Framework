//////////////////////////////////////////////////////////
//  TVMELink.cpp include file   interfaces 
//  TVME class interface: V2718 manager
//  These classes define the DAQ hardware configuration
//  E.d.F (08/2007) test version  v.07
//  (Modified by Han jianlong 07/2012)
//  caen lib wrapper not yet implemented
/////////////////////////////////////////////////////////

#include <iostream>
#include <cstring>
using namespace std;

#include "TVMELink.h"
#include "CAENVMElib.h"

/////////////////////////////////////////////////////////////////////////////////
//static variables initialization
/////////////////////////////////////////////////////////////////////////////////
map<int,int> TVMELink::flookup_ind;


/////////////////////////////////////////////////////////////////////////////////
//constructor
/////////////////////////////////////////////////////////////////////////////////
TVMELink::TVMELink(CVBoardTypes ControlBoardType, short PCILinkNum, short VMEControlBoardNum, int crate)
{
	fControlBoardType      = ControlBoardType;
	fPCILinkNum            = PCILinkNum;
	fVMEControlBoardNum    = VMEControlBoardNum;
	fCrate  = crate;

	fStatus = cvGenericError;
	fHandle = -1;
}

/////////////////////////////////////////////////////////////////////////////////
//copy constructor
/////////////////////////////////////////////////////////////////////////////////
TVMELink::TVMELink(const TVMELink &source)
{
	copy(source);
}

/////////////////////////////////////////////////////////////////////////////////
//Init PCI-VME link 
/////////////////////////////////////////////////////////////////////////////////
int TVMELink::Init()
{
	if(fHandle>=0)  //protecting multi-call of this Init()
	{
		CVErrorCodes check = CAENVME_BoardFWRelease(fHandle, fHwrel);
		if(check == cvSuccess)
		{
			return check;
		}
		else
		{
			CAENVME_End(fHandle);
		}
	}
	//cout <<"CrateNum: " <<fCrate << " PCILinkNUM: " << fPCILinkNum << " ControlBoardNum: " << fVMEControlBoardNum <<endl;
	CVErrorCodes res = CAENVME_Init(fControlBoardType, fPCILinkNum, fVMEControlBoardNum, &fHandle);
	if(res != cvSuccess) 
	{
		cout<<"Fatal error: "<<res<<" of initializing V2718. PCIDeviceNum "<<fPCILinkNum<<" ControlBoardNum "<<fVMEControlBoardNum << " Handle " <<fHandle<<endl;
		fHandle = -1;
	}
	else 
	{
		cout<<"V2718>> PCIDeviceNum "<<fPCILinkNum<<" ControlBoardNum "<<fVMEControlBoardNum<<" initialized "<< " Handele " << fHandle<<endl;
		CAENVME_BoardFWRelease(fHandle, fHwrel);
		CAENVME_SWRelease(fSwrel);
		cout<<"Hardware release: "<<fHwrel<<" Software release: "<<fSwrel<<endl; 
		flookup_ind[fCrate] = fHandle;
	}
	fStatus = (CVErrorCodes )res;
	return res;
}

/////////////////////////////////////////////////////////////////////////////////
//Configure IO-output port
/////////////////////////////////////////////////////////////////////////////////
CVErrorCodes TVMELink::InitIOPort(CVOutputSelect out, CVIOPolarity pol, CVLEDPolarity led)
{
	if(fHandle>=0) {
		return (CVErrorCodes)CAENVME_SetOutputConf(fHandle, out, pol, led, cvManualSW);
	}
	else {
		cout<<"InitIOPort>> no valid handler given"<<endl;
		return (CVErrorCodes)cvGenericError;
	}
}

/////////////////////////////////////////////////////////////////////////////////
//Set the IO-output port
/////////////////////////////////////////////////////////////////////////////////
CVErrorCodes TVMELink::SetIOPort(CVOutputRegisterBits cvbit)
{
	if(fHandle>=0) {
		return (CVErrorCodes)CAENVME_SetOutputRegister(fHandle, cvbit);
	}
	else {
		cout<<"SetIOPort>> "<< cvbit <<" no valid handler given"<<endl;
		return (CVErrorCodes)cvGenericError;
	} 
}


/////////////////////////////////////////////////////////////////////////////////
//Clear the IO-output port 
/////////////////////////////////////////////////////////////////////////////////
CVErrorCodes TVMELink::ClearIOPort(CVOutputRegisterBits cvbit)
{
	if(fHandle>=0) {
		return (CVErrorCodes)CAENVME_ClearOutputRegister(fHandle, cvbit);
	}
	else {
		cout<<"ClearIOPort>> " << cvbit <<" no valid handler given"<<endl;
		return (CVErrorCodes)cvGenericError;
	} 
}

CVErrorCodes TVMELink::PulseOutput(CVOutputRegisterBits cvbit)
{
	if(fHandle>=0) {
		return (CVErrorCodes)CAENVME_PulseOutputRegister(fHandle, cvbit);
	}
	else {
		cout<<"PulseIOPort>> "<< cvbit << " no valid handler given"<<endl;
		return (CVErrorCodes)cvGenericError;
	} 
}

void TVMELink::copy(const TVMELink &source)
{
	fHandle = source.fHandle;
	strncpy(fHwrel, source.fHwrel, sizeof(fHwrel));
	strncpy(fSwrel, source.fSwrel, sizeof(fSwrel));
	fStatus = source.fStatus;
	fControlBoardType  = source.fControlBoardType;
	fPCILinkNum = source.fPCILinkNum;
	fVMEControlBoardNum   = source.fVMEControlBoardNum;
	fCrate  = source.fCrate;
}

TVMELink::~TVMELink()
{
	//CAENVME_End(fHandle);
}

CVErrorCodes TVMELink::EndVMEHandle()
{
		if(fHandle>=0) return CAENVME_End(fHandle);
		return cvInvalidParam;
}