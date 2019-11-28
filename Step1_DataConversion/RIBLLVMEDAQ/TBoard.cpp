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

#include <stdio.h>
#include <string>
#include "TBoard.h"
#include "caenacq.h"
#include "CAENVMEtypes.h"
#include "CAENVMElib.h"
#include "Rtypes.h"

ClassImp(TBoard)

void TBoard::SetName(const char *name)
{
	strncpy(fname, name, sizeof(fname));
}

//Use for A32 address mode and D16 data width only
int TBoard::SingleWriteCycle(UInt_t data, UInt_t mask, UInt_t addroffset, int datasize)
{
	CVErrorCodes status;
	bool RWok = true;
	int rdata=0, loop=20;
	CVDataWidth data_size = cvD16;
	if(datasize == 32) data_size = cvD32;
	long BHandle = GetHandle();
	//	TBoardError err;
	unsigned int addr = fBaseAddr | addroffset;

	int wdata = data & 0x0000ffff;
	status = CAENVME_WriteCycle(BHandle, addr, &wdata, cvA32_U_DATA, data_size);  
	if(mask != 0x0000)
	{
		RWok = false;
		while(loop--) 
		{
			status = CAENVME_ReadCycle(BHandle, addr, &rdata, cvA32_U_DATA, data_size);
			if((rdata & mask) == data)
			{
				RWok = true;
				break;
			}  
			else 
			{
				status = CAENVME_WriteCycle(BHandle, addr, &wdata, cvA32_U_DATA, data_size);
			}
		}
		if(RWok==false)
		{
			printf("INIT>> Warning: Failed in writing 0x%04x to register 0x%08x\n", wdata, addr); 
			printf("INIT>> Handle=%d  ADDR = 0x%08x DATUM_READ = 0x%08x\n\n",BHandle, addr, rdata);
		}
	}
	if(RWok) 
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int TBoard::SoftReset(unsigned int data, unsigned int addroffset, int datasize)
{
	int status = SingleWriteCycle(data, 0, addroffset, datasize);
	return status;
}

int TBoard::SingleReadCycle(unsigned int& rdata, unsigned int addroffset, int datasize)
{
	CVErrorCodes status;
	CVDataWidth data_size = cvD16;
	if(datasize == 32) data_size = cvD32;
	long BHandle = GetHandle();
	if(BHandle<0) return 0;
	//	TBoardError err;
	unsigned int addr = fBaseAddr | addroffset;

	status = CAENVME_ReadCycle(BHandle, addr, &rdata, cvA32_U_DATA, data_size);

	if(status == 0) 
	{
		return 1;
	}
	else
	{
		return 0;
	}
}