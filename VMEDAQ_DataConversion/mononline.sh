#!/bin/bash
# run the vme daq program DAQPC 
# Han Jianlong 08/2012
export DAQServerIP=localhost
export DAQCONFPATH=./acqsetup
export DAQDataPath=./vmedata
if [ $# != 0 ]
then 
 parg=$@
fi
./MonOnline
exit
