////////////////////////////////////////////////
// TOfflineAnalyzer.h: Offline analyzer used
// for data analysis.
// Han Jianlong (08/2012)
////////////////////////////////////////////////

#ifndef TOfflineAnalyzer_H
#define TOfflineAnalyzer_H

#include <fstream>
#include <map>
using namespace std;

class TDataFileReader;
class TDataAnalyser;
class TFile;
class TBoard;
class TTree;

class TOfflineAnalyzer
{
public:
	static TDataFileReader *freader;
	static TDataAnalyser   *danalyzer;
	ifstream        fnamelist;
	ifstream        datafile;
	TFile           *rootfile;
	TTree           *tree;
	map<int, TBoard*> *CrateGeoMap;

public:
	TOfflineAnalyzer(TDataFileReader *filer, TDataAnalyser *anadata);
	virtual ~TOfflineAnalyzer(){}
	bool SetListFile(const char* listfn);
	bool LoopLsitFile(const char* listfn);
	void AnaSingleDataFile(const char* dfile, int nevt_ana);
	void Analysis();
	void CreateTree();
	void CreateTH1I();
	void OpenRootFile(const char* rfname);
	void CloseRootFile();
	void WriteRootFile();
};

#endif //#ifndef TOfflineAnalyzer_H