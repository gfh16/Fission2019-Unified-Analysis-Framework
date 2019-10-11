///////////////////////////////////////////////
// Raw2ROOT.cpp: main() of Raw2ROOT, used
// for offline data analysis.
// Han Jianlong (08/2012)
///////////////////////////////////////////////

/////////////////////////////////////////////////////
// Add some explanations for the code
// Guan, Fenhai (09/2019)
///////////////////////////////////////////////////


#include "TApplication.h"
#include "TDAQApplication.h"
#include "TConfig.h"
#include "TDataFileReader.h"
#include "TDataAnalyser.h"
#include "TBoard.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"


#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <map>
using namespace std;

//const string gDataPath = "DAQDataPath";

void PrintUsage(char *name);

int main(int argc, char *argv[])
{
	string rawlist, rawfile;
	string dfname;
	unsigned int evtana=0;
	unsigned int startn=0;
	fstream listf;
	vector<string> rawdfname;
	bool inter = false;
	TTree *tree = 0;
	TFile *rfile = 0;

	//prepare the environment
	// The Server DAQ Application defining general environment
	TDAQApplication ribll(0, 0, false);
	//where the cblt setup files are
	//string pathchain = ribll.Get_PathEnvdir() + "/cblt_addr_crate";
	//where the filenameqdc and codifier setup file are
	string pathfilename = ribll.Get_PathEnvdir() + "/filenamemod.dat";
	string datapath = gSystem->Getenv(gDataPath.c_str()); // datapath = "./vmedata"
	string rootpath = "./rootdata";   //  used for directory of .root files (Added by gfh)
	cout<< "RawDataPath: " << datapath << endl;
	cout << endl;
	PrintUsage(argv[0]);
	cout << endl;


	if(argc == 1)     //程序参数只有一个，即每次转换单个文件： ./raw2root
	{
		cout <<"Input RawData File Name: ";
		cin >> dfname;
		cout <<"Input Start event number (0 for first event):";
		cin >> startn;
		cout <<"Input Event number to analyze (0 to end of file): ";
		cin >> evtana;
		inter = true;
	}
	else if(argc == 2)   //程序有两个参数，批量转换：./raw2root listfilename
	{
		rawlist  = datapath+"/"+argv[1];  // here to modify the directory of rawdata
		cout <<"List file of raw data files: " << argv[1] << endl;
		inter = false;
	}
	else
	{
		PrintUsage(argv[0]);
		return 0;
	}

//	inter=0;


	if(inter)
	{
		rawdfname.push_back(dfname);
	}
	else
	{
		string item;
		cout << rawlist.c_str() << endl;
		listf.open( rawlist.c_str() );
//		listf.open( "listfilename" );
		if( listf.fail() )
		{
			cout << "File: " << rawlist << " open error!" << endl;
			return 0;
		}
		while( (listf.peek() != EOF) && (!listf.fail()) )
		{
			getline(listf, item);
			cout << item << endl;
			TString tsitem(item);
			if(tsitem.IsWhitespace())  continue;  //skip blank lines
			if(item.c_str()[0] == '*') continue;  //skip the comments
			istringstream s_item(item);
			if(item.size() > 0)
			{
				s_item >> dfname;
				rawdfname.push_back(dfname);
			}
			item.empty();
		}
		listf.close();
	}

	TConfig conf(pathfilename.c_str());
	TDataAnalyser anadata(&conf);
	TDataFileReader DFReader;

	//loop on each file
	for(int i=0; i<rawdfname.size(); i++)
	{
		string rdfname = rawdfname[i];
		TString pathrdfn = datapath;
		pathrdfn += '/';
		pathrdfn += rdfname;
		if(!DFReader.OpenDataFile(pathrdfn.Data())) continue;
		//open root file
		TString strfname(rdfname);
		strfname.ReplaceAll(".", "");
		strfname += ".root";
	//	TString pathrootfn = datapath;
	  TString pathrootfn = rootpath;
		pathrootfn += '/';
		pathrootfn += strfname;   // output file in /rootdata/
		rfile = new TFile(pathrootfn.Data(), "RECREATE");
		//crate 1d histograms
		//UserTH_Init();    //create user defined histograms
		map<int, TBoard*> *CrateGeoMap = anadata.GetCrateGeoMap();
		map<int, TBoard*>::const_iterator it;
		for(it=CrateGeoMap->begin(); it!=CrateGeoMap->end(); it++)
		{
			(*it).second->Create1DHistos();
		}
		//crate tree
		tree = new TTree("RawData", "ModuleData");
		for(it=CrateGeoMap->begin(); it!=CrateGeoMap->end(); it++)
		{
			TString cname(typeid(*((*it).second)).name());
			cname.ReplaceAll("class ", ""); //for windows
			while(true)
			{
				TString ttemp(cname(0,1));
				if(ttemp.IsDigit())
				{
					cname.Replace(0, 1, "");
				}
				else
				{
					break;
				}
			}
			TString tname = "Mod";//(*it).second->Class_Name();
			tname += (*it).first;
			tname += '_';
			tname += cname;
			tree->Branch(tname.Data(), (*it).second->Class_Name(), (*it).second);
		}

		bool skiph = DFReader.SkipFileHeader();
		cout << "Skip Data File header(true/false): " << boolalpha << skiph << noboolalpha<< endl;
		unsigned int numevtana = 0;
		unsigned int vmecount = 0, vmecount_sent = 0;
		int eventcount_sent = 0;
		int increment = 0, tinc = 0;
		int totalevent = 0;
		unsigned int evtc = 0;
		if(startn>0)
		{
			unsigned int sstartn = startn;
			do
			{
				if( !DFReader.EvtReadingLoop() )
				{
					cout << "File do not have enough events: " << startn <<endl;
				}
				sstartn--;
			}while(sstartn>0);
			cout << "The first: " << startn << " events skiped." << endl;
		}
		//ofstream temp("temp.txt");
		while(true)
		{
			//temp << endl;
			if(numevtana>=evtana && evtana!=0) break;
			if( !DFReader.EvtReadingLoop() ) break;   //end of file
			unsigned int len = DFReader.GetEventLength();
			unsigned int *evtbuffer = DFReader.GetEvtBuf();
			//for(unsigned int kks =0; kks<len; kks++)
			//{
			//	unsigned int da = *(evtbuffer+kks);
			//	temp << da << " " << (da>>27) << " " << ((da<<5)>>29) << " " << ((da<<11)>>27)<< endl;
			//}
			evtc = anadata.GlobalDecoder(evtbuffer, len);
			//calculate vme read count
			if(evtc>0 && evtc < eventcount_sent)
			{
				vmecount += eventcount_sent; //in case 'acqstop' during this file
			}
			//calculate Net loss event number
			if(evtc>0 )
			{
				totalevent++;
				if(evtc < eventcount_sent) eventcount_sent = 0;
				if(evtc>eventcount_sent)
				{
					tinc = evtc - eventcount_sent;
					increment += (tinc-1);
				}
				eventcount_sent = evtc;
			}
			//UserTH_Fill();
			tree->Fill();
			numevtana++;
		}
		rfile->Write();
		rfile->Close();
		delete rfile;
		tree = 0;
		vmecount += evtc;
		cout <<"VMERead Event Num= " << vmecount << "; File Event Num= " <<totalevent <<"; NetLoss Event Num = " << increment-startn <<endl;
		cout << rdfname << " Finished!" << endl;
		DFReader.CloseDataFile();
	}
	return 0;
}

void PrintUsage(char *name)
{
	cout<<"Usage: "<<name<<"  "<<endl;               // name = “./Raw2ROOT”
	cout<<"\t Interactive mode." << endl;            // \t 是水平制表符，用于水平对齐
	cout<<"Usage: "<<name<<" /listfilename "<<endl;  // 将文件名都写入名为"listfilename"文本文件中(也可以取其他名字)
	cout<<"\t 'listfilename' is a text file contains the 'raw data file names' in the directory " ;
	cout<<endl;
}
