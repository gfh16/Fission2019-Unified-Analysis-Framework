////////////////////////////////////////////////////////
//  TCrateCBLT.cpp source file   interfaces 
//  These classes defined(combination) the CBLTs
//  in a VME crate
//  E.d.F (08/2007) test version  v.07 
//  caen lib wrapper not yet implemented
//  Modified by Hanjianlong 07/2012
///////////////////////////////////////////////////////

#include <iostream>
#include <list>
#include <string>
using namespace std;

#ifdef WIN32
#include <windows.h>
#define sleep Sleep
#define WaitSec 100
#else
#include <unistd.h>
#define sleep usleep
#define WaitSec 100000
#endif

#include "TCrateCBLT.h"
#include "TBoard.h"
#include "CAENVMElib.h"
#include "TConfig.h"
#include "caenacq.h"

TCrateCBLT::TCrateCBLT(TVMELink &link, TCBLT &readout, int index, string configfile) : TVMELink(link), freadout(readout)
{
	findex = index;
	fenabled = false;
	fcbltnum  = 0; 
	fcblt_addr= 0; 
	fcbltloop = 0;  
	faddr = 0;
	fmask = 0;
	status = freadout.Get_CBLT_Config(configfile, fCrate);
	if(status == 0)
	{
		if(freadout.GetChains()>0)
		{
			fenabled = true;
		}
		else
		{
			fenabled = false;
		}
	}
	ReadoutProcsInit();
}

