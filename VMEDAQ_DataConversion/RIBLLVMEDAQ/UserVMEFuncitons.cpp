/////////////////////////////////////////////////////
// UserVMERead.cpp: Define two VME Read Function
// Some modules can not be read by using CBLT, or 
// the user want to read the VME module for some 
// special purpose, in this case, the user can write
// some thing in this file.
// Han Jianlong 08/2012
/////////////////////////////////////////////////////

////************************************************////
//// All data read form VME modules must be 32bit
//// width. If the modules only support 16bit data
//// mode, you must completing the data to 32bit.
////************************************************////

#include "caenacq.h"
#include "CAENVMEtypes.h"
#include "CAENVMElib.h"
#include "TConfig.h"

///////////////////////////////////////////////////////
// Use the "#define UserVMEFunction.." in 'caenacq.h'
// to switch on this two functions.
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// This function defined some thing you want the
// program to do #before# the normal CBLT reading that
// you defined in the 'config' files.
// Read the content of each function carefully
// include the comments carefully before writing
// your own function.
///////////////////////////////////////////////////////
// Variables Note:
// buff: the pointer to the buffer where the read
//       data to be saved.
//       buff is a pointer, it need to be increased
//       with the real read number of 'int's
//
// gconfig: the TConfig class (always build in the 
//          'main' function, and then transfer to
//          the TEvtBuilder), used to get the
//          Handle by using the 'Crate Number'.
//
// rsize: return the real read number of 'int'
//
// All functions defined in "CAENVMElib.h" can be
// called in this function.
///////////////////////////////////////////////////////


int VMEReadBeforeCBLT(unsigned int*& buff, TConfig &gconfig)
{
////************************************************////
//// All data read form VME modules must be 32bit
//// width. If the modules only support 16bit data
//// mode, you must completing the data to 32bit.
////************************************************////

	//unsigned int fCrate = 1;
	//int fHandle = gconfig.GetHandle(fCrate);   // get the VME handle by using the crate number;

	//if(fHandle)
	//{
	//write the crate header and "data marker" to buff^^^^^^
	//	const int header_w = 8;
	//	*buff = Crate_Header;
	//	*(buff+1) = fCrate;
	//	buff += header_w;
	//	*buff = 0x2A00FFFF;
	//	buff++;
	//write the crate header and "data marker" to buff------

	//	int rlen=0;
	//	int SIZE = 256;
	//	int rsize = 0;
	//	int Address = 0x20040000;  // the VME bus address
	//	CVErrorCodes res=CAENVME_BLTReadCycle(fHandle, Address, (unsigned char *)buff, SIZE, cvA32_U_BLT, cvD32, &rlen);
	//	if(res == cvSuccess)
	//	{
	//		rsize = rlen>>2;       // rlen is the length in byte, '>>' transfered it to int(very important)
	//		buff += rsize;         // increase the pointer 'buff', the pointer 'buff' will be used in the following reading(very important)
	//		return rsize;          // return the real length in int size(very important)
	//	}
	//	*buff = 0x2A00FFFF;
	//	buff++;
	//}

	return 0; //un-normal return
}


//////////////////////////////////////////////////////
// This function defined some thing you want the
// program to do #after# the normal CBLT reading that
// you defined in the 'config' files.
// Read the content of each function carefully
// include the comments carefully before writing
// your own function.
//////////////////////////////////////////////////////
// Variables Note:
// buff: the pointer to the buffer where the read
//       data to be saved.
//       buff is a pointer, it need to be increased
//       with the real read number of 'int's
//
// gconfig: the TConfig class (always build in the 
//          'main' function, and then transfer to
//          the TEvtBuilder), used to get the
//          Handle by using the 'Crate Number'.
//
// rsize: return the real read number of 'int'
//
// All functions defined in "CAENVMElib.h" can be
// called in this function.
//////////////////////////////////////////////////////

int VMEReadAfterCBLT(unsigned int* &buff, TConfig &gconfig)
{
////************************************************////
//// All data read form VME modules must be 32bit
//// width. If the modules only support 16bit data
//// mode, you must completing the data to 32bit.
////************************************************////

	//unsigned int fCrate = 1;
	//int fHandle = gconfig.GetHandle(fCrate);   // get the VME handle by using the crate number;

	//if(fHandle)
	//{
	//write the crate header and "data marker" to buff^^^^^^
	//	const int header_w = 8;
	//	*buff = Crate_Header;
	//	*(buff+1) = fCrate;
	//	buff += header_w;
	//	*buff = 0x2B00FFFF;
	//	buff++;
	//write the crate header and "data marker" to buff------

	//	int rlen=0;
	//	int SIZE = 256;
	//	int rsize = 0;
	//	int Address = 0x20040000;  // the VME bus address
	//	CVErrorCodes res=CAENVME_BLTReadCycle(fHandle, Address, (unsigned char *)buff, SIZE, cvA32_U_BLT, cvD32, &rlen);
	//	if(res == cvSuccess)
	//	{
	//		rsize = rlen>>2;       // rlen is the length in byte, '>>' transfered it to int(very important)
	//		buff += rsize;         // increase the pointer 'buff', the pointer 'buff' will be used in the following reading(very important)
	//		return rsize;          // return the real length in int size(very important)
	//	}
	//	*buff = 0x2B00FFFF
	//	buff++;
	//}
	return 0; //un-normal return
}
