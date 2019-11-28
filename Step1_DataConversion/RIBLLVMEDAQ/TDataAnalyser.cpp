////////////////////////////////////////////////////
// TDataAnalyser.cpp: Global data decoder, used for
// data analysis.
// Han jianlong (08/2012)
///////////////////////////////////////////////////

#include <exception>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <list>
using namespace std;

#include "caenacq.h"
#include "TDataAnalyser.h"
#include "TConfig.h"
#include "TBoard.h"

map<int, TBoard *> TDataAnalyser::CrateGeo_Mod;
map<int, unsigned int> eventcountermod;
bool ecerror = false;

TDataAnalyser::TDataAnalyser(TConfig *config)
{
	int status = config->GetConfigNames();
	if(status<0) return;
	status = config->ReadConfigForAnalysis();
	if(status<0) return;

	list<TBoard *> boardlist = config->GetBoardList();
	list<TBoard *>::const_iterator it = boardlist.begin();
	for(it; it!=boardlist.end(); it++)
	{
		unsigned int tcrate = (*it)->GetCrateNum();
		unsigned int tgeo   = (*it)->GetGeo();
		unsigned int pseudocg = tcrate*100 + tgeo;
		geotable.push_back(pseudocg);
		//int mod = (*it)->GetCrateNum() * 100 + tgeo;
		CrateGeo_Mod[pseudocg] = (*it);
		eventcountermod[pseudocg] = 0;
	}
}

//evtbuf: event buffer; num: event length in int
unsigned int TDataAnalyser::GlobalDecoder(unsigned int *&evtbuf, int num)
{
	unsigned int  eventcount=0, cratenum=0, geo=0;
	unsigned int  pointerid = 0;
	unsigned int* sentinel = evtbuf;
	if(evtbuf[0] == Event_Header) return -1;     // first data should be the eventcounter(Max:0x10000000)
	eventcount = evtbuf[0];
	evtbuf += 1; //to the crate header
	if((*evtbuf) != Crate_Header) return -1;     // data format error

	bool geook = false;

	try
	{
		InitAllBoardData(); //set all data to 0 (out of the event 'do' loop)
		do  //crate numbers loop (loop on one event)
		{
			if((*evtbuf) == Event_Header) break;
			evtbuf += 1; //to the crate id
			cratenum = *evtbuf;
			evtbuf += 7; //skip 'crate header' data
			ecerror = false;
			do //modules numbers loop
			{
				if((*evtbuf) == Event_Header) break;
				if((*evtbuf) == Crate_Header) break;  //found 'crate_header' break
				//this "Geo" function is very important
				geo = GlobalGeo(evtbuf);
				pointerid = cratenum*100 + geo;
				TBoard *mod = GetTBoardPointer(cratenum, geo);
				int necounter = -1;
				if(mod)
				{
					necounter = mod->Decode(evtbuf);  //Send the data to the module classs its self, main analysis procedure.
					if(necounter>0)
					{
						evcheck(necounter, pointerid, mod);
					}
				}
				evtbuf++; //to next data
			}while(evtbuf<(sentinel + num)); //modules numbers loop
			evcheckprint();
		}while(evtbuf<(sentinel + num)); //crate numbers loop
	}
	catch(exception &e)
	{
		cerr << "exception caught: " << e.what() << endl;
	}
	return eventcount;
}


//This 'GlobalGeo' function is very important, all modules must be
//'identified', i.e. get its unique id, in this function.
//
//For module V775*, V785*, V830, V792*, this function 'geo = tdata>>27'
//For module V1190*, V1290* ... this function is different, and may 
//confused with other kind of modules(like V7*** family), however you
//must manage to identify them.
//An alternative way is read the modules which may cause confusion 
//separately (i.e. do not read them in the main CBLT loop) by calling
//the 'VMEReadBeforeCBLT(...)' and 'VMEReadAfterCBLT(...)' function,
//in this way you can 'write' a 'mark' value in the data file(similar to 
//'Event_Header').
//Any how, all modules in the data must be identified in this function.
unsigned int TDataAnalyser::GlobalGeo(const unsigned int *const evtbuf)
{
	//do not change the value of 'evtbuf', i.e. do not do something
	//like: *evtbuf = n, *(evtbuf+1) =n, evtbuf++, evtbuf += 1 ...

	unsigned int tdata = *evtbuf;
	unsigned int tgeo = tdata>>27;
	return tgeo;

	//an example about how to identify V1290
	//example beging-------------------------------

	//if(tgeo == 8) //data 'global header' of V1290
	//{
	//	tgeo = *(evtbuf+1); // following 'global header' is the 'the TDC Header'
	//	if(tgeo == 1) return (tdata<<27)>>27; // the true 'Geo' of V1290
	//}

	//example end----------------------------------

	//an example about how to identify data which is not read by CBLT
	//example beging-------------------------------
	//if(tdata == 0x2A00FFFF) return tdata>>26;
	//if(tdata == 0x2B00FFFF) return tdata>>26;
	//example end----------------------------------

}

//Get the TBoard* pointer by the pseudo id: Crate*100 + Geo
TBoard* TDataAnalyser::GetTBoardPointer(const unsigned int crate, const unsigned int geo)
{
	bool cgok = false;
	unsigned int pseudocg = crate*100 + geo;
	for(unsigned int i=0; i<geotable.size(); i++)
	{
		if(pseudocg == geotable.at(i))
		{
			cgok = true;
			break;
		}
		else
		{
			cgok = false;
		}
	}

	if(cgok)
	{
		return CrateGeo_Mod[pseudocg];
	}
	else
	{
		return 0;
	}
}

void TDataAnalyser::InitAllBoardData()
{
	map<int, TBoard *>::iterator it = CrateGeo_Mod.begin();
	for(it; it != CrateGeo_Mod.end(); it++)
	{
		(*it).second->CleanChData();
	}
}

unsigned int TDataAnalyser::GetRawData(unsigned int Crate, unsigned int Geo, unsigned int channel)
{
	unsigned int CGid = Crate*100 + Geo;
	unsigned int value = 0;
	TBoard *mod = GetTBoardPointer(Crate, Geo);
	if(mod)
	{
		value = mod->GetChannelData(channel);
	}
	return value;
}

void TDataAnalyser::evcheck(int necounter, int pointerid, TBoard * const mod)
{
	int diff = -1;
	diff = necounter - eventcountermod[pointerid];
	if(diff>=0 && diff!=1 && eventcountermod[pointerid]!=0)
	{
		cout << "Mod" << pointerid << "-" << mod->GetName() << " eventcounter error? last ec1= " << eventcountermod[pointerid] << " this ec2= " << necounter << endl;
		ecerror = true;
	}
	eventcountermod[pointerid] = necounter;
}

void TDataAnalyser::evcheckprint()
{
	if(ecerror)
	{
		map<int, unsigned int>::iterator it;
		for(it=eventcountermod.begin(); it!=eventcountermod.end(); it++)
			cout << "Mod" << it->first <<" this ec= " << it->second << endl;
	}
}