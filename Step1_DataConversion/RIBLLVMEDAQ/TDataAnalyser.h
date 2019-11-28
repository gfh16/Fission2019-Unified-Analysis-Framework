////////////////////////////////////////////////////
// TDataAnalyser.h: Global data decoder, used for
// data analysis.
// Han jianlong (08/2012)
///////////////////////////////////////////////////

#ifndef TDataAnalyser_H
#define TDataAnalyser_H

#include <map>
#include <vector>
using namespace std;

class TConfig;
class TBoard;

class TDataAnalyser
{
public:
	static map<int, TBoard *> CrateGeo_Mod;  // <crate*100+Geo, pointer>, pointer = this module (TBoard*)
	vector<unsigned int> geotable;           // pseudo crate_geo table: Crate*100 + Geo

public:
	TDataAnalyser(TConfig *config);
	virtual ~TDataAnalyser(){};
	unsigned int GlobalDecoder(unsigned int * &evtbuf, int num);
	unsigned int GlobalGeo(const unsigned int *const evtbuf);
	map<int, TBoard *> *GetCrateGeoMap() const {return &CrateGeo_Mod;}
	TBoard* GetTBoardPointer(const unsigned int crate, const unsigned int geo);
	void InitAllBoardData();
	unsigned int GetRawData(const unsigned int Crate, const unsigned int Geo, const unsigned int channel);
	void evcheck(int necounter, int pointerid,  TBoard * const mod); //do some check of the event counter of the model
	void evcheckprint(); //print some check errors
};




#endif //#ifndef TDataAnalyser_H
