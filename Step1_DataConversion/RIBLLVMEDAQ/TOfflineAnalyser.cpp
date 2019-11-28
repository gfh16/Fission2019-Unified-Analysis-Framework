////////////////////////////////////////////////
// TOfflineAnalyzer.cpp: Offline analyzer used
// for data analysis.
// Han Jianlong (08/2012)
////////////////////////////////////////////////

#include <map>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#include "TOfflineAnalyser.h"
#include "TDataAnalyser.h"
#include "TDataFileReader.h"
#include "TBoard.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

TDataFileReader *TOfflineAnalyzer::freader=0;
TDataAnalyser   *TOfflineAnalyzer::danalyzer=0;

typedef struct Str_flist //describe the RAW data list file format
{
	char           infilename[80];
	unsigned int   nevtforana;
} D_flist;


TOfflineAnalyzer::TOfflineAnalyzer(TDataFileReader *filer, TDataAnalyser *anadata)
{
	freader = filer;
	danalyzer = anadata;
	rootfile = 0;
	CrateGeoMap = danalyzer->GetCrateGeoMap();
}

bool TOfflineAnalyzer::SetListFile(const char *listfn)
{
	fnamelist.open(listfn);
	return ( fnamelist.good() );
}

//this function is not used, maybe have mistakes
bool TOfflineAnalyzer::LoopLsitFile(const char *listfn)
{
	if(fnamelist.good()) fnamelist.close();
	fnamelist.open(listfn);
	if(!fnamelist.good()) return false;

	string item;
	char filename[300];
	try
	{
		while( (fnamelist.peek() != EOF) && (!fnamelist.fail()) )
		{
			filename[0] = '\0';
			getline(fnamelist, item);
			if(item.c_str()[0] == '*') continue;  //skip the comments
			istringstream s_item(item);
			if(item.size() > 0)
			{
				s_item >>filename;
				cout << "File: " << filename;
				bool bopen = freader->OpenDataFile(filename);
				if(!bopen) continue;
				OpenRootFile(filename);
				CreateTH1I();
				CreateTree();
				freader->SkipFileHeader();     //skip the data file header;
				bool readsucc = true;
				do
				{
					readsucc = freader->EvtReadingLoop();
					danalyzer->GlobalDecoder(freader->GetEvtBuf(), freader->GetEventLength()); //data analysis function
					tree->Fill();
				}while(readsucc);
				WriteRootFile();
				CloseRootFile();
			}
			item.empty();
		}
	}
	catch(ios::failure &e)
	{
		cout << "TOfflineAnalyzer>> file: " << listfn << " read error! " << e.what()<< endl;
		return false;
	}
	return true;
}

void TOfflineAnalyzer::OpenRootFile(const char* rfname)
{
	if(rootfile) CloseRootFile();
	TString tfname = rfname;
	tfname.ReplaceAll(".", "");
	tfname += ".root";
	rootfile = new TFile(tfname.Data(), "RECREATE");
}

void TOfflineAnalyzer::CloseRootFile()
{
	if(rootfile) rootfile->Close();
	rootfile = 0;
}

void TOfflineAnalyzer::WriteRootFile()
{
	rootfile->Write();
}

void TOfflineAnalyzer::CreateTH1I()
{
	map<int, TBoard*>::const_iterator it=CrateGeoMap->begin();
	
	for(it; it!=CrateGeoMap->end(); it++)
	{
		(*it).second->Create1DHistos();
	}
}

void TOfflineAnalyzer::CreateTree()
{
	tree = new TTree("RawData", "ModuleData");
	map<int, TBoard*>::const_iterator it=CrateGeoMap->begin();
	for(it; it!=CrateGeoMap->end(); it++)
	{
		TString tname = (*it).second->Class_Name();
		tname += (*it).first;
		tree->Branch(tname.Data(), (*it).second->Class_Name(), (*it).second);
	}
}

//this function is not used, maybe have mistakes
void TOfflineAnalyzer::AnaSingleDataFile(const char *dfile, int nevt_ana)
{
	bool bopen = freader->OpenDataFile(dfile);
	if(!bopen) return;
	OpenRootFile(dfile);
	CreateTH1I();
	CreateTree();
	freader->SkipFileHeader();     //skip the data file header;
	bool readsucc = true, b_nevt = true;
	int nevtana = 0;
	do
	{
		readsucc = freader->EvtReadingLoop();
		danalyzer->GlobalDecoder(freader->GetEvtBuf(), freader->GetEventLength()); //data analysis function
		tree->Write();
		nevtana++;
		if( nevt_ana>0 && nevtana>=nevt_ana) b_nevt = false;
	}while(readsucc && b_nevt);
	WriteRootFile();
	CloseRootFile();
}