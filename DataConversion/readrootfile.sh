#!/bin/bash
# run the vme daq program DAQPC 
# yj-wang 02/2018
export DAQServerIP=localhost
export DAQCONFPATH=./acqsetup
export DAQDataPath=./vmedata/
if [ $# != 0 ]
then 
 parg=$@
fi
./ReadRootFile2D $parg
exit
