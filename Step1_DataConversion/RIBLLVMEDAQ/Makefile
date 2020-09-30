######################################
#  makefile for RIBLLVMEDAQ          #
#  Han Jianlong 07/2012              #
######################################
#TQObject.h TGTextEntry.h TGNumberEntry.h TGLabel.h TGButton.h TGTextView.h TThread.h

CC=gcc
CF=ifort
FF=gfortran
FFLAG= -w -o2 -fpp

CPP=g++
CFLAG = -O2 -D LINUX -g
ROOTLIB = $(shell root-config --cflags --glibs)
ROOTCINT = rootcint -f
SYSLIB= -lstdc++
ROOTINC = -I$(shell root-config --incdir)
CRFLAG = $(shell root-config --cflags)
ROOTBIN = $(shell root-config --bindir)
CAENLibs = -lCAENVME

PROG = DAQPC ControlPC OnlinePC MonOnline Raw2ROOT ReadRootFile2D

ModHeader = TBoard.h TControl.h TModV785.h TModV785N.h TModV775.h TModV775N.h TModV830AC.h TModV792.h\
            TEvtBuilder.h TClientEvtBuilder.h TControlFrame.h TDataFileBuilder.h TMasterTask.h

OBJS =  OnlineUserFunc.o TBoard.o TCBLT.o TClientEvtBuilder.o TConfig.o TControl.o TControlFrame.o\
        TCrateCBLT.o TDAQApplication.o TDAQPCTask.o TDataAnalyser.o TDataFileBuilder.o TDataFileReader.o\
        TDataReceiver.o TEvtBuilder.o TMasterTask.o TModV775.o TModV775N.o TModV785.o TModV785N.o TModV830AC.o TModV792.o\
        TOfflineAnalyser.o TOnline.o TOnlineFrame.o  TUDPClientSocket.o  TUDPServerSocket.o\
        TVMELink.o UserVMEFuncitons.o classDict.o

all: $(PROG)

DAQPC:DIC $(OBJS) VMEDAQPC.o
	$(CPP) -o $@ VMEDAQPC.o $(OBJS) $(CFLAG) $(ROOTLIB) $(SYSLIB) $(CAENLibs)

ControlPC:DIC $(OBJS) ControlPC.o
	$(CPP) -o ControlPC ControlPC.o $(OBJS) $(CFLAG) $(ROOTLIB) $(SYSLIB) $(CAENLibs)

OnlinePC:DIC $(OBJS) OnlinePC.o
	$(CPP) -o $@ OnlinePC.o $(OBJS) $(CFLAG) $(ROOTLIB) $(SYSLIB) $(CAENLibs)

MonOnline:DIC $(OBJS) MonOnline.o
	$(CPP) -o $@ MonOnline.o $(OBJS) $(CFLAG) $(ROOTLIB) $(SYSLIB) $(CAENLibs)

Raw2ROOT:DIC $(OBJS) Raw2ROOT.o
	$(CPP) -o $@ Raw2ROOT.o $(OBJS) $(CFLAG) $(ROOTLIB) $(SYSLIB) $(CAENLibs)

ReadRootFile2D:DIC $(OBJS) ReadRootFile2D.o
	$(CPP) -o $@ ReadRootFile2D.o $(OBJS) $(CFLAG) $(ROOTLIB) $(SYSLIB) $(CAENLibs)


%.o: %.cpp
	$(CPP) -c $(CFLAG) $(CRFLAG) -o $@ $<

DIC:
	-rm -f classDict.cpp classDict.h
	$(ROOTCINT) classDict.cpp -c -DLINUX $(ModHeader) classLinkDef.h

clean:
	-rm *.o
	-rm $(PROG)
