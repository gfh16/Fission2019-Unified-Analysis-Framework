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
ROOTINC  =  -I$(shell root-config --incdir)
CRFLAG   =  $(shell root-config --cflags)
ROOTBIN  =  $(shell root-config --bindir)
SRC      = ../src/ReadFileModule.C

PROG: QC_ReadTree


QC_ReadTree: QC_ReadTree.o
	$(CPP) -o $@ QC_ReadTree.o $(OBJS) $(CFLAG)  $(ROOTLIB) $(SYSLIB) $(ROOTCLINGORCINT) $(TIMELINK1) $(TIMELINK2) $(SRC)

%.o: %.C
	$(CPP) -c $(CFLAG) $(CRFLAG) -o $@ $<


clean:
	-rm *.o
	-rm QC_ReadTree
