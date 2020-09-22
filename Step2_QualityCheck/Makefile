######################################
#  makefile for a_QA.cc          #
#  yj-wang 2018-2-28                #
######################################

#### 根据需要添加必要的
CC       =  gcc
CPP      =  g++
CFLAG    =  -O2 -D LINUX -g
ROOTLIB  =  $(shell root-config --cflags --glibs)
ROOTCINT =  rootcint -f
SYSLIB   =  -lstdc++
ROOTINC  =  -I $(shell root-config --incdir)
CRFLAG   =  $(shell root-config --cflags)
ROOTBIN  =  $(shell root-config --bindir)
TIMELINK1 =  ../src/ReadFileModule.C
TIMELINK2 =  ../src/TimeAndPercentage.C


PROG: QC_BranchAddress


QC_BranchAddress: QC_BranchAddress.o
	$(CPP) -o $@ QC_BranchAddress.o $(OBJS) $(CFLAG) $(ROOTLIB) $(SYSLIB) $(TIMELINK1) $(TIMELINK2)

%.o: %.C
	$(CPP) -c $(CFLAG) $(CRFLAG) -o $@ $<


clean:
	-rm *.o
	-rm QC_BranchAddress
