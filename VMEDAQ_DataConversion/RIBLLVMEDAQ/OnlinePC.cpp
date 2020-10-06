//////////////////////////////////////////
// ControlPC.cpp: main() of Control PC,
// send commands to DAQ_PC and receive
// the reply. Acquire data form UDP
// broadcast and save it to file. 
// Han Jianlong (08/2012)
//////////////////////////////////////////

#include "TApplication.h"
#include "TRint.h"

#include "TDAQApplication.h"
#include "TConfig.h"
#include "TDataAnalyser.h"
#include "TControlFrame.h"
#include "TGClient.h"
#include "TMasterTask.h"
#include "TControl.h"


int main(int argc, char *argv[])
{
	TApplication apponline("Online", &argc, argv);

	TControl masterpc("192.168.1.102", ONLINE_PC);//192.0.0.17

	TMasterTask mastask(masterpc);

	apponline.Run();
	return 0;
}
