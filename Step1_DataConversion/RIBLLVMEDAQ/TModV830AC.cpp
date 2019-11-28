////////////////////////////////////////////
// TModV830AC.h: Implementation of CAEN
// module V830AC
// All module class must be inherited form
// 'TBoard'
// Han Jianlong 07/2012
////////////////////////////////////////////

#include <iostream>
#include <cstring>
using namespace std;

#include "TModV830AC.h"
#include "TH1.h"
#include "TH1I.h"
#include "TString.h"
#include "Rtypes.h"
#include "caenacq.h"

unsigned int TModV830AC::facqreg[2] = {0x100E, 0x0001};
unsigned int TModV830AC::fMaxDataVal = 200;

static const short DataHeaderRsh=         26;        //Left shift of a int data to check if this is the Data Header
static const short DataHeaderLsh=          5;
static const int   DataHeaderMask= 0x4000000;
static const short DataMarkMask=         0x0;
static const short GeoRsh=                27;
static const short CNTRsh=                18;
static const short CNTLsh=                 8;
static const short TSLsh=                 14;
static const short TSRsh=                 16;
static const short ChNumbRsh=             27;
static const short ChNumbLsh=              0;
static const short DataValueLsh=           6;
static const short DataValueRsh=           6;
static const int   TriggNumMask=      0xFFFF;

static const short GeoAddrOffset=     0x1110;
static const short GeoMask=             0x1F;
static const short MaxGeo=                31;
static const short AMNESIA_Mask=        0x10;
static const short StaReg1AddrOffset= 0x100E;
static const short ConBitSetReg=      0x110A;
static const short EnableDataHeader=    0x20;
static const short Enable26bitMode=      0x4;

static unsigned int ChTempData[TModV830AC::fMaxChannel];

ClassImp(TModV830AC)

///////////////////////////////////////////
// This class definition only for V830AC
// which works in 26-bit mode, and with
// the Data-Header enabled.
///////////////////////////////////////////

void TModV830AC::Initialization()
{
	his1d = new TH1*[fMaxChannel];
	for(unsigned int i=0; i<fMaxChannel; i++) his1d[i] = 0;

	for(int i=0; i<fMaxChannel; i++) //initialize the temp variables
	{
		ChTempData[i] = 0;
	}
}

int TModV830AC::Decode(unsigned int *&data_point)
{
	//CleanChData();
	unsigned int tdata = (*data_point);
	unsigned short ChannelNums = 0;
	unsigned int TrigNums = 0, TrigType = 0;
	unsigned int dataMarker = tdata & DataHeaderMask;
	unsigned int mGeo = (tdata>>GeoRsh);

	if(mGeo != fGeo)
	{
		cout<< GetName() << " Data header error. Module Geo " << fGeo << " in Crate: " << fCrateNum <<"  do not match." << endl;
		data_point++;
		return 0;
	}
	if(dataMarker>0)
	{
		ChannelNums = (tdata<<CNTLsh)>>(CNTLsh+CNTRsh);
		TrigType = (tdata<<TSLsh)>>(TSLsh+TSRsh);
		TrigNums = tdata&TriggNumMask;
	}
	for(unsigned int ch=0; ch<ChannelNums; ch++)
	{
		data_point++;
		tdata = (*data_point);
		unsigned short channel = tdata>>ChNumbRsh;
		dataMarker = tdata & DataHeaderMask;
		if(dataMarker != 0)
		{
			cout<< GetName() << " Data error. Too much headers in Module Geo: " << fGeo << " in Crate: " << fCrateNum << endl;
			return 0;
		}
		if(channel>=fMaxChannel)
		{
			cout<< GetName() << " Data header error. Module channel: " << channel << "> fMaxChannel: " << fMaxChannel << endl;
			continue;
		}
		try
		{
			unsigned int temp = (tdata<<DataValueLsh)>>DataValueRsh;
			unsigned int htemp = 0;
			chdata[channel] = temp;
			if(temp < ChTempData[channel])
				htemp = 0x4000000 + temp - ChTempData[channel];
			else
				htemp = temp - ChTempData[channel];
			if(htemp>fMaxDataVal) htemp = fMaxDataVal;

			ChTempData[channel] = temp;
			if(htemp>0)
			{
				if(his1d[channel]) his1d[channel]->Fill(htemp);
			}
		}
		catch (exception& e)  //want to catch the Array Index Out Of Bounds Exception
		{
			cerr << "exception caught: " << e.what() << endl;
		}

	}
	//data_point++; //to next data header
	return TrigNums;
}

void TModV830AC::Create1DHistos()
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
		
		his1d[i] = new TH1I(hname.Data(), title.Data(), fMaxDataVal, 0.5, fMaxDataVal+0.5);
	}
}

TModV830AC::~TModV830AC()
{
	//SafeDeleteArr(chdata);
}

/////////////////////////////////////////////////
// There is some spetial func in this function.
// The 26-bit mode and Data-Header is enabled
// for V830AC. In this way, we make sure that
// this class implementation will work for the
// data analysis.
////////////////////////////////////////////////
int TModV830AC::WriteGeoToBoard(unsigned int geo)
{
	unsigned int wgeo = geo;
	unsigned int geomask = GeoMask;
	if(wgeo>MaxGeo)
	{
		cout << " Geo " << wgeo << " > " << MaxGeo << endl;
		return 0;
	}
	int wstatus = SingleWriteCycle(wgeo, geomask, GeoAddrOffset, 16);
	//set the 26-bit mode, and enable the Data-Header
	int wstatus1 = SingleWriteCycle(Enable26bitMode,  0x0, ConBitSetReg, 16);
	int wstatus2 = SingleWriteCycle(EnableDataHeader, 0x0, ConBitSetReg, 16);

	return (wstatus + wstatus1 + wstatus2);
}

unsigned int TModV830AC::GetChannelData(int chnum)
{
	if(chnum>=0 && chnum<fMaxChannel)
		return chdata[chnum];
	return 0;
}