######################################
#  makefile for a_QA.cc             #
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
ROOTCLINGORCINT   := rootcling
TIMELINK1 =  src/ReadFileModule.C
TIMELINK2 =  src/TimeAndPercentage.C
TIMELINK3 =  src/CSHINEQualityCheck.C

PROG: test1


test1: test1.o
	$(CPP) -o $@ test1.o $(OBJS) $(CFLAG) $(ROOTLIB) $(SYSLIB) $(TIMELINK1) $(TIMELINK2) $(TIMELINK3) $(ROOTINC)

%.o: %.C
	$(CPP) -c $(CFLAG) $(CRFLAG) -o $@ $<


clean:
	-rm *.o
	-rm test1
