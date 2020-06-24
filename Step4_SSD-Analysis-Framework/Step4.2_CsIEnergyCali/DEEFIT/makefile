############################
# makefile for
# the deefit standalone
# and deefit shared library
# program v. 1.1
# e.d.f (1/2010)
############################
CPP=g++
CPPFLAG = -O2 -DSTANDALONE
ROOTLIB = `root-config --cflags --libs` -L$(ROOTSYS)/lib -lGui -lMinuit
SYSLIB = -lm
CFLAG = $(CPPFLAG) -I$(ROOTSYS)/include
PROG = deefit_S
DICT = deedict


OBJ = MDEEFrame.o MEpax.o MTree.o MFitParam.o MGraphFrame.o $(DICT).o

$(PROG): $(OBJ) $(PROG).C
	$(CPP) $(CFLAG) -o $(PROG) $(PROG).C $(OBJ) $(ROOTLIB) $(SYSLIB)

MDEEFrame.o: MDEEFrame.C MDEEFrame.h
	$(CPP) $(CFLAG) -c MDEEFrame.C $(ROOTLIB)

MEpax.o: MEpax.C MEpax.h
	$(CPP) $(CFLAG) -c MEpax.C $(ROOTLIB)

MTree.o: MTree.C MTree.h
	$(CPP) $(CFLAG) -c MTree.C $(ROOTLIB)

MFitParam.o: MFitParam.C MFitParam.h
	$(CPP) $(CFLAG) -c MFitParam.C $(ROOTLIB)

MGraphFrame.o: MGraphFrame.C MGraphFrame.h
	$(CPP) $(CFLAG) -c MGraphFrame.C $(ROOTLIB)


# create the root dictionary file
$(DICT).o: $(DICT).cxx
	$(CPP) $(CFLAG) -c $(DICT).cxx $(ROOTLIB)
$(DICT).cxx: LinkDef.h MDEEFrame.h MTree.h MFitParam.h MGraphFrame.h
	$(ROOTSYS)/bin/rootcint -f deedict.cxx -c MDEEFrame.h MTree.h MFitParam.h \
MGraphFrame.h LinkDef.h

# create shared library for Root
shared:
	$(CPP) $(CFLAG) -shared -fpic -o $(PROG).so $(OBJ) $(ROOTLIB) $(SYSLIB)

clean:
	@rm *.o
	@rm *.so
