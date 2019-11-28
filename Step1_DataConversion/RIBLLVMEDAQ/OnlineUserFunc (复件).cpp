////////////////////////////////////////////////
// Some Userdefined functions used for
// Online program.
// Han Jianlong (08/2012)
////////////////////////////////////////////////

////////////////////////////////////////////////
// Add some online plot for the HBT fission experiment
// Online program.
// yj-wang (01/2018)
// 1,PPAC X-Y plot
// 2,delteE-E PID plot
// 3,Channel Counts plot (There is a Swift_For_Channel_Counts plot (ON/OFF=1/0) )
////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// PS: The individual channel is already exist for all modules (automatically)
//     The correlation maps are added by hand
// gfh (May, 2019)
// 1. PPAC X-Y plots,
// 2. Au_Si dE-E plots
// 3. SSD HitPattern
// 4. SSD HitMultiplicity
// 5. SSD dE1-dEF plots
// 6. SSD dEF-ECsI plots
//////////////////////////////////////////////////////////////////////

#include <iostream>
using namespace std;

#include "TROOT.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TF1.h"
#include "TList.h"
#include "TOnlineFrame.h"
#include "TOnline.h"

#include <stdlib.h>


////////////////////////////////////////////////////////////////////////////////
/////  definition of some functions to be used
////////////////////////////////////////////////////////////////////////////////
void def_TH1F(int hid, const char *hname, int bins, float xmin, float xmax);
void hfill1(int hid, float value, float weight);
void def_TH2F(int hid, const char *hname, int xbins, float xmin, float xmax, int ybins, float ymin, float ymax);
void hfill2(int hid, float x, float y, float weight);
extern int  GetRawData(int Crate, int Geo, int channel);
extern void HBOOK1(int id, const char *title, int nxbin, float xlow, float xup, float vmx);
extern void HBOOK2(int id, const char *title, int nxbin, float xlow, float xup, int nybin, float ylow, float yup, float vmx);
extern void HF1(int id, float x, float weight);
extern void HF2(int id, float x, float y, float weight);
extern bool HEXIST(int ihist);

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


