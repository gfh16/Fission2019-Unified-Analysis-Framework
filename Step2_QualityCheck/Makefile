######################################
#  makefile for a_QA.cc          #
#  yj-wang 2018-2-28                #
######################################

Obj =   QC_BranchAddress   #QC_ReadTree
Src =   QC_BranchAddress.cpp  #QC_ReadTree.cpp     



CC=gcc
CPP=g++
CFLAG = -O2 -D LINUX -g
ROOTLIB = $(shell root-config --cflags --glibs)
ROOTCINT = rootcint -f
SYSLIB= -lstdc++
ROOTINC = -I$(shell root-config --incdir)
CRFLAG = $(shell root-config --cflags)
ROOTBIN = $(shell root-config --bindir)


$(Obj):
	$(CPP) -o $(Obj) $(Src) $(ROOTLIB) $(SYSLIB)

clean:
	rm -f $(Obj)
