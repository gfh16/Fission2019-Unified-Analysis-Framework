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
SYSLIB    := -lz -l TreePlayer -lMathMore
ROOTCLINGORCINT   := rootcling



PROG: CSHINEQualityCheck


QC_ReadTree: CSHINEQualityCheck.o
	 $(CPP) -o $@ CSHINEQualityCheck.o $(OBJS) $(CFLAG) $(ROOTLIB) $(SYSLIB) $(ROOTCINT) $(SYSLIB) $(ROOTCLINGORCINT)

%.o: %.C
	$(CPP) -c $(CFLAG) $(CRFLAG) -o $@ $<


clean:
	-rm *.o
	-rm CSHINEQualityCheck
