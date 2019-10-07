#include<iostream>
#include<fstream>
#include<iomanip>

using namespace std;

int main()
{
   ofstream output("../../Fission2019_Data/RawRoot/alistfilename1");


      ////////////////////////////////////////////////////////////////////////////////
      ////     用于数据转换(二) 将原始数据(二进制文件)转换成.root文件

         // Fission Data
         for(int i=89; i<121; i++)
         {
      	  output<< "Fission2019_Kr_Pb" <<setw(4)<<setfill('0')<< i <<".root"<<endl;
         }



/*
   ////////////////////////////////////////////////////////////////////////////////
   ////     用于数据转换(二) 将原始数据(二进制文件)转换成.root文件

      // Fission Data
      for(int i=121; i<433; i++)
      {
   	  output<< "Fission2019_Kr_Pb" <<setw(4)<<setfill('0')<< i <<".root"<<endl;
      }

      for(int i=0; i<3; i++)
      {
        output<< "PPAC1_BeamCalibration"<<setw(4)<<setfill('0')<<i<< ".root" <<endl;
      }


      // PPAC Beam Calibration Data
      for(int i=0; i<2; i++)
      {
        output<< "PPAC2_BeamCalibration"<<setw(4)<<setfill('0')<<i<< ".root"<<endl;
      }

        output<< "PPAC3_BeamCalibration0000.root"<<endl;


       // PPAC Cf252 Calibration Data
        output<< "PPAC2_Cf252Calibration0000.root"<<endl;
      for(int i=0; i<13; i++)
      {
        output<< "PPAC3_Cf252Calibration"<<setw(4)<<setfill('0')<<i<< ".root"<<endl;
      }

      // SSD Pulser Calibration Data
      for(int i=0; i<4; i++)
      {
        output<<"SSD"<<i+1<<"_L1_PulserCali_Height0000.root"<<endl;
        output<<"SSD"<<i+1<<"_L1_PulserCali_Switch0000.root"<<endl;
        output<<"SSD"<<i+1<<"_L2_PulserCali_Height0000.root"<<endl;
        output<<"SSD"<<i+1<<"_L2_PulserCali_Switch0000.root"<<endl;
      }

        output<<"SSD1_L3_PulserCali_Height0000.root"<<endl;
        output<<"SSD1_L3_PulserCali_Switch0000.root"<<endl;

        output<<"SSD1_L1_PulserReCali_Gain4_Height0000.root"<<endl;
        output<<"SSD1_L1_PulserReCali_Gain4_Switch0000.root"<<endl;
        output<<"SSD1_L1_PulserReCali_Gain20_Height0000.root"<<endl;
        output<<"SSD1_L1_PulserReCali_Gain20_Switch0000.root"<<endl;

        output<<"SSD2_L1_PulserReCali_Gain74_Height0000.root"<<endl;
        output<<"SSD2_L1_PulserReCali_Gain74_Switch0000.root"<<endl;
        output<<"SSD2_L1_PulserReCali_Gain20_Height0000.root"<<endl;
        output<<"SSD2_L1_PulserReCali_Gain20_Switch0000.root"<<endl;


      // Alpha Calibration Data
      for(int i=0; i<9; i++)
      {
        output<< "SSD_L1_AlphaCali"<<setw(4)<<setfill('0')<<i<< ".root"<<endl;
      }

      for(int i=0; i<49; i++)
      {
        output<< "SSD_L2_AlphaCali"<<setw(4)<<setfill('0')<<i<< ".root"<<endl;
      }

        output<<"SSD_PulserCali_Pedestal0000.root"<<endl;

   ///////////////////////////////////////////////////////////////////////////////
*/

/*
////////////////////////////////////////////////////////////////////////////////
////     用于数据转换(一) 将原始数据(二进制文件)转换成.root文件

   // Fission Data
   for(int i=0; i<433; i++)
   {
	  output<< "Fission2019_Kr_Pb." <<setw(4)<<setfill('0')<< i <<endl;
   }

   for(int i=0; i<3; i++)
   {
     output<< "PPAC1_BeamCalibration."<<setw(4)<<setfill('0')<<i<<endl;
   }


   // PPAC Beam Calibration Data
   for(int i=0; i<2; i++)
   {
     output<< "PPAC2_BeamCalibration."<<setw(4)<<setfill('0')<<i<<endl;
   }

     output<< "PPAC3_BeamCalibration.0000"<<endl;


    // PPAC Cf252 Calibration Data
     output<< "PPAC2_Cf252Calibration.0000"<<endl;
   for(int i=0; i<13; i++)
   {
     output<< "PPAC3_Cf252Calibration."<<setw(4)<<setfill('0')<<i<<endl;
   }

   // SSD Pulser Calibration Data
   for(int i=0; i<4; i++)
   {
     output<<"SSD"<<i+1<<"_L1_PulserCali_Height.0000"<<endl;
     output<<"SSD"<<i+1<<"_L1_PulserCali_Switch.0000"<<endl;
     output<<"SSD"<<i+1<<"_L2_PulserCali_Height.0000"<<endl;
     output<<"SSD"<<i+1<<"_L2_PulserCali_Switch.0000"<<endl;
   }

     output<<"SSD1_L3_PulserCali_Height.0000"<<endl;
     output<<"SSD1_L3_PulserCali_Switch.0000"<<endl;

     output<<"SSD1_L1_PulserReCali_Gain4_Height.0000"<<endl;
     output<<"SSD1_L1_PulserReCali_Gain4_Switch.0000"<<endl;
     output<<"SSD1_L1_PulserReCali_Gain20_Height.0000"<<endl;
     output<<"SSD1_L1_PulserReCali_Gain20_Switch.0000"<<endl;

     output<<"SSD2_L1_PulserReCali_Gain7.4_Height.0000"<<endl;
     output<<"SSD2_L1_PulserReCali_Gain7.4_Switch.0000"<<endl;
     output<<"SSD2_L1_PulserReCali_Gain20_Height.0000"<<endl;
     output<<"SSD2_L1_PulserReCali_Gain20_Switch.0000"<<endl;


   // Alpha Calibration Data
   for(int i=0; i<9; i++)
   {
     output<< "SSD_L1_AlphaCali."<<setw(4)<<setfill('0')<<i<<endl;
   }

   for(int i=0; i<49; i++)
   {
     output<< "SSD_L2_AlphaCali."<<setw(4)<<setfill('0')<<i<<endl;
   }

     output<<"SSD_PulserCali_Pedestal.0000"<<endl;

///////////////////////////////////////////////////////////////////////////////

*/



   output.close();

   return 0;
}
