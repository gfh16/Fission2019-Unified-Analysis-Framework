######################################
#  makefile for RIBLLVMEDAQ          #
#  Han Jianlong 07/2012              #
######################################

PROG = DAQPC ControlPC OnlinePC MonOnline Raw2ROOT ReadRootFile2D

all:$(PROG)

DAQPC:del
	cd ./RIBLLVMEDAQ && $(MAKE) $@
	cp -f ./RIBLLVMEDAQ/$@ ./
	chmod 777 $@

ControlPC:del
	rm -f $@
	cd ./RIBLLVMEDAQ && $(MAKE) $@
	cp -f ./RIBLLVMEDAQ/$@ ./
	chmod 777 $@

OnlinePC:del
	rm -f $@
	cd ./RIBLLVMEDAQ && $(MAKE) $@
	cp -f ./RIBLLVMEDAQ/$@ ./
	chmod 777 $@

MonOnline:del
	rm -f $@
	cd ./RIBLLVMEDAQ && $(MAKE) $@
	cp -f ./RIBLLVMEDAQ/$@ ./
	chmod 777 $@

Raw2ROOT:del
	rm -f $@
	cd ./RIBLLVMEDAQ && $(MAKE) $@
	cp -f ./RIBLLVMEDAQ/$@ ./
	chmod 777 $@

ReadRootFile2D:del
	rm -f $@
	cd ./RIBLLVMEDAQ && $(MAKE) $@
	cp -f ./RIBLLVMEDAQ/$@ ./
	chmod 777 $@

clean:del
	cd ./RIBLLVMEDAQ && $(MAKE) $@
	
del:
	rm -f $(PROG)
