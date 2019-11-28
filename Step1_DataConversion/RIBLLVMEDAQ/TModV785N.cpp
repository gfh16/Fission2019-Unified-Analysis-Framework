////////////////////////////////////////////
// TModV785N.cpp: Implementation of CAEN
// module V785NAC
// All module class must be inherited form
// 'TBoard'
// Han Jianlong 07/2012
////////////////////////////////////////////

#include <iostream>
using namespace std;

#include "TModV785N.h"
#include "TH1.h"
#include "TH1I.h"
#include "TString.h"
#include "Rtypes.h"
#include "caenacq.h"

unsigned int TModV785N::facqreg[2] = {0x100E, 0x0001};     //acquisition register and valid data mask(for data ready)
unsigned int TModV785N::fMaxDataVal = 4096;

static const short DataHeaderRsh=      24;        //Left shift of a int data to check if this is the Data Header
static const short DataHeaderLsh=       5;
static const short DataHeaderMask=    0x2;
static const short DataMarkMask=      0x0;
static const short DataEnderMask=     0x4;
static const short DataNotValidMask=  0x6;
static const short GeoRsh=             27;
static const short CrateRsh=           16;
static const short CrateLsh=            8;
static const short CNTRsh=              8;
static const short CNTLsh=             18;
static const short ChNumbRsh=          17;
static const short ChNumbLsh=          11;
static const int   UnderThrMask=   0x2000;
static const int   OverflewMask=   0x1000;
static const short DataValueLsh=       20;
static const short DataValueRsh=       20;
static const int   EventCountMask= 0xFFFFFF;

static const short GeoAddrOffset=     0x1002;
static const short CrateAddrOffset=   0x103C;
static const short BitSet1Register=   0x1006;
static const short BitClear1Register= 0x1008;
static const short CrateNumMask=        0xFF;
static const short GeoMask=             0x1F;
static const short MaxGeo=                31;
static const short MaxCrateNum=          255;
static const short AMNESIA_Mask=        0x10;
static const short StaReg1AddrOffset= 0x100E;

ClassImp(TModV785N)

void TModV785N::Initialization()
{
	his1d = new TH1*[fMaxChannel];
	for(unsigned int i=0; i<fMaxChannel; i++) his1d[i] = 0;
}

//DataReset of V785N, do this after the writting of Geo to the module
int TModV785N::DataReset()
{
	int set   = SingleWriteCycle(0x4, 0x4, 0x1032, 16);
	int clear = SingleWriteCycle(0x4, 0x0, 0x1034, 16);
	return (set+clear);
}

//SOFTW. RESET(SR) of this module, SR this module after write GEO
int TModV785N::SoftReset()
{
	int set   = SingleWriteCycle(0x80, 0x00, BitSet1Register, 16);
	int clear = SingleWriteCycle(0x80, 0x00, BitClear1Register, 16);
	return (set+clear);
}

int TModV785N::Decode(unsigned int *&data_point)
{
	//CleanChData();
	unsigned int tdata = (*data_point);
	unsigned int ChannelNums = 0;
	unsigned int ecounter = 0;
	unsigned int nCh = 0;
	unsigned int dataMarker = (tdata<<DataHeaderLsh)>>(DataHeaderRsh+DataHeaderLsh);
	if(dataMarker == DataNotValidMask){return -1;}  //for not valid data
	if(dataMarker == DataHeaderMask)
	{
		unsigned int mGeo   = (tdata>>GeoRsh);
		unsigned int mCrate = ( (tdata<<CrateLsh)>>(CrateRsh+CrateLsh) );
		if( (mGeo != fGeo) || (mCrate != fCrateNum))
		{
			cout << GetName() << " Data header error. Module Geo " << fGeo << " or Crate number " << fCrateNum <<"  do not match." << endl;
			return 0;
		}
		ChannelNums = (tdata<<CNTLsh)>>(CNTRsh+CNTLsh);

		while( (dataMarker != DataEnderMask) && (nCh<=ChannelNums) ) //loop to the end of the data belongs to this module
		{
			data_point++;
			tdata  = (*data_point);
			//cout << tdata << endl;
			dataMarker = (tdata<<DataHeaderLsh)>>(DataHeaderRsh+DataHeaderLsh);
			if(dataMarker == DataEnderMask)
			{
				ecounter = tdata&EventCountMask;
			}
			if(dataMarker == DataMarkMask)
			{
				unsigned int channel = (tdata<<ChNumbLsh)>>(ChNumbRsh+ChNumbLsh);
				if(channel>=fMaxChannel)
				{
					cout << GetName() << " Data error. Channel number > fMaxChannel."<< endl;
					return 0;	
				}
				bool dunvalid = (tdata & UnderThrMask)||(tdata & OverflewMask);
				if(!dunvalid)
				{
					unsigned int chd = (tdata<<DataValueLsh)>>DataValueRsh;
					if(chd<=fMaxDataVal)
					{
						chdata[channel] = (unsigned short)chd;
						if(his1d[channel]) his1d[channel]->Fill(chdata[channel]);
					}
				}
				nCh++;
				//cout << "channel N:" <<nCh << endl; 
			}		
		}
	}

	//if( dataMarker == DataEnderMask ) data_point++; //to the next data header

	return ecounter;
}

void TModV785N::Create1DHistos()
{
	TString hnamet = 'h';
	int hisname1 = (fCrateNum*100000 + fGeo*1000);
	int hisname2 = 0;
	TString htitle = 'T';
	int histitle1 = (fCrateNum*100000 + fGeo*1000);
	int histitle2 = 0;
	TString hname, title;
	for(unsigned int i=0; i<fMaxChannel; i++)
	{
		hname = hnamet;
		hisname2 = hisname1 + i;
		hname += hisname2;

		title = htitle;
		histitle2 = histitle1 + i;
		title += histitle2;

		his1d[i] = new TH1I(hname.Data(), title.Data(), fMaxDataVal, 0, fMaxDataVal);
	}
}

TModV785N::~TModV785N()
{
	//SafeDeleteArr(chdata);
}

int TModV785N::WriteCrateNumtoBoard(unsigned int cnum)
{
	if(cnum>MaxCrateNum)
	{
		cout << " Crate Number" << cnum << " Error." << endl;
		return 0;
	}
	unsigned int cratenum = cnum;
	unsigned int cratemask = CrateNumMask;  // cratenum & cratemask == cratenum
	int status = SingleWriteCycle(cratenum, cratemask, CrateAddrOffset, 16);
	return status;
}

int TModV785N::WriteGeoToBoard(unsigned int geo)
{
	unsigned int status_reg1 = 0;
	int status = SingleReadCycle(status_reg1, StaReg1AddrOffset, 16);
	if(!status)
	{
		cout << "Read Status Register 1 of module: " << geo << " error." << endl;
		return 0;
	}

	if( (status_reg1 & AMNESIA_Mask) > 0 )
	{
		unsigned int wgeo = geo;
		unsigned int geomask = GeoMask;
		if(wgeo>MaxGeo)
		{
			cout << " Geo " << wgeo << " > " << MaxGeo << endl;
			return 0;
		}
		int wstatus = SingleWriteCycle(wgeo, geomask, GeoAddrOffset, 16);
		DataReset();
		SoftReset();
		return wstatus;
	}
	else
	{
		return 1;
	}
}

unsigned int TModV785N::GetChannelData(int chnum)
{
	if(chnum>=0 && chnum<fMaxChannel)
		return chdata[chnum];
	return 0;
}