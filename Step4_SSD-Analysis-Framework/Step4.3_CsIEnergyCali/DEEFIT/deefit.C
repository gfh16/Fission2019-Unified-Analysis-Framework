//////////////////////////////////////////////////////////////////
//  DEEFIT program
//  e.de filippo  (2009) 
//  identification file DE-E unified version
//  modif. versione for ROOT
//  ver 0.2  1/2009  starting point
//      0.3  minimal commands, interface definition
//      0.4  09/02/09  first usable version (no fit) 
//           AClic ok.  g++ standalone needs work
//           Added spline (fit) command
//      0.5  full cut database. first part fully functional
//      0.6  fit procedure (just starting point, but running)
//      0.7  consolidation version. Saving fit results. 
//           Fully functional fit with graphical results
//      0.8  Data Analysis for Z,M identification
//      0.9  Graphical interface improvement. Epax2 formula
//      0.91 bug corrections 
//      0.95 Better C++ style. Bug corrections. Microsoft Windows 
//           compatibility with Visual C++ .net ver. 2008 (vc9)
//      0.99 Standard (Tassan-Got) and enhanced (14 parameters) fit. 
//      1.0beta   writing mass formula status 
//      1.1  Complete revision, standalone version, new Analysis commands
/////////////////////////////////////////////////////////////////////////
#include "deefit.h"
#include "MDEEFrame.h"
#include "MEpax.h"

using namespace std;

// The main function (ACLIC only)
void deefit()
{
 MEpax epax(50,112,50,112);    //start with a default system 
 new MDEEFrame(gClient->GetRoot(),400,185, epax);
}


