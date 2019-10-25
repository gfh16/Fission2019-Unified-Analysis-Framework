######################################
#  makefile for a_QA.cc          #
#  yj-wang 2018-2-28                #
######################################

CC=gcc
CPP=g++
CFLAG = -O2 -D LINUX -g
ROOTLIB = $(shell root-config --cflags --glibs)
ROOTCINT = rootcint -f
SYSLIB= -lstdc++
ROOTINC = -I$(shell root-config --incdir)
CRFLAG = $(shell root-config --cflags)
ROOTBIN = $(shell root-config --bindir)

PROG = QC_BranchAddress QC_ReadTree


QC_BranchAddress: QC_BranchAddress.o
	$(CPP) -o $@ QC_BranchAddress.o $(OBJS) $(CFLAG) $(ROOTLIB) $(SYSLIB)

QC_ReadTree: QC_ReadTree.o
	$(CPP) -o $@ QC_ReadTree.o $(OBJS) $(CFLAG) $(ROOTLIB) $(SYSLIB)

%.o: %.cpp
	$(CPP) -c $(CFLAG) $(CRFLAG) -o $@ $<

clean:
	-rm *.o
	-rm $(PROG)