/////////////////////////////////////////////////////////////////////////////////
//This is the readout process initilization for CBLT readout 
/////////////////////////////////////////////////////////////////////////////////
void TCrateCBLT::ReadoutProcsInit(bool print)
{
	list <TBoard *>::const_iterator it;
	int num=0;

	// preparing readout....
	if(print) cout<<"READOUT_PROCESS_INIT>> Crate=#"<<fCrate<<"  Handle=#"<<fHandle<<"  Index=#"<<findex<<endl;
	Init();
	fwait_for_ready = freadout.GetWait();    //loop number for data ready
	fchains = freadout.GetChains();          //number of chains
	fcbltnum = freadout.GetNum();            //board number for each chain 
	fcblt_addr = freadout.GetDummyAddr();    //dummy address for each chain 
	if(fcbltloop) SafeDeleteArr(fcbltloop);
	fcbltloop = new int[fchains];
	for(int i=0; i<fchains; i++)
	{
		fcbltloop[i] = fcbltnum[i] + 1;         //max number of cblt cycles for chains
	}
	int offset = freadout.GetOffsetPolling();
	if(faddr) SafeDeleteArr(faddr);
	faddr = new unsigned int[fwait_for_ready];
	if(fmask) SafeDeleteArr(fmask);
	fmask = new unsigned int[fwait_for_ready]; 
	it = TConfig::LookupTable(fHandle, num);    //Get the first board of each handle(crate)
	if(offset>0) {
		if(num>0 && (offset+fwait_for_ready-1) <num) { 
			int k=offset;
			while(k--) {
				it++;
			}
		}
		else {
			cout<<"READOUT_PROCESS_INIT>> Crate=#"<<fCrate<<
				" Request for polling not existing board ignored"<<endl;
		}
	}
	if(num!=0) {
		for(int i=0; i<fwait_for_ready; i++) {
			faddr[i] = (*it)->GetAcqReg();
			fmask[i] = (*it)->GetAcqRegMask();
			it++;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////
// Polling the status register for data ready
/////////////////////////////////////////////////////////////////////////////////
int TCrateCBLT::WaitForReady()
{
	int count=0, status = 0;
	CVDataWidth data_size = cvD16;

	if(!fenabled)return 1;
#ifndef Wait_Data_Ready_IRQ  //use the DRY register of module as strobe
	CVErrorCodes err;
	count=0;
	for(int i=0; i<fwait_for_ready; i++) {
		int rdata = 0;
		err = CAENVME_ReadCycle(fHandle, faddr[i], &rdata, cvA32_U_DATA, data_size);
		rdata &= fmask[i];
		if(rdata>0)count++;
	}
	if(count==fwait_for_ready) 
		return 1;
	else
		return 0; 
#else
	if(fwait_for_ready>0)   //use IRQ as strobe
	{
		status = CAENVME_IRQEnable(fHandle, IRQ_Level);
		if(status != 0)
		{
			cout << "TCrateCBLT>> VMEbus IRQEnable error!" << endl;
			sleep(WaitSec);
			return 0;
		}
		status = CAENVME_IRQWait(fHandle, IRQ_Level, 10000);
		if(status == 0)
		{
			status = CAENVME_IRQDisable(fHandle, IRQ_Level);
			if(status != 0)
			{
				cout << "TCrateCBLT>> VMEbus IRQDisable error!" << endl;
			}
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 1;
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////////
// Readout
/////////////////////////////////////////////////////////////////////////////////
void TCrateCBLT::Readout()
{
	const int header_w = 8;
	const int SIZE = 256;
	unsigned int *acqbuf = (unsigned int *)(freadout.GetBLTBuff());//TEvtBuilder::fevtbuffer[findex].evtbuf;
	unsigned int *curr_ptr = acqbuf;
	int data_w, buflen;
	//Warning: "chains = 1" (chainid=0) only is managed in this version(cancele this rule by Han Jianlong)
	const int chainid = 0; 
	int cblt_counter,val[4]={0,0xFF,0xFFFF,0xFFFFFF},act_length;
	unsigned int bufdimension = freadout.GetBufDimension();

	data_w = 0;
	buflen = 0;
	*curr_ptr = Crate_Header;// | TEvtBuilder::fevent_counter;
	*(curr_ptr+1) = fCrate;
	curr_ptr += header_w;
	int len = 0;
	for(int nchain=0; nchain<fchains; nchain++)
	{
#ifndef FIFO_Read_Mode
		for(int i=0; i<fcbltloop[nchain]; i++) {
			CVErrorCodes res=CAENVME_BLTReadCycle(fHandle, fcblt_addr[nchain], (unsigned char *)curr_ptr, SIZE, cvA32_U_BLT, cvD32, &len);
			//int res=CAENVME_FIFOBLTReadCycle(fHandle, fcblt_addr[nchain], (unsigned char *)curr_ptr, SIZE, cvA32_U_BLT, cvD32, &len);
			data_w +=len;
			curr_ptr += len>>2;
			if(res<0)
			{
				cout << "CAENVME_FIFOBLTReadCycle Error: " << res << endl;
				vmeerror.Clear();
				vmeerror = "CAENVME_FIFOBLTReadCycle Error: ";
				vmeerror += res;
				break;
			}
		}
#else
		for(int nb=0; nb<fcbltnum[nchain]+1; nb++)
		{
			CVErrorCodes res=CAENVME_FIFOBLTReadCycle(fHandle, fcblt_addr[nchain], (unsigned char *)curr_ptr, bufdimension, cvA32_U_BLT, cvD32, &len);
			data_w += len;
			curr_ptr += len>>2; // byte->int
			//if(len<=0)
			//{
			//	cout << "CAENVME_FIFOBLTReadCycle Error: " << res << " Read length: " << len << endl;
			//	vmeerror.Clear();
			//	vmeerror = "CAENVME_FIFOBLTReadCycle Error: ";
			//	vmeerror += res;
			//	//break;
			//}
			if(res<0) break;
		}
#endif
	}
	cblt_counter = data_w>>2; // byte->int
	act_length = (cblt_counter+header_w)<<2;
	freadout.SetUsedSizeByte(act_length); //set real length of the data in byte
	//TEvtBuilder::fevtbuffer[findex].size = buflen;
}

TCrateCBLT::~TCrateCBLT()
{
	fcbltnum    = 0; //delete[] in TCBLT
	fcblt_addr  = 0; //delete[] in TCBLT
	SafeDeleteArr(fcbltloop);
	SafeDeleteArr(faddr);
	SafeDeleteArr(fmask);
}

TCrateCBLT::TCrateCBLT(TCrateCBLT const &source):TVMELink(source)
{
	copy(source);
}

void TCrateCBLT::copy(TCrateCBLT const &source)
{
	
	freadout          = source.freadout;
	findex            = source.findex;
	fenabled          = source.fenabled;
	fwait_for_ready   = source.fwait_for_ready;
	fchains           = source.fchains;
	fcbltnum          = freadout.GetNum();
	fcblt_addr        = freadout.GetDummyAddr();
	fcbltloop = new int[fchains];
	for(int i=0; i<fchains; i++)
	{
		fcbltloop[i] = fcbltnum[i] + 1; //max number of cblt cycles for chains
	}

	list <TBoard *>::const_iterator it;
	int num = 0;
	int offset = freadout.GetOffsetPolling();
	faddr = new unsigned int[fwait_for_ready];
	fmask = new unsigned int[fwait_for_ready];
	it = TConfig::LookupTable(fHandle, num);    //Get the first board of each handle(crate)
	if(offset>0) {
		if(num>0 && (offset+fwait_for_ready-1) <num) { 
			int k=offset;
			while(k--) {
				it++;
			}
		}
		else {
			cout<<"READOUT_PROCESS_INIT>> Crate=#"<<fCrate<<
				" Request for polling not existing board ignored"<<endl;
		}
	}
	if(num!=0) {
		for(int i=0; i<fwait_for_ready; i++) {
			faddr[i] = (*it)->GetAcqReg();
			fmask[i] = (*it)->GetAcqRegMask();
			it++;
		}
	}
	//cout << "TCrateCBLT>> Copy constructor... " << fCrate << endl;
}

int TCrateCBLT::EnableIRQ()
{
	if(fwait_for_ready>0)   //use IRQ as strobe
	{
		status = CAENVME_IRQEnable(fHandle, IRQ_Level);
		if(status != 0)
		{
			cout << "TCrateCBLT>> VMEbus IRQEnable error!" << endl;
			sleep(WaitSec);
			return 0;
		}
	}
	return 1;
}