///////////////////////////////////////////////////////////////////////////////
//// definition of the histograms to be filled
///////////////////////////////////////////////////////////////////////////////
void UserTH_Init()
{
	   int Bin_Num = 300;
     int Low_CH  = 0;
	   int High_CH = 4096;

    ////////////////////////////////////////////////////////////////////////////
    ///                              For PPACs
    ////////////////////////////////////////////////////////////////////////////
    //For safety, use only 'def_TH1F()' and 'def_TH2F()' in this function
	  //PPAC1
	  def_TH2F(1000, "PPAC1_X_Y", 300, -1500.0, 1500.0, 500, -1500.0, 1500.0);
  	def_TH2F(1001, "PPAC1_X1_X2", Bin_Num, Low_CH, High_CH, Bin_Num, Low_CH, High_CH);
	  def_TH2F(1002, "PPAC1_Y1_Y2", Bin_Num, Low_CH, High_CH, Bin_Num, Low_CH, High_CH);
  	def_TH1F(1003, "PPAC1_X1_plus_X2_minus_2T", Bin_Num, Low_CH, High_CH);
	  def_TH1F(1004, "PPAC1_Y1_plus_Y2_minus_2T", Bin_Num, Low_CH, High_CH);

	  //For safety, use only 'def_TH1F()' and 'def_TH2F()' in this function
  	//PPAC2
	  def_TH2F(1005, "PPAC2_X_Y", 300, -1500.0, 1500.0, 500, -1500.0, 1500.0);
	  def_TH2F(1006, "PPAC2_X1_X2", Bin_Num, Low_CH, High_CH, Bin_Num, Low_CH, High_CH);
	  def_TH2F(1007, "PPAC2_Y1_Y2", Bin_Num, Low_CH, High_CH, Bin_Num, Low_CH, High_CH);
	  def_TH1F(1008, "PPAC2_X1_plus_X2_minus_2T", Bin_Num, Low_CH, High_CH);
	  def_TH1F(1009, "PPAC2_Y1_plus_Y2_minus_2T", Bin_Num, Low_CH, High_CH);

  	//For safety, use only 'def_TH1F()' and 'def_TH2F()' in this function
  	//PPAC3
	  def_TH2F(1010, "PPAC3_X_Y", 300, -1500.0, 1500.0, 500, -1500.0, 1500.0);
	  def_TH2F(1011, "PPAC3_X1_X2", Bin_Num, Low_CH, High_CH, Bin_Num, Low_CH, High_CH);
	  def_TH2F(1012, "PPAC3_Y1_Y2", Bin_Num, Low_CH, High_CH, Bin_Num, Low_CH, High_CH);
	  def_TH1F(1013, "PPAC3_X1_plus_X2_minus_2T", Bin_Num, Low_CH, High_CH);
  	def_TH1F(1014, "PPAC3_Y1_plus_Y2_minus_2T", Bin_Num, Low_CH, High_CH);

	  //PPAC_T_Energy
	  def_TH1F(1015, "PPAC1_T_Energy", Bin_Num, Low_CH, High_CH);
	  def_TH1F(1016, "PPAC2_T_Energy", Bin_Num, Low_CH, High_CH);
	  def_TH1F(1017, "PPAC3_T_Energy", Bin_Num, Low_CH, High_CH);


    ////////////////////////////////////////////////////////////////////////////
    ////                          For Au_Si Detectors
    ////////////////////////////////////////////////////////////////////////////
    def_TH2F(2000, "Au_Si1_dE-E", Bin_Num, Low_CH, High_CH, Bin_Num, Low_CH, High_CH);
		def_TH2F(2001, "Au_Si2_dE-E", Bin_Num, Low_CH, High_CH, Bin_Num, Low_CH, High_CH);


    /////////////////////////////////////////////////////////////////////////////
	  //////               For SSD HitPattern
	  /////////////////////////////////////////////////////////////////////////////
	  int  ChIndex_L1_HitPattern = 0;
	  int  ChIndex_L2_HitPattern = 0;
	  int  ChIndex_L3_HitPattern = 0;
	  char ChName_L1_HitPattern[100] = {0};
	  char ChName_L2_HitPattern[100] = {0};
	  char ChName_L3_HitPattern[100] = {0};
	  for(int i=0; i<4; i++)
	  {
		   ChIndex_L1_HitPattern = 3000 + 100 * i + 0;
		   ChIndex_L2_HitPattern = 3000 + 100 * i + 1;
			 ChIndex_L3_HitPattern = 3000 + 100 * i + 2;

       sprintf(ChName_L1_HitPattern, "SSD%d_L1_HitPattern", i+1);
       sprintf(ChName_L2_HitPattern, "SSD%d_L2F_L2B_HitPattern", i+1);
       sprintf(ChName_L3_HitPattern, "SSD%d_L3_HitPattern", i+1);

			 def_TH1F(ChIndex_L1_HitPattern, ChName_L1_HitPattern, 16,-0.5, 15.5);
		   def_TH2F(ChIndex_L2_HitPattern, ChName_L2_HitPattern, 16,-0.5, 15.5, 16,-0.5, 15.5);
			 def_TH2F(ChIndex_L3_HitPattern, ChName_L3_HitPattern, 3,-0.5, 2.5, 3,-0.5, 2.5);
	  }

	  /////////////////////////////////////////////////////////////////////////////
	  //////               For SSD HitMultiplicity
	  /////////////////////////////////////////////////////////////////////////////
	  int  ChIndex_L1_HitMultiplicity  = 0;
	  int  ChIndex_L2F_HitMultiplicity = 0;
	  int  ChIndex_L2B_HitMultiplicity = 0;
	  int  ChIndex_L3_HitMultiplicity  = 0;
		int  ChIndex_L2_HitMultiplicity_Correlation  = 0;
	  char ChName_L1_HitMultiplicity[100]  = {0};
	  char ChName_L2F_HitMultiplicity[100] = {0};
	  char ChName_L2B_HitMultiplicity[100] = {0};
    char ChName_L3_HitMultiplicity[100]  = {0};
		char ChName_L2_HitMultiplicity_Correlation[100] = {0};
	  for(int i=0; i<4; i++)
	  {
		 	 ChIndex_L1_HitMultiplicity  = 4000 + 100 * i + 0;
			 ChIndex_L2F_HitMultiplicity = 4000 + 100 * i + 1;
			 ChIndex_L2B_HitMultiplicity = 4000 + 100 * i + 2;
			 ChIndex_L3_HitMultiplicity  = 4000 + 100 * i + 3;
			 ChIndex_L2_HitMultiplicity_Correlation  = 4000 + 100 * i + 4;

			 sprintf(ChName_L1_HitMultiplicity, "SSD%d_L1_HitMultiplicity", i+1);
			 sprintf(ChName_L2F_HitMultiplicity, "SSD%d_L2F_HitMultiplicity", i+1);
			 sprintf(ChName_L2B_HitMultiplicity, "SSD%d_L2B_HitMultiplicity", i+1);
			 sprintf(ChName_L3_HitMultiplicity, "SSD%d_L3_HitMultiplicity", i+1);
			 sprintf(ChName_L2_HitMultiplicity_Correlation, "SSD%d_L2_HitMulti_Correlation", i+1);

			 def_TH1F(ChIndex_L1_HitMultiplicity, ChName_L1_HitMultiplicity, 16,-0.5,15.5);
			 def_TH1F(ChIndex_L2F_HitMultiplicity, ChName_L2F_HitMultiplicity, 16,-0.5,15.5);
			 def_TH1F(ChIndex_L2B_HitMultiplicity, ChName_L2B_HitMultiplicity, 16,-0.5,15.5);
			 def_TH1F(ChIndex_L3_HitMultiplicity, ChName_L3_HitMultiplicity, 9,-0.5, 8.5);
			 def_TH2F(ChIndex_L2_HitMultiplicity_Correlation, ChName_L2_HitMultiplicity_Correlation, 16,-0.5,15.5,16,-0.5,15.5);
	  }


		/////////////////////////////////////////////////////////////////////////////
	  //////               For SSD dE1-dEF
	  ////////////////////////////////////////////////////////////////////////////
		int ChIndex_dE1_EF = 0;
		char ChName_dE1_EF[100] = {0};

    for(int i=0; i<4; i++)
		{
       ChIndex_dE1_EF = 5000 + i;
			 sprintf(ChName_dE1_EF, "SSD%d_dE1-EF_PID", i+1);

			 def_TH2F(ChIndex_dE1_EF, ChName_dE1_EF, Bin_Num, Low_CH, High_CH, Bin_Num, Low_CH, High_CH);
		}


		/////////////////////////////////////////////////////////////////////////////
		//////               For SSD dEF-ECsI
		/////////////////////////////////////////////////////////////////////////////
		int ChIndex_dEF_ECsI = 0;
		char ChName_dEF_ECsI[100] = {0};

    for(int i=0; i<4; i++)
		{
       ChIndex_dEF_ECsI = 6000 + i;
			 sprintf(ChName_dEF_ECsI, "SSD%d_dEF-ECsI_PID", i+1);

			 def_TH2F(ChIndex_dEF_ECsI, ChName_dEF_ECsI, Bin_Num, Low_CH, High_CH, Bin_Num, Low_CH, High_CH);
		}

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



////////////////////////////////////////////////////////////////////////////////
////  Fill the histograms
///   0. ch definition
///   1. get rawdata
///   2. fill the histograms
////////////////////////////////////////////////////////////////////////////////
void UserTH_Fill()
{

		const int Au_Si_L1_E_LowCut  = 100;
		const int Au_Si_L1_E_HighCut = 3800;
		const int Au_Si_L2_E_LowCut  = 300;
		const int Au_Si_L2_E_HighCut = 3800;

		const int SSD_L1_LowCut  = 100;
		const int SSD_L1_HighCut = 3800;
		const int SSD_L2_LowCut  = 100;
		const int SSD_L2_HighCut = 3800;
		const int SSD_L3_LowCut  = 300;
		const int SSD_L3_HighCut = 3800;


		////////////////////////////////////////////////////////////////////////////
		////                               For PPACs
		//// 0. definition of variables to be read
    int PPAC_T[3]  = {0};
		int PPAC_X1[3] = {0};
		int PPAC_X2[3] = {0};
		int PPAC_Y1[3] = {0};
		int PPAC_Y2[3] = {0};
		int PPAC_T_Energy[3] = {0};

		int X1_minus_X2[3] = {0};
		int Y1_minus_Y2[3] = {0};
		int X1_plus_X2_minus_2T[3] = {0};
		int Y1_plus_Y2_minus_2T[3] = {0};

		////////////////////////////////////////////////////////////////////////////
		//// 1. get rawdata from modules
		////  103000 - 103004     PPAC1_T, PPAC1_X1, PPAC1_X2, PPAC1_Y1, PPAC1_Y2
		////  103005 - 103009     PPAC2_T, PPAC2_X1, PPAC2_X2, PPAC2_Y1, PPAC2_Y2
		////  103010 - 103014     PPAC3_T, PPAC3_X1, PPAC3_X2, PPAC3_Y1, PPAC3_Y2
		///
		///   104000 - 104002     PPAC1_T_Energy, PPAC2_T_Energy, PPAC3_T_Energy
		////////////////////////////////////////////////////////////////////////////
		for(int i=0; i<3; i++)
		{
			 ///  extern int  GetRawData(int Crate, int Geo, int channel);
			 PPAC_T[i]  = GetRawData(1, 3, 5*i);
			 PPAC_X1[i] = GetRawData(1, 3, 5*i+1);
			 PPAC_X2[i] = GetRawData(1, 3, 5*i+2);
			 PPAC_Y1[i] = GetRawData(1, 3, 5*i+3);
			 PPAC_Y2[i] = GetRawData(1, 3, 5*i+4);
			 PPAC_T_Energy[i] = GetRawData(1, 4, i);

			 X1_minus_X2[i] = PPAC_X1[i] - PPAC_X2[i];
			 Y1_minus_Y2[i] = PPAC_Y1[i] - PPAC_Y2[i];
			 X1_plus_X2_minus_2T[i] = PPAC_X1[i] + PPAC_X2[i] - 2*PPAC_T[i];
			 Y1_plus_Y2_minus_2T[i] = PPAC_Y1[i] + PPAC_Y2[i] - 2*PPAC_T[i];
		}

    /////////////////////////////////////////////////////////////////////////////
	  ///   2. fill histograms
		///  void hfill1(int hid, float value, float weight);
		///  void hfill2(int hid, float x, float y, float weight);
		///  1000-1004  PPAC1_X_Y,PPAC1_X1_X2,PPAC1_Y1_Y2,PPAC1_X1_plus_X2_minus_2T,PPAC1_Y1_plus_Y2_minus_2T
		///  1005-1009  PPAC2_X_Y,PPAC2_X1_X2,PPAC2_Y1_Y2,PPAC2_X1_plus_X2_minus_2T,PPAC2_Y1_plus_Y2_minus_2T
		///  1010-1014  PPAC3_X_Y,PPAC3_X1_X2,PPAC3_Y1_Y2,PPAC3_X1_plus_X2_minus_2T,PPAC3_Y1_plus_Y2_minus_2T
		///  1015-1017  PPAC1_T_Energy, PPAC2_T_Energy, PPAC3_T_Energy
	  for(int i=0; i<3; i++)
		{
			 hfill2(1000+5*i+0, X1_minus_X2[i], Y1_minus_Y2[i], 1);
			 hfill2(1000+5*i+1, PPAC_X1[i]-PPAC_T[i], PPAC_X2[i]-PPAC_T[i], 1);
			 hfill2(1000+5*i+2, PPAC_Y1[i]-PPAC_T[i], PPAC_Y2[i]-PPAC_T[i], 1);
			 hfill1(1000+5*i+3, X1_plus_X2_minus_2T[i], 1);
			 hfill1(1000+5*i+4, Y1_plus_Y2_minus_2T[i], 1);

			 hfill1(1015+i, PPAC_T_Energy[i], 1);
		}


    ////////////////////////////////////////////////////////////////////////////
		/////                  For Au-Si Detectors
		////////////////////////////////////////////////////////////////////////////
    int Au_Si1_L1_E = 0;
		int Au_Si1_L2_E = 0;
		int Au_Si2_L1_E = 0;
		int Au_Si2_L2_E = 0;

    ////////////////////////////////////////////////////////////////////////////
		////  104016	Au_Si1_L1_Energy
    ////  104017	Au_Si1_L2_Energy
    ////  104018	Au_Si2_L1_Energy
    ////  104019	Au_Si2_L2_Energy
		Au_Si1_L1_E = GetRawData(1, 4, 16);
		Au_Si1_L2_E = GetRawData(1, 4, 17);
		Au_Si2_L1_E = GetRawData(1, 4, 18);
		Au_Si2_L2_E = GetRawData(1, 4, 19);

    ////////////////////////////////////////////////////////////////////////////
		////  2000    Au_Si1_dE-E
		////  2001    Au_Si2_dE-E
		if(Au_Si1_L1_E>Au_Si_L1_E_LowCut && Au_Si1_L1_E<Au_Si_L1_E_HighCut)
		{
			if(Au_Si1_L2_E>Au_Si_L2_E_LowCut && Au_Si1_L2_E<Au_Si_L2_E_HighCut)
			{
				hfill2(2000, Au_Si1_L2_E, Au_Si1_L1_E, 1);
			}
		}

		if(Au_Si2_L1_E>Au_Si_L1_E_LowCut && Au_Si2_L1_E<Au_Si_L1_E_HighCut)
		{
			if(Au_Si2_L2_E>Au_Si_L2_E_LowCut && Au_Si2_L2_E<Au_Si_L2_E_HighCut)
			{
				hfill2(2001, Au_Si2_L2_E, Au_Si2_L1_E, 1);
			}
		}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
///                   For SSD
///  1. HitPattern
///  2. HitMultiplicity
///  3. dE1-EF_PID
///  4. dEF-ECsI_PID
/// 111000-111015	SSD1_Layer1_S_CH00-CH15_E
/// 111016-111024	SSD1_Layer3_A_CH00-CH08_E
/// 112000-112015	SSD1_Layer2_F_CH00-CH15_E
/// 112016-112031	SSD1_Layer2_B_CH00-CH15_E

/// 114000-114015	SSD2_Layer1_S_CH00-CH15_E
/// 114016-114024	SSD2_Layer3_A_CH00-CH08_E
/// 115000-115015	SSD2_Layer2_F_CH00-CH15_E
/// 115016-115031	SSD2_Layer2_B_CH00-CH15_E

/// 117000-117015	SSD3_Layer1_S_CH00-CH15_E
/// 117016-117024	SSD3_Layer3_A_CH00-CH08_E
/// 118000-118015	SSD3_Layer2_F_CH00-CH15_E
/// 118016-118031	SSD3_Layer2_B_CH00-CH15_E

/// 120000-120015	SSD4_Layer1_S_CH00-CH15_E
/// 120016-120024	SSD4_Layer3_A_CH00-CH08_E
/// 121000-121015	SSD4_Layer2_F_CH00-CH15_E
/// 121016-121031	SSD4_Layer2_B_CH00-CH15_E
////////////////////////////////////////////////////////////////////////////////
///  0. Get Rawdata
    int SSD_L1_S_E[4][16] = {{0}};
		int SSD_L2_F_E[4][16] = {{0}};
		int SSD_L2_B_E[4][16] = {{0}};

		int SSD_L3_A_E[4][9] = {{0}};

		for(int i=0; i<4; i++)
		{
       for(int j=0; j<16; j++)
			 {
				  SSD_L1_S_E[i][j] = GetRawData(1, 11+3*i, j);
					SSD_L2_F_E[i][j] = GetRawData(1, 12+3*i, j);
					SSD_L2_B_E[i][j] = GetRawData(1, 12+3*i, 16+j);
			 }

			 for(int k=0; k<9; k++)
			 {
				  SSD_L3_A_E[i][k] = GetRawData(1, 11+3*i, 16+i);
			 }
		}

////////////////////////////////////////////////////////////////////////////////
/// 1. Fill HitPattern
/// 3000-3002 SSD1_Layer1_HitPattern, SSD1_Layer2F_Layer2B_HitPattern, SSD1_Layer3_ HitPattern
/// 3100-3102 SSD2_Layer1_HitPattern, SSD2_Layer2F_Layer2B_HitPattern, SSD2_Layer3_ HitPattern
/// 3200-3202 SSD3_Layer1_HitPattern, SSD3_Layer2F_Layer2B_HitPattern, SSD3_Layer3_ HitPattern
/// 3300-3302 SSD4_Layer1_HitPattern, SSD4_Layer2F_Layer2B_HitPattern, SSD4_Layer3_ HitPattern
    int SSD_L1_HitMulti[4]  = {0};
		int SSD_L2F_HitMulti[4] = {0};
		int SSD_L2B_HitMulti[4] = {0};
		int SSD_L3_HitMulti[4]  = {0};

    for(int i=0; i<4; i++)
		{
			 for(int j=0; j<16; j++)
			 {
				  if(SSD_L1_S_E[i][j]>SSD_L1_LowCut && SSD_L1_S_E[i][j]<SSD_L1_HighCut)
					{
						SSD_L1_HitMulti[i]++;
						hfill1(3000+100*i, j, 1);
					}

					if(SSD_L2_F_E[i][j]>SSD_L2_LowCut && SSD_L2_F_E[i][j]<SSD_L2_HighCut)
					{
						SSD_L2F_HitMulti[i]++;
            for(int k=0; k<16; k++)
						{
							if(SSD_L2_B_E[i][k]>SSD_L2_LowCut && SSD_L2_B_E[i][k]<SSD_L2_HighCut)
						  {
							  hfill2(3000+100*i+1, j, k, 1);
						  }
						}
					}

					if(SSD_L2_B_E[i][j]>SSD_L2_LowCut && SSD_L2_B_E[i][j]<SSD_L2_HighCut)
					{
						 SSD_L2B_HitMulti[i]++;
					}
			 }

			 for(int h=0; h<9; h++)
			 {
				  if(SSD_L3_A_E[i][h]>SSD_L3_LowCut && SSD_L3_A_E[i][h]<SSD_L3_HighCut)
					{
						SSD_L3_HitMulti[i]++;
						hfill2(3000+100*i+2, h/3, h%3, 1);
						/* 沿着束流看，SSD安装在束流左侧
						 CsI2   CsI5   CsI8
						 CsI1   CsI4   CsI7
						 CsI0   CsI3   CsI6  	*/
					}
			 }
		}

////////////////////////////////////////////////////////////////////////////////
///  2. Fill HitMultiplicity
     for(int i=0; i<4; i++)
		 {
			  hfill1(4000+100*i, SSD_L1_HitMulti[i], 1);
				hfill1(4000+100*i+1, SSD_L2F_HitMulti[i], 1);
				hfill1(4000+100*i+2, SSD_L2B_HitMulti[i], 1);
				hfill1(4000+100*i+3, SSD_L3_HitMulti[i], 1);
				hfill2(4000+100*i+4, SSD_L2B_HitMulti[i], SSD_L2F_HitMulti[i], 1);
		 }


////////////////////////////////////////////////////////////////////////////////
///  3. Fill dE1-EF_PID
///  PS: (1) Only one-hit events included
///      (2) Only paiticles stop in Layer2 included
///////////////////////////////////////////////////////////////////////////////
    for(int i=0; i<4; i++)
		{
			 if(SSD_L1_HitMulti[i]!=1 || SSD_L2F_HitMulti[i]!=1) continue;
			 if(SSD_L3_HitMulti[i]!=0) continue;

       for(int j=0; j<16; j++)
			 {
				  if(SSD_L1_S_E[i][j]>SSD_L1_LowCut && SSD_L1_S_E[i][j]<SSD_L1_HighCut)
					{
						for(int k=0; k<16; k++)
						{
							if(SSD_L2_F_E[i][k]>SSD_L2_LowCut && SSD_L2_F_E[i][k]<SSD_L2_HighCut)
						  {
								hfill2(5000+i, SSD_L2_F_E[i][k], SSD_L1_S_E[i][j], 1);
							}
						}
					}
			 }
		}


////////////////////////////////////////////////////////////////////////////////
///   4. Fill dEF-ECsI_PID
///   PS: (1) Only one-hit events included
////////////////////////////////////////////////////////////////////////////////
      for(int i=0; i<4; i++)
			{
				 if(SSD_L2F_HitMulti[i]!=1 || SSD_L3_HitMulti[i]!=1) continue;

				 for(int j=0; j<8; j++)
				 {
					  if(SSD_L3_A_E[i][j]<SSD_L3_LowCut || SSD_L3_A_E[i][j]>SSD_L3_HighCut) continue;

            if(j%3==2)
						{
							for(int k=0; k<5; k++)
							{
								if(SSD_L2_F_E[i][k]>SSD_L2_LowCut && SSD_L2_F_E[i][k]<SSD_L2_HighCut)
							  {
									hfill2(6000+i, SSD_L3_A_E[i][j], SSD_L2_F_E[i][k], 1);
								}
							}
						}

						if(j%3==1)
						{
							for(int k=5; k<11; k++)
							{
								if(SSD_L2_F_E[i][k]>SSD_L2_LowCut && SSD_L2_F_E[i][k]<SSD_L2_HighCut)
							  {
									hfill2(6000+i, SSD_L3_A_E[i][j], SSD_L2_F_E[i][k], 1);
								}
							}
						}

						if(j%3==0)
						{
							for(int k=11; k<16; k++)
							{
								if(SSD_L2_F_E[i][k]>SSD_L2_LowCut && SSD_L2_F_E[i][k]<SSD_L2_HighCut)
							  {
									hfill2(6000+i, SSD_L3_A_E[i][j], SSD_L2_F_E[i][k], 1);
								}
							}
						}
						/* 沿着束流看，SSD安装在束流左侧
						 CsI2   CsI5   CsI8              strip0-4
						 CsI1   CsI4   CsI7              strip5-10
						 CsI0   CsI3   CsI6  	           strip11-15   */
				 }
			}
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



void OnlineUserFunction(TOnlineFrame *onl)
{
	//TF1 *func1 = new TF1("func1","sqrt(-x*x-10*x)-5" , -10, 0);
	//TF1 *func2 = new TF1("func1","-sqrt(-x*x-10*x)-5", -10, 0);

	//if(onl->datafile->Get("h272") )
	//{
	//	TList *thfunclist = ( (TH1*)(onl->datafile->Get("h272") ) )->GetListOfFunctions();
	//	if(thfunclist)
	//	{
	//		thfunclist->Add(func1);
	//		thfunclist->Add(func2);
	//	}
	//}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//capsulize of 'TH1F()', for safety
void def_TH1F(int hid, const char *hname, int bins, float xmin, float xmax)
{
	if( HEXIST(hid) )
	{
		cout << "Histogram ID: " << hid << " (mod_id*100+ch_num) already exist." << endl;
		return;
	}
	else
	{
		char name[30];
		strncpy(name, hname, sizeof(name));
		HBOOK1(hid, name, bins, xmin, xmax, 0.);
	}
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//capsulize of 'TH2F()', for safety
void def_TH2F(int hid, const char *hname, int xbins, float xmin, float xmax, int ybins, float ymin, float ymax)
{
	if( HEXIST(hid) )
	{
		cout << "Histogram ID: " << hid << " (mod_id*100+ch_num) already exist. " << endl;
		return;
	}
	else
	{
		char name[30];
		strncpy(name, hname, sizeof(name));
		HBOOK2(hid, name, xbins, xmin, xmax, ybins, ymin, ymax, 0.);
	}
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//capsulize of 'TH1F::Fill(....)', for safety
void hfill1(int hid, float value, float weight=1.)
{
	if(HEXIST(hid)) HF1(hid, value, weight);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//capsulize of 'TH2F::Fill(....)', for safety
void hfill2(int hid, float x, float y, float weight = 1.)
{
	if(HEXIST(hid)) HF2(hid, x, y, weight);
}
